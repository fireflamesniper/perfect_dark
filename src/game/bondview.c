#include <ultra64.h>
#include "constants.h"
#include "game/dlights.h"
#include "game/game_006900.h"
#include "game/atan2f.h"
#include "game/savebuffer.h"
#include "game/sky.h"
#include "game/bondview.h"
#include "game/text.h"
#include "game/gfxmemory.h"
#include "game/lang.h"
#include "game/options.h"
#include "bss.h"
#include "lib/vi.h"
#include "lib/joy.h"
#include "lib/main.h"
#include "lib/rng.h"
#include "lib/str.h"
#include "lib/mtx.h"
#include "data.h"
#include "types.h"
#include "gbiex.h"

#ifdef AVOID_UB
char var800a41c0[26];
#else
char var800a41c0[24];
#endif

u8 g_IrScanlines[2][480];

#if VERSION < VERSION_NTSC_1_0
u8 var800a8b58nb[0x1c0];
#endif

s32 var8007f840 = 0;
u8 var8007f844 = 0;
u8 var8007f848 = 0;
s32 g_IrBinocularRadius = PAL ? 102 : 90;
s32 g_IrCentreSize = 3;
u32 var8007f854 = 0x00000000;
u32 var8007f858 = 0xb8000000;
u32 var8007f85c = 0x00000000;

#if VERSION < VERSION_NTSC_1_0
void bview_prompt_ir(void)
{
	main_override_variable("fsrad", (u32 *)&g_IrBinocularRadius);
	main_override_variable("fscs", (u32 *)&g_IrCentreSize);
}
#endif

Gfx *bview_draw_ir_rect(Gfx *gdl, s32 x1, s32 y1, s32 x2, s32 y2)
{
	gDPFillRectangle(gdl++, x1, y1, x2, y2);

	return gdl;
}

Gfx *bview_load_texture_i8(Gfx *gdl, u16 *fb, s32 y, s32 tile, s32 x, s32 width)
{
	s32 offset = y * vi_get_width() + x;

	gDPPipeSync(gdl++);
	gDPSetTextureImage(gdl++, G_IM_FMT_I, G_IM_SIZ_8b, SCREEN_320, &fb[offset]);
	gDPLoadSync(gdl++);
	gDPLoadBlock(gdl++, tile, 0, 0, width - 1, 0);

	return gdl;
}

Gfx *bview_load_texture_rgba16(Gfx *gdl, u16 *fb, s32 y, s32 tile, s32 x, s32 width)
{
	s32 offset = vi_get_width() * y + x;

	gDPPipeSync(gdl++);
	gDPSetTextureImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, width, &fb[offset]);
	gDPLoadSync(gdl++);
	gDPLoadBlock(gdl++, tile, 0, 0, width - 1, 0);

	return gdl;
}

Gfx *bview_draw_texture(Gfx *gdl, s32 top, s32 height, s32 left, s32 width)
{
	gDPPipeSync(gdl++);

	gSPTextureRectangle(gdl++,
			left << 2,
			top << 2,
			(left + width) << 2,
			(top + 1) << 2,
			G_TX_RENDERTILE, 0, 0, height * 1024, 1024);

	return gdl;
}

Gfx *bview_copy_pixels(Gfx *gdl, u16 *fb, s32 top, u32 tile, s32 arg4, f32 arg5, s32 left, s32 width)
{
	uintptr_t image;
	s32 width2;
	s32 numparts;
	s32 lrs[1];

	if (width > SCREEN_WIDTH_LO) {
		numparts = 2;
		lrs[0] = width / numparts;

		image = (uintptr_t) &fb[vi_get_width() * top + left] & 0x00ffffff;

		gDPSetTextureImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_320, image);
		gDPLoadBlock(gdl++, tile, 0, 0, width / numparts - 1, 0);
		gSPTextureRectangle(gdl++,
				left << 2,
				arg4 << 2,
				((s32) left + width / numparts) << 2,
				(arg4 + 1) << 2,
				G_TX_RENDERTILE,
				(s32) ((width - width / arg5) * 16.0f),
				0,
				(s32) (1024.0f / arg5),
				1024);

		left += lrs[0];

		image = (uintptr_t) &fb[vi_get_width() * top + left] & 0x00ffffff;

		gDPSetTextureImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_320, image);
		gDPLoadBlock(gdl++, tile, 0, 0, lrs[0] - 1, 0);

		gSPTextureRectangle(gdl++,
				left << 2,
				arg4 << 2,
				(left + lrs[0]) << (0, 2),
				(arg4 + 1) << 2,
				G_TX_RENDERTILE,
				0,
				0,
				(s32) (1024.0f / arg5),
				1024);
	} else {
		width2 = width;

		image = (uintptr_t) &fb[vi_get_width() * top + left] & 0x00ffffff;

		gDPSetTextureImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_320, image);
		gDPLoadBlock(gdl++, tile, 0, 0, width2 - 1, 0);

		numparts = (s32) (1024.0f / arg5);

		gSPTextureRectangle(gdl++,
				left << 2,
				arg4 << 2,
				(left + width2) << 2,
				(arg4 + 1) << 2,
				G_TX_RENDERTILE,
				(s32) ((width - width / arg5) * 16.0f),
				0,
				numparts,
				1024);
	}

	return gdl;
}

/**
 * Given x1 and width, draw a rectangle on both ends whose width is determined by frac.
 */
Gfx *bview_draw_fisheye_rect(Gfx *gdl, s32 y1, f32 frac, s32 x1, s32 width)
{
	if (frac < 1) {
		f32 halfwidth = width * 0.5f;
		f32 xmid = x1 + halfwidth;
		f32 fillwidth = (s32) (frac * halfwidth);

		gDPFillRectangle(gdl++, x1, y1, xmid - fillwidth, y1 + 1);
		gDPFillRectangle(gdl++, xmid + fillwidth, y1, x1 + width, y1 + 1);
	}

	return gdl;
}

