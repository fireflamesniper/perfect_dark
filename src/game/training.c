#include <ultra64.h>
#include "constants.h"
#include "game/atan2f.h"
#include "game/bg.h"
#include "game/bondgun.h"
#include "game/chraction.h"
#include "game/debug.h"
#include "game/dlights.h"
#include "game/explosions.h"
#include "game/filemgr.h"
#include "game/gset.h"
#include "game/text.h"
#include "game/gamefile.h"
#include "game/hudmsg.h"
#include "game/inv.h"
#include "game/lang.h"
#include "game/menu.h"
#include "game/objectives.h"
#include "game/pad.h"
#include "game/padhalllv.h"
#include "game/player.h"
#include "game/prop.h"
#include "game/propobj.h"
#include "game/propsnd.h"
#include "game/shards.h"
#include "game/training.h"
#include "game/trainingmenus.h"
#include "game/wallhit.h"
#include "bss.h"
#include "lib/vi.h"
#include "lib/dma.h"
#include "lib/main.h"
#include "lib/snd.h"
#include "lib/memp.h"
#include "lib/rng.h"
#include "lib/mtx.h"
#include "data.h"
#include "types.h"

#define FRSCRIPTINDEX_WEAPONS 0x00
#define FRSCRIPTINDEX_TARGETS 0x22
#define FRSCRIPTINDEX_HELP    0x71

#define FRWEAPON_FALCON2          0
#define FRWEAPON_FALCON2_SCOPE    1
#define FRWEAPON_FALCON2_SILENCER 2
#define FRWEAPON_MAGSEC4          3
#define FRWEAPON_MAULER           4
#define FRWEAPON_PHOENIX          5
#define FRWEAPON_DY357MAGNUM      6
#define FRWEAPON_DY357LX          7
#define FRWEAPON_CMP150           8
#define FRWEAPON_CYCLONE          9
#define FRWEAPON_CALLISTO         10
#define FRWEAPON_RCP120           11
#define FRWEAPON_LAPTOPGUN        12
#define FRWEAPON_DRAGON           13
#define FRWEAPON_K7AVENGER        14
#define FRWEAPON_AR34             15
#define FRWEAPON_SUPERDRAGON      16
#define FRWEAPON_SHOTGUN          17
#define FRWEAPON_SNIPERRIFLE      18
#define FRWEAPON_FARSIGHT         19
#define FRWEAPON_CROSSBOW         20
#define FRWEAPON_TRANQUILIZER     21
#define FRWEAPON_REAPER           22
#define FRWEAPON_DEVASTATOR       23
#define FRWEAPON_ROCKETLAUNCHER   24
#define FRWEAPON_SLAYER           25
#define FRWEAPON_COMBATKNIFE      26
#define FRWEAPON_LASER            27
#define FRWEAPON_GRENADE          28
#define FRWEAPON_TIMEDMINE        29
#define FRWEAPON_PROXIMITYMINE    30
#define FRWEAPON_REMOTEMINE       31

#define FR_DIFFICULTY_TO_SCORE(diff) ((diff) + 1)

extern u8 *_firingrangeSegmentRomStart;
extern u8 *_firingrangeSegmentRomEnd;

struct frdata g_FrData;
struct trainingdata g_DtData;
struct trainingdata g_HtData;

u16 *g_FrScriptOffsets = NULL;
u8 g_FrIsValidWeapon = false;
u8 g_FrDataLoaded = false;
u8 g_FrNumSounds = 0;
u8 *g_FrRomData = NULL;

u16 g_FrPads[] = {
	0x00d6, 0x00d7, 0x00d9, 0x00d8, 0x00da, 0x00db, 0x00dc, 0x00dd,
	0x00de, 0x00df, 0x00e0, 0x00e1, 0x00e2, 0x00e3, 0x00e4, 0x00e5,
	0x00e6, 0x00e7, 0x00e8, 0x00e9, 0x00ea, 0x00eb, 0x00f4, 0x00f3,
	0x00f2, 0x00f1, 0x00f0, 0x00ef, 0x00ee, 0x00ed, 0x00ec,
};

u32 ht_get_stageflag(s32 index);
u32 ci_get_stage_flag_by_device_index(u32 deviceindex);

bool ci_is_tour_done(void)
{
	return gamefile_has_flag(GAMEFILEFLAG_CI_TOUR_DONE);
}

u8 fr_get_score(s32 frweaponnum)
{
#if VERSION == VERSION_JPN_FINAL
	if (frweaponnum == fr_weaponnum_to_frweaponnum(WEAPON_COMBATKNIFE)) {
		// The knife doesn't exist in the JPN version.
		// Treat it as completed so unlockables still work.
		return FRSCORE_GOLD;
	}
#endif

	// Data at firingrangescores is a u8 array where each score uses 2 bits
	return (g_GameFile.firingrangescores[frweaponnum >> 2] >> (frweaponnum % 4) * 2) & 3;
}

void fr_save_score_if_best(s32 frweaponnum, s32 score)
{
	if (score > fr_get_score(frweaponnum)) {
		u32 byteindex = frweaponnum >> 2;
		u32 shiftamount = (frweaponnum % 4) * 2;
		u32 value = g_GameFile.firingrangescores[byteindex];
		u32 mask = (1 << shiftamount) + (1 << (shiftamount + 1));

		value &= 255 - mask;
		value += (score << shiftamount) & mask;

		g_GameFile.firingrangescores[byteindex] = value;
	}
}

s32 fr_weaponnum_to_slotnum(u32 weaponnum)
{
	s32 slot = -1;
	s32 i;

	for (i = 0; i <= WEAPON_HORIZONSCANNER; i++) {
		switch (i) {
		case WEAPON_FALCON2:
		case WEAPON_FALCON2_SCOPE:
		case WEAPON_FALCON2_SILENCER:
		case WEAPON_MAGSEC4:
		case WEAPON_MAULER:
		case WEAPON_PHOENIX:
		case WEAPON_DY357MAGNUM:
		case WEAPON_DY357LX:
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
		case WEAPON_CROSSBOW:
		case WEAPON_TRANQUILIZER:
		case WEAPON_REAPER:
		case WEAPON_DEVASTATOR:
		case WEAPON_ROCKETLAUNCHER:
		case WEAPON_SLAYER:
		case WEAPON_COMBATKNIFE:
		case WEAPON_LASER:
		case WEAPON_GRENADE:
		case WEAPON_TIMEDMINE:
		case WEAPON_PROXIMITYMINE:
		case WEAPON_REMOTEMINE:
			slot++;
		}

		if (i == weaponnum) {
			return slot;
		}
	}

	return -1;
}

u8 fr_is_weapon_found(s32 weaponnum)
{
	u32 byteindex;

	if (weaponnum <= WEAPON_UNARMED) {
		return true;
	}

#if VERSION >= VERSION_NTSC_1_0
	if (weaponnum < (s32)sizeof(g_GameFile.weaponsfound) * 8) {
		byteindex = weaponnum >> 3;
		return g_GameFile.weaponsfound[byteindex] & (1 << (weaponnum % 8));
	}

	return false;
#else
	byteindex = weaponnum >> 3;
	return g_GameFile.weaponsfound[byteindex] & (1 << (weaponnum % 8));
#endif
}

void fr_set_weapon_found(s32 weaponnum)
{
	if (weaponnum < (s32)sizeof(g_GameFile.weaponsfound) * 8) {
		u32 byteindex = weaponnum >> 3;
		u32 value = g_GameFile.weaponsfound[byteindex];

		value |= (1 << (weaponnum % 8));

		g_GameFile.weaponsfound[byteindex] = value;
	}
}

s32 ci_is_stage_complete(s32 stageindex)
{
	return g_GameFile.besttimes[stageindex][0]
		|| g_GameFile.besttimes[stageindex][1]
		|| g_GameFile.besttimes[stageindex][2];
}

bool fr_is_weapon_available_for_mp(s32 weapon)
{
	if (weapon <= 0 || weapon == WEAPON_PSYCHOSISGUN) {
		return false;
	}

	if (weapon == WEAPON_XRAYSCANNER && ci_is_stage_complete(SOLOSTAGEINDEX_INFILTRATION)) {
		return true;
	}

	if (weapon == WEAPON_CLOAKINGDEVICE && ci_is_stage_complete(SOLOSTAGEINDEX_CHICAGO)) {
		return true;
	}

	return fr_is_weapon_found(weapon);
}

bool fr_is_weapon_available_for_fr(s32 weapon)
{
#if VERSION == VERSION_JPN_FINAL
	if (weapon == WEAPON_COMBATKNIFE) {
		return false;
	}
#endif

	if (weapon < WEAPON_FALCON2 || weapon > WEAPON_REMOTEMINE
			|| weapon == WEAPON_PSYCHOSISGUN
			|| weapon == WEAPON_COMBATBOOST
			|| weapon == WEAPON_NBOMB) {
		return false;
	}

	if (weapon == WEAPON_FALCON2 || weapon == WEAPON_CMP150) {
		return true;
	}

#if VERSION < VERSION_NTSC_1_0
#ifdef DEBUG
	if (debug_is_all_training_enabled() && weapon <= WEAPON_XRAYSCANNER) {
		return true;
	}
#endif
#endif

	return fr_is_weapon_found(weapon);
}

u32 fr_weaponnum_to_frweaponnum(u32 weaponnum)
{
	switch (weaponnum) {
	case WEAPON_FALCON2:          return FRWEAPON_FALCON2;
	case WEAPON_FALCON2_SCOPE:    return FRWEAPON_FALCON2_SCOPE;
	case WEAPON_FALCON2_SILENCER: return FRWEAPON_FALCON2_SILENCER;
	case WEAPON_MAGSEC4:          return FRWEAPON_MAGSEC4;
	case WEAPON_MAULER:           return FRWEAPON_MAULER;
	case WEAPON_PHOENIX:          return FRWEAPON_PHOENIX;
	case WEAPON_DY357MAGNUM:      return FRWEAPON_DY357MAGNUM;
	case WEAPON_DY357LX:          return FRWEAPON_DY357LX;
	case WEAPON_CMP150:           return FRWEAPON_CMP150;
	case WEAPON_CYCLONE:          return FRWEAPON_CYCLONE;
	case WEAPON_CALLISTO:         return FRWEAPON_CALLISTO;
	case WEAPON_RCP120:           return FRWEAPON_RCP120;
	case WEAPON_LAPTOPGUN:        return FRWEAPON_LAPTOPGUN;
	case WEAPON_DRAGON:           return FRWEAPON_DRAGON;
	case WEAPON_K7AVENGER:        return FRWEAPON_K7AVENGER;
	case WEAPON_AR34:             return FRWEAPON_AR34;
	case WEAPON_SUPERDRAGON:      return FRWEAPON_SUPERDRAGON;
	case WEAPON_SHOTGUN:          return FRWEAPON_SHOTGUN;
	case WEAPON_SNIPERRIFLE:      return FRWEAPON_SNIPERRIFLE;
	case WEAPON_FARSIGHT:         return FRWEAPON_FARSIGHT;
	case WEAPON_CROSSBOW:         return FRWEAPON_CROSSBOW;
	case WEAPON_TRANQUILIZER:     return FRWEAPON_TRANQUILIZER;
	case WEAPON_REAPER:           return FRWEAPON_REAPER;
	case WEAPON_DEVASTATOR:       return FRWEAPON_DEVASTATOR;
	case WEAPON_ROCKETLAUNCHER:   return FRWEAPON_ROCKETLAUNCHER;
	case WEAPON_SLAYER:           return FRWEAPON_SLAYER;
	case WEAPON_COMBATKNIFE:      return FRWEAPON_COMBATKNIFE;
	case WEAPON_LASER:            return FRWEAPON_LASER;
	case WEAPON_GRENADE:          return FRWEAPON_GRENADE;
	case WEAPON_TIMEDMINE:        return FRWEAPON_TIMEDMINE;
	case WEAPON_PROXIMITYMINE:    return FRWEAPON_PROXIMITYMINE;
	case WEAPON_REMOTEMINE:       return FRWEAPON_REMOTEMINE;
	}

	return 0;
}

u32 fr_get_weapon_script_index(u32 weaponnum)
{
	switch (weaponnum) {
	case WEAPON_FALCON2:          return 1;
	case WEAPON_FALCON2_SCOPE:    return 2;
	case WEAPON_FALCON2_SILENCER: return 3;
	case WEAPON_MAGSEC4:          return 4;
	case WEAPON_MAULER:           return 5;
	case WEAPON_PHOENIX:          return 6;
	case WEAPON_DY357MAGNUM:      return 7;
	case WEAPON_DY357LX:          return 8;
	case WEAPON_CMP150:           return 9;
	case WEAPON_CYCLONE:          return 10;
	case WEAPON_CALLISTO:         return 11;
	case WEAPON_RCP120:           return 12;
	case WEAPON_LAPTOPGUN:        return 13;
	case WEAPON_DRAGON:           return 14;
	case WEAPON_K7AVENGER:        return 15;
	case WEAPON_AR34:             return 16;
	case WEAPON_SUPERDRAGON:      return 17;
	case WEAPON_SHOTGUN:          return 18;
	case WEAPON_SNIPERRIFLE:      return 19;
	case WEAPON_FARSIGHT:         return 20;
	case WEAPON_CROSSBOW:         return 21;
	case WEAPON_TRANQUILIZER:     return 22;
	case WEAPON_REAPER:           return 23;
	case WEAPON_DEVASTATOR:       return 24;
	case WEAPON_ROCKETLAUNCHER:   return 25;
	case WEAPON_SLAYER:           return 26;
	case WEAPON_COMBATKNIFE:      return 27;
	case WEAPON_LASER:            return 28;
	case WEAPON_GRENADE:          return 29;
	case WEAPON_TIMEDMINE:        return 31;
	case WEAPON_PROXIMITYMINE:    return 32;
	case WEAPON_REMOTEMINE:       return 33;
	}

	return 0;
}

s32 fr_is_classic_weapon_unlocked(u32 weapon)
{
	switch (weapon) {
	case WEAPON_PP9I:
		return fr_get_score(FRWEAPON_FALCON2) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_FALCON2_SCOPE) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_FALCON2_SILENCER) == FRSCORE_GOLD;
	case WEAPON_CC13:
		return fr_get_score(FRWEAPON_MAGSEC4) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_MAULER) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_PHOENIX) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_DY357MAGNUM) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_DY357LX) == FRSCORE_GOLD;
	case WEAPON_KL01313:
		return fr_get_score(FRWEAPON_CMP150) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_CYCLONE) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_CALLISTO) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_RCP120) == FRSCORE_GOLD;
	case WEAPON_KF7SPECIAL:
		return fr_get_score(FRWEAPON_LAPTOPGUN) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_DRAGON) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_K7AVENGER) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_AR34) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_SUPERDRAGON) == FRSCORE_GOLD;
	case WEAPON_ZZT:
		return fr_get_score(FRWEAPON_SHOTGUN) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_SNIPERRIFLE) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_ROCKETLAUNCHER) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_SLAYER) == FRSCORE_GOLD;
	case WEAPON_DMC:
#if VERSION >= VERSION_NTSC_1_0
		return fr_get_score(FRWEAPON_TIMEDMINE) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_PROXIMITYMINE) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_REMOTEMINE) == FRSCORE_GOLD;
#else
		return fr_get_score(FRWEAPON_TIMEDMINE) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_PROXIMITYMINE) == FRSCORE_GOLD
			&& fr_get_score(32) == FRSCORE_GOLD
			&& fr_get_score(33) == FRSCORE_GOLD
			&& fr_get_score(34) == FRSCORE_GOLD;
#endif
	case WEAPON_AR53:
		return fr_get_score(FRWEAPON_FARSIGHT) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_CROSSBOW) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_COMBATKNIFE) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_GRENADE) == FRSCORE_GOLD;
	case WEAPON_RCP45:
		return fr_get_score(FRWEAPON_TRANQUILIZER) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_REAPER) == FRSCORE_GOLD
			&& fr_get_score(FRWEAPON_DEVASTATOR) == FRSCORE_GOLD;
	}

	return false;
}

s32 fr_get_slot(void)
{
	return g_FrData.slot;
}

void fr_set_slot(s32 slot)
{
	g_FrData.slot = slot;
}

u32 fr_get_weapon_by_slot(s32 slot)
{
	s32 index = -1;
	s32 weapon;

	for (weapon = WEAPON_NONE; weapon <= WEAPON_HORIZONSCANNER; weapon++) {
		if (fr_is_weapon_available_for_fr(weapon)) {
			index++;
		}

		if (slot == index) {
			return weapon;
		}
	}

	return WEAPON_UNARMED;
}

s32 fr_get_num_weapons_available(void)
{
	s32 count = 0;
	s32 i;

	for (i = WEAPON_UNARMED; i <= WEAPON_HORIZONSCANNER; i++) {
		if (fr_is_weapon_available_for_fr(i)) {
			count++;
		}
	}

	return count;
}

void fr_init_lighting(void)
{
	if (g_FrData.donelighting == false) {
		s32 roomnum;

		for (roomnum = ROOM_DISH_0007; roomnum <= ROOM_DISH_0009; roomnum++) {
			room_set_light_op(roomnum, LIGHTOP_TRANSITION, 50, 100, TICKS(32));
		}

		room_set_light_op(ROOM_DISH_FIRINGRANGE, LIGHTOP_TRANSITION, 25, 100, TICKS(32));

		g_FrData.donelighting = true;

		snd_start(var80095200, SFXNUM_05D5_FR_LIGHTSON, NULL, -1, -1, -1, -1, -1);
	}

	chr_set_stage_flag(NULL, STAGEFLAG_CI_IN_TRAINING);
}

