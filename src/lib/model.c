#include <ultra64.h>
#include "constants.h"
#include "game/game_096700.h"
#include "game/acosfasinf.h"
#include "game/quaternion.h"
#include "game/camera.h"
#include "game/floor.h"
#include "game/ceil.h"
#include "game/tex.h"
#include "game/gfxmemory.h"
#include "game/bg.h"
#include "game/file.h"
#include "bss.h"
#include "lib/rng.h"
#include "lib/mtx.h"
#include "lib/anim.h"
#include "lib/model.h"
#include "data.h"
#include "types.h"

/**
 * -- Model Definitions --
 *
 * A model definition is a representation of a model file.
 * The term is sometimes abbreviated to modeldef.
 *
 * A single model definition can be shared by multiple model instances.
 * For characters, their body is one model instance and their head is another
 * instance, so guards who share the same outfit can share the same body
 * modeldef, and guards who have the same head can share the same head modeldef.
 *
 * A modeldef contains a tree of nodes. A node can have any number of siblings,
 * as well as children and grandchildren and so on. Each node has a type, and
 * each node can contain data specific to that type.
 *
 * Some common node types are:
 * - Distance nodes, which can turn on/off their child nodes based on distance
 *   to the camera.
 * - Toggle nodes, which can turn on/off their child nodes by other places in
 *   the game (for example, hiding Cass's necklace from her model).
 * - Headspot nodes, which mark a point on a body model where a head can be
 *   attached.
 * - Displaylist nodes, which contain GBI bytecode for rendering.
 * - Bbox (bounding box) nodes, which are commonly used for shield hit detection.
 *
 * The data for these nodes is referred to as read-only data (rodata).
 * It's read-only because changing it would affect all model instances that use
 * this modeldef, which is typically undesired.
 *
 * Modeldefs can contain a parts list. The parts list is a mapping of part
 * numbers to certain nodes within the tree. They allow the game to quickly get
 * a pointer to a node by part number, without having to iterate through the
 * tree to find it. A part number can map to a node of any type, but the game
 * assumes certain numbers map to certain types. For example, it assumes that
 * the part number for Cass's necklace maps to a toggle node.
 *
 * -- Model Instances --
 *
 * Model instances are usually just referred to as "models" in decomp.
 *
 * Each model instance contains a pointer to its modeldef and can have an
 * animation applied to it, among other things.
 *
 * Model instances also have read-write data (rwdata). rwdata stores
 * instance-specific node data, such as position/rotation, toggle state and the
 * chosen head. Each instance has a pointer to its rwdata allocation, and each
 * node in the model definition contains a rwdataindex which is its index in any
 * instance's rwdata allocation. The node rwdata for an instance can be found by
 * adding the two together.
 *
 * When working with a model, its common to detach and reattach nodes in the
 * model definition's node tree. For example, a distance node may enable or
 * disable its children, toggle nodes may be enabled or disabled, and heads may
 * be replaced as needed. This is all done by clearing or populating a node's
 * child pointer in the model definition. This leaves the modeldef in an
 * inconsistent state, so any time a model instance is being worked on it needs
 * to re-apply its relations to the modeldef. Some of these relations can be
 * calculated on the fly (such as distance nodes) while others are saved in
 * rwdata (such as the selected head).
 */

#if VERSION >= VERSION_PAL_BETA
u8 var8005efb0_2 = 0;
#endif

u32 var8005efb0 = 0;

bool g_ModelDistanceDisabled = false;
f32 g_ModelDistanceScale = 1;
bool g_ChrsAnimDebugForceLoop = false;
f32 var8005efc0 = 0;
bool (*g_ModelShouldRenderGunDlCallback)(struct model *model, struct modelnode *node) = NULL;

#if VERSION >= VERSION_PAL_BETA
bool var8005efd8_2 = false;
#endif

Vtx *(*g_ModelVtxAllocatorFunc)(s32 numvertices) = NULL;
void (*g_ModelJointPositionedFunc)(s32 mtxindex, Mtxf *mtx) = NULL;

void model_set_distance_checks_disabled(bool disabled)
{
	g_ModelDistanceDisabled = disabled;
}

void model_set_distance_scale(f32 scale)
{
	g_ModelDistanceScale = scale;
}

void model_set_vtx_allocator_func(Vtx *(*fn)(s32 numvertices))
{
	g_ModelVtxAllocatorFunc = fn;
}

/**
 * Ascend the node's parents until a node type is found which supports an mtx.
 * Return the instance-local mtx index for that node.
 *
 * Position nodes support up to 3 matrices. In this case the desired one can be
 * specified with arg1.
 */
s32 model_find_node_mtx_index(struct modelnode *node, s32 arg1)
{
	s32 index;
	union modelrodata *rodata1;
	union modelrodata *rodata2;
	union modelrodata *rodata3;

	while (node) {
		switch (node->type & 0xff) {
		case MODELNODETYPE_CHRINFO:
			rodata1 = node->rodata;
			return rodata1->chrinfo.mtxindex;
		case MODELNODETYPE_POSITION:
			rodata2 = node->rodata;
			return rodata2->position.mtxindexes[arg1 == 0x200 ? 2 : (arg1 == 0x100 ? 1 : 0)];
		case MODELNODETYPE_POSITIONHELD:
			rodata3 = node->rodata;
			return rodata3->positionheld.mtxindex;
		}

		node = node->parent;
	}

	return -1;
}

Mtxf *model_find_node_mtx(struct model *model, struct modelnode *node, s32 arg2)
{
	s32 index = model_find_node_mtx_index(node, arg2);

	if (index >= 0) {
		return &model->matrices[index];
	}

	return NULL;
}

Mtxf *model_get_root_mtx(struct model *model)
{
	return model_find_node_mtx(model, model->definition->rootnode, 0);
}

struct modelnode *model_find_node_by_mtx_index(struct model *model, s32 mtxindex)
{
	struct modelnode *node = model->definition->rootnode;
	union modelrodata *rodata1;
	union modelrodata *rodata2;
	union modelrodata *rodata3;

	while (node) {
		switch (node->type & 0xff) {
		case MODELNODETYPE_CHRINFO:
			rodata1 = node->rodata;
			if (mtxindex == rodata1->chrinfo.mtxindex) {
				return node;
			}
			break;
		case MODELNODETYPE_POSITION:
			rodata2 = node->rodata;
			if (mtxindex == rodata2->position.mtxindexes[0]
					|| mtxindex == rodata2->position.mtxindexes[1]
					|| mtxindex == rodata2->position.mtxindexes[2]) {
				return node;
			}
			break;
		case MODELNODETYPE_POSITIONHELD:
			rodata3 = node->rodata;
			if (mtxindex == rodata3->positionheld.mtxindex) {
				return node;
			}
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node && node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}

	return NULL;
}

struct modelnode *model_node_find_mtx_node(struct modelnode *node)
{
	while (node) {
		u32 type = node->type & 0xff;

		if (type == MODELNODETYPE_CHRINFO
				|| type == MODELNODETYPE_POSITION
				|| type == MODELNODETYPE_POSITIONHELD) {
			break;
		}

		node = node->parent;
	}

	return node;
}

struct modelnode *model_node_find_parent_mtx_node(struct modelnode *node)
{
	while ((node = node->parent)) {
		u32 type = node->type & 0xff;

		if (type == MODELNODETYPE_CHRINFO
				|| type == MODELNODETYPE_POSITION
				|| type == MODELNODETYPE_POSITIONHELD) {
			break;
		}
	}

	return node;
}

struct modelnode *model_node_find_child_mtx_node(struct modelnode *basenode)
{
	struct modelnode *node = basenode->child;

	while (node) {
		u32 type = node->type & 0xff;

		if (type == MODELNODETYPE_CHRINFO
				|| type == MODELNODETYPE_POSITION
				|| type == MODELNODETYPE_POSITIONHELD) {
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node == basenode) {
					node = NULL;
					break;
				}

				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}

	return node;
}

struct modelnode *model_node_find_child_or_parent_mtx_node(struct modelnode *basenode)
{
	struct modelnode *node = basenode;
	struct modelnode *next;
	u32 type;

	while (node) {
		if (node != basenode && node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node != basenode) {
					type = node->type & 0xff;

					if (type == MODELNODETYPE_CHRINFO
							|| type == MODELNODETYPE_POSITION
							|| type == MODELNODETYPE_POSITIONHELD) {
						node = NULL;
						break;
					}
				}

				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}

			if (!node) {
				break;
			}
		}

		type = node->type & 0xff;

		if (type == MODELNODETYPE_CHRINFO
				|| type == MODELNODETYPE_POSITION
				|| type == MODELNODETYPE_POSITIONHELD) {
			break;
		}
	}

	return node;
}

struct modelnode *model_get_part(struct modeldef *modeldef, s32 partnum)
{
	s32 upper;
	s32 lower;
	u32 i;
	s16 *partnums;

	if (modeldef->numparts == 0) {
		return NULL;
	}

	partnums = (s16 *)&modeldef->parts[modeldef->numparts];
	lower = 0;
	upper = modeldef->numparts;

	while (upper >= lower) {
		i = (lower + upper) / 2;

		if (partnum == partnums[i]) {
			return modeldef->parts[i];
		}

		if (partnum < partnums[i]) {
			upper = i - 1;
		} else {
			lower = i + 1;
		}
	}

	return NULL;
}

void *model_get_part_rodata(struct modeldef *modeldef, s32 partnum)
{
	struct modelnode *node = model_get_part(modeldef, partnum);

	if (node) {
		return node->rodata;
	}

	return NULL;
}

f32 model_get_screen_distance(struct model *model)
{
	Mtxf *mtx = model_get_root_mtx(model);

	if (mtx) {
		return -mtx->m[3][2];
	}

	return 0;
}

#if VERSION >= VERSION_NTSC_1_0
// ntsc-beta has this function in another file
void *model_get_node_rw_data(struct model *model, struct modelnode *node)
{
	u32 index = 0;
	u32 *rwdatas = model->rwdatas;

	switch (node->type & 0xff) {
	case MODELNODETYPE_CHRINFO:
		index = node->rodata->chrinfo.rwdataindex;
		break;
	case MODELNODETYPE_DL:
		index = node->rodata->dl.rwdataindex;
		break;
	case MODELNODETYPE_DISTANCE:
		index = node->rodata->distance.rwdataindex;
		break;
	case MODELNODETYPE_TOGGLE:
		index = node->rodata->toggle.rwdataindex;
		break;
	case MODELNODETYPE_REORDER:
		index = node->rodata->reorder.rwdataindex;
		break;
	case MODELNODETYPE_0B:
		index = node->rodata->type0b.rwdataindex;
		break;
	case MODELNODETYPE_CHRGUNFIRE:
		index = node->rodata->chrgunfire.rwdataindex;
		break;
	case MODELNODETYPE_HEADSPOT:
		index = node->rodata->headspot.rwdataindex;
		break;
	}

	while (node->parent) {
		node = node->parent;

		if ((node->type & 0xff) == MODELNODETYPE_HEADSPOT) {
			struct modelrwdata_headspot *tmp = model_get_node_rw_data(model, node);
			rwdatas = tmp->rwdatas;
			break;
		}
	}

	return &rwdatas[index];
}
#endif

void model_node_get_position(struct model *model, struct modelnode *node, struct coord *pos)
{
	switch (node->type & 0xff) {
	case MODELNODETYPE_CHRINFO:
		{
			struct modelrwdata_chrinfo *rwdata = model_get_node_rw_data(model, node);
			pos->x = rwdata->pos.x;
			pos->y = rwdata->pos.y;
			pos->z = rwdata->pos.z;
		}
		break;
	case MODELNODETYPE_POSITION:
		{
			struct modelrodata_position *rodata = &node->rodata->position;
			pos->x = rodata->pos.x;
			pos->y = rodata->pos.y;
			pos->z = rodata->pos.z;
		}
		break;
	case MODELNODETYPE_POSITIONHELD:
		{
			struct modelrodata_positionheld *rodata = &node->rodata->positionheld;
			pos->x = rodata->pos.x;
			pos->y = rodata->pos.y;
			pos->z = rodata->pos.z;
		}
		break;
	default:
		pos->x = 0;
		pos->y = 0;
		pos->z = 0;
		break;
	}
}

void model_node_set_position(struct model *model, struct modelnode *node, struct coord *pos)
{
	switch (node->type & 0xff) {
	case MODELNODETYPE_CHRINFO:
		{
			struct modelrwdata_chrinfo *rwdata = model_get_node_rw_data(model, node);
			struct coord diff[1];

			diff[0].x = pos->x - rwdata->pos.x;
			diff[0].z = pos->z - rwdata->pos.z;

			rwdata->pos.x = pos->x;
			rwdata->pos.y = pos->y;
			rwdata->pos.z = pos->z;

			rwdata->unk24.x += diff[0].x; rwdata->unk24.z += diff[0].z;
			rwdata->unk34.x += diff[0].x; rwdata->unk34.z += diff[0].z;
			rwdata->unk40.x += diff[0].x; rwdata->unk40.z += diff[0].z;
			rwdata->unk4c.x += diff[0].x; rwdata->unk4c.z += diff[0].z;
		}
		break;
	case MODELNODETYPE_POSITION:
		{
			struct modelrodata_position *rodata = &node->rodata->position;
			rodata->pos.x = pos->x;
			rodata->pos.y = pos->y;
			rodata->pos.z = pos->z;
		}
		break;
	case MODELNODETYPE_POSITIONHELD:
		{
			struct modelrodata_positionheld *rodata = &node->rodata->positionheld;
			rodata->pos.x = pos->x;
			rodata->pos.y = pos->y;
			rodata->pos.z = pos->z;
		}
		break;
	}
}

void model_get_root_position(struct model *model, struct coord *pos)
{
	model_node_get_position(model, model->definition->rootnode, pos);
}

void model_set_root_position(struct model *model, struct coord *pos)
{
	model_node_set_position(model, model->definition->rootnode, pos);
}

void model_node_get_model_relative_position(struct model *model, struct modelnode *node, struct coord *pos)
{
	pos->x = 0;
	pos->y = 0;
	pos->z = 0;

	while (node) {
		struct coord nodepos;
		u32 type = node->type & 0xff;

		if (type == MODELNODETYPE_CHRINFO
				|| type == MODELNODETYPE_POSITION
				|| type == MODELNODETYPE_POSITIONHELD) {
			model_node_get_position(model, node, &nodepos);
			pos->x += nodepos.x;
			pos->y += nodepos.y;
			pos->z += nodepos.z;
		}

		node = node->parent;
	}
}

f32 model_get_chr_rot_y(struct model *model)
{
	if ((model->definition->rootnode->type & 0xff) == MODELNODETYPE_CHRINFO) {
		union modelrwdata *rwdata = model_get_node_rw_data(model, model->definition->rootnode);
		return rwdata->chrinfo.yrot;
	}

	return 0;
}

void model_set_chr_rot_y(struct model *model, f32 angle)
{
	if ((model->definition->rootnode->type & 0xff) == MODELNODETYPE_CHRINFO) {
		struct modelrwdata_chrinfo *rwdata = model_get_node_rw_data(model, model->definition->rootnode);
		f32 diff = angle - rwdata->yrot;

		if (diff < 0) {
			diff += BADDTOR(360);
		}

		rwdata->unk30 += diff;

		if (rwdata->unk30 >= BADDTOR(360)) {
			rwdata->unk30 -= BADDTOR(360);
		}

		rwdata->unk20 += diff;

		if (rwdata->unk20 >= BADDTOR(360)) {
			rwdata->unk20 -= BADDTOR(360);
		}

		rwdata->yrot = angle;
	}
}

void model_set_scale(struct model *model, f32 scale)
{
	model->scale = scale;
}

void model_set_anim_scale(struct model *model, f32 scale)
{
	if (model->anim) {
		model->anim->animscale = scale;
	}
}

f32 model_get_effective_scale(struct model *model)
{
	return model->definition->scale * model->scale;
}

void model_tween_pos(struct coord *curpos, struct coord *goalpos, f32 frac)
{
	curpos->x += (goalpos->x - curpos->x) * frac;
	curpos->y += (goalpos->y - curpos->y) * frac;
	curpos->z += (goalpos->z - curpos->z) * frac;
}

f32 model_tween_rot_axis(f32 curangle, f32 goalangle, f32 mult)
{
	f32 diff = goalangle - curangle;

	if (goalangle < curangle) {
		diff += BADDTOR(360);
	}

	if (diff < DTOR(180)) {
		curangle += diff * mult;

		if (curangle >= BADDTOR(360)) {
			curangle -= BADDTOR(360);
		}
	} else {
		curangle -= (BADDTOR(360) - diff) * mult;

		if (curangle < 0) {
			curangle += BADDTOR(360);
		}
	}

	return curangle;
}

