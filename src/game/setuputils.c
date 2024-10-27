#include <ultra64.h>
#include "constants.h"
#include "game/prop.h"
#include "game/setuputils.h"
#include "game/modelmgr.h"
#include "game/bg.h"
#include "game/modeldef.h"
#include "game/propobj.h"
#include "lib/main.h"
#include "lib/model.h"
#include "bss.h"
#include "data.h"
#include "types.h"

struct stagesetup g_StageSetup;
u8 *g_GeCreditsData;

u32 setup_get_cmd_length(u32 *cmd)
{
#if VERSION < VERSION_NTSC_1_0
	static u32 crash1 = 0;

	main_override_variable("crash1", &crash1);
#endif

	switch ((u8)cmd[0]) {
	case OBJTYPE_CHR:                 return sizeof(struct packedchr) / sizeof(u32);
	case OBJTYPE_DOOR:                return sizeof(struct doorobj) / sizeof(u32);
	case OBJTYPE_DOORSCALE:           return sizeof(struct doorscaleobj) / sizeof(u32);
	case OBJTYPE_BASIC:               return sizeof(struct defaultobj) / sizeof(u32);
	case OBJTYPE_DEBRIS:              return sizeof(struct debrisobj) / sizeof(u32);
	case OBJTYPE_GLASS:               return sizeof(struct glassobj) / sizeof(u32);
	case OBJTYPE_TINTEDGLASS:         return sizeof(struct tintedglassobj) / sizeof(u32);
	case OBJTYPE_SAFE:                return sizeof(struct safeobj) / sizeof(u32);
	case OBJTYPE_GASBOTTLE:           return sizeof(struct gasbottleobj) / sizeof(u32);
	case OBJTYPE_KEY:                 return sizeof(struct keyobj) / sizeof(u32);
	case OBJTYPE_ALARM:               return sizeof(struct alarmobj) / sizeof(u32);
	case OBJTYPE_CCTV:                return sizeof(struct cctvobj) / sizeof(u32);
	case OBJTYPE_AMMOCRATE:           return sizeof(struct ammocrateobj) / sizeof(u32);
	case OBJTYPE_WEAPON:              return sizeof(struct weaponobj) / sizeof(u32);
	case OBJTYPE_SINGLEMONITOR:       return sizeof(struct singlemonitorobj) / sizeof(u32);
	case OBJTYPE_MULTIMONITOR:        return sizeof(struct multimonitorobj) / sizeof(u32);
	case OBJTYPE_HANGINGMONITORS:     return sizeof(struct hangingmonitorsobj) / sizeof(u32);
	case OBJTYPE_AUTOGUN:             return sizeof(struct autogunobj) / sizeof(u32);
	case OBJTYPE_LINKGUNS:            return sizeof(struct linkgunsobj) / sizeof(u32);
	case OBJTYPE_HAT:                 return sizeof(struct hatobj) / sizeof(u32);
	case OBJTYPE_GRENADEPROB:         return sizeof(struct grenadeprobobj) / sizeof(u32);
	case OBJTYPE_LINKLIFTDOOR:        return sizeof(struct linkliftdoorobj) / sizeof(u32);
	case OBJTYPE_SAFEITEM:            return sizeof(struct safeitemobj) / sizeof(u32);
	case OBJTYPE_MULTIAMMOCRATE:      return sizeof(struct multiammocrateobj) / sizeof(u32);
	case OBJTYPE_SHIELD:              return sizeof(struct shieldobj) / sizeof(u32);
	case OBJTYPE_TAG:                 return sizeof(struct tag) / sizeof(u32);
	case OBJTYPE_RENAMEOBJ:           return sizeof(struct textoverride) / sizeof(u32);
	case OBJTYPE_BEGINOBJECTIVE:      return sizeof(struct objective) / sizeof(u32);
	case OBJTYPE_ENDOBJECTIVE:        return 1;
	case OBJECTIVETYPE_DESTROYOBJ:    return 2;
	case OBJECTIVETYPE_COMPFLAGS:     return 2;
	case OBJECTIVETYPE_FAILFLAGS:     return 2;
	case OBJECTIVETYPE_COLLECTOBJ:    return 2;
	case OBJECTIVETYPE_THROWOBJ:      return 2;
	case OBJECTIVETYPE_HOLOGRAPH:     return sizeof(struct criteria_holograph) / sizeof(u32);
	case OBJECTIVETYPE_1F:            return 1;
	case OBJECTIVETYPE_ENTERROOM:     return sizeof(struct criteria_roomentered) / sizeof(u32);
	case OBJECTIVETYPE_THROWINROOM:   return sizeof(struct criteria_throwinroom) / sizeof(u32);
	case OBJECTIVETYPE_COPYGOLDENEYE: return 1;
	case OBJTYPE_BRIEFING:            return sizeof(struct briefingobj) / sizeof(u32);
	case OBJTYPE_PADLOCKEDDOOR:       return sizeof(struct padlockeddoorobj) / sizeof(u32);
	case OBJTYPE_TRUCK:               return sizeof(struct truckobj) / sizeof(u32);
	case OBJTYPE_HELI:                return sizeof(struct heliobj) / sizeof(u32);
	case OBJTYPE_TANK:                return 32;
	case OBJTYPE_CAMERAPRESET:        return sizeof(struct camerapresetobj) / sizeof(u32);
	case OBJTYPE_LIFT:                return sizeof(struct liftobj) / sizeof(u32);
	case OBJTYPE_CONDITIONALSCENERY:  return sizeof(struct linksceneryobj) / sizeof(u32);
	case OBJTYPE_BLOCKEDPATH:         return sizeof(struct blockedpathobj) / sizeof(u32);
	case OBJTYPE_HOVERBIKE:           return sizeof(struct hoverbikeobj) / sizeof(u32);
	case OBJTYPE_HOVERPROP:           return sizeof(struct hoverpropobj) / sizeof(u32);
	case OBJTYPE_FAN:                 return sizeof(struct fanobj) / sizeof(u32);
	case OBJTYPE_HOVERCAR:            return sizeof(struct hovercarobj) / sizeof(u32);
	case OBJTYPE_CHOPPER:             return sizeof(struct chopperobj) / sizeof(u32);
	case OBJTYPE_PADEFFECT:           return sizeof(struct padeffectobj) / sizeof(u32);
	case OBJTYPE_MINE:                return sizeof(struct weaponobj) / sizeof(u32);
	case OBJTYPE_ESCASTEP:            return sizeof(struct escalatorobj) / sizeof(u32);
	}

#if VERSION < VERSION_NTSC_1_0
	if (crash1) {
		CRASH();
	}
#endif

	return 1;
}