void fr_restore_lighting(void)
{
	if (g_FrData.donelighting == true) {
		s32 roomnum;

		for (roomnum = ROOM_DISH_0007; roomnum <= ROOM_DISH_0009; roomnum++) {
			room_set_light_op(roomnum, LIGHTOP_TRANSITION, 100, 50, TICKS(8));
		}

		room_set_light_op(ROOM_DISH_FIRINGRANGE, LIGHTOP_TRANSITION, 100, 25, TICKS(8));

		g_FrData.donelighting = false;

		snd_start(var80095200, SFXNUM_05D6_FR_LIGHTSOFF, NULL, -1, -1, -1, -1, -1);
	}
}

void fr_reset(void)
{
	s32 i;

#if VERSION >= VERSION_NTSC_1_0
	g_FrScriptOffsets = NULL;
#endif

	g_FrDataLoaded = false;
	g_FrIsValidWeapon = false;
	g_FrRomData = NULL;

	g_FrData.helpscriptindex = 0;
	g_FrData.helpscriptoffset = 0;
	g_FrData.helpscriptenabled = false;
	g_FrData.helpscriptsleep = 0;

#if VERSION >= VERSION_NTSC_1_0
	g_FrData.menucountdown = 0;
	g_FrData.maxactivetargets = 0;

	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		g_FrData.targets[i].prop = NULL;
		g_FrData.targets[i].inuse = false;
	}

	g_FrNumSounds = 0;
#endif
}

void *fr_load_rom_data(u32 len)
{
	g_FrRomData = memp_alloc(ALIGN16(len), MEMPOOL_STAGE);

	if (g_FrRomData) {
		return dma_exec_with_auto_align(g_FrRomData, (romptr_t) &_firingrangeSegmentRomStart, len);
	}

	return NULL;
}

void fr_set_difficulty(s32 difficulty)
{
	if (difficulty < FRDIFFICULTY_BRONZE) {
		difficulty = FRDIFFICULTY_BRONZE;
	}

	if (difficulty > FRDIFFICULTY_GOLD) {
		difficulty = FRDIFFICULTY_GOLD;
	}

	g_FrData.difficulty = difficulty;
}

u32 fr_get_difficulty(void)
{
	return g_FrData.difficulty;
}

void fr_init_defaults(void)
{
	s32 i;
	struct pad pad;

	g_FrNumSounds = 0;

	pad_unpack(g_FrPads[0], PADFIELD_POS, &pad);

	g_FrData.maxactivetargets = 0;
	g_FrData.goalscore = 0;
	g_FrData.timelimit = 200;
	g_FrData.ammolimit = 255;
	g_FrData.sdgrenadelimit = 255;
	g_FrData.goalaccuracy = 0;
	g_FrData.goaltargets = 255;
	g_FrData.speed = 1;

	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		g_FrData.targets[i].dstpos.x = pad.pos.x;
		g_FrData.targets[i].dstpos.y = pad.pos.y;
		g_FrData.targets[i].dstpos.z = pad.pos.z;

#if VERSION >= VERSION_NTSC_1_0
		g_FrData.targets[i].dstpos.z += 6.0f * i;
#endif

		g_FrData.targets[i].inuse = false;
		g_FrData.targets[i].rotateoncloak = false;
		g_FrData.targets[i].destroyed = false;
		g_FrData.targets[i].damage = 0;
		g_FrData.targets[i].scriptoffset = 0;

		g_FrData.targets[i].travelspeed = 0;
		g_FrData.targets[i].scriptsleep = SECSTOTIME60(255);
		g_FrData.targets[i].timeuntilrotate = 0;

		g_FrData.targets[i].rotating = false;
		g_FrData.targets[i].rotatespeed = 0;
		g_FrData.targets[i].angle = 0;
		g_FrData.targets[i].rotatetoangle = 0;
		g_FrData.targets[i].silent = 0;
		g_FrData.targets[i].donestopsound = false;
		g_FrData.targets[i].travelling = false;
		g_FrData.targets[i].invincibletimer = 0;
		g_FrData.targets[i].frpadnum = -1;

		wallhits_free_by_prop(g_FrData.targets[i].prop, 0);
		wallhits_free_by_prop(g_FrData.targets[i].prop, 1);
	}

	g_FrData.timetaken = TICKS(-240);
	g_FrData.score = 0;
	g_FrData.numtargets = 0;
	g_FrData.targetsdestroyed = 0;
	g_FrData.menucountdown = 0;
	g_FrData.padindexoffset = 0;
	g_FrData.feedbackzone = 0;
	g_FrData.feedbackttl = 0;
	g_FrData.failreason = 0;
	g_FrData.numshots = 0;
	g_FrData.numhitsring3 = 0;
	g_FrData.numhitsring2 = 0;
	g_FrData.numhitsring1 = 0;
	g_FrData.numhitsbullseye = 0;
	g_FrData.helpscriptindex = 0;
	g_FrData.helpscriptoffset = 0;
	g_FrData.helpscriptenabled = false;
	g_FrData.helpscriptsleep = 0;
	g_FrData.proxyendtimer = 0;
	g_FrData.donealarm = false;
	g_FrData.ammohasgrace = true;
	g_FrData.ammoextra = -1;
}

struct frdata *fr_get_data(void)
{
	return &g_FrData;
}

u32 fr_resolve_fr_pad(u32 arg0)
{
	switch (arg0) {
	case 31: return random() % 9 + 4;  // 4 - 12
	case 32: return random() % 9 + 13; // 13 - 21
	case 33: return random() % 9 + 22; // 22 - 30
	case 34: return random() % 27 + 4; // 4 - 30
	}

	return g_FrData.padindexoffset + arg0;
}

bool fr_is_difficulty(u32 flags)
{
	if (g_FrData.difficulty == FRDIFFICULTY_BRONZE) {
		if ((flags & FRTARGETFLAG_BRONZE) == 0) {
			return false;
		}
	} else if (g_FrData.difficulty == FRDIFFICULTY_SILVER) {
		if ((flags & FRTARGETFLAG_SILVER) == 0) {
			return false;
		}
	} else if (g_FrData.difficulty == FRDIFFICULTY_GOLD) {
		if ((flags & FRTARGETFLAG_GOLD) == 0) {
			return false;
		}
	}

	return true;
}

void fr_execute_weapon_script(s32 scriptindex)
{
	s32 offset = 0;

	if (scriptindex >= FRSCRIPTINDEX_WEAPONS && scriptindex < FRSCRIPTINDEX_TARGETS) {
		u8 *script = &g_FrRomData[g_FrScriptOffsets[scriptindex]];
		u8 mult = 1;
		u32 stack[5];
		s32 start;
		s32 capacity;
		s32 index;
		u8 *subscript;
		s32 i;
		s32 val;

		while (script[offset] != FRCMD_END) {
			switch (script[offset]) {
			case FRCMD_SETPADINDEXOFFSET:
				g_FrData.padindexoffset = script[offset + 1];
				offset += 2;
				break;
			case FRCMD_ADDTARGET:
				if (!fr_is_difficulty(script[offset + 4])) {
					offset += 5;
					break;
				}
				if (g_FrData.numtargets < ARRAYCOUNT(g_FrData.targets)) {
					g_FrData.targets[g_FrData.numtargets].frpadindex = fr_resolve_fr_pad(script[offset + 1]);
					g_FrData.targets[g_FrData.numtargets].scriptindex = script[offset + 2];
					g_FrData.targets[g_FrData.numtargets].maxdamage = script[offset + 3];
					g_FrData.targets[g_FrData.numtargets].inuse = true;
					g_FrData.targets[g_FrData.numtargets].flags = script[offset + 4];

					if (g_FrData.targets[g_FrData.numtargets].flags & FRTARGETFLAG_ROTATEONCLOAK) {
						g_FrData.targets[g_FrData.numtargets].rotateoncloak = true;
					}

					if (g_FrData.targets[g_FrData.numtargets].flags & FRTARGETFLAG_ONEHITEXPLODE) {
						g_FrData.targets[g_FrData.numtargets].maxdamage = 1;
					}

					g_FrData.numtargets++;
				}
				offset += 5;
				break;
			case FRCMD_SETMAXACTIVETARGETS:
				g_FrData.maxactivetargets = script[g_FrData.difficulty + offset + 1];
				offset += 4;
				break;
			case FRCMD_SETSCOREMULTIPLIER:
				if (script[g_FrData.difficulty + offset + 1] > 0) {
					mult = script[g_FrData.difficulty + offset + 1];
				} else {
					mult = 1;
				}
				offset += 4;
				break;
			case FRCMD_SETGOALSCORE:
				g_FrData.goalscore = script[g_FrData.difficulty + offset + 1] * mult;
				offset += 4;
				break;
			case FRCMD_SETTIMELIMIT:
				g_FrData.timelimit = script[g_FrData.difficulty + offset + 1];
				if (g_FrData.timelimit == 255) {
					g_FrData.timelimit = 120;
				}
				offset += 4;
				break;
			case FRCMD_SETAMMOLIMIT:
				capacity = bgun_get_capacity_by_ammotype(bgun_get_ammo_type_for_weapon(fr_get_weapon_by_slot(g_FrData.slot), 0));
				g_FrData.ammolimit = script[g_FrData.difficulty + offset + 1];

				if (g_FrData.ammolimit != 255) {
					if (g_FrData.ammolimit > capacity) {
						g_FrData.ammoextra = g_FrData.ammolimit - capacity;
					} else {
						g_FrData.ammoextra = 0;
					}
				}

				offset += 4;
				break;
			case FRCMD_SETGRENADELIMIT:
				capacity = bgun_get_capacity_by_ammotype(AMMOTYPE_DEVASTATOR);
				g_FrData.sdgrenadelimit = script[g_FrData.difficulty + offset + 1];

				if (g_FrData.sdgrenadelimit != 255) {
					if (g_FrData.sdgrenadelimit > capacity) {
						g_FrData.sdgrenadeextra = g_FrData.sdgrenadelimit - capacity;
					} else {
						g_FrData.sdgrenadeextra = 0;
					}
				}

				offset += 4;
				break;
			case FRCMD_SETEXTRASPEED:
				g_FrData.speed = script[g_FrData.difficulty + offset + 1] * 0.1f + 1.0f;
				offset += 4;
				break;
			case FRCMD_SETGOALACCURACY:
				g_FrData.goalaccuracy = script[g_FrData.difficulty + offset + 1];
				offset += 4;
				break;
			case FRCMD_SETGOALTARGETS:
				g_FrData.goaltargets = script[g_FrData.difficulty + offset + 1];
				offset += 4;
				break;
			case FRCMD_SETHELPSCRIPT:
				g_FrData.helpscriptindex = script[offset + 1];
				g_FrData.helpscriptenabled = true;
				index = FRSCRIPTINDEX_HELP + g_FrData.helpscriptindex;
				if (&g_FrRomData[g_FrScriptOffsets[index]]);
				subscript = &g_FrRomData[g_FrScriptOffsets[index]];
				offset += 2;

				if (g_FrData.difficulty == FRDIFFICULTY_BRONZE) {
					start = FRCMD_IFBRONZE;
				} else if (g_FrData.difficulty == FRDIFFICULTY_SILVER) {
					start = FRCMD_IFSILVER;
				} else if (g_FrData.difficulty == FRDIFFICULTY_GOLD) {
					start = FRCMD_IFGOLD;
				}

				g_FrData.helpscriptoffset = 0;
				i = 0;

				while (1) {
					g_FrData.helpscriptoffset++;

					if (subscript[i] == start) {
						i++;

						val = subscript[i];

						if (val >= FRCMD_IFBRONZE) {
							g_FrData.helpscriptoffset++;
							val = subscript[i + 1];
						}

						if (val >= FRCMD_IFBRONZE) {
							g_FrData.helpscriptoffset++;
						}
						break;
					}

					i++;
				}
				break;
			}
		}
	}
}

void fr_set_target_props(void)
{
	s32 i;
	u32 targets[] = {
		0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x11,
		0x12, 0x13, 0x14, 0x15, 0x16, 0x17, 0x18, 0x19, 0x1a,
	};

	for (i = 0; i < ARRAYCOUNT(targets); i++) {
		struct defaultobj *obj = obj_find_by_tag_id(targets[i]);

		if (obj) {
			g_FrData.targets[i].prop = obj->prop;
			obj->flags2 |= OBJFLAG2_INVISIBLE;
		}
	}
}

s32 g_FrWeaponNum = WEAPON_UNARMED;

bool fr_target_is_at_script_start(s32 targetnum)
{
	return g_FrData.targets[targetnum].scriptoffset == 0;
}

/**
 * 0 => "FIRING\n Press Z Button to fire gun.\n"
 * 1 => "AUTO RELOAD\n Release Z Button when out of ammo.\n"
 * 2 => "MANUAL RELOAD\n Press B Button to reload early if magazine not full.\n"
 * 3 => "Aiming: Hold down R Button to enter Aim mode.\n"
 * 4 => "Use Control Stick to move aiming sight.\n"
 * 5 => "AUTO FIRE\n Hold Z Button to repeatedly fire automatically.\n"
 * 6 => "ALTER AIM\n Press Up C Button or Down C Button to move sight up/down.\n"
 * 7 => "ZOOM\n Hold R Button to enter Zoom mode.\n"
 * 8 => "FAST FIRE\n Press Z Button quickly to fire faster.\n"
 */
char *fr_get_instructional_text(u32 index)
{
	u16 textid = (u16)(g_FrRomData[index * 2] << 8) | g_FrRomData[index * 2 + 1];

	return lang_get(textid);
}

void fr_execute_help_script(void)
{
	if (!g_FrData.helpscriptenabled || g_Vars.lvupdate240 == 0) {
		return;
	}

	if (g_FrData.helpscriptsleep == 0) {
		s32 index = FRSCRIPTINDEX_HELP + g_FrData.helpscriptindex;
		u8 *script = &g_FrRomData[g_FrScriptOffsets[index]];
		u32 offset = g_FrData.helpscriptoffset;

		switch (script[offset]) {
		case FRCMD_END:
		case FRCMD_IFBRONZE:
		case FRCMD_IFSILVER:
		case FRCMD_IFGOLD:
			g_FrData.helpscriptenabled = false;
			break;
		case FRCMD_HUDMSG:
			hudmsg_create(fr_get_instructional_text(script[offset + 1]), HUDMSGTYPE_TRAINING);
			g_FrData.helpscriptoffset += 2;
			break;
		case FRCMD_HELPWAITSECONDS:
#if PAL
			g_FrData.helpscriptsleep = script[offset + 1] * 50;
#else
			g_FrData.helpscriptsleep = SECSTOTIME60(script[offset + 1]);
#endif
			g_FrData.helpscriptoffset += 2;
			break;
		case FRCMD_WAITUNTILSHOOT:
			if (g_FrData.numshots) {
				g_FrData.helpscriptoffset++;
			}
			break;
		}
	} else {
		g_FrData.helpscriptsleep -= g_Vars.lvupdate60;

		if (g_FrData.helpscriptsleep <= 0) {
			g_FrData.helpscriptsleep = 0;
		}
	}
}

bool fr_execute_target_script(s32 targetnum)
{
	if (g_FrData.targets[targetnum].inuse) {
		s32 index = FRSCRIPTINDEX_TARGETS + g_FrData.targets[targetnum].scriptindex;
		u8 *script = &g_FrRomData[g_FrScriptOffsets[index]];
		s32 offset = g_FrData.targets[targetnum].scriptoffset;
		struct pad pad;
		s32 frpadnum;

		switch (script[offset]) {
		case FRCMD_END:
			g_FrData.targets[targetnum].scriptenabled = true;
			g_FrData.targets[targetnum].scriptsleep = 255 * 60;
			return true;
		case FRCMD_GOTOPAD:
			frpadnum = fr_resolve_fr_pad(script[offset + 1]);

			if (frpadnum == g_FrData.targets[targetnum].frpadnum) {
				g_FrData.targets[targetnum].scriptoffset += 4;
				return false;
			}

			g_FrData.targets[targetnum].frpadnum = frpadnum;

			pad_unpack(g_FrPads[frpadnum], PADFIELD_POS, &pad);

			g_FrData.targets[targetnum].dstpos.x = pad.pos.x;
			g_FrData.targets[targetnum].dstpos.y = pad.pos.y;
			g_FrData.targets[targetnum].dstpos.z = pad.pos.z;

#if VERSION >= VERSION_NTSC_1_0
			g_FrData.targets[targetnum].dstpos.z += 6.0f * targetnum;
#endif

			if (script[offset + 2] == 0xff) {
				g_FrData.targets[targetnum].travelspeed = -1;
				g_FrData.targets[targetnum].travelling = true;
			} else {
				if (g_FrNumSounds < 3) {
					g_FrNumSounds++;
					ps_create(NULL, g_FrData.targets[targetnum].prop, SFXNUM_05D9_FR_CONVEYER, -1,
							-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
				}

				g_FrData.targets[targetnum].travelspeed = script[offset + 2] / 60.0f * g_FrData.speed;
				g_FrData.targets[targetnum].travelling = true;
			}

			g_FrData.targets[targetnum].scriptsleep = script[offset + 3] * TICKS(60);
			g_FrData.targets[targetnum].donestopsound = false;
			g_FrData.targets[targetnum].scriptoffset += 4;
			return true;
		case FRCMD_RESTART:
			g_FrData.targets[targetnum].scriptoffset = 0;
			return true;
		case FRCMD_WAITSECONDS:
			g_FrData.targets[targetnum].scriptenabled = true;
			g_FrData.targets[targetnum].scriptsleep = script[offset + 1] * TICKS(60);
			g_FrData.targets[targetnum].scriptoffset += 2;
			return true;
		case FRCMD_ROTATE:
			if (g_FrData.targets[targetnum].rotateoncloak == false) {
				f32 angles[4];
				angles[0] = DTOR(-90);
				angles[1] = DTOR(-180);
				angles[2] = DTOR(90);
				angles[3] = DTOR(180);

				g_FrData.targets[targetnum].rotatetoangle = g_FrData.targets[targetnum].angle + angles[script[offset + 1]];
				g_FrData.targets[targetnum].rotatespeed = angles[script[offset + 1]] / (script[offset + 2] * 15);
				g_FrData.targets[targetnum].rotating = true;
				g_FrData.targets[targetnum].scriptenabled = false;
			}

			if (1);
			g_FrData.targets[targetnum].scriptoffset += 3;
			return true;
		}
	}

	return true;
}

void fr_hide_all_targets(void)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		struct prop *prop = g_FrData.targets[i].prop;
		struct defaultobj *target = prop->obj;

		target->flags2 |= OBJFLAG2_INVISIBLE;

		ps_stop_sound(prop, PSTYPE_GENERAL, 0xffff);
	}
}

