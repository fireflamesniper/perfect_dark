#include <ultra64.h>
#include "constants.h"
#include "game/data/data_000000.h"
#include "game/data/data_0083d0.h"
#include "game/data/data_00e460.h"
#include "game/data/data_0160b0.h"
#include "game/data/data_01a3a0.h"
#include "game/data/data_020df0.h"
#include "game/data/data_02da90.h"
#include "game/dlights.h"
#include "game/game_092610.h"
#include "game/game_096750.h"
#include "game/game_0b3350.h"
#include "game/game_150820.h"
#include "game/game_166e40.h"
#include "gvars/gvars.h"
#include "lib/lib_12dc0.h"
#include "lib/lib_159b0.h"
#include "lib/lib_16110.h"
#include "lib/lib_4a360.h"
#include "types.h"

const u32 var7f1b7370[] = {0x3dcccccd};
const u32 var7f1b7374[] = {0x3f333333};
const u32 var7f1b7378[] = {0x3f8f5c29};
const u32 var7f1b737c[] = {0x3df5c28f};
const u32 var7f1b7380[] = {0x40c907a9};
const u32 var7f1b7384[] = {0x3dcccccd};
const u32 var7f1b7388[] = {0x3dcccccd};
const u32 var7f1b738c[] = {0x3dcccccd};
const u32 var7f1b7390[] = {0xc61c4000};
const u32 var7f1b7394[] = {0x461c4000};
const u32 var7f1b7398[] = {0xc61c4000};
const u32 var7f1b739c[] = {0x461c4000};

GLOBAL_ASM(
glabel func0f150820
/*  f150820:	27bdfee8 */ 	addiu	$sp,$sp,-280
/*  f150824:	afbf0094 */ 	sw	$ra,0x94($sp)
/*  f150828:	afb70090 */ 	sw	$s7,0x90($sp)
/*  f15082c:	afb6008c */ 	sw	$s6,0x8c($sp)
/*  f150830:	afb50088 */ 	sw	$s5,0x88($sp)
/*  f150834:	afb40084 */ 	sw	$s4,0x84($sp)
/*  f150838:	afb30080 */ 	sw	$s3,0x80($sp)
/*  f15083c:	afb2007c */ 	sw	$s2,0x7c($sp)
/*  f150840:	afb10078 */ 	sw	$s1,0x78($sp)
/*  f150844:	afb00074 */ 	sw	$s0,0x74($sp)
/*  f150848:	f7be0068 */ 	sdc1	$f30,0x68($sp)
/*  f15084c:	f7bc0060 */ 	sdc1	$f28,0x60($sp)
/*  f150850:	f7ba0058 */ 	sdc1	$f26,0x58($sp)
/*  f150854:	f7b80050 */ 	sdc1	$f24,0x50($sp)
/*  f150858:	f7b60048 */ 	sdc1	$f22,0x48($sp)
/*  f15085c:	f7b40040 */ 	sdc1	$f20,0x40($sp)
/*  f150860:	c4a40000 */ 	lwc1	$f4,0x0($a1)
/*  f150864:	00c08025 */ 	or	$s0,$a2,$zero
/*  f150868:	00808825 */ 	or	$s1,$a0,$zero
/*  f15086c:	e7a400cc */ 	swc1	$f4,0xcc($sp)
/*  f150870:	c4a60004 */ 	lwc1	$f6,0x4($a1)
/*  f150874:	c7aa00cc */ 	lwc1	$f10,0xcc($sp)
/*  f150878:	00e09025 */ 	or	$s2,$a3,$zero
/*  f15087c:	e7a600d0 */ 	swc1	$f6,0xd0($sp)
/*  f150880:	460a5402 */ 	mul.s	$f16,$f10,$f10
/*  f150884:	c4a80008 */ 	lwc1	$f8,0x8($a1)
/*  f150888:	c7b200d0 */ 	lwc1	$f18,0xd0($sp)
/*  f15088c:	e7a800d4 */ 	swc1	$f8,0xd4($sp)
/*  f150890:	46129102 */ 	mul.s	$f4,$f18,$f18
/*  f150894:	c7a800d4 */ 	lwc1	$f8,0xd4($sp)
/*  f150898:	46084282 */ 	mul.s	$f10,$f8,$f8
/*  f15089c:	46048180 */ 	add.s	$f6,$f16,$f4
/*  f1508a0:	0c012974 */ 	jal	sqrtf
/*  f1508a4:	46065300 */ 	add.s	$f12,$f10,$f6
/*  f1508a8:	3c013f80 */ 	lui	$at,0x3f80
/*  f1508ac:	4481f000 */ 	mtc1	$at,$f30
/*  f1508b0:	c7b200cc */ 	lwc1	$f18,0xcc($sp)
/*  f1508b4:	c7a400d0 */ 	lwc1	$f4,0xd0($sp)
/*  f1508b8:	4600f503 */ 	div.s	$f20,$f30,$f0
/*  f1508bc:	c7aa00d4 */ 	lwc1	$f10,0xd4($sp)
/*  f1508c0:	c7b60128 */ 	lwc1	$f22,0x128($sp)
/*  f1508c4:	c7ba012c */ 	lwc1	$f26,0x12c($sp)
/*  f1508c8:	46149402 */ 	mul.s	$f16,$f18,$f20
/*  f1508cc:	00000000 */ 	nop
/*  f1508d0:	46142202 */ 	mul.s	$f8,$f4,$f20
/*  f1508d4:	00000000 */ 	nop
/*  f1508d8:	46145182 */ 	mul.s	$f6,$f10,$f20
/*  f1508dc:	e7b000cc */ 	swc1	$f16,0xcc($sp)
/*  f1508e0:	4600b582 */ 	mul.s	$f22,$f22,$f0
/*  f1508e4:	e7a800d0 */ 	swc1	$f8,0xd0($sp)
/*  f1508e8:	4600d682 */ 	mul.s	$f26,$f26,$f0
/*  f1508ec:	e7a600d4 */ 	swc1	$f6,0xd4($sp)
/*  f1508f0:	c6120000 */ 	lwc1	$f18,0x0($s0)
/*  f1508f4:	e7b200c0 */ 	swc1	$f18,0xc0($sp)
/*  f1508f8:	c6100004 */ 	lwc1	$f16,0x4($s0)
/*  f1508fc:	c7a800c0 */ 	lwc1	$f8,0xc0($sp)
/*  f150900:	e7b000c4 */ 	swc1	$f16,0xc4($sp)
/*  f150904:	46084282 */ 	mul.s	$f10,$f8,$f8
/*  f150908:	c6040008 */ 	lwc1	$f4,0x8($s0)
/*  f15090c:	c7a600c4 */ 	lwc1	$f6,0xc4($sp)
/*  f150910:	e7a400c8 */ 	swc1	$f4,0xc8($sp)
/*  f150914:	46063482 */ 	mul.s	$f18,$f6,$f6
/*  f150918:	c7a400c8 */ 	lwc1	$f4,0xc8($sp)
/*  f15091c:	46042202 */ 	mul.s	$f8,$f4,$f4
/*  f150920:	46125400 */ 	add.s	$f16,$f10,$f18
/*  f150924:	0c012974 */ 	jal	sqrtf
/*  f150928:	46104300 */ 	add.s	$f12,$f8,$f16
/*  f15092c:	4600f503 */ 	div.s	$f20,$f30,$f0
/*  f150930:	c7a600c0 */ 	lwc1	$f6,0xc0($sp)
/*  f150934:	c7b200c4 */ 	lwc1	$f18,0xc4($sp)
/*  f150938:	c7a800c8 */ 	lwc1	$f8,0xc8($sp)
/*  f15093c:	c7b80130 */ 	lwc1	$f24,0x130($sp)
/*  f150940:	c7bc0134 */ 	lwc1	$f28,0x134($sp)
/*  f150944:	46143282 */ 	mul.s	$f10,$f6,$f20
/*  f150948:	00000000 */ 	nop
/*  f15094c:	46149102 */ 	mul.s	$f4,$f18,$f20
/*  f150950:	00000000 */ 	nop
/*  f150954:	46144402 */ 	mul.s	$f16,$f8,$f20
/*  f150958:	e7aa00c0 */ 	swc1	$f10,0xc0($sp)
/*  f15095c:	4600c602 */ 	mul.s	$f24,$f24,$f0
/*  f150960:	e7a400c4 */ 	swc1	$f4,0xc4($sp)
/*  f150964:	4600e702 */ 	mul.s	$f28,$f28,$f0
/*  f150968:	e7b000c8 */ 	swc1	$f16,0xc8($sp)
/*  f15096c:	c64e0008 */ 	lwc1	$f14,0x8($s2)
/*  f150970:	0fc259d4 */ 	jal	func0f096750
/*  f150974:	c64c0000 */ 	lwc1	$f12,0x0($s2)
/*  f150978:	3c0e800a */ 	lui	$t6,%hi(var800a45c0)
/*  f15097c:	8dce45c0 */ 	lw	$t6,%lo(var800a45c0)($t6)
/*  f150980:	4616d501 */ 	sub.s	$f20,$f26,$f22
/*  f150984:	46000786 */ 	mov.s	$f30,$f0
/*  f150988:	4618e081 */ 	sub.s	$f2,$f28,$f24
/*  f15098c:	4602a182 */ 	mul.s	$f6,$f20,$f2
/*  f150990:	05c10003 */ 	bgez	$t6,.L0f1509a0
/*  f150994:	000e7843 */ 	sra	$t7,$t6,0x1
/*  f150998:	25c10001 */ 	addiu	$at,$t6,0x1
/*  f15099c:	00017843 */ 	sra	$t7,$at,0x1
.L0f1509a0:
/*  f1509a0:	448f5000 */ 	mtc1	$t7,$f10
/*  f1509a4:	e7a200ac */ 	swc1	$f2,0xac($sp)
/*  f1509a8:	468054a0 */ 	cvt.s.w	$f18,$f10
/*  f1509ac:	0c012974 */ 	jal	sqrtf
/*  f1509b0:	46123303 */ 	div.s	$f12,$f6,$f18
/*  f1509b4:	4600010d */ 	trunc.w.s	$f4,$f0
/*  f1509b8:	c7aa00cc */ 	lwc1	$f10,0xcc($sp)
/*  f1509bc:	e7a000ec */ 	swc1	$f0,0xec($sp)
/*  f1509c0:	c6300000 */ 	lwc1	$f16,0x0($s1)
/*  f1509c4:	44122000 */ 	mfc1	$s2,$f4
/*  f1509c8:	c7a400c0 */ 	lwc1	$f4,0xc0($sp)
/*  f1509cc:	8fb40138 */ 	lw	$s4,0x138($sp)
/*  f1509d0:	0012c843 */ 	sra	$t9,$s2,0x1
/*  f1509d4:	44994000 */ 	mtc1	$t9,$f8
/*  f1509d8:	8fb3013c */ 	lw	$s3,0x13c($sp)
/*  f1509dc:	00002025 */ 	or	$a0,$zero,$zero
/*  f1509e0:	468043a0 */ 	cvt.s.w	$f14,$f8
/*  f1509e4:	00002825 */ 	or	$a1,$zero,$zero
/*  f1509e8:	24068078 */ 	addiu	$a2,$zero,-32648
/*  f1509ec:	2407ffff */ 	addiu	$a3,$zero,-1
/*  f1509f0:	2408ffff */ 	addiu	$t0,$zero,-1
/*  f1509f4:	240bffff */ 	addiu	$t3,$zero,-1
/*  f1509f8:	460eb300 */ 	add.s	$f12,$f22,$f14
/*  f1509fc:	24010001 */ 	addiu	$at,$zero,0x1
/*  f150a00:	02408025 */ 	or	$s0,$s2,$zero
/*  f150a04:	460ec080 */ 	add.s	$f2,$f24,$f14
/*  f150a08:	460a6182 */ 	mul.s	$f6,$f12,$f10
/*  f150a0c:	26690008 */ 	addiu	$t1,$s3,0x8
/*  f150a10:	266a0028 */ 	addiu	$t2,$s3,0x28
/*  f150a14:	46022202 */ 	mul.s	$f8,$f4,$f2
/*  f150a18:	46068480 */ 	add.s	$f18,$f16,$f6
/*  f150a1c:	c7a600d0 */ 	lwc1	$f6,0xd0($sp)
/*  f150a20:	46124280 */ 	add.s	$f10,$f8,$f18
/*  f150a24:	46066102 */ 	mul.s	$f4,$f12,$f6
/*  f150a28:	c7b200c4 */ 	lwc1	$f18,0xc4($sp)
/*  f150a2c:	e7aa00d8 */ 	swc1	$f10,0xd8($sp)
/*  f150a30:	46029282 */ 	mul.s	$f10,$f18,$f2
/*  f150a34:	c6300004 */ 	lwc1	$f16,0x4($s1)
/*  f150a38:	46048200 */ 	add.s	$f8,$f16,$f4
/*  f150a3c:	c7a400d4 */ 	lwc1	$f4,0xd4($sp)
/*  f150a40:	46085180 */ 	add.s	$f6,$f10,$f8
/*  f150a44:	46046482 */ 	mul.s	$f18,$f12,$f4
/*  f150a48:	c7a800c8 */ 	lwc1	$f8,0xc8($sp)
/*  f150a4c:	e7a600dc */ 	swc1	$f6,0xdc($sp)
/*  f150a50:	46024182 */ 	mul.s	$f6,$f8,$f2
/*  f150a54:	c6300008 */ 	lwc1	$f16,0x8($s1)
/*  f150a58:	46128280 */ 	add.s	$f10,$f16,$f18
/*  f150a5c:	460a3100 */ 	add.s	$f4,$f6,$f10
/*  f150a60:	16800011 */ 	bnez	$s4,.L0f150aa8
/*  f150a64:	e7a400e0 */ 	swc1	$f4,0xe0($sp)
/*  f150a68:	3c01bf80 */ 	lui	$at,0xbf80
/*  f150a6c:	44810000 */ 	mtc1	$at,$f0
/*  f150a70:	afa80010 */ 	sw	$t0,0x10($sp)
/*  f150a74:	afa00014 */ 	sw	$zero,0x14($sp)
/*  f150a78:	afa00018 */ 	sw	$zero,0x18($sp)
/*  f150a7c:	afa0001c */ 	sw	$zero,0x1c($sp)
/*  f150a80:	afa90020 */ 	sw	$t1,0x20($sp)
/*  f150a84:	afaa0028 */ 	sw	$t2,0x28($sp)
/*  f150a88:	afab002c */ 	sw	$t3,0x2c($sp)
/*  f150a8c:	e7a00024 */ 	swc1	$f0,0x24($sp)
/*  f150a90:	e7a00030 */ 	swc1	$f0,0x30($sp)
/*  f150a94:	e7a00034 */ 	swc1	$f0,0x34($sp)
/*  f150a98:	0fc24e7e */ 	jal	func0f0939f8
/*  f150a9c:	e7a00038 */ 	swc1	$f0,0x38($sp)
/*  f150aa0:	1000003c */ 	b	.L0f150b94
/*  f150aa4:	44908000 */ 	mtc1	$s0,$f16
.L0f150aa8:
/*  f150aa8:	56810021 */ 	bnel	$s4,$at,.L0f150b30
/*  f150aac:	24010002 */ 	addiu	$at,$zero,0x2
/*  f150ab0:	0c004b70 */ 	jal	random
/*  f150ab4:	00000000 */ 	nop
/*  f150ab8:	3c01bf80 */ 	lui	$at,0xbf80
/*  f150abc:	44810000 */ 	mtc1	$at,$f0
/*  f150ac0:	8fb3013c */ 	lw	$s3,0x13c($sp)
/*  f150ac4:	30460001 */ 	andi	$a2,$v0,0x1
/*  f150ac8:	00066040 */ 	sll	$t4,$a2,0x1
/*  f150acc:	25860010 */ 	addiu	$a2,$t4,0x10
/*  f150ad0:	00066c00 */ 	sll	$t5,$a2,0x10
/*  f150ad4:	240fffff */ 	addiu	$t7,$zero,-1
/*  f150ad8:	2408ffff */ 	addiu	$t0,$zero,-1
/*  f150adc:	26780008 */ 	addiu	$t8,$s3,0x8
/*  f150ae0:	26790028 */ 	addiu	$t9,$s3,0x28
/*  f150ae4:	afb90028 */ 	sw	$t9,0x28($sp)
/*  f150ae8:	afb80020 */ 	sw	$t8,0x20($sp)
/*  f150aec:	afa8002c */ 	sw	$t0,0x2c($sp)
/*  f150af0:	afaf0010 */ 	sw	$t7,0x10($sp)
/*  f150af4:	000d3403 */ 	sra	$a2,$t5,0x10
/*  f150af8:	00002025 */ 	or	$a0,$zero,$zero
/*  f150afc:	00002825 */ 	or	$a1,$zero,$zero
/*  f150b00:	2407ffff */ 	addiu	$a3,$zero,-1
/*  f150b04:	afa00014 */ 	sw	$zero,0x14($sp)
/*  f150b08:	afa00018 */ 	sw	$zero,0x18($sp)
/*  f150b0c:	afa0001c */ 	sw	$zero,0x1c($sp)
/*  f150b10:	e7a00024 */ 	swc1	$f0,0x24($sp)
/*  f150b14:	e7a00030 */ 	swc1	$f0,0x30($sp)
/*  f150b18:	e7a00034 */ 	swc1	$f0,0x34($sp)
/*  f150b1c:	0fc24e7e */ 	jal	func0f0939f8
/*  f150b20:	e7a00038 */ 	swc1	$f0,0x38($sp)
/*  f150b24:	1000001b */ 	b	.L0f150b94
/*  f150b28:	44908000 */ 	mtc1	$s0,$f16
/*  f150b2c:	24010002 */ 	addiu	$at,$zero,0x2
.L0f150b30:
/*  f150b30:	16810017 */ 	bne	$s4,$at,.L0f150b90
/*  f150b34:	8fb3013c */ 	lw	$s3,0x13c($sp)
/*  f150b38:	3c01bf80 */ 	lui	$at,0xbf80
/*  f150b3c:	44810000 */ 	mtc1	$at,$f0
/*  f150b40:	2409ffff */ 	addiu	$t1,$zero,-1
/*  f150b44:	266a0008 */ 	addiu	$t2,$s3,0x8
/*  f150b48:	266b0028 */ 	addiu	$t3,$s3,0x28
/*  f150b4c:	240cffff */ 	addiu	$t4,$zero,-1
/*  f150b50:	afac002c */ 	sw	$t4,0x2c($sp)
/*  f150b54:	afab0028 */ 	sw	$t3,0x28($sp)
/*  f150b58:	afaa0020 */ 	sw	$t2,0x20($sp)
/*  f150b5c:	afa90010 */ 	sw	$t1,0x10($sp)
/*  f150b60:	00002025 */ 	or	$a0,$zero,$zero
/*  f150b64:	00002825 */ 	or	$a1,$zero,$zero
/*  f150b68:	240605d8 */ 	addiu	$a2,$zero,0x5d8
/*  f150b6c:	2407ffff */ 	addiu	$a3,$zero,-1
/*  f150b70:	afa00014 */ 	sw	$zero,0x14($sp)
/*  f150b74:	afa00018 */ 	sw	$zero,0x18($sp)
/*  f150b78:	afa0001c */ 	sw	$zero,0x1c($sp)
/*  f150b7c:	e7a00024 */ 	swc1	$f0,0x24($sp)
/*  f150b80:	e7a00030 */ 	swc1	$f0,0x30($sp)
/*  f150b84:	e7a00034 */ 	swc1	$f0,0x34($sp)
/*  f150b88:	0fc24e7e */ 	jal	func0f0939f8
/*  f150b8c:	e7a00038 */ 	swc1	$f0,0x38($sp)
.L0f150b90:
/*  f150b90:	44908000 */ 	mtc1	$s0,$f16
.L0f150b94:
/*  f150b94:	44923000 */ 	mtc1	$s2,$f6
/*  f150b98:	c7a400ac */ 	lwc1	$f4,0xac($sp)
/*  f150b9c:	468085a0 */ 	cvt.s.w	$f22,$f16
/*  f150ba0:	8fb3013c */ 	lw	$s3,0x13c($sp)
/*  f150ba4:	0000a825 */ 	or	$s5,$zero,$zero
/*  f150ba8:	3c017f1b */ 	lui	$at,%hi(var7f1b7370)
/*  f150bac:	468032a0 */ 	cvt.s.w	$f10,$f6
/*  f150bb0:	4616a483 */ 	div.s	$f18,$f20,$f22
/*  f150bb4:	e7aa00a4 */ 	swc1	$f10,0xa4($sp)
/*  f150bb8:	c7b000a4 */ 	lwc1	$f16,0xa4($sp)
/*  f150bbc:	4600920d */ 	trunc.w.s	$f8,$f18
/*  f150bc0:	46102483 */ 	div.s	$f18,$f4,$f16
/*  f150bc4:	44164000 */ 	mfc1	$s6,$f8
/*  f150bc8:	00000000 */ 	nop
/*  f150bcc:	02c08825 */ 	or	$s1,$s6,$zero
/*  f150bd0:	4600920d */ 	trunc.w.s	$f8,$f18
/*  f150bd4:	44174000 */ 	mfc1	$s7,$f8
/*  f150bd8:	00000000 */ 	nop
/*  f150bdc:	5ae00046 */ 	blezl	$s7,.L0f150cf8
/*  f150be0:	8fbf0094 */ 	lw	$ra,0x94($sp)
/*  f150be4:	c43c7370 */ 	lwc1	$f28,%lo(var7f1b7370)($at)
/*  f150be8:	3c017f1b */ 	lui	$at,%hi(var7f1b7374)
/*  f150bec:	c43a7374 */ 	lwc1	$f26,%lo(var7f1b7374)($at)
/*  f150bf0:	3c012f80 */ 	lui	$at,0x2f80
/*  f150bf4:	4481c000 */ 	mtc1	$at,$f24
/*  f150bf8:	27b200fc */ 	addiu	$s2,$sp,0xfc
.L0f150bfc:
/*  f150bfc:	1ac00039 */ 	blez	$s6,.L0f150ce4
/*  f150c00:	00008025 */ 	or	$s0,$zero,$zero
/*  f150c04:	44953000 */ 	mtc1	$s5,$f6
/*  f150c08:	c7a400a4 */ 	lwc1	$f4,0xa4($sp)
/*  f150c0c:	468032a0 */ 	cvt.s.w	$f10,$f6
/*  f150c10:	46045502 */ 	mul.s	$f20,$f10,$f4
/*  f150c14:	00000000 */ 	nop
/*  f150c18:	44908000 */ 	mtc1	$s0,$f16
.L0f150c1c:
/*  f150c1c:	c7a600cc */ 	lwc1	$f6,0xcc($sp)
/*  f150c20:	c7a800d8 */ 	lwc1	$f8,0xd8($sp)
/*  f150c24:	468084a0 */ 	cvt.s.w	$f18,$f16
/*  f150c28:	c7b000c0 */ 	lwc1	$f16,0xc0($sp)
/*  f150c2c:	46169002 */ 	mul.s	$f0,$f18,$f22
/*  f150c30:	00000000 */ 	nop
/*  f150c34:	46060282 */ 	mul.s	$f10,$f0,$f6
/*  f150c38:	460a4100 */ 	add.s	$f4,$f8,$f10
/*  f150c3c:	46148482 */ 	mul.s	$f18,$f16,$f20
/*  f150c40:	c7aa00d0 */ 	lwc1	$f10,0xd0($sp)
/*  f150c44:	c7a800dc */ 	lwc1	$f8,0xdc($sp)
/*  f150c48:	460a0402 */ 	mul.s	$f16,$f0,$f10
/*  f150c4c:	46049180 */ 	add.s	$f6,$f18,$f4
/*  f150c50:	c7a400c4 */ 	lwc1	$f4,0xc4($sp)
/*  f150c54:	46104480 */ 	add.s	$f18,$f8,$f16
/*  f150c58:	e7a600fc */ 	swc1	$f6,0xfc($sp)
/*  f150c5c:	46142182 */ 	mul.s	$f6,$f4,$f20
/*  f150c60:	c7b000d4 */ 	lwc1	$f16,0xd4($sp)
/*  f150c64:	c7a800e0 */ 	lwc1	$f8,0xe0($sp)
/*  f150c68:	46100102 */ 	mul.s	$f4,$f0,$f16
/*  f150c6c:	46123280 */ 	add.s	$f10,$f6,$f18
/*  f150c70:	c7b200c8 */ 	lwc1	$f18,0xc8($sp)
/*  f150c74:	46044180 */ 	add.s	$f6,$f8,$f4
/*  f150c78:	e7aa0100 */ 	swc1	$f10,0x100($sp)
/*  f150c7c:	46149282 */ 	mul.s	$f10,$f18,$f20
/*  f150c80:	46065400 */ 	add.s	$f16,$f10,$f6
/*  f150c84:	0c004b70 */ 	jal	random
/*  f150c88:	e7b00104 */ 	swc1	$f16,0x104($sp)
/*  f150c8c:	44824000 */ 	mtc1	$v0,$f8
/*  f150c90:	4406f000 */ 	mfc1	$a2,$f30
/*  f150c94:	86640028 */ 	lh	$a0,0x28($s3)
/*  f150c98:	02402825 */ 	or	$a1,$s2,$zero
/*  f150c9c:	04410005 */ 	bgez	$v0,.L0f150cb4
/*  f150ca0:	46804120 */ 	cvt.s.w	$f4,$f8
/*  f150ca4:	3c014f80 */ 	lui	$at,0x4f80
/*  f150ca8:	44819000 */ 	mtc1	$at,$f18
/*  f150cac:	00000000 */ 	nop
/*  f150cb0:	46122100 */ 	add.s	$f4,$f4,$f18
.L0f150cb4:
/*  f150cb4:	46182282 */ 	mul.s	$f10,$f4,$f24
/*  f150cb8:	c7a800ec */ 	lwc1	$f8,0xec($sp)
/*  f150cbc:	afb40010 */ 	sw	$s4,0x10($sp)
/*  f150cc0:	461a5182 */ 	mul.s	$f6,$f10,$f26
/*  f150cc4:	461c3400 */ 	add.s	$f16,$f6,$f28
/*  f150cc8:	46088482 */ 	mul.s	$f18,$f16,$f8
/*  f150ccc:	44079000 */ 	mfc1	$a3,$f18
/*  f150cd0:	0fc5434e */ 	jal	func0f150d38
/*  f150cd4:	00000000 */ 	nop
/*  f150cd8:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f150cdc:	5611ffcf */ 	bnel	$s0,$s1,.L0f150c1c
/*  f150ce0:	44908000 */ 	mtc1	$s0,$f16
.L0f150ce4:
/*  f150ce4:	26b50001 */ 	addiu	$s5,$s5,0x1
/*  f150ce8:	02b7082a */ 	slt	$at,$s5,$s7
/*  f150cec:	1420ffc3 */ 	bnez	$at,.L0f150bfc
/*  f150cf0:	00000000 */ 	nop
/*  f150cf4:	8fbf0094 */ 	lw	$ra,0x94($sp)
.L0f150cf8:
/*  f150cf8:	d7b40040 */ 	ldc1	$f20,0x40($sp)
/*  f150cfc:	d7b60048 */ 	ldc1	$f22,0x48($sp)
/*  f150d00:	d7b80050 */ 	ldc1	$f24,0x50($sp)
/*  f150d04:	d7ba0058 */ 	ldc1	$f26,0x58($sp)
/*  f150d08:	d7bc0060 */ 	ldc1	$f28,0x60($sp)
/*  f150d0c:	d7be0068 */ 	ldc1	$f30,0x68($sp)
/*  f150d10:	8fb00074 */ 	lw	$s0,0x74($sp)
/*  f150d14:	8fb10078 */ 	lw	$s1,0x78($sp)
/*  f150d18:	8fb2007c */ 	lw	$s2,0x7c($sp)
/*  f150d1c:	8fb30080 */ 	lw	$s3,0x80($sp)
/*  f150d20:	8fb40084 */ 	lw	$s4,0x84($sp)
/*  f150d24:	8fb50088 */ 	lw	$s5,0x88($sp)
/*  f150d28:	8fb6008c */ 	lw	$s6,0x8c($sp)
/*  f150d2c:	8fb70090 */ 	lw	$s7,0x90($sp)
/*  f150d30:	03e00008 */ 	jr	$ra
/*  f150d34:	27bd0118 */ 	addiu	$sp,$sp,0x118
);

