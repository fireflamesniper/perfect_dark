#include <ultra64.h>
#include "constants.h"
#include "game/chraction.h"
#include "game/debug.h"
#include "game/prop.h"
#include "game/setuputils.h"
#include "game/objectives.h"
#include "game/tex.h"
#include "game/camera.h"
#include "game/hudmsg.h"
#include "game/inv.h"
#include "game/playermgr.h"
#include "game/lv.h"
#include "game/training.h"
#include "game/lang.h"
#include "game/propobj.h"
#include "bss.h"
#include "lib/dma.h"
#include "lib/memp.h"
#include "lib/rng.h"
#include "lib/str.h"
#include "lib/mtx.h"
#include "data.h"
#include "types.h"

struct objective *g_Objectives[MAX_OBJECTIVES];
u32 g_ObjectiveStatuses[MAX_OBJECTIVES];
struct tag *g_TagsLinkedList;
struct briefingobj *g_BriefingObjs;
struct criteria_roomentered *g_RoomEnteredCriterias;
struct criteria_throwinroom *g_ThrowInRoomCriterias;
struct criteria_holograph *g_HolographCriterias;
s32 g_NumTags;
struct tag **g_TagPtrs;
u32 var8009d0cc;

s32 g_ObjectiveLastIndex = -1;
bool g_ObjectiveChecksDisabled = false;

#if PIRACYCHECKS
u32 xor_baffbeff(u32 value)
{
	return value ^ 0xbaffbeff;
}

u32 xor_babeffff(u32 value)
{
	return value ^ 0xbabeffff;
}

u32 xor_boobless(u32 value)
{
	return value ^ 0xb00b1e55;
}

void func0f095350(u32 arg0, u32 *arg1)
{
	volatile u32 *ptr;
	u32 value;

	__osPiGetAccess();

	ptr = (u32 *)(xor_boobless(0x04600010 ^ 0xb00b1e55) | 0xa0000000);

	value = *ptr;

	while (value & 3) {
		value = *ptr;
	}

	*arg1 = *(u32 *)((uintptr_t)osRomBase | arg0 | 0xa0000000);

	__osPiRelAccess();
}
#endif

void tags_reset(void)
{
	s32 index = 0;
	struct tag *tag = g_TagsLinkedList;

	while (tag) {
		if (tag->tagnum >= index) {
			index = tag->tagnum + 1;
		}

		tag = tag->next;
	}

	g_NumTags = index;

	if (g_NumTags) {
		u32 size = index * 4;
		g_TagPtrs = memp_alloc(ALIGN16(size), MEMPOOL_STAGE);

		for (index = 0; index < g_NumTags; index++) {
			g_TagPtrs[index] = NULL;
		}
	}

	tag = g_TagsLinkedList;

	while (tag) {
		g_TagPtrs[tag->tagnum] = tag;
		tag = tag->next;
	}

#if PIRACYCHECKS
	{
		// mtx_get_obfuscated_rom_base() returns the value at ROM offset 0xa5c.
		// This value should be 0x1740fff9.
		u32 dummy = xor_baffbeff(0xb0000a5c ^ 0xbaffbeff);
		u32 expected = xor_babeffff(0x1740fff9 ^ 0xbabeffff);

		if (mtx_get_obfuscated_rom_base() != expected) {
			// Read 4KB from a random ROM location within 128KB from the start of
			// the ROM, and write it to a random memory location between 0x80010000
			// and 0x80030ff8. This will corrupt instructions in the lib segment.
			dma_exec((u8 *)((random() & 0x1fff8) + 0x80010000), random() & 0x1fffe, 0x1000);
		}
	}
#endif
}

struct tag *tag_find_by_id(s32 tag_id)
{
	struct tag *tag = NULL;

	if (tag_id >= 0 && tag_id < g_NumTags) {
		tag = g_TagPtrs[tag_id];
	}

	return tag;
}

s32 obj_get_tag_num(struct defaultobj *obj)
{
	struct tag *tag = g_TagsLinkedList;

	if (obj && (obj->hidden & OBJHFLAG_TAGGED)) {
		while (tag) {
			if (obj == tag->obj) {
				return tag->tagnum;
			}

			tag = tag->next;
		}
	}

	return -1;
}

struct defaultobj *obj_find_by_tag_id(s32 tag_id)
{
	struct tag *tag = tag_find_by_id(tag_id);
	struct defaultobj *obj = NULL;

