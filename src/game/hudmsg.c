#include <ultra64.h>
#include "constants.h"
#include "constants.h"
#include "game/propsnd.h"
#include "game/gset.h"
#include "game/player.h"
#include "game/savebuffer.h"
#include "game/hudmsg.h"
#include "game/menugfx.h"
#include "game/playermgr.h"
#include "game/text.h"
#include "game/lv.h"
#include "game/mplayer/mplayer.h"
#include "game/options.h"
#include "game/propobj.h"
#include "bss.h"
#include "lib/lib_317f0.h"
#include "lib/memp.h"
#include "lib/mtx.h"
#include "lib/snd.h"
#include "lib/str.h"
#include "lib/vi.h"
#include "data.h"
#include "types.h"
#include "string.h"

u32 g_NextHudMessageId;

u8 g_HudmsgsActive = 0;

u32 g_HudmsgColours[] = {
	/* 0*/ 0x00ff0000, // green
	/* 1*/ 0x9999ff00, // pastel blue
	/* 2*/ 0xffffff00, // white
	/* 3*/ 0xff777700, // pastel red
	/* 4*/ 0xffff5500, // yellow
	/* 5*/ 0x00ff0000, // green
	/* 6*/ 0xcccccc00, // gray
	/* 7*/ 0xff888800, // pastel red
	/* 8*/ 0xffaa5500, // orange
	/* 9*/ 0x55aaff00, // sky blue
	/*10*/ 0xaa55ff00, // purple
};

s32 g_HudPaddingY = 10;
s32 g_HudPaddingX = 24;
s32 g_NumHudMessages = 0;
struct hudmessage *g_HudMessages = NULL;

#if VERSION < VERSION_NTSC_1_0
struct sndstate *var800736b0nb = NULL;
#endif

struct hudmsgtype g_HudmsgTypes[] = {
	/* 0*/ { 1, 1, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ff0000, 0x000000a0, HUDMSGALIGN_LEFT,    HUDMSGALIGN_BOTTOM,        0, 0, 80  },
	/* 1*/ { 0, 1, 0, &g_CharsHandelGothicMd, &g_FontHandelGothicMd, 0x00ff0000, 0x000000a0, HUDMSGALIGN_XMIDDLE, HUDMSGALIGN_YMIDDLE,       0, 0, 120 },
#if VERSION == VERSION_JPN_FINAL
	/* 2*/ { 0, 0, 1, &g_CharsHandelGothicMd, &g_FontHandelGothicMd, 0xff999900, 0xffffffa0, HUDMSGALIGN_XMIDDLE, HUDMSGALIGN_YMIDDLE,       0, 0, 120 },
#else
	/* 2*/ { 0, 0, 1, &g_CharsHandelGothicMd, &g_FontHandelGothicMd, 0xff000000, 0xffffffa0, HUDMSGALIGN_XMIDDLE, HUDMSGALIGN_YMIDDLE,       0, 0, 120 },
#endif
	/* 3*/ { 0, 1, 0, &g_CharsHandelGothicMd, &g_FontHandelGothicMd, 0x00ff0000, 0x000000a0, HUDMSGALIGN_LEFT,    HUDMSGALIGN_BOTTOM,        0, 0, 120 },
	/* 4*/ { 1, 1, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ffc000, 0x000000a0, HUDMSGALIGN_LEFT,    HUDMSGALIGN_BOTTOM,        0, 0, 40  },
	/* 5*/ { 0, 0, 0, &g_CharsHandelGothicMd, &g_FontHandelGothicMd, 0x00ff0000, 0x000000a0, HUDMSGALIGN_LEFT,    HUDMSGALIGN_TOP,           0, 0, 120 },
	/* 6*/ { 1, 0, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ff0000, 0x000000a0, HUDMSGALIGN_XMIDDLE, HUDMSGALIGN_TOP,           0, 0, 120 },
	/* 7*/ { 1, 1, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ff0000, 0x000000a0, HUDMSGALIGN_XMIDDLE, HUDMSGALIGN_TOP,           0, 0, -1  },
	/* 8*/ { 1, 1, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ffc000, 0x000000a0, HUDMSGALIGN_XMIDDLE, HUDMSGALIGN_BOTTOM,        0, 0, 500 },
#if VERSION == VERSION_JPN_FINAL
	/* 9*/ { 1, 1, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ff0000, 0x000000a0, HUDMSGALIGN_LEFT,    HUDMSGALIGN_BOTTOM,        0, 0, 120 },
#else
	/* 9*/ { 1, 1, 0, &g_CharsHandelGothicXs, &g_FontHandelGothicXs, 0x00ff0000, 0x000000a0, HUDMSGALIGN_LEFT,    HUDMSGALIGN_BOTTOM,        0, 0, 120 },
#endif
	/*10*/ { 1, 1, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ff0000, 0x000000a0, HUDMSGALIGN_LEFT,    HUDMSGALIGN_BOTTOM,        0, 0, 240 },
#if VERSION >= VERSION_NTSC_1_0
	/*11*/ { 0, 0, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ff0000, 0x000000a0, HUDMSGALIGN_XMIDDLE, HUDMSGALIGN_BELOWVIEWPORT, 0, 0, 120 },
#else
	/*11*/ { 1, 0, 0, &g_CharsHandelGothicSm, &g_FontHandelGothicSm, 0x00ff0000, 0x000000a0, HUDMSGALIGN_XMIDDLE, HUDMSGALIGN_BELOWVIEWPORT, 0, 0, 120 },
#endif
};

void hudmsg_create_from_args(char *text, s32 type, s32 conf00, s32 conf01, s32 conf02,
		struct fontchar **conf04, struct font **conf08,
		u32 textcolour, u32 glowcolour,
		u32 alignh, s32 conf16, u32 alignv, s32 conf18, s32 arg14, u32 flags);

u8 hudmsgs_are_active(void)
{
	return g_HudmsgsActive;
}

s32 hudmsg_is_zoom_range_visible(void)
{
	return options_get_show_zoom_range(g_Vars.currentplayerstats->mpindex)
		&& (PLAYERCOUNT() == 1
				|| !g_Vars.mplayerisrunning
				|| g_Vars.coopplayernum >= 0
				|| g_Vars.antiplayernum >= 0)
		&& var80075d60 == 2
		&& gset_get_sight() == SIGHT_ZOOM
		&& g_Vars.currentplayer->cameramode != CAMERAMODE_EYESPY
		&& g_Vars.currentplayer->cameramode != CAMERAMODE_THIRDPERSON;
}

/**
 * hudmsg_render_mission_timer calls vi_get_width (which returns an s16), then stores
 * the width in sp42 while it calls vi_get_height. However, when we do this it
 * stores the width to sp40 instead.
 *
 * Changing the definition of vi_get_height to return an s32 fixes this, but is
 * surely wrong and creates mismatches elsewhere. So we declare a new function
 * with the return type we need, and link it to the same address as vi_get_height
 * via the linker config.
 */
extern s32 vi_get_height_hack(void);

