#include <ultra64.h>
#include "constants.h"
#include "game/chraction.h"
#include "game/game_006900.h"
#include "game/bondgun.h"
#include "game/gset.h"
#include "game/tex.h"
#include "game/savebuffer.h"
#include "game/menu.h"
#include "game/inv.h"
#include "game/text.h"
#include "game/file.h"
#include "game/bot.h"
#include "game/botcmd.h"
#include "game/gfxmemory.h"
#include "game/training.h"
#include "game/lang.h"
#include "game/mplayer/mplayer.h"
#include "game/options.h"
#include "bss.h"
#include "lib/vi.h"
#include "lib/main.h"
#include "lib/mtx.h"
#include "lib/ailist.h"
#include "lib/str.h"
#include "data.h"
#include "types.h"

struct activemenu g_AmMenus[MAX_PLAYERS];
struct fontchar *g_AmFont1;
struct font *g_AmFont2;
s32 g_AmIndex;

struct menudialogdef g_AmPickTargetMenuDialog;

/**
 * This is a map of weapon numbers (as per the weapon set) to active menu slots.
 * For the purpose of this array, the AM slots are:
 *
 * 0-2 = top left to top right
 * 3,4 = left, right
 * 5-7 = bottom left to bottom right
 *
 * The values in the array are the slot numbers where that weapon will go.
 * For example, the value at index 2 is 6 which means weapon #2 from the
 * weapon set will go into slot 6 which is the bottom slot.
 */
const u8 g_AmMapping[] = {
	0, // unarmed
	1, // weapon #1
	6, // weapon #2
	3, // weapon #3
	4, // weapon #4
	7, // weapon #5
	5, // weapon #6
	2, // unused
};

struct chrdata *current_player_get_commanding_aibot(void)
{
	return g_Vars.currentplayer->commandingaibot;
}

void am_open_pick_target(void)
{
	u32 prevplayernum = g_MpPlayerNum;

	if (!mp_is_paused()) {
		g_AmMenus[g_AmIndex].prevallbots = g_AmMenus[g_AmIndex].allbots;
		g_Vars.currentplayer->activemenumode = AMMODE_CLOSED;
		g_MpPlayerNum = g_Vars.currentplayerstats->mpindex;
		menu_push_root_dialog(&g_AmPickTargetMenuDialog, MENUROOT_PICKTARGET);
		g_MpPlayerNum = prevplayernum;
	}
}

MenuItemHandlerResult am_pick_target_menu_list(s32 operation, struct menuitem *item, union handlerdata *data)
{
	static u32 teamcolours[] = {
		0xff666600,
		0xffff0000,
		0x4444ff00,
		0xff00ff00,
		0x00ffff00,
		0xff885500,
		0x8800ff00,
		0x88445500,
	};

	switch (operation) {
	case MENUOP_GET_OPTION_COUNT:
		if (g_AmMenus[g_AmIndex].prevallbots) {
			// All bots: num bot opponents + all human players
			s32 count = 0;
			s32 i;
			struct chrdata *playerchr = g_Vars.currentplayer->prop->chr;

			for (i = PLAYERCOUNT(); i < g_MpNumChrs; i++) {
				if (g_MpAllChrPtrs[i]->team != playerchr->team) {
					count++;
				}
			}

			count += PLAYERCOUNT();
			data->list.value = count;
		} else {
			// Single bot: All except the bot itself
			data->list.value = g_MpNumChrs - 1;
		}
		break;
	case MENUOP_CONFIRM:
		{
			s32 numremaining;
			s32 chrindex;
			struct chrdata *botchr;
			struct chrdata *playerchr;
			s32 i;

			chrindex = -1;
			numremaining = data->list.value;
			botchr = g_MpAllChrPtrs[g_Vars.currentplayer->aibuddynums[g_AmMenus[g_AmIndex].screenindex - 2]];
			playerchr = g_Vars.currentplayer->prop->chr;

			do {
				chrindex++;

				if (g_AmMenus[g_AmIndex].prevallbots) {
					if (playerchr == g_MpAllChrPtrs[chrindex] || playerchr->team != g_MpAllChrPtrs[chrindex]->team) {
						numremaining--;
					}
				} else {
					if (botchr != g_MpAllChrPtrs[chrindex]) {
						numremaining--;
					}
				}
			} while (numremaining >= 0);

			if (g_AmMenus[g_AmIndex].prevallbots) {
				g_AmMenus[g_AmIndex].prevallbots = false;

				for (i = 0; i < g_Vars.currentplayer->numaibuddies; i++) {
					bot_apply_attack(g_MpAllChrPtrs[g_Vars.currentplayer->aibuddynums[i]], g_MpAllChrPtrs[chrindex]->prop);
				}
			} else {
				bot_apply_attack(botchr, g_MpAllChrPtrs[chrindex]->prop);
			}

			menu_pop_dialog();
		}
		break;
	case MENUOP_GET_SELECTED_INDEX:
		data->list.value = 0xfffff;
		break;
	case MENUOP_RENDER:
		{
			Gfx *gdl = data->type19.gdl;
			struct menuitemrenderdata *renderdata = data->type19.renderdata2;
			s32 x;
			s32 y;
			u32 colour;
			s32 numremaining = (s32)data->type19.unk04;
			s32 chrindex = -1;
			struct chrdata *botchr = g_MpAllChrPtrs[g_Vars.currentplayer->aibuddynums[g_AmMenus[g_AmIndex].screenindex - 2]];
			struct chrdata *playerchr = g_Vars.currentplayer->prop->chr;

			do {
				chrindex++;

				if (g_AmMenus[g_AmIndex].prevallbots) {
					if (playerchr == g_MpAllChrPtrs[chrindex] || playerchr->team != g_MpAllChrPtrs[chrindex]->team) {
						numremaining--;
					}
				} else {
					if (botchr != g_MpAllChrPtrs[chrindex]) {
						numremaining--;
					}
				}
			} while (numremaining >= 0);

			colour = teamcolours[g_MpAllChrConfigPtrs[chrindex]->team] | (renderdata->colour & 0xff);

			if (renderdata->unk10) {
				u32 weight = menu_get_sin_osc_frac(40) * 255;
				colour = colour_blend(renderdata->colour | 0xffffff00, colour_blend(colour, colour & 0xff, 0x7f), weight);
			}

			x = renderdata->x + 10;
			y = renderdata->y + 1;

			gdl = text_begin(gdl);
			gdl = text_render_v2(gdl, &x, &y, g_MpAllChrConfigPtrs[chrindex]->name, g_CharsHandelGothicSm, g_FontHandelGothicSm, colour, vi_get_width(), vi_get_height(), 0, 0);
			gdl = text_end(gdl);
			return (s32)gdl;
		}
	case MENUOP_GET_OPTION_HEIGHT:
		data->list.value = LINEHEIGHT;
		break;
	}

	return 0;
}

MenuDialogHandlerResult am_pick_target_menu_dialog(s32 operation, struct menudialogdef *dialogdef, union handlerdata *data)
{
	switch (operation) {
	case MENUOP_ON_OPEN:
		g_PlayersWithControl[g_Vars.currentplayernum] = false;
		break;
	case MENUOP_ON_TICK:
		g_PlayersWithControl[g_Vars.currentplayernum] = false;
		break;
	case MENUOP_ON_CLOSE:
		g_PlayersWithControl[g_Vars.currentplayernum] = true;
		break;
	}

	return false;
}

