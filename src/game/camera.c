#include <ultra64.h>
#include "constants.h"
#include "game/atan2f.h"
#include "game/camera.h"
#include "game/tex.h"
#include "game/playermgr.h"
#include "game/bg.h"
#include "game/texdecompress.h"
#include "bss.h"
#include "lib/mtx.h"
#include "data.h"
#include "types.h"

struct coord var8009dd20;
f32 var8009dd2c;
struct coord var8009dd30;
f32 var8009dd3c;
struct coord var8009dd40;
f32 var8009dd4c;
struct coord var8009dd50;
f32 var8009dd5c;
struct coord var8009dd60;
f32 var8009dd6c;

void cam0f0b4950(void)
{
	// empty
}

void cam_set_screen_size(f32 width, f32 height)
{
	struct player *player = g_Vars.currentplayer;

	player->c_screenwidth = width;
	player->c_screenheight = height;
	player->c_halfwidth = width * 0.5f;
	player->c_halfheight = height * 0.5f;
}

void cam_set_screen_position(f32 left, f32 top)
{
	struct player *player = g_Vars.currentplayer;

	player->c_screenleft = left;
	player->c_screentop = top;
}

void cam_set_perspective(f32 near, f32 fovy, f32 aspect)
{
	struct player *player = g_Vars.currentplayer;

	player->c_perspnear = near;
	player->c_perspfovy = fovy;
	player->c_perspaspect = aspect;
}

f32 cam0f0b49b8(f32 arg0)
{
	f32 result = atan2f(g_Vars.currentplayer->c_scalelod60 * arg0 * g_Vars.currentplayer->c_halfheight, 1.0f);
	result *= 114.591552f;

	if (result < 0) {
		result = -result;
	}

	return result;
}

void cam_set_scale(void)
{
	struct player *player = g_Vars.currentplayer;
	f32 fVar4;
	f32 tmp;
	f32 fVar5;
	f32 fVar2;

	player->c_scaley = sinf(player->c_perspfovy * (DTOR(180) / 360.0f)) / (cosf(player->c_perspfovy * (DTOR(180) / 360.0f)) * player->c_halfheight);
	player->c_scalelod = player->c_scaley;
	player->c_scalex = (player->c_scaley * player->c_perspaspect * player->c_halfheight) / player->c_halfwidth;

	player->c_recipscalex = 1.0f / player->c_scalex;
	player->c_recipscaley = 1.0f / player->c_scaley;

	fVar4 = sinf(DTOR(30)) / (cosf(DTOR(30)) * 120.0f);
	player->c_scalelod60 = fVar4;
	player->c_lodscalez = player->c_scalelod / fVar4;
	tmp = player->c_lodscalez * 65536.0f;

	if (tmp > 4294967296.0f) {
		player->c_lodscalezu32 = 0xffffffff;
	} else {
		player->c_lodscalezu32 = tmp;
	}

	fVar2 = player->c_halfheight * player->c_scaley;
	fVar4 = 1.0f / sqrtf(fVar2 * fVar2 + 1.0f);
	player->c_cameratopnorm.x = 0;
	player->c_cameratopnorm.y = fVar4;
	player->c_cameratopnorm.z = fVar2 * fVar4;

	fVar5 = -player->c_halfwidth * player->c_scalex;
	fVar4 = 1.0f / sqrtf(fVar5 * fVar5 + 1.0f);
	player->c_cameraleftnorm.x = -fVar4;
	player->c_cameraleftnorm.y = 0;
	player->c_cameraleftnorm.z = -fVar5 * fVar4;
}

void cam0f0b4c3c(f32 pos2d[2], struct coord *dir2d, f32 arg2)
{
	struct player *player = g_Vars.currentplayer;
	f32 sp20;
	f32 sp1c;
	f32 sp18 = -1.0f;
	f32 f2;

	sp1c = (player->c_halfheight - (pos2d[1] - player->c_screentop)) * player->c_scaley;
	sp20 = (pos2d[0] - player->c_screenleft - player->c_halfwidth) * player->c_scalex;

	f2 = arg2 / sqrtf(sp20 * sp20 + sp1c * sp1c + sp18 * sp18);

	dir2d->x = sp20 * f2;
	dir2d->y = sp1c * f2;
	dir2d->z = sp18 * f2;
}

