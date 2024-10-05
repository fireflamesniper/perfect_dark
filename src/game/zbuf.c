#include <ultra64.h>
#include "constants.h"
#include "lib/sched.h"
#include "game/player.h"
#include "game/zbuf.h"
#include "game/mplayer/mplayer.h"
#include "game/options.h"
#include "bss.h"
#include "lib/vi.h"
#include "lib/memp.h"
#include "data.h"
#include "types.h"

u32 g_ZbufWidth;
u32 g_ZbufHeight;
u16 g_ArtifactDepths0[384];
u16 g_ArtifactDepths1[384];
u16 g_ArtifactDepths2[384];

u16 *g_ZbufPtr1 = NULL;
u16 *g_ZbufPtr2 = NULL;

void zbuf_allocate(void);

void *zbuf_get_allocation(void)
{
	return g_ZbufPtr1;
}

void zbuf_reset(s32 stagenum)
{
	g_ZbufPtr1 = NULL;
	g_ZbufPtr2 = NULL;

	if (stagenum != STAGE_TITLE) {
		zbuf_allocate();
	}
}

/**
 * In 4MB 2-player, the viewports are displayed with a vertical split and are
 * only half a screen height (the top 25% and bottom 25% of the screen are
 * black), so a half-height z-buffer is allocated.
 *
 * In 8MB, the full hi-res buffer is allocated. This makes sense for solo
 * missions because the player can switch to hi-res mid game. For normal
 * multiplayer this is wasteful but there's plenty of memory. For coop and anti
 * this is also wasteful, and memory is tight. They could have saved 137.5 KB.
 *
 * The allocation sizes need to enforce a minimum because the allocation is also
 * used by lighting initialisation code.
 */
void zbuf_allocate(void)
{
	if (IS4MB()) {
		g_ZbufWidth = MAX(320, FBALLOC_WIDTH_LO);

		if (g_Vars.normmplayerisrunning && PLAYERCOUNT() >= 2) {
			g_ZbufHeight = MAX(220, FBALLOC_HEIGHT_LO) / 2;
		} else {
			g_ZbufHeight = MAX(220, FBALLOC_HEIGHT_LO);
		}
	} else {
		g_ZbufWidth = MAX(640, FBALLOC_WIDTH_HI);

		if (g_Vars.normmplayerisrunning && PLAYERCOUNT() >= 2) {
			g_ZbufHeight = MAX(220, FBALLOC_HEIGHT_HI);
		} else {
			g_ZbufHeight = MAX(220, FBALLOC_HEIGHT_HI);
		}
	}

	g_ZbufPtr1 = memp_alloc(g_ZbufWidth * g_ZbufHeight * sizeof(u16) + 0x40, MEMPOOL_STAGE);
	g_ZbufPtr1 = (void *) (((uintptr_t) g_ZbufPtr1 + 0x3f) & ~0x3f);
	g_ZbufPtr2 = g_ZbufPtr1;
}

/**
 * In an older implementation, the game almost certainly had two z-buffers.
 * It needs a fresh z-buffer after rendering the scene and before rendering the
 * player's gun, but it also needs to read values off both z-buffers for
 * lighting purposes. In this older implementation, two z-buffers would have
 * been used and swapped.
 *
 * Due to memory limitations, this method was removed and an alternative
 * solution was used to read this values.
 *
 * This function is still called but it does nothing.
 * There is only one z-buffer, and both pointers always have the same value.
 */
void zbuf_swap(void)
{
	g_ZbufPtr2 = g_ZbufPtr1;
}

/**
 * In 8MB multiplayer, players on the bottom half of the screen have their
 * z-buffer shifted backwards by half a screen. This is safe because it's using
 * a scissor on the viewport.
 *
 * This allows the z-buffer allocation to be half a screen instead of a full
 * screen, however zbuf_allocate allocates the full hi-res screen for 8MB,
 * so this benefit is not realised. The shifting code is likely from GE.
 */
Gfx *zbuf_configure_rdp(Gfx *gdl)
{
	u32 subamount;
	uintptr_t addr;

	if (g_Vars.normmplayerisrunning
			&& (g_Vars.currentplayernum >= 2 || (PLAYERCOUNT() == 2 && g_Vars.currentplayernum == 1))) {
		subamount = player_get_fb_width() * player_get_fb_height();

		if (IS4MB() || options_get_screen_split() == SCREENSPLIT_VERTICAL) {
			subamount = 0;
		}
	} else {
		subamount = 0;
	}

	addr = (uintptr_t) g_ZbufPtr2 - subamount;
	addr &= ~0x3f;

	gDPPipeSync(gdl++);
	gDPSetDepthImage(gdl++, addr);

	return gdl;
}

/**
 * Clear the current player's portion of the z-buffer.
 */
Gfx *zbuf_clear(Gfx *gdl)
{
	s32 left;
	s32 right;

	gDPPipeSync(gdl++);
	gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
	gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, vi_get_width(), OS_PHYSICAL_TO_K0(g_ZbufPtr2));
	gDPSetCycleType(gdl++, G_CYC_FILL);
	gDPSetFillColor(gdl++, 0xfffcfffc);
	gDPSetScissorFrac(gdl++, G_SC_NON_INTERLACE, 0, 0, player_get_fb_width() * 4.0f, player_get_fb_height() * 4.0f);

	if (PLAYERCOUNT() <= 2) {
		left = 0;
		right = player_get_fb_width() - 1;
	} else if (g_Vars.currentplayernum == 0 || g_Vars.currentplayernum == 2) {
		left = 0;
		right = player_get_fb_width() / 2 - 1;
	} else {
		left = player_get_fb_width() / 2;
		right = player_get_fb_width() - 1;
	}

	gDPFillRectangle(gdl++, left, 0, right, player_get_fb_height() - 1);
	gDPPipeSync(gdl++);

	return gdl;
}