void model_tween_rot(struct coord *currot, struct coord *goalrot, f32 mult)
{
	currot->x = model_tween_rot_axis(currot->x, goalrot->x, mult);
	currot->y = model_tween_rot_axis(currot->y, goalrot->y, mult);
	currot->z = model_tween_rot_axis(currot->z, goalrot->z, mult);
}

void model_update_chr_info(struct model *model, struct modelnode *node)
{
	union modelrwdata *rwdata;
	struct anim *anim = model->anim;
	struct coord sp34;
	struct coord sp28;
	f32 frac;

	if (!anim) {
		return;
	}

	rwdata = model_get_node_rw_data(model, node);

	if (rwdata->chrinfo.unk00) {
		return;
	}

	sp34.x = rwdata->chrinfo.unk34.x;
	sp34.y = rwdata->chrinfo.unk34.y;
	sp34.z = rwdata->chrinfo.unk34.z;

	rwdata->chrinfo.yrot = rwdata->chrinfo.unk30;

	if (g_Vars.in_cutscene && anim->speed > 0.0f) {
#if VERSION >= VERSION_PAL_BETA
		frac = floorf(anim->frac / anim->speed + 0.01f) * anim->speed;
#else
		frac = floorf(anim->frac / anim->speed) * anim->speed;
#endif
	} else {
		frac = anim->frac;
	}

	if (frac != 0.0f && rwdata->chrinfo.unk01) {
		model_tween_pos(&sp34, &rwdata->chrinfo.unk24, frac);

		rwdata->chrinfo.yrot = model_tween_rot_axis(rwdata->chrinfo.unk30, rwdata->chrinfo.unk20, frac);
	}

	if (anim->animnum2 || anim->fracmerge) {
		if (rwdata->chrinfo.unk02) {
			f32 y = rwdata->chrinfo.unk4c.y;

			if (anim->frac2 != 0.0f) {
				y += (rwdata->chrinfo.unk40.y - y) * anim->frac2;
			}

			sp34.y += (y - sp34.y) * anim->fracmerge;
		}
	}

	if (anim->unk70 == NULL) {
		rwdata->chrinfo.pos.x = sp34.x;
		rwdata->chrinfo.pos.y = rwdata->chrinfo.ground + sp34.f[1];
		rwdata->chrinfo.pos.z = sp34.z;
	} else {
		sp28.x = sp34.x;
		sp28.y = sp34.y;
		sp28.z = sp34.z;

		if (anim->unk70(model, &rwdata->chrinfo.pos, &sp28, &rwdata->chrinfo.ground)) {
			rwdata->chrinfo.pos.x = sp28.x;
			rwdata->chrinfo.pos.y = rwdata->chrinfo.ground + sp28.f[1];
			rwdata->chrinfo.pos.z = sp28.z;

			sp34.x = sp28.x - sp34.x;
			sp34.z = sp28.z - sp34.z;

			rwdata->chrinfo.unk34.x += sp34.x;
			rwdata->chrinfo.unk34.z += sp34.z;

			if (rwdata->chrinfo.unk01) {
				rwdata->chrinfo.unk24.x += sp34.x;
				rwdata->chrinfo.unk24.z += sp34.z;
			}

			if (rwdata->chrinfo.unk02) {
				rwdata->chrinfo.unk4c.x += sp34.x;
				rwdata->chrinfo.unk4c.z += sp34.z;

				rwdata->chrinfo.unk40.x += sp34.x;
				rwdata->chrinfo.unk40.z += sp34.z;
			}
		}
	}
}

void model_update_info(struct model *model)
{
	struct modelnode *node = model->definition->rootnode;

	if (node && (node->type & 0xff) == MODELNODETYPE_CHRINFO) {
		model_update_chr_info(model, node);
	}
}