Gfx *bview_prepare_static_rgba16(Gfx *gdl, u32 colour, u32 alpha)
{
	static u32 envcol = 0xffffffff;
	static u32 primcol = 0x7f7f7fff;

	main_override_variable("envcol", &envcol);
	main_override_variable("primcol", &primcol);

	gDPPipeSync(gdl++);
	gDPSetTile(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0x0000, 5, 0,
			G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
			G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
	gDPSetCycleType(gdl++, G_CYC_1CYCLE);
	gSPTexture(gdl++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
	gDPSetTextureLOD(gdl++, G_TL_TILE);
	gDPSetTextureDetail(gdl++, G_TD_CLAMP);
	gDPSetTextureLUT(gdl++, G_TT_NONE);
	gDPSetTile(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 160, 0, G_TX_RENDERTILE, 0,
			G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
			G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
	gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, 0x0800, 0x0020);
	gDPSetTextureFilter(gdl++, G_TF_POINT);
	gDPSetColor(gdl++, G_SETENVCOLOR, (colour & 0xffffff00) | (alpha & 0xff));
	gDPSetCombineLERP(gdl++,
			TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT,
			TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT);
	gDPSetTexturePersp(gdl++, G_TP_NONE);
	gDPSetColorDither(gdl++, G_CD_DISABLE);
	gDPSetAlphaDither(gdl++, G_AD_NOISE);
	gDPSetRenderMode(gdl++, G_RM_CLD_SURF, G_RM_NOOP2);

	return gdl;
}

Gfx *bview_prepare_static_i8(Gfx *gdl, u32 colour, u32 alpha)
{
	static u32 envcol = 0xffffffff;
	static u32 primcol = 0x7f7f7fff;

	main_override_variable("envcol", &envcol);
	main_override_variable("primcol", &primcol);

	gDPPipeSync(gdl++);
	gDPSetTile(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_8b, 0, 0x0000, 5, 0,
			G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
			G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
	gDPSetCycleType(gdl++, G_CYC_1CYCLE);
	gSPTexture(gdl++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
	gDPSetTextureLOD(gdl++, G_TL_TILE);
	gDPSetTextureDetail(gdl++, G_TD_CLAMP);
	gDPSetTextureLUT(gdl++, G_TT_NONE);
	gDPSetTile(gdl++, G_IM_FMT_I, G_IM_SIZ_8b, 160, 0, G_TX_RENDERTILE, 0,
			G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD,
			G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
	gDPSetTileSize(gdl++, G_TX_RENDERTILE, 0, 0, 0x0800, 0x0020);
	gDPSetTextureFilter(gdl++, G_TF_POINT);
	gDPSetColor(gdl++, G_SETENVCOLOR, (colour & 0xffffff00) | (alpha & 0xff));
	gDPSetCombineLERP(gdl++,
			TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT,
			TEXEL0, 0, ENVIRONMENT, 0, 0, 0, 0, ENVIRONMENT);
	gDPSetTexturePersp(gdl++, G_TP_NONE);
	gDPSetColorDither(gdl++, G_CD_DISABLE);
	gDPSetAlphaDither(gdl++, G_AD_NOISE);
	gDPSetRenderMode(gdl++, G_RM_CLD_SURF, G_RM_NOOP2);

	return gdl;
}

Gfx *bview_draw_motion_blur(Gfx *gdl, u32 colour, u32 alpha)
{
	u16 *fb = vi_get_front_buffer();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
	f32 fxxx;
	f32 fyyy;
	s32 viewwidth = vi_get_view_width();
	s32 viewleft = vi_get_view_left();
	f32 somefloat;
	s32 newalpha;
	s32 i;

	static u32 sfyyy = 1000;
	static u32 sfxxx = 1000;

	if (var8007f848) {
		return gdl;
	}

	var8007f848 = true;

	newalpha = alpha;
	newalpha += var8007f844;

	if (newalpha > 230) {
		newalpha = 230;
	}

	var8007f844 = 0;

	main_override_variable("sfxxx", &sfxxx);
	fxxx = sfxxx / 1000.0f;
	main_override_variable("sfyyy", &sfyyy);
	fyyy = sfyyy / 1000.0f;

	gDPPipeSync(gdl++);

	somefloat = (viewheight - viewheight / fyyy) * 0.5f;
	gdl = bview_prepare_static_rgba16(gdl, colour, newalpha);

	for (i = viewtop; i < viewtop + viewheight; i++) {
		gdl = bview_copy_pixels(gdl, fb, viewtop + (s32)somefloat, 5, i, fxxx, viewleft, viewwidth);
		somefloat += 1.0f / fyyy;
	}

	return gdl;
}

/**
 * Draw static for the Infiltration intro cutscene and Slayer rockets.
 */
Gfx *bview_draw_static(Gfx *gdl, u32 colour, s32 alpha)
{
	u16 *fb = vi_get_front_buffer();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 viewleft = vi_get_view_left();
	u16 *fb2 = (u16 *) PHYS_TO_K0(random() & 0xfff00);
	s32 y;

	gDPPipeSync(gdl++);

	gdl = bview_prepare_static_i8(gdl, colour, alpha);

	for (y = viewtop; y < viewtop + viewheight; y++) {
		gdl = bview_copy_pixels(gdl, fb2, random() % 240, 5, y, 1.0f, viewleft, viewwidth);
	}

	if (fb2) {
		// empty
	}

	return gdl;
}

/**
 * Draw the yellow interlace effect for Slayer rockets.
 */
Gfx *bview_draw_slayer_rocket_interlace(Gfx *gdl, u32 colour, u32 alpha)
{
	u16 *fb = vi_get_back_buffer();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 y;
	s32 viewleft = vi_get_view_left();
	f32 angle = DTOR(30);
	s32 offset = (s32)(g_20SecIntervalFrac * 600.0f) % 12;
	f32 increment;

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "interlaceGfx");

	gDPPipeSync(gdl++);

	increment = (DTOR2(150) - angle) / viewheight;

	gdl = bview_prepare_static_rgba16(gdl, colour, alpha);

	for (y = viewtop; y < viewtop + viewheight; y++) {
		s32 offsety = y - offset;

		if ((offsety % 8) == 0 || y == viewtop) {
			if ((offsety % 16) < 8) {
				gDPSetEnvColor(gdl++, 0xff, 0xff, 0x00, 0xff);
			} else {
				gDPSetEnvColor(gdl++, 0xff, 0xff, 0xbf, 0xff);
			}
		}

		gdl = bview_copy_pixels(gdl, fb, y, 5, y, 2.0f - sinf(angle), viewleft, viewwidth);

		angle += increment;
	}

	return gdl;
}

/**
 * Draw the blue film interlace effect for the Infiltration intro cutscene.
 */
Gfx *bview_draw_film_interlace(Gfx *gdl, u32 colour, u32 alpha)
{
	u16 *fb = vi_get_back_buffer();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
	s32 y;
	s32 viewwidth = vi_get_view_width();
	s32 viewleft = vi_get_view_left();
	s32 offset = (s32)(g_20SecIntervalFrac * 600.0f) % 12;
	u32 stack;

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "BlueInterlaceGfx");

	gDPPipeSync(gdl++);

	gdl = bview_prepare_static_rgba16(gdl, colour, alpha);

	for (y = viewtop; y < viewtop + viewheight; y++) {
		s32 offsety = y - offset;
		s32 tmpy = y;

		if (offsety % 6 == 0 || y == viewtop) {
			if (offsety % 12 < 6) {
				gDPSetEnvColor(gdl++, 0x7f, 0xff, 0xff, 0xff);
			} else {
				gDPSetEnvColor(gdl++, 0x00, 0xaf, 0xff, 0xff);
			}
		}

		if (random() % 20 == 1) {
			tmpy = random() % 200;
		}

		gdl = bview_copy_pixels(gdl, fb, tmpy, 5, y, 1, viewleft, viewwidth);
	}

	return gdl;
}

/**
 * Draw a zoom in/out motion blur effect.
 *
 * Used when entering/exiting combat boosts and when entering/exiting xray mode.
 */
Gfx *bview_draw_zoom_blur(Gfx *gdl, u32 colour, s32 alpha, f32 arg3, f32 arg4)
{
	u16 *fb = vi_get_front_buffer();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 viewleft = vi_get_view_left();
	f32 somefloat;
	s32 i;

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "stretchBlurGfx");

	gDPPipeSync(gdl++);

	somefloat = (viewheight - viewheight / arg4) * 0.5f;

	gdl = bview_prepare_static_rgba16(gdl, colour, alpha);

	for (i = viewtop; i < viewtop + viewheight; i++) {
		gdl = bview_copy_pixels(gdl, fb, (s32)somefloat + viewtop, 5, i, arg3, viewleft, viewwidth);
		somefloat += 1.0f / arg4;
	}

	return gdl;
}

/**
 * Given y in the range 0 to half the screen height, determine where the radius
 * ends on this row. The return value is a frac scaled between the viewport's
 * centre X and the viewport's width.
 */
f32 bview_get_fisheye_xradius_frac(s32 y, f32 dir, f32 halfheight, f32 sqhalfheight)
{
	f32 result;
	f32 newy = halfheight;

	if (y < 0 || y > 127) {
		return 0.01f;
	}

	// On NTSC dir is always -1, and on PAL it's pretty close to -1.
	// This is effectively setting newy = halfheight - y.
	newy += y * dir;

	if (newy * newy < sqhalfheight) {
		result = sqrtf(sqhalfheight - newy * newy) * (1.0f / 160.0f);
	} else {
		result = 0.01f;
	}

	return result;
}

/**
 * Draw the fisheye curved effect when using an eyespy.
 *
 * PAL Beta adds padding above and below to compensate for the higher vertical
 * screen resolution, by adjusting the viewtop and viewheight variables and
 * drawing black filler at the end of the function. However the size of these
 * bars is static regardless of the screen layout and size being used.
 *
 * PAL Final improves on beta's mistake by checking the screen size, but there's
 * no check for a vertical split being used, and as a result the fisheye radius
 * is smaller than it should be when using a horizontal split. @bug
 */
Gfx *bview_draw_fisheye(Gfx *gdl, u32 colour, u32 alpha, s32 shuttertime60, s8 startuptimer60, u8 hit)
{
	u16 *fb = vi_get_back_buffer();
	s32 viewtop;
	s32 viewheight;
	f32 f26;
	f32 halfheight;
	f32 sqhalfheight;
	s32 viewwidth;
	s32 viewleft;
	s32 s2;
	s32 i;
	s32 s3;
	u8 starting;
	s32 curradius;
	f32 startupfrac;
	f32 fullradius;
	s32 one = 1;
	s32 spec;
	u8 alpha2;

#if VERSION >= VERSION_PAL_FINAL && PAL
	s32 vpadding;
#endif

	f32 tmp;

#if VERSION >= VERSION_PAL_FINAL
	viewtop = vi_get_view_top();
	viewheight = vi_get_view_height();
	viewwidth = vi_get_view_width();
	viewleft = vi_get_view_left();

	startupfrac = 1.0f;
	s2 = 0;

#if PAL
	if (PLAYERCOUNT() >= 2
			|| options_get_effective_screen_size() == SCREENSIZE_WIDE
			|| options_get_effective_screen_size() == SCREENSIZE_CINEMA) {
		vpadding = 16;
		viewtop += vpadding;
		viewheight -= vpadding * 2;
	} else {
		vpadding = 23;
		viewtop += vpadding;
		viewheight -= vpadding * 2;
	}
#endif

	halfheight = viewheight * 0.5f;
	sqhalfheight = halfheight * halfheight;
	f26 = -(halfheight + halfheight) / viewheight;
#elif VERSION >= VERSION_PAL_BETA
	viewtop = vi_get_view_top() + 16;
	viewheight = vi_get_view_height() - 32;
	halfheight = viewheight * 0.5f;
	sqhalfheight = halfheight * halfheight;
	f26 = -(halfheight + halfheight) / viewheight;
	viewwidth = vi_get_view_width();
	viewleft = vi_get_view_left();
	startupfrac = 1.0f;
	s2 = 0;
#else
	viewtop = vi_get_view_top();
	viewheight = vi_get_view_height();
	halfheight = viewheight * 0.5f;
	sqhalfheight = halfheight * halfheight;
	f26 = -(halfheight + halfheight) / viewheight;
	viewwidth = vi_get_view_width();
	viewleft = vi_get_view_left();
	startupfrac = 1.0f;
	s2 = 0;
#endif

	starting = (startuptimer60 < TICKS(50));

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "blurGfxFisheye");

	s3 = 1;

	if (starting) {
		fullradius = viewheight * 0.5f;
		startupfrac = startuptimer60 / (PAL ? 41.0f : 50.0f);
		curradius = fullradius * startupfrac;
		spec = startupfrac * 255.0f;

		if (spec > 255) {
			spec = 255;
		}
	}

	gDPPipeSync(gdl++);

	gdl = bview_prepare_static_rgba16(gdl, colour, alpha);

	if (starting) {
		for (i = viewtop; i < viewtop + viewheight; i++) {
			if (i % 2) {
				if (i > viewtop + fullradius - curradius && i < viewtop + fullradius + curradius) {
					gDPSetEnvColorViaWord(gdl++, (colour & 0xffffff00) | (spec & 0xff));

					tmp = bview_get_fisheye_xradius_frac(s2, f26, halfheight, sqhalfheight) * startupfrac;
					gdl = bview_copy_pixels(gdl, fb, i, 5, i, tmp, viewleft, viewwidth);
				}
			}

			s2 += s3;

			if (s2 >= viewheight * 0.5f) {
				s2 = viewheight * 0.5f;
				s3 = -s3;
			}
		}
	} else {
		f32 f22 = 1.0f;

		for (i = viewtop; i < viewtop + viewheight; i++) {
			if (hit == EYESPYHIT_DAMAGE) {
				alpha2 = (random() % 120) + 120;
				colour = 0xff333300 | (alpha2 & 0xff);
				f22 = ((random() % 32) + (f32) FBALLOC_HEIGHT) * (1.0f / 256.0f);

				gDPSetEnvColorViaWord(gdl++, colour);
			} else {
				gDPSetEnvColorViaWord(gdl++, 0xffffffff);
			}

			tmp = bview_get_fisheye_xradius_frac(s2, f26, halfheight, sqhalfheight) * f22;
			gdl = bview_copy_pixels(gdl, fb, i, 5, i, tmp, viewleft, viewwidth);

			if (hit == EYESPYHIT_DAMAGE) {
				gDPSetEnvColorViaWord(gdl++, 0xddaaaa99);

				tmp = bview_get_fisheye_xradius_frac(s2, f26, halfheight, sqhalfheight) * 1.03f;
				gdl = bview_copy_pixels(gdl, fb, i, 5, i, tmp, viewleft, viewwidth);
			}

			s2 += s3;

			if ((i % 2) == 0) {
				gDPSetEnvColorViaWord(gdl++, 0x00000055);
				gDPFillRectangle(gdl++, viewleft, i, viewleft + viewwidth, i + 1);
			}

			if (s2 >= viewheight * 0.5f) {
				s2 = viewheight * 0.5f;
				s3 = -s3;
			}
		}
	}

	gDPPipeSync(gdl++);
	gDPSetCycleType(gdl++, G_CYC_1CYCLE);
	gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
	gDPSetPrimColor(gdl++, 0, 0, 0x00, 0x00, 0x00, 0xff);

	s3 = 1;

	if (shuttertime60 != 0 || starting) {
		s32 s7;
		s32 spa8 = viewheight * 0.5f;
		f32 f20;

		if (!starting) {
			shuttertime60 -= TICKS(12);

			if (shuttertime60 < 0) {
				shuttertime60 = -shuttertime60;
			}

			s7 = spa8 * (shuttertime60 / TICKS(12.0f));
		} else {
			s7 = curradius;
		}

		for (i = viewtop; i < viewtop + spa8 - s7; i++) {
			gdl = bview_draw_fisheye_rect(gdl, i, 0.0f, viewleft, viewwidth);
			gdl = bview_draw_fisheye_rect(gdl, viewtop + viewtop + viewheight - i, 0.0f, viewleft, viewwidth);
		}

		gDPSetPrimColorViaWord(gdl++, 0, 0, 0x000000ff);

		tmp = (f32) one * halfheight;
		f20 = halfheight;

		for (i = viewtop + spa8 - s7; i <= viewtop + spa8; i++) {
			f32 f2;

			if (sqhalfheight > f20 * f20) {
				f2 = sqrtf(sqhalfheight - f20 * f20) * (1.0f / 160.0f);
			} else {
				f2 = 0.01f;
			}

			f20 += -tmp / s7;

			gdl = bview_draw_fisheye_rect(gdl, i, f2 * startupfrac, viewleft, viewwidth);

			if (i != viewtop + viewtop + viewheight - i) {
				gdl = bview_draw_fisheye_rect(gdl, viewtop + viewtop + viewheight - i, f2 * startupfrac, viewleft, viewwidth);
			}
		}
	} else {
		s2 = 0;

		for (i = viewtop; i < viewtop + viewheight; i++) {
			tmp = bview_get_fisheye_xradius_frac(s2, f26, halfheight, sqhalfheight);
			gdl = bview_draw_fisheye_rect(gdl, i, tmp, viewleft, viewwidth);

			s2 += s3;

			if (s2 >= viewheight * 0.5f) {
				s2 = viewheight * 0.5f;
				s3 = -s3;
			}
		}
	}

#if PAL
	s3 = vi_get_view_top();
#if VERSION >= VERSION_PAL_FINAL
	s2 = s3 + vi_get_view_height() - vpadding;
#else
	s2 = s3 + vi_get_view_height() - 16;
#endif

#if VERSION >= VERSION_PAL_FINAL
	for (i = 0; i < vpadding; i++)
#else
	for (i = 0; i < 16; i++)
#endif
	{
		gDPFillRectangle(gdl++, viewleft, s3 , viewleft + viewwidth, s3 + 1);
		gDPFillRectangle(gdl++, viewleft, s2, viewleft + viewwidth, s2 + 1);

		s3++;
		s2++;
	}
#endif

	return gdl;
}

/**
 * Draw a black rectangle to the side of the circular fisheye lens.
 *
 * These are each 1px high, and go from the edge of the circle to the edge of
 * the screen. There is one drawn on every row on both sides.
 */
Gfx *bview_draw_eyespy_side_rect(Gfx *gdl, s32 *points, u8 r, u8 g, u8 b, u8 alpha)
{
	Vtx *vertices = gfx_allocate_vertices(4);
	Col *colours = gfx_allocate_colours(2);

	vertices[0].x = points[0] * 10.0f;
	vertices[0].y = points[1] * 10.0f;
	vertices[0].z = -10;

	vertices[1].x = points[2] * 10.0f;
	vertices[1].y = points[3] * 10.0f;
	vertices[1].z = -10;

	vertices[2].x = points[4] * 10.0f;
	vertices[2].y = points[5] * 10.0f;
	vertices[2].z = -10;

	vertices[3].x = points[6] * 10.0f;
	vertices[3].y = points[7] * 10.0f;
	vertices[3].z = -10;

	colours[0].word = r << 0x18 | g << 0x10 | b << 8 | 0xff;
	colours[1].word = r << 0x18 | g << 0x10 | b << 8 | alpha;

	vertices[0].colour = 0;
	vertices[1].colour = 0;
	vertices[2].colour = 4;
	vertices[3].colour = 4;

	gSPColor(gdl++, colours, 2);
	gSPVertex(gdl++, vertices, 4, 0);

	if (colours);

	gSPTri2(gdl++, 0, 1, 2, 0, 2, 3);

	return gdl;
}

/**
 * Renders the eyespy user interface, excluding the fisheye lens. The lens is
 * drawn first by another function, then this one is called to draw the outer
 * information. Care must be taken not to draw over the top of the fisheye lens.
 *
 * Note that the dimensions of the view can differ based on hi-res on/off, as
 * well as using coop mode in both the vertical and horizontal screen splits.
 * Some elements are omitted if a vertical split is being used, and to handle
 * hi-res a scale variable is used to multiply X values and widths where needed.
 *
 * @bug: Many of the X values and widths are not multiplied by the scale which
 * causes them to display incorrectly when using hi-res:
 * - Some of the horizontal lines don't touch the lens circle.
 * - The vertical lines are thinner and closer to the screen edges than intended.
 * - The speed and height bars are stretched.
 * - The device name and model are closer to the screen edge than intended.
 */
Gfx *bview_draw_eyespy_metrics(Gfx *gdl)
{
	char text[256];
	s32 viewleft = vi_get_view_left();
#if VERSION >= VERSION_PAL_FINAL
	s32 viewwidth = vi_get_view_width();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
#elif VERSION >= VERSION_PAL_BETA
	s32 viewwidth = vi_get_view_width();
	s32 viewtop = vi_get_view_top() + 16;
	s32 viewheight = vi_get_view_height() - 32;
#else
	s32 viewtop = vi_get_view_top();
	s32 viewwidth = vi_get_view_width();
	s32 viewheight = vi_get_view_height();
#endif
	s32 viewright = viewleft + viewwidth - 1;
	s32 viewbottom = viewtop + viewheight - 1;
	s32 x;
	s32 y;
	s32 textwidth;
	s32 textheight;
	s32 x2;
	s32 y2;
	struct chrdata *chr;
	s32 savedy;
	s32 movex;
	s32 movey;
	s32 movez;
	f32 movedist;
	f32 sqmovedist = 0.0f;
	u32 colourtextbright;
	u32 colourtextdull;
	u32 colourglow;
#if PAL
	s32 scale = 1;
	f32 palscale = viewwidth > SCREEN_WIDTH_LO ? 1.4f : 1.0f;
#else
	s32 scale = viewwidth > SCREEN_WIDTH_LO ? 2 : 1;
#endif
#if VERSION >= VERSION_NTSC_1_0
	bool vsplit = false;
#endif

#if VERSION >= VERSION_PAL_FINAL
#if PAL
	if (PLAYERCOUNT() >= 2
			|| options_get_effective_screen_size() == SCREENSIZE_WIDE
			|| options_get_effective_screen_size() == SCREENSIZE_CINEMA) {
		viewtop += 16;
		viewheight -= 32;
	} else {
		viewtop += 23;
		viewheight -= 46;
	}
#endif
	viewbottom = viewtop + viewheight - 1;
#endif

	if (g_Vars.currentplayer->eyespy == NULL
			|| g_Vars.currentplayer->eyespy->prop == NULL
			|| g_Vars.currentplayer->eyespy->prop->chr == NULL) {
		return gdl;
	}

	chr = g_Vars.currentplayer->eyespy->prop->chr;

#if VERSION >= VERSION_NTSC_1_0
	if (options_get_screen_split() == SCREENSPLIT_VERTICAL && PLAYERCOUNT() >= 2) {
		vsplit = true;
	}
#endif

#if VERSION >= VERSION_NTSC_1_0
	movex = chr->prop->pos.x - chr->prevpos.x;
	movey = chr->prop->pos.y - chr->prevpos.y;
	movez = chr->prop->pos.z - chr->prevpos.z;

	if (movex != 0.0f || movey != 0.0f || movez != 0.0f) {
		sqmovedist = movex * movex + movey * movey + movez * movez;
	}

	if (sqmovedist > 0.001f) {
		movedist = sqrtf(sqmovedist);
	} else {
		movedist = 0.0f;
	}
#else
	if (g_Vars.coopplayernum < 0 && g_Vars.antiplayernum < 0) {
		movex = chr->prop->pos.x - chr->prevpos.x;
		movey = chr->prop->pos.y - chr->prevpos.y;
		movez = chr->prop->pos.z - chr->prevpos.z;

		if (movex != 0.0f || movey != 0.0f || movez != 0.0f) {
			sqmovedist = movex * movex + movey * movey + movez * movez;
		}

		if (sqmovedist > 0.001f) {
			movedist = sqrtf(sqmovedist);
		} else {
			movedist = 0.0f;
		}
	} else {
		movedist = 0.0f;
	}
#endif

	if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
		gdl = text_begin_boxmode(gdl, 0x00ff0028);
	} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
		gdl = text_begin_boxmode(gdl, 0x2244ffa0);
	} else {
		gdl = text_begin_boxmode(gdl, 0xff3300a0);
	}

