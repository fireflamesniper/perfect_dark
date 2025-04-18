#include <ultra64.h>
#include "constants.h"
#include "game/chraction.h"
#include "game/ceil.h"
#include "game/bondgun.h"
#include "game/gset.h"
#include "game/tex.h"
#include "game/menugfx.h"
#include "game/menu.h"
#include "game/mainmenu.h"
#include "game/inv.h"
#include "game/text.h"
#include "game/file.h"
#include "game/texdecompress.h"
#include "game/tex.h"
#include "game/modeldef.h"
#include "game/lang.h"
#include "game/mplayer/mplayer.h"
#include "game/options.h"
#include "bss.h"
#include "lib/vi.h"
#include "lib/main.h"
#include "lib/model.h"
#include "data.h"
#include "types.h"

struct skeleton *g_Skeletons[] = {
	&g_SkelChr,
	&g_SkelClassicGun,
	&g_Skel06,
	&g_SkelUzi,
	&g_SkelBasic,
	&g_SkelCctv,
	&g_SkelWindowedDoor,
	&g_Skel11,
	&g_Skel12,
	&g_Skel13,
	&g_SkelTerminal,
	&g_SkelCiHub,
	&g_SkelAutogun,
	&g_Skel17,
	&g_Skel18,
	&g_Skel19,
	&g_Skel0A,
	&g_Skel0B,
	&g_SkelCasing,
	&g_SkelChrGun,
	&g_Skel0C,
	&g_SkelJoypad,
	&g_SkelLift,
	&g_SkelSkedar,
	&g_SkelLogo,
	&g_SkelPdLogo,
	&g_SkelHoverbike,
	&g_SkelJumpship,
	&g_Skel20,
	&g_Skel21,
	&g_Skel22,
	&g_SkelLaptopGun,
	&g_SkelK7Avenger,
	&g_SkelChopper,
	&g_SkelFalcon2,
	&g_SkelKnife,
	&g_SkelDrCaroll,
	&g_SkelRope,
	&g_SkelCmp150,
	&g_SkelBanner,
	&g_SkelDragon,
	&g_SkelSuperDragon,
	&g_SkelRocket,
	&g_Skel4A,
	&g_SkelShotgun,
	&g_SkelFarsight,
	&g_Skel4D,
	&g_SkelReaper,
	&g_SkelDropship,
	&g_SkelMauler,
	&g_SkelDevastator,
	&g_SkelRobot,
	&g_SkelPistol,
	&g_SkelAr34,
	&g_SkelMagnum,
	&g_SkelSlayerRocket,
	&g_SkelCyclone,
	&g_SkelSniperRifle,
	&g_SkelTranquilizer,
	&g_SkelCrossbow,
	&g_SkelHudPiece,
	&g_SkelTimedProxyMine,
	&g_SkelPhoenix,
	&g_SkelCallisto,
	&g_SkelHand,
	&g_SkelRcp120,
	&g_SkelSkShuttle,
	&g_SkelLaser,
	&g_SkelMaianUfo,
	&g_SkelGrenade,
	&g_SkelCableCar,
	&g_SkelSubmarine,
	&g_SkelTarget,
	&g_SkelEcmMine,
	&g_SkelUplink,
	&g_SkelRareLogo,
	&g_SkelWireFence,
	&g_SkelRemoteMine,
	&g_SkelBB,
};

void modeldef0f1a7560(struct modeldef *modeldef, u16 filenum, u32 arg2, struct modeldef *modeldef2, struct texpool *texpool, bool arg5)
{
	s32 allocsize;
	s32 loadedsize;
	s32 sp84;
	u32 s0;
	u32 s4;
	uintptr_t s5;
	struct modelnode *node;
	struct modelnode *prevnode;
	uintptr_t gdl;
	Vtx *vertices;

	allocsize = file_get_allocation_size(filenum);
	loadedsize = file_get_loaded_size(filenum);
	node = NULL;

	model_iterate_display_lists(modeldef, &node, (Gfx **)&gdl);

	s5 = gdl;

	if (gdl) {
		s32 v1 = allocsize - (loadedsize - (s32)(((uintptr_t)modeldef + (gdl & 0xffffff)) - (uintptr_t)modeldef));
		sp84 = (s32)v1 + (s32)((uintptr_t)modeldef - ((uintptr_t)modeldef + (gdl & 0xffffff)));

		tex_copy_gdls((Gfx *)((uintptr_t)modeldef + (gdl & 0xffffff)),
				(Gfx *)(v1 + (uintptr_t)modeldef),
				loadedsize - (s32)(((uintptr_t)modeldef + (gdl & 0xffffff)) - (uintptr_t)modeldef));
		tex_load_from_configs(modeldef->texconfigs, modeldef->numtexconfigs, texpool, (uintptr_t)modeldef2 - arg2);

		while (node) {
			prevnode = node;
			s0 = gdl;

			model_iterate_display_lists(modeldef, &node, (Gfx **) &gdl);

			if (gdl) {
				s4 = gdl - s0;
			} else {
				s4 = loadedsize + (uintptr_t)modeldef - (uintptr_t)modeldef - (s0 & 0xffffff);
			}

			model_node_replace_gdl(modeldef, prevnode, (Gfx *) s0, (Gfx *) s5);

			if (prevnode->type == MODELNODETYPE_DL) {
				struct modelrodata_dl *rodata = &prevnode->rodata->dl;
				vertices = rodata->vertices;
			} else {
				vertices = NULL;
			}

			s5 += tex_load_from_gdl((Gfx *)((uintptr_t)modeldef + (s0 & 0xffffff) + sp84), s4, (Gfx *)((uintptr_t)modeldef + (s5 & 0xffffff)), texpool, (u8 *) vertices);
		}

		file_set_size(filenum, modeldef, (((uintptr_t)modeldef + (s5 & 0xffffff)) - (uintptr_t)modeldef + 0xf) & ~0xf, arg5);
	}
}

void model_promote_type_to_pointer(struct modeldef *modeldef)
{
	s32 i;

	if ((u32)modeldef->skel < 0x10000) {
		for (i = 0; g_Skeletons[i] != NULL; i++) {
			if ((s16)modeldef->skel == g_Skeletons[i]->skel) {
				modeldef->skel = g_Skeletons[i];
				return;
			}
		}
	}
}

struct modeldef *modeldef_load(u16 fileid, u8 *dst, s32 size, struct texpool *arg3)
{
	struct modeldef *modeldef;

	g_LoadType = LOADTYPE_MODEL;

	if (dst) {
		modeldef = file_load_to_addr(fileid, FILELOADMETHOD_EXTRAMEM, dst, size);
	} else {
		modeldef = file_load_to_new(fileid, FILELOADMETHOD_EXTRAMEM);
	}

	model_promote_type_to_pointer(modeldef);
	model_promote_offsets_to_pointers(modeldef, 0x5000000, (uintptr_t) modeldef);
	modeldef0f1a7560(modeldef, fileid, 0x5000000, modeldef, arg3, dst == NULL);

	return modeldef;
}

struct modeldef *modeldef_load_to_new(u16 fileid)
{
	return modeldef_load(fileid, NULL, 0, NULL);
}

struct modeldef *modeldef_load_to_addr(u16 fileid, u8 *dst, s32 size)
{
	return modeldef_load(fileid, dst, size, NULL);
}