void model_update_chr_node_mtx(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node)
{
	struct anim *anim = model->anim;
	union modelrodata *rodata = node->rodata;
	union modelrwdata *rwdata = model_get_node_rw_data(model, node);
	f32 scale = model->scale;
	struct coord *sp254 = &rwdata->chrinfo.pos;
	f32 sp250 = rwdata->chrinfo.yrot;
	Mtxf *rendermtx;
	u32 stack1;
	Mtxf *mtx = &model->matrices[rodata->chrinfo.mtxindex];
	s32 animpart = rodata->chrinfo.animpart;
	struct skeleton *skel = model->definition->skel;
	struct coord rot1;
	struct coord translate1;
	struct coord scale1;
	Mtxf sp1d8;
	Mtxf sp198;
	Mtxf sp158;
	f32 sp154;
	struct coord rot2;
	struct coord translate2;
	struct coord scale2;
	struct coord rot3;
	struct coord translate3;
	struct coord scale3;
	f32 spfc[4];
	f32 spec[4];
	f32 spdc[4];
	struct coord rot4;
	struct coord translate4;
	struct coord scale4;
	Mtxf sp78;
	Mtxf sp38;

	if (rodata->chrinfo.mtxindex);

	if (node->parent) {
		rendermtx = model_find_node_mtx(model, node->parent, 0);
	} else {
		rendermtx = renderdata->rendermtx;
	}

	anim_get_rot_translate_scale(animpart, anim->flip, skel, anim->animnum, anim->frameslot1, &rot1, &translate1, &scale1);

	if (g_Vars.in_cutscene && anim->speed > 0) {
#if VERSION >= VERSION_PAL_BETA
		sp154 = floorf(anim->frac / anim->speed + 0.01f) * anim->speed;
#else
		sp154 = floorf(anim->frac / anim->speed) * anim->speed;
#endif
	} else {
		sp154 = anim->frac;
	}

	if (sp154 != 0.0f) {
		anim_get_rot_translate_scale(animpart, anim->flip, skel, anim->animnum, anim->frameslot2, &rot2, &translate2, &scale2);
		model_tween_rot(&rot1, &rot2, sp154);
	}

	if (anim->fracmerge != 0.0f) {
		anim_get_rot_translate_scale(animpart, anim->flip2, skel, anim->animnum2, anim->frameslot3, &rot3, &translate3, &scale3);

		if (anim->frac2 != 0.0f) {
			anim_get_rot_translate_scale(animpart, anim->flip2, skel, anim->animnum2, anim->frameslot4, &rot4, &translate4, &scale4);
			model_tween_rot(&rot3, &rot4, anim->frac2);
		}

		if ((g_Anims[anim->animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION) && (g_Anims[anim->animnum2].flags & ANIMFLAG_ABSOLUTETRANSLATION) == 0) {
			mtx4_load_y_rotation(rwdata->chrinfo.yrot, &sp78);
			mtx4_load_rotation(&rot3, &sp38);
			mtx00015be0(&sp78, &sp38);
			quaternion0f097044(&sp38, spec);
		} else {
			quaternion0f096ca0(&rot3, spec);
		}

		quaternion0f096ca0(&rot1, spfc);
		quaternion0f0976c0(spfc, spec);
		quaternion_slerp(spfc, spec, anim->fracmerge, spdc);
		quaternion_to_mtx(spdc, &sp1d8);
	} else {
		mtx4_load_rotation(&rot1, &sp1d8);
	}

	if (g_Anims[anim->animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION) {
		mtx4_load_translation(sp254, &sp198);
	} else {
		if (rwdata->chrinfo.unk18 != 0.0f) {
			sp250 = model_tween_rot_axis(sp250, rwdata->chrinfo.unk1c, rwdata->chrinfo.unk18);
		}

		mtx4_load_y_rotation_with_translation(sp254, sp250, &sp198);
	}

	mtx00015be4(&sp198, &sp1d8, &sp158);

	if (scale != 1.0f) {
		mtx00015f4c(scale, &sp158);
	}

	if (rendermtx) {
		mtx00015be4(rendermtx, &sp158, mtx);
	} else {
		mtx4_copy(&sp158, mtx);
	}
}

void model_position_joint_using_vec_rot(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node, struct coord *rot, struct coord *pos, bool allowscale, struct coord *arg6)
{
	s32 nodetype = node->type;
	struct modelrodata_position *rodata = &node->rodata->position;
	Mtxf *rendermtx;
	u32 stack;
	Mtxf mtx68;
	s32 mtxindex0 = rodata->mtxindex0;
	s32 mtxindex1 = rodata->mtxindex1;
	s32 mtxindex2 = rodata->mtxindex2;
	Mtxf *matrices = model->matrices;

	if (node->parent != NULL) {
		rendermtx = model_find_node_mtx(model, node->parent, 0);
	} else {
		rendermtx = renderdata->rendermtx;
	}

	if (rendermtx != NULL) {
		Mtxf *nodemtx = &matrices[mtxindex0];

		mtx4_load_rotation_and_translation(pos, rot, &mtx68);

		if (allowscale && model->scale != 1.0f) {
			mtx00015f04(model->scale, &mtx68);
		}

		if (arg6->x != 1.0f) {
			mtx00015df0(arg6->x, &mtx68);
		}

		if (arg6->y != 1.0f) {
			mtx00015e4c(arg6->y, &mtx68);
		}

		if (arg6->z != 1.0f) {
			mtx00015ea8(arg6->z, &mtx68);
		}

		mtx00015be4(rendermtx, &mtx68, nodemtx);

		if (g_ModelJointPositionedFunc != NULL) {
			g_ModelJointPositionedFunc(mtxindex0, nodemtx);
		}
	} else {
		Mtxf *nodemtx = &matrices[mtxindex0];

		mtx4_load_rotation_and_translation(pos, rot, nodemtx);

		if (allowscale && model->scale != 1.0f) {
			mtx00015f04(model->scale, nodemtx);
		}

		if (arg6->x != 1.0f) {
			mtx00015df0(arg6->x, nodemtx);
		}

		if (arg6->y != 1.0f) {
			mtx00015e4c(arg6->y, nodemtx);
		}

		if (arg6->z != 1.0f) {
			mtx00015ea8(arg6->z, nodemtx);
		}
	}

	if (nodetype & MODELNODETYPE_0100) {
		Mtxf *nodemtx = &matrices[mtxindex1];
		f32 sp3c[4];
		f32 sp2c[4];

		quaternion0f096ca0(rot, sp3c);
		quaternion0f097518(sp3c, 0.5f, sp2c);

		if (rendermtx != NULL) {
			quaternion_to_transform_mtx(pos, sp2c, &mtx68);
			mtx00015be4(rendermtx, &mtx68, nodemtx);
		} else {
			quaternion_to_transform_mtx(pos, sp2c, nodemtx);
		}
	}

	if (nodetype & MODELNODETYPE_0200) {
		Mtxf *finalmtx = rendermtx ? &mtx68 : &matrices[mtxindex2];
		f32 roty = rot->y;

		if (roty < DTOR(180)) {
			roty *= 0.5f;
		} else {
			roty = BADDTOR(360) - (BADDTOR(360) - roty) * 0.5f;
		}

		mtx4_load_y_rotation(roty, finalmtx);

		if (roty >= DTOR(180)) {
			roty = BADDTOR(360) - roty;
		}

		if (roty < DTOR(51)) {
			roty = func0f096700(roty);
		} else {
			roty = 1.5f;
		}

		mtx00015edc(roty, finalmtx);
		mtx4_set_translation(pos, finalmtx);

		if (rendermtx != NULL) {
			Mtxf *nodemtx = &matrices[mtxindex2];
			mtx00015be4(rendermtx, finalmtx, nodemtx);
		}
	}
}

void model_position_joint_using_quat_rot(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node, f32 rot[4], struct coord *pos, struct coord *arg5)
{
	s32 nodetype = node->type;
	struct modelrodata_position *rodata = &node->rodata->position;
	Mtxf *rendermtx;
	u32 stack;
	Mtxf mtx58;
	s32 mtxindex0 = rodata->mtxindex0;
	s32 mtxindex1 = rodata->mtxindex1;
	s32 mtxindex2 = rodata->mtxindex2;
	Mtxf *matrices = model->matrices;

	if (node->parent != NULL) {
		rendermtx = model_find_node_mtx(model, node->parent, 0);
	} else {
		rendermtx = renderdata->rendermtx;
	}

	if (rendermtx != NULL) {
		Mtxf *nodemtx = &matrices[mtxindex0];

		quaternion_to_transform_mtx(pos, rot, &mtx58);

		if (arg5->x != 1.0f) {
			mtx00015df0(arg5->x, &mtx58);
		}

		if (arg5->y != 1.0f) {
			mtx00015e4c(arg5->y, &mtx58);
		}

		if (arg5->z != 1.0f) {
			mtx00015ea8(arg5->z, &mtx58);
		}

		mtx00015be4(rendermtx, &mtx58, nodemtx);

		if (g_ModelJointPositionedFunc != NULL) {
			g_ModelJointPositionedFunc(mtxindex0, nodemtx);
		}
	} else {
		Mtxf *nodemtx = &matrices[mtxindex0];

		quaternion_to_transform_mtx(pos, rot, nodemtx);

		if (arg5->x != 1.0f) {
			mtx00015df0(arg5->x, nodemtx);
		}

		if (arg5->y != 1.0f) {
			mtx00015e4c(arg5->y, nodemtx);
		}

		if (arg5->z != 1.0f) {
			mtx00015ea8(arg5->z, nodemtx);
		}
	}

	if (nodetype & MODELNODETYPE_0100) {
		Mtxf *nodemtx = &matrices[mtxindex1];
		f32 sp2c[4];

		quaternion0f097518(rot, 0.5f, sp2c);

		if (rendermtx != NULL) {
			quaternion_to_transform_mtx(pos, sp2c, &mtx58);
			mtx00015be4(rendermtx, &mtx58, nodemtx);
		} else {
			quaternion_to_transform_mtx(pos, sp2c, nodemtx);
		}
	}

	if (nodetype & MODELNODETYPE_0200) {
		Mtxf *finalmtx = rendermtx ? &mtx58 : &matrices[mtxindex2];
		f32 roty = 2.0f * acosf(rot[0]);

		if (roty < DTOR(180)) {
			roty *= 0.5f;
		} else {
			roty = BADDTOR(360) - (BADDTOR(360) - roty) * 0.5f;
		}

		mtx4_load_y_rotation(roty, finalmtx);

		if (roty >= DTOR(180)) {
			roty = BADDTOR(360) - roty;
		}

		if (roty < DTOR(51)) {
			roty = func0f096700(roty);
		} else {
			roty = 1.5f;
		}

		mtx00015edc(roty, finalmtx);
		mtx4_set_translation(pos, finalmtx);

		if (rendermtx != NULL) {
			Mtxf *nodemtx = &matrices[mtxindex2];
			mtx00015be4(rendermtx, finalmtx, nodemtx);
		}
	}
}

void model_update_position_node_mtx(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node)
{
	struct anim *anim;
	struct modelrodata_position *rodata = &node->rodata->position;
	s32 animpart;
	struct skeleton *skel;
	struct coord rot1;
	struct coord translate1;
	struct coord scale1;
	bool sp128;
	Mtxf spe8;
	Mtxf *rendermtx;
	f32 spe0;
	struct coord rot2;
	struct coord translate2;
	struct coord scale2;
	struct coord rot3;
	struct coord translate3;
	struct coord scale3;
	f32 sp88[4];
	f32 sp78[4];
	f32 sp68[4];
	struct coord rot4;
	struct coord translate4;
	struct coord scale4;

	anim = model->anim;

	if (anim != NULL) {
		animpart = rodata->part;
		skel = model->definition->skel;

		if (anim->animnum != 0) {
			sp128 = (g_Anims[anim->animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION) && node == model->definition->rootnode;

			anim_get_rot_translate_scale(animpart, anim->flip, skel, anim->animnum, anim->frameslot1, &rot1, &translate1, &scale1);

			if (g_Vars.in_cutscene && anim->speed > 0.0f) {
#if VERSION >= VERSION_PAL_BETA
				spe0 = floorf(anim->frac / anim->speed + 0.0099999997764826f) * anim->speed;
#else
				spe0 = floorf(anim->frac / anim->speed) * anim->speed;
#endif
			} else {
				spe0 = anim->frac;
			}

			if (spe0 != 0.0f) {
				anim_get_rot_translate_scale(animpart, anim->flip, skel, anim->animnum, anim->frameslot2, &rot2, &translate2, &scale2);
				model_tween_rot(&rot1, &rot2, spe0);

#if VERSION >= VERSION_PAL_BETA
				if (sp128 || var8005efd8_2)
#else
				if (sp128)
#endif
				{
					model_tween_pos(&translate1, &translate2, spe0);
				}
			}
		} else {
			translate1.f[0] = translate1.f[1] = translate1.f[2] = 0.0f;
			rot1.f[0] = rot1.f[1] = rot1.f[2] = 0.0f;
			scale1.f[0] = scale1.f[1] = scale1.f[2] = 1.0f;

			sp128 = false;
		}

		if (anim->fracmerge != 0.0f) {
			anim_get_rot_translate_scale(animpart, anim->flip2, skel, anim->animnum2, anim->frameslot3, &rot3, &translate3, &scale3);

			if (anim->frac2 != 0.0f) {
				anim_get_rot_translate_scale(animpart, anim->flip2, skel, anim->animnum2, anim->frameslot4, &rot4, &translate4, &scale4);
				model_tween_rot(&rot3, &rot4, anim->frac2);
			}

			quaternion0f096ca0(&rot1, sp88);
			quaternion0f096ca0(&rot3, sp78);
			quaternion0f0976c0(sp88, sp78);
			quaternion_slerp(sp88, sp78, anim->fracmerge, sp68);

			if (translate1.f[0] != 0.0f || translate1.f[1] != 0.0f || translate1.f[2] != 0.0f) {
				translate1.x *= anim->animscale;
				translate1.y *= anim->animscale;
				translate1.z *= anim->animscale;

				if (node != model->definition->rootnode) {
					translate1.x += rodata->pos.x;
					translate1.y += rodata->pos.y;
					translate1.z += rodata->pos.z;
				}

				model_position_joint_using_quat_rot(renderdata, model, node, sp68, &translate1, &scale1);
			} else if (node != model->definition->rootnode) {
				model_position_joint_using_quat_rot(renderdata, model, node, sp68, &rodata->pos, &scale1);
			} else {
				model_position_joint_using_quat_rot(renderdata, model, node, sp68, &translate1, &scale1);
			}
		} else if (sp128) {
			f32 mult = bg_get_stage_translation_thing();

			translate1.x *= mult;
			translate1.y *= mult;
			translate1.z *= mult;

			model_position_joint_using_vec_rot(renderdata, model, node, &rot1, &translate1, true, &scale1);
		} else if (translate1.f[0] != 0.0f || translate1.f[1] != 0.0f || translate1.f[2] != 0.0f) {
			translate1.x *= anim->animscale;
			translate1.y *= anim->animscale;
			translate1.z *= anim->animscale;

			if (node != model->definition->rootnode) {
				translate1.x += rodata->pos.x;
				translate1.y += rodata->pos.y;
				translate1.z += rodata->pos.z;
			}

			model_position_joint_using_vec_rot(renderdata, model, node, &rot1, &translate1, false, &scale1);
		} else if (node != model->definition->rootnode) {
			model_position_joint_using_vec_rot(renderdata, model, node, &rot1, &rodata->pos, false, &scale1);
		} else {
			model_position_joint_using_vec_rot(renderdata, model, node, &rot1, &translate1, false, &scale1);
		}
	} else {
		if (node->parent) {
			rendermtx = model_find_node_mtx(model, node->parent, 0);
		} else {
			rendermtx = renderdata->rendermtx;
		}

		if (rendermtx) {
			mtx4_load_translation(&rodata->pos, &spe8);
			mtx00015be4(rendermtx, &spe8, &model->matrices[rodata->mtxindex0]);
		} else {
			mtx4_load_translation(&rodata->pos, &model->matrices[rodata->mtxindex0]);
		}
	}
}

void model_update_position_held_node_mtx(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node)
{
	union modelrodata *rodata = node->rodata;
	Mtxf *rendermtx;
	Mtxf sp28;
	s32 mtxindex = rodata->positionheld.mtxindex;
	Mtxf *matrices = model->matrices;

	if (node->parent) {
		rendermtx = model_find_node_mtx(model, node->parent, 0);
	} else {
		rendermtx = renderdata->rendermtx;
	}

	if (rendermtx) {
		mtx4_load_translation(&rodata->positionheld.pos, &sp28);
		mtx00015be4(rendermtx, &sp28, &matrices[mtxindex]);
	} else {
		mtx4_load_translation(&rodata->positionheld.pos, &matrices[mtxindex]);
	}
}

/**
 * For a distance node, set its target to visible based on distance.
 */
void model_update_distance_relations(struct model *model, struct modelnode *node)
{
	union modelrodata *rodata = node->rodata;
	union modelrwdata *rwdata = model_get_node_rw_data(model, node);
	Mtxf *mtx = model_find_node_mtx(model, node, 0);
	f32 distance;

	if (g_ModelDistanceDisabled || !mtx) {
		distance = 0;
	} else {
		distance = -mtx->m[3][2] * cam_get_lod_scale_z();

		if (g_ModelDistanceScale != 1) {
			distance *= g_ModelDistanceScale;
		}
	}

	if (distance > rodata->distance.near * model->scale || rodata->distance.near == 0) {
		if (distance <= rodata->distance.far * model->scale) {
			rwdata->distance.visible = true;
			node->child = rodata->distance.target;
			return;
		}
	}

	rwdata->distance.visible = false;
	node->child = NULL;
}

void model_apply_distance_relations(struct model *model, struct modelnode *node)
{
	struct modelrodata_distance *rodata = &node->rodata->distance;
	struct modelrwdata_distance *rwdata = model_get_node_rw_data(model, node);

	if (rwdata->visible) {
		node->child = rodata->target;
	} else {
		node->child = NULL;
	}
}

void model_apply_toggle_relations(struct model *model, struct modelnode *node)
{
	struct modelrodata_toggle *rodata = &node->rodata->toggle;
	struct modelrwdata_toggle *rwdata = model_get_node_rw_data(model, node);

	if (rwdata->visible) {
		node->child = rodata->target;
	} else {
		node->child = NULL;
	}
}

/**
 * Attach a head model to its placeholder on the body model.
 *
 * The given modelnode is assumed to be of type MODELNODETYPE_HEADSPOT.
 */
void model_apply_head_relations(struct model *model, struct modelnode *bodynode)
{
	struct modelrwdata_headspot *rwdata = model_get_node_rw_data(model, bodynode);

	if (rwdata->headmodeldef) {
		struct modelnode *headnode = rwdata->headmodeldef->rootnode;

		bodynode->child = headnode;

		while (headnode) {
			headnode->parent = bodynode;
			headnode = headnode->next;
		}
	}
}

void model_apply_reorder_relations_by_arg(struct modelnode *basenode, bool reverse)
{
	union modelrodata *rodata = basenode->rodata;
	struct modelnode *node1;
	struct modelnode *node2;
	struct modelnode *loopnode;

	if (reverse) {
		node1 = rodata->reorder.unk18;
		node2 = rodata->reorder.unk1c;
	} else {
		node1 = rodata->reorder.unk1c;
		node2 = rodata->reorder.unk18;
	}

	if (node1) {
		// I think what's happening here is there's two groups of siblings,
		// where node1 and node2 are the head nodes. Either group can be first,
		// and this is ensuring the node1 group is first.
		// Note that node2 might be NULL.

		basenode->child = node1;
		node1->prev = NULL;

		// Skip through node1's siblings until node2 is found or the end is
		// reached
		loopnode = node1;

		while (loopnode->next && loopnode->next != node2) {
			loopnode = loopnode->next;
		}

		loopnode->next = node2;

		if (node2) {
			// Append node2 and its siblings to node1's siblings
			node2->prev = loopnode;
			loopnode = node2;

			while (loopnode->next && loopnode->next != node1) {
				loopnode = loopnode->next;
			}

			loopnode->next = NULL;
		}
	} else {
		basenode->child = node2;

		if (node2) {
			node2->prev = NULL;
		}
	}
}

void model_apply_reorder_relations(struct model *model, struct modelnode *node)
{
	union modelrwdata *rwdata = model_get_node_rw_data(model, node);

	model_apply_reorder_relations_by_arg(node, rwdata->reorder.reverse);
}

void model_update_reorder_relations(struct model *model, struct modelnode *node)
{
	union modelrodata *rodata = node->rodata;
	union modelrwdata *rwdata = model_get_node_rw_data(model, node);
	Mtxf *mtx = model_find_node_mtx(model, node, 0);
	struct coord sp38;
	struct coord sp2c;
	f32 tmp;

	if (rodata->reorder.side == 0) {
		sp38.x = rodata->reorder.unk0c[0];
		sp38.y = rodata->reorder.unk0c[1];
		sp38.z = rodata->reorder.unk0c[2];
		mtx4_rotate_vec_in_place(mtx, &sp38);
	} else if (rodata->reorder.side == 2) {
		sp38.x = mtx->m[1][0] * rodata->reorder.unk0c[1];
		sp38.y = mtx->m[1][1] * rodata->reorder.unk0c[1];
		sp38.z = mtx->m[1][2] * rodata->reorder.unk0c[1];
	} else if (rodata->reorder.side == 3) {
		sp38.x = mtx->m[2][0] * rodata->reorder.unk0c[2];
		sp38.y = mtx->m[2][1] * rodata->reorder.unk0c[2];
		sp38.z = mtx->m[2][2] * rodata->reorder.unk0c[2];
	} else if (rodata->reorder.side == 1) {
		sp38.x = mtx->m[0][0] * rodata->reorder.unk0c[0];
		sp38.y = mtx->m[0][1] * rodata->reorder.unk0c[0];
		sp38.z = mtx->m[0][2] * rodata->reorder.unk0c[0];
	}

	sp2c.x = rodata->reorder.unk00;
	sp2c.y = rodata->reorder.unk04;
	sp2c.z = rodata->reorder.unk08;

	mtx4_transform_vec_in_place(mtx, &sp2c);

	tmp = sp38.f[0] * sp2c.f[0] + sp38.f[1] * sp2c.f[1] + sp38.f[2] * sp2c.f[2];

	if (tmp < 0) {
		rwdata->reorder.reverse = true;
	} else {
		rwdata->reorder.reverse = false;
	}

	model_apply_reorder_relations(model, node);
}

/**
 * Update a model definition's node relations based on the rwdata from the given
 * model instance, recalculating the rwdata where possible.
 *
 * However, several node types are not descended into, and toggle nodes are not
 * applied to the model definition.
 */
void model_update_relations_quick(struct model *model, struct modelnode *parent)
{
	struct modelnode *node = parent->child;

	if (parent);

	while (node) {
		s32 type = node->type & 0xff;
		bool dochildren = true;

		switch (type) {
		case MODELNODETYPE_CHRINFO:
		case MODELNODETYPE_POSITION:
		case MODELNODETYPE_0B:
		case MODELNODETYPE_CHRGUNFIRE:
		case MODELNODETYPE_0D:
		case MODELNODETYPE_0E:
		case MODELNODETYPE_0F:
		case MODELNODETYPE_POSITIONHELD:
			dochildren = false;
			break;
		case MODELNODETYPE_DISTANCE:
			model_update_distance_relations(model, node);
			break;
		case MODELNODETYPE_REORDER:
			model_update_reorder_relations(model, node);
			break;
		case MODELNODETYPE_HEADSPOT:
			model_apply_head_relations(model, node);
			break;
		case MODELNODETYPE_DL:
			break;
		}

		if (dochildren && node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node == parent->parent) {
					node = NULL;
					break;
				}

				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}
}

/**
 * Update a model definition's node relations based on the rwdata from the given
 * model instance, recalculating the rwdata where possible.
 */
void model_update_relations(struct model *model)
{
	struct modelnode *node = model->definition->rootnode;

	while (node) {
		u32 type = node->type & 0xff;

		switch (type) {
		case MODELNODETYPE_DISTANCE:
			model_update_distance_relations(model, node);
			break;
		case MODELNODETYPE_REORDER:
			model_update_reorder_relations(model, node);
			break;
		case MODELNODETYPE_TOGGLE:
			model_apply_toggle_relations(model, node);
			break;
		case MODELNODETYPE_HEADSPOT:
			model_apply_head_relations(model, node);
			break;
		case MODELNODETYPE_CHRINFO:
		case MODELNODETYPE_DL:
		default:
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}
}

void model_update_matrices(struct modelrenderdata *renderdata, struct model *model)
{
	struct modelnode *node = model->definition->rootnode;

	while (node) {
		u32 type = node->type & 0xff;

		switch (type) {
		case MODELNODETYPE_CHRINFO:
			model_update_chr_node_mtx(renderdata, model, node);
			break;
		case MODELNODETYPE_POSITION:
			model_update_position_node_mtx(renderdata, model, node);
			break;
		case MODELNODETYPE_POSITIONHELD:
			model_update_position_held_node_mtx(renderdata, model, node);
			break;
		case MODELNODETYPE_DISTANCE:
			model_update_distance_relations(model, node);
			break;
		case MODELNODETYPE_REORDER:
			model_update_reorder_relations(model, node);
			break;
		case MODELNODETYPE_TOGGLE:
			model_apply_toggle_relations(model, node);
			break;
		case MODELNODETYPE_HEADSPOT:
			model_apply_head_relations(model, node);
			break;
		case MODELNODETYPE_DL:
		default:
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}
}

void model_set_matrices(struct modelrenderdata *renderdata, struct model *model)
{
	model->matrices = renderdata->matrices;

	renderdata->matrices += model->definition->nummatrices;

#if VERSION >= VERSION_PAL_BETA
	if (var8005efb0_2 || !modelasm00018680(renderdata, model)) {
		model_update_matrices(renderdata, model);
	}
#else
	if (!modelasm00018680(renderdata, model)) {
		model_update_matrices(renderdata, model);
	}
#endif
}

void model_set_matrices_with_anim(struct modelrenderdata *renderdata, struct model *model)
{
	struct anim *anim = model->anim;
	f32 speed;
	f32 frac;
	f32 frac2;

	if (anim && anim->animnum) {
		if (PLAYERCOUNT() >= 2) {
			frac = anim->frac;
			frac2 = anim->frac2;
			speed = anim->speed;

			if (speed < 0) {
				speed = -speed;
			}

			if (speed > 0.5f) {
				anim->frac = 0;
				anim->frac2 = 0;
			}
		}

		anim_load_header(anim->animnum);
		anim->frameslot1 = anim_load_frame(anim->animnum, anim->framea);

		if (anim->frac != 0) {
			anim->frameslot2 = anim_load_frame(anim->animnum, anim->frameb);
		}

		if (anim->animnum2) {
			anim_load_header(anim->animnum2);
			anim->frameslot3 = anim_load_frame(anim->animnum2, anim->frame2a);

			if (anim->frac2 != 0) {
				anim->frameslot4 = anim_load_frame(anim->animnum2, anim->frame2b);
			}
		}

		anim_forget_frame_births();
	}

	model_set_matrices(renderdata, model);

	if (PLAYERCOUNT() >= 2 && anim && anim->animnum) {
		anim->frac = frac;
		anim->frac2 = frac2;
	}
}

s16 model_get_anim_num(struct model *model)
{
	if (model->anim) {
		return model->anim->animnum;
	}

	return 0;
}

bool model_is_flipped(struct model *model)
{
	if (model->anim) {
		return model->anim->flip;
	}

	return false;
}

f32 model_get_cur_anim_frame(struct model *model)
{
	if (model->anim) {
		return model->anim->frame;
	}

	return 0;
}

f32 model_get_anim_end_frame(struct model *model)
{
	struct anim *anim = model->anim;

	if (anim) {
		if (anim->endframe >= 0) {
			return anim->endframe;
		}

		if (anim->animnum) {
			return anim_get_num_frames(anim->animnum) - 1;
		} else {
			return 0;
		}
	}

	return 0;
}

s32 model_get_num_anim_frames(struct model *model)
{
	if (model->anim) {
		return anim_get_num_frames(model_get_anim_num(model));
	}

	return 0;
}

f32 model_get_anim_speed(struct model *model)
{
	if (model->anim) {
		return model->anim->speed;
	}

	return 1;
}

f32 model_get_abs_anim_speed(struct model *model)
{
	f32 speed;

	if (model->anim) {
		speed = model->anim->speed;

		if (speed < 0) {
			speed = -speed;
		}

		return speed;
	}

	return 1;
}

f32 model_get_effective_anim_speed(struct model *model)
{
	if (model->anim) {
		return model_get_anim_speed(model) * model->anim->playspeed;
	}

	return 1;
}

/**
 * Constrain the given frame number to the bounds of the animation, unless the
 * animation is looping in which case wrap it to the other side.
 */
s32 model_constrain_or_wrap_anim_frame(s32 frame, s16 animnum, f32 endframe)
{
	if (frame < 0) {
		if (g_ChrsAnimDebugForceLoop || (g_Anims[animnum].flags & ANIMFLAG_LOOP)) {
			frame = anim_get_num_frames(animnum) - (-frame % anim_get_num_frames(animnum));
		} else {
			frame = 0;
		}
	} else if (endframe >= 0 && frame > (s32)endframe) {
		frame = ceil(endframe);
	} else if (frame >= anim_get_num_frames(animnum)) {
		if (g_ChrsAnimDebugForceLoop || (g_Anims[animnum].flags & ANIMFLAG_LOOP)) {
			frame = frame % anim_get_num_frames(animnum);
		} else {
			frame = anim_get_num_frames(animnum) - 1;
		}
	}

	return frame;
}

void model_copy_anim_for_merge(struct model *model, f32 merge)
{
	struct anim *anim = model->anim;
	struct modelnode *node;
	u32 nodetype;

	if (anim) {
		if (merge > 0 && anim->animnum) {
			if (anim->animnum2 && anim->fracmerge == 1) {
				return;
			}

			node = model->definition->rootnode;
			nodetype = node->type & 0xff;

			anim->frame2 = anim->frame;
			anim->frac2 = anim->frac;
			anim->animnum2 = anim->animnum;
			anim->flip2 = anim->flip;
			anim->frame2a = anim->framea;
			anim->frame2b = anim->frameb;
			anim->speed2 = anim->speed;
			anim->newspeed2 = anim->newspeed;
			anim->oldspeed2 = anim->oldspeed;
			anim->timespeed2 = anim->timespeed;
			anim->elapsespeed2 = anim->elapsespeed;
			anim->endframe2 = anim->endframe;

			if (nodetype == MODELNODETYPE_CHRINFO) {
				struct modelrwdata_chrinfo *rwdata = model_get_node_rw_data(model, node);
				rwdata->unk02 = 1;
				rwdata->unk4c.x = rwdata->unk34.x;
				rwdata->unk4c.y = rwdata->unk34.y;
				rwdata->unk4c.z = rwdata->unk34.z;
				rwdata->unk40.x = rwdata->unk24.x;
				rwdata->unk40.y = rwdata->unk24.y;
				rwdata->unk40.z = rwdata->unk24.z;
			}
		} else {
			anim->animnum2 = 0;
		}
	}
}

void model_set_animation2(struct model *model, s16 animnum, s32 flip, f32 fstartframe, f32 speed, f32 merge)
{
	struct anim *anim = model->anim;

	if (anim) {
		s32 isfirstanim = !anim->animnum;
		s32 type;

		if (anim->animnum2) {
			anim->timemerge = merge;
			anim->elapsemerge = 0;
			anim->fracmerge = 1;
		} else {
			anim->timemerge = 0;
			anim->fracmerge = 0;
		}

		anim->animnum = animnum;
		anim->flip = flip;
		anim->endframe = -1;
		anim->speed = speed;
		anim->timespeed = 0;

		model_set_anim_frame(model, fstartframe);

		anim->looping = false;

		type = model->definition->rootnode->type & 0xff;

		if (type == MODELNODETYPE_CHRINFO) {
			u32 stack;
			struct modelrodata_chrinfo *rodata = &model->definition->rootnode->rodata->chrinfo;
			struct modelrwdata_chrinfo *rwdata = (struct modelrwdata_chrinfo *) model_get_node_rw_data(model, model->definition->rootnode);
			s32 animpart = rodata->animpart;
			struct skeleton *skel = model->definition->skel;
			f32 scale;
			f32 sp98;
			f32 sp94;
			struct coord translate = {0, 0, 0};
			f32 sp84;
			u8 frameslot;
			struct coord rot1;
			struct coord scale1;
			f32 sp64;
			struct coord sp58;
			struct coord sp4c;
			f32 angle;
			f32 y;
			f32 x;
			f32 z;

			if (g_Anims[anim->animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION) {
				sp64 = bg_get_stage_translation_thing();
				anim_load_header(anim->animnum);
				frameslot = anim_load_frame(anim->animnum, anim->framea);
				anim_forget_frame_births();
				anim_get_rot_translate_scale(animpart, anim->flip, skel, anim->animnum, frameslot, &rot1, &translate, &scale1);

				rwdata->unk34.x = translate.x * sp64;
				rwdata->unk34.y = translate.y * sp64;
				rwdata->unk34.z = translate.z * sp64;
				rwdata->unk30 = rwdata->yrot;

				if (anim->frac == 0) {
					rwdata->unk01 = 0;
				} else {
					anim_load_header(anim->animnum);
					frameslot = anim_load_frame(anim->animnum, anim->frameb);
					anim_forget_frame_births();
					anim_get_rot_translate_scale(animpart, anim->flip, skel, anim->animnum, frameslot, &rot1, &translate, &scale1);

					rwdata->unk24.x = translate.x * sp64;
					rwdata->unk24.y = translate.y * sp64;
					rwdata->unk24.z = translate.z * sp64;
					rwdata->unk20 = rwdata->yrot;

					rwdata->unk01 = 1;
				}
			} else {
				sp84 = anim_get_translate_angle(animpart, anim->flip, skel, anim->animnum, anim->frameb, &translate, anim->average);
				scale = model->scale * anim->animscale;

				if (scale != 1) {
					translate.x *= scale;
					translate.y *= scale;
					translate.z *= scale;
				}

				if (anim->average) {
					translate.y = rwdata->pos.y - rwdata->ground;
				}

				sp98 = cosf(rwdata->yrot);
				sp94 = sinf(rwdata->yrot);

				if (anim->frac == 0) {
					rwdata->unk34.x = rwdata->pos.f[0];
					rwdata->unk34.y = rwdata->pos.f[1] - rwdata->ground;
					rwdata->unk34.z = rwdata->pos.f[2];

					rwdata->unk30 = rwdata->yrot;

					sp58.x = rwdata->unk34.f[0] + translate.f[0] * sp98 + translate.f[2] * sp94;
					sp58.y = translate.f[1];
					sp58.z = rwdata->unk34.f[2] - translate.f[0] * sp94 + translate.f[2] * sp98;

					rwdata->unk24.x = sp58.f[0];
					rwdata->unk24.y = sp58.f[1];
					rwdata->unk24.z = sp58.f[2];

					if (rwdata->unk18 == 0) {
						rwdata->unk20 = rwdata->unk30 + sp84;

						if (rwdata->unk20 >= BADDTOR(360)) {
							rwdata->unk20 -= BADDTOR(360);
						}
					}

					rwdata->unk01 = 1;
				} else {
					x = translate.f[0] * sp98 + translate.f[2] * sp94;
					y = translate.f[1];
					z = -translate.f[0] * sp94 + translate.f[2] * sp98;

					sp4c.f[0] = rwdata->pos.f[0] + x * (1 - anim->frac);
					sp4c.f[1] = y;
					sp4c.f[2] = rwdata->pos.f[2] + z * (1 - anim->frac);

					rwdata->unk24.f[0] = sp4c.f[0];
					rwdata->unk24.f[1] = sp4c.f[1];
					rwdata->unk24.f[2] = sp4c.f[2];

					rwdata->unk34.f[0] = rwdata->unk24.f[0] - x;
					rwdata->unk34.f[1] = (rwdata->pos.f[1] - rwdata->ground) - (y - (rwdata->pos.f[1] - rwdata->ground)) * anim->frac / (1 - anim->frac);
					rwdata->unk34.f[2] = rwdata->unk24.f[2] - z;

					angle = rwdata->yrot - sp84;

					if (angle < 0) {
						angle += BADDTOR(360);
					}

					rwdata->unk30 = model_tween_rot_axis(rwdata->yrot, angle, anim->frac);

					if (rwdata->unk18 == 0) {
						rwdata->unk20 = rwdata->unk30 + sp84;

						if (rwdata->unk20 >= BADDTOR(360)) {
							rwdata->unk20 -= BADDTOR(360);
						}
					}

					rwdata->unk01 = 1;
				}

				if (isfirstanim) {
					rwdata->unk34.f[1] = rwdata->unk24.f[1];
				}
			}
		}
	}
}

bool model_is_anim_merging(struct model *model)
{
	if (model && model->anim && model->anim->animnum2
			&& model->anim->fracmerge != 0 && model->anim->fracmerge != 1) {
		return true;
	}

	return false;
}

void model_set_animation_with_merge(struct model *model, s16 animnum, u32 flip, f32 startframe, f32 speed, f32 timemerge, bool domerge)
{
	if (model) {
		if (model->anim && model->anim->animnum
				&& (g_Anims[model->anim->animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION)
				&& (g_Anims[animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION) == 0) {
			timemerge = 0;
		}

		if (domerge) {
			model_copy_anim_for_merge(model, timemerge);
		}

		model_set_animation2(model, animnum, flip, startframe, speed, timemerge);
	}
}

void model_set_animation(struct model *model, s16 animnum, s32 flip, f32 startframe, f32 speed, f32 merge)
{
	if (model) {
		if (model->anim && model->anim->animnum
				&& (g_Anims[model->anim->animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION)
				&& (g_Anims[animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION) == 0) {
			merge = 0;
		}

		model_copy_anim_for_merge(model, merge);
		model_set_animation2(model, animnum, flip, startframe, speed, merge);
	}
}

void model_copy_anim_data(struct model *src, struct model *dst)
{
	if (src->anim && dst->anim) {
		*dst->anim = *src->anim;
	}
}

void model_set_anim_looping(struct model *model, f32 loopframe, f32 loopmerge)
{
	if (model->anim) {
		model->anim->looping = true;
		model->anim->loopframe = loopframe;
		model->anim->loopmerge = loopmerge;
	}
}

void model_set_anim_end_frame(struct model *model, f32 endframe)
{
	struct anim *anim = model->anim;

	if (anim) {
		if (anim->animnum && endframe < anim_get_num_frames(anim->animnum) - 1) {
			anim->endframe = endframe;
		} else {
			anim->endframe = -1;
		}
	}
}

void model_set_anim_flip_function(struct model *model, void *callback)
{
	if (model->anim) {
		model->anim->flipfunc = callback;
	}
}

#if VERSION < VERSION_NTSC_1_0
void model_set_anim_unk6c(struct model *model, s32 value)
{
	if (model->anim) {
		model->anim->unk6c = value;
	}
}
#endif

void model_set_anim_speed(struct model *model, f32 speed, f32 startframe)
{
	struct anim *anim = model->anim;

	if (anim) {
		if (startframe > 0) {
			anim->timespeed = startframe;
			anim->newspeed = speed;
			anim->elapsespeed = 0;
			anim->oldspeed = anim->speed;
		} else {
			anim->speed = speed;
			anim->timespeed = 0;
		}
	}
}

void model_set_anim_speed_auto(struct model *model, f32 arg1, f32 startframe)
{
	struct anim *anim = model->anim;
	f32 tmp;
	f32 speed;

	if (anim) {
		if (anim->frame <= arg1) {
			tmp = arg1 - anim->frame;
		} else {
			tmp = anim_get_num_frames(anim->animnum) - anim->frame + arg1;
		}

		speed = anim->speed + (tmp + tmp) / startframe;

		model_set_anim_speed(model, speed, startframe);
	}
}

void model_set_anim_play_speed(struct model *model, f32 speed, f32 startframe)
{
	struct anim *anim = model->anim;

	if (anim) {
		if (startframe > 0) {
			anim->timeplay = startframe;
			anim->newplay = speed;
			anim->elapseplay = 0;
			anim->oldplay = anim->playspeed;
		} else {
			anim->playspeed = speed;
			anim->timeplay = 0;
		}
	}
}

void model_set_anim70(struct model *model, void *callback)
{
	if (model->anim) {
		model->anim->unk70 = callback;
	}
}

void model_set_anim_frame(struct model *model, f32 frame)
{
	s32 framea;
	s32 frameb;
	bool forwards;
	struct anim *anim = model->anim;

	if (anim) {
		framea = floor(frame);
		forwards = anim->speed >= 0;

		frameb = (forwards ? framea + 1 : framea - 1);

		anim->framea = model_constrain_or_wrap_anim_frame(framea, anim->animnum, anim->endframe);
		anim->frameb = model_constrain_or_wrap_anim_frame(frameb, anim->animnum, anim->endframe);

		if (anim->framea == anim->frameb) {
			anim->frac = 0;
			anim->frame = anim->framea;
		} else if (forwards) {
			anim->frac = frame - framea;
			anim->frame = anim->framea + anim->frac;
		} else {
			anim->frac = 1 - (frame - frameb);
			anim->frame = anim->frameb + (1 - anim->frac);
		}
	}
}

void model_set_anim_frame2(struct model *model, f32 frame1, f32 frame2)
{
	struct anim *anim = model->anim;

	if (anim) {
		model_set_anim_frame(model, frame1);

		if (anim->animnum2) {
			s32 framea = floor(frame2);
			s32 frameb;
			bool forwards = anim->speed2 >= 0;

			frameb = (forwards ? framea + 1 : framea - 1);

			anim->frame2a = model_constrain_or_wrap_anim_frame(framea, anim->animnum2, anim->endframe2);
			anim->frame2b = model_constrain_or_wrap_anim_frame(frameb, anim->animnum2, anim->endframe2);

			if (anim->frame2a == anim->frame2b) {
				anim->frac2 = 0;
				anim->frame2 = anim->frame2a;
			} else if (forwards) {
				anim->frac2 = frame2 - framea;
				anim->frame2 = anim->frame2a + anim->frac2;
			} else {
				anim->frac2 = 1 - (frame2 - frameb);
				anim->frame2 = anim->frame2b + (1 - anim->frac2);
			}
		}
	}
}

bool g_ModelAnimMergingEnabled = true;

void model_set_anim_merging_enabled(bool value)
{
	g_ModelAnimMergingEnabled = value;
}

bool model_is_anim_merging_enabled(void)
{
	return g_ModelAnimMergingEnabled;
}

void model_set_anim_frame2_with_chr_stuff(struct model *model, f32 curframe, f32 endframe, f32 curframe2, f32 endframe2)
{
	struct anim *anim = model->anim;

	if (anim != NULL) {
		struct modelnode *rootnode = model->definition->rootnode;
		u16 nodetype = rootnode->type;

		if ((nodetype & 0xff) == MODELNODETYPE_CHRINFO) {
			struct modelrodata_chrinfo *rodata = &rootnode->rodata->chrinfo;
			struct modelrwdata_chrinfo *rwdata = model_get_node_rw_data(model, rootnode);

			if (rwdata->unk00 == 0) {
				s32 animpart = rodata->animpart;
				struct skeleton *skel = model->definition->skel;
				f32 scale = model->scale * anim->animscale;
				f32 sine;
				f32 cosine;
				struct coord translate = {0, 0, 0};
				u8 frameslot;
				f32 f20;
				s32 floorcur;
				s32 floorend;
				struct coord spe0;
				f32 f30;
				struct coord spd0;
				f32 spcc;
				s32 spc8;
				bool forwards;
				f32 absspeed;
				f32 absspeed2;
				f32 f22;
				s32 s0frame;
				struct coord rot1;
				struct coord scale1;
				struct coord sp90;
				u32 stack;

				spe0.x = rwdata->unk34.x;
				spe0.y = rwdata->unk34.y;
				spe0.z = rwdata->unk34.z;

				f30 = rwdata->unk30;

				spd0.x = rwdata->unk24.x;
				spd0.y = rwdata->unk24.y;
				spd0.z = rwdata->unk24.z;

				spcc = rwdata->unk20;
				spc8 = rwdata->unk01;

				absspeed = anim->speed;

				if (absspeed < 0.0f) {
					absspeed = -absspeed;
				}

				absspeed2 = anim->speed2;

				if (absspeed2 < 0.0f) {
					absspeed2 = -absspeed2;
				}

				forwards = false;

				if (curframe <= endframe) {
					forwards = true;
				}

				if (forwards) {
					floorcur = floor(curframe) + 1;
					floorend = floor(endframe);
				} else {
					floorcur = ceil(curframe) - 1;
					floorend = ceil(endframe);
				}

				if (g_Anims[anim->animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION) {
					f20 = bg_get_stage_translation_thing();

					if (floorend != anim->framea) {
						s0frame = model_constrain_or_wrap_anim_frame(floorend, anim->animnum, anim->endframe);

						anim->framea = s0frame;

						if (spc8 && floorend == anim->frameb) {
							spe0.x = spd0.x;
							spe0.y = spd0.y;
							spe0.z = spd0.z;
						} else {
							anim_load_header(anim->animnum);
							frameslot = anim_load_frame(anim->animnum, s0frame);
							anim_forget_frame_births();
							anim_get_rot_translate_scale(animpart, anim->flip, skel, anim->animnum, frameslot, &rot1, &translate, &scale1);

							spe0.x = translate.x * f20;
							spe0.y = translate.y * f20;
							spe0.z = translate.z * f20;
						}

						floorcur = floorend;

						if (forwards) {
							floorcur++;
						} else {
							floorcur--;
						}

						s0frame = model_constrain_or_wrap_anim_frame(floorcur, anim->animnum, anim->endframe);
						anim->frameb = s0frame;

						anim_load_header(anim->animnum);
						frameslot = anim_load_frame(anim->animnum, s0frame);

						anim_forget_frame_births();
						anim_get_rot_translate_scale(animpart, anim->flip, skel, anim->animnum, frameslot, &rot1, &translate, &scale1);

						spc8 = true;

						spd0.x = translate.x * f20;
						spd0.y = translate.y * f20;
						spd0.z = translate.z * f20;
					}
				} else {
					while (true) {
						if (forwards) {
							if (floorend < floorcur) {
								break;
							}
						} else {
							if (floorend > floorcur) {
								break;
							}
						}

						s0frame = model_constrain_or_wrap_anim_frame(floorcur, anim->animnum, anim->endframe);

						anim->framea = s0frame;

						if (spc8) {
							spe0.x = spd0.x;
							spe0.y = spd0.y;
							spe0.z = spd0.z;

							if (rwdata->unk18 == 0.0f) {
								f30 = spcc;
							}
						} else {
							f22 = anim_get_translate_angle(animpart, anim->flip, skel, anim->animnum, s0frame, &translate, anim->average);

							if (scale != 1.0f) {
								translate.x *= scale;
								translate.y *= scale;
								translate.z *= scale;
							}

							if (!forwards) {
								translate.x = -translate.x;
								translate.z = -translate.z;

								if (f22 > 0.0f) {
									f22 = BADDTOR(360) - f22;
								}
							}

							if (anim->average) {
								translate.y = rwdata->pos.y - rwdata->ground;
							}

							cosine = cosf(rwdata->yrot);
							sine = sinf(rwdata->yrot);

							spe0.x += translate.x * cosine + translate.z * sine;
							spe0.y = translate.y;
							spe0.z += (-translate.x * sine) + (translate.z * cosine);

							if (rwdata->unk18 == 0.0f) {
								f30 += f22;

								if (f30 >= BADDTOR(360)) {
									f30 -= BADDTOR(360);
								}
							}
						}

						if (forwards) {
							floorcur++;
						} else {
							floorcur--;
						}

						s0frame = model_constrain_or_wrap_anim_frame(floorcur, anim->animnum, anim->endframe);
						anim->frameb = s0frame;

						if (anim->frameb != anim->framea) {
							f22 = anim_get_translate_angle(animpart, anim->flip, skel, anim->animnum, s0frame, &translate, anim->average);

							spc8 = true;

							if (scale != 1.0f) {
								translate.x *= scale;
								translate.y *= scale;
								translate.z *= scale;
							}

							if (!forwards) {
								translate.x = -translate.x;
								translate.z = -translate.z;

								if (f22 > 0.0f) {
									f22 = BADDTOR(360) - f22;
								}
							}

							if (anim->average) {
								translate.y = rwdata->unk34.y;
							}

							cosine = cosf(rwdata->unk30);
							sine = sinf(rwdata->unk30);

							if (g_ModelAnimMergingEnabled && anim->animnum2) {
								spd0.x = translate.x * cosine + translate.f[2] * sine;
								spd0.z = -translate.x * sine + translate.f[2] * cosine;

								if (absspeed > 0.0f) {
									f32 f0 = anim->fracmerge - anim->playspeed / (absspeed * anim->timemerge);

									if (f0 < 0.0f) {
										f0 = 0.0f;
									}

									f0 = (f0 + anim->fracmerge) / 2.0f;

									if (1);

									sp90.f[0] = (rwdata->unk40.x - rwdata->unk4c.x) * absspeed2 / absspeed;
									sp90.f[2] = (rwdata->unk40.z - rwdata->unk4c.z) * absspeed2 / absspeed;
									spd0.x += (sp90.x - spd0.x) * f0;
									spd0.z += (sp90.z - spd0.z) * f0;
								} else {
									spd0.x += (rwdata->unk40.x - rwdata->unk4c.x) * anim->fracmerge;
									spd0.z += (rwdata->unk40.z - rwdata->unk4c.z) * anim->fracmerge;
								}

								spd0.x += spe0.x;
								spd0.z += spe0.z;
								spd0.y = translate.y;
							} else {
								spd0.x = spe0.x + translate.x * cosine + translate.f[2] * sine;
								spd0.y = translate.y;
								spd0.z = spe0.z - translate.x * sine + translate.f[2] * cosine;
							}

							if (rwdata->unk5c > 0.0f && absspeed > 0.0f) {
								f32 increment = 1.0f / absspeed;

								if (increment > rwdata->unk5c) {
									increment = rwdata->unk5c;
									rwdata->unk5c = 0.0f;
								} else {
									rwdata->unk5c -= increment;
								}

								f22 += rwdata->unk58 * increment;

								if (f22 < 0.0f) {
									f22 += BADDTOR(360);
								} else if (f22 >= BADDTOR(360)) {
									f22 -= BADDTOR(360);
								}
							}

							if (rwdata->unk18 == 0.0f) {
								spcc = f30 + f22;

								if (spcc >= BADDTOR(360)) {
									spcc -= BADDTOR(360);
								}
							}
						}
					}
				}

				rwdata->unk34.x = spe0.x;
				rwdata->unk34.y = spe0.y;
				rwdata->unk34.z = spe0.z;

				rwdata->unk30 = f30;

				rwdata->unk24.x = spd0.f[0];
				rwdata->unk24.y = spd0.f[1];
				rwdata->unk24.z = spd0.f[2];

				rwdata->unk20 = spcc;
				rwdata->unk01 = spc8;

				if (anim->framea == anim->frameb) {
					anim->frac = 0.0f;
					anim->frame = anim->framea;
				} else if (forwards) {
					anim->frac = endframe - floorend;
					anim->frame = anim->framea + anim->frac;
				} else {
					anim->frac = floorend - endframe;
					anim->frame = anim->frameb + (1.0f - anim->frac);
				}

				if (anim->animnum2 && (g_Anims[anim->animnum].flags & ANIMFLAG_ABSOLUTETRANSLATION) == 0) {
					s32 floorcur2 = floor(curframe2);
					s32 floorend2 = floor(endframe2);

					if ((forwards && floorcur2 < floorend2) || (!forwards && floorend2 < floorcur2)) {
						if (rwdata->unk02 != 0) {
							rwdata->unk4c.f[1] = rwdata->unk40.f[1];
						} else {
							rwdata->unk4c.f[1] = rwdata->unk34.f[1];
						}

						anim->frame2a = model_constrain_or_wrap_anim_frame(floorend2, anim->animnum2, anim->endframe2);

						s0frame = model_constrain_or_wrap_anim_frame(floorend2 + 1, anim->animnum2, anim->endframe2);
						anim->frame2b = s0frame;

						anim_get_translate_angle(animpart, anim->flip2, skel, anim->animnum2, s0frame, &translate, anim->average);

						if (scale != 1.0f) {
							translate.y *= scale;
						}

						if (anim->average) {
							translate.y = rwdata->unk4c.y;
						}

						rwdata->unk40.y = translate.y;
						rwdata->unk02 = 1;
					}

					if (forwards) {
						anim->frac2 = endframe2 - floorend2;
						anim->frame2 = anim->frame2a + anim->frac2;
					} else {
						anim->frac2 = 1.0f - (endframe2 - floorend2);
						anim->frame2 = anim->frame2b + (1.0f - anim->frac2);
					}
				} else {
					rwdata->unk02 = 0;
				}
			} else {
				model_set_anim_frame2(model, endframe, endframe2);
			}
		} else {
			model_set_anim_frame2(model, endframe, endframe2);
		}
	}
}

void model_tick_anim_quarter_speed(struct model *model, s32 lvupdate240, bool arg2)
{
	f32 frame;
	f32 frame2;
	f32 speed;
	f32 speed2;
	f32 startframe;
	f32 endframe;
	f32 realendframe;
	struct anim *anim = model->anim;

	if (anim && lvupdate240 > 0) {
		frame = anim->frame;
		frame2 = anim->frame2;

		for (; lvupdate240 > 0; lvupdate240--) {
			if (anim->timeplay > 0) {
				anim->elapseplay += 0.25f;

				if (anim->elapseplay < anim->timeplay) {
					anim->playspeed = anim->oldplay + (anim->newplay - anim->oldplay) * anim->elapseplay / anim->timeplay;
				} else {
					anim->timeplay = 0;
					anim->playspeed = anim->newplay;
				}
			}

			if (anim->timemerge > 0) {
				anim->elapsemerge += anim->playspeed * 0.25f;

				if (anim->elapsemerge == 0) {
					anim->fracmerge = 1;
				} else {
					if (anim->elapsemerge < anim->timemerge) {
						anim->fracmerge = (anim->timemerge - anim->elapsemerge) / anim->timemerge;
					} else {
						anim->timemerge = 0;
						anim->fracmerge = 0;
						anim->animnum2 = 0;
					}
				}
			}

			if (anim->timespeed > 0) {
				anim->elapsespeed += anim->playspeed * 0.25f;

				if (anim->elapsespeed < anim->timespeed) {
					anim->speed = anim->oldspeed + (anim->newspeed - anim->oldspeed) * anim->elapsespeed / anim->timespeed;
				} else {
					anim->timespeed = 0;
					anim->speed = anim->newspeed;
				}
			}

			speed = anim->speed;
			frame += anim->playspeed * speed * 0.25f;

			if (anim->animnum2) {
				if (anim->timespeed2 > 0) {
					anim->elapsespeed2 += anim->playspeed * 0.25f;

					if (anim->elapsespeed2 < anim->timespeed2) {
						anim->speed2 = anim->oldspeed2 + (anim->newspeed2 - anim->oldspeed2) * anim->elapsespeed2 / anim->timespeed2;
					} else {
						anim->timespeed2 = 0;
						anim->speed2 = anim->newspeed2;
					}
				}

				speed2 = anim->speed2;
				frame2 += anim->playspeed * speed2 * 0.25f;
			}

			if (anim->looping) {
				realendframe = anim->endframe;

				if (speed >= 0) {
					endframe = anim_get_num_frames(anim->animnum) - 1;
					startframe = anim->loopframe;

					if (realendframe >= 0 && endframe > realendframe) {
						endframe = realendframe;
					}
				} else {
					endframe = anim->loopframe;
					startframe = anim_get_num_frames(anim->animnum) - 1;

					if (realendframe >= 0 && startframe > realendframe) {
						startframe = realendframe;
					}
				}

				if ((speed >= 0 && frame >= endframe) || (speed < 0 && frame <= endframe)) {
					f32 prevnewspeed = anim->newspeed;
					f32 prevoldspeed = anim->oldspeed;
					f32 prevtimespeed = anim->timespeed;
					f32 prevelapsespeed = anim->elapsespeed;

					if (arg2) {
						model_set_anim_frame2_with_chr_stuff(model, anim->frame, endframe, 0, 0);
					} else {
						model_set_anim_frame2(model, endframe, 0);
					}

					model_set_animation(model, anim->animnum, anim->flip, startframe, anim->speed, anim->loopmerge);

					anim->looping = true;
					anim->endframe = realendframe;

					anim->newspeed = prevnewspeed;
					anim->oldspeed = prevoldspeed;
					anim->timespeed = prevtimespeed;
					anim->elapsespeed = prevelapsespeed;

					frame2 = frame;
					frame = startframe + frame - endframe;

					if (anim->flipfunc) {
						anim->flipfunc();
					}
				}
			}
		}

		if (arg2) {
			if (anim->animnum2) {
				model_set_anim_frame2_with_chr_stuff(model, anim->frame, frame, anim->frame2, frame2);
			} else {
				model_set_anim_frame2_with_chr_stuff(model, anim->frame, frame, 0, 0);
			}
		} else {
			if (anim->animnum2) {
				model_set_anim_frame2(model, frame, frame2);
			} else {
				model_set_anim_frame2(model, frame, 0);
			}
		}
	}
}

#if VERSION < VERSION_PAL_BETA
/**
 * This is identical to the above function but removes the 0.25f multipliers.
 */
void model_tick_anim(struct model *model, s32 lvupdate240, bool arg2)
{
	f32 frame;
	f32 frame2;
	f32 speed;
	f32 speed2;
	f32 startframe;
	f32 endframe;
	f32 realendframe;
	struct anim *anim = model->anim;

	if (anim && lvupdate240 > 0) {
		frame = anim->frame;
		frame2 = anim->frame2;

		for (; lvupdate240 > 0; lvupdate240--) {
			if (anim->timeplay > 0) {
				anim->elapseplay++;

				if (anim->elapseplay < anim->timeplay) {
					anim->playspeed = anim->oldplay + (anim->newplay - anim->oldplay) * anim->elapseplay / anim->timeplay;
				} else {
					anim->timeplay = 0;
					anim->playspeed = anim->newplay;
				}
			}

			if (anim->timemerge > 0) {
				anim->elapsemerge += anim->playspeed;

				if (anim->elapsemerge == 0) {
					anim->fracmerge = 1;
				} else {
					if (anim->elapsemerge < anim->timemerge) {
						anim->fracmerge = (anim->timemerge - anim->elapsemerge) / anim->timemerge;
					} else {
						anim->timemerge = 0;
						anim->fracmerge = 0;
						anim->animnum2 = 0;
					}
				}
			}

			if (anim->timespeed > 0) {
				anim->elapsespeed += anim->playspeed;

				if (anim->elapsespeed < anim->timespeed) {
					anim->speed = anim->oldspeed + (anim->newspeed - anim->oldspeed) * anim->elapsespeed / anim->timespeed;
				} else {
					anim->timespeed = 0;
					anim->speed = anim->newspeed;
				}
			}

			speed = anim->speed;
			frame += anim->playspeed * speed;

			if (anim->animnum2) {
				if (anim->timespeed2 > 0) {
					anim->elapsespeed2 += anim->playspeed;

					if (anim->elapsespeed2 < anim->timespeed2) {
						anim->speed2 = anim->oldspeed2 + (anim->newspeed2 - anim->oldspeed2) * anim->elapsespeed2 / anim->timespeed2;
					} else {
						anim->timespeed2 = 0;
						anim->speed2 = anim->newspeed2;
					}
				}

				speed2 = anim->speed2;
				frame2 += anim->playspeed * speed2;
			}

			if (anim->looping) {
				realendframe = anim->endframe;

				if (speed >= 0) {
					endframe = anim_get_num_frames(anim->animnum) - 1;
					startframe = anim->loopframe;

					if (realendframe >= 0 && endframe > realendframe) {
						endframe = realendframe;
					}
				} else {
					endframe = anim->loopframe;
					startframe = anim_get_num_frames(anim->animnum) - 1;

					if (realendframe >= 0 && startframe > realendframe) {
						startframe = realendframe;
					}
				}

				if ((speed >= 0 && frame >= endframe) || (speed < 0 && frame <= endframe)) {
					f32 prevnewspeed = anim->newspeed;
					f32 prevoldspeed = anim->oldspeed;
					f32 prevtimespeed = anim->timespeed;
					f32 prevelapsespeed = anim->elapsespeed;

					if (arg2) {
						model_set_anim_frame2_with_chr_stuff(model, anim->frame, endframe, 0, 0);
					} else {
						model_set_anim_frame2(model, endframe, 0);
					}

					model_set_animation(model, anim->animnum, anim->flip, startframe, anim->speed, anim->loopmerge);

					anim->looping = true;
					anim->endframe = realendframe;

					anim->newspeed = prevnewspeed;
					anim->oldspeed = prevoldspeed;
					anim->timespeed = prevtimespeed;
					anim->elapsespeed = prevelapsespeed;

					frame2 = frame;
					frame = startframe + frame - endframe;

					if (anim->flipfunc) {
						anim->flipfunc();
					}
				}
			}
		}

		if (arg2) {
			if (anim->animnum2) {
				model_set_anim_frame2_with_chr_stuff(model, anim->frame, frame, anim->frame2, frame2);
			} else {
				model_set_anim_frame2_with_chr_stuff(model, anim->frame, frame, 0, 0);
			}
		} else {
			if (anim->animnum2) {
				model_set_anim_frame2(model, frame, frame2);
			} else {
				model_set_anim_frame2(model, frame, 0);
			}
		}
	}
}
#endif

void model_apply_rendermode_simple(struct modelrenderdata *renderdata)
{
	gDPPipeSync(renderdata->gdl++);
	gDPSetCycleType(renderdata->gdl++, G_CYC_1CYCLE);

	if (renderdata->zbufferenabled) {
		gDPSetRenderMode(renderdata->gdl++, G_RM_AA_ZB_OPA_SURF, G_RM_AA_ZB_OPA_SURF2);
	} else {
		gDPSetRenderMode(renderdata->gdl++, G_RM_AA_OPA_SURF, G_RM_AA_OPA_SURF2);
	}

	gDPSetCombineMode(renderdata->gdl++, G_CC_MODULATEIA, G_CC_MODULATEIA);
}

void model_apply_rendermode_ctxaware_1pass(struct modelrenderdata *renderdata, bool isopa)
{
	if (renderdata->context == MODELRENDERCONTEXT_CHR_OPA) {
		// envcolour is blood colour
		// fogcolour is shade colour
		if (isopa) {
			gDPPipeSync(renderdata->gdl++);
			gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
			gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);
			gDPSetEnvColorViaWord(renderdata->gdl++, renderdata->envcolour | 0xff);
			gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_17, G_CC_CUSTOM_18);

			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
			}
		} else {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
			}
		}
	} else if (renderdata->context == MODELRENDERCONTEXT_CHR_XLU) {
		// envcolour is opacity
		// fogcolour is shade colour
		if (isopa) {
			gDPPipeSync(renderdata->gdl++);
			gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
			gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);
			gDPSetEnvColorViaWord(renderdata->gdl++, renderdata->envcolour);
			gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_19, G_CC_CUSTOM_18);

			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
			}
		}
	} else if (renderdata->context == MODELRENDERCONTEXT_OBJ_OPA) {
		// envcolour's blue channel is opacity
		// fogcolour is shade colour
		if ((renderdata->envcolour & 0xff) == 0) {
			if (isopa) {
				gDPPipeSync(renderdata->gdl++);
				gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
				gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);
				gDPSetEnvColorViaWord(renderdata->gdl++, 0xffffffff);
				gDPSetPrimColor(renderdata->gdl++, 0, 0, 0, 0, 0, (renderdata->envcolour >> 8) & 0xff);
				gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_CUSTOM_20);

				if (renderdata->zbufferenabled) {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
				} else {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
				}
			} else {
				if (renderdata->zbufferenabled) {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
				} else {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
				}
			}
		} else {
			if (isopa) {
				gDPPipeSync(renderdata->gdl++);
				gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
				gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);
				gDPSetEnvColor(renderdata->gdl++, 0, 0, 0, renderdata->envcolour);
				gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_21, G_CC_CUSTOM_18);

				if (renderdata->zbufferenabled) {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_TEX_EDGE2);
				} else {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_TEX_EDGE2);
				}
			} else {
				gDPSetPrimColor(renderdata->gdl++, 0, 0, 0, 0, 0, (renderdata->envcolour >> 8) & 0xff);
				gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_22, G_CC_CUSTOM_23);

				if (renderdata->zbufferenabled) {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_TEX_EDGE2);
				} else {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_TEX_EDGE2);
				}
			}
		}
	} else if (renderdata->context == MODELRENDERCONTEXT_BONDGUN_OPA) {
		// envcolour is shade colour
		if (isopa) {
			gDPPipeSync(renderdata->gdl++);
			gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
			gDPSetFogColorViaWord(renderdata->gdl++, renderdata->envcolour);
			gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);

			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
			}
		} else {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
			}
		}
	} else if (renderdata->context == MODELRENDERCONTEXT_BONDGUN_OBJ_XLU) {
		// envcolour is opacity
		// fogcolour is shade colour
		u8 alpha;

		if (isopa) {
			gDPPipeSync(renderdata->gdl++);
			gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
			gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);

			alpha = renderdata->envcolour & 0xff;

			if (alpha < 255) {
				gDPSetEnvColor(renderdata->gdl++, 0xff, 0xff, 0xff, alpha);

				if (renderdata->envcolour & 0xff00) {
					gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_24, G_CC_MODULATEIA2);
				} else {
					gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_25, G_CC_MODULATEIA2);
				}
			} else {
				gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);
			}

			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
			}
		} else {
			alpha = renderdata->envcolour & 0xff;

			if (alpha < 255) {
				gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_26, G_CC_MODULATEIA2);
			} else {
				gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);
			}
		}
	} else { // MODELRENDERCONTEXT_MENUMODEL_OPA
		if (isopa) {
			gDPPipeSync(renderdata->gdl++);
			gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
			gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);

			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_OPA_SURF2);
			}
		} else {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_ZB_XLU_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_XLU_SURF2);
			}
		}
	}
}

