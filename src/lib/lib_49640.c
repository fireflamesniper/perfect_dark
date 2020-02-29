#include <ultra64.h>
#include "constants.h"
#include "game/data/data_000000.h"
#include "game/data/data_0083d0.h"
#include "game/data/data_0160b0.h"
#include "game/data/data_020df0.h"
#include "gvars/gvars.h"
#include "lib/lib_49780.h"
#include "lib/lib_4ae00.h"
#include "types.h"

GLOBAL_ASM(
glabel func00049640
/*    49640:	27bdffe8 */ 	addiu	$sp,$sp,-24
/*    49644:	afbf0014 */ 	sw	$ra,0x14($sp)
/*    49648:	3c048006 */ 	lui	$a0,%hi(var800608b0)
/*    4964c:	248408b0 */ 	addiu	$a0,$a0,%lo(var800608b0)
/*    49650:	0c012c30 */ 	jal	func0004b0c0
/*    49654:	24050060 */ 	addiu	$a1,$zero,0x60
/*    49658:	3c0e8006 */ 	lui	$t6,%hi(var800608b0)
/*    4965c:	25ce08b0 */ 	addiu	$t6,$t6,%lo(var800608b0)
/*    49660:	3c018006 */ 	lui	$at,0x8006
/*    49664:	ac2e0910 */ 	sw	$t6,0x910($at)
/*    49668:	3c018006 */ 	lui	$at,0x8006
/*    4966c:	25cf0030 */ 	addiu	$t7,$t6,0x30
/*    49670:	ac2f0914 */ 	sw	$t7,0x914($at)
/*    49674:	24180001 */ 	addiu	$t8,$zero,0x1
/*    49678:	a5d80032 */ 	sh	$t8,0x32($t6)
/*    4967c:	3c088006 */ 	lui	$t0,0x8006
/*    49680:	8d080910 */ 	lw	$t0,0x910($t0)
/*    49684:	24190001 */ 	addiu	$t9,$zero,0x1
/*    49688:	3c0a8006 */ 	lui	$t2,0x8006
/*    4968c:	a5190002 */ 	sh	$t9,0x2($t0)
/*    49690:	8d4a0914 */ 	lw	$t2,0x914($t2)
/*    49694:	3c098000 */ 	lui	$t1,0x8000
/*    49698:	3c0c8006 */ 	lui	$t4,0x8006
/*    4969c:	ad490004 */ 	sw	$t1,0x4($t2)
/*    496a0:	8d8c0910 */ 	lw	$t4,0x910($t4)
/*    496a4:	3c0b8000 */ 	lui	$t3,0x8000
/*    496a8:	3c0d8000 */ 	lui	$t5,0x8000
/*    496ac:	ad8b0004 */ 	sw	$t3,0x4($t4)
/*    496b0:	8dad0300 */ 	lw	$t5,0x300($t5)
/*    496b4:	15a00007 */ 	bnez	$t5,.L000496d4
/*    496b8:	00000000 */ 	sll	$zero,$zero,0x0
/*    496bc:	3c188006 */ 	lui	$t8,0x8006
/*    496c0:	8f180914 */ 	lw	$t8,0x914($t8)
/*    496c4:	3c0f8006 */ 	lui	$t7,%hi(var80061240)
/*    496c8:	25ef1240 */ 	addiu	$t7,$t7,%lo(var80061240)
/*    496cc:	10000011 */ 	beqz	$zero,.L00049714
/*    496d0:	af0f0008 */ 	sw	$t7,0x8($t8)
.L000496d4:
/*    496d4:	3c0e8000 */ 	lui	$t6,0x8000
/*    496d8:	8dce0300 */ 	lw	$t6,0x300($t6)
/*    496dc:	24010002 */ 	addiu	$at,$zero,0x2
/*    496e0:	15c10007 */ 	bne	$t6,$at,.L00049700
/*    496e4:	00000000 */ 	sll	$zero,$zero,0x0
/*    496e8:	3c088006 */ 	lui	$t0,0x8006
/*    496ec:	8d080914 */ 	lw	$t0,0x914($t0)
/*    496f0:	3c198006 */ 	lui	$t9,%hi(var80061290)
/*    496f4:	27391290 */ 	addiu	$t9,$t9,%lo(var80061290)
/*    496f8:	10000006 */ 	beqz	$zero,.L00049714
/*    496fc:	ad190008 */ 	sw	$t9,0x8($t0)
.L00049700:
/*    49700:	3c0a8006 */ 	lui	$t2,0x8006
/*    49704:	8d4a0914 */ 	lw	$t2,0x914($t2)
/*    49708:	3c098006 */ 	lui	$t1,%hi(var800612e0)
/*    4970c:	252912e0 */ 	addiu	$t1,$t1,%lo(var800612e0)
/*    49710:	ad490008 */ 	sw	$t1,0x8($t2)
.L00049714:
/*    49714:	3c0c8006 */ 	lui	$t4,0x8006
/*    49718:	8d8c0914 */ 	lw	$t4,0x914($t4)
/*    4971c:	240b0020 */ 	addiu	$t3,$zero,0x20
/*    49720:	3c0d8006 */ 	lui	$t5,0x8006
/*    49724:	a58b0000 */ 	sh	$t3,0x0($t4)
/*    49728:	8dad0914 */ 	lw	$t5,0x914($t5)
/*    4972c:	3c0ea440 */ 	lui	$t6,0xa440
/*    49730:	8daf0008 */ 	lw	$t7,0x8($t5)
/*    49734:	8df80004 */ 	lw	$t8,0x4($t7)
/*    49738:	adb8000c */ 	sw	$t8,0xc($t5)
/*    4973c:	8dd90010 */ 	lw	$t9,0x10($t6)
/*    49740:	2f21000b */ 	sltiu	$at,$t9,0xb
/*    49744:	14200006 */ 	bnez	$at,.L00049760
/*    49748:	00000000 */ 	sll	$zero,$zero,0x0
.L0004974c:
/*    4974c:	3c08a440 */ 	lui	$t0,0xa440
/*    49750:	8d090010 */ 	lw	$t1,0x10($t0)
/*    49754:	2d21000b */ 	sltiu	$at,$t1,0xb
/*    49758:	1020fffc */ 	beqz	$at,.L0004974c
/*    4975c:	00000000 */ 	sll	$zero,$zero,0x0
.L00049760:
/*    49760:	3c0aa440 */ 	lui	$t2,0xa440
/*    49764:	0c0125e4 */ 	jal	func00049790
/*    49768:	ad400000 */ 	sw	$zero,0x0($t2)
/*    4976c:	8fbf0014 */ 	lw	$ra,0x14($sp)
/*    49770:	27bd0018 */ 	addiu	$sp,$sp,0x18
/*    49774:	03e00008 */ 	jr	$ra
/*    49778:	00000000 */ 	sll	$zero,$zero,0x0
/*    4977c:	00000000 */ 	sll	$zero,$zero,0x0
);