#if VERSION >= VERSION_NTSC_1_0
	if (!vsplit)
#endif
	{
		// Render borders/lines in background
		gDPFillRectangle(gdl++, viewleft + 25, viewtop + 55, viewleft + 26, viewbottom - 24);
		gDPFillRectangle(gdl++, viewleft + 31, viewtop + 55, viewleft + 32, viewbottom - 42);
		gDPFillRectangle(gdl++, viewleft + 25, viewbottom - 25, viewleft + 25 + viewwidth / 5.0f + 1, viewbottom - 24);
		gDPFillRectangle(gdl++, viewleft + 31, viewbottom - 43, viewleft + 25 + viewwidth / 7.0f + 1, viewbottom - 42);
		gDPFillRectangle(gdl++, viewright - 25, viewtop + 25, viewright - 24, viewbottom - 54);
		gDPFillRectangle(gdl++, viewright - 31, viewtop + 43, viewright - 30, viewbottom - 54);
		gDPFillRectangle(gdl++, viewright - 25 - viewwidth / 5.0f, viewtop + 25, viewright - 24, viewtop + 26);
		gDPFillRectangle(gdl++, viewright - 25 - viewwidth / 7.0f, viewtop + 43, viewright - 30, viewtop + 44);
		gDPFillRectangle(gdl++, viewleft, viewtop + 55, viewleft + viewwidth / 5.0f + 1, viewtop + 56);
		gDPFillRectangle(gdl++, viewright - viewwidth / 5.0f, viewbottom - 55, viewright + 1, viewbottom - 54);
	}

	if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
		// Render crosshair
		s32 x = viewleft + (viewwidth >> 1);
		s32 y = viewtop + (viewheight >> 1);

		gDPFillRectangle(gdl++, x + 2, y + 0, x + 7, y + 1);
		gDPFillRectangle(gdl++, x + 2, y + 0, x + 5, y + 1);
		gDPFillRectangle(gdl++, x - 6, y + 0, x - 1, y + 1);
		gDPFillRectangle(gdl++, x - 4, y + 0, x - 1, y + 1);
		gDPFillRectangle(gdl++, x + 0, y + 2, x + 1, y + 7);
		gDPFillRectangle(gdl++, x + 0, y + 2, x + 1, y + 5);
		gDPFillRectangle(gdl++, x + 0, y - 6, x + 1, y - 1);
		gDPFillRectangle(gdl++, x + 0, y - 4, x + 1, y - 1);
	}

	if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
		colourtextbright = 0x00ff00a0;
		colourtextdull = 0x005000ff;
		colourglow = 0x000f00ff;
	} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
		colourtextbright = 0x2244ffff;
		colourtextdull = 0x2244ffff;
		colourglow = 0x00000fff;
	} else {
		colourtextbright = 0xff3300ff;
		colourtextdull = 0xff3300ff;
		colourglow = 0x0f0000ff;
	}

	// "S/MPS"
	sprintf(text, "%s %s%5.2f", lang_get(L_MISC_073), "", movedist * 0.6f);
	savedy = viewtop + 14;
	text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);
	x = viewleft + 25 * scale;
	y = savedy;
	x2 = x + textwidth; \
	y2 = y + textheight; \
	gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);

	gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
			colourtextbright, colourglow, vi_get_width(), vi_get_height(), 0, 0);

	// "H/M"
	sprintf(text, "%s %s%4.2f", lang_get(L_MISC_074), "", g_Vars.currentplayer->eyespy->height * 0.01f);
	savedy += 9;
	text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);
	x = viewleft + 25 * scale;
	y = savedy;
	x2 = x + textwidth; \
	y2 = y + textheight; \
	gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
	gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
			colourtextbright, colourglow, vi_get_width(), vi_get_height(), 0, 0);

	// "Y/D"
	sprintf(text, "%s %d", lang_get(L_MISC_075), (s32)g_Vars.currentplayer->eyespy->theta);
	savedy += 9;
	text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);
	x = viewleft + 25 * scale;
	y = savedy;
	x2 = x + textwidth; \
	y2 = y + textheight; \
	gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
	gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
			colourtextbright, colourglow, vi_get_width(), vi_get_height(), 0, 0);

	// "P/D"
	sprintf(text, "%s %d", lang_get(L_MISC_076), (s32)g_Vars.currentplayer->eyespy->verta);
	savedy += 9;
	text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);
	x = viewleft + 25 * scale;
	y = savedy;
	x2 = x + textwidth; \
	y2 = y + textheight; \
	gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
	gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
			colourtextbright, colourglow, vi_get_width(), vi_get_height(), 0, 0);

	// "CI 2023"
	sprintf(text, "%s", lang_get(L_MISC_077));
	text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);

