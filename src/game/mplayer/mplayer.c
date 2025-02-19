#include <ultra64.h>
#include "constants.h"
#include "game/camdraw.h"
#include "game/title.h"
#include "game/pdmode.h"
#include "game/bondgun.h"
#include "game/gset.h"
#include "game/player.h"
#include "game/savebuffer.h"
#include "game/menu.h"
#include "game/playermgr.h"
#include "game/text.h"
#include "game/lv.h"
#include "game/music.h"
#include "game/mplayer/setup.h"
#include "game/mplayer/scenarios.h"
#include "game/mpstats.h"
#include "game/challenge.h"
#include "game/lang.h"
#include "game/mplayer/mplayer.h"
#include "game/pak.h"
#include "bss.h"
#include "lib/args.h"
#include "lib/vi.h"
#include "lib/main.h"
#include "lib/rng.h"
#include "lib/str.h"
#include "lib/lib_317f0.h"
#include "data.h"
#include "types.h"

// bss
struct chrdata *g_MpAllChrPtrs[MAX_MPCHRS];
struct mpchrconfig *g_MpAllChrConfigPtrs[MAX_MPCHRS];
s32 g_MpNumChrs;
struct mpbotconfig g_BotConfigsArray[MAX_BOTS];
u8 g_MpSimulantDifficultiesPerNumPlayers[MAX_BOTS][MAX_PLAYERS];
struct mpplayerconfig g_PlayerConfigsArray[MAX_MPPLAYERCONFIGS];
u8 g_AmBotCommands[9];
struct mpsetup g_MpSetup;
struct bossfile g_BossFile;
struct mplockinfo g_MpLockInfo;
struct modeldef *g_PheadModeldefs[18];

// Forward declaractions
struct mpweaponset g_MpWeaponSets[12];
s32 g_MpWeaponSetNum;

#if VERSION >= VERSION_NTSC_1_0
const char var7f1b8a00[] = "||||||||||||| Starting game... players %d\n";
#endif

s32 g_MpReturningFromMatch = 0;
bool g_MpEnableMusicSwitching = false;

struct mpweapon g_MpWeapons[NUM_MPWEAPONS] = {
	/*0x00*/ { WEAPON_NONE,             0,                    0,   0,                   0,  1, 0,                                MODEL_CHRTT33,          256 },
	/*0x01*/ { WEAPON_FALCON2,          AMMOTYPE_PISTOL,      80,  0,                   0,  1, 0,                                MODEL_CHRFALCON2,       256 },
	/*0x02*/ { WEAPON_FALCON2_SILENCER, AMMOTYPE_PISTOL,      80,  0,                   0,  1, MPFEATURE_WEAPON_FALCON2SILENCED, MODEL_CHRFALCON2SIL,    256 },
	/*0x03*/ { WEAPON_FALCON2_SCOPE,    AMMOTYPE_PISTOL,      80,  0,                   0,  1, MPFEATURE_WEAPON_FALCON2SCOPE,    MODEL_CHRFALCON2SCOPE,  256 },
	/*0x04*/ { WEAPON_MAGSEC4,          AMMOTYPE_PISTOL,      80,  0,                   0,  1, 0,                                MODEL_CHRLEEGUN1,       256 },
	/*0x05*/ { WEAPON_MAULER,           AMMOTYPE_PISTOL,      92,  0,                   0,  1, MPFEATURE_WEAPON_MAULER,          MODEL_CHRMAULER,        256 },
	/*0x06*/ { WEAPON_PHOENIX,          AMMOTYPE_PISTOL,      64,  0,                   0,  1, MPFEATURE_WEAPON_PHOENIX,         MODEL_CHRMAIANPISTOL,   256 },
	/*0x07*/ { WEAPON_DY357MAGNUM,      AMMOTYPE_MAGNUM,      50,  0,                   0,  1, 0,                                MODEL_CHRDY357,         256 },
	/*0x08*/ { WEAPON_DY357LX,          AMMOTYPE_MAGNUM,      50,  0,                   0,  1, MPFEATURE_WEAPON_DY357LX,         MODEL_CHRDY357TRENT,    256 },
	/*0x09*/ { WEAPON_CMP150,           AMMOTYPE_SMG,         100, 0,                   0,  1, 0,                                MODEL_CHRCMP150,        256 },
	/*0x0a*/ { WEAPON_CYCLONE,          AMMOTYPE_SMG,         150, 0,                   0,  1, 0,                                MODEL_CHRCYCLONE,       256 },
	/*0x0b*/ { WEAPON_CALLISTO,         AMMOTYPE_SMG,         150, 0,                   0,  1, MPFEATURE_WEAPON_CALLISTO,        MODEL_CHRMAIANSMG,      256 },
	/*0x0c*/ { WEAPON_RCP120,           AMMOTYPE_SMG,         150, 0,                   0,  1, MPFEATURE_WEAPON_RCP120,          MODEL_CHRRCP120,        256 },
	/*0x0d*/ { WEAPON_LAPTOPGUN,        AMMOTYPE_SMG,         150, 0,                   0,  1, MPFEATURE_WEAPON_LAPTOPGUN,       MODEL_CHRPCGUN,         256 },
	/*0x0e*/ { WEAPON_DRAGON,           AMMOTYPE_RIFLE,       150, 0,                   0,  1, 0,                                MODEL_CHRDRAGON,        256 },
	/*0x0f*/ { WEAPON_K7AVENGER,        AMMOTYPE_RIFLE,       150, 0,                   0,  1, MPFEATURE_WEAPON_K7AVENGER,       MODEL_CHRAVENGER,       256 },
	/*0x10*/ { WEAPON_AR34,             AMMOTYPE_RIFLE,       100, 0,                   0,  1, 0,                                MODEL_CHRAR34,          256 },
	/*0x11*/ { WEAPON_SUPERDRAGON,      AMMOTYPE_RIFLE,       150, AMMOTYPE_DEVASTATOR, 16, 1, MPFEATURE_WEAPON_SUPERDRAGON,     MODEL_CHRSUPERDRAGON,   256 },
	/*0x12*/ { WEAPON_SHOTGUN,          AMMOTYPE_SHOTGUN,     16,  0,                   0,  1, MPFEATURE_WEAPON_SHOTGUN,         MODEL_CHRSHOTGUN,       256 },
	/*0x13*/ { WEAPON_REAPER,           AMMOTYPE_REAPER,      200, 0,                   0,  1, MPFEATURE_WEAPON_REAPER,          MODEL_CHRSKMINIGUN,     256 },
	/*0x14*/ { WEAPON_SNIPERRIFLE,      AMMOTYPE_RIFLE,       50,  0,                   0,  1, 0,                                MODEL_CHRSNIPERRIFLE,   256 },
	/*0x15*/ { WEAPON_FARSIGHT,         AMMOTYPE_FARSIGHT,    10,  0,                   0,  1, MPFEATURE_WEAPON_FARSIGHT,        MODEL_CHRZ2020,         256 },
	/*0x16*/ { WEAPON_DEVASTATOR,       AMMOTYPE_DEVASTATOR,  16,  0,                   0,  1, MPFEATURE_WEAPON_DEVASTATOR,      MODEL_CHRDEVASTATOR,    256 },
	/*0x17*/ { WEAPON_ROCKETLAUNCHER,   AMMOTYPE_ROCKET,      3,   0,                   0,  1, 0,                                MODEL_CHRDYROCKET,      256 },
	/*0x18*/ { WEAPON_SLAYER,           AMMOTYPE_ROCKET,      3,   0,                   0,  1, MPFEATURE_WEAPON_SLAYER,          MODEL_CHRSKROCKET,      256 },
#if VERSION != VERSION_JPN_FINAL
	/*0x19*/ { WEAPON_COMBATKNIFE,      AMMOTYPE_KNIFE,       5,   0,                   0,  1, 0,                                MODEL_CHRKNIFE,         256 },
#endif
	/*0x1a*/ { WEAPON_CROSSBOW,         AMMOTYPE_CROSSBOW,    10,  0,                   0,  1, MPFEATURE_WEAPON_CROSSBOW,        MODEL_CHRCROSSBOW,      256 },
	/*0x1b*/ { WEAPON_TRANQUILIZER,     AMMOTYPE_SEDATIVE,    50,  0,                   0,  1, MPFEATURE_WEAPON_TRANQUILIZER,    MODEL_CHRDRUGGUN,       256 },
	/*0x1c*/ { WEAPON_GRENADE,          AMMOTYPE_GRENADE,     5,   0,                   0,  0, 0,                                MODEL_CHRGRENADE,       256 },
	/*0x1d*/ { WEAPON_NBOMB,            AMMOTYPE_NBOMB,       3,   0,                   0,  0, MPFEATURE_WEAPON_NBOMB,           MODEL_CHRNBOMB,         256 },
	/*0x1e*/ { WEAPON_TIMEDMINE,        AMMOTYPE_TIMED_MINE,  5,   0,                   0,  0, 0,                                MODEL_CHRTIMEDMINE,     384 },
	/*0x1f*/ { WEAPON_PROXIMITYMINE,    AMMOTYPE_PROXY_MINE,  5,   0,                   0,  0, MPFEATURE_WEAPON_PROXIMITYMINE,   MODEL_CHRPROXIMITYMINE, 384 },
	/*0x20*/ { WEAPON_REMOTEMINE,       AMMOTYPE_REMOTE_MINE, 5,   0,                   0,  0, MPFEATURE_WEAPON_REMOTEMINE,      MODEL_CHRREMOTEMINE,    384 },
	/*0x21*/ { WEAPON_LASER,            0,                    0,   0,                   0,  1, MPFEATURE_WEAPON_LASER,           MODEL_CHRLASER,         512 },
	/*0x22*/ { WEAPON_XRAYSCANNER,      0,                    0,   0,                   0,  1, MPFEATURE_WEAPON_XRAYSCANNER,     MODEL_CHRNIGHTSIGHT,    256 },
	/*0x23*/ { WEAPON_CLOAKINGDEVICE,   0,                    0,   0,                   0,  1, MPFEATURE_WEAPON_CLOAKINGDEVICE,  MODEL_CHRCLOAKER,       256 },
	/*0x24*/ { WEAPON_COMBATBOOST,      0,                    0,   0,                   0,  1, MPFEATURE_WEAPON_COMBATBOOST,     MODEL_CHRSPEEDPILL,     256 },
	/*0x25*/ { WEAPON_MPSHIELD,         0,                    0,   0,                   0,  1, MPFEATURE_WEAPON_SHIELD,          MODEL_CHRSHIELD,        256 },
	/*0x26*/ { WEAPON_DISABLED },
};

void mp_calculate_player_title(struct mpplayerconfig *mpplayer);
s32 mp_choose_random_lock_player(void);
s32 mp_get_chr_index_by_slot_num(s32 slot);

/**
 * Converts the given handicap into a float on a curved scale from 0.1 to 10.
 *
 * handicap 0 will return 0.1
 * handicap 127 will return 1
 * handicap 255 will return 10
 */
f32 mp_handicap_to_value(u8 handicap)
{
	f32 tmp;

	if (handicap < 127) {
		return (handicap / 127.0f) * (handicap / 127.0f) * 0.9f + 0.1f;
	}

	if (handicap == 127) {
		return 1;
	}

	tmp = (handicap - 128) / 127.0f + 1;

	return tmp * tmp * 3 - 2;
}

void mp_reset_mpchrconfig_for_match(struct mpchrconfig *mpchr)
{
	s32 i = 0;

	while (i < ARRAYCOUNT(mpchr->killcounts)) {
		mpchr->killcounts[i++] = 0;
	}

	mpchr->numdeaths = 0;
	mpchr->numpoints = 0;
	mpchr->unk40 = 0;
}

void mp_start_match(void)
{
	s32 i;
	s32 numplayers = 0;
	s32 stagenum;

	mp_configure_quick_team_simulants();

	if (!challenge_is_feature_unlocked(MPFEATURE_ONEHITKILLS)) {
		g_MpSetup.options &= ~MPOPTION_ONEHITKILLS;
	}

	if (!challenge_is_feature_unlocked(MPFEATURE_SLOWMOTION)) {
		g_MpSetup.options &= ~(MPOPTION_SLOWMOTION_ON | MPOPTION_SLOWMOTION_SMART);
	}

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			numplayers++;
		}
	}

	stagenum = g_MpSetup.stagenum;

	if (g_MpSetup.stagenum == STAGE_MP_RANDOM) {
		stagenum = mp_choose_random_stage();
	}

	title_set_next_stage(stagenum);
	main_change_to_stage(stagenum);
	set_num_players(numplayers);
	title_set_next_mode(TITLEMODE_SKIP);

	g_Vars.perfectbuddynum = 1;
}

void mp_reset(void)
{
	s32 i;
	s32 mpindex = 0;

	g_MpNumChrs = 0;
	g_Vars.mplayerisrunning = true;

	if (g_Vars.coopplayernum >= 0 || g_Vars.antiplayernum >= 0) {
		g_Vars.normmplayerisrunning = false;
	} else {
		g_Vars.normmplayerisrunning = true;
	}

	g_Vars.perfectbuddynum = 0;

	if (mp_has_simulants()) {
		g_Vars.lvmpbotlevel = true;
	}

	if (g_Vars.coopplayernum >= 0 || g_Vars.antiplayernum >= 0) {
		struct mpplayerconfig tmp;

		tmp = g_PlayerConfigsArray[MAX_PLAYERS];
		g_PlayerConfigsArray[MAX_PLAYERS] = g_PlayerConfigsArray[0];
		g_PlayerConfigsArray[0] = tmp;

		tmp = g_PlayerConfigsArray[MAX_PLAYERS + 1];
		g_PlayerConfigsArray[MAX_PLAYERS + 1] = g_PlayerConfigsArray[1];
		g_PlayerConfigsArray[1] = tmp;

		// Player index 0
		g_Vars.playerstats[0].mpindex = 0;

		g_PlayerConfigsArray[0].contpad1 = 0;
		g_PlayerConfigsArray[0].contpad2 = 2;

		if ((g_Vars.coopplayernum >= 0 && g_Vars.coopradaron)
				|| (g_Vars.antiplayernum >= 0 && g_Vars.antiradaron)) {
			g_PlayerConfigsArray[0].base.displayoptions |= MPDISPLAYOPTION_RADAR;
		} else {
			g_PlayerConfigsArray[0].base.displayoptions &= ~MPDISPLAYOPTION_RADAR;
		}

		// Player index 1
		g_Vars.playerstats[1].mpindexu32 = 1;

		g_PlayerConfigsArray[1].contpad1 = 1;
		g_PlayerConfigsArray[1].contpad2 = 3;

		if ((g_Vars.coopplayernum >= 0 && g_Vars.coopradaron)
				|| (g_Vars.antiplayernum >= 0 && g_Vars.antiradaron)) {
			g_PlayerConfigsArray[1].base.displayoptions |= MPDISPLAYOPTION_RADAR;
		} else {
			g_PlayerConfigsArray[1].base.displayoptions &= ~MPDISPLAYOPTION_RADAR;
		}

		g_MpNumChrs = 2;
	} else {
		for (i = 0; i < MAX_PLAYERS; i++) {
			if (g_MpSetup.chrslots & (1 << i)) {
				g_Vars.playerstats[mpindex].mpindex = i;

				g_PlayerConfigsArray[i].contpad1 = i;
				g_PlayerConfigsArray[i].contpad2 = 0;

				mp_calculate_player_title(&g_PlayerConfigsArray[i]);

				g_PlayerConfigsArray[i].newtitle = g_PlayerConfigsArray[i].title;
				g_MpNumChrs++;
				mpindex++;
			}
		}
	}

	for (i = 0; i < MAX_MPCHRS; i++) {
		struct mpchrconfig *mpchr = MPCHR(i);

		mp_reset_mpchrconfig_for_match(mpchr);

#if VERSION >= VERSION_NTSC_1_0
		g_MpAllChrPtrs[i] = NULL;
#endif
	}

	g_MpSetup.paused = false;

	g_MpEnableMusicSwitching = false;

	if (g_Vars.normmplayerisrunning) {
		if (mp_get_using_multiple_tunes()) {
			s32 i = 0;
			s32 count = 0;
			s32 numtracks = mp_get_num_unlocked_tracks();

			for (i; i < numtracks && count < 2; i++) {
				if (mp_is_multi_track_slot_enabled(i)) {
					count++;
				}
			}

			if (count >= 2) {
				g_MpEnableMusicSwitching = true;
			}
		}

		g_MpLockInfo.unk04 = -1;
	}

	// Assign aibot commands to active menu slots
	for (i = 0; i < ARRAYCOUNT(g_AmBotCommands); i++) {
		g_AmBotCommands[i] = AIBOTCMD_NORMAL;
	}

	g_AmBotCommands[1] = AIBOTCMD_ATTACK;
	g_AmBotCommands[7] = AIBOTCMD_NORMAL;
	g_AmBotCommands[0] = AIBOTCMD_FOLLOW;
	g_AmBotCommands[2] = AIBOTCMD_PROTECT;
	g_AmBotCommands[3] = AIBOTCMD_DEFEND;
	g_AmBotCommands[5] = AIBOTCMD_HOLD;

	switch (g_MpSetup.scenario) {
	case MPSCENARIO_CAPTURETHECASE:
		g_AmBotCommands[3] = AIBOTCMD_GETCASE;
		g_AmBotCommands[5] = AIBOTCMD_SAVECASE;
		break;
	case MPSCENARIO_KINGOFTHEHILL:
		g_AmBotCommands[6] = AIBOTCMD_DEFHILL;
		g_AmBotCommands[8] = AIBOTCMD_HOLDHILL;
		break;
	case MPSCENARIO_HACKERCENTRAL:
		g_AmBotCommands[6] = AIBOTCMD_DOWNLOAD;
		g_AmBotCommands[8] = AIBOTCMD_DOWNLOAD;
		break;
	case MPSCENARIO_POPACAP:
		g_AmBotCommands[6] = AIBOTCMD_POPCAP;
		g_AmBotCommands[8] = AIBOTCMD_POPCAP;
		break;
	case MPSCENARIO_HOLDTHEBRIEFCASE:
		g_AmBotCommands[6] = AIBOTCMD_GETCASE2;
		g_AmBotCommands[8] = AIBOTCMD_GETCASE2;
		break;
	default:
		g_AmBotCommands[0] = AIBOTCMD_FOLLOW;
		g_AmBotCommands[1] = AIBOTCMD_ATTACK;
		g_AmBotCommands[2] = AIBOTCMD_PROTECT;
		g_AmBotCommands[3] = AIBOTCMD_DEFEND;
		g_AmBotCommands[5] = AIBOTCMD_HOLD;
		g_AmBotCommands[7] = AIBOTCMD_NORMAL;
		break;
	}
}

void mp_calculate_team_is_only_ai(void)
{
	s32 playercount = PLAYERCOUNT();
	s32 i;
	s32 j;

	// Iterate simulants, which go after players in the g_MpAllChrPtrs array
	for (i = playercount; i < g_MpNumChrs; i++) {
		if (!g_MpAllChrPtrs[i]) {
			continue;
		}

		g_MpAllChrPtrs[i]->aibot->teamisonlyai = true;

		if (g_MpSetup.options & MPOPTION_TEAMSENABLED) {
			// Iterate human players
			for (j = 0; j < playercount; j++) {
				if (g_MpAllChrPtrs[i]->team == g_MpAllChrPtrs[j]->team) {
					g_MpAllChrPtrs[i]->aibot->teamisonlyai = false;
					break;
				}
			}
		}
	}
}

void mp_init_handicaps(s32 playernum)
{
	g_PlayerConfigsArray[playernum].base.unk18 = 80;
	g_PlayerConfigsArray[playernum].base.unk1a = 80;
	g_PlayerConfigsArray[playernum].base.unk1c = 75;
}

void mp_init_limits(void)
{
	g_MpSetup.timelimit = 9;
	g_MpSetup.scorelimit = 9;
	g_MpSetup.teamscorelimit = 19;
}