void model_apply_rendermode_ctxaware_2pass(struct modelrenderdata *renderdata, bool isopa)
{
	if (renderdata->context == MODELRENDERCONTEXT_CHR_OPA) {
		// envcolour is blood colour
		// fogcolour is shade colour
		gDPPipeSync(renderdata->gdl++);
		gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
		gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);
		gDPSetEnvColorViaWord(renderdata->gdl++, renderdata->envcolour | 0x000000ff);
		gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_17, G_CC_CUSTOM_18);

		if (isopa) {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
			}
		} else {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
			}
		}
	} else if (renderdata->context == MODELRENDERCONTEXT_CHR_XLU) {
		// envcolour is opacity
		// fogcolour is shade colour
		gDPPipeSync(renderdata->gdl++);
		gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
		gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);
		gDPSetEnvColorViaWord(renderdata->gdl++, renderdata->envcolour);
		gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_19, G_CC_CUSTOM_18);

		if (renderdata->zbufferenabled) {
			gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
		} else {
			gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
		}
	} else if (renderdata->context == MODELRENDERCONTEXT_OBJ_OPA) {
		// envcolour's blue channel is opacity
		// fogcolour is shade colour
		if ((renderdata->envcolour & 0xff) == 0) {
			gDPPipeSync(renderdata->gdl++);
			gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
			gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);
			gDPSetEnvColorViaWord(renderdata->gdl++, 0xffffffff);
			gDPSetPrimColor(renderdata->gdl++, 0, 0, 0, 0, 0, (renderdata->envcolour >> 8) & 0xff);

			if (isopa) {
				gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_CUSTOM_20);

				if (renderdata->zbufferenabled) {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
				} else {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
				}
			} else {
				gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_CUSTOM_20);

				if (renderdata->zbufferenabled) {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
				} else {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
				}
			}
		} else {
			gDPPipeSync(renderdata->gdl++);
			gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
			gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);
			gDPSetEnvColorViaWord(renderdata->gdl++, renderdata->envcolour & 0xff);

			if (isopa) {
				gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_21, G_CC_CUSTOM_18);

				if (renderdata->zbufferenabled) {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_TEX_EDGE2);
				} else {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_TEX_EDGE2);
				}
			} else {
				gDPSetPrimColor(renderdata->gdl++, 0, 0, 0, 0, 0, (renderdata->envcolour >> 8) & 0xff);
				gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_22, G_CC_CUSTOM_23);

				if (renderdata->zbufferenabled) {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_TEX_EDGE2);
				} else {
					gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_TEX_EDGE2);
				}
			}
		}
	} else if (renderdata->context == MODELRENDERCONTEXT_BONDGUN_OPA) {
		// envcolour is shade colour
		gDPPipeSync(renderdata->gdl++);
		gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
		gDPSetFogColorViaWord(renderdata->gdl++, renderdata->envcolour);
		gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);

		if (isopa) {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
			}
		} else {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
			}
		}
	} else if (renderdata->context == MODELRENDERCONTEXT_BONDGUN_OBJ_XLU) {
		// envcolour is opacity
		// fogcolour is shade colour
		u8 alpha;

		gDPPipeSync(renderdata->gdl++);
		gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
		gDPSetFogColorViaWord(renderdata->gdl++, renderdata->fogcolour);

		alpha = renderdata->envcolour & 0xff;

		if (alpha < 255) {
			gDPSetEnvColor(renderdata->gdl++, 0xff, 0xff, 0xff, alpha);

			if (isopa) {
				if (renderdata->envcolour & 0xff00) {
					gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_24, G_CC_MODULATEIA2);
				} else {
					gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_25, G_CC_MODULATEIA2);
				}
			} else {
				gDPSetCombineMode(renderdata->gdl++, G_CC_CUSTOM_26, G_CC_MODULATEIA2);
			}
		} else {
			gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);
		}

		if (renderdata->zbufferenabled) {
			gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
		} else {
			gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
		}
	} else { // MODELRENDERCONTEXT_MENUMODEL_OPA
		gDPPipeSync(renderdata->gdl++);
		gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);
		gDPSetFogColorViaWord(renderdata->gdl++, 0xffffff00);
		gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);

		if (isopa) {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_OPA_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_OPA_SURF2);
			}
		} else {
			if (renderdata->zbufferenabled) {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_ZB_XLU_SURF2);
			} else {
				gDPSetRenderMode(renderdata->gdl++, G_RM_FOG_PRIM_A, G_RM_AA_XLU_SURF2);
			}
		}
	}
}

