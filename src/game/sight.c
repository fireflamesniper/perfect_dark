#include <ultra64.h>
#include "constants.h"
#include "game/chraction.h"
#include "game/bondgun.h"
#include "game/gset.h"
#include "game/game_0b2150.h"
#include "game/tex.h"
#include "game/savebuffer.h"
#include "game/sight.h"
#include "game/text.h"
#include "game/file.h"
#include "game/gfxmemory.h"
#include "game/lang.h"
#include "game/options.h"
#include "game/propobj.h"
#include "bss.h"
#include "lib/vi.h"
#include "lib/main.h"
#include "lib/snd.h"
#include "data.h"
#include "types.h"

/**
 * Return true if the prop is considered friendly (blue sight).
 */
bool sight_is_prop_friendly(struct prop *prop)
{
	if (prop == NULL) {
		prop = g_Vars.currentplayer->lookingatprop.prop;
	}

	if (prop == NULL) {
		return false;
	}

	if (prop->type != PROPTYPE_CHR && prop->type != PROPTYPE_PLAYER) {
		return false;
	}

	if (g_Vars.coopplayernum >= 0 && prop->type == PROPTYPE_PLAYER) {
		return true;
	}

	if (g_Vars.antiplayernum >= 0 && prop->type == PROPTYPE_PLAYER) {
		return false;
	}

	if (g_Vars.normmplayerisrunning == false
			&& prop->chr
			&& (prop->chr->hidden2 & CHRH2FLAG_BLUESIGHT)) {
		return true;
	}

	return chr_compare_teams(g_Vars.currentplayer->prop->chr, prop->chr, COMPARE_FRIENDS);
}

void sight0f0d715c(void)
{
	// empty
}

Gfx *sight0f0d7164(Gfx *gdl)
{
	return gdl;
}

/**
 * Return true if the given prop can be added to the target list.
 */
bool sight_can_target_prop(struct prop *prop, s32 max)
{
	s32 i;

	for (i = 0; i < max; i++) {
		if (prop == g_Vars.currentplayer->trackedprops[i].prop) {
			return false;
		}
	}

	if (prop->type == PROPTYPE_CHR) {
		return true;
	}

	if (prop->type == PROPTYPE_PLAYER) {
		return true;
	}

	if ((prop->type == PROPTYPE_OBJ || prop->type == PROPTYPE_WEAPON || prop->type == PROPTYPE_DOOR)
			&& prop->obj && (prop->obj->flags3 & OBJFLAG3_REACTTOSIGHT)) {
		return true;
	}

	if (bgun_get_weapon_num(HAND_RIGHT) == WEAPON_ROCKETLAUNCHER) {
		return true;
	}

	return false;
}

/**
 * Return true if the sight should change colour when aiming at the given prop.
 */
bool sight_is_reactive_to_prop(struct prop *prop)
{
	if (prop->obj == NULL) {
		return false;
	}

	if (prop->type == PROPTYPE_OBJ || prop->type == PROPTYPE_WEAPON || prop->type == PROPTYPE_DOOR) {
		struct defaultobj *obj = prop->obj;

		if (g_Vars.stagenum == STAGE_CITRAINING
				&& (obj->modelnum == MODEL_COMHUB || obj->modelnum == MODEL_CIHUB || obj->modelnum == MODEL_TARGET)) {
			return true;
		}

		if (obj_get_destroyed_level(obj) > 0) {
			return false;
		}
	} else if (prop->type == PROPTYPE_CHR) {
		struct chrdata *chr = prop->chr;

		if (chr && chr->race == RACE_EYESPY) {
			struct eyespy *eyespy = chr_to_eyespy(chr);

			if (!eyespy || !eyespy->deployed) {
				return false;
			}
		}
	}

	return true;
}

s32 sight_find_free_target_index(s32 max)
{
	s32 i;

	for (i = 0; i < max; i++) {
		if (g_Vars.currentplayer->trackedprops[i].prop == NULL) {
			return i;
		}
	}

	return -1;
}

void func0f0d7364(void)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->trackedprops); i++) {
		g_Vars.currentplayer->trackedprops[i].prop = NULL;
	}
}