Gfx *hudmsg_render_mission_timer(Gfx *gdl, u32 alpha)
{
	s32 x;
	s32 y;
	s32 viewleft;
	s32 timery;
	char buffer[24];
	u32 textcolour;
	s32 is4mb;
	s32 playercount;
	s32 playernum;
	s16 viewtop;
	s16 viewheight;

	textcolour = alpha;

	viewleft = vi_get_view_left() / g_UiScaleX;
	viewtop = vi_get_view_top();
	viewheight = vi_get_view_height();
	playercount = PLAYERCOUNT();
	playernum = g_Vars.currentplayernum;

	timery = viewheight;
	timery += viewtop;
	timery -= g_HudPaddingY;
	timery -= 8;

	is4mb = IS4MB();

	// @bug: There is no check for playercount >= 2 in the next two statements.
	// Because of this, in 1 player the timer is drawn out of place when the
	// screen split option is vertical and either the countdown timer is visible
	// or a zoomable weapon is in use.
	if ((is4mb || options_get_screen_split() == SCREENSPLIT_VERTICAL) && countdown_timer_is_visible()) {
		timery -= 8;
	}

	if ((IS4MB() || options_get_screen_split() == SCREENSPLIT_VERTICAL || playercount >= 3) && hudmsg_is_zoom_range_visible()) {
		timery -= 8;
	}

	if (playercount == 2) {
		if (IS4MB() || (options_get_screen_split() != SCREENSPLIT_VERTICAL && playernum == 0)) {
			timery += 10;
		} else {
			timery += 2;
		}
	} else if (playercount >= 3) {
		if (playernum < 2) {
			timery += 10;
		} else {
			timery += 2;
		}
	} else {
		if (options_get_effective_screen_size() != SCREENSIZE_FULL) {
			timery += 8;
		}
	}

	// If this is a second player with their viewport on the right side of the
	// screen, move the timer left a bit as the safe zone doesn't need to be
	// considered.
	if (playercount == 2 && (options_get_screen_split() == SCREENSPLIT_VERTICAL || IS4MB()) && playernum == 1) {
		viewleft -= 14;
	} else if (playercount >= 3 && (playernum & 1) == 1) {
		viewleft -= 14;
	}

	textcolour = textcolour * 160 / 255;
	if (g_Is4Mb);
	textcolour |= 0x00ff0000;

	format_time(buffer, player_get_mission_time(), TIMEPRECISION_HUNDREDTHS);

	x = viewleft + g_HudPaddingX + 3;
	y = timery;

	gdl = text_render_v1(gdl, &x, &y, buffer, g_CharsNumeric, g_FontNumeric, textcolour, 0x000000a0, vi_get_width(), vi_get_height_hack(), 0, 0);

	return gdl;
}

Gfx *hudmsg_render_zoom_range(Gfx *gdl, u32 alpha)
{
	s32 viewtop;
	s32 viewleft;
	s32 viewhalfwidth;
	s32 viewheight;
	f32 zoominfovy;
	f32 zoomfov;
	s32 playercount;
	f32 curzoom;
	f32 maxzoom;
	char text[24];
	s32 weaponnum;
	s32 texty;
	s32 x;
	s32 y;
	s32 textwidth;
	s32 textheight;
	s32 x2;
	s32 y2;
	u32 colour;

	colour = (alpha * 0xa0 / 255) | 0x00ff0000;
	viewtop = vi_get_view_top();
	viewleft = vi_get_view_left() / g_UiScaleX;
	viewhalfwidth = (vi_get_view_width() / g_UiScaleX) >> 1;
	viewheight = vi_get_view_height();
	texty = viewheight + viewtop - 1;
	maxzoom = 1.0f;
	weaponnum = g_Vars.currentplayer->hands[0].gset.weaponnum;
	playercount = PLAYERCOUNT();

	texty -= 17;

	if (countdown_timer_is_visible()) {
		texty -= 8;
	}

	if (playercount == 2) {
		if (IS4MB() || (options_get_screen_split() != SCREENSPLIT_VERTICAL && g_Vars.currentplayernum == 0)) {
			texty += 10;
		} else {
			texty += 2;
		}
	} else if (playercount >= 3) {
		if (g_Vars.currentplayernum < 2) {
			texty += 10;
		} else {
			texty += 2;
		}
	} else if (options_get_effective_screen_size() != SCREENSIZE_FULL) {
		texty += 8;
	}

	// Left side - current zoom level
	zoomfov = gset_get_gun_zoom_fov();
	zoominfovy = g_Vars.currentplayer->zoominfovy;

	if (zoomfov == 0.0f || zoomfov == 60.0f) {
		if (weaponnum == WEAPON_SNIPERRIFLE) {
			curzoom = 1.0f;
		} else {
			return gdl;
		}
	} else {
		maxzoom = 60.0f / zoomfov;
		curzoom = maxzoom - 1.0f / (zoomfov / zoominfovy) + 1;
	}

	sprintf(text, "%s%s%4.2fX", "", "", curzoom);
	text_measure(&textheight, &textwidth, text, g_CharsNumeric, g_FontNumeric, 0);

	x = viewleft + viewhalfwidth - textwidth - 5;
	y = texty;
	x2 = x + textwidth;
	y2 = y + textheight;

	gdl = text_draw_black_uibox(gdl, &x, &y, &x2, &y2);
	gdl = text_render_v1(gdl, &x, &y, text, g_CharsNumeric, g_FontNumeric, colour, 0x000000a0, vi_get_width(), vi_get_height(), 0, 0);

	// Divider
	sprintf(text, "/");
	text_measure(&textheight, &textwidth, text, g_CharsNumeric, g_FontNumeric, 0);

	x = viewleft + viewhalfwidth - (textwidth >> 1);
	y = texty;
	x2 = x + textwidth;
	y2 = y + textheight;

	gdl = text_draw_black_uibox(gdl, &x, &y, &x2, &y2);
	gdl = text_render_v1(gdl, &x, &y, text, g_CharsNumeric, g_FontNumeric, colour, 0x000000a0, vi_get_width(), vi_get_height(), 0, 0);

	// Right side - max zoom level
	sprintf(text, "%s%s%4.2fX", "", "", maxzoom);
	text_measure(&textheight, &textwidth, text, g_CharsNumeric, g_FontNumeric, 0);

	x = viewleft + viewhalfwidth + 5;
	y = texty;
	x2 = x + textwidth;
	y2 = y + textheight;

	gdl = text_draw_black_uibox(gdl, &x, &y, &x2, &y2);
	gdl = text_render_v1(gdl, &x, &y, text, g_CharsNumeric, g_FontNumeric, colour, 0x000000a0, vi_get_width(), vi_get_height(), 0, 0);

	return gdl;
}

