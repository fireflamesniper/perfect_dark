#include <ultra64.h>
#include "constants.h"
#include "game/chraction.h"
#include "game/debug.h"
#include "game/chr.h"
#include "game/prop.h"
#include "game/propsnd.h"
#include "game/atan2f.h"
#include "game/bondgun.h"
#include "game/gset.h"
#include "game/player.h"
#include "game/playermgr.h"
#include "game/bg.h"
#include "game/mplayer/setup.h"
#include "game/mplayer/scenarios.h"
#include "game/radar.h"
#include "game/bot.h"
#include "game/botcmd.h"
#include "game/botact.h"
#include "game/botinv.h"
#include "game/challenge.h"
#include "game/lang.h"
#include "game/mplayer/mplayer.h"
#include "game/pad.h"
#include "game/padhalllv.h"
#include "game/propobj.h"
#include "game/splat.h"
#include "bss.h"
#include "lib/collision.h"
#include "lib/model.h"
#include "lib/rng.h"
#include "lib/mtx.h"
#include "lib/anim.h"
#include "data.h"
#include "types.h"

#define PICKUPCRITERIA_DEFAULT  0
#define PICKUPCRITERIA_CRITICAL 1
#define PICKUPCRITERIA_ANY      2

struct chrdata *g_MpBotChrPtrs[MAX_BOTS];

u8 g_BotCount = 0;

struct botdifficulty {
	u8 shootdelay60;
	f32 minzerospeed;
	f32 maxzerospeed;
	u16 zerotime60;
	f32 turnunzeromult;
	f32 zerocloakspeed;
	f32 forcezerominspeed;
	s32 dizzyamount;
};

/**
 * shootdelay60:
 *     How long the bot waits between seeing their target and shooting.
 *     It has a cooldown, so a brief break in sight will have little effect.
 *
 * minzerospeed and maxzerospeed:
 *     Controls the speed at which the bot zeroes their target.
 *     These values are scaled linearly based on the bot's current zerotimer60.
 *     A random speed between the scaled min and max is chosen.
 *
 * zerotime60:
 *     How long it takes for the bot to zero their target. A cooldown is used.
 *
 * turnunzeromult:
 *     When a bot is zeroing or has zeroed its target and has to turn naturally,
 *     this controls the rate at which the target is "unzeroed".
 *     The value is a multiplier for the zerotime60 cooldown.
 *
 * zerocloakspeed:
 *     A minimum value for maxzerospeed which is used when the target is cloaked.
 *     It allows the bot to zero faster when they're just starting to zero.
 *
 * forcezerominspeed:
 *     A minimum value for maxzerospeed.
 *     It allows the bot to zero faster when they're just starting to zero.
 *
 * dizzyamount:
 *     How much tranq the bot needs to receive before they start being affected.
 *     (ie. punching/shooting as soon as target is in sight, even if not zeroed)
 */

struct botdifficulty g_BotDifficulties[] = {
	//           shootdelay60                                       turnunzeromult      forcezerominspeed
	//           |          minzerospeed   maxzerospeed zerotime60  |   zerocloakspeed  |            dizzyamount
	/* meat */ { TICKS(90), BADDTOR2(15),   BADDTOR2(30), TICKS(600), 10, BADDTOR2(40),    BADDTOR2(20), TICKS(1000) },
	/* easy */ { TICKS(60), BADDTOR2(7),    BADDTOR2(14), TICKS(360), 10, BADDTOR2(28.5f), BADDTOR2(8),  TICKS(1000) },
	/* norm */ { TICKS(30), BADDTOR2(4),    BADDTOR2(8),  TICKS(180), 4,  BADDTOR2(20),    BADDTOR2(5),  TICKS(1500) },
	/* hard */ { TICKS(15), BADDTOR2(1.5f), BADDTOR2(4),  TICKS(90),  2,  BADDTOR2(14),    BADDTOR2(2),  TICKS(2500) },
	/* perf */ { TICKS(0),  BADDTOR2(0),    BADDTOR2(2),  TICKS(45),  1,  BADDTOR2(10),    BADDTOR2(0),  TICKS(4000) },
	/* dark */ { TICKS(0),  BADDTOR2(0),    BADDTOR2(0),  TICKS(0),   0,  BADDTOR2(8),     BADDTOR2(0),  TICKS(4000) },
	{ 0 },
};

s32 bot_get_num_teammates_defending_hill(struct chrdata *bot);
s32 bot_get_num_opponents_in_hill(struct chrdata *chr);
bool bot_should_return_ctc_token(struct chrdata *chr);

bool bot_is_dizzy(struct chrdata *chr)
{
	return chr->blurdrugamount >= g_BotDifficulties[chr->aibot->config->difficulty].dizzyamount;
}

void bot_reset(struct chrdata *chr, u8 respawning)
{
	s32 i;
	u32 rand;
	struct aibot *aibot = chr->aibot;

	if (aibot) {
		chr->fadealpha = -1;
		chr->chrflags &= ~CHRCFLAG_JUST_INJURED;
		chr->hidden &= ~CHRHFLAG_CLOAKED;
		chr->myaction = MA_AIBOTMAINLOOP;
		chr->shotbondsum = 0;

		if (respawning) {
			chr->numclosearghs = 0;
			chr->damage = 0;
			chr->target = -1;
			chr->chrpreset1 = -1;
			chr->cover = -1;
			chr_set_shield(chr, 0);
			chr->cmnum = 0;
			chr->cmnum2 = 0;

#if VERSION >= VERSION_NTSC_1_0
			bgun_free_fireslot(chr->fireslots[0]);
			bgun_free_fireslot(chr->fireslots[1]);
#endif

			chr->unk32c_12 = 0;
			chr->fireslots[0] = -1;
			chr->fireslots[1] = -1;
			chr->firecount[0] = 0;
			chr->firecount[1] = 0;
			chr->weapons_held[0] = NULL;
			chr->weapons_held[1] = NULL;
			chr->weapons_held[2] = NULL;
			chr->liftaction = 0;
			chr->inlift = 0;
			chr->lift = NULL;
			chr->height = 185;

			for (i = 0; i < 33; i++) {
				aibot->ammoheld[i] = 0;
			}

			botinv_clear(chr);

			aibot->weaponnum = WEAPON_UNARMED;
			aibot->gunfunc = FUNC_PRIMARY;
			aibot->ismeleeweapon = true;
			aibot->loadedammo[0] = 0;
			aibot->loadedammo[1] = 0;
			aibot->gotoprop = NULL;
			aibot->timeuntilreload60[0] = 0;
			aibot->timeuntilreload60[1] = 0;
			aibot->nextbullettimer60[0] = 0;
			aibot->nextbullettimer60[1] = 0;
			aibot->distmode = -1;

#if VERSION < VERSION_PAL_BETA
			aibot->unk030 = 301;
#endif

			aibot->throwtimer60 = 0;
			aibot->burstsdone[0] = 0;
			aibot->burstsdone[1] = 0;
			aibot->skrocket = NULL;
			aibot->htbheldtimer60 = 0;
			aibot->hasbriefcase = false;
			aibot->hascase = false;
			aibot->cloakdeviceenabled = false;
			aibot->rcp120cloakenabled = false;
			aibot->unk04c_04 = false;
			aibot->unk04c_03 = false;
			aibot->hasuplink = false;
			aibot->flags = 0;
			aibot->inhill = false;
			aibot->hillpadnum = -1;
			aibot->hillcovernum = -1;
			aibot->lastknownhill = -1;
			aibot->cyclonedischarging[1] = 0;
			aibot->cyclonedischarging[0] = 0;
			aibot->changeguntimer60 = 0;
			aibot->distmodettl60 = 0;
			aibot->forcemainloop = false;
			aibot->returntodefendtimer60 = 0;
			aibot->punchtimer60[HAND_LEFT] = -1;
			aibot->punchtimer60[HAND_RIGHT] = 0;
			aibot->reaperspeed[HAND_LEFT] = 0;
			aibot->reaperspeed[HAND_RIGHT] = 0;
			aibot->commandtimer60 = 0;
			aibot->shootdelaytimer60 = 0;
			aibot->targetlastseen60 = -1;
			aibot->lastseenanytarget60 = -1;
			aibot->targetinsight = 0;
			aibot->queryplayernum = 0;
			aibot->unk040 = 0;
			aibot->speedmultforwards = 0;
			aibot->speedmultsideways = 0;
			aibot->maulercharge[1] = 0;
			aibot->maulercharge[0] = 0;
			aibot->shotspeed.x = 0;
			aibot->shotspeed.y = 0;
			aibot->shotspeed.z = 0;

			for (i = 0; i != MAX_MPCHRS; i++) {
				aibot->chrnumsbydistanceasc[i] = -1;
				aibot->chrdistances[i] = U32_MAX;
				aibot->chrsinsight[i] = false;
				aibot->chrslastseen60[i] = -1;
				aibot->chrrooms[i] = -1;
			}

			aibot->waypoints[0] = NULL;
			aibot->numwaystepstotarget = 0;
			aibot->random1 = random();
			aibot->random1ttl60 = 0;
			aibot->targetcloaktimer60 = 0;
			aibot->canseecloaked = 0;
			aibot->rcpcloaktimer60 = 0;
			aibot->random2ttl60 = 0;
			aibot->random2 = random();
			aibot->randomfrac = RANDOMFRAC();
			aibot->cheap = 0;

#if VERSION >= VERSION_NTSC_1_0
			aibot->forceslowupdates = 0;
			aibot->distoverrideprop = NULL;
			aibot->distoverridetimer60 = 0;
#endif
		}

		if (aibot->config->type == BOTTYPE_TURTLE || aibot->config->type == BOTTYPE_SHIELD) {
			chr->cshield = 8;
		}

		if (aibot->config->difficulty == BOTDIFF_DARK) {
			// @bug: This is unsetting the UNLIMITEDAMMO flag rather than setting it.
			// It could have been intentional - they could have experimented with
			// darksims having unlimited ammo during development before disabling
			// the feature - but then it would make more sense to remove the flag
			// rather than invert it.
			aibot->flags &= ~BOTFLAG_UNLIMITEDAMMO;

			if (mp_has_shield()) {
				chr->cshield = 8;
			}
		}

		aibot->respawning = true;
		aibot->fadeintimer60 = TICKS(120);
	}
}

void bot_spawn(struct chrdata *chr, u8 respawning)
{
	f32 thing;
	struct prop *prop;
	struct defaultobj *obj;
	struct aibot *aibot = chr->aibot;
	struct coord pos;
	RoomNum rooms[8];

	if (chr->prop) {
		prop = chr->prop->child;

		while (prop) {
			obj = prop->obj;

			if (obj) {
				obj->hidden |= OBJHFLAG_DELETING;
			}

			prop = prop->next;
		}
	}

	if (aibot) {
		bot_reset(chr, respawning);
		splat_reset_chr(chr);
		thing = scenario_choose_spawn_location(chr->radius, &pos, rooms, chr->prop);
		chr->hidden |= CHRHFLAG_WARPONSCREEN;
		chr_move_to_pos(chr, &pos, rooms, thing, true);

		chr->aibot->roty = model_get_chr_rot_y(chr->model);
		chr->aibot->angleoffset = 0;
		chr->aibot->speedtheta = 0;
		chr->aibot->lookangle = model_get_chr_rot_y(chr->model);
		chr->aibot->moveratex = 0;
		chr->aibot->moveratey = 0;

		chr_stand_immediate(chr, 0);
	}
}

void bot_spawn_all(void)
{
	s32 i;

	for (i = 0; i < g_BotCount; i++) {
		bot_spawn(g_MpBotChrPtrs[i], false);
	}
}

#if PIRACYCHECKS
u32 add87654321(u32 value)
{
	return value + 0x87654321;
}
#endif

u32 bot_pickup_prop(struct prop *prop, struct chrdata *chr)
{
	struct defaultobj *obj = prop->obj;

	if (!chr || !chr->aibot) {
		return 0;
	}

	dprint();

	if (1);

	obj->flags3 &= ~OBJFLAG3_ISFETCHTARGET;

	switch (obj->type) {
	case OBJTYPE_KEY:
		// Missing break, but doesn't matter as keys don't exist in multiplayer
	case OBJTYPE_AMMOCRATE:
		{
			struct ammocrateobj *crate = (struct ammocrateobj *)prop->obj;
			s32 qty;

			if (1);
			qty = ammocrate_get_pickup_ammo_qty(crate);
			dprint();

			if (qty) {
				botact_give_ammo_by_type(chr->aibot, crate->ammotype, qty);
			}

			// Pickup sound
			ps_create(NULL, prop, SFXNUM_00EA_PICKUP_AMMO, -1,
				-1, PSFLAG_AMBIENT, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);

			obj_free(obj, false, obj->hidden2 & OBJH2FLAG_CANREGEN);

			if (qty) {
				dprint();
			}
		}
		return 2;
	case OBJTYPE_MULTIAMMOCRATE:
		{
			struct multiammocrateobj *crate = (struct multiammocrateobj *)prop->obj;
			u32 padding[1];
			s32 qty;
			s32 i;
#if PIRACYCHECKS
			{
				u32 *ptr = (u32 *)&door_finish_close;
				u32 *end = (u32 *)&door_set_mode;
				u32 checksum = 0;

				while (ptr < end) {
					checksum += *ptr;
					checksum *= 2;
					ptr++;
				}

				if (checksum != CHECKSUM_PLACEHOLDER) {
					u32 *ptr = (u32 *)chr_check_can_see_target;
					ptr[0] = add87654321(0x24020001 - 0x87654321); // li v0, 1
					ptr[1] = add87654321(0x03e00008 - 0x87654321); // jr ra
					ptr[2] = add87654321(0x00000000 - 0x87654321); // nop
				}
			}
#endif
			dprint();

			for (i = 0; i != 19; i++) {
				qty = crate->slots[i].quantity;

				if (qty) {
					dprint();
				}

				if (qty) {
					botact_give_ammo_by_type(chr->aibot, i + 1, qty);
				}
			}

			// Pickup sound
			ps_create(NULL, prop, SFXNUM_00EA_PICKUP_AMMO, -1,
				-1, PSFLAG_AMBIENT, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);

			obj_free(obj, false, obj->hidden2 & OBJH2FLAG_CANREGEN);
		}
		return 2;
	case OBJTYPE_WEAPON:
		{
			struct weaponobj *weapon = prop->weapon;
			s32 itemtype = botinv_get_item_type(chr, weapon->weaponnum);
			s32 result;
			s32 qty;

			if (weapon->weaponnum == WEAPON_BRIEFCASE2) {
				result = scenario_pick_up_briefcase(chr, prop);
			} else if (weapon->weaponnum == WEAPON_DATAUPLINK) {
				result = scenario_pick_up_uplink(chr, prop);
			} else {
				prop_play_pickup_sound(prop, weapon->weaponnum);
				qty = weapon_get_pickup_ammo_qty(weapon);

				if (qty) {
					botact_give_ammo_by_weapon(chr->aibot, weapon->weaponnum, weapon->gunfunc, qty);
				}

				if (itemtype) {
					struct weapondef *weapondef = gset_get_weapondef(weapon->weaponnum);
					s32 originalpad = botinv_get_weapon_pad(chr, weapon->weaponnum);
					s32 currentpad = obj->pad;

					if (itemtype == INVITEMTYPE_WEAP
							&& weapondef
							&& (weapondef->flags & WEAPONFLAG_DUALWIELD)
							&& originalpad != currentpad) {
						botinv_give_dual_weapon(chr, weapon->weaponnum);
						result = 1;
					} else {
						result = 2;
					}
				} else {
					botinv_give_prop(chr, prop);
					result = 1;
				}

				obj_free(obj, false, obj->hidden2 & OBJH2FLAG_CANREGEN);
			}

			return result;
		}
	case OBJTYPE_SHIELD:
		{
			struct shieldobj *shield = (struct shieldobj *)prop->obj;

			ps_create(NULL, prop, SFXNUM_01CD_PICKUP_SHIELD, -1,
				-1, PSFLAG_AMBIENT, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);

			chr_set_shield(chr, shield->amount * 8);
			obj_free(obj, false, obj->hidden2 & OBJH2FLAG_CANREGEN);
		}
		return 3;
	case OBJTYPE_BASIC:
	case OBJTYPE_GLASS:
	case OBJTYPE_AUTOGUN:
	case OBJTYPE_TINTEDGLASS:
		break;
	}

	return 0;
}