	if (tag) {
		obj = tag->obj;
	}

	if (obj && (obj->hidden & OBJHFLAG_TAGGED) == 0) {
		obj = NULL;
	}

	return obj;
}

s32 objective_get_count(void)
{
	return g_ObjectiveLastIndex + 1;
}

char *objective_get_text(s32 index)
{
	if (index < 10 && g_Objectives[index]) {
		return lang_get(g_Objectives[index]->text);
	}

	return NULL;
}

u32 objective_get_difficulty_bits(s32 index)
{
	if (index < 10 && g_Objectives[index]) {
		return g_Objectives[index]->difficulties;
	}

	return DIFFBIT_A | DIFFBIT_SA | DIFFBIT_PA | DIFFBIT_PD;
}

/**
 * Check if an objective is complete.
 *
 * It starts be setting the objective's status to complete, then iterates each
 * requirement in the objective to decide whether to change it to incomplete or
 * failed.
 */
s32 objective_check(s32 index)
{
	u32 stack[5];
	s32 objstatus = OBJECTIVE_COMPLETE;

	if (index < ARRAYCOUNT(g_Objectives)) {
		if (g_Objectives[index] == NULL) {
			objstatus = g_ObjectiveStatuses[index];
		} else {
			// Note: This is setting the cmd pointer to the start of the
			// beginobjective macro in the stage's setup file. The first
			// iteration of the while loop below will skip past it.
			u32 *cmd = (u32 *)g_Objectives[index];

			while ((u8)cmd[0] != OBJTYPE_ENDOBJECTIVE) {
				// The status of this requirement
				s32 reqstatus = OBJECTIVE_COMPLETE;

				switch ((u8)cmd[0]) {
				case OBJECTIVETYPE_DESTROYOBJ:
					{
						struct defaultobj *obj = obj_find_by_tag_id(cmd[1]);
						if (obj && obj->prop && obj_is_healthy(obj)) {
							reqstatus = OBJECTIVE_INCOMPLETE;
						}
					}
					break;
				case OBJECTIVETYPE_COMPFLAGS:
					if (!chr_has_stage_flag(NULL, cmd[1])) {
						reqstatus = OBJECTIVE_INCOMPLETE;
					}
					break;
				case OBJECTIVETYPE_FAILFLAGS:
					if (chr_has_stage_flag(NULL, cmd[1])) {
						reqstatus = OBJECTIVE_FAILED;
					}
					break;
				case OBJECTIVETYPE_COLLECTOBJ:
					{
						struct defaultobj *obj = obj_find_by_tag_id(cmd[1]);
						s32 prevplayernum;
						s32 collected = false;
						s32 i;

						if (!obj || !obj->prop || !obj_is_healthy(obj)) {
							reqstatus = OBJECTIVE_FAILED;
						} else {
							prevplayernum = g_Vars.currentplayernum;

							for (i = 0; i < PLAYERCOUNT(); i++) {
								if (g_Vars.players[i] == g_Vars.bond || g_Vars.players[i] == g_Vars.coop) {
									set_current_player_num(i);

									if (inv_has_prop(obj->prop)) {
										collected = true;
										break;
									}
								}
							}

							set_current_player_num(prevplayernum);

							if (!collected) {
								reqstatus = OBJECTIVE_INCOMPLETE;
							}
						}
					}
					break;
				case OBJECTIVETYPE_THROWOBJ:
					{
						struct defaultobj *obj = obj_find_by_tag_id(cmd[1]);

						if (obj && obj->prop) {
							s32 i;
							s32 prevplayernum = g_Vars.currentplayernum;

							for (i = 0; i < PLAYERCOUNT(); i++) {
								if (g_Vars.players[i] == g_Vars.bond || g_Vars.players[i] == g_Vars.coop) {
									set_current_player_num(i);

									if (inv_has_prop(obj->prop)) {
										reqstatus = OBJECTIVE_INCOMPLETE;
										break;
									}
								}
							}

							set_current_player_num(prevplayernum);
						}
					}
					break;
				case OBJECTIVETYPE_HOLOGRAPH:
					{
						struct defaultobj *obj = obj_find_by_tag_id(cmd[1]);

						if (cmd[2] == 0) {
							if (!obj || !obj->prop || !obj_is_healthy(obj)) {
								reqstatus = OBJECTIVE_FAILED;
							} else {
								reqstatus = OBJECTIVE_INCOMPLETE;
							}
						}
					}
					break;
				case OBJECTIVETYPE_ENTERROOM:
					if (cmd[2] == 0) {
						reqstatus = OBJECTIVE_INCOMPLETE;
					}
					break;
				case OBJECTIVETYPE_THROWINROOM:
					if (cmd[3] == 0) {
						reqstatus = OBJECTIVE_INCOMPLETE;
					}
					break;
				case OBJTYPE_BEGINOBJECTIVE:
				case OBJTYPE_ENDOBJECTIVE:
					break;
				}

				if (objstatus == OBJECTIVE_COMPLETE) {
					if (reqstatus != OBJECTIVE_COMPLETE) {
						// This is the first requirement that is causing the
						// objective to not be complete, so apply it.
						objstatus = reqstatus;
					}
				} else if (objstatus == OBJECTIVE_INCOMPLETE) {
					if (reqstatus == OBJECTIVE_FAILED) {
						// An earlier requirement was incomplete,
						// and this requirement is failed.
						objstatus = reqstatus;
					}
				}

				cmd = cmd + setup_get_cmd_length(cmd);
			}
		}
	}

	if (debug_force_all_objectives_complete()) {
		objstatus = OBJECTIVE_COMPLETE;
	}

	return objstatus;
}