void cam0f0b4d04(struct coord *in, f32 *out)
{
	struct player *player = g_Vars.currentplayer;
	f32 value = 1.0f / in->z;

	out[1] = in->y * value * player->c_recipscaley + (player->c_screentop + player->c_halfheight);
	out[0] = (player->c_screenleft + player->c_halfwidth) - in->x * value * player->c_recipscalex;
}

void cam0f0b4d68(struct coord *in, f32 out[2])
{
	struct player *player = g_Vars.currentplayer;
	f32 value;

	if (in->z == 0.0f) {
		value = -100000000000000000000.0f;
	} else {
		value = 1.0f / in->z;
	}

	out[1] = in->y * value * player->c_recipscaley + (player->c_screentop + player->c_halfheight);
	out[0] = (player->c_screenleft + player->c_halfwidth) - in->x * value * player->c_recipscalex;
}

void cam0f0b4dec(struct coord *in, f32 out[2])
{
	struct player *player = g_Vars.currentplayer;
	f32 value = 1.0f / in->z;

	if (value < 0) {
		value = -value;
	}

	out[1] = in->y * value * player->c_recipscaley + (player->c_screentop + player->c_halfheight);
	out[0] = (player->c_screenleft + player->c_halfwidth) - in->x * value * player->c_recipscalex;
}

void cam0f0b4e68(f32 in[2], f32 divisor, f32 out[2])
{
	out[1] = in[1] * (1.0f / divisor) * g_Vars.currentplayer->c_recipscaley;
	out[0] = in[0] * (1.0f / divisor) * g_Vars.currentplayer->c_recipscalex;
}

void cam0f0b4eb8(struct coord *arg0, f32 arg1[2], f32 zoom, f32 aspect)
{
	f32 f12;
	f32 f14;
	struct player *player = g_Vars.currentplayer;

	f12 = cosf(zoom * 0.008726646f) * player->c_halfheight / (sinf(zoom * 0.008726646f) * arg0->f[2]);
	f14 = f12 * player->c_halfwidth / (aspect * player->c_halfheight);

	arg1[1] = f12 * arg0->f[1] + (player->c_screentop + player->c_halfheight);
	arg1[0] = player->c_screenleft + player->c_halfwidth - f14 * arg0->f[0];
}

void cam_set_mtxl1738(Mtx *mtx)
{
	g_Vars.currentplayer->mtxl1738 = mtx;
}

Mtx *cam_get_mtxl1738(void)
{
	return g_Vars.currentplayer->mtxl1738;
}

void cam_set_mtxl173c(Mtx *mtx)
{
	g_Vars.currentplayer->mtxl173c = mtx;
}

Mtx *cam_get_mtxl173c(void)
{
	return g_Vars.currentplayer->mtxl173c;
}

void cam_set_mtxf006c(Mtxf *mtx)
{
	g_Vars.currentplayer->mtxf006c = mtx;
}

Mtxf *cam_get_mtxf006c(void)
{
	return g_Vars.currentplayer->mtxf006c;
}

void cam_set_perspective_mtxl(Mtx *mtx)
{
	g_Vars.currentplayer->perspmtxl = mtx;
}

Mtx *cam_get_perspective_mtxl(void)
{
	return g_Vars.currentplayer->perspmtxl;
}

void cam_set_orthogonal_mtxl(Mtx *mtx)
{
	g_Vars.currentplayer->orthomtxl = mtx;
}

Mtx *cam_get_orthogonal_mtxl(void)
{
	return g_Vars.currentplayer->orthomtxl;
}

void cam_set_world_to_screen_mtxf(Mtxf *mtx)
{
	struct player *player = g_Vars.currentplayer;

	player->prevworldtoscreenmtx = player->worldtoscreenmtx;
	player->worldtoscreenmtx = mtx;
	player->c_viewfmdynticknum = g_GfxNumSwaps;
	player->unk0488 = player->unk0484;
	player->unk0484 = g_GfxMemPos;
}