bool bot_test_prop_for_pickup(struct prop *prop, struct chrdata *chr)
{
	struct defaultobj *obj = prop->obj;

	struct weaponobj *weaponobj;
	s32 itemtype;
	struct weapondef *weapon;
	bool singleonly;
	s32 i;

	struct ammocrateobj *crate;

	s32 weaponnum;
	bool ignore1;
	struct multiammocrateobj *crate2;

	u32 stack1;

	struct shieldobj *shield;
	bool ignore2;

	struct prop *chrprop;

	f32 xdist;
	f32 ydist;
	f32 zdist;
	f32 sqrange;
	bool sp3c;
	u32 stack2;

	if (!chr || !chr->aibot || !g_Vars.lvmpbotlevel || chr_is_dead(chr)) {
		return false;
	}

	dprint();

	if (prop->timetoregen != 0) {
		return false;
	}

	if (obj_defaults_to_bounceable_invincible_pickupable(obj) && obj->type != OBJTYPE_HAT) {
		if (obj->flags & OBJFLAG_UNCOLLECTABLE) {
			return false;
		}
	} else {
		if ((obj->flags & OBJFLAG_COLLECTABLE) == 0) {
			return false;
		}
	}

	dprint();

	if (1);

#if VERSION >= VERSION_NTSC_1_0
	if ((obj->hidden & OBJHFLAG_DELETING) || (obj->flags & OBJFLAG_THROWNLAPTOP)) {
		return false;
	}
#else
	if (obj->flags & OBJFLAG_THROWNLAPTOP) {
		return false;
	}
#endif

	dprint();

	if ((obj->hidden & OBJHFLAG_PROJECTILE)
			&& obj->projectile
			&& obj->projectile->pickuptimer240 > 0
			&& obj->projectile->bouncecount == 0) {
		return false;
	}

	dprint();

	if (!obj_can_pickup_from_safe(obj)) {
		return false;
	}

	dprint();

	if (1);

	if (obj->type == OBJTYPE_WEAPON) {
		weaponobj = prop->weapon;
		itemtype = botinv_get_item_type(chr, weaponobj->weaponnum);
		weapon = gset_get_weapondef(weaponobj->weaponnum);
		singleonly = weapon && (weapon->flags & WEAPONFLAG_DUALWIELD) == 0;

		if (weaponobj->weaponnum != WEAPON_BRIEFCASE2) {
			// If aibot is dual wielding, or single wielding and weapon doesn't support dual,
			// ignore the pickup if at max ammo already
			if (itemtype == INVITEMTYPE_DUAL || (itemtype == INVITEMTYPE_WEAP && singleonly)) {
				if (botact_get_ammo_quantity_by_weapon(chr->aibot, weaponobj->weaponnum, weaponobj->gunfunc, false) >= bgun_get_capacity_by_ammotype(botact_get_ammo_type_by_function(weaponobj->weaponnum, weaponobj->gunfunc))) {
					return false;
				}
			}

			// Ignore rockets that are in flight
			if ((weaponobj->weaponnum == WEAPON_ROCKET || weaponobj->weaponnum == WEAPON_HOMINGROCKET)
					&& (obj->hidden & OBJHFLAG_PROJECTILE)) {
				return false;
			}
		}
	} else if (obj->type == OBJTYPE_AMMOCRATE) {
		crate = (struct ammocrateobj *)prop->obj;

		// Ignore ammo crate if at max ammo already
		if (botact_get_ammo_quantity_by_type(chr->aibot, crate->ammotype, false) >= bgun_get_capacity_by_ammotype(crate->ammotype)) {
			return false;
		}
	} else if (obj->type == OBJTYPE_MULTIAMMOCRATE) {
		crate2 = (struct multiammocrateobj *)prop->obj;
		ignore1 = true;

		if (obj_get_destroyed_level(obj)) {
			dprint();
			return false;
		}

		for (i = 0; i < 0x13; i++) {
			weaponnum = botact_get_weapon_by_ammo_type(i + 1);

			if (crate2->slots[i].quantity > 0) {
				if (botact_get_ammo_quantity_by_type(chr->aibot, i + 1, false) < bgun_get_capacity_by_ammotype(i + 1)) {
					ignore1 = false;

					if (weaponnum && !botinv_get_item_type(chr, weaponnum)) {
						dprint();
						botinv_give_prop(chr, prop);
					}

					break;
				}
			}
		}

		if (ignore1) {
			return false;
		}
	} else if (obj->type == OBJTYPE_SHIELD) {
		shield = (struct shieldobj *)prop->obj;
		ignore2 = false;

		if (shield->amount <= chr_get_shield(chr) * 0.125f) {
			ignore2 = true;
		} else if (g_MpSetup.scenario == MPSCENARIO_HOLDTHEBRIEFCASE && chr->aibot->hasbriefcase) {
			ignore2 = true;
		}

		if (ignore2) {
			return false;
		}
	}

	chrprop = chr->prop;

	xdist = prop->pos.x - chrprop->pos.x;
	ydist = prop->pos.y - chrprop->pos.y;
	zdist = prop->pos.z - chrprop->pos.z;

	dprint();

	if (chr->aibot->cheap) {
		if (1);
		if (1);
		if (1);
		if (1);
		if (1);
		sqrange = 250 * 250;
	} else {
		sqrange = 100 * 100;
	}

	sp3c = xdist * xdist + zdist * zdist <= sqrange && ydist >= -200 && ydist <= 200;

	if (sp3c) {
		dprint();

		if ((obj->flags2 & OBJFLAG2_PICKUPWITHOUTLOS) == 0
				&& !cd_test_los_oobtail_autoflags(&chrprop->pos, chrprop->rooms, &prop->pos, prop->rooms, CDTYPE_DOORS | CDTYPE_BG)) {
			sp3c = false;
		}
	}

	if (sp3c) {
		dprint();
		return bot_pickup_prop(prop, chr);
	}

	return false;
}

s32 bot_is_obj_collectable(struct defaultobj *obj)
{
	if (!obj) {
		return false;
	}

	if (obj->type == OBJTYPE_AMMOCRATE
			|| obj->type == OBJTYPE_MULTIAMMOCRATE
			|| obj->type == OBJTYPE_SHIELD) {
		return true;
	}

	if (obj->type == OBJTYPE_WEAPON) {
		struct weaponobj *weapon = (struct weaponobj *)obj;

		if (weapon->weaponnum == WEAPON_NBOMB
				|| weapon->weaponnum == WEAPON_GRENADE
				|| weapon->weaponnum == WEAPON_GRENADEROUND
				|| weapon->weaponnum == WEAPON_PROXIMITYMINE
				|| weapon->weaponnum == WEAPON_REMOTEMINE
				|| weapon->weaponnum == WEAPON_TIMEDMINE
				|| weapon->weaponnum == WEAPON_SKROCKET
				|| (weapon->weaponnum == WEAPON_DRAGON && weapon->gunfunc == FUNC_SECONDARY)) {
			return false;
		}

		return true;
	}

	return false;
}

/**
 * Check nearby props to see if the chr is picking them up on this frame.
 */
void bot_check_pickups(struct chrdata *chr)
{
	s32 i;
	s16 *propnumptr;
	s16 propnums[260];
	RoomNum allrooms[22];
	RoomNum neighbours[12];

	rooms_copy(chr->prop->rooms, allrooms);

	for (i = 0; chr->prop->rooms[i] != -1; i++) {
		bg_room_get_neighbours(chr->prop->rooms[i], neighbours, 10);
		rooms_append(neighbours, allrooms, 20);
	}

	room_get_props(allrooms, propnums, 256);
	propnumptr = propnums;

	while (*propnumptr >= 0) {
		struct prop *prop = &g_Vars.props[*propnumptr];

		if (prop->type == PROPTYPE_OBJ || prop->type == PROPTYPE_WEAPON) {
			if (prop->timetoregen == 0) {
				struct defaultobj *obj = prop->obj;

#if VERSION >= VERSION_NTSC_1_0
				if (obj)
#endif
				{
					if ((obj->hidden & OBJHFLAG_PROJECTILE) == 0
							|| obj->projectile == NULL
							|| obj->projectile->pickuptimer240 <= 0
							|| obj->projectile->bouncecount != 0) {
						if (bot_is_obj_collectable(obj)) {
							if (bot_test_prop_for_pickup(prop, chr)) {
								prop_execute_tick_operation(prop, TICKOP_FREE);
							} else {
								dprint();
							}
						}
					}
				}
			}
		}

		propnumptr++;
	}
}

s32 bot_guess_crouch_pos(struct chrdata *chr)
{
	s32 crouchpos;

	if (chr->height <= 90) {
		crouchpos = CROUCHPOS_SQUAT;
	} else if (chr->height <= 135) {
		crouchpos = CROUCHPOS_DUCK;
	} else {
		crouchpos = CROUCHPOS_STAND;
	}

	return crouchpos;
}

bool bot_apply_movement(struct chrdata *chr)
{
	struct aibot *aibot;
	u32 stack;
	f32 speedforwards;
	f32 speedsideways;
	f32 angle;
	f32 angle2;

	if (!chr || !chr->aibot) {
		return false;
	}

	aibot = chr->aibot;

	angle = chr_get_theta(chr) - chr_get_rot_y(chr);

	if (angle < 0) {
		angle += BADDTOR(360);
	}

	speedforwards = aibot->speedmultforwards * cosf(angle) - sinf(angle) * aibot->speedmultsideways;
	speedsideways = aibot->speedmultforwards * sinf(angle) + cosf(angle) * aibot->speedmultsideways;

	player_choose_third_person_animation(chr, bot_guess_crouch_pos(chr), speedsideways, speedforwards, aibot->speedtheta, &aibot->angleoffset, &aibot->attackanimconfig);

	angle2 = chr_get_theta(chr) - aibot->angleoffset;

	if (angle2 < 0) {
		angle2 += BADDTOR(360);
	}

	if (angle2 >= BADDTOR(360)) {
		angle2 -= BADDTOR(360);
	}

	model_set_chr_rot_y(chr->model, angle2);

	return true;
}

s32 bot_get_weapon_num(struct chrdata *chr)
{
	if (chr->aibot) {
		return chr->aibot->weaponnum;
	}

	return g_Vars.players[playermgr_get_player_num_by_prop(chr->prop)]->hands[HAND_RIGHT].gset.weaponnum;
}

u8 bot_get_targets_weapon_num(struct chrdata *chr)
{
	struct prop *target = chr_get_target_prop(chr);
	u8 weaponnum = WEAPON_NONE;

	if (target) {
		weaponnum = bot_get_weapon_num(target->chr);
	}

	return weaponnum;
}

/**
 * Return true if the bot is in sight of their target, or is about to be.
 *
 * The function is used to make the bot start turning to zero their target.
 * It's also used to trigger cloak.
 */
bool bot_is_about_to_attack(struct chrdata *chr, bool forcloak)
{
	bool result = false;

	if (chr->target != -1) {
		struct prop *target;
		s32 stack;
		s32 mpindex;
		target = chr_get_target_prop(chr);
		mpindex = mp_chr_to_chrindex(target->chr);

		if (chr->aibot->chrsinsight[mpindex]) {
			result = true;
		}

		if (chr->aibot->config->difficulty >= BOTDIFF_EASY) {
			// If seen within the last 4 seconds or in the same room
			if (chr->aibot->chrslastseen60[mpindex] >= g_Vars.lvframe60 - TICKS(240)
					|| (array_intersects(chr->prop->rooms, target->rooms))) {
				result = true;
			}

			if (chr->aibot->config->difficulty >= BOTDIFF_NORMAL) {
				// If bot's room and target's room are neighbours
				// or target is in their last seen room or a neighbour of it
				if (bg_rooms_are_neighbours(chr->prop->rooms[0], target->rooms[0])
						|| chr->aibot->chrrooms[mpindex] == target->rooms[0]
						|| bg_rooms_are_neighbours(chr->aibot->chrrooms[mpindex], target->rooms[0])) {
					result = true;
				}

				if (chr->aibot->config->difficulty == BOTDIFF_NORMAL) {
					if (chr->aibot->numwaystepstotarget >= 1 && chr->aibot->numwaystepstotarget <= 3) {
						result = true;
					}
				} else {
					// >= BOTDIFF_HARD
					if (chr->aibot->numwaystepstotarget >= 1 && chr->aibot->numwaystepstotarget <= 4) {
						result = true;
					}
				}
			}
		}

		/**
		 * Meat: Angle to player must be within 25 degrees.
		 * Easy: Angle to player must be within 90 degrees.
		 * Normal and higher: Angle to player doesn't matter.
		 */
		if (!forcloak
				&& (chr->aibot->config->difficulty == BOTDIFF_MEAT || chr->aibot->config->difficulty == BOTDIFF_EASY)
				&& !chr_gopos_is_waiting(chr)) {
			f32 roty = chr_get_rot_y(chr);
			f32 angletotarget = atan2f(target->pos.x - chr->prop->pos.x, target->pos.z - chr->prop->pos.z) - roty;

			if (angletotarget < 0) {
				angletotarget += BADDTOR(360);
			}

			if (angletotarget > DTOR(180)) {
				angletotarget = BADDTOR(360) - angletotarget;
			}

			if (chr->aibot->config->difficulty == BOTDIFF_MEAT) {
				if (angletotarget > BADDTOR(25)) {
					result = false;
				}
			} else if (chr->aibot->config->difficulty == BOTDIFF_EASY) {
				if (angletotarget > BADDTOR2(90)) {
					result = false;
				}
			}
		}
	}

	return result;
}

s32 bot_tick(struct prop *prop)
{
	struct chrdata *chr = prop->chr;
	struct aibot *aibot = chr->aibot;
	s32 result = TICKOP_NONE;
	bool updateable;
	s32 i;
	f32 diffangle;
	f32 tweenangle;
	f32 targetangle;
	f32 oldangle;
	f32 newangle;

	updateable = (prop->flags & PROPFLAG_NOTYETTICKED) && g_Vars.lvupdate240;

	if (aibot) {
		if (updateable && g_Vars.lvframe60 >= 145) {
			bot_tick_unpaused(chr);

			// Calculate cheap
			aibot->cheap = true;

			for (i = 0; prop->rooms[i] != -1; i++) {
				if (bg_room_is_onscreen(prop->rooms[i]) || bg_room_is_standby(prop->rooms[i])) {
					aibot->cheap = false;
					break;
				}
			}

			// Dampen blur
			if (chr->blurdrugamount > 0) {
				if (chr->blurdrugamount > TICKS(5000)) {
					chr->blurdrugamount = TICKS(5000);
				}

				chr->blurdrugamount -= g_Vars.lvupdate60 * (chr->blurnumtimesdied + 1);

				if (chr->blurdrugamount <= 0) {
					chr->blurdrugamount = 0;
					chr->blurnumtimesdied = 0;
				}
			}

			// Calculate target angle
			oldangle = chr_get_theta(chr);

			if (chr_is_dead(chr)) {
				targetangle = chr_get_theta(chr);
			} else if (aibot->skrocket) {
				targetangle = chr_get_theta(chr);
			} else if (bot_is_about_to_attack(chr, false)) {
				struct prop *target = chr_get_target_prop(chr);
				targetangle = chr_get_angle_to_pos(chr, &target->pos);
				targetangle = oldangle + targetangle + aibot->zeroangle;
			} else if (chr->myaction == MA_AIBOTDOWNLOAD && g_ScenarioData.htm.dlterminalnum != -1) {
				targetangle = chr_get_angle_to_pos(chr, &g_ScenarioData.htm.terminals[g_ScenarioData.htm.dlterminalnum].prop->pos);
				targetangle = oldangle + targetangle;
			} else if (chr->myaction == MA_AIBOTFOLLOW
					&& aibot->followingplayernum >= 0
					&& aibot->chrdistances[aibot->followingplayernum] < 300
					&& aibot->realignangleframe >= g_Vars.lvframe60 - TICKS(60)
					&& aibot->config->difficulty != BOTDIFF_MEAT) {
				targetangle = chr_get_theta(g_MpAllChrPtrs[aibot->followingplayernum]);
			} else if (chr->myaction == MA_AIBOTDEFEND
					&& aibot->realignangleframe >= g_Vars.lvframe60 - TICKS(60)
					&& aibot->config->difficulty != BOTDIFF_MEAT) {
				targetangle = aibot->defendholdrot;
			} else {
				targetangle = chr_get_rot_y(chr);
			}

			while (targetangle >= BADDTOR(360)) {
				targetangle -= BADDTOR(360);
			}

			while (targetangle < 0) {
				targetangle += BADDTOR(360);
			}

			if (chr->blurdrugamount > 0 && !chr_is_dead(chr) && aibot->skrocket == NULL) {
				targetangle += chr->blurdrugamount * PALUPF(0.00031410926021636f) * sinf((g_Vars.lvframe60 % TICKS(120)) * PALUPF(0.052351541817188f));

				if (targetangle >= BADDTOR(360)) {
					targetangle -= BADDTOR(360);
				}

				targetangle += BADDTOR(360);
			}

			tweenangle = g_Vars.lvupdate60freal * 0.061590049415827f;
			diffangle = targetangle - oldangle;

			if (diffangle < DTOR(-180)) {
				diffangle += BADDTOR(360);
			} else if (diffangle >= DTOR(180)) {
				diffangle -= BADDTOR(360);
			}

			if (diffangle >= 0) {
				if (diffangle <= tweenangle) {
					newangle = targetangle;
				} else {
					newangle = oldangle + tweenangle;

					if (newangle >= BADDTOR(360)) {
						newangle -= BADDTOR(360);
					}
				}
			} else {
				if (diffangle >= -tweenangle) {
					newangle = targetangle;
				} else {
					newangle = oldangle - tweenangle;

					if (newangle < 0) {
						newangle += BADDTOR(360);
					}
				}
			}

			aibot->speedtheta = newangle - oldangle;

			if (aibot->speedtheta < 0) {
				aibot->speedtheta += BADDTOR(360);
			}

			if (aibot->speedtheta >= DTOR(180)) {
				aibot->speedtheta -= BADDTOR(360);
			}

			aibot->speedtheta /= g_Vars.lvupdate60freal;
			aibot->speedtheta *= 16.236389160156f;

			while (newangle >= BADDTOR(360)) {
				newangle -= BADDTOR(360);
			}

			while (newangle < 0) {
				newangle += BADDTOR(360);
			}

			chr_set_theta(chr, newangle);

			if (chr->target != -1 && !aibot->ismeleeweapon) {
				bool left = chr->weapons_held[HAND_LEFT] ? true : false;
				bool right = (0, chr->weapons_held[HAND_RIGHT] ? true : false);

				chr_calculate_aimend(chr, aibot->attackanimconfig, left, right, 0);
			} else {
				chr_reset_aimend(chr);
			}

			if (chr->actiontype == ACT_DIE || chr->actiontype == ACT_DEAD) {
				aibot->speedmultforwards = 0;
				aibot->speedmultsideways = 0;
			} else if (aibot->skrocket) {
				aibot->speedmultforwards = 0;
				aibot->speedmultsideways = 0;
				aibot->realignangleframe = g_Vars.lvframe60;
			} else if (chr->actiontype == ACT_GOPOS && (chr->act_gopos.flags & GOPOSFLAG_WAITING) == 0) {
				aibot->speedmultforwards = 1;
				aibot->speedmultsideways = 0;
			} else {
				aibot->speedmultforwards = 0;
				aibot->speedmultsideways = 0;
				aibot->realignangleframe = g_Vars.lvframe60;
			}
		}

		bot_apply_movement(chr);

		result = chr_tick(prop);

		if (g_Vars.lvframe60 >= 145) {
			if (updateable) {
				scenario_tick_chr(chr);
			}

			if (updateable && !chr_is_dead(chr)) {
				bot_check_pickups(chr);
			}
		}
	}

	return result;
}