Gfx *hudmsg_render_box(Gfx *gdl, s32 x1, s32 y1, s32 x2, s32 y2, f32 bgopacity, u32 bordercolour, f32 textopacity)
{
	f32 f0;
	f32 f20;
	f32 f22;

	if (x1);

	g_HudmsgsActive = true;

	f0 = sinf(90 * bgopacity * DTOR(180) / 180.0f);
	f22 = (x2 - x1) * 0.5f;
	f20 = (y2 - y1) * 0.5f;

	if (f0 < 0.5f) {
		f20 = 0.0f;
		f22 *= f0 + f0;
	} else {
		f20 *= (f0 - 0.5f) + (f0 - 0.5f);
	}

	gdl = ortho_begin(gdl);

	gdl = menugfx_draw_filled_rect(gdl, x1, y1, x2, y1 + 1, bordercolour, bordercolour);
	gdl = menugfx_draw_filled_rect(gdl, x1, y2, x2, y2 + 1, bordercolour, bordercolour);
	gdl = menugfx_draw_filled_rect(gdl, x1, y1 + 1, x1 + 1, y2, bordercolour, bordercolour);
	gdl = menugfx_draw_filled_rect(gdl, x2, y1, x2 + 1, y2 + 1, bordercolour, bordercolour);

	gdl = ortho_end(gdl);

	if (textopacity > 0.0f) {
		f32 width = (x1 + x2) * 0.5f;
		f32 height = (y1 + y2) * 0.5f;

		gdl = text_draw_box(gdl,
				(s32)((width - f22) + 1.0f) * g_UiScaleX,
				(height - f20) + 1.0f,
				(s32)(width + f22) * g_UiScaleX,
				height + f20,
				128.0f * textopacity);
	}

	return gdl;
}

s32 hudmsg0f0ddb1c(s32 *arg0, s32 arg1)
{
#if VERSION >= VERSION_PAL_FINAL
	s32 viewwidth = vi_get_view_width() / g_UiScaleX;
	s32 result = 0;

	*arg0 = 24;

	if (PLAYERCOUNT() == 2
			&& options_get_screen_split() == SCREENSPLIT_VERTICAL
			&& (!g_InCutscene || g_MainIsEndscreen)) {
		result -= *arg0 * 2 / 3;

		if (g_Vars.currentplayernum == 0) {
			*arg0 /= 3;
		} else {
			*arg0 /= 6;
		}
	}

	result = result + viewwidth - *arg0 - arg1 - 11;

	if (PLAYERCOUNT() == 1 || (PLAYERCOUNT() == 2 && g_InCutscene && !g_MainIsEndscreen)) {
		result -= 16;

#if VERSION < VERSION_JPN_FINAL
		if (g_ViRes == VIRES_HI) {
			result -= 16;
		}
#endif
	}
#else
	s32 viewwidth = g_Vars.currentplayer->viewwidth / g_UiScaleX;
	s32 result = 0;

	*arg0 = 24;

	if (PLAYERCOUNT() == 2 && options_get_screen_split() == SCREENSPLIT_VERTICAL) {
		result -= *arg0 * 2 / 3;

		if (g_Vars.currentplayernum == 0) {
			*arg0 /= 3;
		} else {
			*arg0 /= 6;
		}
	}

	result = result + viewwidth - *arg0 - arg1 - 11;

	if (PLAYERCOUNT() == 1) {
		result -= 16;
	}
#endif

	return result;
}

void hudmsgs_hide_by_channel(s32 channelnum)
{
	s32 i;

	for (i = 0; i < g_NumHudMessages; i++) {
		if (g_HudMessages[i].state != HUDMSGSTATE_FREE && g_HudMessages[i].channelnum == channelnum) {
			g_HudMessages[i].flags |= HUDMSGFLAG_FORCEOFF;
			break;
		}
	}
}

void hudmsgs_reset(void)
{
	s32 i;

	g_NumHudMessages = g_Vars.mplayerisrunning ? 20 : 8;
	g_HudMessages = memp_alloc(ALIGN64(sizeof(struct hudmessage) * g_NumHudMessages), MEMPOOL_STAGE);

	for (i = 0; i < g_NumHudMessages; i++) {
		g_HudMessages[i].state = HUDMSGSTATE_FREE;
	}

	g_NextHudMessageId = 0;

#if VERSION < VERSION_NTSC_1_0
	var800736b0nb = NULL;
#endif
}

void hudmsg_remove_all(void)
{
	s32 i;

	for (i = 0; i < g_NumHudMessages; i++) {
		g_HudMessages[i].state = HUDMSGSTATE_FREE;
	}
}

s32 hudmsg_get_next(s32 refid)
{
	s32 bestid = -1;
	s32 bestindex = -1;
	s32 i;

	// Finding the smallest ID that is greater than refid
	for (i = 0; i < g_NumHudMessages; i++) {
		if (g_HudMessages[i].state && g_HudMessages[i].id > refid) {
			if (bestid < 0 || g_HudMessages[i].id < bestid) {
				bestindex = i;
				bestid = g_HudMessages[i].id;
			}
		}
	}

	return bestindex;
}

void hudmsg_create(char *text, s32 type)
{
	hudmsg_create_from_args(text, type,
			g_HudmsgTypes[type].unk00,
			g_HudmsgTypes[type].unk01,
			g_HudmsgTypes[type].unk02,
			g_HudmsgTypes[type].unk04,
			g_HudmsgTypes[type].unk08,
			g_HudmsgTypes[type].colour,
			g_HudmsgTypes[type].unk10,
			g_HudmsgTypes[type].alignh,
			g_HudmsgTypes[type].unk16,
			g_HudmsgTypes[type].alignv,
			g_HudmsgTypes[type].unk18,
			-1, 0);
}

void hudmsg_create_with_flags(char *text, s32 type, u32 flags)
{
	hudmsg_create_from_args(text, type,
			g_HudmsgTypes[type].unk00,
			g_HudmsgTypes[type].unk01,
			g_HudmsgTypes[type].unk02,
			g_HudmsgTypes[type].unk04,
			g_HudmsgTypes[type].unk08,
			g_HudmsgTypes[type].colour,
			g_HudmsgTypes[type].unk10,
			g_HudmsgTypes[type].alignh,
			g_HudmsgTypes[type].unk16,
			g_HudmsgTypes[type].alignv,
			g_HudmsgTypes[type].unk18,
			-1, flags);
}

void hudmsg_create_with_colour(char *text, s32 type, u8 colournum)
{
	g_HudmsgTypes[type].colour = g_HudmsgColours[colournum];

	hudmsg_create_from_args(text, type,
			g_HudmsgTypes[type].unk00,
			g_HudmsgTypes[type].unk01,
			g_HudmsgTypes[type].unk02,
			g_HudmsgTypes[type].unk04,
			g_HudmsgTypes[type].unk08,
			g_HudmsgTypes[type].colour,
			g_HudmsgTypes[type].unk10,
			g_HudmsgTypes[type].alignh,
			g_HudmsgTypes[type].unk16,
			g_HudmsgTypes[type].alignv,
			g_HudmsgTypes[type].unk18,
			-1, 0);
}

void hudmsg_create_with_duration(char *text, s32 type, struct hudmsgtype *config, s32 duration60)
{
	hudmsg_create_from_args(text, type,
			config->unk00,
			config->unk01,
			config->unk02,
			config->unk04,
			config->unk08,
			config->colour,
			config->unk10,
			config->alignh,
			config->unk16,
			config->alignv,
			config->unk18,
			duration60, HUDMSGFLAG_NOCHANNEL);
}