Mtxf *cam0f0b5050(u8 *arg0)
{
	Mtxf *result = NULL;
	s32 i;

	if (arg0 >= g_VtxBuffers[g_GfxActiveBufferIndex] && arg0 < g_VtxBuffers[g_GfxActiveBufferIndex + 1]) {
		for (i = 0; i < PLAYERCOUNT(); i++) {
			if (g_Vars.currentplayerindex >= playermgr_get_order_of_player(i)) {
				if (g_GfxNumSwaps == g_Vars.players[i]->c_viewfmdynticknum) {
					if (arg0 >= g_Vars.players[i]->unk0484 && (u8 *)result < g_Vars.players[i]->unk0484) {
						result = g_Vars.players[i]->worldtoscreenmtx;
					}
				}
			}
		}
	} else {
		for (i = 0; i < PLAYERCOUNT(); i++) {
			if (g_Vars.currentplayerindex >= playermgr_get_order_of_player(i)) {
				if (g_GfxNumSwaps == g_Vars.players[i]->c_prevviewfmdynticknum + 1) {
					if (arg0 >= g_Vars.players[i]->unk0488 && (u8 *)result < g_Vars.players[i]->unk0488) {
						result = g_Vars.players[i]->prevworldtoscreenmtx;
					}
				}
			} else {
				if (g_GfxNumSwaps == g_Vars.players[i]->c_viewfmdynticknum + 1) {
					if (arg0 >= g_Vars.players[i]->unk0484 && (u8 *)result < g_Vars.players[i]->unk0484) {
						result = g_Vars.players[i]->worldtoscreenmtx;
					}
				}
			}
		}
	}

	return result;
}

Mtxf *cam0f0b53a4(u8 *arg0)
{
	Mtxf *result = NULL;
	s32 i;

	if (arg0 >= g_VtxBuffers[g_GfxActiveBufferIndex] && arg0 < g_VtxBuffers[g_GfxActiveBufferIndex + 1]) {
		for (i = 0; i < PLAYERCOUNT(); i++) {
			if (g_Vars.currentplayerindex >= playermgr_get_order_of_player(i)) {
				if (g_GfxNumSwaps == g_Vars.players[i]->c_viewfmdynticknum) {
					if (arg0 >= g_Vars.players[i]->unk0484 && (u8 *)result < g_Vars.players[i]->unk0484) {
						result = g_Vars.players[i]->projectionmtx;
					}
				}
			}
		}
	} else {
		for (i = 0; i < PLAYERCOUNT(); i++) {
			if (g_Vars.currentplayerindex >= playermgr_get_order_of_player(i)) {
				if (g_GfxNumSwaps == g_Vars.players[i]->c_prevviewfmdynticknum + 1) {
					if (arg0 >= g_Vars.players[i]->unk0488 && (u8 *)result < g_Vars.players[i]->unk0488) {
						result = g_Vars.players[i]->prevprojectionmtx;
					}
				}
			} else {
				if (g_GfxNumSwaps == g_Vars.players[i]->c_viewfmdynticknum + 1) {
					if (arg0 >= g_Vars.players[i]->unk0484 && (u8 *)result < g_Vars.players[i]->unk0484) {
						result = g_Vars.players[i]->projectionmtx;
					}
				}
			}
		}
	}

	return result;
}

Mtxf *cam_get_world_to_screen_mtxf(void)
{
	return g_Vars.currentplayer->worldtoscreenmtx;
}

void cam_set_mtxf1754(Mtxf *mtx)
{
	g_Vars.currentplayer->mtxf1754 = mtx;
}

Mtxf *cam_get_mtxf1754(void)
{
	return g_Vars.currentplayer->mtxf1754;
}

Mtxf *cam_get_prev_world_to_screen_mtxf(void)
{
	return g_Vars.currentplayer->prevworldtoscreenmtx;
}

void cam_set_mtxf1748(Mtxf *mtx)
{
	g_Vars.currentplayer->mtxf1748 = mtx;
}

Mtxf *cam_get_mtxf1748(void)
{
	return g_Vars.currentplayer->mtxf1748;
}

void cam_set_projection_mtxf(Mtxf *mtx)
{
	struct player *player = g_Vars.currentplayer;

	player->c_prevviewfmdynticknum = player->c_viewfmdynticknum;
	player->prevprojectionmtx = player->projectionmtx;
	player->projectionmtx = mtx;
}

Mtxf *cam_get_projection_mtxf(void)
{
	return g_Vars.currentplayer->projectionmtx;
}

Mtxf *cam_get_prev_projection_mtxf(void)
{
	return g_Vars.currentplayer->prevprojectionmtx;
}

void cam_set_look_at(LookAt *lookat)
{
	g_Vars.currentplayer->lookat = lookat;
}

LookAt *cam_get_look_at(void)
{
	return g_Vars.currentplayer->lookat;
}