void fr_init_targets(void)
{
	s32 count = 0;
	s32 i;
	struct prop *prop;
	struct defaultobj *obj;
	struct pad pad;
	struct coord pos;
	Mtxf sp144;
	f32 sp108[3][3];

	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		prop = g_FrData.targets[i].prop;

#if VERSION >= VERSION_NTSC_1_0
		if (prop)
#endif
		{
			obj = prop->obj;

			obj_free(obj, false, true);

			obj->damage = 0;
			prop->timetoregen = 0;

			if (g_FrData.targets[i].inuse) {
				g_FrData.targets[i].scriptenabled = false;
				g_FrData.targets[i].destroyed = false;

				if (count < g_FrData.maxactivetargets) {
					obj->flags2 &= ~OBJFLAG2_INVISIBLE;
					g_FrData.targets[i].active = true;
				} else {
					obj->flags2 |= OBJFLAG2_INVISIBLE;
					g_FrData.targets[i].active = false;
				}

				pad_unpack(g_FrPads[g_FrData.targets[i].frpadindex], PADFIELD_POS, &pad);

				pos.f[0] = pad.pos.f[0];
				pos.f[1] = pad.pos.f[1];
#if VERSION >= VERSION_NTSC_1_0
				pos.f[2] = pad.pos.f[2] + 6.0f * i;
#else
				pos.f[2] = pad.pos.f[2];
#endif

				fr_execute_target_script(i);

				if (g_FrData.targets[i].travelspeed == -1) {
					pos.x = g_FrData.targets[i].dstpos.x;
					pos.y = g_FrData.targets[i].dstpos.y;
					pos.z = g_FrData.targets[i].dstpos.z;
				}

				count++;
			} else {
				obj->flags2 |= OBJFLAG2_INVISIBLE;
			}

			if (obj->flags2 & OBJFLAG2_INVISIBLE) {
#if VERSION < VERSION_NTSC_1_0
				pad_unpack(g_FrPads[g_FrData.targets[i].frpadindex], PADFIELD_POS, &pad);

				pos.x = 0.0f;
				pos.y = 5000.0f;
				pos.z = 0.0f;
#else
				pos.x = 0.0f;
				pos.y = 5000.0f;
				pos.z = 6.0f * i;
#endif
			}

			if (g_FrData.targets[i].flags & FRTARGETFLAG_SPAWNFACINGAWAY) {
				mtx4_load_y_rotation(0.0f, &sp144);
				g_FrData.targets[i].angle = DTOR(180);
			} else {
				mtx4_load_y_rotation(DTOR(180), &sp144);
			}

			mtx00015f04(obj->model->scale, &sp144);
			mtx4_to_mtx3(&sp144, sp108);
			mtx3_copy(sp108, obj->realrot);

			prop->pos.x = pos.x;
			prop->pos.y = pos.y;
			prop->pos.z = pos.z;

			obj_onmoved(obj, true, false);
		}
	}
}

void fr_close_and_lock_door(void)
{
	struct defaultobj *obj = obj_find_by_tag_id(0x91);

	if (obj && obj->prop && obj->prop->type == PROPTYPE_DOOR) {
		struct doorobj *door = (struct doorobj *)obj;
		door->keyflags |= 0x40;
		doors_request_mode(door, DOORMODE_CLOSING);
	}
}

void fr_unlock_door(void)
{
	struct defaultobj *obj = obj_find_by_tag_id(0x91);

	if (obj && obj->prop && obj->prop->type == PROPTYPE_DOOR) {
		struct doorobj *door = (struct doorobj *)obj;
		door->keyflags &= ~0x40;
	}
}

void fr_load_data(void)
{
	if (!g_FrDataLoaded) {
		s32 len = (s32)&_firingrangeSegmentRomEnd - (s32)&_firingrangeSegmentRomStart;
		s32 index = 0;
		u32 i;
		u32 numscripts = 1;
		s32 size;

		if (index);

		g_FrDataLoaded = true;

		fr_load_rom_data(len);

		for (i = 0x12; i < len; i++) {
			if (g_FrRomData[i] == FRCMD_START) {
				numscripts++;
			}
		}

		size = numscripts * sizeof(*g_FrScriptOffsets);
		g_FrScriptOffsets = memp_alloc(ALIGN16(size), MEMPOOL_STAGE);

		if (numscripts < 0);

		if (g_FrScriptOffsets) {
			for (i = 0x12; i < len; i++) {
				if (g_FrRomData[i] == FRCMD_START) {
					g_FrScriptOffsets[index] = i + 1;
					index++;
				}
			}
		}

		fr_set_target_props();

		g_FrData.slot = 0;
		g_FrData.difficulty = FRDIFFICULTY_BRONZE;
		g_FrData.donelighting = false;
	}
}

u32 fr_init_ammo(s32 weaponnum)
{
	u32 scriptindex;
	u32 ammotype = bgun_get_ammo_type_for_weapon(weaponnum, 0);
	u32 capacity = bgun_get_capacity_by_ammotype(ammotype);

	fr_init_defaults();
	scriptindex = fr_get_weapon_script_index(weaponnum);
	fr_execute_weapon_script(scriptindex);

	if (g_FrData.ammolimit == 255) {
		bgun_set_ammo_quantity(ammotype, capacity);
	} else {
		bgun_set_ammo_quantity(ammotype, g_FrData.ammolimit);
	}

	if (weaponnum == WEAPON_SUPERDRAGON) {
		if (g_FrData.sdgrenadelimit == 255) {
			bgun_set_ammo_quantity(AMMOTYPE_DEVASTATOR, capacity);
		} else {
			bgun_set_ammo_quantity(AMMOTYPE_DEVASTATOR, g_FrData.sdgrenadelimit);
		}
	}

	return scriptindex;
}

void fr_begin_session(s32 weapon)
{
	s32 i;
	struct defaultobj *obj = obj_find_by_tag_id(0x7f); // computer

	if (obj) {
		obj->flags |= OBJFLAG_CANNOT_ACTIVATE;
	}

	fr_close_and_lock_door();

	for (i = 0; i < 2; i++) {
		if (g_Vars.currentplayer->gunctrl.ammotypes[i] >= 0) {
			g_Vars.currentplayer->hands[0].loadedammo[i] = 0;
			g_Vars.currentplayer->hands[1].loadedammo[i] = 0;
		}
	}

	g_FrIsValidWeapon = fr_init_ammo(weapon) == 0 ? false : true;
	fr_init_targets();
	bgun_set_passive_mode(false);
}

char *fr_get_weapon_description(void)
{
	u32 weapon = fr_get_weapon_by_slot(g_FrData.slot);

	switch (weapon) {
#if VERSION >= VERSION_PAL_BETA
	case WEAPON_FALCON2:          return lang_get(L_DISH_283);
	case WEAPON_FALCON2_SCOPE:    return lang_get(L_DISH_284);
	case WEAPON_FALCON2_SILENCER: return lang_get(L_DISH_285);
	case WEAPON_MAGSEC4:          return lang_get(L_DISH_286);
	case WEAPON_MAULER:           return lang_get(L_DISH_287);
	case WEAPON_PHOENIX:          return lang_get(L_DISH_288);
	case WEAPON_DY357MAGNUM:      return lang_get(L_DISH_289);
	case WEAPON_DY357LX:          return lang_get(L_DISH_290);
	case WEAPON_CMP150:           return lang_get(L_DISH_291);
	case WEAPON_CYCLONE:          return lang_get(L_DISH_292);
	case WEAPON_CALLISTO:         return lang_get(L_DISH_293);
	case WEAPON_RCP120:           return lang_get(L_DISH_294);
	case WEAPON_LAPTOPGUN:        return lang_get(L_DISH_295);
	case WEAPON_DRAGON:           return lang_get(L_DISH_296);
	case WEAPON_K7AVENGER:        return lang_get(L_DISH_297);
	case WEAPON_AR34:             return lang_get(L_DISH_298);
	case WEAPON_SUPERDRAGON:      return lang_get(L_DISH_299);
	case WEAPON_SHOTGUN:          return lang_get(L_DISH_300);
	case WEAPON_SNIPERRIFLE:      return lang_get(L_DISH_301);
	case WEAPON_FARSIGHT:         return lang_get(L_DISH_302);
	case WEAPON_CROSSBOW:         return lang_get(L_DISH_303);
	case WEAPON_TRANQUILIZER:     return lang_get(L_DISH_304);
	case WEAPON_REAPER:           return lang_get(L_DISH_305);
	case WEAPON_DEVASTATOR:       return lang_get(L_DISH_306);
	case WEAPON_ROCKETLAUNCHER:   return lang_get(L_DISH_307);
	case WEAPON_SLAYER:           return lang_get(L_DISH_308);
	case WEAPON_COMBATKNIFE:      return lang_get(L_DISH_309);
	case WEAPON_LASER:            return lang_get(L_DISH_310);
	case WEAPON_GRENADE:          return lang_get(L_DISH_311);
	case WEAPON_NBOMB:            return lang_get(L_DISH_312);
	case WEAPON_TIMEDMINE:        return lang_get(L_DISH_313);
	case WEAPON_PROXIMITYMINE:    return lang_get(L_DISH_314);
	case WEAPON_REMOTEMINE:       return lang_get(L_DISH_315);
#else
	case WEAPON_FALCON2:          return lang_get(L_MISC_377);
	case WEAPON_FALCON2_SCOPE:    return lang_get(L_MISC_378);
	case WEAPON_FALCON2_SILENCER: return lang_get(L_MISC_379);
	case WEAPON_MAGSEC4:          return lang_get(L_MISC_380);
	case WEAPON_MAULER:           return lang_get(L_MISC_381);
	case WEAPON_PHOENIX:          return lang_get(L_MISC_382);
	case WEAPON_DY357MAGNUM:      return lang_get(L_MISC_383);
	case WEAPON_DY357LX:          return lang_get(L_MISC_384);
	case WEAPON_CMP150:           return lang_get(L_MISC_385);
	case WEAPON_CYCLONE:          return lang_get(L_MISC_386);
	case WEAPON_CALLISTO:         return lang_get(L_MISC_387);
	case WEAPON_RCP120:           return lang_get(L_MISC_388);
	case WEAPON_LAPTOPGUN:        return lang_get(L_MISC_389);
	case WEAPON_DRAGON:           return lang_get(L_MISC_390);
	case WEAPON_K7AVENGER:        return lang_get(L_MISC_391);
	case WEAPON_AR34:             return lang_get(L_MISC_392);
	case WEAPON_SUPERDRAGON:      return lang_get(L_MISC_393);
	case WEAPON_SHOTGUN:          return lang_get(L_MISC_394);
	case WEAPON_SNIPERRIFLE:      return lang_get(L_MISC_395);
	case WEAPON_FARSIGHT:         return lang_get(L_MISC_396);
	case WEAPON_CROSSBOW:         return lang_get(L_MISC_397);
	case WEAPON_TRANQUILIZER:     return lang_get(L_MISC_398);
	case WEAPON_REAPER:           return lang_get(L_MISC_399);
	case WEAPON_DEVASTATOR:       return lang_get(L_MISC_400);
	case WEAPON_ROCKETLAUNCHER:   return lang_get(L_MISC_401);
	case WEAPON_SLAYER:           return lang_get(L_MISC_402);
	case WEAPON_COMBATKNIFE:      return lang_get(L_MISC_403);
	case WEAPON_LASER:            return lang_get(L_MISC_404);
	case WEAPON_GRENADE:          return lang_get(L_MISC_405);
	case WEAPON_NBOMB:            return lang_get(L_MISC_406);
	case WEAPON_TIMEDMINE:        return lang_get(L_MISC_407);
	case WEAPON_PROXIMITYMINE:    return lang_get(L_MISC_408);
	case WEAPON_REMOTEMINE:       return lang_get(L_MISC_409);
#endif
	}

	return NULL;
}

void fr_end_session(bool hidetargets)
{
	s32 i;
	s32 j;
	s16 propnums[256];
	s16 *propnumptr;
	RoomNum rooms[20];
	u32 stack1;
	RoomNum rooms2[10];
	u32 stack2;

	if (g_FrDataLoaded) {
		struct defaultobj *terminal = obj_find_by_tag_id(0x7f);

		if (terminal) {
			terminal->flags &= ~OBJFLAG_CANNOT_ACTIVATE;
		}

		fr_unlock_door();

		if (g_Vars.currentplayer->visionmode == VISIONMODE_SLAYERROCKET) {
			g_Vars.currentplayer->visionmode = VISIONMODE_NORMAL;
		}

		bgun_set_passive_mode(true);

		g_FrIsValidWeapon = 0;

		fr_restore_lighting();

		if (hidetargets) {
			fr_hide_all_targets();
		}

		if (g_ThrownLaptops[0].base.prop) {
			obj_free_permanently(&g_ThrownLaptops[0].base, true);
		}

		rooms_copy(g_Vars.currentplayer->prop->rooms, rooms);

		for (i = 0; g_Vars.currentplayer->prop->rooms[i] != -1; i++) {
			bg_room_get_neighbours(g_Vars.currentplayer->prop->rooms[i], rooms2, 10);
			rooms_append(rooms2, rooms, 20);
		}

		// Remove projectiles and throwables
		room_get_props(rooms, propnums, 256);

		propnumptr = propnums;

		while (*propnumptr >= 0) {
			struct prop *prop = &g_Vars.props[*propnumptr];

			if (prop) {
				struct defaultobj *obj = prop->obj;

				if (prop->type == PROPTYPE_WEAPON) {
					if (obj->type == OBJTYPE_AUTOGUN) {
						obj_free_permanently(obj, true);
					}

					if (obj->type == OBJTYPE_WEAPON) {
						struct weaponobj *weapon = (struct weaponobj *)obj;

						if (weapon->weaponnum == WEAPON_NBOMB
								|| weapon->weaponnum == WEAPON_BOLT
								|| weapon->weaponnum == WEAPON_COMBATKNIFE
								|| weapon->weaponnum == WEAPON_HOMINGROCKET
								|| weapon->weaponnum == WEAPON_GRENADE
								|| weapon->weaponnum == WEAPON_GRENADEROUND
								|| weapon->weaponnum == WEAPON_PROXIMITYMINE
								|| weapon->weaponnum == WEAPON_REMOTEMINE
								|| weapon->weaponnum == WEAPON_ROCKET
								|| weapon->weaponnum == WEAPON_TIMEDMINE
								|| weapon->weaponnum == WEAPON_SKROCKET
								|| (weapon->weaponnum == WEAPON_DRAGON && weapon->gunfunc == FUNC_SECONDARY)
								|| (weapon->weaponnum == WEAPON_LAPTOPGUN && weapon->gunfunc == FUNC_SECONDARY)) {
							obj_free_permanently(obj, true);
						}
					}
				}
			}

			propnumptr++;
		}

		// Remove explosions
		for (i = 0; i < g_MaxExplosions; i++) {
			g_Explosions[i].age = 256;

			for (j = 0; j < ARRAYCOUNT(g_Explosions[i].parts); j++) {
				g_Explosions[i].parts[j].frame = 0;
			}
		}

		// Remove smoke
		for (i = 0; i < g_MaxSmokes; i++) {
			g_Smokes[i].age = 256;

			for (j = 0; j < ARRAYCOUNT(g_Smokes[i].parts); j++) {
				g_Smokes[i].parts[j].size = 0;
			}
		}
	}

	player_display_health();

	g_Vars.currentplayer->bondhealth = 1;
}

bool fr_was_too_inaccurate(void)
{
	f32 sum = (g_FrData.numhitsring3 +
		+ g_FrData.numhitsbullseye
		+ g_FrData.numhitsring1
		+ g_FrData.numhitsring2) * 100.0f;

	if (g_FrData.numshots) {
		f32 accuracy = sum / g_FrData.numshots;

		if (accuracy < g_FrData.goalaccuracy) {
			return true;
		}
	}

	return false;
}