GLOBAL_ASM(
glabel func0f150d38
/*  f150d38:	27bdff98 */ 	addiu	$sp,$sp,-104
/*  f150d3c:	afb10024 */ 	sw	$s1,0x24($sp)
/*  f150d40:	00048c00 */ 	sll	$s1,$a0,0x10
/*  f150d44:	00117403 */ 	sra	$t6,$s1,0x10
/*  f150d48:	afbf003c */ 	sw	$ra,0x3c($sp)
/*  f150d4c:	afb00020 */ 	sw	$s0,0x20($sp)
/*  f150d50:	00a08025 */ 	or	$s0,$a1,$zero
/*  f150d54:	01c08825 */ 	or	$s1,$t6,$zero
/*  f150d58:	afb60038 */ 	sw	$s6,0x38($sp)
/*  f150d5c:	afb50034 */ 	sw	$s5,0x34($sp)
/*  f150d60:	afb40030 */ 	sw	$s4,0x30($sp)
/*  f150d64:	afb3002c */ 	sw	$s3,0x2c($sp)
/*  f150d68:	afb20028 */ 	sw	$s2,0x28($sp)
/*  f150d6c:	f7b40018 */ 	sdc1	$f20,0x18($sp)
/*  f150d70:	afa40068 */ 	sw	$a0,0x68($sp)
/*  f150d74:	afa60070 */ 	sw	$a2,0x70($sp)
/*  f150d78:	0c004b70 */ 	jal	random
/*  f150d7c:	afa70074 */ 	sw	$a3,0x74($sp)
/*  f150d80:	44822000 */ 	mtc1	$v0,$f4
/*  f150d84:	3c012f80 */ 	lui	$at,0x2f80
/*  f150d88:	4481a000 */ 	mtc1	$at,$f20
/*  f150d8c:	04410005 */ 	bgez	$v0,.L0f150da4
/*  f150d90:	468021a0 */ 	cvt.s.w	$f6,$f4
/*  f150d94:	3c014f80 */ 	lui	$at,0x4f80
/*  f150d98:	44814000 */ 	mtc1	$at,$f8
/*  f150d9c:	00000000 */ 	nop
/*  f150da0:	46083180 */ 	add.s	$f6,$f6,$f8
.L0f150da4:
/*  f150da4:	46143002 */ 	mul.s	$f0,$f6,$f20
/*  f150da8:	3c013f80 */ 	lui	$at,0x3f80
/*  f150dac:	44818000 */ 	mtc1	$at,$f16
/*  f150db0:	46000280 */ 	add.s	$f10,$f0,$f0
/*  f150db4:	46105481 */ 	sub.s	$f18,$f10,$f16
/*  f150db8:	0c004b70 */ 	jal	random
/*  f150dbc:	e7b20064 */ 	swc1	$f18,0x64($sp)
/*  f150dc0:	44822000 */ 	mtc1	$v0,$f4
/*  f150dc4:	04410005 */ 	bgez	$v0,.L0f150ddc
/*  f150dc8:	46802220 */ 	cvt.s.w	$f8,$f4
/*  f150dcc:	3c014f80 */ 	lui	$at,0x4f80
/*  f150dd0:	44813000 */ 	mtc1	$at,$f6
/*  f150dd4:	00000000 */ 	nop
/*  f150dd8:	46064200 */ 	add.s	$f8,$f8,$f6
.L0f150ddc:
/*  f150ddc:	46144282 */ 	mul.s	$f10,$f8,$f20
/*  f150de0:	3c017f1b */ 	lui	$at,%hi(var7f1b7378)
/*  f150de4:	c4307378 */ 	lwc1	$f16,%lo(var7f1b7378)($at)
/*  f150de8:	3c017f1b */ 	lui	$at,%hi(var7f1b737c)
/*  f150dec:	c424737c */ 	lwc1	$f4,%lo(var7f1b737c)($at)
/*  f150df0:	46105482 */ 	mul.s	$f18,$f10,$f16
/*  f150df4:	46049181 */ 	sub.s	$f6,$f18,$f4
/*  f150df8:	0c004b70 */ 	jal	random
/*  f150dfc:	e7a60060 */ 	swc1	$f6,0x60($sp)
/*  f150e00:	3c148008 */ 	lui	$s4,%hi(var8007fa80)
/*  f150e04:	2694fa80 */ 	addiu	$s4,$s4,%lo(var8007fa80)
/*  f150e08:	8e980000 */ 	lw	$t8,0x0($s4)
/*  f150e0c:	2416006c */ 	addiu	$s6,$zero,0x6c
/*  f150e10:	3c15800a */ 	lui	$s5,%hi(var800a45c4)
/*  f150e14:	03160019 */ 	multu	$t8,$s6
/*  f150e18:	26b545c4 */ 	addiu	$s5,$s5,%lo(var800a45c4)
/*  f150e1c:	8eaf0000 */ 	lw	$t7,0x0($s5)
/*  f150e20:	8fb20078 */ 	lw	$s2,0x78($sp)
/*  f150e24:	240d0001 */ 	addiu	$t5,$zero,0x1
/*  f150e28:	3c013fc0 */ 	lui	$at,0x3fc0
/*  f150e2c:	44811000 */ 	mtc1	$at,$f2
/*  f150e30:	3c014040 */ 	lui	$at,0x4040
/*  f150e34:	0000c812 */ 	mflo	$t9
/*  f150e38:	01f94021 */ 	addu	$t0,$t7,$t9
/*  f150e3c:	a1120068 */ 	sb	$s2,0x68($t0)
/*  f150e40:	8e8a0000 */ 	lw	$t2,0x0($s4)
/*  f150e44:	8ea90000 */ 	lw	$t1,0x0($s5)
/*  f150e48:	01560019 */ 	multu	$t2,$s6
/*  f150e4c:	00005812 */ 	mflo	$t3
/*  f150e50:	012b6021 */ 	addu	$t4,$t1,$t3
/*  f150e54:	a5910000 */ 	sh	$s1,0x0($t4)
/*  f150e58:	8e980000 */ 	lw	$t8,0x0($s4)
/*  f150e5c:	8eae0000 */ 	lw	$t6,0x0($s5)
/*  f150e60:	03160019 */ 	multu	$t8,$s6
/*  f150e64:	00007812 */ 	mflo	$t7
/*  f150e68:	01cfc821 */ 	addu	$t9,$t6,$t7
/*  f150e6c:	af2d0004 */ 	sw	$t5,0x4($t9)
/*  f150e70:	8e8a0000 */ 	lw	$t2,0x0($s4)
/*  f150e74:	8ea80000 */ 	lw	$t0,0x0($s5)
/*  f150e78:	c6080000 */ 	lwc1	$f8,0x0($s0)
/*  f150e7c:	01560019 */ 	multu	$t2,$s6
/*  f150e80:	00004812 */ 	mflo	$t1
/*  f150e84:	01095821 */ 	addu	$t3,$t0,$t1
/*  f150e88:	e5680008 */ 	swc1	$f8,0x8($t3)
/*  f150e8c:	8e980000 */ 	lw	$t8,0x0($s4)
/*  f150e90:	8eac0000 */ 	lw	$t4,0x0($s5)
/*  f150e94:	c60a0004 */ 	lwc1	$f10,0x4($s0)
/*  f150e98:	03160019 */ 	multu	$t8,$s6
/*  f150e9c:	44814000 */ 	mtc1	$at,$f8
/*  f150ea0:	00007012 */ 	mflo	$t6
/*  f150ea4:	018e7821 */ 	addu	$t7,$t4,$t6
/*  f150ea8:	e5ea000c */ 	swc1	$f10,0xc($t7)
/*  f150eac:	8e990000 */ 	lw	$t9,0x0($s4)
/*  f150eb0:	8ead0000 */ 	lw	$t5,0x0($s5)
/*  f150eb4:	c6100008 */ 	lwc1	$f16,0x8($s0)
/*  f150eb8:	03360019 */ 	multu	$t9,$s6
/*  f150ebc:	00005012 */ 	mflo	$t2
/*  f150ec0:	01aa4021 */ 	addu	$t0,$t5,$t2
/*  f150ec4:	e5100010 */ 	swc1	$f16,0x10($t0)
/*  f150ec8:	8e8b0000 */ 	lw	$t3,0x0($s4)
/*  f150ecc:	c7b20064 */ 	lwc1	$f18,0x64($sp)
/*  f150ed0:	8ea90000 */ 	lw	$t1,0x0($s5)
/*  f150ed4:	01760019 */ 	multu	$t3,$s6
/*  f150ed8:	46029102 */ 	mul.s	$f4,$f18,$f2
/*  f150edc:	44828000 */ 	mtc1	$v0,$f16
/*  f150ee0:	00000000 */ 	nop
/*  f150ee4:	468084a0 */ 	cvt.s.w	$f18,$f16
/*  f150ee8:	0000c012 */ 	mflo	$t8
/*  f150eec:	01386021 */ 	addu	$t4,$t1,$t8
/*  f150ef0:	e5840020 */ 	swc1	$f4,0x20($t4)
/*  f150ef4:	8e8f0000 */ 	lw	$t7,0x0($s4)
/*  f150ef8:	c7a60060 */ 	lwc1	$f6,0x60($sp)
/*  f150efc:	8eae0000 */ 	lw	$t6,0x0($s5)
/*  f150f00:	01f60019 */ 	multu	$t7,$s6
/*  f150f04:	46083282 */ 	mul.s	$f10,$f6,$f8
/*  f150f08:	0000c812 */ 	mflo	$t9
/*  f150f0c:	01d96821 */ 	addu	$t5,$t6,$t9
/*  f150f10:	04410005 */ 	bgez	$v0,.L0f150f28
/*  f150f14:	e5aa0024 */ 	swc1	$f10,0x24($t5)
/*  f150f18:	3c014f80 */ 	lui	$at,0x4f80
/*  f150f1c:	44812000 */ 	mtc1	$at,$f4
/*  f150f20:	00000000 */ 	nop
/*  f150f24:	46049480 */ 	add.s	$f18,$f18,$f4
.L0f150f28:
/*  f150f28:	46149002 */ 	mul.s	$f0,$f18,$f20
/*  f150f2c:	8e880000 */ 	lw	$t0,0x0($s4)
/*  f150f30:	3c013f80 */ 	lui	$at,0x3f80
/*  f150f34:	44814000 */ 	mtc1	$at,$f8
/*  f150f38:	01160019 */ 	multu	$t0,$s6
/*  f150f3c:	8eaa0000 */ 	lw	$t2,0x0($s5)
/*  f150f40:	46000180 */ 	add.s	$f6,$f0,$f0
/*  f150f44:	46083281 */ 	sub.s	$f10,$f6,$f8
/*  f150f48:	00005812 */ 	mflo	$t3
/*  f150f4c:	46025402 */ 	mul.s	$f16,$f10,$f2
/*  f150f50:	014b4821 */ 	addu	$t1,$t2,$t3
/*  f150f54:	0c004b70 */ 	jal	random
/*  f150f58:	e5300028 */ 	swc1	$f16,0x28($t1)
/*  f150f5c:	44822000 */ 	mtc1	$v0,$f4
/*  f150f60:	04410005 */ 	bgez	$v0,.L0f150f78
/*  f150f64:	468024a0 */ 	cvt.s.w	$f18,$f4
/*  f150f68:	3c014f80 */ 	lui	$at,0x4f80
/*  f150f6c:	44813000 */ 	mtc1	$at,$f6
/*  f150f70:	00000000 */ 	nop
/*  f150f74:	46069480 */ 	add.s	$f18,$f18,$f6
.L0f150f78:
/*  f150f78:	46149202 */ 	mul.s	$f8,$f18,$f20
/*  f150f7c:	3c013f00 */ 	lui	$at,0x3f00
/*  f150f80:	44815000 */ 	mtc1	$at,$f10
/*  f150f84:	3c013f80 */ 	lui	$at,0x3f80
/*  f150f88:	44812000 */ 	mtc1	$at,$f4
/*  f150f8c:	c7b20074 */ 	lwc1	$f18,0x74($sp)
/*  f150f90:	8e8e0000 */ 	lw	$t6,0x0($s4)
/*  f150f94:	460a4402 */ 	mul.s	$f16,$f8,$f10
/*  f150f98:	8eaf0000 */ 	lw	$t7,0x0($s5)
/*  f150f9c:	01d60019 */ 	multu	$t6,$s6
/*  f150fa0:	46048180 */ 	add.s	$f6,$f16,$f4
/*  f150fa4:	46123202 */ 	mul.s	$f8,$f6,$f18
/*  f150fa8:	0000c812 */ 	mflo	$t9
/*  f150fac:	01f96821 */ 	addu	$t5,$t7,$t9
/*  f150fb0:	4600428d */ 	trunc.w.s	$f10,$f8
/*  f150fb4:	440c5000 */ 	mfc1	$t4,$f10
/*  f150fb8:	0c004b70 */ 	jal	random
/*  f150fbc:	a5ac0038 */ 	sh	$t4,0x38($t5)
/*  f150fc0:	44828000 */ 	mtc1	$v0,$f16
/*  f150fc4:	04410005 */ 	bgez	$v0,.L0f150fdc
/*  f150fc8:	46808120 */ 	cvt.s.w	$f4,$f16
/*  f150fcc:	3c014f80 */ 	lui	$at,0x4f80
/*  f150fd0:	44813000 */ 	mtc1	$at,$f6
/*  f150fd4:	00000000 */ 	nop
/*  f150fd8:	46062100 */ 	add.s	$f4,$f4,$f6
.L0f150fdc:
/*  f150fdc:	46142482 */ 	mul.s	$f18,$f4,$f20
/*  f150fe0:	3c013f00 */ 	lui	$at,0x3f00
/*  f150fe4:	44814000 */ 	mtc1	$at,$f8
/*  f150fe8:	3c013f80 */ 	lui	$at,0x3f80
/*  f150fec:	44818000 */ 	mtc1	$at,$f16
/*  f150ff0:	c7a40074 */ 	lwc1	$f4,0x74($sp)
/*  f150ff4:	8e890000 */ 	lw	$t1,0x0($s4)
/*  f150ff8:	46089282 */ 	mul.s	$f10,$f18,$f8
/*  f150ffc:	8eab0000 */ 	lw	$t3,0x0($s5)
/*  f151000:	01360019 */ 	multu	$t1,$s6
/*  f151004:	46105180 */ 	add.s	$f6,$f10,$f16
/*  f151008:	46043482 */ 	mul.s	$f18,$f6,$f4
/*  f15100c:	0000c012 */ 	mflo	$t8
/*  f151010:	01787021 */ 	addu	$t6,$t3,$t8
/*  f151014:	4600920d */ 	trunc.w.s	$f8,$f18
/*  f151018:	440a4000 */ 	mfc1	$t2,$f8
/*  f15101c:	0c004b70 */ 	jal	random
/*  f151020:	a5ca003a */ 	sh	$t2,0x3a($t6)
/*  f151024:	44825000 */ 	mtc1	$v0,$f10
/*  f151028:	04410005 */ 	bgez	$v0,.L0f151040
/*  f15102c:	46805420 */ 	cvt.s.w	$f16,$f10
/*  f151030:	3c014f80 */ 	lui	$at,0x4f80
/*  f151034:	44813000 */ 	mtc1	$at,$f6
/*  f151038:	00000000 */ 	nop
/*  f15103c:	46068400 */ 	add.s	$f16,$f16,$f6
.L0f151040:
/*  f151040:	46148102 */ 	mul.s	$f4,$f16,$f20
/*  f151044:	3c013f00 */ 	lui	$at,0x3f00
/*  f151048:	44819000 */ 	mtc1	$at,$f18
/*  f15104c:	3c013f80 */ 	lui	$at,0x3f80
/*  f151050:	44815000 */ 	mtc1	$at,$f10
/*  f151054:	c7b00074 */ 	lwc1	$f16,0x74($sp)
/*  f151058:	8e8d0000 */ 	lw	$t5,0x0($s4)
/*  f15105c:	46122202 */ 	mul.s	$f8,$f4,$f18
/*  f151060:	8eac0000 */ 	lw	$t4,0x0($s5)
/*  f151064:	01b60019 */ 	multu	$t5,$s6
/*  f151068:	460a4180 */ 	add.s	$f6,$f8,$f10
/*  f15106c:	46103102 */ 	mul.s	$f4,$f6,$f16
/*  f151070:	00004012 */ 	mflo	$t0
/*  f151074:	01884821 */ 	addu	$t1,$t4,$t0
/*  f151078:	4600248d */ 	trunc.w.s	$f18,$f4
/*  f15107c:	44199000 */ 	mfc1	$t9,$f18
/*  f151080:	0c004b70 */ 	jal	random
/*  f151084:	a5390044 */ 	sh	$t9,0x44($t1)
/*  f151088:	c7a00074 */ 	lwc1	$f0,0x74($sp)
/*  f15108c:	44824000 */ 	mtc1	$v0,$f8
/*  f151090:	46000007 */ 	neg.s	$f0,$f0
/*  f151094:	04410005 */ 	bgez	$v0,.L0f1510ac
/*  f151098:	468042a0 */ 	cvt.s.w	$f10,$f8
/*  f15109c:	3c014f80 */ 	lui	$at,0x4f80
/*  f1510a0:	44813000 */ 	mtc1	$at,$f6
/*  f1510a4:	00000000 */ 	nop
/*  f1510a8:	46065280 */ 	add.s	$f10,$f10,$f6
.L0f1510ac:
/*  f1510ac:	46145402 */ 	mul.s	$f16,$f10,$f20
/*  f1510b0:	3c013f00 */ 	lui	$at,0x3f00
/*  f1510b4:	44812000 */ 	mtc1	$at,$f4
/*  f1510b8:	3c013f80 */ 	lui	$at,0x3f80
/*  f1510bc:	44814000 */ 	mtc1	$at,$f8
/*  f1510c0:	8e8e0000 */ 	lw	$t6,0x0($s4)
/*  f1510c4:	8eaa0000 */ 	lw	$t2,0x0($s5)
/*  f1510c8:	46048482 */ 	mul.s	$f18,$f16,$f4
/*  f1510cc:	46089180 */ 	add.s	$f6,$f18,$f8
/*  f1510d0:	01d60019 */ 	multu	$t6,$s6
/*  f1510d4:	46003282 */ 	mul.s	$f10,$f6,$f0
/*  f1510d8:	4600540d */ 	trunc.w.s	$f16,$f10
/*  f1510dc:	00007812 */ 	mflo	$t7
/*  f1510e0:	014f6821 */ 	addu	$t5,$t2,$t7
/*  f1510e4:	44188000 */ 	mfc1	$t8,$f16
/*  f1510e8:	00000000 */ 	nop
/*  f1510ec:	a5b80046 */ 	sh	$t8,0x46($t5)
/*  f1510f0:	0c004b70 */ 	jal	random
/*  f1510f4:	e7a00044 */ 	swc1	$f0,0x44($sp)
/*  f1510f8:	44822000 */ 	mtc1	$v0,$f4
/*  f1510fc:	04410005 */ 	bgez	$v0,.L0f151114
/*  f151100:	468024a0 */ 	cvt.s.w	$f18,$f4
/*  f151104:	3c014f80 */ 	lui	$at,0x4f80
/*  f151108:	44814000 */ 	mtc1	$at,$f8
/*  f15110c:	00000000 */ 	nop
/*  f151110:	46089480 */ 	add.s	$f18,$f18,$f8
.L0f151114:
/*  f151114:	46149182 */ 	mul.s	$f6,$f18,$f20
/*  f151118:	3c013f00 */ 	lui	$at,0x3f00
/*  f15111c:	44815000 */ 	mtc1	$at,$f10
/*  f151120:	3c013f80 */ 	lui	$at,0x3f80
/*  f151124:	44812000 */ 	mtc1	$at,$f4
/*  f151128:	c7b20044 */ 	lwc1	$f18,0x44($sp)
/*  f15112c:	8e890000 */ 	lw	$t1,0x0($s4)
/*  f151130:	460a3402 */ 	mul.s	$f16,$f6,$f10
/*  f151134:	8eb90000 */ 	lw	$t9,0x0($s5)
/*  f151138:	01360019 */ 	multu	$t1,$s6
/*  f15113c:	46048200 */ 	add.s	$f8,$f16,$f4
/*  f151140:	46124182 */ 	mul.s	$f6,$f8,$f18
/*  f151144:	00005812 */ 	mflo	$t3
/*  f151148:	032b7021 */ 	addu	$t6,$t9,$t3
/*  f15114c:	4600328d */ 	trunc.w.s	$f10,$f6
/*  f151150:	44085000 */ 	mfc1	$t0,$f10
/*  f151154:	0c004b70 */ 	jal	random
/*  f151158:	a5c80050 */ 	sh	$t0,0x50($t6)
/*  f15115c:	44828000 */ 	mtc1	$v0,$f16
/*  f151160:	04410005 */ 	bgez	$v0,.L0f151178
/*  f151164:	46808120 */ 	cvt.s.w	$f4,$f16
/*  f151168:	3c014f80 */ 	lui	$at,0x4f80
/*  f15116c:	44814000 */ 	mtc1	$at,$f8
/*  f151170:	00000000 */ 	nop
/*  f151174:	46082100 */ 	add.s	$f4,$f4,$f8
.L0f151178:
/*  f151178:	46142482 */ 	mul.s	$f18,$f4,$f20
/*  f15117c:	3c013f00 */ 	lui	$at,0x3f00
/*  f151180:	44813000 */ 	mtc1	$at,$f6
/*  f151184:	3c013f80 */ 	lui	$at,0x3f80
/*  f151188:	44818000 */ 	mtc1	$at,$f16
/*  f15118c:	c7a40044 */ 	lwc1	$f4,0x44($sp)
/*  f151190:	8e8d0000 */ 	lw	$t5,0x0($s4)
/*  f151194:	46069282 */ 	mul.s	$f10,$f18,$f6
/*  f151198:	8eb80000 */ 	lw	$t8,0x0($s5)
/*  f15119c:	01b60019 */ 	multu	$t5,$s6
/*  f1511a0:	00001825 */ 	or	$v1,$zero,$zero
/*  f1511a4:	00002025 */ 	or	$a0,$zero,$zero
/*  f1511a8:	2402000c */ 	addiu	$v0,$zero,0xc
/*  f1511ac:	24010002 */ 	addiu	$at,$zero,0x2
/*  f1511b0:	46105200 */ 	add.s	$f8,$f10,$f16
/*  f1511b4:	46044482 */ 	mul.s	$f18,$f8,$f4
/*  f1511b8:	00006012 */ 	mflo	$t4
/*  f1511bc:	030c4821 */ 	addu	$t1,$t8,$t4
/*  f1511c0:	4600918d */ 	trunc.w.s	$f6,$f18
/*  f1511c4:	440f3000 */ 	mfc1	$t7,$f6
/*  f1511c8:	00000000 */ 	nop
/*  f1511cc:	a52f0052 */ 	sh	$t7,0x52($t1)
.L0f1511d0:
/*  f1511d0:	8e8b0000 */ 	lw	$t3,0x0($s4)
/*  f1511d4:	8eb90000 */ 	lw	$t9,0x0($s5)
/*  f1511d8:	000b40c0 */ 	sll	$t0,$t3,0x3
/*  f1511dc:	010b4023 */ 	subu	$t0,$t0,$t3
/*  f1511e0:	00084080 */ 	sll	$t0,$t0,0x2
/*  f1511e4:	010b4023 */ 	subu	$t0,$t0,$t3
/*  f1511e8:	00084080 */ 	sll	$t0,$t0,0x2
/*  f1511ec:	03287021 */ 	addu	$t6,$t9,$t0
/*  f1511f0:	01c35021 */ 	addu	$t2,$t6,$v1
/*  f1511f4:	a540003c */ 	sh	$zero,0x3c($t2)
/*  f1511f8:	8e980000 */ 	lw	$t8,0x0($s4)
/*  f1511fc:	8ead0000 */ 	lw	$t5,0x0($s5)
/*  f151200:	001860c0 */ 	sll	$t4,$t8,0x3
/*  f151204:	01986023 */ 	subu	$t4,$t4,$t8
/*  f151208:	000c6080 */ 	sll	$t4,$t4,0x2
/*  f15120c:	01986023 */ 	subu	$t4,$t4,$t8
/*  f151210:	000c6080 */ 	sll	$t4,$t4,0x2
/*  f151214:	01ac7821 */ 	addu	$t7,$t5,$t4
/*  f151218:	01e34821 */ 	addu	$t1,$t7,$v1
/*  f15121c:	a5200040 */ 	sh	$zero,0x40($t1)
/*  f151220:	8e990000 */ 	lw	$t9,0x0($s4)
/*  f151224:	8eab0000 */ 	lw	$t3,0x0($s5)
/*  f151228:	001940c0 */ 	sll	$t0,$t9,0x3
/*  f15122c:	01194023 */ 	subu	$t0,$t0,$t9
/*  f151230:	00084080 */ 	sll	$t0,$t0,0x2
/*  f151234:	01194023 */ 	subu	$t0,$t0,$t9
/*  f151238:	00084080 */ 	sll	$t0,$t0,0x2
/*  f15123c:	01687021 */ 	addu	$t6,$t3,$t0
/*  f151240:	01c35021 */ 	addu	$t2,$t6,$v1
/*  f151244:	a5400042 */ 	sh	$zero,0x42($t2)
/*  f151248:	8e8d0000 */ 	lw	$t5,0x0($s4)
/*  f15124c:	8eb80000 */ 	lw	$t8,0x0($s5)
/*  f151250:	000d60c0 */ 	sll	$t4,$t5,0x3
/*  f151254:	018d6023 */ 	subu	$t4,$t4,$t5
/*  f151258:	000c6080 */ 	sll	$t4,$t4,0x2
/*  f15125c:	018d6023 */ 	subu	$t4,$t4,$t5
/*  f151260:	000c6080 */ 	sll	$t4,$t4,0x2
/*  f151264:	030c7821 */ 	addu	$t7,$t8,$t4
/*  f151268:	01e34821 */ 	addu	$t1,$t7,$v1
/*  f15126c:	a124003f */ 	sb	$a0,0x3f($t1)
/*  f151270:	24840004 */ 	addiu	$a0,$a0,0x4
/*  f151274:	1482ffd6 */ 	bne	$a0,$v0,.L0f1511d0
/*  f151278:	2463000c */ 	addiu	$v1,$v1,0xc
/*  f15127c:	56410067 */ 	bnel	$s2,$at,.L0f15141c
/*  f151280:	8e8b0000 */ 	lw	$t3,0x0($s4)
/*  f151284:	0c004b70 */ 	jal	random
/*  f151288:	00000000 */ 	nop
/*  f15128c:	24010064 */ 	addiu	$at,$zero,0x64
/*  f151290:	0041001b */ 	divu	$zero,$v0,$at
/*  f151294:	00001810 */ 	mfhi	$v1
/*  f151298:	28610014 */ 	slti	$at,$v1,0x14
/*  f15129c:	5020001b */ 	beqzl	$at,.L0f15130c
/*  f1512a0:	28610028 */ 	slti	$at,$v1,0x28
/*  f1512a4:	8e880000 */ 	lw	$t0,0x0($s4)
/*  f1512a8:	8eab0000 */ 	lw	$t3,0x0($s5)
/*  f1512ac:	3c19bbbb */ 	lui	$t9,0xbbbb
/*  f1512b0:	01160019 */ 	multu	$t0,$s6
/*  f1512b4:	3739bbf0 */ 	ori	$t9,$t9,0xbbf0
/*  f1512b8:	3c0daaaa */ 	lui	$t5,0xaaaa
/*  f1512bc:	35adaaf0 */ 	ori	$t5,$t5,0xaaf0
/*  f1512c0:	3c087777 */ 	lui	$t0,0x7777
/*  f1512c4:	350877f0 */ 	ori	$t0,$t0,0x77f0
/*  f1512c8:	00007012 */ 	mflo	$t6
/*  f1512cc:	016e5021 */ 	addu	$t2,$t3,$t6
/*  f1512d0:	ad59005c */ 	sw	$t9,0x5c($t2)
/*  f1512d4:	8e8c0000 */ 	lw	$t4,0x0($s4)
/*  f1512d8:	8eb80000 */ 	lw	$t8,0x0($s5)
/*  f1512dc:	01960019 */ 	multu	$t4,$s6
/*  f1512e0:	00007812 */ 	mflo	$t7
/*  f1512e4:	030f4821 */ 	addu	$t1,$t8,$t7
/*  f1512e8:	ad2d0060 */ 	sw	$t5,0x60($t1)
/*  f1512ec:	8e8e0000 */ 	lw	$t6,0x0($s4)
/*  f1512f0:	8eab0000 */ 	lw	$t3,0x0($s5)
/*  f1512f4:	01d60019 */ 	multu	$t6,$s6
/*  f1512f8:	0000c812 */ 	mflo	$t9
/*  f1512fc:	01795021 */ 	addu	$t2,$t3,$t9
/*  f151300:	100000ae */ 	b	.L0f1515bc
/*  f151304:	ad480064 */ 	sw	$t0,0x64($t2)
/*  f151308:	28610028 */ 	slti	$at,$v1,0x28
.L0f15130c:
/*  f15130c:	50200016 */ 	beqzl	$at,.L0f151368
/*  f151310:	2861003c */ 	slti	$at,$v1,0x3c
/*  f151314:	8e980000 */ 	lw	$t8,0x0($s4)
/*  f151318:	8eac0000 */ 	lw	$t4,0x0($s5)
/*  f15131c:	240200f0 */ 	addiu	$v0,$zero,0xf0
/*  f151320:	03160019 */ 	multu	$t8,$s6
/*  f151324:	00007812 */ 	mflo	$t7
/*  f151328:	018f6821 */ 	addu	$t5,$t4,$t7
/*  f15132c:	ada2005c */ 	sw	$v0,0x5c($t5)
/*  f151330:	8e8e0000 */ 	lw	$t6,0x0($s4)
/*  f151334:	8ea90000 */ 	lw	$t1,0x0($s5)
/*  f151338:	01d60019 */ 	multu	$t6,$s6
/*  f15133c:	00005812 */ 	mflo	$t3
/*  f151340:	012bc821 */ 	addu	$t9,$t1,$t3
/*  f151344:	af220060 */ 	sw	$v0,0x60($t9)
/*  f151348:	8e8a0000 */ 	lw	$t2,0x0($s4)
/*  f15134c:	8ea80000 */ 	lw	$t0,0x0($s5)
/*  f151350:	01560019 */ 	multu	$t2,$s6
/*  f151354:	0000c012 */ 	mflo	$t8
/*  f151358:	01186021 */ 	addu	$t4,$t0,$t8
/*  f15135c:	10000097 */ 	b	.L0f1515bc
/*  f151360:	ad820064 */ 	sw	$v0,0x64($t4)
/*  f151364:	2861003c */ 	slti	$at,$v1,0x3c
.L0f151368:
/*  f151368:	50200017 */ 	beqzl	$at,.L0f1513c8
/*  f15136c:	8e890000 */ 	lw	$t1,0x0($s4)
/*  f151370:	8e8d0000 */ 	lw	$t5,0x0($s4)
/*  f151374:	8eaf0000 */ 	lw	$t7,0x0($s5)
/*  f151378:	3c025533 */ 	lui	$v0,0x5533
/*  f15137c:	01b60019 */ 	multu	$t5,$s6
/*  f151380:	344211f0 */ 	ori	$v0,$v0,0x11f0
/*  f151384:	00007012 */ 	mflo	$t6
/*  f151388:	01ee4821 */ 	addu	$t1,$t7,$t6
/*  f15138c:	ad22005c */ 	sw	$v0,0x5c($t1)
/*  f151390:	8e990000 */ 	lw	$t9,0x0($s4)
/*  f151394:	8eab0000 */ 	lw	$t3,0x0($s5)
/*  f151398:	03360019 */ 	multu	$t9,$s6
/*  f15139c:	00005012 */ 	mflo	$t2
/*  f1513a0:	016a4021 */ 	addu	$t0,$t3,$t2
/*  f1513a4:	ad020060 */ 	sw	$v0,0x60($t0)
/*  f1513a8:	8e8c0000 */ 	lw	$t4,0x0($s4)
/*  f1513ac:	8eb80000 */ 	lw	$t8,0x0($s5)
/*  f1513b0:	01960019 */ 	multu	$t4,$s6
/*  f1513b4:	00006812 */ 	mflo	$t5
/*  f1513b8:	030d7821 */ 	addu	$t7,$t8,$t5
/*  f1513bc:	1000007f */ 	b	.L0f1515bc
/*  f1513c0:	ade20064 */ 	sw	$v0,0x64($t7)
/*  f1513c4:	8e890000 */ 	lw	$t1,0x0($s4)
.L0f1513c8:
/*  f1513c8:	8eae0000 */ 	lw	$t6,0x0($s5)
/*  f1513cc:	3c02ddaa */ 	lui	$v0,0xddaa
/*  f1513d0:	01360019 */ 	multu	$t1,$s6
/*  f1513d4:	344288f0 */ 	ori	$v0,$v0,0x88f0
/*  f1513d8:	0000c812 */ 	mflo	$t9
/*  f1513dc:	01d95821 */ 	addu	$t3,$t6,$t9
/*  f1513e0:	ad62005c */ 	sw	$v0,0x5c($t3)
/*  f1513e4:	8e880000 */ 	lw	$t0,0x0($s4)
/*  f1513e8:	8eaa0000 */ 	lw	$t2,0x0($s5)
/*  f1513ec:	01160019 */ 	multu	$t0,$s6
/*  f1513f0:	00006012 */ 	mflo	$t4
/*  f1513f4:	014cc021 */ 	addu	$t8,$t2,$t4
/*  f1513f8:	af020060 */ 	sw	$v0,0x60($t8)
/*  f1513fc:	8e8f0000 */ 	lw	$t7,0x0($s4)
/*  f151400:	8ead0000 */ 	lw	$t5,0x0($s5)
/*  f151404:	01f60019 */ 	multu	$t7,$s6
/*  f151408:	00004812 */ 	mflo	$t1
/*  f15140c:	01a97021 */ 	addu	$t6,$t5,$t1
/*  f151410:	1000006a */ 	b	.L0f1515bc
/*  f151414:	adc20064 */ 	sw	$v0,0x64($t6)
/*  f151418:	8e8b0000 */ 	lw	$t3,0x0($s4)
.L0f15141c:
/*  f15141c:	8eb90000 */ 	lw	$t9,0x0($s5)
/*  f151420:	24020005 */ 	addiu	$v0,$zero,0x5
/*  f151424:	01760019 */ 	multu	$t3,$s6
/*  f151428:	2403007e */ 	addiu	$v1,$zero,0x7e
/*  f15142c:	240400fb */ 	addiu	$a0,$zero,0xfb
/*  f151430:	00009025 */ 	or	$s2,$zero,$zero
/*  f151434:	241300ff */ 	addiu	$s3,$zero,0xff
/*  f151438:	24110003 */ 	addiu	$s1,$zero,0x3
/*  f15143c:	00004012 */ 	mflo	$t0
/*  f151440:	03285021 */ 	addu	$t2,$t9,$t0
/*  f151444:	a142005c */ 	sb	$v0,0x5c($t2)
/*  f151448:	8e980000 */ 	lw	$t8,0x0($s4)
/*  f15144c:	8eac0000 */ 	lw	$t4,0x0($s5)
/*  f151450:	03160019 */ 	multu	$t8,$s6
/*  f151454:	00007812 */ 	mflo	$t7
/*  f151458:	018f6821 */ 	addu	$t5,$t4,$t7
/*  f15145c:	a1a2005d */ 	sb	$v0,0x5d($t5)
/*  f151460:	8e8e0000 */ 	lw	$t6,0x0($s4)
/*  f151464:	8ea90000 */ 	lw	$t1,0x0($s5)
/*  f151468:	01d60019 */ 	multu	$t6,$s6
/*  f15146c:	00005812 */ 	mflo	$t3
/*  f151470:	012bc821 */ 	addu	$t9,$t1,$t3
/*  f151474:	a323005e */ 	sb	$v1,0x5e($t9)
/*  f151478:	8e8a0000 */ 	lw	$t2,0x0($s4)
/*  f15147c:	8ea80000 */ 	lw	$t0,0x0($s5)
/*  f151480:	01560019 */ 	multu	$t2,$s6
/*  f151484:	0000c012 */ 	mflo	$t8
/*  f151488:	01186021 */ 	addu	$t4,$t0,$t8
/*  f15148c:	a1820060 */ 	sb	$v0,0x60($t4)
/*  f151490:	8e8d0000 */ 	lw	$t5,0x0($s4)
/*  f151494:	8eaf0000 */ 	lw	$t7,0x0($s5)
/*  f151498:	01b60019 */ 	multu	$t5,$s6
/*  f15149c:	00007012 */ 	mflo	$t6
/*  f1514a0:	01ee4821 */ 	addu	$t1,$t7,$t6
/*  f1514a4:	a1240061 */ 	sb	$a0,0x61($t1)
/*  f1514a8:	8e990000 */ 	lw	$t9,0x0($s4)
/*  f1514ac:	8eab0000 */ 	lw	$t3,0x0($s5)
/*  f1514b0:	03360019 */ 	multu	$t9,$s6
/*  f1514b4:	00005012 */ 	mflo	$t2
/*  f1514b8:	016a4021 */ 	addu	$t0,$t3,$t2
/*  f1514bc:	a1030062 */ 	sb	$v1,0x62($t0)
/*  f1514c0:	8e8c0000 */ 	lw	$t4,0x0($s4)
/*  f1514c4:	8eb80000 */ 	lw	$t8,0x0($s5)
/*  f1514c8:	01960019 */ 	multu	$t4,$s6
/*  f1514cc:	00006812 */ 	mflo	$t5
/*  f1514d0:	030d7821 */ 	addu	$t7,$t8,$t5
/*  f1514d4:	a1e40064 */ 	sb	$a0,0x64($t7)
/*  f1514d8:	8e890000 */ 	lw	$t1,0x0($s4)
/*  f1514dc:	8eae0000 */ 	lw	$t6,0x0($s5)
/*  f1514e0:	01360019 */ 	multu	$t1,$s6
/*  f1514e4:	0000c812 */ 	mflo	$t9
/*  f1514e8:	01d95821 */ 	addu	$t3,$t6,$t9
/*  f1514ec:	a1640065 */ 	sb	$a0,0x65($t3)
/*  f1514f0:	8e880000 */ 	lw	$t0,0x0($s4)
/*  f1514f4:	8eaa0000 */ 	lw	$t2,0x0($s5)
/*  f1514f8:	01160019 */ 	multu	$t0,$s6
/*  f1514fc:	00006012 */ 	mflo	$t4
/*  f151500:	014cc021 */ 	addu	$t8,$t2,$t4
/*  f151504:	a3030066 */ 	sb	$v1,0x66($t8)
/*  f151508:	00008025 */ 	or	$s0,$zero,$zero
.L0f15150c:
/*  f15150c:	0c004b70 */ 	jal	random
/*  f151510:	00000000 */ 	nop
/*  f151514:	0053001b */ 	divu	$zero,$v0,$s3
/*  f151518:	8e890000 */ 	lw	$t1,0x0($s4)
/*  f15151c:	8eaf0000 */ 	lw	$t7,0x0($s5)
/*  f151520:	00125880 */ 	sll	$t3,$s2,0x2
/*  f151524:	000970c0 */ 	sll	$t6,$t1,0x3
/*  f151528:	01c97023 */ 	subu	$t6,$t6,$t1
/*  f15152c:	000e7080 */ 	sll	$t6,$t6,0x2
/*  f151530:	01c97023 */ 	subu	$t6,$t6,$t1
/*  f151534:	000e7080 */ 	sll	$t6,$t6,0x2
/*  f151538:	01eec821 */ 	addu	$t9,$t7,$t6
/*  f15153c:	032b4021 */ 	addu	$t0,$t9,$t3
/*  f151540:	01105021 */ 	addu	$t2,$t0,$s0
/*  f151544:	26100001 */ 	addiu	$s0,$s0,0x1
/*  f151548:	00006810 */ 	mfhi	$t5
/*  f15154c:	a14d005c */ 	sb	$t5,0x5c($t2)
/*  f151550:	16600002 */ 	bnez	$s3,.L0f15155c
/*  f151554:	00000000 */ 	nop
/*  f151558:	0007000d */ 	break	0x7
.L0f15155c:
/*  f15155c:	1611ffeb */ 	bne	$s0,$s1,.L0f15150c
/*  f151560:	00000000 */ 	nop
/*  f151564:	26520001 */ 	addiu	$s2,$s2,0x1
/*  f151568:	5651ffe8 */ 	bnel	$s2,$s1,.L0f15150c
/*  f15156c:	00008025 */ 	or	$s0,$zero,$zero
/*  f151570:	8e980000 */ 	lw	$t8,0x0($s4)
/*  f151574:	8eac0000 */ 	lw	$t4,0x0($s5)
/*  f151578:	03160019 */ 	multu	$t8,$s6
/*  f15157c:	00004812 */ 	mflo	$t1
/*  f151580:	01897821 */ 	addu	$t7,$t4,$t1
/*  f151584:	a1f30067 */ 	sb	$s3,0x67($t7)
/*  f151588:	8e990000 */ 	lw	$t9,0x0($s4)
/*  f15158c:	8eae0000 */ 	lw	$t6,0x0($s5)
/*  f151590:	03360019 */ 	multu	$t9,$s6
/*  f151594:	00005812 */ 	mflo	$t3
/*  f151598:	01cb1021 */ 	addu	$v0,$t6,$t3
/*  f15159c:	90430067 */ 	lbu	$v1,0x67($v0)
/*  f1515a0:	a0430063 */ 	sb	$v1,0x63($v0)
/*  f1515a4:	8e8d0000 */ 	lw	$t5,0x0($s4)
/*  f1515a8:	8ea80000 */ 	lw	$t0,0x0($s5)
/*  f1515ac:	01b60019 */ 	multu	$t5,$s6
/*  f1515b0:	00005012 */ 	mflo	$t2
/*  f1515b4:	010ac021 */ 	addu	$t8,$t0,$t2
/*  f1515b8:	a303005f */ 	sb	$v1,0x5f($t8)
.L0f1515bc:
/*  f1515bc:	8e890000 */ 	lw	$t1,0x0($s4)
/*  f1515c0:	8eac0000 */ 	lw	$t4,0x0($s5)
/*  f1515c4:	c7aa0070 */ 	lwc1	$f10,0x70($sp)
/*  f1515c8:	01360019 */ 	multu	$t1,$s6
/*  f1515cc:	44808000 */ 	mtc1	$zero,$f16
/*  f1515d0:	00007812 */ 	mflo	$t7
/*  f1515d4:	018fc821 */ 	addu	$t9,$t4,$t7
/*  f1515d8:	e72a0014 */ 	swc1	$f10,0x14($t9)
/*  f1515dc:	8e8b0000 */ 	lw	$t3,0x0($s4)
/*  f1515e0:	8eae0000 */ 	lw	$t6,0x0($s5)
/*  f1515e4:	01760019 */ 	multu	$t3,$s6
/*  f1515e8:	00006812 */ 	mflo	$t5
/*  f1515ec:	01cd4021 */ 	addu	$t0,$t6,$t5
/*  f1515f0:	0c004b70 */ 	jal	random
/*  f1515f4:	e5100018 */ 	swc1	$f16,0x18($t0)
/*  f1515f8:	44824000 */ 	mtc1	$v0,$f8
/*  f1515fc:	3c014f80 */ 	lui	$at,0x4f80
/*  f151600:	04410004 */ 	bgez	$v0,.L0f151614
/*  f151604:	46804120 */ 	cvt.s.w	$f4,$f8
/*  f151608:	44819000 */ 	mtc1	$at,$f18
/*  f15160c:	00000000 */ 	nop
/*  f151610:	46122100 */ 	add.s	$f4,$f4,$f18
.L0f151614:
/*  f151614:	8e980000 */ 	lw	$t8,0x0($s4)
/*  f151618:	46142182 */ 	mul.s	$f6,$f4,$f20
/*  f15161c:	3c017f1b */ 	lui	$at,%hi(var7f1b7380)
/*  f151620:	03160019 */ 	multu	$t8,$s6
/*  f151624:	c42a7380 */ 	lwc1	$f10,%lo(var7f1b7380)($at)
/*  f151628:	8eaa0000 */ 	lw	$t2,0x0($s5)
/*  f15162c:	460a3402 */ 	mul.s	$f16,$f6,$f10
/*  f151630:	00004812 */ 	mflo	$t1
/*  f151634:	01496021 */ 	addu	$t4,$t2,$t1
/*  f151638:	0c004b70 */ 	jal	random
/*  f15163c:	e590001c */ 	swc1	$f16,0x1c($t4)
/*  f151640:	44824000 */ 	mtc1	$v0,$f8
/*  f151644:	3c014f80 */ 	lui	$at,0x4f80
/*  f151648:	04410004 */ 	bgez	$v0,.L0f15165c
/*  f15164c:	468044a0 */ 	cvt.s.w	$f18,$f8
/*  f151650:	44812000 */ 	mtc1	$at,$f4
/*  f151654:	00000000 */ 	nop
/*  f151658:	46049480 */ 	add.s	$f18,$f18,$f4
.L0f15165c:
/*  f15165c:	8e990000 */ 	lw	$t9,0x0($s4)
/*  f151660:	46149182 */ 	mul.s	$f6,$f18,$f20
/*  f151664:	3c017f1b */ 	lui	$at,%hi(var7f1b7384)
/*  f151668:	03360019 */ 	multu	$t9,$s6
/*  f15166c:	c42a7384 */ 	lwc1	$f10,%lo(var7f1b7384)($at)
/*  f151670:	8eaf0000 */ 	lw	$t7,0x0($s5)
/*  f151674:	460a3402 */ 	mul.s	$f16,$f6,$f10
/*  f151678:	00005812 */ 	mflo	$t3
/*  f15167c:	01eb7021 */ 	addu	$t6,$t7,$t3
/*  f151680:	0c004b70 */ 	jal	random
/*  f151684:	e5d0002c */ 	swc1	$f16,0x2c($t6)
/*  f151688:	44824000 */ 	mtc1	$v0,$f8
/*  f15168c:	3c014f80 */ 	lui	$at,0x4f80
/*  f151690:	04410004 */ 	bgez	$v0,.L0f1516a4
/*  f151694:	46804120 */ 	cvt.s.w	$f4,$f8
/*  f151698:	44819000 */ 	mtc1	$at,$f18
/*  f15169c:	00000000 */ 	nop
/*  f1516a0:	46122100 */ 	add.s	$f4,$f4,$f18
.L0f1516a4:
/*  f1516a4:	8e880000 */ 	lw	$t0,0x0($s4)
/*  f1516a8:	46142182 */ 	mul.s	$f6,$f4,$f20
/*  f1516ac:	3c017f1b */ 	lui	$at,%hi(var7f1b7388)
/*  f1516b0:	01160019 */ 	multu	$t0,$s6
/*  f1516b4:	c42a7388 */ 	lwc1	$f10,%lo(var7f1b7388)($at)
/*  f1516b8:	8ead0000 */ 	lw	$t5,0x0($s5)
/*  f1516bc:	460a3402 */ 	mul.s	$f16,$f6,$f10
/*  f1516c0:	0000c012 */ 	mflo	$t8
/*  f1516c4:	01b85021 */ 	addu	$t2,$t5,$t8
/*  f1516c8:	0c004b70 */ 	jal	random
/*  f1516cc:	e5500030 */ 	swc1	$f16,0x30($t2)
/*  f1516d0:	44824000 */ 	mtc1	$v0,$f8
/*  f1516d4:	3c014f80 */ 	lui	$at,0x4f80
/*  f1516d8:	04410004 */ 	bgez	$v0,.L0f1516ec
/*  f1516dc:	468044a0 */ 	cvt.s.w	$f18,$f8
/*  f1516e0:	44812000 */ 	mtc1	$at,$f4
/*  f1516e4:	00000000 */ 	nop
/*  f1516e8:	46049480 */ 	add.s	$f18,$f18,$f4
.L0f1516ec:
/*  f1516ec:	8e8c0000 */ 	lw	$t4,0x0($s4)
/*  f1516f0:	46149182 */ 	mul.s	$f6,$f18,$f20
/*  f1516f4:	3c017f1b */ 	lui	$at,%hi(var7f1b738c)
/*  f1516f8:	01960019 */ 	multu	$t4,$s6
/*  f1516fc:	c42a738c */ 	lwc1	$f10,%lo(var7f1b738c)($at)
/*  f151700:	8ea90000 */ 	lw	$t1,0x0($s5)
/*  f151704:	3c0d800a */ 	lui	$t5,%hi(var800a45c0)
/*  f151708:	3c028008 */ 	lui	$v0,%hi(var8007fa84)
/*  f15170c:	460a3402 */ 	mul.s	$f16,$f6,$f10
/*  f151710:	2442fa84 */ 	addiu	$v0,$v0,%lo(var8007fa84)
/*  f151714:	240a0001 */ 	addiu	$t2,$zero,0x1
/*  f151718:	0000c812 */ 	mflo	$t9
/*  f15171c:	01397821 */ 	addu	$t7,$t1,$t9
/*  f151720:	e5f00034 */ 	swc1	$f16,0x34($t7)
/*  f151724:	8e8b0000 */ 	lw	$t3,0x0($s4)
/*  f151728:	256e0001 */ 	addiu	$t6,$t3,0x1
/*  f15172c:	ae8e0000 */ 	sw	$t6,0x0($s4)
/*  f151730:	8dad45c0 */ 	lw	$t5,%lo(var800a45c0)($t5)
/*  f151734:	01cd082a */ 	slt	$at,$t6,$t5
/*  f151738:	54200003 */ 	bnezl	$at,.L0f151748
/*  f15173c:	8c580000 */ 	lw	$t8,0x0($v0)
/*  f151740:	ae800000 */ 	sw	$zero,0x0($s4)
/*  f151744:	8c580000 */ 	lw	$t8,0x0($v0)
.L0f151748:
/*  f151748:	57000003 */ 	bnezl	$t8,.L0f151758
/*  f15174c:	8fbf003c */ 	lw	$ra,0x3c($sp)
/*  f151750:	ac4a0000 */ 	sw	$t2,0x0($v0)
/*  f151754:	8fbf003c */ 	lw	$ra,0x3c($sp)
.L0f151758:
/*  f151758:	d7b40018 */ 	ldc1	$f20,0x18($sp)
/*  f15175c:	8fb00020 */ 	lw	$s0,0x20($sp)
/*  f151760:	8fb10024 */ 	lw	$s1,0x24($sp)
/*  f151764:	8fb20028 */ 	lw	$s2,0x28($sp)
/*  f151768:	8fb3002c */ 	lw	$s3,0x2c($sp)
/*  f15176c:	8fb40030 */ 	lw	$s4,0x30($sp)
/*  f151770:	8fb50034 */ 	lw	$s5,0x34($sp)
/*  f151774:	8fb60038 */ 	lw	$s6,0x38($sp)
/*  f151778:	03e00008 */ 	jr	$ra
/*  f15177c:	27bd0068 */ 	addiu	$sp,$sp,0x68
);

