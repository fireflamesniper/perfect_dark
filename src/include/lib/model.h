#ifndef _IN_LIB_MODEL_H
#define _IN_LIB_MODEL_H
#include <ultra64.h>
#include "data.h"
#include "types.h"

extern bool (*g_ModelShouldRenderGunDlCallback)(struct model *model, struct modelnode *node);
extern Vtx *(*g_ModelVtxAllocatorFunc)(s32 numvertices);
extern void (*g_ModelJointPositionedFunc)(s32 mtxindex, Mtxf *mtx);

bool modelasm00018680(struct modelrenderdata *renderdata, struct model *model);
void model_set_distance_scale(f32 value);
void model_set_vtx_allocator_func(Vtx *(*fn)(s32 numvertices));
s32 model_find_node_mtx_index(struct modelnode *node, s32 arg1);
Mtxf *model_find_node_mtx(struct model *model, struct modelnode *node, s32 arg2);
Mtxf *model_get_root_mtx(struct model *model);
struct modelnode *model_find_node_by_mtx_index(struct model *model, s32 mtxindex);
struct modelnode *model_node_find_mtx_node(struct modelnode *node);
struct modelnode *model_node_find_parent_mtx_node(struct modelnode *node);
struct modelnode *model_node_find_child_mtx_node(struct modelnode *node);
struct modelnode *model_node_find_child_or_parent_mtx_node(struct modelnode *node);
struct modelnode *model_get_part(struct modeldef *arg0, s32 partnum);
void *model_get_part_rodata(struct modeldef *modeldef, s32 partnum);
f32 model_get_screen_distance(struct model *model);
void *model_get_node_rw_data(struct model *model, struct modelnode *node);
void model_node_get_position(struct model *model, struct modelnode *node, struct coord *pos);
void model_get_root_position(struct model *model, struct coord *pos);
void model_set_root_position(struct model *model, struct coord *pos);
void model_node_get_model_relative_position(struct model *model, struct modelnode *node, struct coord *pos);
f32 model_get_chr_rot_y(struct model *model);
void model_set_chr_rot_y(struct model *model, f32 angle);
void model_set_scale(struct model *model, f32 scale);
void model_set_anim_scale(struct model *model, f32 scale);
f32 model_get_effective_scale(struct model *model);
f32 model_tween_rot_axis(f32 arg0, f32 angle, f32 frac);
void model_update_info(struct model *model);
void model_apply_distance_relations(struct model *model, struct modelnode *node);
void model_apply_toggle_relations(struct model *model, struct modelnode *node);
void model_apply_head_relations(struct model *model, struct modelnode *node);
void model_update_relations_quick(struct model *model, struct modelnode *node);
void model_update_relations(struct model *model);
void model_set_matrices(struct modelrenderdata *arg0, struct model *model);
void model_set_matrices_with_anim(struct modelrenderdata *renderdata, struct model *model);
s16 model_get_anim_num(struct model *model);
bool model_is_flipped(struct model *model);
f32 model_get_cur_anim_frame(struct model *model);
f32 model_get_anim_end_frame(struct model *model);
s32 model_get_num_anim_frames(struct model *model);
f32 model_get_anim_speed(struct model *model);
f32 model_get_abs_anim_speed(struct model *model);
bool model_is_anim_merging(struct model *model);
void model_set_animation_with_merge(struct model *model, s16 animnum, u32 flip, f32 startframe, f32 speed, f32 timemerge, bool domerge);
void model_set_animation(struct model *model, s16 animnum, s32 flip, f32 fstartframe, f32 speed, f32 merge);
void model_copy_anim_data(struct model *src, struct model *dst);
void model_set_anim_looping(struct model *model, f32 loopframe, f32 loopmerge);
void model_set_anim_end_frame(struct model *model, f32 endframe);
void model_set_anim_flip_function(struct model *model, void *callback);
void model_set_anim_speed(struct model *model, f32 speed, f32 startframe);
void model_set_anim_speed_auto(struct model *model, f32 arg1, f32 startframe);
void model_set_anim_play_speed(struct model *model, f32 speed, f32 frame);
void model_set_anim70(struct model *model, void *callback);
void model_set_anim_frame(struct model *model, f32 startframe);
void model_set_anim_merging_enabled(bool value);
bool model_is_anim_merging_enabled(void);
void model_tick_anim_quarter_speed(struct model *model, s32 lvupdate240, bool arg2);
void model_tick_anim(struct model *model, s32 lvupdate240, bool arg2);
void model_render_node_dl(struct modelrenderdata *renderdata, struct model *model, struct modelnode *node);
void model_render(struct modelrenderdata *renderdata, struct model *model);
bool model_test_bbox_node_for_hit(struct modelrodata_bbox *bbox, Mtxf *mtx, struct coord *arg2, struct coord *arg3);
s32 model_test_for_hit(struct model *model, struct coord *arg1, struct coord *arg2, struct modelnode **startnode);
void model_promote_offsets_to_pointers(struct modeldef *modeldef, u32 arg1, uintptr_t arg2);
void model_allocate_rw_data(struct modeldef *modeldef);
void model_init_rw_data(struct model *model, struct modelnode *node);
void model_init(struct model *model, struct modeldef *modeldef, u32 *rwdatas, bool resetanim);
void anim_init(struct anim *anim);
void model_attach_head(struct model *model, struct modeldef *arg1, struct modelnode *node, struct modeldef *arg3);
void model_iterate_display_lists(struct modeldef *modeldef, struct modelnode **nodeptr, Gfx **gdlptr);
void model_node_replace_gdl(struct modeldef *modeldef, struct modelnode *node, Gfx *find, Gfx *replacement);

#endif