void mp_player_set_defaults(s32 playernum, bool autonames)
{
	s32 i;
	s32 j;

	mp_init_handicaps(playernum);

	g_PlayerConfigsArray[playernum].controlmode = CONTROLMODE_11;

	g_PlayerConfigsArray[playernum].options = OPTION_LOOKAHEAD
		| OPTION_SIGHTONSCREEN
		| OPTION_AUTOAIM
		| OPTION_AMMOONSCREEN
		| OPTION_SHOWGUNFUNCTION
		| OPTION_HEADROLL
		| OPTION_0100
		| OPTION_ALWAYSSHOWTARGET
		| OPTION_SHOWZOOMRANGE;

	g_PlayerConfigsArray[playernum].handicap = 128;

	switch (playernum) {
	case 0:
	default:
		g_PlayerConfigsArray[playernum].base.mpbodynum = MPBODY_DARK_COMBAT;
		break;
	case 1:
		g_PlayerConfigsArray[playernum].base.mpbodynum = MPBODY_CASSANDRA;
		break;
	case 2:
		g_PlayerConfigsArray[playernum].base.mpbodynum = MPBODY_CARRINGTON;
		break;
	case 3:
		g_PlayerConfigsArray[playernum].base.mpbodynum = MPBODY_CILABTECH;
		break;
	}

	g_PlayerConfigsArray[playernum].base.mpheadnum = mp_get_mpheadnum_by_mpbodynum(g_PlayerConfigsArray[playernum].base.mpbodynum);
	g_PlayerConfigsArray[playernum].base.displayoptions = MPDISPLAYOPTION_RADAR | MPDISPLAYOPTION_HIGHLIGHTTEAMS;
	g_PlayerConfigsArray[playernum].fileguid.fileid = 0;
	g_PlayerConfigsArray[playernum].fileguid.deviceserial = 0;

	if (autonames) {
		// "Player 1" etc
		sprintf(g_PlayerConfigsArray[playernum].base.name, "%s %d\n", lang_get(L_MISC_437), playernum + 1);
	} else {
		g_PlayerConfigsArray[playernum].base.name[0] = '\0';
	}

	g_PlayerConfigsArray[playernum].kills = 0;
	g_PlayerConfigsArray[playernum].deaths = 0;
	g_PlayerConfigsArray[playernum].gamesplayed = 0;
	g_PlayerConfigsArray[playernum].gameswon = 0;
	g_PlayerConfigsArray[playernum].gameslost = 0;
	g_PlayerConfigsArray[playernum].time = 0;
	g_PlayerConfigsArray[playernum].distance = 0;
	g_PlayerConfigsArray[playernum].accuracy = 1000;
	g_PlayerConfigsArray[playernum].damagedealt = 0;
	g_PlayerConfigsArray[playernum].painreceived = 0;
	g_PlayerConfigsArray[playernum].headshots = 0;
	g_PlayerConfigsArray[playernum].ammoused = 0;
	g_PlayerConfigsArray[playernum].accuracymedals = 0;
	g_PlayerConfigsArray[playernum].headshotmedals = 0;
	g_PlayerConfigsArray[playernum].killmastermedals = 0;
	g_PlayerConfigsArray[playernum].survivormedals = 0;
	g_PlayerConfigsArray[playernum].title = MPPLAYERTITLE_BEGINNER;

	if (playernum < MAX_PLAYERS) {
		for (i = 0; i < ARRAYCOUNT(g_MpChallenges); i++) {
			for (j = 1; j <= MAX_PLAYERS; j++) {
				challenge_set_completed_by_player_with_num_players(playernum, i, j, false);
			}
		}

		challenge_determine_unlocked_features();
	}

	for (i = 0; i < ARRAYCOUNT(g_PlayerConfigsArray); i++) {
		g_PlayerConfigsArray[playernum].gunfuncs[i] = 0;
	}
}

void mp_init_botconfig(s32 index)
{
	g_BotConfigsArray[index].base.name[0] = '\0';
	g_BotConfigsArray[index].base.mpheadnum = MPHEAD_DARK_COMBAT;
	g_BotConfigsArray[index].base.mpbodynum = MPBODY_DARK_COMBAT;
	g_BotConfigsArray[index].type = BOTTYPE_GENERAL;
	g_BotConfigsArray[index].difficulty = BOTDIFF_DISABLED;
}

void mp_init(void)
{
	s32 i;
	s32 j;

	g_MpSetup.scenario = MPSCENARIO_COMBAT;
	g_MpSetup.stagenum = STAGE_MP_SKEDAR;
	g_MpSetup.options = MPOPTION_DISPLAYTEAM
		| MPOPTION_KILLSSCORE
		| MPOPTION_HTB_HIGHLIGHTBRIEFCASE
		| MPOPTION_HTB_SHOWONRADAR
		| MPOPTION_CTC_SHOWONRADAR
		| MPOPTION_KOH_HILLONRADAR
		| MPOPTION_KOH_MOBILEHILL
		| MPOPTION_00010000
		| MPOPTION_HTM_HIGHLIGHTTERMINAL
		| MPOPTION_HTM_SHOWONRADAR
		| MPOPTION_PAC_HIGHLIGHTTARGET
		| MPOPTION_PAC_SHOWONRADAR;

	g_Vars.mphilltime = 10;

	mp_init_limits();

	g_MpSetup.fileguid.fileid = 0;
	g_MpSetup.fileguid.deviceserial = 0;

	strcpy(g_MpSetup.name, "");

	for (i = 0; i < ARRAYCOUNT(g_PlayerConfigsArray); i++) {
		mp_player_set_defaults(i, false);
	}

	for (i = 0; i < MAX_BOTS; i++) {
		mp_init_botconfig(i);
	}

	if (arg_find_by_prefix(1, "-mpwpnset")) {
		char *value = arg_find_by_prefix(1, "-mpwpnset");
		mp_set_weaponset_slotnum(*value - '0');
	} else {
		mp_set_weaponset_slotnum(0);
	}

	g_Vars.mplayerisrunning = false;
	g_Vars.normmplayerisrunning = false;
	g_Vars.lvmpbotlevel = 0;

	g_MpLockInfo.lockedplayernum = 0;
	g_MpLockInfo.lastwinner = -1;
	g_MpLockInfo.lastloser = -1;
	g_MpLockInfo.unk03 = -1;
	g_MpLockInfo.unk04 = -1;

	challenge_force_unlock_bot_features();

	for (i = 0; i < ARRAYCOUNT(g_PlayerConfigsArray); i++) {
		for (j = 0; j < ARRAYCOUNT(g_PlayerConfigsArray[i].gunfuncs); j++) {
			g_PlayerConfigsArray[i].gunfuncs[j] = 0;
		}
	}

	g_MpSetup.chrslots = 0;
}

#if VERSION >= VERSION_PAL_BETA
void mp_get_teams_with_default_name(u8 *mask)
{
	s32 i;

	*mask = 0;

	for (i = 0; i < ARRAYCOUNT(g_BossFile.teamnames); i++) {
		if (strcmp(g_BossFile.teamnames[i], lang_get(L_OPTIONS_008 + i)) == 0) {
			*mask |= 1 << i;
		}
	}
}
#endif

#if VERSION >= VERSION_PAL_BETA
void mp_set_team_names_to_default(u8 mask)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_BossFile.teamnames); i++) {
		if (mask & (1 << i)) {
			strcpy(g_BossFile.teamnames[i], lang_get(L_OPTIONS_008 + i));
		}
	}
}
#endif

void mp_set_default_names_if_empty(void)
{
	s32 i;

	// Setup file name
	if (g_MpSetup.name[0] == '\0') {
		strcpy(g_MpSetup.name, lang_get(L_MISC_438)); // empty string
	}

	// Team names
	for (i = 0; i < ARRAYCOUNT(g_BossFile.teamnames); i++) {
		if (g_BossFile.teamnames[i][0] == '\0') {
			strcpy(g_BossFile.teamnames[i], lang_get(L_OPTIONS_008 + i)); // "Red", "Yellow" etc
		}
	}

	// Player names
	for (i = 0; i < MAX_PLAYERS; i++) {
		if (g_PlayerConfigsArray[i].base.name[0] == '\0') {
			sprintf(g_PlayerConfigsArray[i].base.name, "%s %d\n", lang_get(L_MISC_437), i + 1); // "Player 1" etc
		}
	}
}

s32 mp_calculate_team_score_limit(void)
{
	s32 limit = g_MpSetup.teamscorelimit;
	s32 i;

	if (g_BossFile.locktype == MPLOCKTYPE_CHALLENGE
			&& g_MpSetup.teamscorelimit != 400
			&& (g_MpSetup.scenario == MPSCENARIO_COMBAT || g_MpSetup.scenario == MPSCENARIO_KINGOFTHEHILL)) {
		s32 numchrs = 0;

		for (i = 0; i < MAX_PLAYERS; i++) {
			if (g_MpSetup.chrslots & (1 << i)) {
				numchrs++;
			}
		}

		switch (numchrs) {
		case 1:
			break;
		case 2:
			limit = limit * 2 + 1;
			break;
		case 3:
			limit = (limit * 5 + 5) / 2 - 1;
			break;
		case 4:
			limit = limit * 3 + 2;
			break;
		}
	}

	return limit;
}

void mp_apply_limits(void)
{
	if (g_MpSetup.timelimit >= 60) {
		lv_set_mp_time_limit60(0);
	} else {
		lv_set_mp_time_limit60(SECSTOTIME60((g_MpSetup.timelimit + 1) * 60));
	}

	if (g_MpSetup.scorelimit >= 100) {
		lv_set_mp_score_limit(0);
	} else {
		lv_set_mp_score_limit(g_MpSetup.scorelimit + 1);
	}

	if (g_MpSetup.teamscorelimit >= 400) {
		lv_set_mp_team_score_limit(0);
	} else {
		lv_set_mp_team_score_limit(mp_calculate_team_score_limit() + 1);
	}
}

/**
 * Given a pointer to an array of 12 ranking structs, populate those rankings
 * with the winner first, loser last. Return the number of rows (players).
 *
 * The function also populates the placement an rankablescore values in the
 * mpchr structs and calculates the last winner and loser for the lock settings.
 */
s32 mp_get_player_rankings(struct ranking *rankings)
{
	s32 i;
	s32 scores[MAX_MPCHRS];
	u32 rankablescores[MAX_MPCHRS];
	struct mpchrconfig *mpchrs[MAX_MPCHRS];
	s32 chrnums[MAX_MPCHRS];
	s32 count = 0;
	s32 numteams;
	struct ranking teamrankings[MAX_MPCHRS];
	s32 winner;
	s32 loser;
	s32 score;
	s32 deaths;
	struct mpchrconfig *mpchr;
	s32 dstindex;
	bool found;
	u32 rankablescore;
	s32 j;

	if (g_MpSetup.options & MPOPTION_TEAMSENABLED) {
		numteams = mp_get_team_rankings(teamrankings);
	}

	// Populate 4 arrays with player info, sorted by highest score descending
	for (i = 0; i < MAX_MPCHRS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			mpchr = MPCHR(i);

			scenario_calculate_player_score(mpchr, i, &score, &deaths);

			rankablescore = (score + 0x8000) << 16 | (0xffff - deaths);
			dstindex = 0;
			found = false;

			// Find where this player should be placed in the sorted arrays
			for (j = 0; j < count && !found; j++) {
				if (rankablescore > rankablescores[j]) {
					dstindex = j;
					found = true;
				}
			}

			if (!found) {
				dstindex = count;
			}

			// Shuffle everything forward after dstindex to create a gap
			for (j = count; j > dstindex; j--) {
				rankablescores[j] = rankablescores[j - 1];
				scores[j] = scores[j - 1];
				mpchrs[j] = mpchrs[j - 1];
				chrnums[j] = chrnums[j - 1];
			}

			count++;

			// Write the new figures
			rankablescores[dstindex] = (score + 0x8000) << 16 | (0xffff - deaths);
			scores[dstindex] = score;
			mpchrs[dstindex] = mpchr;
			chrnums[dstindex] = i;
		}
	}

	// Populate the rankings array, copy some values into the mpchr structs
	// and calculate the winner and loser
	winner = -1;
	loser = -1;

	for (j = 0; j < count; j++) {
		rankings[j].mpchr = mpchrs[j];
		rankings[j].teamnum = chrnums[j];
		rankings[j].positionindex = j;
		rankings[j].unk0c = 0;
		rankings[j].score = scores[j];

		// For a team game, the mpchr's placement has to be the team's placement
		// and not the placement of the individual player.
		if (g_MpSetup.options & MPOPTION_TEAMSENABLED) {
			s32 placement = numteams - 1;
			s32 i;

			for (i = 0; i < numteams; i++) {
				if (teamrankings[i].teamnum == mpchrs[j]->team) {
					placement = i;
				}
			}

			mpchrs[j]->placement = placement;
#if VERSION >= VERSION_NTSC_1_0
			mpchrs[j]->rankablescore = 255 - placement;
#endif
		} else {
			mpchrs[j]->placement = j;
#if VERSION >= VERSION_NTSC_1_0
			mpchrs[j]->rankablescore = rankablescores[j];
#endif
		}

#if VERSION < VERSION_NTSC_1_0
		mpchrs[j]->rankablescore = rankablescores[j];
#endif

		if (chrnums[j] < 4) {
			loser = chrnums[j];

			if (winner == -1) {
				winner = chrnums[j];
				if (1);
			}
		}
	}

	g_MpLockInfo.lastwinner = winner;
	g_MpLockInfo.lastloser = loser;

	if (g_BossFile.locktype == MPLOCKTYPE_RANDOM) {
		g_MpLockInfo.lockedplayernum = mp_choose_random_lock_player();
	}

	return count;
}

/**
 * Iterate players on the given team and add their scores.
 *
 * There are two score values:
 * - The team score, which is written to the result pointer and eventually
 *   displayed on screen
 * - The rankable score, which is the returned value. The rankable score is
 *   similar to the team score but uses the number of deaths as a tie breaker.
 */
s32 mp_calculate_team_score(s32 teamnum, s32 *result)
{
	struct mpchrconfig *mpchr;
	s32 teamscore = 0;
	s32 teamdeaths = 0;
	bool teamexists = false;
	s32 i;
	u32 stack;
	s32 score;
	s32 deaths;

	for (i = 0; i < MAX_MPCHRS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			mpchr = MPCHR(i);

			if (mpchr->team == teamnum) {
				scenario_calculate_player_score(mpchr, i, &score, &deaths);
				teamexists = true;
				teamscore += score;
				teamdeaths += deaths;
			}
		}
	}

	if (teamexists) {
		*result = teamscore;
		return (teamscore + 0x8000) << 16 | (0xffff - teamdeaths);
	}

	return 0;
}

/**
 * Given a pointer to an array of 8 ranking structs, populate those rankings
 * with the winning team first, losing team last. Return the number of rows
 * (teams).
 */
s32 mp_get_team_rankings(struct ranking *rankings)
{
	s32 i;
	s32 count;
	s32 apparentscores[MAX_TEAMS];
	u32 rankablescores[MAX_TEAMS];
	u32 bestrankablescore;
	s32 thisteamnum;

	count = 0;

	// Calculate scores for each team
	for (i = 0; i < MAX_TEAMS; i++) {
		apparentscores[i] = -8000;
		rankablescores[i] = mp_calculate_team_score(i, &apparentscores[i]);
	}

	// Iterate each slot and determine which team belongs in it.
	// As each team is found, set their apparent score to -8000 so they're not
	// found again.
	do {
		bestrankablescore = 0;
		thisteamnum = -8000;

		// Check which team has the best remaining score
		for (i = 0; i < MAX_TEAMS; i++) {
			if (apparentscores[7 - i] > -8000 && rankablescores[7 - i] >= bestrankablescore) {
				thisteamnum = 7 - i;
				bestrankablescore = rankablescores[thisteamnum];
			}
		}

		if (thisteamnum > -8000) {
			// Append to the rankings array
			rankings[count].mpchr = NULL;
			rankings[count].teamnum = thisteamnum;
			rankings[count].positionindex = count + 1;
			rankings[count].unk0c = 0;
			rankings[count].score = apparentscores[thisteamnum];

			apparentscores[thisteamnum] = -8000;

			count++;
		}
	} while (thisteamnum != -8000);

	return count;
}

s32 mp_get_num_mpweapons(void)
{
	return NUM_MPWEAPONS;
}

s32 mp_get_num_weapon_options(void)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_MpWeapons); i++) {
		if (challenge_is_feature_unlocked(g_MpWeapons[i].unlockfeature)) {
			count++;
		}
	}

	return count;
}

char *mp_get_weapon_label(s32 weaponnum)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_MpWeapons); i++) {
		if (challenge_is_feature_unlocked(g_MpWeapons[i].unlockfeature)) {
			if (weaponnum == 0) {
				if (g_MpWeapons[i].weaponnum == WEAPON_NONE) {
					return lang_get(L_MPWEAPONS_058); // "Nothing"
				}

				if (g_MpWeapons[i].weaponnum == WEAPON_MPSHIELD) {
					return lang_get(L_MPWEAPONS_059); // "Shield"
				}

				if (g_MpWeapons[i].weaponnum == WEAPON_DISABLED) {
					return lang_get(L_MPWEAPONS_060); // "Disabled"
				}

				return bgun_get_name(g_MpWeapons[i].weaponnum);
			}

			weaponnum--;
		}
	}

	return "";
}

#if VERSION >= VERSION_NTSC_1_0
const char var7f1b8a5c[] = "Gun index %d -> slot %d = gun %d\n\n";
const char var7f1b8a80[] = "HOLDER: selecting weapon set %d\n";
#endif

void mp_set_weapon_slot(s32 slot, s32 mpweaponnum)
{
	s32 optionindex = mpweaponnum;
	s32 i;

	for (i = 0; i <= mpweaponnum; i++) {
		if (challenge_is_feature_unlocked(g_MpWeapons[i].unlockfeature) == 0) {
			mpweaponnum++;
		}

		optionindex = mpweaponnum;
	}

	g_MpSetup.weapons[slot] = optionindex;
}

s32 mp_get_weapon_slot(s32 slot)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i < g_MpSetup.weapons[slot]; i++) {
		if (challenge_is_feature_unlocked(g_MpWeapons[i].unlockfeature)) {
			count++;
		}
	}

	return count;
}

struct mpweapon *mp_get_mp_weapon_by_location(s32 locationindex)
{
	s32 v0 = locationindex + 1;
	s32 slot = 0;
	s32 a2 = v0;
	u8 mpweaponnum;

	while (v0 > 0) {
		mpweaponnum = g_MpSetup.weapons[slot];

		if (g_MpWeapons[mpweaponnum].weaponnum != WEAPON_DISABLED) {
			v0--;
		}

		if (v0 > 0) {
			slot++;

			if (slot >= ARRAYCOUNT(g_MpSetup.weapons)) {
				slot = 0;

				if (a2 == v0) {
					mpweaponnum = 0;
					v0 = 0;
				}

				a2 = v0;
			}
		}
	}

	return &g_MpWeapons[mpweaponnum];
}

#define MPWEAPONSET_IS_FULLY_UNLOCKED(mpweaponsetnum) \
	(challenge_is_feature_unlocked(g_MpWeaponSets[mpweaponsetnum].requirefeatures[0]) \
	&& challenge_is_feature_unlocked(g_MpWeaponSets[mpweaponsetnum].requirefeatures[1]) \
	&& challenge_is_feature_unlocked(g_MpWeaponSets[mpweaponsetnum].requirefeatures[2]) \
	&& challenge_is_feature_unlocked(g_MpWeaponSets[mpweaponsetnum].requirefeatures[3]))

#define MPWEAPONSET_IS_AVAILABLE(mpweaponsetnum) \
	(MPWEAPONSET_IS_FULLY_UNLOCKED(mpweaponsetnum) || g_MpWeaponSets[i].slotsiflocked[0] != WEAPON_DISABLED)