struct menuitem g_AmPickTargetMenuItems[] = {
	{
		MENUITEMTYPE_LIST,
		0,
		MENUITEMFLAG_LIST_CUSTOMRENDER,
		0x0000005a,
		0,
		am_pick_target_menu_list,
	},
	{ MENUITEMTYPE_END },
};

struct menudialogdef g_AmPickTargetMenuDialog = {
	MENUDIALOGTYPE_DANGER,
	L_OPTIONS_492, // "Pick Target"
	g_AmPickTargetMenuItems,
	am_pick_target_menu_dialog,
	0,
	NULL,
};

void am_set_ai_buddy_temperament(bool aggressive)
{
	s32 i;

	for (i = 0; i < g_Vars.numaibuddies; i++) {
		if (g_Vars.aibuddies[i]) {
			struct chrdata *chr = g_Vars.aibuddies[i]->chr;

			if (chr) {
				if (aggressive) {
					chr->hidden &= ~CHRHFLAG_PASSIVE;
				} else {
					chr->hidden |= CHRHFLAG_PASSIVE;
				}
			}
		}
	}
}

#if VERSION >= VERSION_NTSC_1_0
void am_set_ai_buddy_stealth(void)
{
	s32 i;

	for (i = 0; i < g_Vars.numaibuddies; i++) {
		if (g_Vars.aibuddies[i]) {
			struct chrdata *chr = g_Vars.aibuddies[i]->chr;

			if (chr && chr->prop
					&& !chr_is_dead(chr)
					&& chr->ailist != ailist_find_by_id(GAILIST_BUDDY_STEALTH)
					&& chr->actiontype != ACT_DRUGGEDDROP
					&& chr->actiontype != ACT_DRUGGEDKO
					&& chr->actiontype != ACT_DRUGGEDCOMINGUP) {
				chr_stop_firing(chr);
				chr->ailist = ailist_find_by_id(GAILIST_BUDDY_STEALTH);
				chr->aioffset = 0;
			}
		}
	}
}
#endif

s32 am_get_first_buddy_index(void)
{
	s32 i;

	for (i = 0; i < g_Vars.numaibuddies; i++) {
		if (g_Vars.aibuddies[i]) {
			struct chrdata *chr = g_Vars.aibuddies[i]->chr;

			if (chr && chr->actiontype != ACT_DIE
					&& chr->actiontype != ACT_DEAD
					&& chr->prop
					&& chr->model) {
				return i;
			}
		}
	}

	return -1;
}

void am_apply(s32 slot)
{
	s32 numinvitems;
	s32 invindex;
	bool pass;
	s32 state;
	s32 weaponnum;
	s32 i;

	switch (g_AmMenus[g_AmIndex].screenindex) {
	case 0: // Weapon
		if (slot > 4) {
			slot--;
		}

		invindex = g_AmMenus[g_AmIndex].invindexes[slot];
		numinvitems = inv_get_count();

		if (invindex < numinvitems) {
			weaponnum = inv_get_weapon_num_by_index(invindex);
			pass = true;

			if (weaponnum) {
				state = gset_get_device_state(weaponnum);

				if (state != DEVICESTATE_UNEQUIPPED) {
					pass = false;

					if (state == DEVICESTATE_INACTIVE) {
						gset_set_device_active(weaponnum, true);
					} else {
						gset_set_device_active(weaponnum, false);
					}
				}
			}

			if (pass) {
				pass = true;

				if (g_FrIsValidWeapon) {
					s32 weaponnum = fr_get_weapon_by_slot(fr_get_slot());

					if (g_Vars.currentplayer->hands[HAND_RIGHT].gset.weaponnum == weaponnum) {
						pass = false;
					}
				}

				if (pass) {
					inv_set_current_index(invindex);

					if (inv_has_double_weapon_inc_all_guns(weaponnum, weaponnum)) {
						if (bgun_get_weapon_num(HAND_RIGHT) != weaponnum) {
							bgun_equip_weapon2(HAND_RIGHT, weaponnum);
						}

						if (bgun_get_weapon_num(HAND_LEFT) != weaponnum) {
							bgun_equip_weapon2(HAND_LEFT, weaponnum);
						}
					} else {
						if (bgun_get_weapon_num(HAND_RIGHT) != weaponnum) {
							bgun_equip_weapon2(HAND_RIGHT, weaponnum);
						}

						if (bgun_get_weapon_num(HAND_LEFT) != WEAPON_NONE) {
							bgun_equip_weapon2(HAND_LEFT, WEAPON_NONE);
						}
					}
				}
			}
		}
		break;
	case 1: // Function
		if (g_Vars.currentplayer->gunctrl.weaponnum >= WEAPON_UNARMED
				&& g_Vars.currentplayer->gunctrl.weaponnum <= WEAPON_COMBATBOOST
				&& g_PlayerConfigsArray[g_Vars.currentplayerstats->mpindex].gunfuncs[(g_Vars.currentplayer->gunctrl.weaponnum - 1) >> 3] & (1 << ((g_Vars.currentplayer->gunctrl.weaponnum - 1) & 7))) {
			if (slot == 1) {
				g_AmMenus[g_AmIndex].togglefunc = true;
			}
		} else {
			if (slot != 1) {
				g_AmMenus[g_AmIndex].togglefunc = true;
			}
		}
		break;
	default:
		if (g_MissionConfig.iscoop) {
			if (am_get_first_buddy_index() > -1) {
				if (slot == 1) {
					am_set_ai_buddy_temperament(true); // aggressive
				} else if (slot == 7) {
					am_set_ai_buddy_temperament(false); // passive
#if VERSION >= VERSION_NTSC_1_0
				} else if (slot == 3) {
					am_set_ai_buddy_stealth();
#endif
				}
			}
		} else if (g_Vars.normmplayerisrunning) {
			if (g_AmMenus[g_AmIndex].allbots) {
				for (i = 0; i < g_Vars.currentplayer->numaibuddies; i++) {
					botcmd_apply(g_MpAllChrPtrs[g_Vars.currentplayer->aibuddynums[i]], g_AmBotCommands[slot]);
				}
			} else {
				botcmd_apply(g_MpAllChrPtrs[g_Vars.currentplayer->aibuddynums[g_AmMenus[g_AmIndex].screenindex - 2]], g_AmBotCommands[slot]);
			}
		}
	}
}

