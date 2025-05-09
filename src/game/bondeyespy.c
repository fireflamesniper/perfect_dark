#include <ultra64.h>
#include "constants.h"
#include "game/bondeyespy.h"
#include "game/chraction.h"
#include "game/chr.h"
#include "game/prop.h"
#include "game/propsnd.h"
#include "game/bondgun.h"
#include "game/player.h"
#include "game/hudmsg.h"
#include "game/inv.h"
#include "game/mplayer/ingame.h"
#include "game/playermgr.h"
#include "game/stagetable.h"
#include "game/lang.h"
#include "game/options.h"
#include "game/propobj.h"
#include "bss.h"
#include "lib/joy.h"
#include "lib/main.h"
#include "lib/snd.h"
#include "lib/mtx.h"
#include "lib/anim.h"
#include "lib/collision.h"
#include "data.h"
#include "types.h"

u8 g_EyespyPickup = false;
u8 g_EyespyHit = EYESPYHIT_NONE;
u8 g_EyespyPrevHit = EYESPYHIT_NONE;
u8 var80070ecc = 0;
f32 g_EyespyMaxHeight = 160;
f32 g_EyespyMinHeight = 80;
u32 g_EyespyFallAccel = 100;
u32 g_EyespyMaxFallSpeed = 3000;

/**
 * Determines the eyespy's ground Y value by doing a collision check for a
 * position 50 units above the current position, or less if the eyespy is near
 * the top of its height range.
 */
f32 eyespy_find_ground(RoomNum *floorroom)
{
	struct prop *prop = g_Vars.currentplayer->eyespy->prop;
	s32 inlift;
	struct prop *lift;
	struct coord pos;
	f32 yoffset = 50;
	f32 ground;

	if (g_EyespyMaxHeight - g_Vars.currentplayer->eyespy->height < 50) {
		yoffset = g_EyespyMaxHeight - g_Vars.currentplayer->eyespy->height;
	}

	if (yoffset < 0) {
		yoffset = 0;
	}

	pos.x = prop->pos.x;
	pos.y = prop->pos.y + yoffset;
	pos.z = prop->pos.z;

	ground = cd_find_ground_at_cyl_ctfril(&pos, 26, prop->rooms, NULL, NULL, NULL, floorroom, &inlift, &lift);

	if (ground < -30000) {
		ground = -30000;
	}

	return ground;
}

s32 eyespy_try_move_upwards(f32 yvel)
{
	s32 result;
	struct prop *prop = g_Vars.currentplayer->eyespy->prop;
	struct chrdata *chr = prop->chr;
	struct coord dstpos;
	RoomNum dstrooms[8];
	s32 types;
	u32 stack;
	f32 f0;
	u32 stack2;

	dstpos.x = prop->pos.x;
	dstpos.y = prop->pos.y + yvel;
	dstpos.z = prop->pos.z;

	types = g_Vars.bondcollisions ? CDTYPE_ALL : CDTYPE_BG;

	// Allow eyespy to go up steps 30cm or less
	if (chr->manground + 30 >= g_Vars.currentplayer->eyespy->oldground) {
		f0 = chr->manground - prop->pos.y + 30;
	} else {
		f0 = g_Vars.currentplayer->eyespy->oldground - prop->pos.y;
	}

	los_find_final_room_exhaustive(&prop->pos, prop->rooms, &dstpos, dstrooms);
	chr_find_entered_rooms_at_pos(prop->chr, &dstpos, dstrooms);
	prop_set_perim_enabled(prop, false);

	f0 -= 0.1f;

	result = cd_test_volume_simple(&dstpos, 26, dstrooms, types, CHECKVERTICAL_YES, 15, f0);
	prop_set_perim_enabled(prop, true);

	if (result == CDRESULT_NOCOLLISION) {
		prop->pos.y = dstpos.y;
		prop_deregister_rooms(prop);
		rooms_copy(dstrooms, prop->rooms);
	}

	return result;
}

s32 eyespy_try_delta_nopush(struct coord *vel)
{
	bool cdresult = CDRESULT_NOCOLLISION;
	struct prop *eyespyprop = g_Vars.currentplayer->eyespy->prop;
	struct chrdata *chr = eyespyprop->chr;
	struct coord dstpos;
	RoomNum dstrooms[8];
	RoomNum floorroom;
	RoomNum sp74[24];
	s32 types;
	s32 i;
	f32 ymin;
	f32 xdiff;
	f32 zdiff;
	u32 stack;
	f32 radius;
	f32 halfradius;
	struct prop *prop;
	s32 playernum;
	u32 stack2;

	eyespy_find_ground(&floorroom);

	if (vel->x || vel->y || vel->z) {
		prop_set_perim_enabled(eyespyprop, false);

		dstpos.x = vel->x + eyespyprop->pos.x;
		dstpos.y = vel->y + eyespyprop->pos.y;
		dstpos.z = vel->z + eyespyprop->pos.z;

		types = g_Vars.bondcollisions ? CDTYPE_ALL : CDTYPE_BG;

		// Allow eyespy to go up steps 30cm or less
		if (g_Vars.currentplayer->eyespy->oldground <= chr->manground + 30) {
			ymin = chr->manground - eyespyprop->pos.y + 30;
		} else {
			ymin = g_Vars.currentplayer->eyespy->oldground - eyespyprop->pos.y;
		}

		// This must be populating dstrooms at least
		los_find_intersecting_rooms_exhaustive(&eyespyprop->pos, eyespyprop->rooms, &dstpos, dstrooms, sp74, 20);

		// Check if dstrooms contains the eyespy's old room.
		// If so, simplify dstrooms so it only contains that room.
		for (i = 0; dstrooms[i] != -1; i++) {
			if (dstrooms[i] == floorroom) {
				dstrooms[0] = floorroom;
				dstrooms[1] = -1;
				break;
			}
		}

		chr_find_entered_rooms_at_pos(eyespyprop->chr, &dstpos, dstrooms);

		// Check if the eyespy is moving 13cm or more along either the X or Z
		// axis in a single frame. If less, only do a collision check for the
		// dst position. If more, do a halfway check too?
		xdiff = dstpos.x - eyespyprop->pos.x;
		zdiff = dstpos.z - eyespyprop->pos.z;

		radius = 26;
		halfradius = radius * 0.5f;

		if (xdiff > halfradius || zdiff > halfradius || xdiff < -halfradius || zdiff < -halfradius) {
			cdresult = cd_test_cylmove_oobfail_findclosest_finddist(&eyespyprop->pos, eyespyprop->rooms, &dstpos, dstrooms, radius, types, CHECKVERTICAL_YES, 15, ymin);

			if (cdresult == CDRESULT_NOCOLLISION) {
				cdresult = cd_test_volume_fromdir(&eyespyprop->pos, &dstpos, radius, dstrooms, types, CHECKVERTICAL_YES, 15, ymin);
			}
		} else {
			cdresult = cd_test_volume_fromdir(&eyespyprop->pos, &dstpos, radius, sp74, types, CHECKVERTICAL_YES, 15, ymin);
		}

		if (cdresult == CDRESULT_COLLISION) {
			prop = cd_get_obstacle_prop();

			if (prop && prop->type == PROPTYPE_PLAYER) {
				playernum = g_Vars.currentplayernum;

				if (playernum == playermgr_get_player_num_by_prop(prop)) {
					g_EyespyPickup = true;
				}
			}
		}

		prop_set_perim_enabled(eyespyprop, true);

		if (cdresult == CDRESULT_NOCOLLISION) {
			// Apply the destination
			eyespyprop->pos.x = dstpos.x;
			eyespyprop->pos.y = dstpos.y;
			eyespyprop->pos.z = dstpos.z;

			prop_deregister_rooms(eyespyprop);

			rooms_copy(dstrooms, eyespyprop->rooms);
		}
	}

	return cdresult;
}

