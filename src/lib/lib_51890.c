#include <ultra64.h>
#include "constants.h"
#include "game/data/data_000000.h"
#include "game/data/data_0083d0.h"
#include "game/data/data_00e460.h"
#include "game/data/data_0160b0.h"
#include "game/data/data_01a3a0.h"
#include "game/data/data_020df0.h"
#include "game/data/data_02da90.h"
#include "gvars/gvars.h"
#include "types.h"

GLOBAL_ASM(
glabel func00051890
/*    51890:	3c0ea404 */ 	lui	$t6,0xa404
/*    51894:	03e00008 */ 	jr	$ra
/*    51898:	8dc20010 */ 	lw	$v0,0x10($t6)
/*    5189c:	00000000 */ 	nop
);
