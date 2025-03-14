#include <ultra64.h>
#include "constants.h"
#include "game/bg.h"
#include "game/pad.h"
#include "bss.h"
#include "lib/collision.h"
#include "lib/memp.h"
#include "lib/anim.h"
#include "data.h"
#include "types.h"

void cover_allocate_special(u16 *specialcovernums)
{
	s32 i;

	g_SpecialCoverNums = memp_alloc(ALIGN16(g_NumSpecialCovers * sizeof(g_SpecialCoverNums[0])), MEMPOOL_STAGE);

	if (g_SpecialCoverNums != NULL) {
		for (i = 0; i < g_NumSpecialCovers; i++) {
			g_SpecialCoverNums[i] = specialcovernums[i];
		}
	}
}

void setup_prepare_cover(void)
{
	s32 i;
	s32 numcovers = g_PadsFile->numcovers;
	RoomNum *roomsptr;
	f32 scale = 1;
	struct coord aimpos;
	struct cover cover;
	u16 specialcovernums[1024];
	RoomNum inrooms[21];
	RoomNum aboverooms[21];

	g_CoverFlags = memp_alloc(ALIGN16(numcovers * sizeof(g_CoverFlags[0])), MEMPOOL_STAGE);
	g_CoverRooms = memp_alloc(ALIGN16(numcovers * sizeof(g_CoverRooms[0])), MEMPOOL_STAGE);
	g_CoverCandidates = memp_alloc(ALIGN16(numcovers * sizeof(g_CoverCandidates[0])), MEMPOOL_STAGE);

	g_NumSpecialCovers = 0;
	g_SpecialCoverNums = NULL;

	if (g_CoverFlags && g_CoverRooms && g_CoverCandidates) {
		for (i = 0; i < numcovers; i++) {
			roomsptr = NULL;
			g_CoverFlags[i] = 0;

			if (cover_unpack(i, &cover)
					&& (cover.look->x != 0.0f || cover.look->y != 0.0f || cover.look->z != 0.0f)) {
				if (cover_is_special(&cover)) {
					specialcovernums[g_NumSpecialCovers] = i;
					g_NumSpecialCovers++;
				}

				cover.pos->x *= scale;
				cover.pos->y *= scale;
				cover.pos->z *= scale;

				if (cover.look->x == 1.0f && cover.look->y == 1.0f && cover.look->z == 1.0f) {
					g_CoverFlags[i] |= COVERFLAG_OMNIDIRECTIONAL;
				} else if (!cover_is_special(&cover)) {
					struct coord *look = cover.look;
					look->y = 0;
					guNormalize(&look->x, &look->y, &look->z);
				}

				// Find room
				bg_find_rooms_by_pos(cover.pos, inrooms, aboverooms, 20, NULL);

				if (inrooms[0] != -1) {
					roomsptr = inrooms;
				} else if (aboverooms[0] != -1) {
					roomsptr = aboverooms;
				}

				g_CoverRooms[i] = -1;

				if (roomsptr != NULL) {
					s32 room = cd_find_room_at_pos(cover.pos, roomsptr);

					if (room > 0) {
						g_CoverRooms[i] = (RoomNum)room;
					} else {
						g_CoverRooms[i] = roomsptr[0];
					}
				}

				// Determine if aim is in the same room or not
				if (g_CoverRooms[i] < 0) {
					g_CoverFlags[i] |= COVERFLAG_AIMSAMEROOM;
				} else if ((g_CoverFlags[i] & COVERFLAG_OMNIDIRECTIONAL) == 0) {
					aimpos.x = cover.pos->x + cover.look->f[0] * 600;
					aimpos.y = cover.pos->y;
					aimpos.z = cover.pos->z + cover.look->f[2] * 600;

					bg_find_rooms_by_pos(&aimpos, inrooms, aboverooms, 20, NULL);

					if (inrooms[0] != -1) {
						roomsptr = inrooms;
					} else if (aboverooms[0] != -1) {
						roomsptr = aboverooms;
					}

					if (roomsptr) {
						s32 aimroom = cd_find_room_at_pos(&aimpos, roomsptr);

						if (aimroom > 0) {
							g_CoverFlags[i] |= (g_CoverRooms[i] == (RoomNum)aimroom) ? COVERFLAG_AIMSAMEROOM : COVERFLAG_AIMDIFFROOM;
						} else {
							g_CoverFlags[i] |= (g_CoverRooms[i] == roomsptr[0]) ? COVERFLAG_AIMSAMEROOM : COVERFLAG_AIMDIFFROOM;
						}
					} else {
						g_CoverFlags[i] |= COVERFLAG_AIMDIFFROOM;
					}
				}
			}
		}

		cover_allocate_special(specialcovernums);
	}
}
