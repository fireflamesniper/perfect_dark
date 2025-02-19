#include <ultra64.h>
#include "constants.h"
#include "game/acosfasinf.h"
#include "game/atan2f.h"
#include "game/bg.h"
#include "game/body.h"
#include "game/bondgun.h"
#include "game/bot.h"
#include "game/botact.h"
#include "game/botcmd.h"
#include "game/botinv.h"
#include "game/camera.h"
#include "game/chr.h"
#include "game/chraction.h"
#include "game/chrai.h"
#include "game/debug.h"
#include "game/dlights.h"
#include "game/env.h"
#include "game/explosions.h"
#include "game/footstep.h"
#include "game/game_006900.h"
#include "game/gset.h"
#include "game/text.h"
#include "game/gunfx.h"
#include "game/inv.h"
#include "game/lv.h"
#include "game/modelmgr.h"
#include "game/mplayer/mplayer.h"
#include "game/mpstats.h"
#include "game/objectives.h"
#include "game/options.h"
#include "game/pad.h"
#include "game/padhalllv.h"
#include "game/pak.h"
#include "game/pdmode.h"
#include "game/player.h"
#include "game/playermgr.h"
#include "game/prop.h"
#include "game/propobj.h"
#include "game/propsnd.h"
#include "game/sparks.h"
#include "game/stagetable.h"
#include "game/tex.h"
#include "game/wallhit.h"
#include "bss.h"
#include "lib/joy.h"
#include "lib/lib_17ce0.h"
#include "lib/main.h"
#include "lib/model.h"
#include "lib/snd.h"
#include "lib/rng.h"
#include "lib/mtx.h"
#include "lib/ailist.h"
#include "lib/anim.h"
#include "lib/collision.h"
#include "lib/vi.h"
#include "data.h"
#include "types.h"

s32 g_RecentQuipsPlayed[5];
u32 var8009cd84;
u32 var8009cd88;
u32 var8009cd8c;
u32 var8009cd90;
u32 var8009cd94;
u8 g_RecentQuipsIndex;

f32 g_EnemyAccuracyScale = 1;
f32 g_PlayerDamageRxScale = 1;
f32 g_PlayerDamageTxScale = 1;
f32 g_AttackWalkDurationScale = 1;

#if VERSION >= VERSION_NTSC_1_0
s32 g_NumChrsWithPlayerTarget = 0;
s32 g_NumChrsSeenPlayer = 0;
s32 g_NumChrsSeenPlayerRecently = 0;
#endif

s32 g_NumChrsSeenPlayerRecently2 = 0;

#if VERSION < VERSION_NTSC_1_0
const char *g_ChrActionNames[] = {
	"ACT_INIT",
	"ACT_STAND",
	"ACT_KNEEL",
	"ACT_ANIM",
	"ACT_DIE",
	"ACT_DEAD",
	"ACT_ARGH",
	"ACT_PREARGH",
	"ACT_ATTACK",
	"ACT_ATTACKWALK",
	"ACT_ATTACKROLL",
	"ACT_SIDESTEP",
	"ACT_JUMPOUT",
	"ACT_RUNPOS",
	"ACT_PATROL",
	"ACT_GOPOS",
	"ACT_SURRENDER",
	"ACT_LOOKATTARGET",
	"ACT_SURPRISED",
	"ACT_STARTALARM",
	"ACT_THROWGRENADE",
	"ACT_TURNDIR",
	"ACT_TEST",
	"ACT_BONDINTRO",
	"ACT_BONDDIE",
	"ACT_BONDMULTI",
	"ACT_NULL",
	"ACT_BOT_ATTACKSTAND",
	"ACT_BOT_ATTACKKNEEL",
	"ACT_BOT_ATTACKSTRAFE",
	"ACT_DRUGGEDDROP",
	"ACT_DRUGGEDKO",
	"ACT_DRUGGEDCOMINGUP",
	"ACT_ATTACKAMOUNT",
	"ACT_ROBOTATTACK",
	"ACT_SKJUMP",
	"ACT_PUNCH",
	"ACT_CUTFIRE",
};

const char *g_ChrMyActionNames[] = {
	"NONE",
	"NORMAL",
	"COVERWAIT",
	"GRENADEWAIT",
	"WAITING",
	"COVERGOTO",
	"COVERBREAK",
	"COVERSEEN",
	"FLANKLEFT",
	"FLANKRIGHT",
	"DODGE",
	"GRENADE",
	"WAITSEEN",
	"WITHDRAW",
	"SHOOTING",
	"SYNCSHOOT",
	"WAITTIMEOUT",
	"COVERTIMEOUT",
	"TRACKING",
	"RETREAT",
	"SURRENDER",
	"TALKING",
	"LISTENING",
	"GOTOALARM",
	"BOTFRIENDFOLLOW",
	"BOTHIDE",
	"BOTPATH",
	"BOTINJURED",
	"BOTNORMAL",
	"BOTSHOOTING",
	"DRUGGED",
	"PANIC",
	"RUNFROMGRENADE",
	"UNARMEDATTACK",
	"MA_SKJUMP_START",
	"MA_SKJUMP_AIR",
	"MA_SKJUMP_LAND",
	"MA_SKJUMP_LANDLOOP",
	"MA_SKJUMP_SHOT",
	"AIBOTDEADLIST",
	"AIBOTINIT",
	"AIBOTMAINLOOP",
	"AIBOTGETITEM",
	"AIBOTGOTOPOS",
	"AIBOTGOTOPROP",
	"AIBOTRUNAWAY",
	"AIBOTDOWNLOAD",
	"AIBOTATTACK",
	"unused1",
	"unused2",
	"AIBOTFOLLOW",
	"AIBOTDEFEND",
	"FLANKBEST",
	"FACING",
	"PUNCHING",
};

const char *g_ChrActionNames2[] = {
	"ACT_INIT",
	"ACT_STAND",
	"ACT_KNEEL",
	"ACT_ANIM",
	"ACT_DIE",
	"ACT_DEAD",
	"ACT_ARGH",
	"ACT_PREARGH",
	"ACT_ATTACK",
	"ACT_ATTACKWALK",
	"ACT_ATTACKROLL",
	"ACT_SIDESTEP",
	"ACT_JUMPOUT",
	"ACT_RUNPOS",
	"ACT_PATROL",
	"ACT_GOPOS",
	"ACT_SURRENDER",
	"ACT_LOOKATTARGET",
	"ACT_SURPRISED",
	"ACT_STARTALARM",
	"ACT_THROWGRENADE",
	"ACT_TURNDIR",
	"ACT_TEST",
	"ACT_BONDINTRO",
	"ACT_BONDDIE",
	"ACT_BONDMULTI",
	"ACT_NULL",
	"ACT_BOT_ATTACKSTAND",
	"ACT_BOT_ATTACKKNEEL",
	"ACT_BOT_ATTACKSTRAFE",
	"ACT_DRUGGEDDROP",
	"ACT_DRUGGEDKO",
	"ACT_DRUGGEDCOMINGUP",
	"ACT_ATTACKAMOUNT",
	"ACT_ROBOTATTACK",
	"ACT_SKJUMP",
};

const char *g_ChrLiftActionNames[] = {
	"NOTUSINGLIFT",
	"WAITINGFORLIFT",
	"ONLIFT",
	"WAITINGONLIFT",
};
#endif

struct animtablerow g_DeathAnimsHumanLfoot[] = {
	{ ANIM_DEATH_0020, 0, -1, 0.5, 0, 26, -1 },
	{ 0,      0,  0, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanLshin[] = {
	{ ANIM_DEATH_0020, 0, -1, 0.5, 0, 26, -1 },
	{ 0,      0,  0, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanLthigh[] = {
	{ ANIM_DEATH_0020, 0, -1, 0.5, 1, 26, -1  },
	{ ANIM_DEATH_STOMACH_LONG, 1, -1, 0.5, 0, -1, -1  },
	{ ANIM_0092, 1, -1, 0.4, 0, 42, 103 },
	{ ANIM_0258, 1, -1, 0.5, 0, 43, 100 },
	{ 0,      0, 0,  0.5, 0, -1, -1  },
};

struct animtablerow g_DeathAnimsHumanRfoot[] = {
	{ ANIM_DEATH_0020, 1, -1, 0.5, 0, 26, -1 },
	{ 0,      0, 0,  0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanRshin[] = {
	{ ANIM_DEATH_0020, 1, -1, 0.5, 0, 26, -1 },
	{ 0,      0, 0,  0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanRthigh[] = {
	{ ANIM_DEATH_0020, 1, -1, 0.5, 1, 26, -1  },
	{ ANIM_DEATH_STOMACH_LONG, 0, -1, 0.5, 0, -1, -1  },
	{ ANIM_0092, 0, -1, 0.4, 0, 42, 103 },
	{ ANIM_0258, 0, -1, 0.5, 0, 43, 100 },
	{ 0,      0, 0,  0.5, 0, -1, -1  },
};

struct animtablerow g_DeathAnimsHumanPelvis[] = {
	{ ANIM_DEATH_001A, 0, -1, 0.5, 0, 55,  39  },
	{ ANIM_DEATH_001A, 1, -1, 0.5, 0, 55,  39  },
	{ ANIM_DEATH_001C, 0, -1, 0.5, 1, 29,  -1  },
	{ ANIM_DEATH_001C, 1, -1, 0.5, 1, 29,  -1  },
	{ ANIM_DEATH_0021, 0, -1, 0.5, 0, 97,  64  },
	{ ANIM_DEATH_0021, 1, -1, 0.5, 0, 97,  64  },
	{ ANIM_DEATH_0023, 0, -1, 0.5, 0, 31,  -1  },
	{ ANIM_DEATH_0023, 1, -1, 0.5, 0, 31,  -1  },
	{ ANIM_DEATH_0024, 0, -1, 0.5, 0, 36,  -1  },
	{ ANIM_DEATH_0024, 1, -1, 0.5, 0, 36,  -1  },
	{ ANIM_DEATH_0025, 0, -1, 0.5, 0, 28,  -1  },
	{ ANIM_DEATH_0025, 1, -1, 0.5, 0, 28,  -1  },
	{ ANIM_0090, 0, -1, 0.6, 0, 157, 234 },
	{ ANIM_0090, 1, -1, 0.6, 0, 157, 234 },
	{ ANIM_0091, 0, -1, 0.6, 0, 75,  265 },
	{ ANIM_0091, 1, -1, 0.6, 0, 75,  265 },
	{ ANIM_0250, 0, -1, 0.5, 0, 65,  105 },
	{ ANIM_0250, 1, -1, 0.5, 0, 65,  105 },
	{ 0,      0, -1, 0.5, 0, -1,  -1  },
};

struct animtablerow g_DeathAnimsHumanHead[] = {
	{ ANIM_DEATH_001A, 0, -1, 0.5, 0, 55,  39  },
	{ ANIM_DEATH_001A, 1, -1, 0.5, 0, 55,  39  },
	{ ANIM_DEATH_001C, 0, -1, 0.5, 1, 29,  -1  },
	{ ANIM_DEATH_001C, 1, -1, 0.5, 1, 29,  -1  },
	{ ANIM_DEATH_0020, 0, -1, 0.5, 1, 26,  -1  },
	{ ANIM_DEATH_0020, 1, -1, 0.5, 1, 26,  -1  },
	{ ANIM_DEATH_0021, 0, -1, 0.5, 0, 97,  64  },
	{ ANIM_DEATH_0021, 1, -1, 0.5, 0, 97,  64  },
	{ ANIM_DEATH_0022, 0, -1, 0.5, 0, 94,  66  },
	{ ANIM_DEATH_0022, 1, -1, 0.5, 0, 94,  66  },
	{ ANIM_DEATH_0023, 0, -1, 0.5, 0, 31,  -1  },
	{ ANIM_DEATH_0023, 1, -1, 0.5, 0, 31,  -1  },
	{ ANIM_DEATH_0024, 0, -1, 0.5, 0, 36,  -1  },
	{ ANIM_DEATH_0024, 1, -1, 0.5, 0, 36,  -1  },
	{ ANIM_DEATH_0025, 0, -1, 0.5, 0, 28,  -1  },
	{ ANIM_DEATH_0025, 1, -1, 0.5, 0, 28,  -1  },
	{ ANIM_0038, 0, -1, 0.5, 0, -1,  -1  },
	{ ANIM_0038, 1, -1, 0.5, 0, -1,  -1  },
	{ ANIM_0251, 0, -1, 0.5, 0, 132, 201 },
	{ ANIM_0251, 1, -1, 0.5, 0, 132, 201 },
	{ ANIM_0252, 0, -1, 0.5, 0, 83,  150 },
	{ ANIM_0252, 1, -1, 0.5, 0, 83,  150 },
	{ ANIM_0256, 0, -1, 0.5, 0, 63,  -1  },
	{ ANIM_0256, 1, -1, 0.5, 0, 63,  -1  },
	{ 0,      0, -1, 0.5, 0, -1,  -1  },
};

struct animtablerow g_DeathAnimsHumanLhand[] = {
	{ ANIM_DEATH_0020, 0, -1, 0.5, 0, 26, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanForearm[] = {
	{ ANIM_DEATH_0020, 0, -1, 0.5, 0, 26, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanLbicep[] = {
	{ ANIM_DEATH_0020, 0, -1, 0.5,  1, 26, -1 },
	{ ANIM_008F, 1, -1, 0.45, 1, 52, -1 },
	{ 0,      0, -1, 0.5,  0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanRhand[] = {
	{ ANIM_DEATH_0020, 1, -1, 0.5, 0, 26, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanRforearm[] = {
	{ ANIM_DEATH_0020, 1, -1, 0.5, 0, 26, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanRbicep[] = {
	{ ANIM_DEATH_0020, 1, -1, 0.5,  1, 26, -1 },
	{ ANIM_008F, 0, -1, 0.45, 1, 52, -1 },
	{ 0,      0, -1, 0.5,  0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanTorso[] = {
	{ ANIM_DEATH_001A, 0, -1, 0.5, 0, 55, 39 },
	{ ANIM_DEATH_001A, 1, -1, 0.5, 0, 55, 39 },
	{ ANIM_DEATH_001C, 0, -1, 0.5, 1, 29, -1 },
	{ ANIM_DEATH_001C, 1, -1, 0.5, 1, 29, -1 },
	{ ANIM_DEATH_0020, 0, -1, 0.5, 1, 26, -1 },
	{ ANIM_DEATH_0020, 1, -1, 0.5, 1, 26, -1 },
	{ ANIM_DEATH_0021, 0, -1, 0.5, 0, 97, 64 },
	{ ANIM_DEATH_0021, 1, -1, 0.5, 0, 97, 64 },
	{ ANIM_DEATH_0022, 0, -1, 0.5, 0, 94, 66 },
	{ ANIM_DEATH_0022, 1, -1, 0.5, 0, 94, 66 },
	{ ANIM_DEATH_0023, 0, -1, 0.5, 0, 31, -1 },
	{ ANIM_DEATH_0023, 1, -1, 0.5, 0, 31, -1 },
	{ ANIM_DEATH_0024, 0, -1, 0.5, 0, 36, -1 },
	{ ANIM_DEATH_0024, 1, -1, 0.5, 0, 36, -1 },
	{ ANIM_DEATH_0025, 0, -1, 0.5, 0, 28, -1 },
	{ ANIM_DEATH_0025, 1, -1, 0.5, 0, 28, -1 },
	{ ANIM_024E, 0, -1, 0.4, 0, 60, -1 },
	{ ANIM_024E, 1, -1, 0.4, 0, 60, -1 },
	{ ANIM_024F, 0, -1, 0.5, 0, 49, 80 },
	{ ANIM_024F, 1, -1, 0.5, 0, 49, 80 },
	{ ANIM_0253, 0, -1, 0.5, 1, 22, -1 },
	{ ANIM_0253, 1, -1, 0.5, 1, 22, -1 },
	{ ANIM_0254, 0, -1, 0.5, 0, 52, 75 },
	{ ANIM_0254, 1, -1, 0.5, 0, 52, 75 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsHumanGun[] = {
	{ ANIM_DEATH_001A, 0, -1, 0.5, 0, 55, 39 },
	{ ANIM_DEATH_001A, 1, -1, 0.5, 0, 55, 39 },
	{ ANIM_DEATH_001C, 0, -1, 0.5, 1, 29, -1 },
	{ ANIM_DEATH_001C, 1, -1, 0.5, 1, 29, -1 },
	{ ANIM_DEATH_0021, 0, -1, 0.5, 0, 97, 64 },
	{ ANIM_DEATH_0021, 1, -1, 0.5, 0, 97, 64 },
	{ ANIM_DEATH_0022, 0, -1, 0.5, 0, 94, 66 },
	{ ANIM_DEATH_0022, 1, -1, 0.5, 0, 94, 66 },
	{ ANIM_DEATH_0023, 0, -1, 0.5, 0, 31, -1 },
	{ ANIM_DEATH_0023, 1, -1, 0.5, 0, 31, -1 },
	{ ANIM_DEATH_0024, 0, -1, 0.5, 0, 36, -1 },
	{ ANIM_DEATH_0024, 1, -1, 0.5, 0, 36, -1 },
	{ ANIM_DEATH_0025, 0, -1, 0.5, 0, 28, -1 },
	{ ANIM_DEATH_0025, 1, -1, 0.5, 0, 28, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_AnimTableHumanSlumped[] = {
	{ ANIM_0019, 0, -1, 0.5, 0, 67, 54 },
	{ ANIM_0019, 1, -1, 0.5, 0, 67, 54 },
	{ ANIM_0257, 0, -1, 0.5, 0, 15, 80 },
	{ ANIM_0257, 1, -1, 0.5, 0, 15, 80 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanLfoot[] = {
	{ ANIM_0014, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0015, 1, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanLshin[] = {
	{ ANIM_0014, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0015, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_00BC, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_00BD, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanLthigh[] = {
	{ ANIM_0014, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0015, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_STOMACH_LONG, 1, 20, 0.4, 0, -1, -1 },
	{ ANIM_00BA, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanRfoot[] = {
	{ ANIM_0015, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0014, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_0236, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanRshin[] = {
	{ ANIM_0015, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0014, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_00BE, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanRthigh[] = {
	{ ANIM_0015, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0014, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_STOMACH_LONG, 0, 20, 0.4, 0, -1, -1 },
	{ ANIM_00BF, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanPelvis[] = {
	{ ANIM_DEATH_0022, 0, 20, 0.5,  0, -1, -1 },
	{ ANIM_DEATH_0022, 1, 20, 0.5,  0, -1, -1 },
	{ ANIM_DEATH_001A, 0, 15, 0.5,  0, -1, -1 },
	{ ANIM_DEATH_001A, 1, 15, 0.5,  0, -1, -1 },
	{ ANIM_DEATH_0023, 0, 10, 0.25, 0, -1, -1 },
	{ ANIM_DEATH_0023, 1, 10, 0.25, 0, -1, -1 },
	{ ANIM_00DA, 0, -1, 0.5,  0, -1, -1 },
	{ ANIM_00F4, 0, -1, 0.5,  0, -1, -1 },
	{ 0,      0, -1, 0.5,  0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanHead[] = {
	{ ANIM_DEATH_0022, 0, 20, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_0022, 1, 20, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_001A, 0, 15, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_001A, 1, 15, 0.5, 0, -1, -1 },
	{ ANIM_00F8, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_00FB, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0101, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0113, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanLhand[] = {
	{ ANIM_0012, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0013, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B8, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanForearm[] = {
	{ ANIM_0010, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0011, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B4, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_021B, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanLbicep[] = {
	{ ANIM_000E, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_000F, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_0022, 0, 20, 0.5, 0, -1, -1 },
	{ ANIM_00B0, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B1, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_021C, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B5, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanRhand[] = {
	{ ANIM_0013, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0012, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B9, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanRforearm[] = {
	{ ANIM_0011, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0010, 1, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanRbicep[] = {
	{ ANIM_000F, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_000E, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_0022, 1, 20, 0.5, 0, -1, -1 },
	{ ANIM_0190, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B2, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B3, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B6, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_00B7, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanTorso[] = {
	{ ANIM_DEATH_0022, 0, 20, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_0022, 1, 20, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_001A, 0, 15, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_001A, 1, 15, 0.5, 0, -1, -1 },
	{ ANIM_0114, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_0130, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsHumanGun[] = {
	{ ANIM_DEATH_0022, 0, 20, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_0022, 1, 20, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_001A, 0, 15, 0.5, 0, -1, -1 },
	{ ANIM_DEATH_001A, 1, 15, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarLfoot[] = {
	{ ANIM_0337, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_033C, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_033B, 1, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarLshin[] = {
	{ ANIM_0337, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_033C, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_033B, 1, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarLthigh[] = {
	{ ANIM_0337, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_033C, 1, -1, 0.5, 0, -1, -1 },
	{ ANIM_033B, 1, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarRfoot[] = {
	{ ANIM_0337, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033C, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033B, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarRshin[] = {
	{ ANIM_0337, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033C, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033B, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarRthigh[] = {
	{ ANIM_0337, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033C, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033B, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarPelvis[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarHead[] = {
	{ ANIM_0339, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_0338, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033A, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarLhand[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarLforearm[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarLbicep[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarRhand[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarRforearm[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarRbicep[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarTorso[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarGun[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_DeathAnimsSkedarTail[] = {
	{ ANIM_0336, 0, -1, 0.5, 0, -1, -1 },
	{ ANIM_033D, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarLfoot[] = {
	{ ANIM_038E, 0, -1, 1,   0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarLshin[] = {
	{ ANIM_0390, 0, -1, 0.9, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarLthigh[] = {
	{ ANIM_0390, 0, -1, 0.9, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarRfoot[] = {
	{ ANIM_038E, 1, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarRshin[] = {
	{ ANIM_0390, 1, -1, 0.9, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarRthigh[] = {
	{ ANIM_0390, 1, -1, 0.9, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarPelvis[] = {
	{ ANIM_038C, 0, -1, 0.9, 0, -1, -1 },
	{ ANIM_038C, 1, -1, 0.9, 0, -1, -1 },
	{ ANIM_0341, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarHead[] = {
	{ ANIM_0341, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarLhand[] = {
	{ ANIM_0343, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarLforearm[] = {
	{ ANIM_038B, 0, -1, 0.9, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarLbicep[] = {
	{ ANIM_038B, 0, -1, 0.9, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarRhand[] = {
	{ ANIM_0343, 1, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarRforearm[] = {
	{ ANIM_038B, 1, -1, 0.9, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarRbicep[] = {
	{ ANIM_038B, 1, -1, 0.9, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarTorso[] = {
	{ ANIM_038C, 0, -1, 0.9, 0, -1, -1 },
	{ ANIM_038C, 1, -1, 0.9, 0, -1, -1 },
	{ ANIM_0341, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarGun[] = {
	{ ANIM_0343, 0, -1, 0.5, 0, -1, -1 },
	{ 0,      0, -1, 0.5, 0, -1, -1 },
};

struct animtablerow g_InjuryAnimsSkedarTail[] = {
	{ ANIM_0341, 0, -1, 0.5, 0, -1,  -1  },
	{ 0,      0, -1, 0.5, 0, -1,  -1  },
};

struct animtablerow g_SpecialDieAnims[] = {
	{ ANIM_SPECIALDIE_FALLBACK,    0, -1, 0.5, 0, 149, 175 },
	{ ANIM_SPECIALDIE_ROLL1,       0, -1, 0.5, 0, 115, 152 },
	{ ANIM_SPECIALDIE_ROLL2,       0, -1, 0.5, 0, 115, 152 },
	{ ANIM_SPECIALDIE_ROLL3,       0, -1, 0.5, 0, 115, 152 },
	{ ANIM_SPECIALDIE_OVERRAILING, 0, -1, 0.5, 0, 83,  99  },
	{ ANIM_022B,                   0, -1, 0.5, 0, 0,   0   },
	{ ANIM_022C,                   0, -1, 0.5, 0, 0,   0   },
	{ ANIM_022D,                   0, -1, 0.5, 0, 0,   0   },
	{ ANIM_022E,                   0, -1, 0.5, 0, 0,   0   },
	{ 0,                           0, -1, 0.5, 0, -1,  -1  },
};

struct animtable g_AnimTablesHuman[] = {
	{ 0,                NULL,                      NULL,                       0, 0 },
	{ HITPART_LFOOT,    g_DeathAnimsHumanLfoot,    g_InjuryAnimsHumanLfoot,    0, 0 },
	{ HITPART_LSHIN,    g_DeathAnimsHumanLshin,    g_InjuryAnimsHumanLshin,    0, 0 },
	{ HITPART_LTHIGH,   g_DeathAnimsHumanLthigh,   g_InjuryAnimsHumanLthigh,   0, 0 },
	{ HITPART_RFOOT,    g_DeathAnimsHumanRfoot,    g_InjuryAnimsHumanRfoot,    0, 0 },
	{ HITPART_RSHIN,    g_DeathAnimsHumanRshin,    g_InjuryAnimsHumanRshin,    0, 0 },
	{ HITPART_RTHIGH,   g_DeathAnimsHumanRthigh,   g_InjuryAnimsHumanRthigh,   0, 0 },
	{ HITPART_PELVIS,   g_DeathAnimsHumanPelvis,   g_InjuryAnimsHumanPelvis,   0, 0 },
	{ HITPART_HEAD,     g_DeathAnimsHumanHead,     g_InjuryAnimsHumanHead,     0, 0 },
	{ HITPART_LHAND,    g_DeathAnimsHumanLhand,    g_InjuryAnimsHumanLhand,    0, 0 },
	{ HITPART_LFOREARM, g_DeathAnimsHumanForearm,  g_InjuryAnimsHumanForearm,  0, 0 },
	{ HITPART_LBICEP,   g_DeathAnimsHumanLbicep,   g_InjuryAnimsHumanLbicep,   0, 0 },
	{ HITPART_RHAND,    g_DeathAnimsHumanRhand,    g_InjuryAnimsHumanRhand,    0, 0 },
	{ HITPART_RFOREARM, g_DeathAnimsHumanRforearm, g_InjuryAnimsHumanRforearm, 0, 0 },
	{ HITPART_RBICEP,   g_DeathAnimsHumanRbicep,   g_InjuryAnimsHumanRbicep,   0, 0 },
	{ HITPART_TORSO,    g_DeathAnimsHumanTorso,    g_InjuryAnimsHumanTorso,    0, 0 },
	{ HITPART_GUN,      g_DeathAnimsHumanGun,      g_InjuryAnimsHumanGun,      0, 0 },
	{ HITPART_HAT,      NULL,                      NULL,                       0, 0 },
	{ -1,               NULL,                      NULL,                       0, 0 },
};

struct animtable g_AnimTablesSkedar[] = {
	{ 0,                NULL,                       NULL,                        0, 0 },
	{ HITPART_LFOOT,    g_DeathAnimsSkedarLfoot,    g_InjuryAnimsSkedarLfoot,    0, 0 },
	{ HITPART_LSHIN,    g_DeathAnimsSkedarLshin,    g_InjuryAnimsSkedarLshin,    0, 0 },
	{ HITPART_LTHIGH,   g_DeathAnimsSkedarLthigh,   g_InjuryAnimsSkedarLthigh,   0, 0 },
	{ HITPART_RFOOT,    g_DeathAnimsSkedarRfoot,    g_InjuryAnimsSkedarRfoot,    0, 0 },
	{ HITPART_RSHIN,    g_DeathAnimsSkedarRshin,    g_InjuryAnimsSkedarRshin,    0, 0 },
	{ HITPART_RTHIGH,   g_DeathAnimsSkedarRthigh,   g_InjuryAnimsSkedarRthigh,   0, 0 },
	{ HITPART_PELVIS,   g_DeathAnimsSkedarPelvis,   g_InjuryAnimsSkedarPelvis,   0, 0 },
	{ HITPART_HEAD,     g_DeathAnimsSkedarHead,     g_InjuryAnimsSkedarHead,     0, 0 },
	{ HITPART_LHAND,    g_DeathAnimsSkedarLhand,    g_InjuryAnimsSkedarLhand,    0, 0 },
	{ HITPART_LFOREARM, g_DeathAnimsSkedarLforearm, g_InjuryAnimsSkedarLforearm, 0, 0 },
	{ HITPART_LBICEP,   g_DeathAnimsSkedarLbicep,   g_InjuryAnimsSkedarLbicep,   0, 0 },
	{ HITPART_RHAND,    g_DeathAnimsSkedarRhand,    g_InjuryAnimsSkedarRhand,    0, 0 },
	{ HITPART_RFOREARM, g_DeathAnimsSkedarRforearm, g_InjuryAnimsSkedarRforearm, 0, 0 },
	{ HITPART_RBICEP,   g_DeathAnimsSkedarRbicep,   g_InjuryAnimsSkedarRbicep,   0, 0 },
	{ HITPART_TORSO,    g_DeathAnimsSkedarTorso,    g_InjuryAnimsSkedarTorso,    0, 0 },
	{ HITPART_GUN,      g_DeathAnimsSkedarGun,      g_InjuryAnimsSkedarGun,      0, 0 },
	{ HITPART_HAT,      g_DeathAnimsSkedarHead,     g_InjuryAnimsSkedarHead,     0, 0 },
	{ HITPART_TAIL,     g_DeathAnimsSkedarTail,     g_InjuryAnimsSkedarTail,     0, 0 },
	{ -1,               NULL,                       NULL,                        0, 0 },
};

struct animtable g_AnimTablesDrCaroll[] = {
	{ 0,  NULL, NULL, 0, 0 },
	{ -1, NULL, NULL, 0, 0 },
};

struct animtable g_AnimTablesEyespy[] = {
	{ 0,  NULL, NULL, 0, 0 },
	{ -1, NULL, NULL, 0, 0 },
};

struct animtable g_AnimTablesRobot[] = {
	{ 0,  NULL, NULL, 0, 0 },
	{ -1, NULL, NULL, 0, 0 },
};

struct animtable *g_AnimTablesByRace[] = {
	g_AnimTablesHuman,
	g_AnimTablesSkedar,
	g_AnimTablesDrCaroll,
	g_AnimTablesEyespy,
	g_AnimTablesRobot,
};

/**
 * A yeet anim is an animation config for a chr being launched by an explosion.
 */
struct yeetanim {
	s16 animnum;
	bool flip;
	f32 speed;
	f32 startframe;
	f32 thudframe;
	f32 endframe;
};

struct yeetanim g_YeetAnimsHuman[] = {
	/* 0*/ { ANIM_0082, 0, 0.5, 9,  29, -1 },
	/* 1*/ { ANIM_0082, 1, 0.5, 9,  29, -1 },
	/* 2*/ { ANIM_008A, 0, 0.5, 11, 31, -1 },
	/* 3*/ { ANIM_008A, 1, 0.5, 11, 31, -1 },
	/* 4*/ { ANIM_0089, 0, 0.5, 6,  27, -1 },
	/* 5*/ { ANIM_0089, 1, 0.5, 6,  27, -1 },
	/* 6*/ { ANIM_008C, 0, 0.5, 29, 48, -1 },
	/* 7*/ { ANIM_008C, 1, 0.5, 29, 48, -1 },
	/* 8*/ { ANIM_008D, 0, 0.5, 29, 49, -1 },
	/* 9*/ { ANIM_008D, 1, 0.5, 29, 49, -1 },
	/*10*/ { ANIM_008E, 0, 0.5, 19, 42, -1 },
	/*11*/ { ANIM_008E, 1, 0.5, 19, 42, -1 },
	/*12*/ { ANIM_0086, 0, 0.5, 0,  60, -1 },
	/*13*/ { ANIM_0086, 1, 0.5, 0,  60, -1 },
	/*14*/ { ANIM_0087, 0, 0.5, 6,  29, -1 },
	/*15*/ { ANIM_0087, 1, 0.5, 6,  29, -1 },
	/*16*/ { ANIM_0084, 0, 0.5, 8,  25, -1 },
	/*17*/ { ANIM_0084, 1, 0.5, 8,  25, -1 },
	/*18*/ { ANIM_0085, 0, 0.5, 8,  25, -1 },
	/*19*/ { ANIM_0085, 1, 0.5, 8,  25, -1 },
	/*20*/ { ANIM_0088, 0, 0.5, 12, 29, -1 },
	/*21*/ { ANIM_0088, 1, 0.5, 12, 29, -1 },
	/*22*/ { ANIM_008B, 0, 0.5, 22, 41, -1 },
	/*23*/ { ANIM_008B, 1, 0.5, 22, 41, -1 },
	/*24*/ { 0,         0, 0.5, 0,  0,  -1 },
};

s8 g_YeetAnimIndexesHumanAngle0[] = { 0,  1,  2,  3, 4, 5 };
s8 g_YeetAnimIndexesHumanAngle1[] = { 7,  9,  11          };
s8 g_YeetAnimIndexesHumanAngle2[] = { 6,  8,  10          };
s8 g_YeetAnimIndexesHumanAngle3[] = { 12, 15, 0           };
s8 g_YeetAnimIndexesHumanAngle4[] = { 13, 14, 0           };
s8 g_YeetAnimIndexesHumanAngle5[] = { 18, 19, 20, 21      };
s8 g_YeetAnimIndexesHumanAngle6[] = { 16, 22              };
s8 g_YeetAnimIndexesHumanAngle7[] = { 17, 23              };

struct yeetanim g_YeetAnimsSkedar[] = {
	/* 0*/ { ANIM_033F, 0, 0.5, 0, -1, -1 },
	/* 1*/ { ANIM_033F, 0, 0.5, 0, -1, -1 },
	/* 2*/ { ANIM_033F, 0, 0.5, 0, -1, -1 },
	/* 3*/ { ANIM_033F, 0, 0.5, 0, -1, -1 },
	/* 4*/ { ANIM_033F, 0, 0.5, 0, -1, -1 },
	/* 5*/ { ANIM_033F, 0, 0.5, 0, -1, -1 },
	/* 6*/ { ANIM_033F, 0, 0.5, 0, -1, -1 },
	/* 7*/ { ANIM_033F, 0, 0.5, 0, -1, -1 },
	/* 8*/ { 0,         0, 0.5, 0, 0,  -1 },
};

s8 g_YeetAnimIndexesSkedarAngle0[] = { 0 };
s8 g_YeetAnimIndexesSkedarAngle1[] = { 1 };
s8 g_YeetAnimIndexesSkedarAngle2[] = { 2 };
s8 g_YeetAnimIndexesSkedarAngle3[] = { 3 };
s8 g_YeetAnimIndexesSkedarAngle4[] = { 4 };
s8 g_YeetAnimIndexesSkedarAngle5[] = { 7 };
s8 g_YeetAnimIndexesSkedarAngle6[] = { 5 };
s8 g_YeetAnimIndexesSkedarAngle7[] = { 6 };

struct yeetanimindexlist {
	s8 *indexes;
	s32 count;
};

struct yeetanimindexlist g_YeetAnimIndexesByRaceAngle[][8] = {
	{
		{ g_YeetAnimIndexesHumanAngle0, 6 },
		{ g_YeetAnimIndexesHumanAngle1, 3 },
		{ g_YeetAnimIndexesHumanAngle3, 3 },
		{ g_YeetAnimIndexesHumanAngle6, 2 },
		{ g_YeetAnimIndexesHumanAngle5, 4 },
		{ g_YeetAnimIndexesHumanAngle7, 2 },
		{ g_YeetAnimIndexesHumanAngle4, 3 },
		{ g_YeetAnimIndexesHumanAngle2, 3 },
	}, {
		{ g_YeetAnimIndexesSkedarAngle0, 1 },
		{ g_YeetAnimIndexesSkedarAngle1, 1 },
		{ g_YeetAnimIndexesSkedarAngle3, 1 },
		{ g_YeetAnimIndexesSkedarAngle6, 1 },
		{ g_YeetAnimIndexesSkedarAngle5, 1 },
		{ g_YeetAnimIndexesSkedarAngle7, 1 },
		{ g_YeetAnimIndexesSkedarAngle4, 1 },
		{ g_YeetAnimIndexesSkedarAngle2, 1 },
	},
};

struct attackanimconfig var800656c0[] = {
	{ ANIM_0002, 28, 0, DTOR(0), 0, -1, 23, 54, -1, -1, 18, 54, BADDTOR(50), BADDTOR(-30), BADDTOR(60), BADDTOR(-20), 1.6, 1.8 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80065750 = { var800656c0, 0xffffffff };

struct attackanimconfig var80065758[] = {
	{ ANIM_0032, 37, 0, DTOR(0), 0, -1, 30, 81, -1, -1, 25, 81, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 1.6, 1.75 },
	{ ANIM_0003, 27, 0, DTOR(0), 0, -1, 22, 61, -1, -1, 17, 61, BADDTOR(50), BADDTOR(-15), BADDTOR(40), BADDTOR(-40), 2,   1    },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80065830 = { var80065758, 0xffffffff };

struct attackanimconfig var80065838[] = {
	{ ANIM_0032, 37, 0, DTOR(0), 0, -1, 30, 81, -1, -1, 25, 81, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 1.6, 1.75 },
	{ ANIM_0003, 27, 0, DTOR(0), 0, -1, 22, 61, -1, -1, 17, 61, BADDTOR(50), BADDTOR(-15), BADDTOR(40), BADDTOR(-40), 2,   1    },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80065910 = { var80065838, 0xffffffff };

struct attackanimconfig var80065918[] = {
	{ ANIM_0004, 19, 0, DTOR(90), 0, -1, 19, 61, -1, -1, 14, 61, BADDTOR(50), BADDTOR(-20), BADDTOR(25), BADDTOR(-60), 2.5, 2.5 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var800659a8 = { var80065918, 0xffffffff };

struct attackanimconfig var800659b0[] = {
	{ ANIM_0006, 27, 0, DTOR(0), 0, -1, 39, 74, -1, -1, 34, 74, BADDTOR(50), BADDTOR(-40), BADDTOR(45), BADDTOR(-40), 1.5, 1.5 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80065a40 = { var800659b0, 0xffffffff };

struct attackanimconfig var80065a48[] = {
	{ ANIM_034A, 20, 0, DTOR(0), 0, -1, 25, 50, -1, -1, 10, 50, BADDTOR(20), BADDTOR(-20), BADDTOR(30), BADDTOR(-30), 1, 1 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80065ad8 = { var80065a48, 0xffffffff };

struct attackanimgroup *g_StandHeavyAttackAnims[][32] = {
	{
		// RACE_HUMAN
		&var80065750, &var80065830, &var80065830, &var80065830,
		&var80065830, &var80065830, &var80065830, &var80065830,
		&var80065830, &var80065830, &var800659a8, &var800659a8,
		&var800659a8, &var800659a8, &var800659a8, &var800659a8,
		&var80065a40, &var80065a40, &var80065a40, &var80065a40,
		&var80065a40, &var80065a40, &var80065910, &var80065910,
		&var80065910, &var80065910, &var80065910, &var80065910,
		&var80065910, &var80065910, &var80065910, &var80065750,
	}, {
		// RACE_SKEDAR
		&var80065ad8, &var80065ad8, &var80065ad8, &var80065ad8,
		&var80065ad8, &var80065ad8, &var80065ad8, &var80065ad8,
		&var80065ad8, &var80065ad8, &var80065ad8, &var80065ad8,
		&var80065ad8, &var80065ad8, &var80065ad8, &var80065ad8,
		&var80065ad8, &var80065ad8, &var80065ad8, &var80065ad8,
		&var80065ad8, &var80065ad8, &var80065ad8, &var80065ad8,
		&var80065ad8, &var80065ad8, &var80065ad8, &var80065ad8,
		&var80065ad8, &var80065ad8, &var80065ad8, &var80065ad8,
	},
};

struct attackanimconfig var80065be0[] = {
	{ ANIM_0041, 26, 0, DTOR(0), 12, 140, 58, 92, 60, 79, 20, 120, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 0, 0 },
	{ ANIM_0044, 0,  0, DTOR(0), 17, 100, 25, 87, 30, 55, 20, 93,  BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-60),  0, 0 },
	{ ANIM_0045, 0,  0, DTOR(0), 12, 64,  19, 51, 24, 46, 14, 58,  BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-45),  0, 0 },
	{ ANIM_0046, 22, 0, DTOR(0), 4,  69,  22, 49, 22, 33, 8,  58,  BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-45),  0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80065d48 = { var80065be0, 0xffffffff };

struct attackanimconfig var80065d50[] = {
	{ ANIM_0041, 26, 0, DTOR(0), 12, 140, 58, 92, 60, 79, 20, 120, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 0, 0 },
	{ ANIM_0046, 22, 0, DTOR(0), 4,  69,  22, 49, 22, 33, 8,  58,  BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-45), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80065e28 = { var80065d50, 0xffffffff };

struct attackanimconfig var80065e30[] = {
	{ ANIM_0041, 26, 0, DTOR(0),  12, 140, 58, 92, 60, 79, 20, 120, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 0, 0 },
	{ ANIM_0046, 22, 0, DTOR(0),  4,  69,  22, 49, 22, 33, 8,  58,  BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-45),  0, 0 },
	{ ANIM_0049, 0,  0, DTOR(90), 7,  130, 45, 93, 56, 73, 26, 107, BADDTOR(50), BADDTOR(-40), BADDTOR(20), BADDTOR(-30), 0, 0 },
	{ ANIM_004A, 15, 0, DTOR(90), 5,  76,  20, 31, 31, 38, 15, 49,  BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-60),  0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80065f98 = { var80065e30, 0xffffffff };

struct attackanimconfig var80065fa0[] = {
	{ ANIM_0041, 26, 0, DTOR(0),   12, 140, 58, 92,  60, 79, 20, 120, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 0, 0 },
	{ ANIM_0046, 22, 0, DTOR(0),   4,  69,  22, 49,  22, 33, 8,  58,  BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-45),  0, 0 },
	{ ANIM_0047, 0,  0, DTOR(270), 7,  139, 54, 105, 61, 88, 26, 120, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-35), 0, 0 },
	{ ANIM_0048, 19, 0, DTOR(270), 4,  79,  21, 50,  26, 42, 10, 64,  BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-35), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066108 = { var80065fa0, 0xffffffff };

struct attackanimconfig var80066110[] = {
	{ ANIM_004A, 19, 0, DTOR(90), 5, 76, 20, 31, 31, 38, 15, 49, BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-60), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var800661a0 = { var80066110, 0xffffffff };

struct attackanimconfig var800661a8[] = {
	{ ANIM_0048, 19, 0, DTOR(270), 4, 79, 21, 50, 26, 42, 10, 64, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-35), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066238 = { var800661a8, 0xffffffff };

struct attackanimconfig var80066240[] = {
	{ ANIM_034A, 20, 0, DTOR(0), 0, -1, 25, 50, -1, -1, 10, 50, BADDTOR(20), BADDTOR(-20), BADDTOR(30), BADDTOR(-30), 1, 1 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var800662d0 = { var80066240, 0xffffffff };

struct attackanimgroup *g_StandLightAttackAnims[][32] = {
	{
		// RACE_HUMAN
		&var80065d48, &var80065d48, &var80065e28, &var80065e28,
		&var80065e28, &var80065f98, &var80065f98, &var80065f98,
		&var80065f98, &var80065f98, &var800661a0, &var800661a0,
		&var800661a0, &var800661a0, &var800661a0, &var800661a0,
		&var80066238, &var80066238, &var80066238, &var80066238,
		&var80066238, &var80066238, &var80066108, &var80066108,
		&var80066108, &var80066108, &var80066108, &var80065e28,
		&var80065e28, &var80065e28, &var80065d48, &var80065d48,
	}, {
		// RACE_SKEDAR
		&var800662d0, &var800662d0, &var800662d0, &var800662d0,
		&var800662d0, &var800662d0, &var800662d0, &var800662d0,
		&var800662d0, &var800662d0, &var800662d0, &var800662d0,
		&var800662d0, &var800662d0, &var800662d0, &var800662d0,
		&var800662d0, &var800662d0, &var800662d0, &var800662d0,
		&var800662d0, &var800662d0, &var800662d0, &var800662d0,
		&var800662d0, &var800662d0, &var800662d0, &var800662d0,
		&var800662d0, &var800662d0, &var800662d0, &var800662d0,
	},
};

struct attackanimconfig var800663d8[] = {
	{ ANIM_007A, 26, 0, DTOR(0), 7, 92, 28, 68, -1, -1, 11, 73, BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066468 = { var800663d8, 0xffffffff };

struct attackanimconfig var80066470[] = {
	{ ANIM_007B, 26, 0, DTOR(90), 9,  112, 38, 87, -1, -1, 19, 98, BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-25), 0, 0 },
	{ ANIM_007D, 25, 0, DTOR(90), 10, 112, 32, 86, -1, -1, 19, 97, BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-25), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066548 = { var80066470, 0xffffffff };

struct attackanimconfig var80066550[] = {
	{ ANIM_007C, 39, 0, DTOR(270), 22, 127, 44, 102, -1, -1, 28, 112, BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-25), 0, 0 },
	{ ANIM_007E, 39, 0, DTOR(270), 23, 130, 46, 100, -1, -1, 30, 110, BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-25), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066628 = { var80066550, 0xffffffff };

struct attackanimconfig var80066630[] = {
	{ ANIM_034A, 20, 0, DTOR(0), 0, -1, 25, 50, -1, -1, 10, 50, BADDTOR(20), BADDTOR(-20), BADDTOR(30), BADDTOR(-30), 1, 1 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var800666c0 = { var80066630, 0xffffffff };

struct attackanimgroup *g_StandDualAttackAnims[][32] = {
	{
		// RACE_HUMAN
		&var80066468, &var80066468, &var80066468, &var80066468,
		&var80066468, &var80066548, &var80066548, &var80066548,
		&var80066548, &var80066548, &var80066548, &var80066548,
		&var80066548, &var80066548, &var80066548, &var80066548,
		&var80066628, &var80066628, &var80066628, &var80066628,
		&var80066628, &var80066628, &var80066628, &var80066628,
		&var80066628, &var80066628, &var80066628, &var80066468,
		&var80066468, &var80066468, &var80066468, &var80066468,
	}, {
		// RACE_SKEDAR
		&var800666c0, &var800666c0, &var800666c0, &var800666c0,
		&var800666c0, &var800666c0, &var800666c0, &var800666c0,
		&var800666c0, &var800666c0, &var800666c0, &var800666c0,
		&var800666c0, &var800666c0, &var800666c0, &var800666c0,
		&var800666c0, &var800666c0, &var800666c0, &var800666c0,
		&var800666c0, &var800666c0, &var800666c0, &var800666c0,
		&var800666c0, &var800666c0, &var800666c0, &var800666c0,
		&var800666c0, &var800666c0, &var800666c0, &var800666c0,
	},
};

struct attackanimconfig var800667c8[] = {
	{ ANIM_0007,                 27, 0, DTOR(0), 0, -1, 35, 75, -1, -1, 31, 75, BADDTOR(50), BADDTOR(-40), BADDTOR(52), BADDTOR(-40), 1.5, 1.5 },
	{ ANIM_KNEEL_TWO_HANDED_GUN, 24, 0, DTOR(0), 0, -1, 46, 98, -1, -1, 41, 98, BADDTOR(50), BADDTOR(-30), BADDTOR(65), BADDTOR(-40), 1.6, 1.6 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var800668a0 = { var800667c8, 0xffffffff };

struct attackanimconfig var800668a8[] = {
	{ ANIM_0009, 26, 0, DTOR(0), 0, -1, 34, 87, -1, -1, 29, 87, BADDTOR(50), BADDTOR(-30), BADDTOR(40), BADDTOR(-55), 1.6, 2 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066938 = { var800668a8, 0xffffffff };

struct attackanimconfig var80066940[] = {
	{ ANIM_000A, 28, 0, DTOR(0), 0, -1, 36, 88, -1, -1, 31, 88, BADDTOR(50), BADDTOR(-40), BADDTOR(50), BADDTOR(-25), 1.6, 1.5 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var800669d0 = { var80066940, 0xffffffff };

struct attackanimconfig var800669d8[] = {
	{ ANIM_034A, 20, 0, DTOR(0), 0, -1, 25, 50, -1, -1, 10, 50, BADDTOR(20), BADDTOR(-20), BADDTOR(30), BADDTOR(-30), 1, 1 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066a68 = { var800669d8, 0xffffffff };

struct attackanimgroup *g_KneelHeavyAttackAnims[][32] = {
	{
		&var800668a0, &var800668a0, &var800668a0, &var800668a0,
		&var800668a0, &var800668a0, &var800668a0, &var800668a0,
		&var800668a0, &var800668a0, &var80066938, &var80066938,
		&var80066938, &var80066938, &var80066938, &var80066938,
		&var800669d0, &var800669d0, &var800669d0, &var800669d0,
		&var800669d0, &var800669d0, &var800668a0, &var800668a0,
		&var800668a0, &var800668a0, &var800668a0, &var800668a0,
		&var800668a0, &var800668a0, &var800668a0, &var800668a0,
	}, {
		&var80066a68, &var80066a68, &var80066a68, &var80066a68,
		&var80066a68, &var80066a68, &var80066a68, &var80066a68,
		&var80066a68, &var80066a68, &var80066a68, &var80066a68,
		&var80066a68, &var80066a68, &var80066a68, &var80066a68,
		&var80066a68, &var80066a68, &var80066a68, &var80066a68,
		&var80066a68, &var80066a68, &var80066a68, &var80066a68,
		&var80066a68, &var80066a68, &var80066a68, &var80066a68,
		&var80066a68, &var80066a68, &var80066a68, &var80066a68,
	},
};

struct attackanimconfig var80066b70[] = {
	{ ANIM_KNEEL_SHOOT_RIGHT_HAND, 25, 0, DTOR(0), 12, 132, 55, 87, 67, 87, 26, 111, BADDTOR(50), BADDTOR(-40), BADDTOR(35), BADDTOR(-45), 0, 0 },
	{ ANIM_004C,                   26, 0, DTOR(0), 8,  89,  31, 63, 41, 51, 21, 80,  BADDTOR(50), BADDTOR(-40), BADDTOR(20), BADDTOR(-65), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066c48 = { var80066b70, 0xffffffff };

struct attackanimconfig var80066c50[] = {
	{ ANIM_004F, 47, 0, DTOR(90), 7,  128, 33, 86, 47, 74, 23, 106, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-45),  0,   0 },
	{ ANIM_0050, 18, 0, DTOR(90), 7,  78,  28, 52, 35, 45, 15, 66,  BADDTOR(50), BADDTOR(-5),  BADDTOR(40), BADDTOR(-45),  1.5, 1 },
	{ ANIM_0051, 20, 0, DTOR(90), 13, 92,  37, 67, 42, 55, 25, 84,  BADDTOR(50), BADDTOR(-30), BADDTOR(20), BADDTOR(-40), 0,   0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066d70 = { var80066c50, 0xffffffff };

struct attackanimconfig var80066d78[] = {
	{ ANIM_004D, 28, 0, DTOR(270), 15, 124, 38, 97, 60, 84, 20, 106, BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-50), 0, 0 },
	{ ANIM_004E, 23, 0, DTOR(270), 0,  85,  32, 38, 38, 60, 14, 71,  BADDTOR(50), BADDTOR(-40), BADDTOR(35), BADDTOR(-55), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066e50 = { var80066d78, 0xffffffff };

struct attackanimconfig var80066e58[] = {
	{ ANIM_034A, 20, 0, DTOR(0), 0, -1, 25, 50, -1, -1, 10, 50, BADDTOR(20), BADDTOR(-20), BADDTOR(30), BADDTOR(-30), 1, 1 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80066ee8 = { var80066e58, 0xffffffff };

struct attackanimgroup *g_KneelLightAttackAnims[][32] = {
	{
		&var80066c48, &var80066c48, &var80066c48, &var80066c48,
		&var80066c48, &var80066c48, &var80066c48, &var80066c48,
		&var80066c48, &var80066c48, &var80066d70, &var80066d70,
		&var80066d70, &var80066d70, &var80066d70, &var80066d70,
		&var80066e50, &var80066e50, &var80066e50, &var80066e50,
		&var80066e50, &var80066e50, &var80066c48, &var80066c48,
		&var80066c48, &var80066c48, &var80066c48, &var80066c48,
		&var80066c48, &var80066c48, &var80066c48, &var80066c48,
	}, {
		&var80066ee8, &var80066ee8, &var80066ee8, &var80066ee8,
		&var80066ee8, &var80066ee8, &var80066ee8, &var80066ee8,
		&var80066ee8, &var80066ee8, &var80066ee8, &var80066ee8,
		&var80066ee8, &var80066ee8, &var80066ee8, &var80066ee8,
		&var80066ee8, &var80066ee8, &var80066ee8, &var80066ee8,
		&var80066ee8, &var80066ee8, &var80066ee8, &var80066ee8,
		&var80066ee8, &var80066ee8, &var80066ee8, &var80066ee8,
		&var80066ee8, &var80066ee8, &var80066ee8, &var80066ee8,
	},
};

struct attackanimconfig var80066ff0[] = {
	{ ANIM_0074, 22, 0, DTOR(0), 10, 111, 34, 87, -1, -1, 17, 104, BADDTOR(50), BADDTOR(-40), BADDTOR(35), BADDTOR(-45), 0, 0 },
	{ ANIM_0077, 25, 0, DTOR(0), 9,  92,  33, 62, -1, -1, 18, 69,  BADDTOR(50), BADDTOR(-40), BADDTOR(35), BADDTOR(-45), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var800670c8 = { var80066ff0, 0xffffffff };

struct attackanimconfig var800670d0[] = {
	{ ANIM_0075, 28, 0, DTOR(90), 15, 108, 34, 73, -1, -1, 17, 93, BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-45), 0,   0 },
	{ ANIM_0078, 19, 0, DTOR(90), 3,  95,  30, 64, -1, -1, 14, 71, BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-45), 1.5, 1 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var800671a8 = { var800670d0, 0xffffffff };

struct attackanimconfig var800671b0[] = {
	{ ANIM_0076, 31, 0, DTOR(270), 14, 111, 40, 83, -1, -1, 21, 94, BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-45), 0, 0 },
	{ ANIM_0079, 26, 0, DTOR(270), 7,  89,  34, 60, -1, -1, 20, 68, BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-45), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80067288 = { var800671b0, 0xffffffff };

struct attackanimconfig var80067290[] = {
	{ ANIM_034A, 20, 0, DTOR(0), 0, -1, 25, 50, -1, -1, 10, 50, BADDTOR(20), BADDTOR(-20), BADDTOR(30), BADDTOR(-30), 1, 1 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80067320 = { var80067290, 0xffffffff };

struct attackanimgroup *g_KneelDualAttackAnims[][32] = {
	{
		&var800670c8, &var800670c8, &var800670c8, &var800670c8,
		&var800670c8, &var800670c8, &var800670c8, &var800670c8,
		&var800670c8, &var800670c8, &var800671a8, &var800671a8,
		&var800671a8, &var800671a8, &var800671a8, &var800671a8,
		&var80067288, &var80067288, &var80067288, &var80067288,
		&var80067288, &var80067288, &var800670c8, &var800670c8,
		&var800670c8, &var800670c8, &var800670c8, &var800670c8,
		&var800670c8, &var800670c8, &var800670c8, &var800670c8,
	}, {
		&var80067320, &var80067320, &var80067320, &var80067320,
		&var80067320, &var80067320, &var80067320, &var80067320,
		&var80067320, &var80067320, &var80067320, &var80067320,
		&var80067320, &var80067320, &var80067320, &var80067320,
		&var80067320, &var80067320, &var80067320, &var80067320,
		&var80067320, &var80067320, &var80067320, &var80067320,
		&var80067320, &var80067320, &var80067320, &var80067320,
		&var80067320, &var80067320, &var80067320, &var80067320,
	},
};

struct attackanimconfig g_RollAttackAnims[] = {
	{ ANIM_000B, 76, 0, DTOR(0),  20, -1,  98, 161, -1, -1, 93, 161, BADDTOR(50), BADDTOR(-30), BADDTOR(40), BADDTOR(-40), 1.7,  2   },
	{ ANIM_000C, 58, 0, DTOR(0),  10, -1,  77, 104, -1, -1, 72, 104, BADDTOR(50), BADDTOR(-20), BADDTOR(35), BADDTOR(-40), 1.55, 1.5 },
	{ ANIM_000D, 61, 0, DTOR(0),  10, -1,  83, 128, -1, -1, 78, 128, BADDTOR(50), BADDTOR(-30), BADDTOR(50), BADDTOR(-30), 1.2,  1.3 },
	{ ANIM_0027, 63, 0, DTOR(0),  10, -1,  73, 114, -1, -1, 68, 114, BADDTOR(50), BADDTOR(-30), BADDTOR(35), BADDTOR(-35), 1.65, 1.5 },
	{ ANIM_000B, 76, 0, DTOR(0),  20, 76,  98, 161, -1, -1, 93, 161, BADDTOR(50), BADDTOR(-30), BADDTOR(40), BADDTOR(-40), 1.7,  2   },
	{ ANIM_000C, 58, 0, DTOR(0),  10, 63,  77, 104, -1, -1, 72, 104, BADDTOR(50), BADDTOR(-20), BADDTOR(35), BADDTOR(-40), 1.55, 1.5 },
	{ ANIM_000D, 61, 0, DTOR(0),  10, 56,  83, 128, -1, -1, 78, 128, BADDTOR(50), BADDTOR(-30), BADDTOR(50), BADDTOR(-30), 1.2,  1.3 },
	{ ANIM_0027, 63, 0, DTOR(0),  10, 50,  73, 114, -1, -1, 68, 114, BADDTOR(50), BADDTOR(-30), BADDTOR(35), BADDTOR(-35), 1.65, 1.5 },
	{ ANIM_0045, 0,  0, DTOR(0),  7,  64,  19, 51,  24, 46, 14, 58,  BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-45),  0,    0   },
	{ ANIM_004A, 0,  0, DTOR(90), 14, 76,  26, 31,  31, 38, 15, 49,  BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-60),  0,    0   },
	{ ANIM_004C, 26, 0, DTOR(0),  25, 89,  41, 63,  41, 51, 21, 80,  BADDTOR(50), BADDTOR(-40), BADDTOR(20), BADDTOR(-65),  0,    0   },
	{ ANIM_0050, 18, 0, DTOR(90), 11, 78,  33, 52,  35, 45, 15, 66,  BADDTOR(50), BADDTOR(-5),  BADDTOR(40), BADDTOR(-45),  1.5,  1   },
	{ ANIM_007A, 26, 0, DTOR(0),  7,  92,  28, 68,  -1, -1, 11, 73,  BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 0,    0   },
	{ ANIM_007B, 26, 0, DTOR(90), 9,  112, 38, 87,  -1, -1, 19, 98,  BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-25), 0,    0   },
	{ ANIM_0074, 22, 0, DTOR(0),  10, 11,  34, 87,  -1, -1, 17, 104, BADDTOR(50), BADDTOR(-40), BADDTOR(35), BADDTOR(-45),  0,    0   },
	{ ANIM_0075, 28, 0, DTOR(90), 15, 108, 34, 73,  -1, -1, 17, 93,  BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-45),  0,    0   },
	{ ANIM_007A, 26, 0, DTOR(0),  7,  92,  28, 68,  -1, -1, 11, 73,  BADDTOR(50), BADDTOR(-40), BADDTOR(40), BADDTOR(-40), 0,    0   },
	{ ANIM_007D, 25, 0, DTOR(90), 10, 112, 32, 86,  -1, -1, 19, 97,  BADDTOR(50), BADDTOR(-40), BADDTOR(25), BADDTOR(-25), 0,    0   },
	{ ANIM_0077, 25, 0, DTOR(0),  9,  92,  33, 62,  -1, -1, 18, 69,  BADDTOR(50), BADDTOR(-40), BADDTOR(35), BADDTOR(-45),  0,    0   },
	{ ANIM_0078, 19, 0, DTOR(90), 3,  95,  30, 64,  -1, -1, 14, 71,  BADDTOR(50), BADDTOR(-40), BADDTOR(30), BADDTOR(-45),  1.5,  1   },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimconfig g_WalkAttackAnims[] = {
	{ ANIM_0030, 0, 0, DTOR(0),   0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 1.4, 1.3 },
	{ ANIM_0031, 0, 0, DTOR(0),   0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 1.1, 1.2 },
	{ ANIM_0052, 0, 0, DTOR(0),   0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0,   0   },
	{ ANIM_0055, 0, 0, DTOR(0),   0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0,   0   },
	{ ANIM_006C, 0, 0, DTOR(0),   0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0,   0   },
	{ ANIM_006E, 0, 0, DTOR(0),   0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0,   0   },
	{ ANIM_006D, 0, 0, DTOR(0),   0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0,   0   },
	{ ANIM_006F, 0, 0, DTOR(0),   0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0,   0   },
	{ ANIM_0057, 0, 0, DTOR(90),  0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0,   0   },
	{ ANIM_0056, 0, 0, DTOR(270), 0, -1, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0,   0   },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimconfig g_SnipeAttackAnims[] = {
	{ ANIM_SNIPING_GETDOWN,  0, 0, DTOR(0), 0, 236, 0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0, 0 },
	{ ANIM_SNIPING_ONGROUND, 0, 0, DTOR(0), 0, -1,  0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0, 0 },
	{ ANIM_SNIPING_GETUP,    0, 0, DTOR(0), 0, -1,  0, 0, -1, -1, 0, 0, BADDTOR(50), BADDTOR(-30), BADDTOR(30), BADDTOR(-30), 0, 0 },
	{ 0, 0, 0, 0, 0, -1 },
};

struct attackanimgroup var80067e48 = { g_SnipeAttackAnims, 0xffffffff };

struct attackanimgroup *g_LieAttackAnims = &var80067e48;

u32 g_StageFlags = 0;

struct chrdata *g_BgChrs = NULL;
s16 *g_BgChrnums = NULL;
s32 g_NumBgChrs = 0;

s16 *g_TeamList = NULL;
s16 *g_SquadronList = NULL;

struct animdist g_ChrAvgAnimMoveDistsHuman[] = {
	{ ANIM_0028,               0 },
	{ ANIM_RUNNING_TWOHANDGUN, 0 },
	{ ANIM_0029,               0 },
	{ ANIM_006B,               0 },
	{ ANIM_RUNNING_ONEHANDGUN, 0 },
	{ ANIM_005A,               0 },
	{ ANIM_0072,               0 },
	{ ANIM_0073,               0 },
	{ ANIM_005A,               0 },
	{ ANIM_006C,               0 },
	{ ANIM_0030,               0 },
	{ ANIM_0031,               0 },
	{ ANIM_0052,               0 },
	{ ANIM_0055,               0 },
	{ ANIM_006E,               0 },
	{ ANIM_006F,               0 },
	{ ANIM_0057,               0 },
	{ ANIM_0056,               0 },
	{ ANIM_006D,               0 },
	{ ANIM_RUNNING_ONEHANDGUN, 0 },
	{ ANIM_020A,               0 },
	{ ANIM_020D,               0 },
	{ ANIM_01F9,               0 },
	{ ANIM_01F8,               0 },
	{ ANIM_021D,               0 },
	{ ANIM_0016,               0 },
	{ ANIM_0018,               0 },
	{ ANIM_001B,               0 },
	{ ANIM_001D,               0 },
	{ ANIM_001E,               0 },
	{ ANIM_005C,               0 },
	{ ANIM_005D,               0 },
	{ ANIM_005E,               0 },
	{ ANIM_005F,               0 },
	{ -1 },
};

struct animdist g_ChrAvgAnimMoveDistsSkedar[] = {
	{ ANIM_0392,           0 },
	{ ANIM_0393,           0 },
	{ ANIM_SKEDAR_RUNNING, 0 },
	{ -1 },
};

struct animdist g_ChrAvgAnimMoveDistsDrCaroll[] = {
	{ ANIM_015F, 0 },
	{ ANIM_0160, 0 },
	{ -1 },
};

struct animdist g_ChrAvgAnimMoveDistsEyespy[] = {
	{ ANIM_015F, 0 },
	{ -1 },
};

struct animdist g_ChrAvgAnimMoveDistsRobot[] = {
	{ ANIM_0238, 0 },
	{ -1 },
};

struct animdist *g_ChrAvgAnimMoveDists[] = {
	g_ChrAvgAnimMoveDistsHuman,
	g_ChrAvgAnimMoveDistsSkedar,
	g_ChrAvgAnimMoveDistsDrCaroll,
	g_ChrAvgAnimMoveDistsEyespy,
	g_ChrAvgAnimMoveDistsRobot,
};

void chr_attack(struct chrdata *chr, struct attackanimgroup **animgroups, bool flip, bool *firing, u32 attackflags, s32 entityid, bool standing);

void chr_damage(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset,
		struct prop *aprop, s32 hitpart, bool damageshield, struct prop *prop2,
		struct modelnode *node, struct model *model, s32 side, s16 *hitpos,
		bool explosion, struct coord *explosionpos);

void chr_fade_corpse(struct chrdata *chr);
s32 chr_turn(struct chrdata *chr, s32 turning, f32 endanimframe, f32 speed, f32 toleranceangle);
void robot_attack(struct chrdata *chr);
s32  chr_skjump(struct chrdata *chr, u8 pouncebits, u8 arg2, s32 arg3, u8 arg4);
void chr_get_attack_entity_pos(struct chrdata *chr, u32 attackflags, s32 entityid, struct coord *pos, RoomNum *rooms);
f32 prop_get_distance_to_prop(struct prop *a, struct prop *b);
s32 chr_set_chr_preset_to_chr_near_pos(u8 checktype, struct chrdata *chr, f32 distance, struct coord *pos, RoomNum *rooms);

f32 chr_get_anim_movedist_per_frame(s16 animnum)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_ChrAvgAnimMoveDists); i++) {
		s32 j = 0;
		s16 thisanimnum = g_ChrAvgAnimMoveDists[i][j].animnum;

		while (thisanimnum >= 0) {
			if (thisanimnum == animnum) {
				return g_ChrAvgAnimMoveDists[i][j].value;
			}

			j++;
			thisanimnum = g_ChrAvgAnimMoveDists[i][j].animnum;
		}
	}

	return 1.0f;
}

bool chr_gopos_is_waiting(struct chrdata *chr)
{
	static s16 list1[] = { ANIM_TWO_GUN_HOLD, ANIM_006A, -1 };
	static s16 list2[] = { ANIM_00C0, -1 };
	static s16 list3[] = { ANIM_013E, -1 };
	static s16 list4[] = { ANIM_013E, -1 };
	static s16 list5[] = { ANIM_0237, -1 };
	static s16 *waitableanims[] = { list1, list2, list3, list4, list5 };

	if (chr->aibot) {
		if (chr->actiontype == ACT_STAND
				|| (chr->actiontype == ACT_GOPOS && (chr->act_gopos.flags & GOPOSFLAG_WAITING))) {
			return true;
		}
	} else {
		s16 animnum = model_get_anim_num(chr->model);
		s32 i;

		for (i = 0; i < ARRAYCOUNT(waitableanims); i++) {
			s16 thisanimnum;
			s32 j;

			for (j = 0; (thisanimnum = waitableanims[i][j]) >= 0; j++) {
				if (thisanimnum == animnum) {
					return true;
				}
			}
		}
	}

	return false;
}

bool weapon_is_onehanded(struct prop *prop)
{
	if (prop) {
		struct weaponobj *weapon = prop->weapon;
		return gset_has_weapon_flag(weapon->weaponnum, WEAPONFLAG_ONEHANDED);
	}

	return false;
}

/**
 * Returns a value between min and max based on the chr's speedrating property.
 *
 * chr->speedrating is between 0 and 100. The result is scaled between min and
 * max accordingly.
 *
 * This function also applies the PD mode reaction speed, but the PD mode
 * reaction speed is always zero because PD doesn't have it in the settings.
 * It was used in GE but disabled in PD.
 */
f32 chr_get_ranged_speed(struct chrdata *chr, f32 min, f32 max)
{
	f32 speedrating = chr->speedrating;

	speedrating = pdmode_get_enemy_reaction_speed() * (100.0f - speedrating) + speedrating;

	return (max - min) * speedrating * 0.01f + min;
}

/**
 * Calculates a percentage of how slow the chr is.
 *
 * percentage is expected to be between 0 and 100.
 * chr->speedrating is between 0 and 100.
 *
 * This function takes the difference between the speedrating and the max (100),
 * then multiplies that amount by the given percentage and returns it.
 *
 * For example, if the chr's speedrating is 10 (out of 100) and the given
 * percentage is 50, the result will be 45.
 */
s32 chr_get_percentage_of_slowness(struct chrdata *chr, s32 percentage)
{
	s32 speedrating = chr->speedrating;
	s32 extra = pdmode_get_enemy_reaction_speed() * (100 - speedrating);

	speedrating = extra + speedrating;

	return (100 - speedrating) * percentage / 100;
}

f32 chr_get_ranged_argh_speed(struct chrdata *chr, f32 min, f32 max)
{
	f32 arghrating = chr->arghrating;

	arghrating = pdmode_get_enemy_reaction_speed() * (100.0f - arghrating) + arghrating;

	return (max - min) * arghrating * 0.01f + min;
}

f32 chr_get_attack_entity_relative_angle(struct chrdata *chr, s32 attackflags, s32 entityid)
{
	f32 angle;
	struct coord pos;
	RoomNum rooms[8];

	if (attackflags & ATTACKFLAG_AIMFORWARD) {
		return 0;
	}

	if (attackflags & ATTACKFLAG_AIMATDIRECTION) {
		angle = entityid * (BADDTOR(360) / 65536);
		angle -= chr_get_theta(chr);

		if (angle < 0) {
			angle += BADDTOR(360);
		}

		return angle;
	}

	chr_get_attack_entity_pos(chr, attackflags, entityid, &pos, rooms);

	return chr_get_angle_to_pos(chr, &pos);
}

f32 chr_get_attack_entity_distance(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	if (attackflags & ATTACKFLAG_AIMATTARGET) {
		return chr_get_distance_to_target(chr);
	}

	if (attackflags & ATTACKFLAG_AIMATCHR) {
		return chr_get_distance_to_chr(chr, entityid);
	}

	if (attackflags & ATTACKFLAG_AIMATPAD) {
		return chr_get_distance_to_pad(chr, entityid);
	}

	return 0;
}

/**
 * Given the two ends of an edge, and a push-from position and direction,
 * calculate the position of contact on the edge.
 */
void chr_calculate_push_contact_pos(struct coord *edge1, struct coord *edge2, struct coord *pushfrompos, struct coord *dir, struct coord *dst)
{
	f32 value = dir->z * (edge2->x - edge1->x) - (edge2->z - edge1->z) * dir->x;

	if (value != 0) {
		f32 tmp = ((edge2->z - edge1->z) * (pushfrompos->x - edge1->x) + (edge1->z - pushfrompos->z) * (edge2->x - edge1->x)) / value;
		dst->x = dir->x * tmp + pushfrompos->x;
		dst->y = dir->y * tmp + pushfrompos->y;
		dst->z = dir->z * tmp + pushfrompos->z;
	} else if (dir->x == 0 && dir->z == 0) {
		dst->x = pushfrompos->x;
		dst->y = pushfrompos->y;
		dst->z = pushfrompos->z;
	} else {
		dst->x = edge1->x;
		dst->y = edge1->y;
		dst->z = edge1->z;
	}
}

void chr_calculate_push_contact_pos_using_saved_edge(struct coord *pushfrompos, struct coord *dir, struct coord *dst)
{
	struct coord edge1;
	struct coord edge2;

#if VERSION >= VERSION_PAL_FINAL
	cd_get_edge(&edge1, &edge2, 2298, "chr/chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
	cd_get_edge(&edge1, &edge2, 2298, "chraction.c");
#else
	cd_get_edge(&edge1, &edge2, 2338, "chraction.c");
#endif

	chr_calculate_push_contact_pos(&edge1, &edge2, pushfrompos, dir, dst);
}

f32 chr_prop_get_available_dist_at_angle(struct prop *prop, f32 angle, f32 scandist, u32 cdtypes, f32 ymax, f32 ymin)
{
	struct coord dir;
	struct coord farpos;
	struct chrdata *chr = prop->chr;
	f32 result;
	struct coord sp3c;
	f32 xdiff;
	f32 zdiff;

	dir.x = sinf(angle);
	dir.y = 0;
	dir.z = cosf(angle);

	farpos.x = prop->pos.x + dir.f[0] * scandist;
	farpos.y = prop->pos.y;
	farpos.z = prop->pos.z + dir.f[2] * scandist;

	chr_set_perim_enabled(chr, false);

	if (cd_exam_cyl_move03(&prop->pos, prop->rooms, &farpos, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION) {
		result = scandist;
	} else {
#if VERSION >= VERSION_PAL_FINAL
		cd_get_pos(&sp3c, 2377, "chr/chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
		cd_get_pos(&sp3c, 2377, "chraction.c");
#else
		cd_get_pos(&sp3c, 2417, "chraction.c");
#endif

		xdiff = sp3c.x - prop->pos.x;
		zdiff = sp3c.z - prop->pos.z;

		result = sqrtf(xdiff * xdiff + zdiff * zdiff);
	}

	chr_set_perim_enabled(chr, true);

	return result;
}

f32 chr_get_available_dist_at_angle(struct prop *prop, f32 angle, f32 scandist)
{
	f32 ymax;
	f32 ymin;
	f32 radius;

	chr_get_bbox(prop, &radius, &ymax, &ymin);

	return chr_prop_get_available_dist_at_angle(prop, angle, scandist, CDTYPE_ALL, ymax, ymin);
}

void chr_choose_stand_animation(struct chrdata *chr, f32 mergetime)
{
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	s32 race = CHRRACE(chr);
	s32 prevanimnum = model_get_anim_num(chr->model);

	if (chr->actiontype == ACT_GOPOS) {
		chr->act_gopos.flags |= GOPOSFLAG_WAITING;
	}

	if (chr->aibot) {
		return;
	}

	if (race == RACE_EYESPY) {
		model_set_animation(chr->model, ANIM_013E, 0, 0, 0, mergetime);
	} else if (race == RACE_HUMAN) {
		if (prevanimnum == ANIM_SNIPING_GETDOWN
				|| prevanimnum == ANIM_SNIPING_GETUP
				|| prevanimnum == ANIM_SNIPING_ONGROUND) {
			model_set_animation(chr->model, ANIM_SNIPING_GETUP, chr->model->anim->flip, -1, chr_get_ranged_speed(chr, 0.5, 0.8), 16);
		} else if ((leftgun && rightgun) || (!leftgun && !rightgun)
				|| weapon_is_onehanded(leftgun)
				|| weapon_is_onehanded(rightgun)) {
			model_set_animation(chr->model, ANIM_006A, random() % 2, 0, 0.25, mergetime);
			model_set_anim_looping(chr->model, 0, 16);
		} else if (rightgun || leftgun) {
			model_set_animation(chr->model, ANIM_TWO_GUN_HOLD, leftgun != NULL, 0, 0.25, mergetime);
			model_set_anim_looping(chr->model, 0, 16);
			model_set_anim_end_frame(chr->model, 120);
		}
	} else if (race == RACE_SKEDAR) {
		model_set_animation(chr->model, ANIM_00C0, random() % 2, 0, 0.5, mergetime);
	} else if (race == RACE_DRCAROLL) {
		model_set_animation(chr->model, ANIM_013E, 0, 0, 0.5, mergetime);
	} else if (race == RACE_ROBOT) {
		model_set_animation(chr->model, ANIM_0237, 0, 0, 0.5, mergetime);
	}
}

void chr_stand_immediate(struct chrdata *chr, f32 mergetime)
{
	f32 limit = 127;
	f32 fsleep;

	chr_stop_firing(chr);
	chr->actiontype = ACT_STAND;
	chr->act_stand.prestand = false;
	chr->act_stand.flags = 0;
	chr->act_stand.entityid = 0;
	chr->act_stand.reaim = 0;
	chr->act_stand.turning = TURNSTATE_OFF;
	chr->act_stand.checkfacingwall = false;
	chr->act_stand.wallcount = random() % 120 + 180; // 180 to 299
	chr->act_stand.mergetime = mergetime;
	chr->act_stand.playwalkanim = false;

	fsleep = mergetime;

	if (chr->model->anim->playspeed != PALUPF(1.0f)) {
		fsleep *= PALUPF(1.0f) / chr->model->anim->playspeed;
	}

	if (fsleep > limit) {
		fsleep = limit;
	}

	chr->sleep = fsleep;

	if (model_is_anim_merging(chr->model) && !chr->aibot) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_choose_stand_animation(chr, mergetime);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_stand(struct chrdata *chr)
{
	s32 race = CHRRACE(chr);
	f32 result;

	if (race != RACE_EYESPY) {
		chr_stop_firing(chr);

		if (race == RACE_HUMAN && chr->actiontype == ACT_KNEEL) {
			chr_stop_firing(chr);

			chr->actiontype = ACT_STAND;
			chr->act_stand.prestand = true;
			chr->act_stand.flags = 0;
			chr->act_stand.entityid = 0;
			chr->act_stand.reaim = 0;
			chr->act_stand.turning = TURNSTATE_OFF;
			chr->act_stand.checkfacingwall = false;
			chr->act_stand.wallcount = random() % 120 + 180;
			chr->sleep = 0;
			chr->act_stand.playwalkanim = false;

			if (chr->aibot == NULL) {
				if (model_get_anim_num(chr->model) == ANIM_KNEEL_SHOOT_RIGHT_HAND) {
					result = chr_get_ranged_speed(chr, 0.5, 0.8);
					model_set_animation(chr->model, ANIM_KNEEL_SHOOT_RIGHT_HAND,
							chr->model->anim->flip, 109, result, 16);
					model_set_anim_end_frame(chr->model, 140);
				} else {
					result = chr_get_ranged_speed(chr, 0.5, 0.8);
					model_set_animation(chr->model, ANIM_KNEEL_TWO_HANDED_GUN,
							chr->model->anim->flip, 120, result, 16);
					model_set_anim_end_frame(chr->model, 151);
				}
			}
		} else if (race == RACE_DRCAROLL || race == RACE_ROBOT) {
			chr->actiontype = ACT_STAND;
			chr->act_stand.prestand = true;
			chr->act_stand.flags = 0;
			chr->act_stand.entityid = 0;
			chr->act_stand.reaim = 0;
			chr->act_stand.turning = TURNSTATE_OFF;
			chr->act_stand.checkfacingwall = false;
			chr->act_stand.wallcount = random() % 120 + 180;
			chr->sleep = 0;
			chr->act_stand.playwalkanim = false;

			chr_stand_immediate(chr, 16);
		} else {
			chr_stand_immediate(chr, 16);
		}
	}
}

bool chr_face_cover(struct chrdata *chr)
{
	struct cover cover;

	if (!cover_unpack(chr->cover, &cover)) {
		return false;
	}

	chr_stand(chr);
	chr->act_stand.reaim = 0;
	chr->act_stand.flags = ATTACKFLAG_AIMATDIRECTION;
	chr->act_stand.turning = TURNSTATE_TURNING;
	chr->act_stand.entityid = atan2f(-cover.look->x, -cover.look->z) * (16384 / BADDTOR(90));

	return true;
}

void chr_stand_checkfacingwall(struct chrdata *chr, f32 mergetime)
{
	chr_stand_immediate(chr, mergetime);

	chr->act_stand.checkfacingwall = true;
}

void chr_stop(struct chrdata *chr)
{
	chr_stand(chr);

	chr->act_stand.checkfacingwall = true;
}

void chr_kneel_choose_animation(struct chrdata *chr)
{
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);

	if (chr->aibot == NULL) {
		if ((leftgun && rightgun)
				|| (!leftgun && !rightgun)
				|| weapon_is_onehanded(leftgun)
				|| weapon_is_onehanded(rightgun)) {
			bool flip = random() % 2;
			model_set_animation(chr->model, ANIM_KNEEL_SHOOT_RIGHT_HAND, flip, 0, chr_get_ranged_speed(chr, 0.5, 0.8), 16);
			model_set_anim_end_frame(chr->model, 28);
		} else if (rightgun || leftgun) {
			model_set_animation(chr->model, ANIM_KNEEL_TWO_HANDED_GUN, leftgun != NULL, 0, chr_get_ranged_speed(chr, 0.5, 0.8), 16);
			model_set_anim_end_frame(chr->model, 27);
		}
	}
}

void chr_kneel(struct chrdata *chr)
{
	chr_stop_firing(chr);
	chr->actiontype = ACT_KNEEL;
	chr->sleep = 0;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_kneel_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_startalarm_choose_animation(struct chrdata *chr)
{
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	bool flip = false;

	if (leftgun && !rightgun) {
		flip = true;
	} else if ((leftgun && rightgun) || (!leftgun && !rightgun)) {
		flip = random() % 2;
	}

	model_set_animation(chr->model, ANIM_TALKING_003D, flip, 40, 1, 16);
	model_set_anim_end_frame(chr->model, 82);
}

void chr_startalarm(struct chrdata *chr)
{
	chr_stop_firing(chr);
	chr->actiontype = ACT_STARTALARM;
	chr->sleep = 0;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_startalarm_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_throwgrenade_choose_animation(struct chrdata *chr)
{
	u32 rand = random();

	if (chr->act_throwgrenade.needsequip) {
		if (rand % 3 == 0) {
			model_set_animation(chr->model, ANIM_THROWGRENADE_CROUCHING, chr->act_throwgrenade.hand != 0, 0, chr_get_ranged_speed(chr, 0.5, 1.2), 16);
		} else if (rand % 3 == 1) {
			model_set_animation(chr->model, ANIM_THROWGRENADE_NOPIN, chr->act_throwgrenade.hand != 0, 0, chr_get_ranged_speed(chr, 0.5, 1.2), 16);
		} else {
			model_set_animation(chr->model, ANIM_THROWGRENADE_STANDING, chr->act_throwgrenade.hand != 0, 0, chr_get_ranged_speed(chr, 0.5, 1.2), 16);
		}
	} else {
		if (rand % 3 == 0) {
			model_set_animation(chr->model, ANIM_THROWGRENADE_CROUCHING, chr->act_throwgrenade.hand != 0, 5, chr_get_ranged_speed(chr, 0.5, 1.2), 16);
		} else if (rand % 3 == 1) {
			model_set_animation(chr->model, ANIM_THROWGRENADE_NOPIN, chr->act_throwgrenade.hand != 0, 6, chr_get_ranged_speed(chr, 0.5, 1.2), 16);
		} else {
			model_set_animation(chr->model, ANIM_THROWGRENADE_STANDING, chr->act_throwgrenade.hand != 0, 84, chr_get_ranged_speed(chr, 0.5, 1.2), 16);
		}
	}

	model_set_anim_end_frame(chr->model, -1);
}

void chr_throwgrenade(struct chrdata *chr, s32 hand, s32 needsequip)
{
	chr_stop_firing(chr);
	chr->actiontype = ACT_THROWGRENADE;
	chr->act_throwgrenade.hand = hand;
	chr->act_throwgrenade.needsequip = needsequip;
	chr->sleep = 0;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_throwgrenade_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_surprised_choose_animation(struct chrdata *chr)
{
	if (chr->act_surprised.type == 1) {
		struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
		struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
		s32 flip = 0;

		if (leftgun != NULL && rightgun == NULL) {
			flip = 1;
		} else if ((leftgun != NULL && rightgun != NULL) || (leftgun == NULL && rightgun == NULL)) {
			flip = random() & 1;
		}

		model_set_animation(chr->model, ANIM_003F, flip, 10, chr_get_ranged_speed(chr, 0.6f, 0.96000003f), 16);
		model_set_anim_end_frame(chr->model, 52);
	} else if (chr->act_surprised.type == 2) {
		model_set_animation(chr->model, ANIM_SURRENDER_002E, random() & 1, 0, chr_get_ranged_speed(chr, 0.35f, 0.56f), 16);
		model_set_anim_end_frame(chr->model, 7);
	} else {
		u32 part = random() % 3;
		model_set_animation(chr->model, ANIM_0040, random() & 1, 17, 0.6f, 16);

		if (part == 0) {
			model_set_anim_end_frame(chr->model, chr_get_ranged_speed(chr, 38, 8));
		} else if (part == 1) {
			model_set_anim_end_frame(chr->model, chr_get_ranged_speed(chr, 66, 8));
		} else {
			model_set_anim_end_frame(chr->model, chr_get_ranged_speed(chr, 96, 8));
		}
	}
}

void chr_do_surprised_onehand(struct chrdata *chr)
{
	struct prop *prop = chr_get_target_prop(chr);
	f32 angle = chr_get_angle_to_pos(chr, &prop->pos);

	if (angle < BADDTOR(10) || angle > BADDTOR2(350)) {
		chr_stop_firing(chr);
		chr->actiontype = ACT_SURPRISED;
		chr->act_surprised.type = 1;
		chr->sleep = 0;

		if (model_is_anim_merging(chr->model)) {
			chr->hidden |= CHRHFLAG_NEEDANIM;
		} else {
			chr_surprised_choose_animation(chr);
			chr->hidden &= ~CHRHFLAG_NEEDANIM;
		}
	} else if (!chr_is_stopped(chr)) {
		chr_stand(chr);
	}
}

void chr_do_surprised_surrender(struct chrdata *chr)
{
	chr_stop_firing(chr);
	chr->actiontype = ACT_SURPRISED;
	chr->act_surprised.type = 2;
	chr->sleep = 0;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_surprised_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_do_surprised_lookaround(struct chrdata *chr)
{
	chr_stop_firing(chr);
	chr->actiontype = ACT_SURPRISED;
	chr->act_surprised.type = 3;
	chr->sleep = 0;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_surprised_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_surrender_choose_animation(struct chrdata *chr)
{
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);

	if (rightgun || leftgun) {
		model_set_animation(chr->model, ANIM_SURRENDER_002F, random() & 1, 0, 0.5, 16);
		model_set_anim_looping(chr->model, 40, 16);

		if (leftgun) {
			obj_set_dropped(leftgun, DROPTYPE_SURRENDER);
		}

		if (rightgun) {
			obj_set_dropped(rightgun, DROPTYPE_SURRENDER);
		}

		chr->hidden |= CHRHFLAG_DROPPINGITEM;
	} else {
		model_set_animation(chr->model, ANIM_SURRENDER_002E, random() & 1, 0, 0.5, 16);
		model_set_anim_looping(chr->model, 30, 16);
	}

	chr_drop_concealed_items(chr);
}

void chr_surrender(struct chrdata *chr)
{
	u32 action = ACT_SURRENDER;

	if (chr->actiontype != action) {
		chr_stop_firing(chr);
		chr->actiontype = action;
		chr->sleep = action;

		if (model_is_anim_merging(chr->model)) {
			chr->hidden |= CHRHFLAG_NEEDANIM;
		} else {
			chr_surrender_choose_animation(chr);
			chr->hidden &= ~CHRHFLAG_NEEDANIM;
		}
	}
}

void chr_sidestep_choose_animation(struct chrdata *chr)
{
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	bool flip = false;
	bool allowflip = false;
	u32 race = CHRRACE(chr);

	if (leftgun && rightgun) {
		flip = random() % 2;
		allowflip = random() % 2;
	} else {
		if (weapon_is_onehanded(leftgun) == false
				&& weapon_is_onehanded(rightgun) == false
				&& (leftgun || rightgun)) {
			flip = (leftgun != 0);
			allowflip = random() % 2;
		}
	}

	if (race == RACE_HUMAN) {
		if (allowflip == false) {
			if (chr->act_sidestep.side) {
				model_set_animation(chr->model, ANIM_0068, true, 5, chr_get_ranged_speed(chr, 0.55, 0.88000005), 16);
				model_set_anim_end_frame(chr->model, 36);
			} else {
				model_set_animation(chr->model, ANIM_0068, false, 5, chr_get_ranged_speed(chr, 0.55, 0.88000005), 16);
				model_set_anim_end_frame(chr->model, 36);
			}
		} else {
			if ((chr->act_sidestep.side && !flip) || (chr->act_sidestep.side == 0 && flip)) {
				model_set_animation(chr->model, ANIM_003B, flip, 5, chr_get_ranged_speed(chr, 0.7, 1.12), 16);
				model_set_anim_end_frame(chr->model, 34);
			} else {
				model_set_animation(chr->model, ANIM_003A, flip, 5, chr_get_ranged_speed(chr, 0.7, 1.12), 16);
				model_set_anim_end_frame(chr->model, 32);
			}
		}
	} else if (race == RACE_SKEDAR) {
		if (chr->act_sidestep.side) {
			model_set_animation(chr->model, ANIM_0328, false, 5, chr_get_ranged_speed(chr, 0.55, 0.88000005), 16);
			model_set_anim_end_frame(chr->model, 27);
		} else {
			model_set_animation(chr->model, ANIM_0328, true, 5, chr_get_ranged_speed(chr, 0.55, 0.88000005), 16);
			model_set_anim_end_frame(chr->model, 27);
		}
	}
}

void chr_sidestep(struct chrdata *chr, bool side)
{
	chr_stop_firing(chr);
	chr->actiontype = ACT_SIDESTEP;
	chr->act_sidestep.side = side;
	chr->sleep = 0;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_sidestep_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_jumpout_choose_animation(struct chrdata *chr)
{
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	bool flip = false;

	if (leftgun && !rightgun) {
		flip = true;
	} else if ((leftgun && rightgun) || (!leftgun && !rightgun)
			|| weapon_is_onehanded(leftgun) || weapon_is_onehanded(rightgun)) {
		flip = random() % 2;
	}

	if ((chr->act_jumpout.side && !flip) || (chr->act_jumpout.side == 0 && flip)) {
		model_set_animation(chr->model, ANIM_0068, true, 5, chr_get_ranged_speed(chr, 0.55, 0.88000005), 16);
		model_set_anim_end_frame(chr->model, 36);
	} else {
		model_set_animation(chr->model, ANIM_0068, false, 5, chr_get_ranged_speed(chr, 0.55, 0.88000005), 16);
		model_set_anim_end_frame(chr->model, 36);
	}
}

void chr_jumpout(struct chrdata *chr, bool side)
{
	chr_stop_firing(chr);
	chr->actiontype = ACT_JUMPOUT;
	chr->act_jumpout.side = side;
	chr->sleep = 0;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_jumpout_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_runpos_choose_animation(struct chrdata *chr)
{
	f32 xdiff = chr->prop->pos.x - chr->act_runpos.pos.x;
	f32 ydiff = chr->prop->pos.y - chr->act_runpos.pos.y;
	f32 zdiff = chr->prop->pos.z - chr->act_runpos.pos.z;
	f32 distance = sqrtf(xdiff * xdiff + zdiff * zdiff);
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	bool heavy = true;
	bool flip;
	s32 race = CHRRACE(chr);

	if ((leftgun && rightgun) || (!leftgun && !rightgun)) {
		heavy = false;
		flip = random() % 2;
	} else if (weapon_is_onehanded(leftgun) || weapon_is_onehanded(rightgun)) {
		heavy = false;
		flip = (bool)leftgun != false;
	} else {
		flip = (bool)leftgun != false;
	}

	if (race == RACE_HUMAN) {
		if (heavy) {
			f32 mult = 0.5;
			chr->act_runpos.eta60 = PALDOWNF(1.0f / (chr_get_anim_movedist_per_frame(ANIM_RUNNING_TWOHANDGUN) * mult) * distance);
			model_set_animation(chr->model, ANIM_RUNNING_TWOHANDGUN, flip, 0, mult, 16);
		} else {
			f32 mult = 0.5;
			chr->act_runpos.eta60 = PALDOWNF(1.0f / (chr_get_anim_movedist_per_frame(ANIM_RUNNING_ONEHANDGUN) * mult) * distance);
			model_set_animation(chr->model, ANIM_RUNNING_ONEHANDGUN, flip, 0, mult, 16);
		}
	} else if (race == RACE_SKEDAR) {
		f32 mult = 0.5;
		chr->act_runpos.eta60 = PALDOWNF(1.0f / (chr_get_anim_movedist_per_frame(ANIM_SKEDAR_RUNNING) * mult) * distance);
		model_set_animation(chr->model, ANIM_SKEDAR_RUNNING, flip, 0, mult, 16);
	}
}

void chr_run_to_pos(struct chrdata *chr, struct coord *pos)
{
	chr_stop_firing(chr);
	chr->actiontype = ACT_RUNPOS;
	chr->act_runpos.pos.x = pos->x;
	chr->act_runpos.pos.y = pos->y;
	chr->act_runpos.pos.z = pos->z;
	chr->sleep = 0;
	chr->act_runpos.neardist = 30;
	chr->act_runpos.turnspeed = 0;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_runpos_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_attackstand(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	bool flip;
	struct attackanimgroup **animgroup;
	bool firing[] = {false, false};
	s32 race = CHRRACE(chr);

	if (leftgun && rightgun) {
		struct prop *leftgun2 = chr_get_held_usable_prop(chr, HAND_LEFT);
		struct prop *rightgun2 = chr_get_held_usable_prop(chr, HAND_RIGHT);

		if (leftgun2 && rightgun2) {
			flip = random() % 2;

			if (random() % 3 == 0) {
				animgroup = g_StandLightAttackAnims[race];
				firing[HAND_LEFT] = flip;
				firing[HAND_RIGHT] = !flip;
			} else {
				animgroup = g_StandDualAttackAnims[race];
				firing[HAND_LEFT] = true;
				firing[HAND_RIGHT] = true;
			}
		} else {
			flip = (bool)rightgun2 == false;
			animgroup = g_StandLightAttackAnims[race];
			firing[HAND_LEFT] = (bool)rightgun2 == false;
			firing[HAND_RIGHT] = !flip;
		}
	} else {
		if (weapon_is_onehanded(leftgun) || weapon_is_onehanded(rightgun)) {
			flip = (bool)leftgun != false;
			animgroup = g_StandLightAttackAnims[race];
			firing[HAND_LEFT] = (bool)leftgun != false;
			firing[HAND_RIGHT] = !flip;
		} else {
			flip = (bool)leftgun != false;
			animgroup = g_StandHeavyAttackAnims[race];
			firing[HAND_LEFT] = (bool)leftgun != false;
			firing[HAND_RIGHT] = !flip;
		}
	}

	chr_attack(chr, animgroup, flip, firing, attackflags, entityid, true);
}

void chr_attacklie(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	u32 stack[2];
	struct prop *gun = chr_get_held_prop(chr, HAND_RIGHT);
	s32 firing[2] = {false, false};

	if (chr);

	if (attackflags & ATTACKFLAG_AIMONLY) {
		firing[1] = false;
		firing[0] = false;
	} else {
		bool tmp = gun == NULL;
		firing[1] = tmp;
		firing[0] = (bool)!tmp;
	}

	chr_attack(chr, &g_LieAttackAnims, gun == NULL, firing, attackflags, entityid, false);
}

void chr_attackkneel(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	s32 flip;
	struct attackanimgroup **animgroup;
	bool firing[2] = {false, false};
	s32 race = CHRRACE(chr);
	struct prop *leftgun2;
	struct prop *rightgun2;

	if (leftgun && rightgun) {
		leftgun2 = chr_get_held_usable_prop(chr, HAND_LEFT);
		rightgun2 = chr_get_held_usable_prop(chr, HAND_RIGHT);

		if (leftgun2 && rightgun2) {
			flip = random() % 2;

			if (random() % 3 == 0) {
				animgroup = g_KneelLightAttackAnims[race];
				firing[HAND_LEFT] = flip;
				firing[HAND_RIGHT] = !flip;
			} else {
				animgroup = g_KneelDualAttackAnims[race];
				firing[HAND_LEFT] = true;
				firing[HAND_RIGHT] = true;
			}
		} else {
			flip = (bool)rightgun2 == false;
			animgroup = g_KneelLightAttackAnims[race];
			firing[HAND_LEFT] = (bool)rightgun2 == false;
			firing[HAND_RIGHT] = !flip;
		}
	} else {
		if (weapon_is_onehanded(leftgun) || weapon_is_onehanded(rightgun)) {
			flip = (bool)leftgun != false;
			animgroup = g_KneelLightAttackAnims[race];
			firing[HAND_LEFT] = (bool)leftgun != false;
			firing[HAND_RIGHT] = !flip;
		} else {
			flip = (bool)leftgun != false;
			animgroup = g_KneelHeavyAttackAnims[race];
			firing[HAND_LEFT] = (bool)leftgun != false;
			firing[HAND_RIGHT] = !flip;
		}
	}

	chr_attack(chr, animgroup, flip, firing, attackflags, entityid, false);
}

void chr_attackwalk_choose_animation(struct chrdata *chr)
{
	if (chr->aibot == NULL) {
		model_set_animation(chr->model, chr->act_attackwalk.animcfg->animnum,
				chr->act_attackwalk.flip, chr->act_attackwalk.animcfg->startframe, 0.5, 16);
	}
}

void chr_attackwalk(struct chrdata *chr, bool run)
{
	struct attackanimconfig *animcfg;
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	bool flip;
	bool firing[] = {false, false};
	bool everytick[] = {false, false};
	bool singleshot[] = {false, false};
	s32 i;
	struct prop *prop;
	struct weaponobj *weapon;

	if (leftgun && rightgun) {
		struct prop *leftgun2 = chr_get_held_usable_prop(chr, HAND_LEFT);
		struct prop *rightgun2 = chr_get_held_usable_prop(chr, HAND_RIGHT);
		s32 style = 0;

		if (leftgun2 && rightgun2) {
			flip = random() % 2;
			style = random() % 3;
		} else {
			flip = (bool)rightgun2 == false;
		}

		if (style == 0) {
			if (run) {
				animcfg = &g_WalkAttackAnims[3];
			} else {
				animcfg = &g_WalkAttackAnims[2];
			}

			if (flip) {
				firing[HAND_LEFT] = true;
			} else {
				firing[HAND_RIGHT] = true;
			}
		} else if (style == 1) {
			if (run) {
				animcfg = &g_WalkAttackAnims[5];
			} else {
				animcfg = &g_WalkAttackAnims[4];
			}

			firing[HAND_LEFT] = firing[HAND_RIGHT] = true;
		} else {
			if (run) {
				animcfg = &g_WalkAttackAnims[7];
			} else {
				animcfg = &g_WalkAttackAnims[6];
			}

			firing[HAND_LEFT] = firing[HAND_RIGHT] = true;
		}
	} else {
		if (weapon_is_onehanded(leftgun) || weapon_is_onehanded(rightgun)) {
			flip = (bool)leftgun != false;

			if (run) {
				animcfg = &g_WalkAttackAnims[3];
			} else {
				animcfg = &g_WalkAttackAnims[2];
			}

			if (flip) {
				firing[HAND_LEFT] = true;
			} else {
				firing[HAND_RIGHT] = true;
			}
		} else {
			flip = (bool)leftgun != false;

			if (run) {
				animcfg = &g_WalkAttackAnims[1];
			} else {
				animcfg = &g_WalkAttackAnims[0];
			}

			if (flip) {
				firing[HAND_LEFT] = true;
			} else {
				firing[HAND_RIGHT] = true;
			}
		}
	}

	for (i = 0; i < 2; i++) {
		if (firing[i]) {
			prop = chr_get_held_prop(chr, i);
			weapon = prop->weapon;

			if (weapon_get_num_ticks_per_shot(weapon->weaponnum, weapon->gunfunc) < 1) {
				everytick[i] = true;
			}

			if (weapon->weaponnum == WEAPON_ROCKETLAUNCHER
					|| weapon->weaponnum == WEAPON_KINGSCEPTRE
					|| weapon->weaponnum == WEAPON_SLAYER
					|| weapon->weaponnum == WEAPON_DEVASTATOR
					|| (
						!g_Vars.normmplayerisrunning
						&& weapon->weaponnum == WEAPON_DY357MAGNUM
						&& chr->headnum != HEAD_JONATHAN
						&& chr->headnum != HEAD_CHRIST)
					|| (
						!g_Vars.normmplayerisrunning
						&& weapon->weaponnum == WEAPON_DY357LX)
					|| (
						!g_Vars.normmplayerisrunning
						&& weapon->weaponnum == WEAPON_SHOTGUN)) {
				singleshot[i] = true;
			}
		}
	}

	chr->actiontype = ACT_ATTACKWALK;

	chr->act_attackwalk.frame60count = 0;
#if PAL
	// This is really TICKS(400.0f), but off by one bit :(
	chr->act_attackwalk.frame60max = random() % (s32)(333.33331298828f * g_AttackWalkDurationScale) + TICKS(120);
#else
	chr->act_attackwalk.frame60max = random() % (s32)(400 * g_AttackWalkDurationScale) + TICKS(120);
#endif
	chr->act_attackwalk.facedtarget = false;
	chr->act_attackwalk.animcfg = animcfg;
	chr->act_attackwalk.nextshot60 = 0;
	chr->act_attackwalk.nextgun = random() % 2;
	chr->act_attackwalk.firegun[HAND_LEFT] = firing[HAND_LEFT];
	chr->act_attackwalk.firegun[HAND_RIGHT] = firing[HAND_RIGHT];
	chr->act_attackwalk.everytick[HAND_LEFT] = everytick[HAND_LEFT];
	chr->act_attackwalk.everytick[HAND_RIGHT] = everytick[HAND_RIGHT];
	chr->act_attackwalk.singleshot[HAND_LEFT] = singleshot[HAND_LEFT];
	chr->act_attackwalk.singleshot[HAND_RIGHT] = singleshot[HAND_RIGHT];
	chr->act_attackwalk.turnspeed = 0;
	chr->act_attackwalk.flip = flip;

	chr->sleep = 0;
	chr->chrflags &= ~CHRCFLAG_INJUREDTARGET;

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_attackwalk_choose_animation(chr);

		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_attackroll_choose_animation(struct chrdata *chr)
{
	model_set_animation(chr->model, chr->act_attack.animcfg->animnum, chr->act_attack.flip,
			chr->act_attack.animcfg->startframe, chr_get_ranged_speed(chr, 0.5, 0.8), 16);

	if (chr->act_attack.onehanded == false) {
		if (chr->act_attack.dorecoil) {
			if (chr->act_attack.animcfg->recoilendframe >= 0) {
				model_set_anim_end_frame(chr->model, chr->act_attack.animcfg->recoilendframe);
			} else {
				model_set_anim_end_frame(chr->model, chr->act_attack.animcfg->shootendframe);
			}
		} else {
			if (chr->act_attack.animcfg->recoilstartframe >= 0) {
				model_set_anim_end_frame(chr->model, chr->act_attack.animcfg->recoilstartframe);
			} else if (chr->act_attack.animcfg->endframe >= 0) {
				model_set_anim_end_frame(chr->model, chr->act_attack.animcfg->endframe);
			}
		}
	}
}

void chr_attackroll(struct chrdata *chr, bool toleft)
{
	struct attackanimconfig *animcfg;
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	bool flip;
	bool onehanded = false;
	struct prop *prop;
	struct weaponobj *weapon;
	bool dorecoil = true;
	bool firing[] = {false, false};
	s32 i;
	bool dooneburst = false;
	bool everytick[] = {false, false};
	bool singleshot[] = {false, false};

	if (leftgun && rightgun) {
		struct prop *leftgun2 = chr_get_held_usable_prop(chr, HAND_LEFT);
		struct prop *rightgun2 = chr_get_held_usable_prop(chr, HAND_RIGHT);

		if (leftgun2 && rightgun2) {
			flip = random() % 2;
			onehanded = true;

			if (random() % 3 == 0) {
				firing[HAND_LEFT] = flip;
				firing[HAND_RIGHT] = !flip;
			} else {
				firing[HAND_LEFT] = true;
				firing[HAND_RIGHT] = true;
			}
		} else {
			flip = (bool)rightgun2 == false;
			onehanded = true;
			firing[HAND_LEFT] = flip;
			firing[HAND_RIGHT] = !flip;
		}
	} else {
		if (weapon_is_onehanded(leftgun) || weapon_is_onehanded(rightgun)) {
			flip = (bool)leftgun != false;
			onehanded = true;
			firing[HAND_LEFT] = flip;
			firing[HAND_RIGHT] = !flip;
		} else {
			flip = (bool)leftgun != false;
			firing[HAND_LEFT] = flip;
			firing[HAND_RIGHT] = !flip;
		}
	}

	if ((toleft && !flip) || (!toleft && flip)) {
		// Roll to left
		if (random() % 2) {
			animcfg = &g_RollAttackAnims[0];
		} else {
			animcfg = &g_RollAttackAnims[2];
		}
	} else {
		// Roll to right
		if (random() % 2) {
			animcfg = &g_RollAttackAnims[1];
		} else {
			animcfg = &g_RollAttackAnims[3];
		}
	}

	if (onehanded) {
		// Move the animation pointer to the one-handed version of the same
		// animation. The one-handed versions are in the array immediately after
		// the four heavy-weapon versions.
		animcfg += 4;
	}

	for (i = 0; i < 2; i++) {
		if (firing[i]) {
			prop = chr_get_held_prop(chr, i);
			weapon = prop->weapon;

			if (weapon_get_num_ticks_per_shot(weapon->weaponnum, weapon->gunfunc) < 1) {
				everytick[i] = true;

				if (weapon->weaponnum == WEAPON_LASER) {
					dorecoil = false;
				}
			} else {
				dorecoil = false;
				dooneburst = true;
			}

			if (weapon->weaponnum == WEAPON_ROCKETLAUNCHER
					|| weapon->weaponnum == WEAPON_KINGSCEPTRE
					|| weapon->weaponnum == WEAPON_SLAYER
					|| weapon->weaponnum == WEAPON_DEVASTATOR
					|| (
						!g_Vars.normmplayerisrunning
						&& weapon->weaponnum == WEAPON_DY357MAGNUM
						&& chr->headnum != HEAD_JONATHAN
						&& chr->headnum != HEAD_CHRIST)
					|| (
						!g_Vars.normmplayerisrunning
						&& weapon->weaponnum == WEAPON_DY357LX)
					|| (
						!g_Vars.normmplayerisrunning
						&& weapon->weaponnum == WEAPON_SHOTGUN)) {
				singleshot[i] = true;
			}
		}
	}

	chr->actiontype = ACT_ATTACKROLL;

	chr->act_attack.animcfg = animcfg;
	chr->act_attack.fired = false;
	chr->act_attack.nextgun = random() % 2;
	chr->act_attack.firegun[HAND_LEFT] = firing[HAND_LEFT];
	chr->act_attack.firegun[HAND_RIGHT] = firing[HAND_RIGHT];
	chr->act_attack.everytick[HAND_LEFT] = everytick[HAND_LEFT];
	chr->act_attack.everytick[HAND_RIGHT] = everytick[HAND_RIGHT];
	chr->act_attack.singleshot[HAND_LEFT] = singleshot[HAND_LEFT];
	chr->act_attack.singleshot[HAND_RIGHT] = singleshot[HAND_RIGHT];
	chr->act_attack.dorecoil = dorecoil;
	chr->act_attack.dooneburst = dooneburst;
	chr->act_attack.onehanded = onehanded;
	chr->act_attack.pausecount = 0;
	chr->act_attack.numshots = 0;
	chr->act_attack.turning = TURNSTATE_TURNING;

	if (singleshot[HAND_LEFT] || singleshot[HAND_RIGHT]) {
		if (singleshot[HAND_LEFT] && singleshot[HAND_RIGHT]) {
			chr->act_attack.maxshots = 2;
		} else {
			chr->act_attack.maxshots = 1;
		}
	} else {
		chr->act_attack.maxshots = (random() % 4) + 2;

		if (firing[HAND_RIGHT] && firing[HAND_LEFT]) {
			chr->act_attack.maxshots += (random() % 4) + 2;
		}
	}

	chr->act_attack.flags = ATTACKFLAG_AIMATTARGET;
	chr->act_attack.entityid = 0;
	chr->act_attack.standing = true;
	chr->act_attack.reaim = 0;
	chr->act_attack.lastfire60 = 0;
	chr->act_attack.lastontarget60 = 0;
	chr->act_attack.flip = flip;

	chr->sleep = 0;

	// @bug: CHRCFLAG_INJUREDTARGET is not unset here. This means if the chr
	// does an attack that hits the target, then does a roll attack which
	// misses, their AI list will incorrectly read the roll attack as a hit
	// provided it didn't read and clear the flag between the attacks.
	// It usually (always?) does though, so this isn't really an issue.

	if (model_is_anim_merging(chr->model)) {
		chr->hidden |= CHRHFLAG_NEEDANIM;
	} else {
		chr_attackroll_choose_animation(chr);

		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_anim(struct chrdata *chr, s32 animnum, f32 startframe, f32 endframe, u8 chranimflags, s32 merge, f32 speed)
{
	u32 stack;

	if (chr && chr->model) {
		if (chranimflags & CHRANIMFLAG_REVERSE) {
			speed = -speed;
		}

		if (CHRRACE(chr) != RACE_DRCAROLL) {
			chr_stop_firing(chr);
		}

		chr->actiontype = ACT_ANIM;

		chr->act_anim.movewheninvis = (chranimflags & CHRANIMFLAG_MOVEWHENINVIS) != 0;
		chr->act_anim.pauseatend = (chranimflags & CHRANIMFLAG_PAUSEATEND) != 0;
		chr->act_anim.completed = (chranimflags & CHRANIMFLAG_COMPLETED) != 0;
		chr->act_anim.slowupdate = (chranimflags & CHRANIMFLAG_SLOWUPDATE) != 0;
		chr->act_anim.lockpos = (chranimflags & CHRANIMFLAG_LOCKPOS) != 0;
		chr->act_anim.ishitanim = false;
		chr->act_anim.animnum = animnum;
		chr->act_anim.flip = (chranimflags & CHRANIMFLAG_FLIP) != 0;
		chr->act_anim.startframe = startframe;
		chr->act_anim.endframe = endframe;
		chr->act_anim.speed = speed;
		chr->act_anim.blend = merge;

		chr->sleep = chr->act_anim.slowupdate ? merge : 0;

		if (merge > 0 && model_is_anim_merging(chr->model)) {
			chr->hidden |= CHRHFLAG_NEEDANIM;
		} else {
			model_set_animation(chr->model, animnum, (chranimflags & CHRANIMFLAG_FLIP) != 0, startframe, speed, merge);

			if (endframe >= 0) {
				model_set_anim_end_frame(chr->model, endframe);
			}

			chr->hidden &= ~CHRHFLAG_NEEDANIM;
		}
	}
}

void chr_begin_dead(struct chrdata *chr)
{
	if (chr->actiontype != ACT_DEAD) {
		chr_stop_firing(chr);

		if (chr->cover != -1) {
			cover_set_in_use(chr->cover, false);
			chr->cover = -1;
		}

		chr->actiontype = ACT_DEAD;
		chr->act_dead.fadetimer60 = chr->aibot ? 0 : -1;
		chr->act_dead.fadenow = false;
		chr->act_dead.fadewheninvis = false;
		chr->act_dead.invistimer60 = 0;
		chr->act_dead.notifychrindex = 0;
		chr->sleep = 0;

		if (chr->race == RACE_DRCAROLL) {
			chr->drcarollimage_left = DRCAROLLIMAGE_STATIC;
			chr->drcarollimage_right = DRCAROLLIMAGE_STATIC;
		}
	}
}

void chr_attack_choose_endframe(struct chrdata *chr)
{
	struct model *model = chr->model;
	struct attackanimconfig *animcfg = chr->act_attack.animcfg;

	if (chr->act_attack.flags & ATTACKFLAG_AIMONLY) {
		// When only aiming, stop at the recoil frame or shoot start frame
		if (animcfg->recoilstartframe >= 0 && animcfg->recoilstartframe < animcfg->shootstartframe) {
			model_set_anim_end_frame(model, animcfg->recoilstartframe);
		} else {
			model_set_anim_end_frame(model, animcfg->shootstartframe);
		}
	} else if (chr->act_attack.dorecoil) {
		// If the chr will do a recoil, stop at the recoil frame if it exists, else shoot frame.
		// It likely has to stop here so the recoil shot can be taken.
		if (animcfg->recoilstartframe >= 0) {
			model_set_anim_end_frame(model, animcfg->recoilstartframe);
		} else {
			model_set_anim_end_frame(model, animcfg->shootstartframe);
		}
	} else {
		// Shooting without recoil.
		// Stop at the recoil frame so it can be skipped over once reached,
		// otherwise play out the animation.
		if (animcfg->recoilstartframe >= 0) {
			model_set_anim_end_frame(model, animcfg->recoilstartframe);
		} else if (animcfg->endframe >= 0) {
			model_set_anim_end_frame(model, animcfg->endframe);
		} else {
			model_set_anim_end_frame(model, -1);
		}
	}
}

/**
 * This function implements attack behaviour common to all the attack types,
 * such as stand, kneel and lie.
 */
void chr_attack(struct chrdata *chr, struct attackanimgroup **animgroups, bool flip, bool *firing, u32 attackflags, s32 entityid, bool standing)
{
	struct model *model = chr->model;
	s32 i;
	f32 angle;
	struct attackanimconfig *animcfg;
	struct prop *prop;
	struct weaponobj *weapon;
	s32 groupindex;
	bool dooneburst = false;
	s32 index;
	bool everytick[] = {false, false};
	bool singleshot[] = {false, false};
	bool dorecoil = true;
	s32 race = CHRRACE(chr);
	u8 sniping = false;

	if (race != RACE_DRCAROLL && race != RACE_EYESPY && race != RACE_ROBOT) {
		chr->actiontype = ACT_ATTACK;

		if (&animgroups[0] == &g_LieAttackAnims) {
			sniping = true;

			if (model_get_anim_num(chr->model) != ANIM_SNIPING_ONGROUND) {
				// Getting up or getting down
				animcfg = g_SnipeAttackAnims;
				model_set_animation(model, animcfg->animnum, flip, animcfg->startframe, chr_get_ranged_speed(chr, 0.5f, 0.8f), 16);
				model_set_anim_end_frame(model, 236);
			} else {
				animcfg = &g_SnipeAttackAnims[1];
			}
		} else {
			// Non-sniping animations: Choose animation based on angle to target
			angle = chr_get_attack_entity_relative_angle(chr, attackflags, entityid);

			if (flip) {
				groupindex = (BADDTOR(360) - angle) * 5.0937690734863f + 0.5f;
			} else {
				groupindex = angle * 5.0937690734863f + 0.5f;
			}

			if (groupindex < 0 || groupindex > 31) {
				groupindex = 0;
			}

			index = random() % animgroups[groupindex]->len;
			animcfg = &animgroups[groupindex]->animcfg[index];
		}

		for (i = 0; i < 2; i++) {
			if (firing[i]) {
				prop = chr_get_held_prop(chr, i);

				if (prop == NULL) {
					chr_choose_stand_animation(chr, 16);
					return;
				}

				weapon = prop->weapon;

				if (weapon_get_num_ticks_per_shot(weapon->weaponnum, weapon->gunfunc) < 1) {
					// Note: the only weapon that can enter this branch is the laser
					everytick[i] = true;

					if (weapon->weaponnum == WEAPON_LASER) {
						dorecoil = false;
					}
				} else {
					dorecoil = false;
					dooneburst = true;
				}

				// There's an easter egg here: Any guard with Chris T's head
				// (Foster from the firing range) can fire multiple shots with
				// the magnum.
				if (weapon->weaponnum == WEAPON_ROCKETLAUNCHER
						|| weapon->weaponnum == WEAPON_KINGSCEPTRE
						|| weapon->weaponnum == WEAPON_SLAYER
						|| weapon->weaponnum == WEAPON_DEVASTATOR
						|| (
							!g_Vars.normmplayerisrunning
							&& weapon->weaponnum == WEAPON_DY357MAGNUM
							&& chr->headnum != HEAD_JONATHAN
							&& chr->headnum != HEAD_CHRIST)
						|| (
							!g_Vars.normmplayerisrunning
							&& weapon->weaponnum == WEAPON_DY357LX)
						|| (
							!g_Vars.normmplayerisrunning
							&& weapon->weaponnum == WEAPON_SHOTGUN)) {
					singleshot[i] = true;
				}
			}
		}

		chr->act_attack.turning = TURNSTATE_TURNING;
		chr->act_attack.animcfg = animcfg;
		chr->act_attack.fired = false;
		chr->act_attack.nextgun = random() % 2;
		chr->act_attack.firegun[HAND_LEFT] = firing[HAND_LEFT];
		chr->act_attack.firegun[HAND_RIGHT] = firing[HAND_RIGHT];
		chr->act_attack.everytick[HAND_LEFT] = everytick[HAND_LEFT];
		chr->act_attack.everytick[HAND_RIGHT] = everytick[HAND_RIGHT];
		chr->act_attack.singleshot[HAND_LEFT] = singleshot[HAND_LEFT];
		chr->act_attack.singleshot[HAND_RIGHT] = singleshot[HAND_RIGHT];
		chr->act_attack.dorecoil = dorecoil;
		chr->act_attack.dooneburst = dooneburst;
		chr->act_attack.pausecount = 0;
		chr->act_attack.numshots = 0;

		if (singleshot[HAND_LEFT] || singleshot[HAND_RIGHT]) {
			if (singleshot[HAND_LEFT] && singleshot[HAND_RIGHT]) {
				chr->act_attack.maxshots = 2;
			} else {
				chr->act_attack.maxshots = 1;
			}
		} else {
			if (attackflags & ATTACKFLAG_SINGLESHOT) {
				chr->act_attack.maxshots = 1;
			} else {
				chr->act_attack.maxshots = (random() % 4) + 2;
			}

			// @bug: ATTACKFLAG_SINGLESHOT is not respected here if both guns
			// are firing.
			if (firing[HAND_RIGHT] && firing[HAND_LEFT]) {
				chr->act_attack.maxshots += (random() % 4) + 2;
			}
		}

		chr->act_attack.flags = attackflags;
		chr->act_attack.entityid = entityid;
		chr->act_attack.standing = standing;
		chr->act_attack.reaim = 0;
		chr->act_attack.lastfire60 = 0;
		chr->act_attack.lastontarget60 = 0;
		chr->act_attack.flip = flip;

		chr->sleep = 0;
		chr->chrflags &= ~CHRCFLAG_INJUREDTARGET;

		if (!sniping && !chr->aibot) {
			if (model_is_anim_merging(chr->model)) {
				chr->hidden |= CHRHFLAG_NEEDANIM;
			} else {
				model_set_animation(model, animcfg->animnum, flip, animcfg->startframe, chr_get_ranged_speed(chr, 0.5f, 0.8f), 16);
				chr_attack_choose_endframe(chr);

				chr->hidden &= ~CHRHFLAG_NEEDANIM;
			}
		}
	}
}

void chr_attackamount(struct chrdata *chr, u32 attackflags, u32 entityid, u32 maxshots)
{
	u32 stack;
	struct prop *prop = chr_get_held_prop(chr, HAND_RIGHT);
	struct attackanimgroup **things = NULL;
	bool firing[] = {false, false};
	u32 race = CHRRACE(chr);

	if (prop) {
		things = weapon_is_onehanded(prop) ? g_StandLightAttackAnims[race] : g_StandHeavyAttackAnims[race];

		firing[1] = false;
		firing[0] = true;
	}

	chr_attack(chr, things, false, firing, attackflags, entityid, false);

	chr->actiontype = ACT_ATTACKAMOUNT;
	chr->act_attack.numshots = 0;
	chr->act_attack.maxshots = maxshots;
	chr->act_attack.dooneburst = false;
}

#if PAL
s32 g_DrCarollDyingTimer = 8;
#else
s32 g_DrCarollDyingTimer = 10;
#endif

u8 var80068080 = 50;

/**
 * Given a perfectly alive chr, make them begin the process of dying or being
 * knocked unconscious.
 *
 * This function handles:
 * - Eyespy destruction
 * - Transitioning to ACT_DIE
 * - Choosing and applying death animations
 * - Updating kill statistics
 * - Dropping items
 */
void chr_begin_death(struct chrdata *chr, struct coord *dir, f32 relangle, s32 hitpart, struct gset *gset, bool knockout, s32 aplayernum)
{
	bool overridden = false;
	bool instant;
	s32 index = -1;
	s32 animnum;
	u32 stack1;
	struct prop *prop = chr->prop;
	struct model *model = chr->model;
	u32 stack2;
	s32 race = CHRRACE(chr);
	bool wasknockedout = false;
	s32 prevplayernum;
	s32 i;
	s32 buddyplayernum;
	struct eyespy *eyespy;
	s32 objectivenum;
	f32 impactforce1;
	f32 impactforce2;
	f32 impactforce3;

	// If chr was previously knocked out, they are now dead so decrease KO counter
	if (chr->actiontype == ACT_DRUGGEDCOMINGUP
			|| chr->actiontype == ACT_DRUGGEDDROP
			|| chr->actiontype == ACT_DRUGGEDKO) {
#if VERSION >= VERSION_NTSC_1_0
		if (chr->actiontype == ACT_DRUGGEDKO) {
			wasknockedout = true;
		}
#else
		wasknockedout = true;
#endif

		mpstats_decrement_total_knockout_count();
	}

	// Handle eyespy then return early
	if (race == RACE_EYESPY) {
		prevplayernum = g_Vars.currentplayernum;
		buddyplayernum = -1;
		eyespy = chr_to_eyespy(chr);
		objectivenum = -1;

		// Figure out which playernum has the eyespy that's being destroyed,
		// and the buddy's playernum if applicable. Note that the player count
		// can only be 1 or 2 here.
		for (i = 0; i < PLAYERCOUNT(); i++) {
			if (eyespy == g_Vars.players[i]->eyespy) {
				set_current_player_num(i);
			} else {
				buddyplayernum = i;
			}
		}

		if (g_Vars.currentplayer->eyespy) {
			// Stop using eyespy if active
			if (g_Vars.currentplayer->eyespy->active) {
				g_Vars.currentplayer->eyespy->active = false;
				g_Vars.currentplayer->devicesactive &= ~DEVICE_EYESPY;
			}

			// Destroy the eyespy
			chr->hidden |= CHRHFLAG_DELETING;

			explosion_create_simple(g_Vars.currentplayer->eyespy->prop,
					&g_Vars.currentplayer->eyespy->prop->pos,
					g_Vars.currentplayer->eyespy->prop->rooms, EXPLOSIONTYPE_EYESPY, 0);
			inv_remove_item_by_num(WEAPON_EYESPY);

			ps_stop_sound(g_Vars.currentplayer->eyespy->prop, PSTYPE_GENERAL, 0xffff);
			g_Vars.currentplayer->eyespy = NULL;
			set_current_player_num(prevplayernum);

			// For Investigation and G5 Building, set a stage flag to show that
			// the eyespy is destroyed. The scripting in those stages checks for
			// this flag and fails the objective if set.
			switch (stage_get_index(g_Vars.stagenum)) {
			case STAGEINDEX_INVESTIGATION:
				objectivenum = 0;
				break;
			case STAGEINDEX_G5BUILDING:
				if (lv_get_difficulty() == DIFF_A) {
					objectivenum = 2;
				} else if (lv_get_difficulty() == DIFF_SA) {
					objectivenum = 2;
				} else {
					objectivenum = 2;
				}
				break;
			}

			// But don't set the flag if the coop buddy still has an eyespy
			if (objectivenum >= 0 && buddyplayernum >= 0) {
				set_current_player_num(buddyplayernum);

				if (g_Vars.currentplayer->eyespy) {
					objectivenum = -1;
				}

				set_current_player_num(prevplayernum);
			}

			if (objectivenum >= 0 && objective_check(objectivenum) != OBJECTIVE_COMPLETE) {
				g_StageFlags |= STAGEFLAG_EYESPY_DESTROYED;
			}
		}

		set_current_player_num(prevplayernum);
		return;
	}

	// instant = whether to merge into death animation or switch to it instantly
	instant = chr->actiontype == ACT_ARGH && chr->act_argh.lvframe60 == g_Vars.lvframe60;

	for (i = 0; g_AnimTablesByRace[race][i].hitpart != -1; i++) {
		if (g_AnimTablesByRace[race][i].hitpart == hitpart) {
			index = i;
			break;
		}
	}

	// Set up chr's new action
	chr->blurdrugamount = 0;

	chr_stop_firing(chr);
	chr_uncloak(chr, true);

#if VERSION >= VERSION_NTSC_1_0
	chr->chrflags &= ~CHRCFLAG_HIDDEN;
#endif

	chr->actiontype = (knockout == true ? ACT_DRUGGEDDROP : ACT_DIE);

	chr->act_die.notifychrindex = 0;
	chr->act_die.timeextra = 0;
	chr->act_die.drcarollimagedelay = TICKS(45);
	chr->act_die.thudframe1 = -1;
	chr->act_die.thudframe2 = -1;

	if (chr->race == RACE_DRCAROLL) {
		chr->drcarollimage_left = (s32)((random() % 400) * 0.01f) + 1;
		chr->drcarollimage_right = (s32)((random() % 400) * 0.01f) + 1;
	}

	chr->sleep = 0;

	// Handle robots and Dr Caroll then return early
	if (race == RACE_ROBOT || race == RACE_DRCAROLL) {
		impactforce1 = gset_get_impact_force(gset) * 0.5f;

		if (impactforce1 <= 0) {
			impactforce1 = 3;
		}

		if (impactforce1 != 0.0f) {
			chr->elapseextra = 0;
			chr->timeextra = impactforce1 * 15;
			chr->extraspeed.x = dir->x * impactforce1;
			chr->extraspeed.y = dir->y * impactforce1;
			chr->extraspeed.z = dir->z * impactforce1;
		}

		if (race == RACE_DRCAROLL) {
			g_DrCarollDyingTimer = 0;

			chr->soundtimer = 0;
			chr->voicebox = VOICEBOX_MALE1;

			model_set_animation(chr->model, ANIM_0164, false, 0, 0.5f, 16);
		}

		return;
	}

	// Handle humans and Skedar
	if (race == RACE_HUMAN) {
		animnum = model_get_anim_num(chr->model);

		// Chrs in lying-down sniping poses don't use standard death animations
		if (animnum == ANIM_SNIPING_GETDOWN
				|| animnum == ANIM_SNIPING_GETUP
				|| animnum == ANIM_SNIPING_ONGROUND) {
			model_set_animation(chr->model, ANIM_SNIPING_DIE, false, 0, 0.5f, 16);
		} else {
			// Consider making the chr do an animation where they slump against
			// a wall or object which is behind them.
			if ((relangle < BADDTOR(90) || relangle > BADDTOR(270))
					&& random() % 20 == 0
					&& chr->specialdie == SPECIALDIE_NONE) {
				f32 angle1;
				f32 angle2 = chr_get_theta(chr);
				f32 fval1;
				f32 fval2;

				angle1 = angle2 + RAD(190, 3.3155977725983f);
				angle2 += BADDTOR(170);

				if (angle1 >= BADDTOR(360)) {
					angle1 -= BADDTOR(360);
				}

				if (angle2 >= BADDTOR(360)) {
					angle2 -= BADDTOR(360);
				}

				fval1 = chr_get_available_dist_at_angle(prop, angle1, 150);
				fval2 = chr_get_available_dist_at_angle(prop, angle2, 150);

				if (fval1 < 150 && fval2 < 150
						&& fval1 - fval2 < 10 && fval1 - fval2 > -10
						&& !wasknockedout) {
					struct animtablerow *row = &g_AnimTableHumanSlumped[random() % 4];
					u32 stack3;

					chr->act_die.thudframe1 = row->thudframe1;
					chr->act_die.thudframe2 = row->thudframe2;

					model_set_animation_with_merge(model, row->animnum, row->flip, 0, row->speed, 16, !instant);

					if (row->endframe >= 0) {
						model_set_anim_end_frame(model, row->endframe);
					}

					chr->radius = 10;
					chr->chrflags &= ~CHRCFLAG_HAS_SPECIAL_DEATH_ANIMATION;

					overridden = true;
				}
			}

			// Consider making the chr do a fall forward animation.
			// The player must be behind the chr for it to happen.
			if (relangle > BADDTOR(135) && relangle < BADDTOR2(225)
					&& random() % 5 < 2
					&& chr->specialdie == SPECIALDIE_NONE) {
				struct animtablerow *row;

				struct animtablerow rows[] = {
					{ ANIM_005B, 0, -1, 0.6, 0, 27, -1 },
					{ ANIM_0255, 0, -1, 0.5, 0, 25, -1 },
				};

				bool flip;

				overridden = true;

				if (hitpart == HITPART_LBICEP || hitpart == HITPART_RBICEP) {
					row = &rows[0];

					if (hitpart == HITPART_LBICEP) {
						flip = true;
					} else {
						flip = false;
					}
				} else {
					row = &rows[1];
					flip = random() % 2;
				}

				chr->act_die.thudframe1 = row->thudframe1;
				chr->act_die.thudframe2 = row->thudframe2;

				model_set_animation_with_merge(model, row->animnum, flip, 0, row->speed, 16, !instant);

				if (row->endframe >= 0) {
					model_set_anim_end_frame(model, chr_get_ranged_argh_speed(chr, row->endframe, 8));
				} else {
					model_set_anim_end_frame(model, chr_get_ranged_argh_speed(chr, anim_get_num_frames(row->animnum) - 1, 8));
				}

				chr->chrflags &= ~CHRCFLAG_HAS_SPECIAL_DEATH_ANIMATION;
			}

			if (!overridden && index >= 0) {
				// Handle specialdie animations or choose a random one if not
				// using specialdie
				if (g_AnimTablesByRace[race][index].deathanims
						&& g_AnimTablesByRace[race][index].deathanimcount > 0
						&& !wasknockedout) {
					struct animtablerow *row;

					if (chr->specialdie == SPECIALDIE_NONE) {
						s32 tmp = random() % g_AnimTablesByRace[race][index].deathanimcount;
						row = &g_AnimTablesByRace[race][index].deathanims[tmp];
					} else if (chr->specialdie == SPECIALDIE_ONCHAIR) {
						row = &g_SpecialDieAnims[chr->specialdie + random() % 2];

						// chr->myspecial is the tag number of the chr's chair
						if (chr->myspecial >= 0) {
							struct defaultobj *obj = obj_find_by_tag_id(chr->myspecial);
							obj->flags3 &= ~OBJFLAG3_PUSHABLE;
							obj->flags |= OBJFLAG_INVINCIBLE;

							// The original source likely didn't have the brackets here,
							// but I'm including them to show the logic that's actually
							// being used. There is no bug here, as obj and obj->prop
							// are always set at this point so these checks are
							// unnecessary.
							if ((obj && obj->prop && obj->modelnum == MODEL_DD_REDARM)
									|| obj->modelnum == MODEL_DD_REDSOFA) {
								row = &g_SpecialDieAnims[chr->specialdie - 1];
							}
						}
					} else {
						row = &g_SpecialDieAnims[chr->specialdie - 1];
					}

					chr->act_die.thudframe1 = row->thudframe1;
					chr->act_die.thudframe2 = row->thudframe2;

					if (chr->specialdie == SPECIALDIE_NONE) {
						model_set_animation_with_merge(model, row->animnum, row->flip, 0, row->speed, 16, !instant);
					} else {
						model_set_animation_with_merge(model, row->animnum, row->flip, 0, row->speed, 30, !instant);
					}

					if (row->endframe >= 0) {
						model_set_anim_end_frame(model, row->endframe);
					}

					impactforce2 = gset_get_impact_force(gset);

					if (impactforce2 <= 0 && (chr->chrflags & CHRCFLAG_DIEWITHFORCE)) {
						impactforce2 = 6;
					}

					if (row->unk10 && impactforce2 > 0) {
						chr->act_die.timeextra = impactforce2 * 15;
						chr->act_die.elapseextra = 0;
						chr->act_die.extraspeed.x = dir->x * impactforce2;
						chr->act_die.extraspeed.y = dir->y * impactforce2;
						chr->act_die.extraspeed.z = dir->z * impactforce2;
					}

					chr->chrflags &= ~CHRCFLAG_HAS_SPECIAL_DEATH_ANIMATION;
				}
			}
		}
	} else if (race == RACE_SKEDAR) {
		struct animtablerow *row;

		if (relangle > BADDTOR(135) && relangle < BADDTOR2(225)) {
			// Player is behind the Skedar - use specific set of anims
			row = &g_AnimTablesByRace[race][1 + (random() % 6)].deathanims[random() % 3];

			chr->act_die.thudframe1 = row->thudframe1;
			chr->act_die.thudframe2 = row->thudframe2;

			model_set_animation_with_merge(model, row->animnum, row->flip, 0, row->speed, 16, !instant);

			if (row->endframe >= 0) {
				model_set_anim_end_frame(model, row->endframe);
			}
		} else {
			// Normal Skedar death
			if (index >= 0
					&& g_AnimTablesByRace[race][index].deathanims != NULL
					&& g_AnimTablesByRace[race][index].deathanimcount > 0) {
				s32 tmp = random() % g_AnimTablesByRace[race][index].deathanimcount;
				row = &g_AnimTablesByRace[race][index].deathanims[tmp];
			} else {
				row = &g_AnimTablesByRace[race][0].deathanims[0];
			}

			chr->act_die.thudframe1 = row->thudframe1;
			chr->act_die.thudframe2 = row->thudframe2;

			model_set_animation_with_merge(model, row->animnum, row->flip, 0, row->speed, 16, !instant);

			if (row->endframe >= 0) {
				model_set_anim_end_frame(model, row->endframe);
			}

			impactforce3 = gset_get_impact_force(gset);

			if (impactforce3 <= 0 && (chr->chrflags & CHRCFLAG_DIEWITHFORCE)) {
				impactforce3 = 6;
			}

			if (row->unk10 != 0 && impactforce3 > 0) {
				chr->act_die.timeextra = impactforce3 * 15;
				chr->act_die.elapseextra = 0;
				chr->act_die.extraspeed.x = dir->x * impactforce3;
				chr->act_die.extraspeed.y = dir->y * impactforce3;
				chr->act_die.extraspeed.z = dir->z * impactforce3;
			}
		}
	} else if (race == RACE_DRCAROLL) {
		// empty
	}

	// Handle multiplayer stats and kill count
	if (g_Vars.mplayerisrunning) {
		mpstats_record_death(aplayernum, mp_chr_to_chrindex(chr));
	} else if (aplayernum >= 0) {
		s32 prevplayernum = g_Vars.currentplayernum;
		set_current_player_num(aplayernum);
		mpstats_record_player_kill();
		set_current_player_num(prevplayernum);
	}

	if (chr->chrflags & CHRCFLAG_KILLCOUNTABLE) {
		mpstats_increment_total_kill_count();
	}

	// Drop items
	if (race == RACE_HUMAN || race == RACE_SKEDAR) {
		if (chr->weapons_held[0] && (chr->weapons_held[0]->obj->flags & OBJFLAG_AIUNDROPPABLE) == 0) {
			obj_set_dropped(chr->weapons_held[0], DROPTYPE_DEFAULT);
			chr->hidden |= CHRHFLAG_DROPPINGITEM;
		}

		if (chr->weapons_held[1] && (chr->weapons_held[1]->obj->flags & OBJFLAG_AIUNDROPPABLE) == 0) {
			obj_set_dropped(chr->weapons_held[1], DROPTYPE_DEFAULT);
			chr->hidden |= CHRHFLAG_DROPPINGITEM;
		}

		chr_drop_concealed_items(chr);
	}
}

void chr_begin_argh(struct chrdata *chr, f32 angle, s32 hitpart)
{
	bool doneanim = false;
	s32 instant;
	s32 index = -1;
	struct model *model = chr->model;
	s32 i;
	s32 race = CHRRACE(chr);
	s32 animnum = model_get_anim_num(chr->model);

	if (animnum == ANIM_SNIPING_GETDOWN
			|| animnum == ANIM_SNIPING_GETUP
			|| animnum == ANIM_SNIPING_ONGROUND) {
		chr_flinch_body(chr);
		return;
	}

	if (race == RACE_EYESPY || chr->aibot) {
		return;
	}

	if (race == RACE_DRCAROLL) {
		chr->actiontype = ACT_ARGH;
		chr->act_argh.notifychrindex = 0;
		chr->act_argh.lvframe60 = g_Vars.lvframe60;

		chr->sleep = 0;

		model_set_animation(chr->model, ANIM_0163, false, 0, 0.5f, 16);

		chr->drcarollimage_left = DRCAROLLIMAGE_X;
		chr->drcarollimage_right = DRCAROLLIMAGE_X;
		return;
	}

	instant = chr->actiontype == ACT_ARGH && chr->act_argh.lvframe60 == g_Vars.lvframe60;

	for (i = 0; g_AnimTablesByRace[race][i].hitpart != -1; i++) {
		if (g_AnimTablesByRace[race][i].hitpart == hitpart) {
			index = i;

			if (chr->hitpart == 0) {
				chr->hitpart = hitpart;
			}

			break;
		}
	}

	// If shot in the butt from behind, 2 in 5 chance of doing a special anim
	if (race == RACE_HUMAN
			&& hitpart == HITPART_PELVIS
			&& angle > BADDTOR(135)
			&& angle < BADDTOR2(225)
			&& random() % 5 < 2) {
		struct animtablerow *row;
		struct animtablerow rows[] = {
			{ ANIM_013B, 0, -1, 0.5, 0, -1, -1 },
			{ ANIM_013C, 0, -1, 0.5, 0, -1, -1 },
			{ ANIM_013F, 0, -1, 0.5, 0, -1, -1 },
			{ ANIM_0142, 0, -1, 0.5, 0, -1, -1 },
			{ ANIM_0145, 0, -1, 0.5, 0, -1, -1 },
			{ ANIM_0148, 0, -1, 0.5, 0, -1, -1 },
			{ ANIM_0036, 0, -1, 0.5, 0, -1, -1 },
			{ ANIM_0037, 0, -1, 0.5, 0, -1, -1 },
		};

		chr_stop_firing(chr);

		chr->actiontype = ACT_ARGH;
		chr->act_argh.notifychrindex = 0;
		chr->act_argh.lvframe60 = g_Vars.lvframe60;
		chr->sleep = 0;

		row = &rows[random() % 8];

		model_set_animation_with_merge(model, row->animnum, row->flip, 0, row->speed, 16, !instant);

		if (row->endframe >= 0) {
			model_set_anim_end_frame(model, chr_get_ranged_argh_speed(chr, row->endframe, 8));
			doneanim = true;
		} else {
			model_set_anim_end_frame(model, chr_get_ranged_argh_speed(chr, anim_get_num_frames(row->animnum) - 1, 8));
			doneanim = true;
		}
	}

	if (!doneanim
			&& index >= 0
			&& g_AnimTablesByRace[race][index].injuryanims
			&& g_AnimTablesByRace[race][index].injuryanimcount > 0) {
		// If shot in a hand that's holding a gun, remap the hit location to the
		// forearm because the hand injury animations assume the hand is empty.
		struct prop *lgun = chr_get_held_prop(chr, HAND_LEFT);
		struct prop *rgun = chr_get_held_prop(chr, HAND_RIGHT);
		s32 rowindex;
		struct animtablerow *row;

		if (race == RACE_HUMAN) {
			if (index == 9 && lgun) { // left hand
				index = 10; // left forearm
			} else if (index == 12 && rgun) { // right hand
				index = 13; // right forearm
			}
		}

		// Select a random animation for this hit location and apply it
		rowindex = random() % g_AnimTablesByRace[race][index].injuryanimcount;

		row = &g_AnimTablesByRace[race][index].injuryanims[rowindex];

		chr_stop_firing(chr);

		chr->actiontype = ACT_ARGH;
		chr->act_argh.notifychrindex = 0;
		chr->act_argh.lvframe60 = g_Vars.lvframe60;
		chr->sleep = 0;

		model_set_animation_with_merge(model, row->animnum, row->flip, 0, row->speed, 16, !instant);

		if (row->endframe >= 0) {
			model_set_anim_end_frame(model, chr_get_ranged_argh_speed(chr, row->endframe, 8));
		} else {
			model_set_anim_end_frame(model, chr_get_ranged_argh_speed(chr, anim_get_num_frames(row->animnum) - 1, 8));
		}
	}
}

void chr_react_to_damage(struct chrdata *chr, struct coord *vector, f32 angle, s32 hitpart, struct gset *gset, s32 aplayernum)
{
	s32 race = CHRRACE(chr);
	bool knockedout = false;
	s32 animnum = model_get_anim_num(chr->model);

#if VERSION >= VERSION_NTSC_1_0
	if (chr->actiontype == ACT_DRUGGEDKO)
#else
	if (chr->actiontype == ACT_DRUGGEDCOMINGUP
			|| chr->actiontype == ACT_DRUGGEDDROP
			|| chr->actiontype == ACT_DRUGGEDKO)
#endif
	{
		knockedout = true;
	}

	if (race == RACE_EYESPY) {
		f32 strength = gset_get_impact_force(gset);
		struct eyespy *eyespy = chr_to_eyespy(chr);

		if (eyespy) {
			if (strength <= 0) {
				strength = 6;
			}

			strength *= 4;

			eyespy->hit = EYESPYHIT_DAMAGE;
			eyespy->vel.x += vector->x * strength;
			eyespy->vel.z += vector->z * strength;
		}
	}

	if (chr->damage >= chr->maxdamage) {
		chr_begin_death(chr, vector, angle, hitpart, gset, false, aplayernum);
	} else if (animnum == ANIM_SNIPING_GETDOWN
			|| animnum == ANIM_SNIPING_GETUP
			|| animnum == ANIM_SNIPING_ONGROUND) {
		chr_flinch_body(chr);
	} else if (race == RACE_EYESPY) {
		// empty
	} else if (race == RACE_DRCAROLL || race == RACE_ROBOT) {
		f32 strength = gset_get_impact_force(gset);

		if (race == RACE_DRCAROLL) {
			strength *= 0.5f;
		}

		if (strength <= 0) {
			strength = 6;
		}

		if (strength > 0) {
			chr->elapseextra = 0;
			chr->timeextra = strength * 15;
			chr->extraspeed.x = vector->x * strength;
			chr->extraspeed.y = vector->y * strength;
			chr->extraspeed.z = vector->z * strength;
		}

		if (race == RACE_DRCAROLL) {
			chr_begin_argh(chr, 0, 0);
		}
	} else if (!knockedout) {
		chr_begin_argh(chr, angle, hitpart);
	}
}

/**
 * Launch a chr away from the given pos (for explosions).
 */
void chr_yeet_from_pos(struct chrdata *chr, struct coord *exppos, f32 force)
{
	struct model *model = chr->model;
	struct prop *prop = chr->prop;
	f32 faceangle;
	f32 latangle;
	u32 stack1;
	s32 angleindex;
	u32 stack2;
	struct yeetanim *row;
	struct coord dist;
	u32 stack3;
	s32 race = CHRRACE(chr);
	f32 speed;
	s32 subindex;
	f32 angletoexplosion;

	if (race != RACE_DRCAROLL && race != RACE_EYESPY && race != RACE_ROBOT) {
		faceangle = chr_get_theta(chr);
		latangle = atan2f(prop->pos.x - exppos->x, prop->pos.z - exppos->z);

		dist.x = prop->pos.x - exppos->x;
		dist.y = prop->pos.y - exppos->y;
		dist.z = prop->pos.z - exppos->z;

		if (dist.f[0] == 0 && dist.f[1] == 0 && dist.f[2] == 0) {
			dist.z = 1;
		}

		speed = 0.625f * force / sqrtf(dist.f[0] * dist.f[0] + dist.f[1] * dist.f[1] + dist.f[2] * dist.f[2]);
		angletoexplosion = latangle - faceangle;

		dist.x *= speed;
		dist.y *= speed;
		dist.z *= speed;

		chr->fallspeed.x = dist.x;
		chr->fallspeed.y = dist.y;
		chr->fallspeed.z = dist.z;

		if (latangle < faceangle) {
			angletoexplosion += BADDTOR(360);
		}

		angleindex = angletoexplosion * 1.2734422683716f + 0.5f;

		if (angleindex >= 8) {
			angleindex = 0;
		}

		subindex = random() % g_YeetAnimIndexesByRaceAngle[race][angleindex].count;

		if (race == RACE_HUMAN) {
			row = &g_YeetAnimsHuman[g_YeetAnimIndexesByRaceAngle[race][angleindex].indexes[subindex]];
		} else if (race == RACE_SKEDAR) {
			row = &g_YeetAnimsSkedar[g_YeetAnimIndexesByRaceAngle[race][angleindex].indexes[subindex]];
		}

		chr_stop_firing(chr);
		chr_uncloak(chr, true);

#if VERSION >= VERSION_NTSC_1_0
		chr->chrflags &= ~CHRCFLAG_HIDDEN;
#endif

		chr->actiontype = ACT_DIE;

		chr->act_die.notifychrindex = 0;
		chr->act_die.thudframe1 = row->thudframe;
		chr->act_die.thudframe2 = -1;
		chr->act_die.timeextra = 0;
		chr->act_die.drcarollimagedelay = TICKS(45);

		if (chr->race == RACE_DRCAROLL) {
			chr->drcarollimage_left = 1 + (s32)((random() % 400) * 0.01f);
			chr->drcarollimage_right = 1 + (s32)((random() % 400) * 0.01f);
		}

		chr->sleep = 0;
		model_set_animation(model, row->animnum, row->flip, row->startframe, row->speed, 8);

		if (row->endframe >= 0.0f) {
			model_set_anim_end_frame(model, row->endframe);
		}
	}
}

s32 gset_get_blur_amount(struct gset *gset)
{
	s32 amount = TICKS(1000);

	if (g_Vars.normmplayerisrunning == false) {
		amount = TICKS(250);
	}

	if (gset->weaponnum == WEAPON_TRANQUILIZER) {
		amount = TICKS(2000);
	}

	if (gset->weaponnum == WEAPON_BOLT) {
		amount = TICKS(5000);
	}

	if (gset->weaponnum == WEAPON_NBOMB) {
		amount = TICKS(100);
	}

	return amount;
}

void chr_knockout(struct chrdata *chr, f32 angle, s32 hitpart, struct gset *gset)
{
	if (chr->actiontype != ACT_DRUGGEDCOMINGUP
			&& chr->actiontype != ACT_DRUGGEDDROP
			&& chr->actiontype != ACT_DRUGGEDKO) {
#if VERSION >= VERSION_PAL_FINAL
		if (mpstats_get_total_knockout_count() < 2) {
			chr->chrflags |= CHRCFLAG_KEEPCORPSEKO;

			if (main_get_stage_num() == STAGE_VILLA) {
				chr->hidden |= CHRHFLAG_ANTINONINTERACTABLE;
			}
		}
#elif VERSION >= VERSION_NTSC_1_0
		if (mpstats_get_total_knockout_count() < 2) {
			chr->chrflags |= CHRCFLAG_KEEPCORPSEKO;
		}
#endif

		mpstats_increment_total_knockout_count();

		chr->actiontype = ACT_DRUGGEDCOMINGUP;
		chr->act_druggedcomingup.timer60 = 0;
	}
}

/**
 * Return true if the chr's current animation would be too awkward to transition
 * into an injury animation or if they're already in the PREARGH action state,
 * and set dst to the anim frame number where the chr will become available for
 * transition to an injury animation.
 *
 * The attack roll animation is the only one which is too awkward to transition.
 */
bool chr_is_anim_preventing_argh(struct chrdata *chr, f32 *dst)
{
	bool result = false;
	s32 race = CHRRACE(chr);

	if (race == RACE_DRCAROLL || race == RACE_EYESPY || chr->aibot) {
		return false;
	}

	if (race == RACE_HUMAN) {
		s32 animnum = model_get_anim_num(chr->model);
		f32 endframe;

		if (animnum == ANIM_SNIPING_GETDOWN
				|| animnum == ANIM_SNIPING_GETUP
				|| animnum == ANIM_SNIPING_ONGROUND) {
			chr_flinch_body(chr);
		} else if (chr->actiontype == ACT_ATTACKROLL
				&& model_get_anim_num(chr->model) == chr->act_attack.animcfg->animnum) {
			if (chr->act_attack.onehanded) {
				if (chr->act_attack.animcfg == &g_RollAttackAnims[4]
						|| chr->act_attack.animcfg == &g_RollAttackAnims[5]
						|| chr->act_attack.animcfg == &g_RollAttackAnims[6]
						|| chr->act_attack.animcfg == &g_RollAttackAnims[7]) {
					endframe = chr->act_attack.animcfg->unk04 - 8;

					if (chr->act_attack.animcfg->endframe < chr->act_attack.animcfg->unk04) {
						endframe = chr->act_attack.animcfg->endframe;
					}

					if (endframe > model_get_cur_anim_frame(chr->model)) {
						*dst = endframe;
						result = true;
					}
				}
			} else {
				endframe = chr->act_attack.animcfg->unk04 - 8;

				if (endframe > model_get_cur_anim_frame(chr->model)) {
					*dst = endframe;
					result = true;
				}
			}
		}
	}

	if (chr->actiontype == ACT_PREARGH) {
		result = true;
	}

	return result;
}

void chr_grunt(struct chrdata *chr, s32 choketype)
{
	bool male = false;
	s16 soundnum = -1;
	s32 race = CHRRACE(chr);
	s32 playernum;
	bool human = false;

	static s32 nextindexdrcaroll = 0;
	static s32 nextindexmaian = 0;
	static s32 nextindexskedar = 0;
	static s32 nextindexshock = 0;
	static s32 nextindexmale = 0;
	static s32 nextindexfemale = 0;

	if (race == RACE_EYESPY || race == RACE_ROBOT) {
		return;
	}

	if (chr->prop->type == PROPTYPE_PLAYER) {
		playernum = playermgr_get_player_num_by_prop(chr->prop);

		if (g_Vars.players[playernum]->isdead) {
			return;
		}
	}

	if (g_HeadsAndBodies[chr->headnum].ismale) {
		male = true;
	}

	if (race == RACE_DRCAROLL) {
		s16 sounds[] = {
			SFXNUM_0240_DRC_ARGH,
			SFXNUM_024C_DRC_ARGH,
			SFXNUM_0250_DRC_ARGH,
			SFXNUM_0251_DRC_ARGH,
			SFXNUM_0259_DRC_ARGH,
			SFXNUM_025A_DRC_ARGH,
		};

		if (g_DrCarollDyingTimer > TICKS(10)) {
			g_DrCarollDyingTimer = 0;

			soundnum = sounds[nextindexdrcaroll];
			nextindexdrcaroll++;

			if (nextindexdrcaroll >= ARRAYCOUNT(sounds)) {
				nextindexdrcaroll = 0;
			}
		}
	} else if (chr->headnum == HEAD_THEKING
			|| chr->headnum == HEAD_ELVIS
			|| chr->headnum == HEAD_MAIAN_S
			|| chr->headnum == HEAD_ELVIS_GOGS) {
		s16 sounds[] = {
			SFXNUM_05DF_MAIAN_ARGH,
			SFXNUM_05E0_MAIAN_ARGH,
			SFXNUM_05E1_MAIAN_ARGH
		};

		soundnum = sounds[random() % 3];
		nextindexmaian++;

		if (nextindexmaian >= ARRAYCOUNT(sounds)) {
			nextindexmaian = 0;
		}
	} else if (race == RACE_SKEDAR) {
		if (chr->bodynum == BODY_MINISKEDAR) {
			s16 sounds[] = {
				SFXNUM_0536_SKEDAR_ROAR,
				SFXNUM_0537_SKEDAR_ROAR,
				SFXNUM_0538_SKEDAR_ROAR,
				SFXNUM_0539_SKEDAR_ROAR,
				SFXNUM_053A_SKEDAR_ROAR,
			};

			soundnum = sounds[random() % 5];
			nextindexskedar++;

			if (nextindexskedar >= ARRAYCOUNT(sounds)) {
				nextindexskedar = 0;
			}
		} else {
			s16 sounds[] = {
				SFXNUM_052D_SKEDAR_ROAR,
				SFXNUM_052E_SKEDAR_ROAR,
				SFXNUM_052F_SKEDAR_ROAR,
			};

			soundnum = sounds[random() % 3];
			nextindexskedar++;

			if (nextindexskedar >= ARRAYCOUNT(sounds)) {
				nextindexskedar = 0;
			}
		}
	} else if (chr->headnum == HEAD_DDSHOCK) {
		s16 sounds[] = {
			SFXNUM_0086_M_ARGH,
			SFXNUM_0088_M_ARGH,
			SFXNUM_008A_M_ARGH,
			SFXNUM_008C_M_ARGH,
			SFXNUM_008E_M_ARGH,
			SFXNUM_0090_M_ARGH,
			SFXNUM_0092_M_ARGH,
			SFXNUM_0094_M_ARGH,
			SFXNUM_0096_M_ARGH,
			SFXNUM_0098_M_ARGH,
			SFXNUM_009A_M_ARGH,
			SFXNUM_009C_M_ARGH,
			SFXNUM_009E_M_ARGH,
			SFXNUM_0087_M_ARGH,
		};

		soundnum = sounds[nextindexshock];
		nextindexshock++;

		if (nextindexshock >= ARRAYCOUNT(sounds)) {
			nextindexshock = 0;
		}

		human = true;
	} else if (male) {
		s16 sounds[] = {
			SFXNUM_0086_M_ARGH,
			SFXNUM_0087_M_ARGH,
			SFXNUM_0088_M_ARGH,
			SFXNUM_0089_M_ARGH,
			SFXNUM_008A_M_ARGH,
			SFXNUM_008B_M_ARGH,
			SFXNUM_008C_M_ARGH,
			SFXNUM_008D_M_ARGH,
			SFXNUM_008E_M_ARGH,
			SFXNUM_008F_M_ARGH,
			SFXNUM_0090_M_ARGH,
			SFXNUM_0091_M_ARGH,
			SFXNUM_0092_M_ARGH,
			SFXNUM_0093_M_ARGH,
			SFXNUM_0094_M_ARGH,
			SFXNUM_0095_M_ARGH,
			SFXNUM_0096_M_ARGH,
			SFXNUM_0097_M_ARGH,
			SFXNUM_0098_M_ARGH,
			SFXNUM_0099_M_ARGH,
			SFXNUM_009A_M_ARGH,
			SFXNUM_009B_M_ARGH,
			SFXNUM_009C_M_ARGH,
			SFXNUM_009D_M_ARGH,
			SFXNUM_009E_M_ARGH,
		};

		soundnum = sounds[nextindexmale];
		nextindexmale++;

		human = true;

		if (nextindexmale >= ARRAYCOUNT(sounds)) {
			nextindexmale = 0;
		}
	} else if (chr->headnum == HEAD_DARK_COMBAT
			|| chr->headnum == HEAD_DARK_FROCK
			|| chr->headnum == HEAD_DARKAQUA
			|| chr->headnum == HEAD_DARK_SNOW) {
		s16 sounds[] = {
			SFXNUM_02AA_JO_ARGH,
			SFXNUM_02AB_JO_ARGH,
			SFXNUM_02AC_JO_ARGH,
			SFXNUM_02AD_JO_ARGH,
			SFXNUM_02AE_JO_ARGH,
			SFXNUM_02AF_JO_ARGH,
			SFXNUM_02B0_JO_ARGH,
			SFXNUM_02B1_JO_ARGH,
			SFXNUM_02B2_JO_ARGH,
			SFXNUM_02B3_JO_ARGH,
		};

		soundnum = sounds[random() % 10];
		human = true;
	} else {
		s16 sounds[] = {
			SFXNUM_000D_F_ARGH,
			SFXNUM_000E_F_ARGH,
			SFXNUM_000F_F_ARGH,
		};

		soundnum = sounds[nextindexfemale];
		nextindexfemale++;

		if (nextindexfemale >= ARRAYCOUNT(sounds)) {
			nextindexfemale = 0;
		}

		human = true;
	}

	if (human) {
		if (choketype == CHOKETYPE_GURGLE) {
			s32 sounds[] = {
				SFXNUM_034E_M1_CHOKING,
				SFXNUM_05B1_GURGLE,
				SFXNUM_05B2_GURGLE,
			};

			if ((random() % 8) == 0) {
				soundnum = sounds[random() % 3];
			}

			chr->soundgap = 10;
			chr->soundtimer = 0;
		} else if (choketype == CHOKETYPE_COUGH) {
			if (male) {
				if ((random() % 2) == 0) {
					soundnum = SFXNUM_05AF_COUGH;
				} else {
					soundnum = SFXNUM_05B0_COUGH;
				}
			} else {
				s32 index = random() % 4;
				s32 sounds[] = {
					SFXNUM_05AB_COUGH,
					SFXNUM_05AC_COUGH,
					SFXNUM_05AD_COUGH,
					SFXNUM_05AE_COUGH,
				};

				soundnum = sounds[index];
			}

			chr->soundgap = 10;
			chr->soundtimer = 0;
		}
	}

	if (soundnum >= 0) {
		if (chr->prop->type == PROPTYPE_PLAYER) {
			if (g_Vars.players[playernum]->chokehandle == NULL) {
				snd_start(var80095200, soundnum, &g_Vars.players[playernum]->chokehandle, -1, -1, -1, -1, -1);
			}
		} else {
			ps_stop_sound(chr->prop, PSTYPE_CHRTALK, 0);

			if (!ps_prop_has_sound_with_context(chr->prop, PSTYPE_CHRCHOKE)) {
				ps_create(NULL, chr->prop, soundnum, -1,
						-1, 0, 0, PSTYPE_CHRCHOKE, NULL, -1, NULL, -1, -1, -1, -1);
			}
		}
	}
}

f32 chr_get_shield(struct chrdata *chr)
{
	return chr->cshield;
}

void chr_set_shield(struct chrdata *chr, f32 amount)
{
	if (amount < 0) {
		amount = 0;
	}

	chr->cshield = amount;

	if ((chr->hidden & CHRHFLAG_INFINITESHIELD) && chr->cshield < 1) {
		chr->cshield = 1;
	}

	if (chr->prop->type == PROPTYPE_PLAYER) {
		s32 playernum = playermgr_get_player_num_by_prop(chr->prop);

		if (playernum >= 0) {
			s32 prevplayernum = g_Vars.currentplayernum;
			set_current_player_num(playernum);
			player_display_health();
			g_Vars.currentplayerstats->armourcount += amount * 0.125f;
			set_current_player_num(prevplayernum);
		}
	}
}

bool chr_try_create_shieldhit(struct chrdata *chr, struct modelnode *node, struct prop *prop, struct model *model, s32 side, s16 *hitpos)
{
	if (chr_get_shield(chr) > 0) {
		if (node && (node->type & 0xff) == MODELNODETYPE_BBOX) {
			shieldhit_create(chr->prop, chr_get_shield(chr), prop, node, model, side, hitpos);
		}

		return true;
	}

	return false;
}

/**
 * Damage the chr, bypassing any shield.
 *
 * Used for knife poison, nbomb damage, Investigation radioactivity and Escape gas.
 */
void chr_damage_by_dizziness(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop)
{
	chr_damage(chr, damage, vector, gset, prop, HITPART_GENERAL,
			false,     // damageshield
			NULL,      // prop2
			NULL,      // node
			NULL,      // model
			-1,        // side
			NULL,      // hitpos
			false,     // explosion
			NULL);     // explosionpos
}

void chr_damage_by_laser(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop)
{
	chr_damage(chr, damage, vector, gset, prop, HITPART_GENERAL,
			true,      // damageshield
			chr->prop, // prop2
			NULL,      // node
			NULL,      // model
			-1,        // side
			NULL,      // hitpos
			false,     // explosion
			NULL);     // explosionpos
}

/**
 * Damage the chr due to an impact with a direction.
 *
 * Used by:
 * - Players doing melee attacks
 * - Being shot
 * - Hit by knife
 */
void chr_damage_by_impact(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop, s32 hitpart, struct prop *prop2, struct modelnode *node, struct model *model, s32 side, s16 *hitpos)
{
	chr_damage(chr, damage, vector, gset, prop, hitpart,
			true,      // damageshield
			prop2,     // prop2
			node,      // node
			model,     // model
			side,      // side
			hitpos,    // hitpos
			false,     // explosion
			NULL);     // explosionpos
}

/**
 * Unused, and same as chr_damage_by_general but sets hitpart to HITPART_GENERAL instead of argument.
 */
void chr_damage_by_general_unused(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop)
{
	struct modelnode *node = NULL;
	struct model *model = NULL;
	s32 side = 0;
	s32 hitpart = HITPART_GENERAL;

	if (chr_get_shield(chr) >= 0 && chr->model) {
		chr_calculate_shield_hit(chr, &chr->prop->pos, vector, &node, &hitpart, &model, &side);
	}

	chr_damage(chr, damage, vector, gset, prop, HITPART_GENERAL,
			true,      // damageshield
			chr->prop, // prop2
			node,      // node
			model,     // model
			side,      // side
			NULL,      // hitpos
			false,     // explosion
			NULL);     // explosionpos
}

/**
 * Used by:
 * - AI when killing chrs at end of Infilration
 * - NPCs when punching or kicking
 * - Autoguns shooting a player
 * - Non-rocket projectiles being created directly in a chr's bbox
 */
void chr_damage_by_general(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset, struct prop *prop, s32 hitpart)
{
	struct modelnode *node = NULL;
	struct model *model = NULL;
	s32 side = 0;

	if (chr_get_shield(chr) >= 0 && chr->model) {
		chr_calculate_shield_hit(chr, &chr->prop->pos, vector, &node, &hitpart, &model, &side);
	}

	chr_damage(chr, damage, vector, gset, prop, hitpart,
			true,      // damageshield
			chr->prop, // prop2
			node,      // node
			model,     // model
			side,      // side
			NULL,      // hitpos
			false,     // explosion
			NULL);     // explosionpos
}

void chr_damage_by_explosion(struct chrdata *chr, f32 damage, struct coord *vector, struct prop *prop, struct coord *explosionpos)
{
	chr_damage(chr, damage, vector, NULL, prop, HITPART_GENERAL,
			true,      // damageshield
			chr->prop, // prop2
			NULL,      // node
			NULL,      // model
			-1,        // side
			NULL,      // hitpos
			true,      // explosion
			explosionpos);
}

void player_update_damage_stats(struct prop *attacker, struct prop *victim, f32 damage)
{
	s32 playernum;

	if (attacker && attacker->type == PROPTYPE_PLAYER) {
		playernum = playermgr_get_player_num_by_prop(attacker);

		if (playernum >= 0) {
			g_Vars.playerstats[playernum].damtransmitted += damage;
		}
	}

	if (victim && victim->type == PROPTYPE_PLAYER) {
		playernum = playermgr_get_player_num_by_prop(victim);

		if (playernum >= 0) {
			g_Vars.playerstats[playernum].damreceived += damage;
		}
	}
}

/**
 * Handle a chr being damaged.
 *
 * Calculates and damage based on many factors and applies it to the chr,
 * killing them if needed.
 *
 * The chr can be a player, aibot or regular chr. The chr can also be of any
 * race (human, Skedar, Dr Caroll, robot or eyespy).
 *
 * chr - the chr being damaged
 * damage - the base amount of damage to deal, prior to scaling factors
 * vector - position of the attacker?
 * gset - gun settings struct
 * aprop - the attacker's prop struct
 * hitpart - "i've been hit" value, ie. the body part (see HITPART constants)
 * damageshield - false if attack should bypass shield if any
 * prop2 - ?
 * node - if shielded, model node (of type bbox) which was hit
 * model - if shielded, model of chr
 * side - if shielded, side of the model node's bounding box which was hit (0-5)
 * hitpos - ?
 * explosion - true if damage is coming from an explosion
 * explosionpos - position of said explosion
 */
void chr_damage(struct chrdata *chr, f32 damage, struct coord *vector, struct gset *gset,
		struct prop *aprop, s32 hitpart, bool damageshield, struct prop *prop2,
		struct modelnode *node, struct model *model, s32 side, s16 *hitpos,
		bool explosion, struct coord *explosionpos)
{
	bool onehitko = false;
	s32 race = CHRRACE(chr);
	f32 shield;
	bool makedizzy;
	bool ismelee;
	struct prop *vprop = chr->prop;
	f32 headshotdamagescale = 1;
	bool usedshield = false;
	bool showshield = false;
	bool showdamage = false;
	struct gset gset2 = {0};
	f32 explosionforce = damage;
	f32 healthscale = 1;
	f32 armourscale = 1;
	bool isshoot = true;
	bool forceapplydamage = false;
	struct funcdef *func;
	f32 amount;
	bool grunt = true;
	s32 aplayernum = -1;
	s32 choketype = CHOKETYPE_NONE;

	if (hitpart == HITPART_HEAD) {
		choketype = CHOKETYPE_GURGLE;
	}

	if (gset) {
		if (gset->weaponnum == WEAPON_COMBATKNIFE) {
			if (gset->weaponfunc == FUNC_2) {
				grunt = false;
			}

			if (gset->weaponfunc == FUNC_POISON) {
				choketype = CHOKETYPE_COUGH;
			}
		} else if (gset->weaponnum == WEAPON_TRANQUILIZER) {
			if (gset->weaponfunc == FUNC_SECONDARY) {
				choketype = CHOKETYPE_GURGLE;
			}
		}
	}

	// Don't damage if in CI training outside of training session
	if (chr->prop == g_Vars.currentplayer->prop
			&& g_Vars.currentplayer->training == false
			&& main_get_stage_num() == STAGE_CITRAINING) {
		return;
	}

	// Don't damage if attacker was anti and chr is non-interactable by anti
	if (g_Vars.antiplayernum >= 0
			&& aprop
			&& aprop == g_Vars.anti->prop
			&& (chr->hidden & CHRHFLAG_ANTINONINTERACTABLE)) {
		return;
	}

	// Don't damage if coop and friendly fire is off (human buddy)
	if (g_Vars.coopplayernum >= 0
			&& g_Vars.coopfriendlyfire == false
			&& aprop
			&& aprop != vprop
			&& aprop->type == PROPTYPE_PLAYER
			&& vprop->type == PROPTYPE_PLAYER) {
		return;
	}

	// Don't damage if coop and friendly fire is off (AI buddy)
	if (g_MissionConfig.iscoop
			&& g_Vars.coopfriendlyfire == false
			&& aprop
			&& aprop != vprop
			&& (aprop->type == PROPTYPE_PLAYER || aprop->type == PROPTYPE_CHR)
			&& chr->team == TEAM_ALLY
			&& aprop->chr->team == TEAM_ALLY) {
		return;
	}

	// Don't allow coop AI to kill or destroy anything
	// which anti wouldn't be able to
	if (g_MissionConfig.iscoop
			&& aprop
			&& aprop != vprop
			&& aprop->type == PROPTYPE_CHR
			&& aprop->chr->team == TEAM_ALLY
			&& (chr->hidden & CHRHFLAG_ANTINONINTERACTABLE)) {
		return;
	}

	if (gset == NULL) {
		gset = &gset2;
	}

	func = gset_get_funcdef_by_gset(gset);
	ismelee = func && (func->type & 0xff) == INVENTORYFUNCTYPE_MELEE;
	makedizzy = race != RACE_DRCAROLL && gset_has_function_flags(gset, FUNCFLAG_MAKEDIZZY);

	if (chr->prop == g_Vars.currentplayer->prop && g_Vars.currentplayer->invincible) {
		return;
	}

	if (ismelee) {
		isshoot = false;
	}

	// Set a flag on the victim that makes them switch to their "shot" AI list
	chr->chrflags |= CHRCFLAG_TRIGGERSHOTLIST;

	// Set a flag on the attacker so their AI scripting can tell that they've
	// hit their target
	if (aprop
			&& aprop->type == PROPTYPE_CHR
			&& chr_get_target_prop(aprop->chr) == chr->prop) {
		aprop->chr->chrflags |= CHRCFLAG_INJUREDTARGET;
	}

	// Disarm only hurts the victim in solo missions and if the victim is an NPC
	if (gset_has_function_flags(gset, FUNCFLAG_DISARM)
			&& gset->weaponnum == WEAPON_UNARMED
			&& (vprop->type == PROPTYPE_PLAYER || g_Vars.normmplayerisrunning)) {
		damage = 0;
	}

	// Apply damage scaling based on difficulty settings
	if (g_Vars.mplayerisrunning == false) {
		// Solo
		if (explosion) {
			if (vprop->type == PROPTYPE_PLAYER) {
				damage *= g_ExplosionDamageTxScale;
			}
		} else if (aprop && aprop->type == PROPTYPE_PLAYER) {
			// Player is attacking
			damage *= g_PlayerDamageTxScale;
			headshotdamagescale = 25;
		} else if (aprop && aprop->type == PROPTYPE_CHR && vprop->type == PROPTYPE_PLAYER) {
			// Chr is attacking player
			damage *= g_PlayerDamageRxScale * pdmode_get_enemy_damage();
		}

		if (vprop->type != PROPTYPE_PLAYER) {
			damage /= pdmode_get_enemy_health();
		}

		if (vprop->type == PROPTYPE_PLAYER) {
			healthscale = g_Vars.players[playermgr_get_player_num_by_prop(vprop)]->healthscale;
			armourscale = g_Vars.players[playermgr_get_player_num_by_prop(vprop)]->armourscale;
		}
	} else if (g_Vars.coopplayernum >= 0) {
		// Co-op
		if (explosion) {
			if (vprop->type == PROPTYPE_PLAYER) {
				damage *= g_ExplosionDamageTxScale;
			}
		} else if (aprop && aprop->type == PROPTYPE_PLAYER && vprop->type != PROPTYPE_PLAYER) {
			damage *= g_PlayerDamageTxScale;
			headshotdamagescale = 25;
		} else if (aprop && aprop->type == PROPTYPE_CHR && vprop->type == PROPTYPE_PLAYER) {
			damage *= g_PlayerDamageRxScale * pdmode_get_enemy_damage();
		}

		if (vprop->type != PROPTYPE_PLAYER) {
			damage /= pdmode_get_enemy_health();
		}

		if (vprop->type == PROPTYPE_PLAYER) {
			healthscale = g_Vars.players[playermgr_get_player_num_by_prop(vprop)]->healthscale;
			armourscale = g_Vars.players[playermgr_get_player_num_by_prop(vprop)]->armourscale;
		}
	} else if (g_Vars.antiplayernum >= 0) {
		// Anti
		if (explosion) {
			if (vprop == g_Vars.bond->prop) {
				damage *= g_ExplosionDamageTxScale;
			}
		} else if (aprop && aprop == g_Vars.bond->prop) {
			damage *= g_PlayerDamageTxScale;
			headshotdamagescale = 25;
		} else if (aprop && aprop != g_Vars.bond->prop && vprop == g_Vars.bond->prop) {
			damage *= g_PlayerDamageRxScale * pdmode_get_enemy_damage();
		}

		if (vprop != g_Vars.bond->prop) {
			damage /= pdmode_get_enemy_health();
		}

		if (vprop == g_Vars.bond->prop) {
			healthscale = g_Vars.players[playermgr_get_player_num_by_prop(vprop)]->healthscale;
			armourscale = g_Vars.players[playermgr_get_player_num_by_prop(vprop)]->armourscale;
		}

		// Anti shooting other enemies is lethal
		if (aprop && aprop == g_Vars.anti->prop && vprop != g_Vars.bond->prop) {
			damage *= 100;
		}
	} else {
		// Normal multiplayer
		if (vprop->type == PROPTYPE_PLAYER) {
			s32 prevplayernum = g_Vars.currentplayernum;
			set_current_player_num(playermgr_get_player_num_by_prop(vprop));
			damage *= g_Vars.currentplayerstats->damagescale;
			set_current_player_num(prevplayernum);
		}
	}

	// Apply rumble
	if (vprop->type == PROPTYPE_PLAYER) {
		s32 prevplayernum = g_Vars.currentplayernum;

#if VERSION >= VERSION_NTSC_1_0
		s32 contpad1;
		s32 contpad2;

		set_current_player_num(playermgr_get_player_num_by_prop(vprop));

		joy_get_contpad_nums_for_player(g_Vars.currentplayernum, &contpad1, &contpad2);

		if (contpad1 >= 0) {
			pak_rumble(contpad1, 0.25f, -1, -1);
		}

		if (contpad2 >= 0) {
			pak_rumble(contpad2, 0.25f, -1, -1);
		}
#else
		set_current_player_num(playermgr_get_player_num_by_prop(vprop));

		pak_rumble((s8)g_Vars.currentplayernum, 0.25f, -1, -1);

		if (options_get_control_mode(g_Vars.currentplayerstats->mpindex) >= CONTROLMODE_21) {
			pak_rumble((s8)(PLAYERCOUNT() + g_Vars.currentplayernum), 0.25f, -1, -1);
		}
#endif

		set_current_player_num(prevplayernum);
	}

	// Find the attacker's player number if possible
	// (includes MP aibots, not applicable for solo chrs)
	if (g_Vars.mplayerisrunning) {
		if (aprop && (aprop->type == PROPTYPE_PLAYER || aprop->type == PROPTYPE_CHR)) {
			aplayernum = mp_chr_to_chrindex(aprop->chr);
		}
	} else {
		if (aprop && aprop->type == PROPTYPE_PLAYER) {
			aplayernum = playermgr_get_player_num_by_prop(aprop);
		}
	}

	// If using the shotgun, scale the damage based on distance
	if (aprop && aprop->type == PROPTYPE_CHR && gset->weaponnum == WEAPON_SHOTGUN) {
		f32 xdiff = aprop->pos.x - vprop->pos.x;
		f32 ydiff = aprop->pos.y - vprop->pos.y;
		f32 zdiff = aprop->pos.z - vprop->pos.z;
		f32 sqdist = xdiff * xdiff + ydiff * ydiff + zdiff * zdiff;

		if (sqdist < 200 * 200) {
			damage *= 4.0f + (s32)(random() % 3); // 4, 5 or 6
		} else if (sqdist < 400 * 400) {
			damage *= 3.0f + (s32)(random() % 2); // 3 or 4
		} else if (sqdist < 800 * 800) {
			damage *= 2.0f + (s32)(random() % 2); // 2 or 3
		} else if (sqdist < 1600 * 1600) {
			damage *= 1.0f + (s32)(random() % 2); // 1 or 2
		}
	}

	// damageshield is an argument to this function,
	// but is forced on if using the Farsight.
	if (gset && gset->weaponnum == WEAPON_FARSIGHT) {
		damageshield = true;
		damage *= 10;
	}

	// Handle shield damage
	if (damageshield) {
		shield = chr_get_shield(chr);

		if (chr->aibot && chr->aibot->config->type == BOTTYPE_TURTLE) {
			armourscale = 4;
		}

		if (shield > 0) {
			if (g_Vars.normmplayerisrunning) {
#if VERSION >= VERSION_PAL_FINAL
				// Fixing a @bug
				damage = damage * mp_handicap_to_value(g_PlayerConfigsArray[g_Vars.currentplayerstats->mpindex].handicap);
#else
				damage = damage / mp_handicap_to_value(g_PlayerConfigsArray[g_Vars.currentplayerstats->mpindex].handicap);
#endif
			}

			chr->chrflags |= CHRCFLAG_SHIELDDAMAGED;

			if (prop2 && node && chr->model) {
				chr_try_create_shieldhit(chr, node, prop2, model, side, hitpos);
			} else {
				shieldhit_create(chr->prop, chr_get_shield(chr), NULL, NULL, NULL, 0, NULL);
			}

			if (g_Vars.normmplayerisrunning && (g_MpSetup.options & MPOPTION_ONEHITKILLS)) {
				damage = 0;
				chr_set_shield(chr, 0);
			} else if (shield >= damage / armourscale) {
				// Has enough shield to sustain the damage
				shield -= damage / armourscale;
				damage = 0;
				chr_set_shield(chr, shield);
			} else {
				// Shield is now gone
				damage = 0;
				chr_set_shield(chr, 0);
			}

			showshield = true;
			usedshield = true;
		}
	}

	// Handle hat shots. This is left over from GE, as hats don't exist in PD
	if (damage > 0 && hitpart == HITPART_HAT && chr->weapons_held[2]) {
		s32 type = hat_get_type(chr->weapons_held[2]);

		if (type == HATTYPE_CLOTH) {
			// Hat remains on head and damages the chr (eg. Moonraker Elite)
			hitpart = HITPART_HEAD;
		} else if (type != HATTYPE_METAL) {
			// Normal hat
			damage = 0;
			obj_set_dropped(chr->weapons_held[2], DROPTYPE_HAT);
			chr->hidden |= CHRHFLAG_DROPPINGITEM;
		} else {
			// Metal helmets don't fall off and make a metallic chink noise when shot
			u16 sounds[] = { SFXMAP_807B_HIT_METAL, SFXMAP_8079_HIT_METAL, SFXMAP_807C_HATHIT };
			damage = 0;

			ps_create(NULL, chr->prop, sounds[random() % 3], -1,
					-1, 0, 0, PSTYPE_NONE, NULL, -1, NULL, -1, -1, -1, -1);
		}
	}

	// Handle incrementing player shot count
	if (aprop && aprop->type == PROPTYPE_PLAYER && !explosion) {
		bool alreadydead = false;
		s32 prevplayernum = g_Vars.currentplayernum;
		set_current_player_num(playermgr_get_player_num_by_prop(aprop));

		// ACT_DIE is not checked here, so it would appear that shooting
		// a chr as they're dying will increment the shots hit count
		if (chr && chr->actiontype == ACT_DEAD) {
			alreadydead = true;
		}

		if (vprop->type == PROPTYPE_PLAYER && g_Vars.players[playermgr_get_player_num_by_prop(vprop)]->isdead) {
			alreadydead = true;
		}

		if (!alreadydead && hitpart) {
			switch (hitpart) {
			case HITPART_HEAD:
				mpstats_increment_player_shotcount(gset, SHOTREGION_HEAD);
				break;
			case HITPART_GUN:
				mpstats_increment_player_shotcount(gset, SHOTREGION_GUN);
				break;
			case HITPART_HAT:
				mpstats_increment_player_shotcount(gset, SHOTREGION_HAT);
				break;
			case HITPART_PELVIS:
			case HITPART_TORSO:
				mpstats_increment_player_shotcount(gset, SHOTREGION_BODY);
				break;
			default:
				mpstats_increment_player_shotcount(gset, SHOTREGION_LIMB);
				break;
			}
		}

		set_current_player_num(prevplayernum);
	}

	// If the chr is invincible, make them flinch then we're done
	if (chr->chrflags & CHRCFLAG_INVINCIBLE) {
		chr_flinch_body(chr);
		return;
	}

	// If chr is dying or already dead, consider making their head flinch
	// then we're done
	if (chr->actiontype == ACT_DIE || chr->actiontype == ACT_DEAD) {
		if (hitpart == HITPART_HEAD && chr->actiontype == ACT_DIE && race != RACE_SKEDAR && isshoot) {
			struct coord pos;
			pos.x = vprop->pos.x - vector->x;
			pos.y = vprop->pos.y - vector->y;
			pos.z = vprop->pos.z - vector->z;
			chr_flinch_head(chr, chr_get_angle_to_pos(chr, &pos));
		}

		return;
	}

	// At this point the chr is known to be alive before they were shot
	{
		struct prop *weapon;
		struct coord sp9c;
		f32 angle;

		sp9c.x = vprop->pos.x - vector->x;
		sp9c.y = vprop->pos.y - vector->y;
		sp9c.z = vprop->pos.z - vector->z;
		angle = chr_get_angle_to_pos(chr, &sp9c);

		// Knife in the back to an unalerted chr is lethal
		if (gset->weaponnum == WEAPON_COMBATKNIFE
				&& gset->weaponfunc == FUNC_PRIMARY
				&& angle > BADDTOR(120)
				&& angle < RAD(240, 4.1881237030029f)
				&& (chr->alertness < 100 || chr->lastseetarget60 == 0)) {
			damage *= 1000;
		}

		// Punching and pistol whipping is less effective from the front
		if (gset_has_function_flags(gset, FUNCFLAG_BLUNTIMPACT)) {
			if (angle < BADDTOR(60) || angle > BADDTOR(300)) {
				damage *= 0.4f;
			} else if (angle < BADDTOR(120) || angle > RAD(240, 4.1881237030029f)) {
				damage *= 0.7f;
			} else if (chr->alertness < 100) {
				onehitko = true;
			}

			if (chr_get_held_prop(chr, HAND_RIGHT) == NULL
					&& chr_get_held_prop(chr, HAND_LEFT) == NULL
					&& (chr->gunprop == NULL || chr->actiontype == ACT_SURRENDER || chr->actiontype == ACT_SURPRISED)) {
				// Chr is unarmed and has no hope of getting their gun
				onehitko = true;
			}

			forceapplydamage = true;
		}

		if (hitpart == HITPART_GENERAL) {
			// Halve the damage because it's doubled for torso below
			hitpart = HITPART_TORSO;
			damage *= 0.5f;
		} else if (hitpart == HITPART_GENERALHALF) {
			// Likewise, quarter it here so it becomes half below
			hitpart = HITPART_TORSO;
			damage *= 0.25f;
		}

		// Hits to a Skedar's tail are 10x more lethal
		if (race == RACE_SKEDAR && hitpart == HITPART_TAIL) {
			damage *= 10;
		}

		// Apply damage multipliers based on which body parts were hit,
		// and flinch head if shot in the head
		if (hitpart == HITPART_HEAD) {
			if (race == RACE_SKEDAR) {
				damage += damage;
				chr_flinch_head(chr, angle);
			} else {
				damage *= 4;

				if (isshoot && !usedshield) {
					chr_flinch_head(chr, angle);
					damage *= headshotdamagescale;

					if (gset->weaponnum == WEAPON_COMBATKNIFE && gset->weaponfunc != FUNC_POISON) {
						damage += damage;
					}
				}
			}
		} else if (hitpart == HITPART_TORSO) {
			// Double damage for torso hits
			damage += damage;
		} else if (hitpart == HITPART_GUN) {
			// No damage for gun hits
			damage = 0;
			makedizzy = false;
		} else if (hitpart == HITPART_HAT) {
			// No damage for hat hits
			damage = 0;
			makedizzy = false;
		}

		// Handle situations where the player is the one being shot, then return
		if (vprop->type == PROPTYPE_PLAYER) {
			s32 prevplayernum = g_Vars.currentplayernum;
			set_current_player_num(playermgr_get_player_num_by_prop(vprop));

			if (g_Vars.normmplayerisrunning) {
				damage /= mp_handicap_to_value(g_PlayerConfigsArray[g_Vars.currentplayerstats->mpindex].handicap);
			}

			if (g_Vars.currentplayer->isdead == false && !g_PlayerInvincible) {
				f32 boostscale;

				// Handle player losing gun
				if (gset_has_function_flags(gset, FUNCFLAG_DISARM)) {
					bgun_disarm(aprop);
				}

				// Handle player dizziness
				if (makedizzy && g_Vars.currentplayer->invincible == false) {
					f32 blurscale = 1;
					struct chrdata *achr = NULL;

					if (aprop) {
						achr = aprop->chr;

						if (achr && achr->bodynum == BODY_MINISKEDAR) {
							blurscale = 4;
						}
					}

					if (!achr
							|| !achr->aibot
							|| !gset_has_function_flags(gset, FUNCFLAG_00400000)
							|| chr->blurdrugamount < TICKS(4500)) {
						chr->blurdrugamount += gset_get_blur_amount(gset) * blurscale;
					}

					chr->blurnumtimesdied = 0;
				}

				// Handle player damage
				if (g_Vars.currentplayer->invincible == false && damage > 0) {
					f32 statsamount = amount = damage * 0.125f;

					if (statsamount > g_Vars.currentplayer->bondhealth) {
						statsamount = g_Vars.currentplayer->bondhealth;
					}

					if (g_Vars.normmplayerisrunning && (g_MpSetup.options & MPOPTION_ONEHITKILLS)) {
						statsamount = g_Vars.currentplayer->bondhealth;
					}

					player_update_damage_stats(aprop, vprop, statsamount);
					player_display_health();

					if (g_Vars.normmplayerisrunning && (g_MpSetup.options & MPOPTION_ONEHITKILLS)) {
						g_Vars.currentplayer->bondhealth = 0;
					}

					g_Vars.currentplayer->bondhealth -= amount / healthscale;

					chr->lastattacker = (aprop ? aprop->chr : NULL);

					showdamage = true;

					if (g_Vars.currentplayer->training == false
							&& g_Vars.currentplayer->bondhealth <= 0) {
						player_die_by_shooter(aplayernum, false);
						chr->blurnumtimesdied++;
					}

					if (!lv_is_paused() && grunt) {
						chr_grunt(chr, choketype);
					}

					chr_flinch_body(chr);
				}

				// Handle player boost
				if (ismelee && gset->weaponnum == WEAPON_REAPER) {
					boostscale = 0.1f;
				} else if (g_Vars.normmplayerisrunning) {
					boostscale = 0.75f;
				} else {
					boostscale = 1;
				}

				g_Vars.currentplayer->bondshotspeed.x += vector->x * boostscale;
				g_Vars.currentplayer->bondshotspeed.z += vector->z * boostscale;

				if (showdamage) {
					player_display_damage();
				}

				if (showshield) {
					player_display_shield();
				}

				if (g_Vars.normmplayerisrunning && aprop && aprop->type == PROPTYPE_PLAYER) {
					player_check_if_shot_in_back(prevplayernum, vector->x, vector->z);
				}
			}

			set_current_player_num(prevplayernum);
			return;
		}

		// This check is pointless - a similar check and return exists earlier
		if (chr->actiontype == ACT_DIE || chr->actiontype == ACT_DEAD) {
			return;
		}

		// At this point we know we're dealing with a NPC being shot, and the
		// NPC was alive prior to being shot.

		// Handle aibot/chr losing gun
		if (gset_has_function_flags(gset, FUNCFLAG_DISARM)
				&& ((chr->flags & CHRFLAG0_CANLOSEGUN) || chr->aibot)) {
			if (chr->aibot) {
				bot_disarm(chr, aprop);
			} else {
				weapon = chr_get_held_prop(chr, HAND_RIGHT);

				if (weapon) {
					chr->gunprop = weapon;
					obj_set_dropped(weapon, DROPTYPE_DEFAULT);
					chr->hidden |= CHRHFLAG_DROPPINGITEM;
				}

				weapon = chr_get_held_prop(chr, HAND_LEFT);

				if (weapon) {
					chr->gunprop = weapon;
					obj_set_dropped(weapon, DROPTYPE_DEFAULT);
					chr->hidden |= CHRHFLAG_DROPPINGITEM;
				}
			}
		}

		// Handle chr damage
		if (chr->damage < chr->maxdamage
				|| (!g_Vars.normmplayerisrunning && chr->actiontype != ACT_PREARGH)) {
			f32 sp80 = 0;

			chr->numarghs++;

			// Handle chr dizziness and psychosis
			if (makedizzy && race != RACE_DRCAROLL && race != RACE_ROBOT) {
				if (gset_has_function_flags(gset, FUNCFLAG_PSYCHOSIS)) {
					chr->hidden |= CHRHFLAG_PSYCHOSISED;
				} else {
					chr->blurdrugamount += gset_get_blur_amount(gset);
					chr->blurnumtimesdied = 0;

					if (!chr->aibot && chr->blurdrugamount >= TICKS(5000)) {
						onehitko = true;
					}
				}
			}

			// Handle chr boost
			if (chr->aibot) {
				f32 boostscale;

				if (ismelee && gset->weaponnum == WEAPON_REAPER) {
					boostscale = 0.1f;
				} else {
					boostscale = 0.75f;
				}

				chr->aibot->shotspeed.x += vector->x * boostscale;
				chr->aibot->shotspeed.z += vector->z * boostscale;
			}

			if (gset->weaponnum == WEAPON_UNARMED) {
				sp80 = 2;
			}

			if (gset->weaponnum == WEAPON_TRANQUILIZER || gset->weaponnum == WEAPON_PSYCHOSISGUN) {
				forceapplydamage = true;
			}

			// Handle one-hit knockouts
			if (onehitko && chr->aibot == NULL && race == RACE_HUMAN) {
				chr_knockout(chr, angle, hitpart, gset);
				ps_stop_sound(chr->prop, PSTYPE_CHRTALK, 0);

				if (grunt) {
					chr_grunt(chr, choketype);
				}

				if (gset->weaponnum == WEAPON_UNARMED && chr->actiontype != ACT_DRUGGEDKO) {
					return;
				}

				return;
			}

			// Handle applying damage to NPCs
			// Don't enter this branch if there is no damage to give,
			// or we are making a chr dizzy in solo mode (unless force is set)
			if (damage > 0 && (g_Vars.normmplayerisrunning || !makedizzy || forceapplydamage)) {
				f32 amount = damage;

				if (chr->damage + damage > chr->maxdamage) {
					amount = chr->maxdamage - chr->damage;
				}

				amount *= 0.125f;

				player_update_damage_stats(aprop, vprop, amount);

				chr->damage += damage;
				chr->lastattacker = (aprop ? aprop->chr : NULL);
				chr->chrflags |= CHRCFLAG_JUST_INJURED;

				if (chr->aibot) {
					if (g_Vars.normmplayerisrunning && (g_MpSetup.options & MPOPTION_ONEHITKILLS)) {
						chr->damage = chr->maxdamage;
					}

					if (grunt) {
						chr_grunt(chr, choketype);
					}

					chr_flinch_body(chr);

					if (chr->damage >= chr->maxdamage) {
						chr_die(chr, aplayernum);
					}
				} else if (explosion) {
					// Chrs die instantly from explosion damage provided they
					// don't have any armour (the chr has armour if their
					// chr->damage is negative). Note that damage has already
					// been applied to the chr above, so a perfectly healthy chr
					// with no armour will already have a damage value > 0 at
					// this point.
					if (chr->damage > 0) {
						chr->damage = chr->maxdamage;

						if (race == RACE_DRCAROLL || race == RACE_EYESPY || race == RACE_ROBOT) {
							chr_begin_death(chr, vector, angle, hitpart, gset, false, aplayernum);
						} else {
							chr_yeet_from_pos(chr, explosionpos, explosionforce);
						}

						if (grunt) {
							chr_grunt(chr, choketype);
						}

						if (g_Vars.mplayerisrunning) {
							mpstats_record_death(aplayernum, mp_chr_to_chrindex(chr));
						} else if (aprop && aprop->type == PROPTYPE_PLAYER) {
							s32 prevplayernum = g_Vars.currentplayernum;
							set_current_player_num(playermgr_get_player_num_by_prop(aprop));
							mpstats_record_player_kill();
							set_current_player_num(prevplayernum);
						}

						if (chr->chrflags & CHRCFLAG_KILLCOUNTABLE) {
							mpstats_increment_total_kill_count();
						}

						if (chr->aibot == NULL) {
							chr_drop_concealed_items(chr);
						}

						if (chr->aibot == NULL) {
							weapon = chr->weapons_held[HAND_RIGHT];

							if (weapon && (weapon->obj->flags & OBJFLAG_AIUNDROPPABLE) == 0) {
								obj_set_dropped(weapon, DROPTYPE_DEFAULT);
								chr->hidden |= CHRHFLAG_DROPPINGITEM;
							}

							weapon = chr->weapons_held[HAND_LEFT];

							if (weapon && (weapon->obj->flags & OBJFLAG_AIUNDROPPABLE) == 0) {
								obj_set_dropped(weapon, DROPTYPE_DEFAULT);
								chr->hidden |= CHRHFLAG_DROPPINGITEM;
							}
						}
					}
				} else {
					// Non-explosion damage to solo mode chr
					if (chr->actiontype != ACT_DRUGGEDKO && grunt) {
						chr_grunt(chr, choketype);
					}

					if (makedizzy && chr->damage >= chr->maxdamage) {
						chr->damage = chr->maxdamage - 0.1f;
						chr_knockout(chr, angle, hitpart, gset);
					}

					// If chr has armour or the weapon doesn't stun
					if (chr->damage < 0 ||
							(gset_has_function_flags(gset, FUNCFLAG_NOSTUN) && chr->damage < chr->maxdamage)) {
						f32 endframe = -1;

						if (!chr_is_anim_preventing_argh(chr, &endframe)) {
							chr_flinch_body(chr);
						}
					} else if (hitpart != HITPART_HAT) {
						// Cancel current animation and prepare for argh
						f32 endframe = -1;

						if (chr_is_anim_preventing_argh(chr, &endframe)) {
							if (endframe >= 0) {
								model_set_anim_end_frame(chr->model, endframe);
							}

							chr->actiontype = ACT_PREARGH;
							chr->act_preargh.dir.x = vector->x;
							chr->act_preargh.dir.y = vector->y;
							chr->act_preargh.dir.z = vector->z;
							chr->act_preargh.relshotdir = angle;
							chr->act_preargh.hitpart = hitpart;
							chr->act_preargh.aplayernum = aplayernum;
							chr->act_preargh.gset.weaponnum = gset->weaponnum;
							chr->act_preargh.gset.upgradewant = gset->upgradewant;
							chr->act_preargh.gset.miscbyte = gset->miscbyte;
							chr->act_preargh.gset.weaponfunc = gset->weaponfunc;

							chr->sleep = 0;
						} else {
							chr_react_to_damage(chr, vector, angle, hitpart, gset, aplayernum);
						}
					}
				}

				if (sp80 > 0) {
					chr->timeextra = sp80 * 15;
					chr->elapseextra = 0;

					chr->extraspeed.x = prop2->pos.x - aprop->pos.x;
					chr->extraspeed.y = prop2->pos.y - aprop->pos.y;
					chr->extraspeed.z = prop2->pos.z - aprop->pos.z;

					guNormalize(&chr->extraspeed.x, &chr->extraspeed.y, &chr->extraspeed.z);

					chr->extraspeed.x *= sp80;
					chr->extraspeed.y *= sp80;
					chr->extraspeed.z *= sp80;
				}
			}
		}
	}
}

void chr_die(struct chrdata *chr, s32 aplayernum)
{
	if (chr->actiontype != ACT_DIE) {
		chr_stop_firing(chr);
		chr_uncloak(chr, true);

		chr->actiontype = ACT_DIE;
		chr->act_die.notifychrindex = 0;
		chr->sleep = 0;
		chr->blurnumtimesdied++;
		chr->act_die.thudframe1 = -1;
		chr->act_die.thudframe2 = -1;
		chr->act_die.timeextra = 0;

		chr->ailist = ailist_find_by_id(GAILIST_AIBOT_DEAD);
		chr->aioffset = 0;

		mpstats_record_death(aplayernum, mp_chr_to_chrindex(chr));
		botinv_drop_all(chr, chr->aibot->weaponnum);

#if VERSION >= VERSION_NTSC_1_0
		chr->aibot->hasbriefcase = false;
		chr->aibot->hascase = false;
		chr->aibot->unk04c_04 = false;
		chr->aibot->unk04c_03 = false;
		chr->aibot->hasuplink = false;
#endif
	}
}

/**
 * Test if the chr can move directly to the given pos.
 *
 * Directly means that no consideration is made for turning circles.
 */
bool chr_can_move_directly_to_pos(struct chrdata *chr,
		struct coord *frompos, RoomNum *fromrooms,
		struct coord *topos, struct coord *torooms, s32 arg5)
{
	bool result = false;
	f32 ymax;
	f32 ymin;
	f32 radius;
	RoomNum rooms[8];
	struct prop *prop = chr->prop;

	chr_get_bbox(prop, &radius, &ymax, &ymin);
	chr_set_perim_enabled(chr, false);

	if (cd_test_cyl_move04(frompos, fromrooms, topos, rooms, arg5, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION) {
		if (cd_test_cyl_move01(topos, rooms, torooms, arg5, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION) {
			result = true;
		}
	}

	chr_set_perim_enabled(chr, true);

	return result;
}

/**
 * Calculate if the chr can move from frompos to topos without using the nav system.
 *
 * The chr must have line of sight to topos.
 *
 * The function factors in space needed for the chr to turn to face the topos,
 * as well as turning once topos is reached, so several line of sight checks are done.
 *
 * The caller may provide the dir argument (direction between the two positions)
 * if already known, otherwise it will be calculated.
 */
bool chr_prop_can_move_to_pos_without_nav(struct chrdata *chr,
		struct coord *frompos, RoomNum *fromrooms,
		struct coord *topos, RoomNum *torooms,
		struct coord *dir, f32 turndist, u32 cdtypes)
{
	struct coord tmpdir;
	f32 turndistx;
	f32 turndistz;
	struct prop *prop;
	bool result = false;
	f32 ymax;
	f32 ymin;
	f32 radius;
	struct coord nearfrompos;
	struct coord neartopos;
	RoomNum nearfromrooms[8];
	f32 mult;

	prop = chr->prop;

	chr_set_perim_enabled(chr, false);
	chr_get_bbox(prop, &radius, &ymax, &ymin);

	// Test if the chr has cylindar gap to topos
	if ((torooms != NULL && cd_test_cyl_move02(frompos, fromrooms, topos, torooms, cdtypes, true, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION)
			|| (torooms == NULL && cd_test_cyl_move01(frompos, fromrooms, topos, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION)) {
		// If no dir was given to this function, calculate it
		if (dir == NULL) {
			dir = &tmpdir;

			tmpdir.x = topos->x - frompos->x;
			tmpdir.y = 0;
			tmpdir.z = topos->z - frompos->z;

			if (tmpdir.f[0] == 0 && tmpdir.f[2] == 0) {
				// @bug: Needs to call chr_set_perim_enabled(chr, true) before returning.
				// This likely never happens anyway because the frompos and topos would have to be the same.
				return true;
			}

			mult = 1.0f / sqrtf(tmpdir.f[0] * tmpdir.f[0] + tmpdir.f[2] * tmpdir.f[2]);
			tmpdir.x *= mult;
			tmpdir.z *= mult;
		}

		// Calculate positions one turndist perpendicular to the direction
		// and test those as well
		turndistx = dir->x * turndist;
		turndistz = dir->z * turndist;

		nearfrompos.x = frompos->x + turndistz;
		nearfrompos.y = frompos->y;
		nearfrompos.z = frompos->z - turndistx;

		neartopos.x = topos->x + turndistz;
		neartopos.y = topos->y;
		neartopos.z = topos->z - turndistx;

		if (cd_test_cyl_move04(frompos, fromrooms, &nearfrompos, nearfromrooms, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION
				&& cd_test_cyl_move01(&nearfrompos, nearfromrooms, &neartopos, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION) {
			// And again, but perpendicular on the other side
			nearfrompos.x = frompos->x - turndistz;
			nearfrompos.y = frompos->y;
			nearfrompos.z = frompos->z + turndistx;

			neartopos.x = topos->x - turndistz;
			neartopos.y = topos->y;
			neartopos.z = topos->z + turndistx;

			if (cd_test_cyl_move04(frompos, fromrooms, &nearfrompos, nearfromrooms, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION
					&& cd_test_cyl_move01(&nearfrompos, nearfromrooms, &neartopos, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION) {
				result = true;
			}
		}
	}

	chr_set_perim_enabled(chr, true);

	return result;
}

bool chr_prop_can_move_to_pos_without_nav_fast(struct prop *prop, struct coord *dstpos, struct coord *dir)
{
	return chr_prop_can_move_to_pos_without_nav(prop->chr, &prop->pos, prop->rooms, dstpos, NULL, dir, prop->chr->radius * 1.2f, CDTYPE_ALL);
}

bool chr_prop_can_move_in_dir(struct prop *prop, struct coord *dir, f32 dist)
{
	struct coord dstpos;

	dstpos.x = dir->x * dist + prop->pos.x;
	dstpos.y = prop->pos.y;
	dstpos.z = dir->z * dist + prop->pos.z;

	return chr_prop_can_move_to_pos_without_nav_fast(prop, &dstpos, dir);
}

bool chr_prop_can_move_to_pos_without_nav_slow(struct prop *prop, struct coord *dstpos)
{
	return chr_prop_can_move_to_pos_without_nav(prop->chr, &prop->pos, prop->rooms, dstpos, NULL, NULL, prop->chr->radius * 1.2f, CDTYPE_ALL);
}

void chr_get_side_vector_to_target(struct chrdata *chr, bool side, struct coord *vector)
{
	struct prop *prop = chr->prop;
	struct prop *target = chr_get_target_prop(chr);

	vector->x = 0;
	vector->y = 0;
	vector->z = 1;

	if (target) {
		f32 x = target->pos.x - prop->pos.x;
		f32 z = target->pos.z - prop->pos.z;
		f32 distance = sqrtf(x * x + z * z);

		if (distance > 0) {
			x = x / distance;
			z = z / distance;

			if (side) {
				vector->x = z;
				vector->y = 0;
				vector->z = -x;
			} else {
				vector->x = -z;
				vector->y = 0;
				vector->z = x;
			}
		}
	}
}

bool chr_can_roll_in_direction(struct chrdata *chr, bool side, f32 distance)
{
	struct prop *prop = chr->prop;
	struct coord dir;
	struct coord dstpos;

	chr_get_side_vector_to_target(chr, side, &dir);

	dstpos.x = dir.x * distance + prop->pos.x;
	dstpos.y = prop->pos.y;
	dstpos.z = dir.z * distance + prop->pos.z;

	return chr_prop_can_move_to_pos_without_nav_fast(prop, &dstpos, &dir);
}

void chr_get_side_vector(struct chrdata *chr, bool side, struct coord *vector)
{
	f32 angle = chr_get_theta(chr);

	if (side) {
		vector->x = cosf(angle);
		vector->y = 0;
		vector->z = -sinf(angle);
	} else {
		vector->x = -cosf(angle);
		vector->y = 0;
		vector->z = sinf(angle);
	}
}

bool chr_can_jump_in_direction(struct chrdata *chr, bool side, f32 distance)
{
	struct prop *prop = chr->prop;
	struct coord dir;
	struct coord dstpos;

	chr_get_side_vector(chr, side, &dir);

	dstpos.x = dir.x * distance + prop->pos.x;
	dstpos.y = prop->pos.y;
	dstpos.z = dir.z * distance + prop->pos.z;

	return chr_prop_can_move_to_pos_without_nav_fast(prop, &dstpos, &dir);
}

bool chr_is_room_offscreen(struct chrdata *chr, struct coord *waypos, RoomNum *wayrooms)
{
	struct prop *prop = chr->prop;
	RoomNum sp7c[20];
	u32 stack;
	s32 i;
	RoomNum sp64[8];
	bool offscreen = true;
	RoomNum sp50[8];

	if ((chr->hidden & CHRHFLAG_CLOAKED) == 0 || USINGDEVICE(DEVICE_IRSCANNER)) {
		los_find_intersecting_rooms_exhaustive(&prop->pos, prop->rooms, waypos, sp64, sp7c, 20);

		if (g_Vars.mplayerisrunning) {
			for (i = 0; sp7c[i] != -1; i++) {
				if (g_MpRoomVisibility[sp7c[i]] & 0x0f) {
					offscreen = false;
					break;
				}
			}
		} else {
			for (i = 0; sp7c[i] != -1; i++) {
				if (g_Rooms[sp7c[i]].flags & ROOMFLAG_ONSCREEN) {
					offscreen = false;
					break;
				}
			}
		}
	}

	if (offscreen) {
		for (i = 0; i < PLAYERCOUNT(); i++) {
			portal_find_rooms(waypos, &g_Vars.players[i]->prop->pos, wayrooms, sp50, 0, 0);

			if (array_intersects(g_Vars.players[i]->prop->rooms, sp50)) {
				offscreen = false;
				break;
			}
		}
	}

	return offscreen;
}

void chr_gopos_init_magic(struct chrdata *chr, struct waydata *waydata, struct coord *padpos, struct coord *chrpos)
{
	f32 xdiff1 = padpos->x - chr->prop->pos.x;
	f32 zdiff1 = padpos->z - chr->prop->pos.z;

	f32 angle = atan2f(xdiff1, zdiff1);

	f32 xdiff2 = padpos->x - chrpos->x;
	f32 zdiff2 = padpos->z - chrpos->z;

	waydata->mode = WAYMODE_MAGIC;

	waydata->magictotal = sqrtf(xdiff1 * xdiff1 + zdiff1 * zdiff1);
	waydata->magicdone = waydata->magictotal - sqrtf(xdiff2 * xdiff2 + zdiff2 * zdiff2);

	chr_set_theta(chr, angle);
}

void chr_gopos_get_cur_waypoint_info_with_flags(struct chrdata *chr, struct coord *pos, RoomNum *rooms, u32 *flags)
{
	struct waypoint *waypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex];
	struct pad pad;

	if (waypoint) {
		pad_unpack(waypoint->padnum, PADFIELD_POS | PADFIELD_ROOM | PADFIELD_FLAGS, &pad);

		pos->x = pad.pos.x;
		pos->y = pad.pos.y;
		pos->z = pad.pos.z;

		rooms[0] = pad.room;
		rooms[1] = -1;

		if (flags) {
			*flags = pad.flags;
		}
	} else {
		pos->x = chr->act_gopos.endpos.x;
		pos->y = chr->act_gopos.endpos.y;
		pos->z = chr->act_gopos.endpos.z;

		rooms[0] = chr->act_gopos.endrooms[0];
		rooms[1] = -1;

		if (flags) {
			*flags = 0;
		}
	}
}

void chr_gopos_get_cur_waypoint_info(struct chrdata *chr, struct coord *pos, RoomNum *rooms)
{
	chr_gopos_get_cur_waypoint_info_with_flags(chr, pos, rooms, NULL);
}

f32 chr_get_run_speed(struct chrdata *chr)
{
	f32 result;

	if (chr->aibot) {
		result = bot_calculate_max_speed(chr);
	} else {
		s16 animnum = model_get_anim_num(chr->model);
		result = chr_get_anim_movedist_per_frame(animnum) * (chr->model->scale * 9.999999f);
	}

	return result;
}

s32 chr_gopos_calculate_base_ttl(struct chrdata *chr)
{
	f32 xdiff;
	f32 zdiff;
	u32 stack;
	struct coord pos;
	RoomNum rooms[8];
	f32 speed;

	chr_gopos_get_cur_waypoint_info(chr, &pos, rooms);

	xdiff = pos.x - chr->prop->pos.x;
	zdiff = pos.z - chr->prop->pos.z;

	if (xdiff < 0) {
		xdiff = -xdiff;
	}

	if (zdiff < 0) {
		zdiff = -zdiff;
	}

	speed = chr_get_run_speed(chr);

	if (chr->aibot == NULL) {
		speed *= model_get_abs_anim_speed(chr->model);
	}

	if (speed < 0.001f) {
		speed = 0.001f;
	}

	return (xdiff + zdiff) / speed;
}

void chr_gopos_clear_restart_ttl(struct chrdata *chr)
{
	chr->act_gopos.restartttl = 0;
}

void chr_gopos_consider_restart(struct chrdata *chr)
{
	if (chr->act_gopos.waydata.mode != WAYMODE_MAGIC
			&& chr->liftaction != LIFTACTION_WAITINGONLIFT
			&& chr->liftaction != LIFTACTION_WAITINGFORLIFT) {
		if (chr->act_gopos.restartttl == 0) {
#if PAL
			s32 value = (chr_gopos_calculate_base_ttl(chr) * 100 + 15000) / 60;
#else
			s32 value = chr_gopos_calculate_base_ttl(chr) * 2 + 300;
#endif

			if (value > 0xffff) {
				value = 0xffff;
			}

			chr->act_gopos.restartttl = value;
		} else if (chr->act_gopos.restartttl <= (u16)g_Vars.lvupdate60) {
			if (chr->aibot) {
				bot_check_fetch(chr);
			} else {
				chr_go_to_room_pos(chr, &chr->act_gopos.endpos, chr->act_gopos.endrooms, chr->act_gopos.flags);
			}
		} else {
			chr->act_gopos.restartttl -= (u16)g_Vars.lvupdate60;
		}
	}
}

void chr_gopos_init_expensive(struct chrdata *chr)
{
	struct coord pos;
	RoomNum rooms[8];

	chr_gopos_get_cur_waypoint_info(chr, &pos, rooms);

	chr->act_gopos.waydata.mode = WAYMODE_INIT;
	chr->act_gopos.waydata.iter = 0;
	chr->act_gopos.waydata.gotaimpos = false;
	chr->act_gopos.waydata.aimpos.x = pos.x;
	chr->act_gopos.waydata.aimpos.y = pos.y;
	chr->act_gopos.waydata.aimpos.z = pos.z;

	chr_gopos_clear_restart_ttl(chr);
}

/**
 * Advance the chr's current waypoint index to the next one in the route.
 *
 * The waypoints array allows 6 waypoints and it's important that they have a
 * couple loaded in front of their current one. So if the index is moving too
 * far into the array, new pathfinding will be done and the array and index will
 * be reset.
 */
void chr_gopos_advance_waypoint(struct chrdata *chr)
{
	if (chr->act_gopos.curindex < 3) {
		chr->act_gopos.curindex++;
	} else {
		struct waypoint *from = chr->act_gopos.waypoints[chr->act_gopos.curindex];
		chr->act_gopos.curindex = 1;

		nav_set_seed(CHRNAVSEED(chr), CHRNAVSEED(chr));
		nav_find_route(from, chr->act_gopos.target, chr->act_gopos.waypoints, MAX_CHRWAYPOINTS);
		nav_set_seed(0, 0);
	}

	chr_gopos_init_expensive(chr);
}

/**
 * Determines which step index the chr will be at given their current index, the
 * number of steps to take and in which direction (forward or back).
 *
 * Returns the step index and populates *forward with true or false depending on
 * whether the chr will be traversing the path in the forward direction at that
 * point.
 */
s32 chr_patrol_calculate_step(struct chrdata *chr, bool *forward, s32 numsteps)
{
	s32 nextstep = chr->act_patrol.nextstep;
	bool isforward = *forward;

	if (numsteps < 0) {
		isforward = !isforward;
		numsteps = -numsteps;
	}

	while (numsteps > 0) {
		numsteps--;

		if (isforward) {
			nextstep++;

			if (chr->act_patrol.path->pads[nextstep] < 0) {
				// Reached the end of the list
				if (chr->act_patrol.path->flags & PATHFLAG_CIRCULAR) {
					nextstep = 0;
				} else {
					isforward = false;
					nextstep -= 2;

					if (nextstep < 0) {
						nextstep = 0;
					}
				}
			}
		} else {
			nextstep--;

			if (nextstep < 0) {
				// Reached the start of the list
				if (chr->act_patrol.path->flags & PATHFLAG_CIRCULAR) {
					nextstep = chr->act_patrol.path->len - 1;
				} else {
					isforward = true;
					nextstep = 1;

					if (chr->act_patrol.path->len - 1 <= 0) {
						nextstep = chr->act_patrol.path->len - 1;
					}
				}
			}
		}
	}

	*forward = isforward;

	return nextstep;
}

/**
 * Determines which pad number the chr will be at given their current index and
 * the number of steps to take.
 */
s16 chr_patrol_calculate_pad_num(struct chrdata *chr, s32 numsteps)
{
	s32 *padnumptr;
	bool forward = chr->act_patrol.forward;
	s32 step = chr_patrol_calculate_step(chr, &forward, numsteps);
	padnumptr = &chr->act_patrol.path->pads[step];

	return *padnumptr;
}

void chr_patrol_get_cur_waypoint_info_with_flags(struct chrdata *chr, struct coord *pos, RoomNum *rooms, u32 *flags)
{
	s32 padnum = chr_patrol_calculate_pad_num(chr, 0);
	struct pad pad;

	pad_unpack(padnum, PADFIELD_POS | PADFIELD_ROOM | PADFIELD_FLAGS, &pad);

	pos->x = pad.pos.x;
	pos->y = pad.pos.y;
	pos->z = pad.pos.z;

	rooms[0] = pad.room;
	rooms[1] = -1;

	if (flags) {
		*flags = pad.flags;
	}
}

void chr_patrol_get_cur_waypoint_info(struct chrdata *chr, struct coord *pos, RoomNum *rooms)
{
	chr_patrol_get_cur_waypoint_info_with_flags(chr, pos, rooms, NULL);
}

void chr_patrol_init_expensive(struct chrdata *chr)
{
	RoomNum rooms[8];

	chr->act_patrol.waydata.mode = WAYMODE_INIT;
	chr->act_patrol.waydata.iter = 0;
	chr->act_patrol.waydata.gotaimpos = false;

	chr_patrol_get_cur_waypoint_info(chr, &chr->act_patrol.waydata.aimpos, rooms);
}

void chr_patrol_advance_waypoint(struct chrdata *chr)
{
	s32 nextstep = chr_patrol_calculate_step(chr, &chr->act_patrol.forward, 1);

	chr->act_patrol.nextstep = nextstep;
	chr->patrolnextstep = nextstep;

	chr_patrol_init_expensive(chr);
}

void chr_nav_tick_magic(struct chrdata *chr, struct waydata *waydata, f32 speed, struct coord *arg3, RoomNum *rooms)
{
	RoomNum sp118[8];
	f32 ymax;
	f32 ymin;
	f32 radius;
	f32 ground;
	u16 floorcol;
	u8 floortype;
	RoomNum floorroom;
	struct coord spf4;
	struct prop *prop = chr->prop;
	union modelrwdata *rwdata;
	struct waypoint *waypoint;
	struct coord spdc;
	RoomNum spcc[8];
	u32 stack[4];
	struct pad pad;
	struct coord sp5c;
	RoomNum sp4c[8];

	chr->invalidmove = 0;
	chr->lastmoveok60 = g_Vars.lvframe60;

	waydata->magicdone += speed * model_get_abs_anim_speed(chr->model) * g_Vars.lvupdate60freal;

	if (waydata->magicdone >= waydata->magictotal) {
		// Reached end of segment
		chr_set_perim_enabled(chr, false);
		rooms_copy(rooms, sp118);
		chr_find_entered_rooms_at_pos(chr, arg3, sp118);

		ground = cd_find_ground_info_at_cyl(arg3, chr->radius, sp118, &floorcol, &floortype, 0, &floorroom, NULL, NULL);

		spf4.x = arg3->x;
		spf4.y = prop->pos.y - chr->ground + ground;
		spf4.z = arg3->z;

		rooms_copy(rooms, sp118);
		chr_find_entered_rooms_at_pos(chr, &spf4, sp118);
		chr_get_bbox(chr->prop, &radius, &ymax, &ymin);

		if (cd_test_volume(&spf4, chr->radius, sp118, CDTYPE_ALL, CHECKVERTICAL_YES, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION) {
			// Reached end of segment with no collision
			prop->pos.x = spf4.x;
			prop->pos.y = spf4.y;
			prop->pos.z = spf4.z;

			chr->ground = ground;
			chr->manground = ground;
			chr->sumground = ground * (PAL ? 8.4175090789795f : 9.999998f);

			chr->floorcol = floorcol;
			chr->floortype = floortype;
			chr->floorroom = floorroom;

			prop_deregister_rooms(prop);
			rooms_copy(sp118, prop->rooms);
			prop_register_rooms(prop);

			model_set_root_position(chr->model, &prop->pos);

			rwdata = model_get_node_rw_data(chr->model, chr->model->definition->rootnode);
			rwdata->chrinfo.ground = ground;

			chr->chrflags |= CHRCFLAG_FORCETOGROUND;

			if (chr->actiontype == ACT_PATROL) {
				chr_patrol_advance_waypoint(chr);
				chr_patrol_get_cur_waypoint_info(chr, &spdc, spcc);
				chr_gopos_init_magic(chr, waydata, &spdc, &prop->pos);
			} else if (chr->actiontype == ACT_GOPOS) {
				if (chr->act_gopos.waypoints[chr->act_gopos.curindex] == NULL) {
					// Reached the end of the route
					if (chr->act_gopos.flags & GOPOSFLAG_FORPATHSTART) {
						chr_try_start_patrol(chr);
					} else {
						if (chr->act_gopos.curindex >= 2) {
							waypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex - 2];
							pad_unpack(waypoint->padnum, PADFIELD_POS, &pad);
							chr_set_theta(chr, atan2f(prop->pos.x - pad.pos.x, prop->pos.z - pad.pos.z));
						}

						if (CHRRACE(chr) == RACE_HUMAN || CHRRACE(chr) == RACE_SKEDAR) {
							chr_stop(chr);
						}
					}
				} else {
					// Advance to next segment, still using magic
					chr_gopos_advance_waypoint(chr);
					chr_gopos_get_cur_waypoint_info(chr, &sp5c, sp4c);
					chr_gopos_init_magic(chr, waydata, &sp5c, &prop->pos);
				}
			}
		} else {
			// Collision
			waydata->magicdone = waydata->magictotal;

			if (chr->actiontype == ACT_PATROL) {
				chr->act_patrol.waydata.lastvisible60 = g_Vars.lvframe60;
				chr_patrol_init_expensive(chr);
			} else {
				chr->act_gopos.waydata.lastvisible60 = g_Vars.lvframe60;
				chr_gopos_init_expensive(chr);
			}
		}

		chr_set_perim_enabled(chr, true);
	}
}

/**
 * Calculate the chr's position when using the magic method of navigating.
 *
 * The magic method is used when the chr is off-screen. It measures the distance
 * between two consecutive pads in the route and simply increments a distance
 * value along that segment on each tick, ignoring collisions. Meanwhile, the
 * chr's prop->pos is left as the original location where this segment started.
 *
 * The calculation is finding the fraction of the distance travelled in this
 * segment, then finding the position between the starting pos and the pad's
 * pos.
 *
 * The pos variable is used for both loading the next pad's position and for
 * returning the new position, which means there's less stack usage.
 */
void chr_calculate_position(struct chrdata *chr, struct coord *pos)
{
	RoomNum rooms[8];
	f32 frac;

	if (chr->actiontype == ACT_PATROL && chr->act_patrol.waydata.mode == WAYMODE_MAGIC) {
		chr_patrol_get_cur_waypoint_info(chr, pos, rooms);

		if (!(chr->act_patrol.waydata.magicdone >= chr->act_patrol.waydata.magictotal)
				&& chr->act_patrol.waydata.magictotal > 0) {
			frac = chr->act_patrol.waydata.magicdone / chr->act_patrol.waydata.magictotal;
			pos->x = (pos->x - chr->prop->pos.x) * frac + chr->prop->pos.x;
			pos->y = (pos->y - chr->prop->pos.y) * frac + chr->prop->pos.y;
			pos->z = (pos->z - chr->prop->pos.z) * frac + chr->prop->pos.z;
		}
	} else if (chr->actiontype == ACT_GOPOS && chr->act_gopos.waydata.mode == WAYMODE_MAGIC) {
		chr_gopos_get_cur_waypoint_info(chr, pos, rooms);

		if (!(chr->act_gopos.waydata.magicdone >= chr->act_gopos.waydata.magictotal)
				&& chr->act_gopos.waydata.magictotal > 0) {
			frac = chr->act_gopos.waydata.magicdone / chr->act_gopos.waydata.magictotal;
			pos->x = (pos->x - chr->prop->pos.x) * frac + chr->prop->pos.x;
			pos->y = (pos->y - chr->prop->pos.y) * frac + chr->prop->pos.y;
			pos->z = (pos->z - chr->prop->pos.z) * frac + chr->prop->pos.z;
		}
	} else {
		pos->x = chr->prop->pos.x;
		pos->y = chr->prop->pos.y;
		pos->z = chr->prop->pos.z;
	}
}

void chr_gopos_choose_animation(struct chrdata *chr)
{
	s32 gospeed = chr->act_gopos.flags & GOPOSMASK_SPEED;
	s32 male = g_HeadsAndBodies[chr->bodynum].ismale;
	struct prop *leftgun = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightgun = chr_get_held_prop(chr, HAND_RIGHT);
	s32 flip = false;
	s32 heavy;
	s32 race = CHRRACE(chr);
	s32 anim = -1;
	u32 stack;
	f32 speed = 0.5;
	f32 sp60 = 16;
	f32 animspeed = -1;
	f32 startframe = 16;

	if (chr->actiontype == ACT_GOPOS) {
		chr->act_gopos.flags &= ~GOPOSFLAG_WAITING;
	}

	if (race == RACE_EYESPY || chr->aibot) {
		return;
	}

	if (race == RACE_HUMAN || race == RACE_SKEDAR) {
		if ((leftgun && rightgun) || (!leftgun && !rightgun)) {
			heavy = false;
			flip = random() % 2;
		} else {
			if (weapon_is_onehanded(leftgun) || weapon_is_onehanded(rightgun)) {
				heavy = false;
				flip = (bool)leftgun != false;
			} else {
				heavy = true;
				flip = (bool)leftgun != false;
			}
		}

		if (race == RACE_SKEDAR) {
			if (gospeed == GOPOSFLAG_RUN) {
				anim = ANIM_SKEDAR_RUNNING;
			} else if (gospeed == GOPOSFLAG_JOG) {
				anim = ANIM_0393;
			} else if (gospeed == GOPOSFLAG_WALK) {
				anim = ANIM_0392;
			}
		} else {
			if (heavy) {
				if (gospeed == GOPOSFLAG_RUN) {
					// Human, heavy weapon, running
					if (chr->hitpart == HITPART_LFOOT
							|| chr->hitpart == HITPART_LSHIN
							|| chr->hitpart == HITPART_LTHIGH
							|| chr->hitpart == HITPART_RFOOT
							|| chr->hitpart == HITPART_RSHIN
							|| chr->hitpart == HITPART_RTHIGH) {
						anim = ANIM_020A;
						speed = 0.4;
					} else if (chr->hitpart == HITPART_LHAND
							|| chr->hitpart == HITPART_LFOREARM
							|| chr->hitpart == HITPART_LBICEP
							|| chr->hitpart == HITPART_RHAND
							|| chr->hitpart == HITPART_RFOREARM
							|| chr->hitpart == HITPART_RBICEP) {
						anim = ANIM_020D;
						speed = 0.4;
					} else {
						anim = ANIM_0029;
						speed = 0.25;
					}

					if (chr->chrflags & CHRCFLAG_RUNFASTER) {
						animspeed = 0.65;
						startframe = 48;
					} else {
						animspeed = 0.5;
						startframe = 48;
					}
				} else if (gospeed == GOPOSFLAG_JOG) {
					// Human, heavy weapon, jogging
					if (chr->hitpart == HITPART_LFOOT
							|| chr->hitpart == HITPART_LSHIN
							|| chr->hitpart == HITPART_LTHIGH
							|| chr->hitpart == HITPART_RFOOT
							|| chr->hitpart == HITPART_RSHIN
							|| chr->hitpart == HITPART_RTHIGH) {
						anim = ANIM_01F9;
					} else if (chr->hitpart == HITPART_LHAND
							|| chr->hitpart == HITPART_LFOREARM
							|| chr->hitpart == HITPART_LBICEP
							|| chr->hitpart == HITPART_RHAND
							|| chr->hitpart == HITPART_RFOREARM
							|| chr->hitpart == HITPART_RBICEP) {
						anim = ANIM_01F8;
					} else {
						anim = ANIM_RUNNING_TWOHANDGUN;
					}
				} else {
					// Human, heavy weapon, walking
					if (chr->hitpart == HITPART_LFOOT
							|| chr->hitpart == HITPART_LSHIN
							|| chr->hitpart == HITPART_LTHIGH
							|| chr->hitpart == HITPART_RFOOT
							|| chr->hitpart == HITPART_RSHIN
							|| chr->hitpart == HITPART_RTHIGH) {
						anim = ANIM_01F9;
					} else if (chr->hitpart == HITPART_LHAND
							|| chr->hitpart == HITPART_LFOREARM
							|| chr->hitpart == HITPART_LBICEP
							|| chr->hitpart == HITPART_RHAND
							|| chr->hitpart == HITPART_RFOREARM
							|| chr->hitpart == HITPART_RBICEP) {
						anim = ANIM_01F8;
					} else {
						if (random() % 2) {
							anim = ANIM_0018;
						} else {
							anim = ANIM_0028;
						}
					}
				}
			} else {
				if (gospeed == GOPOSFLAG_RUN) {
					// Human, light weapon, running
					if (chr->hitpart == HITPART_LFOOT
							|| chr->hitpart == HITPART_LSHIN
							|| chr->hitpart == HITPART_LTHIGH) {
						anim = ANIM_020A;
						flip = true;
						speed = 0.4;
					} else if (chr->hitpart == HITPART_RFOOT
							|| chr->hitpart == HITPART_RSHIN
							|| chr->hitpart == HITPART_RTHIGH) {
						anim = ANIM_020A;
						speed = 0.4;
						flip = false;
					} else if (chr->hitpart == HITPART_LHAND
							|| chr->hitpart == HITPART_LFOREARM
							|| chr->hitpart == HITPART_LBICEP) {
						anim = ANIM_020D;
						speed = 0.4;
						flip = true;
					} else if (chr->hitpart == HITPART_RHAND
							|| chr->hitpart == HITPART_RFOREARM
							|| chr->hitpart == HITPART_RBICEP) {
						anim = ANIM_020D;
						speed = 0.4;
						flip = false;
					} else if (male) {
						if (random() % 4 == 0) {
							speed = 0.25;
							anim = ANIM_001E;
						} else {
							speed = 0.25;
							anim = ANIM_005A;
							sp60 = 24;
						}
					} else {
						if (random() % 2) {
							anim = ANIM_005E;
						} else {
							anim = ANIM_005A;
						}

						speed = 0.25;
					}

					if (chr->chrflags & CHRCFLAG_RUNFASTER) {
						animspeed = 0.65;
						startframe = 48;
					} else {
						animspeed = 0.5;
						startframe = 48;
					}
				} else if (gospeed == GOPOSFLAG_JOG) {
					// Human, light weapon, jogging
					if (chr->hitpart == HITPART_LFOOT
							|| chr->hitpart == HITPART_LSHIN
							|| chr->hitpart == HITPART_LTHIGH) {
						anim = ANIM_01F9;
						flip = false;
					} else if (chr->hitpart == HITPART_RFOOT
							|| chr->hitpart == HITPART_RSHIN
							|| chr->hitpart == HITPART_RTHIGH) {
						anim = ANIM_01F9;
						flip = true;
					} else if (chr->hitpart == HITPART_LHAND
							|| chr->hitpart == HITPART_LFOREARM
							|| chr->hitpart == HITPART_LBICEP) {
						anim = ANIM_01F8;
						flip = false;
					} else if (chr->hitpart == HITPART_RHAND
							|| chr->hitpart == HITPART_RFOREARM
							|| chr->hitpart == HITPART_RBICEP) {
						anim = ANIM_01F8;
						flip = true;
					} else if (stage_get_index(g_Vars.stagenum) == STAGEINDEX_CHICAGO) {
						anim = ANIM_005F;
					} else if (male) {
						if (random() % 2) {
							anim = ANIM_001D;
						} else {
							anim = ANIM_RUNNING_ONEHANDGUN;
						}
					} else {
						if (chr->myaction != MA_PANIC) {
							if (random() % 2) {
								anim = ANIM_005D;
							} else {
								anim = ANIM_0073;
							}
						} else {
							anim = ANIM_021D;
						}
					}
				} else {
					// Human, light weapon, walking
					s32 anims[] = {ANIM_006B, ANIM_001B, ANIM_0016};

					if (chr->hitpart == HITPART_LFOOT
							|| chr->hitpart == HITPART_LSHIN
							|| chr->hitpart == HITPART_LTHIGH) {
						anim = ANIM_01F9;
						flip = false;
					} else if (chr->hitpart == HITPART_RFOOT
							|| chr->hitpart == HITPART_RSHIN
							|| chr->hitpart == HITPART_RTHIGH) {
						anim = ANIM_01F9;
						flip = true;
					} else if (chr->hitpart == HITPART_LHAND
							|| chr->hitpart == HITPART_LFOREARM
							|| chr->hitpart == HITPART_LBICEP) {
						anim = ANIM_01F8;
						flip = false;
					} else if (chr->hitpart == HITPART_RHAND
							|| chr->hitpart == HITPART_RFOREARM
							|| chr->hitpart == HITPART_RBICEP) {
						anim = ANIM_01F8;
						flip = true;
					} else if (male) {
						anim = anims[random() % 3];
					} else {
						if (random() % 2) {
							anim = ANIM_005C;
						} else {
							anim = ANIM_0072;
						}
					}
				}
			}
		}
	} else if (race == RACE_DRCAROLL) {
		if (gospeed == GOPOSFLAG_RUN) {
			anim = ANIM_0160;
		} else if (gospeed == GOPOSFLAG_WALK) {
			anim = ANIM_015F;
		} else {
			anim = ANIM_015F;
		}
	} else if (race == RACE_ROBOT) {
		anim = ANIM_0238;
	}

	if (anim >= 0) {
		model_set_animation(chr->model, anim, flip, 0, speed, sp60);

		if (animspeed > 0) {
			model_set_anim_speed(chr->model, animspeed, startframe);
		}
	}
}

bool chr_go_to_room_pos(struct chrdata *chr, struct coord *pos, RoomNum *room, u32 goposflags)
{
	struct prop *prop = chr->prop;
	struct waypoint *nextwaypoint;
	struct waypoint *lastwaypoint;
	struct waypoint *waypoints[MAX_CHRWAYPOINTS];
	s32 i;
	struct coord curwppos;
	RoomNum curwprooms[8];
	s32 isgopos = chr->actiontype == ACT_GOPOS
		&& (chr->act_gopos.flags & GOPOSMASK_SPEED) == (goposflags & 0xff & GOPOSMASK_SPEED)
		&& !chr_gopos_is_waiting(chr);
	s32 ismagic = isgopos && chr->act_gopos.waydata.mode == WAYMODE_MAGIC;
	struct coord prevpos;
	s32 numwaypoints = 0;

	for (i = 0; chr->prop->rooms[i] != -1; i++) {
		chr->oldrooms[i] = chr->prop->rooms[i];
	}

	chr->oldrooms[i] = -1;

	if (isgopos && ismagic && chr->act_gopos.waypoints[chr->act_gopos.curindex]) {
		nextwaypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex];
	} else {
		nextwaypoint = waypoint_find_closest_to_pos(&prop->pos, prop->rooms);
	}

	lastwaypoint = waypoint_find_closest_to_pos(pos, room);

	if (nextwaypoint && lastwaypoint) {
		nav_set_seed(CHRNAVSEED(chr), CHRNAVSEED(chr));
		numwaypoints = nav_find_route(nextwaypoint, lastwaypoint, waypoints, MAX_CHRWAYPOINTS);
		nav_set_seed(0, 0);
	}

	if (numwaypoints > 1) {
		if (isgopos && ismagic) {
			chr_calculate_position(chr, &prevpos);
		} else {
			prevpos.x = prop->pos.x;
			prevpos.y = prop->pos.y;
			prevpos.z = prop->pos.z;
		}

		chr_stop_firing(chr);

		chr->actiontype = ACT_GOPOS;
		chr->act_gopos.endpos.x = pos->x;
		chr->act_gopos.endpos.y = pos->y;
		chr->act_gopos.endpos.z = pos->z;
		rooms_copy(room, chr->act_gopos.endrooms);

		chr->act_gopos.target = lastwaypoint;
		chr->act_gopos.curindex = 0;
		chr->act_gopos.flags = goposflags | GOPOSFLAG_INIT;
		chr->act_gopos.turnspeed = 0;
		chr->unk32c_21 = 0;
		chr->act_gopos.waydata.age = random() % 100;
		chr->act_gopos.waydata.gotaimposobj = 0;

		if (!isgopos) {
			chr->act_gopos.waydata.lastvisible60 = -1;
		}

		for (i = 0; i < MAX_CHRWAYPOINTS; i++) {
			chr->act_gopos.waypoints[i] = waypoints[i];
		}

		chr_gopos_init_expensive(chr);
#if VERSION >= VERSION_NTSC_1_0
		chr->goposforce = -1;
#endif
		chr->sleep = 0;
		chr->liftaction = 0;
		chr->act_gopos.flags &= ~(GOPOSFLAG_DUCK | GOPOSFLAG_CROUCH | GOPOSFLAG_WAITING);
		chr_gopos_get_cur_waypoint_info(chr, &curwppos, curwprooms);

		if ((!isgopos || ismagic)
				&& g_Vars.normmplayerisrunning == false
				&& (prop->flags & (PROPFLAG_ONANYSCREENPREVTICK | PROPFLAG_ONANYSCREENTHISTICK | PROPFLAG_ONTHISSCREENTHISTICK)) == 0
				&& chr_is_room_offscreen(chr, &curwppos, curwprooms)
				&& chr->inlift == false) {
			chr_gopos_init_magic(chr, &chr->act_gopos.waydata, &curwppos, &prevpos);
		}

		if (chr->act_gopos.waydata.mode != WAYMODE_MAGIC
				&& model_is_anim_merging(chr->model) && !chr->aibot) {
			chr->hidden |= CHRHFLAG_NEEDANIM;
			return true;
		} else {
			if (!isgopos) {
				chr_gopos_choose_animation(chr);
			}

			chr->hidden &= ~CHRHFLAG_NEEDANIM;
			return true;
		}
	}

	return false;
}

struct path *path_find_by_id(u32 path_id)
{
	s32 i = 0;

	for (i = 0; g_StageSetup.paths[i].pads; i++) {
		if (path_id == g_StageSetup.paths[i].id) {
			return &g_StageSetup.paths[i];
		}
	}

	return NULL;
}

void chr_patrol_choose_animation(struct chrdata *chr)
{
	struct prop *leftprop = chr_get_held_prop(chr, HAND_LEFT);
	struct prop *rightprop = chr_get_held_prop(chr, HAND_RIGHT);
	s32 flip;
	bool heavy;
	s32 race = CHRRACE(chr);
	s32 ismale = g_HeadsAndBodies[chr->bodynum].ismale;
	f32 speed;

	if (race == RACE_EYESPY) {
		// empty
	} else if (race == RACE_HUMAN || race == RACE_SKEDAR) {
		if ((leftprop && rightprop) || (!leftprop && !rightprop)) {
			// No weapon, or double weapons
			heavy = false;
			flip = random() % 2;
		} else {
			// Single weapon
			if (weapon_is_onehanded(leftprop) || weapon_is_onehanded(rightprop)) {
				heavy = false;
				flip = ((bool)leftprop != false);
			} else {
				heavy = true;
				flip = ((bool)leftprop != false);
			}
		}

		if (race == RACE_SKEDAR) {
			model_set_animation(chr->model, ANIM_0392, flip, 0, 0.25f, 16);
		} else {
			speed = 0.5f * chr_get_anim_movedist_per_frame(ANIM_0028) / chr_get_anim_movedist_per_frame(ANIM_006B);

			if (heavy) {
				model_set_animation(chr->model, random() % 2 ? ANIM_0018 : ANIM_0028, flip, 0, speed, 16);
			} else if (ismale) {
				s32 anims[] = { ANIM_006B, ANIM_001B, ANIM_0016 };
				model_set_animation(chr->model, anims[random() % 3], flip, 0, speed, 16);
			} else {
				model_set_animation(chr->model, random() % 2 ? ANIM_005C : ANIM_0072, flip, 0, speed, 16);
			}
		}
	} else if (race == RACE_DRCAROLL) {
		model_set_animation(chr->model, ANIM_015F, false, 0, 0.5f, 16);
	} else if (race == RACE_ROBOT) {
		model_set_animation(chr->model, ANIM_0238, false, 0, 0.5f, 16);
	}
}

void chr_start_patrol(struct chrdata *chr, struct path *path)
{
	s32 i;
	f32 dist;
	f32 xdiff;
	f32 zdiff;
	s32 *padnumptr;
	struct pad pad;
	struct coord nextpos;
	RoomNum nextrooms[8];
	RoomNum rooms[8];
	f32 ymax;
	f32 ymin;
	f32 radius;
	f32 bestdistance = 0;
	s32 nextstep = -1;
	struct prop *prop = chr->prop;
	RoomNum sp60[2];

	if (CHRRACE(chr) != RACE_EYESPY) {
		// Do some kind of collision test with the pad to resume from...
		// maybe a line of sight check?
		if (chr->patrolnextstep >= 0 && chr->patrolnextstep < path->len) {
			padnumptr = &path->pads[chr->patrolnextstep];
			pad_unpack(*padnumptr, PADFIELD_POS | PADFIELD_ROOM, &pad);

			rooms[0] = pad.room;
			rooms[1] = -1;

			chr_get_bbox(prop, &radius, &ymax, &ymin);

			chr_set_perim_enabled(chr, false);

			if (cd_test_cyl_move04(&prop->pos, prop->rooms, &pad.pos, rooms, CDTYPE_BG, 1,
						ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION) {
				nextstep = chr->patrolnextstep;
			}

			chr_set_perim_enabled(chr, true);
		}

		// If the pad to resume from is not in sight, find the closest pad
		// to the chr's current position and start from there.
		if (nextstep < 0) {
			for (i = 0; path->pads[i] >= 0; i++) {
				padnumptr = &path->pads[i];
				pad_unpack(*padnumptr, PADFIELD_POS, &pad);

				xdiff = pad.pos.x - prop->pos.x;
				zdiff = pad.pos.z - prop->pos.z;
				dist = xdiff * xdiff + zdiff * zdiff;

				if (nextstep < 0 || dist < bestdistance) {
					bestdistance = dist;
					nextstep = i;
				}
			}
		}

		padnumptr = &path->pads[nextstep];
		pad_unpack(*padnumptr, PADFIELD_POS | PADFIELD_ROOM, &pad);

		rooms[0] = pad.room;
		rooms[1] = -1;

		// If chr has line of sight to the pad then begin the patrol,
		// otherwise use gopos to get to the starting pad
		if (chr_prop_can_move_to_pos_without_nav(chr, &prop->pos, prop->rooms, &pad.pos, rooms, NULL,
					chr->radius * 1.2f, CDTYPE_PATHBLOCKER | CDTYPE_BG) != CDRESULT_COLLISION) {
			chr_stop_firing(chr);

			chr->actiontype = ACT_PATROL;
			chr->act_patrol.path = path;
			chr->act_patrol.nextstep = nextstep;
			chr->act_patrol.forward = true;

			chr->act_patrol.waydata.age = random() % 100;
			chr->act_patrol.waydata.gotaimposobj = 0;
			chr->act_patrol.waydata.lastvisible60 = -1;

			chr->act_patrol.turnspeed = 0;

			chr_patrol_init_expensive(chr);

			chr->sleep = 0;
			chr->liftaction = LIFTACTION_NOTUSINGLIFT;
			chr->patrolnextstep = chr->act_patrol.nextstep;

			chr_patrol_get_cur_waypoint_info(chr, &nextpos, nextrooms);

			if (!g_Vars.normmplayerisrunning
					&& (chr->prop->flags & (PROPFLAG_ONTHISSCREENTHISTICK | PROPFLAG_ONANYSCREENTHISTICK | PROPFLAG_ONANYSCREENPREVTICK)) == 0
					&& chr_is_room_offscreen(chr, &nextpos, nextrooms)
					&& !chr->inlift) {
				chr_gopos_init_magic(chr, &chr->act_patrol.waydata, &nextpos, &prop->pos);
			}

			// @bug: This should be act_patrol rather than act_gopos.
			// It's actually reading act_patrol.waydata.aimposobj.y which is a
			// float, so it's taking its binary representation and comparing it
			// with integer 6.
			if (chr->act_gopos.waydata.mode != WAYMODE_MAGIC && model_is_anim_merging(chr->model)) {
				chr->hidden |= CHRHFLAG_NEEDANIM;
			} else {
				chr_patrol_choose_animation(chr);
				chr->hidden &= ~CHRHFLAG_NEEDANIM;
			}
		} else {
			sp60[0] = pad.room;
			sp60[1] = -1;

			chr_go_to_room_pos(chr, &pad.pos, sp60, GOPOSFLAG_FORPATHSTART);
		}
	}
}

void chr_record_last_visible_target_time(struct chrdata *chr)
{
	chr->lastvisibletarget60 = g_Vars.lvframe60;
}

bool chr_has_los_to_entity(struct chrdata *chr, struct coord *chrpos, RoomNum *chrrooms, bool allowextraheight, u32 attackflags, u32 entityid)
{
	bool result = false;
	struct coord targetpos;
	RoomNum targetrooms[8];
	struct prop *targetprop;
	struct chrdata *targetchr;
	u32 types;
	struct prop *weaponprop;

	if (attackflags & ATTACKFLAG_AIMFORWARD) {
		result = true;
	} else {
		types = CDTYPE_DOORSWITHOUTFLAG | CDTYPE_ALL;
		weaponprop = chr_get_held_prop(chr, HAND_RIGHT);

		if (weaponprop == NULL) {
			weaponprop = chr_get_held_prop(chr, HAND_LEFT);
		}

		if (weaponprop) {
			struct weaponobj *weapon = weaponprop->weapon;

			if (weapon->weaponnum == WEAPON_ROCKETLAUNCHER
					|| weapon->weaponnum == WEAPON_SLAYER
					|| weapon->weaponnum == WEAPON_KINGSCEPTRE) {
				types = CDTYPE_DOORSWITHOUTFLAG | CDTYPE_OBJSIMMUNETOEXPLOSIONS | CDTYPE_ALL;
			} else {
				types = CDTYPE_DOORSWITHOUTFLAG | CDTYPE_OBJSIMMUNETOGUNFIRE | CDTYPE_ALL;
			}
		}

		chr_get_attack_entity_pos(chr, attackflags, entityid, &targetpos, targetrooms);
		chr_set_perim_enabled(chr, false);

		if ((attackflags & ATTACKFLAG_AIMATTARGET)) {
			targetprop = chr_get_target_prop(chr);

			if (targetprop->type != PROPTYPE_PLAYER || g_Vars.bondvisible) {
				prop_set_perim_enabled(targetprop, false);

				if (allowextraheight && (chr->chrflags & CHRCFLAG_LOSEXTRAHEIGHT)) {
					struct coord frompos;
					RoomNum fromrooms[8];

					frompos.x = chrpos->x;
					frompos.y = chrpos->y + 70;
					frompos.z = chrpos->z;

					los_find_final_room_properly(chrpos, chrrooms, &frompos, fromrooms);

					if (cd_test_los05(&frompos, fromrooms, &targetpos, targetrooms, types, GEOFLAG_BLOCK_SHOOT)) {
						chr_record_last_visible_target_time(chr);
						result = true;
					}
				} else {
					if (cd_test_los05(chrpos, chrrooms, &targetpos, targetrooms, types, GEOFLAG_BLOCK_SHOOT)) {
						chr_record_last_visible_target_time(chr);
						result = true;
					}
				}

				prop_set_perim_enabled(targetprop, true);
			}
		} else if (attackflags & ATTACKFLAG_AIMATCHR) {
			targetchr = chr_find_by_id(chr, entityid);

			if (!targetchr || !targetchr->prop) {
				targetchr = chr;
			}

			chr_set_perim_enabled(targetchr, false);

			if (cd_test_los05(chrpos, chrrooms, &targetpos, targetrooms, types, GEOFLAG_BLOCK_SHOOT)) {
				result = true;
			}

			chr_set_perim_enabled(targetchr, true);
		} else if (attackflags & ATTACKFLAG_AIMATPAD) {
			if (cd_test_los05(chrpos, chrrooms, &targetpos, targetrooms, types, GEOFLAG_BLOCK_SHOOT)) {
				result = true;
			}
		}

		chr_set_perim_enabled(chr, true);
	}

	return result;
}

bool chr_has_los_to_attack_target(struct chrdata *chr, struct coord *pos, RoomNum *rooms, bool allowextraheight)
{
	u32 attackflags = ATTACKFLAG_AIMATTARGET;
	u32 entityid = 0;

	if (chr->actiontype == ACT_ATTACK) {
		attackflags = chr->act_attack.flags;
		entityid = chr->act_attack.entityid;
	}

	return chr_has_los_to_entity(chr, pos, rooms, allowextraheight, attackflags, entityid);
}

bool chr_has_los_to_chr(struct chrdata *chr, struct chrdata *target, RoomNum *room)
{
	bool cansee = false;
	u32 stack;
	RoomNum sp88[] = {-1, 0, 0, 0, 0, 0, 0, 0};

	if (!bot_is_target_invisible(chr, target)) {
		struct prop *prop = chr->prop;
		struct coord pos;
		RoomNum rooms[8];

		pos.x = prop->pos.x;
		pos.y = chr->ground + chr->height - 20;
		pos.z = prop->pos.z;

		chr_set_perim_enabled(chr, false);
		chr_set_perim_enabled(target, false);

		los_find_final_room_exhaustive(&prop->pos, prop->rooms, &pos, rooms);

		if (cd_test_los07(&pos, rooms, &target->prop->pos, target->prop->rooms, sp88,
					CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PATHBLOCKER | CDTYPE_BG | CDTYPE_AIOPAQUE,
					GEOFLAG_BLOCK_SIGHT)) {
			cansee = true;
		}

		chr_set_perim_enabled(chr, true);
		chr_set_perim_enabled(target, true);
	}

	if (room) {
		*room = sp88[0];
	}

	return cansee;
}

bool chr_has_los_to_target(struct chrdata *chr)
{
	bool cansee;
	struct prop *prop;

#if VERSION >= VERSION_JPN_FINAL
	cansee = false;
	prop = chr_get_target_prop(chr);

	if (prop && prop->chr) {
		cansee = chr_has_los_to_chr(chr, prop->chr, NULL);

		if (cansee) {
			chr_record_last_visible_target_time(chr);
		}
	}
#else
	prop = chr_get_target_prop(chr);
	cansee = chr_has_los_to_chr(chr, prop->chr, NULL);

	if (cansee) {
		chr_record_last_visible_target_time(chr);
	}
#endif

	return cansee;
}

bool chr_has_los_to_pos(struct chrdata *chr, struct coord *pos, RoomNum *rooms)
{
	struct prop *prop = chr->prop;
	bool result = false;
	struct coord eyepos;
	RoomNum chrrooms[8];

	eyepos.x = prop->pos.x;
	eyepos.y = chr->ground + chr->height - 20;
	eyepos.z = prop->pos.z;

	chr_set_perim_enabled(chr, false);
	los_find_final_room_exhaustive(&prop->pos, prop->rooms, &eyepos, chrrooms);

	if (cd_test_los05(&eyepos, chrrooms, pos, rooms,
				CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PATHBLOCKER | CDTYPE_BG | CDTYPE_AIOPAQUE,
				GEOFLAG_BLOCK_SIGHT)) {
		result = true;
	}

	chr_set_perim_enabled(chr, true);

	return result;
}

/**
 * Wasteful because this function calculates angles then does nothing with them.
 * chr_has_los_to_pos is called with the same arguments regardless.
 */
bool chr_has_los_to_pos_wasteful(struct chrdata *chr, struct coord *pos, RoomNum *rooms)
{
	f32 facingangle = chr_get_theta(chr);
	f32 posangle = atan2f(pos->x - chr->prop->pos.x, pos->z - chr->prop->pos.z);
	f32 diffangle = posangle - facingangle;

	if (posangle < facingangle) {
		diffangle += BADDTOR(360);
	}

	if ((diffangle < BADDTOR(100) || diffangle > BADDTOR(260))
			&& chr_has_flag(chr, CHRFLAG1_NOOP_00200000, BANK_1) == false) {
		return chr_has_los_to_pos(chr, pos, rooms);
	}

	return chr_has_los_to_pos(chr, pos, rooms);
}

bool chr_has_los_to_prop(struct chrdata *chr, struct prop *prop)
{
	bool result;

	prop_set_perim_enabled(prop, false);
	result = chr_has_los_to_pos_wasteful(chr, &prop->pos, prop->rooms);
	prop_set_perim_enabled(prop, true);

	return result;
}

void chr_record_last_see_target_time(struct chrdata *chr)
{
	chr->lastseetarget60 = g_Vars.lvframe60;
}

void chr_record_last_hear_target_time(struct chrdata *chr)
{
	chr->hidden |= CHRHFLAG_IS_HEARING_TARGET;
	chr->lastheartarget60 = g_Vars.lvframe60;
}

bool chr_is_stopped(struct chrdata *chr)
{
	s16 anim = model_get_anim_num(chr->model);

	if (anim == ANIM_SNIPING_GETDOWN || anim == ANIM_SNIPING_GETUP) {
		return false;
	}

	if (anim == ANIM_SNIPING_ONGROUND && chr->act_attack.numshots >= chr->act_attack.maxshots) {
		chr_stop_firing(chr);
		return true;
	}

	if (chr->actiontype == ACT_ROBOTATTACK && chr->act_robotattack.finished) {
		return true;
	}

	if (chr->actiontype == ACT_ATTACKAMOUNT && chr->act_attack.numshots >= chr->act_attack.maxshots) {
		return true;
	}

	if (chr->actiontype == ACT_STAND
			&& !chr->act_stand.prestand
			&& !chr->act_stand.reaim
			&& chr->act_stand.turning != 1) {
		return true;
	}

	if (chr->actiontype == ACT_ANIM) {
		if (chr->act_anim.completed
				|| (model_get_anim_speed(chr->model) >= 0 && model_get_cur_anim_frame(chr->model) >= model_get_anim_end_frame(chr->model))
				|| (model_get_anim_speed(chr->model) < 0 && model_get_cur_anim_frame(chr->model) <= 0)) {
			return true;
		}
	} else if (chr->actiontype == ACT_PATROL) {
		return true;
	}

	return false;
}

bool chr_check_can_see_target(struct chrdata *chr)
{
	struct prop *prop = chr->prop;
	struct prop *target = chr_get_target_prop(chr);
	f32 sqdistance;
	f32 invtheta = chr_get_theta(chr);

	f32 x = target->pos.x - prop->pos.x;
	f32 y = target->pos.y - prop->pos.y;
	f32 z = target->pos.z - prop->pos.z;

	f32 angletotarget = atan2f(x, z);
	f32 angle = angletotarget - invtheta;
	bool result = false;

	if (angletotarget < invtheta) {
		angle += BADDTOR(360);
	}

	sqdistance = x * x + y * y + z * z;

	if ((sqdistance < chr->visionrange * chr->visionrange * 10000.0f && (angle < BADDTOR(110) || angle > BADDTOR(250)))
			|| (sqdistance < 40000.0f && (angle < BADDTOR(110) || angle > BADDTOR(250)))) {
		result = false;

		if (sqdistance < env_get_squared_fog_max()) {
			f32 tmp;
			s32 iVar8 = sqrtf(sqdistance) * 0.001875f;
			s32 tmp2;

			if (angle > BADDTOR(45) && angle < BADDTOR(315)) {
				tmp = angle;

				if (tmp > DTOR(180)) {
					tmp = BADDTOR(360) - angle;
				}

				tmp -= BADDTOR(45);
				tmp2 = tmp * DTOR(218.8886f);
				iVar8 *= 1 + tmp2;
			}

			iVar8 = chr_get_percentage_of_slowness(chr, iVar8) + 1;
			result = random() % iVar8 == 0;
		}
	}

	if (result) {
		result = chr_has_los_to_target(chr);
	}

	if (result) {
		chr_record_last_see_target_time(chr);
	}

	return result;
}

bool chr_is_ready_for_orders(struct chrdata *chr)
{
	if (chr->onladder) {
		return false;
	}

	switch ((s32) chr->actiontype) {
#if VERSION >= VERSION_NTSC_1_0
	case ACT_DIE:
	case ACT_DEAD:
	case ACT_PREARGH:
	case ACT_DRUGGEDDROP:
	case ACT_DRUGGEDKO:
	case ACT_DRUGGEDCOMINGUP:
		return false;
	case ACT_ARGH:
		if ((chr->chrflags & CHRCFLAG_CANCHANGEACTDURINGARGH) == 0) {
			return false;
		}
		break;
#else
	case ACT_DIE:
	case ACT_DEAD:
	case ACT_PREARGH:
	case ACT_DRUGGEDDROP:
	case ACT_DRUGGEDKO:
	case ACT_DRUGGEDCOMINGUP:
	case ACT_ARGH:
	case 0x200:
		return false;
#endif
	case ACT_ROBOTATTACK:
		if (!chr->act_robotattack.finished) {
			return false;
		}
		break;
	}

	return true;
}

bool chr_is_dead(struct chrdata *chr)
{
	if (!chr || chr->actiontype == ACT_DIE || chr->actiontype == ACT_DEAD) {
		return true;
	}

	if (chr->prop && chr->prop->type == PROPTYPE_PLAYER) {
		u32 playernum = playermgr_get_player_num_by_prop(chr->prop);

		if (g_Vars.players[playernum]->isdead) {
			return true;
		}
	}

	return false;
}

bool chr_try_sidestep(struct chrdata *chr)
{
	u8 race = CHRRACE(chr);

	if ((race == RACE_HUMAN || race == RACE_SKEDAR)
			&& chr_is_ready_for_orders(chr)) {
		struct prop *prop = chr->prop;
		struct prop *target = chr_get_target_prop(chr);
		f32 a = chr_get_theta(chr);
		f32 b = atan2f(target->pos.x - prop->pos.x, target->pos.z - prop->pos.z);
		f32 angle = b - a;
		u32 stack[2];

		if (angle < 0) {
			angle += BADDTOR(360);
		}

		if (angle < BADDTOR(45) || angle > BADDTOR(315)
				|| (angle > BADDTOR(135) && angle < BADDTOR2(225))) {
			bool side = (random() % 2) == 0;

			if (chr_can_jump_in_direction(chr, side, 100)) {
				chr_sidestep(chr, side);
				return true;
			}

			if (chr_can_jump_in_direction(chr, !side, 100)) {
				chr_sidestep(chr, !side);
				return true;
			}
		}
	}

	return false;
}

bool chr_try_jumpout(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_HUMAN && chr_is_ready_for_orders(chr)) {
		struct prop *prop = chr->prop;
		struct prop *target = chr_get_target_prop(chr);

		f32 a = chr_get_theta(chr);
		f32 b = atan2f(target->pos.x - prop->pos.x, target->pos.z - prop->pos.z);
		f32 angle = b - a;
		u32 stack[2];

		if (angle < 0) {
			angle += BADDTOR(360);
		}

		if (angle < BADDTOR(45) || angle > BADDTOR(315)
				|| (angle > BADDTOR(135) && angle < BADDTOR2(225))) {
			bool side = (random() % 2) == 0;

			if (chr_can_jump_in_direction(chr, side, 200)) {
				chr_jumpout(chr, side);
				return true;
			}

			if (chr_can_jump_in_direction(chr, !side, 200)) {
				chr_jumpout(chr, !side);
				return true;
			}
		}
	}

	return false;
}

bool chr_try_runsideways(struct chrdata *chr)
{
	u32 race = CHRRACE(chr);

	if ((race == RACE_HUMAN || race == RACE_SKEDAR)
			&& chr_is_ready_for_orders(chr)
			&& g_Vars.lvframe60 - chr->lastwalk60 > TICKS(180)) {
		struct prop *prop = chr->prop;
		f32 distance = 200.0f + RANDOMFRAC() * 200.0f;
		struct coord dir;
		struct coord dstpos;

		chr_get_side_vector_to_target(chr, random() % 2 == 0, &dir);

		dstpos.x = dir.x * distance + prop->pos.x;
		dstpos.y = prop->pos.y;
		dstpos.z = dir.z * distance + prop->pos.z;

		if (chr_prop_can_move_to_pos_without_nav_fast(prop, &dstpos, &dir)) {
			chr_run_to_pos(chr, &dstpos);
			return true;
		}

		dir.x = -dir.x;
		dir.z = -dir.z;

		dstpos.x = dir.x * distance + prop->pos.x;
		dstpos.y = prop->pos.y;
		dstpos.z = dir.z * distance + prop->pos.z;

		if (chr_prop_can_move_to_pos_without_nav_fast(prop, &dstpos, &dir)) {
			chr_run_to_pos(chr, &dstpos);
			return true;
		}
	}

	return false;
}

bool chr_try_attackwalk(struct chrdata *chr)
{
	u32 race = CHRRACE(chr);

	if (race == RACE_DRCAROLL || race == RACE_EYESPY) {
		return false;
	}

	if (chr_is_ready_for_orders(chr)) {
		struct prop *prop = chr->prop;

		if (chr_has_los_to_attack_target(chr, &prop->pos, prop->rooms, false)
				&& (chr_get_held_usable_prop(chr, 0) || chr_get_held_usable_prop(chr, 1))
				&& g_Vars.lvframe60 - chr->lastwalk60 > TICKS(120)) {
			struct prop *target = chr_get_target_prop(chr);
			f32 x = target->pos.x - prop->pos.x;
			f32 y = target->pos.y - prop->pos.y;
			f32 z = target->pos.z - prop->pos.z;

			if (race == RACE_HUMAN && x * x + y * y + z * z >= 1000 * 1000) {
				chr_attackwalk(chr, false);
				return true;
			}
		}
	}

	return false;
}

bool chr_try_attackrun(struct chrdata *chr)
{
	u32 race = CHRRACE(chr);

	if (race == RACE_DRCAROLL || race == RACE_EYESPY) {
		return false;
	}

	if (chr_is_ready_for_orders(chr)) {
		struct prop *prop = chr->prop;

		if (chr_has_los_to_attack_target(chr, &prop->pos, prop->rooms, false)
				&& (chr_get_held_usable_prop(chr, 0) || chr_get_held_usable_prop(chr, 1))
				&& g_Vars.lvframe60 - chr->lastwalk60 > TICKS(180)) {
			struct prop *target = chr_get_target_prop(chr);
			f32 x = target->pos.x - prop->pos.x;
			f32 y = target->pos.y - prop->pos.y;
			f32 z = target->pos.z - prop->pos.z;

			if (race == RACE_HUMAN && x * x + y * y + z * z >= 1000 * 1000) {
				chr_attackwalk(chr, true);
				return true;
			}
		}
	}

	return false;
}

bool chr_try_attackroll(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_HUMAN && chr_is_ready_for_orders(chr)) {
		struct prop *prop = chr->prop;

		if (chr_has_los_to_attack_target(chr, &prop->pos, prop->rooms, false) &&
				(chr_get_held_usable_prop(chr, 0) || chr_get_held_usable_prop(chr, 1))) {
			struct prop *target = chr_get_target_prop(chr);
			f32 x = target->pos.x - prop->pos.x;
			f32 y = target->pos.y - prop->pos.y;
			f32 z = target->pos.z - prop->pos.z;
			f32 sqdistance = x * x + y * y + z * z;

			if (sqdistance >= 200 * 200) {
				bool toleft = (random() % 2) == 0;

				if (chr_can_roll_in_direction(chr, toleft, 200)) {
					chr_attackroll(chr, toleft);
					return true;
				}

				if (chr_can_roll_in_direction(chr, !toleft, 200)) {
					chr_attackroll(chr, !toleft);
					return true;
				}
			}
		}
	}

	return false;
}

bool chr_try_attackamount(struct chrdata *chr, u32 arg1, u32 arg2, u8 lower, u8 upper)
{
	u8 race = CHRRACE(chr);

	if ((race == RACE_HUMAN || race == RACE_SKEDAR)
			&& chr_is_ready_for_orders(chr)
			&& chr->weapons_held[0]) {
		s32 quantity;
		f32 percentage;
		struct weaponobj *weapon = chr->weapons_held[0]->weapon;
		struct ammodef *ammo;

		quantity = 1;

		if (upper > 100) {
			upper = 100;
		}

		if (lower < upper) {
			percentage = ((random() % (upper - lower)) + (u32)lower) * 0.01f;
		} else {
			percentage = 0;
		}

		ammo = gset_get_ammodef(weapon->weaponnum, 0);

		if (ammo) {
			quantity = ammo->clipsize * percentage;
		}

		if (quantity < 1) {
			quantity = 1;
		}

		chr->aimendrshoulder = 0;
		chr->aimendlshoulder = 0;
		chr->aimendback = 0;
		chr->aimendsideback = 0;
		chr->aimendcount = 10;

		chr_attackamount(chr, arg1, arg2, quantity);

		return true;
	}

	return false;
}

bool chr_try_attackstand(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	s32 race = CHRRACE(chr);

	if (race == RACE_DRCAROLL || race == RACE_EYESPY) {
		return false;
	}

	if (chr_is_ready_for_orders(chr)) {
		if (race == RACE_ROBOT) {
			robot_attack(chr);
			return true;
		}

		if (race == RACE_HUMAN || race == RACE_SKEDAR) {
			if (chr_get_held_usable_prop(chr, 0) ||
					(chr_get_held_usable_prop(chr, 1))) {
				chr_attackstand(chr, attackflags, entityid);
				return true;
			}
		}
	}

	return false;
}

bool chr_try_attackkneel(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	s32 race = CHRRACE(chr);

	if (race == RACE_HUMAN || race == RACE_SKEDAR) {
		if (chr_is_ready_for_orders(chr) && (chr_get_held_usable_prop(chr, 0) || chr_get_held_usable_prop(chr, 1))) {
			chr_attackkneel(chr, attackflags, entityid);
			return true;
		}
	}

	return false;
}

bool chr_try_attacklie(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	s32 race = CHRRACE(chr);

	if (race == RACE_HUMAN || race == RACE_SKEDAR) {
		if (chr_is_ready_for_orders(chr) && (chr_get_held_usable_prop(chr, 0) || chr_get_held_usable_prop(chr, 1))) {
			chr_attacklie(chr, attackflags, entityid);
			return true;
		}
	}

	return false;
}

bool chr_try_modifyattack(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	s32 race = CHRRACE(chr);

	if (race == RACE_DRCAROLL || race == RACE_EYESPY) {
		return false;
	}

	if (chr->actiontype == ACT_ATTACK
			&& (chr->act_attack.flags & (ATTACKFLAG_AIMONLY | ATTACKFLAG_DONTTURN))) {
		chr->act_attack.flags = attackflags;
		chr->act_attack.entityid = entityid;

		chr_attack_choose_endframe(chr);

		return true;
	}

	return false;
}

bool chr_face_entity(struct chrdata *chr, u32 attackflags, s32 entityid)
{
	if (chr_is_ready_for_orders(chr)) {
		if (chr->actiontype != ACT_STAND) {
			chr_stand(chr);
		}

		if (attackflags != chr->act_stand.flags || entityid != chr->act_stand.entityid) {
			chr->act_stand.flags = attackflags;
			chr->act_stand.entityid = entityid;
			chr->act_stand.reaim = 0;
			chr->act_stand.checkfacingwall = false;

			if (attackflags == ATTACKFLAG_AIMATTARGET && entityid == 1) {
				chr->act_stand.playwalkanim = true;
				chr->act_stand.entityid = 0;
			}
		}

		return true;
	}

	return false;
}

bool chr_go_to_pad(struct chrdata *chr, s32 padnum, u32 goposflags)
{
	if (padnum >= 0
			&& chr_is_ready_for_orders(chr)
#if VERSION >= VERSION_NTSC_1_0
			&& (g_NumChrsSeenPlayerRecently2 <= 8
				|| (chr->hidden & CHRHFLAG_BASICGUARD) == 0
				|| (chr->flags & CHRFLAG0_ACTIVATEALARM))
#else
			&& g_NumChrsSeenPlayerRecently2 <= 9
#endif
				) {
		padnum = chr_resolve_pad_id(chr, padnum);

#if VERSION >= VERSION_NTSC_1_0
		if (padnum >= 0)
#endif
		{
			RoomNum rooms[2];
			struct pad pad;

			pad_unpack(padnum, PADFIELD_ROOM | PADFIELD_POS, &pad);

			rooms[0] = pad.room;
			rooms[1] = -1;

			if (chr_go_to_room_pos(chr, &pad.pos, rooms, goposflags)) {
				return true;
			}
		}
	}

	return false;
}

bool chr_set_path(struct chrdata *chr, u32 path_id)
{
	chr->path = path_id;
	return true;
}

bool chr_try_start_patrol(struct chrdata *chr)
{
	struct path *path = path_find_by_id(chr->path);

	if (path && chr_is_ready_for_orders(chr)) {
		chr_start_patrol(chr, path);
		return true;
	}

	return false;
}

bool chr_try_surrender(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_HUMAN && chr_is_ready_for_orders(chr)) {
		chr_surrender(chr);
		return true;
	}

	return false;
}

bool chr_fade_out(struct chrdata *chr)
{
	chr_begin_dead(chr);
	chr_fade_corpse(chr);

	return true;
}

bool chr_go_to_target(struct chrdata *chr, u32 goposflags)
{
	if (chr_is_ready_for_orders(chr)) {
		if (
#if VERSION >= VERSION_NTSC_1_0
				g_NumChrsSeenPlayerRecently2 <= 8
				|| (chr->hidden & CHRHFLAG_BASICGUARD) == 0
				|| (chr->flags & CHRFLAG0_ACTIVATEALARM)
#else
				g_NumChrsSeenPlayerRecently2 <= 9
#endif
				) {
			struct prop *prop = chr_get_target_prop(chr);

			if (chr_go_to_room_pos(chr, &prop->pos, prop->rooms, goposflags)) {
				return true;
			}
		}
	}

	return false;
}

bool chr_go_to_chr(struct chrdata *chr, u32 dst_chrnum, u32 goposflags)
{
	if (chr_is_ready_for_orders(chr)) {
		if (
#if VERSION >= VERSION_NTSC_1_0
				g_NumChrsSeenPlayerRecently2 <= 8
				|| (chr->hidden & CHRHFLAG_BASICGUARD) == 0
				|| (chr->flags & CHRFLAG0_ACTIVATEALARM)
#else
				g_NumChrsSeenPlayerRecently2 <= 9
#endif
				) {
			struct chrdata *dstchr = chr_find_by_id(chr, dst_chrnum);

			if (dstchr && dstchr->prop && chr_go_to_room_pos(chr, &dstchr->prop->pos, dstchr->prop->rooms, goposflags)) {
				return true;
			}
		}
	}

	return false;
}

bool chr_go_to_prop(struct chrdata *chr, struct prop *prop, u32 goposflags)
{
	if (chr_is_ready_for_orders(chr) && prop) {
		if (chr_go_to_room_pos(chr, &prop->pos, prop->rooms, goposflags)) {
			return true;
		}
	}

	return false;
}

bool chr_go_to_pos(struct chrdata *chr, struct coord *pos, u32 goposflags)
{
	RoomNum inrooms[21];
	RoomNum aboverooms[21];

	if (chr_is_ready_for_orders(chr)) {
#if VERSION >= VERSION_NTSC_1_0
		if (g_NumChrsSeenPlayerRecently2 < 9
				|| (chr->hidden & CHRHFLAG_BASICGUARD) == 0
				|| (chr->flags & CHRCFLAG_NEVERSLEEP))
#else
		if (g_NumChrsSeenPlayerRecently2 < 10)
#endif
		{
			RoomNum *rooms = NULL;

			bg_find_rooms_by_pos(pos, inrooms, aboverooms, 20, NULL);

			if (inrooms[0] != -1) {
				rooms = inrooms;
			} else if (aboverooms[0] != -1) {
				rooms = aboverooms;
			}

			if (rooms != NULL && chr_go_to_room_pos(chr, pos, rooms, goposflags)) {
				return true;
			}
		}
	}

	return false;
}

s32 chr_go_to_cover_opposite_runfrompos(struct chrdata *chr, f32 arg1, u8 arg2)
{
	f32 mindist;

	if (!arg2) {
		arg1 -= chr_get_distance_to_coord(chr, &chr->runfrompos);
	}

	if (arg1 < 0) {
		chr_assign_cover_by_criteria(g_Vars.chrdata,
				COVERCRITERIA_FURTHEREST
				| COVERCRITERIA_DISTTOTARGET
				| COVERCRITERIA_ONLYNEIGHBOURINGROOMS
				| COVERCRITERIA_ROOMSFROMME, 0);
		return chr_go_to_cover(chr, GOPOSFLAG_RUN);
	}

	mindist = arg1 - 2000;

	if (mindist < 2000) {
		mindist = 2000;
	}

	if (!chr_assign_cover_away_from_danger(chr, mindist, arg1 + 10000)) {
		chr_assign_cover_by_criteria(g_Vars.chrdata,
				COVERCRITERIA_FURTHEREST
				| COVERCRITERIA_DISTTOTARGET
				| COVERCRITERIA_ONLYNEIGHBOURINGROOMS
				| COVERCRITERIA_ROOMSFROMME, 0);
	}

	return chr_go_to_cover(chr, GOPOSFLAG_RUN);
}

bool chr_try_stop(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_EYESPY) {
		chr_stand_immediate(chr, 0);
		return true;
	}

	if (chr_is_ready_for_orders(chr)) {
		chr_stop(chr);
		return true;
	}

	return false;
}

bool chr_try_surprised_onehand(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_HUMAN && chr_is_ready_for_orders(chr)) {
		chr_do_surprised_onehand(chr);
		return true;
	}

	return false;
}

bool chr_try_surprised_surrender(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_HUMAN && chr_is_ready_for_orders(chr)) {
		chr_do_surprised_surrender(chr);
		return true;
	}

	return false;
}

bool chr_try_surprised_lookaround(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_HUMAN && chr_is_ready_for_orders(chr)) {
		chr_do_surprised_lookaround(chr);
		return true;
	}

	return false;
}

bool chr_try_kneel(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_HUMAN && chr_is_ready_for_orders(chr)) {
		chr_kneel(chr);
		return true;
	}

	return false;
}

bool chr_try_anim(struct chrdata *chr, s32 animfnum, f32 startframe, f32 endframe, u8 chranimflags, s32 merge, f32 speed)
{
	if (chr_is_ready_for_orders(chr)) {
		chr_anim(chr, animfnum, startframe, endframe, chranimflags, merge, speed);
		return true;
	}

	return false;
}

bool chr_try_run_from_target(struct chrdata *chr)
{
	struct prop *target = chr_get_target_prop(chr);
	struct prop *prop = chr->prop;
	f32 ymax;
	f32 ymin;
	f32 radius;
	struct coord dst;
	RoomNum rooms[8];
	struct coord diff;
	f32 distance;

	if (chr_is_ready_for_orders(chr)) {
		if (!target) {
			return false;
		}

		diff.x = target->pos.x - prop->pos.x;
		diff.z = target->pos.z - prop->pos.z;
		diff.y = 0;

		distance = sqrtf(diff.f[0] * diff.f[0] + diff.f[2] * diff.f[2]);

		// Scale diff into range -1 to +1
		diff.x *= (1 / distance);
		diff.z *= (1 / distance);

		// Set dst to 1000 units in the opposite direction
		dst.x = prop->pos.x - diff.x * 1000;
		dst.z = prop->pos.z - diff.z * 1000;
		dst.y = prop->pos.y;

		prop_get_bbox(prop, &radius, &ymax, &ymin);

		// If dst runs into a wall, set it to closest valid spot
		if (cd_exam_cyl_move03(&prop->pos, prop->rooms, &dst,
					CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PATHBLOCKER | CDTYPE_BG,
					1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION) {
#if VERSION >= VERSION_JPN_FINAL
			cd_get_pos(&dst, 8796, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
			cd_get_pos(&dst, 8793, "chr/chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
			cd_get_pos(&dst, 8788, "chraction.c");
#else
			cd_get_pos(&dst, 8782, "chraction.c");
#endif
		}

		// Adjust dst to be two chr widths closer to avoid collision with wall
		dst.x = dst.x + (diff.x * radius + diff.x * radius);
		dst.z = dst.z + (diff.z * radius + diff.z * radius);

		if (chr_prop_can_move_to_pos_without_nav_slow(prop, &dst)) {
			u32 flags = GOPOSFLAG_RUN;

			if (CHRRACE(chr) == RACE_HUMAN) {
				f32 dist = chr_get_distance_to_coord(chr, &dst);

				if (dist > 100) {
					flags = GOPOSFLAG_RUN;
				} else if (dist < 50) {
					flags = GOPOSFLAG_WALK;
				} else {
					flags = GOPOSFLAG_JOG;
				}
			}

			los_find_final_room_exhaustive(&prop->pos, prop->rooms, &dst, rooms);
			chr_go_to_room_pos(chr, &dst, rooms, flags);

			return true;
		}
	}

	return false;
}

/**
 * Attempt to make the chr find a suitable prop in their current room to use as
 * cover and run to it. However, a bug prevents this from working.
 */
bool chr_go_to_cover_prop(struct chrdata *chr)
{
	struct prop *targetprop = chr_get_target_prop(chr);
	struct prop *chrprop = chr->prop;
	s16 i;
	s16 propnums[258];
	struct defaultobj *obj;
	s16 numprops;
	s16 startindex;

	if (targetprop == NULL) {
		return false;
	}

	if (chr_is_ready_for_orders(chr)) {
		room_get_props(chrprop->rooms, propnums, 256);

		for (numprops = 0; propnums[numprops] >= 0; numprops++);

		if (numprops > 0) {
			startindex = random() % numprops;
		}

		for (i = 0; i < numprops; i++) {
			struct prop *prop = &g_Vars.props[propnums[(i + startindex) % numprops]];
			obj = prop->obj;

			if (prop->type == PROPTYPE_OBJ
					&& (obj->hidden2 & OBJH2FLAG_DESTROYED) == 0
					&& (obj->hidden & OBJHFLAG_ONANOTHEROBJ) == 0
					&& (obj->hidden & OBJHFLAG_OCCUPIEDCHAIR) == 0) {
				f32 targetdist = prop_get_distance_to_prop(targetprop, prop);
				f32 chrdist = prop_get_distance_to_prop(chrprop, prop);

				// @bug: This condition cannot pass
				// (should be chrdist < targetdist + targetdist)
				if (chrdist < 300 && chrdist > targetdist + targetdist && targetdist > 800) {
					f32 propymax;
					f32 propymin;
					f32 propradius;
					f32 chrymax;
					f32 chrymin;
					f32 chrradius;
					f32 propheight;
					f32 chrheight;
					struct coord dstpos;
					RoomNum dstrooms[8];

					prop_get_bbox(prop, &propradius, &propymax, &propymin);
					prop_get_bbox(chrprop, &chrradius, &chrymax, &chrymin);

					propheight = propymax - propymin;
					chrheight = chrymax - chrymin;

					if (propheight > chrheight * 0.4f && propheight < chrheight * 0.9f) {
						prop_set_perim_enabled(prop, false);

						if (cd_test_los04(&chrprop->pos, chrprop->rooms, &prop->pos, CDTYPE_DOORS | CDTYPE_BG)) {
							prop_set_perim_enabled(prop, true);

							dstpos.x = prop->pos.x - (targetprop->pos.x - prop->pos.x) / targetdist * (propradius * 1.25f + chrradius);
							dstpos.z = prop->pos.z - (targetprop->pos.z - prop->pos.z) / targetdist * (propradius * 1.25f + chrradius);
							dstpos.y = prop->pos.y;

							if (chr_prop_can_move_to_pos_without_nav_slow(chrprop, &dstpos)) {
								u32 flags = GOPOSFLAG_JOG;

								if (CHRRACE(chr) == RACE_HUMAN) {
									if (chrdist > 1500) {
										flags = GOPOSFLAG_RUN;
									} else if (chrdist < 100) {
										flags = GOPOSFLAG_WALK;
									}
								}

								los_find_final_room_exhaustive(&chrprop->pos, chrprop->rooms, &dstpos, dstrooms);
								chr_go_to_room_pos(chr, &dstpos, dstrooms, flags);

								chr->proppreset1 = prop - g_Vars.props;
								obj->hidden |= OBJHFLAG_OCCUPIEDCHAIR;

								return true;
							}
						}
					}

					prop_set_perim_enabled(prop, true);
				}
			}
		}
	}

	return false;
}

bool chr_try_startalarm(struct chrdata *chr, s32 pad_id)
{
	if (CHRRACE(chr) == RACE_HUMAN && chr_is_ready_for_orders(chr)) {
		pad_id = chr_resolve_pad_id(chr, pad_id);

#if VERSION >= VERSION_NTSC_1_0
		if (pad_id >= 0)
#endif
		{
			struct defaultobj *obj = obj_find_by_pad_num(pad_id);

			if (obj && obj_is_healthy(obj)) {
				chr_startalarm(chr);
				return true;
			}
		}
	}

	return false;
}

bool chr_consider_grenade_throw(struct chrdata *chr, u32 attackflags, u32 entityid)
{
	bool done = false;

	if (CHRRACE(chr) == RACE_HUMAN &&
			chr->grenadeprob > (random() % 255) &&
			chr_get_distance_to_target(chr) > 200 &&
			chr_is_ready_for_orders(chr)) {
		struct prop *target = chr_get_target_prop(chr);
		struct coord pos;

		if (target) {
			pos.x = target->pos.x;
			pos.y = target->pos.y;
			pos.z = target->pos.z;
		}

		if (target && cd_test_los04(&chr->prop->pos, chr->prop->rooms, &pos,
					CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PATHBLOCKER | CDTYPE_BG)) {
			struct prop *leftprop = chr_get_held_prop(chr, HAND_LEFT);
			struct prop *rightprop = chr_get_held_prop(chr, HAND_RIGHT);
			struct weaponobj *weapon;

#if PIRACYCHECKS
			{
				u32 checksum = 0;
				s32 *i = (s32 *)&bg_reset;
				s32 *end = (s32 *)&bg_build_tables;
				u32 stackpadding[1];

				while (i < end) {
					checksum += *i;
					checksum += (*i >> 1);
					i++;
				}

				if (checksum != CHECKSUM_PLACEHOLDER) {
					// Make player explode continuously
					g_Vars.currentplayer->bondexploding = true;
					g_Vars.currentplayer->bondnextexplode = g_Vars.lvframe60 + 120;
					g_Vars.currentplayer->bondcurexplode = 0;
				}
			}
#endif

			// If grenade is equipped in either hand, use it
			if (rightprop) {
				weapon = rightprop->weapon;

				if (weapon->weaponnum == WEAPON_GRENADE || weapon->weaponnum == WEAPON_NBOMB) {
					chr_throwgrenade(chr, 0, false);
					chr->act_throwgrenade.flags = attackflags;
					chr->act_throwgrenade.entityid = entityid;
					done = true;
				}
			}

			if (!done && leftprop) {
				weapon = leftprop->weapon;

				if (weapon->weaponnum == WEAPON_GRENADE || weapon->weaponnum == WEAPON_NBOMB) {
					chr_throwgrenade(chr, 1, false);
					chr->act_throwgrenade.flags = attackflags;
					chr->act_throwgrenade.entityid = entityid;
					done = true;
				}
			}

			// Grenade not equipped, and using a single weapon
			if (!done && (leftprop == NULL || rightprop == NULL)) {
				struct prop *prop;
				u32 flags = 0;
				u32 stackpadding2[2];

				if (rightprop) {
					flags = OBJFLAG_WEAPON_LEFTHANDED;
				}

				if (stage_get_index(g_Vars.stagenum) == STAGEINDEX_MBR) {
					prop = chr_give_weapon(chr, MODEL_CHRGRENADE, WEAPON_NBOMB, flags);
				} else {
					prop = chr_give_weapon(chr, MODEL_CHRGRENADE, WEAPON_GRENADE, flags);
				}

				if (prop) {
					weapon = prop->weapon;
					weapon->base.hidden |= OBJHFLAG_GONE;
					chr_throwgrenade(chr, rightprop == NULL ? 0 : 1, true);
					chr->act_throwgrenade.flags = attackflags;
					chr->act_throwgrenade.entityid = entityid;
					done = true;
				}
			}
		}
	}

	return done;
}

bool chr_drop_item(struct chrdata *chr, u32 modelnum, u32 weaponnum)
{
	struct weaponobj *weapon;
	u8 race = CHRRACE(chr);

	if (race == RACE_DRCAROLL || race == RACE_ROBOT) {
		return false;
	}

	weapon = weapon_create_projectile_from_weapon_num(modelnum, (u8)weaponnum, chr);

	if (weapon && weapon->base.prop) {
		model_set_scale(weapon->base.model, weapon->base.model->scale);
		prop_reparent(weapon->base.prop, chr->prop);
		weapon->timer240 = TICKS(720);
		obj_set_dropped(weapon->base.prop, DROPTYPE_DEFAULT);
		chr->hidden |= CHRHFLAG_DROPPINGITEM;

		return true;
	}

	return false;
}

void chr_punch_inflict_damage(struct chrdata *chr, s32 damage, s32 range, u8 reverse)
{
	struct prop *targetprop = chr_get_target_prop(chr);
	struct gset gset = {WEAPON_UNARMED, 0, 0, FUNC_PRIMARY};
	struct coord vector;

	if (chr->aibot) {
		gset.weaponnum = chr->aibot->weaponnum;
		gset.weaponfunc = chr->aibot->gunfunc;
	}

	if (chr_is_target_in_fov(chr, 20, reverse)
			&& chr_get_distance_to_target(chr) < range
			&& cd_test_los04(&chr->prop->pos, chr->prop->rooms, &targetprop->pos,
				CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PATHBLOCKER | CDTYPE_BG)) {
		vector.x = targetprop->pos.x - chr->prop->pos.x;
		vector.y = 0;
		vector.z = targetprop->pos.z - chr->prop->pos.z;

		guNormalize(&vector.x, &vector.y, &vector.z);

		bgun_play_prop_hit_sound(&gset, targetprop, -1);

		if (targetprop->type == PROPTYPE_PLAYER || targetprop->type == PROPTYPE_CHR) {
			chr_damage_by_general(targetprop->chr, gset_get_damage(&gset) * damage, &vector, &gset, chr->prop, 200);
		}
	}

	weapon_play_melee_miss_sound(gset.weaponnum, chr->prop);
}

struct punchanim {
	s32 animnum;
	u32 damage;
	s16 hitframe;
	f32 endframe;
};

struct punchanim g_HumanPunchAnims[] = {
	{ ANIM_027C, 5, 20, 60 },
	{ ANIM_027D, 5, 20, 31 },
	{ ANIM_027E, 5, 20, 48 },
	{ ANIM_027F, 5, 20, 69 },
	{ ANIM_0212, 5, 20, 64 },
	{ ANIM_0213, 5, 20, 52 },
	{ ANIM_0214, 5, 20, 51 },
	{ ANIM_020E, 5, 20, 53 },
	{ ANIM_020F, 5, 20, 89 },
	{ ANIM_0210, 5, 20, 71 },
	{ ANIM_0215, 5, 20, 62 },
	{ ANIM_0211, 5, 20, 72 },
};

struct punchanim g_SkedarPunchAnims[] = {
	{ ANIM_034C, 15, 25, 100 },
	{ ANIM_034D, 15, 25, -1  },
	{ ANIM_0395, 15, 25, -1  },
	{ ANIM_0346, 15, 25, -1  },
	{ ANIM_0347, 15, 25, -1  },
	{ ANIM_034F, 15, 25, -1  },
};

/**
 * Make the chr try to punch or kick.
 *
 * The function is only ever called with reverse = 0. If non-zero, it would
 * cause Skedar to kick behind them.
 *
 * Note that the final human anim can't be used because the modulus value is too
 * short by one.
 */
bool chr_try_punch(struct chrdata *chr, u8 reverse)
{
	struct punchanim *anims = NULL;
	s32 race = CHRRACE(chr);
	s32 animindex;
	u32 chranimflags = (random() % 256 > 128) ? CHRANIMFLAG_FLIP : 0;
	s32 chrhitradius;
	s32 playerhitradius;
	f32 startframe;
	bool ok;

	if (race == RACE_HUMAN) {
		anims = g_HumanPunchAnims;
		startframe = 10;
		chrhitradius = 120;
		playerhitradius = 120;
		animindex = random() % 11;
	} else if (race == RACE_SKEDAR) {
		anims = g_SkedarPunchAnims;
		chrhitradius = 200;
#if VERSION >= VERSION_NTSC_1_0
		playerhitradius = 200;
#else
		playerhitradius = 350;
#endif
		startframe = 20;

		if (reverse) {
			// Skedar kick behind
			animindex = 5;
		} else if (!chr->weapons_held[HAND_RIGHT] && !chr->weapons_held[HAND_LEFT]) {
			// Unarmed: Only use indexes 0 or 1
			animindex = random() % 2;
		} else {
			// Allow indexes 0-4, but if 3 or 4 then flip the anim based on
			// which hand is holding the gun
			animindex = random() % 5;

			if (animindex >= 3) {
				if (!chr->weapons_held[HAND_RIGHT] || !chr->weapons_held[HAND_LEFT]) {
					if (chr->weapons_held[HAND_RIGHT]) {
						chranimflags = 0;
					} else if (chr->weapons_held[HAND_LEFT]) {
						chranimflags = CHRANIMFLAG_FLIP;
					}
				}
			}
		}
	}

	if (chr_has_flag(chr, CHRFLAG1_ADJUSTPUNCHSPEED, BANK_1)) {
		if (chr_has_flag(chr, CHRFLAG0_CHUCKNORRIS, BANK_0)) {
			// Fast punch
			ok = chr_try_anim(chr, anims[animindex].animnum, startframe, anims[animindex].endframe, chranimflags, 16, 1.5f);
		} else {
			// Slow punch
			ok = chr_try_anim(chr, anims[animindex].animnum, startframe, anims[animindex].endframe - 25, chranimflags, 16, 0.5f);
		}

		chr->dodgerating = 0;
	} else {
		// Normal punch
		ok = chr_try_anim(chr, anims[animindex].animnum, startframe, anims[animindex].endframe, chranimflags, 16, 0.85f);
		chr->dodgerating = 0;
	}

	if (ok) {
		struct prop *targetprop = chr_get_target_prop(chr);

		if (targetprop->type == PROPTYPE_EYESPY || targetprop->type == PROPTYPE_PLAYER) {
			chr->act_anim.hitradius = playerhitradius;
		} else {
			chr->act_anim.hitradius = chrhitradius;
		}

		chr->act_anim.ishitanim = true;
		chr->act_anim.hitframe = anims[animindex].hitframe;

		if (g_Vars.normmplayerisrunning) {
			chr->act_anim.hitdamage = 1;
		} else if (chr_has_flag(chr, CHRFLAG1_ADJUSTPUNCHSPEED, BANK_1) && chr_has_flag(chr, CHRFLAG0_CHUCKNORRIS, BANK_0)) {
			chr->act_anim.hitdamage = (u16)anims[animindex].damage * (f32)chr->morale + (u16)anims[animindex].damage * (f32)chr->morale;
		} else if (chr_has_flag(chr, CHRFLAG1_PUNCHHARDER, BANK_1)) {
			chr->act_anim.hitdamage = (u16)anims[animindex].damage * 6;
		} else {
			chr->act_anim.hitdamage = anims[animindex].damage;
		}

		chr->act_anim.reverse = reverse;
		chr->chrflags &= ~CHRCFLAG_INJUREDTARGET;

		return true;
	}

	return false;
}

void chr_0f03c03c(void)
{
	// empty
}

/**
 * Verifies that the given chr struct is actually an eyespy and returns the
 * eyespy struct.
 *
 * Eyespys have their own chr struct, even though they aren't a chr. Iterating
 * the player list is required because the only pointer to an eyespy is via the
 * player struct.
 */
struct eyespy *chr_to_eyespy(struct chrdata *chr)
{
	if (chr && chr->prop) {
		if (CHRRACE(chr) == RACE_EYESPY) {
			s32 playercount = PLAYERCOUNT();
			s32 i;

			for (i = 0; i < playercount; i++) {
				if (g_Vars.players[i]->eyespy && chr->prop == g_Vars.players[i]->eyespy->prop) {
					return g_Vars.players[i]->eyespy;
				}
			}

			return NULL;
		}
	}

	return NULL;
}

void chr_tick_stand(struct chrdata *chr)
{
	s32 race;
	s32 i;
	s32 j;
	s32 stack2;
	struct prop *leftgun;
	struct prop *rightgun;
	u32 stack[2];
	f32 angle;
	f32 sp74[8];
	f32 sp70;
	f32 sp6c;
	s32 tmp;
	s32 index;
	s32 sp44[8];

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_choose_stand_animation(chr, chr->act_stand.mergetime);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (model_get_anim_num(chr->model) == ANIM_SNIPING_GETUP) {
		if (model_get_cur_anim_frame(chr->model) >= model_get_anim_end_frame(chr->model)) {
			chr_choose_stand_animation(chr, 8);
			chr->act_stand.prestand = 0;
		}

		return;
	}

	if (chr->sleep > 0) {
		return;
	}

	race = CHRRACE(chr);

	if (race == RACE_EYESPY) {
		return;
	}

	if (chr->act_stand.prestand) {
		if (model_get_cur_anim_frame(chr->model) >= model_get_anim_end_frame(chr->model)) {
			chr_choose_stand_animation(chr, 8);
			chr->act_stand.prestand = 0;
		}

		chr->sleep = 0;
		return;
	}

	if (!chr->aibot && (race == RACE_HUMAN || race == RACE_SKEDAR) && chr->act_stand.flags > 0) {
		if (chr->act_stand.reaim) {
			chr->act_stand.turning = chr_turn(chr, chr->act_stand.turning, model_get_num_anim_frames(chr->model) - 1, 1, 0);

			if (chr->act_stand.turning != TURNSTATE_TURNING) {
				chr_choose_stand_animation(chr, 8);
				chr->act_stand.reaim = false;

				if (chr->act_stand.flags & ATTACKFLAG_AIMATDIRECTION) {
					chr->act_stand.flags = 0;
				}
			}
		} else {
			f32 relangle = chr_get_attack_entity_relative_angle(chr, chr->act_stand.flags, chr->act_stand.entityid);

			if ((relangle > BADDTOR(20) && relangle < BADDTOR(340))
					|| (relangle > BADDTOR(10) && relangle < BADDTOR2(350) && !chr->act_stand.playwalkanim)) {
				leftgun = chr_get_held_prop(chr, HAND_LEFT);
				rightgun = chr_get_held_prop(chr, HAND_RIGHT);

				chr->act_stand.reaim = true;
				chr->act_stand.turning = TURNSTATE_TURNING;

				if (race == RACE_HUMAN) {
					if ((leftgun && rightgun)
							|| (!leftgun && !rightgun)
							|| weapon_is_onehanded(leftgun)
							|| weapon_is_onehanded(rightgun)) {
						model_set_animation(chr->model, ANIM_006B, random() % 2, 0, 0.5f, 16);
						model_set_anim_end_frame(chr->model, anim_get_num_frames(ANIM_006B) - 1);
					} else {
						if (rightgun || leftgun) {
							model_set_animation(chr->model, ANIM_0028, leftgun != NULL, 0, 0.5f, 16);
							model_set_anim_end_frame(chr->model, anim_get_num_frames(ANIM_0028) - 1);
						}
					}
				} else if (race == RACE_SKEDAR) {
					model_set_animation(chr->model, ANIM_0392, random() % 2, 0, 0.5f, 16);
					model_set_anim_end_frame(chr->model, anim_get_num_frames(ANIM_0392) - 1);
				}
			} else if (chr->act_stand.flags & ATTACKFLAG_AIMATDIRECTION) {
				chr->act_stand.flags = 0;
			}
		}

		chr->sleep = 0;
		return;
	}

	if (chr->aibot) {
		return;
	}

	if (chr->prop->flags & PROPFLAG_ONANYSCREENPREVTICK) {
		chr->sleep = 0;
	} else {
		chr->sleep = 14 + (random() % 5);
	}

	if (chr->act_stand.checkfacingwall == false) {
		return;
	}

	if (chr->chrflags & CHRCFLAG_CANFACEWALL) {
		chr->act_stand.checkfacingwall = false;
		return;
	}

	chr->act_stand.wallcount -= chr->sleep;

	if (chr->act_stand.wallcount >= 0) {
		return;
	}

	sp6c = sp70 = chr_get_theta(chr);

	for (i = 0; i < ARRAYCOUNT(sp74); i++) {
		sp6c += BADDTOR(45);

		if (sp6c >= BADDTOR(360)) {
			sp6c -= BADDTOR(360);
		}

		sp74[i] = chr_prop_get_available_dist_at_angle(chr->prop, sp6c, 1000, CDTYPE_BG, 0, 1);
	}

	for (i = 0; i < ARRAYCOUNT(sp44); i++) {
		sp44[i] = i;
	}

	for (i = 0; i < ARRAYCOUNT(sp44) - 1; i++) {
		index = i;

		for (j = index + 1; j < ARRAYCOUNT(sp44); j++) {
			if (sp74[sp44[j]] < sp74[sp44[index]]) {
				index = j;
			}
		}

		j = sp44[i];
		sp44[i] = sp44[index];
		sp44[index] = j;
	}

	index = -1;

	if (sp74[0] < 490) {
		if (sp74[sp44[4]] < 200) {
			index = 7;
		} else if (sp44[0] == 0 || sp44[1] == 0 || sp44[2] == 0) {
			if ((sp44[3] == 4 || sp44[4] == 4) && (random() % 3) == 0) {
				if (sp44[3] == 4) {
					index = 3;
				} else {
					index = 4;
				}
			} else {
				index = 5 + random() % 3;
			}
		} else if ((sp44[0] == 1 || sp44[0] == 7) && sp44[5] && sp44[6] && sp44[7]) {
			index = 5 + random() % 3;
		}
	}

	if (index >= 0) {
		i = sp44[index];
		angle = sp70 + i * BADDTOR(45);

		if (angle >= BADDTOR(360)) {
			angle -= BADDTOR(360);
		}

		chr_face_entity(chr, ATTACKFLAG_AIMATDIRECTION, angle * 10432.0390625f);
	} else {
		chr->act_stand.checkfacingwall = false;
	}
}

void chr_tick_kneel(struct chrdata *chr)
{
	chr->sleep = 0;

	if ((chr->hidden & CHRHFLAG_NEEDANIM) && !model_is_anim_merging(chr->model)) {
		chr_kneel_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}
}

void chr_tick_anim(struct chrdata *chr)
{
	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		model_set_animation(chr->model, chr->act_anim.animnum, chr->act_anim.flip,
				chr->act_anim.startframe, chr->act_anim.speed, chr->act_anim.blend);

		if (chr->act_anim.endframe >= 0) {
			model_set_anim_end_frame(chr->model, chr->act_anim.endframe);
		}

		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (!chr->act_anim.pauseatend && model_get_cur_anim_frame(chr->model) >= model_get_anim_end_frame(chr->model)) {
		chr_stand(chr);
	}

	if (chr->act_anim.ishitanim && model_get_cur_anim_frame(chr->model) >= (s32)chr->act_anim.hitframe) {
		chr->act_anim.ishitanim = false;
		chr_punch_inflict_damage(chr, chr->act_anim.hitdamage, chr->act_anim.hitradius, chr->act_anim.reverse);
	}

	// Play sneezing sound
	if (CHRRACE(chr) == RACE_HUMAN
			&& model_get_anim_num(chr->model) == ANIM_SNEEZE
			&& model_get_cur_anim_frame(chr->model) >= 42
			&& (g_Vars.lvframenum % 2) == 0
			&& chr_get_distance_to_current_player(chr) < 800) {
		ps_create(NULL, chr->prop, SFXNUM_0037, -1,
				-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
	}

	if (chr->sleep <= 0 && chr->act_anim.slowupdate) {
		chr->sleep = 14 + (random() % 5);
	}

	if (model_get_anim_num(chr->model) == ANIM_RELOAD_0209) {
		chr_set_firing(chr, HAND_RIGHT, false);
		chr_set_firing(chr, HAND_LEFT, false);
	}
}

void chr_tick_surrender(struct chrdata *chr)
{
	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_surrender_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (chr->sleep <= 0) {
		if (CHRRACE(chr) == RACE_HUMAN) {
			struct model *model = chr->model;
			chr->sleep = 16;

			// If chr is doing the surrender anim where they walk backwards slowly,
			// check for obstacle and switch to stationary surrender anim if found.
			if (model_get_anim_num(model) == ANIM_SURRENDER_002F && model_get_cur_anim_frame(model) >= 80.0f) {
				struct coord dir = {0, 0, 0};
				f32 angle = chr_get_theta(chr);
				dir.x = -sinf(angle);
				dir.z = -cosf(angle);

				if (!chr_prop_can_move_in_dir(chr->prop, &dir, 20)) {
					model_set_animation(chr->model, ANIM_SURRENDER_002E, random() & 1, 30, 0.5, 16);
					model_set_anim_looping(chr->model, 30, 16);
				}
			}
		}
	}
}

void chr_fade_corpse(struct chrdata *chr)
{
	if (chr->actiontype == ACT_DEAD || chr->actiontype == ACT_DRUGGEDKO) {
		chr->act_dead.fadenow = true;
	}
}

void chr_fade_corpse_when_offscreen(struct chrdata *chr)
{
	if (chr->actiontype == ACT_DEAD) {
		chr->act_dead.fadewheninvis = true;
	}
}

void chr_tick_dead(struct chrdata *chr)
{
	struct aibot *aibot = chr->aibot;

	// If fade is active, handle it
	if (chr->act_dead.fadetimer60 >= 0) {
		chr->act_dead.fadetimer60 += g_Vars.lvupdate60;

		if (chr->act_dead.fadetimer60 >= TICKS(90)) {
			// Fade finished
			chr->fadealpha = 0;

			if (aibot) {
				bot_spawn(chr, true);
			} else {
				chr->hidden |= CHRHFLAG_DELETING;
			}
		} else {
			// Still fading
			chr->fadealpha = (TICKS(90) - chr->act_dead.fadetimer60) * 255 / TICKS(90);
		}
	} else {
		// If fade has been triggered (this can happen when the corpse is on
		// screen and there's lots of other chrs around)
		if (chr->act_dead.fadenow) {
			chr->act_dead.fadetimer60 = 0;
			chr_drop_items_for_owner_reap(chr);
		}

		if (chr->prop->flags & PROPFLAG_ONANYSCREENPREVTICK) {
			// Keep corpse for now
			chr->act_dead.invistimer60 = 0;
		} else {
			chr->act_dead.invistimer60 += g_Vars.lvupdate60;
		}

		if (chr->act_dead.fadewheninvis && chr->act_dead.invistimer60 >= TICKS(120)) {
			// Remove corpse (off-screen)
			if (aibot == NULL) {
				chr->hidden |= CHRHFLAG_DELETING;
			}

			chr->fadealpha = 0;

			chr_drop_items_for_owner_reap(chr);
		}
	}

	if (aibot == NULL) {
		chr->ailist = NULL;
	}
}

/**
 * This function is called when a chr is injured or killed. It iterates other
 * chrs within 2000 units who can see the chr and updates their chrseeshot or
 * chrseedie properties.
 *
 * The search stops once 7 chrs have been found in range, regardless of whether
 * they can see the chr, presumably to avoid doing too many expensive line of
 * sight checks in one tick. The last iterated chr index is stored and continued
 * from there next time the function is called. The function is called on
 * subsequent ticks while the chr is still in their injured or dying action.
 */
void chr_alert_others_of_injury(struct chrdata *chr, bool dying)
{
	s32 index = 0;
	s32 numinrange = 0;
	s32 numchrs = chrs_get_num_slots();

	if (g_Vars.antiplayernum >= 0 && chr->prop == g_Vars.anti->prop) {
		return;
	}

	if (chr->actiontype == ACT_ARGH) {
		index = chr->act_argh.notifychrindex;
	} else if (chr->actiontype == ACT_DIE || chr->actiontype == ACT_DRUGGEDDROP) {
		index = chr->act_die.notifychrindex;
	} else if (chr->actiontype == ACT_DEAD) {
		index = chr->act_dead.notifychrindex;
	}

	for (; index < numchrs && numinrange < 7; index++) {
		struct chrdata *loopchr = &g_ChrSlots[index];

		if (loopchr->model && loopchr->prop && (loopchr->prop->flags & PROPFLAG_ENABLED)) {
			f32 xdiff = loopchr->prop->pos.x - chr->prop->pos.x;
			f32 ydiff = loopchr->prop->pos.y - chr->prop->pos.y;
			f32 zdiff = loopchr->prop->pos.z - chr->prop->pos.z;

			if (xdiff * xdiff + ydiff * ydiff + zdiff * zdiff < 4000000.0f) {
				numinrange++;

				if (chr_has_los_to_pos_wasteful(loopchr, &chr->prop->pos, chr->prop->rooms)) {
					if (dying == false) {
						loopchr->chrseeshot = chr->chrnum;
					} else {
						loopchr->chrseedie = chr->chrnum;
					}
				}
			}
		}
	}

	if (chr->actiontype == ACT_ARGH) {
		chr->act_argh.notifychrindex = index;
	} else if (chr->actiontype == ACT_DIE || chr->actiontype == ACT_DRUGGEDDROP) {
		chr->act_die.notifychrindex = index;
	} else if (chr->actiontype == ACT_DEAD) {
		chr->act_dead.notifychrindex = index;
	}
}

void chr_tick_die(struct chrdata *chr)
{
	struct model *model = chr->model;
	u32 race = CHRRACE(chr);

	u16 thuds[] = {
		SFXMAP_808D_THUD,
		SFXMAP_808E_THUD,
		SFXMAP_808F_THUD,
		SFXMAP_8090_THUD,
		SFXMAP_8091_THUD,
		SFXMAP_8092_THUD,
		SFXMAP_8093_THUD,
		SFXMAP_8094_THUD,
		SFXMAP_8095_THUD,
		SFXMAP_8096_THUD,
		SFXMAP_8097_THUD,
	};

	u16 specialdiesounds[] = {
#if VERSION >= VERSION_NTSC_1_0
		SFXMAP_8129_M1_NOOO, // "Noooo!"
		SFXMAP_812F_M1_SCREAM, // Death scream
		SFXMAP_813A_M2_NOOO, // "Noooo!"
		SFXMAP_813A_M2_NOOO, // "Noooo!"
		SFXMAP_812F_M1_SCREAM, // Death scream
#else
		SFXNUM_0313_M_NOOO,
		SFXNUM_034C_M_WAIL,
		SFXNUM_0411_M_NOOO,
		SFXNUM_0411_M_NOOO,
		SFXNUM_034C_M_WAIL,
#endif
		SFXMAP_8092_THUD,
		SFXMAP_8093_THUD,
		SFXMAP_8094_THUD,
		SFXMAP_8095_THUD,
		SFXMAP_8096_THUD,
		SFXMAP_8097_THUD,
	};

	static s32 thudindex = 0;

	if (race == RACE_EYESPY) {
		return;
	}

	if (race == RACE_ROBOT) {
		struct prop *prop = chr->prop;
		ps_stop_sound(prop, PSTYPE_GENERAL, 0xffff);
		explosion_create_simple(prop, &prop->pos, prop->rooms, EXPLOSIONTYPE_8, g_Vars.currentplayernum);
		chr->hidden |= CHRHFLAG_DELETING;
		return;
	}

	if (race == RACE_DRCAROLL) {
		struct prop *prop = chr->prop;

		if (g_DrCarollDyingTimer > TICKS(120) && chr->voicebox) {
			// Play speech
			u16 phrases[] = {
				SFXNUM_024D_DRC_SYSTEMS_FAILURE,
				SFXNUM_024E_DRC_YOU_GO_ON,
				SFXNUM_024F_DRC_I_CANT_MAKE_IT,
				SFXNUM_0256_DRC_IM_DYING,
				SFXNUM_0257_DRC_GOODBYE,
				SFXNUM_0258_DRC_YOU_WERE_SUPPOSED,
			};

			ps_create(NULL, chr->prop, phrases[random() % 5], -1,
					-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
			chr->voicebox = 0;
		}

		// Change images randomly
		if (chr->act_die.drcarollimagedelay > 0) {
			chr->act_die.drcarollimagedelay -= g_Vars.lvupdate60;
		} else {
			chr->act_die.drcarollimagedelay = (random() % TICKS(1000)) * 0.01f + 5.0f;
			chr->drcarollimage_left = 1 + (s32)((random() % 400) * 0.01f);
			chr->drcarollimage_right = 1 + (s32)((random() % 400) * 0.01f);
		}

		if (g_DrCarollDyingTimer > TICKS(310)) {
			// Explode
			ps_stop_sound(prop, PSTYPE_GENERAL, 0xffff);
			explosion_create_simple(prop, &prop->pos, prop->rooms, EXPLOSIONTYPE_8, g_Vars.currentplayernum);
			chr_begin_dead(chr);
		} else if (chr->soundtimer > (s32)var80068080) {
			// Play shield damage sound
			chr->soundtimer = 0;
			var80068080 -= 5;
			ps_create(NULL, prop, SFXNUM_0064_SHIELD_DAMAGE, -1,
					-1, PSFLAG_AMBIENT, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
			sparks_create(prop->rooms[0], prop, &prop->pos, NULL, 0, SPARKTYPE_ELECTRICAL);
		}

		return;
	}

	// Human or Skedar
	// If due, play thud 1 sound
	if (chr->act_die.thudframe1 >= 0 && model_get_cur_anim_frame(model) >= chr->act_die.thudframe1) {
		if (chr->specialdie == 0) {
			ps_create(NULL, chr->prop, thuds[thudindex], -1,
					-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
		} else if (chr->specialdie != SPECIALDIE_OVERRAILING) {
			ps_create(NULL, chr->prop, specialdiesounds[chr->specialdie - 1], -1,
					-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
		}

		thudindex++;

		if (thudindex > 10) {
			thudindex = 0;
		}

		chr->act_die.thudframe1 = -1;
	}

	// If due, play thud 2 sound
	if (chr->act_die.thudframe2 >= 0 && model_get_cur_anim_frame(model) >= chr->act_die.thudframe2) {
		if (chr->specialdie < 5) {
			ps_create(NULL, chr->prop, SFXMAP_808E_THUD, -1,
					-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
		} else {
			ps_create(NULL, chr->prop, thuds[thudindex], -1,
					-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
		}

		thudindex++;

		if (thudindex > 10) {
			thudindex = 0;
		}

		chr->act_die.thudframe2 = -1;
	}

	// Check for end of death animation and switch to ACT_DEAD
	if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
		if (CHRRACE(chr) == RACE_HUMAN && model_get_anim_num(model) == ANIM_DEATH_STOMACH_LONG) {
			model_set_animation(model, ANIM_003C, !model_is_flipped(model), 50, 0.3, anim_get_num_frames(ANIM_003C) - 51.0f);
			model_set_anim_speed(model, 0.5, anim_get_num_frames(ANIM_003C) - 51.0f);
			return;
		}

		chr_begin_dead(chr);
	}

	chr_alert_others_of_injury(chr, true);
}

void chr_tick_druggedcomingup(struct chrdata *chr)
{
	u16 thuds[] = {
		SFXMAP_808D_THUD,
		SFXMAP_808E_THUD,
		SFXMAP_808F_THUD,
		SFXMAP_8090_THUD,
		SFXMAP_8091_THUD,
		SFXMAP_8092_THUD,
		SFXMAP_8093_THUD,
		SFXMAP_8094_THUD,
		SFXMAP_8095_THUD,
		SFXMAP_8096_THUD,
		SFXMAP_8097_THUD,
	};

	chr->act_druggedcomingup.timer60 += g_Vars.lvupdate60;

	if (chr->act_druggedcomingup.timer60 > 0) {
		struct animtablerow *row;
		s32 race = CHRRACE(chr);
		struct model *model = chr->model;
		s32 i = 0;
		bool done = false;
		struct prop *weapon;

		chr_uncloak(chr, true);

		chr->actiontype = ACT_DRUGGEDDROP;

		while (!done) {
			if (i >= 0
					&& g_AnimTablesByRace[race][i].deathanims != NULL
					&& g_AnimTablesByRace[race][i].deathanimcount > 0) {
				s32 index = random() % g_AnimTablesByRace[race][i].deathanimcount;
				row = &g_AnimTablesByRace[race][i].deathanims[index];

				chr->act_die.thudframe1 = row->thudframe1;
				chr->act_die.thudframe2 = row->thudframe2;

				model_set_animation_with_merge(model, row->animnum, row->flip, 0, row->speed, 16, true);

				if (row->endframe >= 0) {
					model_set_anim_end_frame(model, row->endframe);
				}

				done = true;
			}

			if (!done) {
				i++;

				if (g_AnimTablesByRace[race][i].hitpart == -1) {
					done = true;
				}
			}
		}

		weapon = chr->weapons_held[HAND_RIGHT];

		if (weapon && (weapon->obj->flags & OBJFLAG_AIUNDROPPABLE) == 0) {
			obj_set_dropped(weapon, DROPTYPE_DEFAULT);
			chr->hidden |= CHRHFLAG_DROPPINGITEM;
		}

		weapon = chr->weapons_held[HAND_LEFT];

		if (weapon && (weapon->obj->flags & OBJFLAG_AIUNDROPPABLE) == 0) {
			obj_set_dropped(weapon, DROPTYPE_DEFAULT);
			chr->hidden |= CHRHFLAG_DROPPINGITEM;
		}

		chr_drop_concealed_items(chr);
	}
}

void chr_tick_druggeddrop(struct chrdata *chr)
{
	struct model *model = chr->model;

	u16 thuds[] = {
		SFXMAP_808D_THUD,
		SFXMAP_808E_THUD,
		SFXMAP_808F_THUD,
		SFXMAP_8090_THUD,
		SFXMAP_8091_THUD,
		SFXMAP_8092_THUD,
		SFXMAP_8093_THUD,
		SFXMAP_8094_THUD,
		SFXMAP_8095_THUD,
		SFXMAP_8096_THUD,
		SFXMAP_8097_THUD,
	};

	static s32 thudindex = 0;

	// If due, play thud 1 sound
	if (chr->act_die.thudframe1 >= 0 && model_get_cur_anim_frame(model) >= chr->act_die.thudframe1) {
		ps_create(NULL, chr->prop, thuds[thudindex], -1,
				-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);

		thudindex++;

		if (thudindex > 10) {
			thudindex = 0;
		}

		chr->act_die.thudframe1 = -1;
	}

	// If due, play thud 2 sound
	if (chr->act_die.thudframe2 >= 0 && model_get_cur_anim_frame(model) >= chr->act_die.thudframe2) {
		ps_create(NULL, chr->prop, thuds[thudindex], -1,
				-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);

		thudindex++;

		if (thudindex > 10) {
			thudindex = 0;
		}

		chr->act_die.thudframe2 = -1;
	}

	// If falling animation finished, assign ACT_DRUGGEDKO
	if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
		chr->actiontype = ACT_DRUGGEDKO;
		chr->act_dead.fadetimer60 = chr->aibot ? 0 : -1;
		chr->act_dead.fadenow = false;
		chr->act_dead.fadewheninvis = false;
		chr->act_dead.invistimer60 = 0;
		chr->act_dead.notifychrindex = 0;
		chr->sleep = 0;
	}

	chr_alert_others_of_injury(chr, true);
}

void chr_tick_druggedko(struct chrdata *chr)
{
	bool reap = false;

	// If fade is active, handle it
	if (chr->act_dead.fadetimer60 >= 0) {
		chr->act_dead.fadetimer60 += g_Vars.lvupdate60;

		if (chr->act_dead.fadetimer60 >= TICKS(90)) {
			reap = true;
		} else {
			chr->fadealpha = (TICKS(90) - chr->act_dead.fadetimer60) * 255 / TICKS(90);
		}
	} else if ((chr->chrflags & CHRCFLAG_KEEPCORPSEKO) == 0) {
		if (chr->act_dead.fadenow) {
			chr->act_dead.fadetimer60 = 0;
		}

		if (chr->prop->flags & PROPFLAG_ONANYSCREENPREVTICK) {
			chr->act_dead.invistimer60 = 0;
		} else {
			chr->act_dead.invistimer60 += g_Vars.lvupdate60;
		}

		if (chr->act_dead.fadewheninvis && chr->act_dead.invistimer60 >= TICKS(120)) {
			reap = true;
		}
	}

	if (reap) {
		chr->fadealpha = 0;
		chr->hidden |= CHRHFLAG_DELETING;
		chr_drop_items_for_owner_reap(chr);
	}
}

void chr_tick_argh(struct chrdata *chr)
{
	struct model *model = chr->model;

	if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
		chr_record_last_see_target_time(chr);

		if (CHRRACE(chr) == RACE_HUMAN && model_get_anim_num(model) == ANIM_DEATH_STOMACH_LONG) {
			chr_stand_checkfacingwall(chr, 26);
		} else {
			if (chr->race == RACE_DRCAROLL) {
				chr->drcarollimage_left = DRCAROLLIMAGE_EYESDEFAULT;
				chr->drcarollimage_right = DRCAROLLIMAGE_EYESDEFAULT;
			}

			chr_stop(chr);
		}
	}

	chr_alert_others_of_injury(chr, false);
}

void chr_tick_preargh(struct chrdata *chr)
{
	struct model *model = chr->model;

	if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
		struct coord dir;
		dir.x = chr->act_preargh.dir.x;
		dir.y = chr->act_preargh.dir.y;
		dir.z = chr->act_preargh.dir.z;

		chr_react_to_damage(chr, &dir,
				chr->act_preargh.relshotdir,
				chr->act_preargh.hitpart,
				&chr->act_preargh.gset,
				chr->act_preargh.aplayernum);
	}
}

void chr_tick_sidestep(struct chrdata *chr)
{
	struct model *model = chr->model;

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_sidestep_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
		chr_record_last_see_target_time(chr);
		chr_stand_checkfacingwall(chr, 10);
	}
}

void chr_tick_jumpout(struct chrdata *chr)
{
	struct model *model = chr->model;

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_jumpout_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
		chr_record_last_see_target_time(chr);
		chr_stop(chr);
	}
}

void chr_tick_test(struct chrdata *chr)
{
	struct model *model = chr->model;

	if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
		chr_stand(chr);
	}
}

void chr_tick_startalarm(struct chrdata *chr)
{
	struct model *model = chr->model;

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_startalarm_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (model_get_cur_anim_frame(model) >= 60) {
		alarm_activate();
	}

	if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
		chr_stop(chr);
	}
}

void chr_tick_surprised(struct chrdata *chr)
{
	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_surprised_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (CHRRACE(chr) == RACE_HUMAN) {
		struct model *model = chr->model;

		if (model_get_cur_anim_frame(model) >= model_get_anim_end_frame(model)) {
			if (model_get_anim_num(model) == ANIM_SURRENDER_002E) {
				chr_stand_checkfacingwall(chr, 26);
			} else if (model_get_anim_num(model) == ANIM_003F) {
				chr_stand_checkfacingwall(chr, 26);
			} else {
				chr_stop(chr);
			}
		}
	}
}

void chr_update_fireslot(struct chrdata *chr, s32 handnum, bool withsound, bool withbeam, struct coord *from, struct coord *to)
{
	struct prop *weaponprop;
	struct weaponobj *weapon;
	s32 weaponnum;
	bool playsound = false;
	u8 duration;
	u16 soundnum;

	weaponprop = chr_get_held_prop(chr, handnum);

	if (weaponprop) {
		weapon = weaponprop->weapon;
		weaponnum = weapon->weaponnum;
		duration = gset_get_fireslot_duration(&weapon->gset);
		soundnum = gset_get_single_shoot_sound(&weapon->gset);

		if (chr->fireslots[handnum] < 0) {
			chr->fireslots[handnum] = bgun_allocate_fireslot();
		}

		if (chr->fireslots[handnum] >= 0) {
			struct fireslot *fireslot = &g_Fireslots[chr->fireslots[handnum]];

			if (withsound) {
				if (duration > 0) {
#if VERSION >= VERSION_NTSC_1_0
					if (chr->hidden2 & CHRH2FLAG_FIRESOUNDDONE)
#else
					if (chr->hidden & CHRHFLAG_FIRESOUNDDONE)
#endif
					{
						playsound = false;
					} else {
						if (g_Vars.lvframe60 > fireslot->endlvframe) {
							playsound = true;
						} else {
							playsound = false;
						}
					}
				} else {
					playsound = true;
				}
			}

			if (playsound) {
#if VERSION >= VERSION_NTSC_1_0
				ps_create(NULL, chr->prop, soundnum, -1, -1,
						PSFLAG_AMBIENT, PSFLAG2_PRINTABLE, PSTYPE_CHRSHOOT, NULL, -1, NULL, -1, -1, -1, -1);
				fireslot->endlvframe = (u32)g_Vars.lvframe60 + duration;
				chr->hidden2 |= CHRH2FLAG_FIRESOUNDDONE;
#else
				ps_create(NULL, chr->prop, soundnum, -1, -1,
						PSFLAG_AMBIENT, PSFLAG2_PRINTABLE, PSTYPE_NONE, NULL, -1, NULL, -1, -1, -1, -1);
				fireslot->endlvframe = (u32)g_Vars.lvframe60 + duration;
				chr->hidden |= CHRHFLAG_FIRESOUNDDONE;

				if (chr);
#endif
			}

			if (withbeam) {
				beam_create(&fireslot->beam, weaponnum, from, to);
			}
		}
	}
}

/**
 * Returns the chr's turn angle difference to 360 degrees, in radians.
 *
 *           z+
 *          (0)
 *           |
 * x+ (90) --+-- (270) x-
 *           |
 *         (180)
 *           z-
 */
f32 chr_get_theta(struct chrdata *chr)
{
	if (chr->aibot) {
		return chr->aibot->lookangle;
	}

	if (chr->model == NULL && chr->prop && chr->prop->type == PROPTYPE_PLAYER) {
		struct player *player = g_Vars.players[playermgr_get_player_num_by_prop(chr->prop)];
		f32 angle = BADDTOR2(360 - player->vv_theta);

		if (angle >= BADDTOR(360)) {
			angle -= BADDTOR(360);
		} else if (angle < 0) {
			angle += BADDTOR(360);
		}

		return angle;
	}

	return model_get_chr_rot_y(chr->model);
}

void chr_set_theta(struct chrdata *chr, f32 angle)
{
	if (chr->aibot) {
		chr->aibot->lookangle = angle;
	} else {
		model_set_chr_rot_y(chr->model, angle);
	}
}

f32 chr_get_rot_y(struct chrdata *chr)
{
	if (chr->aibot) {
		return chr->aibot->roty;
	} else {
		return model_get_chr_rot_y(chr->model);
	}
}

void chr_set_rot_y(struct chrdata *chr, f32 roty)
{
	if (chr->aibot) {
		chr->aibot->roty = roty;
	} else {
		model_set_chr_rot_y(chr->model, roty);
	}
}

f32 chr_get_aimx_angle(struct chrdata *chr)
{
	f32 angle = chr_get_theta(chr) + chr->aimsideback;
	f32 offset = 0;

	if (angle >= BADDTOR(360)) {
		angle -= BADDTOR(360);
	} else if (angle < 0) {
		angle += BADDTOR(360);
	}

	if (chr->aibot) {
		if (chr->aibot->attackanimconfig) {
			// empty
		}
	} else if (chr->actiontype == ACT_ATTACK
			|| chr->actiontype == ACT_ATTACKROLL
			|| chr->actiontype == ACT_BOT_ATTACKSTAND
			|| chr->actiontype == ACT_BOT_ATTACKKNEEL
			|| chr->actiontype == ACT_BOT_ATTACKSTRAFE) {
		offset = chr->act_attack.animcfg->angleoffset;
	} else if (chr->prop->type == PROPTYPE_PLAYER) {
		offset += g_Vars.players[playermgr_get_player_num_by_prop(chr->prop)]->angleoffset;
	}

	if (offset) {
		if (chr->model->anim->flip) {
			offset = BADDTOR(360) - offset;
		}

		angle += offset;

		if (angle >= BADDTOR(360)) {
			angle -= BADDTOR(360);
		} else if (angle < BADDTOR(360)) {
			angle += BADDTOR(360);
		}
	}

	return angle;
}

f32 chr_get_aimy_angle(struct chrdata *chr)
{
	f32 sum = chr->aimuprshoulder + chr->aimupback;

	if (sum < 0) {
		sum += BADDTOR(360);
	}

	return sum;
}

/**
 * Turn the chr slightly towards their target.
 */
s32 chr_turn(struct chrdata *chr, s32 turning, f32 endanimframe, f32 speed, f32 toleranceangle)
{
	if (turning != TURNSTATE_OFF) {
		struct model *model = chr->model;
		f32 curframe = model_get_cur_anim_frame(model);
		u32 stack;
		f32 finalangle = chr_get_theta(chr);
		f32 remainingangle;
		f32 increment = BADDTOR(360) / 100.0f * speed * g_Vars.lvupdate60f * model->anim->playspeed;

		if (chr->aibot) {
			struct prop *target = chr_get_target_prop(chr);
			remainingangle = chr_get_angle_to_pos(chr, &target->pos);
		} else if (chr->actiontype == ACT_ATTACK
				|| chr->actiontype == ACT_BOT_ATTACKSTAND
				|| chr->actiontype == ACT_BOT_ATTACKKNEEL
				|| chr->actiontype == ACT_BOT_ATTACKSTRAFE) {
			remainingangle = chr_get_attack_entity_relative_angle(chr, chr->act_attack.flags, chr->act_attack.entityid);
		} else if (chr->actiontype == ACT_STAND) {
			remainingangle = chr_get_attack_entity_relative_angle(chr, chr->act_stand.flags, chr->act_stand.entityid);
		} else if (chr->actiontype == ACT_THROWGRENADE) {
			remainingangle = chr_get_attack_entity_relative_angle(chr, chr->act_throwgrenade.flags, chr->act_throwgrenade.entityid);
		} else {
			struct prop *target = chr_get_target_prop(chr);
			remainingangle = chr_get_angle_to_pos(chr, &target->pos);
		}

		remainingangle -= toleranceangle;

		if (remainingangle < 0) {
			remainingangle += BADDTOR(360);
		}

		if (increment > remainingangle || BADDTOR(360) - increment < remainingangle) {
			// Close enough to stop
			finalangle += remainingangle;

			if (finalangle >= BADDTOR(360)) {
				finalangle -= BADDTOR(360);
			}

			chr_set_theta(chr, finalangle);
			turning = TURNSTATE_ONTARGET;
		} else if (remainingangle < DTOR(180)) {
			// Turning in one direction
			finalangle += increment;

			if (finalangle >= BADDTOR(360)) {
				finalangle -= BADDTOR(360);
			}

			chr_set_theta(chr, finalangle);
		} else {
			// Turning in the other direction
			finalangle -= increment;

			if (finalangle < 0) {
				finalangle += BADDTOR(360);
			}

			chr_set_theta(chr, finalangle);
		}

		if (curframe >= endanimframe) {
			turning = TURNSTATE_OFF;
		}
	}

	return turning;
}

/**
 * When a chr does an aiming/firing animation, their body's turn angle is the
 * same for the duration of the animation but they can move their firing arm
 * horizontally or vertically. The aimend properties determine this finer arm
 * movement.
 *
 * The aimend properties are updated on every frame while the animation is happening.
 * They are "goal" properties. The aim properties (handled elsewhere) are tweened
 * towards the aimend properties.
 *
 * Return true if the target is within the chr's firing field.
 */
bool chr_calculate_aimend(struct chrdata *chr, struct attackanimconfig *animcfg, bool hasleftgun, bool hasrightgun, f32 arg4)
{
	f32 shootroty = 0.0f;
	f32 aimendsideback = 0.0f;
	u32 flags = ATTACKFLAG_AIMATTARGET;
	s32 entityid = 0;
	bool result = true;

	if (!chr->aibot) {
		if (chr->actiontype == ACT_ATTACK
				|| chr->actiontype == ACT_BOT_ATTACKSTAND
				|| chr->actiontype == ACT_BOT_ATTACKKNEEL
				|| chr->actiontype == ACT_BOT_ATTACKSTRAFE) {
			flags = chr->act_attack.flags;
			entityid = chr->act_attack.entityid;
		} else if (chr->actiontype == ACT_STAND) {
			flags = chr->act_stand.flags;
			entityid = chr->act_stand.entityid;
		}
	}

	if ((flags & ATTACKFLAG_AIMFORWARD) == 0) {
		f32 relaimx;
		f32 relaimy;
		f32 relaimz;
		struct prop *chrprop = chr->prop;
		struct prop *targetprop = chr_get_target_prop(chr);
		f32 sqdist;
		bool holdturn;
		struct coord targetpos;
		RoomNum targetrooms[8];

		relaimx = targetprop->pos.x - chrprop->pos.x;
		relaimy = targetprop->pos.y - chrprop->pos.y;
		relaimz = targetprop->pos.z - chrprop->pos.z;

		sqdist = relaimx * relaimx + relaimy * relaimy + relaimz * relaimz;

		if (chr->aibot) {
			holdturn = false;
		} else if (flags & ATTACKFLAG_AIMATTARGET) {
			if (flags & ATTACKFLAG_DONTTURN) {
				holdturn = true;
			} else {
				holdturn = chr_has_los_to_target(chr);
			}
		} else {
			holdturn = true;
		}

		if ((flags & ATTACKFLAG_AIMATTARGET) && targetprop->type == PROPTYPE_PLAYER) {
			f32 eyeheight = g_Vars.players[playermgr_get_player_num_by_prop(targetprop)]->vv_eyeheight;

			targetpos.x = targetprop->pos.x;
			targetpos.y = targetprop->pos.y;
			targetpos.z = targetprop->pos.z;

			if (chr->aibot) {
				relaimy -= eyeheight * (0.4f + (0.05f * RANDOMFRAC() * arg4));
			} else if (chr->chrflags & CHRCFLAG_LOSEXTRAHEIGHT) {
				if (sqdist < 400.0f * 400.0f) {
					if (chrprop->pos.y < targetpos.y - 2.0f * eyeheight) {
						relaimy -= eyeheight * (0.55f + 0.1f * RANDOMFRAC() * arg4);
					} else if (chrprop->pos.y > targetpos.y - eyeheight * 0.5f) {
						relaimy -= eyeheight * (0.15f + 0.1f * RANDOMFRAC() * arg4);
					} else {
						relaimy = (RANDOMFRAC() * 0.1f * arg4 + 1.0f) * 40.0f;
					}
				} else {
					relaimy += eyeheight * (0.025f - 0.05f * RANDOMFRAC() * arg4);
				}
			} else if (sqdist > 1000.0f * 1000.0f) {
				if ((random() % 3) == 0) {
					relaimy += eyeheight * (0.05f + 0.1f * RANDOMFRAC() * arg4);
				} else {
					relaimy -= eyeheight * (0.05f + 0.55f * RANDOMFRAC() * arg4);
				}
			} else {
				if (chrprop->pos.y < targetpos.y - eyeheight) {
					relaimy -= eyeheight * (0.55f + 0.1f * RANDOMFRAC() * arg4);
				} else if (chrprop->pos.y > targetpos.y - eyeheight * 0.5f) {
					relaimy -= eyeheight * (0.15f + 0.1f * RANDOMFRAC() * arg4);
				} else {
					relaimy = (RANDOMFRAC() * 0.1f * arg4 - 0.05f) * eyeheight;
				}
			}
		} else {
			struct coord chrpos;

			model_get_root_position(chr->model, &chrpos);
			chr_get_attack_entity_pos(chr, flags, entityid, &targetpos, targetrooms);

			relaimx = targetpos.x - chrpos.x;
			relaimy = targetpos.y - chrpos.y;
			relaimz = targetpos.z - chrpos.z;
		}

		if ((flags & ATTACKFLAG_NOVERTICAL) == 0) {
			shootroty = atan2f(relaimy, sqrtf(relaimx * relaimx + relaimz * relaimz));

			if (shootroty >= DTOR(180)) {
				shootroty -= BADDTOR(360);
			}
		}

		if (holdturn) {
			f32 aimangle = chr_get_aimx_angle(chr);
			struct prop *gunprop;
			struct modelnode *posnode;
			struct model *gunmodel;
			struct coord sp118;
			s32 sp114;
			struct modelnode *burstnode;
			struct modelrwdata_chrinfo *chrrwdata;
			Mtxf *sp108;
			Mtxf spc8;
			struct modelrodata_chrgunfire *burstrodata;
			struct coord spb8;
			Mtxf *spb4;
			Mtxf *spb0;
			Mtxf sp70;
			Mtxf *sp6c;
			struct coord sp60;
			struct coord sp54;
			struct coord sp48;
			f32 anglev;

			if (flags & ATTACKFLAG_AIMATTARGET) {
				if (hasrightgun) {
					gunprop = chr_get_held_prop(chr, HAND_RIGHT);
				} else {
					gunprop = chr_get_held_prop(chr, HAND_LEFT);
				}

				if (PLAYERCOUNT() == 1
						&& gunprop
						&& (gunprop->flags & PROPFLAG_ONANYSCREENPREVTICK)
						&& sqdist < 1000.0f * 1000.0f) {
					struct defaultobj *gun = gunprop->obj;
					gunmodel = gun->model;
					sp114 = 0;
					burstnode = model_get_part(gunmodel->definition, MODELPART_CHRGUN_GUNFIRE);

					if (burstnode) {
						sp108 = model_find_node_mtx(gunmodel, burstnode, 0);
						burstrodata = &burstnode->rodata->chrgunfire;
						spb4 = cam0f0b53a4((u8 *)sp108);

						if (spb4) {
							mtx00016798(sp108, &spc8);
							mtx00015be0(spb4, &spc8);

							spb8.x = burstrodata->pos.x;
							spb8.y = burstrodata->pos.y;
							spb8.z = burstrodata->pos.z;

							mtx4_transform_vec_in_place(&spc8, &spb8);

							sp114 = 1;
							sp118.x = spb8.x;
							sp118.y = spb8.y;
							sp118.z = spb8.z;
						}
					} else {
						posnode = model_get_part(gunmodel->definition, MODELPART_CHRGUN_0001);

						if (posnode) {
							spb0 = model_find_node_mtx(gunmodel, posnode, 0);
							sp6c = cam0f0b53a4((u8 *)spb0);

							if (sp6c) {
								mtx00016798(spb0, &sp70);
								mtx00015be0(sp6c, &sp70);

								sp114 = 1;
								sp118.x = sp70.m[3][0];
								sp118.y = sp70.m[3][1];
								sp118.z = sp70.m[3][2];
							}
						}
					}

					if (sp114) {
						sp54.x = sinf(aimangle);
						sp54.y = 0.0f;
						sp54.z = cosf(aimangle);

						sp48.x = chrprop->pos.x - relaimz;
						sp48.y = chrprop->pos.y;
						sp48.z = chrprop->pos.z + relaimx;

						chr_calculate_push_contact_pos(&chrprop->pos, &sp48, &sp118, &sp54, &sp60);

						relaimx = targetpos.x - sp60.x;
						relaimz = targetpos.z - sp60.z;
					}
				}
			}

			anglev = atan2f(relaimx, relaimz);
			aimendsideback = anglev - aimangle;

			if (aimendsideback < 0) {
				aimendsideback += BADDTOR(360);
			}

			chrrwdata = model_get_node_rw_data(chr->model, chr->model->definition->rootnode);

			if (chrrwdata->unk5c > 0.0f) {
				aimendsideback -= chrrwdata->unk5c * chrrwdata->unk58;

				if (aimendsideback < 0.0f) {
					aimendsideback += BADDTOR(360);
				}

				if (aimendsideback >= BADDTOR(360)) {
					aimendsideback -= BADDTOR(360);
				}
			}

			if (!chr->aibot
					&& (flags & ATTACKFLAG_AIMATTARGET)
					&& ((flags & (ATTACKFLAG_AIMONLY | ATTACKFLAG_DONTTURN)) == 0)
					&& targetprop->type == PROPTYPE_PLAYER) {
				if (1);
				aimendsideback += chr_get_aim_limit_angle(sqdist) * 0.5f * sinf((((s32) (g_Vars.lvframe60 * chr->model->anim->playspeed) + chr->chrnum) % 60) * 0.10470308f);

				if (aimendsideback < 0.0f) {
					aimendsideback += BADDTOR(360);
				}

				if (aimendsideback >= BADDTOR(360)) {
					aimendsideback -= BADDTOR(360);
				}
			}

			if (aimendsideback >= DTOR(180)) {
				aimendsideback -= BADDTOR(360);
			}

			aimendsideback += chr->aimsideback;

			if (animcfg) {
				// aimendsideback positive is aiming left
				// aimendsideback negative is aiming right
				if (chr->model->anim->flip) {
					if (aimendsideback < -animcfg->maxleft) {
						aimendsideback = -animcfg->maxleft;
						result = false;
					} else if (aimendsideback > -animcfg->maxright) {
						aimendsideback = -animcfg->maxright;
						result = false;
					}
				} else {
					if (aimendsideback > animcfg->maxleft) {
						aimendsideback = animcfg->maxleft;
						result = false;
					} else if (aimendsideback < animcfg->maxright) { // (maxright is negative)
						aimendsideback = animcfg->maxright;
						result = false;
					}
				}
			}
		}
	}

	chr_calculate_aimend_vertical(chr, animcfg, hasleftgun, hasrightgun, shootroty);

	chr->aimendsideback = aimendsideback;
	chr->aimendcount = 10;

	return result;
}

void chr_calculate_aimend_vertical(struct chrdata *chr, struct attackanimconfig *animcfg, bool hasleftgun, bool hasrightgun, f32 shootroty)
{
	f32 freearmangle = 0;
	f32 backangle = 0;
	f32 gunarmangle = shootroty;

	if (animcfg != NULL) {
		if (shootroty > animcfg->maxup) {
			backangle = shootroty - animcfg->maxup;
			gunarmangle = animcfg->maxup;
		} else if (shootroty < animcfg->maxdown) {
			backangle = shootroty - animcfg->maxdown;
			gunarmangle = animcfg->maxdown;
		}

		if (gunarmangle > 0) {
			freearmangle = animcfg->freearmfracup * gunarmangle;
		} else {
			freearmangle = animcfg->freearmfracdown * gunarmangle;
		}
	}

	if (hasrightgun) {
		chr->aimendrshoulder = gunarmangle;

		if (hasleftgun) {
			chr->aimendlshoulder = gunarmangle;
		} else {
			chr->aimendlshoulder = freearmangle;
		}
	} else {
		chr->aimendrshoulder = freearmangle;
		chr->aimendlshoulder = gunarmangle;
	}

	chr->aimendback = backangle;
}

void chr_reset_aimend(struct chrdata *chr)
{
	chr->aimendcount = 10;
	chr->aimendrshoulder = 0;
	chr->aimendlshoulder = 0;
	chr->aimendback = 0;
	chr->aimendsideback = 0;
}

void chr_set_firing(struct chrdata *chr, s32 hand, bool firing)
{
	struct prop *prop = chr_get_held_prop(chr, hand);

	chr->prop->forcetick = firing ? true : false;

	if (prop) {
		weapon_set_gunfire_visible(prop, firing, chr->prop->rooms[0]);
	}
}

bool chr_is_gunfire_visible(struct chrdata *chr, s32 hand)
{
	struct prop *prop = chr_get_held_prop(chr, hand);

	if (prop) {
		return weapon_is_gunfire_visible(prop);
	}

	return false;
}

void chr_stop_firing(struct chrdata *chr)
{
	u8 race = CHRRACE(chr);

	if (race != RACE_DRCAROLL && race != RACE_EYESPY && chr->aibot == NULL) {
		chr_set_firing(chr, HAND_RIGHT, false);
		chr_set_firing(chr, HAND_LEFT, false);

		chr_reset_aimend(chr);

		chr->fireslots[0] = bgun_free_fireslot(chr->fireslots[0]);
		chr->fireslots[1] = bgun_free_fireslot(chr->fireslots[1]);
	}
}

void chr_set_hand_firing(struct chrdata *chr, s32 hand, bool firing)
{
	if (firing) {
		if (hand == HAND_LEFT) {
			chr->hidden |= CHRHFLAG_FIRINGLEFT;
		} else {
			chr->hidden |= CHRHFLAG_FIRINGRIGHT;
		}
	} else {
		if (hand == HAND_LEFT) {
			chr->hidden &= ~CHRHFLAG_FIRINGLEFT;
		} else {
			chr->hidden &= ~CHRHFLAG_FIRINGRIGHT;
		}
	}

	if (!firing) {
		chr_set_firing(chr, hand, false);
	}
}

f32 chr_get_aim_limit_angle(f32 sqdist)
{
	if (sqdist > 1600 * 1600) {
		return BADDTOR(1.0746268141502f);
	}

	if (sqdist > 800 * 800) {
		return BADDTOR(2.155688477219f);
	}

	if (sqdist > 400 * 400) {
		return BADDTOR(4.2857139836688f);
	}

	if (sqdist > 200 * 200) {
		return BADDTOR(8.5714279673376f);
	}

	return BADDTOR(14.4f);
}

/**
 * Calculate whether a chr's shot hits their target on this tick.
 *
 * Each chr maintains a shotbondsum property which is a float ranging from
 * 0 to 1. Each time this function is called, the value is increased slightly
 * based on the chr's accuracy and other factors. Once it reaches 1, the target
 * is considered to be hit and the value is reset to 0.
 *
 * The function writes to the angleokptr argument if the angle to their target
 * is within range, and writes to the hit argument to indicate if the target is
 * being hit or not.
 */
void chr_calculate_hit(struct chrdata *chr, bool *angleokptr, bool *hit, struct gset *gset)
{
	struct prop *prop;
	struct prop *target;
	f32 xdist;
	f32 ydist;
	f32 zdist;
	f32 angletotarget;
	f32 angleaiming;
	f32 anglediff;
	f32 limitangle;
	bool angleok;
	u32 stack;
	f32 taperdist;
	f32 sqdist;

	taperdist = 300;

	// Check that the chr's aim angle is within an acceptable range to their
	// target.
	prop = chr->prop;
	target = chr_get_target_prop(chr);

	xdist = target->pos.x - prop->pos.x;
	ydist = target->pos.y - prop->pos.y;
	zdist = target->pos.z - prop->pos.z;

	angletotarget = atan2f(xdist, zdist);
	angleaiming = chr_get_aimx_angle(chr);
	anglediff = angletotarget - angleaiming;

	sqdist = xdist * xdist + ydist * ydist + zdist * zdist;
	limitangle = chr_get_aim_limit_angle(sqdist);

	if (anglediff < 0) {
		anglediff += BADDTOR(360);
	}

	angleok = anglediff < limitangle || anglediff > BADDTOR(360) - limitangle;

	*angleokptr = angleok;
	*hit = false;

	// Determine the distance at which accuracy starts to taper off
	switch (gset->weaponnum) {
	case WEAPON_FALCON2:
	case WEAPON_FALCON2_SILENCER:
	case WEAPON_MAULER:
	case WEAPON_PHOENIX:
	case WEAPON_DY357MAGNUM:
	case WEAPON_DY357LX:
	case WEAPON_CMP150:
	case WEAPON_CYCLONE:
	case WEAPON_CALLISTO:
	case WEAPON_RCP120:
	case WEAPON_LAPTOPGUN:
	case WEAPON_DRAGON:
	case WEAPON_K7AVENGER:
	case WEAPON_AR34:
	case WEAPON_SUPERDRAGON:
	case WEAPON_SHOTGUN:
	case WEAPON_REAPER:
	case WEAPON_DEVASTATOR:
	case WEAPON_ROCKETLAUNCHER:
	case WEAPON_SLAYER:
	case WEAPON_COMBATKNIFE:
	case WEAPON_CROSSBOW:
	case WEAPON_TRANQUILIZER:
	case WEAPON_LASER:
	case WEAPON_GRENADE:
	case WEAPON_NBOMB:
	case WEAPON_TIMEDMINE:
	case WEAPON_PROXIMITYMINE:
	case WEAPON_REMOTEMINE:
	case WEAPON_COMBATBOOST:
	case WEAPON_PP9I:
	case WEAPON_CC13:
	case WEAPON_KL01313:
	case WEAPON_KF7SPECIAL:
	case WEAPON_ZZT:
	case WEAPON_DMC:
	case WEAPON_AR53:
	case WEAPON_RCP45:
	case WEAPON_PSYCHOSISGUN:
	default:
		// Use default distance (300)
		break;
	case WEAPON_FALCON2_SCOPE:
	case WEAPON_MAGSEC4:
		taperdist = 600;
		break;
	case WEAPON_SNIPERRIFLE:
	case WEAPON_FARSIGHT:
		taperdist = 1200;
		break;
	}

	if (angleok) {
		f32 dist = sqrtf(xdist * xdist + ydist * ydist + zdist * zdist);
		f32 accuracy = 0.16f;

		// Decrease accuracy if further than taperdist
		if (dist > taperdist) {
			accuracy *= taperdist / dist;
		}

		// Scale accuracy up or down based on chr's accuracyrating
		if (chr->accuracyrating > 0) {
			accuracy *= 1 + chr->accuracyrating * 0.1f;
		} else if (chr->accuracyrating < 0) {
			if (chr->accuracyrating <= -100) {
				accuracy = 0;
			} else {
				accuracy *= (chr->accuracyrating + 100) * 0.01f;
			}
		}

		// Apply PD mode enemy accuracy setting (default 1 which is no op)
		if (pdmode_get_enemy_accuracy() <= 1) {
			accuracy *= pdmode_get_enemy_accuracy();
		} else {
			accuracy *= 9 / (10.001f - pdmode_get_enemy_accuracy());
		}

		// Apply difficulty multiplier (solo A = 0.6, SA = 0.8, PA = 1.175)
		accuracy *= g_EnemyAccuracyScale;

		// If the weapon fires more than once per tick, double the value to
		// account for it. No weapons meet this criteria, however.
		if (weapon_get_num_ticks_per_shot(gset->weaponnum, gset->weaponfunc) <= 0) {
			accuracy += accuracy;
		}

		// Shotgun doubles the value due to more bullets
		if (gset->weaponnum == WEAPON_SHOTGUN) {
			accuracy += accuracy;
		}

		chr->shotbondsum += accuracy;

		if (chr->hidden & CHRHFLAG_PERFECTACCURACY) {
			chr->shotbondsum += 1.1f;
		}

		if (chr->shotbondsum >= 1) {
			*hit = true;
			chr->shotbondsum = 0;
		}
	}
}

/**
 * If the chr's gun is on screen, populate gunpos with the world coordinates of
 * their gun and return true.
 *
 * If the chr's gun is off screen, return false without populating gunpos.
 */
bool chr_get_gun_pos(struct chrdata *chr, s32 handnum, struct coord *gunpos)
{
	struct prop *weaponprop = chr_get_held_prop(chr, handnum);
	struct defaultobj *obj;
	struct model *model;
	bool result = false;
	struct modelnode *part0;
	struct modelnode *part1;
	Mtxf *spac;
	Mtxf sp6c;
	struct modelrodata_chrgunfire *rodata;
	Mtxf *sp64;
	Mtxf sp24;

	if (weaponprop) {
		obj = weaponprop->obj;
		model = obj->model;

		if ((chr->prop->flags & PROPFLAG_ONTHISSCREENTHISTICK) && (weaponprop->flags & PROPFLAG_ONTHISSCREENTHISTICK)) {
			if ((part0 = model_get_part(model->definition, MODELPART_0000))) {
				spac = model_find_node_mtx(model, part0, 0);
				rodata = &part0->rodata->chrgunfire;

				gunpos->x = rodata->pos.x;
				gunpos->y = rodata->pos.y;
				gunpos->z = rodata->pos.z;

				mtx00015be4(cam_get_projection_mtxf(), spac, &sp6c);
				mtx4_transform_vec_in_place(&sp6c, gunpos);
				result = true;
			} else if ((part1 = model_get_part(model->definition, MODELPART_0001))) {
				sp64 = model_find_node_mtx(model, part1, 0);

				mtx00015be4(cam_get_projection_mtxf(), sp64, &sp24);

				gunpos->x = sp24.m[3][0];
				gunpos->y = sp24.m[3][1];
				gunpos->z = sp24.m[3][2];

				result = true;
			}
		}
	}

	return result;
}

/**
 * For a shielded chr, determine which model node has been shot and populate the
 * last 4 pointer arguments with information about the hit.
 *
 * Shield hits are calculated using the bounding box of each body part.
 * This gives a bit of padding around the chr which can register as a hit.
 *
 * nodeptr    - will be populated with a pointer to the bbox model node.
 * hitpartptr - will be populated with the HITPART constant value.
 * modelptr   - will be populated with a pointer to the chr's model.
 * sideptr    - will be populated with an index in the range 0-5 which
 *              represents which side of the node's bounding box was hit.
 */
void chr_calculate_shield_hit(struct chrdata *chr, struct coord *pos, struct coord *vector,
		struct modelnode **nodeptr, s32 *hitpartptr, struct model **modelptr, s32 *sideptr)
{
	u32 stack1;
	struct prop *prop = chr->prop;
	bool done = false;
	bool isdifferentmtx;
	struct coord sp124;
	struct coord sp118;
	s32 i;
	struct modelnode *bestnode;
	u32 stack2[2];
	Mtxf spc8;
	f32 bestvolume;
	Mtxf *worldtoscreenmtx;
	struct modelnode *node;
	f32 x;
	f32 y;
	f32 z;
	f32 volume;
	Mtxf *mtxptr1;
	Mtxf *mtxptr2;
	f32 sides[6];
	u32 stack3;

	if (prop->type != PROPTYPE_PLAYER || g_Vars.normmplayerisrunning || chr_get_shield(chr) > 0) {
		if (prop->flags & (PROPFLAG_ONTHISSCREENTHISTICK | PROPFLAG_ONANYSCREENTHISTICK | PROPFLAG_ONANYSCREENPREVTICK)) {
			bestnode = NULL;
			bestvolume = MAXFLOAT;
			worldtoscreenmtx = cam0f0b5050((u8 *)chr->model->matrices);

			if (worldtoscreenmtx) {
				mtx4_transform_vec(worldtoscreenmtx, pos, &sp124);
				mtx4_rotate_vec(worldtoscreenmtx, vector, &sp118);

				isdifferentmtx = (cam_get_world_to_screen_mtxf() != worldtoscreenmtx);
				node = chr->model->definition->rootnode;

				while (node) {
					if ((node->type & 0xff) == MODELNODETYPE_BBOX) {
						mtxptr1 = model_find_node_mtx(chr->model, node, 0);

						if (isdifferentmtx) {
							mtx00016798(mtxptr1, &spc8);
							mtxptr1 = &spc8;
						}

						x = mtxptr1->m[3][0] - sp124.f[0];
						y = mtxptr1->m[3][1] - sp124.f[1];
						z = mtxptr1->m[3][2] - sp124.f[2];

						volume = x * x + y * y + z * z;

						if (volume < bestvolume) {
							bestvolume = volume;
							bestnode = node;
						}
					}

					// Iterate all nodes recursively except headspot's chidlren
					if (node->child && (node->type & 0xff) != MODELNODETYPE_HEADSPOT) {
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

				if (bestvolume);

#if VERSION >= VERSION_NTSC_1_0
				if (bestnode != NULL)
#endif
				{
					Mtxf sp48;
					struct modelrodata_bbox *rodata = &bestnode->rodata->bbox;

					*hitpartptr = rodata->hitpart;
					*nodeptr = bestnode;
					*modelptr = chr->model;
					*sideptr = 0;

					mtxptr2 = model_find_node_mtx(chr->model, bestnode, 0);

					if (isdifferentmtx) {
						mtx00016798(mtxptr2, &sp48);
						mtxptr2 = &sp48;
					}

					bestvolume = -2;

					x = (sp118.f[0] * mtxptr2->m[0][0]) + (sp118.f[1] * mtxptr2->m[0][1]) + (sp118.f[2] * mtxptr2->m[0][2]);
					y = (sp118.f[0] * mtxptr2->m[1][0]) + (sp118.f[1] * mtxptr2->m[1][1]) + (sp118.f[2] * mtxptr2->m[1][2]);
					z = (sp118.f[0] * mtxptr2->m[2][0]) + (sp118.f[1] * mtxptr2->m[2][1]) + (sp118.f[2] * mtxptr2->m[2][2]);

					sides[0] = x;
					sides[1] = -x;
					sides[2] = y;
					sides[3] = -y;
					sides[4] = z;
					sides[5] = -z;

					for (i = 0; i < ARRAYCOUNT(sides); i++) {
						if (sides[i] > bestvolume) {
							bestvolume = sides[i];
							*sideptr = i;
						}
					}

					done = true;
				}
			}
		}

		// If no node was found above, search the model for the torso bbox
		// and return that.
		if (!done) {
			node = chr->model->definition->rootnode;

			while (node) {

				if ((node->type & 0xff) == MODELNODETYPE_BBOX) {
					struct modelrodata_bbox *rodata = &node->rodata->bbox;

					if (rodata->hitpart == HITPART_TORSO) {
						*hitpartptr = rodata->hitpart;
						*nodeptr = node;
						*modelptr = chr->model;
						*sideptr = 0;
						return;
					}
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
	}
}

/**
 * Calculates the trajectory for thrown items.
 */
void chr_calculate_trajectory(struct coord *frompos, f32 arg1, struct coord *aimpos, struct coord *arg3)
{
	f32 xvel;
	f32 yvel;
	f32 zvel;
	f32 latvel;
	f32 vel;
	f32 sp40;
	f32 sp3c;
	f32 sp38;
	f32 sp30;
	f32 sp2c;
	f32 sp24;
	f32 sp28;
	f32 sp20;

	arg1 *= 0.59999999f;

	xvel = (aimpos->x - frompos->x) * 0.01f;
	yvel = (aimpos->y - frompos->y) * 0.01f;
	zvel = (aimpos->z - frompos->z) * 0.01f;

	vel = sqrtf(xvel * xvel + yvel * yvel + zvel * zvel);
	latvel = sqrtf(xvel * xvel + zvel * zvel);
	sp38 = latvel / vel;
	sp40 = acosf(sp38);

	if (yvel < 0) {
		sp40 = -sp40;
	}

	sp2c = (vel * 9.81f * sp38 * sp38) / (arg1 * arg1) + yvel / vel;

	if (sp2c < -1) {
		sp2c = -1;
	} else if (sp2c > 1) {
		sp2c = 1;
	}

	sp3c = (asinf(sp2c) - sp40) * 0.5f + sp40;
	sp28 = cosf(sp3c);
	sp30 = sinf(sp3c);

	arg3->x = xvel / latvel * sp28;
	arg3->y = sp30;
	arg3->z = zvel / latvel * sp28;
}

const char var7f1a8ae4[] = "aimadjust=%d";

/**
 * Fire the chr's gun, check what was hit and do all the appropriate things
 * such as dealing damage, creating beams and sparks and playing sounds.
 *
 * This should be called on every frame while the chr is shooting.
 * The function takes care of the gun's fire rate.
 */
void chr_shoot(struct chrdata *chr, s32 handnum)
{
	struct prop *chrprop = chr->prop;
	struct prop *gunprop;
	u8 isaibot = false;
	u8 normalshoot = true;

	if (chr->aibot) {
		isaibot = true;
	}

	gunprop = chr_get_held_prop(chr, handnum);

	if (gunprop) {
		bool firingthisframe = false;
		struct weaponobj *weapon = gunprop->weapon;
		struct gset gset;
		struct prop *targetprop = chr_get_target_prop(chr);
		u32 attackflags;
		bool shotdue;
		bool makebeam;
		struct coord gunpos;
		RoomNum gunrooms[8];
		struct coord hitpos;
		bool hitsomething;
		RoomNum hitrooms[8];
		bool queriedhitrooms;
		s32 tickspershot;

		gset = weapon->gset;
		attackflags = ATTACKFLAG_AIMATTARGET;

		if (chr->actiontype == ACT_ATTACK
				|| chr->actiontype == ACT_BOT_ATTACKSTAND
				|| chr->actiontype == ACT_BOT_ATTACKKNEEL
				|| chr->actiontype == ACT_BOT_ATTACKSTRAFE) {
			attackflags = chr->act_attack.flags;
		}

		shotdue = false;
		makebeam = false;
		hitsomething = false;
		queriedhitrooms = false;

		// Most guns can fire at most once every few ticks - even automatics.
		// The chr's firecount property tracks how many ticks have elapsed since
		// the last bullet, which is used to determine if another bullet should
		// be discharged on this tick.
		tickspershot = weapon_get_num_ticks_per_shot(gset.weaponnum, gset.weaponfunc);

		if (tickspershot <= 0) {
			shotdue = true;
			makebeam = true;
		} else {
			if (chr->aibot
					&& chr->aibot->weaponnum == WEAPON_REAPER
					&& chr->aibot->gunfunc == FUNC_PRIMARY) {
				f32 sp208 = (TICKS(90) - chr->aibot->reaperspeed[handnum]) * (1.0f / TICKS(18.0f));
				tickspershot *= 1 + sp208;
			}

			chr->firecount[handnum] += g_Vars.lvupdate60;

			if (chr->firecount[handnum] >= tickspershot) {
				chr->firecount[handnum] = 0;
				chr->unk32c_12 ^= 1 << handnum;

				shotdue = true;

				if ((chr->unk32c_12 & (1 << handnum)) || gset.weaponnum == WEAPON_LASER) {
					makebeam = true;
				}

				if (chr->actiontype == ACT_ATTACK) {
					if (model_get_anim_num(chr->model) == ANIM_SNIPING_ONGROUND) {
						chr->act_attack.numshots++;
					}
				}
			}
		}

		if (shotdue) {
			f32 roty = chr_get_aimx_angle(chr);
			f32 rotx = chr_get_aimy_angle(chr);
			bool extracdtypes = isaibot ? CDTYPE_PLAYERS : 0;

			firingthisframe = true;

			if (!chr_get_gun_pos(chr, handnum, &gunpos)) {
				// Gun is off screen - use a quick but inexact calculation
				gunpos.x = chrprop->pos.x;
				gunpos.y = chrprop->pos.y + 30;
				gunpos.z = chrprop->pos.z;

				if (handnum == HAND_LEFT) {
					gunpos.x += cosf(roty) * 10.0f;
					gunpos.z += -sinf(roty) * 10.0f;
				} else {
					gunpos.x += -cosf(roty) * 10.0f;
					gunpos.z += sinf(roty) * 10.0f;
				}
			}

			// Check that the chr isn't clipping their gun through anything such
			// as another chr or a closed door. If they are, the shot won't be
			// taken because that wouldn't be fair.
			// How nice of the developers to check for this!
			chr_set_perim_enabled(chr, false);

			if (cd_test_los10(&chrprop->pos, chrprop->rooms, &gunpos, gunrooms,
						CDTYPE_DOORS | CDTYPE_CHRS | CDTYPE_BG | CDTYPE_DOORSWITHOUTFLAG | extracdtypes,
						GEOFLAG_BLOCK_SHOOT) == CDRESULT_COLLISION) {
				firingthisframe = false;
			}

			chr_set_perim_enabled(chr, true);

			if (firingthisframe) {
				bool angleok = false;
				bool hitplayer = false;
				bool effective = true;
				s32 zero = 0;
				struct coord vector;
				f32 xdiff;
				f32 ydiff;
				f32 zdiff;
				f32 sqshotdist;
				struct prop *hitprop = NULL;
				u32 cdtypes = isaibot
					? CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_CHRS | CDTYPE_PATHBLOCKER | CDTYPE_BG | CDTYPE_DOORSWITHOUTFLAG | CDTYPE_PLAYERS
					: CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_CHRS | CDTYPE_PATHBLOCKER | CDTYPE_BG | CDTYPE_DOORSWITHOUTFLAG;
				u32 stack;
				bool isshootingeyespy = CHRRACE(targetprop->chr) == RACE_EYESPY && chr_get_distance_to_target(chr) > 150;
				bool fudgeforeyespy = false;

				if (isshootingeyespy) {
					vector.x = targetprop->pos.x - gunpos.x;
					vector.y = targetprop->pos.y - gunpos.y;
					vector.z = targetprop->pos.z - gunpos.z;

					guNormalize(&vector.x, &vector.y, &vector.z);
					prop_set_perim_enabled(targetprop, true);
				} else {
					vector.x = cosf(rotx) * sinf(roty);
					vector.y = sinf(rotx);
					vector.z = cosf(rotx) * cosf(roty);

					if (isaibot) {
						bgun_calculate_bot_shot_spread(&vector,
								chr->aibot->weaponnum, chr->aibot->gunfunc,
								chr->aibot->burstsdone[handnum], bot_guess_crouch_pos(chr),
								chr->weapons_held[0] && chr->weapons_held[1]);
					}
				}

				// Handle Farsight shots by aibots specially
				// because they can shoot through walls.
				if (chr->aibot && gset.weaponnum == WEAPON_FARSIGHT && !chr->aibot->targetinsight) {
					makebeam = true;

					// This function can never return 2 though...
					if (botact_shoot_farsight(chr + zero, 0, &vector, &gunpos) == 2) {
						normalshoot = random() % 255 > 200;
					}
				}

				// Check if the shot would hit anything
				hitpos.x = gunpos.x + vector.x * 65536.0f;
				hitpos.y = gunpos.y + vector.y * 65536.0f;
				hitpos.z = gunpos.z + vector.z * 65536.0f;

				chr_set_perim_enabled(chr, false);

				if (isaibot) {
					g_Vars.useperimshoot = true;
				}

				if (cd_exam_los08(&gunpos, gunrooms, &hitpos, cdtypes, GEOFLAG_BLOCK_SHOOT) == CDRESULT_COLLISION) {
					hitsomething = true;
#if VERSION >= VERSION_JPN_FINAL
					cd_get_pos(&hitpos, 12080, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
					cd_get_pos(&hitpos, 12077, "chr/chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
					cd_get_pos(&hitpos, 12072, "chraction.c");
#else
					cd_get_pos(&hitpos, 12086, "chraction.c");
#endif
					hitprop = cd_get_obstacle_prop();
				}

				chr_set_perim_enabled(chr, true);

				if (isaibot) {
					g_Vars.useperimshoot = false;
				}

				// Eyespy is small and hard to hit, so make it a 50/50 chance
				if (hitprop == NULL && isshootingeyespy) {
					fudgeforeyespy = random() % 100 > 50;

					if (fudgeforeyespy) {
						hitprop = targetprop;

						hitpos.x = hitprop->pos.x;
						hitpos.y = hitprop->pos.y;
						hitpos.z = hitprop->pos.z;
					}
				}

				xdiff = hitpos.x - gunpos.x;
				ydiff = hitpos.y - gunpos.y;
				zdiff = hitpos.z - gunpos.z;

				sqshotdist = xdiff * xdiff + ydiff * ydiff + zdiff * zdiff;

				// Handle projectile launchers specially
				if (gset.weaponnum == WEAPON_ROCKETLAUNCHER
						|| gset.weaponnum == WEAPON_SLAYER
						|| (gset.weaponnum == WEAPON_SUPERDRAGON && gset.weaponfunc == FUNC_SECONDARY)
						|| gset.weaponnum == WEAPON_DEVASTATOR
						|| gset.weaponnum == WEAPON_CROSSBOW
						|| gset.weaponnum == WEAPON_KINGSCEPTRE) {
					makebeam = false;

					// Solo chrs won't fire their projectile weapon
					// in less than 4 metres of space
					if (isaibot || sqshotdist > 400.0f * 400.0f) {
						struct weaponobj *projectileobj;
						Mtxf identmtx;
						struct coord sp16c;
						f32 sp168;
						struct coord sp15c;
						Mtxf projectilemtx;
						Mtxf yrotmtx;
						struct weapondef *weapondef = gset_get_weapondef(gset.weaponnum);
						struct funcdef_shootprojectile *func = weapondef->functions[gset.weaponfunc];

						// Handle creating the projectile
						if (gset.weaponnum == WEAPON_ROCKETLAUNCHER
								|| gset.weaponnum == WEAPON_KINGSCEPTRE
								|| gset.weaponnum == WEAPON_SLAYER) {
							s32 rocketweaponnum = WEAPON_ROCKET;

							if (func->base.base.flags & FUNCFLAG_HOMINGROCKET) {
								rocketweaponnum = WEAPON_HOMINGROCKET;
							}

							projectileobj = weapon_create_projectile_from_weapon_num(func->projectilemodelnum, rocketweaponnum, chr);
						} else if (gset.weaponnum == WEAPON_CROSSBOW) {
							projectileobj = weapon_create_projectile_from_weapon_num(func->projectilemodelnum, WEAPON_BOLT, chr);

							if (projectileobj) {
								projectileobj->gunfunc = gset.weaponfunc;
							}
						} else if (gset.weaponnum == WEAPON_DEVASTATOR) {
							projectileobj = weapon_create_projectile_from_weapon_num(func->projectilemodelnum, WEAPON_GRENADEROUND, chr);

							if (projectileobj) {
								projectileobj->gunfunc = gset.weaponfunc;
							}
						} else if (gset.weaponnum == WEAPON_SUPERDRAGON) {
							projectileobj = weapon_create_projectile_from_weapon_num(func->projectilemodelnum, WEAPON_GRENADEROUND, chr);

							if (projectileobj) {
								projectileobj->gunfunc = FUNC_2;
							}
						} else {
							// Unreachable
							projectileobj = weapon_create_projectile_from_gset(func->projectilemodelnum, &gset, g_Vars.currentplayer->prop->chr);
						}

						if (projectileobj) {
							f32 spcc;

							sp168 = func->speed * (1.0f / 0.6f) / 60.0f;
							spcc = func->traveldist * (1.0f / 0.6f);

							// AI bots are a bit smarter than solo chrs
							// with regard to how they aim their projectiles
							if (isaibot && chr_is_target_in_fov(chr, 30, 0)) {
								bool hasaimpos = false;
								u32 stack;
								struct coord aimpos;

								if (gset.weaponfunc == FUNC_PRIMARY &&
										(gset.weaponnum == WEAPON_ROCKETLAUNCHER
										 || gset.weaponnum == WEAPON_KINGSCEPTRE
										 || gset.weaponnum == WEAPON_SLAYER)) {
									if (targetprop->type == PROPTYPE_CHR || targetprop->type == PROPTYPE_PLAYER) {
										// Rockets - aim at target's feet
										aimpos.x = targetprop->pos.x;
										aimpos.y = targetprop->chr->manground;
										aimpos.z = targetprop->pos.z;

										vector.x = aimpos.x - gunpos.x;
										vector.y = aimpos.y - gunpos.y;
										vector.z = aimpos.z - gunpos.z;

										guNormalize(&vector.x, &vector.y, &vector.z);
										hasaimpos = true;
									}
								} else if ((gset.weaponnum == WEAPON_DEVASTATOR && gset.weaponfunc == FUNC_PRIMARY)
										|| gset.weaponnum == WEAPON_SUPERDRAGON) {
									if (targetprop->type == PROPTYPE_CHR || targetprop->type == PROPTYPE_PLAYER) {
										// Grenades - aim at target's feet
										aimpos.x = targetprop->pos.x;
										aimpos.y = targetprop->chr->manground;
										aimpos.z = targetprop->pos.z;

										chr_calculate_trajectory(&gunpos, spcc, &aimpos, &vector);
										hasaimpos = true;
									}
								} else if ((gset.weaponnum == WEAPON_DEVASTATOR && gset.weaponfunc == FUNC_SECONDARY)
										|| gset.weaponnum == WEAPON_CROSSBOW) {
									// Wall hugger grenade or crossbow - aim at target directly
									aimpos.x = targetprop->pos.x;
									aimpos.y = targetprop->pos.y;
									aimpos.z = targetprop->pos.z;

									if (targetprop->type == PROPTYPE_PLAYER) {
										aimpos.y -= 25;
									}

									chr_calculate_trajectory(&gunpos, spcc, &aimpos, &vector);
									hasaimpos = true;
								}

								if (hasaimpos) {
									f32 angle = chr_get_angle_to_pos(chr, &aimpos);
									f32 cos = cosf(angle);
									f32 sin = sinf(angle);
									f32 x = vector.f[0];
									f32 z = vector.f[2];
									u32 stack;

									vector.x = sin * z + cos * x;
									vector.z = cos * z - sin * x;
								}
							}

							// Calculate and projectile's matrix,
							// spawn position and speed
							mtx4_load_identity(&identmtx);
							mtx4_load_x_rotation(rotx, &projectilemtx);
							mtx4_load_y_rotation(roty, &yrotmtx);
							mtx00015be0(&yrotmtx, &projectilemtx);

							sp15c.x = vector.x * sp168;
							sp15c.y = vector.y * sp168;
							sp15c.z = vector.z * sp168;

							sp16c.x = sp15c.f[0] * g_Vars.lvupdate60freal + vector.f[0] * spcc;
							sp16c.y = sp15c.f[1] * g_Vars.lvupdate60freal + vector.f[1] * spcc;
							sp16c.z = sp15c.f[2] * g_Vars.lvupdate60freal + vector.f[2] * spcc;

							projectileobj->timer240 = func->timer60;

							if (projectileobj->timer240 != -1) {
#if PAL
								projectileobj->timer240 = projectileobj->timer240 * 200 / 60;
#else
								projectileobj->timer240 *= 4;
#endif
							}

							bgun_configure_projectile(&projectileobj->base, &gunpos, gunrooms, &projectilemtx, &sp16c, &identmtx, chrprop, &gunpos);

							if (projectileobj->base.hidden & OBJHFLAG_PROJECTILE) {
								if (func->base.base.flags & FUNCFLAG_PROJECTILE_LIGHTWEIGHT) {
									projectileobj->base.projectile->flags |= PROJECTILEFLAG_LIGHTWEIGHT;
								} else if (func->base.base.flags & FUNCFLAG_PROJECTILE_POWERED) {
									projectileobj->base.projectile->flags |= PROJECTILEFLAG_POWERED;
								}

								projectileobj->base.projectile->accel.x = sp15c.x;
								projectileobj->base.projectile->accel.y = sp15c.y;
								projectileobj->base.projectile->accel.z = sp15c.z;

								projectileobj->base.projectile->pickuptimer240 = TICKS(240);
								projectileobj->base.projectile->hitspeedpreservationfrac = func->hitspeedpreservationfrac;
								projectileobj->base.projectile->speeddecel = func->speeddecel * (1.0f / 0.6f);

								projectileobj->base.projectile->targetprop = chr_get_target_prop(chr);

								// Play sound
								if (func->soundnum > 0) {
									ps_create(NULL, projectileobj->base.prop, func->soundnum, -1,
											-1, 0, 0, PSTYPE_NONE, NULL, -1, NULL, -1, -1, -1, -1);
								}
							}
						}
					} else {
						firingthisframe = false;
					}

					normalshoot = false;
				} else if (gset.weaponnum == WEAPON_MAULER && isaibot && gset.weaponfunc == FUNC_SECONDARY) {
					gset.maulercharge = (s32) (chr->aibot->maulercharge[handnum] * 10.0f);
					chr->aibot->maulercharge[handnum] = 0;
				}

				if (normalshoot) {
					if (!isaibot) {
						if ((attackflags & ATTACKFLAG_AIMATTARGET)
								&& targetprop->type == PROPTYPE_PLAYER
								&& chr_has_los_to_attack_target(chr, &gunpos, gunrooms, false)
#if VERSION >= VERSION_NTSC_1_0
								&& chr_compare_teams(targetprop->chr, chr, COMPARE_ENEMIES)
#endif
								) {
							// Solo chr shooting at a player
							xdiff = targetprop->pos.x - gunpos.x - vector.x * 15.0f;
							ydiff = targetprop->pos.y - gunpos.y - vector.y * 15.0f;
							zdiff = targetprop->pos.z - gunpos.z - vector.z * 15.0f;

							if (xdiff * xdiff + ydiff * ydiff + zdiff * zdiff <= sqshotdist) {
								// Player has a chance of being hit
								chr_calculate_hit(chr, &angleok, &hitplayer, &gset);

								// If the player was hit then turn off effective
								// (There's no need to check other props for
								// hits later on in this function)
								effective = !hitplayer;

								if (angleok
										&& (chr->actiontype == ACT_ATTACK
										 || chr->actiontype == ACT_ATTACKROLL
										 || chr->actiontype == ACT_BOT_ATTACKSTAND
										 || chr->actiontype == ACT_BOT_ATTACKKNEEL
										 || chr->actiontype == ACT_BOT_ATTACKSTRAFE)) {
									chr->act_attack.lastontarget60 = g_Vars.lvframe60;
								}
							}
						} else {
							// Solo chr shooting at something else
							if (chr->actiontype == ACT_ATTACK
									|| chr->actiontype == ACT_ATTACKROLL
									|| chr->actiontype == ACT_BOT_ATTACKSTAND
									|| chr->actiontype == ACT_BOT_ATTACKKNEEL
									|| chr->actiontype == ACT_BOT_ATTACKSTRAFE) {
								chr->act_attack.lastontarget60 = g_Vars.lvframe60;
							}
						}

						if (hitplayer) {
							f32 damage = gset_get_damage(&gset);
							struct modelnode *node = NULL;
							struct model *model = NULL;
							s32 side = -1;
							s32 hitpart = HITPART_GENERAL;
							struct chrdata *targetchr = targetprop->chr;

							hitpos.x = targetprop->pos.x;
							hitpos.y = targetprop->pos.y;
							hitpos.z = targetprop->pos.z;

							if (random() % 2) {
								hitpos.y += 2 + random() % 10;
							} else {
								hitpos.y -= 2 + random() % 10;
							}

							bgun_play_prop_hit_sound(&gset, targetprop, -1);

							if (targetchr->model && chr_get_shield(targetchr) > 0) {
								chr_calculate_shield_hit(targetchr, &hitpos, &vector, &node, &hitpart, &model, &side);
							}

							chr_damage_by_impact(targetchr, damage, &vector, &gset, chr->prop, HITPART_GENERAL, targetprop, node, model, side, NULL);
						} else if ((hitprop == NULL || (hitprop->type != PROPTYPE_CHR && hitprop->type != PROPTYPE_PLAYER))
								&& sqshotdist < 100.0f * 100.0f) {
							// Hit the background or something other than a
							// player or chr, and the shot distance was less
							// than 1 metre. Don't bother applying damage etc.
							effective = false;
						}
					}

					if (effective) {
						if (hitprop) {
							if (hitprop->type == PROPTYPE_PLAYER || hitprop->type == PROPTYPE_CHR) {
								// Hit a player or chr other than the one they
								// were aiming for
								if (isaibot
										|| fudgeforeyespy
										|| ((chr->chrflags & CHRCFLAG_NOFRIENDLYFIRE) && chr_compare_teams(hitprop->chr, chr, COMPARE_ENEMIES))) {
									struct modelnode *node = NULL;
									struct model *model = NULL;
									s32 side = -1;
									s32 hitpart = HITPART_GENERAL;
									f32 damage = gset_get_damage(&gset);
									struct chrdata *hitchr = hitprop->chr;

									bgun_play_prop_hit_sound(&gset, hitprop, -1);

									if (hitchr->model && chr_get_shield(hitchr) > 0) {
										chr_calculate_shield_hit(hitchr, &hitpos, &vector, &node, &hitpart, &model, &side);
									}

									chr_emit_sparks(hitchr, hitprop, hitpart, &hitpos, &vector, chr);
									chr_damage_by_impact(hitchr, damage, &vector, &gset, chr->prop, HITPART_GENERAL, hitprop, node, model, side, NULL);
								} else {
									makebeam = false;
									firingthisframe = false;
								}
							} else if (hitprop->type == PROPTYPE_OBJ
									|| hitprop->type == PROPTYPE_WEAPON
									|| hitprop->type == PROPTYPE_DOOR) {
								// Hit an object
								struct defaultobj *hitobj = hitprop->obj;
								s32 playernum = -1;

								if (g_Vars.mplayerisrunning) {
									playernum = mp_chr_to_chrindex(chr);
								}

								bgun_play_prop_hit_sound(&gset, hitprop, -1);
								los_find_final_room_exhaustive(&gunpos, gunrooms, &hitpos, hitrooms);
								queriedhitrooms = true;

								if (chr_is_using_paintball(chr)) {
									sparks_create(hitrooms[0], hitprop, &hitpos, NULL, NULL, SPARKTYPE_PAINT);
								} else {
									sparks_create(hitrooms[0], hitprop, &hitpos, NULL, NULL, SPARKTYPE_DEFAULT);
								}

								if (g_MissionConfig.iscoop && chr->team == TEAM_ALLY
										&& (hitobj->flags2 & OBJFLAG2_IMMUNETOANTI)) {
									// Co-op can't damage mission critical objects
								} else {
									obj_damage_by_gunfire(hitobj, gset_get_damage(&gset), &hitpos, gset.weaponnum, playernum);
								}
							}
						} else if (hitsomething) {
							// Hit the background
							los_find_final_room_exhaustive(&gunpos, gunrooms, &hitpos, hitrooms);
							queriedhitrooms = true;
							bgun_play_bg_hit_sound(&gset, &hitpos, -1, hitrooms);

							if (chr_is_using_paintball(chr)) {
								sparks_create(hitrooms[0], 0, &hitpos, NULL, NULL, SPARKTYPE_PAINT);
							} else {
								sparks_create(hitrooms[0], 0, &hitpos, NULL, NULL, SPARKTYPE_DEFAULT);
							}
						}

						// Create explosion if using Phoenix
						if (gset.weaponnum == WEAPON_PHOENIX && gset.weaponfunc == FUNC_SECONDARY) {
							s32 playernum = chr->aibot ? mp_chr_to_chrindex(chr) : g_Vars.currentplayernum;

							if (!queriedhitrooms) {
								los_find_final_room_exhaustive(&gunpos, gunrooms, &hitpos, hitrooms);
							}

							explosion_create_simple(NULL, &hitpos, hitrooms, EXPLOSIONTYPE_PHOENIX, playernum);
						}
					}
				}

				if (isshootingeyespy) {
					prop_set_perim_enabled(targetprop, false);
				}
			}
		}

		if (makebeam) {
			switch (gset.weaponnum) {
			case WEAPON_FALCON2:
			case WEAPON_FALCON2_SILENCER:
			case WEAPON_FALCON2_SCOPE:
			case WEAPON_MAGSEC4:
			case WEAPON_MAULER:
			case WEAPON_PHOENIX:
			case WEAPON_DY357MAGNUM:
			case WEAPON_DY357LX:
			case WEAPON_CMP150:
			case WEAPON_CYCLONE:
			case WEAPON_CALLISTO:
			case WEAPON_RCP120:
			case WEAPON_LAPTOPGUN:
			case WEAPON_DRAGON:
			case WEAPON_K7AVENGER:
			case WEAPON_AR34:
			case WEAPON_SUPERDRAGON:
			case WEAPON_REAPER:
			case WEAPON_SNIPERRIFLE:
			case WEAPON_FARSIGHT:
			case WEAPON_TRANQUILIZER:
			case WEAPON_LASER:
			case WEAPON_PP9I:
			case WEAPON_CC13:
			case WEAPON_KL01313:
			case WEAPON_KF7SPECIAL:
			case WEAPON_ZZT:
			case WEAPON_DMC:
			case WEAPON_AR53:
			case WEAPON_RCP45:
				makebeam = true;
				break;
			default:
				makebeam = false;
				break;
			}
		}

		// osSyncPrintf is defined to nothing, so these print statements
		// get optimised out but leave their strings in rodata.
		// The on/off strings are surely used in a ternary statement in the
		// previous string, but making this a ternary creates a mismatch.
		osSyncPrintf("firing(%d) %d, beam(%d) %s", handnum, firingthisframe, handnum);
		osSyncPrintf("on");
		osSyncPrintf("off");

		chr_update_fireslot(chr, handnum, firingthisframe, firingthisframe && makebeam, &gunpos, &hitpos);

		if (isaibot) {
			if (firingthisframe) {
				osSyncPrintf("firecount(%d) = %d", handnum, chr->aibot->loadedammo[handnum]);

				if (chr->aibot->loadedammo[handnum] > 0) {
					chr->aibot->loadedammo[handnum]--;
				}

				osSyncPrintf("numshots(%d) = %d", handnum, chr->aibot->loadedammo[handnum]);
			}

			chr_set_firing(chr, handnum, firingthisframe && normalshoot);
		} else {
			chr_set_firing(chr, handnum, firingthisframe);
		}
	}
}

void chr_tick_shots(struct chrdata *chr)
{
#if VERSION >= VERSION_NTSC_1_0
	chr->hidden2 &= ~CHRH2FLAG_FIRESOUNDDONE;
#else
	chr->hidden &= ~CHRHFLAG_FIRESOUNDDONE;
#endif

	if (chr->actiontype == ACT_ROBOTATTACK) {
		if (chr->act_robotattack.firing[0]) {
			chr->prop->forcetick = true;
			projectile_create(chr->prop, chr->unk348[0], &chr->act_robotattack.pos[0],
					&chr->act_robotattack.dir[0], chr->act_robotattack.guntype[0], chr_get_target_prop(chr));
			chr->unk348[1]->unk08 = g_Vars.lvframe60 + 2;
			chr->unk348[1]->unk14 = chr->unk348[0]->unk14;
		} else {
			chr->prop->forcetick = false;
		}

		if (chr->act_robotattack.firing[1]) {
			chr->prop->forcetick = true;
			projectile_create(chr->prop, chr->unk348[1], &chr->act_robotattack.pos[1],
					&chr->act_robotattack.dir[1], chr->act_robotattack.guntype[1], chr_get_target_prop(chr));
			chr->unk348[0]->unk14 = chr->unk348[1]->unk14;
		} else {
			chr->prop->forcetick = false;
		}

		beam_tick(chr->unk348[0]->beam);
		beam_tick(chr->unk348[1]->beam);
	} else if (chr->actiontype == ACT_ATTACKAMOUNT) {
		if (chr->act_attack.numshots < chr->act_attack.maxshots && (chr->hidden & CHRHFLAG_FIRINGRIGHT)) {
			chr_shoot(chr, HAND_RIGHT);
		}
	} else {
		if (chr->hidden & CHRHFLAG_FIRINGRIGHT) {
			chr_shoot(chr, HAND_RIGHT);
			chr->hidden &= ~CHRHFLAG_FIRINGRIGHT;
		}

		if (chr->hidden & CHRHFLAG_FIRINGLEFT) {
			chr_shoot(chr, HAND_LEFT);
			chr->hidden &= ~CHRHFLAG_FIRINGLEFT;
		}
	}
}

bool chr_attack_handle_roll_end(struct chrdata *chr)
{
	if (CHRRACE(chr) == RACE_HUMAN) {
		if (chr->act_attack.animcfg == &g_RollAttackAnims[2] || chr->act_attack.animcfg == &g_RollAttackAnims[3]) {
			struct model *model = chr->model;
			struct attackanimconfig *animcfg = &g_RollAttackAnims[1];
			bool flip = model->anim->flip;

			chr->act_attack.turning = TURNSTATE_OFF;
			chr->act_attack.animcfg = animcfg;
			chr->sleep = 0;

			model_set_animation(model, animcfg->animnum, flip, animcfg->shootendframe, chr_get_ranged_speed(chr, 0.7f, 1.12f), 22);

			if (animcfg->endframe >= 0) {
				model_set_anim_end_frame(model, animcfg->endframe);
			}

			return true;
		}
	}

	return false;
}

void chr_attack_handle_recoil_end(struct chrdata *chr)
{
	struct model *model = chr->model;

	if (chr->act_attack.animcfg->recoilendframe > 0) {
		model_set_animation(model,
				model_get_anim_num(model),
				model->anim->flip,
				chr->act_attack.animcfg->recoilendframe,
				chr_get_ranged_speed(chr, 0.5f, 0.8f),
				8);
	} else {
		model_set_animation(model,
				model_get_anim_num(model),
				model->anim->flip,
				chr->act_attack.animcfg->shootendframe,
				chr_get_ranged_speed(chr, 0.5f, 0.8f),
				8);
	}

	if (chr->act_attack.animcfg->endframe >= 0) {
		model_set_anim_end_frame(model, chr->act_attack.animcfg->endframe);
	}
}

void chr_attack_handle_shooting(struct chrdata *chr)
{
	struct model *model = chr->model;
	f32 curframe = model_get_cur_anim_frame(model);
	s32 i;

	if (model_get_anim_num(model) == ANIM_SNIPING_GETDOWN) {
		return;
	}

	// If off-target for 0.5 seconds and have been shooting for 10 frames, stop
	if (chr->act_attack.lastontarget60 < chr->act_attack.lastfire60 - TICKS(30)
			&& model->anim->animnum2 == 0
			&& curframe > chr->act_attack.animcfg->shootstartframe + 10
			&& curframe < chr->act_attack.animcfg->shootendframe
			&& (chr->act_attack.animcfg->recoilendframe < 0 || curframe < chr->act_attack.animcfg->recoilendframe)) {
		if (!chr->act_attack.dorecoil) {
			if (!chr_attack_handle_roll_end(chr)) {
				model_set_animation(model, model_get_anim_num(model), model->anim->flip,
						chr->act_attack.animcfg->shootendframe, chr_get_ranged_speed(chr, 0.5f, 0.8f), 8);

				if (chr->act_attack.animcfg->endframe >= 0) {
					model_set_anim_end_frame(model, chr->act_attack.animcfg->endframe);
				}
			}
		} else {
			chr_attack_handle_recoil_end(chr);
		}

		chr->act_attack.numshots = chr->act_attack.maxshots + 1;

		curframe = model_get_cur_anim_frame(model);
	}

	// Handle recoil and/or restarting the shooting phase of the animation
	if (curframe >= model_get_anim_end_frame(model)) {
		if (model_get_anim_num(model) != ANIM_SNIPING_ONGROUND
				&& (chr->act_attack.dooneburst || chr->act_attack.numshots > chr->act_attack.maxshots)) {
			if (!chr_attack_handle_roll_end(chr)) {
				if (chr->act_attack.flags & ATTACKFLAG_AIMATTARGET) {
					chr_record_last_see_target_time(chr);
				}

				chr_stop(chr);
				return;
			}
		} else if (chr->act_attack.numshots == chr->act_attack.maxshots) {
			chr->act_attack.numshots++;
			chr_attack_handle_recoil_end(chr);
		} else if (chr->act_attack.fired) {
			f32 f2 = 0.5f;
			f32 startframe;
			f32 endframe;
			f32 diff;

			if (chr->act_attack.dorecoil) {
				if (chr->act_attack.animcfg->recoilstartframe > 0) {
					startframe = chr->act_attack.animcfg->recoilstartframe;
				} else {
					startframe = chr->act_attack.animcfg->shootstartframe;
				}

				if (chr->act_attack.animcfg->recoilendframe > 0) {
					endframe = chr->act_attack.animcfg->recoilendframe;
				} else {
					endframe = chr->act_attack.animcfg->shootendframe;
				}
			} else {
				startframe = chr->act_attack.animcfg->shootstartframe;

				if (chr->act_attack.animcfg->recoilstartframe > 0) {
					endframe = chr->act_attack.animcfg->recoilstartframe;
				} else {
					endframe = chr->act_attack.animcfg->shootendframe;
				}
			}

			diff = endframe - startframe;

			if (diff < 12) {
				f2 = diff * (1.0f / 24.0f);
			} else if (diff > 16) {
				f2 = diff * (1.0f / 32.0f);
			}

			if (chr->act_attack.everytick[HAND_RIGHT] && chr->act_attack.everytick[HAND_LEFT]) {
				f2 = f2 + f2;
			}

			chr->act_attack.fired = false;

			model_set_animation(model, model_get_anim_num(model), model->anim->flip, startframe, f2, 8);
			model_set_anim_end_frame(model, endframe);
		}

		curframe = model_get_cur_anim_frame(model);
	}

	// Handle turning
	if (model_get_anim_num(model) != ANIM_SNIPING_ONGROUND && (chr->act_attack.flags & ATTACKFLAG_DONTTURN) == 0) {
		f32 angleoffset = chr->act_attack.animcfg->angleoffset;
		f32 f12 = chr->act_attack.animcfg->unk04;

		if ((chr->act_attack.flags & ATTACKFLAG_AIMONLY) && f12 > model_get_anim_end_frame(model)) {
			f12 = model_get_anim_end_frame(model);
		}

		if (model->anim->flip) {
			angleoffset = BADDTOR(360) - angleoffset;
		}

		chr->act_attack.turning = chr_turn(chr, chr->act_attack.turning, f12, chr_get_ranged_speed(chr, 1, 1.6f), angleoffset);
	}

	// Handle aiming (swiveling arm toward target, and leaning forward/back if needed)
	if ((curframe > chr->act_attack.animcfg->aimstartframe && curframe < chr->act_attack.animcfg->aimendframe)
			|| model_get_anim_num(model) == ANIM_SNIPING_ONGROUND) {
		chr_calculate_aimend(chr, chr->act_attack.animcfg, chr->act_attack.firegun[HAND_LEFT], chr->act_attack.firegun[HAND_RIGHT], 1);
	} else {
		chr_reset_aimend(chr);
	}

	// Determine if firing is starting or stopping
	for (i = 0; i < 2; i++) {
		if (chr->act_attack.firegun[i]) {
			if (chr->act_attack.everytick[i] == 0) {
				if (model_get_anim_num(model) == ANIM_SNIPING_ONGROUND
						|| (curframe >= chr->act_attack.animcfg->shootstartframe && curframe < chr->act_attack.animcfg->shootendframe)) {
					chr_set_hand_firing(chr, i, true);

					chr->act_attack.lastfire60 = g_Vars.lvframe60;

					if (chr->actiontype == ACT_ATTACKROLL) {
						f32 shootduration = chr->act_attack.animcfg->shootendframe - chr->act_attack.animcfg->shootstartframe;

						if (shootduration < 30) {
							if (chr->act_attack.pausecount >= TICKS(60) - (s32)(PAL ? shootduration * (50.0f / 60.0f) : shootduration) * 2) {
								model_set_anim_speed(model, 0.5f, 0);
							} else {
								model_set_anim_speed(model, 0.1f, 0);
								chr->act_attack.pausecount += g_Vars.lvupdate60;
							}
						} else {
							model_set_anim_speed(model, 0.5f, 0);
						}
					} else {
						model_set_anim_speed(model, 0.5f, 0);
					}
				} else {
					chr_set_hand_firing(chr, i, false);

					if (chr->actiontype == ACT_ATTACKROLL) {
						model_set_anim_speed(model, chr_get_ranged_speed(chr, 0.5f, 0.8f), 0);
					} else {
						model_set_anim_speed(model, chr_get_ranged_speed(chr, 0.5f, 0.8f), 0);
					}
				}
			} else if (model_get_anim_num(model) == ANIM_SNIPING_ONGROUND
					|| ((!chr->act_attack.fired && (i == chr->act_attack.nextgun || !chr->act_attack.everytick[chr->act_attack.nextgun]))
						&& ((chr->act_attack.animcfg->recoilstartframe >= 0 && curframe >= chr->act_attack.animcfg->recoilstartframe && curframe <= chr->act_attack.animcfg->recoilendframe)
							|| (chr->act_attack.animcfg->recoilstartframe < 0 && curframe >= chr->act_attack.animcfg->shootstartframe)))) {
				chr->act_attack.fired = true;
				chr->act_attack.nextgun = 1 - chr->act_attack.nextgun;
				chr->act_attack.numshots++;
				chr->act_attack.lastfire60 = g_Vars.lvframe60;

				chr_set_hand_firing(chr, i, true);
			} else {
				chr_set_hand_firing(chr, i, false);
			}
		} else {
			chr_set_hand_firing(chr, i, false);
		}
	}
}

void chr_tick_attackamount(struct chrdata *chr)
{
	struct model *model = chr->model;
	f32 frame = model_get_cur_anim_frame(model);
	f32 angleoffset = chr->act_attack.animcfg->angleoffset;
	f32 unk04 = chr->act_attack.animcfg->unk04;

	chr_turn(chr, 1, unk04, chr_get_ranged_speed(chr, 1, 1.6f), angleoffset);

	if (frame > chr->act_attack.animcfg->aimstartframe && frame < chr->act_attack.animcfg->aimendframe) {
		chr_calculate_aimend(chr, chr->act_attack.animcfg, false, true, 0.2f);
	} else {
		chr_reset_aimend(chr);
	}

	if (frame >= chr->act_attack.animcfg->shootstartframe && chr->act_attack.dooneburst == false) {
		chr->act_attack.dooneburst = true;
	}

	if (chr->act_attack.dooneburst) {
		if (chr->act_attack.numshots++ < chr->act_attack.maxshots) {
			chr_set_hand_firing(chr, HAND_RIGHT, true);
		} else {
			chr_attack_handle_recoil_end(chr);
			chr_set_hand_firing(chr, HAND_RIGHT, false);
		}
	} else {
		chr_set_hand_firing(chr, HAND_RIGHT, false);
	}
}

/**
 * Sets the robot's muzzle flash to on or off.
 *
 * There are two muzzles, left and right, which is specified using the `right`
 * argument.
 */
void robot_set_muzzle_flash(struct chrdata *chr, bool right, bool visible)
{
	struct modelnode *node;
	union modelrwdata *rwdata;
	s32 partnum;

	if (right) {
		partnum = MODELPART_ROBOT_RGUNFIRE;
	} else {
		partnum = MODELPART_ROBOT_LGUNFIRE;
	}

	node = model_get_part(chr->model->definition, partnum);

	if (node) {
		rwdata = model_get_node_rw_data(chr->model, node);
	}

	if (rwdata) {
		rwdata->chrgunfire.visible = visible;
	}
}

void robot_attack(struct chrdata *chr)
{
	u32 numshots = random() % 20;

	if (chr->unk348[0] && chr->unk348[1]) {
		chr->actiontype = ACT_ROBOTATTACK;

		chr->unk348[0]->beam->age = -1;
		chr->unk348[0]->unk00 = random() % 3;
		chr->unk348[0]->unk01 = 0;
		chr->unk348[0]->unk08 = -1;
		chr->unk348[0]->unk0c = 0.85f;

		if ((lv_get_difficulty() == DIFF_PA) * 0.2f) {
			chr->unk348[0]->unk10 = 2.0f;
		} else {
			chr->unk348[0]->unk10 = 1.0f;
		}

		chr->unk348[0]->unk14 = 0.0f;

		chr->act_robotattack.pos[0].x = 0.0f;
		chr->act_robotattack.pos[0].y = 0.0f;
		chr->act_robotattack.pos[0].z = 0.0f;
		chr->act_robotattack.dir[0].x = 0.0f;
		chr->act_robotattack.dir[0].y = 0.0f;
		chr->act_robotattack.dir[0].z = 0.0f;
		chr->act_robotattack.guntype[0] = WEAPON_WATCHLASER;
		chr->act_robotattack.firing[0] = false;

		chr->unk348[1]->beam->age = -1;
		chr->unk348[1]->unk00 = random() % 3;
		chr->unk348[1]->unk01 = 0;
		chr->unk348[1]->unk08 = -1;
		chr->unk348[1]->unk0c = 0.85f;
		chr->unk348[1]->unk10 = 0.2f;
		chr->unk348[1]->unk14 = 0.0f;

		chr->act_robotattack.guntype[1] = WEAPON_WATCHLASER;
		chr->act_robotattack.firing[1] = false;
		chr->act_robotattack.finished = false;
		chr->act_robotattack.numshots[0] = numshots;
		chr->act_robotattack.numshots[1] = numshots;
		chr->act_robotattack.pos[1].x = 0.0f;
		chr->act_robotattack.pos[1].y = 0.0f;
		chr->act_robotattack.pos[1].z = 0.0f;
		chr->act_robotattack.dir[1].x = 0.0f;
		chr->act_robotattack.dir[1].y = 0.0f;
		chr->act_robotattack.dir[1].z = 0.0f;

		chr_choose_stand_animation(chr, 16);
	}
}

void chr_robot_turn_toward_target(struct chrdata *chr, f32 frac, f32 yrot)
{
	struct prop *targetprop = chr_get_target_prop(chr);
	f32 angle = atan2f(targetprop->pos.x - chr->prop->pos.x, targetprop->pos.z - chr->prop->pos.z);
	f32 value = model_tween_rot_axis(yrot, angle, frac);

	chr_set_theta(chr, value);
}

void chr_tick_robotattack(struct chrdata *chr)
{
	s32 i;
	f32 roty = 0.0f;
	f32 rotx = 0.0f;
	struct prop *targetprop = chr_get_target_prop(chr);
	bool firing;
	bool empty;
	f32 invtheta = chr_get_theta(chr);
	struct act_robotattack *act = &chr->act_robotattack;

	chr_robot_turn_toward_target(chr, 0.085f, invtheta);

	if (chr->model->definition->skel != &g_SkelRobot) {
		act->finished = true;
		return;
	}

	for (i = 0; i < 2; i++) {
		empty = false;

		if (act->numshots[i] > 0) {
			chr->unk348[i]->unk01 = !(chr->unk348[i]->unk00 % 3);
			firing = !(chr->unk348[i]->unk00 % 2);
		} else {
			chr->unk348[i]->unk01 = 0;
			firing = false;
		}

		act->firing[i] = firing;

		if (act->numshots[0] <= 0 && act->numshots[1] <= 0) {
			empty = true;

			if (ABS(chr->gunroty[0]) < 0.03f
					&& ABS(chr->gunrotx[0]) < 0.03f
					&& ABS(chr->gunroty[1]) < 0.03f
					&& ABS(chr->gunrotx[1]) < 0.03f) {
				act->finished = true;
			}
		}

		if (empty);
		if ((f32)empty);

		if (!empty) {
			f32 aimy;
			union modelrodata *rodata;
			struct coord spe4;
			Mtxf spa4;

			aimy = targetprop->pos.y - 20.0f;
			rodata = model_get_part_rodata(chr->model->definition, (i ? MODELPART_ROBOT_0000 : MODELPART_ROBOT_0001));

			act->pos[i].x = rodata->position.pos.x;
			act->pos[i].y = rodata->position.pos.y - 300.0f;
			act->pos[i].z = rodata->position.pos.z;

			mtx4_load_y_rotation(invtheta, &spa4);
			mtx4_rotate_vec(&spa4, &act->pos[i], &spe4);

			spe4.x *= chr->model->scale;
			spe4.y *= chr->model->scale;
			spe4.z *= chr->model->scale;

			act->pos[i].x = spe4.x + chr->prop->pos.x;
			act->pos[i].y = spe4.y + chr->prop->pos.y;
			act->pos[i].z = spe4.z + chr->prop->pos.z;

			roty = atan2f(targetprop->pos.x - act->pos[i].x, targetprop->pos.z - act->pos[i].z) - invtheta;

			if (roty < 0.0f) {
				roty += BADDTOR(360);
			}

			if (roty > BADDTOR(180)) {
				roty -= BADDTOR(360);
			}

			if (roty < -0.524f) {
				roty = -0.524f;
			}

			if (roty > 0.524f) {
				roty = 0.524f;
			}

#define X() (targetprop->pos.x - act->pos[i].x)
#define Z() (targetprop->pos.z - act->pos[i].z)

			rotx = BADDTOR(360) - atan2f(aimy - act->pos[i].y, sqrtf(Z() * Z() + X() * X()));

			if (rotx > BADDTOR(180)) {
				rotx -= BADDTOR(360);
			}

			if (rotx < -0.524f) {
				rotx = -0.524f;
			}

			if (rotx > 0.524f) {
				rotx = 0.524f;
			}
		}

		chr->gunroty[i] += (roty - chr->gunroty[i]) * 0.15f;
		chr->gunrotx[i] += (rotx - chr->gunrotx[i]) * 0.15f;

		if (!empty) {
			if (firing) {
				f32 gunrotx = chr->gunrotx[i];
				f32 gunroty = chr->gunroty[i];

				if (gunrotx < 0.0f) {
					gunrotx += BADDTOR(360);
				}

				if (gunroty < 0.0f) {
					gunroty += BADDTOR(360);
				}

				gunroty += invtheta;

				if (gunroty >= BADDTOR(360)) {
					gunroty -= BADDTOR(360);
				}

				act->dir[i].x = sinf(gunroty) * cosf(gunrotx);
				act->dir[i].y = -sinf(gunrotx);
				act->dir[i].z = cosf(gunroty) * cosf(gunrotx);

				robot_set_muzzle_flash(chr, i, true);

				act->numshots[i]--;
			}

			chr->unk348[i]->unk00++;
		}
	}
}

void chr_tick_attack(struct chrdata *chr)
{
	struct model *model = chr->model;
	f32 curframe = model_get_cur_anim_frame(model);

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		model_set_animation(model, chr->act_attack.animcfg->animnum, chr->act_attack.flip,
				chr->act_attack.animcfg->startframe, chr_get_ranged_speed(chr, 0.5f, 0.8f), 16);
		chr_attack_choose_endframe(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (chr->act_attack.animcfg->animnum == ANIM_SNIPING_GETDOWN) {
		if (curframe >= model_get_anim_end_frame(model)) {
			chr->act_attack.animcfg = &g_SnipeAttackAnims[1];

			model_set_animation(model, chr->act_attack.animcfg->animnum, chr->act_attack.flip,
					chr->act_attack.animcfg->startframe, chr_get_ranged_speed(chr, 0.5f, 0.8f), 16);
		}
	}

	if (!chr->aibot && chr->act_attack.reaim != 0) {
		if (chr->act_attack.reaim == 1) {
			f32 startframe;

			if (chr->act_attack.animcfg->recoilendframe >= 0) {
				startframe = chr->act_attack.animcfg->recoilendframe;
			} else {
				startframe = chr->act_attack.animcfg->shootendframe;
			}

			model_set_animation(model, model_get_anim_num(model), model->anim->flip, startframe, chr_get_ranged_speed(chr, 0.5f, 0.8f), 16);

			if (chr->act_attack.animcfg->endframe >= 0) {
				model_set_anim_end_frame(model, chr->act_attack.animcfg->endframe);
			}

			chr->act_attack.reaim = 2;
			chr_reset_aimend(chr);
			return;
		}

		if (chr->act_attack.reaim == 2) {
			if (curframe >= model_get_anim_end_frame(model)) {
				chr->act_attack.flags &= ~ATTACKFLAG_DONTTURN;

				if (chr->act_attack.standing) {
					chr_attackstand(chr, chr->act_attack.flags, chr->act_attack.entityid);
				} else {
					chr_attackkneel(chr, chr->act_attack.flags, chr->act_attack.entityid);
				}
			}
			return;
		}
	}

	if (!chr->aibot && (chr->act_attack.flags & ATTACKFLAG_AIMONLY)) {
		if (chr->act_attack.flags & ATTACKFLAG_DONTTURN) {
			if (!chr_calculate_aimend(chr, chr->act_attack.animcfg, chr->act_attack.firegun[HAND_LEFT], chr->act_attack.firegun[HAND_RIGHT], 0.2f)) {
				chr->act_attack.reaim = 1;
			}
			return;
		}

		if (curframe >= model_get_anim_end_frame(model)) {
			chr->act_attack.flags |= ATTACKFLAG_DONTTURN;
			chr->act_attack.turning = TURNSTATE_OFF;
			return;
		}
	}

	if (!chr->aibot
			&& chr->act_attack.dorecoil == false
			&& chr->act_attack.animcfg->recoilendframe > 0
			&& curframe <= chr->act_attack.animcfg->recoilendframe
			&& curframe >= model_get_anim_end_frame(model)) {
		model_set_animation(model, model_get_anim_num(model), model->anim->flip,
				chr->act_attack.animcfg->recoilendframe, chr_get_ranged_speed(chr, 0.5f, 0.8f), 16);

		if (chr->act_attack.dooneburst) {
			if (chr->act_attack.animcfg->endframe >= 0) {
				model_set_anim_end_frame(model, chr->act_attack.animcfg->endframe);
			}
		} else {
			model_set_anim_end_frame(model, chr->act_attack.animcfg->shootendframe);
		}
	}

	chr_attack_handle_shooting(chr);
}

void chr_tick_attackroll(struct chrdata *chr)
{
	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_attackroll_choose_animation(chr);

		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	if (chr->act_attack.onehanded) {
		struct model *model = chr->model;
		f32 curframe = model_get_cur_anim_frame(model);

		if (chr->act_attack.animcfg == &g_RollAttackAnims[4]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[5]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[6]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[7]) {
			if (curframe >= chr->act_attack.animcfg->endframe) {
				struct attackanimconfig *newanimcfg = chr->act_attack.animcfg + 4;
				bool flip = model->anim->flip;
				f32 mergetime = 16;

				if (chr->act_attack.firegun[HAND_LEFT] && chr->act_attack.firegun[HAND_RIGHT]) {
					if (random() % 2 == 0) {
						newanimcfg += 4;
					} else {
						newanimcfg += 8;
					}
				}

				if (newanimcfg == &g_RollAttackAnims[8]) {
					mergetime = 24;
				} else if (newanimcfg == &g_RollAttackAnims[9]) {
					mergetime = 24;
				} else if (newanimcfg == &g_RollAttackAnims[10]) {
					mergetime = 32;
				} else if (newanimcfg == &g_RollAttackAnims[11]) {
					mergetime = 44;
				} else if (newanimcfg == &g_RollAttackAnims[12]) {
					mergetime = 24;
				} else if (newanimcfg == &g_RollAttackAnims[13]) {
					mergetime = 34;
				} else if (newanimcfg == &g_RollAttackAnims[14]) {
					mergetime = 32;
				} else if (newanimcfg == &g_RollAttackAnims[15]) {
					mergetime = 44;
				} else if (newanimcfg == &g_RollAttackAnims[16]) {
					mergetime = 24;
				} else if (newanimcfg == &g_RollAttackAnims[17]) {
					mergetime = 34;
				} else if (newanimcfg == &g_RollAttackAnims[18]) {
					mergetime = 32;
				} else if (newanimcfg == &g_RollAttackAnims[19]) {
					mergetime = 44;
				}

				chr->act_attack.turning = TURNSTATE_OFF;
				chr->act_attack.animcfg = newanimcfg;
				chr->sleep = 0;

				model_set_animation(model, newanimcfg->animnum, flip, newanimcfg->startframe,
						chr_get_ranged_speed(chr, 0.5f, 0.8f), mergetime);

				if (chr->act_attack.dorecoil) {
					if (newanimcfg->recoilendframe >= 0.0f) {
						model_set_anim_end_frame(model, newanimcfg->recoilendframe);
					} else {
						model_set_anim_end_frame(model, newanimcfg->shootendframe);
					}
				} else {
					if (newanimcfg->recoilstartframe >= 0.0f) {
						model_set_anim_end_frame(model, newanimcfg->recoilstartframe);
					} else {
						if (newanimcfg->endframe >= 0.0f) {
							model_set_anim_end_frame(model, newanimcfg->endframe);
						}
					}
				}

				if (chr->act_attack.animcfg->angleoffset != 0.0f) {
					union modelrwdata *rwdata = model_get_node_rw_data(model, model->definition->rootnode);
					rwdata->chrinfo.unk5c = mergetime;
					rwdata->chrinfo.unk58 = -chr->act_attack.animcfg->angleoffset / mergetime;

					if (flip) {
						rwdata->chrinfo.unk58 = -rwdata->chrinfo.unk58;
					}
				}
			}
		} else if (chr->act_attack.animcfg == &g_RollAttackAnims[8]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[9]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[10]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[11]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[12]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[13]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[14]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[15]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[16]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[17]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[18]
				|| chr->act_attack.animcfg == &g_RollAttackAnims[19]) {
			if (!chr->act_attack.dorecoil
					&& chr->act_attack.animcfg->recoilendframe > 0
					&& curframe <= chr->act_attack.animcfg->recoilendframe
					&& curframe >= model_get_anim_end_frame(model)) {
				model_set_animation(model, model_get_anim_num(model), model->anim->flip,
						chr->act_attack.animcfg->recoilendframe, chr_get_ranged_speed(chr, 0.5f, 0.8f), 16);

				if (chr->act_attack.dooneburst) {
					if (chr->act_attack.animcfg->endframe >= 0) {
						model_set_anim_end_frame(model, chr->act_attack.animcfg->endframe);
					}
				} else {
					u32 stack;
					model_set_anim_end_frame(model, chr->act_attack.animcfg->shootendframe);
				}
			}
		}
	}

	chr_attack_handle_shooting(chr);
}

void prop_print_dangerous(void)
{
	u8 i;

	osSyncPrintf("Current dangerous items:");

	for (i = 0; i < ARRAYCOUNT(g_DangerousProps); i++) {
		struct prop *prop = g_DangerousProps[i];

		if (prop) {
			if (prop->weapon
					&& prop->weapon->weaponnum == WEAPON_GRENADE
					&& prop->type == PROPTYPE_WEAPON) {
				osSyncPrintf("    Grenade %x", prop);
			} else if (prop->type == PROPTYPE_EXPLOSION) {
				osSyncPrintf("    Explosion %x", prop);
			} else {
				osSyncPrintf("misc dangerous prop");
			}
		}
	}
}

void prop_unset_dangerous(struct prop *prop)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_DangerousProps); i++) {
		if (g_DangerousProps[i] == prop) {
			g_DangerousProps[i] = NULL;
			return;
		}
	}
}

void prop_set_dangerous(struct prop *prop)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_DangerousProps); i++) {
		if (g_DangerousProps[i] == NULL) {
			g_DangerousProps[i] = prop;
			return;
		}
	}
}

void chr_tick_throwgrenade(struct chrdata *chr)
{
	struct model *model;
	f32 frame;
	u32 hand;
	struct prop *weaponprop;
	struct defaultobj *obj;
	struct weaponobj *weapon;
	f32 frame2;

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_throwgrenade_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	model = chr->model;
	frame = model_get_cur_anim_frame(model);
	hand = model->anim->flip ? HAND_LEFT : HAND_RIGHT;
	weaponprop = chr_get_held_prop(chr, hand);

	// Decide at which frame the grenade should become visible in the chr's hand
	if ((frame >= 20 && weaponprop && model_get_anim_num(model) == ANIM_THROWGRENADE_STANDING)
			|| (frame >= 1 && weaponprop && model_get_anim_num(model) == ANIM_THROWGRENADE_NOPIN)
			|| (frame >= 1 && weaponprop && model_get_anim_num(model) == ANIM_THROWGRENADE_CROUCHING)) {
		obj = weaponprop->obj;
		obj->hidden &= ~OBJHFLAG_GONE;
	}

	// Decide at which frame the grenade leaves the chr's hand
	if ((frame >= 119 && weaponprop && model_get_anim_num(model) == ANIM_THROWGRENADE_STANDING)
			|| (frame >= 57 && weaponprop && model_get_anim_num(model) == ANIM_THROWGRENADE_NOPIN)
			|| (frame >= 58 && weaponprop && model_get_anim_num(model) == ANIM_THROWGRENADE_CROUCHING)) {
		weapon = weaponprop->weapon;
		obj_set_dropped(weaponprop, DROPTYPE_THROWGRENADE);
		chr->hidden |= CHRHFLAG_DROPPINGITEM;
		weapon->timer240 = TICKS(240);
	}

	frame2 = model_get_cur_anim_frame(model);

	if (frame2 >= model_get_anim_end_frame(model)) {
		chr_stop(chr);
	} else {
		if ((frame >= 87 && frame <= 110 && model_get_anim_num(model) == ANIM_THROWGRENADE_STANDING)
				|| (frame >= 5 && frame <= 45 && model_get_anim_num(model) == ANIM_THROWGRENADE_NOPIN)
				|| ((frame >= 20 && frame <= 45 && model_get_anim_num(model) == ANIM_THROWGRENADE_CROUCHING))) {
			f32 value = chr_get_ranged_speed(chr, 1, 3.2);
			chr_turn(chr, 1, 110, value, 0);
		}
	}
}

bool chr_detect_dangerous_object(struct chrdata *chr, u8 flags)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_DangerousProps); i++) {
		struct prop *prop = g_DangerousProps[i];
		bool pass = false;

		if (prop) {
			if ((flags & 1) && prop->weapon &&
					prop->weapon->weaponnum == WEAPON_GRENADE &&
					prop->weapon->timer240 < TICKS(480)) {
				pass = true;
			}

			if ((flags & 2) && prop->type == PROPTYPE_EXPLOSION) {
				pass = true;
			}

			if (pass && chr_get_squared_distance_to_coord(chr, &prop->pos) < 1600) {
				chr->runfrompos.x = g_DangerousProps[i]->pos.x;
				chr->runfrompos.y = g_DangerousProps[i]->pos.y;
				chr->runfrompos.z = g_DangerousProps[i]->pos.z;

				if (chr->aibot) {
					chr->aibot->flags |= BOTFLAG_AVOIDINGDANGEROUSPROP;
					chr->aibot->dangerouspropnum = i;
				}

				return true;
			}
		}
	}

	if (chr->aibot) {
		chr->aibot->flags &= ~BOTFLAG_AVOIDINGDANGEROUSPROP;
		chr->aibot->dangerouspropnum = -1;
	}

	return false;
}

void chr_tick_bonddie(struct chrdata *chr)
{
	// empty
}

bool chr_turn_toward(struct chrdata *chr, struct coord *aimpos, u32 goposflags, f32 *turnspeed)
{
	struct prop *prop = chr->prop;
	struct model *model = chr->model;
	f32 accel;
	f32 maxspeed;
	f32 angle1;
	f32 finalangle;
	f32 angle2;
	f32 angle3;
	f32 xdiff = aimpos->f[0] - prop->pos.f[0];
	f32 zdiff = aimpos->f[2] - prop->pos.f[2];
	bool result;

	result = false;
	angle1 = atan2f(xdiff, zdiff);
	finalangle = chr_get_rot_y(chr);
	angle2 = angle1 - finalangle;

	if (angle2 < 0) {
		angle2 += BADDTOR(360);
	}

	if (chr->aibot) {
		if (chr->blurdrugamount > 0) {
			angle1 += chr->blurdrugamount * PALUPF(0.00031410926021636f) * sinf((g_Vars.lvframe60 % TICKS(1200)) * PALUPF(0.0052351541817188f));

			if (angle1 >= BADDTOR(360)) {
				angle1 -= BADDTOR(360);
			}

			angle1 += BADDTOR(360);
		}

		finalangle = angle1;
		*turnspeed = 0;
		result = true;
	} else {
		angle3 = angle2;

		if (angle3 > BADDTOR(180)) {
			angle3 = BADDTOR(360) - angle3;
		}

		if ((goposflags & (GOPOSFLAG_JOG | GOPOSFLAG_RUN)) == GOPOSFLAG_RUN) {
			maxspeed = 0.29915165901184f;
			accel = 0.014957583509386f;
		} else if ((goposflags & (GOPOSFLAG_JOG | GOPOSFLAG_RUN)) == GOPOSFLAG_JOG) {
			if (angle3 < BADDTOR(22.5f)) {
				maxspeed = 0.019631829112768f;
			} else if (angle3 < BADDTOR(72)) {
				maxspeed = 0.098159141838551f;
			} else {
				maxspeed = 0.1963182836771f;
			}

			accel = 0.014957583509386f;
		} else {
			if (angle3 < BADDTOR(22.5f)) {
				maxspeed = 0.0098159145563841f;
			} else if (angle3 < BADDTOR(72)) {
				maxspeed = 0.049079570919275f;
			} else {
				maxspeed = 0.12564370036125f;
			}

			accel = 0.0098159145563841f;
		}

		maxspeed *= model->anim->playspeed;
		accel *= model->anim->playspeed;

		apply_rotation(&finalangle, angle1, turnspeed, accel, accel + accel, maxspeed);

		if (ABS(finalangle - angle1) < 0.01f) {
			*turnspeed = 0;
			result = true;
		}
	}

	chr_set_rot_y(chr, finalangle);

	return result;
}

void chr_tick_attackwalk(struct chrdata *chr)
{
	struct model *model = chr->model;
	struct prop *prop = chr->prop;
	struct prop *targetprop = chr_get_target_prop(chr);
	s32 i;
	f32 xdiff;
	f32 zdiff;

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_attackwalk_choose_animation(chr);

		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	chr->act_attackwalk.frame60count += g_Vars.lvupdate60;
	chr->lastwalk60 = g_Vars.lvframe60;

	// If stuck or time exceeded, stop
	if (chr->invalidmove == 1
			|| chr->lastmoveok60 < g_Vars.lvframe60 - TICKS(60)
			|| chr->act_attackwalk.frame60count > chr->act_attackwalk.frame60max) {
		if (model_get_cur_anim_frame(model) > model_get_num_anim_frames(model) * 0.5f) {
			model_set_anim_speed_auto(model, 0, 16);
		} else {
			model_set_anim_speed_auto(model, model_get_num_anim_frames(model) * 0.5f, 16);
		}

		chr_record_last_see_target_time(chr);
		chr_stop(chr);
		return;
	}

	// If within 3 metres of the player, stop
	xdiff = targetprop->pos.x - prop->pos.x;
	zdiff = targetprop->pos.z - prop->pos.z;

	if (xdiff < 300 && xdiff > -300 && zdiff < 300 && zdiff > -300) {
		chr_record_last_see_target_time(chr);
		chr_stop(chr);
		return;
	}

	if (chr_turn_toward(chr, &targetprop->pos, GOPOSFLAG_WALK, &chr->act_attackwalk.turnspeed)) {
		chr->act_attackwalk.facedtarget = true;
	}

	if (chr->act_attackwalk.frame60count > TICKS(20)) {
		chr_calculate_aimend(chr, chr->act_attackwalk.animcfg,
				chr->act_attackwalk.firegun[HAND_LEFT],
				chr->act_attackwalk.firegun[HAND_RIGHT], 1);
	} else {
		chr_reset_aimend(chr);
	}

	if (chr->act_attackwalk.facedtarget && chr->act_attackwalk.frame60count > TICKS(30)) {
		for (i = 0; i < 2; i++) {
			if (chr->act_attackwalk.firegun[i]) {
				if (!chr->act_attackwalk.everytick[i]) {
					chr_set_hand_firing(chr, i, true);
				} else if (chr->act_attackwalk.frame60count > chr->act_attackwalk.nextshot60
						&& (i == chr->act_attackwalk.nextgun || chr->act_attackwalk.everytick[chr->act_attackwalk.nextgun] == 0)) {
					chr->act_attackwalk.nextshot60 = chr->act_attackwalk.frame60count;

					if (chr->act_attackwalk.everytick[1 - i]) {
						if (chr->act_attackwalk.singleshot[i]) {
							chr->act_attackwalk.nextshot60 += TICKS(90);
						} else {
							chr->act_attackwalk.nextshot60 += TICKS(20);
						}
					} else {
						if (chr->act_attackwalk.singleshot[i]) {
							chr->act_attackwalk.nextshot60 += TICKS(180);
						} else {
							chr->act_attackwalk.nextshot60 += TICKS(40);
						}
					}

					chr->act_attackwalk.nextgun = 1 - chr->act_attackwalk.nextgun;

					chr_set_hand_firing(chr, i, true);
				} else {
					chr_set_hand_firing(chr, i, false);
				}
			} else {
				chr_set_hand_firing(chr, i, false);
			}
		}
	} else {
		chr_set_hand_firing(chr, HAND_LEFT, false);
		chr_set_hand_firing(chr, HAND_RIGHT, false);
	}
}

/**
 * This function might be misnamed. It was named by isolating it and testing it
 * with different inputs to see what it returns, but I couldn't determine how
 * the range affects the latter part of the function.
 */
bool pos_is_moving_towards_pos_or_stopped_in_range(struct coord *prevpos, struct coord *moveddelta, struct coord *targetpos, f32 range)
{
	struct coord prevdist;
	f32 tmp;

	prevdist.x = targetpos->x - prevpos->x;
	prevdist.z = targetpos->z - prevpos->z;

	if (moveddelta->f[0] == 0 && moveddelta->f[2] == 0) {
		return prevdist.f[0] * prevdist.f[0] + prevdist.f[2] * prevdist.f[2] <= range * range;
	}

	tmp = moveddelta->f[0] * prevdist.f[0] + moveddelta->f[2] * prevdist.f[2];

	if (tmp > 0) {
		f32 sqmoveddist = moveddelta->f[0] * moveddelta->f[0] + moveddelta->f[2] * moveddelta->f[2];
		f32 sqprevdist = prevdist.f[0] * prevdist.f[0] + prevdist.f[2] * prevdist.f[2];

		if ((sqprevdist - range * range) * sqmoveddist <= tmp * tmp) {
			return true;
		}

		return false;
	}

	return false;
}

/**
 * Return true if:
 * - either prevpos or curpos is within the given range of targetpos, and
 * - the prev -> cur delta is moving towards the targetpos or is stopped inside
 *   the range.
 *
 * This is a lateral check, meaning the Y value is not considered.
 */
bool pos_is_arriving_laterally_at_pos(struct coord *prevpos, struct coord *curpos, struct coord *targetpos, f32 range)
{
	struct coord moveddelta;

	if (prevpos->x <= targetpos->x - range && curpos->x <= targetpos->x - range) {
		return false;
	}

	if (prevpos->x >= targetpos->x + range && curpos->x >= targetpos->x + range) {
		return false;
	}

	if (prevpos->z <= targetpos->z - range && curpos->z <= targetpos->z - range) {
		return false;
	}

	if (prevpos->z >= targetpos->z + range && curpos->z >= targetpos->z + range) {
		return false;
	}

	moveddelta.x = curpos->x - prevpos->x;
	moveddelta.y = 0;
	moveddelta.z = curpos->z - prevpos->z;

	return pos_is_moving_towards_pos_or_stopped_in_range(prevpos, &moveddelta, targetpos, range);
}

/**
 * Return true if:
 * - either prevpos or curpos is within the given range of targetpos,
 * - the prev -> cur delta is moving towards the targetpos or is stopped inside
 *   the range, and
 * - either prevpos or curpos is within 150cm vertically of targetpos.
 */
bool pos_is_arriving_at_pos(struct coord *prevpos, struct coord *curpos, struct coord *targetpos, f32 range)
{
	if (prevpos->y <= targetpos->y - 150 && curpos->y <= targetpos->y - 150) {
		return false;
	}

	if (prevpos->y >= targetpos->y + 150 && curpos->y >= targetpos->y + 150) {
		return false;
	}

	return pos_is_arriving_laterally_at_pos(prevpos, curpos, targetpos, range);
}

void chr_tick_runpos(struct chrdata *chr)
{
	struct prop *prop = chr->prop;
	struct model *model = chr->model;
	u32 race = CHRRACE(chr);
	f32 fVar6;
	f32 zero;
	f32 fVar7;

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_runpos_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	chr->lastwalk60 = g_Vars.lvframe60;

	if (chr->invalidmove == 1
			|| g_Vars.lvframe60 - TICKS(60) > chr->lastmoveok60
			|| pos_is_arriving_laterally_at_pos(&chr->prevpos, &prop->pos, &chr->act_runpos.pos, chr->act_runpos.neardist)) {
		if (race == RACE_HUMAN) {
			model_get_anim_num(model);
		}

		zero = 0;
		fVar7 = model_get_cur_anim_frame(model);
		fVar6 = fVar7 - zero;

		if (fVar7 < 0) {
			fVar6 += model_get_num_anim_frames(model);
		}

		if (model_get_num_anim_frames(model) * 0.5f < fVar6) {
			zero = 0;
			model_set_anim_speed_auto(model, model_get_num_anim_frames(model) - zero, 16);
		} else {
			zero = 0;
			fVar7 = model_get_num_anim_frames(model) * 0.5f;
			fVar6 = fVar7 - zero;

			if (fVar7 < 0) {
				fVar6 += model_get_num_anim_frames(model);
			}

			model_set_anim_speed_auto(model, fVar6, 16);
		}

		chr_stop(chr);
		return;
	}

	chr_turn_toward(chr, &chr->act_runpos.pos, GOPOSFLAG_JOG, &chr->act_runpos.turnspeed);

	if (chr->act_runpos.eta60 > 0) {
		chr->act_runpos.eta60 -= g_Vars.lvupdate60;
	} else {
		fVar7 = 1;

		if (race == RACE_HUMAN) {
			if (model_get_anim_num(model) == ANIM_RUNNING_ONEHANDGUN) {
				fVar7 = chr_get_anim_movedist_per_frame(ANIM_RUNNING_ONEHANDGUN);
			} else {
				fVar7 = chr_get_anim_movedist_per_frame(ANIM_RUNNING_TWOHANDGUN);
			}
		} else if (race == RACE_SKEDAR) {
			fVar7 = chr_get_anim_movedist_per_frame(ANIM_SKEDAR_RUNNING);
		}

		chr->act_runpos.neardist += fVar7 * g_Vars.lvupdate60freal * model_get_abs_anim_speed(model);
	}
}

void chr_nav_consider_swap_edges(struct coord *edge1, struct coord *edge2, struct coord *arg2)
{
	struct coord tmp;
	struct coord sp00;

	tmp.x = edge2->x - edge1->x;
	tmp.y = edge2->y - edge1->y;
	tmp.z = edge2->z - edge1->z;

	sp00.x = -arg2->z;
	sp00.y = 0;
	sp00.z = arg2->x;

	if (sp00.f[0] * tmp.f[0] + sp00.f[2] * tmp.f[2] > 0) {
		// empty
	} else {
		tmp.x = edge1->x;
		tmp.y = edge1->y;
		tmp.z = edge1->z;

		edge1->x = edge2->x;
		edge1->y = edge2->y;
		edge1->z = edge2->z;

		edge2->x = tmp.x;
		edge2->y = tmp.y;
		edge2->z = tmp.z;
	}
}

/**
 * Check to see if the chr can see the next pos in their route.
 *
 * If an obstacle is found, set the leftpos and rightpos coordinates to the
 * left and right corners of the object from the chr's perspective and return
 * false.
 *
 * Return true if path ahead is clear.
 *
 * This is similar to chr_nav_check_for_obstacle. The difference between the two are
 * not yet understood.
 */
bool chr_nav_can_see_next_pos(struct chrdata *chr, struct coord *chrpos, RoomNum *chrrooms, struct coord *aimpos, struct coord *leftpos, struct coord *rightpos, f32 negchrradius, f32 chrradius, s32 cdtypes, s32 arg9)
{
	struct coord spd4;
	f32 spd0;
	f32 spcc;
	f32 spc8;
	f32 spc4;
	f32 norm;
	bool spbc = false;
	bool spb8 = false;
	struct coord spac;
	struct coord spa0;
	struct coord sp94;
	struct coord sp88;
	bool result = false;
	f32 ymax;
	f32 ymin;
	f32 radius2;
	struct coord sp6c;
	struct coord sp60;
	RoomNum sp50[8];
	RoomNum sp40[8];
	struct prop *prop = chr->prop;

	chr_get_bbox(prop, &radius2, &ymax, &ymin);

	spd4.x = aimpos->x - chrpos->x;
	spd4.y = 0.0f;
	spd4.z = aimpos->z - chrpos->z;

	if (spd4.f[0] == 0.0f && spd4.f[2] == 0.0f) {
		return true;
	}

	norm = 1.0f / sqrtf(spd4.f[0] * spd4.f[0] + spd4.f[2] * spd4.f[2]);
	spd4.x *= norm;
	spd4.z *= norm;

	spd0 = spd4.x * chrradius * 0.95f;
	spcc = spd4.z * chrradius * 0.95f;
	spc8 = spd4.x * chrradius * 1.2f;
	spc4 = spd4.z * chrradius * 1.2f;

	chr_set_perim_enabled(chr, false);

	sp6c.x = chrpos->x + spcc;
	sp6c.y = chrpos->y;
	sp6c.z = chrpos->z - spd0;

	sp60.x = (spd4.x * negchrradius) + (aimpos->x + spc4);
	sp60.y = aimpos->y;
	sp60.z = (spd4.z * negchrradius) + (aimpos->z - spc8);

	if (cd_exam_cyl_move07(chrpos, chrrooms, &sp6c, sp50, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION
			|| cd_exam_cyl_move03(&sp6c, sp50, &sp60, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION) {
		spbc = true;
#if VERSION >= VERSION_JPN_FINAL
		cd_get_edge(&spac, &spa0, 14154, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
		cd_get_edge(&spac, &spa0, 14151, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_BETA
		cd_get_edge(&spac, &spa0, 14146, "chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
		cd_get_edge(&spac, &spa0, 14145, "chraction.c");
#else
		cd_get_edge(&spac, &spa0, 14158, "chraction.c");
#endif
		chr_nav_consider_swap_edges(&spac, &spa0, &spd4);
	}

	sp6c.x = chrpos->x - spcc;
	sp6c.y = chrpos->y;
	sp6c.z = chrpos->z + spd0;

	sp60.x = (spd4.x * negchrradius) + (aimpos->x - spc4);
	sp60.y = aimpos->y;
	sp60.z = (spd4.z * negchrradius) + (aimpos->z + spc8);

	if (cd_exam_cyl_move07(chrpos, chrrooms, &sp6c, sp50, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION
			|| cd_exam_cyl_move03(&sp6c, chrrooms, &sp60, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION) {
		spb8 = true;
#if VERSION >= VERSION_JPN_FINAL
		cd_get_edge(&sp94, &sp88, 14169, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
		cd_get_edge(&sp94, &sp88, 14166, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_BETA
		cd_get_edge(&sp94, &sp88, 14161, "chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
		cd_get_edge(&sp94, &sp88, 14160, "chraction.c");
#else
		cd_get_edge(&sp94, &sp88, 14173, "chraction.c");
#endif
		chr_nav_consider_swap_edges(&sp94, &sp88, &spd4);
	}

	if (spbc && spb8) {
		chr_nav_consider_swap_edges(&spac, &sp94, &spd4);
		chr_nav_consider_swap_edges(&spa0, &sp88, &spd4);

		leftpos->x = spac.x;
		leftpos->y = spac.y;
		leftpos->z = spac.z;

		rightpos->x = sp88.x;
		rightpos->y = sp88.y;
		rightpos->z = sp88.z;
	} else if (spbc) {
		leftpos->x = spac.x;
		leftpos->y = spac.y;
		leftpos->z = spac.z;

		rightpos->x = spa0.x;
		rightpos->y = spa0.y;
		rightpos->z = spa0.z;
	} else if (spb8) {
		leftpos->x = sp94.x;
		leftpos->y = sp94.y;
		leftpos->z = sp94.z;

		rightpos->x = sp88.x;
		rightpos->y = sp88.y;
		rightpos->z = sp88.z;
	} else if (cd_exam_cyl_move07(chrpos, chrrooms, aimpos, sp40, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION
			&& (!arg9 || cd_exam_cyl_move01(chrpos, aimpos, chrradius, sp40, cdtypes, CHECKVERTICAL_YES, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION)) {
		result = true;
	} else {
#if VERSION >= VERSION_JPN_FINAL
		cd_get_edge(leftpos, rightpos, 14239, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
		cd_get_edge(leftpos, rightpos, 14236, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_BETA
		cd_get_edge(leftpos, rightpos, 14231, "chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
		cd_get_edge(leftpos, rightpos, 14230, "chraction.c");
#else
		cd_get_edge(leftpos, rightpos, 14243, "chraction.c");
#endif
		chr_nav_consider_swap_edges(leftpos, rightpos, &spd4);
	}

	chr_set_perim_enabled(chr, true);

	return result;
}

/**
 * Check if the path ahead contains an obstacle that the chr should route
 * around.
 *
 * If an obstacle is found, set the leftpos and rightpos coordinates to the
 * left and right corners of the object from the chr's perspective and return
 * false.
 *
 * Return true if path ahead is clear.
 *
 * This is similar to chr_nav_can_see_next_pos. The only difference is this one uses
 * the value1 and value2 variables.
 */
bool chr_nav_check_for_obstacle(struct chrdata *chr, struct coord *chrpos, RoomNum *chrrooms, struct coord *aimpos, struct coord *leftpos, struct coord *rightpos, f32 negchrradius, f32 chrradius, s32 cdtypes, bool hasobstacle)
{
	struct coord spd4;
	f32 spd0;
	f32 spcc;
	f32 spc8;
	f32 spc4;
	f32 norm;
	bool spbc = false;
	bool spb8 = false;
	struct coord spac;
	struct coord spa0;
	struct coord sp94;
	struct coord sp88;
	bool result = false;
	f32 value1;
	f32 value2;
	f32 ymax;
	f32 ymin;
	f32 radius2;
	struct coord sp6c;
	struct coord sp60;
	RoomNum sp50[8];
	RoomNum sp40[8];
	struct prop *prop = chr->prop;

	chr_get_bbox(prop, &radius2, &ymax, &ymin);

	spd4.x = aimpos->x - chrpos->x;
	spd4.y = 0.0f;
	spd4.z = aimpos->z - chrpos->z;

	if (spd4.f[0] == 0.0f && spd4.f[2] == 0.0f) {
		return true;
	}

	norm = 1.0f / sqrtf(spd4.f[0] * spd4.f[0] + spd4.f[2] * spd4.f[2]);
	spd4.x *= norm;
	spd4.z *= norm;

	spd0 = spd4.x * chrradius * 0.95f;
	spcc = spd4.z * chrradius * 0.95f;
	spc8 = spd4.x * chrradius * 1.2f;
	spc4 = spd4.z * chrradius * 1.2f;

	chr_set_perim_enabled(chr, false);

	sp6c.x = chrpos->x + spcc;
	sp6c.y = chrpos->y;
	sp6c.z = chrpos->z - spd0;
	sp60.x = (spd4.x * negchrradius) + (aimpos->x + spc4);
	sp60.y = aimpos->y;
	sp60.z = (spd4.z * negchrradius) + (aimpos->z - spc8);

	if (cd_exam_cyl_move07(chrpos, chrrooms, &sp6c, sp50, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION
			|| cd_exam_cyl_move03(&sp6c, sp50, &sp60, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION) {
		spbc = true;
#if VERSION >= VERSION_JPN_FINAL
		cd_get_edge(&spac, &spa0, 14319, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
		cd_get_edge(&spac, &spa0, 14316, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_BETA
		cd_get_edge(&spac, &spa0, 14311, "chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
		cd_get_edge(&spac, &spa0, 14310, "chraction.c");
#else
		cd_get_edge(&spac, &spa0, 14323, "chraction.c");
#endif
		chr_nav_consider_swap_edges(&spac, &spa0, &spd4);
		value1 = cd_00024e40();
	}

	sp6c.x = chrpos->x - spcc;
	sp6c.y = chrpos->y;
	sp6c.z = chrpos->z + spd0;

	sp60.x = (spd4.x * negchrradius) + (aimpos->x - spc4);
	sp60.y = aimpos->y;
	sp60.z = (spd4.z * negchrradius) + (aimpos->z + spc8);

	if (cd_exam_cyl_move07(chrpos, chrrooms, &sp6c, sp50, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION
			|| cd_exam_cyl_move03(&sp6c, chrrooms, &sp60, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) == CDRESULT_COLLISION) {
		spb8 = true;
#if VERSION >= VERSION_JPN_FINAL
		cd_get_edge(&sp94, &sp88, 14334, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
		cd_get_edge(&sp94, &sp88, 14331, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_BETA
		cd_get_edge(&sp94, &sp88, 14326, "chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
		cd_get_edge(&sp94, &sp88, 14325, "chraction.c");
#else
		cd_get_edge(&sp94, &sp88, 14338, "chraction.c");
#endif
		chr_nav_consider_swap_edges(&sp94, &sp88, &spd4);
		value2 = cd_00024e40();
	}

	if (spbc && spb8) {
		if (value1 < value2) {
			leftpos->x = spac.x;
			leftpos->y = spac.y;
			leftpos->z = spac.z;

			rightpos->x = spa0.x;
			rightpos->y = spa0.y;
			rightpos->z = spa0.z;
		} else {
			leftpos->x = sp94.x;
			leftpos->y = sp94.y;
			leftpos->z = sp94.z;

			rightpos->x = sp88.x;
			rightpos->y = sp88.y;
			rightpos->z = sp88.z;
		}
	} else if (spbc) {
		leftpos->x = spac.x;
		leftpos->y = spac.y;
		leftpos->z = spac.z;

		rightpos->x = spa0.x;
		rightpos->y = spa0.y;
		rightpos->z = spa0.z;
	} else if (spb8) {
		leftpos->x = sp94.x;
		leftpos->y = sp94.y;
		leftpos->z = sp94.z;

		rightpos->x = sp88.x;
		rightpos->y = sp88.y;
		rightpos->z = sp88.z;
	} else if (cd_exam_cyl_move07(chrpos, chrrooms, aimpos, sp40, cdtypes, 1, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION
			&& (!hasobstacle || cd_exam_cyl_move01(chrpos, aimpos, chrradius, sp40, cdtypes, CHECKVERTICAL_YES, ymax - prop->pos.y, ymin - prop->pos.y) != CDRESULT_COLLISION)) {
		result = true;
	} else {
#if VERSION >= VERSION_JPN_FINAL
		cd_get_edge(leftpos, rightpos, 14404, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
		cd_get_edge(leftpos, rightpos, 14401, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_BETA
		cd_get_edge(leftpos, rightpos, 14396, "chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
		cd_get_edge(leftpos, rightpos, 14395, "chraction.c");
#else
		cd_get_edge(leftpos, rightpos, 14408, "chraction.c");
#endif
		chr_nav_consider_swap_edges(leftpos, rightpos, &spd4);
	}

	chr_set_perim_enabled(chr, true);

	return result;
}

bool chr_nav_try_obstacle(struct chrdata *chr, struct coord *arg1, bool arg2, struct coord *arg3, f32 radius, bool arg5, struct coord *nextpos, struct waydata *waydata, f32 arg8, s32 cdtypes, s32 arg10)
{
	struct prop *prop = chr->prop;
	struct coord sp68;
	struct coord aimpos;
	f32 norm;
	f32 angle;
	struct coord sp48;
	struct coord *sp44;
	struct coord *sp40;

	if (arg2) {
		sp44 = arg1;
		sp40 = arg3;
	} else {
		sp44 = arg3;
		sp40 = arg1;
	}

	sp68.x = arg1->x - prop->pos.x;
	sp68.y = 0.0f;
	sp68.z = arg1->z - prop->pos.z;

	if (sp68.f[0] != 0.0f || sp68.f[2] != 0.0f) {
		norm = sqrtf(sp68.f[0] * sp68.f[0] + sp68.f[2] * sp68.f[2]);

		if (norm > 0.0f) {
			norm = 1.0f / norm;
			sp68.x *= radius * norm;
			sp68.z *= radius * norm;
		} else {
			norm = 1.0f;
			sp68.z = radius * norm;
		}
	} else {
		norm = 1.0f;
		sp68.z = radius * norm;
	}

	if (radius * norm > 1.0f) {
		angle = BADDTOR(45);
	} else {
		angle = acosf(radius * norm);
	}

	if (!arg2 && angle != 0.0f) {
		angle = BADDTOR(360) - angle;
	}

	sp48.x = -cosf(angle) * sp68.f[0] + sinf(angle) * sp68.f[2];
	sp48.y = 0.0f;
	sp48.z = -sinf(angle) * sp68.f[0] - cosf(angle) * sp68.f[2];

	aimpos.x = arg1->x + sp48.f[0];
	aimpos.y = arg1->y;
	aimpos.z = arg1->z + sp48.f[2];

	if (chr_nav_can_see_next_pos(chr, &prop->pos, prop->rooms, &aimpos, sp44, sp40, arg8, chr->radius, cdtypes, 1)) {
		if (!arg5 || chr_can_move_directly_to_pos(chr, &prop->pos, prop->rooms, &aimpos, nextpos, cdtypes)) {
			if (arg10) {
				waydata->gotaimposobj = true;
				waydata->aimposobj.x = aimpos.x;
				waydata->aimposobj.y = aimpos.y;
				waydata->aimposobj.z = aimpos.z;
			} else {
				waydata->gotaimpos = true;
				waydata->aimpos.x = aimpos.x;
				waydata->aimpos.y = aimpos.y;
				waydata->aimpos.z = aimpos.z;
			}

			return true;
		}
	}

	return false;
}

#if VERSION >=  VERSION_NTSC_1_0
const char var7f1a8be8[] = "CHARS -> FRAMETIMESCALEI(240)  = %d";
const char var7f1a8c0c[] = "CHARS -> numseenbond1      \t= %d/%d";
const char var7f1a8c30[] = "CHARS -> numseenbond2      \t= %d/%d";
const char var7f1a8c54[] = "CHARS -> numseenbond3      \t= %d/%d";
const char var7f1a8c78[] = "CHARS -> numseenbond       \t= %d/%d";
#endif

const char var7f1a8c9c[] = "CHARS -> DEAD = %d/%d";
const char var7f1a8cb4[] = "chrdisttopad : %x -> %d : Dist=%f";

/**
 * Check for doors in front of the chr and open them if possible.
 *
 * I'm guessing the coord argument is a position in front of the chr, and the
 * collision check is looking for doors between the chr and that point.
 *
 * - Multiplayer simulants can open any doors
 * - Solo chrs cannot open doors with OBJFLAG2_AICANNOTUSE
 * - In ntsc-beta, solo chrs can bring down lasers even if they have that flag
 *
 * The chr must be within 200cm of the door unless it's a laser.
 */
struct prop *chr_open_door(struct chrdata *chr, struct coord *rangepos)
{
	struct prop *doorprop = NULL;

	if (cd_exam_cyl_move03(&chr->prop->pos, chr->prop->rooms, rangepos,
				CDTYPE_BG | CDTYPE_CLOSEDDOORS | CDTYPE_AJARDOORS,
				1, 0, 0) == CDRESULT_COLLISION) {
		doorprop = cd_get_obstacle_prop();
	}

	if (doorprop) {
		struct doorobj *door = doorprop->door;

#if VERSION >= VERSION_NTSC_1_0
		if (chr->aibot || (door->base.flags2 & OBJFLAG2_AICANNOTUSE) == 0)
#else
		if (chr->aibot
				|| (door->base.flags2 & OBJFLAG2_AICANNOTUSE) == 0
				|| (door->doorflags & DOORFLAG_DAMAGEONCONTACT))
#endif
		{
			f32 xdiff = doorprop->pos.x - chr->prop->pos.x;
			f32 zdiff = doorprop->pos.z - chr->prop->pos.z;

			if (xdiff * xdiff + zdiff * zdiff < 200 * 200 || (door->doorflags & DOORFLAG_DAMAGEONCONTACT)) {
				chr_gopos_clear_restart_ttl(chr);
				doors_choose_swing_direction(chr->prop, doorprop->door);

				if (!door_call_lift(doorprop, false)) {
					doors_request_mode(doorprop->door, DOORMODE_OPENING);
				}
			} else {
				doorprop = NULL;
			}
		} else {
			doorprop = NULL;
		}
	}

	return doorprop;
}

/**
 * Tick a chr's navigation process when they are using the "main" method of
 * navigating.
 *
 * Main refers to the chr being on screen or close to on screen, where all
 * collision checks are enabled. This is opposed to the magic method, where
 * they are off screen and the collision checks are skipped.
 *
 * The chr will generally move towards the nextpos, which is the position of the
 * next pad/corner in the route, and open doors along the way. However they must
 * also check for obstacles ahead of them such as other chrs or doors which have
 * opened into their path. If one is found, the chr routes around the obstacle
 * and then to the next pos again.
 */
void chr_nav_tick_main(struct chrdata *chr, struct coord *nextpos, struct waydata *waydata, bool arg3)
{
	struct prop *prop = chr->prop;
	struct coord sp100;
	struct coord spf4;
	s32 i;
	s32 cdtypes;

	// By default, do collision checks for everything except doors that AI can
	// open, but if they're blocking a door then include all doors
	cdtypes = (CDTYPE_ALL & ~CDTYPE_DOORS) | CDTYPE_DOORSLOCKEDTOAI;

	if (chr->hidden & CHRHFLAG_BLOCKINGDOOR) {
		cdtypes = CDTYPE_ALL;
	}

	for (i = 0; i < 1; i++) {
		if (0.0f);

		if (waydata->mode == WAYMODE_INIT || waydata->mode == WAYMODE_RETRY) {
			sp100.x = nextpos->x;
			sp100.y = nextpos->y;
			sp100.z = nextpos->z;

			// Check to see if the chr can see the next pad. This is almost
			// always true, but if the chr has tried to avoid an object they
			// may have gone behind a wall and can't see the pad any more.
			if (chr_nav_can_see_next_pos(chr, &prop->pos, prop->rooms, &sp100, &waydata->obstacleleft, &waydata->obstacleright, -chr->radius, chr->radius, CDTYPE_PATHBLOCKER | CDTYPE_BG, arg3)) {
				// Can see the next pad
				waydata->gotaimpos = true;
				waydata->aimpos.x = sp100.x;
				waydata->aimpos.y = sp100.y;
				waydata->aimpos.z = sp100.z;
				waydata->mode = WAYMODE_HAVEAIMPOS;
			} else if (waydata->mode == WAYMODE_INIT) {
				waydata->mode = WAYMODE_LOST1;
				waydata->iter = 0;
			} else if (waydata->mode == WAYMODE_RETRY) {
				waydata->mode = WAYMODE_LOST2;
				waydata->iter = 0;
			}
		} else if (waydata->mode == WAYMODE_LOST1) {
			// Chr has tried to avoid an obstacle, and in the process can no
			// longer see the next pad. The chr will try to get back on the
			// route by navigating to either side of the obstacle they were
			// trying to avoid.
			f32 wantclearance = chr->radius * 1.26f;

			if (chr_nav_try_obstacle(chr, &waydata->obstacleleft, true, &spf4, wantclearance, true, nextpos, waydata, 0, CDTYPE_PATHBLOCKER | CDTYPE_BG, 0)) {
				// Will go to left side
				waydata->mode = WAYMODE_HAVEAIMPOS;
			} else if (chr_nav_try_obstacle(chr, &waydata->obstacleright, false, &spf4, wantclearance, true, nextpos, waydata, 0, CDTYPE_PATHBLOCKER | CDTYPE_BG, 0)) {
				// Will go to right side
				waydata->mode = WAYMODE_HAVEAIMPOS;
			} else {
				// Can't see the obstacle either!
				// Remain in LOST1 for 5 iterations to see if line of sight
				// comes back. If not, retry the next pad again.
				waydata->iter++;

				if (waydata->iter > 5) {
					waydata->mode = WAYMODE_RETRY;
				}
			}
		} else if (waydata->mode == WAYMODE_LOST2) {
			// Chr has gone through LOST1 without seeing the obstacle, then
			// tried to find the next pad but can't see that either. Try
			// navigating to the obstacle again, but with different arguments.
			// This is a more desparate attempt at returning to the path.
			f32 wantclearance = chr->radius * 1.26f;
			u32 stack;

			if (chr_nav_try_obstacle(chr, &waydata->obstacleleft, true, &spf4, wantclearance, false, NULL, waydata, 0, CDTYPE_PATHBLOCKER | CDTYPE_BG, 0)) {
				// Will go to left side
				waydata->mode = WAYMODE_HAVEAIMPOS;
			} else if (chr_nav_try_obstacle(chr, &waydata->obstacleright, false, &spf4, wantclearance, false, NULL, waydata, 0, CDTYPE_PATHBLOCKER | CDTYPE_BG, 0)) {
				// Will go to right side
				waydata->mode = WAYMODE_HAVEAIMPOS;
			} else {
				// Still can't see the obstacle using the desparate check.
				// Remain in LOST2 for 5 iterations to see if line of sight
				// comes back.
				waydata->iter++;

				if (waydata->iter > 5) {
					// Clear any references to the obstacle we were trying to
					// avoid and attempt to restart the process. I don't think
					// this is likely to work, but there's nothing else that can
					// be done.
					waydata->gotaimposobj = waydata->gotaimpos = false;
					waydata->aimposobj.x = waydata->aimpos.x = nextpos->x;
					waydata->aimposobj.y = waydata->aimpos.y = nextpos->y;
					waydata->aimposobj.z = waydata->aimpos.z = nextpos->z;
					waydata->mode = WAYMODE_INIT;
				}
			}
		} else if (waydata->mode == WAYMODE_HAVEAIMPOS) {
			// Chr is happily walking towards a position (aim pos).
			// The position could be the next pad, or it could be an arbitrary
			// position next to an obstacle that has to be routed around.
			// Either way, check that there's no new obstacles between the chr
			// and their aim pos.
			bool hasobstacle = true;

			if (!arg3
					&& nextpos->x == waydata->aimpos.x
					&& nextpos->y == waydata->aimpos.y
					&& nextpos->z == waydata->aimpos.z) {
				hasobstacle = false;
			}

			if (chr_nav_check_for_obstacle(chr, &prop->pos, prop->rooms, &waydata->aimpos, &waydata->obstacleleft, &waydata->obstacleright, -chr->radius, chr->radius, cdtypes, hasobstacle)) {
				// No obstacle ahead
				waydata->gotaimposobj = true;
				waydata->mode = WAYMODE_INIT;
				waydata->aimposobj.x = waydata->aimpos.x;
				waydata->aimposobj.y = waydata->aimpos.y;
				waydata->aimposobj.z = waydata->aimpos.z;
			} else {
				// Obstacle ahead
				waydata->mode = WAYMODE_NEWOBSTACLE;
				waydata->iter = 0;
			}
		} else if (waydata->mode == WAYMODE_NEWOBSTACLE) {
			// Chr has just noticed an obstacle ahead of them which they need to
			// route around. The obstacleleft and obstacleright coordinates are
			// already populated and they indicate the left and right corners of
			// the object. This part of the code decides which side to use and
			// calculates some turning angles.
			f32 f20;
			u32 stack;
			f32 spd0;
			f32 spcc;
			u32 stack2;
			f32 f24 = chr->radius * 1.26f;

			f20 = atan2f(waydata->aimpos.x - prop->pos.x, waydata->aimpos.z - prop->pos.z);
			spd0 = f20 - atan2f(waydata->obstacleleft.x - prop->pos.x, waydata->obstacleleft.z - prop->pos.z);
			spcc = f20 - atan2f(waydata->obstacleright.x - prop->pos.x, waydata->obstacleright.z - prop->pos.z);

			if (spd0 < 0) {
				spd0 += BADDTOR(360);
			}

			if (spd0 >= BADDTOR(180)) {
				spd0 -= BADDTOR(360);
			}

			if (spd0 < 0) {
				spd0 = -spd0;
			}

			if (spcc < 0) {
				spcc += BADDTOR(360);
			}

			if (spcc >= BADDTOR(180)) {
				spcc -= BADDTOR(360);
			}

			if (spcc < 0) {
				spcc = -spcc;
			}

			if (spd0 < spcc) {
				f32 spc0;
				f32 spbc;
				u32 stack;
				f32 f22 = f24 * 1.1f;

				if (chr_nav_try_obstacle(chr, &waydata->obstacleleft, true, &spf4, f24, false, NULL, waydata, f22, cdtypes, 1)) {
					waydata->mode = WAYMODE_INIT;
					break;
				}

				spc0 = f20 - atan2f(waydata->obstacleleft.x - prop->pos.x, waydata->obstacleleft.z - prop->pos.z);
				spbc = f20 - atan2f(spf4.x - prop->pos.x, spf4.z - prop->pos.z);

				if (spc0 < 0) {
					spc0 += BADDTOR(360);
				}

				if (spc0 >= BADDTOR(180)) {
					spc0 -= BADDTOR(360);
				}

				if (spc0 < 0) {
					spc0 = -spc0;
				}

				if (spbc < 0) {
					spbc += BADDTOR(360);
				}

				if (spbc >= BADDTOR(180)) {
					spbc -= BADDTOR(360);
				}

				if (spbc < 0) {
					spbc = -spbc;
				}

				if (spbc < spc0) {
					if (chr_nav_try_obstacle(chr, &spf4, false, &spf4, f24, false, NULL, waydata, f22, cdtypes, 1)) {
						waydata->mode = WAYMODE_INIT;
						break;
					}
				}
			} else {
				f32 spb0;
				f32 spac;
				f32 f22 = f24 * 1.1f;

				if (chr_nav_try_obstacle(chr, &waydata->obstacleright, false, &spf4, f24, false, NULL, waydata, f22, cdtypes, 1)) {
					waydata->mode = WAYMODE_INIT;
					break;
				}

				spb0 = f20 - atan2f(waydata->obstacleright.x - prop->pos.x, waydata->obstacleright.z - prop->pos.z);
				spac = f20 - atan2f(spf4.x - prop->pos.x, spf4.z - prop->pos.z);

				if (spb0 < 0) {
					spb0 += BADDTOR(360);
				}

				if (spb0 >= BADDTOR(180)) {
					spb0 -= BADDTOR(360);
				}

				if (spb0 < 0) {
					spb0 = -spb0;
				}

				if (spac < 0) {
					spac += BADDTOR(360);
				}

				if (spac >= BADDTOR(180)) {
					spac -= BADDTOR(360);
				}

				if (spac < 0) {
					spac = -spac;
				}

				if (spac < spb0) {
					if (chr_nav_try_obstacle(chr, &spf4, true, &spf4, f24, false, NULL, waydata, f22, cdtypes, 1)) {
						waydata->mode = WAYMODE_INIT;
						break;
					}
				}
			}

			waydata->iter++;

			if (waydata->iter > 5) {
				waydata->gotaimposobj = false;
				waydata->mode = WAYMODE_INIT;
			}
		}
	}

	if (!waydata->gotaimposobj) {
		waydata->aimposobj.x = waydata->aimpos.x;
		waydata->aimposobj.y = waydata->aimpos.y;
		waydata->aimposobj.z = waydata->aimpos.z;
	}

	// Every 10 ticks, attempt to open any door in front of the chr
	if (waydata->age % 10 == 0) {
		struct prop *doorprop = chr_open_door(chr, &waydata->aimposobj);

		// Consider returning to stand animation while door is opening
		if (doorprop
				&& chr->aibot == NULL
				&& (chr->hidden & CHRHFLAG_BLOCKINGDOOR) == 0
				&& !chr_gopos_is_waiting(chr)) {
			chr_choose_stand_animation(chr, 16);
			chr->lastmoveok60 = g_Vars.lvframe60;
		}

		// Resume moving if there's no longer a door in the way
		// or if the chr is blocking the door
		if (!doorprop || (chr->hidden & CHRHFLAG_BLOCKINGDOOR)) {
			if (chr->aibot == NULL
					&& chr_gopos_is_waiting(chr)
					&& chr->liftaction != LIFTACTION_WAITINGFORLIFT
					&& chr->liftaction != LIFTACTION_WAITINGONLIFT) {
				if (chr->actiontype == ACT_PATROL) {
					chr_patrol_choose_animation(chr);
				} else {
					chr_gopos_choose_animation(chr);
				}
			}

			if (!doorprop) {
				chr->hidden &= ~CHRHFLAG_BLOCKINGDOOR;
			}
		}
	}

	// Handle turning and anim speed
	if (chr->actiontype == ACT_PATROL) {
		chr_turn_toward(chr, &waydata->aimposobj, GOPOSFLAG_WALK, &chr->act_patrol.turnspeed);
	} else {
		chr->unk32c_21 = chr->act_gopos.turnspeed != 0;

		chr_turn_toward(chr, &waydata->aimposobj, chr->act_gopos.flags, &chr->act_gopos.turnspeed);

		if (chr->aibot == NULL && !chr_gopos_is_waiting(chr)) {
			if ((chr->act_gopos.flags & GOPOSMASK_SPEED) == GOPOSFLAG_RUN) {
				if (chr->act_gopos.turnspeed) {
					if (!chr->unk32c_21) {
						model_set_anim_speed(chr->model, 0.25f, 8);
					}
				} else {
					if (chr->unk32c_21) {
						if (chr->chrflags & CHRCFLAG_RUNFASTER) {
							model_set_anim_speed(chr->model, 0.65f, 32);
						} else {
							model_set_anim_speed(chr->model, 0.5f, 32);
						}
					}
				}
			} else if ((chr->act_gopos.flags & GOPOSMASK_SPEED) == GOPOSFLAG_JOG) {
				if (chr->act_gopos.turnspeed) {
					model_set_anim_speed(chr->model, 0.4f, 0);
				} else {
					model_set_anim_speed(chr->model, 0.5f, 0);
				}
			}
		}
	}
}

bool chr_gopos_update_lift_action(struct chrdata *chr, u32 curpadflags, bool arg2, bool arrivingatlift, s16 curpadnum, s32 nextpadnum)
{
	bool advance = false;
	struct pad nextpad;
	u32 nextpadflags = 0;
	f32 nextground;
	f32 lifty;
	struct liftobj *lift;
	struct prop *liftprop = lift_find_by_pad(curpadnum);
	u32 stack;

	if (!liftprop) {
		return false;
	}

	lift = (struct liftobj *) liftprop->obj;

	lifty = lift_get_y(lift);

	if (nextpadnum >= 0) {
		pad_unpack(nextpadnum, PADFIELD_POS | PADFIELD_ROOM | PADFIELD_FLAGS, &nextpad);
		nextpadflags = nextpad.flags;
	}

	if (curpadflags & PADFLAG_AIWAITLIFT) {
		if (nextpadflags & PADFLAG_AIONLIFT) {
			if (arrivingatlift || chr->liftaction == LIFTACTION_WAITINGFORLIFT) {
				if (nextpadflags);

				// Begin entering lift if lift is under 40cm above this level
				advance = (lifty <= chr->manground + 40);

				// ...and (if solo mode) lift is over 1m under this level
				// (this logic allows MP simulants to drop down onto lifts)
				if (!g_Vars.normmplayerisrunning && advance) {
					advance = (lifty > chr->manground - 100);
				}

				// ...and if the lift has a door, is at least halfway open
				if (advance && lift->doors[lift->levelcur] && lift->doors[lift->levelcur]->frac < 0.5f) {
					advance = false;
				}
			}

			if (!advance) {
				if (arrivingatlift && chr->liftaction != LIFTACTION_WAITINGFORLIFT) {
					// Just arrived at lift
					chr->liftaction = LIFTACTION_WAITINGFORLIFT;

					chr_choose_stand_animation(chr, 16);

					if (nextpadnum >= 0) {
						// Call the lift
						chr_open_door(chr, &nextpad.pos);
					}
				}
			} else {
				// Enter lift
				chr->liftaction = LIFTACTION_NOTUSINGLIFT;

				if (chr_gopos_is_waiting(chr)) {
					if (chr->actiontype == ACT_PATROL) {
						chr_patrol_choose_animation(chr);
					} else {
						chr_gopos_choose_animation(chr);
					}
				}
			}
		} else {
			// On a wait lift pad, but the next pad in the route is not on the
			// lift, so the chr is running past the lift without using it.
			if (arrivingatlift) {
				advance = true;
				chr->liftaction = LIFTACTION_NOTUSINGLIFT;
			}
		}
	} else if (curpadflags & PADFLAG_AIONLIFT) {
		if (nextpadflags & PADFLAG_AIWAITLIFT) {
			// Waiting for door to close or lift to arrive at destination
			if (arg2 || chr->liftaction == LIFTACTION_WAITINGONLIFT) {
				// Continue waiting
				RoomNum rooms[] = {0, -1};
				u32 stack2;

				rooms[0] = nextpad.room;

				nextground = cd_find_floor_y_colour_type_at_pos(&nextpad.pos, rooms, NULL, NULL);

				// Begin exiting lift if lift is 30cm under destination or higher
				advance = (lifty >= nextground - 30);

				// ...and (if solo) lift is under 1m above destination
				if (!g_Vars.normmplayerisrunning && advance) {
					advance = (lifty < nextground + 100);
				}

				// ...and if the lift has a door, is at least halfway open
				if (advance && lift->doors[lift->levelcur] && lift->doors[lift->levelcur]->frac < 0.5f) {
					advance = false;
				}
			}

			if (!advance) {
				if (arg2 && chr->liftaction != LIFTACTION_WAITINGONLIFT) {
					// Just arrived inside lift
					chr->liftaction = LIFTACTION_WAITINGONLIFT;
					chr_choose_stand_animation(chr, 16);
				}
			} else {
				// Start disembarking
				chr->liftaction = LIFTACTION_ONLIFT;

				if (chr_gopos_is_waiting(chr)) {
					if (chr->actiontype == ACT_PATROL) {
						chr_patrol_choose_animation(chr);
					} else {
						chr_gopos_choose_animation(chr);
					}
				}
			}
		} else {
			// Not our stop? Not sure why advance is true here. I guess the chr
			// can't go anywhere anyway because the next pad is above or below
			// them.
			if (arg2) {
				advance = true;
				chr->liftaction = LIFTACTION_ONLIFT;
			}
		}
	}

	return advance;
}

s32 chr_is_using_lift(struct chrdata *chr)
{
	return (chr->actiontype == ACT_GOPOS || chr->actiontype == ACT_PATROL) && chr->liftaction > 0;
}

s16 chr_gopos_get_next_pad_num(struct chrdata *chr)
{
	if (chr->act_gopos.waypoints[chr->act_gopos.curindex + 1]) {
		return chr->act_gopos.waypoints[chr->act_gopos.curindex + 1]->padnum;
	}

	return -1;
}

void chr_tick_gopos(struct chrdata *chr)
{
	struct waypoint *waypoint;
	struct coord nextpos;
	RoomNum nextrooms[8];
	struct prop *prop = chr->prop;
	bool enteringmagic = false;
	struct pad pad;
	bool sp240 = true;
	struct coord curwppos;
	RoomNum curwprooms[8];
	u32 curwpflags;

	chr->act_gopos.flags &= ~(GOPOSFLAG_CROUCH | GOPOSFLAG_DUCK);

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_gopos_choose_animation(chr);

		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	chr->act_gopos.waydata.age++;

	// If stuck for 1 second
	if (chr->lastmoveok60 < g_Vars.lvframe60 - TICKS(60)) {
#if VERSION >= VERSION_NTSC_1_0
		if (chr->goposforce >= 0) {
			// Try and warp the chr past whatever obstacle is blocking them?
			struct coord sp196 = {0, 0, 0};

			chr_damage_by_dizziness(chr, 1, &sp196, NULL, NULL);

			chr->lastmoveok60 = g_Vars.lvframe60;
			return;
		}
#endif

		// Goposforce was not set - restart the action to try and find a new route
		chr_go_to_room_pos(chr, &chr->act_gopos.endpos, chr->act_gopos.endrooms, chr->act_gopos.flags);
	}

	chr_gopos_consider_restart(chr);
	chr_gopos_get_cur_waypoint_info_with_flags(chr, &curwppos, curwprooms, &curwpflags);

	// If magic mode ended over 3 seconds ago, not multiplayer, not in view of
	// eyespy, pad is nothing special and not in lift, then enter the magic move
	// mode.
	if (chr->act_gopos.waydata.mode != WAYMODE_MAGIC
			&& chr->act_gopos.waydata.lastvisible60 + TICKS(180) < g_Vars.lvframe60
			&& g_Vars.normmplayerisrunning == false
			&& chr_is_room_offscreen(chr, &curwppos, curwprooms) // related to eyespy
			&& (curwpflags & (PADFLAG_AIWAITLIFT | PADFLAG_AIONLIFT)) == 0
			&& chr->inlift == false) {
		enteringmagic = true;
		chr_gopos_init_magic(chr, &chr->act_gopos.waydata, &curwppos, &prop->pos);
	}

#if VERSION >= VERSION_NTSC_1_0
	if (g_NumChrsSeenPlayerRecently2 >= 9
			&& (chr->hidden & CHRHFLAG_BASICGUARD)
			&& (chr->flags & CHRFLAG0_ACTIVATEALARM) == 0) {
		chr_stop(chr);
		return;
	}
#else
	if (g_NumChrsSeenPlayerRecently2 >= 10) {
		chr_stop(chr);
		return;
	}
#endif

	// If goposforce is set then decrease it on each tick. If it's reached -1
	// then stop the chr. I guess goposforce is not only used to warp past
	// obstacles, but is also used as a run countdown timer.
#if VERSION >= VERSION_NTSC_1_0
	if (chr->goposforce >= 0) {
		chr->goposforce -= g_Vars.lvupdate60;

		if (chr->goposforce < 0) {
			chr_stop(chr);
			return;
		}
	}
#endif

	// Check if chr needs to exit magic mode
	if (chr->act_gopos.waydata.mode == WAYMODE_MAGIC) {
		if ((!enteringmagic && ((prop->flags & (PROPFLAG_ONANYSCREENPREVTICK | PROPFLAG_ONANYSCREENTHISTICK | PROPFLAG_ONTHISSCREENTHISTICK)) || !chr_is_room_offscreen(chr, &curwppos, curwprooms)))
				|| (curwpflags & (PADFLAG_AIWAITLIFT | PADFLAG_AIONLIFT))
				|| chr->inlift) {
			// Exiting magic mode
			chr_gopos_init_expensive(chr);
			chr->act_gopos.waydata.lastvisible60 = g_Vars.lvframe60;
			return;
		}

		chr_nav_tick_magic(chr, &chr->act_gopos.waydata, chr_get_run_speed(chr), &curwppos, curwprooms);
	} else {
		bool advance = false;
		bool arrivingxyz;
		bool arrivingxz;
		f32 sp180;
		f32 sp176;
		f32 sp172;
		f32 sp168;
		bool candosomething;
		f32 sp160;
		f32 sp156;
		struct waypoint *next;
		struct pad pad2;

		waypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex];

		if (waypoint) {
			pad_unpack(waypoint->padnum, PADFIELD_FLAGS | PADFIELD_POS, &pad);

			arrivingxyz = pos_is_arriving_at_pos(&chr->prevpos, &prop->pos, &pad.pos, 30);
			arrivingxz = pos_is_arriving_laterally_at_pos(&chr->prevpos, &prop->pos, &pad.pos, 30);

			if (pad.flags & PADFLAG_AICROUCH) {
				chr->act_gopos.flags |= GOPOSFLAG_CROUCH;
			} else if (pad.flags & PADFLAG_AIDUCK) {
				chr->act_gopos.flags |= GOPOSFLAG_DUCK;
			}

			if ((pad.flags & PADFLAG_AIWAITLIFT) || (pad.flags & PADFLAG_AIONLIFT)) {
				advance = chr_gopos_update_lift_action(chr, pad.flags, arrivingxz, arrivingxyz, waypoint->padnum, chr_gopos_get_next_pad_num(chr));
			} else {
				if (arrivingxyz || (arrivingxz && (chr->inlift || (pad.flags & PADFLAG_AIIGNOREY)))) {
					advance = true;
				}
			}
		} else {
			// No more waypoints - chr is finished
			if (pos_is_arriving_at_pos(&chr->prevpos, &prop->pos, &chr->act_gopos.endpos, 30) ||
					(chr->inlift && pos_is_arriving_laterally_at_pos(&chr->prevpos, &prop->pos, &chr->act_gopos.endpos, 30))) {
				if (chr->act_gopos.flags & GOPOSFLAG_FORPATHSTART) {
					chr_try_start_patrol(chr);
					return;
				}

				chr_stop(chr);
				return;
			}
		}

		if (advance) {
			chr_gopos_advance_waypoint(chr);
		}

		// Every 10 ticks: Check something a couple of waypoints ahead
		// This might be checking if the chr has line of sight to that pad and can
		// walk straight to it.
		if (chr->act_gopos.waydata.age % 10 == 5 || (chr->act_gopos.flags & GOPOSFLAG_INIT)) {
			// Load waypoint that the chr is running to
			waypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex];

			if (waypoint) {
				pad_unpack(waypoint->padnum, PADFIELD_FLAGS, &pad);

				if ((pad.flags & PADFLAG_AIWALKDIRECT) == 0) {
					// The waypoint the chr is running to doesn't have
					// PADFLAG_AIWALKDIRECT, so the chr is able to ignore it and run
					// towards the next one if it's in sight.

					// Load the next waypoint after the one the chr is running to
					waypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex + 1];

					if (waypoint) {
						pad_unpack(waypoint->padnum, PADFIELD_FLAGS, &pad);

						if ((pad.flags & PADFLAG_AIWALKDIRECT) == 0) {
							// And this one doesn't have PADFLAG_AIWALKDIRECT either,
							// so the chr can consider skipping this one too.
							waypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex + 2];

							if (waypoint) {
								pad_unpack(waypoint->padnum, PADFIELD_ROOM | PADFIELD_POS, &pad);

								nextpos.x = pad.pos.x;
								nextpos.y = pad.pos.y;
								nextpos.z = pad.pos.z;

								nextrooms[0] = pad.room;
								nextrooms[1] = -1;
							} else {
								nextpos.x = chr->act_gopos.endpos.x;
								nextpos.y = chr->act_gopos.endpos.y;
								nextpos.z = chr->act_gopos.endpos.z;

								rooms_copy(chr->act_gopos.endrooms, nextrooms);
							}

							// Some bbox related check
							if (chr_prop_can_move_to_pos_without_nav(chr, &prop->pos, prop->rooms, &nextpos, nextrooms, NULL, chr->radius * 1.2f, CDTYPE_PATHBLOCKER | CDTYPE_BG)) {
								chr_gopos_advance_waypoint(chr);
								chr_gopos_advance_waypoint(chr);
							}
						}
					}
				}
			}
		}

		if (chr->act_gopos.waydata.age % 10 == 0 || (chr->act_gopos.flags & GOPOSFLAG_INIT)) {
			waypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex];

			if (waypoint) {
				candosomething = (chr->act_gopos.flags & GOPOSFLAG_INIT) != 0;
				pad_unpack(waypoint->padnum, PADFIELD_FLAGS | PADFIELD_POS, &pad);

				next = chr->act_gopos.waypoints[chr->act_gopos.curindex + 1];

				if (next) {
					pad_unpack(next->padnum, PADFIELD_ROOM | PADFIELD_POS, &pad2);

					if ((pad.flags & (PADFLAG_AIWAITLIFT | PADFLAG_AIONLIFT))
							&& (pad2.flags & (PADFLAG_AIWAITLIFT | PADFLAG_AIONLIFT))) {
						candosomething = false;
					}
				}

				if ((pad.flags & PADFLAG_AIWALKDIRECT) == 0 || candosomething) {
					if (next) {
						nextpos.x = pad2.pos.x;
						nextpos.y = pad2.pos.y;
						nextpos.z = pad2.pos.z;

						nextrooms[0] = pad2.room;
						nextrooms[1] = -1;
					} else {
						nextpos.x = chr->act_gopos.endpos.x;
						nextpos.y = chr->act_gopos.endpos.y;
						nextpos.z = chr->act_gopos.endpos.z;

						rooms_copy(chr->act_gopos.endrooms, nextrooms);
					}

					// I suspect this is making the chr turn to face the next pad
					if ((pad.flags & PADFLAG_AIWALKDIRECT) && candosomething) {
						if (true) {
							sp180 = prop->pos.x - pad.pos.x;
							sp176 = prop->pos.z - pad.pos.z;
							sp172 = nextpos.x - pad.pos.x;
							sp168 = nextpos.z - pad.pos.z;
						}

						sp156 = sqrtf((sp180 * sp180 + sp176 * sp176) * (sp172 * sp172 + sp168 * sp168));

						if (sp156 > 0) {
							sp160 = acosf((sp180 * sp172 + sp176 * sp168) / sp156);

							if (sp160 < BADDTOR(45) || sp160 > BADDTOR(315)) {
								if (chr_prop_can_move_to_pos_without_nav(chr, &prop->pos, prop->rooms, &nextpos, nextrooms, NULL, chr->radius * 1.2f, CDTYPE_PATHBLOCKER | CDTYPE_BG)) {
									chr_gopos_advance_waypoint(chr);
								}
							}
						}
					} else {
						if (chr_prop_can_move_to_pos_without_nav(chr, &prop->pos, prop->rooms, &nextpos, nextrooms, NULL, chr->radius * 1.2f, CDTYPE_PATHBLOCKER | CDTYPE_BG)) {
							chr_gopos_advance_waypoint(chr);
						}
					}
				}
			}

			chr->act_gopos.flags &= ~GOPOSFLAG_INIT;
		}

		waypoint = chr->act_gopos.waypoints[chr->act_gopos.curindex];

		if (waypoint) {
			pad_unpack(waypoint->padnum, PADFIELD_POS, &pad);

			nextpos.x = pad.pos.x;
			nextpos.y = pad.pos.y;
			nextpos.z = pad.pos.z;
		} else {
			nextpos.x = chr->act_gopos.endpos.x;
			nextpos.y = chr->act_gopos.endpos.y;
			nextpos.z = chr->act_gopos.endpos.z;

			if (chr->aibot && chr->myaction == MA_AIBOTGETITEM) {
				sp240 = false;
			}
		}

		chr_nav_tick_main(chr, &nextpos, &chr->act_gopos.waydata, sp240);
	}
}

void chr_tick_patrol(struct chrdata *chr)
{
	struct prop *prop = chr->prop;
	bool enteringmagic;
	u32 flags = 0;
	struct coord sp58;
	RoomNum sp48[8];
	s32 arrivinglaterally;
	s32 arriving;
	bool advance;

	enteringmagic = 0;

	if (chr->hidden & CHRHFLAG_NEEDANIM) {
		if (model_is_anim_merging(chr->model)) {
			return;
		}

		chr_patrol_choose_animation(chr);
		chr->hidden &= ~CHRHFLAG_NEEDANIM;
	}

	chr_patrol_get_cur_waypoint_info_with_flags(chr, &sp58, sp48, &flags);

	chr->act_patrol.waydata.age++;

	// Consider starting magic
	if (chr->act_patrol.waydata.mode != WAYMODE_MAGIC
			&& g_Vars.lvframe60 > chr->act_patrol.waydata.lastvisible60 + TICKS(180)
			&& !g_Vars.normmplayerisrunning
			&& chr_is_room_offscreen(chr, &sp58, sp48)
			&& (flags & (PADFLAG_AIWAITLIFT | PADFLAG_AIONLIFT)) == 0
			&& !chr->inlift) {
		enteringmagic = true;
		chr_gopos_init_magic(chr, &chr->act_patrol.waydata, &sp58, &prop->pos);
	}

	if (chr->act_patrol.waydata.mode == WAYMODE_MAGIC) {
		if ((!enteringmagic && ((prop->flags & (PROPFLAG_ONTHISSCREENTHISTICK | PROPFLAG_ONANYSCREENTHISTICK | PROPFLAG_ONANYSCREENPREVTICK)) || !chr_is_room_offscreen(chr, &sp58, sp48)))
				|| (flags & (PADFLAG_AIWAITLIFT | PADFLAG_AIONLIFT))
				|| chr->inlift) {
			// Exit magic for lifts
			chr->act_patrol.waydata.lastvisible60 = g_Vars.lvframe60;
			chr_patrol_init_expensive(chr);
		} else {
			// Continue magic
			chr_nav_tick_magic(chr, &chr->act_patrol.waydata, chr_get_run_speed(chr), &sp58, sp48);
		}

		footstep_check_magic(chr);
	} else {
		arrivinglaterally = pos_is_arriving_laterally_at_pos(&chr->prevpos, &prop->pos, &sp58, 30);
		arriving = pos_is_arriving_at_pos(&chr->prevpos, &prop->pos, &sp58, 30);
		advance = false;

		if ((flags & PADFLAG_AIWAITLIFT) || (flags & PADFLAG_AIONLIFT)) {
			advance = chr_gopos_update_lift_action(chr, flags, arrivinglaterally, arriving,
					chr_patrol_calculate_pad_num(chr, 0),
					chr_patrol_calculate_pad_num(chr, 1));
		} else if (arriving) {
			advance = true;
		}

		if (advance) {
			chr_patrol_advance_waypoint(chr);
			chr_patrol_get_cur_waypoint_info(chr, &sp58, sp48);
		}

		chr_nav_tick_main(chr, &sp58, &chr->act_patrol.waydata, true);
		footstep_check_default(chr);
	}
}

bool chr_try_skjump(struct chrdata *chr, u8 pouncebits, u8 arg2, s32 arg3, u8 arg4)
{
	if (chr && chr->actiontype != ACT_SKJUMP
			&& chr_is_ready_for_orders(chr)
			&& CHRRACE(chr) == RACE_SKEDAR) {
		return chr_skjump(chr, pouncebits, arg2, arg3, arg4);
	}

	return false;
}

bool chr_skjump(struct chrdata *chr, u8 pouncebits, u8 arg2, s32 arg3, u8 arg4)
{
	f32 radius;
	f32 ymax;
	f32 ymin;
	struct prop *prop = chr->prop;
	struct prop *target = chr_get_target_prop(chr);
	bool iVar2;
	f32 distance = chr_get_distance_to_coord(chr, &target->pos);
	f32 diffs[2];
	f32 thing;
	s32 time60;

	if (distance < 200 || distance > 550 || !target) {
		return false;
	}

	chr_get_bbox(prop, &radius, &ymax, &ymin);
	chr_set_perim_enabled(chr, false);
	prop_set_perim_enabled(target, false);
	iVar2 = cd_test_cyl_move01(&prop->pos, prop->rooms, &target->pos,
			CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PATHBLOCKER | CDTYPE_BG,
			1, ymax - prop->pos.y, ymin - prop->pos.y);
	chr_set_perim_enabled(chr, true);
	prop_set_perim_enabled(target, true);

	if (iVar2) {
		diffs[0] = target->pos.x - chr->prop->pos.x;
		diffs[1] = target->pos.z - chr->prop->pos.z;
		thing = sqrtf(diffs[0] * diffs[0] + diffs[1] * diffs[1]) * 2.5f / PALUPF(21.0f);
		time60 = thing;

		if (time60 < TICKS(10)) {
			time60 = TICKS(10);
		}

		chr->act_skjump.vel[0] = diffs[0] / time60;
		chr->act_skjump.vel[1] = diffs[1] / time60;
		chr->act_skjump.roty = chr_get_theta(chr) + chr_get_angle_to_pos(chr, &target->pos);
		chr->act_skjump.hit = false;
		chr->act_skjump.timer60 = time60;
		chr->act_skjump.total60 = time60;
		chr->act_skjump.ground = cd_find_ground_at_cyl(&chr->prop->pos, chr->radius, chr->prop->rooms, NULL, NULL);
	} else {
		return false;
	}

	chr->actiontype = ACT_SKJUMP;
	chr->act_skjump.needsnewanim = true;
	chr->act_skjump.state = SKJUMPSTATE_TAKEOFF;

	return true;
}

void chr_tick_skjump(struct chrdata *chr)
{
	if (g_Vars.lvupdate60 == 0) {
		return;
	}

	if (chr->act_skjump.needsnewanim) {
		chr->act_skjump.needsnewanim = false;

		switch (chr->act_skjump.state) {
		case SKJUMPSTATE_TAKEOFF:
			model_set_animation(chr->model, ANIM_SKEDAR_JUMPSTART, 0, 0, -1, 8);
			model_set_anim_speed(chr->model, 2.5, 0);
			break;
		case SKJUMPSTATE_AIRBORNE: {
				u16 sounds[] = {
					SFXNUM_0532_SKEDAR_ROAR,
					SFXNUM_0533_SKEDAR_ROAR,
					SFXNUM_0534_SKEDAR_ROAR,
				};

				ps_create(NULL, chr->prop, sounds[random() % 3], -1,
						-1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
				model_set_animation(chr->model, ANIM_SKEDAR_JUMPAIR, 0, 0, -1, 16);
				model_set_anim_speed(chr->model, 1, 0);
			}
			break;
		}
	}

	{
		f32 fVar5;
		f32 fVar6;
		f32 fVar7;
		u32 stack[1];
		f32 frame;

		switch (chr->act_skjump.state) {
		case SKJUMPSTATE_TAKEOFF:
			fVar6 = chr_get_theta(chr);
			fVar5 = model_tween_rot_axis(fVar6, chr->act_skjump.roty, 0.35);
			chr_set_theta(chr, fVar5);
			frame = model_get_cur_anim_frame(chr->model);

			if (frame >= model_get_anim_end_frame(chr->model)) {
				chr->act_skjump.state++;
				chr->act_skjump.needsnewanim = true;
			}
			break;
		case SKJUMPSTATE_AIRBORNE:
			chr->act_skjump.pos.x = chr->act_skjump.vel[0] * g_Vars.lvupdate60 + chr->prop->pos.x;
			chr->act_skjump.pos.z = chr->act_skjump.vel[1] * g_Vars.lvupdate60 + chr->prop->pos.z;

			if (chr->act_skjump.total60 > 0) {
				fVar6 = 1.0f - chr->act_skjump.timer60 / (f32)chr->act_skjump.total60;
				fVar7 = sinf(DTOR(180) * fVar6);
				fVar7 = fVar7 * 160.0f + chr->act_skjump.ground;
			} else {
				fVar6 = 1;
				fVar7 = chr->act_skjump.ground;
			}

			chr->act_skjump.pos.y = fVar7 - chr->prop->pos.y;

			if (fVar6 < 0.5f && chr->act_skjump.pos.y < 0.0f) {
				chr->act_skjump.pos.y = 0;
			}

			if (!chr->act_skjump.hit && chr_get_distance_to_target(chr) < 150.0f) {
				chr_punch_inflict_damage(chr, 3, 150, false);
				chr->act_skjump.hit = true;
			}

			if (chr->act_skjump.timer60 > 0) {
				chr->act_skjump.timer60 -= g_Vars.lvupdate60;
			} else {
				chr_try_stop(chr);
			}
			break;
		}
	}
}

void chra_tick(struct chrdata *chr)
{
	u32 race = CHRRACE(chr);

	if (g_Vars.lvupdate240 < 1) {
		return;
	}

	if (chr->actiontype == ACT_INIT) {
		chr->chrflags |= CHRCFLAG_FORCETOGROUND;
		chr_stand_immediate(chr, 0);
		chr->sleep = 0;
	}

	if (race == RACE_DRCAROLL) {
		g_DrCarollDyingTimer += g_Vars.lvupdate60;
	}

	chr->soundtimer += g_Vars.lvupdate60;
	chr->talktimer += g_Vars.lvupdate60;

#if VERSION < VERSION_PAL_BETA
	if (chr->aibot) {
		chr->aibot->unk030 += g_Vars.lvupdate60;
	}
#endif

	if (chr->hidden & CHRHFLAG_TIMER_RUNNING) {
		chr->timer60 += g_Vars.lvupdate60;
	}

	chr->sleep -= g_Vars.lvupdate60;

	if (chr->sleep < 0
			|| (chr->chrflags & CHRCFLAG_NEVERSLEEP)
			|| chr->alertness >= 65
			|| (chr->aibot && (chr->actiontype == ACT_DIE || chr->actiontype == ACT_DEAD))) {
		u8 pass = race == RACE_HUMAN || race == RACE_SKEDAR;
		chr->sleep = 0;

		chrai_execute(chr, PROPTYPE_CHR);

		// Consider setting shootingatmelist
		if (chr->prop) {
			if (chr->aimtesttimer60 < 1) {
				chr->aimtesttimer60 = TICKS(30);

				if (chr->aishootingatmelist >= 0
						&& ailist_find_by_id(chr->aishootingatmelist) != chr->ailist
						&& chr_is_target_aiming_at_me(chr)) {
					chr->chrflags |= CHRCFLAG_CONSIDER_DODGE;
				}
			} else {
				chr->aimtesttimer60 -= g_Vars.lvupdate60;
			}
		}

		// Consider setting darkroomlist
		if (chr->prop
				&& chr->aidarkroomlist >= 0
				&& room_get_settled_local_brightness(chr->prop->rooms[0]) < 25
				&& ailist_find_by_id(chr->aidarkroomlist) != chr->ailist) {
			chr->darkroomthing = true;
		}

		// Consider setting playerdeadlist
		if (chr->prop && chr->aiplayerdeadlist >= 0 && g_Vars.currentplayer->isdead) {
			u32 prevplayernum = g_Vars.currentplayernum;
			s32 i;
			s32 playercount = PLAYERCOUNT();
			bool alldead = true;

			if (playercount >= 2) {
				for (i = 0; i < playercount && alldead; i++) {
					if (i != prevplayernum) {
						set_current_player_num(i);

						if (g_Vars.currentplayer->isdead == false) {
							alldead = false;
						}
					}
				}

				set_current_player_num(prevplayernum);
			}

			if (alldead && ailist_find_by_id(chr->aiplayerdeadlist) != chr->ailist) {
				chr->playerdeadthing = true;
			}
		}

		if (race == RACE_ROBOT) {
			robot_set_muzzle_flash(chr, 0, false);
			robot_set_muzzle_flash(chr, 1, false);
		}

		if (chr->prop) {
			if (g_Vars.in_cutscene) {
				switch (chr->actiontype) {
				case ACT_ANIM:   chr_tick_anim(chr);   break;
				case ACT_PATROL: chr_tick_patrol(chr); pass = false; break;
				}
			} else {
				switch (chr->actiontype) {
				case ACT_STAND:           chr_tick_stand(chr);           break;
				case ACT_KNEEL:           chr_tick_kneel(chr);           break;
				case ACT_ANIM:            chr_tick_anim(chr);            break;
				case ACT_DIE:             chr_tick_die(chr);             break;
				case ACT_ARGH:            chr_tick_argh(chr);            break;
				case ACT_PREARGH:         chr_tick_preargh(chr);         break;
				case ACT_SIDESTEP:        chr_tick_sidestep(chr);        break;
				case ACT_JUMPOUT:         chr_tick_jumpout(chr);         break;
				case ACT_DEAD:            chr_tick_dead(chr);            break;
				case ACT_ATTACK:          chr_tick_attack(chr);          break;
				case ACT_ATTACKWALK:      chr_tick_attackwalk(chr);      break;
				case ACT_ATTACKROLL:      chr_tick_attackroll(chr);      break;
				case ACT_RUNPOS:          chr_tick_runpos(chr);          break;
				case ACT_PATROL:          chr_tick_patrol(chr);          pass = false; break;
				case ACT_GOPOS:           chr_tick_gopos(chr);           break;
				case ACT_SURRENDER:       chr_tick_surrender(chr);       break;
				case ACT_TEST:            chr_tick_test(chr);            break;
				case ACT_SURPRISED:       chr_tick_surprised(chr);       break;
				case ACT_STARTALARM:      chr_tick_startalarm(chr);      break;
				case ACT_THROWGRENADE:    chr_tick_throwgrenade(chr);    break;
				case ACT_BONDDIE:         chr_tick_bonddie(chr);         break;
				case ACT_DRUGGEDCOMINGUP: chr_tick_druggedcomingup(chr); break;
				case ACT_DRUGGEDDROP:     chr_tick_druggeddrop(chr);     break;
				case ACT_DRUGGEDKO:       chr_tick_druggedko(chr);       break;
				case ACT_ATTACKAMOUNT:    chr_tick_attackamount(chr);    break;
				case ACT_ROBOTATTACK:     chr_tick_robotattack(chr);     break;
				case ACT_SKJUMP:          chr_tick_skjump(chr);          break;
				}
			}
		}

#if VERSION >= VERSION_NTSC_1_0
		chr->hidden &= ~CHRHFLAG_IS_HEARING_TARGET;
		chr->hidden2 &= ~CHRH2FLAG_CONSIDERPROXIES;
#else
		chr->hidden &= ~(CHRHFLAG_IS_HEARING_TARGET | CHRHFLAG_CONSIDERPROXIES);
#endif

		if (pass) {
			footstep_check_default(chr);
		}
	} else {
		footstep_check_magic(chr);
	}
}

void cutscene_start(u32 ailistid)
{
	struct prop *prop;

#if PAL
	var8009e388pf = 0;
#else
	g_CutsceneFrameOverrun240 = 0;
#endif
	g_CutsceneSkipRequested = false;
	g_CutsceneCurTotalFrame60f = 0;

	prop = g_Vars.activeprops;

	while (prop) {
		prop->lastupdateframe = 0xffff;
		prop->propupdate240 = 0;
		prop->propupdate60err = 2;

		prop = prop->next;
	}

	g_BgChrs[g_NumBgChrs - 1].ailist = ailist_find_by_id(ailistid);
	g_BgChrs[g_NumBgChrs - 1].aioffset = 0;
	g_BgChrs[g_NumBgChrs - 1].aireturnlist = -1;
}

/**
 * Execute background tasks.
 *
 * - Calculate number of engaged chrs
 * - Handle switching to new cutscenes when playing all
 * - Run BG scripts
 * - Choose which corpses to fade
 */
void chra_tick_bg(void)
{
	s32 i;
	s32 numchrs = chrs_get_num_slots();
	s32 numaliveonscreen;
	s32 numdeadonscreen;
	s32 onscreenlen;
	s32 offscreenlen;
#if VERSION >= VERSION_NTSC_1_0
	s32 spawnslen;
#endif
	struct chrdata *onscreen[5];
	struct chrdata *offscreen[5];
#if VERSION >= VERSION_NTSC_1_0
	struct chrdata *spawns[10];
#endif
	s32 writeindex;
	s32 maxdeadonscreen;

#if VERSION >= VERSION_NTSC_1_0
	static u32 var80068454 = 0;
#endif

	static s32 mosteveralive = 0;

	numaliveonscreen = 0;

#if VERSION >= VERSION_NTSC_1_0
	spawnslen = 0;
	numdeadonscreen = 0;
#else
	numdeadonscreen = 0;
	onscreenlen = 0;
	offscreenlen = 0;
#endif

#if VERSION >= VERSION_NTSC_1_0
	// Count the number of chrs who are engaged with the player.
	// When these numbers are high, chrs can ignore gopos commands.
	// NTSC beta has a simple version of this logic in a loop
	// near the end of this function.
	g_NumChrsWithPlayerTarget = 0;
	g_NumChrsSeenPlayer = 0;
	g_NumChrsSeenPlayerRecently = 0;
	g_NumChrsSeenPlayerRecently2 = 0;

	if (g_Vars.normmplayerisrunning == false) {
		for (i = 0; i < numchrs; i++) {
			struct chrdata *chr = &g_ChrSlots[i];

			if (chr->model && chr->prop && !chr_is_dead(chr)) {
				struct prop *targetprop = chr_get_target_prop(chr);

				if (targetprop && (targetprop->type == PROPTYPE_CHR || targetprop->type == PROPTYPE_PLAYER)) {
					if ((targetprop->type == PROPTYPE_PLAYER
								&& !(g_Vars.antiplayernum >= 0 && g_Vars.anti && g_Vars.anti->prop == targetprop)
								&& chr_compare_teams(chr, targetprop->chr, COMPARE_ENEMIES))
							|| CHRRACE(targetprop->chr) == RACE_EYESPY) {
						s32 time60;
						s32 lastsee;
						s32 lastvis;

						g_NumChrsWithPlayerTarget++;

						lastsee = chr->lastseetarget60;
						lastvis = chr->lastvisibletarget60;
						time60 = lastsee > lastvis ? lastsee : lastvis;

						if (time60) {
							g_NumChrsSeenPlayer++;

							if (g_Vars.lvframe60 - time60 < TICKS(240)) {
								g_NumChrsSeenPlayerRecently++;
								g_NumChrsSeenPlayerRecently2++;
							}
						}
					}
				}
			}
		}
	}

	var80068454++;

	if (var80068454 > 10) {
		var80068454 = 0;
	}
#endif

	// If enabled, print a list of dangerous props to the developer's console
	if (debug_dangerous_props()) {
		prop_print_dangerous();
	}

	// Handle switching to a new cutscene when using "Play All" from the menu
	if (g_Vars.autocutnum >= 0) {
		cutscene_start(g_Vars.autocutnum + 0xc00);
		g_Vars.autocutnum = -1;
		g_Vars.autocutplaying = true;
	}

	// Run BG scripts
	for (i = 0; i < g_NumBgChrs; i++) {
		if (!g_Vars.autocutplaying || (g_BgChrs[i].hidden2 & CHRH2FLAG_TICKDURINGAUTOCUT)) {
			chra_tick(&g_BgChrs[i]);
		}
	}

	// Calculate alive/dead counters. For *spawned* chrs that have died,
	// allow 10 corpses and start fading if there's more.
	{
		s32 numalive = 0;

		for (i = 0; i < numchrs; i++) {
			struct chrdata *chr = &g_ChrSlots[i];

			if (chr->model && chr->prop) {
				if (chr->actiontype != ACT_DEAD && chr->actiontype != ACT_DRUGGEDKO) {
					numalive++;
				}

				if (chr->prop->flags & PROPFLAG_ONANYSCREENPREVTICK) {
					if (chr->actiontype != ACT_DEAD && chr->actiontype != ACT_DRUGGEDKO) {
						numaliveonscreen++;
					} else if (chr->actiontype == ACT_DRUGGEDKO) {
						if ((chr->chrflags & CHRCFLAG_KEEPCORPSEKO) == 0) {
							numdeadonscreen++;
						}
					} else {
						numdeadonscreen++;
					}
				}

#if VERSION >= VERSION_NTSC_1_0
				if (chr->actiontype == ACT_DEAD
						|| (chr->actiontype == ACT_DRUGGEDKO && (chr->chrflags & CHRCFLAG_KEEPCORPSEKO) == 0)) {
					if (chr->hidden2 & CHRH2FLAG_SPAWNED) {
						spawns[spawnslen] = chr;
						spawnslen++;

						if (spawnslen >= 10) {
							writeindex = random() % spawnslen;
							chr_fade_corpse(spawns[writeindex]);
							spawns[writeindex] = spawns[spawnslen - 1];
							spawnslen--;
						}
					}
				}
#endif
			}
		}

		if (numalive > mosteveralive) {
			mosteveralive = numalive;
		}
	}

	// Calculate how many corpses are allowed on screen. Ideally no more than
	// 5 chrs (alive or dead) to keep the number of polygons low.
	maxdeadonscreen = 5 - numaliveonscreen;

	// If the game is lagging, take the number of 60ths over 6 since the
	// previous frame and reduce the limit by that amount. So if it's been, say,
	// 8 60ths since the last frame then the max corpses will be reduced by 2.
	// This is why corpses are more likely to fade on screen when you change
	// weapons - the lag spike from changing guns triggers a lower corpse limit.
	if (g_Vars.lvupdate60 > 6) {
		maxdeadonscreen = maxdeadonscreen - g_Vars.lvupdate60 + 6;
	}

	if (maxdeadonscreen < 0) {
		maxdeadonscreen = 0;
	}

#if VERSION >= VERSION_JPN_FINAL
	// JPN fades corpses immediately
	if (numdeadonscreen && g_Vars.stagenum != STAGE_CITRAINING) {
		for (i = 0; i < numchrs; i++) {
			struct chrdata *chr = &g_ChrSlots[i];

			if (chr->model
					&& (chr->actiontype == ACT_DEAD || (chr->actiontype == ACT_DRUGGEDKO
							&& chr->prop && (chr->chrflags & CHRCFLAG_KEEPCORPSEKO) == 0))
					&& chr->act_dead.fadetimer60 < 0
					&& !chr->act_dead.fadenow) {
				chr_fade_corpse(chr);
			}
		}
	}
#else
	// Decide which corpses to fade.
	// NTSC Beta implements its engagement counter here.
	if (numdeadonscreen) {
		numdeadonscreen = 0;
#if VERSION >= VERSION_NTSC_1_0
		onscreenlen = 0;
		offscreenlen = 0;
#else
		g_NumChrsSeenPlayerRecently2 = 0;
#endif

		for (i = 0; i < numchrs; i++) {
			struct chrdata *chr = &g_ChrSlots[i];

			if (chr->model) {
#if VERSION < VERSION_NTSC_1_0
				if (!g_Vars.mplayerisrunning
						&& chr_get_target_prop(chr)->type == PROPTYPE_PLAYER
						&& chr->lastseetarget60 > 0
						&& g_Vars.lvframe60 - chr->lastseetarget60 < TICKS(120)) {
					g_NumChrsSeenPlayerRecently2++;
				}
#endif

				if (chr->actiontype == ACT_DEAD
						|| (chr->actiontype == ACT_DRUGGEDKO && chr->prop && (chr->chrflags & CHRCFLAG_KEEPCORPSEKO) == 0)) {
					if (chr->prop->flags & PROPFLAG_ONANYSCREENPREVTICK) {
						// On-screen
						if (chr->act_dead.fadetimer60 < 0 && !chr->act_dead.fadenow) {
							numdeadonscreen++;

							// If there's too many corpses on screen, start fading.
							if (numdeadonscreen > maxdeadonscreen || chr->aibot) {
								chr_fade_corpse(chr);
								numdeadonscreen--;
							} else if (!chr->act_dead.fadewheninvis) {
								// If there are 2 or more corpses on screen,
								// start marking them to be removed once off screen
								onscreen[onscreenlen] = chr;
								onscreenlen++;

								if (onscreenlen >= (VERSION >= VERSION_NTSC_1_0 ? 2 : 3)) {
									writeindex = random() % onscreenlen;
									chr_fade_corpse_when_offscreen(onscreen[writeindex]);
									onscreen[writeindex] = onscreen[onscreenlen - 1];
									onscreenlen--;
								}
							}
						}
					} else {
						// Off-screen
						if (!chr->act_dead.fadewheninvis) {
							offscreen[offscreenlen] = chr;
							offscreenlen++;

							// Allow up to 5 corpses off-screen
							if (offscreenlen >= (VERSION >= VERSION_NTSC_1_0 ? 5 : 6)) {
								writeindex = random() % offscreenlen;

								if (offscreen[writeindex]->actiontype != ACT_DEAD) {
									chr_begin_dead(offscreen[writeindex]);
								}

								chr_fade_corpse_when_offscreen(offscreen[writeindex]);
								offscreen[writeindex] = offscreen[offscreenlen - 1];
								offscreenlen--;
							}
						}
					}
				}
			}
		}
	}
#endif

	if (1);
}

bool chr_saw_target_recently(struct chrdata *chr)
{
	if (chr->lastseetarget60 > 0 && g_Vars.lvframe60 - chr->lastseetarget60 < TICKS(600)) {
		return true;
	}

	return false;
}

bool chr_heard_target_recently(struct chrdata *chr)
{
	if (chr->lastheartarget60 > 0 && g_Vars.lvframe60 - chr->lastheartarget60 < TICKS(600)) {
		return true;
	}

	return false;
}

f32 chr_get_angle_to_pos(struct chrdata *chr, struct coord *pos)
{
	f32 theta;
	f32 angle;
	struct prop *prop;

	if (chr->prop->type == PROPTYPE_PLAYER) {
		s32 playernum = playermgr_get_player_num_by_prop(chr->prop);
		theta = BADDTOR2(360 - g_Vars.players[playernum]->vv_theta);
	} else {
		theta = chr_get_theta(chr);
	}

	prop = chr->prop;
	angle = atan2f(pos->x - prop->pos.x, pos->z - prop->pos.z) - theta;

	if (angle < 0) {
		angle += BADDTOR(360);
	}

	return angle;
}

f32 chr_get_angle_to_target(struct chrdata *chr)
{
	struct prop *prop = chr_get_target_prop(chr);
	return chr_get_angle_to_pos(chr, &prop->pos);
}

void chr_get_attack_entity_pos(struct chrdata *chr, u32 attackflags, s32 entityid, struct coord *pos, RoomNum *rooms)
{
	struct prop *targetprop;
	struct chrdata *targetchr;
	struct pad pad;

	if (attackflags & ATTACKFLAG_AIMATCHR) {
		// Aiming at a chr by chrnum
		targetchr = chr_find_by_id(chr, entityid);

		if (!targetchr || !targetchr->prop) {
			targetchr = chr;
		}

		pos->x = targetchr->prop->pos.x;
		pos->y = targetchr->prop->pos.y;
		pos->z = targetchr->prop->pos.z;

		if (targetchr) {
			chr = targetprop->chr;
		} else {
			chr = NULL;
		}

		if (chr);

		rooms_copy(targetchr->prop->rooms, rooms);
	} else if (attackflags & ATTACKFLAG_AIMATPAD) {
		// Aiming at a pad by padnum
		s32 padnum = chr_resolve_pad_id(chr, entityid);
		pad_unpack(padnum, PADFIELD_POS | PADFIELD_ROOM, &pad);

		pos->x = pad.pos.x;
		pos->y = pad.pos.y;
		pos->z = pad.pos.z;

		rooms[0] = pad.room;
		rooms[1] = -1;
	} else {
		// Aiming at the chr's preconfigured target
		targetprop = chr_get_target_prop(chr);

		pos->x = targetprop->pos.x;
		pos->y = targetprop->pos.y;
		pos->z = targetprop->pos.z;

		if (targetprop->type == PROPTYPE_CHR && targetprop->chr) {
			chr = targetprop->chr;
		} else {
			chr = NULL;
		}

		if (chr);

		rooms_copy(targetprop->rooms, rooms);
	}
}

f32 chr_get_angle_from_targets_fov(struct chrdata *chr)
{
	f32 targetfacingangle = 0;
	struct prop *prop = chr->prop;
	struct prop *target = chr_get_target_prop(chr);
	f32 xdiff;
	f32 zdiff;
	f32 angletotarget;
	f32 result;

	if (prop && target) {
		xdiff = prop->pos.f[0] - target->pos.f[0];
		zdiff = prop->pos.f[2] - target->pos.f[2];
		angletotarget = atan2f(xdiff, zdiff);

		if (target->type == PROPTYPE_PLAYER) {
			s32 playernum = playermgr_get_player_num_by_prop(target);
			targetfacingangle = BADDTOR3(360.0f - g_Vars.players[playernum]->vv_theta);
		} else if (target->type == PROPTYPE_CHR) {
			targetfacingangle = chr_get_theta(target->chr);
		}

		result = angletotarget - targetfacingangle;

		if (result < 0) {
			result += BADDTOR(360);
		}
	}

	return result;
}

f32 chr_get_vertical_angle_to_target(struct chrdata *chr)
{
	struct prop *prop = chr->prop;
	struct prop *target = chr_get_target_prop(chr);
	f32 result = 0;

	if (prop && target) {
		f32 ydiff;
		f32 xdiff;
		f32 zdiff;

		xdiff = prop->pos.x - target->pos.x;
		ydiff = prop->pos.y - target->pos.y;
		zdiff = prop->pos.z - target->pos.z;

		result = atan2f(ydiff, sqrtf(xdiff * xdiff + zdiff * zdiff));

		if (result < 0) {
			result += BADDTOR(360);
		}
	}

	return result;
}

#define D256TOR(degrees256) ((degrees256) * (360.0f * M_BADPI / 180.0f / 256.0f))

bool chr_is_in_targets_fovx(struct chrdata *chr, u8 degrees256)
{
	f32 angle = chr_get_angle_from_targets_fov(chr);

	if ((angle < D256TOR(degrees256) && angle < DTOR(180))
			|| (angle > BADDTOR(360) - D256TOR(degrees256) && angle > DTOR(180))) {
		return true;
	}

	return false;
}

bool chr_is_vertical_angle_to_target_within(struct chrdata *chr, u8 degrees256)
{
	f32 val = chr_get_vertical_angle_to_target(chr);

	if ((val < D256TOR(degrees256) && val < DTOR(180))
			|| (val > BADDTOR(360) - D256TOR(degrees256) && val > DTOR(180))) {
		return true;
	}

	return false;
}

f32 chr_get_angle_to_target2(struct chrdata *chr, u8 reverse)
{
	f32 result;

	struct prop *chrprop = chr->prop;
	struct prop *targetprop = chr_get_target_prop(chr);

	f32 xdiff = chrprop->pos.x - targetprop->pos.x;
	f32 zdiff = chrprop->pos.z - targetprop->pos.z;

	f32 angle1 = atan2f(-xdiff, -zdiff);
	f32 angle2 = chr_get_theta(chr) + DTOR(180) * (s32)reverse;

	result = angle1 - angle2;

	if (result < 0) {
		result += BADDTOR(360);
	}

	return result;
}

bool chr_is_target_in_fov(struct chrdata *chr, u8 degrees256, u8 reverse)
{
	f32 angle = chr_get_angle_to_target2(chr, reverse);

	if ((angle < D256TOR(degrees256) && angle < DTOR(180))
			|| (angle > BADDTOR(360) - D256TOR(degrees256) && angle > DTOR(180))) {
		return true;
	}

	return false;
}

bool chr_is_looking_at_pos(struct chrdata *chr, struct coord *pos, u8 degrees256)
{
	f32 angle = chr_get_angle_to_pos(chr, pos);

	if ((angle < D256TOR(degrees256) && angle < DTOR(180))
			|| (angle > BADDTOR(360) - D256TOR(degrees256) && angle > DTOR(180))) {
		return true;
	}

	return false;
}

f32 chr_get_distance_to_target(struct chrdata *chr)
{
	struct prop *prop = chr_get_target_prop(chr);
	return prop_get_distance_to_prop(chr->prop, prop);
}

// Redundant function - it's the same as above
f32 chr_get_distance_to_target2(struct chrdata *chr)
{
	struct prop *prop = chr_get_target_prop(chr);
	return prop_get_distance_to_prop(chr->prop, prop);
}

f32 chr_get_distance_to_current_player(struct chrdata *chr)
{
	return prop_get_distance_to_prop(chr->prop, g_Vars.currentplayer->prop);
}

f32 prop_get_distance_to_prop(struct prop *a, struct prop *b)
{
	f32 xdiff = a->pos.x - b->pos.x;
	f32 ydiff = a->pos.y - b->pos.y;
	f32 zdiff = a->pos.z - b->pos.z;

	return sqrtf(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
}

f32 prop_get_lateral_distance_to_prop(struct prop *a, struct prop *b)
{
	f32 xdiff = a->pos.x - b->pos.x;
	f32 zdiff = a->pos.z - b->pos.z;

	return sqrtf(xdiff * xdiff + zdiff * zdiff);
}

f32 chr_get_distance_to_pad(struct chrdata *chr, s32 pad_id)
{
	struct prop *prop = chr->prop;
	f32 xdiff, ydiff, zdiff;
	f32 distance = 0;
	struct pad pad;
	pad_id = chr_resolve_pad_id(chr, pad_id);

#if VERSION >= VERSION_NTSC_1_0
	if (pad_id >= 0)
#endif
	{
		pad_unpack(pad_id, PADFIELD_POS, &pad);
		xdiff = pad.pos.x - prop->pos.x;
		ydiff = pad.pos.y - prop->pos.y;
		zdiff = pad.pos.z - prop->pos.z;
		distance = sqrtf(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
	}

	return distance;
}

#if VERSION >= VERSION_NTSC_1_0
f32 chr_get_same_floor_distance_to_pad(struct chrdata *chr, s32 pad_id)
{
	struct prop *prop = chr->prop;
	f32 xdiff, ydiff, zdiff, ydiff_absolute;
	struct pad pad;
	f32 ret;

	pad_id = chr_resolve_pad_id(chr, pad_id);
	pad_unpack(pad_id, PADFIELD_POS, &pad);
	xdiff = pad.pos.x - prop->pos.x;
	ydiff = pad.pos.y - prop->pos.y;
	zdiff = pad.pos.z - prop->pos.z;

	if (ydiff > 0) {
		ydiff_absolute = ydiff;
	} else {
		ydiff_absolute = -ydiff;
	}

	if (ydiff_absolute < 150) {
		ret = sqrtf(xdiff * xdiff + zdiff * zdiff);
	} else {
		ret = 100000000;
	}

	return ret;
}
#endif

f32 chr_get_distance_to_coord(struct chrdata *chr, struct coord *pos)
{
	f32 xdiff = pos->x - chr->prop->pos.x;
	f32 ydiff = pos->y - chr->prop->pos.y;
	f32 zdiff = pos->z - chr->prop->pos.z;

	return sqrtf(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
}

f32 chr_get_lateral_distance_to_coord(struct chrdata *chr, struct coord *pos)
{
	f32 xdiff = pos->x - chr->prop->pos.x;
	f32 zdiff = pos->z - chr->prop->pos.z;

	return sqrtf(xdiff * xdiff + zdiff * zdiff);
}

f32 chr_get_lateral_distance_to_pad(struct chrdata *chr, s32 pad_id)
{
	struct prop *prop = chr->prop;
	f32 xdiff, zdiff;
	struct pad pad;

#if VERSION >= VERSION_NTSC_1_0
	f32 distance = 0;

	pad_id = chr_resolve_pad_id(chr, pad_id);

	if (pad_id >= 0) {
		pad_unpack(pad_id, PADFIELD_POS, &pad);
		xdiff = pad.pos.x - prop->pos.x;
		zdiff = pad.pos.z - prop->pos.z;
		distance = sqrtf(xdiff * xdiff + zdiff * zdiff);
	}

	return distance;
#else
	pad_id = chr_resolve_pad_id(chr, pad_id);
	pad_unpack(pad_id, PADFIELD_POS, &pad);
	xdiff = pad.pos.x - prop->pos.x;
	zdiff = pad.pos.z - prop->pos.z;
	return sqrtf(xdiff * xdiff + zdiff * zdiff);
#endif
}

f32 chr_get_squared_distance_to_coord(struct chrdata *chr, struct coord *pos)
{
	f32 xdiff = pos->x - chr->prop->pos.x;
	f32 ydiff = pos->y - chr->prop->pos.y;
	f32 zdiff = pos->z - chr->prop->pos.z;

	return xdiff * xdiff + ydiff * ydiff + zdiff * zdiff;
}

f32 coord_get_squared_distance_to_coord(struct coord *a, struct coord *b)
{
	f32 xdiff = a->x - b->x;
	f32 ydiff = a->y - b->y;
	f32 zdiff = a->z - b->z;

	return xdiff * xdiff + ydiff * ydiff + zdiff * zdiff;
}

s32 chr_get_pad_room(struct chrdata *chr, s32 pad_id)
{
	s32 ret = -1;
	s32 pad_id_backup = pad_id;
	struct pad pad;

	if (pad_id >= 10000) {
		s32 resolved_pad_id = chr_resolve_pad_id(chr, pad_id - 10000);

#if VERSION >= VERSION_NTSC_1_0
		if (resolved_pad_id >= 0)
#endif
		{
			pad_unpack(resolved_pad_id, PADFIELD_ROOM, &pad);
			ret = pad.room;
		}
	} else {
		ret = pad_id;
	}

	return ret;
}

s32 chr_resolve_pad_id(struct chrdata *chr, s32 pad_id)
{
	if (pad_id == 9000) {
		pad_id = chr->padpreset1;
	}

	return pad_id;
}

/**
 * For all chrs, clear their target and p1p2 values if set to the given player.
 *
 * This function is called when the given player has died. It causes all guards
 * to switch their focus to the remaining coop player.
 */
void chrs_clear_refs_to_player(s32 playernum)
{
	s32 otherplayernum;
	s32 playerpropnum;
	s32 i;

	if (g_Vars.coopplayernum >= 0) {
		if (playernum == g_Vars.bondplayernum) {
			otherplayernum = g_Vars.coopplayernum;
			playerpropnum = g_Vars.bond->prop - g_Vars.props;
		} else {
			otherplayernum = g_Vars.bondplayernum;
			playerpropnum = g_Vars.coop->prop - g_Vars.props;
		}

		for (i = 0; i < chrs_get_num_slots(); i++) {
			if (g_ChrSlots[i].p1p2 == playernum) {
				g_ChrSlots[i].p1p2 = otherplayernum;
			}

			if (g_ChrSlots[i].target == playerpropnum) {
				g_ChrSlots[i].target = -1;
			}
		}

		for (i = 0; i < g_NumBgChrs; i++) {
			if (g_BgChrs[i].p1p2 == playernum) {
				g_BgChrs[i].p1p2 = otherplayernum;
			}

			if (g_BgChrs[i].target == playerpropnum) {
				g_BgChrs[i].target = -1;
			}
		}
	}
}

s32 chr_resolve_id(struct chrdata *ref, s32 id)
{
	if (ref) {
		switch (id) {
		case CHR_SEESHOT:
			id = ref->chrseeshot;
			break;
		case CHR_SEEDIE:
			id = ref->chrseedie;
			break;
		case CHR_PRESET:
			id = ref->chrpreset1;
			break;
		case CHR_SELF:
			id = ref->chrnum;
			break;
		case CHR_CLONE:
			id = ref->chrdup;
			break;
		case CHR_BOND:
			if (g_Vars.bond && g_Vars.bond->prop && g_Vars.bond->prop->chr) {
				id = g_Vars.bond->prop->chr->chrnum;
			}
			break;
		case CHR_COOP:
			if (g_Vars.coop && g_Vars.coop->prop && g_Vars.coop->prop->chr) {
				id = g_Vars.coop->prop->chr->chrnum;
			}
			break;
		case CHR_ANTI:
			if (g_Vars.anti && g_Vars.anti->prop && g_Vars.anti->prop->chr) {
				id = g_Vars.anti->prop->chr->chrnum;
			}
			break;
		case CHR_P1P2:
			{
				u32 index = g_Vars.coopplayernum >= 0 ? ref->p1p2 : g_Vars.bondplayernum;
				struct player *player = g_Vars.players[index];
				if (player && player->prop && player->prop->chr) {
					id = player->prop->chr->chrnum;
				}
			}
			break;
		case CHR_P1P2_OPPOSITE:
			if (g_Vars.coopplayernum >= 0) {
				struct player *player = g_Vars.players[1 - ref->p1p2];
				if (player && player->prop && player->prop->chr) {
					id = player->prop->chr->chrnum;
				}
			}
			break;
		case CHR_TARGET:
			{
				struct prop *target = chr_get_target_prop(ref);
				if ((target->type == PROPTYPE_CHR || target->type == PROPTYPE_PLAYER) && target->chr) {
					id = target->chr->chrnum;
				}
			}
			break;
		}
	} else { // ref is NULL
		switch (id) {
		case CHR_BOND:
			if (g_Vars.bond && g_Vars.bond->prop && g_Vars.bond->prop->chr) {
				id = g_Vars.bond->prop->chr->chrnum;
			}
			break;
		case CHR_COOP:
			if (g_Vars.coop && g_Vars.coop->prop && g_Vars.coop->prop->chr) {
				id = g_Vars.coop->prop->chr->chrnum;
			}
			break;
		case CHR_ANTI:
			if (g_Vars.anti && g_Vars.anti->prop && g_Vars.anti->prop->chr) {
				id = g_Vars.anti->prop->chr->chrnum;
			}
			break;
		case CHR_P1P2:
			{
				struct player *player = g_Vars.players[g_Vars.bondplayernum];
				if (player && player->prop && player->prop->chr) {
					id = player->prop->chr->chrnum;
				}
			}
			break;
		case CHR_P1P2_OPPOSITE:
			if (g_Vars.coopplayernum >= 0) {
				struct player *player = g_Vars.players[g_Vars.coopplayernum];
				if (player && player->prop && player->prop->chr) {
					id = player->prop->chr->chrnum;
				}
			}
			break;
		}
	}

	return id;
}

struct chrdata *chr_find_by_id(struct chrdata *basechr, s32 chrnum)
{
	struct chrdata *chr;
	s32 lower;
	s32 upper;
	s32 i;

	chrnum = chr_resolve_id(basechr, chrnum);
	chr = chr_find_by_literal_id(chrnum);

	if (chr) {
		return chr;
	}

	lower = 0;
	upper = g_NumBgChrs;

	while (upper >= lower) {
		i = (lower + upper) / 2;

		if (chrnum == g_BgChrnums[i]) {
			return &g_BgChrs[i];
		}

		if (chrnum < g_BgChrnums[i]) {
			upper = i - 1;
		} else {
			lower = i + 1;
		}
	}

	return NULL;
}

s32 prop_get_index_by_chr_id(struct chrdata *basechr, s32 chrnum)
{
	s32 index;

	if (chrnum == CHR_BOND || chrnum == CHR_BOND) {
		index = g_Vars.bond->prop - g_Vars.props;
	} else {
		struct chrdata *chr = chr_find_by_id(basechr, chrnum);

		if (chr && chr->prop) {
			index = chr->prop - g_Vars.props;
		} else {
			index = -1;
		}
	}

	return index;
}

f32 chr_get_distance_to_chr(struct chrdata *chr1, s32 chr2num)
{
	struct prop *prop1 = chr1->prop;
	struct chrdata *chr2 = chr_find_by_id(chr1, chr2num);
	f32 distance = 0;

	if (chr2 && chr2->prop) {
		f32 xdiff = chr2->prop->pos.x - prop1->pos.x;
		f32 ydiff = chr2->prop->pos.y - prop1->pos.y;
		f32 zdiff = chr2->prop->pos.z - prop1->pos.z;
		distance = sqrtf(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
	}

	return distance;
}

f32 chr_get_distance_from_target_to_pad(struct chrdata *chr, s32 pad_id)
{
	struct prop *prop = chr_get_target_prop(chr);
	f32 xdiff, ydiff, zdiff;
	struct pad pad;
	f32 distance = 0;
	pad_id = chr_resolve_pad_id(chr, pad_id);

#if VERSION >= VERSION_NTSC_1_0
	if (pad_id >= 0)
#endif
	{
		pad_unpack(pad_id, PADFIELD_POS, &pad);
		xdiff = pad.pos.x - prop->pos.x;
		ydiff = pad.pos.y - prop->pos.y;
		zdiff = pad.pos.z - prop->pos.z;
		distance = sqrtf(xdiff * xdiff + ydiff * ydiff + zdiff * zdiff);
	}

	return distance;
}

void chr_set_flags(struct chrdata *chr, u32 flags, u8 bank)
{
	if (bank == 0) {
		chr->flags |= flags;
	} else {
		chr->flags2 |= flags;
	}
}

void chr_unset_flags(struct chrdata *chr, u32 flags, u8 bank)
{
	if (bank == 0) {
		chr->flags &= ~flags;
	} else {
		chr->flags2 &= ~flags;
	}
}

bool chr_has_flag(struct chrdata *chr, u32 flag, u8 bank)
{
	if (bank == 0) {
		return (chr->flags & flag) != 0;
	} else {
		return (chr->flags2 & flag) != 0;
	}
}

void chr_set_flags_by_id(struct chrdata *ref, u32 chrnum, u32 flags, u32 bank)
{
	struct chrdata *chr = chr_find_by_id(ref, chrnum);

	if (chr) {
		chr_set_flags(chr, flags, bank);
	}
}

void chr_unset_flags_by_id(struct chrdata *ref, u32 chrnum, u32 flags, u32 bank)
{
	struct chrdata *chr = chr_find_by_id(ref, chrnum);

	if (chr) {
		chr_unset_flags(chr, flags, bank);
	}
}

bool chr_has_flag_by_id(struct chrdata *ref, u32 chrnum, u32 flag, u32 bank)
{
	struct chrdata *chr = chr_find_by_id(ref, chrnum);

	if (chr) {
		return chr_has_flag(chr, flag, bank);
	}

	return false;
}

void chr_set_stage_flag(struct chrdata *chr, u32 flag)
{
	g_StageFlags |= flag;
}

void chr_unset_stage_flag(struct chrdata *chr, u32 flag)
{
	g_StageFlags = g_StageFlags & ~flag;
}

bool chr_has_stage_flag(struct chrdata *chr, u32 flag)
{
	return (g_StageFlags & flag) != 0;
}

bool chr_is_hearing_target(struct chrdata *chr)
{
	return (chr->hidden & CHRHFLAG_IS_HEARING_TARGET) != 0;
}

void chr_restart_timer(struct chrdata *chr)
{
	chr->timer60 = 0;
	chr->hidden |= CHRHFLAG_TIMER_RUNNING;
}

f32 chr_get_timer(struct chrdata *chr)
{
	return chr->timer60 * FRAMEDURATION;
}

bool chr_is_target_aiming_at_me(struct chrdata *chr)
{
	struct prop *target = chr_get_target_prop(chr);

	if (target) {
		if (target->type == PROPTYPE_CHR) {
			if (!chr_has_los_to_target(chr)) {
				return false;
			}

			return chr_is_in_targets_fovx(chr, 20);
		}

		if (target->type == PROPTYPE_PLAYER) {
			if (g_Vars.bondvisible &&
					(cd_test_los05(&target->pos, target->rooms, &chr->prop->pos, chr->prop->rooms,
									  CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PATHBLOCKER | CDTYPE_BG,
									  GEOFLAG_BLOCK_SIGHT))) {
				struct model *model = chr->model;
				struct coord playerpos;
				struct coord playeraimdir;
				struct coord mypos;
				f32 myradius = model_get_effective_scale(model) * 0.8f;

				bgun0f0a0c08(&playerpos, &playeraimdir);
				model_get_root_position(model, &mypos);
				mtx4_transform_vec_in_place(cam_get_world_to_screen_mtxf(), &mypos);

				if (pos_is_facing_pos(&playerpos, &playeraimdir, &mypos, myradius)) {
					return true;
				}
			}
		}
	}

	return false;
}

bool chr_reset_nearmiss(struct chrdata *chr)
{
	bool has_flag = (chr->chrflags & CHRCFLAG_NEAR_MISS) != 0;
	chr->chrflags &= ~CHRCFLAG_NEAR_MISS;

	return has_flag;
}

s32 chr_get_num_arghs(struct chrdata *chr)
{
	return chr->numarghs;
}

s32 chr_get_num_closearghs(struct chrdata *chr)
{
	return chr->numclosearghs;
}

bool chr_saw_injury(struct chrdata *chr, u8 arg1)
{
	bool saw_injury = chr->chrseeshot >= 0;

	if (saw_injury && arg1 == 0) {
		chr->chrseeshot = -1;
	} else if (saw_injury && arg1 == 1) {
		struct chrdata *victim = chr_find_by_id(chr, chr->chrseeshot);

		if (victim && !chr_compare_teams(chr, victim, COMPARE_FRIENDS)) {
			saw_injury = false;
		}
	} else {
		chr->chrseeshot = -1;
	}

	return saw_injury;
}

bool chr_saw_death(struct chrdata *chr, u8 arg1)
{
	bool saw_death = chr->chrseedie >= 0;

	// The commented line below was likely originally there but removed before
	// the final version. Compare with chr_saw_injury above.
	if (saw_death && arg1 == 0) {
		//chr->chrseedie = -1;
	} else if (saw_death && arg1 == 1) {
		struct chrdata *victim = chr_find_by_id(chr, chr->chrseedie);

		if (victim && !chr_compare_teams(chr, victim, COMPARE_FRIENDS)) {
			saw_death = false;
			chr->chrseedie = -1;
		}
	} else {
		chr->chrseedie = -1;
	}

	return saw_death;
}

void decrement_byte(u8 *dst, u8 amount)
{
	if (*dst < amount) {
		*dst = 0;
		return;
	}

	*dst -= amount;
}

void increment_byte(u8 *dst, u8 amount)
{
	if (0xff - amount < *dst) {
		*dst = 0xff;
		return;
	}

	*dst += amount;
}

bool chr_can_hear_alarm(struct chrdata *chr)
{
	return alarm_is_active();
}

bool waypoint_is_within_90_degrees_of_pos_angle(struct waypoint *waypoint, struct coord *pos, f32 angle)
{
	u32 stack[3];
	f32 diffangle;
	struct pad pad;

	pad_unpack(waypoint->padnum, PADFIELD_POS, &pad);

	diffangle = angle - atan2f(pad.pos.x - pos->x, pad.pos.z - pos->z);

	if (diffangle < 0) {
		diffangle += BADDTOR(360);
	}

	if (diffangle < BADDTOR(90) || diffangle > BADDTOR(270)) {
		return true;
	}

	return false;
}

/**
 * Attempt to find a waypoint near pos which is in a particular quadrant to pos,
 * then return its padnum.
 *
 * For example, pos is typically the player's position, angle is the direction
 * the player is facing, and quadrant is which quadrant (front/back/left/right)
 * that is desired relative to the player's position and angle.
 *
 * The function starts by finding the closest waypoint to the pos. If it's not
 * in the quadrant then its neighouring waypoints are checked too. If none of
 * those are in the quadrant then no further checks are made and the function
 * returns -1.
 */
s32 chr_find_waypoint_within_pos_quadrant(struct coord *pos, RoomNum *rooms, f32 angle, u8 quadrant)
{
	struct waypoint *waypoint = waypoint_find_closest_to_pos(pos, rooms);
	s32 neighbournum;
	s32 i;

	if (waypoint) {
		switch (quadrant) {
		case QUADRANT_BACK:
			angle += BADDTOR(180);
			break;
		case QUADRANT_SIDE1:
			angle += BADDTOR(90);
			break;
		case QUADRANT_SIDE2:
			angle += BADDTOR(270);
			break;
		case QUADRANT_FRONT:
			break;
		}

		if (angle >= BADDTOR(360)) {
			angle -= BADDTOR(360);
		}

		if (waypoint_is_within_90_degrees_of_pos_angle(waypoint, pos, angle)) {
			return waypoint->padnum;
		}

		for (i = 0; (neighbournum = waypoint->neighbours[i]) >= 0; i++) {
			if ((neighbournum & 0x8000) == 0) {
				neighbournum &= 0x3fff;

				if (waypoint_is_within_90_degrees_of_pos_angle(&g_StageSetup.waypoints[neighbournum], pos, angle)) {
					return g_StageSetup.waypoints[neighbournum].padnum;
				}
			}
		}
	}

	return -1;
}

bool chr_set_pad_preset_to_waypoint_within_quadrant(struct chrdata *chr, u8 quadrant)
{
	if (quadrant == QUADRANT_TOWARDSTARGET || quadrant == QUADRANT_AWAYFROMTARGET) {
		struct prop *prop = chr->prop;
		struct prop *target = chr_get_target_prop(chr);

		struct waypoint *chrwp = waypoint_find_closest_to_pos(&prop->pos, prop->rooms);
		struct waypoint *tarwp = waypoint_find_closest_to_pos(&target->pos, target->rooms);

		struct waypoint *waypoints[3];
		s32 numwaypoints;

		if (chrwp && tarwp) {
			if (quadrant == QUADRANT_TOWARDSTARGET) {
				nav_set_seed(CHRNAVSEED(chr), CHRNAVSEED(chr));
				numwaypoints = nav_find_route(chrwp, tarwp, waypoints, 3);
				nav_set_seed(0, 0);

				if (numwaypoints >= 3) {
					chr->padpreset1 = waypoints[1]->padnum;
					return true;
				}
			} else {
				nav_set_seed(CHRNAVSEED(chr), CHRNAVSEED(chr));
				chrwp = nav_choose_retreat_point(chrwp, tarwp);
				nav_set_seed(0, 0);

				if (chrwp) {
					chr->padpreset1 = chrwp->padnum;
					return true;
				}
			}
		}
	} else {
		s32 padnum = chr_find_waypoint_within_pos_quadrant(&chr->prop->pos, chr->prop->rooms, chr_get_theta(chr), quadrant);

		if (padnum >= 0) {
			chr->padpreset1 = padnum;
			return true;
		}
	}

	return false;
}

bool chr_set_pad_preset_to_waypoint_within_target_quadrant(struct chrdata *chr, u8 quadrant)
{
	f32 angle;
	s32 padnum;
	struct prop *prop;

	if (quadrant == QUADRANT_TOWARDSTARGET || quadrant == QUADRANT_AWAYFROMTARGET) {
		return chr_set_pad_preset_to_waypoint_within_quadrant(chr, quadrant);
	}

	angle = 0;
	prop = chr_get_target_prop(chr);

	if (prop->type == PROPTYPE_PLAYER) {
		angle = BADDTOR3(360.0f - g_Vars.players[playermgr_get_player_num_by_prop(prop)]->vv_theta);
	} else if (prop->type == PROPTYPE_CHR) {
		angle = chr_get_theta(prop->chr);
	}

	padnum = chr_find_waypoint_within_pos_quadrant(&prop->pos, prop->rooms, angle, quadrant);

	if (padnum >= 0) {
		chr->padpreset1 = padnum;
		return true;
	}

	return false;
}

bool chr_set_chr_preset_to_any_chr_near_self(struct chrdata *chr, f32 distance)
{
	return chr_set_chr_preset_to_chr_near_self(COMPARE_ANY, chr, distance);
}

bool chr_set_chr_preset_to_chr_near_self(u8 checktype, struct chrdata *chr, f32 distance)
{
	return chr_set_chr_preset_to_chr_near_pos(checktype, chr, distance, &chr->prop->pos, chr->prop->rooms);
}

bool chr_set_chr_preset_to_chr_near_pad(u32 checktype, struct chrdata *chr, f32 distance, s32 padnum)
{
	struct pad pad;
	RoomNum rooms[2];

	padnum = chr_resolve_pad_id(chr, padnum);
	pad_unpack(padnum, PADFIELD_POS | PADFIELD_ROOM, &pad);

	rooms[0] = pad.room;
	rooms[1] = -1;

	return chr_set_chr_preset_to_chr_near_pos(checktype, chr, distance, &pad.pos, rooms);
}

bool chr_set_chr_preset_to_chr_near_pos(u8 checktype, struct chrdata *chr, f32 distance, struct coord *pos, RoomNum *rooms)
{
	s32 i;
	s16 *propnumptr;
	s16 propnums[256];
	RoomNum allrooms[21];
	f32 xmin = pos->x - distance;
	f32 xmax = pos->x + distance;
	f32 ymin = pos->y - distance;
	f32 ymax = pos->y + distance;
	f32 zmin = pos->z - distance;
	f32 zmax = pos->z + distance;
	RoomNum neighbours[11];

	rooms_copy(rooms, allrooms);

	for (i = 0; rooms[i] != -1; i++) {
		bg_room_get_neighbours(rooms[i], neighbours, 10);
		rooms_append(neighbours, allrooms, 20);
	}

	room_get_props(allrooms, propnums, 256);

	propnumptr = propnums;

	while (*propnumptr >= 0) {
		struct prop *prop = &g_Vars.props[*propnumptr];

		if (prop->type == PROPTYPE_CHR || prop->type == PROPTYPE_PLAYER) {
			struct chrdata *loopchr = prop->chr;

			if (loopchr->chrnum != chr->chrnum
					&& !chr_is_dead(loopchr)
					&& prop->pos.x >= xmin
					&& prop->pos.x <= xmax
					&& prop->pos.y >= ymin
					&& prop->pos.y <= ymax
					&& prop->pos.z >= zmin
					&& prop->pos.z <= zmax
					&& chr_compare_teams(loopchr, chr, checktype)) {
				chr->chrpreset1 = loopchr->chrnum;
				return true;
			}
		}

		*propnumptr++;
	}

	return false;
}

bool chr_compare_teams(struct chrdata *chr1, struct chrdata *chr2, u8 checktype)
{
	if (chr1 && chr1->prop) {
		if (checktype == COMPARE_ANY) {
			return true;
		}

		if (checktype == COMPARE_FRIENDS) { // Return true if chrs are friends
			if (g_Vars.normmplayerisrunning) {
				if ((g_MpSetup.options & MPOPTION_TEAMSENABLED) && chr2->team == chr1->team) {
					return true;
				}
			} else {
				if (g_Vars.bond && g_Vars.bond->prop) {
					struct chrdata *playerchr = g_Vars.bond->prop->chr;

					// @bug: This makes Jon an ally in Duel
					if ((chr2 == playerchr && chr1->headnum == HEAD_JONATHAN) ||
							(chr1 == playerchr && chr2->headnum == HEAD_JONATHAN)) {
						return true;
					}
				}

				if ((chr2->team & chr1->team) != 0) {
					return true;
				}
			}
		} else if (checktype == COMPARE_ENEMIES) { // Return true if chrs are enemies
			if (g_Vars.normmplayerisrunning) {
				if ((g_MpSetup.options & MPOPTION_TEAMSENABLED) == 0 || chr2->team != chr1->team) {
					return true;
				}
			} else {
				if (g_Vars.bond && g_Vars.bond->prop) {
					struct chrdata *playerchr = g_Vars.bond->prop->chr;

					// @bug: This makes Jon an ally in Duel
					if ((chr2 == playerchr && chr1->headnum == HEAD_JONATHAN) ||
							(chr1 == playerchr && chr2->headnum == HEAD_JONATHAN)) {
						return false;
					}
				}

				if ((chr2->team & chr1->team) == 0) {
					return true;
				}
			}
		}

		return false;
	}

	return false;
}

void chr_set_chr_preset(struct chrdata *chr, s32 chrpreset)
{
	chr->chrpreset1 = chr_resolve_id(chr, chrpreset);
}

void chr_set_chr_preset_by_chrnum(struct chrdata *basechr, s32 chrnum, s32 chrpreset)
{
	struct chrdata *chr = chr_find_by_id(basechr, chrnum);

	if (chr) {
		chr->chrpreset1 = chr_resolve_id(basechr, chrpreset);
	}
}

void chr_set_pad_preset(struct chrdata *chr, s32 pad_id)
{
	chr->padpreset1 = chr_resolve_pad_id(chr, pad_id);
}

void chr_set_pad_preset_by_chrnum(struct chrdata *basechr, s32 chrnum, s32 pad_id)
{
	struct chrdata *chr = chr_find_by_id(basechr, chrnum);

	if (chr) {
		chr->padpreset1 = chr_resolve_pad_id(basechr, pad_id);
	}
}

/**
 * Check if chr has line of sight to their target. If not, find a route
 * to them then iterate the route backwards (from target to chr).
 * Find the first waypoint in view of both the chr and their target
 * and store it as the chr's pad preset.
 */
bool chr_set_pad_preset_to_pad_on_route_to_target(struct chrdata *chr)
{
	struct prop *target = chr_get_target_prop(chr);
	struct prop *prop = chr->prop;
	struct waypoint *fromwp;
	struct waypoint *towp;
	struct waypoint *waypoints[5];
	s32 numwaypoints;
	s32 i;
	struct pad pad;

	if (target->type != PROPTYPE_PLAYER || g_Vars.bondvisible) {
		if (cd_test_los04(&prop->pos, prop->rooms, &target->pos, CDTYPE_BG)) {
			return false;
		}

		fromwp = waypoint_find_closest_to_pos(&prop->pos, prop->rooms);
		towp = waypoint_find_closest_to_pos(&target->pos, target->rooms);

		if (fromwp && towp) {
			// Note from/to are swapped here, so the route is from target to chr
			nav_set_seed(CHRNAVSEED(chr), CHRNAVSEED(chr));
			numwaypoints = nav_find_route(towp, fromwp, waypoints, 5);
			nav_set_seed(0, 0);

			if (numwaypoints >= 3) {
				for (i = 0; waypoints[i] != NULL; i++) {
					struct waypoint *wp = waypoints[i];

					pad_unpack(wp->padnum, PADFIELD_POS, &pad);

					if (cd_test_los04(&target->pos, target->rooms, &pad.pos, CDTYPE_BG)) {
						if (cd_test_los04(&prop->pos, prop->rooms, &pad.pos, CDTYPE_BG)) {
							chr->padpreset1 = wp->padnum;
							return true;
						}
					}
				}
			}
		}
	}

	return false;
}

bool chr_is_pos_offscreen(struct coord *pos, RoomNum *rooms)
{
	bool offscreen = true;
	s32 i;

	if (env_is_pos_in_fog_max_distance(pos, 0)) {
		for (i = 0; rooms[i] != -1; i++) {
			if (bg_room_is_onscreen(rooms[i])) {
				break;
			}
		}

		if (rooms[i] != -1) {
			// Room is visible by player
			offscreen = !cam_is_pos_in_screen_box(pos, 200, bg_get_room_draw_slot(rooms[i]));
		}
	}

	return offscreen;
}

/**
 * Test if a chr can be spawned into or nearby the given position, taking into
 * consideration collision checks.
 *
 * If the spawn can happen, the position and rooms are updated with the actual
 * position to be used and the function returns true.
 *
 * If the spawn cannot happen, the function return false.
 */
#if VERSION >= VERSION_NTSC_1_0
bool chr_adjust_pos_for_spawn(f32 chrradius, struct coord *pos, RoomNum *rooms, f32 angle, bool allowonscreen, bool force, bool onlysurrounding)
{
	struct coord testpos;
	s32 i;
	u32 types;
	RoomNum testrooms[8];
	f32 ymin;
	f32 ymax = 200;
	f32 curangle = angle;
	f32 ground;

	if (force) {
		types = CDTYPE_ALL & ~CDTYPE_BG;
		allowonscreen = true;
	} else {
		types = CDTYPE_ALL;
	}

	if (onlysurrounding) {
		// Skip testing the given pos, and just do the surrounding checks below.
		// Add 45 degrees to the angle here, but this isn't necessary.
		curangle += BADDTOR(45);

		if (curangle >= BADDTOR(360)) {
			curangle -= BADDTOR(360);
		}
	} else {
		// Check that the chr isn't being spawned out of bounds, and do a volume
		// test reaching 200cm above and below the chr's feet... unless there is
		// ground under the chr (highly likely), in which case reduce the volume
		// to be tested to the ground Y value. I'm not sure why this is useful,
		// because if the chr was being spawned on top of another chr or object
		// then the calculated ground value would be raised.
		ymin = -200;
		ground = cd_find_ground_at_cyl(pos, chrradius, rooms, NULL, NULL);

		if (ground > -100000 && ground - pos->y < -200) {
			ymin = ground - pos->y;
		}

		if (cd_test_volume(pos, chrradius, rooms, types, CHECKVERTICAL_YES, ymax, ymin) != CDRESULT_COLLISION
				&& (allowonscreen || chr_is_pos_offscreen(pos, rooms))) {
			return true;
		}
	}

	// Try 60cm in 8 directions
	for (i = 0; i < 8; i++) {
		testpos.x = pos->x + sinf(curangle) * 60;
		testpos.y = pos->y;
		testpos.z = pos->z + cosf(curangle) * 60;

		if ((onlysurrounding && cd_test_cyl_move04(pos, rooms, &testpos, testrooms, CDTYPE_ALL & ~CDTYPE_PLAYERS, 1, ymax, -200) != CDRESULT_COLLISION)
				|| (!onlysurrounding && cd_test_los11(pos, rooms, &testpos, testrooms, CDTYPE_BG))) {
			chr_find_entered_rooms_at_pos(NULL, &testpos, testrooms);
			ground = cd_find_ground_at_cyl(&testpos, chrradius, testrooms, 0, 0);
			ymin = -200;

			if (ground > -100000 && ground - pos->y < -200) {
				ymin = ground - pos->y;
			}

			if (cd_test_volume(&testpos, chrradius, testrooms, CDTYPE_ALL, CHECKVERTICAL_YES, ymax, ymin) != CDRESULT_COLLISION
					&& (allowonscreen || chr_is_pos_offscreen(&testpos, testrooms))
					&& (!onlysurrounding || ground > -100000)) {
				pos->x = testpos.x;
				pos->y = testpos.y;
				pos->z = testpos.z;
				rooms_copy(testrooms, rooms);
				return true;
			}
		}

		curangle += BADDTOR(45);

		if (curangle >= BADDTOR(360)) {
			curangle -= BADDTOR(360);
		}
	}

	return false;
}
#else
/**
 * ntsc-beta's version of this function doesn't have the onlysurrounding argument
 * nor out of bounds checking, and lacks the reduction for the volume test.
 */
bool chr_adjust_pos_for_spawn(f32 chrradius, struct coord *pos, RoomNum *rooms, f32 angle, bool allowonscreen, bool force)
{
	struct coord testpos;
	s32 i;
	u32 types;
	RoomNum testrooms[8];
	f32 curangle = angle;

	if (force) {
		types = CDTYPE_ALL & ~CDTYPE_BG;
		allowonscreen = true;
	} else {
		types = CDTYPE_ALL;
	}

	if (cd_test_volume(pos, chrradius, rooms, types, CHECKVERTICAL_YES, 200, -200) != CDRESULT_COLLISION
			&& (allowonscreen || chr_is_pos_offscreen(pos, rooms))) {
		return true;
	}

	for (i = 0; i < 8; i++) {
		testpos.x = sinf(curangle) * 60 + pos->x;
		testpos.y = pos->y;
		testpos.z = cosf(curangle) * 60 + pos->z;

		if (cd_test_los11(pos, rooms, &testpos, testrooms, CDTYPE_BG)
				&& cd_test_volume(&testpos, chrradius, testrooms, CDTYPE_ALL, CHECKVERTICAL_YES, 200, -200.0f) != CDRESULT_COLLISION
				&& (allowonscreen || chr_is_pos_offscreen(&testpos, testrooms))) {
			pos->x = testpos.x;
			pos->y = testpos.y;
			pos->z = testpos.z;
			rooms_copy(testrooms, rooms);
			return true;
		}

		curangle += BADDTOR(45);

		if (curangle >= BADDTOR(360)) {
			curangle -= BADDTOR(360);
		}
	}

	return false;
}
#endif

/**
 * Attempts to spawn a chr at the given coordinates.
 *
 * In low memory conditions, the function will iterate all existing chrs in
 * search of a corpse that can be reaped. If one is found then the reap will be
 * triggered, but the function will not attempt to spawn the chr until the next
 * time it's called.
 */
struct prop *chr_spawn_at_coord(s32 bodynum, s32 headnum, struct coord *pos, RoomNum *rooms, f32 angle, u8 *ailist, u32 spawnflags)
{
	struct prop *prop;
	struct coord pos2;
	RoomNum rooms2[8];
	s32 stack;

	if (chrs_get_num_free() > 1) {
		if (headnum < 0) {
			headnum = body_choose_head(bodynum);
		}

		pos2.x = pos->x;
		pos2.y = pos->y;
		pos2.z = pos->z;
		rooms_copy(rooms, rooms2);

#if VERSION >= VERSION_NTSC_1_0
		if (chr_adjust_pos_for_spawn(20, &pos2, rooms2, angle, (spawnflags & SPAWNFLAG_ALLOWONSCREEN) != 0, false, false))
#else
		if (chr_adjust_pos_for_spawn(20, &pos2, rooms2, angle, (spawnflags & SPAWNFLAG_ALLOWONSCREEN) != 0, false))
#endif
		{
			struct model *model = body_instantiate_model_with_spawnflags(bodynum, headnum, spawnflags);
			struct chrdata *chr;

			if (model) {
				prop = chr_create_with_model(model, &pos2, rooms2, angle, ailist);

				if (prop) {
					prop_activate_this_frame(prop);
					prop_enable(prop);

					chr = prop->chr;
					chr->headnum = headnum;
					chr->bodynum = bodynum;
					chr->race = body_get_race(chr->bodynum);
					chr->flags = 0;
					chr->flags2 = 0;
#if VERSION >= VERSION_NTSC_1_0
					chr->hidden2 |= CHRH2FLAG_SPAWNED;
#endif

					if (spawnflags & SPAWNFLAG_NOBLOOD) {
						chr->noblood = true;
					}

					return prop;
				}

				modelmgr_free_model(model);
			}
		}
	}

	// Low memory - find a corpse to reap
	if (chrs_get_num_free() < 4) {
#if VERSION >= VERSION_NTSC_1_0
		s32 stack2;
		struct chrdata *replacechr;
		s32 startindex;
		s32 index;
#else
		s32 startindex;
		struct chrdata *replacechr;
		s32 index;
#endif

		replacechr = NULL;
		startindex = random() % g_NumChrSlots;
		index = startindex;

#if VERSION < VERSION_NTSC_1_0
		if (startindex);
#endif

		do {
			if (g_ChrSlots[index].chrnum >= 0 && g_ChrSlots[index].model && g_ChrSlots[index].prop) {
				if (g_ChrSlots[index].actiontype == ACT_DEAD
#if VERSION >= VERSION_NTSC_1_0
						|| (g_ChrSlots[index].actiontype == ACT_DRUGGEDKO && (g_ChrSlots[index].chrflags & CHRCFLAG_KEEPCORPSEKO) == 0)
#endif
						) {
					// If we've found a chr that's ready to be reaped, great.
					// Bail out of the loop.
					if (g_ChrSlots[index].act_dead.invistimer60 >= TICKS(120)) {
						replacechr = &g_ChrSlots[index];
						break;
					}

					// Otherwise, this chr is dead/KO'ed and can be reaped as a
					// last resort, so store them and keep looping in search of
					// a better chr.
					if (replacechr == NULL) {
						replacechr = &g_ChrSlots[index];
					}
				}
			}

			index = (index + 1) % g_NumChrSlots;
		} while (index != startindex);

		if (replacechr) {
			replacechr->act_dead.fadewheninvis = true;
			replacechr->act_dead.fadenow = true;
		}
	}

	return NULL;
}

struct prop *chr_spawn_at_pad(struct chrdata *basechr, s32 body, s32 head, s32 pad_id, u8 *ailist, u32 spawnflags)
{
	s32 resolved_pad_id = chr_resolve_pad_id(basechr, pad_id);
	struct pad pad;
	RoomNum room[2];
	f32 fvalue;
	pad_unpack(resolved_pad_id, PADFIELD_POS | PADFIELD_LOOK | PADFIELD_ROOM, &pad);
	fvalue = atan2f(pad.look.x, pad.look.z);
	room[0] = pad.room;
	room[1] = -1;

	return chr_spawn_at_coord(body, head, &pad.pos, &room[0], fvalue, ailist, spawnflags);
}

struct prop *chr_spawn_at_chr(struct chrdata *basechr, s32 body, s32 head, u32 chrnum, u8 *ailist, u32 spawnflags)
{
	struct chrdata *chr = chr_find_by_id(basechr, chrnum);
	f32 fvalue;

	if (1) {
		fvalue = chr_get_theta(chr);
	}

	return chr_spawn_at_coord(body, head, &chr->prop->pos, chr->prop->rooms, fvalue, ailist, spawnflags);
}

bool chr_is_prop_preset_blocking_sight_to_target(struct chrdata *chr)
{
	bool result = false;
	struct prop *prop = chr->prop;
	struct prop *target = chr_get_target_prop(chr);

	if (chr->proppreset1 >= 0) {
		chr_set_perim_enabled(chr, false);
		prop_set_perim_enabled(target, false);

		if (!cd_test_los04(&prop->pos, prop->rooms, &target->pos,
					CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_PATHBLOCKER | CDTYPE_BG)) {
			struct prop *obstacle = cd_get_obstacle_prop();

			if (obstacle && obstacle->type == PROPTYPE_OBJ
					&& chr->proppreset1 == (s16)(obstacle - g_Vars.props)) {
				result = true;
			}
		}

		chr_set_perim_enabled(chr, true);
		prop_set_perim_enabled(target, true);
	}

	return result;
}

void chr_0f04b740(void)
{
	// empty
}

bool chr_move_to_pos(struct chrdata *chr, struct coord *pos, RoomNum *rooms, f32 angle, bool force)
{
	struct coord pos2;
	RoomNum rooms2[8];
	bool result = false;
	u32 nodetype;
	union modelrwdata *rwdata;
	struct player *player;
	f32 ground;

	pos2.x = pos->x;
	pos2.y = pos->y;
	pos2.z = pos->z;

	rooms_copy(rooms, rooms2);
	prop_set_perim_enabled(chr->prop, false);

#if VERSION >= VERSION_NTSC_1_0
	if (chr_adjust_pos_for_spawn(chr->radius, &pos2, rooms2, angle, (chr->hidden & CHRHFLAG_WARPONSCREEN) != 0, force, (chr->hidden & CHRHFLAG_SPAWNONLYSURROUNDING) != 0))
#else
	if (chr_adjust_pos_for_spawn(chr->radius, &pos2, rooms2, angle, (chr->hidden & CHRHFLAG_WARPONSCREEN) != 0, force))
#endif
	{
		ground = cd_find_ground_info_at_cyl(&pos2, chr->radius, rooms2, &chr->floorcol,
				&chr->floortype, NULL, &chr->floorroom, NULL, NULL);

		chr->ground = ground;
		chr->manground = ground;
		chr->sumground = ground * (PAL ? 8.4175090789795f : 9.999998f);
		chr->prop->pos.x = pos2.x;
		chr->prop->pos.y = pos2.y;
		chr->prop->pos.z = pos2.z;

		prop_deregister_rooms(chr->prop);
		rooms_copy(rooms2, chr->prop->rooms);
		chr_detect_rooms(chr);
		model_set_root_position(chr->model, &pos2);

		nodetype = chr->model->definition->rootnode->type;

		if ((nodetype & 0xff) == MODELNODETYPE_CHRINFO) {
			rwdata = model_get_node_rw_data(chr->model, chr->model->definition->rootnode);
			rwdata->chrinfo.ground = ground;
		}

		chr->chrflags |= CHRCFLAG_FORCETOGROUND;
		chr_set_theta(chr, angle);

		if (chr->prop->type == PROPTYPE_PLAYER) {
			player = g_Vars.players[playermgr_get_player_num_by_prop(chr->prop)];
			player->vv_manground = ground;
			player->vv_ground = ground;
			player->vv_theta = BADRTOD4(BADDTOR(360) - angle);
			player->vv_verta = 0;
			player->unk1c64 = 1;
		}

		result = true;
	}

	prop_set_perim_enabled(chr->prop, true);

	return result;
}

bool chr_check_cover_out_of_sight(struct chrdata *chr, s32 covernum, bool soft)
{
	struct cover cover;
	struct prop *target;
	bool targetcanseecover;

	// @bug: Should be >= cover_get_count()
	if (covernum < 0 || covernum > cover_get_count() || !cover_unpack(covernum, &cover)) {
		return false;
	}

	target = chr_get_target_prop(chr);

	if (!target) {
		return false;
	}

	if (soft) {
		targetcanseecover = cd_test_los03(&target->pos, target->rooms, cover.pos,
				CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_BG,
				GEOFLAG_BLOCK_SIGHT);
	} else {
		targetcanseecover = cd_is_nearly_in_sight(&target->pos, target->rooms, cover.pos, 50.0f,
				CDTYPE_OBJS | CDTYPE_DOORS | CDTYPE_BG);
	}

	if (!targetcanseecover != false) {
		// Target can't see cover
		cover_set_out_of_sight(covernum, true);
		return true;
	}

	cover_set_out_of_sight(covernum, false);
	return false;
}

s32 chr_assign_cover_by_criteria(struct chrdata *chr, u16 criteria, s32 refdist)
{
	RoomNum rooms[8];
	struct cover cover;
	struct covercandidate tmp;
	s32 oldcover;
	s32 i;
	struct prop *roomprop;
	s32 numcovers = cover_get_count();
	s32 numcandidates = 0;
	struct prop *target = chr_get_target_prop(chr);
	bool userandomdist = false;
	bool changed;
	f32 sqdist;
	f32 y = chr->prop->pos.y + 170;
	s32 currefdist = refdist;
	struct prop *gotoprop;

	if (criteria & COVERCRITERIA_DISTTOFETCHPROP) {
		if (!chr->aibot || !chr->aibot->gotoprop) {
			return -1;
		}

		gotoprop = chr->aibot->gotoprop;
	}

	if (chr == NULL) {
		return 0;
	}

	oldcover = chr->cover;
	refdist *= refdist;

	// Iterate all cover, filter them by criteria and store them in g_CoverCandidates
	for (i = 0; i < numcovers; i++) {
		if (cover_unpack(i, &cover)
				&& !cover_is_special(&cover)
				&& ((criteria & COVERCRITERIA_2000) == 0 || (cover.flags & COVERFLAG_OMNIDIRECTIONAL))
				&& ((criteria & COVERCRITERIA_1000) || (cover.flags & COVERFLAG_AIMDIFFROOM) == 0 || !array_intersects(cover.rooms, target->rooms))) {
			userandomdist = false;

			if ((criteria & COVERCRITERIA_0001) && (criteria & COVERCRITERIA_FURTHEREST)) {
				userandomdist = true;
				criteria &= ~(COVERCRITERIA_0001 | COVERCRITERIA_FURTHEREST);
			}

			if (((criteria & COVERCRITERIA_FORCENEWCOVER) == 0 || i != oldcover)
					&& ((criteria & COVERCRITERIA_2000) || !(cover_is_in_use(i) || cover.pos->y > y))) {
				if (criteria & COVERCRITERIA_ROOMSFROMME) {
					roomprop = chr->prop;
				} else if (criteria & COVERCRITERIA_ROOMSFROMTARGET) {
					roomprop = target;
				} else if (criteria & COVERCRITERIA_DISTTOTARGET) {
					roomprop = target;
				} else {
					roomprop = chr->prop;
				}

				rooms[0] = roomprop->rooms[0];
				rooms[1] = -1;

				if (criteria & COVERCRITERIA_ALLOWNEIGHBOURINGROOMS) {
					bg_room_get_neighbours(roomprop->rooms[0], &rooms[1], 6);
				} else if (criteria & COVERCRITERIA_ONLYNEIGHBOURINGROOMS) {
					bg_room_get_neighbours(roomprop->rooms[0], &rooms[0], 7);
				}

				if (((criteria & COVERCRITERIA_0040) == 0 || !array_intersects(cover.rooms, rooms))
						&& ((criteria & COVERCRITERIA_0020) == 0 || array_intersects(cover.rooms, rooms))
						&& (rooms[1] == -1
							|| chr->oldrooms[0] == -1
							|| (criteria & COVERCRITERIA_0200) == 0
							|| !array_intersects(cover.rooms, chr->oldrooms))) {
					if (criteria & COVERCRITERIA_DISTTOME) {
						sqdist = chr_get_squared_distance_to_coord(chr, cover.pos);
					} else if (criteria & COVERCRITERIA_DISTTOTARGET) {
						sqdist = coord_get_squared_distance_to_coord(&target->pos, cover.pos);
					} else if (criteria & COVERCRITERIA_DISTTOFETCHPROP) {
						sqdist = coord_get_squared_distance_to_coord(&gotoprop->pos, cover.pos);
					} else if (userandomdist) {
						sqdist = random() % 0xf000;
					} else {
						sqdist = 0;
						currefdist = 0;
					}

					if (!currefdist
							|| (currefdist < 0 && sqdist > refdist)
							|| (currefdist > 0 && sqdist < refdist)) {
						g_CoverCandidates[numcandidates].sqdist = sqdist;
						g_CoverCandidates[numcandidates].covernum = i;
						numcandidates++;
					}
				}
			}
		}
	}

	// Sort candidates by distance ascending, or descending if using COVERCRITERIA_FURTHEREST
	if (numcandidates >= 2) {
		do {
			changed = false;

			for (i = 0; i < numcandidates - 1; i++) {
				if ((((criteria & COVERCRITERIA_0001) || (criteria & COVERCRITERIA_DISTTOFETCHPROP) || userandomdist) && (g_CoverCandidates[i].sqdist > g_CoverCandidates[i + 1].sqdist))
						|| ((criteria & COVERCRITERIA_FURTHEREST) && g_CoverCandidates[i].sqdist < g_CoverCandidates[i + 1].sqdist)) {
					changed = true;

					tmp = g_CoverCandidates[i];
					g_CoverCandidates[i] = g_CoverCandidates[i + 1];
					g_CoverCandidates[i + 1] = tmp;
				}
			}
		} while (changed);
	}

	// Assign the first out of sight cover
	for (i = 0; i < numcandidates; i++) {
		if (1 && chr_check_cover_out_of_sight(chr, g_CoverCandidates[i].covernum, criteria & COVERCRITERIA_ALLOWSOFT)) {
			chr->cover = g_CoverCandidates[i].covernum;

			if (oldcover != -1) {
				cover_set_in_use(oldcover, false);
			}

			cover_set_in_use(chr->cover, true);

			return g_CoverCandidates[i].covernum;
		}
	}

	return -1;
}

/**
 * Find cover in the opposite direction (?) of the chr's runfrompos and assign
 * it to the chr. The distance from the runfrompos to the cover must be between
 * mindist and maxdist along the X/Z plane. The chr will not choose cover more
 * than 170cm higher than their current elevation, but strangely there is no
 * lower Y limit.
 *
 * Testing with this function logic results in the following, where:
 * R = run from pos
 * C = chr's pos
 * A = available cover
 * . = unavailable cover
 *
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A R A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . .
 *  A A A A A A A A A A A A A A . . . . . A
 *  A A A A A A A A A A A A A A . . . A A A
 *  A A A A A A A A A A A A A A . A A A A A
 *  A A A A A A A A A A A A A C A A A A A A
 *  . . . . . . . . . . . A A A A A A A A A
 *  . . . . . . . . . A A A A A A A A A A A
 *
 * The block of available cover around the runfrompos might be a bug, but if
 * this function is called with mindist = chr - runfrompos or higher then it
 * will work as expected.
 *
 * Preference is given to cover which is the "most opposite", meaning ones
 * which are directly behind the chr from the perspective of runfrompos.
 */
s32 chr_assign_cover_away_from_danger(struct chrdata *chr, s32 mindist, s32 maxdist)
{
	s32 i;
	f32 vecfromdanger[2];
	f32 vectocover[2];
	f32 y;
	f32 ymax;
	f32 bestsqdist;
	f32 sqdist;
	s32 numcovers;
	s32 prevcover;
	s32 newcover;
	f32 coversqdistfrompos;
	struct cover cover;

	ymax = chr->prop->pos.y + 170;
	y = 0;
	bestsqdist = 0;
	newcover = -1;

	numcovers = cover_get_count();
	prevcover = chr->cover;

	mindist *= mindist;
	maxdist *= maxdist;

	if (mindist);
	if (maxdist);

	vecfromdanger[0] = chr->prop->pos.x - chr->runfrompos.x;
	vecfromdanger[1] = chr->prop->pos.z - chr->runfrompos.z;

	guNormalize(&vecfromdanger[0], &y, &vecfromdanger[1]);

	for (i = 0; i < numcovers; i++) {
		if (cover_unpack(i, &cover) && !cover_is_in_use(i) && !(cover.pos->y > ymax) && !cover_is_special(&cover)) {
			coversqdistfrompos = coord_get_squared_distance_to_coord(&chr->runfrompos, cover.pos);

			if (!(coversqdistfrompos < mindist) && !(coversqdistfrompos > maxdist)) {
				vectocover[0] = cover.pos->x - chr->prop->pos.x;
				vectocover[1] = cover.pos->z - chr->prop->pos.z;

				guNormalize(&vectocover[0], &y, &vectocover[1]);

				sqdist = vecfromdanger[0] * vectocover[0] + vecfromdanger[1] * vectocover[1];

				if (!(sqdist < 0) && sqdist > bestsqdist) {
					bestsqdist = sqdist;
					newcover = i;
					if (1);
				}
			}
		}

		if (numcovers);
	}

	chr->cover = newcover;

	if (newcover != -1) {
		if (prevcover != -1) {
			cover_set_in_use(prevcover, false);
		}

		cover_set_in_use(chr->cover, true);
	}

	return newcover;
}

s16 chr_go_to_cover(struct chrdata *chr, u8 speed)
{
	struct cover cover;

	if (!chr) {
		return 0;
	}

	if (chr_is_ready_for_orders(chr) && chr->cover != -1 && cover_unpack(chr->cover, &cover)) {
		chr_go_to_room_pos(chr, cover.pos, &cover.rooms[0], speed);
		return chr->cover;
	}

	return -1;
}

bool chr_run_from_pos(struct chrdata *chr, u32 goposflags, f32 rundist, struct coord *frompos)
{
	f32 curdistfrompos;
	struct coord delta;
	RoomNum rooms[8];

	if (chr_is_ready_for_orders(chr)) {
		delta.x = chr->prop->pos.x - frompos->x;
		delta.y = chr->prop->pos.y;
		delta.z = chr->prop->pos.z - frompos->z;

		// @bug: This check should be &&. The runfrompos will fail if the
		// frompos is on the same X or Z axis as the chr, which is unlikely
		// because it's a floating point number.
		if (delta.f[0] == 0.0f || delta.f[2] == 0.0f) {
			return false;
		}

		curdistfrompos = sqrtf(delta.f[0] * delta.f[0] + delta.f[2] * delta.f[2]);
		delta.x *= rundist / curdistfrompos;
		delta.z *= rundist / curdistfrompos;

		chr_set_perim_enabled(chr, false);

		if (cd_exam_los08(&chr->prop->pos, chr->prop->rooms, &delta, CDTYPE_ALL, GEOFLAG_WALL) == CDRESULT_COLLISION) {
#if VERSION >= VERSION_JPN_FINAL
			cd_get_pos(&delta, 18592, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
			cd_get_pos(&delta, 18555, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_BETA
			cd_get_pos(&delta, 18550, "chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
			cd_get_pos(&delta, 18547, "chraction.c");
#else
			cd_get_pos(&delta, 18277, "chraction.c");
#endif
		}

		chr_set_perim_enabled(chr, true);

		los_find_final_room_exhaustive(&chr->prop->pos, chr->prop->rooms, &delta, rooms);

		return chr_go_to_room_pos(chr, &delta, rooms, goposflags);
	}

	return false;
}

void chr_add_target_to_bdlist(struct chrdata *chr)
{
	if (chr->prop) {
		struct prop *target = chr_get_target_prop(chr);
		s32 i;

		if (target) {
			for (i = 0; i < g_Vars.lvupdate60; i++) {
				chr->bdlist[chr->bdstart] = target->pos.x - chr->prop->pos.x;
				chr->bdstart++;
				chr->bdstart %= 60;

				chr->bdlist[chr->bdstart] = target->pos.z - chr->prop->pos.z;
				chr->bdstart++;
				chr->bdstart %= 60;
			}
		}
	}
}

s32 chr_get_distance_lost_to_target_in_last_second(struct chrdata *chr)
{
	s32 *bdlist = &chr->bdlist[0];
	s32 index = chr->bdstart;
	u32 stack[2];

	s32 x1 = bdlist[(index + 1) % 60];
	s32 z1 = bdlist[index];
	s32 olddist = sqrtf(x1 * x1 + z1 * z1);

	s32 x2 = bdlist[(index + 59) % 60];
	s32 z2 = bdlist[(index + 58) % 60];
	s32 curdist = sqrtf(x2 * x2 + z2 * z2);

	return curdist - olddist;
}

bool chr_is_target_nearly_in_sight(struct chrdata *chr, u32 distance)
{
	struct prop *target = chr_get_target_prop(chr);

	return cd_is_nearly_in_sight(&chr->prop->pos, chr->prop->rooms, &target->pos, distance, CDTYPE_BG);
}

bool chr_is_nearly_in_targets_sight(struct chrdata *chr, u32 distance)
{
	struct prop *target = chr_get_target_prop(chr);

	return cd_is_nearly_in_sight(&target->pos, target->rooms, &chr->prop->pos, distance, CDTYPE_BG);
}

f32 chr_get_angle_from_targets_pov(struct chrdata *chr)
{
	f32 theta = 0;
	u32 stack;
	struct prop *target = chr_get_target_prop(chr);
	f32 angletotarget;
	f32 degrees;

	if (target->type == PROPTYPE_CHR) {
		// @bug: chr_get_theta returns the angle in radians but here we assume
		// it's already in degrees. Because of this, two AI chrs who are
		// fighting each other will get a bad value from this function.
		theta = chr_get_theta(target->chr);
	} else if (target->type == PROPTYPE_PLAYER) {
		s32 playernum = playermgr_get_player_num_by_prop(target);
		theta = g_Vars.players[playernum]->vv_theta;
	}

	angletotarget = atan2f(target->pos.z - chr->prop->pos.z, target->pos.x - chr->prop->pos.x);
	degrees = BADRTOD4(angletotarget) - theta + 90;

	if (degrees > 180.0f) {
		degrees -= 360.0f;
	}

	return degrees;
}

bool chr_flank(struct chrdata *chr, u32 angle360, struct coord *pos, u8 use_closest_side, u8 goposflags)
{
	struct prop *target = chr_get_target_prop(chr);
	f32 sqdist = 0;
	f32 targets_pov_angle = chr_get_angle_from_targets_pov(chr);
	f32 cosine;
	f32 sine;
	struct coord chrpos;
	f32 xdiff;
	f32 ydiff;
	f32 zdiff;
	f32 scale;
	bool again;
	struct coord saved;
	s32 result;
	f32 ymax;
	f32 ymin;
	f32 radius;

	chrpos.x = chr->prop->pos.x;
	chrpos.y = chr->prop->pos.y;
	chrpos.z = chr->prop->pos.z;

	do {
		f32 angle360f = angle360;

		if (angle360f > 180) {
			angle360f -= 360;
		}

		angle360f -= targets_pov_angle;
		angle360f *= BADDTOR(180) / 180.0f;

		cosine = cosf(angle360f);
		sine = sinf(angle360f);
		xdiff = chrpos.x - target->pos.x;
		zdiff = chrpos.z - target->pos.z;

		pos->x = target->pos.x + (xdiff * cosine - zdiff * sine);
		pos->y = chrpos.y;
		pos->z = target->pos.z + (xdiff * sine + zdiff * cosine);

		chr_get_bbox(chr->prop, &radius, &ymax, &ymin);

		result = cd_exam_cyl_move03(&chrpos, chr->prop->rooms, pos,
				CDTYPE_BG | CDTYPE_OBJS | CDTYPE_DOORS, 1,
				ymax - chrpos.f[1],
				ymin - chrpos.f[1]);

		if (result == CDRESULT_COLLISION) {
			f32 xdiff;
			f32 zdiff;
			f32 tmp;

#if VERSION >= VERSION_JPN_FINAL
			cd_get_pos(pos, 18731, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_FINAL
			cd_get_pos(pos, 18694, "chr/chraction.c");
#elif VERSION >= VERSION_PAL_BETA
			cd_get_pos(pos, 18689, "chraction.c");
#elif VERSION >= VERSION_NTSC_1_0
			cd_get_pos(pos, 18686, "chraction.c");
#else
			cd_get_pos(pos, 18416, "chraction.c");
#endif

			xdiff = pos->x - chrpos.x;
			zdiff = pos->z - chrpos.z;
			tmp = sqrtf(xdiff * xdiff + zdiff * zdiff);
			scale = (tmp - 50.0f) / tmp;

			if (scale < 0) {
				pos->x = chrpos.x;
				pos->y = chrpos.y;
				pos->z = chrpos.z;
			} else {
				xdiff *= scale;
				zdiff *= scale;

				pos->x = chrpos.x + xdiff;
				pos->y = chrpos.y;
				pos->z = chrpos.z + zdiff;
			}
		}

		if (use_closest_side) {
			xdiff = chrpos.x - pos->x;
			ydiff = chrpos.y - pos->y;
			zdiff = chrpos.z - pos->z;

			sqdist = xdiff * xdiff + ydiff * ydiff + zdiff * zdiff;

			saved.x = pos->x;
			saved.y = pos->y;
			saved.z = pos->z;

			angle360 = 360 - angle360;
			again = true;
			use_closest_side = 0;
		} else {
			again = false;
		}
	} while (again);

	if (sqdist != 0) {
		f32 sqdist2;
		xdiff = chrpos.x - pos->x;
		ydiff = chrpos.y - pos->y;
		zdiff = chrpos.z - pos->z;

		sqdist2 = xdiff * xdiff + ydiff * ydiff + zdiff * zdiff;

		if (sqdist2 < sqdist) {
			pos->x = saved.f[0];
			pos->y = saved.f[1];
			pos->z = saved.f[2];
		}
	}

	chr_go_to_pos(chr, pos, goposflags);

	return true;
}

/**
 * Chrs are assigned to teams, and they can be assigned to more than one.
 * The team assignments determine if a chr considers another chr to be friendly
 * or not.
 *
 * The chr->team value is a bitmask of which teams they belong to. There are 8
 * teams total.
 *
 * The team list is an array of 264 shorts. The first 7 are indexes into
 * the same list which mark the start of each team. Team 0 does not have
 * an entry in this list because it always starts at offset 7.
 *
 * Elements 7 onwards are chrnums. Each team is terminated with -2.
 */
void rebuild_teams(void)
{
	s32 numchrs = chrs_get_num_slots();
	s16 index = 7;
	s32 team;
	s32 i;
	struct chrdata *chr;
	u8 teammasks[] = { 0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80 };

	for (team = 0; team < ARRAYCOUNT(teammasks); team++) {
		if (team != 0) {
			g_TeamList[team - 1] = index;
		}

		for (i = 0; i < numchrs; i++) {
			chr = &g_ChrSlots[i];

			if (chr->chrnum >= 0 && (chr->team & teammasks[team])) {
				g_TeamList[index] = chr->chrnum;
				index++;
			}
		}

		g_TeamList[index] = -2;
		index++;

		if (index >= (MAX_CHRSPERTEAM + 1) * MAX_TEAMS) {
			break;
		}
	}
}

/**
 * Chrs are partitioned into squadrons for AI scripting purposes, where their
 * squadron number is in the range 0-15.
 *
 * The squadron list is an array of 272 shorts. The first 15 are indexes into
 * the same list which mark the start of each squadron. Squadron 0 does not have
 * an entry in this list because it always starts at offset 15.
 *
 * Elements 15 onwards are chrnums. Each squadron is terminated with -2.
 */
void rebuild_squadrons(void)
{
	s32 numchrs = chrs_get_num_slots();
	s16 index = 15;
	s32 squadron;
	s32 i;

	for (squadron = 0; squadron < MAX_SQUADRONS; squadron++) {
		if (squadron != 0) {
			g_SquadronList[squadron - 1] = index;
		}

		for (i = 0; i < numchrs; i++) {
			struct chrdata *chr = &g_ChrSlots[i];

			if (chr->chrnum >= 0 && chr->squadron == squadron) {
				if (chr->prop == NULL || chr->prop->type != PROPTYPE_PLAYER) {
					g_SquadronList[index] = chr->chrnum;
					index++;
				}
			}
		}

		g_SquadronList[index] = -2;
		index++;

		if (index >= (MAX_CHRSPERSQUADRON + 1) * MAX_SQUADRONS) {
			break;
		}
	}
}

s16 *team_get_chr_ids(s32 team_id)
{
	s32 i;
	u8 lookup[8] = {0x01, 0x02, 0x04, 0x08, 0x10, 0x20, 0x40, 0x80};

	for (i = 0; i < MAX_TEAMS; i++) {
		if (lookup[i] == team_id) {
			team_id = i;
			break;
		}
	}

	if (team_id < 0 || team_id >= MAX_TEAMS) {
		return NULL;
	}

	if (team_id != 0) {
		return &g_TeamList[g_TeamList[team_id - 1]];
	}

	return &g_TeamList[MAX_TEAMS - 1];
}

s16 *squadron_get_chr_ids(s32 squadron_id)
{
	if (squadron_id < 0 || squadron_id >= MAX_SQUADRONS) {
		return NULL;
	}

	if (squadron_id != 0) {
		return &g_SquadronList[g_SquadronList[squadron_id - 1]];
	}

	return &g_SquadronList[MAX_SQUADRONS - 1];
}

void audio_mark_as_recently_played(s16 audioid)
{
	g_RecentQuipsPlayed[g_RecentQuipsIndex++] = audioid;

	if (g_RecentQuipsIndex >= ARRAYCOUNT(g_RecentQuipsPlayed)) {
		g_RecentQuipsIndex = 0;
	}
}

bool audio_was_not_played_recently(s16 audioid)
{
	u8 i;

	for (i = 0; i < ARRAYCOUNT(g_RecentQuipsPlayed); i++) {
		if (g_RecentQuipsPlayed[i] == audioid) {
			return false;
		}
	}

	return true;
}

#if VERSION >= VERSION_NTSC_1_0
Gfx *chrs_render_chr_stats(Gfx *gdl, RoomNum *rooms)
{
	return gdl;
}
#else
Gfx *chrs_render_chr_stats(Gfx *gdl, RoomNum *rooms)
{
	s32 x;
	s32 y;
	s32 textwidth = 0;
	s32 textheight = 0;
	s32 x2;
	s32 y2;
	struct coord sp20c;
	f32 sp204[2];
	char fullbuffer[256];
	char aibotbuffer[120];
	u8 aibot = 0;
	s32 i;
	s32 numchrs = chrs_get_num_slots();

	gdl = text_begin(gdl);

	for (i = 0; i < numchrs; i++) {
		struct chrdata *chr = &g_ChrSlots[i];

		if (chr && chr->prop && array_intersects(chr->prop->rooms, rooms)) {
			sp20c.x = chr->prop->pos.x;
			sp20c.y = chr->ground + chr->height - 30;
			sp20c.z = chr->prop->pos.z;

			mtx4_transform_vec_in_place(g_Vars.currentplayer->worldtoscreenmtx, &sp20c);

			if (sp20c.z < -100 && sp20c.z > -1000) {
				cam0f0b4eb8(&sp20c, sp204, g_Vars.currentplayer->c_perspfovy, g_Vars.currentplayer->c_perspaspect);
				x = sp204[0];
				y = sp204[1];

				if (chr->aibot) {
					if (g_MpSetup.options & MPOPTION_TEAMSENABLED) {
						aibot = 1;
						sprintf(aibotbuffer, "\nTEAM %d: Cmd: %s", chr->team, bot_get_command_name(chr->aibot->command));
					}
				}

				sprintf(fullbuffer, "CHR[%d]\n%s\n%s\n%s%s", chr->chrnum,
						chr->myaction >= ARRAYCOUNT(g_ChrMyActionNames) ? "myaction?" : g_ChrMyActionNames[chr->myaction],
						chr->actiontype >= ARRAYCOUNT(g_ChrActionNames) || chr->actiontype < 0 ? "ACT_?" : g_ChrActionNames2[chr->actiontype],
						chr->actiontype == ACT_GOPOS || chr->actiontype == ACT_PATROL ? g_ChrLiftActionNames[chr->liftaction] : g_ChrLiftActionNames[0],
						aibot ? aibotbuffer : "");

				text_measure(&textheight, &textwidth, fullbuffer, g_CharsHandelGothicXs, g_FontHandelGothicXs, 0);

				x2 = x + textwidth;
				y2 = y + textheight;

				gdl = text_draw_black_box(gdl, &x, &y, &x2, &y2);
				gdl = text_render_v1(gdl, &x, &y, fullbuffer,
						g_CharsHandelGothicXs, g_FontHandelGothicXs,
						0xff8800aa, 0x00000088, vi_get_width(), vi_get_height(), 0, 0);
			}
		}
	}

	gdl = text_end(gdl);

	return gdl;
}
#endif

void chr_toggle_model_part(struct chrdata *chr, s32 partnum)
{
	if (chr && chr->model && chr->model->definition) {
		struct modelnode *node = model_get_part(chr->model->definition, partnum);
		union modelrwdata *rwdata = NULL;

		if (node) {
			rwdata = model_get_node_rw_data(chr->model, node);
		}

		if (rwdata) {
			bool visible = rwdata->toggle.visible;
			rwdata->toggle.visible = !visible;
		}
	}
}

/**
 * Make the chr do an avoiding animation, as if they are avoiding some object
 * being thrown at them.
 *
 * The chosen animation is based on the chr's relative angle to their target.
 *
 * Collision checks are done, and no animation will be done if there would be a
 * collision.
 */
void chr_avoid(struct chrdata *chr)
{
	// The first 4 items here are animation numbers
	// and the second 4 are their corresponding end frames.
	s32 anims[] = {
		ANIM_0064, ANIM_0065, ANIM_0066, ANIM_0067,
		48, 58, 35, 35
	};

	s32 animindex;
	struct prop *target = chr_get_target_prop(chr);
	f32 degrees = BADRTOD3(chr_get_angle_to_pos(chr, &target->pos));
	u32 chranimflags = 0;
	f32 ymax;
	f32 ymin;
	f32 radius;
	s32 cdresult = CDRESULT_NOCOLLISION;
	f32 xdiff;
	f32 zdiff;
	f32 halfchrradius;
	f32 chrangle = model_get_chr_rot_y(chr->model);
	RoomNum dstrooms[8];
	struct coord dstpos;

	// @bug: This shouldn't be here, and the perim is not enabled again
	// if the chr is not ready for orders.
	chr_set_perim_enabled(chr, false);

	if (chr_is_ready_for_orders(chr)) {
		chr_set_perim_enabled(chr, false);

		if (degrees > 45 && degrees <= 135) {
			animindex = 3;
			chranimflags = CHRANIMFLAG_FLIP;
			chrangle += DTOR(180);
		} else if (degrees > 135 && degrees < 225) {
			animindex = 2;

			if ((random() % 255) >= 2) {
				cdresult = CDRESULT_COLLISION;
				chr->chrflags &= ~CHRCFLAG_AVOIDING;
			}
		} else if (degrees > 225 && degrees < 315) {
			animindex = 3;
			chrangle += DTOR(180);
		} else {
			animindex = (random() % 100 < 50) ? 0 : 1;

			if (animindex == 0) {
				chrangle -= DTOR(90);
			} else {
				chrangle += DTOR(90);
			}
		}

		if (cdresult != CDRESULT_COLLISION) {
			dstpos.x = chr->prop->pos.x - sinf(chrangle) * 100;
			dstpos.y = chr->prop->pos.y;
			dstpos.z = chr->prop->pos.z + cosf(chrangle) * 100;

			chr_get_bbox(chr->prop, &radius, &ymax, &ymin);

			halfchrradius = radius * 0.5f;

			los_find_final_room_exhaustive(&chr->prop->pos, chr->prop->rooms, &dstpos, dstrooms);
			chr_find_entered_rooms_at_pos(chr, &dstpos, dstrooms);

			xdiff = dstpos.x - chr->prop->pos.x;
			zdiff = dstpos.z - chr->prop->pos.z;

			if (xdiff > halfchrradius || zdiff > halfchrradius || xdiff < -halfchrradius || zdiff < -halfchrradius) {
				cdresult = cd_exam_cyl_move05(&chr->prop->pos, chr->prop->rooms, &dstpos, dstrooms, CDTYPE_ALL, true, ymax - chr->prop->pos.y, ymin - chr->prop->pos.y);
			}

			if (cdresult == CDRESULT_ERROR) {
				chr->chrflags &= ~CHRCFLAG_AVOIDING;
			} else if (cdresult == CDRESULT_NOCOLLISION) {
				chr_anim(chr, anims[animindex], 0, anims[4 + animindex], chranimflags, 2, 0.6f);
			} else {
				chr->chrflags &= ~CHRCFLAG_AVOIDING;
			}
		}

		chr_set_perim_enabled(chr, true);
	}
}

/**
 * Checks if the chr is doing an avoiding animation.
 *
 * These animations are possibly unused. In each one, the chr jumps backwards or
 * to the side as if avoiding something, then looks at whatever it was that just
 * went past.
 */
bool chr_is_avoiding(struct chrdata *chr)
{
	s32 anim = model_get_anim_num(chr->model);
	chr->chrflags &= ~CHRCFLAG_AVOIDING;

	// Possible @bug or just sloppy code: The flag check below can never pass
	// because that flag was just turned off above.
	if (anim == ANIM_0064
			|| anim == ANIM_0065
			|| anim == ANIM_0066
			|| anim == ANIM_0067
			|| (chr->chrflags & CHRCFLAG_AVOIDING)) {
		return true;
	}

	return false;
}

void chr_drcaroll_emit_sparks(struct chrdata *chr)
{
	if (chr && chr->prop) {
		ps_create(0, chr->prop, SFXNUM_0064_SHIELD_DAMAGE, -1, -1, 0, 0, PSTYPE_NONE, 0, -1, 0, -1, -1, -1, -1);
		sparks_create(chr->prop->rooms[0], chr->prop, &chr->prop->pos, NULL, 0, SPARKTYPE_ELECTRICAL);
	}
}