s32 mp_mpweaponset_to_slotnum(s32 slotnum)
{
	s32 i;
	s32 count = 0;

	if (slotnum >= ARRAYCOUNT(g_MpWeaponSets)) {
		// Custom, Random or Random Five
		count = slotnum - ARRAYCOUNT(g_MpWeaponSets);
		slotnum = ARRAYCOUNT(g_MpWeaponSets);
	}

	for (i = 0; i < slotnum; i++) {
		if (MPWEAPONSET_IS_AVAILABLE(i)) {
			count++;
		}
	}

	return count;
}

s32 mp_slotnum_to_mpweaponset(s32 mpweaponset)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_MpWeaponSets); i++) {
		if (MPWEAPONSET_IS_AVAILABLE(i)) {
			if (mpweaponset == 0) {
				break;
			}

			mpweaponset--;
		}
	}

	return i + mpweaponset;
}

s32 mp_get_num_weaponset_slots(bool full)
{
	return mp_mpweaponset_to_slotnum(full ? ARRAYCOUNT(g_MpWeaponSets) + 3 : ARRAYCOUNT(g_MpWeaponSets));
}

s32 mp_get_custom_weaponset_slot(void)
{
	return mp_mpweaponset_to_slotnum(ARRAYCOUNT(g_MpWeaponSets) + 2);
}

char *mp_get_weaponset_name_by_slotnum(s32 index)
{
	index = mp_slotnum_to_mpweaponset(index);

	if (index < 0 || index >= ARRAYCOUNT(g_MpWeaponSets) + 2) {
		return lang_get(L_MPWEAPONS_041); // "Custom"
	}

	if (index == ARRAYCOUNT(g_MpWeaponSets) + 1) {
		return lang_get(L_MPWEAPONS_042); // "Random"
	}

	if (index == ARRAYCOUNT(g_MpWeaponSets)) {
		return lang_get(L_MPWEAPONS_043); // "Random Five"
	}

	return lang_get(g_MpWeaponSets[index].name);
}

void mp_find_weaponsetnum_by_weapons(void)
{
	s32 i;
	bool done = false;
	u8 *slots;
	s32 j;

	for (i = 0; !done && i < ARRAYCOUNT(g_MpWeaponSets); i++) {
		if (MPWEAPONSET_IS_FULLY_UNLOCKED(i)) {
			slots = g_MpWeaponSets[i].slots;
		} else if (g_MpWeaponSets[i].slotsiflocked[0] != WEAPON_DISABLED) {
			slots = g_MpWeaponSets[i].slotsiflocked;
		} else {
			slots = NULL;
		}

		if (slots != NULL) {
			bool ok = true;

			for (j = 0; j < ARRAYCOUNT(g_MpWeaponSets[j].slots); j++) {
				s32 weaponnum = slots[j];

				if (weaponnum == WEAPON_MPSHIELD) {
					if (!challenge_is_feature_unlocked(MPFEATURE_WEAPON_SHIELD)) {
						weaponnum = 0;
					}
				}

				if (weaponnum != g_MpWeapons[g_MpSetup.weapons[j]].weaponnum) {
					ok = false;
				}
			}

			if (ok) {
				done = true;
				g_MpWeaponSetNum = i;
			}
		}
	}

	if (!done) {
		g_MpWeaponSetNum = WEAPONSET_CUSTOM;
	}
}

void mp_apply_weaponset(void)
{
	s32 i;
	u8 *slots;

	if (g_MpWeaponSetNum >= 0 && g_MpWeaponSetNum < ARRAYCOUNT(g_MpWeaponSets)) {
		if (MPWEAPONSET_IS_FULLY_UNLOCKED(g_MpWeaponSetNum)) {
			slots = g_MpWeaponSets[g_MpWeaponSetNum].slots;
		} else if (g_MpWeaponSets[g_MpWeaponSetNum].slotsiflocked[0] != WEAPON_DISABLED) {
			slots = g_MpWeaponSets[g_MpWeaponSetNum].slotsiflocked;
		} else {
			slots = NULL;
		}

		if (slots != NULL) {
			for (i = 0; i < ARRAYCOUNT(g_MpSetup.weapons); i++) {
				u32 j;
				bool done = false;
				s32 mpweaponnum = MPWEAPON_NONE;
				s32 weaponnum = slots[i];

				if (weaponnum == WEAPON_MPSHIELD && !challenge_is_feature_unlocked(MPFEATURE_WEAPON_SHIELD)) {
					weaponnum = 0;
				}

				for (j = 0; !done; j++) {
					if (j > MPWEAPON_DISABLED) {
						done = true;
					} else if (weaponnum == g_MpWeapons[j].weaponnum) {
						mpweaponnum = j;
						done = true;
					}
				}

				g_MpSetup.weapons[i] = mpweaponnum;
			}
		}
	} else if (g_MpWeaponSetNum == WEAPONSET_RANDOM) {
		s32 numoptions = mp_get_num_weapon_options();

		for (i = 0; i < ARRAYCOUNT(g_MpSetup.weapons); i++) {
			mp_set_weapon_slot(i, random() % numoptions);
		}
	} else if (g_MpWeaponSetNum == WEAPONSET_RANDOMFIVE) {
		s32 numoptions = mp_get_num_weapon_options() - 2;

		for (i = 0; i < 5; i++) {
			mp_set_weapon_slot(i, random() % numoptions + 1);
		}

		mp_set_weapon_slot(i, mp_get_num_weapon_options() - 1);
	}
}

void mp_set_weaponset_slotnum(s32 slotnum)
{
	g_MpWeaponSetNum = mp_slotnum_to_mpweaponset(slotnum);
	mp_apply_weaponset();
}

void mp_apply_weaponset_if_standard(void)
{
	if (g_MpWeaponSetNum < ARRAYCOUNT(g_MpWeaponSets)) {
		mp_apply_weaponset();
	}
}

s32 mp_get_weaponset_slotnum(void)
{
	return mp_mpweaponset_to_slotnum(g_MpWeaponSetNum);
}

bool mp_is_paused(void)
{
	if (PLAYERCOUNT() == 1
			&& g_Vars.mplayerisrunning
			&& g_Menus[g_Vars.currentplayerstats->mpindex].curdialog) {
		return true;
	}

	if (g_MpSetup.paused == PAUSEMODE_UNPAUSED) {
		return false;
	}

	return true;
}

void mp_set_paused(u8 mode)
{
	g_MpSetup.paused = mode;
}

/**
 * Render "Paused" in the middle of the viewport if paused,
 * or "Press START" if player has finished their death animation.
 *
 * @bug: When calling text_render_v1(), vi_get_width() is used twice but it should be
 * vi_get_view_width() and vi_get_view_height(). These arguments to text_render_v1() set
 * the crop box for the text, but the text doesn't extend past the box anyway
 * so it has no effect.
 */
Gfx *mp_render_modal_text(Gfx *gdl)
{
	s32 textwidth;
	s32 textheight;
	s32 x;
	s32 y;
	char text[50];
	s32 stack1;

#if VERSION >= VERSION_JPN_FINAL
	g_UiScaleX = g_ViRes == VIRES_HI ? 2 : 1;
#endif

	if (g_MpSetup.paused == MPPAUSEMODE_PAUSED) {
		s32 red = (s32) ((1.0f - g_20SecIntervalFrac) * 20.0f * 255.0f) % 255;
		s32 stack2;

		gdl = text_begin(gdl);

		strcpy(text, lang_get(L_MPWEAPONS_040)); // "Paused"

		x = vi_get_view_left() + vi_get_view_width() / 2;

#if VERSION >= VERSION_JPN_FINAL
		x = x / g_UiScaleX;
#endif

#if VERSION >= VERSION_NTSC_1_0
		if (g_Menus[g_Vars.currentplayerstats->mpindex].curdialog)
#else
		if (g_Menus[g_Vars.currentplayernum].curdialog)
#endif
		{
			y = vi_get_view_top() + 10;
		} else {
			y = vi_get_view_top() + vi_get_view_height() / 2;
		}

#if VERSION >= VERSION_PAL_BETA
		// Use smaller fonts
		text_measure(&textheight, &textwidth, text, g_CharsHandelGothicSm, g_FontHandelGothicSm, 0);
#else
		text_measure(&textheight, &textwidth, text, g_CharsHandelGothicMd, g_FontHandelGothicMd, 0);
#endif
		x -= textwidth / 2;

#if VERSION >= VERSION_PAL_BETA
		gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicSm, g_FontHandelGothicSm, (red << 24) | 0x00ff00ff, 0x000000ff, vi_get_width(), vi_get_width(), 0, 0);
#else
		gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicMd, g_FontHandelGothicMd, (red << 24) | 0x00ff00ff, 0x000000ff, vi_get_width(), vi_get_width(), 0, 0);
#endif

		gdl = text_end(gdl);
	} else if (!g_MainIsEndscreen
			&& g_MpSetup.paused == MPPAUSEMODE_UNPAUSED
			&& g_Vars.currentplayer->isdead
			&& g_Vars.currentplayer->redbloodfinished
			&& g_Vars.currentplayer->deathanimfinished
			&& !(g_Vars.coopplayernum >= 0 && ((g_Vars.bond->isdead && g_Vars.coop->isdead) || !g_Vars.currentplayer->coopcanrestart || g_InCutscene))
			&& !(g_Vars.antiplayernum >= 0 && ((g_Vars.currentplayer != g_Vars.anti || g_InCutscene)))
			&& g_NumReasonsToEndMpMatch == 0) {
		// Render "Press START" text
		gdl = text_begin(gdl);

		strcpy(text, lang_get(L_MPWEAPONS_039));

		x = vi_get_view_left() + vi_get_view_width() / 2;

#if VERSION >= VERSION_JPN_FINAL
		x = x / g_UiScaleX;
#endif

		y = vi_get_view_top() + vi_get_view_height() / 2;

		text_measure(&textheight, &textwidth, text, g_CharsHandelGothicSm, g_FontHandelGothicSm, 0);
		x -= textwidth / 2;

		gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicSm, g_FontHandelGothicSm, 0xff0000ff, 0x000000ff, vi_get_width(), vi_get_width(), 0, 0);

		if (g_Vars.currentplayer->deadtimer > 0) {
			// Render countdown timer
			s32 countdowny = vi_get_view_top() + vi_get_view_height() / 2 + textheight + 2;
			s32 countdownx = vi_get_view_left() + vi_get_view_width() / 2;

#if VERSION >= VERSION_JPN_FINAL
			countdownx = countdownx / g_UiScaleX;
#endif
			sprintf(text, "%d\n", (g_Vars.currentplayer->deadtimer + TICKS(60) - 1) / TICKS(60));

			text_measure(&textheight, &textwidth, text, g_CharsHandelGothicSm, g_FontHandelGothicSm, 0);
			x = countdownx - textwidth / 2;
			y = countdowny;

			gdl = text_render_vx(gdl, &x, &y, text, g_CharsHandelGothicSm, g_FontHandelGothicSm, 0xff0000ff, 0x000000ff, vi_get_width(), vi_get_width(), 0, 0);
		}

		gdl = text_end(gdl);

		g_Menus[g_Vars.currentplayerstats->mpindex].openinhibit = 10;
	}

#if VERSION >= VERSION_JPN_FINAL
	g_UiScaleX = 1;
#endif

	return gdl;
}

s32 mp_find_max_int(s32 numplayers, s32 val0, s32 val1, s32 val2, s32 val3)
{
	s32 bestvalue = val0;
	s32 bestplayer = 0;

	if (numplayers >= 2) {
		if (val1 > bestvalue || (val1 == bestvalue && (random() % 2))) {
			bestplayer = 1;
			bestvalue = val1;
		} else {
			bestvalue = val0;
			bestplayer = 0;
		}

		if (numplayers >= 3) {
			if (val2 > bestvalue || (val2 == bestvalue && (random() % 2))) {
				bestplayer = 2;
				bestvalue = val2;
			}

			if (numplayers >= 4) {
				if (val3 > bestvalue || (val3 == bestvalue && (random() % 2))) {
					bestplayer = 3;
				}
			}
		}
	}

	return bestplayer;
}

s32 mp_find_min_int(s32 numplayers, s32 val0, s32 val1, s32 val2, s32 val3)
{
	s32 bestvalue = val0;
	s32 bestplayer = 0;

	if (numplayers >= 2) {
		if (val1 < bestvalue || (val1 == bestvalue && (random() % 2))) {
			bestplayer = 1;
			bestvalue = val1;
		} else {
			bestvalue = val0;
			bestplayer = 0;
		}

		if (numplayers >= 3) {
			if (val2 < bestvalue || (val2 == bestvalue && (random() % 2))) {
				bestplayer = 2;
				bestvalue = val2;
			}

			if (numplayers >= 4) {
				if (val3 < bestvalue || (val3 == bestvalue && (random() % 2))) {
					bestplayer = 3;
				}
			}
		}
	}

	return bestplayer;
}

s32 mp_find_max_float(s32 numplayers, f32 val0, f32 val1, f32 val2, f32 val3)
{
	// @bug: bestvalue should be an f32. Any value saved into here will be
	// rounded down which may cause the function to return an incorrect result.
	s32 bestvalue;
	s32 bestplayer = 0;

	if (numplayers >= 2) {
		if (val1 > val0 || (val1 == val0 && (random() % 2))) {
			bestplayer = 1;
			bestvalue = val1;
		} else {
			bestvalue = val0;
			bestplayer = 0;
		}

		if (numplayers >= 3) {
			if (val2 > bestvalue || (val2 == bestvalue && (random() % 2))) {
				bestplayer = 2;
				bestvalue = val2;
			}

			if (numplayers >= 4) {
				if (val3 > bestvalue || (val3 == bestvalue && (random() % 2))) {
					bestplayer = 3;
				}
			}
		}
	}

	return bestplayer;
}

s32 mp_find_min_float(s32 numplayers, f32 val0, f32 val1, f32 val2, f32 val3)
{
	// @bug: bestvalue should be an f32. Any value saved into here will be
	// rounded down which may cause the function to return an incorrect result.
	s32 bestvalue;
	s32 bestplayer = 0;

	if (numplayers >= 2) {
		if (val0);

		if (val1 < val0 || (val1 == val0 && (random() % 2))) {
			bestplayer = 1;
			bestvalue = val1;
		} else {
			bestplayer = 0;
			bestvalue = val0;
		}

		if (numplayers >= 3) {
			if (val2 < bestvalue || (val2 == bestvalue && (random() % 2))) {
				bestplayer = 2;
				bestvalue = val2;
			}

			if (numplayers >= 4) {
				if (val3 < bestvalue || (val3 == bestvalue && (random() % 2))) {
					bestplayer = 3;
				}
			}
		}
	}

	return bestplayer;
}

struct mpweaponset g_MpWeaponSets[12] = {
	{ /*0x00*/
		L_MPWEAPONS_055, // Pistols
		{ WEAPON_FALCON2,           WEAPON_MAGSEC4,          WEAPON_PHOENIX, WEAPON_MAULER,      WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_PHOENIX, MPFEATURE_WEAPON_MAULER, 0,              0                   },
		{ WEAPON_FALCON2,           WEAPON_MAGSEC4,          WEAPON_FALCON2, WEAPON_DY357MAGNUM, WEAPON_MPSHIELD, WEAPON_DISABLED },
	},
	{ /*0x01*/
		L_MPWEAPONS_054, // Automatics
		{ WEAPON_FALCON2,             WEAPON_CMP150, WEAPON_LAPTOPGUN, WEAPON_AR34, WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_LAPTOPGUN, 0,             0,                0            },
		{ WEAPON_FALCON2,             WEAPON_CMP150, WEAPON_DRAGON,    WEAPON_AR34, WEAPON_MPSHIELD, WEAPON_DISABLED },
	},
	{ /*0x02*/
		L_MPWEAPONS_053, // Power
		{ WEAPON_MAGSEC4,           WEAPON_DY357MAGNUM,      WEAPON_SHOTGUN, WEAPON_RCP120, WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_SHOTGUN, MPFEATURE_WEAPON_RCP120, 0,              0              },
		{ WEAPON_MAGSEC4,           WEAPON_DY357MAGNUM,      WEAPON_DRAGON,  WEAPON_AR34,   WEAPON_MPSHIELD, WEAPON_DISABLED },
	},
	{ /*0x03*/
		L_MPWEAPONS_052, // FarSight
		{ WEAPON_PHOENIX,            WEAPON_CYCLONE,            WEAPON_CALLISTO, WEAPON_FARSIGHT, WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_FARSIGHT, MPFEATURE_WEAPON_CALLISTO, 0,               0                },
		{ WEAPON_DISABLED,           WEAPON_DISABLED,           WEAPON_DISABLED, WEAPON_DISABLED, WEAPON_DISABLED, WEAPON_DISABLED },
	},
	{ /*0x04*/
		L_MPWEAPONS_051, // Tranquilizer
		{ WEAPON_FALCON2,                WEAPON_CMP150,   WEAPON_DRAGON,   WEAPON_TRANQUILIZER, WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_TRANQUILIZER, 0,               0,               0                    },
		{ WEAPON_DISABLED,               WEAPON_DISABLED, WEAPON_DISABLED, WEAPON_DISABLED,     WEAPON_DISABLED, WEAPON_DISABLED },
	},
	{ /*0x05*/
		L_MPWEAPONS_050, // Heavy
		{ WEAPON_MAULER,                WEAPON_K7AVENGER,        WEAPON_REAPER,              WEAPON_SUPERDRAGON,     WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_SUPERDRAGON, MPFEATURE_WEAPON_MAULER, MPFEATURE_WEAPON_K7AVENGER, MPFEATURE_WEAPON_REAPER },
		{ WEAPON_DISABLED,              WEAPON_DISABLED,         WEAPON_DISABLED,            WEAPON_DISABLED,        WEAPON_DISABLED, WEAPON_DISABLED },
	},
	{ /*0x06*/
		L_MPWEAPONS_049, // Golden Magnum
		{ WEAPON_FALCON2_SILENCER,          WEAPON_GRENADE,           WEAPON_CMP150,   WEAPON_DY357LX,  WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_FALCON2SILENCED, MPFEATURE_WEAPON_DY357LX, 0,               0                },
		{ WEAPON_DISABLED,                  WEAPON_DISABLED,          WEAPON_DISABLED, WEAPON_DISABLED, WEAPON_DISABLED, WEAPON_DISABLED },
	},
	{ /*0x07*/
		L_MPWEAPONS_048, // Explosive
		{ WEAPON_DEVASTATOR,            WEAPON_DEVASTATOR,           WEAPON_SUPERDRAGON, WEAPON_SUPERDRAGON, WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_SUPERDRAGON, MPFEATURE_WEAPON_DEVASTATOR, 0,                  0                   },
		{ WEAPON_DISABLED,              WEAPON_DISABLED,             WEAPON_DISABLED,    WEAPON_DISABLED,    WEAPON_DISABLED, WEAPON_DISABLED },
	},
	{ /*0x08*/
		L_MPWEAPONS_047, // Grenade Launcher
		{ WEAPON_MAGSEC4,              WEAPON_CMP150,   WEAPON_AR34,     WEAPON_DEVASTATOR, WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_DEVASTATOR, 0,               0,               0                  },
		{ WEAPON_DISABLED,             WEAPON_DISABLED, WEAPON_DISABLED, WEAPON_DISABLED,   WEAPON_DISABLED, WEAPON_DISABLED },
	},
	{ /*0x09*/
		L_MPWEAPONS_046, // Rocket Launcher
		{ WEAPON_MAULER,           WEAPON_CYCLONE, WEAPON_DRAGON, WEAPON_ROCKETLAUNCHER, WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_MAULER, 0,              0,             0                      },
		{ WEAPON_FALCON2,          WEAPON_CYCLONE, WEAPON_DRAGON, WEAPON_ROCKETLAUNCHER, WEAPON_MPSHIELD, WEAPON_DISABLED },
	},
	{ /*0x0a*/
		L_MPWEAPONS_045, // Proximity Mine
		{ WEAPON_MAGSEC4,             WEAPON_LAPTOPGUN,           WEAPON_K7AVENGER,               WEAPON_PROXIMITYMINE, WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_LAPTOPGUN, MPFEATURE_WEAPON_K7AVENGER, MPFEATURE_WEAPON_PROXIMITYMINE, 0                     },
		{ WEAPON_DISABLED,            WEAPON_DISABLED,            WEAPON_DISABLED,                WEAPON_DISABLED,      WEAPON_DISABLED, WEAPON_DISABLED },
	},
	{ /*0x0b*/
		L_MPWEAPONS_044, // Close Combat
#if VERSION == VERSION_JPN_FINAL
		{ WEAPON_TIMEDMINE,          WEAPON_CROSSBOW,  WEAPON_TIMEDMINE, WEAPON_CROSSBOW,  WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_CROSSBOW, 0,                0,                0                 },
		{ WEAPON_TIMEDMINE,          WEAPON_TIMEDMINE, WEAPON_TIMEDMINE, WEAPON_TIMEDMINE, WEAPON_MPSHIELD, WEAPON_DISABLED },
#else
		{ WEAPON_COMBATKNIFE,        WEAPON_COMBATKNIFE, WEAPON_TIMEDMINE, WEAPON_CROSSBOW,  WEAPON_MPSHIELD, WEAPON_DISABLED },
		{ MPFEATURE_WEAPON_CROSSBOW, 0,                  0,                0                 },
		{ WEAPON_COMBATKNIFE,        WEAPON_COMBATKNIFE, WEAPON_TIMEDMINE, WEAPON_TIMEDMINE, WEAPON_MPSHIELD, WEAPON_DISABLED },
#endif
	},
};