void model_apply_rendermode_trilerp(struct modelrenderdata *renderdata)
{
	gDPPipeSync(renderdata->gdl++);
	gDPSetCycleType(renderdata->gdl++, G_CYC_2CYCLE);

	if (renderdata->zbufferenabled) {
		gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_ZB_OPA_SURF2);
	} else {
		gDPSetRenderMode(renderdata->gdl++, G_RM_PASS, G_RM_AA_OPA_SURF2);
	}

	gDPSetCombineMode(renderdata->gdl++, G_CC_TRILERP, G_CC_MODULATEIA2);
}

void model_apply_cull_mode(struct modelrenderdata *renderdata)
{
	if (renderdata->cullmode == CULLMODE_NONE) {
		gSPClearGeometryMode(renderdata->gdl++, G_CULL_BOTH);
	} else if (renderdata->cullmode == CULLMODE_FRONT) {
		gSPSetGeometryMode(renderdata->gdl++, G_CULL_FRONT);
	} else if (renderdata->cullmode == CULLMODE_BACK) {
		gSPSetGeometryMode(renderdata->gdl++, G_CULL_BACK);
	}
}

void model_render_node_gundl(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node)
{
	struct modelrodata_gundl *rodata = &node->rodata->gundl;

	if (g_ModelShouldRenderGunDlCallback && !g_ModelShouldRenderGunDlCallback(model, node)) {
		return;
	}

	if ((renderdata->flags & MODELRENDERFLAG_OPA) && rodata->opagdl) {
		gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->baseaddr));

		if (renderdata->cullmode) {
			model_apply_cull_mode(renderdata);
		}

		switch (rodata->rendermode) {
		case MODELRENDERMODE_SIMPLE:
			model_apply_rendermode_simple(renderdata);
			break;
		case MODELRENDERMODE_CTXAWARE_1PASS:
			model_apply_rendermode_ctxaware_1pass(renderdata, true);
			break;
		case MODELRENDERMODE_CTXAWARE_2PASS:
			model_apply_rendermode_ctxaware_2pass(renderdata, true);
			break;
		case MODELRENDERMODE_TRILERP:
			model_apply_rendermode_trilerp(renderdata);
			break;
		}

		gSPDisplayList(renderdata->gdl++, rodata->opagdl);

		if (rodata->rendermode == MODELRENDERMODE_CTXAWARE_1PASS && rodata->xlugdl) {
			model_apply_rendermode_ctxaware_1pass(renderdata, false);

			gSPDisplayList(renderdata->gdl++, rodata->xlugdl);
		}
	}

	if ((renderdata->flags & MODELRENDERFLAG_XLU) && rodata->opagdl && rodata->rendermode == MODELRENDERMODE_CTXAWARE_2PASS && rodata->xlugdl) {
		gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->baseaddr));

		if (renderdata->cullmode) {
			model_apply_cull_mode(renderdata);
		}

		model_apply_rendermode_ctxaware_2pass(renderdata, false);

		gSPDisplayList(renderdata->gdl++, rodata->xlugdl);
	}
}