void sight_tick(bool sighton)
{
	struct trackedprop *trackedprop;
	u8 newtracktype;
	s32 i;
	s32 index;
	struct invaimsettings *gunsettings = gset_get_aim_settings(&g_Vars.currentplayer->hands[0].gset);
	struct funcdef *func = gset_get_funcdef_by_weaponnum_funcnum(g_Vars.currentplayer->hands[0].gset.weaponnum,
			g_Vars.currentplayer->hands[0].gset.weaponfunc);

	g_Vars.currentplayer->sighttimer240 += g_Vars.lvupdate240;

	for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->targetset); i++) {
		if (g_Vars.currentplayer->targetset[i] > TICKS(512)) {
			if (g_Vars.currentplayer->targetset[i] < (VERSION >= VERSION_PAL_BETA ? TICKS(1020) : 1024) - g_Vars.lvupdate240) {
				g_Vars.currentplayer->targetset[i] += g_Vars.lvupdate240;
			} else {
				g_Vars.currentplayer->targetset[i] = TICKS(1020);
			}
		} else {
			if (g_Vars.currentplayer->targetset[i] < (VERSION >= VERSION_PAL_BETA ? TICKS(512) : 516) - g_Vars.lvupdate240) {
				g_Vars.currentplayer->targetset[i] += g_Vars.lvupdate240;
			} else {
				g_Vars.currentplayer->targetset[i] = TICKS(512);
			}
		}
	}

	newtracktype = gunsettings->tracktype;

	if (gset_has_function_flags(&g_Vars.currentplayer->hands[0].gset, FUNCFLAG_THREATDETECTOR)) {
		newtracktype = SIGHTTRACKTYPE_THREATDETECTOR;
	}

	if (func && (func->type & 0xff) == INVENTORYFUNCTYPE_MELEE) {
		newtracktype = SIGHTTRACKTYPE_NONE;
	}

	if (newtracktype != g_Vars.currentplayer->sighttracktype) {
		if (newtracktype == SIGHTTRACKTYPE_THREATDETECTOR) {
			for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->trackedprops); i++) {
				g_Vars.currentplayer->trackedprops[i].prop = NULL;
			}
		}

		g_Vars.currentplayer->sighttracktype = newtracktype;

		switch (newtracktype) {
		case SIGHTTRACKTYPE_NONE:
		case SIGHTTRACKTYPE_DEFAULT:
		case SIGHTTRACKTYPE_BETASCANNER:
		case SIGHTTRACKTYPE_ROCKETLAUNCHER:
		case SIGHTTRACKTYPE_FOLLOWLOCKON:
			break;
		}
	}

	if (sighton && g_Vars.currentplayer->lastsighton == false && newtracktype != SIGHTTRACKTYPE_THREATDETECTOR) {
		for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->trackedprops); i++) {
			g_Vars.currentplayer->trackedprops[i].prop = NULL;
		}
	}

	for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->trackedprops); i++) {
		trackedprop = &g_Vars.currentplayer->trackedprops[i];

		if (trackedprop->prop && !sight_is_reactive_to_prop(trackedprop->prop)) {
			trackedprop->prop = NULL;
		}
	}

	trackedprop = &g_Vars.currentplayer->lookingatprop;

	if (trackedprop->prop && !sight_is_reactive_to_prop(trackedprop->prop)) {
		trackedprop->prop = NULL;
	}

	switch (g_Vars.currentplayer->sighttracktype) {
	case SIGHTTRACKTYPE_DEFAULT:
	case SIGHTTRACKTYPE_BETASCANNER:
		// Conditionally copy lookingatprop to trackedprops[0], overwriting anything that's there
		if (sighton) {
			if (g_Vars.currentplayer->lookingatprop.prop) {
				if (g_Vars.currentplayer->lookingatprop.prop != g_Vars.currentplayer->trackedprops[0].prop) {
					struct sndstate *handle;

					handle = snd_start_extra(&handle, false, AL_VOL_FULL, AL_PAN_CENTER, SFXNUM_0007, 1, 1, -1, true);

					trackedprop = &g_Vars.currentplayer->trackedprops[0];

					trackedprop->prop = g_Vars.currentplayer->lookingatprop.prop;
					trackedprop->x1 = g_Vars.currentplayer->lookingatprop.x1;
					trackedprop->y1 = g_Vars.currentplayer->lookingatprop.y1;
					trackedprop->x2 = g_Vars.currentplayer->lookingatprop.x2;
					trackedprop->y2 = g_Vars.currentplayer->lookingatprop.y2;

					g_Vars.currentplayer->targetset[0] = 0;
				}
			} else {
				g_Vars.currentplayer->trackedprops[0].prop = NULL;
			}
		}
		break;
	case SIGHTTRACKTYPE_ROCKETLAUNCHER:
		// Conditionally copy lookingatprop to trackedprops[0], but only if that slot is empty
		if (sighton && g_Vars.currentplayer->lookingatprop.prop
				&& sight_can_target_prop(g_Vars.currentplayer->lookingatprop.prop, 1)) {
			index = sight_find_free_target_index(1);

			if (index >= 0) {
				struct sndstate *handle;

				handle = snd_start_extra(&handle, false, AL_VOL_FULL, AL_PAN_CENTER, SFXNUM_0007, 1, 1, -1, true);

				trackedprop = &g_Vars.currentplayer->trackedprops[index];

				trackedprop->prop = g_Vars.currentplayer->lookingatprop.prop;
				trackedprop->x1 = g_Vars.currentplayer->lookingatprop.x1;
				trackedprop->y1 = g_Vars.currentplayer->lookingatprop.y1;
				trackedprop->x2 = g_Vars.currentplayer->lookingatprop.x2;
				trackedprop->y2 = g_Vars.currentplayer->lookingatprop.y2;

				g_Vars.currentplayer->targetset[index] = 0;
			}
		}
		break;
	case SIGHTTRACKTYPE_FOLLOWLOCKON:
		// Conditionally copy lookingatprop to any trackedprops slot, but only if the slot is empty
		if (sighton && g_Vars.currentplayer->lookingatprop.prop
				&& sight_can_target_prop(g_Vars.currentplayer->lookingatprop.prop, 4)) {
			index = sight_find_free_target_index(4);

			if (index >= 0) {
				struct sndstate *handle;

				handle = snd_start_extra(&handle, false, AL_VOL_FULL, AL_PAN_CENTER, SFXNUM_0007, 1, 1, -1, true);

				trackedprop = &g_Vars.currentplayer->trackedprops[index];

				trackedprop->prop = g_Vars.currentplayer->lookingatprop.prop;
				trackedprop->x1 = g_Vars.currentplayer->lookingatprop.x1;
				trackedprop->y1 = g_Vars.currentplayer->lookingatprop.y1;
				trackedprop->x2 = g_Vars.currentplayer->lookingatprop.x2;
				trackedprop->y2 = g_Vars.currentplayer->lookingatprop.y2;

				g_Vars.currentplayer->targetset[index] = 0;
			}
		}
		break;
	case SIGHTTRACKTYPE_NONE:
	case SIGHTTRACKTYPE_THREATDETECTOR:
		break;
	}

	g_Vars.currentplayer->lastsighton = sighton;
}

/**
 * Calculate the position of one border of a target box.
 *
 * The arguments here are named for a left border,
 * but can be called for any of the four edges.
 */
s32 sight_calculate_box_bound(s32 targetx, s32 viewleft, s32 timeelapsed, s32 timeend)
{
	s32 value;

	if (timeelapsed > timeend) {
		timeelapsed = timeend;
	}

	value = (targetx - viewleft) * timeelapsed;

	return viewleft + value / timeend;
}

/**
 * Draw a red (or blue) box around the given trackedprop.
 *
 * textid can be:
 * 0 to have no label
 * 1 to label it as "0"
 * 2 to label it as "1"
 * ...
 * 6 to label it as "5"
 * 7 or above to treat textid as a proper language text ID.
 */
Gfx *sight_draw_target_box(Gfx *gdl, struct trackedprop *trackedprop, s32 textid, s32 time)
{
	s32 viewleft = vi_get_view_left() / g_UiScaleX;
	s32 viewtop = vi_get_view_top();
	s32 viewwidth = vi_get_view_width() / g_UiScaleX;
	s32 viewheight = vi_get_view_height();
	s32 viewright = viewleft + viewwidth - 1;
	s32 viewbottom = viewtop + viewheight - 1;
	u32 colour;
	s32 boxleft;
	s32 boxright;
	s32 boxtop;
	s32 boxbottom;
	bool textonscreen = true;

	if (time > TICKS(512)) {
		time = TICKS(512);
	}

	boxleft = sight_calculate_box_bound(trackedprop->x1 / g_UiScaleX, viewleft, time, TICKS(80));
	boxtop = sight_calculate_box_bound(trackedprop->y1, viewtop, time, TICKS(80));
	boxright = sight_calculate_box_bound(trackedprop->x2 / g_UiScaleX, viewright, time, TICKS(80));
	boxbottom = sight_calculate_box_bound(trackedprop->y2, viewbottom, time, TICKS(80));

	if (trackedprop->prop) {
		colour = sight_is_prop_friendly(trackedprop->prop) ? 0x000ff60 : 0xff000060;

		gdl = text_begin_boxmode(gdl, colour);

		// Left
		if (boxleft >= viewleft && boxleft <= viewright && boxtop <= viewbottom && boxbottom >= viewtop) {
			gDPHudRectangle(gdl++,
					boxleft, (boxtop > viewtop ? boxtop : viewtop),
					boxleft, (boxbottom < viewbottom ? boxbottom : viewbottom));
		}

		// Right
		if (boxright >= viewleft && boxright <= viewright && boxtop <= viewbottom && boxbottom >= viewtop) {
			gDPHudRectangle(gdl++,
					boxright, (boxtop > viewtop ? boxtop : viewtop),
					boxright, (boxbottom < viewbottom ? boxbottom : viewbottom));
		} else {
			textonscreen = false;
		}

		// Top
		if (boxtop >= viewtop && boxtop <= viewbottom && boxleft <= viewright && boxright >= viewleft) {
			gDPHudRectangle(gdl++,
					(boxleft > viewleft ? boxleft : viewleft), boxtop,
					(boxright < viewright ? boxright : viewright), boxtop);
		} else {
			textonscreen = false;
		}

		// Bottom
		if (boxbottom >= viewtop && boxbottom <= viewbottom && boxleft <= viewright && boxright >= viewleft) {
			gDPHudRectangle(gdl++,
					(boxleft > viewleft ? boxleft : viewleft), boxbottom,
					(boxright < viewright ? boxright : viewright), boxbottom);
		}

		gdl = text_end_boxmode(gdl);

		if (textid != 0 && textonscreen) {
			s32 x = boxright + 3;
			s32 y = boxtop + 3;

			if (textid < 7) {
				char label[] = {'1', '\n', '\0'};

				// textid 1 writes '0'
				label[0] = textid + 0x2f;

				gdl = text_render_v1(gdl, &x, &y, label, g_CharsNumeric, g_FontNumeric, 0x00ff00a0, 0x000000a0, vi_get_width(), vi_get_height(), 0, 0);
			} else {
				char *text = lang_get(textid);
				gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0x00ff00a0, 0x000000a0, vi_get_width(), vi_get_height(), 0, 0);
			}
		}
	}

	return gdl;
}