f32 bot_calculate_max_speed(struct chrdata *chr)
{
	f32 speed;

	if (chr->aibot->hascase || chr->aibot->hasbriefcase) {
		speed = -63.600006103516f;
	} else {
		speed = g_HeadsAndBodies[chr->bodynum].height * (1.0f / 159.0f);
	}

	speed = speed * 0.002830188954249f + 1.0f;

	if (chr->aibot->config->type == BOTTYPE_TURTLE) {
		speed *= 3.5f;
	} else if (chr->aibot->config->type == BOTTYPE_SPEED) {
		speed *= 14.0f;
	} else {
		switch (chr->aibot->config->difficulty) {
		case BOTDIFF_MEAT:
			speed *= 5.0f;
			break;
		case BOTDIFF_EASY:
			speed *= 6.2f;
			break;
		default:
		case BOTDIFF_NORMAL:
			speed *= 7.6f;
			break;
		case BOTDIFF_HARD:
			speed *= 9.4f;
			break;
		case BOTDIFF_PERFECT:
			speed *= 11.2f;
			break;
		case BOTDIFF_DARK:
			speed *= 11.2f;
			break;
		}
	}

	if (bot_guess_crouch_pos(chr) == CROUCHPOS_SQUAT) {
		speed *= 0.35f;
	} else if (bot_guess_crouch_pos(chr) == CROUCHPOS_DUCK) {
		speed *= 0.5f;
	} else if (chr->actiontype == ACT_GOPOS
			&& chr->act_gopos.waypoints[chr->act_gopos.curindex] == NULL
			&& chr_get_lateral_distance_to_coord(chr, &chr->act_gopos.endpos) < 200) {
		speed *= 0.5f;
	}

	return speed;
}

#if VERSION >= VERSION_NTSC_1_0
void bot_update_lateral(struct chrdata *chr, f32 *move, s32 numupdates, f32 arg3)
#else
void bot_update_lateral(struct chrdata *chr, f32 *move)
#endif
{
	s32 i;
	f32 sp50;
	f32 cosine;
	f32 sine;
	u32 stack[4];
	f32 sp30[2];
	f32 speedsideways;
	f32 speedforwards;
	f32 speed;
	f32 tmp;

	if (!chr || !chr->aibot) {
		return;
	}

	if (g_Vars.lvframe60 < 145) {
		move[0] = 0;
		move[1] = 0;
		return;
	}

	speedsideways = chr->aibot->speedmultsideways;
	speedforwards = chr->aibot->speedmultforwards;

	speed = bot_calculate_max_speed(chr);

	speedsideways *= speed;
	speedforwards *= speed;

	sp50 = chr_get_rot_y(chr);

	cosine = cosf(sp50);
	sine = sinf(sp50);

	sp30[0] = speedsideways * cosine + speedforwards * sine;
	sp30[1] = -speedsideways * sine + speedforwards * cosine;

	move[0] = 0;
	move[1] = 0;

#if VERSION >= VERSION_NTSC_1_0
	tmp = (PAL ? 0.065f : 0.055000007152557f) * arg3 / numupdates;

	for (i = 0; i < numupdates; i++) {
		chr->aibot->moveratex = (PAL ? 0.935f : 0.945f) * chr->aibot->moveratex + sp30[0];
		chr->aibot->moveratey = (PAL ? 0.935f : 0.945f) * chr->aibot->moveratey + sp30[1];

		move[0] += chr->aibot->moveratex * tmp;
		move[1] += chr->aibot->moveratey * tmp;
	}
#else
	tmp = (PAL ? 0.065f : 0.055000007152557f) * g_Vars.lvupdate60freal / g_Vars.lvupdate240;

	for (i = 0; i < g_Vars.lvupdate240; i++) {
		chr->aibot->moveratex = (PAL ? 0.935f : 0.945f) * chr->aibot->moveratex + sp30[0];
		chr->aibot->moveratey = (PAL ? 0.935f : 0.945f) * chr->aibot->moveratey + sp30[1];

		move[0] += chr->aibot->moveratex * tmp;
		move[1] += chr->aibot->moveratey * tmp;
	}
#endif
}

char *bot_get_command_name(s32 command)
{
	static u32 names[] = {
		L_MISC_175, // "Follow"
		L_MISC_176, // "Attack"
		L_MISC_177, // "Defend"
		L_MISC_178, // "Hold"
		L_MISC_179, // "Normal"
		L_MISC_180, // "Download"
		L_MISC_181, // "Get Case"
		L_MISC_182, // "Tag Box"
		L_MISC_209, // "Save Case"
		L_MISC_210, // "Def Hill"
		L_MISC_211, // "Hold Hill"
		L_MISC_212, // "Get Case"
		L_MISC_213, // "Pop Cap"
		L_MISC_214, // "Protect"
	};

	if (command < 0 || command >= ARRAYCOUNT(names)) {
		return lang_get(L_MISC_179); // "Normal"
	}

	return lang_get(names[command]);
}

void bot_apply_attack(struct chrdata *chr, struct prop *prop)
{
	chr->aibot->command = AIBOTCMD_ATTACK;
	chr->aibot->attackpropnum = prop - g_Vars.props;
	chr->aibot->forcemainloop = true;
}

void bot_apply_follow(struct chrdata *chr, struct prop *prop)
{
	chr->aibot->command = AIBOTCMD_FOLLOW;
	chr->aibot->followprotectpropnum = prop - g_Vars.props;
	chr->aibot->forcemainloop = true;
}

void bot_apply_protect(struct chrdata *chr, struct prop *prop)
{
	chr->aibot->command = AIBOTCMD_PROTECT;
	chr->aibot->followprotectpropnum = prop - g_Vars.props;
	chr->aibot->forcemainloop = true;
}

void bot_apply_defend(struct chrdata *chr, struct coord *pos, RoomNum *room, f32 angle)
{
	chr->aibot->command = AIBOTCMD_DEFEND;
	chr->aibot->defendholdpos.x = pos->x;
	chr->aibot->defendholdpos.y = pos->y;
	chr->aibot->defendholdpos.z = pos->z;
	rooms_copy(room, chr->aibot->defendholdrooms);
	chr->aibot->defendholdrot = angle;
	chr->aibot->forcemainloop = true;
}

void bot_apply_hold(struct chrdata *chr, struct coord *pos, RoomNum *room, f32 angle)
{
	chr->aibot->command = AIBOTCMD_HOLD;
	chr->aibot->defendholdpos.x = pos->x;
	chr->aibot->defendholdpos.y = pos->y;
	chr->aibot->defendholdpos.z = pos->z;
	rooms_copy(room, chr->aibot->defendholdrooms);
	chr->aibot->defendholdrot = angle;
	chr->aibot->forcemainloop = true;
}

void bot_apply_scenario_command(struct chrdata *chr, u32 command)
{
	chr->aibot->command = command;
	chr->aibot->forcemainloop = true;
}

void bot_disarm(struct chrdata *chr, struct prop *attackerprop)
{
	if (chr->aibot->weaponnum >= WEAPON_FALCON2 && chr->aibot->weaponnum != WEAPON_BRIEFCASE2) {
		struct prop *prop = NULL;
		struct defaultobj *obj;

		if (chr->weapons_held[HAND_LEFT]) {
			obj = chr->weapons_held[HAND_LEFT]->obj;

			obj->hidden |= OBJHFLAG_DELETING;
			chr->weapons_held[HAND_LEFT] = NULL;
		}

		if (chr->weapons_held[HAND_RIGHT]) {
			prop = chr->weapons_held[HAND_RIGHT];
			weapon_set_gunfire_visible(prop, false, -1);
			chr->weapons_held[HAND_RIGHT] = NULL;
		} else {
			s32 modelnum = playermgr_get_model_of_weapon(chr->aibot->weaponnum);

			if (modelnum >= 0) {
				prop = weapon_create_for_chr(chr, modelnum, chr->aibot->weaponnum, OBJFLAG_WEAPON_AICANNOTUSE, NULL, NULL);
			}
		}

		if (prop && prop->obj) {
			obj = prop->obj;
			obj_set_dropped(prop, DROPTYPE_DEFAULT);
			chr->hidden |= CHRHFLAG_DROPPINGITEM;

			if (obj->hidden & OBJHFLAG_PROJECTILE) {
				obj->projectile->pickuptimer240 = TICKS(240);
				obj->projectile->pickupby = attackerprop;
			}
		}

		botinv_remove_item(chr, chr->aibot->weaponnum);

		chr->aibot->loadedammo[0] = 0;
		chr->aibot->loadedammo[1] = 0;

		botinv_switch_to_weapon(chr, WEAPON_UNARMED, FUNC_PRIMARY);
	}
}

/**
 * Set the bot's target and update tracking figures.
 *
 * This should be called on each tick even if the target hasn't changed
 * because the tracking figures need to be constantly updated.
 */
void bot_set_target(struct chrdata *botchr, s32 propnum)
{
	struct chrdata *otherchr = NULL;
	s32 index;

	if (propnum >= 0) {
		otherchr = (g_Vars.props + propnum)->chr;

		index = mp_chr_to_chrindex(otherchr);

		botchr->aibot->targetinsight = botchr->aibot->chrsinsight[index];
		botchr->aibot->targetlastseen60 = botchr->aibot->chrslastseen60[index];
	} else {
		botchr->aibot->targetinsight = false;
		botchr->aibot->targetlastseen60 = -1;
	}

	if (botchr->aibot->targetlastseen60 > botchr->aibot->lastseenanytarget60) {
		botchr->aibot->lastseenanytarget60 = botchr->aibot->targetlastseen60;
	}

	if (botchr->target != propnum) {
		botchr->target = propnum;
		botchr->aibot->shootdelaytimer60 = 0;
		botchr->aibot->waypoints[0] = NULL;
		botchr->aibot->numwaystepstotarget = 0;

		if (botchr->aibot->targetinsight && otherchr) {
			botchr->aibot->targetcloaktimer60 = TICKS(120);
		} else {
			botchr->aibot->targetcloaktimer60 = 0;
		}
	} else {
		if (botchr->aibot->targetinsight) {
			if (g_Vars.lvupdate240 > 0) {
				botchr->aibot->shootdelaytimer60 += g_Vars.diffframe60;
			}
		} else {
			if (g_Vars.lvupdate240 > 0) {
				botchr->aibot->shootdelaytimer60 -= g_Vars.diffframe60;
			}

			if (botchr->aibot->shootdelaytimer60 < 0) {
				botchr->aibot->shootdelaytimer60 = 0;
			}
		}
	}

	if (botchr->aibot->targetinsight && otherchr) {
		if ((otherchr->hidden & CHRHFLAG_CLOAKED) == 0) {
			botchr->aibot->targetcloaktimer60 = TICKS(120);
		} else {
			if (botchr->aibot->targetcloaktimer60 > 0) {
				botchr->aibot->targetcloaktimer60 -= g_Vars.lvupdate60;
			}
		}
	} else {
		botchr->aibot->targetcloaktimer60 = 0;
	}
}

bool bot_is_target_invisible(struct chrdata *botchr, struct chrdata *otherchr)
{
	if (otherchr->prop->type == PROPTYPE_PLAYER && !g_Vars.bondvisible) {
		return true;
	}

	if (otherchr->chrflags & CHRCFLAG_HIDDEN) {
		return true;
	}

	if ((otherchr->hidden & CHRHFLAG_CLOAKED)) {
		if (botchr && botchr->aibot
				&& ((botchr->target != -1 && chr_get_target_prop(botchr) == otherchr->prop && botchr->aibot->targetcloaktimer60 > 0)
				|| (botchr->aibot->canseecloaked && chr_is_looking_at_pos(botchr, &otherchr->prop->pos, 32)))) {
			return false;
		}

		return true;
	}

	return false;
}

/**
 * Return true if there's ground between the chr and the death barrier.
 *
 * The death barrier is at -30000.
 * It's assumed that no walkable ground exists below -20000.
 */
bool bot_has_ground(struct chrdata *chr)
{
	return chr->ground >= -20000;
}

/**
 * The zero angle is the additional angle that's needed for the bot to zero in
 * on its target from its natural facing angle.
 */
void bot_update_zero_angle(struct chrdata *chr)
{
	struct aibot *aibot = chr->aibot;
	s32 diff = aibot->config->difficulty;
	s32 i;
	f32 minspeed;
	f32 maxspeed;
	f32 tmp;
	f32 frac;

	// Consider recalculating random values
	aibot->random3ttl60 -= g_Vars.lvupdate60;

	if (aibot->random3ttl60 <= 0) {
		aibot->random3 = random();
		aibot->random3ttl60 = TICKS(20) + random() % TICKS(20);
	}

	// Increase or decrease curzerotimer60 depending on whether the target is in sight or not
	if (g_Vars.lvupdate240 > 0) {
		if (aibot->targetinsight) {
			aibot->curzerotimer60 += g_Vars.diffframe60;
		} else {
			aibot->curzerotimer60 -= g_Vars.diffframe60;
		}

		// If the bot is naturally turning, decrease the zero timer
		tmp = g_BotDifficulties[diff].turnunzeromult * (aibot->speedtheta * g_Vars.lvupdate60f);

		if (tmp < 0) {
			tmp = -tmp;
		}

		aibot->curzerotimer60 -= tmp;
	}

	// Don't allow the bot to zero faster than its shoot delay timer.
	// ie. The bot should not zero its target then have to wait before it can shoot.
	if (aibot->curzerotimer60 > aibot->shootdelaytimer60) {
		aibot->curzerotimer60 = aibot->shootdelaytimer60;
	}

	if (aibot->curzerotimer60 < 0) {
		aibot->curzerotimer60 = 0;
	}

	// Figure out the turn speed that will be used.
	// The actual speed will be somewhere between minspeed and maxspeed using randomness.
	if (aibot->curzerotimer60 >= g_BotDifficulties[diff].zerotime60) {
		aibot->curzerotimer60 = g_BotDifficulties[diff].zerotime60;
		minspeed = 0;
		maxspeed = 0;
	} else {
		frac = (g_BotDifficulties[diff].zerotime60 - aibot->curzerotimer60) / g_BotDifficulties[diff].zerotime60;
		minspeed = g_BotDifficulties[diff].minzerospeed * frac;
		maxspeed = g_BotDifficulties[diff].maxzerospeed * frac;
	}

	// If the bot's target is cloaked, make sure the max speed is at least a
	// certain value. So the bot can zero faster when their target is cloaked?
	// Rare might have intended for this to apply to zeroinc instead.
	if (chr->target != -1) {
		if (chr_get_target_prop(chr)->chr->hidden & CHRHFLAG_CLOAKED) {
			if (maxspeed < g_BotDifficulties[diff].zerocloakspeed) {
				maxspeed = g_BotDifficulties[diff].zerocloakspeed;
			}
		}
	}

	// Force a minimum max speed (applies if the bot is just starting to zero).
	// As above, Rare might have intended for this to apply to zeroinc instead.
	if (maxspeed < g_BotDifficulties[diff].forcezerominspeed) {
		maxspeed = g_BotDifficulties[diff].forcezerominspeed;
	}

	// Calculate the actual speed increment to use
	aibot->zeroinc = (maxspeed - minspeed) * (aibot->random3 % 0x10000) * (1.0f / 65535.0f) + minspeed;

	// Negating this seems like a weird choice... unsure what effect this has
	if (aibot->random3 & 0x10000) {
		aibot->zeroinc = -aibot->zeroinc;
	}

	for (i = 0; i < g_Vars.lvupdate240; i++) {
		aibot->zerospeed = aibot->zerospeed * (PAL ? 0.97f : 0.97500002384186f) + aibot->zeroinc;
	}

	aibot->zeroangle = aibot->zerospeed * (PAL ? 0.029999971389771f : 0.024999976158142f);
}

/**
 * Return true if the bot is a peacesim and is happy to fight the given chr,
 * or if the bot is not a peacesim.
 */