void am_get_slot_details(s32 slot, u32 *flags, char *label)
{
	u32 weaponnum;
	s32 qty;
	s32 secs;
	s32 modulo;
	struct funcdef *prifunc;
	struct funcdef *secfunc;

	switch (g_AmMenus[g_AmIndex].screenindex) {
	case 0: // Weapon screen
		if (slot == 4) {
			strcpy(label, lang_get(L_MISC_170)); // "Weapon"
			return;
		}

		if (slot > 4) {
			slot--;
		}

		if (inv_get_current_index() == g_AmMenus[g_AmIndex].invindexes[slot]) {
			*flags |= AMSLOTFLAG_CURRENT;
		}

		if (g_AmMenus[g_AmIndex].invindexes[slot] >= inv_get_count()) {
			strcpy(label, "");
		} else {
			if (inv_get_weapon_num_by_index(g_AmMenus[g_AmIndex].invindexes[slot]) == WEAPON_CLOAKINGDEVICE) {
				// Special case: "Cloak %d"
				qty = bgun_get_reserved_ammo_count(AMMOTYPE_CLOAK);
				secs = qty / TICKS(60);
				modulo = (qty - (secs * TICKS(60))) * 100 / TICKS(60);
				sprintf(label, lang_get(L_OPTIONS_491), secs + (modulo > 0 ? 1 : 0)); // "cloak %d"
			} else {
				strcpy(label, inv_get_short_name_by_index(g_AmMenus[g_AmIndex].invindexes[slot]));
			}
		}

		weaponnum = inv_get_weapon_num_by_index(g_AmMenus[g_AmIndex].invindexes[slot]);

		if (gset_get_device_state(weaponnum) == DEVICESTATE_ACTIVE) {
			*flags |= AMSLOTFLAG_ACTIVE;
		}

		weaponnum = inv_get_weapon_num_by_index(g_AmMenus[g_AmIndex].invindexes[slot]);

		if (!bgun_has_ammo_for_weapon(weaponnum)) {
			*flags |= AMSLOTFLAG_NOAMMO;
		}
		break;
	case 1: // Function screen
		strcpy(label, "");

		if (slot == 4) {
			strcpy(label, lang_get(L_MISC_171)); // "Function"
		} else if (slot == 1 || slot == 7) {
			prifunc = gset_get_funcdef_by_gset_funcnum(&g_Vars.currentplayer->hands[HAND_RIGHT].gset, FUNC_PRIMARY);
			secfunc = gset_get_funcdef_by_gset_funcnum(&g_Vars.currentplayer->hands[HAND_RIGHT].gset, FUNC_SECONDARY);

			if (slot == 1) {
				if (!secfunc || !FUNCISSEC()) {
					*flags |= AMSLOTFLAG_CURRENT;
				}

				if (prifunc) {
					strcpy(label, lang_get(prifunc->name));
				}
			} else {
				if (!prifunc || FUNCISSEC()) {
					*flags |= AMSLOTFLAG_CURRENT;
				}

				if (secfunc) {
					strcpy(label, lang_get(secfunc->name));
				}
			}
		}
		break;
	default: // Orders screen
		strcpy(label, "");

		if (g_MissionConfig.iscoop) {
			if (slot == 4) {
				strcpy(label, lang_get(L_MISC_474)); // "Perfect Buddies"
			} else if (slot == 1) {
				strcpy(label, lang_get(L_MISC_472)); // "Aggressive"
			} else if (slot == 7) {
				strcpy(label, lang_get(L_MISC_473)); // "Passive"
#if VERSION >= VERSION_NTSC_1_0
			} else if (slot == 3) {
				strcpy(label, lang_get(L_MISC_475)); // "Stealth"
#endif
			}
		} else {
			if (slot == 4) {
				strcpy(label, lang_get(L_MISC_172)); // "Orders"
			} else {
				strcpy(label, bot_get_command_name(g_AmBotCommands[slot]));
			}
		}
		break;
	}
}

void am_reset(void)
{
	s32 i;
	s32 j;

	// @bug? Should this be set for each player?
	g_Vars.currentplayer->activemenumode = AMMODE_CLOSED;

	for (i = 0; i < ARRAYCOUNT(g_AmMenus); i++) {
		g_AmMenus[i].togglefunc = false;

		for (j = 0; j < ARRAYCOUNT(g_AmMenus[i].favourites); j++) {
			g_AmMenus[i].favourites[j] = 0xff;
		}

		if (g_Vars.normmplayerisrunning) {
			s32 index = 0;

			g_AmMenus[i].favourites[g_AmMapping[index]] = WEAPON_UNARMED;
			index++;

			for (j = 0; j < MIN(ARRAYCOUNT(g_AmMapping), ARRAYCOUNT(g_MpSetup.weapons)); j++) {
				s32 weaponnum = g_MpWeapons[g_MpSetup.weapons[j]].weaponnum;

				switch (weaponnum) {
				case WEAPON_NONE:
				case WEAPON_MPSHIELD:
				case WEAPON_DISABLED:
					break;
				default:
					g_AmMenus[i].favourites[g_AmMapping[index]] = weaponnum;
					index++;
					break;
				}
			}
		}
	}

#if VERSION == VERSION_JPN_FINAL
	g_AmFont1 = g_CharsHandelGothicSm;
	g_AmFont2 = g_FontHandelGothicSm;
#else
	if (PLAYERCOUNT() >= 2) {
		g_AmFont1 = g_CharsHandelGothicXs;
		g_AmFont2 = g_FontHandelGothicXs;
	} else {
		g_AmFont1 = g_CharsHandelGothicSm;
		g_AmFont2 = g_FontHandelGothicSm;
	}
#endif

	g_AmIndex = 0;
}

s16 am_calculate_slot_width(void)
{
	s32 textheight;
	s32 textwidth;
	s32 max = 0;
	s32 i;
	u32 flags;
	char text[32];

	for (i = 0; i < ARRAYCOUNT(g_AmBotCommands); i++) {
		am_get_slot_details(i, &flags, text);
		text_measure(&textheight, &textwidth, text, g_AmFont1, g_AmFont2, 0);

		if (textwidth > max) {
			max = textwidth;
		}
	}

#if VERSION >= VERSION_NTSC_1_0
	if (PLAYERCOUNT() > 1) {
		max += 3;
	} else {
		max += 4;
	}
#else
	max += 4;
#endif

	return max;
}

void am_change_screen(s32 step)
{
	s32 maxscreenindex;

	g_AmMenus[g_AmIndex].screenindex += step;

	if (g_Vars.normmplayerisrunning && (g_MpSetup.options & MPOPTION_TEAMSENABLED)) {
		if (g_AmMenus[g_AmIndex].allbots) {
			// Weapon selection, second function, and bot command menu

			// @bug: This is missing a check to see if there are any bots on
			// your team. In most cases this isn't a problem because the player
			// opens the screen for a single bot then uses R to switch to all
			// bots. When they do this without buddy bots the else part below
			// runs first, limits the max screen index to 1 and all is good.
			// But if you hold R as you switch to the bot command menu then this
			// part runs first and sets the screen index to an invalid value,
			// causing a crash.
			maxscreenindex = 2;
		} else {
			// Weapon selection, second function and one for each AI buddy
			maxscreenindex = g_Vars.currentplayer->numaibuddies + 1;
		}
	} else {
		// Solo missions, or MP with no teams
		if (g_MissionConfig.iscoop && am_get_first_buddy_index() >= 0) {
			// Weapon selection, second function and AI buddy commands
			maxscreenindex = 2;
		} else {
			// Weapon selection and second function
			maxscreenindex = 1;
		}
	}

	if (g_AmMenus[g_AmIndex].screenindex > maxscreenindex) {
		g_AmMenus[g_AmIndex].screenindex = 0;
	}

	if (g_AmMenus[g_AmIndex].screenindex < 0) {
		g_AmMenus[g_AmIndex].screenindex = maxscreenindex;
	}

	g_AmMenus[g_AmIndex].xradius = 10;
	g_AmMenus[g_AmIndex].dstx = -123;
	g_AmMenus[g_AmIndex].slotnum = 4;
	g_AmMenus[g_AmIndex].returntimer = 0;
	g_AmMenus[g_AmIndex].cornertimer = 0;
	g_AmMenus[g_AmIndex].alphafrac = 0;
	g_AmMenus[g_AmIndex].slotwidth = am_calculate_slot_width();
}