/**
 * Create a hudmsg that is tied to the given audio channel. When the audio
 * finishes the hudmsg is removed.
 *
 * This function is used for both in-game subtitles and cutscene subtitles.
 * If a cutscene is in progress, the function forces the type to cutscene.
 * This allows the caller to specify the type as in-game unconditionally
 * and it will do the right thing.
 *
 * For cutscene subtitles, a dynamic width is used which means the source text
 * has to be re-wrapped. There is also a limit of two lines at a time.
 *
 * The source text is split into individual messages. These splits are
 * usually at the end of each sentence, but they also occur after commas and
 * semi-colons.
 *
 * Each message is wrapped and appended to an accumulator. Every time the
 * accumulator would exceed two lines, the accumulator is queued as a hudmsg
 * and then cleared prior to appending the new message.
 *
 * Each hudmsg is assigned a duration according to its character length relative
 * to the entire string and the audio duration.
 */
void hudmsg_create_as_subtitle(char *srctext, s32 type, u8 colourindex, s32 audiochannelnum)
{
	s32 audioduration60;
	struct hudmsgtype *config;

	audioduration60 = ps_get_duration60(audiochannelnum);

	if (type == HUDMSGTYPE_INGAMESUBTITLE) {
		if (g_Vars.tickmode == TICKMODE_CUTSCENE) {
			if (!options_get_cutscene_subtitles()) {
				return;
			}

			type = HUDMSGTYPE_CUTSCENESUBTITLE;
		} else if (!options_get_in_game_subtitles()) {
			return;
		}
	}

	config = &g_HudmsgTypes[type];
	config->colour = g_HudmsgColours[colourindex];

	if (g_Vars.tickmode == TICKMODE_CUTSCENE && audioduration60 >= 0) {
#if VERSION == VERSION_JPN_FINAL
		u16 totallen = strlen(srctext);
		bool alldone = false;
		f32 time60perchar = audioduration60 / (f32) totallen;
		char buffer[300];
		char *ptr = srctext;

		while (!alldone) {
			u8 len = 0;
			bool paragraphdone = false;

			while (!paragraphdone) {
				if (*ptr == '\0') {
					alldone = true;
					paragraphdone = true;
				} else {
					if (*ptr == 'P' || *ptr == 'p') {
						paragraphdone = true;
					} else {
						buffer[len] = *ptr;
						len++;
					}

					ptr++;
				}
			}

			buffer[len] = '\0';

			if (len != 0) {
				hudmsg_create_with_duration(buffer, type, config, len * time60perchar);
			}
		}
#else
		char puncchars[] = { '.', ';', '!', '?', ',' };
		u16 srclen;
		s32 sp4a8;
		s32 wrapwidth;
		char accum[250];
		char prewrap[250];
		char postwrap[250];
		char msg[250];
		s32 msglen;
		bool split;
		s32 accumlen;
		s32 linecount;
		f32 time60perchar;
		s32 i;
		s32 j;
		bool append;
		bool foundpunctuation;

		srclen = strlen(srctext);
		wrapwidth = hudmsg0f0ddb1c(&sp4a8, config->unk16);

		accumlen = 0;
		i = 0;
		time60perchar = (f32)audioduration60 / srclen;

		// These two loops both work with the i iterator.
		// The inner loop increments i and is looking for places to split the
		// text, while the outer loop iterates once per split until the srctext
		// has been completely scanned.
		while (srctext[i] != '\0') {
			msglen = 0;
			foundpunctuation = false;
			split = false;

			while (srctext[i] != '\0' && (!foundpunctuation || !split || i > srclen - 10)) {
				// Check if the current char is punctuation
				for (j = 0; j < ARRAYCOUNT(puncchars); j++) {
					if (puncchars[j] == srctext[i]) {
						foundpunctuation = true;
					}
				}

				// Avoid splitting in the middle of trailing dots,
				// and also avoid splitting after "Dr." or "Mr."
				if (foundpunctuation && srctext[i] == '.') {
					if (srctext[i + 1] == '.') {
						foundpunctuation = false;
					}

					if (i >= 2) {
						if ((srctext[i - 2] == 'D' || srctext[i - 2] == 'd')
								&& (srctext[i - 1] == 'r' || srctext[i - 1] == 'R')) {
							foundpunctuation = false;
						}

						if ((srctext[i - 2] == 'M' || srctext[i - 2] == 'm')
								&& (srctext[i - 1] == 'r' || srctext[i - 1] == 'R')) {
							foundpunctuation = false;
						}
					}
				}

				// Copy the character from srctext to msg, except:
				// - if it's a space at the start of the string
				// - if it's a consecutive space
				// - if it's a line break (sometimes copy a space instead)
				if (msglen < 249) {
					bool ignore = false;

					if (srctext[i] == ' ') {
						if (msglen == 0) {
							ignore = true;
						} else if (msg[msglen - 1] == ' ') {
							ignore = true;
						}
					}

					if (srctext[i] == '\n') {
						ignore = true;

						if (msglen != 0 && msg[msglen - 1] != ' ' && srctext[i + 1] != ' ') {
							msg[msglen] = ' ';
							msglen++;
						}
					}

					if (foundpunctuation && srctext[i] == ' ') {
						split = true;
					}

					if (!ignore) {
						msg[msglen] = srctext[i];
						msglen++;
					}
				}

				if (1);

				i++;
			} // end of inner loop

			// At this point the string in msg is a single sentence,
			// free of line breaks. It still needs to be wrapped.

			// Make sure msg ends in a space
			if (msglen > 0 && msg[msglen - 1] != ' ') {
				msg[msglen] = ' ';
				msglen++;
			}

			// Rebuild prewrap by concatenating the accumulator and msg.
			// prewrap will be everything that's been read so far and has yet to
			// be queued.
			for (j = 0; j < accumlen; j++) {
				prewrap[j] = accum[j];
			}

			for (j = 0; j < msglen; j++) {
				prewrap[j + accumlen] = msg[j];
			}

			prewrap[accumlen + msglen] = '\n';
			prewrap[accumlen + msglen + 1] = '\0';

			// Apply text wrapping to prewrap
			text_wrap(wrapwidth, prewrap, postwrap, g_CharsHandelGothicSm, g_FontHandelGothicSm);

			// Next, count the number of lines in the wrapped message.
			// If it's more than two, send the accumulator out as a hudmsg and
			// then put msg in the accumulator. Otherwise, just append msg to
			// the accumulator.

			// Note that these strings always end in a line break, so counting
			// the line breaks is the same as counting visual lines
			linecount = 0;

			for (j = 0; postwrap[j] != '\0'; j++) {
				if (postwrap[j] == '\n') {
					linecount++;
				}
			}

			append = true;

			if (linecount >= 3) {
				if (accumlen == 0) {
					// Nothing is in the accumulator, so just queue the message
					msg[msglen] = '\n';
					msglen++;

					msg[msglen] = '\0';

					hudmsg_create_with_duration(msg, type, config, msglen * time60perchar);
					append = false;
				} else {
					// Queue the accumulator and then clear it.
					// The current message will be copied into the accumulator
					// for the next iteration.
					accum[accumlen] = '\n';
					accumlen++;

					accum[accumlen] = '\0';

					hudmsg_create_with_duration(accum, type, config, accumlen * time60perchar);
					accumlen = 0;
				}
			}

			if (append) {
				for (j = 0; j < msglen; j++) {
					accum[accumlen + j] = msg[j];
				}

				accumlen += msglen;
			}

			msg[msglen] = '\0';
		} // end of outer loop

		// If there's anything remaining in the accumulator, queue it
		if (accumlen != 0) {
			accum[accumlen] = '\n';
			accumlen++;

			accum[accumlen] = '\0';

			hudmsg_create_with_duration(accum, type, config, accumlen * time60perchar);
		}
#endif
	} else {
		hudmsg_create_from_args(srctext, type, config->unk00, config->unk01, config->unk02,
				config->unk04, config->unk08, config->colour, config->unk10, config->alignh,
				config->unk16, config->alignv, config->unk18, audiochannelnum, 0);
	}
}