s32 g_MpWeaponSetNum = 0;

u16 g_AwardNames[] = {
	L_MPMENU_000, // "Most Suicidal"
	L_MPMENU_001, // "Who Needs Ammo?"
	L_MPMENU_002, // "Least Shielded"
	L_MPMENU_003, // "Best Protected"
	L_MPMENU_004, // "Marksmanship"
	L_MPMENU_005, // "Most Professional"
	L_MPMENU_006, // "Most Deadly"
	L_MPMENU_007, // "Most Harmless"
	L_MPMENU_008, // "Most Cowardly"
	L_MPMENU_009, // "Most Frantic"
	L_MPMENU_010, // "Most Honorable"
	L_MPMENU_011, // "Most Dishonourable"
	L_MPMENU_012, // "Shortest Life"
	L_MPMENU_013, // "Longest Life"
	L_MPMENU_014, // "Double Kill"
	L_MPMENU_015, // "Triple Kill"
	L_MPMENU_016, // "Quad Kill"
};

void mp_calculate_player_title(struct mpplayerconfig *mpplayer)
{
#if VERSION >= VERSION_NTSC_1_0
	const u32 tiers[] = { 2, 4, 8, 16, 28, 60, 100, 150, 210, 300 };
#else
	const u32 tiers[] = { 2, 4, 8, 16, 28, 48, 78, 138, 198, 300 };
#endif
	s32 tallies[] = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0 };
	s32 sum;
	s32 max;
	s32 i;

#if VERSION >= VERSION_NTSC_1_0
#define MULT(val) (val * 3)
#else
#define MULT(val) (val)
#endif

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->kills >= tiers[i] * MULT(20)) {
			tallies[0]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->gameswon >= tiers[i] * MULT(1)) {
			tallies[1]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->accuracymedals >= tiers[i] * MULT(1)) {
			tallies[2]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->headshotmedals >= tiers[i] * MULT(1)) {
			tallies[3]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->killmastermedals >= tiers[i] * MULT(1)) {
			tallies[4]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->time >= tiers[i] * MULT(1200)) {
			tallies[5]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->distance >= tiers[i] * MULT(100)) {
			tallies[6]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->damagedealt >= tiers[i] * MULT(1)) {
			tallies[7]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->ammoused >= tiers[i] * MULT(500)) {
			tallies[8]++;
		} else {
			break;
		}
	}

	for (i = 0; i < ARRAYCOUNT(tiers); i++) {
		if (mpplayer->survivormedals >= tiers[i] * MULT(1)) {
			tallies[9]++;
		} else {
			break;
		}
	}

	sum = 0;

	for (i = 0; i < ARRAYCOUNT(tallies); i++) {
		sum = sum + tallies[i];
	}

	if (sum > 100) {
		sum = 100;
	}

	mpplayer->title = sum / 5;

	max = MPPLAYERTITLE_PERFECT;

	if (mpplayer->title > max) {
		mpplayer->title = max;
	}
}

#if VERSION >= VERSION_NTSC_1_0
const char var7f1b8ad0[] = "%s%sAccuracy Peak! real value: %f (*100)\n";
const char var7f1b8afc[] = "";
const char var7f1b8b00[] = "";
const char var7f1b8b04[] = "%splayer %d Accuracy :%f\n";
const char var7f1b8b20[] = "";
const char var7f1b8b24[] = "%splayer %d dist:%f -> %f = %d\n";
const char var7f1b8b44[] = "";
const char var7f1b8b48[] = "Player %d TitleCalc ============\n";
#endif

struct mphead g_MpBeauHeads[] = {
	// head, require feature
	{ HEAD_BEAU2, 0 },
	{ HEAD_BEAU3, 0 },
	{ HEAD_BEAU4, 0 },
	{ HEAD_BEAU5, 0 },
	{ HEAD_BEAU6, 0 },
};

struct mphead g_MpHeads[] = {
	// head, require feature
	{ /*0x00*/ HEAD_DARK_COMBAT,  0                          },
	{ /*0x01*/ HEAD_DARK_FROCK,   MPFEATURE_CHR_CI           },
	{ /*0x02*/ HEAD_DARKAQUA,     MPFEATURE_CHR_PELAGIC      },
	{ /*0x03*/ HEAD_DARK_SNOW,    MPFEATURE_CHR_DARKSNOW     },
	{ /*0x04*/ HEAD_ELVIS,        MPFEATURE_CHR_ELVIS        },
	{ /*0x05*/ HEAD_ELVIS_GOGS,   MPFEATURE_CHR_ELVIS        },
	{ /*0x06*/ HEAD_CARRINGTON,   0                          },
	{ /*0x07*/ HEAD_MRBLONDE,     MPFEATURE_CHR_MRBLONDE     },
	{ /*0x08*/ HEAD_CASSANDRA,    0                          },
	{ /*0x09*/ HEAD_TRENT,        MPFEATURE_CHR_TRENT        },
	{ /*0x0a*/ HEAD_JONATHAN,     MPFEATURE_CHR_INFILTRATION },
	{ /*0x0b*/ HEAD_VD,           0                          },
	{ /*0x0c*/ HEAD_PRESIDENT,    MPFEATURE_CHR_CI           },
	{ /*0x0d*/ HEAD_DDSHOCK,      0                          },
	{ /*0x0e*/ HEAD_BIOTECH,      MPFEATURE_CHR_BIOTECH      },
	{ /*0x0f*/ HEAD_DDSNIPER,     MPFEATURE_CHR_VILLACHRS    },
	{ /*0x10*/ HEAD_A51FACEPLATE, MPFEATURE_CHR_INFILTRATION },
	{ /*0x11*/ HEAD_SECRETARY,    0                          },
	{ /*0x12*/ HEAD_FEM_GUARD,    MPFEATURE_CHR_FEMGUARD     },
	{ /*0x13*/ HEAD_FEM_GUARD2,   MPFEATURE_CHR_FEMGUARD     },
	{ /*0x14*/ HEAD_MAIAN_S,      MPFEATURE_CHR_ELVIS        },
	{ /*0x15*/ HEAD_JON,          0                          },
	{ /*0x16*/ HEAD_BEAU1,        0                          },
	{ /*0x17*/ HEAD_ROSS,         0                          },
	{ /*0x18*/ HEAD_MARK2,        0                          },
	{ /*0x19*/ HEAD_CHRIST,       0                          },
	{ /*0x1a*/ HEAD_RUSS,         0                          },
	{ /*0x1b*/ HEAD_DARLING,      0                          },
	{ /*0x1c*/ HEAD_BRIAN,        0                          },
	{ /*0x1d*/ HEAD_JAMIE,        0                          },
	{ /*0x1e*/ HEAD_DUNCAN2,      0                          },
	{ /*0x1f*/ HEAD_KEITH,        0                          },
	{ /*0x20*/ HEAD_STEVEM,       0                          },
	{ /*0x21*/ HEAD_GRANT,        0                          },
	{ /*0x22*/ HEAD_PENNY,        0                          },
	{ /*0x23*/ HEAD_DAVEC,        0                          },
	{ /*0x24*/ HEAD_JONES,        0                          },
	{ /*0x25*/ HEAD_GRAHAM,       0                          },
	{ /*0x26*/ HEAD_ROBERT,       0                          },
	{ /*0x27*/ HEAD_NEIL2,        0                          },
	{ /*0x28*/ HEAD_SHAUN,        0                          },
	{ /*0x29*/ HEAD_ROBIN,        0                          },
	{ /*0x2a*/ HEAD_COOK,         0                          },
	{ /*0x2b*/ HEAD_PRYCE,        0                          },
	{ /*0x2c*/ HEAD_SILKE,        0                          },
	{ /*0x2d*/ HEAD_SMITH,        0                          },
	{ /*0x2e*/ HEAD_GARETH,       0                          },
	{ /*0x2f*/ HEAD_MURCHIE,      0                          },
	{ /*0x30*/ HEAD_WONG,         0                          },
	{ /*0x31*/ HEAD_CARTER,       0                          },
	{ /*0x32*/ HEAD_TINTIN,       0                          },
	{ /*0x33*/ HEAD_MUNTON,       0                          },
	{ /*0x34*/ HEAD_STAMPER,      0                          },
	{ /*0x35*/ HEAD_PHELPS,       0                          },
	{ /*0x36*/ HEAD_ALEX,         0                          },
	{ /*0x37*/ HEAD_JULIANNE,     0                          },
	{ /*0x38*/ HEAD_LAURA,        0                          },
	{ /*0x39*/ HEAD_EDMCG,        0                          },
	{ /*0x3a*/ HEAD_ANKA,         0                          },
	{ /*0x3b*/ HEAD_LESLIE_S,     0                          },
	{ /*0x3c*/ HEAD_MATT_C,       0                          },
	{ /*0x3d*/ HEAD_PEER_S,       0                          },
	{ /*0x3e*/ HEAD_EILEEN_T,     0                          },
	{ /*0x3f*/ HEAD_ANDY_R,       0                          },
	{ /*0x40*/ HEAD_BEN_R,        0                          },
	{ /*0x41*/ HEAD_STEVE_K,      0                          },
	{ /*0x42*/ HEAD_SANCHEZ,      0                          },
	{ /*0x43*/ HEAD_TIM,          0                          },
	{ /*0x44*/ HEAD_KEN,          0                          },
	{ /*0x45*/ HEAD_EILEEN_H,     0                          },
	{ /*0x46*/ HEAD_SCOTT_H,      0                          },
	{ /*0x47*/ HEAD_JOEL,         0                          },
	{ /*0x48*/ HEAD_GRIFFEY,      0                          },
#if VERSION != VERSION_JPN_FINAL
	{ /*0x49*/ HEAD_MOTO,         0                          },
#endif
	{ /*0x4a*/ HEAD_WINNER,       0                          },
};

u32 g_BotHeads[] = {
	MPHEAD_JON,
	MPHEAD_BEAU1,
	MPHEAD_ROSS,
	MPHEAD_MARK2,
	MPHEAD_CHRIST,
	MPHEAD_RUSS,
	MPHEAD_DARLING,
	MPHEAD_BRIAN,
	MPHEAD_JAMIE,
	MPHEAD_DUNCAN2,
	MPHEAD_KEITH,
	MPHEAD_STEVEM,
	MPHEAD_GRANT,
	MPHEAD_PENNY,
	MPHEAD_DAVEC,
	MPHEAD_JONES,
	MPHEAD_GRAHAM,
	MPHEAD_ROBERT,
	MPHEAD_NEIL2,
	MPHEAD_SHAUN,
	MPHEAD_ROBIN,
	MPHEAD_COOK,
	MPHEAD_PRYCE,
	MPHEAD_SILKE,
	MPHEAD_SMITH,
	MPHEAD_GARETH,
	MPHEAD_MURCHIE,
	MPHEAD_WONG,
	MPHEAD_CARTER,
	MPHEAD_TINTIN,
	MPHEAD_MUNTON,
	MPHEAD_STAMPER,
	MPHEAD_PHELPS,
	MPHEAD_ALEX,
	MPHEAD_JULIANNE,
	MPHEAD_LAURA,
	MPHEAD_EDMCG,
	MPHEAD_ANKA,
	MPHEAD_LESLIE_S,
	MPHEAD_MATT_C,
	MPHEAD_PEER_S,
	MPHEAD_EILEEN_T,
	MPHEAD_ANDY_R,
	MPHEAD_BEN_R,
	MPHEAD_STEVE_K,
	MPHEAD_SANCHEZ,
	MPHEAD_TIM,
	MPHEAD_KEN,
	MPHEAD_EILEEN_H,
	MPHEAD_SCOTT_H,
	MPHEAD_JOEL,
	MPHEAD_GRIFFEY,
#if VERSION != VERSION_JPN_FINAL
	MPHEAD_MOTO,
#endif
};

struct botprofile g_BotProfiles[] = {
	// type,           difficulty,      name,       body,                 require feature
	{ BOTTYPE_GENERAL, BOTDIFF_MEAT,    L_MISC_088, MPBODY_DD_GUARD,      0                         },
	{ BOTTYPE_GENERAL, BOTDIFF_EASY,    L_MISC_089, MPBODY_DD_SECGUARD,   0                         },
	{ BOTTYPE_GENERAL, BOTDIFF_NORMAL,  L_MISC_090, MPBODY_DD_SHOCK_INF,  0                         },
	{ BOTTYPE_GENERAL, BOTDIFF_HARD,    L_MISC_091, MPBODY_DDSHOCK,       MPFEATURE_BOTDIFF_HARD    },
	{ BOTTYPE_GENERAL, BOTDIFF_PERFECT, L_MISC_092, MPBODY_STRIPES,       MPFEATURE_BOTDIFF_PERFECT },
	{ BOTTYPE_GENERAL, BOTDIFF_DARK,    L_MISC_093, MPBODY_MOORE,         MPFEATURE_BOTDIFF_DARK    },
	{ BOTTYPE_PEACE,   BOTDIFF_NORMAL,  L_MISC_094, MPBODY_DD_LABTECH,    0                         },
	{ BOTTYPE_SHIELD,  BOTDIFF_NORMAL,  L_MISC_095, MPBODY_G5_SWAT_GUARD, 0                         },
	{ BOTTYPE_ROCKET,  BOTDIFF_NORMAL,  L_MISC_096, MPBODY_G5_GUARD,      0                         },
	{ BOTTYPE_KAZE,    BOTDIFF_NORMAL,  L_MISC_097, MPBODY_PRES_SECURITY, 0                         },
	{ BOTTYPE_FIST,    BOTDIFF_NORMAL,  L_MISC_098, MPBODY_PELAGIC_GUARD, 0                         },
	{ BOTTYPE_PREY,    BOTDIFF_NORMAL,  L_MISC_099, MPBODY_DDSHOCK,       0                         },
	{ BOTTYPE_COWARD,  BOTDIFF_NORMAL,  L_MISC_100, MPBODY_PRESIDENT,     0                         },
	{ BOTTYPE_JUDGE,   BOTDIFF_NORMAL,  L_MISC_101, MPBODY_STEWARD,       0                         },
	{ BOTTYPE_FEUD,    BOTDIFF_NORMAL,  L_MISC_102, MPBODY_NSA_LACKEY,    0                         },
	{ BOTTYPE_SPEED,   BOTDIFF_NORMAL,  L_MISC_103, MPBODY_MRBLONDE,      0                         },
	{ BOTTYPE_TURTLE,  BOTDIFF_NORMAL,  L_MISC_104, MPBODY_CARRINGTON,    0                         },
	{ BOTTYPE_VENGE,   BOTDIFF_NORMAL,  L_MISC_105, MPBODY_ALASKAN_GUARD, 0                         },
};

