#include <ultra64.h>
#include "constants.h"
#include "game/data/data_000000.h"
#include "game/data/data_0083d0.h"
#include "game/data/data_00e460.h"
#include "game/data/data_0160b0.h"
#include "game/data/data_01a3a0.h"
#include "game/data/data_020df0.h"
#include "game/data/data_02da90.h"
#include "game/game_015470.h"
#include "game/game_01afc0.h"
#include "gvars/gvars.h"
#include "lib/lib_03fb0.h"
#include "lib/lib_04790.h"
#include "lib/lib_04840.h"
#include "lib/lib_04a80.h"
#include "lib/lib_09660.h"
#include "lib/lib_0d0a0.h"
#include "lib/lib_317f0.h"
#include "types.h"

const u32 var7f1a83b0[] = {0x459c4000};
const u32 var7f1a83b4[] = {0x00000000};
const u32 var7f1a83b8[] = {0x00000000};
const u32 var7f1a83bc[] = {0x00000000};

GLOBAL_ASM(
glabel func0f015470
/*  f015470:	27bdffd0 */ 	addiu	$sp,$sp,-48
/*  f015474:	3c02800a */ 	lui	$v0,%hi(g_Vars)
/*  f015478:	24429fc0 */ 	addiu	$v0,$v0,%lo(g_Vars)
/*  f01547c:	8c4e006c */ 	lw	$t6,0x6c($v0)
/*  f015480:	afb40028 */ 	sw	$s4,0x28($sp)
/*  f015484:	afbf002c */ 	sw	$ra,0x2c($sp)
/*  f015488:	afb30024 */ 	sw	$s3,0x24($sp)
/*  f01548c:	afb20020 */ 	sw	$s2,0x20($sp)
/*  f015490:	afb1001c */ 	sw	$s1,0x1c($sp)
/*  f015494:	afb00018 */ 	sw	$s0,0x18($sp)
/*  f015498:	11c00003 */ 	beqz	$t6,.L0f0154a8
/*  f01549c:	0000a025 */ 	or	$s4,$zero,$zero
/*  f0154a0:	10000002 */ 	b	.L0f0154ac
/*  f0154a4:	24070001 */ 	addiu	$a3,$zero,0x1
.L0f0154a8:
/*  f0154a8:	00003825 */ 	or	$a3,$zero,$zero
.L0f0154ac:
/*  f0154ac:	8c4f0068 */ 	lw	$t7,0x68($v0)
/*  f0154b0:	00002825 */ 	or	$a1,$zero,$zero
/*  f0154b4:	00001825 */ 	or	$v1,$zero,$zero
/*  f0154b8:	11e00003 */ 	beqz	$t7,.L0f0154c8
/*  f0154bc:	00145880 */ 	sll	$t3,$s4,0x2
/*  f0154c0:	10000001 */ 	b	.L0f0154c8
/*  f0154c4:	24050001 */ 	addiu	$a1,$zero,0x1
.L0f0154c8:
/*  f0154c8:	8c580064 */ 	lw	$t8,0x64($v0)
/*  f0154cc:	3c0c800a */ 	lui	$t4,%hi(g_Vars)
/*  f0154d0:	258c9fc0 */ 	addiu	$t4,$t4,%lo(g_Vars)
/*  f0154d4:	13000003 */ 	beqz	$t8,.L0f0154e4
/*  f0154d8:	016c9021 */ 	addu	$s2,$t3,$t4
/*  f0154dc:	10000001 */ 	b	.L0f0154e4
/*  f0154e0:	24030001 */ 	addiu	$v1,$zero,0x1
.L0f0154e4:
/*  f0154e4:	8c590070 */ 	lw	$t9,0x70($v0)
/*  f0154e8:	00001025 */ 	or	$v0,$zero,$zero
/*  f0154ec:	24130f48 */ 	addiu	$s3,$zero,0xf48
/*  f0154f0:	13200003 */ 	beqz	$t9,.L0f015500
/*  f0154f4:	00000000 */ 	nop
/*  f0154f8:	10000001 */ 	b	.L0f015500
/*  f0154fc:	24020001 */ 	addiu	$v0,$zero,0x1
.L0f015500:
/*  f015500:	00434021 */ 	addu	$t0,$v0,$v1
/*  f015504:	01054821 */ 	addu	$t1,$t0,$a1
/*  f015508:	01275021 */ 	addu	$t2,$t1,$a3
/*  f01550c:	19400030 */ 	blez	$t2,.L0f0155d0
/*  f015510:	00008825 */ 	or	$s1,$zero,$zero
.L0f015514:
/*  f015514:	8e4d0064 */ 	lw	$t5,0x64($s2)
.L0f015518:
/*  f015518:	01b18021 */ 	addu	$s0,$t5,$s1
/*  f01551c:	8e040804 */ 	lw	$a0,0x804($s0)
/*  f015520:	26100638 */ 	addiu	$s0,$s0,0x638
/*  f015524:	50800008 */ 	beqzl	$a0,.L0f015548
/*  f015528:	263107a4 */ 	addiu	$s1,$s1,0x7a4
/*  f01552c:	0c00cdfc */ 	jal	func000337f0
/*  f015530:	00000000 */ 	nop
/*  f015534:	50400004 */ 	beqzl	$v0,.L0f015548
/*  f015538:	263107a4 */ 	addiu	$s1,$s1,0x7a4
/*  f01553c:	0c00cec9 */ 	jal	audioStop
/*  f015540:	8e0401cc */ 	lw	$a0,0x1cc($s0)
/*  f015544:	263107a4 */ 	addiu	$s1,$s1,0x7a4
.L0f015548:
/*  f015548:	5633fff3 */ 	bnel	$s1,$s3,.L0f015518
/*  f01554c:	8e4d0064 */ 	lw	$t5,0x64($s2)
/*  f015550:	3c02800a */ 	lui	$v0,%hi(g_Vars+0x6c)
/*  f015554:	8c42a02c */ 	lw	$v0,%lo(g_Vars+0x6c)($v0)
/*  f015558:	3c03800a */ 	lui	$v1,%hi(g_Vars+0x68)
/*  f01555c:	3c04800a */ 	lui	$a0,%hi(g_Vars+0x64)
/*  f015560:	3c06800a */ 	lui	$a2,%hi(g_Vars+0x70)
/*  f015564:	26940001 */ 	addiu	$s4,$s4,0x1
/*  f015568:	26520004 */ 	addiu	$s2,$s2,0x4
/*  f01556c:	8cc6a030 */ 	lw	$a2,%lo(g_Vars+0x70)($a2)
/*  f015570:	8c84a024 */ 	lw	$a0,%lo(g_Vars+0x64)($a0)
/*  f015574:	10400003 */ 	beqz	$v0,.L0f015584
/*  f015578:	8c63a028 */ 	lw	$v1,%lo(g_Vars+0x68)($v1)
/*  f01557c:	10000002 */ 	b	.L0f015588
/*  f015580:	24070001 */ 	addiu	$a3,$zero,0x1
.L0f015584:
/*  f015584:	00003825 */ 	or	$a3,$zero,$zero
.L0f015588:
/*  f015588:	10600003 */ 	beqz	$v1,.L0f015598
/*  f01558c:	00002825 */ 	or	$a1,$zero,$zero
/*  f015590:	10000001 */ 	b	.L0f015598
/*  f015594:	24050001 */ 	addiu	$a1,$zero,0x1
.L0f015598:
/*  f015598:	10800003 */ 	beqz	$a0,.L0f0155a8
/*  f01559c:	00001825 */ 	or	$v1,$zero,$zero
/*  f0155a0:	10000001 */ 	b	.L0f0155a8
/*  f0155a4:	24030001 */ 	addiu	$v1,$zero,0x1
.L0f0155a8:
/*  f0155a8:	10c00003 */ 	beqz	$a2,.L0f0155b8
/*  f0155ac:	00001025 */ 	or	$v0,$zero,$zero
/*  f0155b0:	10000001 */ 	b	.L0f0155b8
/*  f0155b4:	24020001 */ 	addiu	$v0,$zero,0x1
.L0f0155b8:
/*  f0155b8:	00437021 */ 	addu	$t6,$v0,$v1
/*  f0155bc:	01c57821 */ 	addu	$t7,$t6,$a1
/*  f0155c0:	01e7c021 */ 	addu	$t8,$t7,$a3
/*  f0155c4:	0298082a */ 	slt	$at,$s4,$t8
/*  f0155c8:	5420ffd2 */ 	bnezl	$at,.L0f015514
/*  f0155cc:	00008825 */ 	or	$s1,$zero,$zero
.L0f0155d0:
/*  f0155d0:	8fbf002c */ 	lw	$ra,0x2c($sp)
/*  f0155d4:	8fb00018 */ 	lw	$s0,0x18($sp)
/*  f0155d8:	8fb1001c */ 	lw	$s1,0x1c($sp)
/*  f0155dc:	8fb20020 */ 	lw	$s2,0x20($sp)
/*  f0155e0:	8fb30024 */ 	lw	$s3,0x24($sp)
/*  f0155e4:	8fb40028 */ 	lw	$s4,0x28($sp)
/*  f0155e8:	03e00008 */ 	jr	$ra
/*  f0155ec:	27bd0030 */ 	addiu	$sp,$sp,0x30
);