Gfx *sight_draw_aimer(Gfx *gdl, s32 x, s32 y, s32 radius, s32 cornergap, u32 colour)
{
	s32 viewleft = vi_get_view_left() / g_UiScaleX;
	s32 viewtop = vi_get_view_top();
	s32 viewwidth = vi_get_view_width() / g_UiScaleX;
	s32 viewheight = vi_get_view_height();
	s32 viewright = viewleft + viewwidth - 1;
	s32 viewbottom = viewtop + viewheight - 1;

	gdl = text_begin_boxmode(gdl, 0x00ff0028);

	// Draw the lines that span most of the viewport
	if (PLAYERCOUNT() == 1) {
		gDPHudRectangle(gdl++, viewleft + 48, y, x - radius + 2, y);
		gDPHudRectangle(gdl++, x + radius - 2, y, viewright - 49, y);
		gDPHudRectangle(gdl++, x, viewtop + 10, x, y - radius + 2);
		gDPHudRectangle(gdl++, x, y + radius - 2, x, viewbottom - 10);
	} else {
		gDPHudRectangle(gdl++, viewleft, y, x - radius + 2, y);
		gDPHudRectangle(gdl++, x + radius - 2, y, viewright, y);
		gDPHudRectangle(gdl++, x, viewtop, x, y - radius + 2);
		gDPHudRectangle(gdl++, x, y + radius - 2, x, viewbottom);
	}

	gdl = text_end_boxmode(gdl);
	gdl = text_begin_boxmode(gdl, colour);

	// Draw the box
	gDPHudRectangle(gdl++, x - radius, y - radius, x - radius, y + radius);
	gDPHudRectangle(gdl++, x + radius, y - radius, x + radius, y + radius);
	gDPHudRectangle(gdl++, x - radius, y - radius, x + radius, y - radius);
	gDPHudRectangle(gdl++, x - radius, y + radius, x + radius, y + radius);

	// Go over the corners a second time
	gDPHudRectangle(gdl++, x - radius, y - radius, x - radius, y - cornergap);
	gDPHudRectangle(gdl++, x - radius, y + cornergap, x - radius, y + radius);
	gDPHudRectangle(gdl++, x + radius, y - radius, x + radius, y - cornergap);
	gDPHudRectangle(gdl++, x + radius, y + cornergap, x + radius, y + radius);
	gDPHudRectangle(gdl++, x - radius, y - radius, x - cornergap, y - radius);
	gDPHudRectangle(gdl++, x + cornergap, y - radius, x + radius, y - radius);
	gDPHudRectangle(gdl++, x - radius, y + radius, x - cornergap, y + radius);
	gDPHudRectangle(gdl++, x + cornergap, y + radius, x + radius, y + radius);

	gdl = text_end_boxmode(gdl);

	return gdl;
}

/**
 * The delayed aimer is an unused aimer box. It's twice as big as the normal one
 * and follows the gun's cursor with a very noticeable delay. The lines that
 * span the viewport are not used here, and a 3x3 box is filled in with green
 * at the live crosshair position.
 *
 * Because its position and speed properties are static variables, they only get
 * updated when the aimer is held. This means releasing and pressing R again
 * causes the box to appear where it was last.
 *
 * The default Y position is not quite centered, is not updated for PAL,
 * and is not reset for split screen play. There's also no viewport boundary
 * checks. It's likely that this feature was just a concept and was dropped
 * pretty early.
 */