void hudmsg_create_from_args_without_flags(char *text, s32 type, s32 conf00, s32 conf01, s32 conf02, struct fontchar **conf04, struct font **conf08, u32 textcolour, u32 shadowcolour, u32 alignh, s32 conf16, u32 alignv, s32 conf18, s32 arg14)
{
	hudmsg_create_from_args(text, type,
			conf00,
			conf01,
			conf02,
			conf04,
			conf08,
			textcolour,
			shadowcolour,
			alignh,
			conf16,
			alignv,
			conf18,
			arg14, 0);
}

void hudmsg_calculate_position(struct hudmessage *msg)
{
	s32 x;
	s32 y;
	s32 viewleft = g_Vars.players[msg->playernum]->viewleft / g_UiScaleX;
	s32 viewtop = g_Vars.players[msg->playernum]->viewtop;
	s32 viewwidth = g_Vars.players[msg->playernum]->viewwidth / g_UiScaleX;
	s32 viewheight = g_Vars.players[msg->playernum]->viewheight;
	s32 v0;

#if VERSION >= VERSION_NTSC_1_0
	s32 offset = (msg->alignh == HUDMSGALIGN_XMIDDLE) ? 10 : 0;

	if (PLAYERCOUNT() >= 3) {
		viewwidth -= offset;

		if (g_Vars.currentplayernum == 0 || g_Vars.currentplayernum == 2) {
			viewleft += offset;
		}
	}

	if (PLAYERCOUNT() == 2 && (options_get_screen_split() == SCREENSPLIT_VERTICAL || IS4MB())) {
#if VERSION >= VERSION_PAL_FINAL
		if (!g_InCutscene || g_MainIsEndscreen)
#endif
		{
			viewwidth -= offset;

			if (g_Vars.currentplayernum == 0) {
				viewleft += offset;
			}
		}
	}
#endif

	switch (msg->alignh) {
	case HUDMSGALIGN_SCREENLEFT:
		x = msg->xmargin;
		break;
	case HUDMSGALIGN_LEFT:
		v0 = (g_InCutscene && !g_MainIsEndscreen) ? 24 : msg->xmarginextra;

		x = viewleft + v0 + msg->xmargin + 3;

		if (PLAYERCOUNT() == 2
				&& (options_get_screen_split() == SCREENSPLIT_VERTICAL || IS4MB())
				&& (!g_InCutscene || g_MainIsEndscreen)) {
			if (IS4MB()) {
				if (msg->playernum == 0) {
					x--;
				} else if (msg->playernum == 1) {
					x -= 16;
				}
			} else {
				if (msg->playernum == 0) {
					x += 15;
				} else if (msg->playernum == 1) {
					x += 4;
				}
			}
		} else if (PLAYERCOUNT() >= 3) {
			if ((msg->playernum % 2) == 0) {
				x--;
			} else {
				x -= 16;
			}
		}
		break;
	case HUDMSGALIGN_RIGHT:
		x = viewleft + viewwidth - msg->width - msg->xmargin - 57;
		break;
	case HUDMSGALIGN_XMIDDLE:
		x = (viewwidth - msg->width) / 2 + viewleft + msg->xmargin;
		break;
	default:
		x = msg->xmargin;
		break;
	}

	switch (msg->alignv) {
	case HUDMSGALIGN_SCREENTOP:
		y = msg->ymargin;
		break;
	case HUDMSGALIGN_TOP:
		y = viewtop + msg->ymargin + 13;
		break;
	case HUDMSGALIGN_BOTTOM:
		y = viewtop + viewheight - msg->height - msg->ymargin - 14;

		if (PLAYERCOUNT() == 2 && (g_InCutscene == 0 || g_MainIsEndscreen)) {
			if (IS4MB() || (options_get_screen_split() != SCREENSPLIT_VERTICAL && msg->playernum == 0)) {
				y += 8;
			} else {
				y += 3;
			}
		} else if (PLAYERCOUNT() >= 3) {
			if (msg->playernum <= 1) {
				y += 8;
			} else {
				y += 3;
			}
		} else {
			if (options_get_effective_screen_size() != SCREENSIZE_FULL) {
				y += 8;
			}
		}
		break;
	case HUDMSGALIGN_YMIDDLE:
		y = (viewheight - msg->height) / 2 + viewtop + msg->ymargin;
		break;
	case HUDMSGALIGN_BELOWVIEWPORT:
		y = viewtop + viewheight - (msg->height / 2) + 18;
		break;
	default:
		y = msg->ymargin;
		break;
	}

	msg->x = x;
	msg->y = y;
}