GLOBAL_ASM(
glabel func0f0155f0
/*  f0155f0:	44852000 */ 	mtc1	$a1,$f4
/*  f0155f4:	27bdfef8 */ 	addiu	$sp,$sp,-264
/*  f0155f8:	3c018006 */ 	lui	$at,%hi(var80062468)
/*  f0155fc:	468021a0 */ 	cvt.s.w	$f6,$f4
/*  f015600:	afbf0034 */ 	sw	$ra,0x34($sp)
/*  f015604:	afb00030 */ 	sw	$s0,0x30($sp)
/*  f015608:	afa60110 */ 	sw	$a2,0x110($sp)
/*  f01560c:	3c0f8006 */ 	lui	$t7,%hi(var80061360)
/*  f015610:	25ef1360 */ 	addiu	$t7,$t7,%lo(var80061360)
/*  f015614:	e4262468 */ 	swc1	$f6,%lo(var80062468)($at)
/*  f015618:	3c0e0600 */ 	lui	$t6,0x600
/*  f01561c:	ac8e0000 */ 	sw	$t6,0x0($a0)
/*  f015620:	ac8f0004 */ 	sw	$t7,0x4($a0)
/*  f015624:	3c188006 */ 	lui	$t8,%hi(var80062494)
/*  f015628:	8f182494 */ 	lw	$t8,%lo(var80062494)($t8)
/*  f01562c:	24900008 */ 	addiu	$s0,$a0,0x8
/*  f015630:	02001025 */ 	or	$v0,$s0,$zero
/*  f015634:	13000007 */ 	beqz	$t8,.L0f015654
/*  f015638:	3c19ba00 */ 	lui	$t9,0xba00
/*  f01563c:	0fc06bf0 */ 	jal	func0f01afc0
/*  f015640:	02002025 */ 	or	$a0,$s0,$zero
/*  f015644:	3c018006 */ 	lui	$at,%hi(var80062494)
/*  f015648:	00408025 */ 	or	$s0,$v0,$zero
/*  f01564c:	10000023 */ 	b	.L0f0156dc
/*  f015650:	ac202494 */ 	sw	$zero,%lo(var80062494)($at)
.L0f015654:
/*  f015654:	37391402 */ 	ori	$t9,$t9,0x1402
/*  f015658:	3c0e0030 */ 	lui	$t6,0x30
/*  f01565c:	ac4e0004 */ 	sw	$t6,0x4($v0)
/*  f015660:	ac590000 */ 	sw	$t9,0x0($v0)
/*  f015664:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015668:	afb000fc */ 	sw	$s0,0xfc($sp)
/*  f01566c:	0c002f02 */ 	jal	viGetX
/*  f015670:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015674:	244fffff */ 	addiu	$t7,$v0,-1
/*  f015678:	8fae00fc */ 	lw	$t6,0xfc($sp)
/*  f01567c:	31f80fff */ 	andi	$t8,$t7,0xfff
/*  f015680:	3c01ff10 */ 	lui	$at,0xff10
/*  f015684:	0301c825 */ 	or	$t9,$t8,$at
/*  f015688:	0c002ac7 */ 	jal	viGetUnk28
/*  f01568c:	add90000 */ 	sw	$t9,0x0($t6)
/*  f015690:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f015694:	00402025 */ 	or	$a0,$v0,$zero
/*  f015698:	8faf00fc */ 	lw	$t7,0xfc($sp)
/*  f01569c:	02001825 */ 	or	$v1,$s0,$zero
/*  f0156a0:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f0156a4:	ade20004 */ 	sw	$v0,0x4($t7)
/*  f0156a8:	3c190001 */ 	lui	$t9,0x1
/*  f0156ac:	37390001 */ 	ori	$t9,$t9,0x1
/*  f0156b0:	3c18f700 */ 	lui	$t8,0xf700
/*  f0156b4:	02002025 */ 	or	$a0,$s0,$zero
/*  f0156b8:	ac780000 */ 	sw	$t8,0x0($v1)
/*  f0156bc:	ac790004 */ 	sw	$t9,0x4($v1)
/*  f0156c0:	3c0f0032 */ 	lui	$t7,0x32
/*  f0156c4:	3c0ef66d */ 	lui	$t6,0xf66d
/*  f0156c8:	35cec5ec */ 	ori	$t6,$t6,0xc5ec
/*  f0156cc:	35ef0190 */ 	ori	$t7,$t7,0x190
/*  f0156d0:	ac8f0004 */ 	sw	$t7,0x4($a0)
/*  f0156d4:	ac8e0000 */ 	sw	$t6,0x0($a0)
/*  f0156d8:	26100008 */ 	addiu	$s0,$s0,0x8
.L0f0156dc:
/*  f0156dc:	3c0142c8 */ 	lui	$at,0x42c8
/*  f0156e0:	44814000 */ 	mtc1	$at,$f8
/*  f0156e4:	3c017f1b */ 	lui	$at,%hi(var7f1a83b0)
/*  f0156e8:	3c188006 */ 	lui	$t8,%hi(var80062410)
/*  f0156ec:	8f182410 */ 	lw	$t8,%lo(var80062410)($t8)
/*  f0156f0:	c42a83b0 */ 	lwc1	$f10,%lo(var7f1a83b0)($at)
/*  f0156f4:	3c013f80 */ 	lui	$at,0x3f80
/*  f0156f8:	3c0e800a */ 	lui	$t6,%hi(var8009cc80)
/*  f0156fc:	8dcecc80 */ 	lw	$t6,%lo(var8009cc80)($t6)
/*  f015700:	44818000 */ 	mtc1	$at,$f16
/*  f015704:	3c073faa */ 	lui	$a3,0x3faa
/*  f015708:	0018c980 */ 	sll	$t9,$t8,0x6
/*  f01570c:	34e7aaab */ 	ori	$a3,$a3,0xaaab
/*  f015710:	27a500f2 */ 	addiu	$a1,$sp,0xf2
/*  f015714:	3c064270 */ 	lui	$a2,0x4270
/*  f015718:	e7a80010 */ 	swc1	$f8,0x10($sp)
/*  f01571c:	e7aa0014 */ 	swc1	$f10,0x14($sp)
/*  f015720:	032e2021 */ 	addu	$a0,$t9,$t6
/*  f015724:	0c001289 */ 	jal	func00004a24
/*  f015728:	e7b00018 */ 	swc1	$f16,0x18($sp)
/*  f01572c:	3c0fbc00 */ 	lui	$t7,0xbc00
/*  f015730:	35ef000e */ 	ori	$t7,$t7,0xe
/*  f015734:	02001025 */ 	or	$v0,$s0,$zero
/*  f015738:	ac4f0000 */ 	sw	$t7,0x0($v0)
/*  f01573c:	97b800f2 */ 	lhu	$t8,0xf2($sp)
/*  f015740:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015744:	3c190103 */ 	lui	$t9,0x103
/*  f015748:	37390040 */ 	ori	$t9,$t9,0x40
/*  f01574c:	02005825 */ 	or	$t3,$s0,$zero
/*  f015750:	ac580004 */ 	sw	$t8,0x4($v0)
/*  f015754:	ad790000 */ 	sw	$t9,0x0($t3)
/*  f015758:	3c0e8006 */ 	lui	$t6,%hi(var80062410)
/*  f01575c:	8dce2410 */ 	lw	$t6,%lo(var80062410)($t6)
/*  f015760:	3c18800a */ 	lui	$t8,%hi(var8009cc80)
/*  f015764:	8f18cc80 */ 	lw	$t8,%lo(var8009cc80)($t8)
/*  f015768:	000e7980 */ 	sll	$t7,$t6,0x6
/*  f01576c:	afab00e8 */ 	sw	$t3,0xe8($sp)
/*  f015770:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015774:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f015778:	01f82021 */ 	addu	$a0,$t7,$t8
/*  f01577c:	8fab00e8 */ 	lw	$t3,0xe8($sp)
/*  f015780:	02001825 */ 	or	$v1,$s0,$zero
/*  f015784:	3c19b600 */ 	lui	$t9,0xb600
/*  f015788:	ad620004 */ 	sw	$v0,0x4($t3)
/*  f01578c:	240effff */ 	addiu	$t6,$zero,-1
/*  f015790:	ac6e0004 */ 	sw	$t6,0x4($v1)
/*  f015794:	ac790000 */ 	sw	$t9,0x0($v1)
/*  f015798:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f01579c:	02004025 */ 	or	$t0,$s0,$zero
/*  f0157a0:	3c0fb700 */ 	lui	$t7,0xb700
/*  f0157a4:	3c180006 */ 	lui	$t8,0x6
/*  f0157a8:	37182204 */ 	ori	$t8,$t8,0x2204
/*  f0157ac:	ad0f0000 */ 	sw	$t7,0x0($t0)
/*  f0157b0:	ad180004 */ 	sw	$t8,0x4($t0)
/*  f0157b4:	3c098006 */ 	lui	$t1,%hi(var8006246c)
/*  f0157b8:	3c0c8006 */ 	lui	$t4,%hi(var80062460)
/*  f0157bc:	258c2460 */ 	addiu	$t4,$t4,%lo(var80062460)
/*  f0157c0:	2529246c */ 	addiu	$t1,$t1,%lo(var8006246c)
/*  f0157c4:	c5320000 */ 	lwc1	$f18,0x0($t1)
/*  f0157c8:	c5800000 */ 	lwc1	$f0,0x0($t4)
/*  f0157cc:	c5260004 */ 	lwc1	$f6,0x4($t1)
/*  f0157d0:	c5820004 */ 	lwc1	$f2,0x4($t4)
/*  f0157d4:	46009100 */ 	add.s	$f4,$f18,$f0
/*  f0157d8:	c52a0008 */ 	lwc1	$f10,0x8($t1)
/*  f0157dc:	c58c0008 */ 	lwc1	$f12,0x8($t4)
/*  f0157e0:	3c0a8006 */ 	lui	$t2,%hi(var80062478)
/*  f0157e4:	254a2478 */ 	addiu	$t2,$t2,%lo(var80062478)
/*  f0157e8:	3c198006 */ 	lui	$t9,%hi(var80062410)
/*  f0157ec:	46023200 */ 	add.s	$f8,$f6,$f2
/*  f0157f0:	8f392410 */ 	lw	$t9,%lo(var80062410)($t9)
/*  f0157f4:	e7a40010 */ 	swc1	$f4,0x10($sp)
/*  f0157f8:	460c5400 */ 	add.s	$f16,$f10,$f12
/*  f0157fc:	3c0f800a */ 	lui	$t7,%hi(var8009cc84)
/*  f015800:	8defcc84 */ 	lw	$t7,%lo(var8009cc84)($t7)
/*  f015804:	c5440004 */ 	lwc1	$f4,0x4($t2)
/*  f015808:	c5460008 */ 	lwc1	$f6,0x8($t2)
/*  f01580c:	c5520000 */ 	lwc1	$f18,0x0($t2)
/*  f015810:	44050000 */ 	mfc1	$a1,$f0
/*  f015814:	44061000 */ 	mfc1	$a2,$f2
/*  f015818:	44076000 */ 	mfc1	$a3,$f12
/*  f01581c:	00197180 */ 	sll	$t6,$t9,0x6
/*  f015820:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015824:	e7b00018 */ 	swc1	$f16,0x18($sp)
/*  f015828:	e7a80014 */ 	swc1	$f8,0x14($sp)
/*  f01582c:	01cf2021 */ 	addu	$a0,$t6,$t7
/*  f015830:	e7a40020 */ 	swc1	$f4,0x20($sp)
/*  f015834:	e7a60024 */ 	swc1	$f6,0x24($sp)
/*  f015838:	0c0010a9 */ 	jal	func000042a4
/*  f01583c:	e7b2001c */ 	swc1	$f18,0x1c($sp)
/*  f015840:	3c180102 */ 	lui	$t8,0x102
/*  f015844:	37180040 */ 	ori	$t8,$t8,0x40
/*  f015848:	02001825 */ 	or	$v1,$s0,$zero
/*  f01584c:	ac780000 */ 	sw	$t8,0x0($v1)
/*  f015850:	3c198006 */ 	lui	$t9,%hi(var80062410)
/*  f015854:	8f392410 */ 	lw	$t9,%lo(var80062410)($t9)
/*  f015858:	3c0f800a */ 	lui	$t7,%hi(var8009cc84)
/*  f01585c:	8defcc84 */ 	lw	$t7,%lo(var8009cc84)($t7)
/*  f015860:	00197180 */ 	sll	$t6,$t9,0x6
/*  f015864:	afa300dc */ 	sw	$v1,0xdc($sp)
/*  f015868:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f01586c:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f015870:	01cf2021 */ 	addu	$a0,$t6,$t7
/*  f015874:	8fa300dc */ 	lw	$v1,0xdc($sp)
/*  f015878:	3c180100 */ 	lui	$t8,0x100
/*  f01587c:	37180040 */ 	ori	$t8,$t8,0x40
/*  f015880:	02005025 */ 	or	$t2,$s0,$zero
/*  f015884:	ac620004 */ 	sw	$v0,0x4($v1)
/*  f015888:	ad580000 */ 	sw	$t8,0x0($t2)
/*  f01588c:	3c198006 */ 	lui	$t9,%hi(var80062410)
/*  f015890:	8f392410 */ 	lw	$t9,%lo(var80062410)($t9)
/*  f015894:	3c0f800a */ 	lui	$t7,%hi(var8009cc88)
/*  f015898:	8defcc88 */ 	lw	$t7,%lo(var8009cc88)($t7)
/*  f01589c:	00197180 */ 	sll	$t6,$t9,0x6
/*  f0158a0:	afaa00d8 */ 	sw	$t2,0xd8($sp)
/*  f0158a4:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f0158a8:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f0158ac:	01cf2021 */ 	addu	$a0,$t6,$t7
/*  f0158b0:	8faa00d8 */ 	lw	$t2,0xd8($sp)
/*  f0158b4:	02003825 */ 	or	$a3,$s0,$zero
/*  f0158b8:	3c18bc00 */ 	lui	$t8,0xbc00
/*  f0158bc:	3c198000 */ 	lui	$t9,0x8000
/*  f0158c0:	ad420004 */ 	sw	$v0,0x4($t2)
/*  f0158c4:	37390040 */ 	ori	$t9,$t9,0x40
/*  f0158c8:	37180002 */ 	ori	$t8,$t8,0x2
/*  f0158cc:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f0158d0:	02004025 */ 	or	$t0,$s0,$zero
/*  f0158d4:	acf80000 */ 	sw	$t8,0x0($a3)
/*  f0158d8:	acf90004 */ 	sw	$t9,0x4($a3)
/*  f0158dc:	3c0e0386 */ 	lui	$t6,0x386
/*  f0158e0:	3c0f8006 */ 	lui	$t7,%hi(var80062450)
/*  f0158e4:	25ef2450 */ 	addiu	$t7,$t7,%lo(var80062450)
/*  f0158e8:	35ce0010 */ 	ori	$t6,$t6,0x10
/*  f0158ec:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f0158f0:	3c038006 */ 	lui	$v1,%hi(var80062448)
/*  f0158f4:	02004825 */ 	or	$t1,$s0,$zero
/*  f0158f8:	ad0e0000 */ 	sw	$t6,0x0($t0)
/*  f0158fc:	ad0f0004 */ 	sw	$t7,0x4($t0)
/*  f015900:	3c180388 */ 	lui	$t8,0x388
/*  f015904:	24632448 */ 	addiu	$v1,$v1,%lo(var80062448)
/*  f015908:	37180010 */ 	ori	$t8,$t8,0x10
/*  f01590c:	3c198006 */ 	lui	$t9,%hi(var80062488)
/*  f015910:	ad380000 */ 	sw	$t8,0x0($t1)
/*  f015914:	ad230004 */ 	sw	$v1,0x4($t1)
/*  f015918:	27392488 */ 	addiu	$t9,$t9,%lo(var80062488)
/*  f01591c:	8f210000 */ 	lw	$at,0x0($t9)
/*  f015920:	27a400c0 */ 	addiu	$a0,$sp,0xc0
/*  f015924:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015928:	ac810000 */ 	sw	$at,0x0($a0)
/*  f01592c:	8f2f0004 */ 	lw	$t7,0x4($t9)
/*  f015930:	27a500c4 */ 	addiu	$a1,$sp,0xc4
/*  f015934:	27a600c8 */ 	addiu	$a2,$sp,0xc8
/*  f015938:	ac8f0004 */ 	sw	$t7,0x4($a0)
/*  f01593c:	8f210008 */ 	lw	$at,0x8($t9)
/*  f015940:	0c0011e4 */ 	jal	scaleTo1
/*  f015944:	ac810008 */ 	sw	$at,0x8($a0)
/*  f015948:	3c0142fe */ 	lui	$at,0x42fe
/*  f01594c:	44810000 */ 	mtc1	$at,$f0
/*  f015950:	c7a800c0 */ 	lwc1	$f8,0xc0($sp)
/*  f015954:	3c038006 */ 	lui	$v1,%hi(var80062448)
/*  f015958:	240e0001 */ 	addiu	$t6,$zero,0x1
/*  f01595c:	46004282 */ 	mul.s	$f10,$f8,$f0
/*  f015960:	24632448 */ 	addiu	$v1,$v1,%lo(var80062448)
/*  f015964:	93a20113 */ 	lbu	$v0,0x113($sp)
/*  f015968:	3c07e700 */ 	lui	$a3,0xe700
/*  f01596c:	a0600000 */ 	sb	$zero,0x0($v1)
/*  f015970:	a0600001 */ 	sb	$zero,0x1($v1)
/*  f015974:	a0600002 */ 	sb	$zero,0x2($v1)
/*  f015978:	4458f800 */ 	cfc1	$t8,$31
/*  f01597c:	44cef800 */ 	ctc1	$t6,$31
/*  f015980:	a0600004 */ 	sb	$zero,0x4($v1)
/*  f015984:	a0600005 */ 	sb	$zero,0x5($v1)
/*  f015988:	46005424 */ 	cvt.w.s	$f16,$f10
/*  f01598c:	a0600006 */ 	sb	$zero,0x6($v1)
/*  f015990:	a0620008 */ 	sb	$v0,0x8($v1)
/*  f015994:	a0620009 */ 	sb	$v0,0x9($v1)
/*  f015998:	444ef800 */ 	cfc1	$t6,$31
/*  f01599c:	a062000a */ 	sb	$v0,0xa($v1)
/*  f0159a0:	a062000c */ 	sb	$v0,0xc($v1)
/*  f0159a4:	31ce0078 */ 	andi	$t6,$t6,0x78
/*  f0159a8:	a062000d */ 	sb	$v0,0xd($v1)
/*  f0159ac:	11c00013 */ 	beqz	$t6,.L0f0159fc
/*  f0159b0:	a062000e */ 	sb	$v0,0xe($v1)
/*  f0159b4:	3c014f00 */ 	lui	$at,0x4f00
/*  f0159b8:	44818000 */ 	mtc1	$at,$f16
/*  f0159bc:	240e0001 */ 	addiu	$t6,$zero,0x1
/*  f0159c0:	46105401 */ 	sub.s	$f16,$f10,$f16
/*  f0159c4:	44cef800 */ 	ctc1	$t6,$31
/*  f0159c8:	00000000 */ 	nop
/*  f0159cc:	46008424 */ 	cvt.w.s	$f16,$f16
/*  f0159d0:	444ef800 */ 	cfc1	$t6,$31
/*  f0159d4:	00000000 */ 	nop
/*  f0159d8:	31ce0078 */ 	andi	$t6,$t6,0x78
/*  f0159dc:	15c00005 */ 	bnez	$t6,.L0f0159f4
/*  f0159e0:	00000000 */ 	nop
/*  f0159e4:	440e8000 */ 	mfc1	$t6,$f16
/*  f0159e8:	3c018000 */ 	lui	$at,0x8000
/*  f0159ec:	10000007 */ 	b	.L0f015a0c
/*  f0159f0:	01c17025 */ 	or	$t6,$t6,$at
.L0f0159f4:
/*  f0159f4:	10000005 */ 	b	.L0f015a0c
/*  f0159f8:	240effff */ 	addiu	$t6,$zero,-1
.L0f0159fc:
/*  f0159fc:	440e8000 */ 	mfc1	$t6,$f16
/*  f015a00:	00000000 */ 	nop
/*  f015a04:	05c0fffb */ 	bltz	$t6,.L0f0159f4
/*  f015a08:	00000000 */ 	nop
.L0f015a0c:
/*  f015a0c:	44d8f800 */ 	ctc1	$t8,$31
/*  f015a10:	c7b200c4 */ 	lwc1	$f18,0xc4($sp)
/*  f015a14:	240f0001 */ 	addiu	$t7,$zero,0x1
/*  f015a18:	02002025 */ 	or	$a0,$s0,$zero
/*  f015a1c:	46009102 */ 	mul.s	$f4,$f18,$f0
/*  f015a20:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015a24:	02002825 */ 	or	$a1,$s0,$zero
/*  f015a28:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015a2c:	02003025 */ 	or	$a2,$s0,$zero
/*  f015a30:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015a34:	02004025 */ 	or	$t0,$s0,$zero
/*  f015a38:	4459f800 */ 	cfc1	$t9,$31
/*  f015a3c:	44cff800 */ 	ctc1	$t7,$31
/*  f015a40:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015a44:	02001025 */ 	or	$v0,$s0,$zero
/*  f015a48:	460021a4 */ 	cvt.w.s	$f6,$f4
/*  f015a4c:	3c0b0701 */ 	lui	$t3,0x701
/*  f015a50:	a06e0010 */ 	sb	$t6,0x10($v1)
/*  f015a54:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015a58:	444ff800 */ 	cfc1	$t7,$31
/*  f015a5c:	3c09fd10 */ 	lui	$t1,0xfd10
/*  f015a60:	356b4050 */ 	ori	$t3,$t3,0x4050
/*  f015a64:	31ef0078 */ 	andi	$t7,$t7,0x78
/*  f015a68:	11e00013 */ 	beqz	$t7,.L0f015ab8
/*  f015a6c:	3c0af510 */ 	lui	$t2,0xf510
/*  f015a70:	3c014f00 */ 	lui	$at,0x4f00
/*  f015a74:	44813000 */ 	mtc1	$at,$f6
/*  f015a78:	240f0001 */ 	addiu	$t7,$zero,0x1
/*  f015a7c:	46062181 */ 	sub.s	$f6,$f4,$f6
/*  f015a80:	44cff800 */ 	ctc1	$t7,$31
/*  f015a84:	00000000 */ 	nop
/*  f015a88:	460031a4 */ 	cvt.w.s	$f6,$f6
/*  f015a8c:	444ff800 */ 	cfc1	$t7,$31
/*  f015a90:	00000000 */ 	nop
/*  f015a94:	31ef0078 */ 	andi	$t7,$t7,0x78
/*  f015a98:	15e00005 */ 	bnez	$t7,.L0f015ab0
/*  f015a9c:	00000000 */ 	nop
/*  f015aa0:	440f3000 */ 	mfc1	$t7,$f6
/*  f015aa4:	3c018000 */ 	lui	$at,0x8000
/*  f015aa8:	10000007 */ 	b	.L0f015ac8
/*  f015aac:	01e17825 */ 	or	$t7,$t7,$at
.L0f015ab0:
/*  f015ab0:	10000005 */ 	b	.L0f015ac8
/*  f015ab4:	240fffff */ 	addiu	$t7,$zero,-1
.L0f015ab8:
/*  f015ab8:	440f3000 */ 	mfc1	$t7,$f6
/*  f015abc:	00000000 */ 	nop
/*  f015ac0:	05e0fffb */ 	bltz	$t7,.L0f015ab0
/*  f015ac4:	00000000 */ 	nop
.L0f015ac8:
/*  f015ac8:	44d9f800 */ 	ctc1	$t9,$31
/*  f015acc:	c7a800c8 */ 	lwc1	$f8,0xc8($sp)
/*  f015ad0:	240e0001 */ 	addiu	$t6,$zero,0x1
/*  f015ad4:	a06f0011 */ 	sb	$t7,0x11($v1)
/*  f015ad8:	46004282 */ 	mul.s	$f10,$f8,$f0
/*  f015adc:	3c19fc12 */ 	lui	$t9,0xfc12
/*  f015ae0:	3c1f073f */ 	lui	$ra,0x73f
/*  f015ae4:	3c014f00 */ 	lui	$at,0x4f00
/*  f015ae8:	37397e24 */ 	ori	$t9,$t9,0x7e24
/*  f015aec:	240ff9fc */ 	addiu	$t7,$zero,-1540
/*  f015af0:	37fff100 */ 	ori	$ra,$ra,0xf100
/*  f015af4:	4458f800 */ 	cfc1	$t8,$31
/*  f015af8:	44cef800 */ 	ctc1	$t6,$31
/*  f015afc:	3c0ce600 */ 	lui	$t4,0xe600
/*  f015b00:	3c0df300 */ 	lui	$t5,0xf300
/*  f015b04:	46005424 */ 	cvt.w.s	$f16,$f10
/*  f015b08:	444ef800 */ 	cfc1	$t6,$31
/*  f015b0c:	00000000 */ 	nop
/*  f015b10:	31ce0078 */ 	andi	$t6,$t6,0x78
/*  f015b14:	51c00013 */ 	beqzl	$t6,.L0f015b64
/*  f015b18:	440e8000 */ 	mfc1	$t6,$f16
/*  f015b1c:	44818000 */ 	mtc1	$at,$f16
/*  f015b20:	240e0001 */ 	addiu	$t6,$zero,0x1
/*  f015b24:	46105401 */ 	sub.s	$f16,$f10,$f16
/*  f015b28:	44cef800 */ 	ctc1	$t6,$31
/*  f015b2c:	00000000 */ 	nop
/*  f015b30:	46008424 */ 	cvt.w.s	$f16,$f16
/*  f015b34:	444ef800 */ 	cfc1	$t6,$31
/*  f015b38:	00000000 */ 	nop
/*  f015b3c:	31ce0078 */ 	andi	$t6,$t6,0x78
/*  f015b40:	15c00005 */ 	bnez	$t6,.L0f015b58
/*  f015b44:	00000000 */ 	nop
/*  f015b48:	440e8000 */ 	mfc1	$t6,$f16
/*  f015b4c:	3c018000 */ 	lui	$at,0x8000
/*  f015b50:	10000007 */ 	b	.L0f015b70
/*  f015b54:	01c17025 */ 	or	$t6,$t6,$at
.L0f015b58:
/*  f015b58:	10000005 */ 	b	.L0f015b70
/*  f015b5c:	240effff */ 	addiu	$t6,$zero,-1
/*  f015b60:	440e8000 */ 	mfc1	$t6,$f16
.L0f015b64:
/*  f015b64:	00000000 */ 	nop
/*  f015b68:	05c0fffb */ 	bltz	$t6,.L0f015b58
/*  f015b6c:	00000000 */ 	nop
.L0f015b70:
/*  f015b70:	a06e0012 */ 	sb	$t6,0x12($v1)
/*  f015b74:	ac800004 */ 	sw	$zero,0x4($a0)
/*  f015b78:	ac870000 */ 	sw	$a3,0x0($a0)
/*  f015b7c:	aca00004 */ 	sw	$zero,0x4($a1)
/*  f015b80:	aca70000 */ 	sw	$a3,0x0($a1)
/*  f015b84:	44d8f800 */ 	ctc1	$t8,$31
/*  f015b88:	acd90000 */ 	sw	$t9,0x0($a2)
/*  f015b8c:	accf0004 */ 	sw	$t7,0x4($a2)
/*  f015b90:	3c18ba00 */ 	lui	$t8,0xba00
/*  f015b94:	37181301 */ 	ori	$t8,$t8,0x1301
/*  f015b98:	3c0e0008 */ 	lui	$t6,0x8
/*  f015b9c:	ad0e0004 */ 	sw	$t6,0x4($t0)
/*  f015ba0:	ad180000 */ 	sw	$t8,0x0($t0)
/*  f015ba4:	3c19ba00 */ 	lui	$t9,0xba00
/*  f015ba8:	37391102 */ 	ori	$t9,$t9,0x1102
/*  f015bac:	ac590000 */ 	sw	$t9,0x0($v0)
/*  f015bb0:	02001825 */ 	or	$v1,$s0,$zero
/*  f015bb4:	ac400004 */ 	sw	$zero,0x4($v0)
/*  f015bb8:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015bbc:	3c0fba00 */ 	lui	$t7,0xba00
/*  f015bc0:	35ef1001 */ 	ori	$t7,$t7,0x1001
/*  f015bc4:	02002025 */ 	or	$a0,$s0,$zero
/*  f015bc8:	ac6f0000 */ 	sw	$t7,0x0($v1)
/*  f015bcc:	ac600004 */ 	sw	$zero,0x4($v1)
/*  f015bd0:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015bd4:	3c18ba00 */ 	lui	$t8,0xba00
/*  f015bd8:	37180e02 */ 	ori	$t8,$t8,0xe02
/*  f015bdc:	02002825 */ 	or	$a1,$s0,$zero
/*  f015be0:	ac980000 */ 	sw	$t8,0x0($a0)
/*  f015be4:	ac800004 */ 	sw	$zero,0x4($a0)
/*  f015be8:	3c0eba00 */ 	lui	$t6,0xba00
/*  f015bec:	35ce0c02 */ 	ori	$t6,$t6,0xc02
/*  f015bf0:	24192000 */ 	addiu	$t9,$zero,0x2000
/*  f015bf4:	acb90004 */ 	sw	$t9,0x4($a1)
/*  f015bf8:	acae0000 */ 	sw	$t6,0x0($a1)
/*  f015bfc:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c00:	02003025 */ 	or	$a2,$s0,$zero
/*  f015c04:	3c0fba00 */ 	lui	$t7,0xba00
/*  f015c08:	35ef0903 */ 	ori	$t7,$t7,0x903
/*  f015c0c:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c10:	accf0000 */ 	sw	$t7,0x0($a2)
/*  f015c14:	24180c00 */ 	addiu	$t8,$zero,0xc00
/*  f015c18:	02001025 */ 	or	$v0,$s0,$zero
/*  f015c1c:	acd80004 */ 	sw	$t8,0x4($a2)
/*  f015c20:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c24:	02001825 */ 	or	$v1,$s0,$zero
/*  f015c28:	ac400004 */ 	sw	$zero,0x4($v0)
/*  f015c2c:	ac470000 */ 	sw	$a3,0x0($v0)
/*  f015c30:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c34:	02002025 */ 	or	$a0,$s0,$zero
/*  f015c38:	ac600004 */ 	sw	$zero,0x4($v1)
/*  f015c3c:	ac670000 */ 	sw	$a3,0x0($v1)
/*  f015c40:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c44:	3c0ebb00 */ 	lui	$t6,0xbb00
/*  f015c48:	3c190800 */ 	lui	$t9,0x800
/*  f015c4c:	37390800 */ 	ori	$t9,$t9,0x800
/*  f015c50:	35ce0001 */ 	ori	$t6,$t6,0x1
/*  f015c54:	02002825 */ 	or	$a1,$s0,$zero
/*  f015c58:	ac8e0000 */ 	sw	$t6,0x0($a0)
/*  f015c5c:	ac990004 */ 	sw	$t9,0x4($a0)
/*  f015c60:	3c0f0200 */ 	lui	$t7,0x200
/*  f015c64:	25ef4dc8 */ 	addiu	$t7,$t7,0x4dc8
/*  f015c68:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c6c:	02004025 */ 	or	$t0,$s0,$zero
/*  f015c70:	acaf0004 */ 	sw	$t7,0x4($a1)
/*  f015c74:	aca90000 */ 	sw	$t1,0x0($a1)
/*  f015c78:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c7c:	02001025 */ 	or	$v0,$s0,$zero
/*  f015c80:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c84:	ad0b0004 */ 	sw	$t3,0x4($t0)
/*  f015c88:	ad0a0000 */ 	sw	$t2,0x0($t0)
/*  f015c8c:	02001825 */ 	or	$v1,$s0,$zero
/*  f015c90:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015c94:	ac400004 */ 	sw	$zero,0x4($v0)
/*  f015c98:	ac4c0000 */ 	sw	$t4,0x0($v0)
/*  f015c9c:	02002025 */ 	or	$a0,$s0,$zero
/*  f015ca0:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015ca4:	ac7f0004 */ 	sw	$ra,0x4($v1)
/*  f015ca8:	ac6d0000 */ 	sw	$t5,0x0($v1)
/*  f015cac:	02002825 */ 	or	$a1,$s0,$zero
/*  f015cb0:	ac800004 */ 	sw	$zero,0x4($a0)
/*  f015cb4:	ac870000 */ 	sw	$a3,0x0($a0)
/*  f015cb8:	3c0e0001 */ 	lui	$t6,0x1
/*  f015cbc:	3c18f510 */ 	lui	$t8,0xf510
/*  f015cc0:	37181000 */ 	ori	$t8,$t8,0x1000
/*  f015cc4:	35ce4050 */ 	ori	$t6,$t6,0x4050
/*  f015cc8:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015ccc:	02003025 */ 	or	$a2,$s0,$zero
/*  f015cd0:	acae0004 */ 	sw	$t6,0x4($a1)
/*  f015cd4:	acb80000 */ 	sw	$t8,0x0($a1)
/*  f015cd8:	3c0f0007 */ 	lui	$t7,0x7
/*  f015cdc:	35efc07c */ 	ori	$t7,$t7,0xc07c
/*  f015ce0:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015ce4:	3c19f200 */ 	lui	$t9,0xf200
/*  f015ce8:	acd90000 */ 	sw	$t9,0x0($a2)
/*  f015cec:	accf0004 */ 	sw	$t7,0x4($a2)
/*  f015cf0:	02001025 */ 	or	$v0,$s0,$zero
/*  f015cf4:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015cf8:	3c180600 */ 	lui	$t8,0x600
/*  f015cfc:	3c0e0200 */ 	lui	$t6,0x200
/*  f015d00:	25ce43f8 */ 	addiu	$t6,$t6,0x43f8
/*  f015d04:	ac580000 */ 	sw	$t8,0x0($v0)
/*  f015d08:	02001825 */ 	or	$v1,$s0,$zero
/*  f015d0c:	ac4e0004 */ 	sw	$t6,0x4($v0)
/*  f015d10:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015d14:	3c0f0200 */ 	lui	$t7,0x200
/*  f015d18:	25ef4468 */ 	addiu	$t7,$t7,0x4468
/*  f015d1c:	3c190600 */ 	lui	$t9,0x600
/*  f015d20:	02002025 */ 	or	$a0,$s0,$zero
/*  f015d24:	ac790000 */ 	sw	$t9,0x0($v1)
/*  f015d28:	ac6f0004 */ 	sw	$t7,0x4($v1)
/*  f015d2c:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015d30:	3c180200 */ 	lui	$t8,0x200
/*  f015d34:	27185dd0 */ 	addiu	$t8,$t8,0x5dd0
/*  f015d38:	02002825 */ 	or	$a1,$s0,$zero
/*  f015d3c:	ac980004 */ 	sw	$t8,0x4($a0)
/*  f015d40:	ac890000 */ 	sw	$t1,0x0($a0)
/*  f015d44:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015d48:	02004025 */ 	or	$t0,$s0,$zero
/*  f015d4c:	acab0004 */ 	sw	$t3,0x4($a1)
/*  f015d50:	acaa0000 */ 	sw	$t2,0x0($a1)
/*  f015d54:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015d58:	02001025 */ 	or	$v0,$s0,$zero
/*  f015d5c:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015d60:	ad000004 */ 	sw	$zero,0x4($t0)
/*  f015d64:	ad0c0000 */ 	sw	$t4,0x0($t0)
/*  f015d68:	02001825 */ 	or	$v1,$s0,$zero
/*  f015d6c:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015d70:	ac5f0004 */ 	sw	$ra,0x4($v0)
/*  f015d74:	ac4d0000 */ 	sw	$t5,0x0($v0)
/*  f015d78:	02002025 */ 	or	$a0,$s0,$zero
/*  f015d7c:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015d80:	ac600004 */ 	sw	$zero,0x4($v1)
/*  f015d84:	ac670000 */ 	sw	$a3,0x0($v1)
/*  f015d88:	3c190001 */ 	lui	$t9,0x1
/*  f015d8c:	3c0ef510 */ 	lui	$t6,0xf510
/*  f015d90:	35ce1000 */ 	ori	$t6,$t6,0x1000
/*  f015d94:	37394050 */ 	ori	$t9,$t9,0x4050
/*  f015d98:	02002825 */ 	or	$a1,$s0,$zero
/*  f015d9c:	ac990004 */ 	sw	$t9,0x4($a0)
/*  f015da0:	ac8e0000 */ 	sw	$t6,0x0($a0)
/*  f015da4:	3c180007 */ 	lui	$t8,0x7
/*  f015da8:	3718c07c */ 	ori	$t8,$t8,0xc07c
/*  f015dac:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015db0:	3c0ff200 */ 	lui	$t7,0xf200
/*  f015db4:	acaf0000 */ 	sw	$t7,0x0($a1)
/*  f015db8:	acb80004 */ 	sw	$t8,0x4($a1)
/*  f015dbc:	02003025 */ 	or	$a2,$s0,$zero
/*  f015dc0:	3c190200 */ 	lui	$t9,0x200
/*  f015dc4:	27394700 */ 	addiu	$t9,$t9,0x4700
/*  f015dc8:	26100008 */ 	addiu	$s0,$s0,0x8
/*  f015dcc:	3c0e0600 */ 	lui	$t6,0x600
/*  f015dd0:	acce0000 */ 	sw	$t6,0x0($a2)
/*  f015dd4:	acd90004 */ 	sw	$t9,0x4($a2)
/*  f015dd8:	02001825 */ 	or	$v1,$s0,$zero
/*  f015ddc:	3c180200 */ 	lui	$t8,0x200
/*  f015de0:	27184d28 */ 	addiu	$t8,$t8,0x4d28
/*  f015de4:	3c0f0600 */ 	lui	$t7,0x600
/*  f015de8:	ac6f0000 */ 	sw	$t7,0x0($v1)
/*  f015dec:	ac780004 */ 	sw	$t8,0x4($v1)
/*  f015df0:	8fbf0034 */ 	lw	$ra,0x34($sp)
/*  f015df4:	26020008 */ 	addiu	$v0,$s0,0x8
/*  f015df8:	8fb00030 */ 	lw	$s0,0x30($sp)
/*  f015dfc:	03e00008 */ 	jr	$ra
/*  f015e00:	27bd0108 */ 	addiu	$sp,$sp,0x108
/*  f015e04:	3c018006 */ 	lui	$at,%hi(var80062414)
/*  f015e08:	a0202414 */ 	sb	$zero,%lo(var80062414)($at)
/*  f015e0c:	3c01445c */ 	lui	$at,0x445c
/*  f015e10:	44812000 */ 	mtc1	$at,$f4
/*  f015e14:	3c01800a */ 	lui	$at,%hi(var8009cc8c)
/*  f015e18:	3c0e0200 */ 	lui	$t6,0x200
/*  f015e1c:	e424cc8c */ 	swc1	$f4,%lo(var8009cc8c)($at)
/*  f015e20:	3c01c220 */ 	lui	$at,0xc220
/*  f015e24:	44813000 */ 	mtc1	$at,$f6
/*  f015e28:	3c0f0200 */ 	lui	$t7,0x200
/*  f015e2c:	3c018006 */ 	lui	$at,%hi(var80062484)
/*  f015e30:	25ef0000 */ 	addiu	$t7,$t7,0x0
/*  f015e34:	25ce65d0 */ 	addiu	$t6,$t6,0x65d0
/*  f015e38:	27bdffe8 */ 	addiu	$sp,$sp,-24
/*  f015e3c:	01cf3023 */ 	subu	$a2,$t6,$t7
/*  f015e40:	e4262484 */ 	swc1	$f6,%lo(var80062484)($at)
/*  f015e44:	afa5001c */ 	sw	$a1,0x1c($sp)
/*  f015e48:	3c02800a */ 	lui	$v0,%hi(var8009cc90)
/*  f015e4c:	3c018006 */ 	lui	$at,%hi(var80062498)
/*  f015e50:	24c6003f */ 	addiu	$a2,$a2,0x3f
/*  f015e54:	2442cc90 */ 	addiu	$v0,$v0,%lo(var8009cc90)
/*  f015e58:	afbf0014 */ 	sw	$ra,0x14($sp)
/*  f015e5c:	ac202498 */ 	sw	$zero,%lo(var80062498)($at)
/*  f015e60:	34d8003f */ 	ori	$t8,$a2,0x3f
/*  f015e64:	3c05007f */ 	lui	$a1,%hi(_unknown2SegmentRomStart)
/*  f015e68:	ac440000 */ 	sw	$a0,0x0($v0)
/*  f015e6c:	24a5bdc0 */ 	addiu	$a1,$a1,%lo(_unknown2SegmentRomStart)
/*  f015e70:	0c003504 */ 	jal	func0000d410
/*  f015e74:	3b06003f */ 	xori	$a2,$t8,0x3f
/*  f015e78:	8fbf0014 */ 	lw	$ra,0x14($sp)
/*  f015e7c:	27bd0018 */ 	addiu	$sp,$sp,0x18
/*  f015e80:	03e00008 */ 	jr	$ra
/*  f015e84:	00000000 */ 	nop
/*  f015e88:	3c028006 */ 	lui	$v0,%hi(var80062410)
/*  f015e8c:	24422410 */ 	addiu	$v0,$v0,%lo(var80062410)
/*  f015e90:	8c4e0000 */ 	lw	$t6,0x0($v0)
/*  f015e94:	27bdffc8 */ 	addiu	$sp,$sp,-56
/*  f015e98:	240f0001 */ 	addiu	$t7,$zero,0x1
/*  f015e9c:	afb00020 */ 	sw	$s0,0x20($sp)
/*  f015ea0:	3c19bc00 */ 	lui	$t9,0xbc00
/*  f015ea4:	01eec023 */ 	subu	$t8,$t7,$t6
/*  f015ea8:	afbf0024 */ 	sw	$ra,0x24($sp)
/*  f015eac:	ac580000 */ 	sw	$t8,0x0($v0)
/*  f015eb0:	37390806 */ 	ori	$t9,$t9,0x806
/*  f015eb4:	ac990000 */ 	sw	$t9,0x0($a0)
/*  f015eb8:	24900008 */ 	addiu	$s0,$a0,0x8
/*  f015ebc:	00801825 */ 	or	$v1,$a0,$zero
/*  f015ec0:	3c04800a */ 	lui	$a0,%hi(var8009cc90)
/*  f015ec4:	8c84cc90 */ 	lw	$a0,%lo(var8009cc90)($a0)
/*  f015ec8:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f015ecc:	afa30034 */ 	sw	$v1,0x34($sp)
/*  f015ed0:	8fa30034 */ 	lw	$v1,0x34($sp)
/*  f015ed4:	3c0a8006 */ 	lui	$t2,%hi(var80062414)
/*  f015ed8:	254a2414 */ 	addiu	$t2,$t2,%lo(var80062414)
/*  f015edc:	ac620004 */ 	sw	$v0,0x4($v1)
/*  f015ee0:	91440000 */ 	lbu	$a0,0x0($t2)
/*  f015ee4:	3c098006 */ 	lui	$t1,%hi(var80062498)
/*  f015ee8:	25292498 */ 	addiu	$t1,$t1,%lo(var80062498)
/*  f015eec:	10800003 */ 	beqz	$a0,.L0f015efc
/*  f015ef0:	3c0c800a */ 	lui	$t4,%hi(g_Vars)
/*  f015ef4:	24010001 */ 	addiu	$at,$zero,0x1
/*  f015ef8:	14810076 */ 	bne	$a0,$at,.L0f0160d4
.L0f015efc:
/*  f015efc:	258c9fc0 */ 	addiu	$t4,$t4,%lo(g_Vars)
/*  f015f00:	8d8f0034 */ 	lw	$t7,0x34($t4)
/*  f015f04:	8d2d0000 */ 	lw	$t5,0x0($t1)
/*  f015f08:	240b00ff */ 	addiu	$t3,$zero,0xff
/*  f015f0c:	24040118 */ 	addiu	$a0,$zero,0x118
/*  f015f10:	01af7021 */ 	addu	$t6,$t5,$t7
/*  f015f14:	01cb0019 */ 	multu	$t6,$t3
/*  f015f18:	01c0c025 */ 	or	$t8,$t6,$zero
/*  f015f1c:	44801000 */ 	mtc1	$zero,$f2
/*  f015f20:	ad2e0000 */ 	sw	$t6,0x0($t1)
/*  f015f24:	3c18800a */ 	lui	$t8,%hi(var8009cc88)
/*  f015f28:	3c0f8006 */ 	lui	$t7,%hi(var80062410)
/*  f015f2c:	00001812 */ 	mflo	$v1
/*  f015f30:	00000000 */ 	nop
/*  f015f34:	00000000 */ 	nop
/*  f015f38:	0064001a */ 	div	$zero,$v1,$a0
/*  f015f3c:	14800002 */ 	bnez	$a0,.L0f015f48
/*  f015f40:	00000000 */ 	nop
/*  f015f44:	0007000d */ 	break	0x7
.L0f015f48:
/*  f015f48:	2401ffff */ 	addiu	$at,$zero,-1
/*  f015f4c:	14810004 */ 	bne	$a0,$at,.L0f015f60
/*  f015f50:	3c018000 */ 	lui	$at,0x8000
/*  f015f54:	14610002 */ 	bne	$v1,$at,.L0f015f60
/*  f015f58:	00000000 */ 	nop
/*  f015f5c:	0006000d */ 	break	0x6
.L0f015f60:
/*  f015f60:	00004012 */ 	mflo	$t0
/*  f015f64:	29010100 */ 	slti	$at,$t0,0x100
/*  f015f68:	14200002 */ 	bnez	$at,.L0f015f74
/*  f015f6c:	00000000 */ 	nop
/*  f015f70:	01604025 */ 	or	$t0,$t3,$zero
.L0f015f74:
/*  f015f74:	05010002 */ 	bgez	$t0,.L0f015f80
/*  f015f78:	3c01fffd */ 	lui	$at,0xfffd
/*  f015f7c:	00004025 */ 	or	$t0,$zero,$zero
.L0f015f80:
/*  f015f80:	34218280 */ 	ori	$at,$at,0x8280
/*  f015f84:	0061c821 */ 	addu	$t9,$v1,$at
/*  f015f88:	0324001a */ 	div	$zero,$t9,$a0
/*  f015f8c:	14800002 */ 	bnez	$a0,.L0f015f98
/*  f015f90:	00000000 */ 	nop
/*  f015f94:	0007000d */ 	break	0x7
.L0f015f98:
/*  f015f98:	2401ffff */ 	addiu	$at,$zero,-1
/*  f015f9c:	14810004 */ 	bne	$a0,$at,.L0f015fb0
/*  f015fa0:	3c018000 */ 	lui	$at,0x8000
/*  f015fa4:	17210002 */ 	bne	$t9,$at,.L0f015fb0
/*  f015fa8:	00000000 */ 	nop
/*  f015fac:	0006000d */ 	break	0x6
.L0f015fb0:
/*  f015fb0:	00006812 */ 	mflo	$t5
/*  f015fb4:	016d1023 */ 	subu	$v0,$t3,$t5
/*  f015fb8:	28410100 */ 	slti	$at,$v0,0x100
/*  f015fbc:	14200002 */ 	bnez	$at,.L0f015fc8
/*  f015fc0:	00000000 */ 	nop
/*  f015fc4:	01601025 */ 	or	$v0,$t3,$zero
.L0f015fc8:
/*  f015fc8:	04430003 */ 	bgezl	$v0,.L0f015fd8
/*  f015fcc:	c5800044 */ 	lwc1	$f0,0x44($t4)
/*  f015fd0:	00001025 */ 	or	$v0,$zero,$zero
/*  f015fd4:	c5800044 */ 	lwc1	$f0,0x44($t4)
.L0f015fd8:
/*  f015fd8:	3c038006 */ 	lui	$v1,%hi(var80062484)
/*  f015fdc:	24632484 */ 	addiu	$v1,$v1,%lo(var80062484)
/*  f015fe0:	46000180 */ 	add.s	$f6,$f0,$f0
/*  f015fe4:	c4640000 */ 	lwc1	$f4,0x0($v1)
/*  f015fe8:	44061000 */ 	mfc1	$a2,$f2
/*  f015fec:	3c073f80 */ 	lui	$a3,0x3f80
/*  f015ff0:	46062200 */ 	add.s	$f8,$f4,$f6
/*  f015ff4:	e7a20010 */ 	swc1	$f2,0x10($sp)
/*  f015ff8:	afa2002c */ 	sw	$v0,0x2c($sp)
/*  f015ffc:	afa80030 */ 	sw	$t0,0x30($sp)
/*  f016000:	e4680000 */ 	swc1	$f8,0x0($v1)
/*  f016004:	8def2410 */ 	lw	$t7,%lo(var80062410)($t7)
/*  f016008:	8f18cc88 */ 	lw	$t8,%lo(var8009cc88)($t8)
/*  f01600c:	8c650000 */ 	lw	$a1,0x0($v1)
/*  f016010:	000f7180 */ 	sll	$t6,$t7,0x6
/*  f016014:	0c001301 */ 	jal	func00004c04
/*  f016018:	01d82021 */ 	addu	$a0,$t6,$t8
/*  f01601c:	8fa2002c */ 	lw	$v0,0x2c($sp)
/*  f016020:	8fa80030 */ 	lw	$t0,0x30($sp)
/*  f016024:	240b00ff */ 	addiu	$t3,$zero,0xff
/*  f016028:	3c01800a */ 	lui	$at,%hi(var8009cc8c)
/*  f01602c:	01020019 */ 	multu	$t0,$v0
/*  f016030:	c42acc8c */ 	lwc1	$f10,%lo(var8009cc8c)($at)
/*  f016034:	3c098006 */ 	lui	$t1,%hi(var80062498)
/*  f016038:	3c0a8006 */ 	lui	$t2,%hi(var80062414)
/*  f01603c:	4600540d */ 	trunc.w.s	$f16,$f10
/*  f016040:	254a2414 */ 	addiu	$t2,$t2,%lo(var80062414)
/*  f016044:	25292498 */ 	addiu	$t1,$t1,%lo(var80062498)
/*  f016048:	02002025 */ 	or	$a0,$s0,$zero
/*  f01604c:	44058000 */ 	mfc1	$a1,$f16
/*  f016050:	00006812 */ 	mflo	$t5
/*  f016054:	00000000 */ 	nop
/*  f016058:	00000000 */ 	nop
/*  f01605c:	01ab001a */ 	div	$zero,$t5,$t3
/*  f016060:	00003012 */ 	mflo	$a2
/*  f016064:	15600002 */ 	bnez	$t3,.L0f016070
/*  f016068:	00000000 */ 	nop
/*  f01606c:	0007000d */ 	break	0x7
.L0f016070:
/*  f016070:	2401ffff */ 	addiu	$at,$zero,-1
/*  f016074:	15610004 */ 	bne	$t3,$at,.L0f016088
/*  f016078:	3c018000 */ 	lui	$at,0x8000
/*  f01607c:	15a10002 */ 	bne	$t5,$at,.L0f016088
/*  f016080:	00000000 */ 	nop
/*  f016084:	0006000d */ 	break	0x6
.L0f016088:
/*  f016088:	0fc0557c */ 	jal	func0f0155f0
/*  f01608c:	00000000 */ 	nop
/*  f016090:	3c098006 */ 	lui	$t1,%hi(var80062498)
/*  f016094:	25292498 */ 	addiu	$t1,$t1,%lo(var80062498)
/*  f016098:	8d230000 */ 	lw	$v1,0x0($t1)
/*  f01609c:	3c0a8006 */ 	lui	$t2,%hi(var80062414)
/*  f0160a0:	254a2414 */ 	addiu	$t2,$t2,%lo(var80062414)
/*  f0160a4:	28610410 */ 	slti	$at,$v1,0x410
/*  f0160a8:	1420000a */ 	bnez	$at,.L0f0160d4
/*  f0160ac:	00408025 */ 	or	$s0,$v0,$zero
/*  f0160b0:	28610488 */ 	slti	$at,$v1,0x488
/*  f0160b4:	54200008 */ 	bnezl	$at,.L0f0160d8
/*  f0160b8:	8fbf0024 */ 	lw	$ra,0x24($sp)
/*  f0160bc:	914f0000 */ 	lbu	$t7,0x0($t2)
/*  f0160c0:	ad200000 */ 	sw	$zero,0x0($t1)
/*  f0160c4:	25f80001 */ 	addiu	$t8,$t7,0x1
/*  f0160c8:	a1580000 */ 	sb	$t8,0x0($t2)
/*  f0160cc:	27190001 */ 	addiu	$t9,$t8,0x1
/*  f0160d0:	a1590000 */ 	sb	$t9,0x0($t2)
.L0f0160d4:
/*  f0160d4:	8fbf0024 */ 	lw	$ra,0x24($sp)
.L0f0160d8:
/*  f0160d8:	02001025 */ 	or	$v0,$s0,$zero
/*  f0160dc:	8fb00020 */ 	lw	$s0,0x20($sp)
/*  f0160e0:	03e00008 */ 	jr	$ra
/*  f0160e4:	27bd0038 */ 	addiu	$sp,$sp,0x38
/*  f0160e8:	3c028006 */ 	lui	$v0,%hi(var80062414)
/*  f0160ec:	90422414 */ 	lbu	$v0,%lo(var80062414)($v0)
/*  f0160f0:	384e0002 */ 	xori	$t6,$v0,0x2
/*  f0160f4:	03e00008 */ 	jr	$ra
/*  f0160f8:	2dc20001 */ 	sltiu	$v0,$t6,0x1
/*  f0160fc:	00000000 */ 	nop
);