bool objective_is_all_complete(void)
{
	s32 i;

	for (i = 0; i < objective_get_count(); i++) {
		u32 diffbits = objective_get_difficulty_bits(i);

		if ((1 << lv_get_difficulty() & diffbits) &&
				objective_check(i) != OBJECTIVE_COMPLETE) {
			return false;
		}
	}

	return true;
}

void objectives_disable_checking(void)
{
	g_ObjectiveChecksDisabled = true;
}

#if VERSION >= VERSION_NTSC_1_0
void objectives_show_hudmsg(char *buffer, s32 hudmsgtype)
{
	s32 prevplayernum = g_Vars.currentplayernum;
	s32 i;

	for (i = 0; i < PLAYERCOUNT(); i++) {
		set_current_player_num(i);

		if (g_Vars.currentplayer == g_Vars.bond || g_Vars.currentplayer == g_Vars.coop) {
			hudmsg_create_with_flags(buffer, hudmsgtype, HUDMSGFLAG_DELAY | HUDMSGFLAG_ALLOWDUPES);
		}
	}

	set_current_player_num(prevplayernum);
}
#endif

void objectives_check_all(void)
{
	s32 availableindex = 0;
	s32 i;
	char buffer[50] = "";

	if (!g_ObjectiveChecksDisabled) {
		for (i = 0; i <= g_ObjectiveLastIndex; i++) {
			s32 status = objective_check(i);

			if (g_ObjectiveStatuses[i] != status) {
				g_ObjectiveStatuses[i] = status;

				if (objective_get_difficulty_bits(i) & (1 << lv_get_difficulty())) {
#if VERSION >= VERSION_JPN_FINAL
					u8 jpnstr[] = {0, 0, 0};
					jpnstr[0] = 0x80;
					jpnstr[1] = 0x80 | (0x11 + availableindex);
					sprintf(buffer, "%s %s: ", lang_get(L_MISC_044), jpnstr); // "Objective"
#else
					sprintf(buffer, "%s %d: ", lang_get(L_MISC_044), availableindex + 1); // "Objective"
#endif

#if VERSION >= VERSION_NTSC_1_0
					// NTSC 1.0 and above shows objective messages to everyone,
					// while beta only shows them to the current player.
					if (status == OBJECTIVE_COMPLETE) {
						strcat(buffer, lang_get(L_MISC_045)); // "Completed"
						objectives_show_hudmsg(buffer, HUDMSGTYPE_OBJECTIVECOMPLETE);
					} else if (status == OBJECTIVE_INCOMPLETE) {
						strcat(buffer, lang_get(L_MISC_046)); // "Incomplete"
						objectives_show_hudmsg(buffer, HUDMSGTYPE_OBJECTIVECOMPLETE);
					} else if (status == OBJECTIVE_FAILED) {
						strcat(buffer, lang_get(L_MISC_047)); // "Failed"
						objectives_show_hudmsg(buffer, HUDMSGTYPE_OBJECTIVEFAILED);
					}
#else
					if (status == OBJECTIVE_COMPLETE) {
						strcat(buffer, lang_get(L_MISC_045)); // "Completed"
						hudmsg_create_with_flags(buffer, HUDMSGTYPE_OBJECTIVECOMPLETE, HUDMSGFLAG_ALLOWDUPES);
					} else if (status == OBJECTIVE_INCOMPLETE) {
						strcat(buffer, lang_get(L_MISC_046)); // "Incomplete"
						hudmsg_create_with_flags(buffer, HUDMSGTYPE_OBJECTIVECOMPLETE, HUDMSGFLAG_ALLOWDUPES);
					} else if (status == OBJECTIVE_FAILED) {
						strcat(buffer, lang_get(L_MISC_047)); // "Failed"
						hudmsg_create_with_flags(buffer, HUDMSGTYPE_OBJECTIVEFAILED, HUDMSGFLAG_ALLOWDUPES);
					}
#endif
				}
			}

			if (objective_get_difficulty_bits(i) & (1 << lv_get_difficulty())) {
				availableindex++;
			}
		}
	}
}