void fr_set_fail_reason(s32 failreason)
{
	fr_end_session(false);

	g_FrData.failreason = fr_was_too_inaccurate() ? FRFAILREASON_INACCURATE : failreason;
	g_FrData.menutype = FRMENUTYPE_FAILED;
	g_FrData.menucountdown = TICKS(60);
}

void fr_set_completed(void)
{
	fr_end_session(false);

	if (fr_was_too_inaccurate()) {
		g_FrData.failreason = FRFAILREASON_INACCURATE;
		g_FrData.menutype = FRMENUTYPE_FAILED;
	} else {
		u32 frweaponindex = fr_weaponnum_to_frweaponnum(fr_get_weapon_by_slot(g_FrData.slot));
		fr_save_score_if_best(frweaponindex, FR_DIFFICULTY_TO_SCORE(g_FrData.difficulty));
		g_FrData.menutype = FRMENUTYPE_COMPLETED;
	}

	g_FrData.menucountdown = TICKS(60);
}

bool fr_is_target_one_hit_explodable(struct prop *prop)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		if (g_FrData.targets[i].inuse
				&& g_FrData.targets[i].destroyed == false
				&& g_FrData.targets[i].active
				&& prop == g_FrData.targets[i].prop) {
			if (g_FrData.targets[i].flags & FRTARGETFLAG_ONEHITEXPLODE) {
				return true;
			}

			return false;
		}
	}

	return false;
}

f32 fr_get_target_angle_to_pos(struct coord *targetpos, f32 targetangle, struct coord *pos)
{
	f32 xdiff = targetpos->x - pos->x;
	f32 zdiff = targetpos->z - pos->z;
	f32 directangle = atan2f(xdiff, zdiff);
	f32 relativeangle = directangle - targetangle;

	if (directangle < targetangle) {
		relativeangle += BADDTOR(360);
	}

	return relativeangle;
}

bool fr_is_target_facing_pos(struct prop *prop, struct coord *pos)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		if (prop == g_FrData.targets[i].prop) {
			f32 angle;

			if (g_FrData.targets[i].destroyed) {
				return false;
			}

			angle = fr_get_target_angle_to_pos(&prop->pos, g_FrData.targets[i].angle, pos);

			if (angle > DTOR(90) && angle < BADDTOR(270)) {
				return false;
			}

			return true;
		}
	}

	return true;
}

struct prop *fr_choose_autogun_target(struct coord *autogunpos)
{
	f32 closestdist = 0x20000000;
	s32 facingtargets[ARRAYCOUNT(g_FrData.targets)];
	s32 len = 0;
	struct prop *closesttarget = NULL;
	s32 i;

	// Make list of targets which are facing the laptop gun
	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		if (g_FrData.targets[i].inuse
				&& g_FrData.targets[i].destroyed == false
				&& g_FrData.targets[i].active) {
			f32 angle = fr_get_target_angle_to_pos(&g_FrData.targets[i].prop->pos, g_FrData.targets[i].angle, autogunpos);

			if (angle > DTOR(90) && angle < BADDTOR(270)) {
				// facing away
			} else {
				facingtargets[len++] = i;
			}
		}
	}

	// Determine which of the facing targets is closest
	for (i = 0; i < len; i++) {
		struct prop *prop = g_FrData.targets[facingtargets[i]].prop;
		f32 xdiff = prop->pos.f[0] - autogunpos->f[0];
		f32 ydiff = prop->pos.f[1] - autogunpos->f[1];
		f32 zdiff = prop->pos.f[2] - autogunpos->f[2];
		f32 dist = xdiff * xdiff + ydiff * ydiff + zdiff * zdiff;

		if (dist < closestdist) {
			closestdist = dist;
			closesttarget = prop;
		}
	}

	if (facingtargets);

	return closesttarget;
}

bool fr_is_ammo_wasted(void)
{
	s32 weaponnum = fr_get_weapon_by_slot(g_FrData.slot);
	s32 i;
	s32 priammotype = bgun_get_ammo_type_for_weapon(weaponnum, 0);
	s32 secammotype = bgun_get_ammo_type_for_weapon(weaponnum, 1);
	struct hand *hand0 = &g_Vars.currentplayer->hands[0];
	struct hand *hand1 = &g_Vars.currentplayer->hands[1];
	s32 ammoloaded[2];
	s32 ammototal[2];
	s16 *propnumptr;
	s16 propnums[258];
	RoomNum rooms20[22];
	RoomNum rooms10[12];
	u32 stack[4];
	s32 ammotype;
	struct hand *hand;
	struct prop *prop;
	struct prop *child;

	// Laser has unlimited ammo
	if (weaponnum == WEAPON_LASER) {
		return false;
	}

	// Check if player has ammo
	ammoloaded[0] = hand0->loadedammo[0] + hand1->loadedammo[0];
	ammoloaded[1] = hand0->loadedammo[1] + hand1->loadedammo[1];
	ammototal[0] = bgun_get_reserved_ammo_count(priammotype) + ammoloaded[0];
	ammototal[1] = bgun_get_reserved_ammo_count(secammotype) + ammoloaded[1];

	if (ammototal[0] <= 0 && ammototal[1] <= 0) {
		// Don't do any further checks if this is the first frame where we've
		// gotten this far. I'm guessing this fixes a frame perfect bug, however
		// testing with this check removed doesn't cause any unusual behaviour.
		if (g_FrData.ammohasgrace) {
			g_FrData.ammohasgrace = false;
			return false;
		}

		// Check if there are any explosions
		for (i = 0; i != MAX_EXPLOSIONS; i++) {
			if (g_Explosions[i].prop) {
				return false;
			}
		}

		// Check projectiles
		if (weaponnum == WEAPON_ROCKETLAUNCHER
				|| weaponnum == WEAPON_SLAYER
				|| weaponnum == WEAPON_DEVASTATOR
				|| weaponnum == WEAPON_SUPERDRAGON
				|| weaponnum == WEAPON_COMBATKNIFE
				|| weaponnum == WEAPON_CROSSBOW
				|| weaponnum == WEAPON_GRENADE
				|| weaponnum == WEAPON_NBOMB
				|| weaponnum == WEAPON_TIMEDMINE
				|| weaponnum == WEAPON_PROXIMITYMINE
				|| weaponnum == WEAPON_REMOTEMINE) {
			rooms_copy(g_Vars.currentplayer->prop->rooms, rooms20);

			for (i = 0; g_Vars.currentplayer->prop->rooms[i] != -1; i++) {
				bg_room_get_neighbours(g_Vars.currentplayer->prop->rooms[i], rooms10, 10);
				rooms_append(rooms10, rooms20, 20);
			}

			room_get_props(rooms20, propnums, 256);
			propnumptr = propnums;

			while (*propnumptr >= 0) {
				prop = &g_Vars.props[*propnumptr];
				child = prop->child;

				if ((child && child->type == PROPTYPE_WEAPON && child->weapon->weaponnum == WEAPON_TIMEDMINE)
						|| (child && child->type == PROPTYPE_WEAPON && child->weapon->weaponnum == WEAPON_REMOTEMINE)
						|| (child && child->type == PROPTYPE_WEAPON && child->weapon->weaponnum == WEAPON_PROXIMITYMINE)
						|| (child && child->type == PROPTYPE_WEAPON && child->weapon->weaponnum == WEAPON_GRENADEROUND)) {
					return false;
				}

				if (prop->type == PROPTYPE_WEAPON) {
					if (prop->weapon->weaponnum == WEAPON_ROCKET
							|| prop->weapon->weaponnum == WEAPON_HOMINGROCKET
							|| prop->weapon->weaponnum == WEAPON_GRENADE
							|| prop->weapon->weaponnum == WEAPON_GRENADEROUND) {
						return false;
					}

					if (prop->weapon->weaponnum == WEAPON_BOLT
							|| prop->weapon->weaponnum == WEAPON_COMBATKNIFE) {
						if (prop->obj->hidden & OBJHFLAG_PROJECTILE) {
							return false;
						}
					} else if (prop->weapon->weaponnum == WEAPON_TIMEDMINE
							|| prop->weapon->weaponnum == WEAPON_REMOTEMINE) {
						return false;
					} else if (prop->weapon->weaponnum == WEAPON_PROXIMITYMINE) {
						if (g_FrData.proxyendtimer == -255) {
							return false;
						}

						if (g_FrData.proxyendtimer == 0) {
							// Initial state - set the timer to 5 seconds if player is now out of mines
							ammotype = bgun_get_ammo_type_for_weapon(weaponnum, 0);
							hand = &g_Vars.currentplayer->hands[HAND_RIGHT];

							if (bgun_get_reserved_ammo_count(ammotype) + hand->loadedammo[0] == 0) {
								g_FrData.proxyendtimer = TICKS(300);
							}

							return false;
						}

						g_FrData.proxyendtimer -= g_Vars.lvupdate60;

						if (g_FrData.proxyendtimer <= 0) {
							// Timer has just hit zero - remove all proxy items
							for (i = 0; i < ARRAYCOUNT(g_Proxies); i++) {
								if (g_Proxies[i]) {
									g_Proxies[i]->timer240 = 0;
								}
							}

							g_FrData.proxyendtimer = -255;
							return true;
						}

						return false;
					}
				}

				propnumptr++;
			}
		}

		return true;
	}

	return false;
}

void fr_tick(void)
{
	s32 ammotype;
	s32 capacity;
	s32 weaponnum;
	struct coord diff;
	struct coord newpos;
	u8 weaponnum2;
	struct prop *prop;
	struct defaultobj *obj;
	struct defaultobj *obj2;
	s32 invincible;
	s32 i;
	s32 j;
	f32 dist;
	u32 stack;
	struct ammodef *ammo;
	u8 exploding;
	bool oldside;
	struct modelrodata_bbox *bbox;
	s32 tmp;
#if VERSION >= VERSION_NTSC_1_0
	f32 mult;
#endif
	bool newside;
	struct chrdata *chr;
	bool cloaked;
	f32 toangle;
	f32 speed;
	Mtxf spbc;
	f32 sp98[3][3];

	if (g_FrIsValidWeapon
			&& g_Vars.currentplayer->gunctrl.throwing == false
			&& inv_has_single_weapon_inc_all_guns(fr_get_weapon_by_slot(g_FrData.slot))) {
		bgun_equip_weapon(fr_get_weapon_by_slot(g_FrData.slot));
	}

	// NTSC beta does the room code then menu code,
	// while everything else does the menu code then room code
#if VERSION < VERSION_NTSC_1_0
	// End the session if the player slipped through the door before it closed
	if (g_Vars.currentplayer->prop->rooms[0] != ROOM_DISH_FIRINGRANGE) {
		if (g_FrIsValidWeapon) {
			for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
				if (g_FrData.targets[i].inuse
						&& g_FrData.targets[i].destroyed == false
						&& g_FrData.targets[i].silent == false
						&& g_FrData.targets[i].travelling) {
					g_FrData.targets[i].silent = true;
					ps_stop_sound(g_FrData.targets[i].prop, PSTYPE_GENERAL, 0xffff);
				}
			}

			g_Vars.currentplayer->training = false;
			fr_end_session(true);
			g_FrData.menucountdown = 0; // This assignment is in NTSC beta only
			chr_unset_stage_flag(NULL, STAGEFLAG_CI_IN_TRAINING);
		}
		return;
	}
#endif

	// Handle the menu countdown
	if (g_FrData.menucountdown != 0) {
		g_FrData.menucountdown -= g_Vars.lvupdate60;

		// Prevent showing the menu until gun is put away
		if (g_FrData.menucountdown <= 0) {
			if ((g_FrData.menutype == FRMENUTYPE_FAILED || g_FrData.menutype == FRMENUTYPE_COMPLETED)
					&& g_Vars.currentplayer->hands[HAND_RIGHT].gset.weaponnum != WEAPON_UNARMED) {
				g_FrData.menucountdown = 1;
			}
		}

		if (g_FrData.menucountdown <= 0) {
			g_FrData.menucountdown = 0;

			for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
				if (g_FrData.targets[i].prop) {
					ps_stop_sound(g_FrData.targets[i].prop, PSTYPE_GENERAL, 0xffff);
				}
			}

			switch (g_FrData.menutype) {
			case FRMENUTYPE_WEAPONLIST:
				menu_push_root_dialog_and_pause(ci_get_fr_weapon_list_menu_dialog(), MENUROOT_TRAINING);
				break;
			case FRMENUTYPE_DETAILS:
				menu_push_root_dialog_and_pause(&g_FrTrainingInfoPreGameMenuDialog, MENUROOT_TRAINING);
				break;
			case FRMENUTYPE_FAILED:
				snd_start(var80095200, SFXNUM_05DB_TRAINING_FAIL, NULL, -1, -1, -1, -1, -1);
				menu_push_root_dialog_and_pause(&g_FrFailedMenuDialog, MENUROOT_TRAINING);
				break;
			case FRMENUTYPE_COMPLETED:
				snd_start(var80095200, SFXNUM_05DC_TRAINING_COMPLETE, NULL, -1, -1, -1, -1, -1);
				menu_push_root_dialog_and_pause(&g_FrCompletedMenuDialog, MENUROOT_TRAINING);
				filemgr_save_or_load(&g_GameFileGuid, FILEOP_SAVE_GAME_000, 0);
				break;
			}
		}
		return;
	}

#if VERSION >= VERSION_NTSC_1_0
	// End the session if the player slipped through the door before it closed
	if (g_Vars.currentplayer->prop->rooms[0] != ROOM_DISH_FIRINGRANGE) {
		if (g_FrIsValidWeapon) {
			for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
				if (g_FrData.targets[i].inuse
						&& g_FrData.targets[i].destroyed == false
						&& g_FrData.targets[i].silent == false
						&& g_FrData.targets[i].travelling) {
					g_FrData.targets[i].silent = true;
					ps_stop_sound(g_FrData.targets[i].prop, PSTYPE_GENERAL, 0xffff);
				}
			}

			g_Vars.currentplayer->training = false;
			fr_end_session(true);
			chr_unset_stage_flag(NULL, STAGEFLAG_CI_IN_TRAINING);
		}
		return;
	}