u32 *setup_get_cmd_by_index(s32 wantindex)
{
	u32 *cmd = g_StageSetup.props;

	if (wantindex >= 0 && cmd) {
		s32 cmdindex = 0;

		while ((u8)cmd[0] != OBJTYPE_END) {
			if (cmdindex == wantindex) {
				return cmd;
			}

			cmd = cmd + setup_get_cmd_length(cmd);
			cmdindex++;
		}
	}

	return NULL;
}

s32 setup_get_cmd_index_by_tag(struct tag *tag)
{
	u32 *cmd = g_StageSetup.props;

	if (cmd) {
		s32 cmdindex = 0;

		while ((u8)cmd[0] != OBJTYPE_END) {
			if ((struct tag *)cmd == tag) {
				return cmdindex;
			}

			cmd = cmd + setup_get_cmd_length(cmd);
			cmdindex++;
		}
	}

	return -1;
}

u32 setup_get_cmd_index_by_prop(struct prop *prop)
{
	u32 *cmd = g_StageSetup.props;

	if (cmd) {
		s32 cmdindex = 0;

		while ((u8)cmd[0] != OBJTYPE_END) {
			if ((struct prop *)cmd[5] == prop) {
				return cmdindex;
			}

			cmd = cmd + setup_get_cmd_length(cmd);
			cmdindex++;
		}
	}

	return -1;
}