Gfx *sight_draw_delayed_aimer(Gfx *gdl, s32 x, s32 y, s32 radius, s32 cornergap, u32 colour)
{
	s32 boxx;
	s32 boxy;
	s32 i;
	f32 dist;
	f32 accel;
	u32 stack;

	static f32 xpos = 160;
	static f32 ypos = 120;
	static f32 xspeed = 0;
	static f32 yspeed = 0;

	for (i = 0; i < g_Vars.lvupdate60; i++) {
		dist = x - xpos;

		if (dist > 0.5f || dist < -0.5f) {
			accel = dist * 0.05f;

			if (accel > PALUPF(2.0f)) {
				accel = PALUPF(2.0f);
			}

			if (accel < -PALUPF(2.0f)) {
				accel = -PALUPF(2.0f);
			}

			if (accel > xspeed) {
				accel = PALUPF(0.05f);
			} else if (accel < xspeed) {
				accel = -PALUPF(0.05f);
			} else {
				accel = 0.0f;
			}

			xspeed += accel;

			if (xspeed > PALUPF(2.0f)) {
				xspeed = PALUPF(2.0f);
			}

			if (xspeed < -PALUPF(2.0f)) {
				xspeed = -PALUPF(2.0f);
			}

			xpos += xspeed;
		} else {
			xpos = x;
			xspeed = 0.0f;
		}

		dist = y - ypos;

		if (dist > 0.5f || dist < -0.5f) {
			accel = dist * 0.05f;

			if (accel > PALUPF(2.0f)) {
				accel = PALUPF(2.0f);
			}
			if (accel < -PALUPF(2.0f)) {
				accel = -PALUPF(2.0f);
			}

			if (yspeed < accel) {
				accel = PALUPF(0.05f);
			} else if (accel < yspeed) {
				accel = -PALUPF(0.05f);
			} else {
				accel = 0.0f;
			}

			yspeed += accel;

			if (yspeed > PALUPF(2.0f)) {
				yspeed = PALUPF(2.0f);
			}

			if (yspeed < -PALUPF(2.0f)) {
				yspeed = -PALUPF(2.0f);
			}

			ypos += yspeed;
		} else {
			ypos = y;
			yspeed = 0.0f;
		}
	}

	boxx = xpos;
	boxy = ypos;

	gdl = text_begin_boxmode(gdl, 0x00ff0028);

	// Fill a 3x3 box at the live crosshair
	gDPHudRectangle(gdl++, x - 1, y - 1, x + 1, y - 1);
	gDPHudRectangle(gdl++, x - 1, y + 0, x + 1, y + 0);
	gDPHudRectangle(gdl++, x - 1, y + 1, x + 1, y + 1);

	gdl = text_end_boxmode(gdl);

	gdl = text_begin_boxmode(gdl, colour);

	// Draw the box
	gDPHudRectangle(gdl++, boxx - radius, boxy - radius, boxx - radius, boxy + radius);
	gDPHudRectangle(gdl++, boxx + radius, boxy - radius, boxx + radius, boxy + radius);
	gDPHudRectangle(gdl++, boxx - radius, boxy - radius, boxx + radius, boxy - radius);
	gDPHudRectangle(gdl++, boxx - radius, boxy + radius, boxx + radius, boxy + radius);

	// Go over the corners a second time
	gDPHudRectangle(gdl++, boxx - radius, boxy - radius, boxx - radius, boxy - cornergap);
	gDPHudRectangle(gdl++, boxx - radius, boxy + cornergap, boxx - radius, boxy + radius);
	gDPHudRectangle(gdl++, boxx + radius, boxy - radius, boxx + radius, boxy - cornergap);
	gDPHudRectangle(gdl++, boxx + radius, boxy + cornergap, boxx + radius, boxy + radius);
	gDPHudRectangle(gdl++, boxx - radius, boxy - radius, boxx - cornergap, boxy - radius);
	gDPHudRectangle(gdl++, boxx + cornergap, boxy - radius, boxx + radius, boxy - radius);
	gDPHudRectangle(gdl++, boxx - radius, boxy + radius, boxx - cornergap, boxy + radius);
	gDPHudRectangle(gdl++, boxx + cornergap, boxy + radius, boxx + radius, boxy + radius);

	gdl = text_end_boxmode(gdl);

	return gdl;
}

Gfx *sight_draw_default(Gfx *gdl, bool sighton)
{
	s32 radius;
	s32 cornergap;
	u32 colour;
	s32 x = (s32) g_Vars.currentplayer->crosspos[0] / g_UiScaleX;
	s32 y = g_Vars.currentplayer->crosspos[1];
	struct trackedprop *trackedprop;
	s32 i;

	static s32 sight = 0;
	static s32 identifytimer = 0;

	gdl = text_begin(gdl);

	if (1);

	switch (g_Vars.currentplayer->sighttracktype) {
	case SIGHTTRACKTYPE_NONE:
		// SIGHTTRACKTYPE_NONE is used for unarmed, but this appears to be
		// unreachable. The aimer is never drawn when unarmed.
		if (sighton) {
			colour = 0x00ff0028;
			radius = 8;
			cornergap = 5;
			gdl = sight_draw_aimer(gdl, x, y, radius, cornergap, colour);
		}
		break;
	case SIGHTTRACKTYPE_DEFAULT:
		// For most guns, render the aimer if holding R
		if (sighton) {
			if (g_Vars.currentplayer->lookingatprop.prop == NULL) {
				colour = 0x00ff0028;
				radius = 8;
				cornergap = 5;
			} else {
				colour = sight_is_prop_friendly(NULL) ? 0x0000ff60 : 0xff000060;
				radius = 6;
				cornergap = 3;
			}

			main_override_variable("sight", &sight);

			switch (sight) {
			case 0:
				gdl = sight_draw_aimer(gdl, x, y, radius, cornergap, colour);
				break;
			case 1:
				gdl = sight_draw_delayed_aimer(gdl, x, y, radius * 2, cornergap * 2, colour);
				break;
			}
		}
		break;
	case SIGHTTRACKTYPE_BETASCANNER:
		// An unused sight target. When holding R, it flashes the text
		// "Identify" and draws a red box around the targetted prop.
		if (sighton) {
			s32 textx;
			s32 texty;

			if (g_Vars.currentplayer->lookingatprop.prop == NULL) {
				colour = 0x00ff0028;
				radius = 8;
				cornergap = 5;
			} else {
				colour = sight_is_prop_friendly(NULL) ? 0x0000ff60 : 0xff000060;
				radius = 6;
				cornergap = 3;
			}

			textx = 135;
			texty = 200;

			identifytimer += g_Vars.lvupdate240;

			if (identifytimer & 0x80) {
				// "Identify"
				gdl = text_render_vx(gdl, &textx, &texty, lang_get(L_MISC_439),
						g_CharsHandelGothicXs, g_FontHandelGothicXs, 0x00ff00a0, 0x000000a0,
						vi_get_width(), vi_get_height(), 0, 0);
			}

			gdl = sight_draw_aimer(gdl, x, y, radius, cornergap, colour);

			if (g_Vars.currentplayer->lookingatprop.prop) {
				gdl = sight_draw_target_box(gdl, &g_Vars.currentplayer->lookingatprop, 1, g_Vars.currentplayer->targetset[0]);
			}
		}
		break;
	case SIGHTTRACKTYPE_ROCKETLAUNCHER:
		for (i = 0; i < 1; i++) {
			trackedprop = &g_Vars.currentplayer->trackedprops[i];

			if (trackedprop->prop) {
				gdl = sight_draw_target_box(gdl, trackedprop, 0, g_Vars.currentplayer->targetset[i]);
			}
		}

		if (sighton) {
			if (g_Vars.currentplayer->lookingatprop.prop == NULL) {
				colour = 0x00ff0028;
				radius = 8;
				cornergap = 5;
			} else {
				colour = sight_is_prop_friendly(NULL) ? 0x0000ff60 : 0xff000060;
				radius = 6;
				cornergap = 3;
			}

			gdl = sight_draw_aimer(gdl, x, y, radius, cornergap, colour);
		}
		break;
	case SIGHTTRACKTYPE_FOLLOWLOCKON:
	case SIGHTTRACKTYPE_THREATDETECTOR:
		for (i = 0; i < ARRAYCOUNT(g_Vars.currentplayer->trackedprops); i++) {
			trackedprop = &g_Vars.currentplayer->trackedprops[i];

			if (trackedprop->prop) {
				if (g_Vars.currentplayer->sighttracktype == SIGHTTRACKTYPE_THREATDETECTOR) {
					struct defaultobj *obj = trackedprop->prop->obj;
					struct weaponobj *weapon;
					u32 textid = 0;

					// @dangerous: There is no check here to see if the prop
					// type is obj. However, it's likely that only objs can be
					// in the cmdfollowprops list at this point, so it's
					// probably OK.
					if (obj && obj->type == OBJTYPE_AUTOGUN
							&& (obj->flags2 & (OBJFLAG2_AUTOGUN_MALFUNCTIONING | OBJFLAG2_AUTOGUN_WINDMILL)) == 0) {
						textid = L_GUN_215; // "AUTOGUN"
					}

					weapon = trackedprop->prop->weapon;

					if (weapon && weapon->base.type == OBJTYPE_WEAPON) {
						switch (weapon->weaponnum) {
						case WEAPON_GRENADE:
							// "PROXY" and "TIMED"
							textid = (weapon->gunfunc == FUNC_SECONDARY) ? L_GUN_212 : L_GUN_213;
							break;
						case WEAPON_NBOMB:
							// "PROXY" and "IMPACT"
							textid = (weapon->gunfunc == FUNC_SECONDARY) ? L_GUN_212 : L_GUN_216;
							break;
						case WEAPON_TIMEDMINE:
							textid = L_GUN_213; // "TIMED"
							break;
						case WEAPON_PROXIMITYMINE:
							textid = L_GUN_212; // "PROXY"
							break;
						case WEAPON_REMOTEMINE:
							textid = L_GUN_214; // "REMOTE"
							break;
						case WEAPON_DRAGON:
							if (weapon->gunfunc == FUNC_SECONDARY) {
								textid = L_GUN_212; // "PROXY"
							}
							break;
						}
					}

					gdl = sight_draw_target_box(gdl, trackedprop, textid, g_Vars.currentplayer->targetset[i]);
				} else {
					// CMP150-tracked prop
					gdl = sight_draw_target_box(gdl, trackedprop, i + 2, g_Vars.currentplayer->targetset[i]);
				}
			}
		}

		if (sighton) {
			if (g_Vars.currentplayer->lookingatprop.prop == NULL) {
				colour = 0x00ff0028;
				radius = 8;
				cornergap = 5;
			} else {
				colour = sight_is_prop_friendly(NULL) ? 0x0000ff60 : 0xff000060;
				radius = 6;
				cornergap = 3;
			}

			gdl = sight_draw_aimer(gdl, x, y, radius, cornergap, colour);
		}
		break;
	}

	gdl = text_end(gdl);

	return gdl;
}