bool eyespy_try_delta(struct coord *vel)
{
	s32 cdresult = eyespy_try_delta_nopush(vel);
	struct prop *prop;

	if (cdresult != CDRESULT_NOCOLLISION) {
		g_EyespyHit = EYESPYHIT_BG;

		prop = cd_get_obstacle_prop();

		if (prop && g_Vars.lvupdate240 > 0) {
			if (prop->type == PROPTYPE_DOOR) {
				struct doorobj *door = prop->door;

				g_EyespyHit = EYESPYHIT_DOOR;

				if (door->doorflags & DOORFLAG_DAMAGEONCONTACT) {
					f32 sp38[3];
					struct coord sp2c;
					struct coord sp20;

					cd_get_edge(&sp2c, &sp20, 286, "bondeyespy.c");

					// Nothing is actually done with these coordinates...
					// This code was likely copied from bondwalk then the bounce
					// feature removed
					sp38[0] = sp20.z - sp2c.z;
					sp38[1] = 0;
					sp38[2] = sp2c.x - sp20.x;

					if (sp38[0] || sp38[2]) {
						guNormalize(&sp38[0], &sp38[1], &sp38[2]);
					} else {
						sp38[2] = 1;
					}

					if (prop);

					g_EyespyHit = EYESPYHIT_DAMAGE;
				}
			}

			if (prop->type == PROPTYPE_CHR) {
				g_EyespyHit = EYESPYHIT_CHR;
			} else if (prop->type == PROPTYPE_PLAYER) {
				g_EyespyHit = EYESPYHIT_CHR;
			} else if (prop->type == PROPTYPE_OBJ) {
				g_EyespyHit = EYESPYHIT_OBJ;
			}
		}
	}

	return cdresult;
}

s32 eyespy_try_quarterdelta(struct coord *vel, struct coord *prevedge1, struct coord *prevedge2, struct coord *newedge1, struct coord *newedge2)
{
	if (cd_has_distance()) {
		struct coord tryvel;
		s32 cdresult;
		f32 distance = cd_get_distance();

		tryvel.x = vel->x * distance * 0.25f;
		tryvel.y = vel->y * distance * 0.25f;
		tryvel.z = vel->z * distance * 0.25f;

		cdresult = eyespy_try_delta(&tryvel);

		if (cdresult == CDRESULT_NOCOLLISION) {
			return CDRESULT_NOCOLLISION;
		}

		if (cdresult == CDRESULT_COLLISION) {
			cd_get_edge(newedge1, newedge2, 350, "bondeyespy.c");

			if (newedge1->f[0] != prevedge1->f[0]
					|| newedge1->f[1] != prevedge1->f[1]
					|| newedge1->f[2] != prevedge1->f[2]
					|| newedge2->f[0] != prevedge2->f[0]
					|| newedge2->f[1] != prevedge2->f[1]
					|| newedge2->f[2] != prevedge2->f[2]) {
				return CDRESULT_COLLISION;
			}
		}
	}

	return CDRESULT_ERROR;
}

s32 eyespy_try_slide_along_edge(struct coord *vel, struct coord *edge1, struct coord *edge2)
{
	f32 frac;
	struct coord tri;
	struct coord tryvel;
	f32 edgelen;

	if (edge1->f[0] != edge2->f[0] || edge1->f[2] != edge2->f[2]) {
		tri.x = edge2->x - edge1->x;
		tri.y = 0;
		tri.z = edge2->z - edge1->z;

		edgelen = sqrtf(tri.f[0] * tri.f[0] + tri.f[2] * tri.f[2]);

		tri.x *= 1.0f / edgelen;
		tri.z *= 1.0f / edgelen;

		frac = vel->f[0] * tri.f[0] + vel->f[2] * tri.f[2];

		tryvel.x = tri.x * frac;
		tryvel.y = 0;
		tryvel.z = tri.z * frac;

		return eyespy_try_delta(&tryvel);
	}

	return CDRESULT_ERROR;
}