bool setup_load_modeldef(s32 modelnum)
{
	if (g_ModelStates[modelnum].modeldef == NULL) {
		g_ModelStates[modelnum].modeldef = modeldef_load_to_new(g_ModelStates[modelnum].fileid);
		model_allocate_rw_data(g_ModelStates[modelnum].modeldef);
		return true;
	}

	return false;
}

bool setup_get_obj_bbox(struct defaultobj *obj, struct coord *pos, f32 realrot[3][3], struct coord *arg3, struct coord *arg4)
{
	struct modelrodata_bbox *bbox = obj_find_bbox_rodata(obj);

	if (bbox != NULL) {
		arg3->x = pos->x + obj_get_rotated_local_x_min_by_mtx3(bbox, realrot);
		arg3->y = pos->y + obj_get_rotated_local_y_min_by_mtx3(bbox, realrot);
		arg3->z = pos->z + obj_get_rotated_local_z_min_by_mtx3(bbox, realrot);

		arg4->x = pos->x + obj_get_rotated_local_x_max_by_mtx3(bbox, realrot);
		arg4->y = pos->y + obj_get_rotated_local_y_max_by_mtx3(bbox, realrot);
		arg4->z = pos->z + obj_get_rotated_local_z_max_by_mtx3(bbox, realrot);

		return true;
	}

	return false;
}

bool setup0f092304(struct defaultobj *obj, struct coord *arg1, struct coord *arg2)
{
	return setup_get_obj_bbox(obj, &obj->prop->pos, obj->realrot, arg1, arg2);
}

void obj_find_rooms(struct defaultobj *obj, struct coord *pos, f32 realrot[3][3], RoomNum *rooms)
{
	struct coord bbmin;
	struct coord bbmax;
	u32 stack;

	if (setup_get_obj_bbox(obj, pos, realrot, &bbmin, &bbmax)) {
		bbmin.x -= 1;
		bbmin.y -= 1;
		bbmin.z -= 1;
		bbmax.x += 1;
		bbmax.y += 1;
		bbmax.z += 1;

		bg_find_entered_rooms(&bbmin, &bbmax, rooms, 7, false);
	}
}

void obj_detect_rooms(struct defaultobj *obj)
{
	prop_deregister_rooms(obj->prop);
	obj_find_rooms(obj, &obj->prop->pos, obj->realrot, obj->prop->rooms);
	prop_register_rooms(obj->prop);
}

struct defaultobj *setup_get_obj_by_cmd_index(u32 cmdindex)
{
	u32 *cmd = setup_get_cmd_by_index(cmdindex);

	if (cmd) {
		switch ((u8)cmd[0]) {
		case OBJTYPE_DOOR:
		case OBJTYPE_BASIC:
		case OBJTYPE_KEY:
		case OBJTYPE_ALARM:
		case OBJTYPE_CCTV:
		case OBJTYPE_AMMOCRATE:
		case OBJTYPE_WEAPON:
		case OBJTYPE_SINGLEMONITOR:
		case OBJTYPE_MULTIMONITOR:
		case OBJTYPE_HANGINGMONITORS:
		case OBJTYPE_AUTOGUN:
		case OBJTYPE_DEBRIS:
		case OBJTYPE_HAT:
		case OBJTYPE_MULTIAMMOCRATE:
		case OBJTYPE_SHIELD:
		case OBJTYPE_GASBOTTLE:
		case OBJTYPE_TRUCK:
		case OBJTYPE_HELI:
		case OBJTYPE_29:
		case OBJTYPE_GLASS:
		case OBJTYPE_SAFE:
		case OBJTYPE_TANK:
		case OBJTYPE_TINTEDGLASS:
		case OBJTYPE_LIFT:
		case OBJTYPE_HOVERBIKE:
		case OBJTYPE_HOVERPROP:
		case OBJTYPE_FAN:
		case OBJTYPE_HOVERCAR:
		case OBJTYPE_CHOPPER:
		case OBJTYPE_ESCASTEP:
			return (struct defaultobj *)cmd;
		case OBJTYPE_DOORSCALE:
		case OBJTYPE_CHR:
		case OBJTYPE_LINKGUNS:
		case OBJTYPE_GRENADEPROB:
		case OBJTYPE_LINKLIFTDOOR:
		case OBJTYPE_TAG:
		case OBJTYPE_BEGINOBJECTIVE:
		case OBJTYPE_ENDOBJECTIVE:
		case OBJECTIVETYPE_DESTROYOBJ:
		case OBJECTIVETYPE_COMPFLAGS:
		case OBJECTIVETYPE_FAILFLAGS:
		case OBJECTIVETYPE_COLLECTOBJ:
		case OBJECTIVETYPE_THROWOBJ:
		case OBJECTIVETYPE_HOLOGRAPH:
		case OBJECTIVETYPE_1F:
		case OBJECTIVETYPE_ENTERROOM:
		case OBJECTIVETYPE_THROWINROOM:
		case OBJECTIVETYPE_COPYGOLDENEYE:
		case OBJTYPE_BRIEFING:
		case OBJTYPE_RENAMEOBJ:
		case OBJTYPE_PADLOCKEDDOOR:
		case OBJTYPE_SAFEITEM:
		case OBJTYPE_CAMERAPRESET:
		case OBJTYPE_CONDITIONALSCENERY:
		case OBJTYPE_BLOCKEDPATH:
		case OBJTYPE_PADEFFECT:
			return NULL;
		}
	}