#if VERSION >= VERSION_NTSC_1_0
	x = (vsplit ? -3 : 0) + viewleft + 25 * scale + 5;
	y = (vsplit ? 18 : 0) + viewbottom - 41;
#else
	x = viewleft + 25 * scale + 5;
	y = viewbottom - 41;
#endif
	x2 = x + textwidth; \
	y2 = y + textheight; \
	gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
	gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
			colourtextdull, colourglow, vi_get_width(), vi_get_height(), 0, 0);

	if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
		sprintf(text, "%s", lang_get(L_MISC_078)); // "YKK: 95935"
	} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
		sprintf(text, "%s", lang_get(L_MISC_208)); // "JM: 201172"
	} else {
		sprintf(text, "%s", lang_get(L_MISC_217)); // "BNC: 15877"
	}

	text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);

#if VERSION >= VERSION_NTSC_1_0
	x = viewleft + 25 * scale + (vsplit ? -3 : 0) + 5;
	y = viewbottom + (vsplit ? 20 : 0) - 34;
#else
	x = viewleft + 25 * scale + 5;
	y = viewbottom - 34;
#endif

	x2 = x + textwidth; \
	y2 = y + textheight; \
	gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
	gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
			colourtextdull, colourglow, vi_get_width(), vi_get_height(), 0, 0);

	if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
		// "CAMSPY"
		sprintf(text, "   %s", lang_get(L_MISC_079));
		text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);

#if VERSION >= VERSION_PAL_FINAL
		x = viewright - textwidth - 27;
#else
		x = viewright - scale * 53 - 25;
#endif
#if VERSION >= VERSION_NTSC_1_0
		y = (vsplit ? -13 : 0) + viewtop + 27;
#else
		y = viewtop + 27;
#endif
		x2 = x + textwidth; \
		y2 = y + textheight; \
		gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
		gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
				colourtextdull, colourglow, vi_get_width(), vi_get_height(), 0, 0);
	} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
		// "DRUGSPY"
		sprintf(text, "   %s", lang_get(L_MISC_468));
		text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);

#if VERSION >= VERSION_PAL_FINAL
		x = viewright - textwidth - 27;
#else
		x = viewright - scale * 53 - 25;
#endif
#if VERSION >= VERSION_NTSC_1_0
		y = (vsplit ? -13 : 0) + viewtop + 27;
#else
		y = viewtop + 27;
#endif
		x2 = x + textwidth; \
		y2 = y + textheight; \
		gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
		gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
				colourtextdull, colourglow, vi_get_width(), vi_get_height(), 0, 0);
	} else {
		// "BOMBSPY"
		sprintf(text, "   %s", lang_get(L_MISC_469));
		text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);

#if VERSION >= VERSION_PAL_FINAL
		x = viewright - textwidth - 27;
#else
		x = viewright - scale * 59 - 25;
#endif
#if VERSION >= VERSION_NTSC_1_0
		y = (vsplit ? -13 : 0) + viewtop + 27;
#else
		y = viewtop + 27;
#endif
		x2 = x + textwidth; \
		y2 = y + textheight; \
		gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
		gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
				colourtextdull, colourglow, vi_get_width(), vi_get_height(), 0, 0);
	}

	// Model number
	if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
		sprintf(text, "%s", lang_get(L_MISC_080)); // "MODEL 1.2"
	} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
		sprintf(text, "%s", lang_get(L_MISC_207)); // "MODEL 1.4"
	} else {
		sprintf(text, "%s", lang_get(L_MISC_216)); // "MODEL 1.3"
	}

	text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);
#if VERSION >= VERSION_PAL_FINAL
	x = (vsplit ? 3 : 0) + viewright - textwidth - 27;
	y = (vsplit ? -11 : 0) + viewtop + 34;
#elif VERSION >= VERSION_NTSC_1_0
	x = (vsplit ? 3 : 0) + viewright - scale * 46 - 25;
	y = (vsplit ? -11 : 0) + viewtop + 34;
#else
	x = viewright - scale * 46 - 25;
	y = viewtop + 34;
#endif
	x2 = x + textwidth; \
	y2 = y + textheight; \
	gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
	gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
			colourtextdull, colourglow, vi_get_width(), vi_get_height(), 0, 0);

	// Gyrostat/dartammo text
	if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY
			|| g_Vars.currentplayer->eyespy->mode == EYESPYMODE_BOMBSPY) {
		sprintf(text, "%s", lang_get(L_MISC_081)); // "GYROSTAT"
	} else {
		sprintf(text, "%s", lang_get(L_MISC_206)); // "DARTAMMO"
	}

	text_measure(&textheight, &textwidth, text, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);
#if VERSION >= VERSION_PAL_FINAL
	x = viewright - textwidth - 27;
	y = (vsplit ? -2 : 0) + viewbottom - 12;
#elif VERSION >= VERSION_NTSC_1_0
	x = (vsplit ? -35 : 0) + (viewright - viewwidth / 5.0f - 3 - (scale * 5 - 5));
	y = (vsplit ? -2 : 0) + viewbottom - 12;
#else
	x = viewright - viewwidth / 5.0f - 3 - (scale * 5 - 5);
	y = viewbottom - 12;