bool bot_passes_peace_check(struct chrdata *botchr, struct chrdata *otherchr)
{
	struct aibot *aibot = botchr->aibot;
	bool pass = true;

	if (aibot->config->type == BOTTYPE_PEACE) {
		s32 otherweaponnum = bot_get_weapon_num(otherchr);

		if (otherweaponnum == WEAPON_NONE || otherweaponnum == WEAPON_UNARMED) {
			pass = false;
		}
	}

	return pass;
}

/**
 * Return true if the bot is a cowardsim and is happy to fight the given chr,
 * or if the bot is not a cowardsim.
 */
bool bot_passes_coward_check(struct chrdata *botchr, struct chrdata *otherchr)
{
	struct aibot *aibot = botchr->aibot;
	bool pass = true;
	s32 otherweaponnum;
	s32 myscore1;
	s32 myscore2;
	s32 theirscore1;
	s32 theirscore2;

	if (aibot->config->type == BOTTYPE_COWARD) {
		otherweaponnum = bot_get_weapon_num(otherchr);

		botinv_score_weapon(botchr, aibot->weaponnum, FUNC_PRIMARY, 1, false, &myscore1, &myscore2, false, false);
		botinv_score_weapon(botchr, otherweaponnum, FUNC_PRIMARY, 1, false, &theirscore1, &theirscore2, false, false);

		if (theirscore1 >= myscore1 - 30) {
			pass = false;
		}
	}

	return pass;
}

/**
 * Choose and assign a general target to chase and attack.
 *
 * The function considers the distances and visibility of other chrs.
 *
 * The function does not compare weapons with the target, nor ammo counts,
 * and does not factor in the bot types (eg. VengeSim).
 */
void bot_choose_general_target(struct chrdata *botchr)
{
	struct aibot *aibot = botchr->aibot;
	s32 i;
	s32 j;
	bool distancesdone[MAX_MPCHRS];
	RoomNum room = -1;
	struct chrdata *trychr;
	s32 playernum;

	// Advance the bot's internal pointer to the next chr
	// and update stats about that chr
	aibot->queryplayernum = (aibot->queryplayernum + 1) % g_MpNumChrs;

	trychr = mp_chrindex_to_chr(aibot->queryplayernum);

	if (trychr != botchr) {
		// This condition passes on average once every 4 minutes per player.
		// However, the usage of canseecloaked appears to be botched.
		// It is implemented in bot_is_target_invisible, but that function is not
		// called here while canseecloaked is true.
		if (random() % TICKS(4 * 60 * 60) < g_MpNumChrs * g_Vars.lvupdate60) {
			aibot->canseecloaked = true;
		}

		aibot->chrdistances[aibot->queryplayernum] = chr_get_distance_to_coord(botchr, &trychr->prop->pos);
		aibot->chrsinsight[aibot->queryplayernum] = chr_has_los_to_chr(botchr, trychr, &room);
		aibot->chrrooms[aibot->queryplayernum] = room;

		aibot->canseecloaked = false;
	}

	// Update last seen timestamps for all visible chrs
	for (i = 0; i < g_MpNumChrs; i++) {
		if (aibot->chrsinsight[i]) {
			aibot->chrslastseen60[i] = g_Vars.lvframe60;
		}
	}

	// Update chrnumsbydistanceasc
	for (i = 0; i < g_MpNumChrs; i++) {
		distancesdone[i] = false;
	}

	for (i = 0; i < g_MpNumChrs; i++) {
		s32 closestplayernum = -1;
		f32 closestdistance = 0;

		for (j = 0; j < g_MpNumChrs; j++) {
			if (!distancesdone[j] && (closestplayernum < 0 || aibot->chrdistances[j] < closestdistance)) {
				closestplayernum = j;
				closestdistance = aibot->chrdistances[j];
			}
		}

		if (closestplayernum >= 0) {
			aibot->chrnumsbydistanceasc[i] = closestplayernum;
			distancesdone[closestplayernum] = true;
		}
	}

	bot_update_zero_angle(botchr);

	// If the bot is data uplinking, clear the target
	if (botchr->myaction == MA_AIBOTDOWNLOAD) {
		bot_set_target(botchr, -1);
		return;
	}

	// If the bot is attacking, keep the same target if possible
	if (botchr->myaction == MA_AIBOTATTACK
			&& aibot->attackingplayernum >= 0
			&& aibot->chrsinsight[aibot->attackingplayernum]
			&& !chr_is_dead(g_MpAllChrPtrs[aibot->attackingplayernum])) {
		bot_set_target(botchr, g_MpAllChrPtrs[aibot->attackingplayernum]->prop - g_Vars.props);
		return;
	}

	// Check if existing target needs to be invalidated
	if (botchr->target != -1) {
		struct prop *targetprop = chr_get_target_prop(botchr);

		if (chr_is_dead(targetprop->chr)) {
			botchr->target = -1;
		}

		if (!botchr->aibot->targetinsight && bot_is_target_invisible(botchr, targetprop->chr)) {
			botchr->target = -1;
		}

		if (chr_compare_teams(botchr, targetprop->chr, COMPARE_FRIENDS)) {
			botchr->target = -1;
		}

		if (!bot_passes_peace_check(botchr, targetprop->chr)) {
			botchr->target = -1;
		}

		if (!botchr->aibot->targetinsight && !bot_passes_coward_check(botchr, targetprop->chr)) {
			botchr->target = -1;
		}
	}

	// If there's no existing target, try all chrs in distance order
	if (botchr->target == -1) {
		s32 closestavailablechrnum = -1;
		s32 tmp;
		s32 stack;

		for (tmp = 0; tmp < g_MpNumChrs; tmp++) {
			s32 i = aibot->chrnumsbydistanceasc[tmp];
			trychr = mp_chrindex_to_chr(i);

			if (trychr != botchr
					&& !chr_is_dead(trychr)
					&& chr_compare_teams(botchr, trychr, COMPARE_ENEMIES)
					&& bot_passes_peace_check(botchr, trychr)) {
				// If the chr is in sight, that's it
				if (aibot->chrsinsight[i]) {
					bot_set_target(botchr, trychr->prop - g_Vars.props);
					return;
				}

				// Meat and easy sims will target the closest chr, even if that
				// chr isn't in sight and when there are other chrs in sight who
				// are further away
				if (!bot_is_target_invisible(botchr, trychr)
						&& (aibot->config->difficulty == BOTDIFF_MEAT || aibot->config->difficulty == BOTDIFF_EASY)) {
					bot_set_target(botchr, trychr->prop - g_Vars.props);
					return;
				}

				// Other sim types will prioritise chrs in sight, which means
				// the closest out of sight chrnum must be stored for later
				if (!bot_is_target_invisible(botchr, trychr) && closestavailablechrnum < 0) {
					closestavailablechrnum = i;
				}
			}
		}

		// Use closest out of sight chr
		if (closestavailablechrnum >= 0) {
			trychr = mp_chrindex_to_chr(closestavailablechrnum);
			bot_set_target(botchr, trychr->prop - g_Vars.props);
			return;
		}

		// No one available - maybe everyone else is dead or cloaked
		bot_set_target(botchr, -1);
		return;
	}

	// Bot has an existing target
	// If they're still in sight, keep the target
	playernum = mp_chr_to_chrindex((g_Vars.props + botchr->target)->chr);

	if (aibot->chrsinsight[playernum]) {
		bot_set_target(botchr, botchr->target);
		return;
	}

	// Target is no longer in sight
	// Check for other chrs who are in sight, by distance
	for (i = 0; i < g_MpNumChrs; i++) {
		if (aibot->chrsinsight[aibot->chrnumsbydistanceasc[i]]) {
			trychr = mp_chrindex_to_chr(aibot->chrnumsbydistanceasc[i]);

			if (trychr != botchr
					&& !chr_is_dead(trychr)
					&& chr_compare_teams(botchr, trychr, COMPARE_ENEMIES)
					&& bot_passes_peace_check(botchr, trychr)) {
				bot_set_target(botchr, trychr->prop - g_Vars.props);
				return;
			}
		}
	}

	// No one else in sight - maintain original target
	bot_set_target(botchr, botchr->target);
}

/**
 * Check if the bot is capable of following the given chr.
 *
 * They are not capable if it would create a circular follow loop.
 */
bool bot_can_follow(struct chrdata *botchr, struct chrdata *leader)
{
	bool canfollow = true;

	while (true) {
		struct aibot *aibot = leader->aibot;

		if (!aibot || leader->myaction != MA_AIBOTFOLLOW || aibot->followingplayernum < 0) {
			// Okay to follow
			break;
		}

		leader = g_MpAllChrPtrs[aibot->followingplayernum];

		if (leader == botchr) {
			// Can't follow - it would create a follow loop
			canfollow = false;
			break;
		}
	}

	return canfollow;
}

s32 bot_find_teammate_to_follow(struct chrdata *chr, f32 range)
{
	s32 result = -1;

	if ((g_MpSetup.options & MPOPTION_TEAMSENABLED)
			&& chr->myaction != MA_AIBOTFOLLOW
			&& (random() % 100) < chr->aibot->followchance) {
		f32 closestdistance = 0;
		s32 closestplayernum = -1;
		s32 i;

		for (i = 0; i < g_MpNumChrs; i++) {
			if (chr != g_MpAllChrPtrs[i]
					&& !chr_is_dead(g_MpAllChrPtrs[i])
					&& chr->team == g_MpAllChrPtrs[i]->team
					&& bot_can_follow(chr, g_MpAllChrPtrs[i])) {
				f32 distance = chr->aibot->chrdistances[i];

				if (closestplayernum < 0 || distance < closestdistance) {
					closestplayernum = i;
					closestdistance = distance;
				}
			}
		}

		if (closestplayernum >= 0 && closestdistance < range) {
			result = closestplayernum;
		}
	}

	return result;
}

void bot_schedule_reload(struct chrdata *chr, s32 handnum)
{
	chr->aibot->timeuntilreload60[handnum] = g_BotWeaponConfigs[chr->aibot->weaponnum].reloaddelay * (PAL ? 50 : 60);

	if (g_BotWeaponConfigs[chr->aibot->weaponnum].allowpartialreloaddelay) {
		s32 capacity = botact_get_clip_capacity_by_function(chr->aibot->weaponnum, chr->aibot->gunfunc);

		chr->aibot->timeuntilreload60[handnum] *= capacity - chr->aibot->loadedammo[handnum];
		chr->aibot->timeuntilreload60[handnum] /= capacity;
	}
}

#define HASENOUGHPRI(aibot, weaponnum, goal) (g_BotWeaponConfigs[weaponnum].haspriammogoal && botact_get_ammo_quantity_by_weapon(aibot, weaponnum, FUNC_PRIMARY, true) >= (goal))
#define HASENOUGHSEC(aibot, weaponnum, goal) (g_BotWeaponConfigs[weaponnum].hassecammogoal && botact_get_ammo_quantity_by_weapon(aibot, weaponnum, FUNC_SECONDARY, true) >= (goal))

/**
 * Find a prop for the bot to pick up.
 *
 * Criteria can be:
 *
 * PICKUPCRITERIA_DEFAULT:
 *     This is the most common criteria. It is used when the bot has spawned and
 *     is gathering weapons, as well as when they return to their main loop.
 *
 * PICKUPCRITERIA_CRITICAL:
 *     Find props only if the bot needs them critically, eg if they are low on
 *     shield or ammo. This is used when the bot has other things it can be
 *     doing such as scenario objectives and has to decide between the two.
 *
 * PICKUPCRITERIA_ANY:
 *     Find pretty much any prop. This is used when the bot has nothing else to
 *     do (eg. if all opponents are cloaked) and may as well stock up on ammo.
 */
struct prop *bot_find_pickup(struct chrdata *chr, s32 criteria)
{
	struct aibot *aibot = chr->aibot;
	s32 weaponnums[NUM_MPWEAPONSLOTS];
	s32 scores1[NUM_MPWEAPONSLOTS];
	s32 scores2[NUM_MPWEAPONSLOTS];
	struct prop *weapproplist[NUM_MPWEAPONSLOTS];
	f32 weapdistlist[NUM_MPWEAPONSLOTS];
	struct prop *ammoproplist[33];
	f32 ammodistlist[33];
	struct invitem *invitems[NUM_MPWEAPONSLOTS];
	s32 i;
	s32 j;
	struct prop *prop;
	struct weaponobj *weapon;
	struct prop *chosenprop = NULL;
	bool barelydominatinghill = false;
	s32 numteam;
	s32 numopponents;
	struct multiammocrateobj *crate;
	s32 weaponnum;
	f32 sqdist1;
	f32 sqdist2;
	struct defaultobj *obj;
	s32 ammotype;
	s32 bestscore1;
	bool done;

	if (&aibot);
	if (&criteria);

	// If the hill has one or two bots from the same team in it, the bots will
	// be less likely to leave the hill for pickups (barelydominatinghill = true).
	// If there are three or more then this limitation is removed.
	// The amount increases if there are opponents in the hill too.
	if (aibot->teamisonlyai
			&& g_MpSetup.scenario == MPSCENARIO_KINGOFTHEHILL
			&& chr->prop->rooms[0] == g_ScenarioData.koh.hillrooms[0]) {
		numteam = bot_get_num_teammates_defending_hill(chr);
		numopponents = bot_get_num_opponents_in_hill(chr);

		if (numteam >= numopponents && numteam <= numopponents + 2) {
			barelydominatinghill = true;
		}
	}

	// botinv_score_all_weapons populates weaponnums, scores1 and scores2
	// and sorts them by score1 descending
	botinv_score_all_weapons(chr, weaponnums, scores1, scores2);

	for (i = 0; i < ARRAYCOUNT(weapproplist); i++) {
		weapproplist[i] = NULL;
	}

	for (i = 0; i < ARRAYCOUNT(ammoproplist); i++) {
		ammoproplist[i] = NULL;
	}

	for (i = 0; i < ARRAYCOUNT(weaponnums); i++) {
		invitems[i] = botinv_get_item(chr, weaponnums[i]);
	}

	// Iterate all active props and populate the proplist and distlist arrays.
	// Generally these arrays are populated with the closest prop of each weapon
	// and ammotype, however there's a 1/16 chance that any prop will be skipped
	// and a 1/16 chance that a further prop will overwrite the current closest.
	prop = g_Vars.activeprops;

	while (prop) {
		if (prop->parent == NULL && prop->timetoregen == 0) {
			if (prop->type == PROPTYPE_WEAPON) {
				weapon = prop->weapon;

				if ((weapon->base.flags3 & OBJFLAG3_ISFETCHTARGET) == 0) {
					sqdist1 = chr_get_squared_distance_to_coord(chr, &prop->pos);

					for (i = 0; i < ARRAYCOUNT(weaponnums); i++) {
						if (weaponnums[i] > WEAPON_UNARMED && weaponnums[i] == weapon->weaponnum) {
							if (random() % 16) {
								if (weapproplist[i] == NULL || sqdist1 < weapdistlist[i] || random() % 16 == 0) {
									weapproplist[i] = prop;
									weapdistlist[i] = sqdist1;
								}
							}
							break;
						}
					}

					ammotype = botact_get_ammo_type_by_function(weapon->weaponnum, FUNC_PRIMARY);

					if (ammotype > 0 && random() % 16) {
						if (ammoproplist[ammotype] == NULL || sqdist1 < ammodistlist[ammotype] || random() % 16 == 0) {
							ammoproplist[ammotype] = prop;
							ammodistlist[ammotype] = sqdist1;
						}
					}
				}
			} else if (prop->type == PROPTYPE_OBJ) {
				obj = prop->obj;

				if ((obj->flags3 & OBJFLAG3_ISFETCHTARGET) == 0) {
					if (obj->type == OBJTYPE_MULTIAMMOCRATE) {
						crate = (struct multiammocrateobj *)prop->obj;
						sqdist2 = chr_get_squared_distance_to_coord(chr, &prop->pos);

						for (i = 0; i < 19; i++) {
							s32 ammotype = i + 1;

							if (crate->slots[i].quantity > 0) {
								weaponnum = botact_get_weapon_by_ammo_type(ammotype);

								if (weaponnum > 0) {
									for (j = 0; j < ARRAYCOUNT(weaponnums); j++) {
										if (weaponnums[j] > WEAPON_UNARMED && weaponnum == weaponnums[j]) {
											if (random() % 16) {
												if (weapproplist[j] == NULL || sqdist2 < weapdistlist[j] || random() % 16 == 0) {
													weapproplist[j] = prop;
													weapdistlist[j] = sqdist2;
												}
											}
											break;
										}
									}
								}

								if (random() % 16) {
									if (ammoproplist[ammotype] == NULL || sqdist2 < ammodistlist[ammotype] || random() % 16 == 0) {
										ammoproplist[ammotype] = prop;
										ammodistlist[ammotype] = sqdist2;
									}
								}
							}
						}
					} else if (obj->type == OBJTYPE_SHIELD) {
						for (i = 0; i < ARRAYCOUNT(weaponnums); i++) {
							if (weaponnums[i] == WEAPON_MPSHIELD) {
								sqdist2 = chr_get_squared_distance_to_coord(chr, &prop->pos);

								if (random() % 16 == 0) {
									break;
								}

								if (weapproplist[i] == NULL || sqdist2 < weapdistlist[i] || random() % 16 == 0) {
									weapproplist[i] = prop;
									weapdistlist[i] = sqdist2;
								}

								break;
							}
						}
					}
				}
			}
		}

		prop = prop->next;
	}