f32 cam_get_lod_scale_z(void)
{
	return g_Vars.currentplayer->c_lodscalez;
}

u32 cam_get_lod_scale_z_u32(void)
{
	return g_Vars.currentplayer->c_lodscalezu32;
}

f32 cam_get_screen_width(void)
{
	return g_Vars.currentplayer->c_screenwidth;
}

f32 cam_get_screen_height(void)
{
	return g_Vars.currentplayer->c_screenheight;
}

f32 cam_get_screen_left(void)
{
	return g_Vars.currentplayer->c_screenleft;
}

f32 cam_get_screen_top(void)
{
	return g_Vars.currentplayer->c_screentop;
}

f32 cam_get_persp_fov_y(void)
{
	return g_Vars.currentplayer->c_perspfovy;
}

f32 cam_get_persp_aspect(void)
{
	return g_Vars.currentplayer->c_perspaspect;
}

void cam0f0b5838(void)
{
	f32 sp2c;
	f32 sp28;
	f32 sp24;
	f32 sp20;
	struct player *player;
	Mtxf *mtx;
	f32 sp14;
	f32 sp10;

	player = g_Vars.currentplayer;
	sp24 = player->c_halfheight * player->c_scaley;
	mtx = player->projectionmtx;

	sp2c = 1.0f / sqrtf(sp24 * sp24 + 1.0f);
	sp24 *= sp2c;
	sp20 = -sp2c;

	var8009dd20.f[0] = -sp20 * mtx->m[1][0] + (sp24) * mtx->m[2][0];
	var8009dd20.f[1] = -sp20 * mtx->m[1][1] + (sp24) * mtx->m[2][1];
	var8009dd20.f[2] = -sp20 * mtx->m[1][2] + (sp24) * mtx->m[2][2];

	var8009dd2c = var8009dd20.f[0] * mtx->m[3][0] + var8009dd20.f[1] * mtx->m[3][1] + var8009dd20.f[2] * mtx->m[3][2];

	var8009dd30.f[0] = sp20 * mtx->m[1][0] + (sp24) * mtx->m[2][0];
	var8009dd30.f[1] = sp20 * mtx->m[1][1] + (sp24) * mtx->m[2][1];
	var8009dd30.f[2] = sp20 * mtx->m[1][2] + (sp24) * mtx->m[2][2];

	var8009dd3c = var8009dd30.f[0] * mtx->m[3][0] + var8009dd30.f[1] * mtx->m[3][1] + var8009dd30.f[2] * mtx->m[3][2];

	sp28 = -player->c_halfwidth * player->c_scalex;

	sp10 = 1.0f / sqrtf(sp28 * sp28 + 1.0f);
	sp28 *= sp10;
	sp14 = -sp10;

	var8009dd40.f[0] = sp14 * mtx->m[0][0] - sp28 * mtx->m[2][0];
	var8009dd40.f[1] = sp14 * mtx->m[0][1] - sp28 * mtx->m[2][1];
	var8009dd40.f[2] = sp14 * mtx->m[0][2] - sp28 * mtx->m[2][2];

	var8009dd4c = var8009dd40.f[0] * mtx->m[3][0] + var8009dd40.f[1] * mtx->m[3][1] + var8009dd40.f[2] * mtx->m[3][2];

	var8009dd50.f[0] = -sp14 * mtx->m[0][0] - sp28 * mtx->m[2][0];
	var8009dd50.f[1] = -sp14 * mtx->m[0][1] - sp28 * mtx->m[2][1];
	var8009dd50.f[2] = -sp14 * mtx->m[0][2] - sp28 * mtx->m[2][2];

	var8009dd5c = var8009dd50.f[0] * mtx->m[3][0] + var8009dd50.f[1] * mtx->m[3][1] + var8009dd50.f[2] * mtx->m[3][2];

	var8009dd60.f[0] = -mtx->m[3][0];
	var8009dd60.f[1] = -mtx->m[3][1];
	var8009dd60.f[2] = -mtx->m[3][2];

	var8009dd6c = mtx->m[2][0] * mtx->m[3][0] + mtx->m[2][1] * mtx->m[3][1] + mtx->m[2][2] * mtx->m[3][2];
}