#endif
	if (1);
	x2 = x + textwidth; \
	y2 = y + textheight; \
	gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
	gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicXs, g_FontHandelGothicXs,
			colourtextdull, colourglow, vi_get_width(), vi_get_height(), 0, 0);

	gdl = text_end_boxmode(gdl);

	{
		s8 contpadnum = options_get_contpad_num1(g_Vars.currentplayerstats->mpindex);
		u16 buttonsdown = joy_get_buttons(contpadnum, 0xffff); \
		u16 buttonsthisframe = joy_get_buttons_pressed_this_frame(contpadnum, 0xffff);
		s8 cstickx = joy_get_stick_x(contpadnum); \
		s8 csticky = joy_get_stick_y(contpadnum);
		s32 xpos;
		s32 tmpval;
		u8 brightness;
		u8 brightness2;
		s32 points[8];
		s32 r;
		s32 g;
		s32 b;

		gDPPipeSync(gdl++); \
		gDPSetCycleType(gdl++, G_CYC_1CYCLE); \
		gDPSetColorDither(gdl++, G_CD_DISABLE); \
		gDPSetTexturePersp(gdl++, G_TP_NONE); \
		gDPSetAlphaCompare(gdl++, G_AC_NONE); \
		gDPSetTextureLOD(gdl++, G_TL_TILE); \
		gDPSetTextureFilter(gdl++, G_TF_BILERP); \
		gDPSetTextureConvert(gdl++, G_TC_FILT); \
		gDPSetTextureLUT(gdl++, G_TT_NONE); \
		gDPSetRenderMode(gdl++, G_RM_CLD_SURF, G_RM_CLD_SURF2); \
		gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);

#if VERSION >= VERSION_NTSC_1_0
		if (!vsplit)
#endif
		{
			xpos = (scale == 2) ? -76 : -85;

			// Up
			if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
				brightness = 20; \
				if (buttonsdown & (U_JPAD | U_CBUTTONS)) { \
					brightness += 20; \
				} \
				if (buttonsthisframe & (U_JPAD | U_CBUTTONS)) { \
					brightness += 20; \
				} \
				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff); \
			} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
				brightness = 127;

				if (buttonsdown & (U_JPAD | U_CBUTTONS)) {
					brightness += 63;
				}

				if (buttonsthisframe & (U_JPAD | U_CBUTTONS)) {
					brightness += 63; \
				} \
				gDPSetPrimColor(gdl++, 0, 0, 0x10, 0x20, brightness, 0xff);
			} else {
				brightness = 20;

				if (buttonsdown & (U_JPAD | U_CBUTTONS)) {
					brightness += 20;
				}

				if (buttonsthisframe & (U_JPAD | U_CBUTTONS)) {
					brightness += 20; \
				} \
				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
			}

			gDPFillRectangle(gdl++, xpos * scale + viewright, viewtop + 10, (xpos + 8) * scale + viewright, viewtop + 18);
			xpos += 10;

			// Down
			if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
				brightness = 20;

				if (buttonsdown & (D_JPAD | D_CBUTTONS)) { \
					brightness += 20; \
				} \
				if (buttonsthisframe & (D_JPAD | D_CBUTTONS)) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
			} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
				brightness = 127;

				if (buttonsdown & (D_JPAD | D_CBUTTONS)) {
					brightness += 63;
				}

				if (buttonsthisframe & (D_JPAD | D_CBUTTONS)) {
					brightness += 63;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0x10, 0x20, brightness, 0xff);
			} else {
				brightness = 20;

				if (buttonsdown & (D_JPAD | D_CBUTTONS)) {
					brightness += 20;
				}

				if (buttonsthisframe & (D_JPAD | D_CBUTTONS)) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
			}

			gDPFillRectangle(gdl++, xpos * scale + viewright, viewtop + 10, (xpos + 8) * scale + viewright, viewtop + 18);
			xpos += 10;

			// Left
			if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
				brightness = 20;

				if (buttonsdown & (L_JPAD | L_CBUTTONS)) { \
					brightness += 20; \
				} \
				if (buttonsthisframe & (L_JPAD | L_CBUTTONS)) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
			} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
				brightness = 127;

				if (buttonsdown & (L_JPAD | L_CBUTTONS)) {
					brightness += 63;
				}

				if (buttonsthisframe & (L_JPAD | L_CBUTTONS)) {
					brightness += 63;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0x10, 0x20, brightness, 0xff);
			} else {
				brightness = 20;

				if (buttonsdown & (L_JPAD | L_CBUTTONS)) {
					brightness += 20;
				}

				if (buttonsthisframe & (L_JPAD | L_CBUTTONS)) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
			}

			gDPFillRectangle(gdl++, xpos * scale + viewright, viewtop + 10, (xpos + 8) * scale + viewright, viewtop + 18);
			xpos += 10;

			// Right
			if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
				brightness = 20;

				if (buttonsdown & (R_JPAD | R_CBUTTONS)) { \
					brightness += 20; \
				} \
				if (buttonsthisframe & (R_JPAD | R_CBUTTONS)) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
			} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
				brightness = 127;

				if (buttonsdown & (R_JPAD | R_CBUTTONS)) {
					brightness += 63;
				}

				if (buttonsthisframe & (R_JPAD | R_CBUTTONS)) {
					brightness += 63;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0x10, 0x20, brightness, 0xff);
			} else {
				brightness = 20;

				if (buttonsdown & (R_JPAD | R_CBUTTONS)) {
					brightness += 20;
				}

				if (buttonsthisframe & (R_JPAD | R_CBUTTONS)) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
			}

			gDPFillRectangle(gdl++, xpos * scale + viewright, viewtop + 10, (xpos + 8) * scale + viewright, viewtop + 18);
			xpos += 10;

			// Shoulder buttons
			if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
				brightness = 20;

				if (buttonsdown & (L_TRIG | R_TRIG)) { \
					brightness += 20; \
				} \
				if (buttonsthisframe & (L_TRIG | R_TRIG)) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
			} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
				brightness = 127;

				if (buttonsdown & (L_TRIG | R_TRIG)) {
					brightness += 63;
				}

				if (buttonsthisframe & (L_TRIG | R_TRIG)) {
					brightness += 63;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0x10, 0x20, brightness, 0xff);
			} else {
				brightness = 20;

				if (buttonsdown & (L_TRIG | R_TRIG)) {
					brightness += 20;
				}

				if (buttonsthisframe & (L_TRIG | R_TRIG)) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
			}

			gDPFillRectangle(gdl++, xpos * scale + viewright, viewtop + 10, (xpos + 8) * scale + viewright, viewtop + 18);
			xpos += 10;

			// Z button
			if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
				brightness = 20;

				if (buttonsdown & Z_TRIG) { \
					brightness += 20; \
				} \
				if (buttonsthisframe & Z_TRIG) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
			} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
				brightness = 127;

				if (buttonsdown & Z_TRIG) {
					brightness += 63;
				}

				if (buttonsthisframe & Z_TRIG) {
					brightness += 63;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0x10, 0x20, brightness, 0xff);
			} else {
				brightness = 20;

				if (buttonsdown & Z_TRIG) {
					brightness += 20;
				}

				if (buttonsthisframe & Z_TRIG) {
					brightness += 20;
				}

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
			}

			gDPFillRectangle(gdl++, xpos * scale + viewright, viewtop + 10, (xpos + 8) * scale + viewright, viewtop + 18);
		}

		xpos = (scale == 2) ? -48 : -55;

		// Stick X
		tmpval = cstickx * 96.0f / 80.0f;
		brightness = tmpval < 0 ? -tmpval : tmpval;

		if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
			gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
		} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
			r = brightness / 96.0f * 16.0f;
			g = brightness / 96.0f * 32.0f;
			b = brightness * 2.5f;
			gDPSetPrimColor(gdl++, 0, 0, r, g, b, 0xff);
		} else {
			gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
		}

#if VERSION >= VERSION_NTSC_1_0
		if (!vsplit)
#endif
		{
			tmpval = cstickx * 28.0f / 80.0f;

			if (cstickx > 0) {
				gDPFillRectangle(gdl++,
						xpos * scale + viewright,
						viewtop + 19,
						(tmpval + xpos) * scale + viewright,
						viewtop + 21);
			} else {
				gDPFillRectangle(gdl++,
						(tmpval + xpos) * scale + viewright,
						viewtop + 19,
						xpos * scale + viewright,
						viewtop + 21);
			}
		}

		// Stick Y
		tmpval = csticky * 96.0f / 80.0f;
		brightness = tmpval < 0 ? -tmpval : tmpval;

		if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
			gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
		} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
			r = brightness / 96.0f * 16.0f;
			g = brightness / 96.0f * 32.0f;
			b = brightness * 2.5f;
			gDPSetPrimColor(gdl++, 0, 0, r, g, b, 0xff);
		} else {
			gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
		}

#if VERSION >= VERSION_NTSC_1_0
		if (!vsplit)
#endif
		{
			tmpval = csticky * 28.0f / 80.0f;

			if (csticky > 0) {
				gDPFillRectangle(gdl++,
						xpos * scale + viewright,
						viewtop + 22,
						(tmpval + xpos) * scale + viewright,
						viewtop + 24);
			} else {
				gDPFillRectangle(gdl++,
						(tmpval + xpos) * scale + viewright,
						viewtop + 22,
						xpos * scale + viewright,
						viewtop + 24);
			}
		}

		if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
#if VERSION >= VERSION_NTSC_1_0
			if (!vsplit)
