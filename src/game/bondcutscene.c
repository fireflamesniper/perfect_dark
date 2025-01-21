#include <ultra64.h>
#include "constants.h"
#include "bss.h"
#include "lib/lib_317f0.h"
#include "data.h"
#include "types.h"

void bcutscene_init(void)
{
	s32 i;
	s32 j;

	g_Vars.currentplayer->bondmovemode = MOVEMODE_CUTSCENE;

	for (i = 0; i < PLAYERCOUNT(); i++) {
		g_Vars.players[i]->joybutinhibit = 0xffffffff;

		// @bug? Should this be < 2?
		for (j = 0; j < 1; j++) {
			struct hand *hand = &g_Vars.players[i]->hands[j];

			if (hand->audiohandle && sndp_get_state(hand->audiohandle) != AL_STOPPED) {
				sndp_stop_sound(hand->audiohandle);
				hand->audiohandle = NULL;
			}

			hand->firing = false;
		}
	}
}

void bcutscene_tick(void)
{
	// empty
}