s32 eyespy_try_slide_along_corner(struct coord *vel, struct coord *edge1, struct coord *edge2)
{
	struct coord tri;
	struct coord tryvel;
	f32 width = 26;
	struct prop *prop = g_Vars.currentplayer->eyespy->prop;
	f32 tmp;

	tri.x = edge1->x - (prop->pos.x + vel->f[0]);
	tri.z = edge1->z - (prop->pos.z + vel->f[2]);

	if (tri.f[0] * tri.f[0] + tri.f[2] * tri.f[2] <= width * width) {
		if (edge1->f[0] != prop->pos.f[0] || edge1->f[2] != prop->pos.f[2]) {
			tri.x = -(edge1->z - prop->pos.z);
			tri.y = 0;
			tri.z = edge1->x - prop->pos.x;

			tmp = sqrtf(tri.f[0] * tri.f[0] + tri.f[2] * tri.f[2]);

			tri.x = tri.f[0] * (1.0f / tmp);
			tri.z = tri.f[2] * (1.0f / tmp);

			tmp = vel->f[0] * tri.f[0] + vel->f[2] * tri.f[2];

			tri.x = tri.x * tmp;
			tri.z = tri.z * tmp;

			tryvel.x = tri.x;
			tryvel.y = 0;
			tryvel.z = tri.z;

			if (eyespy_try_delta(&tryvel) == CDRESULT_NOCOLLISION) {
				return CDRESULT_NOCOLLISION;
			}
		}
	} else {
		tri.x = edge2->x - (prop->pos.x + vel->f[0]);
		tri.z = edge2->z - (prop->pos.z + vel->f[2]);

		if (tri.f[0] * tri.f[0] + tri.f[2] * tri.f[2] <= width * width) {
			if (edge2->f[0] != prop->pos.f[0] || edge2->f[2] != prop->pos.f[2]) {
				tri.x = -(edge2->z - prop->pos.z);
				tri.y = 0;
				tri.z = edge2->x - prop->pos.x;

				tmp = sqrtf(tri.f[0] * tri.f[0] + tri.f[2] * tri.f[2]);

				tri.x = tri.f[0] * (1.0f / tmp);
				tri.z = tri.f[2] * (1.0f / tmp);

				tmp = vel->f[0] * tri.f[0] + vel->f[2] * tri.f[2];

				tri.x = tri.x * tmp;
				tri.z = tri.z * tmp;

				tryvel.x = tri.x;
				tryvel.y = 0;
				tryvel.z = tri.z;

				if (eyespy_try_delta(&tryvel) == CDRESULT_NOCOLLISION) {
					return CDRESULT_NOCOLLISION;
				}
			}
		}
	}

	return CDRESULT_COLLISION;
}

s32 eyespy_try_fulldelta(struct coord *vel, struct coord *edge1, struct coord *edge2)
{
	bool cdresult = eyespy_try_delta(vel);

	if (cdresult != CDRESULT_NOCOLLISION) {
		cd_get_edge(edge1, edge2, 473, "bondeyespy.c");
	}

	return cdresult;
}

void eyespy_update_position(void)
{
	struct coord edgea_vtx1;
	struct coord edgea_vtx2;
	struct prop *prop = g_Vars.currentplayer->eyespy->prop;
	struct coord vel;
	f32 newground;
	struct chrdata *chr = prop->chr;
	struct coord origpos;
	f32 accel;
	f32 maxfallspeed;
	u8 hit = EYESPYHIT_NONE;
	f32 newy;
	struct coord edgeb_vtx1;
	struct coord edgeb_vtx2;
	u32 stack;
	struct coord edgec_vtx1;
	struct coord edgec_vtx2;

	origpos.f[0] = prop->pos.x;
	origpos.f[1] = prop->pos.y;
	origpos.f[2] = prop->pos.z;

	// Handle lateral movement
	vel.x = g_Vars.currentplayer->eyespy->vel.x;
	vel.y = 0;
	vel.z = g_Vars.currentplayer->eyespy->vel.z;

	if (eyespy_try_fulldelta(&vel, &edgea_vtx1, &edgea_vtx2) == CDRESULT_COLLISION) {
		if (eyespy_try_quarterdelta(&vel, &edgea_vtx1, &edgea_vtx2, &edgeb_vtx1, &edgeb_vtx2) != CDRESULT_COLLISION) {
			if (eyespy_try_slide_along_edge(&vel, &edgea_vtx1, &edgea_vtx2) <= CDRESULT_COLLISION) {
				eyespy_try_slide_along_corner(&vel, &edgea_vtx1, &edgea_vtx2);
			}
		} else {
			eyespy_try_quarterdelta(&vel, &edgeb_vtx1, &edgeb_vtx2, &edgec_vtx1, &edgec_vtx2);

			if (eyespy_try_slide_along_edge(&vel, &edgeb_vtx1, &edgeb_vtx2) <= CDRESULT_COLLISION
					&& eyespy_try_slide_along_edge(&vel, &edgea_vtx1, &edgea_vtx2) <= CDRESULT_COLLISION
					&& eyespy_try_slide_along_corner(&vel, &edgeb_vtx1, &edgeb_vtx2) <= CDRESULT_COLLISION) {
				eyespy_try_slide_along_corner(&vel, &edgea_vtx1, &edgea_vtx2);
			}
		}
	}

	// Handle gravity
	main_override_variable("g", &g_EyespyFallAccel);
	main_override_variable("l", &g_EyespyMaxFallSpeed);

	accel = g_EyespyFallAccel * 0.01; // always 1
	maxfallspeed = g_EyespyMaxFallSpeed * 0.01f; // always 30

	newy = chr->manground;
	newground = eyespy_find_ground(NULL);

	if (newground != chr->manground) {
		if (newground < chr->manground) {
			// Descending a slope or ledge
			if (g_Vars.currentplayer->eyespy->gravity >= maxfallspeed) {
				g_Vars.currentplayer->eyespy->gravity = maxfallspeed;
			} else {
				g_Vars.currentplayer->eyespy->gravity += accel;
			}

			newy -= g_Vars.currentplayer->eyespy->gravity;

			if (newy < newground) {
				g_Vars.currentplayer->eyespy->gravity = 0;
				newy = newground;
			}
		} else if (newground > chr->manground) {
			// Ascending a slope or steps
			if (g_Vars.currentplayer->eyespy->gravity >= maxfallspeed) {
				g_Vars.currentplayer->eyespy->gravity = maxfallspeed;
			} else {
				g_Vars.currentplayer->eyespy->gravity += accel;
			}

			newy += g_Vars.currentplayer->eyespy->gravity;

			if (newy > newground) {
				g_Vars.currentplayer->eyespy->gravity = 0;
				newy = newground;
			}
		}

		if (chr->manground + g_Vars.currentplayer->eyespy->height < newground + g_EyespyMinHeight) {
			newy = (newground + g_EyespyMinHeight) - g_Vars.currentplayer->eyespy->height;
		}
	}

	if (eyespy_try_move_upwards(newy - chr->manground)) {
		chr->manground = newy;
	} else {
		g_Vars.currentplayer->eyespy->gravity = 0;
	}

	g_Vars.currentplayer->eyespy->oldground = newground;
	chr->ground = chr->manground;
	chr->sumground = chr->manground * (PAL ? 8.4175090789795f : 9.999998f);

	// Handle rebound if hitting the min or max height
	if (g_Vars.currentplayer->eyespy->vel.y != 0) {
		f32 newheight = g_Vars.currentplayer->eyespy->vel.y + g_Vars.currentplayer->eyespy->height;
		bool rebound = false;

		if (newheight < g_EyespyMinHeight) {
			newheight = g_EyespyMinHeight;
			rebound = true;
		} else if (newheight > g_EyespyMaxHeight) {
			newheight = g_EyespyMaxHeight;
			rebound = true;
		}

		if (eyespy_try_move_upwards(newheight - g_Vars.currentplayer->eyespy->height)) {
			g_Vars.currentplayer->eyespy->height = newheight;
		} else {
			rebound = true;
			hit = EYESPYHIT_BG;
		}

		if (rebound) {
			g_Vars.currentplayer->eyespy->vel.y = -g_Vars.currentplayer->eyespy->vel.y * 0.25f;
		}
	}

	if (g_EyespyHit == EYESPYHIT_NONE) {
		g_EyespyHit = hit;
	}

	chr_detect_rooms(chr);

	vel.x = prop->pos.x - origpos.x;
	vel.y = prop->pos.y - origpos.y;
	vel.z = prop->pos.z - origpos.z;

	g_Vars.currentplayer->eyespy->speed = vel.f[0] * vel.f[0] + vel.f[1] * vel.f[1] + vel.f[2] * vel.f[2];
}