struct mpbody g_MpBodies[] = {
	// global body ID,                name,            head,             require feature
	/*0x00*/ { BODY_DARK_COMBAT,      L_OPTIONS_016,   HEAD_DARK_COMBAT, 0                          },
	/*0x01*/ { BODY_DARK_TRENCH,      L_OPTIONS_017,   HEAD_DARK_COMBAT, MPFEATURE_CHR_JOTRENCH     },
	/*0x02*/ { BODY_DARK_FROCK,       L_OPTIONS_018,   HEAD_DARK_FROCK,  MPFEATURE_CHR_CI           },
	/*0x03*/ { BODY_DARK_RIPPED,      L_OPTIONS_019,   HEAD_DARK_FROCK,  MPFEATURE_CHR_CI           },
	/*0x04*/ { BODY_DARK_AF1,         L_OPTIONS_020,   HEAD_DARK_COMBAT, MPFEATURE_CHR_AF1          },
	/*0x05*/ { BODY_DARK_LEATHER,     L_MPWEAPONS_156, HEAD_DARK_COMBAT, MPFEATURE_CHR_G5           },
	/*0x06*/ { BODY_DARK_NEGOTIATOR,  L_MPWEAPONS_157, HEAD_DARK_COMBAT, MPFEATURE_CHR_VILLACHRS    },
	/*0x07*/ { BODY_DARKWET,          L_OPTIONS_021,   HEAD_DARKAQUA,    MPFEATURE_CHR_PELAGIC      },
	/*0x08*/ { BODY_DARKAQUALUNG,     L_OPTIONS_022,   HEAD_DARKAQUA,    MPFEATURE_CHR_PELAGIC      },
	/*0x09*/ { BODY_DARKSNOW,         L_OPTIONS_023,   HEAD_DARK_SNOW,   MPFEATURE_CHR_DARKSNOW     },
	/*0x0a*/ { BODY_DARKLAB,          L_OPTIONS_024,   HEAD_DARK_COMBAT, MPFEATURE_CHR_INFILTRATION },
	/*0x0b*/ { BODY_THEKING,          L_OPTIONS_025,   HEAD_ELVIS,       MPFEATURE_CHR_ELVIS        },
	/*0x0c*/ { BODY_ELVIS1,           L_OPTIONS_026,   HEAD_ELVIS,       MPFEATURE_CHR_ELVIS        },
	/*0x0d*/ { BODY_ELVISWAISTCOAT,   L_MPWEAPONS_158, HEAD_ELVIS,       MPFEATURE_CHR_ELVIS        },
	/*0x0e*/ { BODY_CARRINGTON,       L_OPTIONS_027,   HEAD_CARRINGTON,  0                          },
	/*0x0f*/ { BODY_CARREVENINGSUIT,  L_OPTIONS_028,   HEAD_CARRINGTON,  MPFEATURE_CHR_CI           },
	/*0x10*/ { BODY_MRBLONDE,         L_OPTIONS_029,   HEAD_MRBLONDE,    MPFEATURE_CHR_MRBLONDE     },
	/*0x11*/ { BODY_CASSANDRA,        L_OPTIONS_030,   HEAD_CASSANDRA,   0                          },
	/*0x12*/ { BODY_TRENT,            L_OPTIONS_031,   HEAD_TRENT,       MPFEATURE_CHR_TRENT        },
	/*0x13*/ { BODY_JONATHAN,         L_OPTIONS_032,   HEAD_JONATHAN,    MPFEATURE_CHR_JONATHAN     },
	/*0x14*/ { BODY_CILABTECH,        L_OPTIONS_033,   1000,             0                          },
	/*0x15*/ { BODY_CIFEMTECH,        L_OPTIONS_034,   1000,             0                          },
	/*0x16*/ { BODY_CISOLDIER,        L_OPTIONS_035,   1000,             0                          },
	/*0x17*/ { BODY_DDSHOCK,          L_OPTIONS_036,   HEAD_DDSHOCK,     0                          },
	/*0x18*/ { BODY_FEM_GUARD,        L_OPTIONS_037,   1000,             MPFEATURE_CHR_FEMGUARD     },
	/*0x19*/ { BODY_DD_SECGUARD,      L_OPTIONS_038,   1000,             0                          },
	/*0x1a*/ { BODY_DD_GUARD,         L_OPTIONS_039,   1000,             0                          },
	/*0x1b*/ { BODY_DD_SHOCK_INF,     L_OPTIONS_040,   1000,             0                          },
	/*0x1c*/ { BODY_SECRETARY,        L_OPTIONS_041,   1000,             0                          },
	/*0x1d*/ { BODY_OFFICEWORKER,     L_OPTIONS_042,   1000,             MPFEATURE_CHR_OFFICEWORKER },
	/*0x1e*/ { BODY_OFFICEWORKER2,    L_OPTIONS_043,   1000,             MPFEATURE_CHR_OFFICEWORKER },
	/*0x1f*/ { BODY_NEGOTIATOR,       L_OPTIONS_044,   1000,             MPFEATURE_CHR_VILLACHRS    },
	/*0x20*/ { BODY_DDSNIPER,         L_OPTIONS_045,   HEAD_DDSNIPER,    MPFEATURE_CHR_VILLACHRS    },
	/*0x21*/ { BODY_G5_GUARD,         L_OPTIONS_046,   1000,             MPFEATURE_CHR_G5           },
	/*0x22*/ { BODY_G5_SWAT_GUARD,    L_OPTIONS_047,   1000,             MPFEATURE_CHR_G5           },
	/*0x23*/ { BODY_CIAGUY,           L_OPTIONS_048,   1000,             MPFEATURE_CHR_CIAFBI       },
	/*0x24*/ { BODY_FBIGUY,           L_OPTIONS_049,   1000,             MPFEATURE_CHR_CIAFBI       },
	/*0x25*/ { BODY_AREA51GUARD,      L_OPTIONS_050,   1000,             MPFEATURE_CHR_INFILTRATION },
	/*0x26*/ { BODY_A51TROOPER,       L_OPTIONS_051,   1000,             MPFEATURE_CHR_INFILTRATION },
	/*0x27*/ { BODY_A51AIRMAN,        L_OPTIONS_052,   1000,             MPFEATURE_CHR_INFILTRATION },
	/*0x28*/ { BODY_OVERALL,          L_OPTIONS_053,   1000,             MPFEATURE_CHR_INFILTRATION },
	/*0x29*/ { BODY_STRIPES,          L_OPTIONS_054,   1000,             MPFEATURE_CHR_STRIPES      },
	/*0x2a*/ { BODY_LABTECH,          L_OPTIONS_055,   1000,             MPFEATURE_CHR_LABTECH      },
	/*0x2b*/ { BODY_FEMLABTECH,       L_OPTIONS_056,   1000,             MPFEATURE_CHR_LABTECH      },
	/*0x2c*/ { BODY_DD_LABTECH,       L_OPTIONS_057,   1000,             MPFEATURE_CHR_LABTECH      },
	/*0x2d*/ { BODY_BIOTECH,          L_OPTIONS_058,   HEAD_BIOTECH,     MPFEATURE_CHR_BIOTECH      },
	/*0x2e*/ { BODY_ALASKAN_GUARD,    L_OPTIONS_059,   1000,             MPFEATURE_CHR_ALASKANGUARD },
	/*0x2f*/ { BODY_PILOTAF1,         L_OPTIONS_060,   1000,             MPFEATURE_CHR_AF1          },
	/*0x30*/ { BODY_STEWARD,          L_OPTIONS_061,   1000,             MPFEATURE_CHR_AF1          },
	/*0x31*/ { BODY_STEWARDESS,       L_OPTIONS_062,   1000,             MPFEATURE_CHR_AF1          },
	/*0x32*/ { BODY_STEWARDESS_COAT,  L_OPTIONS_063,   1000,             MPFEATURE_CHR_AF1          },
	/*0x33*/ { BODY_PRESIDENT,        L_OPTIONS_064,   HEAD_PRESIDENT,   MPFEATURE_CHR_CI           },
	/*0x34*/ { BODY_NSA_LACKEY,       L_OPTIONS_065,   1000,             MPFEATURE_CHR_NSALACKEY    },
	/*0x35*/ { BODY_PRES_SECURITY,    L_OPTIONS_066,   1000,             MPFEATURE_CHR_PRESSECURITY },
	/*0x36*/ { BODY_PRESIDENT_CLONE2, L_OPTIONS_067,   HEAD_PRESIDENT,   MPFEATURE_CHR_PRESCLONE    },
	/*0x37*/ { BODY_PELAGIC_GUARD,    L_OPTIONS_068,   1000,             MPFEATURE_CHR_PELAGIC      },
	/*0x38*/ { BODY_MAIAN_SOLDIER,    L_OPTIONS_069,   HEAD_MAIAN_S,     MPFEATURE_CHR_ELVIS        },
	/*0x39*/ { BODY_CONNERY,          L_OPTIONS_070,   1000,             MPFEATURE_8BOTS            },
	/*0x3a*/ { BODY_MOORE,            L_OPTIONS_070,   1000,             MPFEATURE_8BOTS            },
	/*0x3b*/ { BODY_DALTON,           L_OPTIONS_070,   1000,             MPFEATURE_8BOTS            },
	/*0x3c*/ { BODY_DJBOND,           L_OPTIONS_070,   1000,             MPFEATURE_8BOTS            },
};

u32 g_MpMaleHeads[] = {
	HEAD_JON,
	HEAD_BEAU1,
	HEAD_ROSS,
	HEAD_MARK2,
	HEAD_CHRIST,
	HEAD_RUSS,
	HEAD_DARLING,
	HEAD_BRIAN,
	HEAD_JAMIE,
	HEAD_DUNCAN2,
	HEAD_KEITH,
	HEAD_STEVEM,
	HEAD_GRANT,
	HEAD_PENNY,
	HEAD_DAVEC,
	HEAD_JONES,
	HEAD_GRAHAM,
	HEAD_NEIL2,
	HEAD_SHAUN,
	HEAD_ROBIN,
	HEAD_COOK,
	HEAD_PRYCE,
	HEAD_SILKE,
	HEAD_SMITH,
	HEAD_GARETH,
	HEAD_MURCHIE,
	HEAD_WONG,
	HEAD_CARTER,
	HEAD_TINTIN,
	HEAD_MUNTON,
	HEAD_STAMPER,
	HEAD_PHELPS,
	HEAD_EDMCG,
	HEAD_MATT_C,
	HEAD_PEER_S,
	HEAD_ANDY_R,
	HEAD_BEN_R,
	HEAD_STEVE_K,
	HEAD_SANCHEZ,
	HEAD_TIM,
	HEAD_KEN,
	HEAD_SCOTT_H,
	HEAD_JOEL,
#if VERSION != VERSION_JPN_FINAL
	HEAD_MOTO,
#endif
};

u32 g_MpFemaleHeads[] = {
	HEAD_ALEX,
	HEAD_JULIANNE,
	HEAD_LAURA,
	HEAD_ANKA,
	HEAD_LESLIE_S,
	HEAD_EILEEN_T,
	HEAD_EILEEN_H,
};

/**
 * Calculate player awards, medals, and update character statistics.
 */
void mp_calculate_awards(void)
{
	s32 playercount;
	s32 i;
	s32 j;
	s32 prevplayernum;
	s32 duration60;
	struct awardmetrics metrics[MAX_PLAYERS];

	// @bug: playerrankings should have 12 elements. Because it's too small,
	// overflow occurs in mp_get_player_rankings. The overflow writes into the
	// metrics array (above) which is yet to be initialised, so this bug has
	// no effect on IDO.
#ifdef AVOID_UB
	struct ranking playerrankings[MAX_MPCHRS];
#else
	struct ranking playerrankings[1];
#endif

	s32 numchrs;
#if VERSION >= VERSION_NTSC_1_0
	s32 numteams;
	struct ranking teamrankings[MAX_MPCHRS];
	u32 stack[4];
#else
	u32 stack[3];
#endif

	playercount = PLAYERCOUNT();

	duration60 = player_get_mission_time();

	sndp_stop_all();

	numchrs = mp_get_player_rankings(playerrankings);
#if VERSION >= VERSION_NTSC_1_0
	numteams = (g_MpSetup.options & MPOPTION_TEAMSENABLED) ? mp_get_team_rankings(teamrankings) : 0;
#endif

	prevplayernum = g_Vars.currentplayernum;

	// Iterate all human players and update their character stats.
	// At the same time, populate the metrics array
	// which is a temporary array for award calculation.
	for (i = 0; i < playercount; i++) {
		struct mpchrconfig *mpchr = mp_get_chr_config_by_slot_num(i);
		struct mpplayerconfig *mpplayer = (struct mpplayerconfig *)mpchr;
#if VERSION >= VERSION_NTSC_1_0
		s32 chrnum = mp_get_chr_index_by_slot_num(i);
#else
		s32 chrnum = i;
#endif
		s32 sum;

		set_current_player_num(i);

		g_Vars.currentplayer->award1 = NULL;
		g_Vars.currentplayer->award2 = NULL;

		metrics[i].numshots = mpstats_get_player_shotcount_by_region(SHOTREGION_TOTAL);
		metrics[i].numheadshots = mpstats_get_player_shotcount_by_region(SHOTREGION_HEAD);
		metrics[i].numkills = 0;
		metrics[i].numdeaths = 0;
		metrics[i].numsuicides = 0;

		for (j = 0; j < MAX_MPCHRS; j++) {
#if VERSION >= VERSION_NTSC_1_0
			if (chrnum == j)
#else
			if (i == j)
#endif
			{
				metrics[i].numsuicides += mpchr->killcounts[j];
			} else {
				metrics[i].numkills += mpchr->killcounts[j];
			}
		}

		for (j = 0; j < MAX_MPCHRS; j++) {
			struct mpchrconfig *othermpchr = MPCHR(j);
#if VERSION >= VERSION_NTSC_1_0
			metrics[i].numdeaths += othermpchr->killcounts[chrnum];
#else
			metrics[i].numdeaths += othermpchr->killcounts[i];
#endif
		}

		metrics[i].ksratio = metrics[i].numkills * 100.0f / (metrics[i].numshots + 1.0f);
		metrics[i].kdratio = metrics[i].numkills * 100.0f / (metrics[i].numdeaths + 1.0f);
		metrics[i].backshotcount = g_Vars.playerstats[i].backshotcount;
		metrics[i].drawplayercount = g_Vars.playerstats[i].drawplayercount;
		metrics[i].avgkmperhour = g_Vars.playerstats[i].distance / 100000.0f / ((duration60 + 1) / (3600.0f * 60.0f));
		metrics[i].armourcount = g_Vars.playerstats[i].armourcount;
		metrics[i].awards = 0;
		metrics[i].longestlife = g_Vars.playerstats[i].longestlife;
		metrics[i].shortestlife = g_Vars.playerstats[i].shortestlife;

		sum = mpstats_get_player_shotcount_by_region(SHOTREGION_HEAD)
			+ mpstats_get_player_shotcount_by_region(SHOTREGION_BODY)
			+ mpstats_get_player_shotcount_by_region(SHOTREGION_LIMB)
			+ mpstats_get_player_shotcount_by_region(SHOTREGION_GUN)
			+ mpstats_get_player_shotcount_by_region(SHOTREGION_HAT)
			+ mpstats_get_player_shotcount_by_region(SHOTREGION_OBJECT);

		if (metrics[i].numshots > 0) {
			metrics[i].accuracyfrac = sum / (f32)metrics[i].numshots;
		} else {
			metrics[i].accuracyfrac = 0.0f;
		}

		if (metrics[i].accuracyfrac > 1.0f) {
			metrics[i].accuracyfrac = 1.0f;
		}

		mpplayer->medals = 0;

		if (!g_CheatsActiveBank0 && !g_CheatsActiveBank1) {
			mpplayer->kills += metrics[i].numkills;
			mpplayer->deaths += metrics[i].numdeaths;
			mpplayer->gamesplayed++;
			mpplayer->time += duration60 / 60;
			mpplayer->distance += (u32)(g_Vars.playerstats[i].distance / 10000.0f);

#if VERSION >= VERSION_NTSC_1_0
			if (metrics[i].numshots > 0) {
				if (mpplayer->gamesplayed < 2) {
					mpplayer->accuracy = metrics[i].accuracyfrac * 1000.0f;
				} else {
					mpplayer->accuracy = ((metrics[i].accuracyfrac * 0.3f) + (mpplayer->accuracy / 1000.0f * 0.7f)) * 1000.0f;
				}
			}
#else
			mpplayer->accuracy = ((metrics[i].accuracyfrac * 0.3f) + (mpplayer->accuracy / 1000.0f * 0.7f)) * 1000.0f;
#endif

			mpplayer->damagedealt += (u32)(g_Vars.playerstats[i].damtransmitted / 0.1f);
			mpplayer->painreceived += (u32)(g_Vars.playerstats[i].damreceived / 0.1f);
			mpplayer->headshots += metrics[i].numheadshots;
			mpplayer->ammoused += metrics[i].numshots;

#if VERSION >= VERSION_NTSC_1_0
			if ((numchrs >= 2 && (g_MpSetup.options & MPOPTION_TEAMSENABLED) == 0) || numteams >= 2)
#else
			if (numchrs >= 2)
#endif
			{
				if (mpplayer->base.placement == 0) {
					bool lost = false;

					for (j = 0; j < MAX_MPCHRS; j++) {
#if VERSION >= VERSION_NTSC_1_0
						if (g_MpSetup.chrslots & (1 << j))
#endif
						{
							struct mpchrconfig *othermpchr = MPCHR(j);

							if (othermpchr->rankablescore == mpplayer->base.rankablescore
									&& othermpchr != &mpplayer->base
									&& !((g_MpSetup.options & MPOPTION_TEAMSENABLED) && othermpchr->team == mpplayer->base.team)) {
								lost = true;
							}
						}
					}

					if (!lost) {
						mpplayer->gameswon++;
					}
				}

#if VERSION >= VERSION_NTSC_1_0
				if (((g_MpSetup.options & MPOPTION_TEAMSENABLED) == 0 && numchrs == mpplayer->base.placement + 1)
						|| ((g_MpSetup.options & MPOPTION_TEAMSENABLED) && numteams == mpplayer->base.placement + 1))
#else
				if (mpplayer->base.placement == numchrs - 1)
#endif
				{
					bool won = false;

					for (j = 0; j < MAX_MPCHRS; j++) {
#if VERSION >= VERSION_NTSC_1_0
						if (g_MpSetup.chrslots & (1 << j))
#endif
						{
							struct mpchrconfig *othermpchr = MPCHR(j);

							if (othermpchr->rankablescore == mpplayer->base.rankablescore
									&& &mpplayer->base != othermpchr
									&& !((g_MpSetup.options & MPOPTION_TEAMSENABLED) && othermpchr->team == mpplayer->base.team)) {
								won = true;
							}
						}
					}

					if (!won) {
						mpplayer->gameslost++;
					}
				}
			}
#if VERSION >= VERSION_NTSC_1_0
			else {
				// empty
			}
#endif
		}
	}

	set_current_player_num(prevplayernum);

	// Choose which players are eligible for which awards
	i = mp_find_max_int(playercount, metrics[0].numsuicides, metrics[1].numsuicides, metrics[2].numsuicides, metrics[3].numsuicides);

	if (metrics[i].numsuicides > 0) {
		metrics[i].awards |= AWARD_MOSTSUICIDAL;
	}

	i = mp_find_min_int(playercount, metrics[0].numshots, metrics[1].numshots, metrics[2].numshots, metrics[3].numshots);

	if (metrics[i].numshots < 100) {
		metrics[i].awards |= AWARD_WHONEEDSAMMO;
	}

	i = mp_find_min_float(playercount, metrics[0].armourcount, metrics[1].armourcount, metrics[2].armourcount, metrics[3].armourcount);

	if (metrics[i].armourcount <= 2.0f) {
		metrics[i].awards |= AWARD_LEASTSHIELDED;
	}

	i = mp_find_max_float(playercount, metrics[0].armourcount, metrics[1].armourcount, metrics[2].armourcount, metrics[3].armourcount);

	if (metrics[i].armourcount > 6.0f) {
		metrics[i].awards |= AWARD_BESTPROTECTED;
	}

	i = mp_find_max_int(playercount, metrics[0].numheadshots, metrics[1].numheadshots, metrics[2].numheadshots, metrics[3].numheadshots);

	if (metrics[i].numheadshots > 0) {
		metrics[i].awards |= AWARD_MARKSMANSHIP;
	}

	i = mp_find_max_float(playercount, metrics[0].ksratio, metrics[1].ksratio, metrics[2].ksratio, metrics[3].ksratio);

	if (metrics[i].ksratio > 0.0f) {
		metrics[i].awards |= AWARD_MOSTPROFESSIONAL;
	}

	i = mp_find_max_float(playercount, metrics[0].kdratio, metrics[1].kdratio, metrics[2].kdratio, metrics[3].kdratio);

	if (metrics[i].kdratio > 0.0f) {
		metrics[i].awards |= AWARD_MOSTDEADLY;
	}

	i = mp_find_min_float(playercount, metrics[0].kdratio, metrics[1].kdratio, metrics[2].kdratio, metrics[3].kdratio);

	if (PLAYERCOUNT() >= 2) {
		metrics[i].awards |= AWARD_MOSTHARMLESS;
	}

	i = mp_find_min_int(playercount, metrics[0].drawplayercount, metrics[1].drawplayercount, metrics[2].drawplayercount, metrics[3].drawplayercount);

	if (PLAYERCOUNT() >= 2) {
		metrics[i].awards |= AWARD_MOSTCOWARDLY;
	}

	i = mp_find_max_float(playercount, metrics[0].avgkmperhour, metrics[1].avgkmperhour, metrics[2].avgkmperhour, metrics[3].avgkmperhour);

	if (metrics[i].avgkmperhour > 10.0f) {
		metrics[i].awards |= AWARD_MOSTFRANTIC;
	}

	i = mp_find_min_int(playercount, metrics[0].backshotcount, metrics[1].backshotcount, metrics[2].backshotcount, metrics[3].backshotcount);
	metrics[i].awards |= AWARD_MOSTHONORABLE;

	i = mp_find_max_int(playercount, metrics[0].backshotcount, metrics[1].backshotcount, metrics[2].backshotcount, metrics[3].backshotcount);

	if (metrics[i].backshotcount > 0 && (metrics[i].awards & AWARD_MOSTHONORABLE) == 0) {
		metrics[i].awards |= AWARD_MOSTDISHONORABLE;
	}

	i = mp_find_max_int(playercount, metrics[0].longestlife, metrics[1].longestlife, metrics[2].longestlife, metrics[3].longestlife);

	if (metrics[i].longestlife > 0) {
		metrics[i].awards |= AWARD_LONGESTLIFE;
	}

	i = mp_find_min_int(playercount, metrics[0].shortestlife, metrics[1].shortestlife, metrics[2].shortestlife, metrics[3].shortestlife);

	if (metrics[i].shortestlife > 0 && metrics[i].numdeaths > 0) {
		metrics[i].awards |= AWARD_SHORTESTLIFE;
	}

	for (i = 0; i < playercount; i++) {
		if (g_Vars.playerstats[i].maxsimulkills == 4) {
			metrics[i].awards |= AWARD_QUADKILL;
		} else if (g_Vars.playerstats[i].maxsimulkills == 3) {
			metrics[i].awards |= AWARD_TRIPLEKILL;
		} else if (g_Vars.playerstats[i].maxsimulkills == 2) {
			metrics[i].awards |= AWARD_DOUBLEKILL;
		}
	}

	// These loops would have printed each player's awards
	for (i = 0; i < playercount; i++) {
		for (j = 0; j < 17; j++) {
			if (metrics[i].awards & (1 << j));
		}
	}

	// For each player, choose which two awards to actually give them.
	// Note that the first award checked is quad kill, but after that the awards
	// are checked randomly. So if a player has quad kill they'll definitely see
	// it on the endscreen, but this is not the case for triple kill or any
	// other awards.
	for (i = 0; i < playercount; i++) {
		s32 numdone = 0;
		s32 awardindex = 16;

		if (playercount == 1) {
			// In a single player game, only allow the following awards
			metrics[i].awards &= 0
				| AWARD_MARKSMANSHIP
				| AWARD_MOSTPROFESSIONAL
				| AWARD_MOSTDEADLY
				| AWARD_MOSTFRANTIC
				| AWARD_MOSTHONORABLE
				| AWARD_DOUBLEKILL
				| AWARD_TRIPLEKILL
				| AWARD_QUADKILL;
		}

		while (numdone == 0) {
			if (metrics[i].awards & (1 << awardindex)) {
				metrics[i].awards &= ~(1 << awardindex);
				g_Vars.players[i]->award1 = lang_get(g_AwardNames[awardindex]);
				numdone = 1;
			}

			if (metrics[i].awards == 0) {
				numdone = 1;
			}

			awardindex = random() % 17;
		}

		while (numdone < 2) {
			awardindex = random() % 17;

			if (metrics[i].awards & (1 << awardindex)) {
				metrics[i].awards &= ~(1 << awardindex);
				g_Vars.players[i]->award2 = lang_get(g_AwardNames[awardindex]);
				numdone = 2;
			}

			if (metrics[i].awards == 0) {
				numdone = 2;
			}
		}
	}

	// Calculate KillMaster and Survivor medals
	if (numchrs >= 2) {
		s32 mostkillsvalue = 0;
		s32 mostkillsplayer = -1;
		s32 leastdeathsvalue = 0xffffff;
		s32 leastdeathsplayer = -1;
		s32 k;

		for (k = 0; k < MAX_MPCHRS; k++) {
			if (g_MpSetup.chrslots & (1 << k)) {
				s32 totalkills = 0;
				struct mpchrconfig *mpchr = MPCHR(k);

				for (j = 0; j < MAX_MPCHRS; j++) {
					// @bug: i should be k. The value of i was incremented after
					// the last iteration of its loop above so it'll be between
					// 1 and 4 inclusively depending on the number of players.
					// The bot nums start from 4 regardless of how many players
					// there are. So in a game with 4 humans, the kills on the
					// first bot will not be considered for the killmaster medal
					// which means the medal could go to a player who got fewer
					// total kills. Additionally, suicides are counted as kills
					// while the intention here was to omit them.
					if (i != j) {
						totalkills += mpchr->killcounts[j];
					}
				}

				if (totalkills == mostkillsvalue) {
					mostkillsplayer = -1;
				}

				if (totalkills > mostkillsvalue) {
					mostkillsplayer = k;
					mostkillsvalue = totalkills;
				}

				if (mpchr->numdeaths == leastdeathsvalue) {
					leastdeathsplayer = -1;
				}

				if (mpchr->numdeaths < leastdeathsvalue) {
					leastdeathsplayer = k;
					leastdeathsvalue = mpchr->numdeaths;
				}
			}
		}

		if (!g_CheatsActiveBank0 && !g_CheatsActiveBank1) {
			if (mostkillsplayer < 4 && mostkillsplayer >= 0) {
				struct mpplayerconfig *mpplayer = (struct mpplayerconfig *)MPCHR(mostkillsplayer);
				mpplayer->medals |= MEDAL_KILLMASTER;
				mpplayer->killmastermedals++;
			}

			if (leastdeathsplayer < 4 && leastdeathsplayer >= 0) {
				struct mpplayerconfig *mpplayer = (struct mpplayerconfig *)MPCHR(leastdeathsplayer);
				mpplayer->medals |= MEDAL_SURVIVOR;
				mpplayer->survivormedals++;
			}
		}
	}

	// Calculate Headshot and Accuracy medals
	if (!g_CheatsActiveBank0 && !g_CheatsActiveBank1 && playercount >= 2) {
		s32 mostheadshotvalue = 0;
		f32 mostaccuratevalue = 0.5f;
		s32 mostheadshotplayer = -1;
		s32 mostaccurateplayer = -1;

		for (i = 0; i < playercount; i++) {
			if (mostheadshotvalue == metrics[i].numheadshots) {
				mostheadshotplayer = -1;
			}

			if (metrics[i].numheadshots > mostheadshotvalue) {
				mostheadshotplayer = i;
				mostheadshotvalue = metrics[i].numheadshots;
			}

			if (metrics[i].accuracyfrac > mostaccuratevalue) {
				mostaccurateplayer = i;
				mostaccuratevalue = metrics[i].accuracyfrac;
			}
		}

		if (mostheadshotplayer >= 0) {
			struct mpplayerconfig *mpplayer = (struct mpplayerconfig *)mp_get_chr_config_by_slot_num(mostheadshotplayer);
			mpplayer->medals |= MEDAL_HEADSHOT;
			mpplayer->headshotmedals++;
		}

		if (mostaccurateplayer >= 0) {
			struct mpplayerconfig *mpplayer = (struct mpplayerconfig *)mp_get_chr_config_by_slot_num(mostaccurateplayer);
			mpplayer->medals |= MEDAL_ACCURACY;
			mpplayer->accuracymedals++;
		}
	}

	// Recalculate title for all players
	for (i = 0; i < playercount; i++) {
		struct mpplayerconfig *mpchr = (struct mpplayerconfig *)mp_get_chr_config_by_slot_num(i);
		mp_calculate_player_title(mpchr);
	}
}