bool cam0f0b5b9c(struct coord *arg0, f32 arg1)
{
	Mtxf *mtx = g_Vars.currentplayer->projectionmtx;

	if (var8009dd6c + arg1 < mtx->m[2][0] * arg0->f[0] + mtx->m[2][1] * arg0->f[1] + mtx->m[2][2] * arg0->f[2]) {
		return false;
	}

	if (var8009dd4c + arg1 < var8009dd40.f[0] * arg0->f[0] + var8009dd40.f[1] * arg0->f[1] + var8009dd40.f[2] * arg0->f[2]) {
		return false;
	}

	if (var8009dd5c + arg1 < var8009dd50.f[0] * arg0->f[0] + var8009dd50.f[1] * arg0->f[1] + var8009dd50.f[2] * arg0->f[2]) {
		return false;
	}

	if (var8009dd2c + arg1 < var8009dd20.f[0] * arg0->f[0] + var8009dd20.f[1] * arg0->f[1] + var8009dd20.f[2] * arg0->f[2]) {
		return false;
	}

	if (var8009dd3c + arg1 < var8009dd30.f[0] * arg0->f[0] + var8009dd30.f[1] * arg0->f[1] + var8009dd30.f[2] * arg0->f[2]) {
		return false;
	}

	return true;
}

bool cam_is_pos_in_screen_box(struct coord *pos, f32 arg1, struct drawslot *drawslot)
{
	struct coord sp74;
	f32 sp70;
	struct coord sp64;
	f32 sp60;
	struct coord sp54;
	f32 sp50;
	struct coord sp44;
	f32 sp40;
	f32 sp3c;
	f32 sp38;
	f32 sp34;
	f32 sp30;
	f32 sp2c;
	f32 sp28;
	f32 sp24;
	f32 sp20;
	f32 sp1c;
	f32 sp18;

	if (var8009dd6c + arg1 < g_Vars.currentplayer->projectionmtx->m[2][0] * pos->f[0] + g_Vars.currentplayer->projectionmtx->m[2][1] * pos->f[1] + g_Vars.currentplayer->projectionmtx->m[2][2] * pos->f[2]) {
		return false;
	}

	sp38 = (drawslot->box.xmin - g_Vars.currentplayer->c_screenleft - g_Vars.currentplayer->c_halfwidth) * g_Vars.currentplayer->c_scalex;

	sp3c = 1.0f / sqrtf(sp38 * sp38 + 1.0f);
	sp38 *= sp3c;
	sp24 = -sp3c;

	sp54.f[0] = sp24 * g_Vars.currentplayer->projectionmtx->m[0][0] - sp38 * g_Vars.currentplayer->projectionmtx->m[2][0];
	sp54.f[1] = sp24 * g_Vars.currentplayer->projectionmtx->m[0][1] - sp38 * g_Vars.currentplayer->projectionmtx->m[2][1];
	sp54.f[2] = sp24 * g_Vars.currentplayer->projectionmtx->m[0][2] - sp38 * g_Vars.currentplayer->projectionmtx->m[2][2];

	sp50 = sp54.f[0] * g_Vars.currentplayer->projectionmtx->m[3][0] + sp54.f[1] * g_Vars.currentplayer->projectionmtx->m[3][1] + sp54.f[2] * g_Vars.currentplayer->projectionmtx->m[3][2];

	if (sp50 + arg1 < sp54.f[0] * pos->f[0] + sp54.f[1] * pos->f[1] + sp54.f[2] * pos->f[2]) {
		return false;
	}

	sp38 = -(drawslot->box.xmax - g_Vars.currentplayer->c_screenleft - g_Vars.currentplayer->c_halfwidth) * g_Vars.currentplayer->c_scalex;
	sp30 = 1.0f / sqrtf(sp38 * sp38 + 1.0f);
	sp38 *= sp30;
	sp20 = -sp30;

	sp44.f[0] = -sp20 * g_Vars.currentplayer->projectionmtx->m[0][0] - sp38 * g_Vars.currentplayer->projectionmtx->m[2][0];
	sp44.f[1] = -sp20 * g_Vars.currentplayer->projectionmtx->m[0][1] - sp38 * g_Vars.currentplayer->projectionmtx->m[2][1];
	sp44.f[2] = -sp20 * g_Vars.currentplayer->projectionmtx->m[0][2] - sp38 * g_Vars.currentplayer->projectionmtx->m[2][2];

	sp40 = sp44.f[0] * g_Vars.currentplayer->projectionmtx->m[3][0] + sp44.f[1] * g_Vars.currentplayer->projectionmtx->m[3][1] + sp44.f[2] * g_Vars.currentplayer->projectionmtx->m[3][2];

	if (sp40 + arg1 < sp44.f[0] * pos->f[0] + sp44.f[1] * pos->f[1] + sp44.f[2] * pos->f[2]) {
		return false;
	}

	sp34 = (g_Vars.currentplayer->c_halfheight - (drawslot->box.ymin - g_Vars.currentplayer->c_screentop)) * g_Vars.currentplayer->c_scaley;
	sp2c = 1.0f / sqrtf(sp34 * sp34 + 1.0f);
	sp34 *= sp2c;
	sp1c = -sp2c;

	sp74.f[0] = -sp1c * g_Vars.currentplayer->projectionmtx->m[1][0] + sp34 * g_Vars.currentplayer->projectionmtx->m[2][0];
	sp74.f[1] = -sp1c * g_Vars.currentplayer->projectionmtx->m[1][1] + sp34 * g_Vars.currentplayer->projectionmtx->m[2][1];
	sp74.f[2] = -sp1c * g_Vars.currentplayer->projectionmtx->m[1][2] + sp34 * g_Vars.currentplayer->projectionmtx->m[2][2];

	sp70 = sp74.f[0] * g_Vars.currentplayer->projectionmtx->m[3][0] + sp74.f[1] * g_Vars.currentplayer->projectionmtx->m[3][1] + sp74.f[2] * g_Vars.currentplayer->projectionmtx->m[3][2];

	if (sp70 + arg1 < sp74.f[0] * pos->f[0] + sp74.f[1] * pos->f[1] + sp74.f[2] * pos->f[2]) {
		return false;
	}

	sp34 = -(g_Vars.currentplayer->c_halfheight - (drawslot->box.ymax - g_Vars.currentplayer->c_screentop)) * g_Vars.currentplayer->c_scaley;
	sp28 = 1.0f / sqrtf(sp34 * sp34 + 1.0f);
	sp34 *= sp28;
	sp18 = -sp28;

	sp64.f[0] = sp18 * g_Vars.currentplayer->projectionmtx->m[1][0] + sp34 * g_Vars.currentplayer->projectionmtx->m[2][0];
	sp64.f[1] = sp18 * g_Vars.currentplayer->projectionmtx->m[1][1] + sp34 * g_Vars.currentplayer->projectionmtx->m[2][1];
	sp64.f[2] = sp18 * g_Vars.currentplayer->projectionmtx->m[1][2] + sp34 * g_Vars.currentplayer->projectionmtx->m[2][2];

	sp60 = sp64.f[0] * g_Vars.currentplayer->projectionmtx->m[3][0] + sp64.f[1] * g_Vars.currentplayer->projectionmtx->m[3][1] + sp64.f[2] * g_Vars.currentplayer->projectionmtx->m[3][2];

	if (sp60 + arg1 < sp64.f[0] * pos->f[0] + sp64.f[1] * pos->f[1] + sp64.f[2] * pos->f[2]) {
		return false;
	}

	return true;
}

