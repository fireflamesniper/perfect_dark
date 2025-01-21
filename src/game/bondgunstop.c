#include <ultra64.h>
#include "constants.h"
#include "bss.h"
#include "lib/vi.h"
#include "lib/dma.h"
#include "lib/lib_317f0.h"
#include "data.h"
#include "types.h"

void bgun_stop(void)
{
	s32 i;
	s32 j;

	for (i = 0; i < PLAYERCOUNT(); i++) {
		for (j = 0; j < 2; j++) {
			struct hand *hand = &g_Vars.players[i]->hands[j];

			if (hand->audiohandle2 && sndp_get_state(hand->audiohandle2) != AL_STOPPED) {
				sndp_stop_sound(hand->audiohandle2);
			}
		}
	}
}