void mp_end_match(void)
{
	s32 stack;
	s32 playercount = PLAYERCOUNT();
	s32 prevplayernum;
	s32 i;

	music_start_menu();
	mp_set_paused(MPPAUSEMODE_GAMEOVER);

	prevplayernum = g_Vars.currentplayernum;

	for (i = 0; i < playercount; i++) {
		set_current_player_num(i);

		g_Vars.currentplayer->award1 = NULL;
		g_Vars.currentplayer->award2 = NULL;

		if (g_Vars.currentplayer->visionmode == VISIONMODE_SLAYERROCKET) {
			g_Vars.currentplayer->visionmode = VISIONMODE_NORMAL;
		}
	}

	set_current_player_num(prevplayernum);
	mp_calculate_awards();

	if (g_BossFile.locktype == MPLOCKTYPE_CHALLENGE) {
		challenge_consider_marking_complete();
	}

	menu_save_and_push_root_dialog(NULL, MENUROOT_END_MP_MATCH);
}

s32 mp_get_num_heads2(void)
{
	return ARRAYCOUNT(g_MpHeads);
}

s32 mp_get_num_heads(void)
{
	return ARRAYCOUNT(g_MpHeads);
}

s32 mp_get_head_id(u8 headnum)
{
	return g_MpHeads[headnum].headnum;
}

s32 mp_get_head_required_feature(u8 headnum)
{
	return g_MpHeads[headnum].requirefeature;
}

s32 mp_get_beau_head_id(u8 headnum)
{
	return g_MpBeauHeads[headnum].headnum;
}

s32 mp_get_num_beau_heads(void)
{
	return ARRAYCOUNT(g_MpBeauHeads);
}

u32 mp_get_num_bodies(void)
{
	return ARRAYCOUNT(g_MpBodies);
}

s32 mp_get_body_id(u8 bodynum)
{
	/**
	 * @bug: bodynum 61 (0x3d) would cause an array overflow.
	 * ARRAYCOUNT(g_MpBodies) is 61.
	 */
	if (bodynum > ARRAYCOUNT(g_MpBodies)) {
		if (bodynum == ARRAYCOUNT(g_MpBodies) + 1) {
			return BODY_DRCAROLL;
		}

		return BODY_DARK_COMBAT;
	}

	return g_MpBodies[bodynum].bodynum;
}

s32 mp_get_mpbodynum_by_bodynum(u16 bodynum)
{
	s32 i;

	if (bodynum == BODY_DRCAROLL) {
		return ARRAYCOUNT(g_MpBodies) + 1;
	}

	for (i = 0; i < ARRAYCOUNT(g_MpBodies); i++) {
		if (g_MpBodies[i].bodynum == bodynum) {
			return i;
		}
	}

	return g_MpBodies[0].bodynum;
}

char *mp_get_body_name(u8 mpbodynum)
{
	// @bug: This should be >=
	if (mpbodynum > ARRAYCOUNT(g_MpBodies)) {
		mpbodynum = 0;
	}

	return lang_get(g_MpBodies[mpbodynum].name);
}

u8 mp_get_body_required_feature(u8 mpbodynum)
{
	// @bug: This should be >=
	if (mpbodynum > ARRAYCOUNT(g_MpBodies)) {
		mpbodynum = 0;
	}

	return g_MpBodies[mpbodynum].requirefeature;
}

s32 mp_get_mpheadnum_by_mpbodynum(s32 mpbodynum)
{
	s32 headnum;
	s32 index = 0;
	s32 i;

	if (mpbodynum >= HEAD_VD) {
		mpbodynum = 0;
	}

	headnum = g_MpBodies[mpbodynum].headnum;

	if (headnum == 1000) {
		if (g_HeadsAndBodies[g_MpBodies[mpbodynum].bodynum].ismale) {
			headnum = g_MpMaleHeads[random() % ARRAYCOUNT(g_MpMaleHeads)];
		} else {
			headnum = g_MpFemaleHeads[random() % ARRAYCOUNT(g_MpFemaleHeads)];
		}
	}

	for (i = 0; i != ARRAYCOUNT(g_MpHeads); i++) {
		if (g_MpHeads[i].headnum == headnum) {
			index = i;
		}
	}

	return index;
}

void mp_find_unused_head_and_body(u8 *mpheadnum, u8 *mpbodynum)
{
	struct mpchrconfig *mpchr;
	bool available;
	u8 trympheadnum;
	u8 trympbodynum;
	s32 i;

	do {
		available = true;
		trympheadnum = random() % ARRAYCOUNT(g_MpHeads);
		trympbodynum = random() % ARRAYCOUNT(g_MpBodies);

		for (i = 0; i < MAX_MPCHRS; i++) {
			if (g_MpSetup.chrslots & (1 << i)) {
				mpchr = MPCHR(i);

				if (mpchr->mpheadnum == trympheadnum) {
					available = false;
				}

				if (mpchr->mpbodynum == trympbodynum) {
					available = false;
				}
			}
		}
	} while (!available);

	if (1);

	*mpheadnum = trympheadnum;
	*mpbodynum = trympbodynum;
}

s32 mp_choose_random_lock_player(void)
{
	s32 start = random() % 4;
	s32 i;

	for (i = (start + 1) % 4;; i = (i + 1) % 4) {
		if ((g_MpSetup.chrslots & (1 << i)) || i == start) {
			break;
		}
	}

	return i;
}

bool mp_set_lock(s32 locktype, s32 playernum)
{
	g_BossFile.locktype = locktype;

	if (g_BossFile.locktype == MPLOCKTYPE_RANDOM) {
		g_MpLockInfo.lockedplayernum = mp_choose_random_lock_player();
	} else {
		g_MpLockInfo.lockedplayernum = playernum;
	}

	return true;
}

s32 mp_get_lock_type(void)
{
	return g_BossFile.locktype;
}

u32 mp_get_lock_player_num(void)
{
	return g_MpLockInfo.lockedplayernum;
}

bool mp_is_player_locked_out(s32 playernum)
{
	if (g_BossFile.locktype == MPLOCKTYPE_NONE) {
		return false;
	}

	if (g_MpLockInfo.lockedplayernum == playernum) {
		return false;
	}

	return true;
}

void mp_calculate_lock_if_last_winner_or_loser(void)
{
	if (g_BossFile.locktype == MPLOCKTYPE_LASTWINNER && g_MpLockInfo.lastwinner >= 0) {
		g_MpLockInfo.lockedplayernum = g_MpLockInfo.lastwinner;
	}

	if (g_BossFile.locktype == MPLOCKTYPE_LASTLOSER && g_MpLockInfo.lastloser >= 0) {
		g_MpLockInfo.lockedplayernum = g_MpLockInfo.lastloser;
	}

	if (g_MpLockInfo.lockedplayernum >= 0
			&& g_BossFile.locktype != MPLOCKTYPE_CHALLENGE
			&& (g_MpSetup.chrslots & (1 << g_MpLockInfo.lockedplayernum)) == 0) {
		g_MpLockInfo.lastwinner = g_MpLockInfo.lastloser = -1;
		g_MpLockInfo.lockedplayernum = mp_choose_random_lock_player();
	}
}

struct mptrack g_MpTracks[] = {
	// Audio ID, duration, name, unlock after stage
	/*0x00*/ { MUSIC_DARK_COMBAT,     160, L_MISC_124, -1 }, // "Dark Combat"
	/*0x01*/ { MUSIC_SKEDAR_MYSTERY,  170, L_MISC_125, -1 }, // "Skedar Mystery"
	/*0x02*/ { MUSIC_CI_OPERATIVE,    170, L_MISC_126, -1 }, // "CI Operative"
	/*0x03*/ { MUSIC_DATADYNE_ACTION, 180, L_MISC_127, -1 }, // "dataDyne Action"
	/*0x04*/ { MUSIC_MAIAN_TEARS,     200, L_MISC_128, -1 }, // "Maian Tears"
	/*0x05*/ { MUSIC_ALIEN_CONFLICT,  197, L_MISC_129, -1 }, // "Alien Conflict"
	/*0x06*/ { MUSIC_CI,              120, L_MISC_130, -1 }, // "Carrington Institute"
	/*0x07*/ { MUSIC_DEFECTION,       120, L_MISC_131, SOLOSTAGEINDEX_DEFECTION }, // "dD Central"
	/*0x08*/ { MUSIC_DEFECTION_X,     120, L_MISC_132, SOLOSTAGEINDEX_DEFECTION }, // "dD Central X"
	/*0x09*/ { MUSIC_INVESTIGATION,   120, L_MISC_133, SOLOSTAGEINDEX_INVESTIGATION }, // "dD Research"
	/*0x0a*/ { MUSIC_INVESTIGATION_X, 120, L_MISC_134, SOLOSTAGEINDEX_INVESTIGATION }, // "dD Research X"
	/*0x0b*/ { MUSIC_EXTRACTION,      120, L_MISC_135, SOLOSTAGEINDEX_EXTRACTION }, // "dD Extraction"
	/*0x0c*/ { MUSIC_EXTRACTION_X,    120, L_MISC_136, SOLOSTAGEINDEX_EXTRACTION }, // "dD Extraction X"
	/*0x0d*/ { MUSIC_VILLA,           120, L_MISC_137, SOLOSTAGEINDEX_VILLA }, // "Carrington Villa"
	/*0x0e*/ { MUSIC_VILLA_X,         120, L_MISC_138, SOLOSTAGEINDEX_VILLA }, // "Carrington Villa X"
	/*0x0f*/ { MUSIC_CHICAGO,         120, L_MISC_139, SOLOSTAGEINDEX_CHICAGO }, // "Chicago"
	/*0x10*/ { MUSIC_CHICAGO_X,       120, L_MISC_140, SOLOSTAGEINDEX_CHICAGO }, // "Chicago X"
	/*0x11*/ { MUSIC_G5,              120, L_MISC_141, SOLOSTAGEINDEX_G5BUILDING }, // "G5 Building"
	/*0x12*/ { MUSIC_G5_X,            120, L_MISC_142, SOLOSTAGEINDEX_G5BUILDING }, // "G5 Building X"
	/*0x13*/ { MUSIC_INFILTRATION,    120, L_MISC_143, SOLOSTAGEINDEX_INFILTRATION }, // "A51 Infiltration"
	/*0x14*/ { MUSIC_INFILTRATION_X,  120, L_MISC_144, SOLOSTAGEINDEX_INFILTRATION }, // "A51 Infiltration X"
	/*0x15*/ { MUSIC_RESCUE,          120, L_MISC_145, SOLOSTAGEINDEX_RESCUE }, // "A51 Rescue"
	/*0x16*/ { MUSIC_RESCUE_X,        120, L_MISC_146, SOLOSTAGEINDEX_RESCUE }, // "A51 Rescue X"
	/*0x17*/ { MUSIC_ESCAPE,          120, L_MISC_147, SOLOSTAGEINDEX_ESCAPE }, // "A51 Escape"
	/*0x18*/ { MUSIC_ESCAPE_X,        120, L_MISC_148, SOLOSTAGEINDEX_ESCAPE }, // "A51 Escape X"
	/*0x19*/ { MUSIC_AIRBASE,         120, L_MISC_149, SOLOSTAGEINDEX_AIRBASE }, // "Air Base"
	/*0x1a*/ { MUSIC_AIRBASE_X,       120, L_MISC_150, SOLOSTAGEINDEX_AIRBASE }, // "Air Base X"
	/*0x1b*/ { MUSIC_AIRFORCEONE,     120, L_MISC_151, SOLOSTAGEINDEX_AIRFORCEONE }, // "Air Force One"
	/*0x1c*/ { MUSIC_AIRFORCEONE_X,   120, L_MISC_152, SOLOSTAGEINDEX_AIRFORCEONE }, // "Air Force One X"
	/*0x1d*/ { MUSIC_CRASHSITE,       120, L_MISC_153, SOLOSTAGEINDEX_CRASHSITE }, // "Crash Site"
	/*0x1e*/ { MUSIC_CRASHSITE_X,     120, L_MISC_154, SOLOSTAGEINDEX_CRASHSITE }, // "Crash Site X"
	/*0x1f*/ { MUSIC_PELAGIC,         120, L_MISC_155, SOLOSTAGEINDEX_PELAGIC }, // "Pelagic II"
	/*0x20*/ { MUSIC_PELAGIC_X,       120, L_MISC_156, SOLOSTAGEINDEX_PELAGIC }, // "Pelagic II X"
	/*0x21*/ { MUSIC_DEEPSEA,         120, L_MISC_157, SOLOSTAGEINDEX_DEEPSEA }, // "Deep Sea"
	/*0x22*/ { MUSIC_DEEPSEA_X,       120, L_MISC_158, SOLOSTAGEINDEX_DEEPSEA }, // "Deep Sea X"
	/*0x23*/ { MUSIC_DEFENSE,         120, L_MISC_159, SOLOSTAGEINDEX_DEFENSE }, // "Institute Defense"
	/*0x24*/ { MUSIC_DEFENSE_X,       120, L_MISC_160, SOLOSTAGEINDEX_DEFENSE }, // "Institute Defense X"
	/*0x25*/ { MUSIC_ATTACKSHIP,      120, L_MISC_161, SOLOSTAGEINDEX_ATTACKSHIP }, // "Attack Ship"
	/*0x26*/ { MUSIC_ATTACKSHIP_X,    120, L_MISC_162, SOLOSTAGEINDEX_ATTACKSHIP }, // "Attack Ship X"
	/*0x27*/ { MUSIC_SKEDARRUINS,     120, L_MISC_163, SOLOSTAGEINDEX_SKEDARRUINS }, // "Skedar Ruins"
	/*0x28*/ { MUSIC_SKEDARRUINS_X,   120, L_MISC_164, SOLOSTAGEINDEX_SKEDARRUINS }, // "Skedar Ruins X"
	/*0x29*/ { MUSIC_CREDITS,         120, L_MISC_165, SOLOSTAGEINDEX_SKEDARRUINS }, // "End Credits"
};

bool mp_is_track_unlocked(s32 tracknum)
{
	s16 stageindex = g_MpTracks[tracknum].unlockstage;
	bool unlocked = false;
	u32 i;

	if (stageindex < 0 || stageindex > SOLOSTAGEINDEX_SKEDARRUINS) {
		unlocked = true;
	} else {
		for (i = 0; i != 3; i++) {
			if (g_GameFile.besttimes[stageindex][i]) {
				unlocked = true;
				break;
			}
		}
	}

	return unlocked;
}

s32 mp_get_track_slot_index(s32 tracknum)
{
	s32 i;
	s32 slotindex = 0;

	for (i = 0; i < tracknum; i++) {
		if (mp_is_track_unlocked(i)) {
			slotindex++;
		}
	}

	return slotindex;
}

s32 mp_get_track_num_at_slot_index(s32 slotindex)
{
	s32 i;
	s32 numunlocked = 0;

	for (i = 0; i != ARRAYCOUNT(g_MpTracks); i++) {
		if (mp_is_track_unlocked(i)) {
			if (numunlocked == slotindex) {
				break;
			}

			numunlocked++;
		}
	}

	return i;
}