void am_assign_weapon_slots(void)
{
	s32 numitems = inv_get_count();
	u8 weaponnum;
	s32 i;
	s32 j;

	g_AmMenus[g_AmIndex].numitems = numitems;

	// Reset inventory indexes
	for (i = 0; i < ARRAYCOUNT(g_AmMenus[g_AmIndex].invindexes);) {
		g_AmMenus[g_AmIndex].invindexes[i] = 0xff;
		i++;
	}

	// Assign favourites
	for (i = 0; i < numitems; i++) {
		weaponnum = inv_get_weapon_num_by_index(i);

		if ((weaponnum >= WEAPON_UNARMED && weaponnum <= WEAPON_DISGUISE41)
				|| weaponnum == WEAPON_SUICIDEPILL
				|| weaponnum == WEAPON_BACKUPDISK
				|| weaponnum == WEAPON_SUITCASE) {
			for (j = 0; j < ARRAYCOUNT(g_AmMenus[g_AmIndex].favourites); j++) {
				if (g_AmMenus[g_AmIndex].favourites[j] == weaponnum) {
					if (g_AmMenus[g_AmIndex].invindexes[j] == 0xff) {
						g_AmMenus[g_AmIndex].invindexes[j] = i;
					} else {
						// empty
					}
					break;
				}
			}
		}
	}

	// If there are still unused slots, fill the remaining slots in inventory
	// order with unfavourited weapons.
	for (i = 0; i < numitems; i++) {
		bool isfavourited = false;

		for (j = 0; j < ARRAYCOUNT(g_AmMenus[g_AmIndex].invindexes); j++) {
			if (g_AmMenus[g_AmIndex].invindexes[j] == i) {
				isfavourited = true;
			}
		}

		if (!isfavourited) {
			weaponnum = inv_get_weapon_num_by_index(i);

			if ((weaponnum >= WEAPON_UNARMED && weaponnum <= WEAPON_DISGUISE41)
					|| weaponnum == WEAPON_SUICIDEPILL
					|| weaponnum == WEAPON_SUITCASE) {
				s32 useindex = -1;
				s32 j;

				// Try to find any mapping which is not yet used.
				// While it could just iterate the invitems or weaponnums arrays
				// directly, doing it using the mapping makes it allocate these
				// somewhat randomly rather than in slot order.
				for (j = 0; j < ARRAYCOUNT(g_AmMapping); j++) {
					if (g_AmMenus[g_AmIndex].favourites[g_AmMapping[j]] == 0xff) {
						useindex = j;
						break;
					}
				}

				if (useindex == -1) {
					// This part is pointless. If this part of the code is
					// reached then all the mappings were in use, and therefore
					// all the slots are in use too. There's no way this can
					// find any new slots.
					for (j = 0; j < ARRAYCOUNT(g_AmMapping); j++) {
						if (g_AmMenus[g_AmIndex].invindexes[g_AmMapping[j]] == 0xff) {
							useindex = j;
							break;
						}
					}
				}

				if (useindex >= 0) {
					g_AmMenus[g_AmIndex].invindexes[g_AmMapping[useindex]] = i;
					g_AmMenus[g_AmIndex].favourites[g_AmMapping[useindex]] = weaponnum;
				}
			}
		}
	}
}

void am_open(void)
{
	if (g_Vars.currentplayer->gunctrl.passivemode == false) {
		g_AmIndex = g_Vars.currentplayernum;
		g_Vars.currentplayer->activemenumode = AMMODE_VIEW;
		g_PlayersWithControl[g_Vars.currentplayernum] = false;
		g_AmMenus[g_AmIndex].screenindex = 0;
		g_AmMenus[g_AmIndex].selpulse = 0;
		am_assign_weapon_slots();
		am_change_screen(0);
		g_AmMenus[g_AmIndex].xradius = g_AmMenus[g_AmIndex].slotwidth + 5;
		g_AmMenus[g_AmIndex].alphafrac = 0.3;
		g_AmMenus[g_AmIndex].origscreennum = 0;
		g_AmMenus[g_AmIndex].prevallbots = 0;
		g_AmMenus[g_AmIndex].allbots = false;
	}
}

void am_close(void)
{
	if (g_AmMenus[g_AmIndex].slotnum != 4) {
		am_apply(g_AmMenus[g_AmIndex].slotnum);
	}

	g_Vars.currentplayer->activemenumode = AMMODE_CLOSED;
	g_Vars.currentplayer->joybutinhibit = 0xffffffff;
	g_PlayersWithControl[g_Vars.currentplayernum] = 1;
}

bool am_is_cramped(void)
{
#if VERSION == VERSION_JPN_FINAL
	if (PLAYERCOUNT() >= 3 && g_AmMenus[g_AmIndex].screenindex != 1) {
		return true;
	}

	if (IS4MB() && PLAYERCOUNT() == 2) {
		return true;
	}

	if (options_get_screen_split() == SCREENSPLIT_VERTICAL
			&& PLAYERCOUNT() == 2
			&& g_AmMenus[g_AmIndex].screenindex != 1) {
		return true;
	}

	return false;
#else
	return (g_AmMenus[g_AmIndex].screenindex == 0 && PLAYERCOUNT() >= 3)
		|| (IS4MB() && PLAYERCOUNT() == 2)
		|| (PLAYERCOUNT() == 2 && options_get_screen_split() == SCREENSPLIT_VERTICAL);
#endif
}