bool eyespy_try_launch(void)
{
	struct coord playerpos;
	struct coord testfrompos;
	struct chrdata *chr = g_Vars.currentplayer->eyespy->prop->chr;
	bool launched;
	bool insafe;
	char text[48];

	insafe = stage_get_index(g_Vars.stagenum) == STAGEINDEX_G5BUILDING
		&& g_Vars.currentplayer->prop->rooms[0] == 0x53;

	playerpos.x = g_Vars.currentplayer->prop->pos.x;
	playerpos.y = g_Vars.currentplayer->prop->pos.y;
	playerpos.z = g_Vars.currentplayer->prop->pos.z;

	chr->soundtimer = TICKS(10);

	g_Vars.currentplayer->eyespy->held = false;
	g_Vars.currentplayer->eyespy->up.x = 0;
	g_Vars.currentplayer->eyespy->up.y = 1;
	g_Vars.currentplayer->eyespy->up.z = 0;
	g_Vars.currentplayer->eyespy->startuptimer60 = 0;

	if (stage_get_index(g_Vars.stagenum) == STAGEINDEX_G5BUILDING
			|| stage_get_index(g_Vars.stagenum) == STAGEINDEX_CITRAINING) {
		g_EyespyMinHeight = 30;
	} else {
		g_EyespyMinHeight = 80;
	}

	g_Vars.currentplayer->eyespy->oldground = g_Vars.currentplayer->vv_ground;
	chr->ground = g_Vars.currentplayer->eyespy->oldground;
	chr->manground = g_Vars.currentplayer->eyespy->oldground;

	g_Vars.currentplayer->eyespy->height = g_EyespyMinHeight;
	g_Vars.currentplayer->eyespy->theta = g_Vars.currentplayer->vv_theta + 180;

	if (g_Vars.currentplayer->eyespy->theta >= 360) {
		g_Vars.currentplayer->eyespy->theta -= 360;
	}

	g_Vars.currentplayer->eyespy->costheta = cosf(g_Vars.currentplayer->eyespy->theta * 0.017453292384744f);
	g_Vars.currentplayer->eyespy->sintheta = sinf(g_Vars.currentplayer->eyespy->theta * 0.017453292384744f);

	chr->prevpos.x = g_Vars.currentplayer->eyespy->prop->pos.x = playerpos.f[0] + g_Vars.currentplayer->eyespy->sintheta * 100;
	chr->prevpos.y = g_Vars.currentplayer->eyespy->prop->pos.y = g_Vars.currentplayer->eyespy->oldground + g_Vars.currentplayer->eyespy->height;
	chr->prevpos.z = g_Vars.currentplayer->eyespy->prop->pos.z = playerpos.f[2] + g_Vars.currentplayer->eyespy->costheta * -100;

	chr->fallspeed.x = 0;
	chr->fallspeed.y = 0;
	chr->fallspeed.z = 0;

	testfrompos.x = playerpos.x;
	testfrompos.y = g_Vars.currentplayer->eyespy->oldground + g_Vars.currentplayer->eyespy->height;
	testfrompos.z = playerpos.z;

	g_Vars.currentplayer->eyespy->look.x = g_Vars.currentplayer->eyespy->sintheta;
	g_Vars.currentplayer->eyespy->look.y = 0;
	g_Vars.currentplayer->eyespy->look.z = -g_Vars.currentplayer->eyespy->costheta;
	g_Vars.currentplayer->eyespy->verta = 0;
	g_Vars.currentplayer->eyespy->cosverta = 1;
	g_Vars.currentplayer->eyespy->sinverta = 0;
	g_Vars.currentplayer->eyespy->vel.x = 0;
	g_Vars.currentplayer->eyespy->vel.y = 0;
	g_Vars.currentplayer->eyespy->vel.z = 0;
	g_Vars.currentplayer->eyespy->bobdir = 1;
	g_Vars.currentplayer->eyespy->bobtimer = 0;
	g_Vars.currentplayer->eyespy->bobactive = true;
	g_Vars.currentplayer->eyespy->speed = 0;
	g_Vars.currentplayer->eyespy->gravity = 0;
	g_Vars.currentplayer->eyespy->active = true;
	g_Vars.currentplayer->eyespy->camerabuttonheld = false;
	g_Vars.currentplayer->eyespy->buttonheld = g_Vars.currentplayer->eyespy->camerabuttonheld;
	g_Vars.currentplayer->eyespy->camerashuttertime = 0;
	g_Vars.currentplayer->eyespy->opendoor = false;
	g_Vars.currentplayer->eyespy->velf[0] = 0;
	g_Vars.currentplayer->eyespy->velf[1] = 0;
	g_Vars.currentplayer->eyespy->vels[0] = 0;
	g_Vars.currentplayer->eyespy->vels[1] = 0;
	g_Vars.currentplayer->eyespy->pitch = 0;

	player_set_perim_enabled(g_Vars.currentplayer->prop, false);

	if (insafe || !cd_test_los_oobok_findclosest(&testfrompos, g_Vars.currentplayer->prop->rooms,
				&g_Vars.currentplayer->eyespy->prop->pos,
				CDTYPE_ALL,
				GEOFLAG_FLOOR1 | GEOFLAG_FLOOR2 | GEOFLAG_WALL | GEOFLAG_BLOCK_SIGHT)) {
		// Launch failed due to not enough physical space, or we're in the G5 safe
		g_Vars.currentplayer->eyespy->deployed = false;

		chr->prevpos.x = g_Vars.currentplayer->eyespy->prop->pos.x = playerpos.f[0];
		chr->prevpos.y = g_Vars.currentplayer->eyespy->prop->pos.y = g_Vars.currentplayer->eyespy->oldground + g_Vars.currentplayer->eyespy->height;
		chr->prevpos.z = g_Vars.currentplayer->eyespy->prop->pos.z = playerpos.f[2];

		prop_set_perim_enabled(g_Vars.currentplayer->eyespy->prop, false);

		// "Not enough room to launch "
		sprintf(text, "%s%s", lang_get(L_MISC_218), bgun_get_name(WEAPON_EYESPY));
		hudmsg_create(text, HUDMSGTYPE_DEFAULT);
		launched = false;
	} else {
		// Launch successful
		g_Vars.currentplayer->eyespy->deployed = true;

		snd_start(var80095200, SFXMAP_80AB_DETONATE, 0, -1, -1, -1, -1, -1);

		launched = true;

		chr->chrflags &= ~CHRCFLAG_HIDDEN;
		chr->chrflags &= ~CHRCFLAG_INVINCIBLE;

#if VERSION >= VERSION_NTSC_1_0
		ps_create(NULL, g_Vars.currentplayer->eyespy->prop, SFXNUM_01BD_EYESPY_RUNNING, -1,
				-1, PSFLAG_REPEATING, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
#else
		ps_create(NULL, g_Vars.currentplayer->eyespy->prop, SFXNUM_01BD_EYESPY_RUNNING, -1,
				-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
#endif
	}

	player_set_perim_enabled(g_Vars.currentplayer->prop, true);
	prop_deregister_rooms(g_Vars.currentplayer->eyespy->prop);
	los_find_final_room_exhaustive(&g_Vars.currentplayer->prop->pos, g_Vars.currentplayer->prop->rooms,
			&g_Vars.currentplayer->eyespy->prop->pos, g_Vars.currentplayer->eyespy->prop->rooms);

	chr_detect_rooms(chr);

	return launched;
}

void eyespy_process_input(bool allowbuttons)
{
	struct chrdata *chr = g_Vars.currentplayer->eyespy->prop->chr;
	f32 spe0 = PAL ? 0.952f : 0.96f;
	f32 f;
	s32 i;
	f32 spd4;
	f32 spd0;
	f32 spcc;
	f32 spc8;
	f32 spc4;
	s8 contpad1 = options_get_contpad_num1(g_Vars.currentplayerstats->mpindex);
	s8 c1stickx = joy_get_stick_x(contpad1);
	s8 c2stickx;
	s8 c1sticky = joy_get_stick_y(contpad1);
	s8 c2sticky;
	u16 c1buttons = allowbuttons ? joy_get_buttons(contpad1, 0xffff) : 0;
	u16 c2buttons;
	bool domovecentre = true;
	s32 controlmode = options_get_control_mode(g_Vars.currentplayerstats->mpindex);

	bool aimpressed;
	bool shootpressed;
	bool exitpressed;
	bool activatepressed;
	f32 forwardspeed = 0.0f;
	f32 pitchspeed = 0.0f;
	f32 sidespeed = 0.0f;
	f32 angle;
	f32 ascendspeed = 0.0f;
	f32 prevverta = g_Vars.currentplayer->eyespy->verta;
	RoomNum prevrooms[8];
	s32 contpad2;
	u32 stack[5];
	f32 tmp;

	if (controlmode >= CONTROLMODE_21) {
		contpad2 = (s8) options_get_contpad_num2(g_Vars.currentplayerstats->mpindex);
		c2stickx = joy_get_stick_x(contpad2);
		c2sticky = joy_get_stick_y(contpad2);

		c2buttons = allowbuttons ? joy_get_buttons(contpad2, 0xffff) : 0;
	} else {
		c2stickx = c1stickx;
		c2sticky = c1sticky;
		c2buttons = c1buttons;
	}

	if (controlmode == CONTROLMODE_13 || controlmode == CONTROLMODE_14) {
		aimpressed = c1buttons & Z_TRIG;
		shootpressed = c1buttons & A_BUTTON;
		exitpressed = c1buttons & R_TRIG;
		activatepressed = c1buttons & B_BUTTON;
	} else if (controlmode <= CONTROLMODE_14) {
		aimpressed = c1buttons & (L_TRIG | R_TRIG);
		shootpressed = c1buttons & Z_TRIG;
		exitpressed = c1buttons & A_BUTTON;
		activatepressed = c1buttons & B_BUTTON;
	} else {
		if (controlmode >= CONTROLMODE_23) {
			aimpressed = c1buttons & Z_TRIG;
			shootpressed = c2buttons & Z_TRIG;
		} else {
			aimpressed = c2buttons & Z_TRIG;
			shootpressed = c1buttons & Z_TRIG;
		}

		exitpressed = (c1buttons | c2buttons) & A_BUTTON;
		activatepressed = (c1buttons | c2buttons) & B_BUTTON;
	}

	// Apply safe zone for c1stickx
	if (c1stickx > -10 && c1stickx < 10) {
		c1stickx = 0;
	}

	if (c1stickx <= -10) {
		c1stickx += 10;
	}

	if (c1stickx >= 10) {
		c1stickx -= 10;
	}

	// Apply safe zone for c2stickx
	if (c2stickx > -10 && c2stickx < 10) {
		c2stickx = 0;
	}

	if (c2stickx <= -10) {
		c2stickx += 10;
	}

	if (c2stickx >= 10) {
		c2stickx -= 10;
	}

	// Apply safe zone for c1sticky
	if (c1sticky > -20 && c1sticky < 20) {
		c1sticky = 0;
	}

	if (c1sticky <= -20) {
		c1sticky += 20;
	}

	if (c1sticky >= 20) {
		c1sticky -= 20;
	}

	// Apply safe zone for c2sticky
	if (c2sticky > -20 && c2sticky < 20) {
		c2sticky = 0;
	}

	if (c2sticky <= -20) {
		c2sticky += 20;
	}

	if (c2sticky >= 20) {
		c2sticky -= 20;
	}

	c1sticky *= 0.3f;
	c2sticky *= 0.3f;

	if (controlmode == CONTROLMODE_11 || controlmode == CONTROLMODE_13) {
		if (aimpressed) {
			domovecentre = false;
			pitchspeed = c1sticky;
		} else {
			forwardspeed = c1sticky;
		}

		ascendspeed = (c1buttons & (U_CBUTTONS | U_JPAD) ? 1 : 0) - (c1buttons & (D_CBUTTONS | D_JPAD) ? 1 : 0);
		sidespeed = (c1buttons & (R_CBUTTONS | R_JPAD) ? 1 : 0) - (c1buttons & (L_CBUTTONS | L_JPAD) ? 1 : 0);
	} else if (controlmode <= CONTROLMODE_14) {
		if (aimpressed) {
			domovecentre = false;
			pitchspeed = c1sticky;
		} else {
			ascendspeed = c1sticky * 0.25f;
			forwardspeed = (c1buttons & (U_CBUTTONS | U_JPAD) ? 24.0f : 0) - (c1buttons & (D_CBUTTONS | D_JPAD) ? 24.0f : 0);
		}

		sidespeed = (c1buttons & (R_CBUTTONS | R_JPAD) ? 1 : 0) - (c1buttons & (L_CBUTTONS | L_JPAD) ? 1 : 0);
	} else if (controlmode == CONTROLMODE_21 || controlmode == CONTROLMODE_23) {
		forwardspeed = c1sticky;

		if (aimpressed) {
			pitchspeed = c2sticky;
		} else {
			ascendspeed = c2sticky * 0.25f;
		}

		sidespeed = c2stickx * 0.0125f;
	} else {
		if (aimpressed) {
			pitchspeed = c1sticky;
		} else {
			ascendspeed = c1sticky * 0.25f;
		}

		forwardspeed = c2sticky;
		sidespeed = c2stickx * 0.0125f;
	}

	g_EyespyPickup = false;

#if VERSION >= VERSION_PAL_BETA
	for (f = 1; f < g_Vars.lvupdate60; f++) {
		spe0 *= PAL ? 0.952f : 0.96f;
	}
#else
	for (f = 1; f < g_Vars.lvupdate60freal; f++) {
		spe0 *= 0.96f;
	}
#endif

	if (g_Vars.currentplayer->eyespy->startuptimer60 < TICKS(50)) {
		g_Vars.currentplayer->eyespy->startuptimer60 += g_Vars.lvupdate60;
	} else {
		g_Vars.currentplayer->eyespy->startuptimer60 = TICKS(50);
	}

	chr->prevpos.x = g_Vars.currentplayer->eyespy->prop->pos.x;
	chr->prevpos.y = g_Vars.currentplayer->eyespy->prop->pos.y;
	chr->prevpos.z = g_Vars.currentplayer->eyespy->prop->pos.z;

	rooms_copy(g_Vars.currentplayer->eyespy->prop->rooms, prevrooms);

	if (!inv_has_single_weapon_inc_all_guns(WEAPON_EYESPY)) {
		g_Vars.currentplayer->eyespy->deployed = false;
		g_Vars.currentplayer->eyespy->held = true;
		g_Vars.currentplayer->eyespy->active = false;

		chr->chrflags |= CHRCFLAG_HIDDEN;

		chr_clear_references(g_Vars.currentplayer->eyespy->prop - g_Vars.props);
	}

	if (g_Vars.currentplayer->eyespy->active && g_PlayersWithControl[g_Vars.currentplayernum]) {
		g_Vars.currentplayer->joybutinhibit = 0xffffffff;

#if VERSION < VERSION_NTSC_1_0
		if (g_Vars.currentplayer->isdead == false
				&& g_Vars.currentplayer->pausemode == PAUSEMODE_UNPAUSED
				&& (c1buttons & START_BUTTON)) {
			if (!g_Vars.mplayerisrunning) {
				player_pause(MENUROOT_MAINMENU);
			} else {
				mp_push_pause_dialog();
			}
		}
#endif

		// Update theta
		g_Vars.currentplayer->eyespy->theta += c1stickx * 0.0625f * g_Vars.lvupdate60freal;

		while (g_Vars.currentplayer->eyespy->theta < 0.0f) {
			g_Vars.currentplayer->eyespy->theta += 360.0f;
		}

		while (g_Vars.currentplayer->eyespy->theta >= 360.0f) {
			g_Vars.currentplayer->eyespy->theta -= 360.0f;
		}

		g_Vars.currentplayer->eyespy->costheta = cosf(g_Vars.currentplayer->eyespy->theta * 0.017453292384744f);
		g_Vars.currentplayer->eyespy->sintheta = sinf(g_Vars.currentplayer->eyespy->theta * 0.017453292384744f);

		// Update verta
		g_Vars.currentplayer->eyespy->verta -= pitchspeed * 0.0625f * g_Vars.lvupdate60freal;

		if (prevverta != g_Vars.currentplayer->eyespy->verta) {
			while (g_Vars.currentplayer->eyespy->verta < 0.0f) {
				g_Vars.currentplayer->eyespy->verta += 360.0f;
			}

			while (g_Vars.currentplayer->eyespy->verta >= 360.0f) {
				g_Vars.currentplayer->eyespy->verta -= 360.0f;
			}

			g_Vars.currentplayer->eyespy->cosverta = cosf(g_Vars.currentplayer->eyespy->verta * 0.017453292384744f);
			g_Vars.currentplayer->eyespy->sinverta = sinf(g_Vars.currentplayer->eyespy->verta * 0.017453292384744f);
		}

		spc4 = 0.0f;
		spc8 = 0.0f;
		spcc = 0.0f;
		spd0 = 0.0f;
		spd4 = 0.0f;

		// Make eyespy look horizontally
		if (domovecentre) {
			if (g_Vars.currentplayer->eyespy->verta > 0.0f && forwardspeed != 0) {
				if (g_Vars.currentplayer->eyespy->verta < 180.0f) {
					tmp = g_Vars.currentplayer->eyespy->verta;

					for (i = 0; i < g_Vars.lvupdate60; i++) {
						tmp *= 0.04f;
						g_Vars.currentplayer->eyespy->verta -= tmp;
					}
				} else {
					tmp = 360.0f - g_Vars.currentplayer->eyespy->verta;

					for (i = 0; i < g_Vars.lvupdate60; i++) {
						tmp *= 0.04f;
						g_Vars.currentplayer->eyespy->verta += tmp;
					}
				}

				g_Vars.currentplayer->eyespy->cosverta = cosf(g_Vars.currentplayer->eyespy->verta * 0.017453292384744f);
				g_Vars.currentplayer->eyespy->sinverta = sinf(g_Vars.currentplayer->eyespy->verta * 0.017453292384744f);
			}

			spcc += forwardspeed * g_Vars.currentplayer->eyespy->sintheta * 0.15f * g_Vars.lvupdate60freal;
			spc8 += -forwardspeed * g_Vars.currentplayer->eyespy->costheta * 0.15f * g_Vars.lvupdate60freal;
		}

		if (sidespeed != 0) {
			spd4 += sidespeed * 5 * g_Vars.currentplayer->eyespy->costheta * 0.15f * g_Vars.lvupdate60freal;
			spd0 += sidespeed * 5 * g_Vars.currentplayer->eyespy->sintheta * 0.15f * g_Vars.lvupdate60freal;
		}

		if (ascendspeed != 0) {
			spc4 += ascendspeed * 3 * 0.15f * g_Vars.lvupdate60freal;

			g_Vars.currentplayer->eyespy->bobdir = (ascendspeed < 0.0f) ? -1 : 1;
			g_Vars.currentplayer->eyespy->bobtimer = 0;
			g_Vars.currentplayer->eyespy->bobactive = false;
		}

		g_Vars.currentplayer->eyespy->velf[0] += spcc;
		g_Vars.currentplayer->eyespy->velf[1] += spc8;
		g_Vars.currentplayer->eyespy->vels[0] += spd4;
		g_Vars.currentplayer->eyespy->vels[1] += spd0;

		spcc = g_Vars.currentplayer->eyespy->velf[0] * g_Vars.currentplayer->eyespy->velf[0]
			+ g_Vars.currentplayer->eyespy->velf[1] * g_Vars.currentplayer->eyespy->velf[1];

		if (spcc > 90.25f * g_Vars.lvupdate60freal * g_Vars.lvupdate60freal) {
			spcc = 9.5f * g_Vars.lvupdate60freal / sqrtf(spcc);

			g_Vars.currentplayer->eyespy->velf[0] *= spcc;
			g_Vars.currentplayer->eyespy->velf[1] *= spcc;
		}

		spd4 = g_Vars.currentplayer->eyespy->vels[0] * g_Vars.currentplayer->eyespy->vels[0]
			+ g_Vars.currentplayer->eyespy->vels[1] * g_Vars.currentplayer->eyespy->vels[1];

		if (spd4 > 225.0f * g_Vars.lvupdate60freal * g_Vars.lvupdate60freal) {
			spd4 = 15.0f * g_Vars.lvupdate60freal / sqrtf(spd4);

			g_Vars.currentplayer->eyespy->vels[0] *= spd4;
			g_Vars.currentplayer->eyespy->vels[1] *= spd4;
		}

		g_Vars.currentplayer->eyespy->vel.y += spc4;

		if (g_Vars.currentplayer->eyespy->vel.y < -(5 * g_Vars.lvupdate60freal)) {
			g_Vars.currentplayer->eyespy->vel.y = -(5 * g_Vars.lvupdate60freal);
		}

		if (g_Vars.currentplayer->eyespy->vel.y > 5 * g_Vars.lvupdate60freal) {
			g_Vars.currentplayer->eyespy->vel.y = 5 * g_Vars.lvupdate60freal;
		}
	} else {
		g_Vars.currentplayer->eyespy->bobactive = true;
	}

	// Update bob
	if (spc4 == 0.0f) {
		if (g_Vars.currentplayer->eyespy->bobactive || ABS(g_Vars.currentplayer->eyespy->vel.y) < 0.1f) {
			g_Vars.currentplayer->eyespy->bobactive = true;
			g_Vars.currentplayer->eyespy->bobtimer += g_Vars.lvupdate60;
			g_Vars.currentplayer->eyespy->vel.y += 0.025f * g_Vars.currentplayer->eyespy->bobdir;

			if (g_Vars.currentplayer->eyespy->bobtimer > TICKS(120)) {
				g_Vars.currentplayer->eyespy->bobtimer = 0;
				g_Vars.currentplayer->eyespy->bobdir = -g_Vars.currentplayer->eyespy->bobdir;
			}
		}
	}

	g_Vars.currentplayer->eyespy->vel.y *= spe0;

	g_Vars.currentplayer->eyespy->vels[0] *= spe0;
	g_Vars.currentplayer->eyespy->vels[1] *= spe0;
	g_Vars.currentplayer->eyespy->velf[0] *= spe0;
	g_Vars.currentplayer->eyespy->velf[1] *= spe0;

	g_Vars.currentplayer->eyespy->vel.x = g_Vars.currentplayer->eyespy->vels[0] + g_Vars.currentplayer->eyespy->velf[0];
	g_Vars.currentplayer->eyespy->vel.z = g_Vars.currentplayer->eyespy->vels[1] + g_Vars.currentplayer->eyespy->velf[1];

	g_Vars.currentplayer->eyespy->look.y = g_Vars.currentplayer->eyespy->sinverta;
	g_Vars.currentplayer->eyespy->look.x = g_Vars.currentplayer->eyespy->cosverta * g_Vars.currentplayer->eyespy->sintheta;
	g_Vars.currentplayer->eyespy->look.z = -g_Vars.currentplayer->eyespy->cosverta * g_Vars.currentplayer->eyespy->costheta;

	g_Vars.currentplayer->eyespy->up.y = g_Vars.currentplayer->eyespy->cosverta;
	g_Vars.currentplayer->eyespy->up.x = -g_Vars.currentplayer->eyespy->sinverta * g_Vars.currentplayer->eyespy->sintheta;
	g_Vars.currentplayer->eyespy->up.z = g_Vars.currentplayer->eyespy->sinverta * g_Vars.currentplayer->eyespy->costheta;

	g_EyespyPrevHit = g_EyespyHit;
	g_EyespyHit = EYESPYHIT_NONE;
	var80070ecc = 0;

	eyespy_update_position();

	// Consider playing the tap sound when the eyespy is driven into a wall or object
	if (g_Vars.currentplayer->eyespy->active
			&& g_EyespyHit != g_EyespyPrevHit
			&& g_EyespyHit > 0
			&& chr->soundtimer > TICKS(10)) {
		chr->soundtimer = 0;

		switch (g_EyespyHit) {
		case EYESPYHIT_BG:
			snd_start_extra(NULL, false, 16000, AL_PAN_CENTER, SFXMAP_808C_EYESPYHIT, 1, 1, -1, true);
			break;
		case EYESPYHIT_OBJ:
			snd_start_extra(NULL, false, 16000, AL_PAN_CENTER, SFXMAP_808C_EYESPYHIT, 1, 1, -1, true);
			break;
		case EYESPYHIT_DOOR:
			snd_start_extra(NULL, false, 16000, AL_PAN_CENTER, SFXMAP_808C_EYESPYHIT, 1, 1, -1, true);
			break;
		case EYESPYHIT_CHR:
			snd_start_extra(NULL, false, 16000, AL_PAN_CENTER, SFXMAP_808C_EYESPYHIT, 1, 1, -1, true);
			break;
		case EYESPYHIT_DAMAGE:
			snd_start(var80095200, SFXNUM_00F2_PICKUP_LASER, NULL, -1, -1, -1, -1, -1);
			break;
		default:
			break;
		}
	}

	// Apply speed and angle
	if (g_Vars.currentplayer->eyespy->speed > 1600.0f) {
		g_Vars.currentplayer->eyespy->speed = 1600.0f;
	}

	tmp = g_Vars.currentplayer->eyespy->speed / 1600.0f;
	ps_set_volume(g_Vars.currentplayer->eyespy->prop, tmp * 400.0f);

	angle = 180 - g_Vars.currentplayer->eyespy->theta;

	if (angle < 0) {
		angle += 360.0f;
	}

	angle = BADDTOR4(angle);

	chr_set_theta(chr, angle);

	if (g_Vars.currentplayer->eyespy->startuptimer60 < TICKS(50)) {
		return;
	}

	// Handle trigger
	if (g_Vars.currentplayer->eyespy->camerashuttertime <= 0 && shootpressed && g_Vars.currentplayer->eyespy->active) {
		if (g_Vars.currentplayer->eyespy->camerabuttonheld == false) {
			g_Vars.currentplayer->eyespy->camerabuttonheld = true;
			g_Vars.currentplayer->eyespy->camerashuttertime = TICKS(24);
		}
	} else {
		g_Vars.currentplayer->eyespy->camerabuttonheld = false;
	}

	// Handle exit
	if (exitpressed && g_Vars.currentplayer->eyespy->active) {
		g_Vars.currentplayer->eyespy->active = false;
		g_Vars.currentplayer->devicesactive &= ~DEVICE_EYESPY;
	}

	// Handle B button activation
	if (activatepressed && g_Vars.currentplayer->eyespy->active) {
		if (g_Vars.currentplayer->eyespy->buttonheld == false) {
			g_Vars.currentplayer->eyespy->buttonheld = true;
			g_Vars.currentplayer->eyespy->opendoor = true;
		} else {
			g_Vars.currentplayer->eyespy->opendoor = false;
		}
	} else {
		g_Vars.currentplayer->eyespy->opendoor = false;
		g_Vars.currentplayer->eyespy->buttonheld = false;
	}

	g_Vars.currentplayer->eyespy->hit = g_EyespyHit;

	// Check if the eyespy is inactive and coasting into the player's pickup range
	if (g_Vars.currentplayer->eyespy->active == false && !g_EyespyPickup) {
		s32 cdresult;

		f32 xdiff = g_Vars.currentplayer->eyespy->prop->pos.x - g_Vars.currentplayer->prop->pos.x;
		f32 zdiff = g_Vars.currentplayer->eyespy->prop->pos.z - g_Vars.currentplayer->prop->pos.z;

		g_EyespyPickup = true;

		if (xdiff * xdiff + zdiff * zdiff > 100 * 100) {
			g_EyespyPickup = false;
		}

		cdresult = cd_test_los_oobfail(&g_Vars.currentplayer->prop->pos, g_Vars.currentplayer->prop->rooms,
				&g_Vars.currentplayer->eyespy->prop->pos, g_Vars.currentplayer->eyespy->prop->rooms,
				CDTYPE_DOORS | CDTYPE_BG,
				GEOFLAG_WALL | GEOFLAG_BLOCK_SIGHT | GEOFLAG_BLOCK_SHOOT);

		if (cdresult == CDRESULT_COLLISION) {
			// Something's in the way
			g_EyespyPickup = false;
		}
	}

	// Sanity check - can't pick up the eyespy while it's active
	if (g_Vars.currentplayer->eyespy->active) {
		g_EyespyPickup = false;
	}

	// Handle pickup
	if (g_EyespyPickup) {
		g_Vars.currentplayer->eyespy->deployed = false;
		g_Vars.currentplayer->eyespy->held = true;
		g_Vars.currentplayer->eyespy->active = false;

		chr->chrflags |= CHRCFLAG_HIDDEN;
		chr->chrflags |= CHRCFLAG_INVINCIBLE;

		weapon_play_pickup_sound(WEAPON_EYESPY);
		current_player_queue_pickup_weapon_hudmsg(WEAPON_EYESPY, false);
		ps_stop_sound(g_Vars.currentplayer->eyespy->prop, PSTYPE_GENERAL, 0xffff);
		chr_clear_references(g_Vars.currentplayer->eyespy->prop - g_Vars.props);
	}

	coord_trigger_proxies(&chr->prop->pos, true);
}