#endif
			{
				// Camspy gyrobar 1
				x = viewright - viewwidth / 5.0f;
				y = viewbottom - 13;

				tmpval = g_Vars.currentplayer->eyespy->theta * 96.0f / 360.0f;
				textheight = g_Vars.currentplayer->eyespy->theta * 35.0f / 360.0f;
				brightness = tmpval < 0 ? -tmpval : tmpval; \
				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Camspy gyrobar 2
				x += scale * 2 + scale * 5;

				tmpval = (g_Vars.currentplayer->eyespy->costheta + 1.0f) * 96.0f * 0.5f;
				textheight = (g_Vars.currentplayer->eyespy->costheta + 1.0f) * 35.0f * 0.5f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Camspy gyrobar 3
				x += scale * 2 + scale * 5;

				tmpval = (g_Vars.currentplayer->eyespy->sintheta + 1.0f) * 96.0f * 0.5f;
				textheight = (g_Vars.currentplayer->eyespy->sintheta + 1.0f) * 35.0f * 0.5f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Camspy gyrobar 4
				x += scale * 2 + scale * 5;

				tmpval = g_Vars.currentplayer->eyespy->verta * 96.0f / 360.0f;
				textheight = g_Vars.currentplayer->eyespy->verta * 35.0f / 360.0f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Camspy gyrobar 5
				x += scale * 2 + scale * 5;

				tmpval = (g_Vars.currentplayer->eyespy->cosverta + 1.0f) * 96.0f * 0.5f;
				textheight = (g_Vars.currentplayer->eyespy->cosverta + 1.0f) * 35.0f * 0.5f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Camspy gyrobar 6
				x += scale * 2 + scale * 5;

				tmpval = (g_Vars.currentplayer->eyespy->sinverta + 1.0f) * 96.0f * 0.5f;
				textheight = (g_Vars.currentplayer->eyespy->sinverta + 1.0f) * 35.0f * 0.5f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, 0, brightness, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				x += scale * 2 + scale * 5;
			}
		} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_BOMBSPY) {
#if VERSION >= VERSION_NTSC_1_0
			if (!vsplit)
#endif
			{
				// Bombspy gyrobar 1
				x = viewright - viewwidth / 5.0f;
				y = viewbottom - 13;

				tmpval = g_Vars.currentplayer->eyespy->theta * 96.0f / 360.0f;
				textheight = g_Vars.currentplayer->eyespy->theta * 35.0f / 360.0f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Bombspy gyrobar 2
				x += scale * 2 + scale * 5;

				tmpval = (g_Vars.currentplayer->eyespy->costheta + 1.0f) * 96.0f * 0.5f;
				textheight = (g_Vars.currentplayer->eyespy->costheta + 1.0f) * 35.0f * 0.5f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Bombspy gyrobar 3
				x += scale * 2 + scale * 5;

				tmpval = (g_Vars.currentplayer->eyespy->sintheta + 1.0f) * 96.0f * 0.5f;
				textheight = (g_Vars.currentplayer->eyespy->sintheta + 1.0f) * 35.0f * 0.5f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Bombspy gyrobar 4
				x += scale * 2 + scale * 5;

				tmpval = g_Vars.currentplayer->eyespy->verta * 96.0f / 360.0f;
				textheight = g_Vars.currentplayer->eyespy->verta * 35.0f / 360.0f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Bombspy gyrobar 5
				x += scale * 2 + scale * 5;

				tmpval = (g_Vars.currentplayer->eyespy->cosverta + 1.0f) * 96.0f * 0.5f;
				textheight = (g_Vars.currentplayer->eyespy->cosverta + 1.0f) * 35.0f * 0.5f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				// Bombspy gyrobar 6
				x += scale * 2 + scale * 5;

				tmpval = (g_Vars.currentplayer->eyespy->sinverta + 1.0f) * 96.0f * 0.5f;
				textheight = (g_Vars.currentplayer->eyespy->sinverta + 1.0f) * 35.0f * 0.5f;
				brightness = tmpval < 0 ? -tmpval : tmpval;

				gDPSetPrimColor(gdl++, 0, 0, brightness, brightness >> 2, 0, 0xff);
				gDPFillRectangle(gdl++, x, y - textheight, x + scale * 5, y);

				x += scale * 2 + scale * 5;
			}
		} else {
			// Drugspy ammo
			s32 i;
#if VERSION >= VERSION_NTSC_1_0
			s32 width;
#endif

			brightness2 = 255;
#if VERSION >= VERSION_NTSC_1_0
			width = scale * 30;
#endif

			y = viewbottom - 13;
			x = viewright - viewwidth / 5.0f + 5;

#if VERSION >= VERSION_NTSC_1_0
			if (vsplit) {
				x -= 12;
				y -= 3;
				width = 15;
			}
#endif

			for (i = 0; i < MAX_EYESPYDARTS; i++) {
				if (i >= g_Vars.currentplayer->eyespydarts) {
					brightness2 = 0x88;
				}

				gDPSetPrimColor(gdl++, 0, 0, 0x10, 0x20, brightness2, 0xff);
#if VERSION >= VERSION_NTSC_1_0
				gDPFillRectangle(gdl++, x, y - 4, x + width, y);
#else
				gDPFillRectangle(gdl++, x, y - 4, x + 30 * scale, y);
#endif

				y -= 5;
			}
		}

		gDPPipeSync(gdl++); \
		gDPSetColorDither(gdl++, G_CD_BAYER); \
		gDPSetTexturePersp(gdl++, G_TP_PERSP); \
		gDPSetTextureLOD(gdl++, G_TL_LOD);

		if (g_Vars.coopplayernum < 0 && g_Vars.antiplayernum < 0) {
			s32 barheight = (viewbottom - viewtop - 103) / 17.0f - 1;
			s32 centrey = viewheight / 2.0f;
			s32 sqcentrey = centrey * centrey;
			s32 ypos;
			u8 alpha;
			u32 stack;
			u32 stack2;
			s8 yoffset;
			s32 value;
			s32 i;

#if VERSION == VERSION_PAL_BETA
			if (centrey);
#endif

			value = 17.0f * movedist / 25.0f;

			if (value > 17) {
				value = 17;
			}

			value = 17 - value;

			if (viewheight == FBALLOC_HEIGHT) {
				yoffset = 10;
			} else if (viewheight == 180) { // screen size: wide
				barheight--;
				yoffset = -8;
			} else {
				yoffset = 0;
			}

			gdl = ortho_begin(gdl);

			gSPClearGeometryMode(gdl++, G_CULL_BOTH);
			gSPSetGeometryMode(gdl++, G_SHADE | G_SHADING_SMOOTH);
			gDPSetCombineMode(gdl++, G_CC_SHADE, G_CC_SHADE);
			gDPSetTextureFilter(gdl++, G_TF_BILERP);
			gDPSetCycleType(gdl++, G_CYC_1CYCLE);
			gDPSetRenderMode(gdl++, G_RM_AA_XLU_SURF, G_RM_AA_XLU_SURF2);

			// Speed bars (left side)
			y = viewtop + 58;

			if (viewheight == 180) { // screen size: wide
				y += 5;
			}

			ypos = centrey - y + 10;

#if VERSION == VERSION_PAL_BETA
			ypos += 16;
#endif

#if VERSION == VERSION_PAL_FINAL
			if (PLAYERCOUNT() >= 2
					|| options_get_effective_screen_size() == SCREENSIZE_WIDE
					|| options_get_effective_screen_size() == SCREENSIZE_CINEMA) {
				ypos += 16;
			} else {
				ypos += 23;
			}
#endif

			for (i = 0; i < 17; i++) {
				brightness = i < value ? 40 : 80;
				alpha = i < value ? 0x22 : 0x56;

				points[0] = x = viewleft + 34;
				points[1] = y;
				points[2] = x = viewleft + 34;
				points[3] = y + barheight;
#if PAL
				points[6] = x = viewwidth / 2.0f - sqrtf(sqcentrey - (ypos - yoffset) * (ypos - yoffset)) * palscale - 5.0f;
#else
				points[6] = x = viewwidth / 2.0f - sqrtf(sqcentrey - (ypos - yoffset) * (ypos - yoffset)) * scale - 5.0f;
#endif
				points[5] = y + barheight;

				ypos -= barheight;

#if PAL
				points[4] = x = viewwidth / 2.0f - sqrtf(sqcentrey - (ypos - yoffset) * (ypos - yoffset)) * palscale - 5.0f;
#else
				points[4] = x = viewwidth / 2.0f - sqrtf(sqcentrey - (ypos - yoffset) * (ypos - yoffset)) * scale - 5.0f;
#endif
				points[7] = y;

				ypos -= 2;

				if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
					gdl = bview_draw_eyespy_side_rect(gdl, points, 0, brightness, 0, alpha);
				} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
					gdl = bview_draw_eyespy_side_rect(gdl, points, 0x10, brightness, brightness * 3, alpha);
				} else {
					gdl = bview_draw_eyespy_side_rect(gdl, points, brightness, brightness >> 2, 0, alpha);
				}

				y += barheight;
				y += 2;
			}

			// Height bars (right side)
			value = g_Vars.currentplayer->eyespy->height * 17.0f * (1.0f / 160.0f);

			if (value > 17) {
				value = 17;
			}

			value = 17 - value;

			y = viewtop + 46;

			if (viewheight == 180) { // screen size: wide
				y += 5;
			}

			ypos = centrey - y + 10;

#if VERSION == VERSION_PAL_BETA
			ypos += 16;
#endif

#if VERSION == VERSION_PAL_FINAL
			if (PLAYERCOUNT() >= 2
					|| options_get_effective_screen_size() == SCREENSIZE_WIDE
					|| options_get_effective_screen_size() == SCREENSIZE_CINEMA) {
				ypos += 16;
			} else {
				ypos += 23;
			}
#endif

			for (i = 0; i < 17; i++) {
				brightness = i < value ? 40 : 80;
				alpha = i < value ? 0x22 : 0x56;

				points[0] = x = viewright - 34;
				points[1] = y;
				points[3] = y + barheight;
				points[2] = x = viewright - 34;
#if PAL
				points[6] = x = viewwidth / 2.0f + sqrtf(sqcentrey - (ypos - yoffset) * (ypos - yoffset)) * palscale + 5.0f;
#else
				points[6] = x = viewwidth / 2.0f + sqrtf(sqcentrey - (ypos - yoffset) * (ypos - yoffset)) * scale + 5.0f;
#endif
				points[5] = y + barheight;

				ypos -= barheight;

#if PAL
				points[4] = x = viewwidth / 2.0f + sqrtf(sqcentrey - (ypos - yoffset) * (ypos - yoffset)) * palscale + 5.0f;
#else
				points[4] = x = viewwidth / 2.0f + sqrtf(sqcentrey - (ypos - yoffset) * (ypos - yoffset)) * scale + 5.0f;
#endif
				points[7] = y;

				ypos -= 2;

				if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_CAMSPY) {
					gdl = bview_draw_eyespy_side_rect(gdl, points, 0, brightness, 0, alpha);
				} else if (g_Vars.currentplayer->eyespy->mode == EYESPYMODE_DRUGSPY) {
					gdl = bview_draw_eyespy_side_rect(gdl, points, 0x10, brightness, brightness * 3U, alpha);
				} else {
					gdl = bview_draw_eyespy_side_rect(gdl, points, brightness, brightness >> 2, 0, alpha);
				}

				y += barheight;
				y += 2;
			}
		}

		gdl = ortho_end(gdl);
	}

	return gdl;
}