void objective_check_room_entered(s32 currentroom)
{
	struct criteria_roomentered *criteria = g_RoomEnteredCriterias;

	while (criteria) {
		if (criteria->status == OBJECTIVE_INCOMPLETE) {
			s32 room = chr_get_pad_room(NULL, criteria->pad);

			if (room >= 0 && room == currentroom) {
				criteria->status = OBJECTIVE_COMPLETE;
			}
		}

		criteria = criteria->next;
	}
}

void objective_check_throw_in_room(s32 arg0, RoomNum *inrooms)
{
	struct criteria_throwinroom *criteria = g_ThrowInRoomCriterias;

	while (criteria) {
		if (criteria->status == OBJECTIVE_INCOMPLETE && criteria->unk04 == arg0) {
			s32 room = chr_get_pad_room(NULL, criteria->pad);

			if (room >= 0) {
				RoomNum requirerooms[2];
				requirerooms[0] = room;
				requirerooms[1] = -1;

				if (array_intersects(requirerooms, inrooms)) {
					criteria->status = OBJECTIVE_COMPLETE;
				}
			}
		}

		criteria = criteria->next;
	}
}

void objective_check_holograph(f32 maxdist)
{
	struct criteria_holograph *criteria = g_HolographCriterias;

	while (criteria) {
		if (g_Vars.stagenum == STAGE_CITRAINING) {
			criteria->status = OBJECTIVE_INCOMPLETE;
		}

		if (criteria->status == OBJECTIVE_INCOMPLETE) {
			struct defaultobj *obj = obj_find_by_tag_id(criteria->obj);

			if (obj && obj->prop
					&& (obj->prop->flags & PROPFLAG_ONTHISSCREENTHISTICK)
					&& obj->prop->z >= 0
					&& obj_is_healthy(obj)) {
				struct coord sp9c;
				f32 xrange[2];
				f32 yrange[2];
				f32 dist = -1;

				if (maxdist != 0.0f) {
					f32 xdiff = obj->prop->pos.x - g_Vars.currentplayer->cam_pos.x;
					f32 zdiff = obj->prop->pos.z - g_Vars.currentplayer->cam_pos.z;
					dist = xdiff * xdiff + zdiff * zdiff;
					maxdist = maxdist * maxdist;
				}

				if (dist < maxdist && prop_get_screen_coords(obj->prop, &sp9c, xrange, yrange)) {
					f32 sp78[2];
					f32 sp70[2];
					obj_get_screeninfo(&sp9c, xrange, yrange, sp78, sp70);

					if (sp78[0] > cam_get_screen_left()
							&& sp78[0] < cam_get_screen_left() + cam_get_screen_width()
							&& sp70[0] > cam_get_screen_left()
							&& sp70[0] < cam_get_screen_left() + cam_get_screen_width()
							&& sp78[1] > cam_get_screen_top()
							&& sp78[1] < cam_get_screen_top() + cam_get_screen_height()
							&& sp70[1] > cam_get_screen_top()
							&& sp70[1] < cam_get_screen_top() + cam_get_screen_height()) {
						criteria->status = OBJECTIVE_COMPLETE;

						if (g_Vars.stagenum == STAGE_CITRAINING) {
							struct trainingdata *data = dt_get_data();
							data->holographedpc = true;
						}
					}
				}
			}
		}

		criteria = criteria->next;
	}
}