void hudmsg_create_from_args(char *text, s32 type, s32 conf00, s32 conf01, s32 conf02,
		struct fontchar **conf04, struct font **conf08,
		u32 textcolour, u32 glowcolour,
		u32 alignh, s32 conf16, u32 alignv, s32 conf18, s32 arg14, u32 flags)
{
	s32 j;
	struct hudmessage *msg;
	s32 hash = 0;
	s32 i;
	s32 index;
	s32 textwidth;
	s32 textheight;
	s32 xmarginaextra;
	s32 wrapwidth;
	char stacktext[400];
	s32 writeindex;

	if (type == HUDMSGTYPE_INGAMESUBTITLE && !options_get_in_game_subtitles()) {
		return;
	}

	for (j = 0; text[j] != '\0'; j++) {
		hash = hash + text[j];
	}

	if ((flags & HUDMSGFLAG_ONLYIFALIVE) == 0 || !g_Vars.currentplayer->isdead) {
		if ((flags & HUDMSGFLAG_ALLOWDUPES) == 0) {
			// Check for duplicate messages
			s32 dupeofindex = -1;

			for (index = 0; index < g_NumHudMessages; index++) {
				if (g_HudMessages[index].state != HUDMSGSTATE_FREE
						&& g_HudMessages[index].state != HUDMSGSTATE_FADINGOUT
						&& g_HudMessages[index].playernum == g_Vars.currentplayernum
						&& g_HudMessages[index].hash == hash) {
					dupeofindex = index;
				}
			}

			if (dupeofindex >= 0) {
				return;
			}
		}

#if PAL
		g_UiScaleX = 1;
#else
		g_UiScaleX = g_ViRes == VIRES_HI ? 2 : 1;
#endif

		// Find an unused index for the new message
		for (index = 0; index < g_NumHudMessages; index++) {
			if (g_HudMessages[index].state == HUDMSGSTATE_FREE) {
				break;
			}
		}

		if (index >= g_NumHudMessages
				&& (type == HUDMSGTYPE_OBJECTIVECOMPLETE
					|| type == HUDMSGTYPE_OBJECTIVEFAILED
					|| type == HUDMSGTYPE_INGAMESUBTITLE)) {
			// Out of space - Check if an existing message can be replaced
			index = hudmsg_get_next(-1);

			while (index >= 0) {
				if (g_HudMessages[index].state == HUDMSGSTATE_QUEUED) {
					if (g_HudMessages[index].type == HUDMSGTYPE_DEFAULT
							|| g_HudMessages[index].type == HUDMSGTYPE_3
							|| g_HudMessages[index].type == HUDMSGTYPE_4) {
						// Good to replace this one
						break;
					}
				}

				// Can't replace - try and find another
				index = hudmsg_get_next(g_HudMessages[index].id);
			}
		}

		if (index >= 0 && index < g_NumHudMessages) {
			xmarginaextra = 0;
			msg = &g_HudMessages[index];
			wrapwidth = hudmsg0f0ddb1c(&xmarginaextra, conf16);
			text_measure(&textheight, &textwidth, text, *conf04, *conf08, 0);

#if VERSION >= VERSION_JPN_FINAL
			if (textwidth > wrapwidth && (flags & HUDMSGFLAG_NOWRAP) == 0)
#else
			if (textwidth > wrapwidth)
#endif
			{
				i = 0;
				writeindex = 0;

				while (i < 400 && text[i] != '\0') {
					if (text[i] != '\n') {
						stacktext[writeindex++] = text[i];
					}

					i++;
				}

				stacktext[writeindex++] = '\n';
				stacktext[writeindex++] = '\0';

				text_wrap(wrapwidth, stacktext, msg->text, *conf04, *conf08);
				text_measure(&textheight, &textwidth, msg->text, *conf04, *conf08, 0);
			} else {
				strncpy(msg->text, text, 399);
				msg->text[399] = '\0';
			}

			msg->flags = flags;
			msg->playernum = g_Vars.currentplayernum;
			msg->type = type;
			msg->id = g_NextHudMessageId++;
			msg->state = HUDMSGSTATE_QUEUED;
			msg->timer = 0;
			msg->boxed = conf00;
			msg->allowfadein = conf01;
			msg->flash = conf02;
			msg->font1 = *conf04;
			msg->font2 = *conf08;
			msg->textcolour = textcolour;
			msg->glowcolour = glowcolour;
			msg->alignh = alignh;
			msg->alignv = alignv;
			msg->width = textwidth;
			msg->height = textheight;
			msg->xmarginextra = xmarginaextra;
			msg->xmargin = conf16;
			msg->ymargin = conf18;
			msg->hash = hash;

			hudmsg_calculate_position(msg);

			if (flags & HUDMSGFLAG_NOCHANNEL) {
				msg->showduration = TICKS(arg14);
				msg->channelnum = -1;
			} else {
				msg->showduration = TICKS(g_HudmsgTypes[type].duration);
				msg->channelnum = arg14;
			}
		}

		g_UiScaleX = 1;
	}
}