void bview0f1572f8(void)
{
	// empty
}

u8 var8007f878 = 0;

Gfx *bview_draw_nv_lens(Gfx *gdl)
{
	u16 *fb = vi_get_back_buffer();
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 viewtop = vi_get_view_top();
	s32 viewleft = vi_get_view_left();
	s32 viewbottom = viewtop + viewheight;
	s32 brightness;
	s32 y;

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "Fullscreen_DrawFaultScope");

	g_GooglesRoomBrightness = 0xbc;
	g_GogglesChrColourIntensity = 0xbe;
	g_GogglesChrColourAlpha = 0xde;
	g_GogglesObjColourIntensity = 0x50;
	g_GogglesObjColourAlpha = 0xc0;

	brightness = room_get_final_brightness(g_Vars.currentplayer->prop->rooms[0]);

	if (brightness > 128) {
		sky_set_overexposure(brightness, brightness, brightness);
	}

	if (g_Menus[g_Vars.currentplayerstats->mpindex].curdialog == NULL) {
		gdl = bview_draw_motion_blur(gdl, 0x00ff0000, 0x60);
	}

	gDPPipeSync(gdl++);

	gdl = bview_prepare_static_rgba16(gdl, 0xffffffff, 0xff);

	var8007f878++;

	for (y = viewtop; y < viewbottom; y++) {
		u8 green;

		if (((var8007f878 & 1) != (y & 1)) != 0) {
			u8 tmp = random() % 12;
			green = 0xff - tmp;
		} else {
			green = 0x94;
		}

		gDPSetColor(gdl++, G_SETENVCOLOR, (green << 16) + 0xff);

		gdl = bview_copy_pixels(gdl, fb, y, 5, y, 1, viewleft, viewwidth);
	}

	return gdl;
}

/**
 * Night vision doesn't have binoculars.
 */
Gfx *bview_draw_nv_binoculars(Gfx *gdl)
{
	return gdl;
}

Gfx *bview_draw_ir_lens(Gfx *gdl)
{
	s32 i;
	s32 fadeincrement;
	u16 *fb = vi_get_back_buffer();
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 viewtop = vi_get_view_top();
	s32 viewleft = vi_get_view_left();
	s32 viewright;
	s32 viewbottom;
	s32 viewcentrex;
	s32 sqinnerradius;
	s32 stack[3];
	s32 scanincrement;
	s32 scantop;
	s32 scanbottom;
	s32 scanrate = 4;
	s32 faderate = 2;
	u32 red;
	s32 outerradius;
	s32 innerradius;
	s32 viewcentrey;
	f32 viewheightf;
	s32 a0;

#if VERSION < VERSION_NTSC_1_0
	static s32 fsscanline = 0;
	static s32 fslastradius = -1;
#endif

	viewright = viewleft + viewwidth;
	viewcentrex = (viewleft + viewright) / 2;

	outerradius = g_IrBinocularRadius;
	innerradius = g_IrBinocularRadius / g_IrCentreSize;

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "Fullscreen_DrawFaultScope");

#if VERSION < VERSION_NTSC_1_0
	osSyncPrintf("Fault Scope is active\n");
	bview_prompt_ir();
#endif

	viewbottom = viewtop + viewheight;
	viewcentrey = (viewtop + viewbottom) / 2;
	scantop = viewcentrey - outerradius;
	scanbottom = viewcentrey + outerradius;
	i = viewheight;

	if (scantop > viewbottom) {
		scantop = viewbottom;
	}

#if VERSION >= VERSION_NTSC_1_0
	if (scanbottom > viewbottom) {
		scanbottom = viewbottom;
	}
#else
	if (scantop > viewbottom) {
		scantop = viewbottom;
	}
#endif

	if (scantop < viewtop) {
		scantop = viewtop;
	}

	if (scanbottom < viewtop) {
		scanbottom = viewtop;
	}

	scanincrement = (f32) scanrate * i / 240.0f;
	fadeincrement = (f32) faderate * i / 240.0f;

	// This code runs on the first frame of IR use (90 != 0),
	// and in debug versions developers could change the radius at runtime.
#if VERSION >= VERSION_NTSC_1_0
	if (g_Vars.currentplayer->fslastradius != outerradius) {
		for (i = 0; i < 480; i++) {
			g_IrScanlines[g_Vars.currentplayernum][i] = 0xff;
		}

		g_Vars.currentplayer->fsscanline = 0;
		g_Vars.currentplayer->fslastradius = outerradius;
	}
#else
	if (fslastradius != outerradius) {
		for (i = 0; i < 480; i++) {
			g_IrScanlines[0][i] = 0xff;
		}

		fsscanline = 0;
		fslastradius = outerradius;
	}
#endif

	// Increment the scanline
#if VERSION >= VERSION_NTSC_1_0
	for (i = 0; i < scanincrement; i++) {
		if (g_Vars.currentplayer->fsscanline >= scanbottom) {
			g_Vars.currentplayer->fsscanline = scantop;
		}

		g_IrScanlines[g_Vars.currentplayernum][g_Vars.currentplayer->fsscanline] = 0xff - i;

		g_Vars.currentplayer->fsscanline++;
	}
#else
	for (i = 0; i < scanincrement; i++, fsscanline++) {
		if (fsscanline == scanbottom) {
			fsscanline = scantop;
		}

		g_IrScanlines[0][fsscanline] = 0xff - i;
	}
#endif

	g_GooglesRoomBrightness = 0xff;
	g_GogglesChrColourIntensity = 0xde;
	g_GogglesChrColourAlpha = 0xde;

	gDPPipeSync(gdl++);

	gdl = bview_prepare_static_rgba16(gdl, 0xffffffff, 255);

	sqinnerradius = innerradius * innerradius;

	for (i = scantop; i < scanbottom; i++) {
#if VERSION >= VERSION_NTSC_1_0
		if (i & 1) {
			red = g_IrScanlines[g_Vars.currentplayernum][i];
		} else {
			red = g_IrScanlines[g_Vars.currentplayernum][i] * 2 / 3;
		}
#else
		if (i & 1) {
			red = g_IrScanlines[0][i];
		} else {
			red = g_IrScanlines[0][i] * 2 / 3;
		}
#endif

		red += random() % 8;

		if (red > 255) {
			red = 255;
		}

		gDPSetEnvColorViaWord(gdl++, (red << 24) + 0xff);

		a0 = viewcentrey - i;

		if (a0 * a0 < sqinnerradius) {
			// Rendering a line that overlaps the semicircle
			// in the middle of the screen
			f32 f0 = a0;
			s32 semicirclewidth = sqrtf(sqinnerradius - (s32) (f0 * f0)) * (viewwidth / (f32) SCREEN_WIDTH_LO);
			s32 semicircleright = viewcentrex + semicirclewidth;
			s32 rightsidewidth = viewwidth - semicircleright;

			// Left and right of semicircle
			gdl = bview_copy_pixels(gdl, fb, i, 5, i, 1.0f, viewleft, viewcentrex);
			gdl = bview_copy_pixels(gdl, fb, i, 5, i, 1.0f, semicircleright, rightsidewidth);

			// The semicircle itself has a static colour
			gDPSetEnvColorViaWord(gdl++, 0xee0000ff);
			gdl = bview_copy_pixels(gdl, fb, i, 5, i, 1.0f, viewcentrex, semicirclewidth);
		} else {
			gdl = bview_copy_pixels(gdl, fb, i, 5, i, 1.0f, viewleft, viewwidth);
		}

#if VERSION >= VERSION_NTSC_1_0
		if (g_IrScanlines[g_Vars.currentplayernum][i] > fadeincrement) {
			g_IrScanlines[g_Vars.currentplayernum][i] -= fadeincrement;
		}
#else
		if (g_IrScanlines[0][i] > fadeincrement) {
			g_IrScanlines[0][i] -= fadeincrement;
		}
#endif
	}

	if (g_Menus[g_Vars.currentplayerstats->mpindex].curdialog == NULL) {
		gdl = bview_draw_motion_blur(gdl, 0xff000000, 0x40);
	}

	return gdl;
}

/**
 * Draw a horizontal blur/sretch effect. Unused.
 *
 * The term "Intro" used in the string suggests that was made for an older
 * version of the title screen, similar to bview_draw_intro_text.
 */
Gfx *bview_draw_intro_fader_blur(Gfx *gdl, s32 arg1)
{
	u16 *fb = vi_get_back_buffer();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 viewleft = vi_get_view_left();
	f32 halfheight;
	f32 extra;
	s32 y;

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "IntroFaderBlurGfx");

	gDPPipeSync(gdl++);

	gdl = bview_prepare_static_rgba16(gdl, 0xffffffff, 255);

	halfheight = viewheight * 0.5f;

	extra = 0.5f;
	extra += 0.5f;

	for (y = viewtop; y < viewtop + viewheight; y++) {
		f32 frac = (y - viewtop - halfheight) / halfheight;

		if (frac < 0.0f) {
			frac = -frac;
		}

		frac += extra;

		if (frac > 1.0f) {
			frac = 1.0f;
		}

		gdl = bview_copy_pixels(gdl, fb, y, 5, y, RANDOMFRAC() * frac + 1.0f, viewleft, viewwidth);
	}

	return gdl;
}

/**
 * Called from the title screen's "Rare Presents" mode, which is unused.
 */
Gfx *bview_draw_intro_text(Gfx *gdl)
{
	u16 *fb = vi_get_back_buffer();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 viewleft = vi_get_view_left();
	s32 y;

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "IntroTextInterfereGfx");

	gDPPipeSync(gdl++);

	gdl = bview_prepare_static_rgba16(gdl, 0x8f8f8f8f, 255);

	for (y = viewtop; y < viewtop + viewheight; y += 2) {
		gdl = bview_copy_pixels(gdl, fb, y, 5, y, 1.0f, viewleft, viewwidth);
	}

	return gdl;
}