void am_calculate_slot_position(s16 column, s16 row, s16 *x, s16 *y)
{
#if VERSION == VERSION_JPN_FINAL
	s32 playercount = PLAYERCOUNT();

	*x = g_AmMenus[g_AmIndex].xradius * (column - 1);
	*y = (row - 1) * 50;

	if (column != 1 && row != 1) {
		*x = *x / 2;
		*y = *y / 2;
	}

	if (am_is_cramped()) {
		s32 offset = 1;

		if (row == 1) {
			offset = 3;
		}

		if (column == 0) {
			*x = -(g_AmMenus[g_AmIndex].slotwidth / 2) - offset;
		} else if (column == 2) {
			*x = g_AmMenus[g_AmIndex].slotwidth / 2 + offset;
		}
	} else {
		if (playercount >= 2) {
			if (row == 1 && !am_is_cramped()) {
				*x = (*x * 6) / 7;
			}
		} else {
			if (playercount >= 3 && row == 1 && !am_is_cramped()) {
				*x = (*x * 6) / 14;
			}
		}
	}

	if (playercount >= 2) {
		*y = (*y * 7) / 10;
	}

	*x += vi_get_view_left() / g_UiScaleX + vi_get_view_width() / (g_UiScaleX * 2);
	*y += vi_get_view_top() + vi_get_view_height() / 2;

	if (playercount >= 2) {
		*y += 4;
	}

	if ((playercount == 2 && (options_get_screen_split() == SCREENSPLIT_VERTICAL || IS4MB()))
			|| playercount >= 3) {
		if ((g_Vars.currentplayernum % 2) == 0) {
			*x += 8;
		} else {
			*x -= 8;
		}
	}
#elif VERSION >= VERSION_NTSC_1_0
	s32 playercount = PLAYERCOUNT();

	*x = g_AmMenus[g_AmIndex].xradius * (column - 1);
	*y = (row - 1) * 50;

	if (column != 1 && row != 1) {
		*x = *x / 2;
		*y = *y / 2;
	}

	if (am_is_cramped()) {
		s32 offset = 1;

		if (row == 1) {
			offset = 3;
		}

		if (column == 0) {
			*x = -(g_AmMenus[g_AmIndex].slotwidth / 2) - offset;
		} else if (column == 2) {
			*x = g_AmMenus[g_AmIndex].slotwidth / 2 + offset;
		}
	} else {
		if (playercount >= 2) {
			if (row == 1 && !am_is_cramped()) {
				*x = (*x * 6) / 7;
			}
		} else {
			if (playercount >= 3 && row == 1 && !am_is_cramped()) {
				*x = (*x * 6) / 14;
			}
		}
	}

	if (playercount >= 2) {
		*y = (*y * 3) / 5;
	} else if (playercount >= 3) {
		*y = (*y * 3) / 5;
	}

	*x += vi_get_view_left() / g_UiScaleX + vi_get_view_width() / (g_UiScaleX * 2);
	*y += vi_get_view_top() + vi_get_view_height() / 2;

	if ((playercount == 2 && (options_get_screen_split() == SCREENSPLIT_VERTICAL || IS4MB()))
			|| playercount >= 3) {
		if ((g_Vars.currentplayernum % 2) == 0) {
			*x += 8;
		} else {
			*x -= 8;
		}
	}
#else
	s32 playercount = PLAYERCOUNT();

	*x = g_AmMenus[g_AmIndex].xradius * (column - 1);
	*y = (row - 1) * 50;

	if (column != 1 && row != 1) {
		*x = *x / 2;
		*y = *y / 2;

		if (1);
	}

	if (row == 1 && am_is_cramped() && column != 1) {
		*x = *x / 2;

		if (*x < 0) {
			*x -= 4;
		} else {
			*x += 4;
		}
	}

	if (playercount >= 2) {
		if (row == 1 && !am_is_cramped()) {
			*x = (*x * 6) / 7;
		}

		*y = (*y * 3) / 5;
	} else if (playercount >= 3) {
		if (row == 1 && !am_is_cramped()) {
			*x = (*x * 6) / 14;
		}

		*y = (*y * 3) / 5;
	}

	*x += vi_get_view_left() / g_UiScaleX + vi_get_view_width() / (g_UiScaleX * 2);
	*y += vi_get_view_top() + vi_get_view_height() / 2;

	if ((playercount == 2 && options_get_screen_split() == SCREENSPLIT_VERTICAL) || playercount >= 3) {
		if ((g_Vars.currentplayernum % 2) == 0) {
			*x += 8;
		} else {
			*x -= 8;
		}
	}
#endif
}

Gfx *am_render_text(Gfx *gdl, char *text, u32 colour, s16 left, s16 top)
{
	s32 x;
	s32 y;
	s32 textwidth;
	s32 textheight;

	text_measure(&textheight, &textwidth, text, g_AmFont1, g_AmFont2, 0);

	x = left - (textwidth / 2);
	y = top - 4;
	gdl = text_render_v2(gdl, &x, &y, text, g_AmFont1, g_AmFont2, colour, SCREEN_320, SCREEN_240, 0, 0);

	return gdl;
}

Gfx *am_render_aibot_info(Gfx *gdl, s32 buddynum)
{
	s32 x;
	s32 y;
	s32 textwidth;
	s32 textheight;
	s32 weaponnum;
	char *weaponname;
	char *aibotname;
	s32 offset = 0;
#if VERSION >= VERSION_NTSC_1_0
	bool wide = false;
#endif

#if VERSION >= VERSION_NTSC_1_0
	if (PLAYERCOUNT() == 1 && options_get_effective_screen_size() != SCREENSIZE_FULL) {
		wide = true;
	}
#endif

#if VERSION >= VERSION_NTSC_1_0
	if ((PLAYERCOUNT() == 2 && (options_get_screen_split() == SCREENSPLIT_VERTICAL || IS4MB())) || PLAYERCOUNT() >= 3)
#else
	if ((PLAYERCOUNT() == 2 && options_get_screen_split() == SCREENSPLIT_VERTICAL) || PLAYERCOUNT() >= 3)
#endif
	{
		if ((g_Vars.currentplayernum % 2) == 0) {
			offset = 8;
		} else {
			offset = -8;
		}
	}

	if (!g_AmMenus[g_AmIndex].allbots) {
		buddynum = g_Vars.currentplayer->aibuddynums[buddynum];
		aibotname = g_MpAllChrConfigPtrs[buddynum]->name;

		if (g_MpAllChrPtrs[buddynum]->aibot) {
			weaponnum = g_MpAllChrPtrs[buddynum]->aibot->weaponnum;
		} else {
			weaponnum = 0;
		}

		if (weaponnum < WEAPON_FALCON2 || weaponnum > WEAPON_HORIZONSCANNER) {
			weaponname = lang_get(L_MISC_173); // "No Weapon"
		} else {
			weaponname = bgun_get_short_name(weaponnum);
		}

		text_measure(&textheight, &textwidth, aibotname, g_AmFont1, g_AmFont2, 0);

		x = vi_get_view_left() / g_UiScaleX
			+ (s32)(vi_get_view_width() / g_UiScaleX * 0.5f)
			- (s32)(textwidth * 0.5f)
			+ offset;

		if (PLAYERCOUNT() >= 2) {
			y = vi_get_view_top() + 5;
		} else {
			y = vi_get_view_top() + 10;
		}

#if VERSION >= VERSION_NTSC_1_0
		if (wide) {
			x = vi_get_view_left() / g_UiScaleX + 32;
		}
#endif

		gdl = text_render_vx(gdl, &x, &y, aibotname, g_AmFont1, g_AmFont2, -1,
				0x000000ff, SCREEN_320, SCREEN_240, 0, 0);

		y += (PLAYERCOUNT() >= 2) ? 0 : (s32)(textheight * 1.1f);

#if VERSION != VERSION_JPN_FINAL
		text_measure(&textheight, &textwidth, weaponname, g_AmFont1, g_AmFont2, 0);

		x = vi_get_view_left() / g_UiScaleX
			+ (s32)(vi_get_view_width() / g_UiScaleX * 0.5f)
			- (s32)(textwidth * 0.5f)
			+ offset;

#if VERSION >= VERSION_NTSC_1_0
		if (wide) {
			x = vi_get_view_left() / g_UiScaleX + 32;
		}
#endif

		gdl = text_render_v1(gdl, &x, &y, weaponname, g_AmFont1, g_AmFont2, -1,
				0x000000ff, SCREEN_320, SCREEN_240, 0, 0);
#endif

		g_Vars.currentplayer->commandingaibot = g_MpAllChrPtrs[buddynum];
	} else {
		char *title = lang_get(L_MISC_215); // "All Simulants"

		text_measure(&textheight, &textwidth, title, g_AmFont1, g_AmFont2, 0);

		x = vi_get_view_left() / g_UiScaleX
			+ (s32)(vi_get_view_width() / g_UiScaleX * 0.5f)
			- (s32)(textwidth * 0.5f)
			+ offset;

		if (PLAYERCOUNT() >= 2) {
			y = vi_get_view_top() + 5;
		} else {
			y = vi_get_view_top() + 10;
		}

#if VERSION >= VERSION_NTSC_1_0
		if (wide) {
			x = vi_get_view_left() / g_UiScaleX + 32;
		}
#endif

		gdl = text_render_vx(gdl, &x, &y, title, g_AmFont1, g_AmFont2, -1,
				0x000000ff, SCREEN_320, SCREEN_240, 0, 0);
	}

	return gdl;
}

