#ifndef _IN_GAME_CHR_CHRACTION_H
#define _IN_GAME_CHR_CHRACTION_H
#include <ultra64.h>
#include "data.h"
#include "types.h"

bool chr_gopos_is_waiting(struct chrdata *chr);
bool weapon_is_onehanded(struct prop *prop);
f32 chr_get_attack_entity_distance(struct chrdata *chr, u32 attackflags, s32 entityid);
void chr_calculate_push_contact_pos(struct coord *a, struct coord *b, struct coord *c, struct coord *d, struct coord *dst);
void chr_calculate_push_contact_pos_using_saved_edge(struct coord *arg0, struct coord *arg1, struct coord *dst);
void chr_stand(struct chrdata *chr);
bool chr_face_cover(struct chrdata *chr);
void chr_begin_death(struct chrdata *chr, struct coord *dir, f32 relangle, s32 hitpart, struct gset *gset, bool knockout, s32 aplayernum);
void chr_yeet_from_pos(struct chrdata *chr, struct coord *pos, f32 arg2);
void chr_set_shield(struct chrdata *chr, f32 shield);
bool chr_try_create_shieldhit(struct chrdata *chr, struct modelnode *node, struct prop *prop, struct model *model, s32 side, s16 *arg5);
void chr_damage_by_dizziness(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop);
void chr_damage_by_laser(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop);
void chr_damage_by_impact(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop, s32 hitpart, struct prop *prop2, struct modelnode *node, struct model *model, s32 side, s16 *hitpos);
void chr_damage_by_general(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop, s32 arg5);
void chr_damage_by_explosion(struct chrdata *chr, f32 damage, struct coord *vector, struct prop *prop, struct coord *explosionpos);
void chr_calculate_position(struct chrdata *chr, struct coord *pos);
bool chr_go_to_room_pos(struct chrdata *chr, struct coord *pos, RoomNum *rooms, u32 goposflags);
bool chr_has_los_to_attack_target(struct chrdata *chr, struct coord *pos, RoomNum *rooms, bool allowextraheight);
bool chr_has_los_to_chr(struct chrdata *chr, struct chrdata *target, RoomNum *room);
bool chr_has_los_to_pos(struct chrdata *viewerchr, struct coord *pos, RoomNum *rooms);
bool chr_has_los_to_prop(struct chrdata *chr, struct prop *prop);
bool chr_is_stopped(struct chrdata *chr);
bool chr_check_can_see_target(struct chrdata *chr);
bool chr_try_sidestep(struct chrdata *chr);
bool chr_try_jumpout(struct chrdata *chr);
bool chr_try_runsideways(struct chrdata *chr);
bool chr_try_attackwalk(struct chrdata *chr);
bool chr_try_attackrun(struct chrdata *chr);
bool chr_try_attackroll(struct chrdata *chr);
bool chr_try_attackamount(struct chrdata *chr, u32 arg1, u32 arg2, u8 arg3, u8 arg4);
bool chr_go_to_pad(struct chrdata *chr, s32 padnum, u32 goposflags);
s32 chr_go_to_cover_opposite_runfrompos(struct chrdata *chr, f32 arg1, u8 arg2);
bool chr_try_run_from_target(struct chrdata *chr);
bool chr_go_to_cover_prop(struct chrdata *chr);
bool chr_drop_item(struct chrdata *chr, u32 modelnum, u32 weaponnum);
void chr_punch_inflict_damage(struct chrdata *chr, s32 damage, s32 range, u8 reverse);
bool chr_try_punch(struct chrdata *chr, u8 reverse);
struct eyespy *chr_to_eyespy(struct chrdata *chr);
void chr_update_fireslot(struct chrdata *chr, s32 handnum, bool withsound, bool withbeam, struct coord *from, struct coord *to);
f32 chr_get_theta(struct chrdata *chr);
f32 chr_get_aimx_angle(struct chrdata *chr);
f32 chr_get_aimy_angle(struct chrdata *chr);
bool chr_calculate_aimend(struct chrdata *chr, struct attackanimconfig *animcfg, bool hasleftgun, bool hasrightgun, f32 arg4);
void chr_calculate_aimend_vertical(struct chrdata *chr, struct attackanimconfig *animcfg, bool hasleftgun, bool hasrightgun, f32 shootrotx);
f32 chr_get_aim_limit_angle(f32 sqdist);
bool chr_get_gun_pos(struct chrdata *chr, s32 handnum, struct coord *gunpos);
void chr_calculate_shield_hit(struct chrdata *chr, struct coord *pos, struct coord *vector, struct modelnode **node, s32 *hitpart, struct model **model, s32 *side);
void chr_calculate_trajectory(struct coord *frompos, f32 arg1, struct coord *aimpos, struct coord *arg3);
void chr_tick_shots(struct chrdata *chr);
bool pos_is_moving_towards_pos_or_stopped_in_range(struct coord *prevpos, struct coord *moveddelta, struct coord *targetpos, f32 range);
bool pos_is_arriving_laterally_at_pos(struct coord *prevpos, struct coord *curpos, struct coord *targetpos, f32 range);
void chra_tick(struct chrdata *chr);
void cutscene_start(u32 ailistid);
void chra_tick_bg(void);
f32 chr_get_angle_to_pos(struct chrdata *chr, struct coord *pos);
bool chr_is_in_targets_fovx(struct chrdata *chr, u8 fov360);
bool chr_is_vertical_angle_to_target_within(struct chrdata *chr, u8 arg1);
bool chr_is_target_in_fov(struct chrdata *chr, u8 arg1, u8 reverse);
bool chr_is_looking_at_pos(struct chrdata *chr, struct coord *pos, u8 arg2);
f32 chr_get_same_floor_distance_to_pad(struct chrdata *chr, s32 pad_id);
void chrs_clear_refs_to_player(s32 playernum);
f32 chr_get_timer(struct chrdata *chr);
bool chr_is_target_aiming_at_me(struct chrdata *chr);
bool chr_saw_injury(struct chrdata *chr, u8 arg1);
bool chr_saw_death(struct chrdata *chr, u8 arg1);
bool chr_set_pad_preset_to_waypoint_within_quadrant(struct chrdata *chr, u8 quadrant);
bool chr_set_pad_preset_to_waypoint_within_target_quadrant(struct chrdata *chr, u8 arg1);
bool chr_set_chr_preset_to_chr_near_pad(u32 checktype, struct chrdata *chr, f32 distance, s32 padnum);
bool chr_set_pad_preset_to_pad_on_route_to_target(struct chrdata *chr);