GLOBAL_ASM(
glabel func0f151780
/*  f151780:	27bdfed0 */ 	addiu	$sp,$sp,-304
/*  f151784:	3c0e8008 */ 	lui	$t6,%hi(var8007fa84)
/*  f151788:	8dcefa84 */ 	lw	$t6,%lo(var8007fa84)($t6)
/*  f15178c:	afb30054 */ 	sw	$s3,0x54($sp)
/*  f151790:	00809825 */ 	or	$s3,$a0,$zero
/*  f151794:	afbf006c */ 	sw	$ra,0x6c($sp)
/*  f151798:	afbe0068 */ 	sw	$s8,0x68($sp)
/*  f15179c:	afb70064 */ 	sw	$s7,0x64($sp)
/*  f1517a0:	afb60060 */ 	sw	$s6,0x60($sp)
/*  f1517a4:	afb5005c */ 	sw	$s5,0x5c($sp)
/*  f1517a8:	afb40058 */ 	sw	$s4,0x58($sp)
/*  f1517ac:	afb20050 */ 	sw	$s2,0x50($sp)
/*  f1517b0:	afb1004c */ 	sw	$s1,0x4c($sp)
/*  f1517b4:	afb00048 */ 	sw	$s0,0x48($sp)
/*  f1517b8:	f7be0040 */ 	sdc1	$f30,0x40($sp)
/*  f1517bc:	f7bc0038 */ 	sdc1	$f28,0x38($sp)
/*  f1517c0:	f7ba0030 */ 	sdc1	$f26,0x30($sp)
/*  f1517c4:	f7b80028 */ 	sdc1	$f24,0x28($sp)
/*  f1517c8:	f7b60020 */ 	sdc1	$f22,0x20($sp)
/*  f1517cc:	11c001ad */ 	beqz	$t6,.L0f151e84
/*  f1517d0:	f7b40018 */ 	sdc1	$f20,0x18($sp)
/*  f1517d4:	a7a0012e */ 	sh	$zero,0x12e($sp)
/*  f1517d8:	24930008 */ 	addiu	$s3,$a0,0x8
/*  f1517dc:	3c0fb600 */ 	lui	$t7,0xb600
/*  f1517e0:	24183000 */ 	addiu	$t8,$zero,0x3000
/*  f1517e4:	ac980004 */ 	sw	$t8,0x4($a0)
/*  f1517e8:	ac8f0000 */ 	sw	$t7,0x0($a0)
/*  f1517ec:	02601825 */ 	or	$v1,$s3,$zero
/*  f1517f0:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f1517f4:	3c19b700 */ 	lui	$t9,0xb700
/*  f1517f8:	ac790000 */ 	sw	$t9,0x0($v1)
/*  f1517fc:	24080204 */ 	addiu	$t0,$zero,0x204
/*  f151800:	02602025 */ 	or	$a0,$s3,$zero
/*  f151804:	ac680004 */ 	sw	$t0,0x4($v1)
/*  f151808:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f15180c:	3c09fcff */ 	lui	$t1,0xfcff
/*  f151810:	3c0afffe */ 	lui	$t2,0xfffe
/*  f151814:	354a793c */ 	ori	$t2,$t2,0x793c
/*  f151818:	3529ffff */ 	ori	$t1,$t1,0xffff
/*  f15181c:	02602825 */ 	or	$a1,$s3,$zero
/*  f151820:	ac890000 */ 	sw	$t1,0x0($a0)
/*  f151824:	ac8a0004 */ 	sw	$t2,0x4($a0)
/*  f151828:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f15182c:	3c0bba00 */ 	lui	$t3,0xba00
/*  f151830:	356b0c02 */ 	ori	$t3,$t3,0xc02
/*  f151834:	240c2000 */ 	addiu	$t4,$zero,0x2000
/*  f151838:	02603025 */ 	or	$a2,$s3,$zero
/*  f15183c:	acac0004 */ 	sw	$t4,0x4($a1)
/*  f151840:	acab0000 */ 	sw	$t3,0x0($a1)
/*  f151844:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151848:	3c0dba00 */ 	lui	$t5,0xba00
/*  f15184c:	35ad1402 */ 	ori	$t5,$t5,0x1402
/*  f151850:	3c0e0010 */ 	lui	$t6,0x10
/*  f151854:	02601025 */ 	or	$v0,$s3,$zero
/*  f151858:	acce0004 */ 	sw	$t6,0x4($a2)
/*  f15185c:	accd0000 */ 	sw	$t5,0x0($a2)
/*  f151860:	3c0fba00 */ 	lui	$t7,0xba00
/*  f151864:	35ef1001 */ 	ori	$t7,$t7,0x1001
/*  f151868:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f15186c:	3c180001 */ 	lui	$t8,0x1
/*  f151870:	3c190103 */ 	lui	$t9,0x103
/*  f151874:	3c17800a */ 	lui	$s7,%hi(g_Vars)
/*  f151878:	ac580004 */ 	sw	$t8,0x4($v0)
/*  f15187c:	ac4f0000 */ 	sw	$t7,0x0($v0)
/*  f151880:	37390040 */ 	ori	$t9,$t9,0x40
/*  f151884:	02608025 */ 	or	$s0,$s3,$zero
/*  f151888:	26f79fc0 */ 	addiu	$s7,$s7,%lo(g_Vars)
/*  f15188c:	ae190000 */ 	sw	$t9,0x0($s0)
/*  f151890:	0fc2d402 */ 	jal	currentPlayerGetUnk1758
/*  f151894:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151898:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f15189c:	00402025 */ 	or	$a0,$v0,$zero
/*  f1518a0:	ae020004 */ 	sw	$v0,0x4($s0)
/*  f1518a4:	3c08800a */ 	lui	$t0,%hi(var800a45c0)
/*  f1518a8:	8d0845c0 */ 	lw	$t0,%lo(var800a45c0)($t0)
/*  f1518ac:	0000f025 */ 	or	$s8,$zero,$zero
/*  f1518b0:	0000a025 */ 	or	$s4,$zero,$zero
/*  f1518b4:	19000159 */ 	blez	$t0,.L0f151e1c
/*  f1518b8:	3c014316 */ 	lui	$at,0x4316
/*  f1518bc:	4481f000 */ 	mtc1	$at,$f30
/*  f1518c0:	3c017f1b */ 	lui	$at,%hi(var7f1b7390)
/*  f1518c4:	c43c7390 */ 	lwc1	$f28,%lo(var7f1b7390)($at)
/*  f1518c8:	3c017f1b */ 	lui	$at,%hi(var7f1b7394)
/*  f1518cc:	c43a7394 */ 	lwc1	$f26,%lo(var7f1b7394)($at)
/*  f1518d0:	3c013f80 */ 	lui	$at,0x3f80
/*  f1518d4:	3c158008 */ 	lui	$s5,%hi(var8007fa80)
/*  f1518d8:	3c12800a */ 	lui	$s2,%hi(var800a45c4)
/*  f1518dc:	4481c000 */ 	mtc1	$at,$f24
/*  f1518e0:	265245c4 */ 	addiu	$s2,$s2,%lo(var800a45c4)
/*  f1518e4:	26b5fa80 */ 	addiu	$s5,$s5,%lo(var8007fa80)
/*  f1518e8:	c7b600bc */ 	lwc1	$f22,0xbc($sp)
/*  f1518ec:	2416000c */ 	addiu	$s6,$zero,0xc
/*  f1518f0:	8e490000 */ 	lw	$t1,0x0($s2)
.L0f1518f4:
/*  f1518f4:	01341021 */ 	addu	$v0,$t1,$s4
/*  f1518f8:	8c4a0004 */ 	lw	$t2,0x4($v0)
/*  f1518fc:	1940013f */ 	blez	$t2,.L0f151dfc
/*  f151900:	00000000 */ 	nop
/*  f151904:	904b0068 */ 	lbu	$t3,0x68($v0)
/*  f151908:	24010002 */ 	addiu	$at,$zero,0x2
/*  f15190c:	24100001 */ 	addiu	$s0,$zero,0x1
/*  f151910:	1561013a */ 	bne	$t3,$at,.L0f151dfc
/*  f151914:	00000000 */ 	nop
/*  f151918:	0fc59e66 */ 	jal	func0f167998
/*  f15191c:	4600c506 */ 	mov.s	$f20,$f24
/*  f151920:	8ee30284 */ 	lw	$v1,0x284($s7)
/*  f151924:	24010001 */ 	addiu	$at,$zero,0x1
/*  f151928:	00408825 */ 	or	$s1,$v0,$zero
/*  f15192c:	946c0010 */ 	lhu	$t4,0x10($v1)
/*  f151930:	15810025 */ 	bne	$t4,$at,.L0f1519c8
/*  f151934:	00000000 */ 	nop
/*  f151938:	8e4d0000 */ 	lw	$t5,0x0($s2)
/*  f15193c:	c46a0018 */ 	lwc1	$f10,0x18($v1)
/*  f151940:	c4660020 */ 	lwc1	$f6,0x20($v1)
/*  f151944:	01b41021 */ 	addu	$v0,$t5,$s4
/*  f151948:	c4480008 */ 	lwc1	$f8,0x8($v0)
/*  f15194c:	c4440010 */ 	lwc1	$f4,0x10($v0)
/*  f151950:	c450000c */ 	lwc1	$f16,0xc($v0)
/*  f151954:	460a4081 */ 	sub.s	$f2,$f8,$f10
/*  f151958:	c472001c */ 	lwc1	$f18,0x1c($v1)
/*  f15195c:	46062001 */ 	sub.s	$f0,$f4,$f6
/*  f151960:	46021102 */ 	mul.s	$f4,$f2,$f2
/*  f151964:	46128381 */ 	sub.s	$f14,$f16,$f18
/*  f151968:	460e7182 */ 	mul.s	$f6,$f14,$f14
/*  f15196c:	46062200 */ 	add.s	$f8,$f4,$f6
/*  f151970:	46000282 */ 	mul.s	$f10,$f0,$f0
/*  f151974:	0c012974 */ 	jal	sqrtf
/*  f151978:	46085300 */ 	add.s	$f12,$f10,$f8
/*  f15197c:	8eee0284 */ 	lw	$t6,0x284($s7)
/*  f151980:	c5c20024 */ 	lwc1	$f2,0x24($t6)
/*  f151984:	4600103c */ 	c.lt.s	$f2,$f0
/*  f151988:	461e1301 */ 	sub.s	$f12,$f2,$f30
/*  f15198c:	45000002 */ 	bc1f	.L0f151998
/*  f151990:	46020583 */ 	div.s	$f22,$f0,$f2
/*  f151994:	00008025 */ 	or	$s0,$zero,$zero
.L0f151998:
/*  f151998:	4600603c */ 	c.lt.s	$f12,$f0
/*  f15199c:	00000000 */ 	nop
/*  f1519a0:	45020005 */ 	bc1fl	.L0f1519b8
/*  f1519a4:	4616c03c */ 	c.lt.s	$f24,$f22
/*  f1519a8:	460c0401 */ 	sub.s	$f16,$f0,$f12
/*  f1519ac:	461e8483 */ 	div.s	$f18,$f16,$f30
/*  f1519b0:	4612c501 */ 	sub.s	$f20,$f24,$f18
/*  f1519b4:	4616c03c */ 	c.lt.s	$f24,$f22
.L0f1519b8:
/*  f1519b8:	00000000 */ 	nop
/*  f1519bc:	45000002 */ 	bc1f	.L0f1519c8
/*  f1519c0:	00000000 */ 	nop
/*  f1519c4:	4600c586 */ 	mov.s	$f22,$f24
.L0f1519c8:
/*  f1519c8:	1200010c */ 	beqz	$s0,.L0f151dfc
/*  f1519cc:	00000000 */ 	nop
/*  f1519d0:	8e4f0000 */ 	lw	$t7,0x0($s2)
/*  f1519d4:	27a600e8 */ 	addiu	$a2,$sp,0xe8
/*  f1519d8:	01f41021 */ 	addu	$v0,$t7,$s4
/*  f1519dc:	24440008 */ 	addiu	$a0,$v0,0x8
/*  f1519e0:	0c0059a9 */ 	jal	func000166a4
/*  f1519e4:	24450014 */ 	addiu	$a1,$v0,0x14
/*  f1519e8:	8ee30284 */ 	lw	$v1,0x284($s7)
/*  f1519ec:	c7a40118 */ 	lwc1	$f4,0x118($sp)
/*  f1519f0:	c7a8011c */ 	lwc1	$f8,0x11c($sp)
/*  f1519f4:	c4660038 */ 	lwc1	$f6,0x38($v1)
/*  f1519f8:	46062281 */ 	sub.s	$f10,$f4,$f6
/*  f1519fc:	c7a40120 */ 	lwc1	$f4,0x120($sp)
/*  f151a00:	e7aa0118 */ 	swc1	$f10,0x118($sp)
/*  f151a04:	c470003c */ 	lwc1	$f16,0x3c($v1)
/*  f151a08:	461a503c */ 	c.lt.s	$f10,$f26
/*  f151a0c:	46104481 */ 	sub.s	$f18,$f8,$f16
/*  f151a10:	e7b2011c */ 	swc1	$f18,0x11c($sp)
/*  f151a14:	c4660040 */ 	lwc1	$f6,0x40($v1)
/*  f151a18:	46062201 */ 	sub.s	$f8,$f4,$f6
/*  f151a1c:	450000f7 */ 	bc1f	.L0f151dfc
/*  f151a20:	e7a80120 */ 	swc1	$f8,0x120($sp)
/*  f151a24:	460ae03c */ 	c.lt.s	$f28,$f10
/*  f151a28:	00000000 */ 	nop
/*  f151a2c:	450000f3 */ 	bc1f	.L0f151dfc
/*  f151a30:	00000000 */ 	nop
/*  f151a34:	461a903c */ 	c.lt.s	$f18,$f26
/*  f151a38:	00000000 */ 	nop
/*  f151a3c:	450000ef */ 	bc1f	.L0f151dfc
/*  f151a40:	00000000 */ 	nop
/*  f151a44:	4612e03c */ 	c.lt.s	$f28,$f18
/*  f151a48:	00000000 */ 	nop
/*  f151a4c:	450000eb */ 	bc1f	.L0f151dfc
/*  f151a50:	00000000 */ 	nop
/*  f151a54:	461a403c */ 	c.lt.s	$f8,$f26
/*  f151a58:	00000000 */ 	nop
/*  f151a5c:	450000e7 */ 	bc1f	.L0f151dfc
/*  f151a60:	00000000 */ 	nop
/*  f151a64:	4608e03c */ 	c.lt.s	$f28,$f8
/*  f151a68:	27a400e8 */ 	addiu	$a0,$sp,0xe8
/*  f151a6c:	450000e3 */ 	bc1f	.L0f151dfc
/*  f151a70:	00000000 */ 	nop
/*  f151a74:	0c005815 */ 	jal	func00016054
/*  f151a78:	02202825 */ 	or	$a1,$s1,$zero
/*  f151a7c:	3c180102 */ 	lui	$t8,0x102
/*  f151a80:	37180040 */ 	ori	$t8,$t8,0x40
/*  f151a84:	02608025 */ 	or	$s0,$s3,$zero
/*  f151a88:	ae180000 */ 	sw	$t8,0x0($s0)
/*  f151a8c:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151a90:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f151a94:	02202025 */ 	or	$a0,$s1,$zero
/*  f151a98:	ae020004 */ 	sw	$v0,0x4($s0)
/*  f151a9c:	8ef90284 */ 	lw	$t9,0x284($s7)
/*  f151aa0:	24010001 */ 	addiu	$at,$zero,0x1
/*  f151aa4:	97280010 */ 	lhu	$t0,0x10($t9)
/*  f151aa8:	150100ad */ 	bne	$t0,$at,.L0f151d60
/*  f151aac:	3c01437f */ 	lui	$at,0x437f
/*  f151ab0:	44818000 */ 	mtc1	$at,$f16
/*  f151ab4:	24100001 */ 	addiu	$s0,$zero,0x1
/*  f151ab8:	3c014f00 */ 	lui	$at,0x4f00
/*  f151abc:	4610b102 */ 	mul.s	$f4,$f22,$f16
/*  f151ac0:	24040003 */ 	addiu	$a0,$zero,0x3
/*  f151ac4:	4449f800 */ 	cfc1	$t1,$31
/*  f151ac8:	44d0f800 */ 	ctc1	$s0,$31
/*  f151acc:	00000000 */ 	nop
/*  f151ad0:	460021a4 */ 	cvt.w.s	$f6,$f4
/*  f151ad4:	4450f800 */ 	cfc1	$s0,$31
/*  f151ad8:	00000000 */ 	nop
/*  f151adc:	32100078 */ 	andi	$s0,$s0,0x78
/*  f151ae0:	52000013 */ 	beqzl	$s0,.L0f151b30
/*  f151ae4:	44103000 */ 	mfc1	$s0,$f6
/*  f151ae8:	44813000 */ 	mtc1	$at,$f6
/*  f151aec:	24100001 */ 	addiu	$s0,$zero,0x1
/*  f151af0:	46062181 */ 	sub.s	$f6,$f4,$f6
/*  f151af4:	44d0f800 */ 	ctc1	$s0,$31
/*  f151af8:	00000000 */ 	nop
/*  f151afc:	460031a4 */ 	cvt.w.s	$f6,$f6
/*  f151b00:	4450f800 */ 	cfc1	$s0,$31
/*  f151b04:	00000000 */ 	nop
/*  f151b08:	32100078 */ 	andi	$s0,$s0,0x78
/*  f151b0c:	16000005 */ 	bnez	$s0,.L0f151b24
/*  f151b10:	00000000 */ 	nop
/*  f151b14:	44103000 */ 	mfc1	$s0,$f6
/*  f151b18:	3c018000 */ 	lui	$at,0x8000
/*  f151b1c:	10000007 */ 	b	.L0f151b3c
/*  f151b20:	02018025 */ 	or	$s0,$s0,$at
.L0f151b24:
/*  f151b24:	10000005 */ 	b	.L0f151b3c
/*  f151b28:	2410ffff */ 	addiu	$s0,$zero,-1
/*  f151b2c:	44103000 */ 	mfc1	$s0,$f6
.L0f151b30:
/*  f151b30:	00000000 */ 	nop
/*  f151b34:	0600fffb */ 	bltz	$s0,.L0f151b24
/*  f151b38:	00000000 */ 	nop
.L0f151b3c:
/*  f151b3c:	44c9f800 */ 	ctc1	$t1,$31
/*  f151b40:	3c01437f */ 	lui	$at,0x437f
/*  f151b44:	44819000 */ 	mtc1	$at,$f18
/*  f151b48:	4616c281 */ 	sub.s	$f10,$f24,$f22
/*  f151b4c:	24110001 */ 	addiu	$s1,$zero,0x1
/*  f151b50:	3c014f00 */ 	lui	$at,0x4f00
/*  f151b54:	46125202 */ 	mul.s	$f8,$f10,$f18
/*  f151b58:	444af800 */ 	cfc1	$t2,$31
/*  f151b5c:	44d1f800 */ 	ctc1	$s1,$31
/*  f151b60:	00000000 */ 	nop
/*  f151b64:	46004424 */ 	cvt.w.s	$f16,$f8
/*  f151b68:	4451f800 */ 	cfc1	$s1,$31
/*  f151b6c:	00000000 */ 	nop
/*  f151b70:	32310078 */ 	andi	$s1,$s1,0x78
/*  f151b74:	52200013 */ 	beqzl	$s1,.L0f151bc4
/*  f151b78:	44118000 */ 	mfc1	$s1,$f16
/*  f151b7c:	44818000 */ 	mtc1	$at,$f16
/*  f151b80:	24110001 */ 	addiu	$s1,$zero,0x1
/*  f151b84:	46104401 */ 	sub.s	$f16,$f8,$f16
/*  f151b88:	44d1f800 */ 	ctc1	$s1,$31
/*  f151b8c:	00000000 */ 	nop
/*  f151b90:	46008424 */ 	cvt.w.s	$f16,$f16
/*  f151b94:	4451f800 */ 	cfc1	$s1,$31
/*  f151b98:	00000000 */ 	nop
/*  f151b9c:	32310078 */ 	andi	$s1,$s1,0x78
/*  f151ba0:	16200005 */ 	bnez	$s1,.L0f151bb8
/*  f151ba4:	00000000 */ 	nop
/*  f151ba8:	44118000 */ 	mfc1	$s1,$f16
/*  f151bac:	3c018000 */ 	lui	$at,0x8000
/*  f151bb0:	10000007 */ 	b	.L0f151bd0
/*  f151bb4:	02218825 */ 	or	$s1,$s1,$at
.L0f151bb8:
/*  f151bb8:	10000005 */ 	b	.L0f151bd0
/*  f151bbc:	2411ffff */ 	addiu	$s1,$zero,-1
/*  f151bc0:	44118000 */ 	mfc1	$s1,$f16
.L0f151bc4:
/*  f151bc4:	00000000 */ 	nop
/*  f151bc8:	0620fffb */ 	bltz	$s1,.L0f151bb8
/*  f151bcc:	00000000 */ 	nop
.L0f151bd0:
/*  f151bd0:	44caf800 */ 	ctc1	$t2,$31
/*  f151bd4:	0fc59e73 */ 	jal	func0f1679cc
/*  f151bd8:	00000000 */ 	nop
/*  f151bdc:	8e4b0000 */ 	lw	$t3,0x0($s2)
/*  f151be0:	00403025 */ 	or	$a2,$v0,$zero
/*  f151be4:	01746021 */ 	addu	$t4,$t3,$s4
/*  f151be8:	8d830004 */ 	lw	$v1,0x4($t4)
/*  f151bec:	3c0b0708 */ 	lui	$t3,0x708
/*  f151bf0:	356b000c */ 	ori	$t3,$t3,0xc
/*  f151bf4:	28610064 */ 	slti	$at,$v1,0x64
/*  f151bf8:	54200013 */ 	bnezl	$at,.L0f151c48
/*  f151bfc:	3c013f00 */ 	lui	$at,0x3f00
/*  f151c00:	44832000 */ 	mtc1	$v1,$f4
/*  f151c04:	3c014248 */ 	lui	$at,0x4248
/*  f151c08:	44815000 */ 	mtc1	$at,$f10
/*  f151c0c:	468021a0 */ 	cvt.s.w	$f6,$f4
/*  f151c10:	3c013f80 */ 	lui	$at,0x3f80
/*  f151c14:	44819000 */ 	mtc1	$at,$f18
/*  f151c18:	3c013f80 */ 	lui	$at,0x3f80
/*  f151c1c:	460a3003 */ 	div.s	$f0,$f6,$f10
/*  f151c20:	4600903c */ 	c.lt.s	$f18,$f0
/*  f151c24:	00000000 */ 	nop
/*  f151c28:	45020004 */ 	bc1fl	.L0f151c3c
/*  f151c2c:	4600c201 */ 	sub.s	$f8,$f24,$f0
/*  f151c30:	44810000 */ 	mtc1	$at,$f0
/*  f151c34:	00000000 */ 	nop
/*  f151c38:	4600c201 */ 	sub.s	$f8,$f24,$f0
.L0f151c3c:
/*  f151c3c:	46144502 */ 	mul.s	$f20,$f8,$f20
/*  f151c40:	00000000 */ 	nop
/*  f151c44:	3c013f00 */ 	lui	$at,0x3f00
.L0f151c48:
/*  f151c48:	44818000 */ 	mtc1	$at,$f16
/*  f151c4c:	00002025 */ 	or	$a0,$zero,$zero
/*  f151c50:	00401825 */ 	or	$v1,$v0,$zero
/*  f151c54:	4610a502 */ 	mul.s	$f20,$f20,$f16
/*  f151c58:	00000000 */ 	nop
.L0f151c5c:
/*  f151c5c:	a0700000 */ 	sb	$s0,0x0($v1)
/*  f151c60:	a0710001 */ 	sb	$s1,0x1($v1)
/*  f151c64:	a0600002 */ 	sb	$zero,0x2($v1)
/*  f151c68:	8eae0000 */ 	lw	$t6,0x0($s5)
/*  f151c6c:	8e4d0000 */ 	lw	$t5,0x0($s2)
/*  f151c70:	3c014f80 */ 	lui	$at,0x4f80
/*  f151c74:	000e78c0 */ 	sll	$t7,$t6,0x3
/*  f151c78:	01ee7823 */ 	subu	$t7,$t7,$t6
/*  f151c7c:	000f7880 */ 	sll	$t7,$t7,0x2
/*  f151c80:	01ee7823 */ 	subu	$t7,$t7,$t6
/*  f151c84:	000f7880 */ 	sll	$t7,$t7,0x2
/*  f151c88:	01afc021 */ 	addu	$t8,$t5,$t7
/*  f151c8c:	0304c821 */ 	addu	$t9,$t8,$a0
/*  f151c90:	9328005f */ 	lbu	$t0,0x5f($t9)
/*  f151c94:	24840004 */ 	addiu	$a0,$a0,0x4
/*  f151c98:	44882000 */ 	mtc1	$t0,$f4
/*  f151c9c:	05010004 */ 	bgez	$t0,.L0f151cb0
/*  f151ca0:	468021a0 */ 	cvt.s.w	$f6,$f4
/*  f151ca4:	44815000 */ 	mtc1	$at,$f10
/*  f151ca8:	00000000 */ 	nop
/*  f151cac:	460a3180 */ 	add.s	$f6,$f6,$f10
.L0f151cb0:
/*  f151cb0:	46143482 */ 	mul.s	$f18,$f6,$f20
/*  f151cb4:	240a0001 */ 	addiu	$t2,$zero,0x1
/*  f151cb8:	3c014f00 */ 	lui	$at,0x4f00
/*  f151cbc:	4449f800 */ 	cfc1	$t1,$31
/*  f151cc0:	44caf800 */ 	ctc1	$t2,$31
/*  f151cc4:	00000000 */ 	nop
/*  f151cc8:	46009224 */ 	cvt.w.s	$f8,$f18
/*  f151ccc:	444af800 */ 	cfc1	$t2,$31
/*  f151cd0:	00000000 */ 	nop
/*  f151cd4:	314a0078 */ 	andi	$t2,$t2,0x78
/*  f151cd8:	51400013 */ 	beqzl	$t2,.L0f151d28
/*  f151cdc:	440a4000 */ 	mfc1	$t2,$f8
/*  f151ce0:	44814000 */ 	mtc1	$at,$f8
/*  f151ce4:	240a0001 */ 	addiu	$t2,$zero,0x1
/*  f151ce8:	46089201 */ 	sub.s	$f8,$f18,$f8
/*  f151cec:	44caf800 */ 	ctc1	$t2,$31
/*  f151cf0:	00000000 */ 	nop
/*  f151cf4:	46004224 */ 	cvt.w.s	$f8,$f8
/*  f151cf8:	444af800 */ 	cfc1	$t2,$31
/*  f151cfc:	00000000 */ 	nop
/*  f151d00:	314a0078 */ 	andi	$t2,$t2,0x78
/*  f151d04:	15400005 */ 	bnez	$t2,.L0f151d1c
/*  f151d08:	00000000 */ 	nop
/*  f151d0c:	440a4000 */ 	mfc1	$t2,$f8
/*  f151d10:	3c018000 */ 	lui	$at,0x8000
/*  f151d14:	10000007 */ 	b	.L0f151d34
/*  f151d18:	01415025 */ 	or	$t2,$t2,$at
.L0f151d1c:
/*  f151d1c:	10000005 */ 	b	.L0f151d34
/*  f151d20:	240affff */ 	addiu	$t2,$zero,-1
/*  f151d24:	440a4000 */ 	mfc1	$t2,$f8
.L0f151d28:
/*  f151d28:	00000000 */ 	nop
/*  f151d2c:	0540fffb */ 	bltz	$t2,.L0f151d1c
/*  f151d30:	00000000 */ 	nop
.L0f151d34:
/*  f151d34:	44c9f800 */ 	ctc1	$t1,$31
/*  f151d38:	24630004 */ 	addiu	$v1,$v1,0x4
/*  f151d3c:	1496ffc7 */ 	bne	$a0,$s6,.L0f151c5c
/*  f151d40:	a06affff */ 	sb	$t2,-0x1($v1)
/*  f151d44:	02608025 */ 	or	$s0,$s3,$zero
/*  f151d48:	ae0b0000 */ 	sw	$t3,0x0($s0)
/*  f151d4c:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151d50:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f151d54:	00c02025 */ 	or	$a0,$a2,$zero
/*  f151d58:	10000018 */ 	b	.L0f151dbc
/*  f151d5c:	ae020004 */ 	sw	$v0,0x4($s0)
.L0f151d60:
/*  f151d60:	8e4c0000 */ 	lw	$t4,0x0($s2)
/*  f151d64:	87ad012e */ 	lh	$t5,0x12e($sp)
/*  f151d68:	01947021 */ 	addu	$t6,$t4,$s4
/*  f151d6c:	85c50000 */ 	lh	$a1,0x0($t6)
/*  f151d70:	51a50009 */ 	beql	$t5,$a1,.L0f151d98
/*  f151d74:	3c080708 */ 	lui	$t0,0x708
/*  f151d78:	0fc0044e */ 	jal	func0f001138
/*  f151d7c:	02602025 */ 	or	$a0,$s3,$zero
/*  f151d80:	8e4f0000 */ 	lw	$t7,0x0($s2)
/*  f151d84:	00409825 */ 	or	$s3,$v0,$zero
/*  f151d88:	01f4c021 */ 	addu	$t8,$t7,$s4
/*  f151d8c:	87190000 */ 	lh	$t9,0x0($t8)
/*  f151d90:	a7b9012e */ 	sh	$t9,0x12e($sp)
/*  f151d94:	3c080708 */ 	lui	$t0,0x708
.L0f151d98:
/*  f151d98:	3508000c */ 	ori	$t0,$t0,0xc
/*  f151d9c:	02608025 */ 	or	$s0,$s3,$zero
/*  f151da0:	ae080000 */ 	sw	$t0,0x0($s0)
/*  f151da4:	8e490000 */ 	lw	$t1,0x0($s2)
/*  f151da8:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151dac:	01342021 */ 	addu	$a0,$t1,$s4
/*  f151db0:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f151db4:	2484005c */ 	addiu	$a0,$a0,0x5c
/*  f151db8:	ae020004 */ 	sw	$v0,0x4($s0)
.L0f151dbc:
/*  f151dbc:	3c0a0420 */ 	lui	$t2,0x420
/*  f151dc0:	354a0024 */ 	ori	$t2,$t2,0x24
/*  f151dc4:	02608025 */ 	or	$s0,$s3,$zero
/*  f151dc8:	ae0a0000 */ 	sw	$t2,0x0($s0)
/*  f151dcc:	8e4b0000 */ 	lw	$t3,0x0($s2)
/*  f151dd0:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151dd4:	01742021 */ 	addu	$a0,$t3,$s4
/*  f151dd8:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f151ddc:	24840038 */ 	addiu	$a0,$a0,0x38
/*  f151de0:	02601825 */ 	or	$v1,$s3,$zero
/*  f151de4:	ae020004 */ 	sw	$v0,0x4($s0)
/*  f151de8:	3c0cbf00 */ 	lui	$t4,0xbf00
/*  f151dec:	240e0a14 */ 	addiu	$t6,$zero,0xa14
/*  f151df0:	ac6e0004 */ 	sw	$t6,0x4($v1)
/*  f151df4:	ac6c0000 */ 	sw	$t4,0x0($v1)
/*  f151df8:	26730008 */ 	addiu	$s3,$s3,0x8
.L0f151dfc:
/*  f151dfc:	3c0d800a */ 	lui	$t5,%hi(var800a45c0)
/*  f151e00:	8dad45c0 */ 	lw	$t5,%lo(var800a45c0)($t5)
/*  f151e04:	27de0001 */ 	addiu	$s8,$s8,0x1
/*  f151e08:	2694006c */ 	addiu	$s4,$s4,0x6c
/*  f151e0c:	03cd082a */ 	slt	$at,$s8,$t5
/*  f151e10:	5420feb8 */ 	bnezl	$at,.L0f1518f4
/*  f151e14:	8e490000 */ 	lw	$t1,0x0($s2)
/*  f151e18:	e7b600bc */ 	swc1	$f22,0xbc($sp)
.L0f151e1c:
/*  f151e1c:	87af012e */ 	lh	$t7,0x12e($sp)
/*  f151e20:	55e00005 */ 	bnezl	$t7,.L0f151e38
/*  f151e24:	02601025 */ 	or	$v0,$s3,$zero
/*  f151e28:	0fc004c0 */ 	jal	func0f001300
/*  f151e2c:	02602025 */ 	or	$a0,$s3,$zero
/*  f151e30:	00409825 */ 	or	$s3,$v0,$zero
/*  f151e34:	02601025 */ 	or	$v0,$s3,$zero
.L0f151e38:
/*  f151e38:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151e3c:	3c18b600 */ 	lui	$t8,0xb600
/*  f151e40:	3c190006 */ 	lui	$t9,0x6
/*  f151e44:	3c080103 */ 	lui	$t0,0x103
/*  f151e48:	ac590004 */ 	sw	$t9,0x4($v0)
/*  f151e4c:	ac580000 */ 	sw	$t8,0x0($v0)
/*  f151e50:	35080040 */ 	ori	$t0,$t0,0x40
/*  f151e54:	02608025 */ 	or	$s0,$s3,$zero
/*  f151e58:	ae080000 */ 	sw	$t0,0x0($s0)
/*  f151e5c:	0fc2d3fa */ 	jal	currentPlayerGetUnk1750
/*  f151e60:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151e64:	3c090102 */ 	lui	$t1,0x102
/*  f151e68:	ae020004 */ 	sw	$v0,0x4($s0)
/*  f151e6c:	35290040 */ 	ori	$t1,$t1,0x40
/*  f151e70:	02608825 */ 	or	$s1,$s3,$zero
/*  f151e74:	ae290000 */ 	sw	$t1,0x0($s1)
/*  f151e78:	0fc2d3ea */ 	jal	currentPlayerGetUnk173c
/*  f151e7c:	26730008 */ 	addiu	$s3,$s3,0x8
/*  f151e80:	ae220004 */ 	sw	$v0,0x4($s1)
.L0f151e84:
/*  f151e84:	8fbf006c */ 	lw	$ra,0x6c($sp)
/*  f151e88:	02601025 */ 	or	$v0,$s3,$zero
/*  f151e8c:	8fb30054 */ 	lw	$s3,0x54($sp)
/*  f151e90:	d7b40018 */ 	ldc1	$f20,0x18($sp)
/*  f151e94:	d7b60020 */ 	ldc1	$f22,0x20($sp)
/*  f151e98:	d7b80028 */ 	ldc1	$f24,0x28($sp)
/*  f151e9c:	d7ba0030 */ 	ldc1	$f26,0x30($sp)
/*  f151ea0:	d7bc0038 */ 	ldc1	$f28,0x38($sp)
/*  f151ea4:	d7be0040 */ 	ldc1	$f30,0x40($sp)
/*  f151ea8:	8fb00048 */ 	lw	$s0,0x48($sp)
/*  f151eac:	8fb1004c */ 	lw	$s1,0x4c($sp)
/*  f151eb0:	8fb20050 */ 	lw	$s2,0x50($sp)
/*  f151eb4:	8fb40058 */ 	lw	$s4,0x58($sp)
/*  f151eb8:	8fb5005c */ 	lw	$s5,0x5c($sp)
/*  f151ebc:	8fb60060 */ 	lw	$s6,0x60($sp)
/*  f151ec0:	8fb70064 */ 	lw	$s7,0x64($sp)
/*  f151ec4:	8fbe0068 */ 	lw	$s8,0x68($sp)
/*  f151ec8:	03e00008 */ 	jr	$ra
/*  f151ecc:	27bd0130 */ 	addiu	$sp,$sp,0x130
);