void model_render_node_dl(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node)
{
	union modelrodata *rodata = node->rodata;

	if (g_ModelShouldRenderGunDlCallback && !g_ModelShouldRenderGunDlCallback(model, node)) {
		return;
	}

	if (renderdata->flags & MODELRENDERFLAG_OPA) {
		union modelrwdata *rwdata = model_get_node_rw_data(model, node);

		if (rwdata->dl.gdl) {
			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->dl.colours));

			if (renderdata->cullmode) {
				model_apply_cull_mode(renderdata);
			}

			switch (rodata->dl.rendermode) {
			case MODELRENDERMODE_SIMPLE:
				model_apply_rendermode_simple(renderdata);
				break;
			case MODELRENDERMODE_CTXAWARE_1PASS:
				model_apply_rendermode_ctxaware_1pass(renderdata, true);
				break;
			case MODELRENDERMODE_CTXAWARE_2PASS:
				model_apply_rendermode_ctxaware_2pass(renderdata, true);
				break;
			case MODELRENDERMODE_TRILERP:
				model_apply_rendermode_trilerp(renderdata);
				break;
			}

			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_VTX, osVirtualToPhysical(rwdata->dl.vertices));
			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL2, osVirtualToPhysical(rwdata->dl.colours));

			gSPDisplayList(renderdata->gdl++, rwdata->dl.gdl);

			if (rodata->dl.rendermode == MODELRENDERMODE_CTXAWARE_1PASS && rodata->dl.xlugdl) {
				model_apply_rendermode_ctxaware_1pass(renderdata, false);

				gSPDisplayList(renderdata->gdl++, rodata->dl.xlugdl);
			}
		}
	}

	if (renderdata->flags & MODELRENDERFLAG_XLU) {
		union modelrwdata *rwdata = model_get_node_rw_data(model, node);

		if (rwdata->dl.gdl && rodata->dl.rendermode == MODELRENDERMODE_CTXAWARE_2PASS && rodata->dl.xlugdl) {
			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->dl.colours));

			if (renderdata->cullmode) {
				model_apply_cull_mode(renderdata);
			}

			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_VTX, osVirtualToPhysical(rwdata->dl.vertices));
			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL2, osVirtualToPhysical(rwdata->dl.colours));

			model_apply_rendermode_ctxaware_2pass(renderdata, false);

			gSPDisplayList(renderdata->gdl++, rodata->dl.xlugdl);
		}
	}
}

/**
 * Star gunfire is a muzzle flash in a first person perspective, where the
 * muzzle flash has 3 or 4 "arms" that flare out from the main body.
 *
 * Some weapons that use this are the Cyclone, Dragon, K7 Avenger, AR34,
 * SuperDragon and RC-P45.
 *
 * This function reads vertices from the model definition, tweaks them randomly,
 * writes them to a newly allocated vertices table and queues the node's
 * displaylist to the renderdata's DL.
 */
void model_render_node_star_gunfire(struct modelrenderdata *renderdata, struct modelnode *node)
{
	if (renderdata->flags & MODELRENDERFLAG_XLU) {
		struct modelrodata_stargunfire *rodata = &node->rodata->stargunfire;
		s32 i;

		if (rodata->gdl) {
			Vtx *src = (Vtx *) rodata->vertices;
			Vtx *dst = g_ModelVtxAllocatorFunc(rodata->unk00 * 4);

			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_VTX, osVirtualToPhysical(dst));
			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL2, osVirtualToPhysical((void *)ALIGN8((uintptr_t)&rodata->vertices[rodata->unk00 << 2])));
			gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->baseaddr));

			gDPSetFogColor(renderdata->gdl++, 0x00, 0x00, 0x00, 0x00);
			gSPDisplayList(renderdata->gdl++, rodata->gdl);

			for (i = 0; i < rodata->unk00; i++) {
				u16 rand1 = (random() << 10) & 0xffff;
				s32 s4 = ((coss(rand1) << 5) * 181) >> 18;
				s32 s3 = ((sins(rand1) << 5) * 181) >> 18;
				s32 s1 = random() >> 31;
				s32 mult = 0x10000 - (random() & 0x3fff);
				s32 corner1 = 0x200 + s3;
				s32 corner2 = 0x200 - s3;
				s32 corner3 = 0x200 - s4;
				s32 corner4 = 0x200 + s4;

				dst[0] = src[0];
				dst[1] = src[1];
				dst[2] = src[2];
				dst[3] = src[3];

				dst[0].s = corner3;
				dst[0].t = corner2;
				dst[0].x = (src[(s1 + 0) % 4].x * mult) >> 16;
				dst[0].y = (src[(s1 + 0) % 4].y * mult) >> 16;
				dst[0].z = (src[(s1 + 0) % 4].z * mult) >> 16;

				dst[1].s = corner1;
				dst[1].t = corner3;
				dst[1].x = (src[(s1 + 1) % 4].x * mult) >> 16;
				dst[1].y = (src[(s1 + 1) % 4].y * mult) >> 16;
				dst[1].z = (src[(s1 + 1) % 4].z * mult) >> 16;

				dst[2].s = corner4;
				dst[2].t = corner1;
				dst[2].x = (src[(s1 + 2) % 4].x * mult) >> 16;
				dst[2].y = (src[(s1 + 2) % 4].y * mult) >> 16;
				dst[2].z = (src[(s1 + 2) % 4].z * mult) >> 16;

				dst[3].s = corner2;
				dst[3].t = corner4;
				dst[3].x = (src[(s1 + 3) % 4].x * mult) >> 16;
				dst[3].y = (src[(s1 + 3) % 4].y * mult) >> 16;
				dst[3].z = (src[(s1 + 3) % 4].z * mult) >> 16;

				src += 4;
				dst += 4;
			}
		}
	}
}