#if VERSION >= VERSION_NTSC_1_0
bool chr_adjust_pos_for_spawn(f32 chrradius, struct coord *pos, RoomNum *rooms, f32 angle, bool allowonscreen, bool force, bool onlysurrounding);
#else
bool chr_adjust_pos_for_spawn(f32 chrradius, struct coord *pos, RoomNum *rooms, f32 angle, bool allowonscreen, bool force);
#endif

struct prop *chr_spawn_at_coord(s32 body, s32 head, struct coord *pos, RoomNum *room, f32 angle, u8 *ailist, u32 spawnflags);
bool chr_is_prop_preset_blocking_sight_to_target(struct chrdata *chr);
bool chr_move_to_pos(struct chrdata *chr, struct coord *pos, RoomNum *room, f32 angle, bool ignorebg);
bool chr_check_cover_out_of_sight(struct chrdata *chr, s32 covernum, bool soft);
s32 chr_assign_cover_by_criteria(struct chrdata *chr, u16 arg1, s32 arg2);
s32 chr_assign_cover_away_from_danger(struct chrdata *chr, s32 mindist, s32 maxdist);
bool chr_run_from_pos(struct chrdata *chr, u32 goposflags, f32 distance, struct coord *frompos);
bool chr_flank(struct chrdata *chr, u32 angle360, struct coord *pos, u8 use_closest_side, u8 goposflags);
void rebuild_teams(void);
void rebuild_squadrons(void);
void chr_avoid(struct chrdata *chr);
void chr_stand_immediate(struct chrdata *chr, f32 mergetime);
f32 chr_get_shield(struct chrdata *chr);
void chr_die(struct chrdata *chr, s32 aplayernum);
struct path *path_find_by_id(u32 path_id);
bool chr_has_los_to_target(struct chrdata *chr);
void chr_record_last_hear_target_time(struct chrdata *chr);
bool chr_is_dead(struct chrdata *chr);
bool chr_try_attackstand(struct chrdata *chr, u32 attackflags, s32 entityid);
bool chr_try_attackkneel(struct chrdata *chr, u32 attackflags, s32 entityid);
bool chr_try_attacklie(struct chrdata *chr, u32 attackflags, s32 entityid);
bool chr_try_modifyattack(struct chrdata *chr, u32 attackflags, s32 entityid);
bool chr_face_entity(struct chrdata *chr, u32 attackflags, s32 entityid);
bool chr_set_path(struct chrdata *chr, u32 path_id);
bool chr_try_start_patrol(struct chrdata *chr);
bool chr_try_surrender(struct chrdata *chr);
bool chr_fade_out(struct chrdata *chr);
bool chr_go_to_target(struct chrdata *chr, u32 goposflags);
bool chr_go_to_chr(struct chrdata *chr, u32 dst_chrnum, u32 goposflags);
bool chr_go_to_prop(struct chrdata *chr, struct prop *prop, u32 goposflags);
bool chr_try_stop(struct chrdata *chr);
bool chr_try_surprised_onehand(struct chrdata *chr);
bool chr_try_surprised_surrender(struct chrdata *chr);
bool chr_try_surprised_lookaround(struct chrdata *chr);
bool chr_try_kneel(struct chrdata *chr);
bool chr_try_anim(struct chrdata *chr, s32 animnum, f32 startframe, f32 endframe, u8 chranimflags, s32 merge, f32 speed);
bool chr_try_startalarm(struct chrdata *chr, s32 pad_id);
bool chr_consider_grenade_throw(struct chrdata *chr, u32 attackflags, u32 entityid);
void chr_fade_corpse_when_offscreen(struct chrdata *chr);
void chr_set_theta(struct chrdata *chr, f32 angle);
f32 chr_get_rot_y(struct chrdata *chr);
void chr_reset_aimend(struct chrdata *chr);
void chr_set_firing(struct chrdata *chr, s32 hand, bool firing);
void chr_stop_firing(struct chrdata *chr);
void chr_set_hand_firing(struct chrdata *chr, s32 hand, bool arg2);
void prop_unset_dangerous(struct prop *prop);
void prop_set_dangerous(struct prop *prop);
bool chr_detect_dangerous_object(struct chrdata *chr, u8 flags);
s32 chr_is_using_lift(struct chrdata *chr);
bool chr_try_skjump(struct chrdata *chr, u8 pouncebits, u8 arg2, s32 arg3, u8 arg4);
bool chr_saw_target_recently(struct chrdata *chr);
bool chr_heard_target_recently(struct chrdata *chr);
f32 chr_get_angle_to_target(struct chrdata *chr);
f32 chr_get_distance_to_target(struct chrdata *chr);
f32 chr_get_distance_to_target2(struct chrdata *chr);
f32 chr_get_distance_to_current_player(struct chrdata *chr);
f32 chr_get_distance_to_pad(struct chrdata *chr, s32 pad_id);
f32 chr_get_distance_to_coord(struct chrdata *chr, struct coord *pos);
f32 chr_get_lateral_distance_to_coord(struct chrdata *chr, struct coord *pos);
f32 chr_get_squared_distance_to_coord(struct chrdata *chr, struct coord *pos);
f32 coord_get_squared_distance_to_coord(struct coord *a, struct coord *b);
s32 chr_get_pad_room(struct chrdata *chr, s32 pad_id);
s32 chr_resolve_pad_id(struct chrdata *chr, s32 pad_id);
struct chrdata *chr_find_by_id(struct chrdata *data, s32 chrnum);
s32 prop_get_index_by_chr_id(struct chrdata *chr, s32 chrnum);
f32 chr_get_distance_to_chr(struct chrdata *chr1, s32 chr2num);
f32 chr_get_distance_from_target_to_pad(struct chrdata *chr, s32 pad_id);
void chr_set_flags(struct chrdata *chr, u32 flags, u8 bank);
void chr_unset_flags(struct chrdata *chr, u32 flags, u8 bank);
bool chr_has_flag(struct chrdata *chr, u32 flag, u8 bank);
void chr_set_flags_by_id(struct chrdata *ref, u32 chrnum, u32 flags, u32 bank);
void chr_unset_flags_by_id(struct chrdata *ref, u32 chrnum, u32 flags, u32 bank);
bool chr_has_flag_by_id(struct chrdata *ref, u32 chrnum, u32 flag, u32 bank);
void chr_set_stage_flag(struct chrdata *chr, u32 flag);
void chr_unset_stage_flag(struct chrdata *chr, u32 flag);
bool chr_has_stage_flag(struct chrdata *chr, u32 flag);
bool chr_is_hearing_target(struct chrdata *chr);
void chr_restart_timer(struct chrdata *chr);
bool chr_reset_nearmiss(struct chrdata *chr);
s32 chr_get_num_arghs(struct chrdata *chr);
s32 chr_get_num_closearghs(struct chrdata *chr);
void decrement_byte(u8 *dst, u8 amount);
void increment_byte(u8 *dst, u8 amount);
bool chr_can_hear_alarm(struct chrdata *chr);
bool chr_set_chr_preset_to_any_chr_near_self(struct chrdata *chr, f32 distance);
bool chr_set_chr_preset_to_chr_near_self(u8 checktype, struct chrdata *chr, f32 distance);
bool chr_compare_teams(struct chrdata *chr1, struct chrdata *chr2, u8 checktype);
void chr_set_chr_preset(struct chrdata *chr, s32 chrpreset);
void chr_set_chr_preset_by_chrnum(struct chrdata *chr, s32 chrnum, s32 chrpreset);
void chr_set_pad_preset(struct chrdata *chr, s32 pad_id);
void chr_set_pad_preset_by_chrnum(struct chrdata *chr, s32 chrnum, s32 pad_id);
struct prop *chr_spawn_at_pad(struct chrdata *chr, s32 body, s32 head, s32 pad, u8 *ailist, u32 spawnflags);
struct prop *chr_spawn_at_chr(struct chrdata *basechr, s32 body, s32 head, u32 chrnum, u8 *ailist, u32 spawnflags);
s16 chr_go_to_cover(struct chrdata *chr, u8 speed);
void chr_add_target_to_bdlist(struct chrdata *chr);
s32 chr_get_distance_lost_to_target_in_last_second(struct chrdata *chr);
bool chr_is_target_nearly_in_sight(struct chrdata *chr, u32 distance);
bool chr_is_nearly_in_targets_sight(struct chrdata *chr, u32 distance);
s16 *team_get_chr_ids(s32 team_id);
s16 *squadron_get_chr_ids(s32 squadron_id);
void audio_mark_as_recently_played(s16 audioid);
bool audio_was_not_played_recently(s16 audioid);
Gfx *chrs_render_chr_stats(Gfx *gdl, RoomNum *rooms);
void chr_toggle_model_part(struct chrdata *chr, s32 partnum);
bool chr_is_avoiding(struct chrdata *chr);
void chr_drcaroll_emit_sparks(struct chrdata *chr);

#endif