const char var7f1b2b34[] = "Here is where the activemenu sets favourites\n";
const char var7f1b2b64[] = "slot %d = guntype %d\n";
const char var7f1b2b7c[] = "put it in %d\n";

#if VERSION < VERSION_JPN_FINAL
const char var7f1b2b8c[] = "activemenu: setting up for multiplayer\n";
const char var7f1b2bb4[] = "activemenu: setting up for single player\n";
#endif

const char var7f1b2be0[] = "Put guntype %d in slot %d\n";
const char var7f1b2bfc[] = "ActiveMenu: Two or more equipped items of guntype %d\n";
const char var7f1b2c34[] = "FAV: Added gun %d to slot %d\n";

u8 var800719a0[][3] = { {0, 1, 2}, {3, 4, 5}, {6, 7, 8} };

Gfx *am_render_slot(Gfx *gdl, char *text, s16 x, s16 y, s32 mode, s32 flags)
{
	static u32 obcol = 0xff00004f; // outer border
	static u32 ibcol = VERSION >= VERSION_NTSC_1_0 ? 0x3f00008f : 0x3f00006f; // inner background
	static u32 defcol = 0xff4f00ff; // text
	static u32 favcol = 0xffff7fff; // unused
	static u32 pickcol = 0xff4f00ff; // unused
	static u32 pickcol2 = 0xff4f00ff; // unused

	u32 colour;
	s32 paddingtop;
	s32 paddingbottom;

#if VERSION == VERSION_JPN_FINAL
	paddingtop = 7;
	paddingbottom = 7;
#else
	paddingtop = 6;
	paddingbottom = 6;

	if (PLAYERCOUNT() >= 2) {
		paddingtop = 5;
		paddingbottom = 3;
	}
#endif

	if (text == NULL || strcmp(text, "") == 0) {
		return gdl;
	}

	main_override_variable("obcol", &obcol);
	main_override_variable("ibcol", &ibcol);
	main_override_variable("defcol", &defcol);
	main_override_variable("favcol", &favcol);
	main_override_variable("pickcol", &pickcol);
	main_override_variable("pickcol2", &pickcol2);

	// Render background colour
	colour = (u32)(g_AmMenus[g_AmIndex].alphafrac * (ibcol & 0xff)) | (ibcol & 0xffffff00);

	if (mode == AMSLOTMODE_FOCUSED) {
		colour &= 0x000000ff;
	}

	if (mode == AMSLOTMODE_CURRENT || (flags & AMSLOTFLAG_CURRENT)) {
		colour &= 0x000000ff;
	}

	if (flags & AMSLOTFLAG_NOAMMO) {
		colour &= 0x000000ff;
	}

	if (g_Vars.currentplayer->activemenumode == AMMODE_EDIT) {
		colour = 0x0000006f;
	}

	gdl = text_begin_boxmode(gdl, colour);

	gDPFillRectangleScaled(gdl++,
			x - g_AmMenus[g_AmIndex].slotwidth / 2 + 1,
			y - paddingtop + 1,
			x + g_AmMenus[g_AmIndex].slotwidth / 2,
			y + paddingbottom);

	gdl = text_end_boxmode(gdl);

	// Render borders
	colour = obcol;

	if (flags & AMSLOTFLAG_NOAMMO) {
		colour &= 0x000000ff;
	}

	if (mode == AMSLOTMODE_CURRENT || (flags & AMSLOTFLAG_CURRENT)) {
		colour = 0xffffff8f;
	}

	colour = (u32)(g_AmMenus[g_AmIndex].alphafrac * (colour & 0xff)) | (colour & 0xffffff00);

	if (g_Vars.currentplayer->activemenumode == AMMODE_EDIT) {
		colour = 0x4f4f4f7f;
	}

	gdl = text_begin_boxmode(gdl, colour);

	// Top border
	gDPFillRectangleScaled(gdl++,
			x - g_AmMenus[g_AmIndex].slotwidth / 2,
			y - paddingtop,
			x + g_AmMenus[g_AmIndex].slotwidth / 2 + 1,
			y - paddingtop + 1);

	// Bottom border
	gDPFillRectangleScaled(gdl++,
			x - g_AmMenus[g_AmIndex].slotwidth / 2,
			y + paddingbottom,
			x + g_AmMenus[g_AmIndex].slotwidth / 2 + 1,
			y + paddingbottom + 1);

	// Left border
	gDPFillRectangleScaled(gdl++,
			x - g_AmMenus[g_AmIndex].slotwidth / 2,
			y - paddingtop + 1,
			x - g_AmMenus[g_AmIndex].slotwidth / 2 + 1,
			y + paddingbottom);

	// Right border
	gDPFillRectangleScaled(gdl++,
			x + g_AmMenus[g_AmIndex].slotwidth / 2,
			y - paddingtop + 1,
			x + g_AmMenus[g_AmIndex].slotwidth / 2 + 1,
			y + paddingbottom);

	gdl = text_end_boxmode(gdl);

	// Render text
	colour = defcol;

	if (mode == AMSLOTMODE_CURRENT || (flags & AMSLOTFLAG_CURRENT)) {
		colour = 0xffffffff;
	}

	if (flags & AMSLOTFLAG_ACTIVE) {
		colour = colour_blend(0xffaf8fff, colour, menu_get_cos_osc_frac(10) * 255.0f);
	}

	colour = (u32)(g_AmMenus[g_AmIndex].alphafrac * (colour & 0xff)) | (colour & 0xffffff00);

	if (g_Vars.currentplayer->activemenumode == AMMODE_EDIT) {
		colour = 0x4f4f4f7f;
	}

	gdl = am_render_text(gdl, text, colour, x, y);

	return gdl;
}