Gfx *sight_draw_classic(Gfx *gdl, bool sighton)
{
	struct textureconfig *tconfig = &g_TexGeCrosshairConfigs[TEX_GECROSSHAIR_00];
	f32 spc4[2];
	f32 spbc[2];
	s32 x = g_Vars.currentplayer->crosspos[0];
	s32 y = g_Vars.currentplayer->crosspos[1];
	s32 x1;
	s32 x2;
	s32 y1;
	s32 y2;

	if (!sighton) {
		return gdl;
	}

	gDPSetColorDither(gdl++, G_CD_DISABLE);
	gDPSetTexturePersp(gdl++, G_TP_NONE);
	gDPSetAlphaCompare(gdl++, G_AC_NONE);
	gDPSetTextureLOD(gdl++, G_TL_TILE);
	gDPSetTextureFilter(gdl++, G_TF_POINT);
	gDPSetTextureConvert(gdl++, G_TC_FILT);
	gDPSetTextureLUT(gdl++, G_TT_NONE);
	gDPPipeSync(gdl++);
	gDPSetCycleType(gdl++, G_CYC_1CYCLE);
	gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
	gDPSetPrimColor(gdl++, 0, 0, 0x00, 0x00, 0x00, 0x00);

	x1 = x - (tconfig->width >> 1);
	y1 = y - (tconfig->height >> 1);
	x2 = x + (tconfig->width >> 1);
	y2 = y + (tconfig->height >> 1);

	gDPFillRectangle(gdl++, x1, y1, x2, y2);

	spc4[0] = x;
	spc4[1] = y;

	spbc[0] = (tconfig->width >> 1) * (f32)g_UiScaleX;
	spbc[1] = tconfig->height >> 1;

	tex_select(&gdl, tconfig, 2, 0, 0, 1, NULL);

	func0f0b278c(&gdl, spc4, spbc, tconfig->width, tconfig->height,
			0, 0, 1, 0xff, 0xff, 0xff, 0x7f, tconfig->level > 0, 0);

	gDPPipeSync(gdl++);
	gDPSetColorDither(gdl++, G_CD_BAYER);
	gDPSetTexturePersp(gdl++, G_TP_PERSP);
	gDPSetAlphaCompare(gdl++, G_AC_NONE);
	gDPSetTextureLOD(gdl++, G_TL_LOD);
	gDPSetTextureFilter(gdl++, G_TF_BILERP);
	gDPSetTextureConvert(gdl++, G_TC_FILT);
	gDPSetTextureLUT(gdl++, G_TT_NONE);

	return gdl;
}

Gfx *sight_draw_type2(Gfx *gdl, bool sighton)
{
	return sight_draw_classic(gdl, sighton);
}

#define COLOUR_LIGHTRED 0xff555564
#define COLOUR_DARKRED  0xff0000b2
#define COLOUR_GREEN    0x55ff5564
#define COLOUR_DARKBLUE 0x0000ff60

#define DIR_UP    0
#define DIR_DOWN  1
#define DIR_LEFT  2
#define DIR_RIGHT 3

Gfx *sight_draw_skedar_triangle(Gfx *gdl, s32 x, s32 y, s32 dir, u32 colour)
{
	s32 points[6];
	Vtx *vertices = gfx_allocate_vertices(3);
	Col *colours = gfx_allocate_colours(2);

	switch (dir) {
	case DIR_UP:
		points[0] = x;
		points[1] = y;
		points[2] = x + 5;
		points[3] = y + 7;
		points[4] = x - 5;
		points[5] = y + 7;
		break;
	case DIR_DOWN:
		points[0] = x;
		points[1] = y;
		points[2] = x + 5;
		points[3] = y - 7;
		points[4] = x - 5;
		points[5] = y - 7;
		break;
	case DIR_LEFT:
		points[0] = x;
		points[1] = y;
		points[2] = x + 7;
		points[3] = y - 5;
		points[4] = x + 7;
		points[5] = y + 5;
		break;
	case DIR_RIGHT:
		points[0] = x;
		points[1] = y;
		points[2] = x - 7;
		points[3] = y - 5;
		points[4] = x - 7;
		points[5] = y + 5;
		break;
	default:
		return gdl;
	}

	vertices[0].x = points[0] * 10;
	vertices[0].y = points[1] * 10;
	vertices[0].z = -10;
	vertices[1].x = points[2] * 10;
	vertices[1].y = points[3] * 10;
	vertices[1].z = -10;
	vertices[2].x = points[4] * 10;
	vertices[2].y = points[5] * 10;
	vertices[2].z = -10;

	// @bug: This also needs to check for COLOUR_LIGHTRED because the caller can
	// use two shades of red. The second colour is used when zeroing the sight
	// in on a new target. Because of this bug, targeting an ally with the
	// Mauler or Reaper will show a red crosshair while it's still zeroing.
	if (colour == COLOUR_DARKRED && sight_is_prop_friendly(NULL)) {
		colour = COLOUR_DARKBLUE;
	}

#define RGBA(r, g, b, a) (((r) & 0xff) << 24 | ((g) & 0xff) << 16 | ((b) & 0xff) << 8 | ((a) & 0xff))

	colours[0].word = colour;
	colours[1].word = RGBA((colour >> 24) & 0xff, (colour >> 16) & 0xff, (colour >> 8) & 0xff, 0x08);

	vertices[0].colour = 0;
	vertices[1].colour = 4;
	vertices[2].colour = 4;

	gSPColor(gdl++, colours, 2);
	gSPVertex(gdl++, vertices, 3, 0);
	gSPTri1(gdl++, 0, 1, 2);

	return gdl;
}