	// Find the best score out of the 6 weapons, only considering ones which
	// the bot is allowed to carry and which require ammo
	bestscore1 = 0;
	done = false;

	for (i = 0; i < ARRAYCOUNT(weaponnums); i++) {
		if (1);
		if ((botinv_allows_weapon(chr, weaponnums[i], FUNC_PRIMARY) || botinv_allows_weapon(chr, weaponnums[i], FUNC_SECONDARY))
				&& (g_BotWeaponConfigs[weaponnums[i]].haspriammogoal || g_BotWeaponConfigs[weaponnums[i]].hassecammogoal)
				&& scores1[i] > bestscore1) {
			bestscore1 = scores1[i];
		}
	}

	// Decide if the bot wants to find a shield, based on the amount of health
	// and shield the bot currently has.
	// Note that max health and shield is 8 each, and that the bot must be under
	// BOTH the limits for a shield to be fetched.
	for (i = 0; i < ARRAYCOUNT(weaponnums) && !done; i++) {
		if (weaponnums[i] == WEAPON_MPSHIELD
				&& (g_MpSetup.scenario != MPSCENARIO_HOLDTHEBRIEFCASE || !chr->aibot->hasbriefcase)) {
			f32 triggerathealth = 8.1f;
			f32 desiredshield = 0;
			s32 rand;

			if (aibot->config->type == BOTTYPE_SHIELD) {
				// ShieldSims are more likely to fetch shields
				if (criteria == PICKUPCRITERIA_ANY) {
					desiredshield = 7.9f;
				} else if (criteria == PICKUPCRITERIA_DEFAULT) {
					desiredshield = 6 - (aibot->randomfrac + aibot->randomfrac);
				} else if (criteria == PICKUPCRITERIA_CRITICAL) {
					desiredshield = 4 - (aibot->randomfrac + aibot->randomfrac);
				}
			} else if (barelydominatinghill) {
				// Bots will be less likely to fetch shields while defending the hill
				triggerathealth = 4 - (aibot->randomfrac + aibot->randomfrac);
				desiredshield = 1 - aibot->randomfrac;
			} else if (g_MpSetup.scenario == MPSCENARIO_CAPTURETHECASE && bot_should_return_ctc_token(chr)) {
				// Bots will be less likely to fetch shields while returning a CTC case
				triggerathealth = 3 - (aibot->randomfrac + aibot->randomfrac);
			} else if (chr->myaction == MA_AIBOTDOWNLOAD) {
				// Bots will be less likely to fetch shields while uplinking
				triggerathealth = 4 - (aibot->randomfrac + aibot->randomfrac);
				desiredshield = 1;
			} else {
				// Default behaviour
				if (criteria == PICKUPCRITERIA_ANY) {
					desiredshield = 7.9f;
				} else if (criteria == PICKUPCRITERIA_DEFAULT) {
					desiredshield = 4 - (aibot->randomfrac + aibot->randomfrac);
				} else if (criteria == PICKUPCRITERIA_CRITICAL) {
					desiredshield = 2 - aibot->randomfrac;
				}
			}

			// Meat, easy and normal sims reduce the limits further,
			// making them less likely to fetch shields.
			if (aibot->config->difficulty == BOTDIFF_MEAT) {
				rand = aibot->random2 % 8;

				if (rand < 2) {
					desiredshield = 0;
					triggerathealth = 0;
				} else if (rand < 4) {
					desiredshield = 0;
					triggerathealth = 2 - aibot->randomfrac;
				} else {
					desiredshield -= aibot->randomfrac * 16;

					if (desiredshield <= 0) {
						triggerathealth += desiredshield;
						desiredshield = 0;
					}
				}
			} else if (aibot->config->difficulty == BOTDIFF_EASY) {
				rand = aibot->random2 % 8;

				if (rand <= 0) {
					desiredshield = 0;
					triggerathealth = 0;
				} else {
					desiredshield -= aibot->randomfrac * 11;

					if (desiredshield <= 0) {
						triggerathealth += desiredshield;
						desiredshield = 0;
					}
				}
			} else if (aibot->config->difficulty == BOTDIFF_NORMAL) {
				desiredshield -= aibot->randomfrac * 4;

				if (desiredshield <= 0) {
					triggerathealth += desiredshield;
					desiredshield = 0;
				}
			}

			// Actually check the limits and decide if the shield is desired
			if (chr->maxdamage - chr->damage < triggerathealth
					&& chr->cshield <= desiredshield
					&& weapproplist[i] != NULL
					&& scores2[i] >= bestscore1) {
				chosenprop = weapproplist[i];
				done = true;
				break;
			}
		}
	}

	// Consider ammo for weapons that the bot already has.
	// This loop is iterated in order of highest scoring weapons first. If the
	// first iterated weapon which the bot holds has enough ammo then the lower
	// scoring weapons will not be considered, nor will any new weapons be
	// picked up.
	for (i = 0; i < ARRAYCOUNT(weaponnums) && !done; i++) {
		if (weaponnums[i] != WEAPON_MPSHIELD
				&& invitems[i] != NULL
				&& (g_BotWeaponConfigs[weaponnums[i]].haspriammogoal
					|| g_BotWeaponConfigs[weaponnums[i]].hassecammogoal)
				&& scores2[i] >= bestscore1) {
			s32 desiredpriammo;
			s32 desiredsecammo;
			s32 funcnum;
			bool include_equipped = true;
			s32 stack;

			// Don't go after ammo when returning a CTC token
			if (g_MpSetup.scenario == MPSCENARIO_CAPTURETHECASE && bot_should_return_ctc_token(chr)) {
				done = true;
				break;
			}

			// Don't go after ammo when downloading in Hacker Central
			if (chr->myaction == MA_AIBOTDOWNLOAD) {
				done = true;
				break;
			}

			if (barelydominatinghill) {
				// If the bot's team is only barely controlling the hill,
				// don't leave it unless the bot is out of ammo, and even then
				// just get one ammo pickup
				desiredpriammo = g_BotWeaponConfigs[weaponnums[i]].criticalammopri;

				if (desiredpriammo > 1) {
					desiredpriammo = 1;
				}

				desiredsecammo = g_BotWeaponConfigs[weaponnums[i]].criticalammosec;

				if (desiredsecammo > 1) {
					desiredsecammo = 1;
				}

				if (HASENOUGHPRI(aibot, weaponnums[i], desiredpriammo) || HASENOUGHSEC(aibot, weaponnums[i], desiredsecammo)) {
					done = true;
					break;
				}
			} else if (criteria == PICKUPCRITERIA_ANY) {
				// If looking for any pickups at all, use the weapon's ammo
				// capacities as the goal ammo
				desiredpriammo = bgun_get_capacity_by_ammotype(botact_get_ammo_type_by_function(weaponnums[i], FUNC_PRIMARY));
				desiredsecammo = bgun_get_capacity_by_ammotype(botact_get_ammo_type_by_function(weaponnums[i], FUNC_SECONDARY));

				// If bot has max ammo for both weapon's functions
				if ((g_BotWeaponConfigs[weaponnums[i]].haspriammogoal == false
							|| botact_get_ammo_quantity_by_weapon(aibot, weaponnums[i], FUNC_PRIMARY, false) >= desiredpriammo)
						&& (g_BotWeaponConfigs[weaponnums[i]].hassecammogoal == false
							|| botact_get_ammo_quantity_by_weapon(aibot, weaponnums[i], FUNC_SECONDARY, false) >= desiredsecammo)) {
					// Consider next weapon
					continue;
				}

				include_equipped = false;
			} else if (criteria == PICKUPCRITERIA_DEFAULT) {
				// Default - use the target ammo amount
				desiredpriammo = g_BotWeaponConfigs[weaponnums[i]].targetammopri;
				desiredsecammo = g_BotWeaponConfigs[weaponnums[i]].targetammosec;

				if (HASENOUGHPRI(aibot, weaponnums[i], desiredpriammo) || HASENOUGHSEC(aibot, weaponnums[i], desiredsecammo)) {
					done = true;
					break;
				}
			} else if (criteria == PICKUPCRITERIA_CRITICAL) {
				// Critical - use the critical ammo amount
				desiredpriammo = g_BotWeaponConfigs[weaponnums[i]].criticalammopri;
				desiredsecammo = g_BotWeaponConfigs[weaponnums[i]].criticalammosec;

				if (HASENOUGHPRI(aibot, weaponnums[i], desiredpriammo) || HASENOUGHSEC(aibot, weaponnums[i], desiredsecammo)) {
					done = true;
					break;
				}
			}

			// Iterate both weapon functions and check
			// if the bot has enough ammo for that function
			for (funcnum = 0; funcnum < 2; funcnum++) {
				if (botinv_allows_weapon(chr, weaponnums[i], funcnum)) {
					s32 ammotype = botact_get_ammo_type_by_function(weaponnums[i], funcnum);

					if (ammotype > 0) {
						s32 goal = funcnum ? desiredsecammo : desiredpriammo;
						s32 qty = botact_get_ammo_quantity_by_type(aibot, ammotype, include_equipped);

						if (qty < goal && ammoproplist[ammotype]) {
							chosenprop = ammoproplist[ammotype];
							done = true;
							break;
						}
					}
				}
			}
		}
	}

	// If done is still false, the bot mustn't have any weapons.
	// Consider picking up weapons that the bot doesn't have.
	// Fetch the highest scoring weapon if there are any pickups for it.
	for (i = 0; i < ARRAYCOUNT(weaponnums) && !done; i++) {
		if (weaponnums[i] != WEAPON_MPSHIELD) {
			if (g_MpSetup.scenario == MPSCENARIO_CAPTURETHECASE && bot_should_return_ctc_token(chr)) {
				done = true;
				break;
			}

			if (chr->myaction == MA_AIBOTDOWNLOAD) {
				done = true;
				break;
			}

			if (!barelydominatinghill
					&& (botinv_allows_weapon(chr, weaponnums[i], FUNC_PRIMARY) || botinv_allows_weapon(chr, weaponnums[i], FUNC_SECONDARY))
					&& invitems[i] == NULL
					&& weapproplist[i] != NULL) {
				chosenprop = weapproplist[i];
				done = true;
				break;
			}
		}
	}

	if (criteria == PICKUPCRITERIA_ANY) {
		// Consider ammo even for weapons that the bot doesn't have
		for (i = 0; i < ARRAYCOUNT(weaponnums) && !done; i++) {
			if (weaponnums[i] != WEAPON_MPSHIELD) {
				for (j = 0; j < 2; j++) {
					if (botinv_allows_weapon(chr, weaponnums[i], j)) {
						s32 ammotype = botact_get_ammo_type_by_function(weaponnums[i], j);

						if (ammotype > 0
								&& botact_get_ammo_quantity_by_type(aibot, ammotype, false) < bgun_get_capacity_by_ammotype(ammotype)
								&& ammoproplist[ammotype] != NULL) {
							chosenprop = ammoproplist[ammotype];
							done = true;
							break;
						}
					}
				}
			}
		}
	}

	return chosenprop;
}

/**
 * Check if the bot wants to do a critical pickup.
 *
 * This returns true when the bot is low on health or ammo and there are pickups
 * available.
 */
bool bot_can_do_critical_pickup(struct chrdata *chr)
{
	return bot_find_pickup(chr, PICKUPCRITERIA_CRITICAL) != NULL;
}

/**
 * Find a pickup to fetch based on default criteria. Default criteria basically
 * means a good amount of ammo - not lacking but not excessive either.
 */
struct prop *bot_find_default_pickup(struct chrdata *chr)
{
	return bot_find_pickup(chr, PICKUPCRITERIA_DEFAULT);
}

/**
 * Find any pickup to fetch. This is used when the bot has nothing else to do
 * (eg. if all opponents are cloaked).
 */
struct prop *bot_find_any_pickup(struct chrdata *chr)
{
	return bot_find_pickup(chr, PICKUPCRITERIA_ANY);
}

s32 bot_get_team_size(struct chrdata *chr)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < g_MpNumChrs; i++) {
		if (chr->team == g_MpAllChrPtrs[i]->team) {
			count++;
		}
	}

	return count;
}

s32 bot_get_count_in_team_doing_command(struct chrdata *self, u32 command, bool includeself)
{
	s32 count = 0;
	s32 i;

	for (i = PLAYERCOUNT(); i < g_MpNumChrs; i++) {
		if (self->team == g_MpAllChrPtrs[i]->team) {
			if (includeself || self != g_MpAllChrPtrs[i]) {
				if (command == g_MpAllChrPtrs[i]->aibot->command) {
					count++;
				}
			}
		}
	}

	return count;
}

s32 bot_is_chrs_ctc_token_held(struct chrdata *chr)
{
	struct mpchrconfig *mpchr = g_MpAllChrConfigPtrs[mp_chr_to_chrindex(chr)];
	struct prop *prop = g_ScenarioData.ctc.tokens[mpchr->team];

	return prop && (prop->type == PROPTYPE_CHR || prop->type == PROPTYPE_PLAYER);
}

/**
 * If chr doesn't have the case, return false.
 * If chr has the case:
 *     If chr is on a team by themself and their token is stolen, return false
 *     Otherwise, return true
 */
bool bot_should_return_ctc_token(struct chrdata *chr)
{
	if (chr->aibot->hascase) {
		if (!chr->aibot->teamisonlyai || bot_get_team_size(chr) >= 2 || !bot_is_chrs_ctc_token_held(chr)) {
			return true;
		}
	}

	return false;
}

s32 bot_get_num_teammates_defending_hill(struct chrdata *bot)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < g_MpNumChrs; i++) {
		if (bot->team == g_MpAllChrPtrs[i]->team
				&& g_MpAllChrPtrs[i]->prop->rooms[0] == g_ScenarioData.koh.hillrooms[0]) {
			if (g_MpAllChrPtrs[i]->aibot->command == AIBOTCMD_DEFHILL
					|| g_MpAllChrPtrs[i]->aibot->command == AIBOTCMD_HOLDHILL) {
				count++;
			}
		}
	}

	return count;
}

/**
 * Find the opposing team who has the most players in the hill and return the
 * number of their players who are in the hill.
 *
 * This function is slightly misnamed.
 */
s32 bot_get_num_opponents_in_hill(struct chrdata *chr)
{
	struct mpchrconfig *mpchr = g_MpAllChrConfigPtrs[mp_chr_to_chrindex(chr)];
	struct mpchrconfig *loopmpchr;
	s32 countsperteam[MAX_TEAMS] = {0};
	s32 max = 0;
	s32 i;

	for (i = 0; i < g_MpNumChrs; i++) {
		if (g_MpAllChrPtrs[i]->prop->rooms[0] == g_ScenarioData.koh.hillrooms[0]) {
			s32 mpindex = mp_chrindex_to_chrslot(i);

			loopmpchr = MPCHR(mpindex);

			if (loopmpchr->team != mpchr->team) {
				countsperteam[loopmpchr->team]++;
			}
		}
	}

	for (i = 0; i < ARRAYCOUNT(countsperteam); i++) {
		if (countsperteam[i] > max) {
			max = countsperteam[i];
		}
	}

	return max;
}

/**
 * The tick function for a multiplayer simulant while the game is running.
 *
 * The function implements per-scenario decision logic as well as shooting and
 * reloading.
 *
 * General function overview:
 * - Some minor things at the start
 * - If team is only AI, choose scenario commands as if player had assigned them
 * - If the bot is in its main loop:
 *     - Choose a new myaction value based on the command.
 *     - Apply the new myaction if any
 * - If the existing myaction is no longer valid, force a return to the main
 *   loop on the next tick
 * - Decide whether to switch weapons (via botinv_tick)
 * - Decide whether to discharge a shot on this tick
 */