void hudmsgs_tick(void)
{
	s32 k;
	s32 previd;
	bool show;
	struct hudmessage *msg;
#if VERSION >= VERSION_NTSC_1_0
	s32 prevplayernum;
#endif
	s32 i;
	s32 j;
#if VERSION >= VERSION_NTSC_1_0
	s32 index;
	bool hide;
#else
	bool hide;
	s32 index;
#endif
	f32 fadeintime;
	f32 fadeouttime;

	g_HudmsgsActive = false;

#if PAL
	g_UiScaleX = 1;
#else
	g_UiScaleX = (g_ViRes == VIRES_HI) ? 2 : 1;
#endif

#if VERSION >= VERSION_NTSC_1_0
	prevplayernum = g_Vars.currentplayernum;

	for (k = 0; k < g_NumHudMessages; k++) {
		if (g_HudMessages[k].state != HUDMSGSTATE_FREE) {
			if (g_Vars.tickmode == TICKMODE_CUTSCENE) {
				for (j = 0; j < g_NumHudMessages; j++) {
					if (k != j
							&& g_HudMessages[j].state != HUDMSGSTATE_FREE
							&& g_HudMessages[j].hash == g_HudMessages[k].hash) {
						g_HudMessages[j].state = HUDMSGSTATE_FREE;
					}
				}
			}

			set_current_player_num(g_HudMessages[k].playernum);
			hudmsg_calculate_position(&g_HudMessages[k]);
		}
	}

	set_current_player_num(prevplayernum);
#else
	for (k = 0; k < g_NumHudMessages; k++) {
		if (g_HudMessages[k].state != HUDMSGSTATE_FREE) {
			hudmsg_calculate_position(&g_HudMessages[k]);
		}
	}
#endif

	previd = -1; \
	while (true) {
		index = hudmsg_get_next(previd);

		if (index < 0) {
			break;
		}

		msg = &g_HudMessages[index];
		previd = msg->id;

		if (msg->channelnum >= 0) {
			msg->opacity = ps_get_subtitle_opacity(msg->channelnum);
		} else {
			msg->opacity = 0xff;
		}

		if (msg->type == HUDMSGTYPE_CUTSCENESUBTITLE && g_Vars.tickmode != TICKMODE_CUTSCENE) {
			msg->state = HUDMSGSTATE_FREE;
			msg->timer = 0;
		}

		switch (msg->state) {
		case HUDMSGSTATE_QUEUED:
#if VERSION >= VERSION_NTSC_1_0
			if (msg->flags & HUDMSGFLAG_DELAY) {
				msg->timer++;

				if (msg->timer > 3) {
					msg->flags &= ~HUDMSGFLAG_DELAY;
				}
			} else
#endif
			{
				show = true;

				if (g_Vars.players[msg->playernum]->isdead) {
					show = false;
				}

				if (show) {
					// Check if any other message is occupying our space
					for (i = 0; i < g_NumHudMessages; i++) {
						if (g_HudMessages[i].state != HUDMSGSTATE_FREE
								&& g_HudMessages[i].state != HUDMSGSTATE_QUEUED
								&& g_HudMessages[i].x + g_HudMessages[i].width >= msg->x
								&& g_HudMessages[i].x <= msg->x + msg->width
								&& g_HudMessages[i].y + g_HudMessages[i].height >= msg->y
								&& g_HudMessages[i].y <= msg->y + msg->height) {
							show = false;

							// Consider booting the previous message out earlier
							if (g_HudMessages[i].type == msg->type
									&& msg->boxed
									&& g_HudMessages[i].boxed
									&& g_HudMessages[i].state == HUDMSGSTATE_FADINGOUT) {
								g_HudMessages[i].state = HUDMSGSTATE_FREE;
								g_HudMessages[i].timer = 0;
								msg->state = HUDMSGSTATE_FADINGIN;
								msg->timer = 0;
							}

							break;
						}
					}
				}

				if (show) {
					if (msg->boxed) {
						msg->state = HUDMSGSTATE_CHOOSETRANSITION;
					} else if (msg->allowfadein) {
						msg->state = HUDMSGSTATE_FADINGIN;
					} else {
						msg->state = HUDMSGSTATE_ONSCREEN;
					}

					msg->timer = 0;

					if (msg->type == HUDMSGTYPE_CUTSCENESUBTITLE) {
						msg->state = HUDMSGSTATE_ONSCREEN;
					}
				}
			}
			break;
		case HUDMSGSTATE_CHOOSETRANSITION:
			if (msg->boxed && msg->allowfadein) {
				msg->state = HUDMSGSTATE_FADINGIN;
			} else {
				msg->state = HUDMSGSTATE_ONSCREEN;
			}

			if (msg->type == HUDMSGTYPE_CUTSCENESUBTITLE) {
				msg->state = HUDMSGSTATE_ONSCREEN;
			}

			msg->timer = 0;
			break;
		case HUDMSGSTATE_FADINGIN:
			if (msg->type == HUDMSGTYPE_CUTSCENESUBTITLE) {
				// Cutscene subtitles appear immediately
				msg->state = HUDMSGSTATE_ONSCREEN;
				msg->timer = 0;
			} else {
				// Most HUD messages play a swish sound effect
				if (msg->timer == 0
						&& !lv_is_paused()
						&& !mp_is_paused()
#if VERSION >= VERSION_NTSC_1_0
						&& msg->type != HUDMSGTYPE_CUTSCENESUBTITLE
						&& msg->type != HUDMSGTYPE_INGAMESUBTITLE
#endif
						&& PLAYERCOUNT() == 1) {
#if VERSION >= VERSION_NTSC_1_0
					snd_start(var80095200, SFXNUM_003E_HUDMSG, NULL, -1, -1, -1, -1, -1);
#else
					snd_start(var80095200, SFXNUM_003E_HUDMSG, &var800736b0nb, -1, -1, -1, -1, -1);
#endif
				}

				fadeintime = (sqrtf(msg->width * msg->width + msg->height * msg->height) + 132) / PALUPF(7.0f);

				msg->timer += g_Vars.lvupdate60;

				if (msg->timer >= (s32)fadeintime || msg->type == HUDMSGTYPE_CUTSCENESUBTITLE) {
					msg->state = HUDMSGSTATE_ONSCREEN;
					msg->timer = 0;
				}
			}
			break;
		case HUDMSGSTATE_ONSCREEN:
			msg->timer += g_Vars.lvupdate60;

			hide = false;

			// Subtitles have an audio channel number and are hidden when the audio stops
			if (msg->channelnum >= 0) {
				if (ps_is_channel_free(msg->channelnum)) {
					hide = true;
				} else if (msg->flags & HUDMSGFLAG_FORCEOFF) {
					msg->flags &= ~HUDMSGFLAG_FORCEOFF;
					hide = true;
				}
			} else if (msg->timer >= msg->showduration && msg->showduration != -1) {
				hide = true;
			}

			if (hide) {
				if (msg->boxed) {
					msg->state = HUDMSGSTATE_FADINGOUT;
				} else {
					msg->state = HUDMSGSTATE_FREE;
				}

				msg->timer = 0;
			}
			break;
		case HUDMSGSTATE_FADINGOUT:
			fadeouttime = (sqrtf(msg->width * msg->width + msg->height * msg->height) + 92) / PALUPF(7.0f);

			msg->timer += g_Vars.lvupdate60;

			if (msg->timer >= (s32)fadeouttime) {
				msg->state = HUDMSGSTATE_FREE;
				msg->timer = 0;
			}
			break;
		case HUDMSGSTATE_FREE:
			break;
		}
	}

	g_UiScaleX = 1;
}

void hudmsgs_set_on(u32 reason)
{
	g_Vars.currentplayer->hudmessoff &= ~reason;
}

void hudmsgs_set_off(u32 reason)
{
	g_Vars.currentplayer->hudmessoff |= reason;
}

void hudmsgs_remove_for_dead_player(s32 playernum)
{
	s32 i;

	for (i = 0; i < g_NumHudMessages; i++) {
		if (g_HudMessages[i].state
				&& g_HudMessages[i].playernum == playernum
				&& (g_HudMessages[i].flags & HUDMSGFLAG_ONLYIFALIVE)) {
			g_HudMessages[i].state = HUDMSGSTATE_FREE;
			g_HudMessages[i].timer = 0;
		}
	}
}