Gfx *sight_draw_skedar(Gfx *gdl, bool sighton)
{
	s32 viewleft = vi_get_view_left() / g_UiScaleX;
	s32 viewtop = vi_get_view_top();
	s32 viewwidth = vi_get_view_width() / g_UiScaleX;
	s32 viewheight = vi_get_view_height();
	s32 viewright = viewleft + viewwidth - 1;
	s32 viewbottom = viewtop + viewheight - 1;
	s32 paddingy = viewheight / 4;
	s32 paddingx = viewwidth / 4;
	s32 x = (s32) (g_Vars.currentplayer->crosspos[0] / g_UiScaleX);
	s32 trix1;
	s32 trix2;
	s32 y = g_Vars.currentplayer->crosspos[1];
	s32 triy2;
	s32 triy1;
	u32 colour;
	u8 dir;
	bool hasprop = g_Vars.currentplayer->lookingatprop.prop != NULL;
	f32 frac;

	if (!sighton) {
		return gdl;
	}

	if (!hasprop) {
		g_Vars.currentplayer->sighttimer240 = 0;
	}

	gdl = ortho_begin(gdl);

	gSPClearGeometryMode(gdl++, G_CULL_BOTH);
	gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
	gDPSetCombineMode(gdl++, G_CC_SHADE, G_CC_SHADE);
	gDPSetTextureFilter(gdl++, G_TF_BILERP);
	gDPSetCycleType(gdl++, G_CYC_1CYCLE);
	gDPSetRenderMode(gdl++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);

	trix1 = x;
	triy1 = y;
	trix2 = x;
	triy2 = y;

	if (hasprop && g_Vars.currentplayer->sighttimer240 < TICKS(48)) {
		frac = g_Vars.currentplayer->sighttimer240 / TICKS(48.0f);
	}

	// Outer top triangle
	if (!hasprop) {
		colour = COLOUR_LIGHTRED;

		if (x < viewleft + paddingx) {
			// Aiming far left
			dir = DIR_LEFT;
			trix1 = viewleft + paddingx;
		} else if (x > viewright - paddingx) {
			// Aiming far right
			dir = DIR_RIGHT;
			trix1 = viewright - paddingx;
		} else {
			// Aiming within the bounds
			dir = DIR_DOWN;
			colour = COLOUR_GREEN;
		}

		triy1 = viewtop + paddingy;
	} else {
		if (g_Vars.currentplayer->sighttimer240 < TICKS(48)) {
			// Zeroing on a prop
			colour = COLOUR_LIGHTRED;
			dir = DIR_DOWN;
			triy1 = (y - viewtop - paddingy - 2) * frac + viewtop + paddingy;
		} else {
			// Zeroed on a prop
			colour = COLOUR_DARKRED;
			dir = DIR_DOWN;
			triy1 = y - 2;
		}
	}

	gdl = sight_draw_skedar_triangle(gdl, trix1, triy1, dir, colour);

	// Outer bottom triangle
	if (!hasprop) {
		colour = COLOUR_LIGHTRED;

		if (dir == DIR_DOWN) {
			colour = COLOUR_GREEN;
			dir = DIR_UP;
		}

		triy1 = viewbottom - paddingy;
	} else {
		if (g_Vars.currentplayer->sighttimer240 < TICKS(48)) {
			// Zeroing on a prop
			colour = COLOUR_LIGHTRED;
			dir = DIR_UP;
			triy1 = (y - viewbottom + paddingy + 2) * frac + viewbottom - paddingy;
		} else {
			// Zeroed on a prop
			colour = COLOUR_DARKRED;
			dir = DIR_UP;
			triy1 = y + 2;
		}
	}

	gdl = sight_draw_skedar_triangle(gdl, trix1, triy1, dir, colour);

	// Outer right triangle
	if (!hasprop) {
		colour = COLOUR_LIGHTRED;

		if (y < viewtop + paddingy) {
			// Aiming far up
			dir = DIR_UP;
			triy2 = viewtop + paddingy;
		} else if (y > viewbottom - paddingy) {
			// Aiming far down
			dir = DIR_DOWN;
			triy2 = viewbottom - paddingy;
		} else {
			// Aiming within the bounds
			dir = DIR_LEFT;
			colour = COLOUR_GREEN;
		}

		trix2 = viewright - paddingx;
	} else {
		if (g_Vars.currentplayer->sighttimer240 < TICKS(48)) {
			// Zeroing on a prop
			colour = COLOUR_LIGHTRED;
			dir = DIR_LEFT;
			trix2 = (x - viewright + paddingx + 2) * frac + viewright - paddingx;
		} else {
			colour = COLOUR_DARKRED;
			// Zeroed on a prop
			dir = DIR_LEFT;
			trix2 = x + 2;
		}
	}

	gdl = sight_draw_skedar_triangle(gdl, trix2, triy2, dir, colour);

	// Outer left triangle
	if (!hasprop) {
		colour = COLOUR_LIGHTRED;

		if (dir == DIR_LEFT) {
			colour = COLOUR_GREEN;
			dir = DIR_RIGHT;
		}

		trix2 = viewleft + paddingx;
	} else {
		if (g_Vars.currentplayer->sighttimer240 < TICKS(48)) {
			// Zeroing on a prop
			colour = COLOUR_LIGHTRED;
			dir = DIR_RIGHT;
			trix2 = (x - viewleft - paddingx - 2) * frac + viewleft + paddingx;
		} else {
			// Zeroed on a prop
			colour = COLOUR_DARKRED;
			dir = DIR_RIGHT;
			trix2 = x - 2;
		}
	}

	gdl = sight_draw_skedar_triangle(gdl, trix2, triy2, dir, colour);

	// Inner triangles
	if (!hasprop || g_Vars.currentplayer->sighttimer240 < TICKS(48)) {
		colour = hasprop ? COLOUR_LIGHTRED : COLOUR_GREEN;

		gdl = sight_draw_skedar_triangle(gdl, x + 0, y - 2, DIR_DOWN, colour);
		gdl = sight_draw_skedar_triangle(gdl, x + 0, y + 2, DIR_UP, colour);
		gdl = sight_draw_skedar_triangle(gdl, x - 2, y + 0, DIR_RIGHT, colour);
		gdl = sight_draw_skedar_triangle(gdl, x + 2, y + 0, DIR_LEFT, colour);
	}

	gdl = ortho_end(gdl);

	return gdl;
}