Gfx *bview_draw_horizon_scanner(Gfx *gdl)
{
	u16 *fb = vi_get_back_buffer();
	s32 viewtop = vi_get_view_top();
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 viewleft = vi_get_view_left();
	char directiontext[32];
	char hertztext[24];
	char zoomtext[24];
#if VERSION >= VERSION_NTSC_1_0
	char nametext[52];
#endif
	f32 lookx = g_Vars.currentplayer->cam_look.x;
	f32 lookz = g_Vars.currentplayer->cam_look.z;
	s32 x;
	s32 y;

	char directions[][3] = {
		{'n', '\0', '\0'},
		{'n', 'e',  '\0'},
		{'e', '\0', '\0'},
		{'s', 'e',  '\0'},
		{'s', '\0', '\0'},
		{'s', 'w',  '\0'},
		{'w', '\0', '\0'},
		{'n', 'w',  '\0'},
		{'n', '\0', '\0'},
	};

	s32 turnangle = RTOD(atan2f(-lookx, lookz));
	f32 fovy;
	char arrows[12];
	s32 tmplensheight = 130;
	s32 lenstop;
	s32 lensheight;
	s32 liney;
	s32 scale = 1;
	s32 vsplit = false;
	u32 colour;
	f32 range;

	var8007f840++;

	if (var8007f840 >= 2) {
		return gdl;
	}

	strcpy(var800a41c0, "BinocularViewGfx");

	if (!PAL && g_ViRes == VIRES_HI) {
		scale = 2;
	}

	if (options_get_screen_split() == SCREENSPLIT_VERTICAL && PLAYERCOUNT() >= 2) {
		vsplit = true;
	}

	if (tmplensheight > viewheight - 30) {
		tmplensheight = viewheight - 30;
	}

	if (((s32)(g_20SecIntervalFrac * 30.0f) & 1) == 1) {
		sprintf(arrows, ">> ");
	} else {
		sprintf(arrows, " >>");
	}

	lensheight = tmplensheight;
	lenstop = viewtop + (viewheight / 2) - (lensheight / 2);

	// Black out areas above and below lens
	gdl = text_begin_boxmode(gdl, 0x000000ff);

	gDPFillRectangle(gdl++, viewleft, viewtop, viewleft + viewwidth, lenstop);
	gDPFillRectangle(gdl++, viewleft, lenstop + lensheight, viewleft + viewwidth, viewtop + viewheight);

	gdl = text_end_boxmode(gdl);

	// Prepare text buffers
	sprintf(directiontext, "%s %s:%03d", arrows, &directions[(turnangle + 22) / 45], turnangle);
	sprintf(hertztext, "%s %s%s%4.2fh", arrows, "", "", menu_get_cos_osc_frac(4) * 4.6f + 917.4f);

	fovy = vi_get_fov_y();

	if (fovy == 0 || fovy == 60.0f) {
		fovy = 1;
	} else {
		fovy = 60.0f / fovy + 1;
	}

	sprintf(zoomtext, "%s %s%s%4.2fX", arrows, "", "", fovy);

	gdl = text_begin(gdl);

	// Arrows left of product name
	if (vsplit) {
		x = viewleft + 15 * scale;
	} else {
		x = viewleft + 25 * scale;
	}

	y = lenstop - 7;
	gdl = text_render_v2(gdl, &x, &y, arrows,
			g_CharsHandelGothicXs, g_FontHandelGothicXs, 0xffffff7f, vi_get_width(), vi_get_height(), 0, 0);

	// Product name
#if VERSION >= VERSION_NTSC_1_0
	strcpy(nametext, " JMBC");

	if (!vsplit) {
		strcat(nametext, " WIDE BAND");
	}

	strcat(nametext, " SCANNER\n");

	gdl = text_render_v2(gdl, &x, &y, nametext,
			g_CharsHandelGothicXs, g_FontHandelGothicXs, 0xffffff7f, vi_get_width(), vi_get_height(), 0, 0);
#else
	gdl = text_render_v2(gdl, &x, &y, " JMBC WIDE BAND SCANNER\n",
			g_CharsHandelGothicXs, g_FontHandelGothicXs, 0xffffff7f, vi_get_width(), vi_get_height(), 0, 0);
#endif

	// Hertz
	x = viewleft + 75 * scale;
	y = lenstop + lensheight + 1;
	gdl = text_render_v2(gdl, &x, &y, hertztext,
			g_CharsHandelGothicXs, g_FontHandelGothicXs, 0xffffff7f, vi_get_width(), vi_get_height(), 0, 0);

	// Zoom level
	if (vsplit) {
		x = viewleft + 75 * scale;
		y = lenstop + lensheight + 8;
	} else {
		x = viewleft + 150 * scale;
		y = lenstop + lensheight + 1;
	}

	gdl = text_render_v2(gdl, &x, &y, zoomtext,
			g_CharsHandelGothicXs, g_FontHandelGothicXs, 0xffffff7f, vi_get_width(), vi_get_height(), 0, 0);

	// Direction
	if (vsplit) {
		x = viewleft + 75 * scale;
		y = lenstop + lensheight + 15;
	} else {
		x = viewleft + 225 * scale;
		y = lenstop + lensheight + 1;
	}

	gdl = text_render_v2(gdl, &x, &y, directiontext,
			g_CharsHandelGothicXs, g_FontHandelGothicXs, 0xffffff7f, vi_get_width(), vi_get_height(), 0, 0);
	gdl = text_end(gdl);

	gDPPipeSync(gdl++);

	gdl = bview_prepare_static_rgba16(gdl, 0xffffffff, 255);

	if (vsplit) {
		vsplit = 14;
	}

	// Iterate horizontal lines down the lens with a bit extra on top and bottom
	for (liney = lenstop - 9; liney < lenstop + lensheight + vsplit + 9; liney++) {
		if (liney < lenstop + lensheight && liney >= lenstop) {
			// Inside the lens
			if ((liney % 2) == 0) {
				colour = 0x00ffffff;
			} else {
				colour = 0x7fffffff;
			}

			range = (liney - lenstop - lensheight * 0.5f) / (lensheight * 0.5f);

			if (range < 0) {
				range = -range;
			}

			if (range > 1) {
				range = 0;
			}

			range = (range - 0.75f) * 4.0f;

			if (range < 0) {
				range = 0;
			}

			if (range > 0) {
				colour = colour_blend(0x000000ff, colour, range * 255.0f);
			}
		} else {
			// Outside of the lens
			if ((liney % 2) == 0) {
				colour = 0x007f7fff;
			} else {
				colour = 0x7fffffff;
			}

			range = 0;
		}

		// Different coloured lines at 1/4 and 3/4 marks in the lens
		if (liney == lenstop + lensheight / 4 || liney == lenstop + lensheight - lensheight / 4) {
			colour = 0xffffffff;
		}

		gDPSetColor(gdl++, G_SETENVCOLOR, colour);

		gdl = bview_copy_pixels(gdl, fb, liney, 5, liney, RANDOMFRAC() * range + 1, viewleft, viewwidth);
	}

	return gdl;
}

/**
 * Draws the black part of the IR scanner, which obscures the edges of the
 * screen and creates the binocular effect.
 *
 * The two circles are "placed" at the 1/3 and 2/3 marks horizontally. The
 * screen is then iterated top to bottom, one line at a time, and draws
 * black rectangles on each line to fill in the area outside the circles.
 */
Gfx *bview_draw_ir_binoculars(Gfx *gdl)
{
	s32 viewheight = vi_get_view_height();
	s32 viewwidth = vi_get_view_width();
	s32 viewtop = vi_get_view_top();
	s32 viewleft = vi_get_view_left();
	s32 viewright = viewleft + viewwidth;
	s32 viewbottom = viewtop + viewheight;
	s32 leftx = viewleft + viewwidth / 3;
	s32 rightx = viewleft + (viewwidth * 2) / 3;
	s32 centrey = (viewtop + viewbottom) / 2;
	s32 radius = g_IrBinocularRadius;
	s32 sqradius = radius * radius;
	s32 y;
	u32 stack[2];

	gDPPipeSync(gdl++);
	gDPSetCycleType(gdl++, G_CYC_1CYCLE);
	gDPSetRenderMode(gdl++, G_RM_XLU_SURF, G_RM_XLU_SURF2);
	gDPSetCombineMode(gdl++, G_CC_PRIMITIVE, G_CC_PRIMITIVE);
	gDPSetPrimColor(gdl++, 0, 0, 0x00, 0x00, 0x00, 0xff);

	for (y = viewtop; y < viewbottom; y++) {
		s32 ytocentre = centrey - y;
		s32 sqytocentre = ytocentre * ytocentre;

		if (sqytocentre < sqradius) {
			s32 xoffset = (viewwidth / (f32) SCREEN_WIDTH_LO) * sqrtf(sqradius - sqytocentre);

			// Left side
			if (leftx - xoffset > viewleft) {
				gdl = bview_draw_ir_rect(gdl, viewleft, y, leftx - xoffset, y + 1);
			}

			// Middle (top and bottom)
			if (leftx + xoffset < rightx - xoffset) {
				gdl = bview_draw_ir_rect(gdl, leftx + xoffset, y, rightx - xoffset, y + 1);
			}

			// Right side
			if (rightx + xoffset < viewright) {
				gdl = bview_draw_ir_rect(gdl, rightx + xoffset, y, viewright, y + 1);
			}
		} else {
			// Very top or bottom - whole line is black
#if VERSION >= VERSION_NTSC_1_0
			gdl = bview_draw_ir_rect(gdl, viewleft, y, viewright, y + 1);
#else
			gdl = bview_draw_ir_rect(gdl, viewleft, y, viewright - 1, y + 1);
#endif
		}
	}

	return gdl;
}

Gfx *bview0f148b38(Gfx *gdl)
{
	return gdl;
}

void bview_set_motion_blur(u32 bluramount)
{
	var8007f840 = 0;
	var8007f848 = 0;
	var8007f844 = (bluramount << 1) / 3;
}

void bview_clear_motion_blur(void)
{
	var8007f844 = 0;
}