void bot_tick_unpaused(struct chrdata *chr)
{
	s32 newaction = -1;

	if (!chr_is_dead(chr)) {
		struct aibot *aibot = chr->aibot;
		s32 i;

		// Consider updating random values
		aibot->random2ttl60 -= g_Vars.lvupdate60;

		if (aibot->random2ttl60 < 0) {
			aibot->random2ttl60 = TICKS(1800) + random() % TICKS(60 * 240);
			aibot->random2 = random();
			aibot->randomfrac = RANDOMFRAC();
		}

		// Consider reloading
		for (i = 0; i != 2; i++) {
			// Reload if timer has reached 0
			if (aibot->timeuntilreload60[i] > 0) {
				aibot->timeuntilreload60[i] -= g_Vars.lvupdate60;

				if (aibot->timeuntilreload60[i] <= 0) {
					botact_reload(chr, i, true);
				}
			} else if (!botact_is_weapon_throwable(aibot->weaponnum, aibot->gunfunc)) {
				// If the weapon is reloadable, schedule a reload if bot is out
				// of ammo or has less than half a clip and last saw their
				// target 2 seconds ago
				s32 loadedammo = aibot->loadedammo[i];
				s32 clipsize = botact_get_clip_capacity_by_function(aibot->weaponnum, aibot->gunfunc);

				if (loadedammo <= 0 && clipsize > 0) {
					bot_schedule_reload(chr, i);
				} else if (loadedammo < clipsize / 2 && aibot->lastseenanytarget60 < g_Vars.lvframe60 - TICKS(120)) {
					bot_schedule_reload(chr, i);
				}
			}
		}

		// Handle switching weapons
		if (aibot->changeguntimer60 > 0) {
			aibot->changeguntimer60 -= g_Vars.lvupdate60;

			if (aibot->changeguntimer60 <= 0) {
				struct invitem *item = botinv_get_item(chr, aibot->weaponnum);
				s32 modelnum = playermgr_get_model_of_weapon(aibot->weaponnum);
				s32 i;

				if (item && modelnum >= 0) {
					chr_give_weapon(chr, modelnum, aibot->weaponnum, 0);
					botact_reload(chr, HAND_RIGHT, false);

					if (item->type == INVITEMTYPE_DUAL) {
						chr_give_weapon(chr, modelnum, aibot->weaponnum, OBJFLAG_WEAPON_LEFTHANDED);
						botact_reload(chr, HAND_LEFT, false);
					}
				} else {
					// Bot doesn't have the weapon it was told to switch to
					chr->aibot->weaponnum = WEAPON_UNARMED;
					chr->aibot->gunfunc = FUNC_PRIMARY;
					chr->aibot->ismeleeweapon = true;
				}

				aibot->throwtimer60 = 0;

				for (i = 0; i < 2; i++) {
					aibot->punchtimer60[i] = 0;

					if (chr->weapons_held[i]) {
						chr->weapons_held[i]->weapon->gunfunc = chr->aibot->gunfunc;
					}
				}
			}
		}

		// The laser has unlimited ammo
		if (aibot->weaponnum == WEAPON_LASER) {
			chr->aibot->loadedammo[HAND_RIGHT] = 999;
		}

		// Consider starting or stopping cloak
		if (aibot->ammoheld[AMMOTYPE_CLOAK] > 0
				&& (bot_is_about_to_attack(chr, true) || chr->myaction == MA_AIBOTDOWNLOAD)) {
			aibot->cloakdeviceenabled = true;
		} else {
#if VERSION >= VERSION_PAL_FINAL
			if (aibot->ammoheld[AMMOTYPE_CLOAK] > TICKS(1200) + (aibot->random1 >> 5) % TICKS(1200)) {
				aibot->cloakdeviceenabled = true;
			} else if (aibot->ammoheld[AMMOTYPE_CLOAK] <= (aibot->random1 >> 17) % TICKS(1200)) {
				aibot->cloakdeviceenabled = false;
			}
#else
			if (aibot->ammoheld[AMMOTYPE_CLOAK] > 1200 + (aibot->random1 >> 5) % 1200) {
				aibot->cloakdeviceenabled = true;
			} else if (aibot->ammoheld[AMMOTYPE_CLOAK] <= (aibot->random1 >> 17) % 1200) {
				aibot->cloakdeviceenabled = false;
			}
#endif
		}

		// Consider starting or stopping RC-P120 cloak
		if (!aibot->cloakdeviceenabled && aibot->weaponnum == WEAPON_RCP120) {
			s32 qty = botact_get_ammo_quantity_by_weapon(aibot, WEAPON_RCP120, FUNC_PRIMARY, true);

			if (bot_is_about_to_attack(chr, true)) {
				if (qty > 200 + (aibot->random1 >> 6) % 200) {
					aibot->rcp120cloakenabled = true;
				} else if (qty <= 30 + (aibot->random1 >> 16) % 70) {
					aibot->rcp120cloakenabled = false;
				}
			} else {
				if (qty > 300 + (aibot->random1 >> 12) % 500) {
					aibot->rcp120cloakenabled = true;
				} else {
					aibot->rcp120cloakenabled = false;
				}
			}
		} else {
			aibot->rcp120cloakenabled = false;
		}

		// KazeSims will attack on sight
		if (aibot->config->type == BOTTYPE_KAZE
				&& chr->target != -1
				&& aibot->targetinsight
				&& chr->myaction != MA_AIBOTATTACK) {
			aibot->forcemainloop = true;
		}

		// If there's no humans on the bot's team to give it commands, figure
		// out which commands to apply automatically based on the scenario.
		if (aibot->teamisonlyai) {
			if (aibot->commandtimer60 > 0) {
				aibot->commandtimer60 -= g_Vars.lvupdate60;
			}

			if (aibot->commandtimer60 <= 0) {
				s32 teamsize = bot_get_team_size(chr);

				if (g_MpSetup.scenario == MPSCENARIO_HOLDTHEBRIEFCASE) {
					s32 numgetting = bot_get_count_in_team_doing_command(chr, AIBOTCMD_GETCASE2, false);

					if (numgetting <= 0 || (numgetting < (teamsize + 1) / 2 || random() % 100 < 66)) {
						bot_apply_scenario_command(chr, AIBOTCMD_GETCASE2);
					} else {
						bot_apply_scenario_command(chr, AIBOTCMD_NORMAL);
					}
				} else if (g_MpSetup.scenario == MPSCENARIO_HACKERCENTRAL) {
					s32 numbots = bot_get_count_in_team_doing_command(chr, AIBOTCMD_DOWNLOAD, false);

					if (aibot->hasuplink || numbots <= 0 || (numbots < (teamsize + 1) / 2 || random() % 100 < 50)) {
						bot_apply_scenario_command(chr, AIBOTCMD_DOWNLOAD);
					} else {
						bot_apply_scenario_command(chr, AIBOTCMD_NORMAL);
					}
				} else if (g_MpSetup.scenario == MPSCENARIO_POPACAP) {
					s32 numchasing = bot_get_count_in_team_doing_command(chr, AIBOTCMD_POPCAP, false);

					if (numchasing <= 0 || numchasing < (teamsize + 1) / 2 || random() % 100 < 50) {
						bot_apply_scenario_command(chr, AIBOTCMD_POPCAP);
					} else {
						bot_apply_scenario_command(chr, AIBOTCMD_NORMAL);
					}
				} else if (g_MpSetup.scenario == MPSCENARIO_KINGOFTHEHILL) {
					s32 numinhill = bot_get_num_teammates_defending_hill(chr);

					// Don't count ourselves
					if (chr->prop->rooms[0] == g_ScenarioData.koh.hillrooms[0]) {
						numinhill--;
					}

					if (numinhill <= 0 || numinhill < teamsize / 2) {
						bot_apply_scenario_command(chr, AIBOTCMD_HOLDHILL);
					} else if (numinhill > bot_get_num_opponents_in_hill(chr)) {
						if (random() % 100 < 50) {
							bot_apply_scenario_command(chr, AIBOTCMD_DEFHILL);
						} else {
							bot_apply_scenario_command(chr, AIBOTCMD_NORMAL);
						}
					} else {
						bot_apply_scenario_command(chr, AIBOTCMD_HOLDHILL);
					}
				} else if (g_MpSetup.scenario == MPSCENARIO_CAPTURETHECASE) {
					if (teamsize == 1) {
						// One man team
						s32 numgetting = bot_get_count_in_team_doing_command(chr, AIBOTCMD_GETCASE, true);

						if (bot_should_return_ctc_token(chr)) {
							bot_apply_scenario_command(chr, AIBOTCMD_GETCASE);
						} else if (bot_is_chrs_ctc_token_held(chr)) {
							if (random() % 100 < 30) {
								bot_apply_scenario_command(chr, AIBOTCMD_GETCASE);
							} else {
								bot_apply_scenario_command(chr, AIBOTCMD_SAVECASE);
							}
						} else {
							if (random() % 100 < 70 || numgetting <= 0) {
								bot_apply_scenario_command(chr, AIBOTCMD_GETCASE);
							} else {
								bot_apply_scenario_command(chr, AIBOTCMD_SAVECASE);
							}
						}
					} else {
						// Not a one man team
						s32 numgetting = bot_get_count_in_team_doing_command(chr, AIBOTCMD_GETCASE, false);
						s32 numsaving = bot_get_count_in_team_doing_command(chr, AIBOTCMD_SAVECASE, false);

						if (bot_should_return_ctc_token(chr)) {
							bot_apply_scenario_command(chr, AIBOTCMD_GETCASE);
						} else if (bot_is_chrs_ctc_token_held(chr)) {
							if (numsaving <= 0 || random() % 100 < 70) {
								bot_apply_scenario_command(chr, AIBOTCMD_SAVECASE);
							} else {
								bot_apply_scenario_command(chr, AIBOTCMD_GETCASE);
							}
						} else if (numgetting <= 0 || numgetting < teamsize / 3) {
							bot_apply_scenario_command(chr, AIBOTCMD_GETCASE);
						} else if (numsaving <= 0 || numsaving < teamsize / 4) {
							bot_apply_scenario_command(chr, AIBOTCMD_SAVECASE);
						} else if (random() % 100 < 30) {
							bot_apply_scenario_command(chr, AIBOTCMD_GETCASE);
						} else if (random() % 100 < 30) {
							bot_apply_scenario_command(chr, AIBOTCMD_SAVECASE);
						} else {
							bot_apply_scenario_command(chr, AIBOTCMD_NORMAL);
						}
					}
				}

				// Consider changing command in 20 to 60 seconds
				aibot->commandtimer60 = TICKS(1200) + random() % TICKS(2400);
			}
		}

		// The main loop is entered whenever the bot needs something to do
		// or when a player gives it a command. It calculates a new myaction
		// value and associated arguments based on its assigned command.
		if (chr->myaction == MA_AIBOTMAINLOOP || aibot->forcemainloop) {
			aibot->forcemainloop = false;
			aibot->attackingplayernum = -1;

			// KazeSim will attack people on sight regardless of command
			if (aibot->config->type == BOTTYPE_KAZE && chr->target != -1 && aibot->targetinsight) {
				newaction = MA_AIBOTATTACK;
				aibot->attackingplayernum = mp_chr_to_chrindex(chr_get_target_prop(chr)->chr);
				aibot->abortattacktimer60 = -1;
			}

			// Check if the bot needs to fetch some weapons or ammo
			if (newaction < 0) {
				aibot->gotoprop = bot_find_default_pickup(chr);

				if (aibot->gotoprop) {
					newaction = MA_AIBOTGETITEM;
				}
			}

			// Bot is good to implement the assigned command
			if (newaction < 0) {
				if (aibot->command == AIBOTCMD_ATTACK) {
					// Attack the prop (player) given in attackpropnum
					// This is a human command only
					struct chrdata *targetchr = (g_Vars.props + aibot->attackpropnum)->chr;

					if (!chr_is_dead(targetchr)
							&& !bot_is_target_invisible(chr, targetchr)
							&& bot_passes_coward_check(chr, targetchr)) {
						newaction = MA_AIBOTATTACK;
						aibot->attackingplayernum = mp_chr_to_chrindex(targetchr);
						aibot->abortattacktimer60 = -1;
					}
				} else if (aibot->command == AIBOTCMD_FOLLOW) {
					// Follow the prop (player) given in followprotectpropnum
					// This is a human command only
					newaction = MA_AIBOTFOLLOW;
					aibot->canbreakfollow = true;
					aibot->followingplayernum = mp_chr_to_chrindex((g_Vars.props + aibot->followprotectpropnum)->chr);
				} else if (aibot->command == AIBOTCMD_PROTECT) {
					// Protect the prop (player) given in followprotectpropnum
					// This is a human command only
					newaction = MA_AIBOTFOLLOW;
					aibot->canbreakfollow = false;
					aibot->followingplayernum = mp_chr_to_chrindex((g_Vars.props + aibot->followprotectpropnum)->chr);
				} else if (aibot->command == AIBOTCMD_DEFEND) {
					// Defend the position given in defendholdpos
					// This is a human command only
					newaction = MA_AIBOTDEFEND;
					aibot->canbreakdefend = true;
				} else if (aibot->command == AIBOTCMD_HOLD) {
					// Hold the position given in defendholdpos
					// This is a human command only
					newaction = MA_AIBOTDEFEND;
					aibot->canbreakdefend = false;
				} else if (aibot->command == AIBOTCMD_GETCASE) {
					// Capture the case - fetch and return the opponent's token
					if (g_MpSetup.scenario == MPSCENARIO_CAPTURETHECASE && !aibot->hascase) {
						// Make an array of pointers to other teams' tokens
						// but ignore enemy tokens held by other teams
						s32 botteamindex = radar_get_team_index(chr->team);
						s32 i;
						struct prop *tokens[4];
						s32 numtokens = 0;

						for (i = 0; i != 4; i++) {
							if (i != botteamindex && g_ScenarioData.ctc.playercountsperteam[i]) {
								if (g_ScenarioData.ctc.tokens[i]->type == PROPTYPE_WEAPON
										|| g_ScenarioData.ctc.tokens[i]->type == PROPTYPE_OBJ) {
									// Token is not held
									tokens[numtokens++] = g_ScenarioData.ctc.tokens[i];
								} else if (g_ScenarioData.ctc.tokens[i]->type == PROPTYPE_CHR
										|| g_ScenarioData.ctc.tokens[i]->type == PROPTYPE_PLAYER) {
									// Token is held
									struct chrdata *tokenchr = g_ScenarioData.ctc.tokens[i]->chr;

									if (tokenchr->team == chr->team) {
										// Token is held by teammate
										tokens[numtokens++] = g_ScenarioData.ctc.tokens[i];
									}
								}
							}
						}

						// Prefer a token within 10 metres, otherwise pick any
						if (numtokens > 0) {
							s32 index;
							s32 i;

							index = random() % numtokens;

							i = (index + 1) % numtokens;

							while (true) {
								f32 sqdist = chr_get_squared_distance_to_coord(chr, &tokens[i]->pos);

								if (sqdist < 1000 * 1000) {
									index = i;
									break;
								}

								if (i == index) {
									break;
								}

								i = (i + 1) % numtokens;
							}

							if (index);

							// If the chosen token is not held then collect it,
							// otherwise it's held by a teammate so go protect them
							if (tokens[index]->type == PROPTYPE_WEAPON || tokens[index]->type == PROPTYPE_OBJ) {
								newaction = MA_AIBOTGETITEM;
								aibot->gotoprop = tokens[index];
							} else if (bot_can_follow(chr, tokens[index]->chr)) {
								newaction = MA_AIBOTFOLLOW;
								aibot->canbreakfollow = random() % 4 == 0;
								aibot->followingplayernum = mp_chr_to_chrindex(tokens[index]->chr);
							}
						}
					}
				} else if (aibot->command == AIBOTCMD_SAVECASE) {
					// Capture the case - recover/protect bot's own token
					if (g_MpSetup.scenario == MPSCENARIO_CAPTURETHECASE) {
						// Find out where the bot's token is
						struct prop *token = g_ScenarioData.ctc.tokens[radar_get_team_index(chr->team)];

						if (token->type == PROPTYPE_CHR || token->type == PROPTYPE_PLAYER) {
							struct chrdata *tokenchr = token->chr;

							if (tokenchr->team == chr->team) {
								// Held by a teammate - follow/protect them
								if (bot_can_follow(chr, tokenchr)) {
									newaction = MA_AIBOTFOLLOW;
									aibot->canbreakfollow = random() % 4 == 0;
									aibot->followingplayernum = mp_chr_to_chrindex(tokenchr);
								}
							} else {
								// Held by an opponent - attack them
								if (!chr_is_dead(tokenchr)
										&& !bot_is_target_invisible(chr, tokenchr)
										&& bot_passes_coward_check(chr, tokenchr)) {
									newaction = MA_AIBOTATTACK;
									aibot->attackingplayernum = mp_chr_to_chrindex(tokenchr);
									aibot->abortattacktimer60 = -1;
								}
							}
						} else {
							// Token is not held - go to the pos to defend it
							newaction = MA_AIBOTGOTOPOS;
							aibot->gotopos.x = token->pos.x;
							aibot->gotopos.y = token->pos.y;
							aibot->gotopos.z = token->pos.z;
							rooms_copy(token->rooms, aibot->gotorooms);
							aibot->inhill = false;
						}
					}
				} else if (aibot->command == AIBOTCMD_DEFHILL) {
					// King of the hill - defend the hill (allow wandering out)
					if (g_MpSetup.scenario == MPSCENARIO_KINGOFTHEHILL) {
						if (chr->prop->rooms[0] == g_ScenarioData.koh.hillrooms[0]
								&& chr->target != -1
								&& aibot->targetinsight
								&& bot_passes_coward_check(chr, chr_get_target_prop(chr)->chr)) {
							// Bot is in the hill and sees target - attack them
							newaction = MA_AIBOTATTACK;
							aibot->attackingplayernum = mp_chr_to_chrindex(chr_get_target_prop(chr)->chr);
							aibot->abortattacktimer60 = TICKS(300);
						} else {
							// Go to the hill if not there already
							u32 stack;
							struct coord posinhill;
							f32 angle;
							s32 padnuminhill;
							s32 covernuminhill;

							if (botroom_find_pos(g_ScenarioData.koh.hillrooms[0], &posinhill, &angle, &padnuminhill, &covernuminhill)) {
								newaction = MA_AIBOTGOTOPOS;
								aibot->gotopos.x = posinhill.x;
								aibot->gotopos.y = posinhill.y;
								aibot->gotopos.z = posinhill.z;
								rooms_copy(g_ScenarioData.koh.hillrooms, aibot->gotorooms);
								aibot->inhill = (chr->prop->rooms[0] == g_ScenarioData.koh.hillrooms[0]) != 0;
								aibot->hillpadnum = padnuminhill;
								aibot->hillcovernum = covernuminhill;
								aibot->lastknownhill = g_ScenarioData.koh.hillrooms[0];
							}
						}
					}
				} else if (aibot->command == AIBOTCMD_HOLDHILL) {
					// King of the hill - hold the hill (don't wander out)
					if (g_MpSetup.scenario == MPSCENARIO_KINGOFTHEHILL) {
						struct coord posinhill;
						f32 angle;
						s32 padnuminhill;
						s32 covernuminhill;

						// Go to the hill if not there already
						if (botroom_find_pos(g_ScenarioData.koh.hillrooms[0], &posinhill, &angle, &padnuminhill, &covernuminhill)) {
							newaction = MA_AIBOTGOTOPOS;
							aibot->gotopos.x = posinhill.x;
							aibot->gotopos.y = posinhill.y;
							aibot->gotopos.z = posinhill.z;
							rooms_copy(g_ScenarioData.koh.hillrooms, aibot->gotorooms);
							aibot->inhill = (chr->prop->rooms[0] == g_ScenarioData.koh.hillrooms[0]) != 0;
							aibot->hillpadnum = padnuminhill;
							aibot->hillcovernum = covernuminhill;
							aibot->lastknownhill = g_ScenarioData.koh.hillrooms[0];
						}
					}
				} else if (aibot->command == AIBOTCMD_DOWNLOAD) {
					// Hacker Central - fetch uplink
					if (g_MpSetup.scenario == MPSCENARIO_HACKERCENTRAL
							&& g_ScenarioData.htm.uplink
							&& g_ScenarioData.htm.uplink != chr->prop) {
						// Uplink is not held by current bot
						if (g_ScenarioData.htm.uplink->type == PROPTYPE_CHR
								|| g_ScenarioData.htm.uplink->type == PROPTYPE_PLAYER) {
							struct chrdata *uplinkchr = g_ScenarioData.htm.uplink->chr;

							if ((g_MpSetup.options & MPOPTION_TEAMSENABLED) && uplinkchr->team == chr->team) {
								// Uplink is held by teammate - protect them
								if (bot_can_follow(chr, uplinkchr)) {
									newaction = MA_AIBOTFOLLOW;
									aibot->canbreakfollow = random() % 4 == 0;
									aibot->followingplayernum = mp_chr_to_chrindex(uplinkchr);
								}
							} else {
								// Uplink is held by opponent - attack them
								if (!bot_is_target_invisible(chr, uplinkchr) && bot_passes_coward_check(chr, uplinkchr)) {
									newaction = MA_AIBOTATTACK;
									aibot->attackingplayernum = mp_chr_to_chrindex(uplinkchr);
									aibot->abortattacktimer60 = -1;
								}
							}
						} else {
							// Uplink is not held by anyone - fetch it
							newaction = MA_AIBOTGOTOPROP;
							aibot->gotoprop = g_ScenarioData.htm.uplink;
						}
					}
				} else if (aibot->command == AIBOTCMD_GETCASE2) {
					// Hold the briefcase - fetch and hold the case
					if (g_MpSetup.scenario == MPSCENARIO_HOLDTHEBRIEFCASE
							&& g_ScenarioData.htb.token
							&& g_ScenarioData.htb.token != chr->prop) {
						// Briefcase is not held by current bot
						if (g_ScenarioData.htb.token->type == PROPTYPE_CHR
								|| g_ScenarioData.htb.token->type == PROPTYPE_PLAYER) {
							struct chrdata *tokenchr = g_ScenarioData.htb.token->chr;

							if ((g_MpSetup.options & MPOPTION_TEAMSENABLED) && tokenchr->team == chr->team) {
								// Briefcase is held by teammate - protect them
								if (bot_can_follow(chr, tokenchr)) {
									newaction = MA_AIBOTFOLLOW;
									aibot->canbreakfollow = random() % 4 == 0;
									aibot->followingplayernum = mp_chr_to_chrindex(tokenchr);
								}
							} else if (!bot_is_target_invisible(chr, tokenchr) && bot_passes_coward_check(chr, tokenchr)) {
								// Briefcase is held by opponent - attack them
								newaction = MA_AIBOTATTACK;
								aibot->attackingplayernum = mp_chr_to_chrindex(tokenchr);
								aibot->abortattacktimer60 = -1;
							}
						} else {
							// Briefcase is not held by anyone - fetch it
							newaction = MA_AIBOTGOTOPROP;
							aibot->gotoprop = g_ScenarioData.htb.token;
						}
					}
				} else if (aibot->command == AIBOTCMD_POPCAP) {
					// Pop a cap - attack the target
					if (g_MpSetup.scenario == MPSCENARIO_POPACAP && g_ScenarioData.pac.victimindex >= 0) {
						struct prop *victimprop = g_MpAllChrPtrs[g_ScenarioData.pac.victims[g_ScenarioData.pac.victimindex]]->prop;

						if (victimprop != chr->prop) {
							// Victim is not the current bot
							struct chrdata *victimchr = victimprop->chr;

							if ((g_MpSetup.options & MPOPTION_TEAMSENABLED) && victimchr->team == chr->team) {
								// Victim is a teammate - protect them
								if (bot_can_follow(chr, victimchr)) {
									newaction = MA_AIBOTFOLLOW;
									aibot->canbreakfollow = random() % 4 == 0;
									aibot->followingplayernum = mp_chr_to_chrindex(victimchr);
								}
							} else {
								// Victim is an opponent - attack them
								if (!bot_is_target_invisible(chr, victimchr) && bot_passes_coward_check(chr, victimchr)) {
									newaction = MA_AIBOTATTACK;
									aibot->attackingplayernum = mp_chr_to_chrindex(victimchr);
									aibot->abortattacktimer60 = -1;
								}
							}
						}
					}
				}
			}

			// The bot can still not have an action if it's general combat, or
			// in some situations like having the uplink in Hacker Central or
			// holding the briefcase in Hold the Briefcase.
			if (newaction < 0) {
				if (g_MpSetup.scenario == MPSCENARIO_HOLDTHEBRIEFCASE) {
					if (aibot->hasbriefcase) {
						// Current bot has the briefcase - follow a teammate for protection
						s32 playernum = -1;

						if (random() % 100 < 66) {
							playernum = bot_find_teammate_to_follow(chr, 100000);
						}

						if (playernum >= 0) {
							newaction = MA_AIBOTFOLLOW;
							aibot->canbreakfollow = random() % 4 == 0;
							aibot->followingplayernum = playernum;
						}
					}
				} else if (g_MpSetup.scenario == MPSCENARIO_POPACAP) {
					if (g_ScenarioData.pac.victimindex >= 0) {
						struct prop *victimprop = g_MpAllChrPtrs[g_ScenarioData.pac.victims[g_ScenarioData.pac.victimindex]]->prop;

						if (victimprop == chr->prop) {
							// Current bot is the victim - follow a teammate for protection
							s32 playernum = -1;

							if (random() % 100 < 66) {
								playernum = bot_find_teammate_to_follow(chr, 100000);
							}

							if (playernum >= 0) {
								newaction = MA_AIBOTFOLLOW;
								aibot->canbreakfollow = random() % 4 == 0;
								aibot->followingplayernum = playernum;
							}
						}
					}
				} else if (g_MpSetup.scenario == MPSCENARIO_CAPTURETHECASE) {
					// If the bot is holding an opponent's token, take it home
					if (bot_should_return_ctc_token(chr)) {
						struct pad pad;
						s32 teamindex = g_ScenarioData.ctc.teamindexes[radar_get_team_index(chr->team)];
						newaction = MA_AIBOTGOTOPOS;
						pad_unpack(g_ScenarioData.ctc.spawnpadsperteam[teamindex].homepad, PADFIELD_POS | PADFIELD_ROOM, &pad);
						aibot->gotopos.x = pad.pos.x;
						aibot->gotopos.y = pad.pos.y;
						aibot->gotopos.z = pad.pos.z;
						aibot->gotorooms[0] = pad.room;
						aibot->gotorooms[1] = -1;
						aibot->inhill = false;
					}
				} else if (g_MpSetup.scenario == MPSCENARIO_HACKERCENTRAL) {
					// If the bot has the uplink, go to the terminal
					if (g_ScenarioData.htm.uplink == chr->prop) {
						if (g_ScenarioData.htm.playernuminrange != mp_chr_to_chrindex(chr)) {
							newaction = MA_AIBOTGOTOPROP;
							aibot->gotoprop = g_ScenarioData.htm.terminals[0].prop;
						} else {
							newaction = MA_AIBOTDOWNLOAD;
						}
					}
				}
			}

			// If there's nothing to do for scenarios then go find some people to kill
			if (newaction < 0) {
				if (aibot->config->type == BOTTYPE_VENGE) {
					// Attack the last player who killed the bot
					if (aibot->lastkilledbyplayernum >= 0
							&& !bot_is_target_invisible(chr, g_MpAllChrPtrs[aibot->lastkilledbyplayernum])) {
						newaction = MA_AIBOTATTACK;
						aibot->attackingplayernum = aibot->lastkilledbyplayernum;
						aibot->abortattacktimer60 = -1;
					}
				} else if (aibot->config->type == BOTTYPE_FEUD) {
					// Attack a single player the whole match
					if (aibot->feudplayernum < 0
							&& aibot->lastkilledbyplayernum >= 0
							&& !chr_compare_teams(chr, g_MpAllChrPtrs[aibot->lastkilledbyplayernum], COMPARE_FRIENDS)) {
						aibot->feudplayernum = aibot->lastkilledbyplayernum;
					}

					if (aibot->feudplayernum >= 0 && !bot_is_target_invisible(chr, g_MpAllChrPtrs[aibot->feudplayernum])) {
						newaction = MA_AIBOTATTACK;
						aibot->abortattacktimer60 = -1;
						aibot->attackingplayernum = aibot->feudplayernum;
					}
				} else if (aibot->config->type == BOTTYPE_JUDGE) {
					// Attack the winning player
					struct ranking rankings[MAX_MPCHRS];
					s32 count = mp_get_player_rankings(rankings);
					s32 i;

					for (i = 0; i < count; i++) {
						s32 playernum = mp_chrslot_to_chrindex(rankings[i].chrnum);
						struct chrdata *otherchr = mp_chrindex_to_chr(playernum);

						if (otherchr != chr && !chr_is_dead(otherchr)) {
#if PAL
							if (1);
#endif
							if (chr_compare_teams(chr, otherchr, COMPARE_ENEMIES)
									&& !bot_is_target_invisible(chr, otherchr)) {
								newaction = MA_AIBOTATTACK;
								aibot->attackingplayernum = playernum;
								aibot->abortattacktimer60 = -1;
							}
						}
					}
				} else if (aibot->config->type == BOTTYPE_PREY) {
					// Attack the weakest player
					f32 minhealth = 0;
					s32 weakestplayernum = -1;
					f32 health;
					s32 i;

					for (i = 0; i < g_MpNumChrs; i++) {
						struct chrdata *otherchr = mp_chrindex_to_chr(i);

						if (otherchr != chr
								&& !chr_is_dead(otherchr)
								&& chr_compare_teams(chr, otherchr, COMPARE_ENEMIES)
								&& !bot_is_target_invisible(chr, otherchr)) {
							if (otherchr->aibot) {
								health = otherchr->maxdamage - otherchr->damage;
							} else {
								health = g_Vars.players[playermgr_get_player_num_by_prop(otherchr->prop)]->bondhealth * 8;
							}

							if (weakestplayernum < 0 || health < minhealth) {
								weakestplayernum = i;
								minhealth = health;
							}
						}
					}

					if (weakestplayernum >= 0) {
						newaction = MA_AIBOTATTACK;
						aibot->attackingplayernum = weakestplayernum;
						aibot->abortattacktimer60 = -1;
					}
				}
			}

			// If the bot didn't set an action above,
			// try attacking the existing target
			if (newaction < 0) {
				if (chr->target != -1 && bot_passes_coward_check(chr, chr_get_target_prop(chr)->chr)) {
					newaction = MA_AIBOTATTACK;
					aibot->abortattacktimer60 = -1;
				}
			}

			// If there's no existing target, just follow a teammate
			if (newaction < 0) {
				s32 playernum = bot_find_teammate_to_follow(chr, 300);

				if (playernum >= 0) {
					newaction = MA_AIBOTFOLLOW;
					aibot->canbreakfollow = random() % 4 == 0;
					aibot->followingplayernum = playernum;
				}
			}

			// If there's no teammate to follow, stock up on weapons and ammo
			if (newaction < 0) {
				aibot->gotoprop = bot_find_any_pickup(chr);

				if (aibot->gotoprop) {
					newaction = MA_AIBOTGETITEM;
				}
			}
		}

		// Implement the new action
		if (newaction >= 0) {
			if (newaction == MA_AIBOTGETITEM) {
				if (aibot->gotoprop) {
					chr_go_to_prop(chr, aibot->gotoprop, GOPOSFLAG_RUN);
					chr->myaction = newaction;
				}
			} else if (newaction == MA_AIBOTATTACK) {
				if (chr->myaction != MA_AIBOTATTACK) {
					chr->myaction = newaction;
					aibot->distmode = -1;
				}
			} else if (newaction == MA_AIBOTFOLLOW) {
				if (chr->myaction != MA_AIBOTFOLLOW) {
					chr->myaction = newaction;
					aibot->distmode = -1;

					if (aibot->canbreakfollow) {
						bot_set_target(chr, -1);
					}
				}
			} else if (newaction == MA_AIBOTDEFEND) {
				chr->myaction = newaction;

				if (aibot->canbreakdefend) {
					bot_set_target(chr, -1);
				}

				chr_go_to_room_pos(chr, &aibot->defendholdpos, aibot->defendholdrooms, GOPOSFLAG_RUN);
			} else if (newaction == MA_AIBOTGOTOPOS) {
				f32 xdist = chr->prop->pos.x - aibot->gotopos.x;
				f32 ydist = chr->prop->pos.y - aibot->gotopos.y;
				f32 zdist = chr->prop->pos.z - aibot->gotopos.z;

				if (xdist < 0) {
					xdist = -xdist;
				}

				if (ydist < 0) {
					ydist = -ydist;
				}

				if (zdist < 0) {
					zdist = -zdist;
				}

				if (xdist > 20 || zdist > 20 || (ydist > 200 && chr->inlift == 0)) {
					chr->myaction = newaction;
					chr_go_to_room_pos(chr, &aibot->gotopos, aibot->gotorooms, GOPOSFLAG_RUN);
				} else {
					chr_stand(chr);
				}
			} else if (newaction == MA_AIBOTGOTOPROP) {
				if (aibot->gotoprop) {
					chr->myaction = newaction;
					chr_go_to_prop(chr, aibot->gotoprop, GOPOSFLAG_RUN);
				}
			} else if (newaction == MA_AIBOTDOWNLOAD) {
				chr->myaction = newaction;
				chr_stand(chr);
			}
		}

		// If the action is no longer valid, go back to the main loop
		// so another action can be chosen on the next tick
		if (chr->myaction == MA_AIBOTGETITEM) {
			if (chr->actiontype != ACT_GOPOS
					|| aibot->gotoprop == NULL
					|| aibot->gotoprop->parent
					|| aibot->gotoprop->timetoregen != 0) {
				chr->myaction = MA_AIBOTMAINLOOP;
			}
		} else if (chr->myaction == MA_AIBOTATTACK) {
			if (aibot->attackingplayernum >= 0
					&& (chr_is_dead(g_MpAllChrPtrs[aibot->attackingplayernum]) || !bot_passes_coward_check(chr, g_MpAllChrPtrs[aibot->attackingplayernum]))) {
				chr->myaction = MA_AIBOTMAINLOOP;
			} else if (aibot->attackingplayernum < 0
					&& (chr->target == -1
						|| chr_is_dead(chr_get_target_prop(chr)->chr)
						|| !bot_passes_coward_check(chr, chr_get_target_prop(chr)->chr))) {
				chr->myaction = MA_AIBOTMAINLOOP;
			} else {
				botcmd_tick_dist_mode(chr);

				if (bot_can_do_critical_pickup(chr)) {
					chr->myaction = MA_AIBOTMAINLOOP;
				} else if (aibot->abortattacktimer60 >= 0 && aibot->targetlastseen60 < g_Vars.lvframe60 - aibot->abortattacktimer60) {
					chr->myaction = MA_AIBOTMAINLOOP;
				}
			}
		} else if (chr->myaction == MA_AIBOTFOLLOW) {
			if (aibot->followingplayernum < 0
					|| chr_is_dead(g_MpAllChrPtrs[aibot->followingplayernum])) {
				chr->myaction = MA_AIBOTMAINLOOP;
			} else {
				botcmd_tick_dist_mode(chr);

				if (aibot->canbreakfollow
						&& chr->target != -1
						&& aibot->targetinsight
						&& bot_passes_coward_check(chr, chr_get_target_prop(chr)->chr)) {
					f32 xdist = chr->prop->pos.x - g_MpAllChrPtrs[aibot->followingplayernum]->prop->pos.x;
					f32 zdist = chr->prop->pos.z - g_MpAllChrPtrs[aibot->followingplayernum]->prop->pos.z;

					if (xdist < 0) {
						xdist = -xdist;
					}

					if (zdist < 0) {
						zdist = -zdist;
					}

					// No y check?
					if (xdist < 500 && zdist < 500) {
						chr->myaction = MA_AIBOTATTACK;
						aibot->attackingplayernum = mp_chr_to_chrindex(chr_get_target_prop(chr)->chr);
						aibot->abortattacktimer60 = TICKS(300);
						aibot->distmode = -1;
					}
				}

				if (bot_can_do_critical_pickup(chr)) {
					chr->myaction = MA_AIBOTMAINLOOP;
				}
			}
		} else if (chr->myaction == MA_AIBOTDEFEND) {
			if (chr->actiontype != ACT_GOPOS) {
				f32 xdist = chr->prop->pos.x - aibot->defendholdpos.x;
				f32 ydist = chr->prop->pos.y - aibot->defendholdpos.y;
				f32 zdist = chr->prop->pos.z - aibot->defendholdpos.z;

				if (xdist < 0) {
					xdist = -xdist;
				}

				if (ydist < 0) {
					ydist = -ydist;
				}

				if (zdist < 0) {
					zdist = -zdist;
				}

				if (aibot->returntodefendtimer60 > 0) {
					aibot->returntodefendtimer60 -= g_Vars.lvupdate60;
				}

				if (xdist > 40 || zdist > 40 || (ydist > 200 && !chr->inlift)) {
					if (aibot->returntodefendtimer60 <= 0) {
						chr_go_to_room_pos(chr, &aibot->defendholdpos, aibot->defendholdrooms, GOPOSFLAG_RUN);
					}
				} else if (aibot->canbreakdefend
						&& chr->target != -1
						&& aibot->targetinsight
						&& bot_passes_coward_check(chr, chr_get_target_prop(chr)->chr)) {
					chr->myaction = MA_AIBOTATTACK;
					aibot->attackingplayernum = mp_chr_to_chrindex(chr_get_target_prop(chr)->chr);
					aibot->abortattacktimer60 = TICKS(300);
					aibot->distmode = -1;
				}

				if (aibot->returntodefendtimer60 <= 0) {
					aibot->returntodefendtimer60 = TICKS(60);
				}
			}

			if (bot_can_do_critical_pickup(chr)) {
				chr->myaction = MA_AIBOTMAINLOOP;
			}
		} else if (chr->myaction == MA_AIBOTGOTOPOS) {
			if (g_MpSetup.scenario == MPSCENARIO_KINGOFTHEHILL && aibot->inhill) {
				if (aibot->lastknownhill != g_ScenarioData.koh.hillrooms[0]) {
					// Someone scored the hill
					aibot->inhill = false;
				} else if (chr->prop->rooms[0] == g_ScenarioData.koh.hillrooms[0]) {
					// empty
				} else if (aibot->hillpadnum >= 0) {
					pad_set_flag(aibot->hillpadnum, PADFLAG_AIBOTINUSE);
				} else if (aibot->hillcovernum >= 0) {
					cover_set_flag(aibot->hillcovernum, COVERFLAG_AIBOTINUSE);
				}
			}

			if (chr->actiontype != ACT_GOPOS) {
				chr->myaction = MA_AIBOTMAINLOOP;
			} else if (bot_can_do_critical_pickup(chr)) {
				chr->myaction = MA_AIBOTMAINLOOP;
			}
		} else if (chr->myaction == MA_AIBOTGOTOPROP) {
			if (bot_can_do_critical_pickup(chr)) {
				chr->myaction = MA_AIBOTMAINLOOP;
			} else if (chr->actiontype != ACT_GOPOS || aibot->gotoprop == NULL || aibot->gotoprop->parent) {
				chr->myaction = MA_AIBOTMAINLOOP;
			} else if (g_MpSetup.scenario == MPSCENARIO_HOLDTHEBRIEFCASE) {
				// empty
			} else if (g_MpSetup.scenario == MPSCENARIO_HACKERCENTRAL
					&& g_ScenarioData.htm.uplink == chr->prop
					&& g_ScenarioData.htm.playernuminrange == mp_chr_to_chrindex(chr)) {
				chr->myaction = MA_AIBOTMAINLOOP;
			}
		} else if (chr->myaction == MA_AIBOTDOWNLOAD) {
			if (bot_can_do_critical_pickup(chr)) {
				chr->myaction = MA_AIBOTMAINLOOP;
			} else if (g_ScenarioData.htm.playernuminrange != mp_chr_to_chrindex(chr)) {
				chr->myaction = MA_AIBOTMAINLOOP;
			}
		}

		// Regardless of the action, choose a general target and maintain a
		// route to them, even if it won't be followed
		bot_choose_general_target(chr);

		if (mp_chr_to_chrindex(chr) == (g_Vars.lvframenum % g_MpNumChrs) && chr->target != -1) {
			struct prop *targetprop = chr_get_target_prop(chr);
			struct waypoint *first = waypoint_find_closest_to_pos(&chr->prop->pos, chr->prop->rooms);
			struct waypoint *last = waypoint_find_closest_to_pos(&targetprop->pos, targetprop->rooms);

			if (first && last) {
				nav_set_seed(CHRNAVSEED(chr), CHRNAVSEED(chr));
				aibot->numwaystepstotarget = nav_find_route(last, first, aibot->waypoints, ARRAYCOUNT(aibot->waypoints));
				nav_set_seed(0, 0);
			}
		}

		// Tick the bot's inventory. They may decide to switch weapons.
		botinv_tick(chr);

		// Iterate both hands and handle shooting
		{
			u32 stack;
			bool firingright = false;
			s32 i;

			for (i = 0; i < 2; i++) {
				bool firing = false;

				// nextbullettimer60 is positive if the chr is firing
				// It's used to implement the weapon's fire rate correctly
				if (aibot->nextbullettimer60[i] > 0) {
					aibot->nextbullettimer60[i] -= g_Vars.lvupdate60;
				}

				// Don't shoot the left hand on the same frame as the right
				// (note that right is iterated first)
				if (i == HAND_LEFT && firingright) {
					aibot->nextbullettimer60[i] = 1;
				}

				if (aibot->skrocket == NULL && aibot->changeguntimer60 <= 0) {
					if (aibot->ismeleeweapon) {
						// Consider punching, pistol whipping etc
						// Despite the name, punchtimer60 is used for all close
						// range attacks. Its value is 0 if not currently
						// punching, positive (and ticking down) when cooling
						// down from a previous punch, and negative when
						// starting the punch.
						if (aibot->punchtimer60[i] >= 0 && aibot->timeuntilreload60[i] <= 0) {
							if (aibot->weaponnum == WEAPON_TRANQUILIZER
									&& aibot->loadedammo[i] < bgun_get_min_clip_qty(WEAPON_TRANQUILIZER, FUNC_SECONDARY)) {
								aibot->punchtimer60[i] = 0;
								bot_schedule_reload(chr, i);
							} else {
								f32 range = 210;

								// Decide whether to actually punch or not.
								// This seems a bit backwards in that the timer
								// is set to negative (ie. punch) then the check
								// below has to cancel it by setting it back to 0.
								aibot->punchtimer60[i] -= g_Vars.lvupdate60;

								if (chr->target != -1
										&& aibot->targetinsight
										&& aibot->shootdelaytimer60 >= g_BotDifficulties[aibot->config->difficulty].shootdelay60) {
									if (!bot_is_dizzy(chr)) {
										if (aibot->weaponnum == WEAPON_TRANQUILIZER) {
											if (!chr_is_target_in_fov(chr, 30, 0) || chr_get_distance_to_target(chr) > range) {
												aibot->punchtimer60[i] = 0;
											}
										} else {
											if (!chr_is_target_in_fov(chr, 40, 0) || chr_get_distance_to_target(chr) > range + 150) {
												aibot->punchtimer60[i] = 0;
											}
										}
									}
								} else {
									aibot->punchtimer60[i] = 0;
								}

								// If the punch was not cancelled, execute it
								if (aibot->punchtimer60[i] < 0) {
									chr_uncloak_temporarily(chr);
									chr_punch_inflict_damage(chr, 2, range, false);

									if (i == HAND_RIGHT) {
										// Set the punch cooldown timer
										switch (aibot->weaponnum) {
										case WEAPON_UNARMED:
										case WEAPON_MAGSEC4:
										case WEAPON_MAULER:
										case WEAPON_PHOENIX:
										case WEAPON_CMP150:
										case WEAPON_CYCLONE:
										case WEAPON_CALLISTO:
										case WEAPON_RCP120:
										case WEAPON_LAPTOPGUN:
										case WEAPON_DRAGON:
										case WEAPON_K7AVENGER:
										case WEAPON_AR34:
										case WEAPON_SUPERDRAGON:
										case WEAPON_SHOTGUN:
										case WEAPON_SNIPERRIFLE:
										case WEAPON_FARSIGHT:
										case WEAPON_DEVASTATOR:
										case WEAPON_ROCKETLAUNCHER:
										case WEAPON_SLAYER:
										case WEAPON_CROSSBOW:
										default:
											if (chr->aibot->config->difficulty == BOTDIFF_MEAT) {
												aibot->punchtimer60[0] = TICKS(120);
											} else if (chr->aibot->config->difficulty == BOTDIFF_EASY) {
												aibot->punchtimer60[0] = TICKS(60);
											} else {
												aibot->punchtimer60[0] = TICKS(30);
											}

											if (random() % 3 == 0) {
												aibot->punchtimer60[1] = aibot->punchtimer60[0] - TICKS(20);
											}
											break;
										case WEAPON_FALCON2:
										case WEAPON_FALCON2_SILENCER:
										case WEAPON_FALCON2_SCOPE:
										case WEAPON_DY357MAGNUM:
										case WEAPON_DY357LX:
										case WEAPON_COMBATKNIFE:
											if (chr->aibot->config->difficulty == BOTDIFF_MEAT) {
												aibot->punchtimer60[0] = TICKS(120);
											} else if (chr->aibot->config->difficulty == BOTDIFF_EASY) {
												aibot->punchtimer60[0] = TICKS(90);
											} else {
												aibot->punchtimer60[0] = TICKS(60);
											}

											if (chr->weapons_held[HAND_LEFT]) {
												aibot->punchtimer60[1] = aibot->punchtimer60[0] - TICKS(40);
											}
											break;
										case WEAPON_TRANQUILIZER:
											aibot->punchtimer60[0] = TICKS(60);
											aibot->loadedammo[0] -= bgun_get_min_clip_qty(WEAPON_TRANQUILIZER, FUNC_SECONDARY);
											break;
										case WEAPON_REAPER:
											aibot->punchtimer60[0] = 0;
											break;
										}
									}
								}
							}
						}
					} else if (aibot->weaponnum == WEAPON_SLAYER && aibot->gunfunc != FUNC_PRIMARY && chr->target != -1) {
						// Bots fire Slayer rockets regardless of where they are
						// on the map provided they have ammo
						if (aibot->loadedammo[0] > 0) {
							chr_uncloak_temporarily(chr);
							botact_create_slayer_rocket(chr);
							aibot->loadedammo[0]--;
						}
					} else if (botact_is_weapon_throwable(aibot->weaponnum, aibot->gunfunc)) {
						// Hand throwing a weapon
						if (i == HAND_RIGHT) {
							if (aibot->throwtimer60 > 0) {
								aibot->throwtimer60 -= g_Vars.lvupdate60;
							}

							if (chr->aibot->throwtimer60 <= 0) {
								if (botact_get_ammo_quantity_by_weapon(aibot, aibot->weaponnum, aibot->gunfunc, false) > 0
										|| aibot->weaponnum == WEAPON_LAPTOPGUN
										|| aibot->weaponnum == WEAPON_DRAGON) {
									bool throw = false;

									if (chr->target != -1
											&& aibot->targetinsight
											&& aibot->shootdelaytimer60 >= g_BotDifficulties[aibot->config->difficulty].shootdelay60
											&& (bot_is_dizzy(chr) || chr_is_target_in_fov(chr, 45, false))) {
										throw = true;
									}

									if (throw) {
										struct funcdef *func;

										chr_uncloak_temporarily(chr);
										botact_try_remove_ammo_from_reserve(aibot, aibot->weaponnum, aibot->gunfunc, 1);
										botact_throw(chr);

										func = gset_get_funcdef_by_weaponnum_funcnum(aibot->weaponnum, aibot->gunfunc);

										if (func && (func->flags & FUNCFLAG_DISCARDWEAPON)) {
											botinv_remove_item(chr, aibot->weaponnum);
											botinv_switch_to_weapon(chr, WEAPON_UNARMED, FUNC_PRIMARY);
										}

										aibot->throwtimer60 = botact_get_projectile_throw_interval(chr->aibot->weaponnum);
									}
								}
							}
						}
					} else if (chr->weapons_held[i] && aibot->loadedammo[i] > 0) {
						// Handle firing a regular weapon
						bool canshoot = false;

						if (weapon_get_num_ticks_per_shot(aibot->weaponnum, aibot->gunfunc) <= 0) {
							// Increment the mauler charge and deplete ammo as
							// the charge amount crosses each whole number.
							// Yes, this is actually implemented for bots.
							if (aibot->weaponnum == WEAPON_MAULER
									&& aibot->gunfunc == FUNC_SECONDARY
									&& aibot->loadedammo[i] >= 2) {
								s32 newchargei;
								s32 oldchargei = aibot->maulercharge[i];

								aibot->maulercharge[i] += g_Vars.lvupdate60freal * 0.05f;

								if (aibot->maulercharge[i] > 5) {
									aibot->maulercharge[i] = 5;
								}

								newchargei = aibot->maulercharge[i];

								if (newchargei != oldchargei) {
									aibot->loadedammo[i]--;
								}
							}

							if (aibot->nextbullettimer60[i] <= 0) {
								canshoot = true;
							}
						} else {
							canshoot = true;
						}

						if (canshoot) {
							if (aibot->cyclonedischarging[i] || aibot->burstsdone[i] > 0) {
								firing = true;
							} else if (chr->target != -1
									&& aibot->targetinsight
									&& aibot->shootdelaytimer60 >= g_BotDifficulties[aibot->config->difficulty].shootdelay60
									&& (bot_is_dizzy(chr) || chr_is_target_in_fov(chr, 45, false))
									&& !chr_is_dead(chr_get_target_prop(chr)->chr)) {
								firing = true;

								if (aibot->weaponnum == WEAPON_CYCLONE && aibot->gunfunc == FUNC_SECONDARY) {
									aibot->cyclonedischarging[i] = true;
								} else if (aibot->weaponnum == WEAPON_REAPER) {
									aibot->reaperspeed[i] += g_Vars.lvupdate60;

									if (aibot->reaperspeed[i] > TICKS(90)) {
										aibot->reaperspeed[i] = TICKS(90);
									}
								}
							}

							// The Reaper continues shooting momentarily when
							// the trigger is released
							if (!firing && aibot->reaperspeed[i] > 0) {
								firing = true;

								aibot->reaperspeed[i] -= g_Vars.lvupdate60;

								if (aibot->reaperspeed[i] < 0) {
									aibot->reaperspeed[i] = 0;
								}
							}
						}

						if (weapon_get_num_ticks_per_shot(aibot->weaponnum, aibot->gunfunc) <= 0 && firing) {
							struct funcdef *func;
							aibot->nextbullettimer60[i] = botact_get_shoot_interval60(aibot->weaponnum, aibot->gunfunc);

#if PAL
							if (aibot->nextbullettimer60[i] >= 6) {
								aibot->nextbullettimer60[i] = TICKS(aibot->nextbullettimer60[i]);
							}
#endif

							func = gset_get_funcdef_by_weaponnum_funcnum(aibot->weaponnum, aibot->gunfunc);

							if (func
									&& (func->flags & (FUNCFLAG_BURST3 | FUNCFLAG_BURST2))
									&& aibot->loadedammo[i] >= 2) {
								s32 burstqty = (func->flags & FUNCFLAG_BURST2) ? 2 : 3;

								chr->aibot->burstsdone[i]++;
								chr->aibot->burstsdone[i] %= burstqty;

								if (chr->aibot->burstsdone[i]) {
									chr->aibot->nextbullettimer60[i] = 5;
								}
							}
						}
					} else {
						aibot->cyclonedischarging[i] = false;
						aibot->burstsdone[i] = 0;
						aibot->reaperspeed[i] = 0;
					}
				}

				if (firing) {
					chr_uncloak_temporarily(chr);

					if (i == HAND_RIGHT) {
						firingright = true;
					}
				}

				chr_set_hand_firing(chr, i, firing);
			}
		}
	}
}