Gfx *sight_draw_zoom(Gfx *gdl, bool sighton)
{
	s32 viewleft = vi_get_view_left() / g_UiScaleX;
	s32 viewtop = vi_get_view_top();
	s32 viewhalfwidth = (vi_get_view_width() / g_UiScaleX) >> 1;
	s32 viewhalfheight = vi_get_view_height() >> 1;
	s32 viewright = viewleft + viewhalfwidth * 2 - 1;
	s32 viewbottom = viewtop + viewhalfheight * 2 - 1;
	f32 maxfovy;
	s32 availableabove;
	s32 availablebelow;
	s32 availableleft;
	s32 availableright;
	f32 zoominfovy;
	f32 frac;
	f32 marginright;
	f32 margintop;
	f32 marginbottom;
	f32 marginleft;
	s32 cornerwidth;
	s32 cornerheight;
	s32 weaponnum;
	u8 showzoomrange;

	// The 48, 49 and 10 numbers are padding values. When zoomed in, the left
	// corner will be 48px from the viewport's left edge. The available values
	// are the zoomable range from the padding to the middle of the viewport.
	availableleft = viewhalfwidth - 48;
	availableright = viewhalfwidth - 49;
	availableabove = viewhalfheight - 10;
	availablebelow = viewhalfheight - 10;
	frac = 1.0f;
	weaponnum = g_Vars.currentplayer->hands[HAND_RIGHT].gset.weaponnum;
	cornerwidth = (viewhalfwidth >> 1) - 60;
	cornerheight = (viewhalfheight >> 1) - 22;

	showzoomrange = options_get_show_zoom_range(g_Vars.currentplayerstats->mpindex)
		&& options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex);

	maxfovy = gset_get_gun_zoom_fov();
	zoominfovy = g_Vars.currentplayer->zoominfovy;

	if (maxfovy == 0.0f || maxfovy == 60.0f) {
		if (weaponnum != WEAPON_SNIPERRIFLE) {
			showzoomrange = false;
		}
	} else {
		frac = maxfovy / zoominfovy;
	}

	if (showzoomrange) {
		gdl = text_begin(gdl);
		gdl = text_begin_boxmode(gdl, 0x00ff0028);

		if (frac < 0.2f) {
			cornerwidth *= 0.2f;
			cornerheight *= 0.2f;
		} else {
			cornerwidth *= frac;
			cornerheight *= frac;
		}

		if (PLAYERCOUNT() >= 2) {
			cornerheight *= 2;
		}

		if (cornerwidth < 5) {
			cornerwidth = 5;
		}

		if (cornerheight < 5) {
			cornerheight = 5;
		}

		// Margin is the gap from the viewport edge to the zoom box
		marginleft = viewhalfwidth - availableleft * frac;
		marginright = viewhalfwidth - availableright * frac;
		marginbottom = viewhalfheight - availablebelow * frac;
		margintop = viewhalfheight - availableabove * frac;

#define BOXLEFT   (viewleft + marginleft)
#define BOXRIGHT  (viewright - marginright)
#define BOXBOTTOM (viewbottom - marginbottom)
#define BOXTOP    (viewtop + margintop)

		if (cornerwidth > BOXRIGHT - BOXLEFT) {
			cornerwidth = BOXRIGHT - BOXLEFT;
		}

		if (cornerheight > BOXBOTTOM - BOXTOP) {
			cornerheight = BOXBOTTOM - BOXTOP;
		}

		// Top left
		gDPHudRectangle(gdl++, BOXLEFT + 1, BOXTOP, BOXLEFT + cornerwidth - 1, BOXTOP);
		gDPHudRectangle(gdl++, BOXLEFT, BOXTOP, BOXLEFT, BOXTOP + cornerheight - 1);

		// Top right
		gDPHudRectangle(gdl++, BOXRIGHT - cornerwidth + 2, BOXTOP, BOXRIGHT - 1, BOXTOP);
		gDPHudRectangle(gdl++, BOXRIGHT, BOXTOP, BOXRIGHT, BOXTOP + cornerheight - 1);

		// Bottom left
		gDPHudRectangle(gdl++, BOXLEFT + 1, BOXBOTTOM, BOXLEFT + cornerwidth - 1, BOXBOTTOM);
		gDPHudRectangle(gdl++, BOXLEFT, BOXBOTTOM - cornerheight + 1, BOXLEFT, BOXBOTTOM);

		// Bottom right
		gDPHudRectangle(gdl++, BOXRIGHT - cornerwidth + 2, BOXBOTTOM, BOXRIGHT - 1, BOXBOTTOM);
		gDPHudRectangle(gdl++, BOXRIGHT, BOXBOTTOM - cornerheight + 1, BOXRIGHT, BOXBOTTOM);

		// Draw over the corners again, but only half as wide/high
		cornerwidth >>= 1;
		cornerheight >>= 1;

		// Top left
		gDPHudRectangle(gdl++, BOXLEFT, BOXTOP, BOXLEFT + cornerwidth, BOXTOP);
		gDPHudRectangle(gdl++, BOXLEFT, BOXTOP, BOXLEFT, BOXTOP + cornerheight);

		// Top right
		gDPHudRectangle(gdl++, BOXRIGHT - cornerwidth, BOXTOP, BOXRIGHT, BOXTOP);
		gDPHudRectangle(gdl++, BOXRIGHT, BOXTOP, BOXRIGHT, BOXTOP + cornerheight);

		// Bottom left
		gDPHudRectangle(gdl++, BOXLEFT, BOXBOTTOM, BOXLEFT + cornerwidth, BOXBOTTOM);
		gDPHudRectangle(gdl++, BOXLEFT, BOXBOTTOM - cornerheight, BOXLEFT, BOXBOTTOM);

		// Bottom right
		gDPHudRectangle(gdl++, BOXRIGHT - cornerwidth, BOXBOTTOM, BOXRIGHT, BOXBOTTOM);
		gDPHudRectangle(gdl++, BOXRIGHT, BOXBOTTOM - cornerheight, BOXRIGHT, BOXBOTTOM);

		gdl = text_end_boxmode(gdl);
		gdl = text_end(gdl);
	}

	gdl = sight_draw_default(gdl, sighton);

	return gdl;
}