s32 mp_get_num_unlocked_tracks(void)
{
	return mp_get_track_slot_index(ARRAYCOUNT(g_MpTracks));
}

s32 mp_get_track_music_num(s32 slotindex)
{
	s32 tracknum = mp_get_track_num_at_slot_index(slotindex);

	return g_MpTracks[tracknum].musicnum;
}

char *mp_get_track_name(s32 slotindex)
{
	s32 tracknum = mp_get_track_num_at_slot_index(slotindex);

	return lang_get(g_MpTracks[tracknum].name);
}

void mp_set_using_multiple_tunes(bool enable)
{
	g_BossFile.usingmultipletunes = enable;
}

bool mp_get_using_multiple_tunes(void)
{
	return g_BossFile.usingmultipletunes;
}

bool mp_is_multi_track_slot_enabled(s32 slot)
{
	s32 tracknum = mp_get_track_num_at_slot_index(slot);
	u8 index = tracknum >> 3;
	u8 value = 1 << (tracknum & 7);

	if ((g_BossFile.multipletracknums[index] & value) == 0) {
		return false;
	}

	return true;
}

void mp_set_multi_track_slot_enabled(s32 slot, bool enable)
{
	s32 tracknum = mp_get_track_num_at_slot_index(slot);
	u8 value = 1 << (tracknum & 7);
	u8 index = tracknum >> 3;

	if (enable) {
		g_BossFile.multipletracknums[index] |= value;
	} else {
		g_BossFile.multipletracknums[index] &= ~value;
	}
}

void mp_set_track_slot_enabled(s32 slot)
{
	if (mp_get_using_multiple_tunes()) {
		mp_set_multi_track_slot_enabled(slot, 1 - mp_is_multi_track_slot_enabled(slot));
	} else {
		g_BossFile.tracknum = mp_get_track_num_at_slot_index(slot);
	}
}

void mp_enable_all_multi_tracks(void)
{
	s32 i;

	for (i = 0; i != ARRAYCOUNT(g_BossFile.multipletracknums); i++) {
		g_BossFile.multipletracknums[i] = 0xff;
	}
}

void mp_disable_all_multi_tracks(void)
{
	s32 i;

	for (i = 0; i != ARRAYCOUNT(g_BossFile.multipletracknums); i++) {
		g_BossFile.multipletracknums[i] = 0;
	}
}

void mp_randomise_multi_tracks(void)
{
	s32 i;

	for (i = 0; i != ARRAYCOUNT(g_BossFile.multipletracknums); i++) {
		g_BossFile.multipletracknums[i] = random();
	}
}

void mp_set_track_to_random(void)
{
	g_BossFile.tracknum = -1;
}

s32 mp_get_current_track_slot_num(void)
{
	if (g_BossFile.tracknum < 0) {
		return g_BossFile.tracknum;
	}

	return mp_get_track_slot_index(g_BossFile.tracknum);
}

s32 mp_choose_track(void)
{
	s32 i;
	s32 tracknum;

	if (mp_get_using_multiple_tunes()) {
		s32 numunlocked = mp_get_num_unlocked_tracks();
		s32 numselected = 0;

		for (i = 0; i < numunlocked; i++) {
			if (mp_is_multi_track_slot_enabled(i)) {
				numselected++;
			}
		}

		if (numselected == 0) {
			do {
				tracknum = mp_get_track_num_at_slot_index(random() % numunlocked);
			} while (tracknum == g_MpLockInfo.unk04);

			g_MpLockInfo.unk04 = tracknum;
			g_MusicLife60 = g_MpTracks[tracknum].duration * TICKS(60);

			return g_MpTracks[tracknum].musicnum;
		}

		do {
			s32 selectionindex = random() % numselected;
			s32 selectioncount = 0;
			tracknum = -1;

			for (i = 0; i < numunlocked; i++) {
				if (mp_is_multi_track_slot_enabled(i)) {
					if (selectionindex == selectioncount) {
						tracknum = i;
					}

					selectioncount++;
				}
			}

			if (tracknum == -1) {
				g_MusicLife60 = g_MpTracks[0].duration * TICKS(60);
				return g_MpTracks[0].musicnum;
			}

			tracknum = mp_get_track_num_at_slot_index(tracknum);
		} while (numselected > 1 && tracknum == g_MpLockInfo.unk04);

		g_MpLockInfo.unk04 = tracknum;
		g_MusicLife60 = g_MpTracks[tracknum].duration * TICKS(60);

		return g_MpTracks[tracknum].musicnum;
	}

	tracknum = mp_get_current_track_slot_num();

	if (tracknum < 0) {
		s32 numunlocked = mp_get_num_unlocked_tracks();

		do {
			tracknum = mp_get_track_num_at_slot_index(random() % numunlocked);
		} while (tracknum == g_MpLockInfo.unk04);

		g_MpLockInfo.unk04 = tracknum;
		g_MusicLife60 = g_MpTracks[tracknum].duration * TICKS(60);
		return g_MpTracks[tracknum].musicnum;
	}

	tracknum = mp_get_track_num_at_slot_index(tracknum);
	g_MpLockInfo.unk04 = tracknum;
	g_MusicLife60 = g_MpTracks[tracknum].duration * TICKS(60);

	return g_MpTracks[tracknum].musicnum;
}

struct mpchrconfig *mp_get_chr_config_by_slot_num(s32 slot)
{
	s32 count = 0;
	struct mpchrconfig *result = NULL;
	s32 i;

	for (i = 0; i < MAX_MPCHRS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			if (count == slot) {
				result = MPCHR(i);
				break;
			}

			count++;
		}
	}

	return result;
}

#if VERSION >= VERSION_NTSC_1_0
s32 mp_get_chr_index_by_slot_num(s32 slot)
{
	s32 count = 0;
	s32 result = 0;
	s32 i;

	for (i = 0; i < MAX_MPCHRS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			if (count == slot) {
				result = i;
				break;
			}

			count++;
		}
	}

	return result;
}
#endif

s32 mp_get_num_chrs(void)
{
	s32 count = 0;
	s32 i;

	for (i = 0; i != MAX_MPCHRS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			count++;
		}
	}

	return count;
}

u8 mp_find_unused_team_num(void)
{
	u8 teamnum = 0;
	bool available = false;
	s32 i;

	while (teamnum < 7 && !available) {
		available = true;

		for (i = 0; i < MAX_MPCHRS; i++) {
			if (g_MpSetup.chrslots & (1 << i)) {
				struct mpchrconfig *mpchr = MPCHR(i);

				if (mpchr->team == teamnum) {
					available = false;
				}
			}
		}

		if (!available) {
			teamnum++;
		}
	}

	if (teamnum > 7) {
		teamnum = 7;
	}

	return teamnum;
}

void mp_create_bot_from_profile(s32 botnum, u8 profilenum)
{
	s32 headnum = 0;
	u8 team = mp_find_unused_team_num();
	bool available = false;
	s32 i;

	g_BotConfigsArray[botnum].type = g_BotProfiles[profilenum].type;
	g_BotConfigsArray[botnum].difficulty = g_BotProfiles[profilenum].difficulty;

	for (i = 0; i < MAX_PLAYERS; i++) {
		g_MpSimulantDifficultiesPerNumPlayers[botnum][i] = g_BotConfigsArray[botnum].difficulty;
	}

	g_MpSetup.chrslots |= 1 << (botnum + 4);
	strcpy(g_BotConfigsArray[botnum].base.name, "Sim\n");
	g_BotConfigsArray[botnum].base.team = team;

	while (!available) {
		headnum = g_BotHeads[random() % ARRAYCOUNT(g_BotHeads)];
		available = true;

		for (i = 0; i < MAX_MPCHRS; i++) {
			if (g_MpSetup.chrslots & (1 << i)) {
				struct mpchrconfig *mpchr = MPCHR(i);

				if (mpchr->mpheadnum == headnum) {
					available = false;
				}
			}
		}
	}

	g_BotConfigsArray[botnum].base.mpheadnum = headnum;
	g_BotConfigsArray[botnum].base.mpbodynum = g_BotProfiles[profilenum].body;
}

void mp_set_bot_difficulty(s32 botnum, s32 difficulty)
{
	s32 i;

	g_BotConfigsArray[botnum].difficulty = difficulty;

	for (i = 0; i < MAX_PLAYERS; i++) {
		g_MpSimulantDifficultiesPerNumPlayers[botnum][i] = g_BotConfigsArray[botnum].difficulty;
	}
}

/**
 * Return the first unused bot slot (0-7), or return 7 if all slots are full.
 *
 * This is used for the quick team feature.
 */
s32 mp_get_slot_for_new_bot(void)
{
	s32 i = 0;

	while (i < MAX_BOTS - 1 && g_MpSetup.chrslots & (1 << (i + 4))) {
		i++;
	}

	return i;
}

void mp_remove_simulant(s32 index)
{
	g_MpSetup.chrslots &= ~(1 << (index + 4));
	g_BotConfigsArray[index].base.name[0] = '\0';
	mp_init_botconfig(index);
	mp_generate_bot_names();
}

bool mp_has_simulants(void)
{
	if ((g_MpSetup.chrslots & 0xfff0) != 0) {
		return true;
	}

	return false;
}

bool mp_has_unused_bot_slots(void)
{
	s32 numvacant = challenge_is_feature_unlocked(MPFEATURE_8BOTS) ? MAX_BOTS : 4;
	s32 i;

	for (i = 4; i < MAX_MPCHRS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			numvacant--;
		}
	}

	if (numvacant > 0) {
		return true;
	}

	return false;
}

bool mp_is_sim_slot_enabled(s32 slot)
{
	s32 numfree = MAX_BOTS;
	s32 i;

	if ((g_MpSetup.chrslots & (1 << (slot + 4))) == 0) {
		for (i = 0; i < MAX_BOTS; i++) {
			if (g_MpSetup.chrslots & (1 << (i + 4))) {
				numfree--;
			}
		}

		if (numfree > 0) {
			return true;
		}

		return false;
	}

	return true;
}

s32 mp_find_bot_profile(s32 type, s32 difficulty)
{
	s32 i;

	if (type == BOTTYPE_GENERAL) {
		for (i = 0; i < ARRAYCOUNT(g_BotProfiles); i++) {
			if (g_BotProfiles[i].difficulty == difficulty) {
				break;
			}
		}
	} else {
		for (i = 0; i < ARRAYCOUNT(g_BotProfiles); i++) {
			if (g_BotProfiles[i].type == type) {
				break;
			}
		}
	}

	if (i >= ARRAYCOUNT(g_BotProfiles)) {
		i = -1;
	}

	return i;
}

void mp_generate_bot_names(void)
{
	s32 counts[ARRAYCOUNT(g_BotProfiles)];
	s32 profilenum;
	s32 i;
	char name[16];

	for (i = 0; i < ARRAYCOUNT(g_BotProfiles); i++) {
		counts[i] = 0;
	}

	// Count the number of bots using each profile (MeatSim, TurtleSim etc)
	for (i = 4; i < MAX_MPCHRS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			profilenum = mp_find_bot_profile(g_BotConfigsArray[i - 4].type, g_BotConfigsArray[i - 4].difficulty);

			if (profilenum >= 0 && profilenum < ARRAYCOUNT(g_BotProfiles)) {
				counts[profilenum]++;
			}
		}
	}

	// Profiles with only one bot don't need to have to number appended to the
	// name, so mark those as -1. For profiles with multiple bots, reset them
	// to 0 because they'll be a counter for the final loop.
	for (i = 0; i < ARRAYCOUNT(g_BotProfiles); i++) {
		if (counts[i] <= 1) {
			counts[i] = -1;
		} else {
			counts[i] = 0;
		}
	}

	for (i = 4; i < MAX_MPCHRS; i++) {
		if (g_MpSetup.chrslots & (1 << i)) {
			profilenum = mp_find_bot_profile(g_BotConfigsArray[i - 4].type, g_BotConfigsArray[i - 4].difficulty);

			if (profilenum >= 0 && profilenum < ARRAYCOUNT(g_BotProfiles)) {
				if (counts[profilenum] >= 0) {
					// Multiple bots using this profile - append the number
					counts[profilenum]++;
					sprintf(name, "%s:%d\n", lang_get(g_BotProfiles[profilenum].name), counts[profilenum]);
					strcpy(g_BotConfigsArray[i - 4].base.name, name);
				} else {
					// One bots using this profile - just use the profile name
					sprintf(name, "%s\n", lang_get(g_BotProfiles[profilenum].name));
					strcpy(g_BotConfigsArray[i - 4].base.name, name);
				}
			}
		}
	}
}

#if VERSION >= VERSION_NTSC_1_0
const char var7f1b8b80[] = "Adding GBCHead to load to slot %d: guid is %x-%x, player is %d\n";
const char var7f1b8bc0[] = "PakId for player %d: %d\n";
const char var7f1b8bdc[] = "Save Player Result: %d   New GUID: %x\n";
const char var7f1b8c04[] = "PakId for player %d: %d\n";
const char var7f1b8c20[] = "Load Player - Result: %d\n";
#endif

s32 mp_chr_to_chrindex(struct chrdata *chr)
{
	s32 i;

	for (i = 0; i < g_MpNumChrs; i++) {
		if (g_MpAllChrPtrs[i] == chr) {
			return i;
		}
	}

	return -1;
}

struct chrdata *mp_chrindex_to_chr(s32 index)
{
	if (index >= 0 && index < g_MpNumChrs) {
		return g_MpAllChrPtrs[index];
	}

	return NULL;
}

/**
 * chrslots 0-3 are human players
 * chrslots 4-11 are bot players
 *
 * So for a match with 2 humans and a bot, the chrslots are:
 * HH..B.......
 *
 * player 1 = chrindex 0, chrslot 0
 * player 2 = chrindex 1, chrslot 1
 * bot      = chrindex 2, chrslot 4
 */
s32 mp_chrindex_to_chrslot(s32 chrnum)
{
	s32 i;

	for (i = 0; i < MAX_PLAYERS; i++) {
		if (&g_PlayerConfigsArray[i].base == g_MpAllChrConfigPtrs[chrnum]) {
			return i;
		}
	}

	for (i = 0; i < MAX_BOTS; i++) {
		if (&g_BotConfigsArray[i].base == g_MpAllChrConfigPtrs[chrnum]) {
			return MAX_PLAYERS + i;
		}
	}

	return -1;
}

s32 mp_chrslot_to_chrindex(s32 chrslot)
{
	s32 i;

	if (chrslot < MAX_PLAYERS) {
		for (i = 0; i < g_MpNumChrs; i++) {
			if (g_MpAllChrConfigPtrs[i] == &g_PlayerConfigsArray[chrslot].base) {
				return i;
			}
		}
	} else {
		for (i = 0; i < g_MpNumChrs; i++) {
			if (g_MpAllChrConfigPtrs[i] == &g_BotConfigsArray[chrslot - MAX_PLAYERS].base) {
				return i;
			}
		}
	}

	return -1;
}

void mpplayerfile_load_gun_funcs(struct savebuffer *buffer, s32 playernum)
{
	s32 bitsremaining = 35;
	s32 i = 0;

	while (bitsremaining > 0) {
		s32 numbits = bitsremaining;

		if (numbits > 8) {
			numbits = 8;
		}

		g_PlayerConfigsArray[playernum].gunfuncs[i] = savebuffer_read_bits(buffer, numbits);

		bitsremaining -= 8;
		i++;
	}
}

void mpplayerfile_save_gun_funcs(struct savebuffer *buffer, s32 playernum)
{
	s32 bitsremaining = 35;
	s32 i = 0;

	while (bitsremaining > 0) {
		s32 numbits = bitsremaining;

		if (numbits > 8) {
			numbits = 8;
		}

		savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].gunfuncs[i], numbits);

		bitsremaining -= 8;
		i++;
	}
}

void mpplayerfile_load_wad(s32 playernum, struct savebuffer *buffer, bool arg2)
{
	struct fileguid guid;
	u32 stack;
	s32 i;
	s32 j;

	savebuffer_read_string(buffer, g_PlayerConfigsArray[playernum].base.name, 1);
	g_PlayerConfigsArray[playernum].time = savebuffer_read_bits(buffer, 28);

	if (arg2) {
		g_PlayerConfigsArray[playernum].base.mpheadnum = savebuffer_read_bits(buffer, 7);
		g_PlayerConfigsArray[playernum].base.mpbodynum = savebuffer_read_bits(buffer, 7);

		savebuffer_read_guid(buffer, &guid);

		if (g_PlayerConfigsArray[playernum].base.mpheadnum >= mp_get_num_heads2()) {
			if (guid.fileid != 0 && guid.deviceserial != 0) {
				if (g_MenuData.lastperfectheadfile < 11) {
					g_MenuData.lastperfectheadfile++;
					g_MenuData.perfectheadfiles[g_MenuData.lastperfectheadfile].fileguid.fileid = guid.fileid;
					g_MenuData.perfectheadfiles[g_MenuData.lastperfectheadfile].fileguid.deviceserial = guid.deviceserial;
					g_MenuData.perfectheadfiles[g_MenuData.lastperfectheadfile].playernum1 = g_MpPlayerNum;
					g_MenuData.perfectheadfiles[g_MenuData.lastperfectheadfile].playernum2 = g_MpPlayerNum;
				}
			} else {
				g_PlayerConfigsArray[playernum].base.mpheadnum = MPHEAD_DARK_COMBAT;
			}
		}
	} else {
		savebuffer_read_bits(buffer, 7);
		savebuffer_read_bits(buffer, 7);
		savebuffer_read_guid(buffer, &guid);
	}

	g_PlayerConfigsArray[playernum].base.displayoptions = savebuffer_read_bits(buffer, 8);
	g_PlayerConfigsArray[playernum].kills = savebuffer_read_bits(buffer, 20);
	g_PlayerConfigsArray[playernum].deaths = savebuffer_read_bits(buffer, 20);
	g_PlayerConfigsArray[playernum].gamesplayed = savebuffer_read_bits(buffer, 19);
	g_PlayerConfigsArray[playernum].gameswon = savebuffer_read_bits(buffer, 19);
	g_PlayerConfigsArray[playernum].gameslost = savebuffer_read_bits(buffer, 19);
	g_PlayerConfigsArray[playernum].distance = savebuffer_read_bits(buffer, 25);
	g_PlayerConfigsArray[playernum].accuracy = savebuffer_read_bits(buffer, 10);
	g_PlayerConfigsArray[playernum].damagedealt = savebuffer_read_bits(buffer, 26);
	g_PlayerConfigsArray[playernum].painreceived = savebuffer_read_bits(buffer, 26);
	g_PlayerConfigsArray[playernum].headshots = savebuffer_read_bits(buffer, 20);
	g_PlayerConfigsArray[playernum].ammoused = savebuffer_read_bits(buffer, 30);
	g_PlayerConfigsArray[playernum].accuracymedals = savebuffer_read_bits(buffer, 18);
	g_PlayerConfigsArray[playernum].headshotmedals = savebuffer_read_bits(buffer, 18);
	g_PlayerConfigsArray[playernum].killmastermedals = savebuffer_read_bits(buffer, 18);
	g_PlayerConfigsArray[playernum].survivormedals = savebuffer_read_bits(buffer, 16);
	g_PlayerConfigsArray[playernum].controlmode = savebuffer_read_bits(buffer, 2);
	g_PlayerConfigsArray[playernum].options = savebuffer_read_bits(buffer, 12);

	for (i = 0; i < ARRAYCOUNT(g_MpChallenges); i++) {
		for (j = 1; j < MAX_PLAYERS + 1; j++) {
			challenge_set_completed_by_player_with_num_players(playernum, i, j, savebuffer_read_bits(buffer, 1));
		}
	}

	challenge_determine_unlocked_features();
	mp_calculate_player_title(&g_PlayerConfigsArray[playernum]);
	mpplayerfile_load_gun_funcs(buffer, playernum);
}