	return (struct defaultobj *)cmd;
}

/**
 * Find an object in the setup file of little importance that can be reused for
 * something of greater importance.
 *
 * For example, if a weapon is being dropped on screen and there are no extra
 * weapon structs left, this function is used to find another weapon elsewhere
 * in the stage so it can be repurposed.
 *
 * The caller can specify if the obj must already have a prop or model. The
 * caller sets these to true if the game is already at the prop or model limit.
 *
 * If an unused object can be found then it will be returned. Otherwise, for
 * non-weapons, the function will return NULL and write some candidates to
 * offscreenobjptr and anyobjptr. offscreenobjptr will be an object that is
 * offscreen, while anyobjptr will be any object.
 *
 * In reality this function is only called with objtypes weapon and hat, and the
 * hat caller is unreachable because hats don't exist in PD. So it's only used
 * for weapons which means the candidate logic isn't used.
 */
struct defaultobj *setup_find_obj_for_reuse(s32 wanttype, struct defaultobj **offscreenobjptr, struct defaultobj **anyobjptr, bool musthaveprop, bool musthavemodel, struct modeldef *modeldef)
{
	struct defaultobj *offscreenobj = NULL;
	struct defaultobj *anyobj = NULL;

	u32 *cmd = g_StageSetup.props;

	if (cmd) {
		while ((u8)cmd[0] != OBJTYPE_END) {
			if ((wanttype & 0xff) == (u8)cmd[0]) {
				struct defaultobj *obj = (struct defaultobj *)cmd;

				if (obj->prop == NULL) {
					if (!musthaveprop && !musthavemodel) {
						return obj;
					}
				} else if (wanttype != OBJTYPE_WEAPON
						&& (obj->hidden & OBJHFLAG_PROJECTILE) == 0
						&& (obj->hidden2 & OBJH2FLAG_CANREGEN) == 0
#if VERSION >= VERSION_NTSC_1_0
						&& (obj->flags & OBJFLAG_HELDROCKET) == 0
#endif
						&& obj->prop->parent == NULL
						&& (!musthavemodel || modelmgr_can_slot_fit_rwdata(obj->model, modeldef))) {
					if (offscreenobj == NULL && (obj->prop->flags & (PROPFLAG_ONTHISSCREENTHISTICK | PROPFLAG_ONANYSCREENTHISTICK | PROPFLAG_ONANYSCREENPREVTICK)) == 0) {
						offscreenobj = obj;
					}

					if (anyobj == NULL) {
						anyobj = obj;
					}
				}
			}

			cmd = cmd + setup_get_cmd_length(cmd);
		}
	}

	*offscreenobjptr = offscreenobj;
	*anyobjptr = anyobj;

	return NULL;
}