#endif

	if (!g_FrIsValidWeapon) {
		return;
	}

	if (g_Vars.currentplayer->isdead) {
		fr_end_session(false);
	}

	// If paused, stop any target sounds
	if (g_Vars.lvupdate240 == 0) {
		for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
			if (g_FrData.targets[i].inuse
					&& g_FrData.targets[i].destroyed == false
					&& g_FrData.targets[i].silent == false
					&& g_FrData.targets[i].travelling) {
				g_FrData.targets[i].silent = true;
				ps_stop_sound(g_FrData.targets[i].prop, PSTYPE_GENERAL, 0xffff);
			}
		}
		return;
	}

	g_Vars.currentplayer->training = true;
	fr_execute_help_script();

	// Top up the player's ammo if the config defined more ammo than the
	// weapon allows, or if it defined unlimited ammo
	if (g_FrData.numshotssincetopup != 0) {
		weaponnum = fr_get_weapon_by_slot(g_FrData.slot);
		ammotype = bgun_get_ammo_type_for_weapon(weaponnum, 0);
		capacity = bgun_get_capacity_by_ammotype(ammotype);
		ammo = gset_get_ammodef(weaponnum, 0);
		capacity -= (ammo ? ammo->clipsize : 0);

		if (g_FrData.ammoextra > 0) {
			tmp = bgun_get_reserved_ammo_count(ammotype);
			g_FrData.ammoextra -= g_FrData.numshotssincetopup;

			if (g_FrData.ammoextra < 0) {
				g_FrData.ammoextra = 0;
			}

			capacity = tmp + g_FrData.numshotssincetopup;
			bgun_set_ammo_quantity(ammotype, capacity);
		} else if (g_FrData.ammoextra == -1) {
			bgun_set_ammo_quantity(ammotype, capacity);
		}

		if (weaponnum == WEAPON_SUPERDRAGON) {
			capacity = bgun_get_capacity_by_ammotype(AMMOTYPE_DEVASTATOR);

			if (g_FrData.sdgrenadeextra > 0) {
				tmp = bgun_get_reserved_ammo_count(AMMOTYPE_DEVASTATOR);
				g_FrData.sdgrenadeextra -= g_FrData.numshotssincetopup;

				if (g_FrData.sdgrenadeextra < 0) {
					g_FrData.sdgrenadeextra = 0;
				}

				capacity = tmp + g_FrData.numshotssincetopup;
				bgun_set_ammo_quantity(AMMOTYPE_DEVASTATOR, capacity);
			} else if (g_FrData.sdgrenadeextra == -1) {
				bgun_set_ammo_quantity(AMMOTYPE_DEVASTATOR, capacity);
			}
		}

		g_FrData.numshotssincetopup = 0;
	}

	g_FrData.timetaken += g_Vars.lvupdate60;

	// Handle prestart
	if (g_FrData.timetaken < 0) {
		if (g_FrData.numshots == 0) {
			if (g_FrData.donealarm == false && g_FrData.timetaken > TICKS(-180)) {
				g_FrData.donealarm = true;
				snd_start(var80095200, SFXNUM_05D4_FR_ALARM, NULL, -1, -1, -1, -1, -1);
			}

			if (!g_FrData.donelighting && g_FrData.timetaken > TICKS(-225)) {
				fr_init_lighting();
			}

			return;
		}

		// Fired a shot during prestart
		if (!g_FrData.donelighting) {
			fr_init_lighting();
		}

		g_FrData.timetaken = 0;
		g_FrData.donealarm = true;
	}

	// Iterate each target and handle their health active/inactive state
	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		if (g_FrData.targets[i].inuse && g_FrData.targets[i].destroyed == false && g_FrData.targets[i].active) {
			invincible = false;
			exploding = false;
			weaponnum2 = fr_get_weapon_by_slot(g_FrData.slot);
			prop = g_FrData.targets[i].prop;
			obj = prop->obj;

			switch (weaponnum2) {
			case WEAPON_GRENADE:
			case WEAPON_PROXIMITYMINE:
				coord_trigger_proxies(&prop->pos, true);
				break;
			}

			if (g_FrData.targets[i].travelling && g_FrData.targets[i].silent && g_FrData.targets[i].travelspeed != -1) {
				g_FrData.targets[i].silent = false;
				ps_create(NULL, g_FrData.targets[i].prop, SFXNUM_05D9_FR_CONVEYER, -1,
						-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
			}

			if (g_FrData.targets[i].angle > DTOR(131.29858422661f) && g_FrData.targets[i].angle < DTOR2(228.7014257913f)) {
				obj->damage = 0;
			}

			if ((g_FrData.targets[i].flags & FRTARGETFLAG_TMPINVINCIBLE)
					&& g_FrData.targets[i].invincibletimer < TICKS(300)) {
				invincible = true;
				g_FrData.targets[i].invincibletimer += g_Vars.lvupdate60;
			}

			if (obj->damage > 0) {
				if (invincible || g_FrData.targets[i].angle == DTOR(180)) {
					obj->damage = 0;
				} else if (g_FrData.targets[i].flags & FRTARGETFLAG_ONEHITEXPLODE
						|| obj->damage >= obj->maxdamage
						|| fr_get_weapon_by_slot(g_FrData.slot) == WEAPON_PHOENIX) {
					g_FrData.numhitsbullseye++;
					g_FrData.score += 10;
					exploding = true;
					g_FrData.feedbackttl = TICKS(60);
					g_FrData.feedbackzone = FRZONE_EXPLODE;
				}
			}

			// Handle target being destroyed
			if (exploding || (g_FrData.targets[i].maxdamage != 255
						&& g_FrData.targets[i].damage >= g_FrData.targets[i].maxdamage)) {
				bbox = obj_find_bbox_rodata(obj);

				if (g_FrNumSounds && g_FrData.targets[i].travelling) {
					g_FrNumSounds--;
					ps_stop_sound(prop, PSTYPE_GENERAL, 0xffff);
				}

				if (g_FrNumSounds);

				shards_create(&prop->pos, &obj->realrot[0][0], &obj->realrot[1][0], &obj->realrot[2][0],
						bbox->xmin, bbox->xmax, bbox->ymin, bbox->ymax, 2, prop);

				g_FrData.targetsdestroyed++;

				if (g_FrData.targets[i].flags & FRTARGETFLAG_ONEHITEXPLODE) {
					explosion_create_simple(g_FrData.targets[i].prop, &g_FrData.targets[i].prop->pos,
							g_FrData.targets[i].prop->rooms, EXPLOSIONTYPE_FRTARGET, 1);
				}

				g_FrData.targets[i].travelling = false;
				g_FrData.targets[i].active = false;
				g_FrData.targets[i].destroyed = true;

				obj->flags2 |= OBJFLAG2_INVISIBLE;

				prop->pos.x = 0;
				prop->pos.y = -5000;
				prop->pos.z = 0;

				obj_onmoved(obj, true, false);

				// Activate another target
				for (j = 0; j < ARRAYCOUNT(g_FrData.targets); j++) {
					if (g_FrData.targets[j].destroyed == false
							&& g_FrData.targets[j].inuse
							&& g_FrData.targets[j].active == false) {
						obj2 = g_FrData.targets[j].prop->obj;
						g_FrData.targets[j].active = true;
						obj2->flags2 &= ~OBJFLAG2_INVISIBLE;
						break;
					}
				}
			}
		}
	}

	// Check if the session should end
	if (g_FrData.goaltargets == 255) {
		if (g_FrData.goalscore && g_FrData.score >= g_FrData.goalscore) {
			fr_set_completed();
			return;
		}
	} else {
		if (g_FrData.targetsdestroyed >= g_FrData.goaltargets
				&& (g_FrData.goalscore == 0 || g_FrData.score >= g_FrData.goalscore)) {
			fr_set_completed();
			return;
		}
	}

	if (g_FrData.targetsdestroyed >= g_FrData.numtargets) {
		fr_set_fail_reason(FRFAILREASON_SCOREUNATTAINABLE);
		return;
	}

	if (fr_is_ammo_wasted()) {
		fr_set_fail_reason(FRFAILREASON_OUTOFAMMO);
		return;
	}

	if (g_FrData.timelimit != 255 && g_FrData.timetaken >= g_FrData.timelimit * TICKS(60)) {
		fr_set_fail_reason(FRFAILREASON_TIMEOVER);
		return;
	}

	// Tick each target
	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		if (g_FrData.targets[i].inuse
				&& g_FrData.targets[i].destroyed == false
				&& g_FrData.targets[i].active) {
			prop = g_FrData.targets[i].prop;
			obj = prop->obj;

			if (g_FrData.targets[i].travelling) {
				if (g_FrData.targets[i].travelspeed == -1) {
					g_FrData.targets[i].donestopsound = true;
					g_FrData.targets[i].travelling = false;
#if VERSION >= VERSION_NTSC_1_0
					mult = 1;
#endif
					dist = -2;
				} else {
					diff.x = g_FrData.targets[i].dstpos.x - prop->pos.x;
					diff.y = g_FrData.targets[i].dstpos.y - prop->pos.y;
					diff.z = g_FrData.targets[i].dstpos.z - prop->pos.z;

					dist = sqrtf(diff.f[0] * diff.f[0] + diff.f[1] * diff.f[1] + diff.f[2] * diff.f[2]);
#if VERSION >= VERSION_NTSC_1_0
					mult = 1;
#endif

					if (dist != 0) {
#if VERSION >= VERSION_NTSC_1_0

#if VERSION >= VERSION_PAL_BETA
						mult = g_FrData.targets[i].travelspeed * g_Vars.lvupdate60freal;
#else
						mult = (g_FrData.targets[i].travelspeed * g_Vars.lvupdate240) * 0.25f;
#endif
						diff.x *= 1.0f / dist;
						diff.y *= 1.0f / dist;
						diff.z *= 1.0f / dist;
						newpos.x = diff.x * mult + prop->pos.x;
						newpos.y = diff.y * mult + prop->pos.y;
						newpos.z = diff.z * mult + prop->pos.z;
#else
						diff.x *= 1.0f / dist;
						diff.y *= 1.0f / dist;
						diff.z *= 1.0f / dist;
						newpos.x = diff.x * g_FrData.targets[i].travelspeed * g_Vars.lvupdate240 * 0.25f + prop->pos.x;
						newpos.y = diff.y * g_FrData.targets[i].travelspeed * g_Vars.lvupdate240 * 0.25f + prop->pos.y;
						newpos.z = diff.z * g_FrData.targets[i].travelspeed * g_Vars.lvupdate240 * 0.25f + prop->pos.z;
#endif
					} else {
						dist = -2;
					}
				}

#if VERSION >= VERSION_NTSC_1_0
				if (mult >= dist)
#else
				if (dist < g_FrData.targets[i].travelspeed)
#endif
				{
					// Target is stopping
					newpos.x = g_FrData.targets[i].dstpos.x;
					newpos.y = g_FrData.targets[i].dstpos.y;
					newpos.z = g_FrData.targets[i].dstpos.z;

					g_FrData.targets[i].scriptenabled = true;
					g_FrData.targets[i].travelling = false;

					if (g_FrData.targets[i].donestopsound == false) {
						g_FrData.targets[i].donestopsound = true;

						if (g_FrNumSounds) {
							g_FrNumSounds--;
						}

						ps_stop_sound(prop, PSTYPE_GENERAL, 0xffff);
						ps_create(NULL, prop, SFXNUM_05DA_FR_CONVEYER_STOP, -1,
								-1, PSFLAG_AMBIENT, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);

						if (g_FrNumSounds);
					}
				}

				prop->pos.x = newpos.x;
				prop->pos.y = newpos.y;
				prop->pos.z = newpos.z;

				obj_onmoved(obj, true, false);
			}

			if (g_FrData.targets[i].rotateoncloak && g_FrData.targets[i].rotating == false) {
				if (g_FrData.targets[i].timeuntilrotate == 0) {
					chr = g_Vars.currentplayer->prop->chr;
					cloaked = chr->hidden & CHRHFLAG_CLOAKED;

					if (cloaked) {
						if (g_FrData.targets[i].angle == DTOR(180)) {
							g_FrData.targets[i].timeuntilrotate = TICKS(60);
							g_FrData.targets[i].rotatetoangle = 0;
							g_FrData.targets[i].rotatespeed = DTOR(-180) / 90;
						}
					} else {
						if (g_FrData.targets[i].angle == 0) {
							g_FrData.targets[i].timeuntilrotate = TICKS(60);
							g_FrData.targets[i].rotatetoangle = DTOR(180);
							g_FrData.targets[i].rotatespeed = DTOR(180) / 90;
						}
					}
				} else {
					g_FrData.targets[i].timeuntilrotate -= g_Vars.lvupdate60;

					if (g_FrData.targets[i].timeuntilrotate <= 0) {
						g_FrData.targets[i].timeuntilrotate = 0;
						g_FrData.targets[i].rotating = true;
					}
				}
			} else if (g_FrData.targets[i].rotating) {
				toangle = g_FrData.targets[i].rotatetoangle;
				speed = g_FrData.targets[i].rotatespeed;

				if (toangle);

				oldside = 0;

				if (g_FrData.targets[i].angle < toangle) {
					oldside = 1;
				}

				oldside = (u8)oldside;

#if VERSION >= VERSION_PAL_BETA
				g_FrData.targets[i].angle += speed * g_Vars.lvupdate60freal;
#else
				g_FrData.targets[i].angle += speed * g_Vars.lvupdate240 * 0.25f;
#endif

				newside = 0;

				toangle = g_FrData.targets[i].rotatetoangle;

				if (g_FrData.targets[i].angle < toangle) {
					newside = 1;
				}

				newside = (u8)newside;

				if (newside != oldside || g_FrData.targets[i].angle == toangle) {
					// Reached desired angle
					g_FrData.targets[i].angle = g_FrData.targets[i].rotatetoangle;
					g_FrData.targets[i].rotating = false;
					g_FrData.targets[i].scriptenabled = true;
					g_FrData.targets[i].scriptsleep = 0;

					while (g_FrData.targets[i].angle > BADDTOR(360)) {
						g_FrData.targets[i].angle -= BADDTOR(360);
					}

					while (g_FrData.targets[i].angle < 0) {
						g_FrData.targets[i].angle += BADDTOR(360);
					}
				}

				mtx4_load_y_rotation(g_FrData.targets[i].angle + DTOR(180), &spbc);
				mtx00015f04(obj->model->scale, &spbc);
				mtx4_to_mtx3(&spbc, sp98);
				mtx3_copy(sp98, obj->realrot);
			}

			if (g_FrData.targets[i].scriptenabled && g_FrData.targets[i].scriptsleep != SECSTOTIME60(255)) {
				g_FrData.targets[i].scriptsleep -= g_Vars.lvupdate60;

				if (g_FrData.targets[i].scriptsleep <= 0) {
					g_FrData.targets[i].scriptenabled = false;

					while (!fr_execute_target_script(i));

					if (fr_target_is_at_script_start(i)) {
						while (!fr_execute_target_script(i));
					}
				}
			}
		}
	}
}

void fr_track_target(struct prop *prop)
{
	struct defaultobj *obj = prop->obj;
	s32 i;

	if (obj->modelnum == MODEL_TARGET) {
		f32 sp68;
		f32 sp64;
		f32 sp60;
		f32 sp56;

		sp64 = -1;
		sp68 = -1;
		sp56 = -2;
		sp60 = -2;

		model_get_screen_coords(obj->model, &sp56, &sp64, &sp60, &sp68);

		for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->trackedprops); i++) {
			if (g_Vars.currentplayer->trackedprops[i].prop == prop) {
				return;
			}

			if (g_Vars.currentplayer->trackedprops[i].prop == NULL) {
				g_Vars.currentplayer->trackedprops[i].prop = prop;

				g_Vars.currentplayer->trackedprops[i].x1 = sp64 - 2;
				g_Vars.currentplayer->trackedprops[i].x2 = sp56 + 2;
				g_Vars.currentplayer->trackedprops[i].y1 = sp68 - 2;
				g_Vars.currentplayer->trackedprops[i].y2 = sp60 + 2;
				g_Vars.currentplayer->targetset[i] = 0;
				return;
			}
		}
	}
}

bool fr_choose_farsight_target(void)
{
	struct prop *bestprop = NULL;
	f32 bestvalue = 1;
	f32 bestdist = -1;
	bool found = false;
	s32 i;

	if (bgun_get_weapon_num(HAND_RIGHT) == WEAPON_FARSIGHT) {
		for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
			if (g_FrData.targets[i].inuse
					&& g_FrData.targets[i].destroyed == false
					&& g_FrData.targets[i].active
					&& g_FrData.targets[i].flags & FRTARGETFLAG_FARSIGHTAUTOTARGETABLE) {
				struct prop *prop = g_FrData.targets[i].prop;
				f32 xdiff = g_Vars.currentplayer->bond2.pos.x - prop->pos.x;
				f32 ydiff = g_Vars.currentplayer->bond2.pos.y - prop->pos.y;
				f32 zdiff = g_Vars.currentplayer->bond2.pos.z - prop->pos.z;
				f32 dist = sqrtf(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);

				if (dist > 0) {
					f32 value = (xdiff * g_Vars.currentplayer->bond2.look.f[0]
							+ ydiff * g_Vars.currentplayer->bond2.look.f[1]
							+ zdiff * g_Vars.currentplayer->bond2.look.f[2]) / dist;

					if (value);

					if (value < 0 && value < bestvalue) {
						bestvalue = value;
						bestprop = prop;
						bestdist = dist;
						found = true;
					}
				}
			}
		}
	}

	g_Vars.currentplayer->autoeraserdist = bestdist;
	g_Vars.currentplayer->autoerasertarget = bestprop;

	return found;
}

s32 fr_is_in_training(void)
{
	if (g_FrData.menucountdown > 0 &&
			(g_FrData.menutype == FRMENUTYPE_FAILED || g_FrData.menutype == FRMENUTYPE_COMPLETED)) {
		return true;
	}

	return g_Vars.currentplayer->prop->rooms[0] == ROOM_DISH_FIRINGRANGE
		&& g_FrIsValidWeapon
		&& main_get_stage_num() == STAGE_CITRAINING;
}

void fr_calculate_hit(struct defaultobj *obj, struct coord *hitpos, f32 maulercharge)
{
	s32 i;

	if (g_FrIsValidWeapon == false) {
		return;
	}

	for (i = 0; i < ARRAYCOUNT(g_FrData.targets); i++) {
		struct prop *prop = g_FrData.targets[i].prop;

		if (obj == prop->obj) {
			f32 xdiff = hitpos->x - prop->pos.x;
			f32 ydiff = hitpos->y - prop->pos.y;
			f32 zdiff = hitpos->z - prop->pos.z;

			f32 dist = sqrtf(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);

			if (g_FrData.targets[i].flags & FRTARGETFLAG_ONEHITEXPLODE) {
				g_FrData.targets[i].damage = g_FrData.targets[i].maxdamage;
			} else if (fr_get_weapon_by_slot(g_FrData.slot) == WEAPON_MAULER) {
				g_FrData.targets[i].damage += (f32)((s32)(maulercharge * 0.1f) + 1);
			} else if ((g_FrData.targets[i].flags & FRTARGETFLAG_TMPINVINCIBLE) == 0
					|| g_FrData.targets[i].invincibletimer >= TICKS(300)) {
				g_FrData.targets[i].damage++;
			}

			if (dist < 18) {
				g_FrData.feedbackzone = FRZONE_BULLSEYE;
				g_FrData.numhitsbullseye++;
			} else if (dist < 37) {
				g_FrData.feedbackzone = FRZONE_RING1;
				g_FrData.numhitsring1++;
			} else if (dist < 56) {
				g_FrData.feedbackzone = FRZONE_RING2;
				g_FrData.numhitsring2++;
			} else {
				g_FrData.feedbackzone = FRZONE_RING3;
				g_FrData.numhitsring3++;
			}

			g_FrData.feedbackttl = TICKS(60);
			g_FrData.score += g_FrData.feedbackzone;
		}
	}
}

void fr_increment_num_shots(void)
{
	g_FrData.numshots++;
	g_FrData.numshotssincetopup++;
}