u16 *zbuf_get_artifacts_depth_samples(s32 index)
{
	u16 *addr;

	if (index == 0) {
		addr = g_ArtifactDepths0;
	}

	if (index == 1) {
		addr = g_ArtifactDepths1;
	}

	if (index == 2) {
		addr = g_ArtifactDepths2;
	}

	addr = (u16 *) (((uintptr_t) addr + 0x3f) & ~0x3f);

	return addr;
}

/**
 * Append GDL commands which make the GPU read from the z-buffer as a texture
 * and write it to the scheduler's write artifacts list. Only the individual
 * pixels of interest are copied.
 */
Gfx *zbuf_save_artifact_depths(Gfx *gdl)
{
	struct artifact *artifacts = sched_get_write_artifacts();
	u32 stack;
	u16 *zbuf = g_ZbufPtr1;
	u32 numsamples = 0;
	u16 *samples;
	u16 *thissample;
	u16 *zbufrow;
	s32 i;

	vi_get_back_buffer();
	samples = zbuf_get_artifacts_depth_samples(g_SchedWriteArtifactsIndex);

	g_SchedArtifactsWithDualBuffers[g_SchedWriteArtifactsIndex] = true;

	gDPPipeSync(gdl++);
	gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, vi_get_buf_width(), OS_PHYSICAL_TO_K0(samples));
	gDPSetScissor(gdl++, G_SC_NON_INTERLACE, 0, 0, SCREEN_320, SCREEN_240);
	gDPSetCycleType(gdl++, G_CYC_COPY);
	gDPSetTile(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0x0000, 5, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
	gDPSetTile(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 0, 0x0080, 4, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
	gDPSetTile(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, 160, 0x0000, G_TX_RENDERTILE, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD);
	gDPSetTile(gdl++, G_IM_FMT_I, G_IM_SIZ_8b, 160, 0x0080, 1, 0, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOLOD, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, 15);
	gSPTexture(gdl++, 0xffff, 0xffff, 0, G_TX_RENDERTILE, G_ON);
	gDPSetEnvColor(gdl++, 0xff, 0xff, 0xff, 0xff);
	gDPSetPrimColor(gdl++, 0, 0, 0xff, 0xff, 0xff, 0xff);
	gDPSetRenderMode(gdl++, G_RM_NOOP, G_RM_NOOP2);
	gDPSetCombineMode(gdl++, G_CC_DECALRGBA, G_CC_DECALRGBA);
	gDPSetTextureFilter(gdl++, G_TF_POINT);
	gDPSetTexturePersp(gdl++, G_TP_NONE);
	gDPSetColorDither(gdl++, G_CD_DISABLE);
	gDPSetAlphaDither(gdl++, G_AD_DISABLE);
	gDPSetTextureLOD(gdl++, G_TL_TILE);
	gDPSetTextureDetail(gdl++, G_TD_CLAMP);
	gDPSetTextureLUT(gdl++, G_TT_NONE);
	gDPSetAlphaCompare(gdl++, G_AC_NONE);
	gSPClearGeometryMode(gdl++, G_ZBUFFER);
	gDPTileSync(gdl++);

	for (i = 0; i < MAX_ARTIFACTS; i++) {
		if (1);

		if (artifacts[i].type != ARTIFACTTYPE_FREE) {
			thissample = &samples[numsamples];
			zbufrow = &zbuf[artifacts[i].screeny * vi_get_width()];

			gDPPipeSync(gdl++);
			gDPSetTextureImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, SCREEN_320, zbufrow);
			gDPLoadSync(gdl++);
			gDPLoadBlock(gdl++, 5, 0, 0, vi_get_width() - 1, 0);
			gDPPipeSync(gdl++);

			gSPTextureRectangle(gdl++,
					numsamples << 2, 0,
					(numsamples + 3) << 2, 0,
					G_TX_RENDERTILE, (artifacts[i].screenx * 32) + 16, 0x0010, 0x1000, 0);

			artifacts[i].depthptr = thissample;
			numsamples++;

			if (thissample);
		}
	}

	gDPPipeSync(gdl++);
	gDPLoadSync(gdl++);
	gDPTileSync(gdl++);
	gDPSetColorImage(gdl++, G_IM_FMT_RGBA, G_IM_SIZ_16b, vi_get_buf_width(), OS_PHYSICAL_TO_K0(vi_get_back_buffer()));
	gDPSetScissorFrac(gdl++, G_SC_NON_INTERLACE, 0, 0, vi_get_width() * 4.0f, vi_get_height() * 4.0f);
	gSPSetGeometryMode(gdl++, G_ZBUFFER);
	gDPSetTextureFilter(gdl++, G_TF_BILERP);
	gDPSetTexturePersp(gdl++, G_TP_PERSP);
	gDPSetColorDither(gdl++, G_CD_BAYER);

	if (samples);

	return gdl;
}