void mpplayerfile_save_wad(s32 playernum, struct savebuffer *buffer)
{
	s32 i;
	s32 j;
	u32 stack;

	savebuffer_write_string(buffer, g_PlayerConfigsArray[playernum].base.name);

	if (g_PlayerConfigsArray[playernum].time > 0x0fffffff) { // over 3106 days
		g_PlayerConfigsArray[playernum].time = 0x0fffffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].time, 28);
	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].base.mpheadnum, 7);
	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].base.mpbodynum, 7);

	if (g_PlayerConfigsArray[playernum].base.mpheadnum >= mp_get_num_heads2()) {
		struct fileguid guid;
		phead_get_guid(g_PlayerConfigsArray[playernum].base.mpheadnum - mp_get_num_heads2(), &guid);
		savebuffer_write_guid(buffer, &guid);
	} else {
		struct fileguid guid;
		guid.deviceserial = 0;
		guid.fileid = 0;
		savebuffer_write_guid(buffer, &guid);
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].base.displayoptions, 8);

	if (g_PlayerConfigsArray[playernum].kills > 0xfffff) { // 1,048,575
		g_PlayerConfigsArray[playernum].kills = 0xfffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].kills, 20);

	if (g_PlayerConfigsArray[playernum].deaths > 0xfffff) { // 1,048,575
		g_PlayerConfigsArray[playernum].deaths = 0xfffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].deaths, 20);

	if (g_PlayerConfigsArray[playernum].gamesplayed > 0x7ffff) { // 524,287
		g_PlayerConfigsArray[playernum].gamesplayed = 0x7ffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].gamesplayed, 19);

	if (g_PlayerConfigsArray[playernum].gameswon > 0x7ffff) { // 524,287
		g_PlayerConfigsArray[playernum].gameswon = 0x7ffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].gameswon, 19);

	if (g_PlayerConfigsArray[playernum].gameslost > 0x7ffff) { // 524,287
		g_PlayerConfigsArray[playernum].gameslost = 0x7ffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].gameslost, 19);

	if (g_PlayerConfigsArray[playernum].distance > 0x1ffffff) { // 33,554,431
		g_PlayerConfigsArray[playernum].distance = 0x1ffffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].distance, 25);

	if (g_PlayerConfigsArray[playernum].accuracy > 0x3ff) { // 1023
		g_PlayerConfigsArray[playernum].accuracy = 0x3ff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].accuracy, 10);

	if (g_PlayerConfigsArray[playernum].damagedealt > 0x3ffffff) { // 67,108,863
		g_PlayerConfigsArray[playernum].damagedealt = 0x3ffffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].damagedealt, 26);

	if (g_PlayerConfigsArray[playernum].painreceived > 0x3ffffff) { // 67,108,863
		g_PlayerConfigsArray[playernum].painreceived = 0x3ffffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].painreceived, 26);

	if (g_PlayerConfigsArray[playernum].headshots > 0xfffff) { // 1,048,575
		g_PlayerConfigsArray[playernum].headshots = 0xfffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].headshots, 20);

	if (g_PlayerConfigsArray[playernum].ammoused > 0x3fffffff) { // 1,073,741,823
		g_PlayerConfigsArray[playernum].ammoused = 0x3fffffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].ammoused, 30);

	if (g_PlayerConfigsArray[playernum].accuracymedals > 0x3ffff) { // 262,143
		g_PlayerConfigsArray[playernum].accuracymedals = 0x3ffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].accuracymedals, 18);

	if (g_PlayerConfigsArray[playernum].headshotmedals > 0x3ffff) { // 262,143
		g_PlayerConfigsArray[playernum].headshotmedals = 0x3ffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].headshotmedals, 18);

	if (g_PlayerConfigsArray[playernum].killmastermedals > 0x3ffff) { // 262,143
		g_PlayerConfigsArray[playernum].killmastermedals = 0x3ffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].killmastermedals, 18);

	if (g_PlayerConfigsArray[playernum].survivormedals > 0xffff) { // 65,535
		g_PlayerConfigsArray[playernum].survivormedals = 0xffff;
	}

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].survivormedals, 16);

	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].controlmode, 2);
	savebuffer_write_bits(buffer, g_PlayerConfigsArray[playernum].options, 12);

	for (i = 0; i < ARRAYCOUNT(g_MpChallenges); i++) {
		for (j = 1; j < MAX_PLAYERS + 1; j++) {
			savebuffer_write_bits(buffer, challenge_is_completed_by_player_with_num_players(playernum, i, j), 1);
		}
	}

	mpplayerfile_save_gun_funcs(buffer, playernum);
}

void mpplayerfile_get_overview(char *arg0, char *name, u32 *playtime)
{
	struct savebuffer buffer;

	savebuffer_prepare_string(&buffer, arg0, 15);

	savebuffer_read_string(&buffer, name, 0);
	*playtime = savebuffer_read_bits(&buffer, 28);
}

s32 mpplayerfile_save(s32 playernum, s32 device, s32 fileid, u16 deviceserial)
{
	s32 ret;
	s32 newfileid;
	struct savebuffer buffer;

	if (device >= 0) {
		savebuffer_reset(&buffer);
		mpplayerfile_save_wad(playernum, &buffer);
		savebuffer_print(&buffer);

		var80075bd0[2] = true;

		ret = pak_save_at_guid(device, fileid, PAKFILETYPE_MPPLAYER, buffer.bytes, &newfileid, 0);

		if (ret == 0) {
			g_PlayerConfigsArray[playernum].fileguid.fileid = newfileid;
			g_PlayerConfigsArray[playernum].fileguid.deviceserial = deviceserial;
			return 0;
		}

		g_FilemgrLastPakError = ret;
		return -1;
	}

	return -1;
}

s32 mpplayerfile_load(s32 playernum, s32 device, s32 fileid, u16 deviceserial)
{
	s32 ret;
	struct savebuffer buffer;

	if (device >= 0) {
		savebuffer_reset(&buffer);

		ret = pak_read_body_at_guid(device, fileid, buffer.bytes, 0);

		if (ret == 0) {
			g_PlayerConfigsArray[playernum].fileguid.fileid = fileid;
			g_PlayerConfigsArray[playernum].fileguid.deviceserial = deviceserial;

			mpplayerfile_load_wad(playernum, &buffer, true);
			savebuffer_print(&buffer);

			g_PlayerConfigsArray[playernum].handicap = 0x80;
			return 0;
		}

		g_FilemgrLastPakError = ret;
		return -1;
	}

	return -1;
}

struct mppreset g_MpPresets[] = {
	{ L_MPWEAPONS_025, MPCONFIG_NOSHIELD   }, // "No Shield"
	{ L_MPWEAPONS_026, MPCONFIG_AUTOMATICS }, // "Automatics"
	{ L_MPWEAPONS_027, MPCONFIG_ROCKETS    }, // "Rocket Launcher"
	{ L_MPWEAPONS_028, MPCONFIG_SIMULANTS  }, // "Simulants"
	{ L_MPWEAPONS_029, MPCONFIG_SKEDARKING }, // "King of the Hill"
	{ L_MPWEAPONS_030, MPCONFIG_FARSIGHT   }, // "Complex FarSight"
	{ L_MPWEAPONS_031, MPCONFIG_BRIEFCASE  }, // "Hold the Briefcase"
	{ L_MPWEAPONS_032, MPCONFIG_PISTOLS    }, // "Pistol One-Hit Kills"
	{ L_MPWEAPONS_033, MPCONFIG_FLAGS      }, // "Capture the Case"
	{ L_MPWEAPONS_034, MPCONFIG_CLOAKING   }, // "Cloaking"
	{ L_MPWEAPONS_035, MPCONFIG_TEMPLE     }, // "Temple Explosives"
	{ L_MPWEAPONS_036, MPCONFIG_SLAYER     }, // "Slayer"
	{ L_MPWEAPONS_037, MPCONFIG_DRUGGUN    }, // "Tranquilizr"
	{ L_MPWEAPONS_038, MPCONFIG_SLOWMO     }, // "Slow Motion"
};

s32 mp_get_num_presets(void)
{
	return ARRAYCOUNT(g_MpPresets);
}

bool mp_is_preset_unlocked(s32 presetnum)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_MpPresets[presetnum].requirefeatures); i++) {
		if (!challenge_is_feature_unlocked(g_MpPresets[presetnum].requirefeatures[i]) &&
				g_MpPresets[presetnum].requirefeatures[i] != MPFEATURE_WEAPON_SHIELD) {
			return false;
		}
	}

	return true;
}

s32 mp_get_num_unlocked_presets(void)
{
	s32 numunlocked = 0;
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_MpPresets); i++) {
		if (mp_is_preset_unlocked(i)) {
			numunlocked++;
		}
	}

	return numunlocked;
}

char *mp_get_preset_name_by_slot(s32 slot)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_MpPresets); i++) {
		if (mp_is_preset_unlocked(i)) {
			if (slot == 0) {
				return lang_get(g_MpPresets[i].name);
			}

			slot--;
		}
	}

	return "";
}

#if VERSION >= VERSION_NTSC_1_0
const char var7f1b8c40[] = "bot %d headId %d bodyId %d\n";
const char var7f1b8c5c[] = "team change %s %d\n";

#if VERSION >= VERSION_PAL_FINAL
const char var7f1b8c70[] = "options/mplayer.c";
#else
const char var7f1b8c70[] = "mplayer.c";
#endif

const char var7f1b8c7c[] = "team change %s %d\n";

#if VERSION >= VERSION_PAL_FINAL
const char var7f1b8c90[] = "options/mplayer.c";
#else
const char var7f1b8c90[] = "mplayer.c";
#endif

const char var7f1b8c9c[] = "SaveMultiGameFile : PakId=0x%x, FileId=0x%x\n";
const char var7f1b8ccc[] = "SaveGame Result: %d   New GUID: %x\n";
const char var7f1b8cf0[] = "LoadMultiGameFile : PakId=0x%x, FileId=0x%x\n";
const char var7f1b8d20[] = "LoadGame Result: %d\n";
#endif // >= VERSION_NTSC_1_0

void mp_apply_config(struct mpconfigfull *config)
{
	s32 i;
	s32 j;
	u16 chrslots;

	g_MpSetup.scenario = config->config.setup.scenario;

#if VERSION >= VERSION_PAL_FINAL
	chrslots = g_MpSetup.chrslots;
#endif

	scenario_init();

	g_MpSetup = config->config.setup;

#if VERSION >= VERSION_PAL_FINAL
	g_MpSetup.chrslots = chrslots;
#endif

	for (i = 0; i < MAX_BOTS; i++) {
		g_BotConfigsArray[i].type = config->config.simulants[i].type;

		for (j = 0; j < MAX_PLAYERS; j++) {
			g_MpSimulantDifficultiesPerNumPlayers[i][j] = config->config.simulants[i].difficulties[j];
		}

		g_BotConfigsArray[i].difficulty = g_MpSimulantDifficultiesPerNumPlayers[i][0];

#if VERSION >= VERSION_NTSC_1_0
		if (IS4MB()) {
			// "ShockSim:%d"
			sprintf(g_BotConfigsArray[i].base.name, lang_get(L_MPWEAPONS_241), i + 1);
		} else {
			strcpy(g_BotConfigsArray[i].base.name, config->strings.aibotnames[i]);
		}
#else
		strcpy(g_BotConfigsArray[i].base.name, config->strings.aibotnames[i]);
#endif

		g_BotConfigsArray[i].base.mpheadnum = config->config.simulants[i].mpheadnum;
		g_BotConfigsArray[i].base.mpbodynum = config->config.simulants[i].mpbodynum;
		g_BotConfigsArray[i].base.team = config->config.simulants[i].team;
	}

	if (!challenge_is_feature_unlocked(MPFEATURE_WEAPON_SHIELD)) {
		for (i = 0; i < ARRAYCOUNT(g_MpSetup.weapons); i++) {
			if (g_MpSetup.weapons[i] == MPWEAPON_SHIELD) {
				g_MpSetup.weapons[i] = MPWEAPON_NONE;
			}
		}
	}

	mp_find_weaponsetnum_by_weapons();
	challenge_remove_force_unlocks();
}

void mp_load_preset_by_slotnum(s32 slot)
{
	struct mpconfigfull *config;
	u8 buffer[0x1ca];
	s32 confignum = 0;
	u32 i;

	for (i = 0; i < ARRAYCOUNT(g_MpPresets); i++) {
		if (mp_is_preset_unlocked(i)) {
			if (slot == 0) {
				confignum = g_MpPresets[i].confignum;
				break;
			}

			slot--;
		}
	}

	config = challenge_load_config(confignum, buffer, sizeof(buffer));

	mp_apply_config(config);

#if VERSION >= VERSION_JPN_FINAL
	g_MpSetup.chrslots &= 0x0f;

	for (i = 0; i < MAX_BOTS; i++) {
		if (g_BotConfigsArray[i].difficulty != BOTDIFF_DISABLED) {
			g_MpSetup.chrslots |= 1 << (i + 4);
		}
	}
#endif
}

void mpsetupfile_load_wad(struct savebuffer *buffer)
{
	s32 i;
	s32 j;

	savebuffer_read_string(buffer, g_MpSetup.name, false);
	savebuffer_read_bits(buffer, 4);

	g_MpSetup.stagenum = savebuffer_read_bits(buffer, 7);
	g_MpSetup.scenario = savebuffer_read_bits(buffer, 3);

	scenario_init();
	scenario_read_save(buffer);

	g_MpSetup.options = savebuffer_read_bits(buffer, 21);
	g_MpSetup.chrslots &= 0x000f;

	for (i = 0; i < MAX_BOTS; i++) {
		g_BotConfigsArray[i].base.name[0] = '\0';
		g_BotConfigsArray[i].type = savebuffer_read_bits(buffer, 5);
		g_BotConfigsArray[i].difficulty = savebuffer_read_bits(buffer, 3);

		for (j = 0; j < MAX_PLAYERS; j++) {
			g_MpSimulantDifficultiesPerNumPlayers[i][j] = g_BotConfigsArray[i].difficulty;
		}

		if (g_BotConfigsArray[i].difficulty != BOTDIFF_DISABLED) {
			g_MpSetup.chrslots |= 1 << (i + 4);
		}

		g_BotConfigsArray[i].base.mpheadnum = savebuffer_read_bits(buffer, 7);
		g_BotConfigsArray[i].base.mpbodynum = savebuffer_read_bits(buffer, 7);
		g_BotConfigsArray[i].base.team = savebuffer_read_bits(buffer, 3);
	}

	mp_generate_bot_names();

	for (i = 0; i < ARRAYCOUNT(g_MpSetup.weapons); i++) {
		g_MpSetup.weapons[i] = savebuffer_read_bits(buffer, 7);
	}

	mp_find_weaponsetnum_by_weapons();

	g_MpSetup.timelimit = savebuffer_read_bits(buffer, 6);
	g_MpSetup.scorelimit = savebuffer_read_bits(buffer, 7);
	g_MpSetup.teamscorelimit = savebuffer_read_bits(buffer, 9);

	for (i = 0; i < MAX_PLAYERS; i++) {
		g_PlayerConfigsArray[i].base.team = savebuffer_read_bits(buffer, 3);
	}

	challenge_force_unlock_bot_features();
}

void mpsetupfile_save_wad(struct savebuffer *buffer)
{
	s32 numsims = 0;
	s32 mpbodynum;
	s32 i;

	savebuffer_write_string(buffer, g_MpSetup.name);

	for (i = 0; i < MAX_BOTS; i++) {
		if (g_MpSetup.chrslots & (1 << (i + 4))) {
			numsims++;
		}
	}

	savebuffer_write_bits(buffer, numsims, 4);
	savebuffer_write_bits(buffer, g_MpSetup.stagenum, 7);
	savebuffer_write_bits(buffer, g_MpSetup.scenario, 3);

	scenario_write_save(buffer);

	savebuffer_write_bits(buffer, g_MpSetup.options, 21);

	for (i = 0; i < MAX_BOTS; i++) {
		savebuffer_write_bits(buffer, g_BotConfigsArray[i].type, 5);

		if (g_MpSetup.chrslots & (1 << (i + 4))) {
			savebuffer_write_bits(buffer, g_BotConfigsArray[i].difficulty, 3);
		} else {
			savebuffer_write_bits(buffer, BOTDIFF_DISABLED, 3);
		}

		savebuffer_write_bits(buffer, g_BotConfigsArray[i].base.mpheadnum, 7);

		if (g_BotConfigsArray[i].base.mpbodynum == 0xff) {
			s32 profilenum = mp_find_bot_profile(g_BotConfigsArray[i].type, g_BotConfigsArray[i].difficulty);

			if (profilenum < 0 || profilenum >= ARRAYCOUNT(g_BotProfiles)) {
				profilenum = 0;
			}

			mpbodynum = g_BotProfiles[profilenum].body;
		} else {
			mpbodynum = g_BotConfigsArray[i].base.mpbodynum;
		}

		savebuffer_write_bits(buffer, mpbodynum, 7);
		savebuffer_write_bits(buffer, g_BotConfigsArray[i].base.team, 3);
	}

	for (i = 0; i < ARRAYCOUNT(g_MpSetup.weapons); i++) {
		savebuffer_write_bits(buffer, g_MpSetup.weapons[i], 7);
	}

	savebuffer_write_bits(buffer, g_MpSetup.timelimit, 6);
	savebuffer_write_bits(buffer, g_MpSetup.scorelimit, 7);
	savebuffer_write_bits(buffer, g_MpSetup.teamscorelimit, 9);

	for (i = 0; i < MAX_PLAYERS; i++) {
		savebuffer_write_bits(buffer, g_PlayerConfigsArray[i].base.team, 3);
	}
}

void mpsetupfile_get_overview(char *arg0, char *filename, u16 *numsims, u16 *stagenum, u16 *scenarionum)
{
	struct savebuffer buffer;

	savebuffer_prepare_string(&buffer, arg0, 15);

	savebuffer_read_string(&buffer, filename, 0);

	*numsims = savebuffer_read_bits(&buffer, 4);
	*stagenum = savebuffer_read_bits(&buffer, 7);
	*scenarionum = savebuffer_read_bits(&buffer, 3);
}

s32 mpsetupfile_save(s32 device, s32 fileid, u16 deviceserial)
{
	s32 ret;
	s32 newfileid;
	struct savebuffer buffer;

	if (device >= 0) {
		savebuffer_reset(&buffer);
		mpsetupfile_save_wad(&buffer);
		savebuffer_print(&buffer);

		ret = pak_save_at_guid(device, fileid, PAKFILETYPE_MPSETUP, buffer.bytes, &newfileid, 0);
		var80075bd0[1] = true;

		if (ret == 0) {
			g_MpSetup.fileguid.fileid = newfileid;
			g_MpSetup.fileguid.deviceserial = deviceserial;
			return 0;
		}

		g_FilemgrLastPakError = ret;
		return -1;
	}

	return -1;
}

s32 mpsetupfile_load(s32 device, s32 fileid, u16 deviceserial)
{
	s32 ret;
	struct savebuffer buffer;

	if (device >= 0) {
		savebuffer_reset(&buffer);
		ret = pak_read_body_at_guid(device, fileid, buffer.bytes, 0);

		if (ret == 0) {
			g_MpSetup.fileguid.fileid = fileid;
			g_MpSetup.fileguid.deviceserial = deviceserial;

			mpsetupfile_load_wad(&buffer);
			savebuffer_print(&buffer);

			return 0;
		}

		g_FilemgrLastPakError = ret;

		return -1;
	}

	return -1;
}

void mp_reset_phead_modeldefs(void)
{
	s32 i;

	for (i = 0; i < ARRAYCOUNT(g_PheadModeldefs); i++) {
		g_PheadModeldefs[i] = NULL;
	}
}

struct modeldef *mp_get_phead_modeldef(s32 index, s32 *headnum)
{
#if VERSION >= VERSION_NTSC_1_0
	osSyncPrintf("GBCHead: Call to create head for slot %d (gbcheadobjs[slotno]=%x)\n", index, g_PheadModeldefs[index]);
#endif

	return g_PheadModeldefs[index];
}
