#include <ultra64.h>
#include "constants.h"
#include "game/game_006900.h"
#include "game/savebuffer.h"
#include "game/text.h"
#include "game/file.h"
#include "game/lang.h"
#include "bss.h"
#include "lib/vi.h"
#include "lib/dma.h"
#include "lib/main.h"
#include "data.h"
#include "types.h"

f32 func0f1577f0(f32 arg0[2], f32 arg1[2], f32 arg2[2], f32 arg3[2])
{
	f32 mult1 = arg2[1] - arg3[1];
	f32 mult2 = arg3[0] - arg2[0];
	f32 a = (arg2[1] - arg0[1]) * mult2 + (arg2[0] - arg0[0]) * mult1;
	f32 b = (arg1[1] - arg0[1]) * mult2 + (arg1[0] - arg0[0]) * mult1;

	if (b == 0.0f) {
		return 1.0f;
	}

	a /= b;

	if (a < 0.0f || a > 1.0f) {
		return 1.0f;
	}

	return a;
}

f32 func0f1578c8(struct radiusxz *arg0, struct xz *arg1, struct xz *arg2)
{
	f32 value2;
	f32 value1;
	f32 sp24;
	f32 mult1;
	f32 mult2;

	mult1 = arg2->x - arg0->x;
	mult2 = arg2->z - arg0->z;

	value1 = mult2 * arg1->x - mult1 * arg1->z;
	value2 = mult1 * arg1->x + mult2 * arg1->z;

	sp24 = (arg0->radius - value1) * (arg0->radius + value1);

	if (sp24 < 0.0f) {
		return MAXFLOAT;
	}

	value2 -= sqrtf(sp24);

	if (value2 < 0.0f) {
		if (value2 * value2 + value1 * value1 <= arg0->radius * arg0->radius) {
			return 0.0f;
		}

		return MAXFLOAT;
	}

	return value2;
}

f32 func0f1579cc(struct radiusxz *rxz, struct xz *edge_vtx1, struct xz *edge_vtx2, struct xz *diff)
{
	f32 spac;
	f32 spa8;
	struct xz spa0;
	f32 sp9c;
	f32 sp98;
	f32 sp94;
	f32 sp90;
	f32 sp8c;
	f32 sp88;
	f32 sp84;
	f32 sp80;
	f32 sp7c;
	f32 sp78;
	f32 sp74;
	f32 sp70;
	f32 sp6c;
	f32 sp68;
	f32 sp64;
	f32 sp60;
	f32 sp5c;
	f32 sp58;
	f32 sp54;

	spac = sqrtf(diff->x * diff->x + diff->z * diff->z);

	if (spac == 0.0f) {
		return 1.0f;
	}

	spa0.x = diff->x * (1.0f / spac);
	spa0.z = diff->z * (1.0f / spac);

	sp98 = edge_vtx2->x - edge_vtx1->x;
	sp9c = edge_vtx2->z - edge_vtx1->z;

	sp94 = sqrtf(sp98 * sp98 + sp9c * sp9c);

	if (sp94 == 0.0f) {
		goto handlezero;
	}

	sp90 = 1.0f / sp94;
	sp88 = sp9c * sp90;
	sp8c = -sp98 * sp90;

	sp84 = rxz->radius * sp88;
	sp80 = rxz->radius * sp8c;

	if (sp84 * (rxz->x - edge_vtx1->x) + sp80 * (rxz->z - edge_vtx1->z) < 0.0f) {
		sp84 = -sp84;
		sp80 = -sp80;
	}

	sp78 = edge_vtx1->x + sp84;
	sp7c = edge_vtx1->z + sp80;
	sp70 = edge_vtx2->x + sp84;
	sp74 = edge_vtx2->z + sp80;

	sp68 = (diff->z * sp78) - (sp7c * diff->x);
	sp6c = (rxz->x * diff->z) - (rxz->z * diff->x);
	sp64 = (diff->z * sp70) - (sp74 * diff->x);

	if (sp64 < sp68) {
		struct xz *tmp;

		spa8 = sp68;
		sp68 = sp64;
		sp64 = spa8;

		tmp = edge_vtx1;
		edge_vtx1 = edge_vtx2;
		edge_vtx2 = tmp;

		sp88 = -sp88;
		sp8c = -sp8c;
	}

	if (sp64 == sp68) {
		sp60 = func0f1578c8(rxz, &spa0, edge_vtx1);
		sp5c = func0f1578c8(rxz, &spa0, edge_vtx2);

		if (sp5c < sp60) {
			sp60 = sp5c;
		}
	} else if (sp64 < sp6c) {
handlezero:
		sp60 = func0f1578c8(rxz, &spa0, edge_vtx2);
	} else if (sp6c < sp68) {
		sp60 = func0f1578c8(rxz, &spa0, edge_vtx1);
	} else {
		sp58 = sp88 * (rxz->x - edge_vtx1->x) + sp8c * (rxz->z - edge_vtx1->z);
		sp54 = sp88 * (rxz->x + diff->x - edge_vtx1->x) + sp8c * (rxz->z + diff->z - edge_vtx1->z);

		if (sp58 == sp54) {
			return 1.0f;
		}

		sp60 = (sp58 - rxz->radius) * spac / (sp58 - sp54);
	}

	if (spac < sp60) {
		return 1.0f;
	}

	if (sp60 < 0.0f) {
		return 0.0f;
	}

	return (f32) sp60 * (1.0f / spac);
}