GLOBAL_ASM(
glabel func0f151ed0
/*  f151ed0:	27bdfec0 */ 	addiu	$sp,$sp,-320
/*  f151ed4:	3c0e8008 */ 	lui	$t6,%hi(var8007fa84)
/*  f151ed8:	8dcefa84 */ 	lw	$t6,%lo(var8007fa84)($t6)
/*  f151edc:	afbf007c */ 	sw	$ra,0x7c($sp)
/*  f151ee0:	afbe0078 */ 	sw	$s8,0x78($sp)
/*  f151ee4:	afb70074 */ 	sw	$s7,0x74($sp)
/*  f151ee8:	afb60070 */ 	sw	$s6,0x70($sp)
/*  f151eec:	afb5006c */ 	sw	$s5,0x6c($sp)
/*  f151ef0:	afb40068 */ 	sw	$s4,0x68($sp)
/*  f151ef4:	afb30064 */ 	sw	$s3,0x64($sp)
/*  f151ef8:	afb20060 */ 	sw	$s2,0x60($sp)
/*  f151efc:	afb1005c */ 	sw	$s1,0x5c($sp)
/*  f151f00:	afb00058 */ 	sw	$s0,0x58($sp)
/*  f151f04:	f7be0050 */ 	sdc1	$f30,0x50($sp)
/*  f151f08:	f7bc0048 */ 	sdc1	$f28,0x48($sp)
/*  f151f0c:	f7ba0040 */ 	sdc1	$f26,0x40($sp)
/*  f151f10:	f7b80038 */ 	sdc1	$f24,0x38($sp)
/*  f151f14:	f7b60030 */ 	sdc1	$f22,0x30($sp)
/*  f151f18:	f7b40028 */ 	sdc1	$f20,0x28($sp)
/*  f151f1c:	11c001de */ 	beqz	$t6,.L0f152698
/*  f151f20:	afa40140 */ 	sw	$a0,0x140($sp)
/*  f151f24:	3c16800a */ 	lui	$s6,%hi(g_Vars)
/*  f151f28:	26d69fc0 */ 	addiu	$s6,$s6,%lo(g_Vars)
/*  f151f2c:	8ecf0284 */ 	lw	$t7,0x284($s6)
/*  f151f30:	a7a0013e */ 	sh	$zero,0x13e($sp)
/*  f151f34:	24010001 */ 	addiu	$at,$zero,0x1
/*  f151f38:	95f80010 */ 	lhu	$t8,0x10($t7)
/*  f151f3c:	27a40140 */ 	addiu	$a0,$sp,0x140
/*  f151f40:	3c05800b */ 	lui	$a1,%hi(var800ab590)
/*  f151f44:	1701000d */ 	bne	$t8,$at,.L0f151f7c
/*  f151f48:	24060002 */ 	addiu	$a2,$zero,0x2
/*  f151f4c:	24190002 */ 	addiu	$t9,$zero,0x2
/*  f151f50:	24080001 */ 	addiu	$t0,$zero,0x1
/*  f151f54:	afa80014 */ 	sw	$t0,0x14($sp)
/*  f151f58:	afb90010 */ 	sw	$t9,0x10($sp)
/*  f151f5c:	27a40140 */ 	addiu	$a0,$sp,0x140
/*  f151f60:	00002825 */ 	or	$a1,$zero,$zero
/*  f151f64:	24060002 */ 	addiu	$a2,$zero,0x2
/*  f151f68:	24070001 */ 	addiu	$a3,$zero,0x1
/*  f151f6c:	0fc2ce70 */ 	jal	func0f0b39c0
/*  f151f70:	afa00018 */ 	sw	$zero,0x18($sp)
/*  f151f74:	1000000b */ 	b	.L0f151fa4
/*  f151f78:	8fab0140 */ 	lw	$t3,0x140($sp)
.L0f151f7c:
/*  f151f7c:	8ca5b590 */ 	lw	$a1,%lo(var800ab590)($a1)
/*  f151f80:	24090002 */ 	addiu	$t1,$zero,0x2
/*  f151f84:	240a0001 */ 	addiu	$t2,$zero,0x1
/*  f151f88:	afaa0014 */ 	sw	$t2,0x14($sp)
/*  f151f8c:	afa90010 */ 	sw	$t1,0x10($sp)
/*  f151f90:	24070001 */ 	addiu	$a3,$zero,0x1
/*  f151f94:	afa00018 */ 	sw	$zero,0x18($sp)
/*  f151f98:	0fc2ce70 */ 	jal	func0f0b39c0
/*  f151f9c:	24a5000c */ 	addiu	$a1,$a1,0x000c
/*  f151fa0:	8fab0140 */ 	lw	$t3,0x140($sp)
.L0f151fa4:
/*  f151fa4:	3c0dbb00 */ 	lui	$t5,0xbb00
/*  f151fa8:	3c0e0d80 */ 	lui	$t6,0xd80
/*  f151fac:	256c0008 */ 	addiu	$t4,$t3,0x8
/*  f151fb0:	afac0140 */ 	sw	$t4,0x140($sp)
/*  f151fb4:	35ce0d80 */ 	ori	$t6,$t6,0xd80
/*  f151fb8:	35ad1001 */ 	ori	$t5,$t5,0x1001
/*  f151fbc:	ad6d0000 */ 	sw	$t5,0x0($t3)
/*  f151fc0:	ad6e0004 */ 	sw	$t6,0x4($t3)
/*  f151fc4:	8faf0140 */ 	lw	$t7,0x140($sp)
/*  f151fc8:	3c19ba00 */ 	lui	$t9,0xba00
/*  f151fcc:	37391402 */ 	ori	$t9,$t9,0x1402
/*  f151fd0:	25f80008 */ 	addiu	$t8,$t7,0x8
/*  f151fd4:	afb80140 */ 	sw	$t8,0x140($sp)
/*  f151fd8:	3c080010 */ 	lui	$t0,0x10
/*  f151fdc:	ade80004 */ 	sw	$t0,0x4($t7)
/*  f151fe0:	adf90000 */ 	sw	$t9,0x0($t7)
/*  f151fe4:	8fa90140 */ 	lw	$t1,0x140($sp)
/*  f151fe8:	3c0bba00 */ 	lui	$t3,0xba00
/*  f151fec:	356b1001 */ 	ori	$t3,$t3,0x1001
/*  f151ff0:	252a0008 */ 	addiu	$t2,$t1,0x8
/*  f151ff4:	afaa0140 */ 	sw	$t2,0x140($sp)
/*  f151ff8:	3c0c0001 */ 	lui	$t4,0x1
/*  f151ffc:	ad2c0004 */ 	sw	$t4,0x4($t1)
/*  f152000:	ad2b0000 */ 	sw	$t3,0x0($t1)
/*  f152004:	8fad0140 */ 	lw	$t5,0x140($sp)
/*  f152008:	3c0fb600 */ 	lui	$t7,0xb600
/*  f15200c:	24183000 */ 	addiu	$t8,$zero,0x3000
/*  f152010:	25ae0008 */ 	addiu	$t6,$t5,0x8
/*  f152014:	afae0140 */ 	sw	$t6,0x140($sp)
/*  f152018:	adb80004 */ 	sw	$t8,0x4($t5)
/*  f15201c:	adaf0000 */ 	sw	$t7,0x0($t5)
/*  f152020:	8fb90140 */ 	lw	$t9,0x140($sp)
/*  f152024:	3c09ba00 */ 	lui	$t1,0xba00
/*  f152028:	35290c02 */ 	ori	$t1,$t1,0xc02
/*  f15202c:	27280008 */ 	addiu	$t0,$t9,0x8
/*  f152030:	afa80140 */ 	sw	$t0,0x140($sp)
/*  f152034:	240a2000 */ 	addiu	$t2,$zero,0x2000
/*  f152038:	af2a0004 */ 	sw	$t2,0x4($t9)
/*  f15203c:	af290000 */ 	sw	$t1,0x0($t9)
/*  f152040:	8ecb0284 */ 	lw	$t3,0x284($s6)
/*  f152044:	8fad0140 */ 	lw	$t5,0x140($sp)
/*  f152048:	24010001 */ 	addiu	$at,$zero,0x1
/*  f15204c:	956c0010 */ 	lhu	$t4,0x10($t3)
/*  f152050:	3c0fb700 */ 	lui	$t7,0xb700
/*  f152054:	3c180006 */ 	lui	$t8,0x6
/*  f152058:	11810004 */ 	beq	$t4,$at,.L0f15206c
/*  f15205c:	25ae0008 */ 	addiu	$t6,$t5,0x8
/*  f152060:	afae0140 */ 	sw	$t6,0x140($sp)
/*  f152064:	adb80004 */ 	sw	$t8,0x4($t5)
/*  f152068:	adaf0000 */ 	sw	$t7,0x0($t5)
.L0f15206c:
/*  f15206c:	8fb00140 */ 	lw	$s0,0x140($sp)
/*  f152070:	3c090103 */ 	lui	$t1,0x103
/*  f152074:	35290040 */ 	ori	$t1,$t1,0x40
/*  f152078:	26080008 */ 	addiu	$t0,$s0,0x8
/*  f15207c:	afa80140 */ 	sw	$t0,0x140($sp)
/*  f152080:	0fc2d402 */ 	jal	currentPlayerGetUnk1758
/*  f152084:	ae090000 */ 	sw	$t1,0x0($s0)
/*  f152088:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f15208c:	00402025 */ 	or	$a0,$v0,$zero
/*  f152090:	ae020004 */ 	sw	$v0,0x4($s0)
/*  f152094:	3c0a800a */ 	lui	$t2,%hi(var800a45c0)
/*  f152098:	8d4a45c0 */ 	lw	$t2,%lo(var800a45c0)($t2)
/*  f15209c:	0000b825 */ 	or	$s7,$zero,$zero
/*  f1520a0:	00009825 */ 	or	$s3,$zero,$zero
/*  f1520a4:	1940015f */ 	blez	$t2,.L0f152624
/*  f1520a8:	3c014316 */ 	lui	$at,0x4316
/*  f1520ac:	4481f000 */ 	mtc1	$at,$f30
/*  f1520b0:	3c017f1b */ 	lui	$at,%hi(var7f1b7398)
/*  f1520b4:	c43c7398 */ 	lwc1	$f28,%lo(var7f1b7398)($at)
/*  f1520b8:	3c017f1b */ 	lui	$at,%hi(var7f1b739c)
/*  f1520bc:	c43a739c */ 	lwc1	$f26,%lo(var7f1b739c)($at)
/*  f1520c0:	3c013f80 */ 	lui	$at,0x3f80
/*  f1520c4:	3c148008 */ 	lui	$s4,%hi(var8007fa80)
/*  f1520c8:	3c12800a */ 	lui	$s2,%hi(var800a45c4)
/*  f1520cc:	4481c000 */ 	mtc1	$at,$f24
/*  f1520d0:	265245c4 */ 	addiu	$s2,$s2,%lo(var800a45c4)
/*  f1520d4:	2694fa80 */ 	addiu	$s4,$s4,%lo(var8007fa80)
/*  f1520d8:	c7b600cc */ 	lwc1	$f22,0xcc($sp)
/*  f1520dc:	27be00f8 */ 	addiu	$s8,$sp,0xf8
/*  f1520e0:	2415000c */ 	addiu	$s5,$zero,0xc
/*  f1520e4:	8e4b0000 */ 	lw	$t3,0x0($s2)
.L0f1520e8:
/*  f1520e8:	01731021 */ 	addu	$v0,$t3,$s3
/*  f1520ec:	8c4c0004 */ 	lw	$t4,0x4($v0)
/*  f1520f0:	19800144 */ 	blez	$t4,.L0f152604
/*  f1520f4:	00000000 */ 	nop
/*  f1520f8:	904d0068 */ 	lbu	$t5,0x68($v0)
/*  f1520fc:	24010002 */ 	addiu	$at,$zero,0x2
/*  f152100:	24100001 */ 	addiu	$s0,$zero,0x1
/*  f152104:	11a1013f */ 	beq	$t5,$at,.L0f152604
/*  f152108:	00000000 */ 	nop
/*  f15210c:	0fc59e66 */ 	jal	func0f167998
/*  f152110:	4600c506 */ 	mov.s	$f20,$f24
/*  f152114:	8ec30284 */ 	lw	$v1,0x284($s6)
/*  f152118:	24010001 */ 	addiu	$at,$zero,0x1
/*  f15211c:	00408825 */ 	or	$s1,$v0,$zero
/*  f152120:	946e0010 */ 	lhu	$t6,0x10($v1)
/*  f152124:	15c10025 */ 	bne	$t6,$at,.L0f1521bc
/*  f152128:	00000000 */ 	nop
/*  f15212c:	8e4f0000 */ 	lw	$t7,0x0($s2)
/*  f152130:	c46a0018 */ 	lwc1	$f10,0x18($v1)
/*  f152134:	c4660020 */ 	lwc1	$f6,0x20($v1)
/*  f152138:	01f31021 */ 	addu	$v0,$t7,$s3
/*  f15213c:	c4480008 */ 	lwc1	$f8,0x8($v0)
/*  f152140:	c4440010 */ 	lwc1	$f4,0x10($v0)
/*  f152144:	c450000c */ 	lwc1	$f16,0xc($v0)
/*  f152148:	460a4081 */ 	sub.s	$f2,$f8,$f10
/*  f15214c:	c472001c */ 	lwc1	$f18,0x1c($v1)
/*  f152150:	46062001 */ 	sub.s	$f0,$f4,$f6
/*  f152154:	46021102 */ 	mul.s	$f4,$f2,$f2
/*  f152158:	46128381 */ 	sub.s	$f14,$f16,$f18
/*  f15215c:	460e7182 */ 	mul.s	$f6,$f14,$f14
/*  f152160:	46062200 */ 	add.s	$f8,$f4,$f6
/*  f152164:	46000282 */ 	mul.s	$f10,$f0,$f0
/*  f152168:	0c012974 */ 	jal	sqrtf
/*  f15216c:	46085300 */ 	add.s	$f12,$f10,$f8
/*  f152170:	8ed80284 */ 	lw	$t8,0x284($s6)
/*  f152174:	c7020024 */ 	lwc1	$f2,0x24($t8)
/*  f152178:	4600103c */ 	c.lt.s	$f2,$f0
/*  f15217c:	461e1301 */ 	sub.s	$f12,$f2,$f30
/*  f152180:	45000002 */ 	bc1f	.L0f15218c
/*  f152184:	46020583 */ 	div.s	$f22,$f0,$f2
/*  f152188:	00008025 */ 	or	$s0,$zero,$zero
.L0f15218c:
/*  f15218c:	4600603c */ 	c.lt.s	$f12,$f0
/*  f152190:	00000000 */ 	nop
/*  f152194:	45020005 */ 	bc1fl	.L0f1521ac
/*  f152198:	4616c03c */ 	c.lt.s	$f24,$f22
/*  f15219c:	460c0401 */ 	sub.s	$f16,$f0,$f12
/*  f1521a0:	461e8483 */ 	div.s	$f18,$f16,$f30
/*  f1521a4:	4612c501 */ 	sub.s	$f20,$f24,$f18
/*  f1521a8:	4616c03c */ 	c.lt.s	$f24,$f22
.L0f1521ac:
/*  f1521ac:	00000000 */ 	nop
/*  f1521b0:	45000002 */ 	bc1f	.L0f1521bc
/*  f1521b4:	00000000 */ 	nop
/*  f1521b8:	4600c586 */ 	mov.s	$f22,$f24
.L0f1521bc:
/*  f1521bc:	12000111 */ 	beqz	$s0,.L0f152604
/*  f1521c0:	00000000 */ 	nop
/*  f1521c4:	8e590000 */ 	lw	$t9,0x0($s2)
/*  f1521c8:	03c03025 */ 	or	$a2,$s8,$zero
/*  f1521cc:	03331021 */ 	addu	$v0,$t9,$s3
/*  f1521d0:	24440008 */ 	addiu	$a0,$v0,0x8
/*  f1521d4:	0c0059a9 */ 	jal	func000166a4
/*  f1521d8:	24450014 */ 	addiu	$a1,$v0,0x14
/*  f1521dc:	8ec30284 */ 	lw	$v1,0x284($s6)
/*  f1521e0:	c7a40128 */ 	lwc1	$f4,0x128($sp)
/*  f1521e4:	c7a8012c */ 	lwc1	$f8,0x12c($sp)
/*  f1521e8:	c4660038 */ 	lwc1	$f6,0x38($v1)
/*  f1521ec:	46062281 */ 	sub.s	$f10,$f4,$f6
/*  f1521f0:	c7a40130 */ 	lwc1	$f4,0x130($sp)
/*  f1521f4:	e7aa0128 */ 	swc1	$f10,0x128($sp)
/*  f1521f8:	c470003c */ 	lwc1	$f16,0x3c($v1)
/*  f1521fc:	461a503c */ 	c.lt.s	$f10,$f26
/*  f152200:	46104481 */ 	sub.s	$f18,$f8,$f16
/*  f152204:	e7b2012c */ 	swc1	$f18,0x12c($sp)
/*  f152208:	c4660040 */ 	lwc1	$f6,0x40($v1)
/*  f15220c:	46062201 */ 	sub.s	$f8,$f4,$f6
/*  f152210:	450000fc */ 	bc1f	.L0f152604
/*  f152214:	e7a80130 */ 	swc1	$f8,0x130($sp)
/*  f152218:	460ae03c */ 	c.lt.s	$f28,$f10
/*  f15221c:	00000000 */ 	nop
/*  f152220:	450000f8 */ 	bc1f	.L0f152604
/*  f152224:	00000000 */ 	nop
/*  f152228:	461a903c */ 	c.lt.s	$f18,$f26
/*  f15222c:	00000000 */ 	nop
/*  f152230:	450000f4 */ 	bc1f	.L0f152604
/*  f152234:	00000000 */ 	nop
/*  f152238:	4612e03c */ 	c.lt.s	$f28,$f18
/*  f15223c:	00000000 */ 	nop
/*  f152240:	450000f0 */ 	bc1f	.L0f152604
/*  f152244:	00000000 */ 	nop
/*  f152248:	461a403c */ 	c.lt.s	$f8,$f26
/*  f15224c:	00000000 */ 	nop
/*  f152250:	450000ec */ 	bc1f	.L0f152604
/*  f152254:	00000000 */ 	nop
/*  f152258:	4608e03c */ 	c.lt.s	$f28,$f8
/*  f15225c:	03c02025 */ 	or	$a0,$s8,$zero
/*  f152260:	450000e8 */ 	bc1f	.L0f152604
/*  f152264:	00000000 */ 	nop
/*  f152268:	0c005815 */ 	jal	func00016054
/*  f15226c:	02202825 */ 	or	$a1,$s1,$zero
/*  f152270:	8fb00140 */ 	lw	$s0,0x140($sp)
/*  f152274:	3c0a0102 */ 	lui	$t2,0x102
/*  f152278:	354a0040 */ 	ori	$t2,$t2,0x40
/*  f15227c:	26090008 */ 	addiu	$t1,$s0,0x8
/*  f152280:	afa90140 */ 	sw	$t1,0x140($sp)
/*  f152284:	02202025 */ 	or	$a0,$s1,$zero
/*  f152288:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f15228c:	ae0a0000 */ 	sw	$t2,0x0($s0)
/*  f152290:	ae020004 */ 	sw	$v0,0x4($s0)
/*  f152294:	8ecb0284 */ 	lw	$t3,0x284($s6)
/*  f152298:	24010001 */ 	addiu	$at,$zero,0x1
/*  f15229c:	956c0010 */ 	lhu	$t4,0x10($t3)
/*  f1522a0:	158100ae */ 	bne	$t4,$at,.L0f15255c
/*  f1522a4:	3c01437f */ 	lui	$at,0x437f
/*  f1522a8:	44818000 */ 	mtc1	$at,$f16
/*  f1522ac:	24100001 */ 	addiu	$s0,$zero,0x1
/*  f1522b0:	3c014f00 */ 	lui	$at,0x4f00
/*  f1522b4:	4610b102 */ 	mul.s	$f4,$f22,$f16
/*  f1522b8:	24040003 */ 	addiu	$a0,$zero,0x3
/*  f1522bc:	444df800 */ 	cfc1	$t5,$31
/*  f1522c0:	44d0f800 */ 	ctc1	$s0,$31
/*  f1522c4:	00000000 */ 	nop
/*  f1522c8:	460021a4 */ 	cvt.w.s	$f6,$f4
/*  f1522cc:	4450f800 */ 	cfc1	$s0,$31
/*  f1522d0:	00000000 */ 	nop
/*  f1522d4:	32100078 */ 	andi	$s0,$s0,0x78
/*  f1522d8:	52000013 */ 	beqzl	$s0,.L0f152328
/*  f1522dc:	44103000 */ 	mfc1	$s0,$f6
/*  f1522e0:	44813000 */ 	mtc1	$at,$f6
/*  f1522e4:	24100001 */ 	addiu	$s0,$zero,0x1
/*  f1522e8:	46062181 */ 	sub.s	$f6,$f4,$f6
/*  f1522ec:	44d0f800 */ 	ctc1	$s0,$31
/*  f1522f0:	00000000 */ 	nop
/*  f1522f4:	460031a4 */ 	cvt.w.s	$f6,$f6
/*  f1522f8:	4450f800 */ 	cfc1	$s0,$31
/*  f1522fc:	00000000 */ 	nop
/*  f152300:	32100078 */ 	andi	$s0,$s0,0x78
/*  f152304:	16000005 */ 	bnez	$s0,.L0f15231c
/*  f152308:	00000000 */ 	nop
/*  f15230c:	44103000 */ 	mfc1	$s0,$f6
/*  f152310:	3c018000 */ 	lui	$at,0x8000
/*  f152314:	10000007 */ 	b	.L0f152334
/*  f152318:	02018025 */ 	or	$s0,$s0,$at
.L0f15231c:
/*  f15231c:	10000005 */ 	b	.L0f152334
/*  f152320:	2410ffff */ 	addiu	$s0,$zero,-1
/*  f152324:	44103000 */ 	mfc1	$s0,$f6
.L0f152328:
/*  f152328:	00000000 */ 	nop
/*  f15232c:	0600fffb */ 	bltz	$s0,.L0f15231c
/*  f152330:	00000000 */ 	nop
.L0f152334:
/*  f152334:	44cdf800 */ 	ctc1	$t5,$31
/*  f152338:	3c01437f */ 	lui	$at,0x437f
/*  f15233c:	44819000 */ 	mtc1	$at,$f18
/*  f152340:	4616c281 */ 	sub.s	$f10,$f24,$f22
/*  f152344:	24110001 */ 	addiu	$s1,$zero,0x1
/*  f152348:	3c014f00 */ 	lui	$at,0x4f00
/*  f15234c:	46125202 */ 	mul.s	$f8,$f10,$f18
/*  f152350:	444ef800 */ 	cfc1	$t6,$31
/*  f152354:	44d1f800 */ 	ctc1	$s1,$31
/*  f152358:	00000000 */ 	nop
/*  f15235c:	46004424 */ 	cvt.w.s	$f16,$f8
/*  f152360:	4451f800 */ 	cfc1	$s1,$31
/*  f152364:	00000000 */ 	nop
/*  f152368:	32310078 */ 	andi	$s1,$s1,0x78
/*  f15236c:	52200013 */ 	beqzl	$s1,.L0f1523bc
/*  f152370:	44118000 */ 	mfc1	$s1,$f16
/*  f152374:	44818000 */ 	mtc1	$at,$f16
/*  f152378:	24110001 */ 	addiu	$s1,$zero,0x1
/*  f15237c:	46104401 */ 	sub.s	$f16,$f8,$f16
/*  f152380:	44d1f800 */ 	ctc1	$s1,$31
/*  f152384:	00000000 */ 	nop
/*  f152388:	46008424 */ 	cvt.w.s	$f16,$f16
/*  f15238c:	4451f800 */ 	cfc1	$s1,$31
/*  f152390:	00000000 */ 	nop
/*  f152394:	32310078 */ 	andi	$s1,$s1,0x78
/*  f152398:	16200005 */ 	bnez	$s1,.L0f1523b0
/*  f15239c:	00000000 */ 	nop
/*  f1523a0:	44118000 */ 	mfc1	$s1,$f16
/*  f1523a4:	3c018000 */ 	lui	$at,0x8000
/*  f1523a8:	10000007 */ 	b	.L0f1523c8
/*  f1523ac:	02218825 */ 	or	$s1,$s1,$at
.L0f1523b0:
/*  f1523b0:	10000005 */ 	b	.L0f1523c8
/*  f1523b4:	2411ffff */ 	addiu	$s1,$zero,-1
/*  f1523b8:	44118000 */ 	mfc1	$s1,$f16
.L0f1523bc:
/*  f1523bc:	00000000 */ 	nop
/*  f1523c0:	0620fffb */ 	bltz	$s1,.L0f1523b0
/*  f1523c4:	00000000 */ 	nop
.L0f1523c8:
/*  f1523c8:	44cef800 */ 	ctc1	$t6,$31
/*  f1523cc:	0fc59e73 */ 	jal	func0f1679cc
/*  f1523d0:	00000000 */ 	nop
/*  f1523d4:	8e4f0000 */ 	lw	$t7,0x0($s2)
/*  f1523d8:	00403025 */ 	or	$a2,$v0,$zero
/*  f1523dc:	01f3c021 */ 	addu	$t8,$t7,$s3
/*  f1523e0:	8f030004 */ 	lw	$v1,0x4($t8)
/*  f1523e4:	28610064 */ 	slti	$at,$v1,0x64
/*  f1523e8:	54200013 */ 	bnezl	$at,.L0f152438
/*  f1523ec:	3c013f00 */ 	lui	$at,0x3f00
/*  f1523f0:	44832000 */ 	mtc1	$v1,$f4
/*  f1523f4:	3c014248 */ 	lui	$at,0x4248
/*  f1523f8:	44815000 */ 	mtc1	$at,$f10
/*  f1523fc:	468021a0 */ 	cvt.s.w	$f6,$f4
/*  f152400:	3c013f80 */ 	lui	$at,0x3f80
/*  f152404:	44819000 */ 	mtc1	$at,$f18
/*  f152408:	3c013f80 */ 	lui	$at,0x3f80
/*  f15240c:	460a3003 */ 	div.s	$f0,$f6,$f10
/*  f152410:	4600903c */ 	c.lt.s	$f18,$f0
/*  f152414:	00000000 */ 	nop
/*  f152418:	45020004 */ 	bc1fl	.L0f15242c
/*  f15241c:	4600c201 */ 	sub.s	$f8,$f24,$f0
/*  f152420:	44810000 */ 	mtc1	$at,$f0
/*  f152424:	00000000 */ 	nop
/*  f152428:	4600c201 */ 	sub.s	$f8,$f24,$f0
.L0f15242c:
/*  f15242c:	46144502 */ 	mul.s	$f20,$f8,$f20
/*  f152430:	00000000 */ 	nop
/*  f152434:	3c013f00 */ 	lui	$at,0x3f00
.L0f152438:
/*  f152438:	44818000 */ 	mtc1	$at,$f16
/*  f15243c:	00002025 */ 	or	$a0,$zero,$zero
/*  f152440:	00401825 */ 	or	$v1,$v0,$zero
/*  f152444:	4610a502 */ 	mul.s	$f20,$f20,$f16
/*  f152448:	00000000 */ 	nop
.L0f15244c:
/*  f15244c:	a0700000 */ 	sb	$s0,0x0($v1)
/*  f152450:	a0710001 */ 	sb	$s1,0x1($v1)
/*  f152454:	a0600002 */ 	sb	$zero,0x2($v1)
/*  f152458:	8e880000 */ 	lw	$t0,0x0($s4)
/*  f15245c:	8e590000 */ 	lw	$t9,0x0($s2)
/*  f152460:	3c014f80 */ 	lui	$at,0x4f80
/*  f152464:	000848c0 */ 	sll	$t1,$t0,0x3
/*  f152468:	01284823 */ 	subu	$t1,$t1,$t0
/*  f15246c:	00094880 */ 	sll	$t1,$t1,0x2
/*  f152470:	01284823 */ 	subu	$t1,$t1,$t0
/*  f152474:	00094880 */ 	sll	$t1,$t1,0x2
/*  f152478:	03295021 */ 	addu	$t2,$t9,$t1
/*  f15247c:	01445821 */ 	addu	$t3,$t2,$a0
/*  f152480:	916c005f */ 	lbu	$t4,0x5f($t3)
/*  f152484:	24840004 */ 	addiu	$a0,$a0,0x4
/*  f152488:	448c2000 */ 	mtc1	$t4,$f4
/*  f15248c:	05810004 */ 	bgez	$t4,.L0f1524a0
/*  f152490:	468021a0 */ 	cvt.s.w	$f6,$f4
/*  f152494:	44815000 */ 	mtc1	$at,$f10
/*  f152498:	00000000 */ 	nop
/*  f15249c:	460a3180 */ 	add.s	$f6,$f6,$f10
.L0f1524a0:
/*  f1524a0:	46143482 */ 	mul.s	$f18,$f6,$f20
/*  f1524a4:	240e0001 */ 	addiu	$t6,$zero,0x1
/*  f1524a8:	3c014f00 */ 	lui	$at,0x4f00
/*  f1524ac:	444df800 */ 	cfc1	$t5,$31
/*  f1524b0:	44cef800 */ 	ctc1	$t6,$31
/*  f1524b4:	00000000 */ 	nop
/*  f1524b8:	46009224 */ 	cvt.w.s	$f8,$f18
/*  f1524bc:	444ef800 */ 	cfc1	$t6,$31
/*  f1524c0:	00000000 */ 	nop
/*  f1524c4:	31ce0078 */ 	andi	$t6,$t6,0x78
/*  f1524c8:	51c00013 */ 	beqzl	$t6,.L0f152518
/*  f1524cc:	440e4000 */ 	mfc1	$t6,$f8
/*  f1524d0:	44814000 */ 	mtc1	$at,$f8
/*  f1524d4:	240e0001 */ 	addiu	$t6,$zero,0x1
/*  f1524d8:	46089201 */ 	sub.s	$f8,$f18,$f8
/*  f1524dc:	44cef800 */ 	ctc1	$t6,$31
/*  f1524e0:	00000000 */ 	nop
/*  f1524e4:	46004224 */ 	cvt.w.s	$f8,$f8
/*  f1524e8:	444ef800 */ 	cfc1	$t6,$31
/*  f1524ec:	00000000 */ 	nop
/*  f1524f0:	31ce0078 */ 	andi	$t6,$t6,0x78
/*  f1524f4:	15c00005 */ 	bnez	$t6,.L0f15250c
/*  f1524f8:	00000000 */ 	nop
/*  f1524fc:	440e4000 */ 	mfc1	$t6,$f8
/*  f152500:	3c018000 */ 	lui	$at,0x8000
/*  f152504:	10000007 */ 	b	.L0f152524
/*  f152508:	01c17025 */ 	or	$t6,$t6,$at
.L0f15250c:
/*  f15250c:	10000005 */ 	b	.L0f152524
/*  f152510:	240effff */ 	addiu	$t6,$zero,-1
/*  f152514:	440e4000 */ 	mfc1	$t6,$f8
.L0f152518:
/*  f152518:	00000000 */ 	nop
/*  f15251c:	05c0fffb */ 	bltz	$t6,.L0f15250c
/*  f152520:	00000000 */ 	nop
.L0f152524:
/*  f152524:	44cdf800 */ 	ctc1	$t5,$31
/*  f152528:	24630004 */ 	addiu	$v1,$v1,0x4
/*  f15252c:	1495ffc7 */ 	bne	$a0,$s5,.L0f15244c
/*  f152530:	a06effff */ 	sb	$t6,-0x1($v1)
/*  f152534:	8fb00140 */ 	lw	$s0,0x140($sp)
/*  f152538:	3c080708 */ 	lui	$t0,0x708
/*  f15253c:	3508000c */ 	ori	$t0,$t0,0xc
/*  f152540:	26180008 */ 	addiu	$t8,$s0,0x8
/*  f152544:	afb80140 */ 	sw	$t8,0x140($sp)
/*  f152548:	00c02025 */ 	or	$a0,$a2,$zero
/*  f15254c:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f152550:	ae080000 */ 	sw	$t0,0x0($s0)
/*  f152554:	10000019 */ 	b	.L0f1525bc
/*  f152558:	ae020004 */ 	sw	$v0,0x4($s0)
.L0f15255c:
/*  f15255c:	8e590000 */ 	lw	$t9,0x0($s2)
/*  f152560:	87aa013e */ 	lh	$t2,0x13e($sp)
/*  f152564:	03334821 */ 	addu	$t1,$t9,$s3
/*  f152568:	85250000 */ 	lh	$a1,0x0($t1)
/*  f15256c:	51450009 */ 	beql	$t2,$a1,.L0f152594
/*  f152570:	8fb00140 */ 	lw	$s0,0x140($sp)
/*  f152574:	0fc0044e */ 	jal	func0f001138
/*  f152578:	8fa40140 */ 	lw	$a0,0x140($sp)
/*  f15257c:	8e4b0000 */ 	lw	$t3,0x0($s2)
/*  f152580:	afa20140 */ 	sw	$v0,0x140($sp)
/*  f152584:	01736021 */ 	addu	$t4,$t3,$s3
/*  f152588:	858d0000 */ 	lh	$t5,0x0($t4)
/*  f15258c:	a7ad013e */ 	sh	$t5,0x13e($sp)
/*  f152590:	8fb00140 */ 	lw	$s0,0x140($sp)
.L0f152594:
/*  f152594:	3c180708 */ 	lui	$t8,0x708
/*  f152598:	3718000c */ 	ori	$t8,$t8,0xc
/*  f15259c:	260f0008 */ 	addiu	$t7,$s0,0x8
/*  f1525a0:	afaf0140 */ 	sw	$t7,0x140($sp)
/*  f1525a4:	ae180000 */ 	sw	$t8,0x0($s0)
/*  f1525a8:	8e480000 */ 	lw	$t0,0x0($s2)
/*  f1525ac:	01132021 */ 	addu	$a0,$t0,$s3
/*  f1525b0:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f1525b4:	2484005c */ 	addiu	$a0,$a0,0x5c
/*  f1525b8:	ae020004 */ 	sw	$v0,0x4($s0)
.L0f1525bc:
/*  f1525bc:	8fb00140 */ 	lw	$s0,0x140($sp)
/*  f1525c0:	3c0a0420 */ 	lui	$t2,0x420
/*  f1525c4:	354a0024 */ 	ori	$t2,$t2,0x24
/*  f1525c8:	26090008 */ 	addiu	$t1,$s0,0x8
/*  f1525cc:	afa90140 */ 	sw	$t1,0x140($sp)
/*  f1525d0:	ae0a0000 */ 	sw	$t2,0x0($s0)
/*  f1525d4:	8e4b0000 */ 	lw	$t3,0x0($s2)
/*  f1525d8:	01732021 */ 	addu	$a0,$t3,$s3
/*  f1525dc:	0c012d20 */ 	jal	osVirtualToPhysical
/*  f1525e0:	24840038 */ 	addiu	$a0,$a0,0x38
/*  f1525e4:	ae020004 */ 	sw	$v0,0x4($s0)
/*  f1525e8:	8fac0140 */ 	lw	$t4,0x140($sp)
/*  f1525ec:	3c0ebf00 */ 	lui	$t6,0xbf00
/*  f1525f0:	240f0a14 */ 	addiu	$t7,$zero,0xa14
/*  f1525f4:	258d0008 */ 	addiu	$t5,$t4,0x8
/*  f1525f8:	afad0140 */ 	sw	$t5,0x140($sp)
/*  f1525fc:	ad8f0004 */ 	sw	$t7,0x4($t4)
/*  f152600:	ad8e0000 */ 	sw	$t6,0x0($t4)
.L0f152604:
/*  f152604:	3c18800a */ 	lui	$t8,%hi(var800a45c0)
/*  f152608:	8f1845c0 */ 	lw	$t8,%lo(var800a45c0)($t8)
/*  f15260c:	26f70001 */ 	addiu	$s7,$s7,0x1
/*  f152610:	2673006c */ 	addiu	$s3,$s3,0x6c
/*  f152614:	02f8082a */ 	slt	$at,$s7,$t8
/*  f152618:	5420feb3 */ 	bnezl	$at,.L0f1520e8
/*  f15261c:	8e4b0000 */ 	lw	$t3,0x0($s2)
/*  f152620:	e7b600cc */ 	swc1	$f22,0xcc($sp)
.L0f152624:
/*  f152624:	87a8013e */ 	lh	$t0,0x13e($sp)
/*  f152628:	55000005 */ 	bnezl	$t0,.L0f152640
/*  f15262c:	8fb90140 */ 	lw	$t9,0x140($sp)
/*  f152630:	0fc004c0 */ 	jal	func0f001300
/*  f152634:	8fa40140 */ 	lw	$a0,0x140($sp)
/*  f152638:	afa20140 */ 	sw	$v0,0x140($sp)
/*  f15263c:	8fb90140 */ 	lw	$t9,0x140($sp)
.L0f152640:
/*  f152640:	3c0ab600 */ 	lui	$t2,0xb600
/*  f152644:	3c0b0006 */ 	lui	$t3,0x6
/*  f152648:	27290008 */ 	addiu	$t1,$t9,0x8
/*  f15264c:	afa90140 */ 	sw	$t1,0x140($sp)
/*  f152650:	af2b0004 */ 	sw	$t3,0x4($t9)
/*  f152654:	af2a0000 */ 	sw	$t2,0x0($t9)
/*  f152658:	8fb00140 */ 	lw	$s0,0x140($sp)
/*  f15265c:	3c0e0103 */ 	lui	$t6,0x103
/*  f152660:	35ce0040 */ 	ori	$t6,$t6,0x40
/*  f152664:	260d0008 */ 	addiu	$t5,$s0,0x8
/*  f152668:	afad0140 */ 	sw	$t5,0x140($sp)
/*  f15266c:	0fc2d3fa */ 	jal	currentPlayerGetUnk1750
/*  f152670:	ae0e0000 */ 	sw	$t6,0x0($s0)
/*  f152674:	ae020004 */ 	sw	$v0,0x4($s0)
/*  f152678:	8fb10140 */ 	lw	$s1,0x140($sp)
/*  f15267c:	3c080102 */ 	lui	$t0,0x102
/*  f152680:	35080040 */ 	ori	$t0,$t0,0x40
/*  f152684:	26380008 */ 	addiu	$t8,$s1,0x8
/*  f152688:	afb80140 */ 	sw	$t8,0x140($sp)
/*  f15268c:	0fc2d3ea */ 	jal	currentPlayerGetUnk173c
/*  f152690:	ae280000 */ 	sw	$t0,0x0($s1)
/*  f152694:	ae220004 */ 	sw	$v0,0x4($s1)
.L0f152698:
/*  f152698:	8fbf007c */ 	lw	$ra,0x7c($sp)
/*  f15269c:	8fa20140 */ 	lw	$v0,0x140($sp)
/*  f1526a0:	d7b40028 */ 	ldc1	$f20,0x28($sp)
/*  f1526a4:	d7b60030 */ 	ldc1	$f22,0x30($sp)
/*  f1526a8:	d7b80038 */ 	ldc1	$f24,0x38($sp)
/*  f1526ac:	d7ba0040 */ 	ldc1	$f26,0x40($sp)
/*  f1526b0:	d7bc0048 */ 	ldc1	$f28,0x48($sp)
/*  f1526b4:	d7be0050 */ 	ldc1	$f30,0x50($sp)
/*  f1526b8:	8fb00058 */ 	lw	$s0,0x58($sp)
/*  f1526bc:	8fb1005c */ 	lw	$s1,0x5c($sp)
/*  f1526c0:	8fb20060 */ 	lw	$s2,0x60($sp)
/*  f1526c4:	8fb30064 */ 	lw	$s3,0x64($sp)
/*  f1526c8:	8fb40068 */ 	lw	$s4,0x68($sp)
/*  f1526cc:	8fb5006c */ 	lw	$s5,0x6c($sp)
/*  f1526d0:	8fb60070 */ 	lw	$s6,0x70($sp)
/*  f1526d4:	8fb70074 */ 	lw	$s7,0x74($sp)
/*  f1526d8:	8fbe0078 */ 	lw	$s8,0x78($sp)
/*  f1526dc:	03e00008 */ 	jr	$ra
/*  f1526e0:	27bd0140 */ 	addiu	$sp,$sp,0x140
);

GLOBAL_ASM(
glabel func0f1526e4
/*  f1526e4:	27bdffe8 */ 	addiu	$sp,$sp,-24
/*  f1526e8:	afbf0014 */ 	sw	$ra,0x14($sp)
/*  f1526ec:	0fc545e0 */ 	jal	func0f151780
/*  f1526f0:	00000000 */ 	nop
/*  f1526f4:	0fc547b4 */ 	jal	func0f151ed0
/*  f1526f8:	00402025 */ 	or	$a0,$v0,$zero
/*  f1526fc:	8fbf0014 */ 	lw	$ra,0x14($sp)
/*  f152700:	27bd0018 */ 	addiu	$sp,$sp,0x18
/*  f152704:	03e00008 */ 	jr	$ra
/*  f152708:	00000000 */ 	nop
/*  f15270c:	00000000 */ 	nop
);