Gfx *hudmsgs_render(Gfx *gdl)
{
	struct hudmessage *msg;
	s32 i;
	u32 textcolour;
	u32 glowcolour;
	f32 sin;
	s32 x;
	s32 y;
	s32 timerthing = 255;
	s32 spdc = true;

#if PAL
	g_UiScaleX = 1;
#else
	g_UiScaleX = g_ViRes == VIRES_HI ? 2 : 1;
#endif

	gdl = text_begin(gdl);

	if ((g_Vars.coopplayernum >= 0 || g_Vars.antiplayernum >= 0)
			&& g_InCutscene
			&& g_MainIsEndscreen == 0
			&& g_Vars.currentplayernum == 0) {
		spdc = false;
	}

	for (i = 0; i < g_NumHudMessages; i++) {
		msg = &g_HudMessages[i];

		if (!msg->opacity) {
			continue;
		}

		if (msg->state == HUDMSGSTATE_FREE
				|| msg->state == HUDMSGSTATE_QUEUED
				|| (spdc && g_Vars.currentplayernum != msg->playernum)) {
			continue;
		}

		if (msg->flash) {
			s32 alpha;
			sin = sinf((msg->timer * DTOR(180)) / 60.0f);

			if (sin < 0.0f) {
				sin = -sin;
			}

			alpha = 192.0f * sin;

			textcolour = (msg->textcolour & 0xffffff00) + alpha;
			glowcolour = msg->glowcolour;
		} else {
			textcolour = msg->textcolour | 0xa0;
			glowcolour = msg->glowcolour;
		}

		if (msg->opacity != 255) {
			u32 textalpha = textcolour & 0xff;
			u32 glowalpha = glowcolour & 0xff;

			textalpha = (msg->opacity * textalpha) / 255;
			glowalpha = (msg->opacity * glowalpha) / 255;

			textcolour = (textcolour & 0xffffff00) + (textalpha & 0xff);
			glowcolour = (glowcolour & 0xffffff00) + (glowalpha & 0xff);
		}

		x = msg->x;
		y = msg->y;

		if (msg->type == HUDMSGTYPE_INGAMESUBTITLE && player_is_health_visible()) {
			y += (s32)(16.0f * player_get_health_bar_height_frac());
		}

		if (msg->type == HUDMSGTYPE_CUTSCENESUBTITLE) {
#if VERSION >= VERSION_NTSC_1_0
			gDPSetScissor(gdl++, 0,
					(x - 4) * g_UiScaleX, 0,
					(x + msg->width + 3) * g_UiScaleX, vi_get_buf_height());
#else
			gDPSetScissor(gdl++, 0,
					(x - 4) * g_UiScaleX, y - 4,
					(x + msg->width + 3) * g_UiScaleX, y + msg->height + 3);
#endif
		}

		switch (msg->state) {
		case HUDMSGSTATE_FREE:
		case HUDMSGSTATE_QUEUED:
			break;
		case HUDMSGSTATE_FADINGIN:
			{
				u32 bordercolour = msg->textcolour | 0x40;
				f32 tmp;
				f32 spc0;

				if (msg->opacity != 255) {
					u32 alpha = (msg->opacity * (bordercolour & 0xff)) / 255;
					bordercolour = (bordercolour & 0xffffff00) + (alpha & 0xff);
				}

				spc0 = (sqrtf(msg->width * msg->width + msg->height * msg->height) + 132.0f) / PALUPF(7.0f);

				if (spc0 > 30.0f) {
					spc0 = 30.0f;
				}

				spc0 = msg->timer / spc0;

				if (spc0 > 1.0f) {
					spc0 = 1.0f;
				}

				if (spc0 < 0.0f) {
					spc0 = 0.0f;
				}

				tmp = msg->timer * PALUPF(7.0f);

				text_set_diagonal_blend(x, y, tmp, DIAGMODE_FADEIN);

				if (msg->boxed) {
#if VERSION >= VERSION_JPN_FINAL
					gdl = hudmsg_render_box(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height - 1, 1.0f, bordercolour, spc0);
#else
					gdl = hudmsg_render_box(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height + 2, 1.0f, bordercolour, spc0);
#endif

					gdl = text_render_v2(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, vi_get_width(), vi_get_height(), 0, 0);
				} else {
					gdl = text_draw_box(gdl, x, y, x + msg->width, y + msg->height, 0);
					gdl = text_render_vx(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, glowcolour, vi_get_width(), vi_get_height(), 0, 0);
				}

				if (msg->alignv == 6) {
					timerthing = 0;
				}

				text_reset_blends();
			}
			break;
		case HUDMSGSTATE_ONSCREEN:
			if (msg->boxed) {
				u32 bordercolour = msg->textcolour | 0x40;

				if (msg->opacity != 255) {
					u32 alpha = (msg->opacity * (bordercolour & 0xff)) / 255;
					bordercolour = (bordercolour & 0xffffff00) + (alpha & 0xff);
				}

#if VERSION >= VERSION_JPN_FINAL
				gdl = hudmsg_render_box(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height - 1, 1.0f, bordercolour, 1.0f);
#else
				gdl = hudmsg_render_box(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height + 2, 1.0f, bordercolour, 1.0f);
#endif

				gdl = text_render_v2(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, vi_get_width(), vi_get_height(), 0, 0);
			} else {
				gdl = text_draw_box(gdl, x, y, x + msg->width, y + msg->height, 0);
				gdl = text_render_vx(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, glowcolour, vi_get_width(), vi_get_height(), 0, 0);
			}
			if (msg->alignv == 6) {
				timerthing = 0;
			}
			break;
		case HUDMSGSTATE_FADINGOUT:
			{
				u32 bordercolour;
				u32 stack;
				f32 spa8 = (sqrtf(msg->width * msg->width + msg->height * msg->height) + 92.0f) / PALUPF(7.0f);
				f32 tmp;

				bordercolour = msg->textcolour | 0x40;

				if (msg->opacity != 255) {
					u32 alpha = (msg->opacity * (bordercolour & 0xff)) / 255;
					bordercolour = (bordercolour & 0xffffff00) + (alpha & 0xff);
				}

				tmp = (spa8 - msg->timer) * PALUPF(7.0f);

				text_set_diagonal_blend(x + msg->width, y + msg->height, tmp, DIAGMODE_FADEOUT);

				if (spa8 > 30.0f) {
					spa8 = 30.0f;
				}

				spa8 = msg->timer / spa8;

				if (spa8 > 1.0f) {
					spa8 = 1.0f;
				}

				if (msg->boxed) {
#if VERSION >= VERSION_JPN_FINAL
					gdl = hudmsg_render_box(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height - 1, 1.0f, bordercolour, 1.0f - spa8);
#else
					gdl = hudmsg_render_box(gdl, x - 3, y - 3, x + msg->width + 2, y + msg->height + 2, 1.0f, bordercolour, 1.0f - spa8);
#endif

					gdl = text_render_v2(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, vi_get_width(), vi_get_height(), 0, 0);
				} else {
					gdl = text_draw_box(gdl, x, y, x + msg->width, y + msg->height, 0);
					gdl = text_render_vx(gdl, &x, &y, msg->text, msg->font1, msg->font2, textcolour, glowcolour, vi_get_width(), vi_get_height(), 0, 0);
				}

				if (msg->alignv == 6) {
					timerthing = 0;
				}

				text_reset_blends();
			}
			break;
		}

		if (msg->type == HUDMSGTYPE_CUTSCENESUBTITLE) {
			gDPSetScissor(gdl++, 0,
					vi_get_view_left(), vi_get_view_top(),
					vi_get_view_left() + vi_get_view_width(), vi_get_view_top() + vi_get_view_height());
		}
	}

	if (timerthing) {
		if (options_get_show_mission_time(g_Vars.currentplayerstats->mpindex)
				&& var80075d60 == 2
				&& g_Vars.normmplayerisrunning == false
				&& g_Vars.stagenum != STAGE_CITRAINING
				&& g_Vars.currentplayer->cameramode != CAMERAMODE_EYESPY
				&& g_Vars.currentplayer->cameramode != CAMERAMODE_THIRDPERSON) {
			gdl = hudmsg_render_mission_timer(gdl, timerthing);
		}

		if (hudmsg_is_zoom_range_visible()) {
			gdl = hudmsg_render_zoom_range(gdl, timerthing);
		}

		gdl = countdown_timer_render(gdl);
	}

	gdl = text_end(gdl);

	g_UiScaleX = 1;

	return gdl;
}

void hudmsgs_stop(void)
{
	s32 i;

	for (i = 0; i < g_NumHudMessages; i++) {
		g_HudMessages[i].state = HUDMSGSTATE_FREE;
	}

#if VERSION < VERSION_NTSC_1_0
	if (var800736b0nb && sndp_get_state(var800736b0nb) != AL_STOPPED) {
		sndp_stop_sound(var800736b0nb);
	}
#endif
}