bool cam_is_pos_in_fov_and_visible_room(RoomNum *rooms, struct coord *pos, f32 modelscale)
{
	bool hasdata = false;
	RoomNum *ptr;
	RoomNum room;
	struct drawslot *roomslot;
	struct drawslot tmpslot;

	for (ptr = rooms; (room = *ptr) != -1; ptr++) {
		if (g_Rooms[room].flags & ROOMFLAG_ONSCREEN) {
			roomslot = bg_get_room_draw_slot(room);

			if (hasdata == false) {
				tmpslot.box.xmin = roomslot->box.xmin;
				tmpslot.box.ymin = roomslot->box.ymin;
				tmpslot.box.xmax = roomslot->box.xmax;
				tmpslot.box.ymax = roomslot->box.ymax;
			} else {
				tmpslot.box.xmin = MIN(roomslot->box.xmin, tmpslot.box.xmin);
				tmpslot.box.ymin = MIN(roomslot->box.ymin, tmpslot.box.ymin);
				tmpslot.box.xmax = MAX(roomslot->box.xmax, tmpslot.box.xmax);
				tmpslot.box.ymax = MAX(roomslot->box.ymax, tmpslot.box.ymax);
			}

			hasdata = true;
		}
	}

	if (!hasdata) {
		return false;
	}

	return cam_is_pos_in_screen_box(pos, modelscale, &tmpslot);
}