bool ci_is_chr_bio_unlocked(u32 bodynum)
{
	switch (bodynum) {
	case BODY_DARK_COMBAT:
	case BODY_CARRINGTON:
		return true;
	case BODY_CASSANDRA:
		return ci_is_stage_complete(SOLOSTAGEINDEX_DEFECTION);
	case BODY_DRCAROLL:
		return ci_is_stage_complete(SOLOSTAGEINDEX_INVESTIGATION);
	case BODY_MRBLONDE:
		return ci_is_stage_complete(SOLOSTAGEINDEX_EXTRACTION);
	case BODY_TRENT:
		return ci_is_stage_complete(SOLOSTAGEINDEX_G5BUILDING);
	case BODY_JONATHAN:
		return ci_is_stage_complete(SOLOSTAGEINDEX_INFILTRATION);
	case BODY_THEKING:
		return ci_is_stage_complete(SOLOSTAGEINDEX_RESCUE);
	case BODY_PRESIDENT:
		return ci_is_stage_complete(SOLOSTAGEINDEX_AIRFORCEONE);
	}

	return false;
}

u8 g_ChrBioSlot = 0;

struct chrbio *ci_get_chr_bio_by_bodynum(u32 bodynum)
{
#ifdef AVOID_UB
	static
#endif
	struct chrbio bios[] = {
		// name, race, age, profile
#if VERSION >= VERSION_PAL_BETA
		/*0*/ { L_DISH_125, L_DISH_126, L_DISH_127, L_DISH_128 }, // Joanna Dark
		/*1*/ { L_DISH_129, L_DISH_130, L_DISH_131, L_DISH_132 }, // Jonathan
		/*2*/ { L_DISH_133, L_DISH_134, L_DISH_135, L_DISH_136 }, // Daniel Carrington
		/*3*/ { L_DISH_137, L_DISH_138, L_DISH_139, L_DISH_140 }, // Cassandra De Vries
		/*4*/ { L_DISH_141, L_DISH_142, L_DISH_143, L_DISH_144 }, // Trent Easton
		/*5*/ { L_DISH_145, L_DISH_146, L_DISH_147, L_DISH_148 }, // Dr. Caroll
		/*6*/ { L_DISH_149, L_DISH_150, L_DISH_151, L_DISH_152 }, // Elvis
		/*7*/ { L_DISH_153, L_DISH_154, L_DISH_155, L_DISH_156 }, // Mr. Blonde
		/*8*/ { L_DISH_157, L_DISH_158, L_DISH_159, L_DISH_160 }, // Mr. Blonde (repeat)
		/*9*/ { L_DISH_161, L_DISH_162, L_DISH_163, L_DISH_164 }, // The U.S. President
#else
		/*0*/ { L_MISC_219, L_MISC_220, L_MISC_221, L_MISC_222 }, // Joanna Dark
		/*1*/ { L_MISC_223, L_MISC_224, L_MISC_225, L_MISC_226 }, // Jonathan
		/*2*/ { L_MISC_227, L_MISC_228, L_MISC_229, L_MISC_230 }, // Daniel Carrington
		/*3*/ { L_MISC_231, L_MISC_232, L_MISC_233, L_MISC_234 }, // Cassandra De Vries
		/*4*/ { L_MISC_235, L_MISC_236, L_MISC_237, L_MISC_238 }, // Trent Easton
		/*5*/ { L_MISC_239, L_MISC_240, L_MISC_241, L_MISC_242 }, // Dr. Caroll
		/*6*/ { L_MISC_243, L_MISC_244, L_MISC_245, L_MISC_246 }, // Elvis
		/*7*/ { L_MISC_247, L_MISC_248, L_MISC_249, L_MISC_250 }, // Mr. Blonde
		/*8*/ { L_MISC_251, L_MISC_252, L_MISC_253, L_MISC_254 }, // Mr. Blonde (repeat)
		/*9*/ { L_MISC_255, L_MISC_256, L_MISC_257, L_MISC_258 }, // The U.S. President
#endif
	};

	switch (bodynum) {
	case BODY_DARK_COMBAT:
		return &bios[0];
	case BODY_JONATHAN:
		return &bios[1];
	case BODY_CARRINGTON:
		return &bios[2];
	case BODY_CASSANDRA:
		return &bios[3];
	case BODY_TRENT:
		return &bios[4];
	case BODY_DRCAROLL:
		return &bios[5];
	case BODY_THEKING:
		return &bios[6];
	case BODY_MRBLONDE:
		if (ci_is_stage_complete(SOLOSTAGEINDEX_CRASHSITE)) {
			return &bios[8];
		}
		return &bios[7];
	case BODY_PRESIDENT:
		return &bios[9];
	}

	return NULL;
}

char *ci_get_chr_bio_description(void)
{
	struct chrbio *bio = ci_get_chr_bio_by_bodynum(ci_get_chr_bio_bodynum_by_slot(g_ChrBioSlot));
	return lang_get(bio->description);
}

s32 ci_get_num_unlocked_chr_bios(void)
{
	s32 count = 0;
	s32 bodynum;

	for (bodynum = 0; bodynum < ARRAYCOUNT(g_HeadsAndBodies) - 1; bodynum++) {
		if (ci_is_chr_bio_unlocked(bodynum)) {
			count++;
		}
	}

	return count;
}

s32 ci_get_chr_bio_bodynum_by_slot(s32 slot)
{
	s32 index = -1;
	s32 bodynum;

	for (bodynum = 0; bodynum < ARRAYCOUNT(g_HeadsAndBodies) - 1; bodynum++) {
		if (ci_is_chr_bio_unlocked(bodynum)) {
			index++;
		}

		if (index == slot) {
			return bodynum;
		}
	}

	return 0;
}

struct miscbio *ci_get_misc_bio(s32 index)
{
#ifdef AVOID_UB
	static
#endif
	struct miscbio bios[] = {
		// name, description
#if VERSION >= VERSION_PAL_BETA
		{ L_DISH_165, L_DISH_166 },
		{ L_DISH_167, L_DISH_168 },
		{ L_DISH_169, L_DISH_170 },
		{ L_DISH_171, L_DISH_172 },
#else
		{ L_MISC_259, L_MISC_260 },
		{ L_MISC_261, L_MISC_262 },
		{ L_MISC_263, L_MISC_264 },
		{ L_MISC_265, L_MISC_266 },
#endif
	};

	switch (index) {
	case MISCBIO_MAIANS:     return &bios[0];
	case MISCBIO_SKEDAR:     return &bios[1];
	case MISCBIO_BACKGROUND: return &bios[2];
	case MISCBIO_STORY:      return &bios[3];
	}

	return NULL;
}

bool ci_is_misc_bio_unlocked(s32 index)
{
	switch (index) {
	case MISCBIO_MAIANS:
		return ci_is_stage_complete(SOLOSTAGEINDEX_RESCUE);
	case MISCBIO_SKEDAR:
		return ci_is_stage_complete(SOLOSTAGEINDEX_ATTACKSHIP);
	case MISCBIO_BACKGROUND:
	case MISCBIO_STORY:
		return ci_is_stage_complete(SOLOSTAGEINDEX_MBR);
	}

	return false;
}

s32 ci_get_num_unlocked_misc_bios(void)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < 4; i++) {
		if (ci_is_misc_bio_unlocked(i)) {
			count++;
		}
	}

	return count;
}

s32 ci_get_misc_bio_index_by_slot(s32 slot)
{
	s32 index = -1;
	s32 i;

	for (i = 0; i < 4; i++) {
		if (ci_is_misc_bio_unlocked(i)) {
			index++;
		}

		if (index == slot) {
			return i;
		}
	}

	return 0;
}

char *ci_get_misc_bio_description(void)
{
	s32 index = ci_get_misc_bio_index_by_slot(g_ChrBioSlot - ci_get_num_unlocked_chr_bios());
	struct miscbio *bio = ci_get_misc_bio(index);

	return lang_get(bio->description);
}

bool ci_is_hangar_bio_a_vehicle(s32 index)
{
	return index >= HANGARBIO_JUMPSHIP;
}

u8 g_HangarBioSlot = 0;

struct hangarbio *ci_get_hangar_bio(s32 index)
{
#ifdef AVOID_UB
	static
#endif
	struct hangarbio bios[] = {
		// name, description
#if VERSION >= VERSION_PAL_BETA
		{ L_DISH_196, L_DISH_219 }, // Carrington Institute
		{ L_DISH_197, L_DISH_220 }, // Lucerne Tower
		{ L_DISH_198, L_DISH_221 }, // Laboratory Basement
		{ L_DISH_199, L_DISH_222 }, // Carrington Villa
		{ L_DISH_200, L_DISH_223 }, // Chicago
		{ L_DISH_201, L_DISH_224 }, // G5 Building
		{ L_DISH_202, L_DISH_225 }, // Area 51
		{ L_DISH_203, L_DISH_226 }, // Alaskan Air Base
		{ L_DISH_204, L_DISH_227 }, // Air Force One
		{ L_DISH_205, L_DISH_228 }, // Crash Site
		{ L_DISH_206, L_DISH_229 }, // Pelagic II
		{ L_DISH_207, L_DISH_230 }, // Cetan Ship
		{ L_DISH_208, L_DISH_231 }, // Skedar Assault Ship
		{ L_DISH_209, L_DISH_232 }, // Skedar Homeworld
		{ L_DISH_210, L_DISH_233 }, // Jumpship
		{ L_DISH_211, L_DISH_234 }, // HoverCrate
		{ L_DISH_212, L_DISH_235 }, // HoverBike
		{ L_DISH_213, L_DISH_236 }, // Cleaning Hovbot
		{ L_DISH_214, L_DISH_237 }, // Hovercopter
		{ L_DISH_215, L_DISH_238 }, // G5 Robot
		{ L_DISH_216, L_DISH_239 }, // A51 Interceptor
		{ L_DISH_217, L_DISH_240 }, // Maian Vessel
		{ L_DISH_218, L_DISH_241 }, // Skedar Shuttle
#else
		{ L_MISC_290, L_MISC_313 }, // Carrington Institute
		{ L_MISC_291, L_MISC_314 }, // Lucerne Tower
		{ L_MISC_292, L_MISC_315 }, // Laboratory Basement
		{ L_MISC_293, L_MISC_316 }, // Carrington Villa
		{ L_MISC_294, L_MISC_317 }, // Chicago
		{ L_MISC_295, L_MISC_318 }, // G5 Building
		{ L_MISC_296, L_MISC_319 }, // Area 51
		{ L_MISC_297, L_MISC_320 }, // Alaskan Air Base
		{ L_MISC_298, L_MISC_321 }, // Air Force One
		{ L_MISC_299, L_MISC_322 }, // Crash Site
		{ L_MISC_300, L_MISC_323 }, // Pelagic II
		{ L_MISC_301, L_MISC_324 }, // Cetan Ship
		{ L_MISC_302, L_MISC_325 }, // Skedar Assault Ship
		{ L_MISC_303, L_MISC_326 }, // Skedar Homeworld
		{ L_MISC_304, L_MISC_327 }, // Jumpship
		{ L_MISC_305, L_MISC_328 }, // HoverCrate
		{ L_MISC_306, L_MISC_329 }, // HoverBike
		{ L_MISC_307, L_MISC_330 }, // Cleaning Hovbot
		{ L_MISC_308, L_MISC_331 }, // Hovercopter
		{ L_MISC_309, L_MISC_332 }, // G5 Robot
		{ L_MISC_310, L_MISC_333 }, // A51 Interceptor
		{ L_MISC_311, L_MISC_334 }, // Maian Vessel
		{ L_MISC_312, L_MISC_335 }, // Skedar Shuttle
#endif
	};

	switch (index) {
	case HANGARBIO_INSTITUTE:      return &bios[0];
	case HANGARBIO_DDTOWER:        return &bios[1];
	case HANGARBIO_LABBASEMENT:    return &bios[2];
	case HANGARBIO_VILLA:          return &bios[3];
	case HANGARBIO_CHICAGO:        return &bios[4];
	case HANGARBIO_G5:             return &bios[5];
	case HANGARBIO_AREA51:         return &bios[6];
	case HANGARBIO_AIRBASE:        return &bios[7];
	case HANGARBIO_AIRFORCEONE:    return &bios[8];
	case HANGARBIO_CRASHSITE:      return &bios[9];
	case HANGARBIO_PELAGIC:        return &bios[10];
	case HANGARBIO_DEEPSEA:        return &bios[11];
	case HANGARBIO_ATTACKSHIP:     return &bios[12];
	case HANGARBIO_SKEDARRUINS:    return &bios[13];
	case HANGARBIO_JUMPSHIP:       return &bios[14];
	case HANGARBIO_HOVERCRATE:     return &bios[15];
	case HANGARBIO_HOVERBIKE:      return &bios[16];
	case HANGARBIO_HOVERBOT:       return &bios[17];
	case HANGARBIO_HOVERCOPTER:    return &bios[18];
	case HANGARBIO_G5ROBOT:        return &bios[19];
	case HANGARBIO_A51INTERCEPTOR: return &bios[20];
	case HANGARBIO_MAIANVESSEL:    return &bios[21];
	case HANGARBIO_SKEDARSHUTTLE:  return &bios[22];
	}

	return NULL;
}

bool ci_is_hangar_bio_unlocked(u32 bioindex)
{
	u32 stage;

	switch (bioindex) {
	case HANGARBIO_INSTITUTE:
	case HANGARBIO_HOVERCRATE:
		return true;
	case HANGARBIO_DDTOWER:
		stage = SOLOSTAGEINDEX_DEFECTION;
		break;
	case HANGARBIO_LABBASEMENT:
	case HANGARBIO_HOVERBOT:
		stage = SOLOSTAGEINDEX_INVESTIGATION;
		break;
	case HANGARBIO_HOVERCOPTER:
		stage = SOLOSTAGEINDEX_EXTRACTION;
		break;
	case HANGARBIO_VILLA:
	case HANGARBIO_JUMPSHIP:
		stage = SOLOSTAGEINDEX_VILLA;
		break;
	case HANGARBIO_CHICAGO:
		stage = SOLOSTAGEINDEX_CHICAGO;
		break;
	case HANGARBIO_G5:
	case HANGARBIO_G5ROBOT:
		stage = SOLOSTAGEINDEX_G5BUILDING;
		break;
	case HANGARBIO_AREA51:
	case HANGARBIO_HOVERBIKE:
	case HANGARBIO_A51INTERCEPTOR:
		stage = SOLOSTAGEINDEX_INFILTRATION;
		break;
	case HANGARBIO_AIRBASE:
		stage = SOLOSTAGEINDEX_AIRBASE;
		break;
	case HANGARBIO_AIRFORCEONE:
		stage = SOLOSTAGEINDEX_AIRFORCEONE;
		break;
	case HANGARBIO_CRASHSITE:
	case HANGARBIO_MAIANVESSEL:
		stage = SOLOSTAGEINDEX_CRASHSITE;
		break;
	case HANGARBIO_PELAGIC:
		stage = SOLOSTAGEINDEX_PELAGIC;
		break;
	case HANGARBIO_DEEPSEA:
		stage = SOLOSTAGEINDEX_DEEPSEA;
		break;
	case HANGARBIO_ATTACKSHIP:
	case HANGARBIO_SKEDARSHUTTLE:
		stage = SOLOSTAGEINDEX_DEFENSE;
		break;
	case HANGARBIO_SKEDARRUINS:
		stage = SOLOSTAGEINDEX_ATTACKSHIP;
		break;
	default:
		return false;
	}

	return ci_is_stage_complete(stage);
}

s32 ci_get_num_unlocked_location_bios(void)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < 23; i++) {
		if (ci_is_hangar_bio_a_vehicle(i)) {
			return count;
		}

		if (ci_is_hangar_bio_unlocked(i)) {
			count++;
		}
	}

	return count;
}

s32 ci_get_num_unlocked_hangar_bios(void)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < 23; i++) {
		if (ci_is_hangar_bio_unlocked(i)) {
			count++;
		}
	}

	return count;
}

s32 ci_get_hangar_bio_index_by_slot(s32 slot)
{
	s32 index = -1;
	s32 i;

	for (i = 0; i < 23; i++) {
		if (ci_is_hangar_bio_unlocked(i)) {
			index++;
		}

		if (index == slot) {
			return i;
		}
	}

	return 0;
}

char *ci_get_hangar_bio_description(void)
{
	struct hangarbio *bio = ci_get_hangar_bio(ci_get_hangar_bio_index_by_slot(g_HangarBioSlot));
	return lang_get(bio->description);
}

u8 g_DtSlot = 0;
u8 g_DtInitialised = 0;

struct trainingdata *dt_get_data(void)
{
	return &g_DtData;
}

void dt_restore_player(void)
{
	bgun_set_passive_mode(true);

	if (g_DtData.obj) {
		obj_free_permanently(g_DtData.obj, true);
	}

	g_DtData.obj = NULL;

	if (dt_get_weapon_by_device_index(dt_get_index_by_slot(g_DtSlot)) == WEAPON_ECMMINE) {
		bgun_set_ammo_quantity(AMMOTYPE_ECM_MINE, 0);
	}

	if (g_Vars.currentplayer->eyespy) {
		struct chrdata *chr = g_Vars.currentplayer->eyespy->prop->chr;
		g_Vars.currentplayer->eyespy->deployed = false;
		g_Vars.currentplayer->eyespy->held = true;
		g_Vars.currentplayer->eyespy->active = false;

		chr->chrflags |= CHRCFLAG_HIDDEN;

		ps_stop_sound(g_Vars.currentplayer->eyespy->prop, PSTYPE_GENERAL, 0xffff);

		g_Vars.currentplayer->devicesactive &= ~DEVICE_EYESPY;
	}
}