Gfx *sight_draw_maian(Gfx *gdl, bool sighton)
{
	s32 viewleft = vi_get_view_left() / g_UiScaleX;
	s32 viewtop = vi_get_view_top();
	s32 viewwidth = vi_get_view_width() / g_UiScaleX;
	s32 viewheight = vi_get_view_height();
	s32 viewright = viewleft + viewwidth - 1;
	s32 viewbottom = viewtop + viewheight - 1;
	s32 x = (s32)g_Vars.currentplayer->crosspos[0] / g_UiScaleX;
	s32 y = g_Vars.currentplayer->crosspos[1];
	Vtx *vertices;
	Col *colours;
	s32 inner[4];
	bool hasprop = g_Vars.currentplayer->lookingatprop.prop != NULL;
	u32 colour = 0xff000060;

	if (!sighton) {
		return gdl;
	}

	if (sight_is_prop_friendly(NULL)) {
		colour = 0x0000ff60;
	}

	vertices = gfx_allocate_vertices(8);
	colours = gfx_allocate_colours(2);
	gdl = ortho_begin(gdl);

	gSPClearGeometryMode(gdl++, G_CULL_BOTH);
	gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
	gDPSetCombineMode(gdl++, G_CC_SHADE, G_CC_SHADE);
	gDPSetTextureFilter(gdl++, G_TF_BILERP);
	gDPSetCycleType(gdl++, G_CYC_1CYCLE);
	gDPSetRenderMode(gdl++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);

	vertices[0].x = (viewleft + (viewwidth >> 1)) * 10;
	vertices[0].y = (viewtop + 10) * 10;
	vertices[0].z = -10;
	vertices[1].x = (viewleft + (viewwidth >> 1)) * 10;
	vertices[1].y = (viewbottom - 10) * 10;
	vertices[1].z = -10;
	vertices[2].x = (viewleft + 48) * 10;
	vertices[2].y = (viewtop + (viewheight >> 1)) * 10;
	vertices[2].z = -10;
	vertices[3].x = (viewright - 49) * 10;
	vertices[3].y = (viewtop + (viewheight >> 1)) * 10;
	vertices[3].z = -10;

	inner[0] = x + 4;
	inner[1] = x - 4;
	inner[2] = y + 4;
	inner[3] = y - 4;

	vertices[4].x = inner[1] * 10;
	vertices[4].y = inner[3] * 10;
	vertices[4].z = -10;
	vertices[5].x = inner[0] * 10;
	vertices[5].y = inner[3] * 10;
	vertices[5].z = -10;
	vertices[6].x = inner[0] * 10;
	vertices[6].y = inner[2] * 10;
	vertices[6].z = -10;
	vertices[7].x = inner[1] * 10;
	vertices[7].y = inner[2] * 10;
	vertices[7].z = -10;

	colours[0].word = 0x00ff000f;
	colours[1].word = hasprop ? colour : 0x00ff0044;

	vertices[0].colour = 0;
	vertices[1].colour = 0;
	vertices[2].colour = 0;
	vertices[3].colour = 0;
	vertices[4].colour = 4;
	vertices[5].colour = 4;
	vertices[6].colour = 4;
	vertices[7].colour = 4;

	// Draw the main 4 triangles
	gSPColor(gdl++, colours, 2);
	gSPVertex(gdl++, vertices, 8, 0);
	gSPTri4(gdl++, 0, 4, 5, 5, 3, 6, 7, 6, 1, 4, 7, 2);

	gdl = ortho_end(gdl);
	gdl = text_begin_boxmode(gdl, 0x00ff0028);

	// Draw border over inner points
	gDPHudRectangle(gdl++, x - 4, y - 4, x - 4, y + 4); // left
	gDPHudRectangle(gdl++, x + 4, y - 4, x + 4, y + 4); // right
	gDPHudRectangle(gdl++, x - 4, y - 4, x + 4, y - 4); // top
	gDPHudRectangle(gdl++, x - 4, y + 4, x + 4, y + 4); // bottom

	gdl = text_end_boxmode(gdl);

	return gdl;
}

Gfx *sight_draw_target(Gfx *gdl)
{
	s32 x = (s32)g_Vars.currentplayer->crosspos[0] / g_UiScaleX;
	s32 y = g_Vars.currentplayer->crosspos[1];

	static u32 var80070f9c = 0x00ff00ff;
	static u32 var80070fa0 = 0x00ff0011;

	main_override_variable("sout", &var80070f9c);
	main_override_variable("sin", &var80070fa0);

	gdl = text_begin_boxmode(gdl, 0x00ff0028);

	gDPHudRectangle(gdl++, x + 2, y + 0, x + 6, y + 0);
	gDPHudRectangle(gdl++, x + 2, y + 0, x + 4, y + 0);
	gDPHudRectangle(gdl++, x - 6, y + 0, x - 2, y + 0);
	gDPHudRectangle(gdl++, x - 4, y + 0, x - 2, y + 0);
	gDPHudRectangle(gdl++, x + 0, y + 2, x + 0, y + 6);
	gDPHudRectangle(gdl++, x + 0, y + 2, x + 0, y + 4);
	gDPHudRectangle(gdl++, x + 0, y - 6, x + 0, y - 2);
	gDPHudRectangle(gdl++, x + 0, y - 4, x + 0, y - 2);

	gdl = text_end_boxmode(gdl);

	return gdl;
}

bool sight_has_target_while_aiming(s32 sight)
{
	if (sight == SIGHT_DEFAULT || sight == SIGHT_ZOOM) {
		return true;
	}

	return false;
}

/**
 * sighton is true if the player is using the aimer (ie. holding R).
 */
Gfx *sight_draw(Gfx *gdl, bool sighton, s32 sight)
{
	if (sight);

	if (g_Vars.currentplayer->activemenumode != AMMODE_CLOSED) {
		return gdl;
	}

	if (g_Vars.currentplayer->gunctrl.passivemode) {
		return gdl;
	}

#if PAL
	g_UiScaleX = 1;
#else
	if (g_ViRes == VIRES_HI) {
		g_UiScaleX = 2;
	} else {
		g_UiScaleX = 1;
	}
#endif

	if (PLAYERCOUNT() >= 2 && g_Vars.coopplayernum < 0 && g_Vars.antiplayernum < 0) {
		sight = SIGHT_DEFAULT;
	}

	sight_tick(sighton);

	switch (sight) {
	case SIGHT_DEFAULT:
		gdl = sight_draw_default(gdl, sighton && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex));
		break;
	case SIGHT_CLASSIC:
		gdl = sight_draw_classic(gdl, sighton && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex));
		break;
	case SIGHT_2:
		gdl = sight_draw_type2(gdl, sighton && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex));
		break;
	case SIGHT_3:
		gdl = sight_draw_default(gdl, sighton && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex));
		break;
	case SIGHT_SKEDAR:
		gdl = sight_draw_skedar(gdl, sighton && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex));
		break;
	case SIGHT_ZOOM:
		gdl = sight_draw_zoom(gdl, sighton && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex));
		break;
	case SIGHT_MAIAN:
		gdl = sight_draw_maian(gdl, sighton && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex));
		break;
	default:
		gdl = sight_draw_default(gdl, sighton && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex));
		break;
	case SIGHT_NONE:
		break;
	}

	if (sight != SIGHT_NONE && options_get_sight_on_screen(g_Vars.currentplayerstats->mpindex)) {
		if ((options_get_always_show_target(g_Vars.currentplayerstats->mpindex) && !sighton)
				|| (sighton && sight_has_target_while_aiming(sight))) {
			gdl = sight_draw_target(gdl);
		}
	}

	g_UiScaleX = 1;

	return gdl;
}