Gfx *am_render(Gfx *gdl)
{
	struct chrdata *chr;
	u32 flags;
	Col *colours;
	Vtx *vertices;
	s32 mpchrnum;
	s16 column;
	s16 row;
	u32 colour;
	s16 slotx;
	s16 sloty;
	s16 tmp1;
	s16 tmp2;

#if PAL
	g_UiScaleX = 1;
#else
	g_UiScaleX = g_ViRes == VIRES_HI ? 2 : 1;
#endif

	g_AmIndex = g_Vars.currentplayernum;
	g_Vars.currentplayer->commandingaibot = NULL;

	if (g_Vars.currentplayer->activemenumode != AMMODE_CLOSED) {
		// Draw diamond
		gdl = text_begin(gdl);

		if (g_Vars.normmplayerisrunning
				&& g_AmMenus[g_AmIndex].screenindex >= 2) {
			mpchrnum = g_Vars.currentplayer->aibuddynums[g_AmMenus[g_AmIndex].screenindex - 2];
		}

		if (g_AmMenus[g_AmIndex].dstx == -123) {
			am_calculate_slot_position(
					g_AmMenus[g_AmIndex].slotnum % 3,
					g_AmMenus[g_AmIndex].slotnum / 3,
					&g_AmMenus[g_AmIndex].selx,
					&g_AmMenus[g_AmIndex].sely);
			g_AmMenus[g_AmIndex].dstx = g_AmMenus[g_AmIndex].selx;
			g_AmMenus[g_AmIndex].dsty = g_AmMenus[g_AmIndex].sely;
		} else {
			am_calculate_slot_position(
					g_AmMenus[g_AmIndex].slotnum % 3,
					g_AmMenus[g_AmIndex].slotnum / 3,
					&g_AmMenus[g_AmIndex].dstx,
					&g_AmMenus[g_AmIndex].dsty);
		}

		gdl = ortho_begin(gdl);

		colours = gfx_allocate_colours(2);
		vertices = gfx_allocate_vertices(8);

		gDPPipeSync(gdl++);
		gDPSetCycleType(gdl++, G_CYC_1CYCLE);
		gDPSetAlphaCompare(gdl++, G_AC_NONE);
		gDPSetAlphaDither(gdl++, G_AD_NOISE);
		gDPSetCombineMode(gdl++, G_CC_MODULATEI, G_CC_MODULATEI);
		gSPClearGeometryMode(gdl++, G_CULL_BOTH);

		tex_select(&gdl, NULL, 2, 0, 2, 1, NULL);

		gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);

		// Top
		am_calculate_slot_position(1, 0, &slotx, &sloty);

		vertices[0].x = slotx * 10;
		vertices[0].y = sloty * 10;
		vertices[0].z = -10;

		// Right
		am_calculate_slot_position(2, 1, &slotx, &sloty);

		vertices[1].x = slotx * 10;
		vertices[1].y = sloty * 10;
		vertices[1].z = -10;

		// Bottom
		am_calculate_slot_position(1, 2, &slotx, &sloty);

		vertices[2].x = slotx * 10;
		vertices[2].y = sloty * 10;
		vertices[2].z = -10;

		// Left
		am_calculate_slot_position(0, 1, &slotx, &sloty);

		vertices[3].x = slotx * 10;
		vertices[3].y = sloty * 10;
		vertices[3].z = -10;

		vertices[4].z = -10;
		vertices[5].z = -10;
		vertices[6].z = -10;
		vertices[7].z = -10;

		tmp2 = (vertices[1].x - vertices[3].x) / 8;
		tmp1 = (vertices[2].y - vertices[0].y) / 8;

		vertices[4].x = vertices[0].x;
		vertices[4].y = vertices[0].y + tmp1;
		vertices[5].x = vertices[1].x - tmp2;
		vertices[5].y = vertices[1].y;
		vertices[6].x = vertices[2].x;
		vertices[6].y = vertices[2].y - tmp1;
		vertices[7].x = vertices[3].x + tmp2;
		vertices[7].y = vertices[3].y;

		vertices[0].colour = 0;
		vertices[1].colour = 0;
		vertices[2].colour = 0;
		vertices[3].colour = 0;
		vertices[4].colour = 4;
		vertices[5].colour = 4;
		vertices[6].colour = 4;
		vertices[7].colour = 4;

		colours[0].word = 0x22222200;
		colours[1].word = 0x0000004f;

		gSPColor(gdl++, osVirtualToPhysical(colours), 2);
		gSPVertex(gdl++, osVirtualToPhysical(vertices), 8, 0);

		gSPTri2(gdl++, 4, 5, 6, 6, 7, 4);
		gSPTri4(gdl++, 0, 4, 7, 7, 3, 0, 0, 1, 5, 5, 4, 0);
		gSPTri4(gdl++, 1, 2, 6, 6, 5, 1, 6, 2, 3, 3, 7, 6);

		gdl = ortho_end(gdl);

		// Draw slots
		for (column = 0; column < 3; column++) {
			for (row = 0; row < 3; row++) {
				s16 slotx;
				s16 sloty;
				u32 mode;
				char text[32];
				s32 buddynum;

				mode = AMSLOTMODE_DEFAULT;
				am_calculate_slot_position(column, row, &slotx, &sloty);
				flags = 0;

				if (column + row * 3 == g_AmMenus[g_AmIndex].slotnum) {
					mode = AMSLOTMODE_FOCUSED;
				}

				if (g_MissionConfig.iscoop && (buddynum = am_get_first_buddy_index(), buddynum >= 0)) {
					if (mode == AMSLOTMODE_DEFAULT && g_AmMenus[g_AmIndex].screenindex >= 2) {
						struct chrdata *chr = g_Vars.aibuddies[buddynum]->chr;

#if VERSION >= VERSION_NTSC_1_0
						if (var800719a0[row][column] == 7) {
							if (chr->hidden & CHRHFLAG_PASSIVE) {
								mode = AMSLOTMODE_CURRENT;
							}
						} else if (var800719a0[row][column] == 1) {
							if ((chr->hidden & CHRHFLAG_PASSIVE) == 0) {
								mode = AMSLOTMODE_CURRENT;
							}
						}
#else
						if (chr->hidden & CHRHFLAG_PASSIVE) {
							if (var800719a0[row][column] == 7) {
								mode = AMSLOTMODE_CURRENT;
							}
						} else {
							if (var800719a0[row][column] == 1) {
								mode = AMSLOTMODE_CURRENT;
							}
						}
#endif
					}
				} else {
					if (g_Vars.normmplayerisrunning
							&& mode == AMSLOTMODE_DEFAULT
							&& g_AmMenus[g_AmIndex].screenindex >= 2) {
						s32 slotcmd = g_AmBotCommands[var800719a0[row][column]];
						s32 botcmd = g_MpAllChrPtrs[mpchrnum]->aibot->command;

						if (slotcmd == botcmd) {
							mode = AMSLOTMODE_CURRENT;
						}
					}
				}

				colour = 0xffffffff;

				if (g_Vars.currentplayer->activemenumode == AMMODE_EDIT) {
					colour = 0x4f4f4f7f;
				}

				am_get_slot_details(column + row * 3, &flags, text);

				if (column == 1 && row == 1) {
					if (!am_is_cramped()) {
						gdl = am_render_text(gdl, text, colour, slotx, sloty);
					}
				} else {
					gdl = am_render_slot(gdl, text, slotx, sloty, mode, flags);
				}
			}
		}

		// Render AI bot name and weapon
		{
			struct g_vars *vars = &g_Vars;

#if VERSION >= VERSION_JPN_FINAL
			if (!(g_MissionConfig.iscoop && am_get_first_buddy_index() >= 0)
					&& g_Vars.normmplayerisrunning
					&& g_AmMenus[g_AmIndex].screenindex >= 2) {
				gdl = am_render_aibot_info(gdl, g_AmMenus[g_AmIndex].screenindex - 2);
			}
#else
			if (!(g_MissionConfig.iscoop && am_get_first_buddy_index() >= 0)
					&& vars->normmplayerisrunning
					&& g_AmMenus[g_AmIndex].screenindex >= 2) {
				gdl = am_render_aibot_info(gdl, g_AmMenus[g_AmIndex].screenindex - 2);
			}
#endif
		}

		// Note: the column and row values will never be 1 here, so this
		// condition always passes. Looks like they intended to skip drawing the
		// selection box on the simulants screen if the middle box was selected.
		if (g_AmMenus[g_AmIndex].screenindex < 2 || column != 1 || row != 1) {
			// Render selection
			s32 halfwidth;
			s16 above;
			s16 below;

#if VERSION == VERSION_JPN_FINAL
			above = 7;
			below = 7;
#else
			above = 6;
			below = 6;

			if (PLAYERCOUNT() >= 2) {
				above = 5;
				below = 3;
			}
#endif

			colour = (sinf(g_AmMenus[g_AmIndex].selpulse) + 1) * 127;
			colour = 0xff0000ff | colour << 8 | colour << 16;

			if (g_Vars.currentplayer->activemenumode == AMMODE_EDIT) {
				colour = 0x4f4f4f7f;
			}

			gdl = text_begin_boxmode(gdl, colour);

			halfwidth = g_AmMenus[g_AmIndex].slotwidth / 2;

#if VERSION >= VERSION_NTSC_1_0
			if (g_AmMenus[g_AmIndex].slotnum == 4) {
				if (am_is_cramped()) {
					halfwidth = 1;
					above = 2;
					below = 0;
				} else if (PLAYERCOUNT() >= 2) {
					s32 textheight;
					s32 textwidth;
					char text[32];
					u32 flags;

					am_get_slot_details(4, &flags, text);
					text_measure(&textheight, &textwidth, text, g_AmFont1, g_AmFont2, 0);

					halfwidth = textwidth / 2 + 2;
				}
			}
#else
			if (g_AmMenus[g_AmIndex].slotnum == 4 && am_is_cramped()) {
				halfwidth = 4;
			}
#endif

			// Top
			gDPFillRectangleScaled(gdl++,
					g_AmMenus[g_AmIndex].selx - halfwidth,
					g_AmMenus[g_AmIndex].sely - above,
					g_AmMenus[g_AmIndex].selx + halfwidth + 1,
					g_AmMenus[g_AmIndex].sely - above + 1);

			// Bottom
			gDPFillRectangleScaled(gdl++,
					g_AmMenus[g_AmIndex].selx - halfwidth,
					g_AmMenus[g_AmIndex].sely + below,
					g_AmMenus[g_AmIndex].selx + halfwidth + 1,
					g_AmMenus[g_AmIndex].sely + below + 1);

			// Left
			gDPFillRectangleScaled(gdl++,
					g_AmMenus[g_AmIndex].selx - halfwidth,
					g_AmMenus[g_AmIndex].sely - above + 1,
					g_AmMenus[g_AmIndex].selx - halfwidth + 1,
					g_AmMenus[g_AmIndex].sely + below);

			// Right
			gDPFillRectangleScaled(gdl++,
					g_AmMenus[g_AmIndex].selx + halfwidth,
					g_AmMenus[g_AmIndex].sely - above + 1,
					g_AmMenus[g_AmIndex].selx + halfwidth + 1,
					g_AmMenus[g_AmIndex].sely + below);

			gdl = text_end_boxmode(gdl);
		}

		gdl = text_end(gdl);
	}