void bot_check_fetch(struct chrdata *chr)
{
	bool alreadyfetching = false;
	struct aibot *aibot = chr->aibot;

	if (chr->myaction == MA_AIBOTGETITEM) {
		if (chr->act_gopos.waypoints[chr->act_gopos.curindex] == 0) {
			struct prop *prop = aibot->gotoprop;

#if VERSION >= VERSION_PAL_FINAL
			// pal-final adds a check for prop->obj
			if (prop && prop->obj && !prop->parent && prop->timetoregen == 0) {
				if (prop->type == PROPTYPE_WEAPON || prop->type == PROPTYPE_OBJ) {
					prop->obj->flags3 |= OBJFLAG3_ISFETCHTARGET;
				}
			}
#else
			if (prop && !prop->parent && prop->timetoregen == 0) {
				if (prop->type == PROPTYPE_WEAPON || prop->type == PROPTYPE_OBJ) {
					prop->obj->flags3 |= OBJFLAG3_ISFETCHTARGET;
				}
			}
#endif
		}

		aibot->forcemainloop = true;
		alreadyfetching = true;
	}

	if (!alreadyfetching) {
		chr_go_to_room_pos(chr, &chr->act_gopos.endpos, chr->act_gopos.endrooms, chr->act_gopos.flags);
	}
}