void model_select_texture(struct modelrenderdata *renderdata, struct textureconfig *tconfig, s32 arg2)
{
	tex_select(&renderdata->gdl, tconfig, arg2, renderdata->zbufferenabled, 2, 1, NULL);
}

void model_render_node_chr_gunfire(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node)
{
	u32 stack[3];
	f32 negspc0;
	struct modelrodata_chrgunfire *rodata = &node->rodata->chrgunfire;
	union modelrwdata *rwdata = model_get_node_rw_data(model, node);
	Vtx *vertices;
	f32 spf0;
	f32 spec;
	struct coord spe0;
	f32 spdc;
	f32 spd8;
	f32 rot2;
	f32 spd0;
	f32 spcc;
	f32 spc8;
	f32 spc4;
	f32 spc0;
	f32 spbc;
	f32 negspcc;
	f32 negspc8;
	f32 scale;
	Mtxf *mtx;
	f32 tmp;
	struct coord sp9c;
	struct coord sp90;
	Vtx vtxtemplate = {0};
	Col colourtemplate = {0xffffffff};
	struct textureconfig *tconfig;
	Col *colours;
	f32 distance;

	if ((renderdata->flags & MODELRENDERFLAG_XLU) && rwdata->chrgunfire.visible) {
		s32 index = model_find_node_mtx_index(node, 0);
		mtx = &model->matrices[index];

		spe0.x = -(rodata->pos.f[0] * mtx->m[0][0] + rodata->pos.f[1] * mtx->m[1][0] + rodata->pos.f[2] * mtx->m[2][0] + mtx->m[3][0]);
		spe0.y = -(rodata->pos.f[0] * mtx->m[0][1] + rodata->pos.f[1] * mtx->m[1][1] + rodata->pos.f[2] * mtx->m[2][1] + mtx->m[3][1]);
		spe0.z = -(rodata->pos.f[0] * mtx->m[0][2] + rodata->pos.f[1] * mtx->m[1][2] + rodata->pos.f[2] * mtx->m[2][2] + mtx->m[3][2]);

		distance = sqrtf(spe0.f[0] * spe0.f[0] + spe0.f[1] * spe0.f[1] + spe0.f[2] * spe0.f[2]);

		if (distance > 0) {
			f32 tmp = 1 / (model->scale * distance);
			spe0.f[0] *= tmp;
			spe0.f[1] *= tmp;
			spe0.f[2] *= tmp;
		} else {
			spe0.f[0] = 0;
			spe0.f[1] = 0;
			spe0.f[2] = 1 / model->scale;
		}

		spec = acosf(spe0.f[0] * mtx->m[1][0] + spe0.f[1] * mtx->m[1][1] + spe0.f[2] * mtx->m[1][2]);
		spf0 = acosf(-(spe0.f[0] * mtx->m[2][0] + spe0.f[1] * mtx->m[2][1] + spe0.f[2] * mtx->m[2][2]) / sinf(spec));

		tmp = -(spe0.f[0] * mtx->m[0][0] + spe0.f[1] * mtx->m[0][1] + spe0.f[2] * mtx->m[0][2]);

		if (tmp < 0) {
			spf0 = BADDTOR(360) - spf0;
		}

		spdc = cosf(spf0);
		spd8 = sinf(spf0);
		rot2 = cosf(spec);
		spd0 = sinf(spec);

		scale = 0.75f + (random() % 128) * (1.0f / 256.0f); // 0.75 to 1.25

		sp9c.f[0] = rodata->dim.f[0] * scale;
		sp9c.f[1] = rodata->dim.f[1] * scale;
		sp9c.f[2] = rodata->dim.f[2] * scale;

		spcc = sp9c.f[0] * spdc * 0.5f;
		spc8 = sp9c.f[2] * spd8 * 0.5f;
		spc4 = sp9c.f[1] * spd0 * 0.5f;

		spc0 = sp9c.f[0] * rot2 * spd8 * 0.5f;
		spbc = sp9c.f[2] * rot2 * spdc * 0.5f;

		negspcc = -spcc;
		negspc8 = -spc8;
		negspc0 = -spc0;

		sp90.f[0] = rodata->pos.f[0] - sp9c.f[0] * 0.5f;
		sp90.f[1] = rodata->pos.f[1];
		sp90.f[2] = rodata->pos.f[2];

		vertices = g_ModelVtxAllocatorFunc(4);

		colours = (Col *) gfx_allocate_colours(1);

		vertices[0] = vtxtemplate;
		vertices[1] = vtxtemplate;
		vertices[2] = vtxtemplate;
		vertices[3] = vtxtemplate;

		colours[0] = colourtemplate;

		vertices[0].x = sp90.f[0] + negspcc + negspc0;
		vertices[0].y = sp90.f[1] - spc4;
		vertices[0].z = sp90.f[2] - negspc8 + -spbc;
		vertices[1].x = sp90.f[0] + negspcc - negspc0;
		vertices[1].y = sp90.f[1] + spc4;
		vertices[1].z = sp90.f[2] - negspc8 - -spbc;
		vertices[2].x = sp90.f[0] - negspcc - negspc0;
		vertices[2].y = sp90.f[1] + spc4;
		vertices[2].z = sp90.f[2] + negspc8 - -spbc;
		vertices[3].x = sp90.f[0] - negspcc + negspc0;
		vertices[3].y = sp90.f[1] - spc4;
		vertices[3].z = sp90.f[2] + negspc8 + -spbc;

		gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_COL1, osVirtualToPhysical(rodata->baseaddr));

		if (rodata->texture) {
			s32 centre;
			u16 sp62;
			s32 sp5c;
			s32 sp58;

			tconfig = rodata->texture;

			sp62 = (random() * 1024) & 0xffff;
			sp5c = (coss(sp62) * tconfig->width * 0xb5) >> 18;
			sp58 = (sins(sp62) * tconfig->width * 0xb5) >> 18;

			centre = tconfig->width << 4;

			vertices[0].s = centre - sp5c;
			vertices[0].t = centre - sp58;
			vertices[1].s = centre + sp58;
			vertices[1].t = centre - sp5c;
			vertices[2].s = centre + sp5c;
			vertices[2].t = centre + sp58;
			vertices[3].s = centre - sp58;
			vertices[3].t = centre + sp5c;

			model_select_texture(renderdata, tconfig, 4);
		} else {
			model_select_texture(renderdata, NULL, 1);
		}

		gSPSetGeometryMode(renderdata->gdl++, G_CULL_BACK);
		gSPMatrix(renderdata->gdl++, osVirtualToPhysical(mtx), G_MTX_NOPUSH | G_MTX_LOAD | G_MTX_MODELVIEW);
		gSPColor(renderdata->gdl++, osVirtualToPhysical(colours), 1);
		gSPVertex(renderdata->gdl++, osVirtualToPhysical(vertices), 4, 0);
		gSPTri2(renderdata->gdl++, 0, 1, 2, 2, 3, 0);
	}
}

void model_render(struct modelrenderdata *renderdata, struct model *model)
{
	union modelrodata *rodata;
	union modelrwdata *rwdata;
	u32 type;
	struct modelnode *node = model->definition->rootnode;

	gSPSegment(renderdata->gdl++, SPSEGMENT_MODEL_MTX, osVirtualToPhysical(model->matrices));

	while (node) {
		type = node->type & 0xff;

		switch (type) {
		case MODELNODETYPE_DISTANCE:
		case MODELNODETYPE_TOGGLE:
			rodata = node->rodata;
			rwdata = model_get_node_rw_data(model, node);

			switch (type) {
			case MODELNODETYPE_DISTANCE:
				node->child = rwdata->distance.visible ? rodata->distance.target : NULL;
				break;
			case MODELNODETYPE_TOGGLE:
				node->child = rwdata->toggle.visible ? rodata->toggle.target : NULL;
				break;
			}
			break;
		case MODELNODETYPE_HEADSPOT:
			rwdata = model_get_node_rw_data(model, node);

			if (rwdata->headspot.headmodeldef) {
				struct modelnode *loopnode = rwdata->headspot.headmodeldef->rootnode;
				node->child = loopnode;

				while (loopnode) {
					loopnode->parent = node;
					loopnode = loopnode->next;
				}
			}
			break;
		case MODELNODETYPE_REORDER:
			model_apply_reorder_relations(model, node);
			break;
		case MODELNODETYPE_CHRGUNFIRE:
			model_render_node_chr_gunfire(renderdata, model, node);
			break;
		case MODELNODETYPE_GUNDL:
			model_render_node_gundl(renderdata, model, node);
			break;
		case MODELNODETYPE_DL:
			model_render_node_dl(renderdata, model, node);
			break;
		case MODELNODETYPE_STARGUNFIRE:
			model_render_node_star_gunfire(renderdata, node);
			break;
		case MODELNODETYPE_CHRINFO:
		default:
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}
}

bool model_test_bbox_node_for_hit(struct modelrodata_bbox *bbox, Mtxf *mtx, struct coord *arg2, struct coord *arg3)
{
	f32 xthingx;
	f32 xthingy;
	f32 xthingz;
	u32 stack1[3];
	f32 xsum1;
	f32 xsum2;
	f32 xsum3;

	f32 f0;
	u32 stack;

	f32 ythingx;
	f32 ythingy;
	f32 ythingz;
	u32 stack2[3];
	f32 ysum1;
	f32 ysum2;
	f32 ysum3;

	f32 mult1;
	f32 mult2;

	f32 bestsum2;
	f32 bestsum1;
	f32 anotherbestsum3;
	f32 anotherbestsum1;

	f32 xmin = bbox->xmin;
	f32 xmax = bbox->xmax;
	f32 ymin = bbox->ymin;
	f32 ymax = bbox->ymax;
	f32 zmin = bbox->zmin;
	f32 zmax = bbox->zmax;

	f32 mult3;
	f32 mult4;

	f32 zthingx;
	f32 zthingy;
	f32 zthingz;
	u32 stack3[3];
	f32 zsum1;
	f32 zsum2;
	f32 zsum3;

	if (var8005efc0 != 0.0f) {
		xmin -= var8005efc0;
		xmax += var8005efc0;
		ymin -= var8005efc0;
		ymax += var8005efc0;
		zmin -= var8005efc0;
		zmax += var8005efc0;
	}

	// x
	xthingx = mtx->m[0][0] * mtx->m[0][0];
	xthingy = mtx->m[0][1] * mtx->m[0][1];
	xthingz = mtx->m[0][2] * mtx->m[0][2];

	xsum1 = mtx->m[0][0] * arg3->f[0] + mtx->m[0][1] * arg3->f[1] + mtx->m[0][2] * arg3->f[2];
	xsum2 = mtx->m[0][0] * (arg2->f[0] - mtx->m[3][0]) + mtx->m[0][1] * (arg2->f[1] - mtx->m[3][1]) + mtx->m[0][2] * (arg2->f[2] - mtx->m[3][2]);

	f0 = -(xthingx + xthingy + xthingz) * xmax;
	xsum3 = -(xsum2 + f0);
	f0 = -(xthingx + xthingy + xthingz) * xmin;
	xsum2 = -(xsum2 + f0);

	if (xsum1 < 0.0f) {
		xsum1 = -xsum1;
		xsum2 = -xsum2;
		xsum3 = -xsum3;
	}

	if (xsum2 < 0.0f && xsum3 < 0.0f) {
		return false;
	}

	if (xsum3 < xsum2) {
		f32 tmp = xsum2;
		xsum2 = xsum3;
		xsum3 = tmp;
	}

	// y
	ythingx = mtx->m[1][0] * mtx->m[1][0];
	ythingy = mtx->m[1][1] * mtx->m[1][1];
	ythingz = mtx->m[1][2] * mtx->m[1][2];

	ysum1 = mtx->m[1][0] * arg3->f[0] + mtx->m[1][1] * arg3->f[1] + mtx->m[1][2] * arg3->f[2];
	ysum2 = mtx->m[1][0] * (arg2->f[0] - mtx->m[3][0]) + mtx->m[1][1] * (arg2->f[1] - mtx->m[3][1]) + mtx->m[1][2] * (arg2->f[2] - mtx->m[3][2]);

	f0 = -(ythingx + ythingy + ythingz) * ymax;
	ysum3 = -(ysum2 + f0);
	f0 = -(ythingx + ythingy + ythingz) * ymin;
	ysum2 = -(ysum2 + f0);

	if (ysum1 < 0.0f) {
		ysum1 = -ysum1;
		ysum2 = -ysum2;
		ysum3 = -ysum3;
	}

	if (ysum2 < 0.0f && ysum3 < 0.0f) {
		return false;
	}

	if (ysum3 < ysum2) {
		f32 tmp = ysum2;
		ysum2 = ysum3;
		ysum3 = tmp;
	}

	// Do x and y comparison things
	mult1 = ysum2 * xsum1;
	mult2 = xsum2 * ysum1;
	mult3 = xsum3 * ysum1;
	mult4 = ysum3 * xsum1;

	if (mult1 < mult2) {
		if (mult4 < mult2) {
			return false;
		}

		bestsum2 = xsum2;
		bestsum1 = xsum1;
	} else {
		if (mult3 < mult1) {
			return false;
		}

		bestsum2 = ysum2;
		bestsum1 = ysum1;
	}

	if (mult3 < mult4) {
		anotherbestsum3 = xsum3;
		anotherbestsum1 = xsum1;
	} else {
		anotherbestsum3 = ysum3;
		anotherbestsum1 = ysum1;
	}

	// z
	zthingx = mtx->m[2][0] * mtx->m[2][0];
	zthingy = mtx->m[2][1] * mtx->m[2][1];
	zthingz = mtx->m[2][2] * mtx->m[2][2];

	zsum1 = mtx->m[2][0] * arg3->f[0] + mtx->m[2][1] * arg3->f[1] + mtx->m[2][2] * arg3->f[2];
	zsum2 = mtx->m[2][0] * (arg2->f[0] - mtx->m[3][0]) + mtx->m[2][1] * (arg2->f[1] - mtx->m[3][1]) + mtx->m[2][2] * (arg2->f[2] - mtx->m[3][2]);

	f0 = -(zthingx + zthingy + zthingz) * zmax;
	zsum3 = -(zsum2 + f0);
	f0 = -(zthingx + zthingy + zthingz) * zmin;
	zsum2 = -(zsum2 + f0);

	if (zsum1 < 0.0f) {
		zsum1 = -zsum1;
		zsum2 = -zsum2;
		zsum3 = -zsum3;
	}

	if (zsum2 < 0.0f && zsum3 < 0.0f) {
		return false;
	}

	if (zsum3 < zsum2) {
		f32 tmp = zsum2;
		zsum2 = zsum3;
		zsum3 = tmp;
	}

	// Do z comparison things with the result of the x/y comparison thing
	if (bestsum2 * zsum1 < zsum2 * bestsum1) {
		if (anotherbestsum3 * zsum1 < zsum2 * anotherbestsum1) {
			return false;
		}
	} else {
		if (zsum3 * bestsum1 < bestsum2 * zsum1) {
			return false;
		}
	}

	return true;
}

/**
 * Note that this only checks bbox nodes.
 * This is okay for most objects as well as shielded chrs.
 * For non-shielded chrs, an accurate polygon test is done elsewhere.
 */
s32 model_test_for_hit(struct model *model, struct coord *arg1, struct coord *arg2, struct modelnode **startnode)
{
	struct modelnode *node;
	bool dochildren = true;
	Mtxf *mtx;
	union modelrodata *rodata;
	union modelrwdata *rwdata;
	u32 type;

	if (model);

	if (*startnode) {
		node = *startnode;
		*startnode = NULL;
	} else {
		node = model->definition->rootnode;
	}

	while (node) {
		if (dochildren && node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}

			if (!node) {
				break;
			}
		}

		dochildren = true;
		type = node->type & 0xff;

		switch (type) {
		case MODELNODETYPE_BBOX:
			rodata = node->rodata;
			mtx = model_find_node_mtx(model, node, 0);

			if (model_test_bbox_node_for_hit(&rodata->bbox, mtx, arg1, arg2)) {
				*startnode = node;
				return rodata->bbox.hitpart;
			}

			dochildren = false;
			break;
		case MODELNODETYPE_DISTANCE:
			rodata = node->rodata;
			rwdata = model_get_node_rw_data(model, node);
			node->child = rwdata->distance.visible ? rodata->distance.target : NULL;
			break;
		case MODELNODETYPE_TOGGLE:
			rodata = node->rodata;
			rwdata = model_get_node_rw_data(model, node);
			node->child = rwdata->toggle.visible ? rodata->toggle.target : NULL;
			break;
		case MODELNODETYPE_HEADSPOT:
			rwdata = model_get_node_rw_data(model, node);

			if (rwdata->headspot.headmodeldef) {
				struct modelnode *loopnode = rwdata->headspot.headmodeldef->rootnode;

				node->child = loopnode;

				while (loopnode) {
					loopnode->parent = node;
					loopnode = loopnode->next;
				}
			}
			break;
		case MODELNODETYPE_CHRINFO:
		case MODELNODETYPE_DL:
		default:
			break;
		}
	}

	return 0;
}