#if VERSION != VERSION_JPN_FINAL
	chr = g_Vars.currentplayer->commandingaibot;

	if (chr) {
		// Render health bar
		f32 healthfrac = (chr->maxdamage - chr->damage) / chr->maxdamage;
		f32 shieldfrac = chr->cshield * 0.125f;
		bool redhealth = false;
		s32 xoffset;
		s32 barwidth;
		s32 barheight;
		s32 part1width;
		s32 part1left;
#if VERSION >= VERSION_NTSC_1_0
		s32 part2left;
#endif
		s32 y;
		s32 a2;

		if (healthfrac < 0.25f) {
			redhealth = true;
		}

#if VERSION < VERSION_NTSC_1_0
		{
			struct player *tmp = g_Vars.players[2];
			if (tmp);
		}
#endif

		barwidth = PLAYERCOUNT() >= 2 ? 48 : 64;
		barheight = PLAYERCOUNT() >= 2 ? 7 : 11;
		xoffset = 0;

#if VERSION >= VERSION_NTSC_1_0
		if ((PLAYERCOUNT() == 2 && (options_get_screen_split() == SCREENSPLIT_VERTICAL || IS4MB())) || PLAYERCOUNT() >= 3) {
			xoffset = (g_Vars.currentplayernum & 1) == 0 ? 8 : -8;
		}

		if (PLAYERCOUNT() == 1 && options_get_effective_screen_size() != SCREENSIZE_FULL) {
			part1left = vi_get_view_left() / g_UiScaleX + 32;
		} else {
			part1left = (s32) ((vi_get_view_width() / g_UiScaleX) * 0.5f)
				+ (s32) (vi_get_view_left() / g_UiScaleX)
				- (s32) (barwidth * 0.5f)
				+ xoffset;
		}
#else
		if ((PLAYERCOUNT() == 2 && options_get_screen_split() == SCREENSPLIT_VERTICAL) || PLAYERCOUNT() >= 3) {
			xoffset = (g_Vars.currentplayernum & 1) == 0 ? 8 : -8;
		}

		part1left = (s32) ((vi_get_view_width() / g_UiScaleX) * 0.5f)
			+ (s32) (vi_get_view_left() / g_UiScaleX)
			- (s32) (barwidth * 0.5f)
			+ xoffset;
#endif

		part1width = (s32) (barwidth * 0.25f) - 1;

#if VERSION >= VERSION_NTSC_1_0
		if (part1width);
		part2left = part1left + part1width + 2;
#endif

		if (healthfrac < 0) {
			healthfrac = 0;
		}

		gDPPipeSync(gdl++);
		gDPSetCycleType(gdl++, G_CYC_1CYCLE);
		gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
		gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);

		y = vi_get_view_top() + vi_get_view_height() - (PLAYERCOUNT() >= 2 ? 19 : 34);

		// NTSC beta doesn't scale the health bar when hi-res is on,
		// and it only matches if part2left is an inline expression
#if VERSION >= VERSION_NTSC_1_0
#define RECT(gdl, x1, y1, x2, y2) gDPFillRectangleScaled(gdl, x1, y1, x2, y2)
#define PART2LEFT() part2left
#else
#define RECT(gdl, x1, y1, x2, y2) gDPFillRectangle(gdl, x1, y1, x2, y2)
#define PART2LEFT() (part1left + part1width + 2)
#endif

		if (redhealth) {
			a2 = part1left + part1width - (s32) (part1width * (0.25f - healthfrac) * 4.0f);

			gDPSetPrimColorViaWord(gdl++, 0, 0, 0xff000060);

			// Part 1 red
			RECT(gdl++, a2, y, part1left + part1width, y + barheight);

			gDPSetPrimColorViaWord(gdl++, 0, 0, 0x00000080);

			// Part 1 black
			RECT(gdl++, part1left, y, a2, y + barheight);

			// Part 2 black
			RECT(gdl++, PART2LEFT(), y, part1left + barwidth, y + barheight);
		} else {
			gDPSetPrimColorViaWord(gdl++, 0, 0, 0x00c00060);

			// Part 1 green
			RECT(gdl++, part1left, y, part1left + part1width, y + barheight);

			// Part 2 green
			a2 = part1left + (s32) (barwidth * healthfrac);

			RECT(gdl++, PART2LEFT(), y, a2, y + barheight);

			gDPSetPrimColorViaWord(gdl++, 0, 0, 0x00000080);

			// Part 2 black
			RECT(gdl++, a2, y, part1left + barwidth, y + barheight);
		}

		// Render shield bar
		y = y + barheight + 2;
		barheight = barheight * 0.75f;

		gDPSetPrimColorViaWord(gdl++, 0, 0, 0x00c00060);

		a2 = part1left + (s32) (barwidth * shieldfrac);

		RECT(gdl++, part1left, y, a2, y + barheight);

		gDPSetPrimColorViaWord(gdl++, 0, 0, 0x00000080);

		RECT(gdl++, a2, y, part1left + barwidth, y + barheight);
	}
#endif

	g_UiScaleX = 1;

	return gdl;
}