void dt_push_endscreen(void)
{
	if (g_DtData.completed) {
		menu_push_root_dialog_and_pause(&g_DtCompletedMenuDialog, MENUROOT_TRAINING);
	} else if (g_DtData.failed) {
		menu_push_root_dialog_and_pause(&g_DtFailedMenuDialog, MENUROOT_TRAINING);
	}

	g_DtData.timeleft = 0;
	g_DtData.completed = false;
	g_DtData.failed = false;
	g_DtData.finished = false;
	g_DtData.holographedpc = false;
}

void dt_tick(void)
{
	if (g_DtInitialised) {
		if (g_DtData.intraining) {
			g_DtData.timetaken += g_Vars.lvupdate60;

			if (g_Vars.currentplayer->isdead) {
				dt_end();
			}

			if (chr_has_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_DEVICE_FAILURE)) {
				dt_end();
				g_DtData.failed = true;
				g_DtData.timeleft = 1;
				g_DtData.finished = true;
			} else if (chr_has_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_DEVICE_SUCCESS)) {
				dt_end();
				g_DtData.completed = true;
				g_DtData.timeleft = 1;
				g_DtData.finished = true;
			}
		} else if (g_DtData.finished) {
			if (g_DtData.timeleft <= 0) {
				dt_push_endscreen();
			} else {
				g_DtData.timeleft -= g_Vars.lvupdate60;
			}
		}
	}
}

void dt_reset(void)
{
	if (g_DtInitialised == false) {
		g_DtInitialised = true;
		g_DtData.intraining = false;
		g_DtData.failed = false;
		g_DtData.completed = false;
		g_DtData.finished = false;
		g_DtData.timeleft = 0;
		g_DtData.holographedpc = false;
		g_DtData.timetaken = 0;
		g_DtData.obj = NULL;
		chr_unset_stage_flag(NULL, STAGEFLAG_CI_DEVICE_ABORTING);
		chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_DEVICE_SUCCESS);
		chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_DEVICE_FAILURE);
	}
}

void dt_begin(void)
{
	g_DtData.intraining = true;
	g_DtData.timetaken = 0;
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_DEVICE_ABORTING);
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_DEVICE_SUCCESS);
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_DEVICE_FAILURE);
	chr_set_stage_flag(NULL, ci_get_stage_flag_by_device_index(dt_get_index_by_slot(g_DtSlot)));
	g_Vars.currentplayer->training = true;
	bgun_set_passive_mode(false);
	chr_set_stage_flag(NULL, STAGEFLAG_CI_IN_TRAINING);
}

void dt_end(void)
{
	g_DtData.intraining = false;
	dt_restore_player();
	bgun_set_ammo_quantity(AMMOTYPE_CLOAK, 0);
	chr_set_stage_flag(NULL, STAGEFLAG_CI_DEVICE_ABORTING);
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_DEVICE_FAILURE);
	chr_unset_stage_flag(NULL, ci_get_stage_flag_by_device_index(dt_get_index_by_slot(g_DtSlot)));
	g_Vars.currentplayer->training = false;
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_IN_TRAINING);
	player_display_health();
	g_Vars.currentplayer->bondhealth = 1;
}

bool dt_is_available(s32 deviceindex)
{
	u8 flags[] = {
		GAMEFILEFLAG_CI_UPLINK_DONE,
		GAMEFILEFLAG_CI_ECMMINE_DONE,
		GAMEFILEFLAG_CI_CAMSPY_DONE,
		GAMEFILEFLAG_CI_NIGHTVISION_DONE,
		GAMEFILEFLAG_CI_DOORDECODER_DONE,
		GAMEFILEFLAG_CI_RTRACKER_DONE,
		GAMEFILEFLAG_CI_IR_DONE,
		GAMEFILEFLAG_CI_XRAY_DONE,
		GAMEFILEFLAG_CI_DISGUISE_DONE,
		GAMEFILEFLAG_CI_CLOAK_DONE,
	};

	deviceindex--;

	if (deviceindex >= ARRAYCOUNT(flags)) {
		return true;
	}

	if (deviceindex < 0 || gamefile_has_flag(flags[deviceindex])) {
		return true;
	}

	return false;
}

s32 dt_get_num_available(void)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < NUM_DEVICETESTS; i++) {
		if (dt_is_available(i)) {
			count++;
		}
	}

	return count;
}

s32 dt_get_index_by_slot(s32 wantindex)
{
	s32 index = -1;
	s32 i;

	for (i = 0; i < NUM_DEVICETESTS; i++) {
		if (dt_is_available(i)) {
			index++;
		}

		if (index == wantindex) {
			return i;
		}
	}

	return 0;
}

u32 dt_get_weapon_by_device_index(s32 deviceindex)
{
	u32 weapons[] = {
		WEAPON_DATAUPLINK,
		WEAPON_ECMMINE,
		WEAPON_EYESPY,
		WEAPON_NIGHTVISION,
		WEAPON_DOORDECODER,
		WEAPON_RTRACKER,
		WEAPON_IRSCANNER,
		WEAPON_XRAYSCANNER,
		WEAPON_DISGUISE41,
		WEAPON_CLOAKINGDEVICE,
	};

	return weapons[deviceindex];
}

u32 ci_get_stage_flag_by_device_index(u32 deviceindex)
{
	u32 flags[] = {
		STAGEFLAG_CI_TRIGGER_UPLINK,
		STAGEFLAG_CI_TRIGGER_ECMMINE,
		STAGEFLAG_CI_TRIGGER_CAMSPY,
		STAGEFLAG_CI_TRIGGER_NIGHTVISION,
		STAGEFLAG_CI_TRIGGER_DOORDECODER,
		STAGEFLAG_CI_TRIGGER_RTRACKER,
		STAGEFLAG_CI_TRIGGER_IR,
		STAGEFLAG_CI_TRIGGER_XRAY,
		STAGEFLAG_CI_TRIGGER_DISGUISE,
		STAGEFLAG_CI_TRIGGER_CLOAK,
	};

	return flags[deviceindex];
}

char *dt_get_description(void)
{
	u32 texts[] = {
#if VERSION >= VERSION_PAL_BETA
		/*0*/ L_DISH_186, // Data uplink
		/*1*/ L_DISH_185, // ECM mine
		/*2*/ L_DISH_177, // CamSpy
		/*3*/ L_DISH_178, // Night vision
		/*4*/ L_DISH_179, // Door decoder
		/*5*/ L_DISH_183, // R-tracker
		/*6*/ L_DISH_182, // IR scanner
		/*7*/ L_DISH_180, // X-ray scanner
		/*8*/ L_DISH_181, // Disguise
		/*9*/ L_DISH_184, // Cloak
#else
		/*0*/ L_MISC_280, // Data uplink
		/*1*/ L_MISC_279, // ECM mine
		/*2*/ L_MISC_271, // CamSpy
		/*3*/ L_MISC_272, // Night vision
		/*4*/ L_MISC_273, // Door decoder
		/*5*/ L_MISC_277, // R-tracker
		/*6*/ L_MISC_276, // IR scanner
		/*7*/ L_MISC_274, // X-ray scanner
		/*8*/ L_MISC_275, // Disguise
		/*9*/ L_MISC_278, // Cloak
#endif
	};

	return lang_get(texts[dt_get_index_by_slot(g_DtSlot)]);
}

char *dt_get_tip1(void)
{
	u32 texts[] = {
#if VERSION >= VERSION_PAL_BETA
		/*0*/ L_DISH_263,
		/*1*/ L_DISH_264,
		/*2*/ L_DISH_265,
		/*3*/ L_DISH_266,
		/*4*/ L_DISH_267,
		/*5*/ L_DISH_268,
		/*6*/ L_DISH_269,
		/*7*/ L_DISH_270,
		/*8*/ L_DISH_271,
		/*9*/ L_DISH_272,
#else
		/*0*/ L_MISC_357,
		/*1*/ L_MISC_358,
		/*2*/ L_MISC_359,
		/*3*/ L_MISC_360,
		/*4*/ L_MISC_361,
		/*5*/ L_MISC_362,
		/*6*/ L_MISC_363,
		/*7*/ L_MISC_364,
		/*8*/ L_MISC_365,
		/*9*/ L_MISC_366,
#endif
	};

	return lang_get(texts[dt_get_index_by_slot(g_DtSlot)]);
}

char *dt_get_tip2(void)
{
	u32 texts[] = {
#if VERSION >= VERSION_PAL_BETA
		/*0*/ L_DISH_273,
		/*1*/ L_DISH_274,
		/*2*/ L_DISH_275,
		/*3*/ L_DISH_276,
		/*4*/ L_DISH_277,
		/*5*/ L_DISH_278,
		/*6*/ L_DISH_279,
		/*7*/ L_DISH_280,
		/*8*/ L_DISH_281,
		/*9*/ L_DISH_282,
#else
		/*0*/ L_MISC_367,
		/*1*/ L_MISC_368,
		/*2*/ L_MISC_369,
		/*3*/ L_MISC_370,
		/*4*/ L_MISC_371,
		/*5*/ L_MISC_372,
		/*6*/ L_MISC_373,
		/*7*/ L_MISC_374,
		/*8*/ L_MISC_375,
		/*9*/ L_MISC_376,
#endif
	};

	return lang_get(texts[dt_get_index_by_slot(g_DtSlot)]);
}

u8 g_HtSlot = 0;
u8 g_HtInitialised = 0;

struct trainingdata *get_holo_training_data(void)
{
	return &g_HtData;
}

void ht_push_endscreen(void)
{
	if (g_HtData.completed) {
		menu_push_root_dialog_and_pause(&g_HtCompletedMenuDialog, MENUROOT_TRAINING);
	} else if (g_HtData.failed) {
		menu_push_root_dialog_and_pause(&g_HtFailedMenuDialog, MENUROOT_TRAINING);
	}

	g_HtData.timeleft = 0;
	g_HtData.completed = false;
	g_HtData.failed = false;
	g_HtData.finished = false;
}

void ht_tick(void)
{
	if (g_HtInitialised) {
		if (g_HtData.intraining) {
			g_HtData.timetaken += g_Vars.lvupdate60;

			if (g_Vars.currentplayer->isdead) {
				ht_end();
			}

			if (chr_has_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_HOLO_FAILURE)) {
				ht_end();
				g_HtData.failed = true;
				g_HtData.timeleft = 1;
				g_HtData.finished = true;
			} else if (chr_has_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_HOLO_SUCCESS)) {
				ht_end();
				g_HtData.completed = true;
				g_HtData.timeleft = 1;
				g_HtData.finished = true;
			}
		} else if (g_HtData.finished) {
			if (g_HtData.timeleft <= 0) {
				ht_push_endscreen();
			} else {
				g_HtData.timeleft -= g_Vars.lvupdate60;
			}
		}
	}
}

void ht_reset(void)
{
	if (g_HtInitialised == false) {
		g_HtInitialised = true;
		g_HtData.intraining = false;
		g_HtData.failed = false;
		g_HtData.completed = false;
		g_HtData.finished = false;
		g_HtData.timeleft = 0;
		g_HtData.timetaken = 0;
		chr_unset_stage_flag(NULL, STAGEFLAG_CI_HOLO_ABORTING);
		chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_HOLO_SUCCESS);
		chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_HOLO_FAILURE);
	}
}

void ht_begin(void)
{
	struct waypoint *waypoints = g_StageSetup.waypoints;

	g_HtData.intraining = true;
	g_HtData.timetaken = 0;
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_HOLO_ABORTING);
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_HOLO_SUCCESS);
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_HOLO_FAILURE);
	chr_set_stage_flag(NULL, ht_get_stageflag(ht_get_index_by_slot(g_HtSlot)));

	// Disable segment leading out of the door
	nav_disable_segment(&waypoints[0x20], &waypoints[0x31]);

	g_Vars.currentplayer->training = true;
	bgun_set_passive_mode(false);
	chr_set_stage_flag(NULL, STAGEFLAG_CI_IN_TRAINING);
}

void ht_end(void)
{
	struct prop *prop;
	s16 *propnum;
	s16 propnums[256];
	RoomNum rooms[] = {
		ROOM_DISH_HOLO1,
		ROOM_DISH_HOLO2,
		ROOM_DISH_HOLO3,
		ROOM_DISH_HOLO4,
		-1,
	};
	struct waypoint *waypoints = g_StageSetup.waypoints;

	g_HtData.intraining = false;
	chr_set_stage_flag(NULL, STAGEFLAG_CI_HOLO_ABORTING);
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_TRIGGER_HOLO_FAILURE);
	chr_unset_stage_flag(NULL, ht_get_stageflag(ht_get_index_by_slot(g_HtSlot)));

	// Enable segment leading out of the door
	nav_enable_segment(&waypoints[0x20], &waypoints[0x31]);

	g_Vars.currentplayer->training = false;
	room_get_props(rooms, propnums, 256);
	propnum = &propnums[0];

	// Remove dropped weapons
	while (*propnum >= 0) {
		prop = &g_Vars.props[*propnum];

		if (prop && prop->type == PROPTYPE_WEAPON) {
			struct defaultobj *obj = prop->obj;

			if (obj->type == OBJTYPE_WEAPON) {
				obj_free_permanently(obj, true);
			}
		}

		propnum++;
	}

	bgun_set_passive_mode(true);
	chr_unset_stage_flag(NULL, STAGEFLAG_CI_IN_TRAINING);
	player_display_health();
	g_Vars.currentplayer->bondhealth = 1;
}

bool ht_is_unlocked(u32 value)
{
	switch (value) {
	case 0:
	case 1:
	case 2:
	case 3:
	case 4:
	case 5:
	case 6:
		return true;
	}

	return false;
}

s32 ht_get_num_unlocked(void)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < NUM_HOLOTESTS; i++) {
		if (ht_is_unlocked(i)) {
			count++;
		}
	}

	return count;
}

s32 ht_get_index_by_slot(s32 slot)
{
	s32 index = -1;
	s32 i;

	for (i = 0; i < NUM_HOLOTESTS; i++) {
		if (ht_is_unlocked(i)) {
			index++;
		}

		if (index == slot) {
			return i;
		}
	}

	return 0;
}

char *ht_get_name(s32 index)
{
	u32 texts[] = {
#if VERSION >= VERSION_PAL_BETA
		L_DISH_316, // "Holo 1 - Looking Around"
		L_DISH_317, // "Holo 2 - Movement 1"
		L_DISH_318, // "Holo 3 - Movement 2"
		L_DISH_319, // "Holo 4 - Unarmed Combat 1"
		L_DISH_320, // "Holo 5 - Unarmed Combat 2"
		L_DISH_321, // "Holo 6 - Live Combat 1"
		L_DISH_322, // "Holo 7 - Live Combat 2"
#else
		L_MISC_410, // "Holo 1 - Looking Around"
		L_MISC_411, // "Holo 2 - Movement 1"
		L_MISC_412, // "Holo 3 - Movement 2"
		L_MISC_413, // "Holo 4 - Unarmed Combat 1"
		L_MISC_414, // "Holo 5 - Unarmed Combat 2"
		L_MISC_415, // "Holo 6 - Live Combat 1"
		L_MISC_416, // "Holo 7 - Live Combat 2"
#endif
	};

	return lang_get(texts[index]);
}

u32 ht_get_stageflag(s32 index)
{
	u32 flags[] = {
		STAGEFLAG_CI_IN_HOLO1,
		STAGEFLAG_CI_IN_HOLO2,
		STAGEFLAG_CI_IN_HOLO3,
		STAGEFLAG_CI_IN_HOLO4,
		STAGEFLAG_CI_IN_HOLO5,
		STAGEFLAG_CI_IN_HOLO6,
		STAGEFLAG_CI_IN_HOLO7,
		STAGEFLAG_CI_GENERAL_PURPOSE,
	};

	return flags[index];
}

char *ht_get_description(void)
{
	u32 texts[] = {
#if VERSION >= VERSION_PAL_BETA
		L_DISH_242,
		L_DISH_243,
		L_DISH_244,
		L_DISH_245,
		L_DISH_246,
		L_DISH_247,
		L_DISH_248,
#else
		L_MISC_336,
		L_MISC_337,
		L_MISC_338,
		L_MISC_339,
		L_MISC_340,
		L_MISC_341,
		L_MISC_342,
#endif
	};

	return lang_get(texts[ht_get_index_by_slot(g_HtSlot)]);
}

char *ht_get_tip1(void)
{
	u32 texts[] = {
#if VERSION >= VERSION_PAL_BETA
		L_DISH_249, // "For greater precision..."
		L_DISH_250, // "Think about where you want to go..."
		L_DISH_251, // "Ducking enables you to..."
		L_DISH_252, // "Attacking opponents from behind..."
		L_DISH_253, // "Only stay close long enough..."
		L_DISH_254, // "Don't hang around and wait..."
		L_DISH_255, // "Go for the armed opponents..."
#else
		L_MISC_343, // "For greater precision..."
		L_MISC_344, // "Think about where you want to go..."
		L_MISC_345, // "Ducking enables you to..."
		L_MISC_346, // "Attacking opponents from behind..."
		L_MISC_347, // "Only stay close long enough..."
		L_MISC_348, // "Don't hang around and wait..."
		L_MISC_349, // "Go for the armed opponents..."
#endif
	};

	return lang_get(texts[ht_get_index_by_slot(g_HtSlot)]);
}