#define PROMOTE(var) \
	if (var) \
		var = (void *)((uintptr_t)var + diff)

void model_promote_node_offsets_to_pointers(struct modelnode *node, u32 vma, u32 fileramaddr)
{
	union modelrodata *rodata;
	s32 diff = fileramaddr - vma;

	while (node) {
		u32 type = node->type & 0xff;

		PROMOTE(node->rodata);
		PROMOTE(node->parent);
		PROMOTE(node->next);
		PROMOTE(node->prev);
		PROMOTE(node->child);

		switch (type) {
		case MODELNODETYPE_CHRINFO:
			break;
		case MODELNODETYPE_POSITION:
			break;
		case MODELNODETYPE_GUNDL:
			rodata = node->rodata;
			PROMOTE(rodata->gundl.vertices);
			rodata->gundl.baseaddr = (void *)fileramaddr;
			break;
		case MODELNODETYPE_DL:
			rodata = node->rodata;
			PROMOTE(rodata->dl.vertices);
			rodata->dl.colours = (void *)fileramaddr;
			break;
		case MODELNODETYPE_DISTANCE:
			rodata = node->rodata;
			PROMOTE(rodata->distance.target);
			node->child = rodata->distance.target;
			break;
		case MODELNODETYPE_TOGGLE:
			rodata = node->rodata;
			PROMOTE(rodata->toggle.target);
			break;
		case MODELNODETYPE_REORDER:
			rodata = node->rodata;
			PROMOTE(rodata->reorder.unk18);
			PROMOTE(rodata->reorder.unk1c);
			break;
		case MODELNODETYPE_11:
			rodata = node->rodata;
			PROMOTE(rodata->type11.unk14);
			break;
		case MODELNODETYPE_0B:
			rodata = node->rodata;
			PROMOTE(rodata->type0b.unk3c);
			rodata->type0b.baseaddr = (void *)fileramaddr;
			break;
		case MODELNODETYPE_CHRGUNFIRE:
			rodata = node->rodata;
			PROMOTE(rodata->chrgunfire.texture);
			rodata->chrgunfire.baseaddr = (void *)fileramaddr;
			break;
		case MODELNODETYPE_0D:
			rodata = node->rodata;
			PROMOTE(rodata->type0d.unk10);
			PROMOTE(rodata->type0d.unk14);
			rodata->type0d.baseaddr = (void *)fileramaddr;
			break;
		case MODELNODETYPE_STARGUNFIRE:
			rodata = node->rodata;
			PROMOTE(rodata->stargunfire.vertices);
			rodata->stargunfire.baseaddr = (void *)fileramaddr;
			break;
		default:
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}
}

/**
 * Convert a model file's file-relative offsets to global pointers,
 * and sort the part numbers list so they can be looked up using bisection.
 *
 * Offsets in model files are based from virtual memory address 0x0f000000.
 * This vma address is specified as an argument to the function.
 */
void model_promote_offsets_to_pointers(struct modeldef *modeldef, u32 vma, uintptr_t fileramaddr)
{
	s32 diff = fileramaddr - vma;
	s32 i;
	s16 *partnums;

	PROMOTE(modeldef->rootnode);
	PROMOTE(modeldef->parts);
	PROMOTE(modeldef->texconfigs);

	for (i = 0; i < modeldef->numparts; i++) {
		PROMOTE(modeldef->parts[i]);
	}

	model_promote_node_offsets_to_pointers(modeldef->rootnode, vma, fileramaddr);

	// Sort parts by part number so they can be bisected during lookup
	partnums = (s16 *)&modeldef->parts[modeldef->numparts];

	if (modeldef->numparts) {
		struct modelnode *tmpnode;
		s16 tmpnum;
		bool changed;

		do {
			changed = false;

			for (i = 0; i < modeldef->numparts - 1; i++) {
				if (partnums[i] > partnums[i + 1]) {
					tmpnum = partnums[i];
					partnums[i] = partnums[i + 1];
					partnums[i + 1] = tmpnum;

					tmpnode = modeldef->parts[i];
					modeldef->parts[i] = modeldef->parts[i + 1];
					modeldef->parts[i + 1] = tmpnode;

					changed = true;
				}
			}
		} while (changed == true);
	}
}

s32 model_calculate_rw_data_indexes(struct modelnode *basenode)
{
	u16 len = 0;
	struct modelnode *node = basenode;
	union modelrodata *rodata;

	while (node) {
		u32 type = node->type & 0xff;

		switch (type) {
		case MODELNODETYPE_CHRINFO:
			rodata = node->rodata;
			rodata->chrinfo.rwdataindex = len;
			len += sizeof(struct modelrwdata_chrinfo) / 4;
			break;
		case MODELNODETYPE_DISTANCE:
			rodata = node->rodata;
			rodata->distance.rwdataindex = len;
			len += sizeof(struct modelrwdata_distance) / 4;
			node->child = rodata->distance.target;
			break;
		case MODELNODETYPE_TOGGLE:
			rodata = node->rodata;
			rodata->toggle.rwdataindex = len;
			len += sizeof(struct modelrwdata_toggle) / 4;
			node->child = rodata->toggle.target;
			break;
		case MODELNODETYPE_HEADSPOT:
			rodata = node->rodata;
			rodata->headspot.rwdataindex = len;
			len += sizeof(struct modelrwdata_headspot) / 4;
			node->child = NULL;
			break;
		case MODELNODETYPE_REORDER:
			rodata = node->rodata;
			rodata->reorder.rwdataindex = len;
			len += sizeof(struct modelrwdata_reorder) / 4;
			model_apply_reorder_relations_by_arg(node, false);
			break;
		case MODELNODETYPE_0B:
			rodata = node->rodata;
			rodata->type0b.rwdataindex = len;
			len += sizeof(struct modelrwdata_0b) / 4;
			break;
		case MODELNODETYPE_CHRGUNFIRE:
			rodata = node->rodata;
			rodata->chrgunfire.rwdataindex = len;
			len += sizeof(struct modelrwdata_chrgunfire) / 4;
			break;
		case MODELNODETYPE_DL:
			rodata = node->rodata;
			rodata->dl.rwdataindex = len;
			len += sizeof(struct modelrwdata_dl) / 4;
			break;
		default:
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node == basenode->parent) {
					node = NULL;
					break;
				}

				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}

	return len;
}

void model_allocate_rw_data(struct modeldef *modeldef)
{
	modeldef->rwdatalen = model_calculate_rw_data_indexes(modeldef->rootnode);
}

void model_init_rw_data(struct model *model, struct modelnode *startnode)
{
	struct modelnode *node = startnode;
	union modelrodata *rodata;
	union modelrwdata *rwdata;

	while (node) {
		u32 type = node->type & 0xff;

		switch (type) {
		case MODELNODETYPE_CHRINFO:
			rwdata = model_get_node_rw_data(model, node);

			rwdata->chrinfo.unk00 = 0;
			rwdata->chrinfo.ground = 0;
			rwdata->chrinfo.pos.x = 0;
			rwdata->chrinfo.pos.y = 0;
			rwdata->chrinfo.pos.z = 0;
			rwdata->chrinfo.yrot = 0;
			rwdata->chrinfo.unk18 = 0;
			rwdata->chrinfo.unk1c = 0;

			rwdata->chrinfo.unk01 = 0;
			rwdata->chrinfo.unk34.x = 0;
			rwdata->chrinfo.unk34.y = 0;
			rwdata->chrinfo.unk34.z = 0;
			rwdata->chrinfo.unk30 = 0;
			rwdata->chrinfo.unk24.x = 0;
			rwdata->chrinfo.unk24.y = 0;
			rwdata->chrinfo.unk24.z = 0;
			rwdata->chrinfo.unk20 = 0;

			rwdata->chrinfo.unk02 = 0;
			rwdata->chrinfo.unk4c.x = 0;
			rwdata->chrinfo.unk4c.y = 0;
			rwdata->chrinfo.unk4c.z = 0;
			rwdata->chrinfo.unk40.x = 0;
			rwdata->chrinfo.unk40.y = 0;
			rwdata->chrinfo.unk40.z = 0;
			rwdata->chrinfo.unk5c = 0;
			break;
		case MODELNODETYPE_DISTANCE:
			rodata = node->rodata;
			rwdata = model_get_node_rw_data(model, node);
			rwdata->distance.visible = false;
			node->child = rodata->distance.target;
			break;
		case MODELNODETYPE_TOGGLE:
			rodata = node->rodata;
			rwdata = model_get_node_rw_data(model, node);
			rwdata->toggle.visible = true;
			node->child = rodata->toggle.target;
			break;
		case MODELNODETYPE_HEADSPOT:
			rwdata = model_get_node_rw_data(model, node);
			rwdata->headspot.headmodeldef = NULL;
			rwdata->headspot.rwdatas = NULL;
			break;
		case MODELNODETYPE_REORDER:
			rwdata = model_get_node_rw_data(model, node);
			rwdata->reorder.reverse = false;
			model_apply_reorder_relations(model, node);
			break;
		case MODELNODETYPE_0B:
			rwdata = model_get_node_rw_data(model, node);
			rwdata->type0b.unk00 = 0;
			break;
		case MODELNODETYPE_CHRGUNFIRE:
			rwdata = model_get_node_rw_data(model, node);
			rwdata->chrgunfire.visible = false;
			break;
		case MODELNODETYPE_DL:
			rodata = node->rodata;
			rwdata = model_get_node_rw_data(model, node);
			rwdata->dl.vertices = rodata->dl.vertices;
			rwdata->dl.gdl = rodata->dl.opagdl;
			rwdata->dl.colours = (void *) ALIGN8((uintptr_t)(rodata->dl.vertices + rodata->dl.numvertices));
			if (rodata->dl.numvertices);
			if (rodata->dl.numvertices);
			if (rodata->dl.numvertices);
			break;
		default:
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node == startnode->parent) {
					node = NULL;
					break;
				}

				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}
}

void model_init(struct model *model, struct modeldef *modeldef, u32 *rwdatas, bool resetanim)
{
	struct modelnode *node;

	model->unk00 = 0;
	model->definition = modeldef;
	model->rwdatas = rwdatas;
	model->rwdatalen = -1;
	model->scale = 1;
	model->attachedtomodel = NULL;
	model->attachedtonode = NULL;

	node = modeldef->rootnode;

	while (node) {
		u32 type = node->type & 0xff;

		if (type == MODELNODETYPE_HEADSPOT) {
			model->unk00 |= 1;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node == modeldef->rootnode->parent) {
					node = NULL;
					break;
				}

				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}

	if (rwdatas != NULL) {
		model_init_rw_data(model, modeldef->rootnode);
	}

	if (resetanim) {
		model->anim = NULL;
	}
}

void anim_init(struct anim *anim)
{
	anim->animnum = 0;
	anim->animnum2 = 0;
	anim->looping = 0;
	anim->flipfunc = NULL;
	anim->unk6c = 0;
	anim->unk70 = NULL;
	anim->average = 0;
	anim->frac = 0;
	anim->timespeed = 0;
	anim->frac2 = 0;
	anim->timespeed2 = 0;
	anim->fracmerge = 0;
	anim->timemerge = 0;
	anim->timeplay = 0;
	anim->endframe = -1;
	anim->endframe2 = -1;
	anim->speed = 1;
	anim->speed2 = 1;
	anim->playspeed = 1;
	anim->animscale = 1;
}

void model_attach_head(struct model *bodymode, struct modeldef *bodymodeldef, struct modelnode *headspotnode, struct modeldef *headmodeldef)
{
	struct modelrwdata_headspot *rwdata = model_get_node_rw_data(bodymode, headspotnode);
	struct modelnode *node;

	rwdata->headmodeldef = headmodeldef;
	rwdata->rwdatas = &bodymode->rwdatas[bodymodeldef->rwdatalen];

	headspotnode->child = headmodeldef->rootnode;

	node = headspotnode->child;

	while (node) {
		node->parent = headspotnode;
		node = node->next;
	}

	bodymodeldef->rwdatalen += model_calculate_rw_data_indexes(headspotnode->child);
}

/**
 * This function can be called repeatedly to iterate a model's display lists.
 *
 * On the first call, the value passed as nodeptr should point to a NULL value.
 * Each time the function is called, it will update *gdlptr to point to the next
 * display list, and will update *nodeptr to point to the current node. On
 * subsequent calls, the same values should be passed as nodeptr and gdlptr so
 * the function can continue correctly.
 *
 * Note that some node types support multiple display lists, so the function
 * may return the same node while it iterates the display lists for that node.
 */
void model_iterate_display_lists(struct modeldef *modeldef, struct modelnode **nodeptr, Gfx **gdlptr)
{
	struct modelnode *node = *nodeptr;
	union modelrodata *rodata;
	Gfx *gdl = NULL;

	if (node == NULL) {
		node = modeldef->rootnode;
	}

	while (node) {
		u32 type = node->type & 0xff;

		switch (type) {
		case MODELNODETYPE_GUNDL:
			rodata = node->rodata;

			if (node != *nodeptr) {
				gdl = rodata->gundl.opagdl;
			} else if (rodata->gundl.xlugdl != *gdlptr) {
				gdl = rodata->gundl.xlugdl;
			}
			break;
		case MODELNODETYPE_DL:
			rodata = node->rodata;

			if (node != *nodeptr) {
				gdl = rodata->dl.opagdl;
			} else if (rodata->dl.xlugdl != *gdlptr) {
				gdl = rodata->dl.xlugdl;
			}
			break;
		case MODELNODETYPE_STARGUNFIRE:
			rodata = node->rodata;

			if (node != *nodeptr) {
				gdl = rodata->stargunfire.gdl;
			}
			break;
		case MODELNODETYPE_DISTANCE:
			rodata = node->rodata;
			node->child = rodata->distance.target;
			break;
		case MODELNODETYPE_TOGGLE:
			rodata = node->rodata;
			node->child = rodata->toggle.target;
			break;
		case MODELNODETYPE_REORDER:
			model_apply_reorder_relations_by_arg(node, true);
			break;
		}

		if (gdl) {
			break;
		}

		if (node->child) {
			node = node->child;
		} else {
			while (node) {
				if (node->next) {
					node = node->next;
					break;
				}

				node = node->parent;
			}
		}
	}

	*gdlptr = gdl;
	*nodeptr = node;
}

void model_node_replace_gdl(struct modeldef *modeldef, struct modelnode *node, Gfx *find, Gfx *replacement)
{
	union modelrodata *rodata;
	u32 type = node->type & 0xff;

	switch (type) {
	case MODELNODETYPE_GUNDL:
		rodata = node->rodata;

		if (rodata->gundl.opagdl == find) {
			rodata->gundl.opagdl = replacement;
			return;
		}

		if (rodata->gundl.xlugdl == find) {
			rodata->gundl.xlugdl = replacement;
			return;
		}
		break;
	case MODELNODETYPE_DL:
		rodata = node->rodata;

		if (rodata->dl.opagdl == find) {
			rodata->dl.opagdl = replacement;
			return;
		}

		if (rodata->dl.xlugdl == find) {
			rodata->dl.xlugdl = replacement;
			return;
		}
		break;
	case MODELNODETYPE_STARGUNFIRE:
		rodata = node->rodata;

		if (rodata->stargunfire.gdl == find) {
			rodata->stargunfire.gdl = replacement;
			return;
		}
		break;
	}
}