char *ht_get_tip2(void)
{
	u32 texts[] = {
#if VERSION >= VERSION_PAL_BETA
		L_DISH_256, // "For greater precision..."
		L_DISH_257, // "Sidestepping and strafing..."
		L_DISH_258, // "Ducking enables you to..."
		L_DISH_259, // "Attacking opponents from behind..."
		L_DISH_260, // "Only stay close long enough..."
		L_DISH_261, // "Don't hang around and wait..."
		L_DISH_262, // "Go for the armed opponents..."
#else
		L_MISC_350, // "For greater precision..."
		L_MISC_351, // "Sidestepping and strafing..."
		L_MISC_352, // "Ducking enables you to..."
		L_MISC_353, // "Attacking opponents from behind..."
		L_MISC_354, // "Only stay close long enough..."
		L_MISC_355, // "Don't hang around and wait..."
		L_MISC_356, // "Go for the armed opponents..."
#endif
	};

	return lang_get(texts[ht_get_index_by_slot(g_HtSlot)]);
}

#if VERSION >= VERSION_JPN_FINAL
void fr_get_goal_targets_text(char *buffer, char *buffer2)
{
	sprintf(buffer, "%s", lang_get(L_MISC_417));
	sprintf(buffer2, "%d\n", g_FrData.goaltargets);
}
#else
void fr_get_goal_targets_text(char *buffer)
{
	// "GOAL TARGETS:"
	sprintf(buffer, "%s %d\n", lang_get(L_MISC_417), g_FrData.goaltargets);
}
#endif

void fr_get_targets_destroyed_value(char *buffer)
{
	sprintf(buffer, "%02d\n", g_FrData.targetsdestroyed);
}

void fr_get_score_value(char *buffer)
{
	sprintf(buffer, "%03d\n", g_FrData.score);
}

#if VERSION >= VERSION_JPN_FINAL
void fr_get_goal_score_text(char *buffer1, char *buffer2)
{
	if (g_FrData.goalscore) {
		sprintf(buffer1, "%s", lang_get(L_MISC_418));
		sprintf(buffer2, "%d\n", g_FrData.goalscore);
	} else {
		sprintf(buffer1, "");
		sprintf(buffer2, "");
	}
}
#else
void fr_get_goal_score_text(char *buffer)
{
	if (g_FrData.goalscore) {
		// "GOAL SCORE:"
		sprintf(buffer, "%s %d\n", lang_get(L_MISC_418), g_FrData.goalscore);
	} else {
		sprintf(buffer, "");
	}
}
#endif

f32 fr_get_accuracy(char *buffer)
{
	f32 sum = (g_FrData.numhitsring3
		+ g_FrData.numhitsbullseye
		+ g_FrData.numhitsring1
		+ g_FrData.numhitsring2) * 100.0f;
	f32 accuracy = 100.0f;

	if (g_FrData.numshots) {
		accuracy = sum / (f32)g_FrData.numshots;
	}

	if (accuracy > 100.0f) {
		accuracy = 100.0f;
	}

	sprintf(buffer, "%s%s%.2f%%\n", "", "", accuracy);

	return accuracy;
}

#if VERSION >= VERSION_JPN_FINAL
bool fr_get_min_accuracy(char *buffer1, f32 accuracy, char *buffer2)
{
	sprintf(buffer1, "%s", lang_get(L_MISC_419));
	sprintf(buffer2, "%d%%\n", g_FrData.goalaccuracy);

	return accuracy < g_FrData.goalaccuracy;
}
#else
bool fr_get_min_accuracy(char *buffer, f32 accuracy)
{
	// "MIN ACCURACY:"
	sprintf(buffer, "%s %d%%\n", lang_get(L_MISC_419), g_FrData.goalaccuracy);

	return accuracy < g_FrData.goalaccuracy;
}
#endif

/**
 * Formats either the time taken or time limit into buffer, and returns true if
 * the time should induce a failure.
 *
 * The time limit will be used if it exists and the time take exceeds it,
 * otherwise time taken will be used.
 *
 * Negative time taken (such as when the player aborts before the challenge
 * starts) is wrapped to positive and will induce a failure.
 */
bool fr_format_time(char *buffer)
{
	s32 mins = 0;
	s32 mult = 1;
	f32 secs = g_FrData.timetaken / TICKS(60.0f);
	u8 failed = false;

	if (g_FrData.timelimit != 255 && secs >= g_FrData.timelimit) {
		failed = true;
		secs = g_FrData.timelimit;
	} else if (g_FrData.timetaken < 0) {
		failed = true;
	}

	if (secs < 0) {
		mult = -1;
		secs = -secs;
	}

	if (secs >= 60) {
		while (secs >= 60) {
			secs -= 60;
			mins++;
		}
	}

	sprintf(buffer, "%02d:%02d\n", mult * mins, (s32)secs);

	return failed;
}

#if VERSION >= VERSION_JPN_FINAL
bool fr_get_hud_middle_subtext(char *buffer1, char *buffer2)
{
	s32 secs;
	s32 mins;

	sprintf(buffer2, "");

	if (g_FrData.timetaken < TICKS(-180)) {
		sprintf(buffer1, "%s", lang_get(L_MISC_420)); // "FIRE TO START"
		return false;
	}

	if (g_FrData.timetaken < 0) {
		sprintf(buffer1, "%s", lang_get(L_MISC_421)); // "GET READY!"
		return true;
	}

	if (g_FrData.timelimit == 255) {
		return false;
	}

	secs = g_FrData.timelimit;
	mins = 0;

	if (secs >= 60) {
		while (secs >= 60) {
			secs -= 60;
			mins++;
		}
	}

	sprintf(buffer1, "%s", lang_get(L_MISC_422)); // "LIMIT:"
	sprintf(buffer2, "%02d:%02d\n", mins, secs);
	return true;
}
#else
bool fr_get_hud_middle_subtext(char *buffer)
{
	s32 secs;
	s32 mins;

	if (g_FrData.timetaken < TICKS(-180)) {
		sprintf(buffer, "%s", lang_get(L_MISC_420)); // "FIRE TO START"
		return false;
	}

	if (g_FrData.timetaken < 0) {
		sprintf(buffer, "%s", lang_get(L_MISC_421)); // "GET READY!"
		return true;
	}

	if (g_FrData.timelimit == 255) {
		return false;
	}

	secs = g_FrData.timelimit;
	mins = 0;

	if (secs >= 60) {
		while (secs >= 60) {
			secs -= 60;
			mins++;
		}
	}

	sprintf(buffer, "%s %02d:%02d\n", lang_get(L_MISC_422), mins, secs); // "LIMIT:"
	return true;
}
#endif

#if VERSION >= VERSION_JPN_FINAL
bool fr_get_feedback(char *scorebuffer, char *zonebuffer, char *extrabuffer)
{
	u32 texts[] = {
		L_MISC_423, // "ZONE 3"
		L_MISC_424, // "ZONE 2"
		L_MISC_425, // "ZONE 1"
		L_MISC_426, // "BULL'S-EYE"
		L_MISC_427, // "EXPLODED"
	};

	sprintf(extrabuffer, "");

	if (g_FrData.feedbackzone) {
		g_FrData.feedbackttl -= g_Vars.lvupdate60;

		if (g_FrData.feedbackttl <= 0) {
			g_FrData.feedbackzone = 0;
			g_FrData.feedbackttl = 0;
			return false;
		}

		if (g_FrData.feedbackzone == FRZONE_EXPLODE) {
			sprintf(scorebuffer, "010\n");
		} else {
			sprintf(scorebuffer, "%03d\n", g_FrData.feedbackzone);
		}

		switch (g_FrData.feedbackzone) {
		case FRZONE_RING3:
			sprintf(zonebuffer, "%s", lang_get(texts[0]));
			return true;
		case FRZONE_RING2:
			sprintf(zonebuffer, "%s", lang_get(texts[1]));
			return true;
		case FRZONE_RING1:
			sprintf(zonebuffer, "%s", lang_get(texts[2]));
			return true;
		case FRZONE_BULLSEYE:
			sprintf(zonebuffer, "%s", lang_get(texts[3]));
			return true;
		case FRZONE_EXPLODE:
			sprintf(zonebuffer, "%s", lang_get(texts[4]));
			return true;
		}

		sprintf(zonebuffer, "\n");
		return true;
	}

	return false;
}
#else
bool fr_get_feedback(char *scorebuffer, char *zonebuffer)
{
	u32 texts[] = {
		L_MISC_423, // "ZONE 3"
		L_MISC_424, // "ZONE 2"
		L_MISC_425, // "ZONE 1"
		L_MISC_426, // "BULL'S-EYE"
		L_MISC_427, // "EXPLODED"
	};

	if (g_FrData.feedbackzone) {
		g_FrData.feedbackttl -= g_Vars.lvupdate60;

		if (g_FrData.feedbackttl <= 0) {
			g_FrData.feedbackzone = 0;
			g_FrData.feedbackttl = 0;
			return false;
		}

		if (g_FrData.feedbackzone == FRZONE_EXPLODE) {
			sprintf(scorebuffer, "010\n");
		} else {
			sprintf(scorebuffer, "%03d\n", g_FrData.feedbackzone);
		}

		switch (g_FrData.feedbackzone) {
		case FRZONE_RING3:
			sprintf(zonebuffer, "%s", lang_get(texts[0]));
			return true;
		case FRZONE_RING2:
			sprintf(zonebuffer, "%s", lang_get(texts[1]));
			return true;
		case FRZONE_RING1:
			sprintf(zonebuffer, "%s", lang_get(texts[2]));
			return true;
		case FRZONE_BULLSEYE:
			sprintf(zonebuffer, "%s", lang_get(texts[3]));
			return true;
		case FRZONE_EXPLODE:
			sprintf(zonebuffer, "%s", lang_get(texts[4]));
			return true;
		}

		sprintf(zonebuffer, "\n");
		return true;
	}

	return false;
}
#endif

#if VERSION >= VERSION_JPN_FINAL
Gfx *fr_render_hud_element(Gfx *gdl, s32 x, s32 y, char *string1, char *string2, char *string3, u32 colour, u8 alpha)
#else
Gfx *fr_render_hud_element(Gfx *gdl, s32 x, s32 y, char *string1, char *string2, u32 colour, u8 alpha)
#endif
{
	s32 textheight;
	s32 textwidth;
	s32 x2;
	s32 y2;

	u32 halfalpha = alpha >> 1;
	u32 fullcolour = (colour & 0xffffff00) | alpha;

	text_measure(&textheight, &textwidth, string1, g_CharsHandelGothicMd, g_FontHandelGothicMd, 0);

	x2 = x - (textwidth >> 1);
	y2 = y;
	gdl = text_draw_black_box(gdl, &x2, &y2, &textwidth, &textheight);

	gdl = text_render_vx(gdl, &x2, &y2, string1,
			g_CharsHandelGothicMd, g_FontHandelGothicMd, fullcolour, halfalpha, vi_get_width(), vi_get_height(), 0, 0);

	if (string2) {
#if VERSION >= VERSION_JPN_FINAL
		s32 textheight2;
		s32 textwidth2;
		s32 textheight3;
		s32 textwidth3;

		text_measure(&textheight2, &textwidth2, string2, g_CharsHandelGothicSm, g_FontHandelGothicSm, 0);
		text_measure(&textheight3, &textwidth3, string3, g_CharsHandelGothicSm, g_FontHandelGothicSm, 0);

		textheight = textheight2;
		textwidth = textwidth2 + textwidth3;
		x2 = x - (textwidth >> 1);
		y2 = y;
		y2 += 17;

		gdl = text_draw_black_box(gdl, &x2, &y2, &textwidth, &textheight);

		gdl = text_render_vx(gdl, &x2, &y2, string2,
			g_CharsHandelGothicSm, g_FontHandelGothicSm, fullcolour, halfalpha, vi_get_width(), vi_get_height(), 0, 0);

		y2 = y;
		y2 += 17;
		y2++;
		x2 -= 4;

		gdl = text_render_vx(gdl, &x2, &y2, string3,
			g_CharsHandelGothicSm, g_FontHandelGothicSm, fullcolour, halfalpha, vi_get_width(), vi_get_height(), 0, 0);
#else
		text_measure(&textheight, &textwidth, string2, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);

		x2 = x - (textwidth >> 1);
		y2 = y + 17;
		gdl = text_draw_black_box(gdl, &x2, &y2, &textwidth, &textheight);

		gdl = text_render_vx(gdl, &x2, &y2, string2,
			g_CharsHandelGothicXs, g_FontHandelGothicXs, fullcolour, halfalpha, vi_get_width(), vi_get_height(), 0, 0);
#endif
	}

	return gdl;
}

#if VERSION >= VERSION_JPN_FINAL
Gfx *fr_render_hud(Gfx *gdl)
{
	char string1[128];
	char string2[128];
	char string3[128];
	bool red;
	bool exists;
	s32 alpha = 0xa0;
	f32 mult;

	if (vi_get_view_width() > 400) {
		mult = 1.7f;
	} else {
		mult = 1;
	}

	if (!g_FrIsValidWeapon && g_FrData.menucountdown <= 0) {
		return gdl;
	}

	if (g_FrData.menucountdown != 0) {
		alpha = (f32)(g_FrData.menucountdown * 160) / TICKS(60.0f);
	}

	gdl = text_begin(gdl);

	// Time
	red = fr_format_time(string1);
	exists = fr_get_hud_middle_subtext(string2, string3);

	gdl = fr_render_hud_element(gdl, vi_get_view_width() >> 1, vi_get_view_top() + 12,
			string1,
			exists ? string2 : NULL,
			exists ? string3 : NULL,
			red ? 0xff4444ff : 0x00ff00a0,
			alpha);

	// Score
	fr_get_score_value(string1);
	fr_get_goal_score_text(string2, string3);
	gdl = fr_render_hud_element(gdl, vi_get_view_left() + 65.0f * mult, vi_get_view_top() + 12,
			string1, string2, string3, 0x00ff00a0, alpha);

	// Feedback
	if (fr_get_feedback(string1, string2, string3)) {
		gdl = fr_render_hud_element(gdl,vi_get_view_left() + 65.0f * mult, vi_get_view_top() + 48,
				string1, string2, string3, 0x00ff00a0, alpha);
	}

	if (g_FrData.goalaccuracy > 0) {
		red = fr_get_min_accuracy(string2, fr_get_accuracy(string1), string3);

		gdl = fr_render_hud_element(gdl, vi_get_view_left() + vi_get_view_width() - 70.0f * mult, vi_get_view_top() + 12,
				string1, string2, string3,
				red ? 0xff4444ff : 0x00ff00a0,
				alpha);
	} else if (g_FrData.goaltargets != 255) {
		fr_get_targets_destroyed_value(string1);
		fr_get_goal_targets_text(string2, string3);

		if (mult == 2) {
			mult = 2.4;
		}

		gdl = fr_render_hud_element(gdl, vi_get_view_left() + vi_get_view_width() - 70.0f * mult, vi_get_view_top() + 12,
				string1, string2, string3, 0x00ff00a0, alpha);
	}

	return text_end(gdl);
}
#else
Gfx *fr_render_hud(Gfx *gdl)
{
	char string1[128];
	char string2[128];
	bool red;
	bool exists;
	s32 alpha = 0xa0;
	f32 mult;

	if (vi_get_view_width() > (VERSION >= VERSION_PAL_FINAL ? 330 : 400)) {
		mult = VERSION >= VERSION_PAL_FINAL ? 1.5f : 2;
	} else {
		mult = 1;
	}

	if (!g_FrIsValidWeapon && g_FrData.menucountdown <= 0) {
		return gdl;
	}

	if (g_FrData.menucountdown != 0) {
		alpha = (f32)(g_FrData.menucountdown * 160) / TICKS(60.0f);
	}

	gdl = text_begin(gdl);

	// Time
	red = fr_format_time(string1);
	exists = fr_get_hud_middle_subtext(string2);

	gdl = fr_render_hud_element(gdl, vi_get_view_width() >> 1, vi_get_view_top() + 12,
			string1, exists ? string2 : NULL,
			red ? 0xff0000a0 : 0x00ff00a0,
			alpha);

	// Score
	fr_get_score_value(string1);
	fr_get_goal_score_text(string2);
	gdl = fr_render_hud_element(gdl, vi_get_view_left() + 65.0f * mult, vi_get_view_top() + 12,
			string1, string2, 0x00ff00a0, alpha);

	// Feedback
	if (fr_get_feedback(string1, string2)) {
		gdl = fr_render_hud_element(gdl,vi_get_view_left() + 65.0f * mult, vi_get_view_top() + 40,
				string1, string2, 0x00ff00a0, alpha);
	}

	if (g_FrData.goalaccuracy > 0) {
		red = fr_get_min_accuracy(string2, fr_get_accuracy(string1));

		gdl = fr_render_hud_element(gdl, vi_get_view_left() + vi_get_view_width() - 70.0f * mult, vi_get_view_top() + 12,
				string1, string2,
				red ? 0xff0000a0 : 0x00ff00a0,
				alpha);
	} else if (g_FrData.goaltargets != 255) {
		fr_get_targets_destroyed_value(string1);
		fr_get_goal_targets_text(string2);

		if (mult == 2) {
			mult = 2.4;
		}

		gdl = fr_render_hud_element(gdl, vi_get_view_left() + vi_get_view_width() - 70.0f * mult, vi_get_view_top() + 12,
				string1, string2, 0x00ff00a0, alpha);
	}

	return text_end(gdl);
}
#endif
