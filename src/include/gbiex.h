#ifndef _IN_GBIEX_H
#define _IN_GBIEX_H

/**
 * 07 gSPColor - copy colors from segment + offset into DMEM
 *
 * upper word
 * 00FF0000	- number of bytes to copy minus 4 (eg. 4 colours = 0x0c)
 * 0000FFFF	- nubmer of bytes to copy
 *
 * lower word
 * 0F000000	- segment
 * 00FFFFFF	- offset in color table
 */
#define gSPColor(pkt, v, n)                           \
    gDma1p(pkt, G_COL, v, sizeof(Col)*(n),((n)-1)<<2)

#define gsSPColor(v, n, v0)                        \
    gsDma1p(G_COL, v, sizeof(Col)*(n), ((n)-1)<<2)

/**
 * B1	rsp_tri4
 * Draws up to four triangles at a time.
 * Expects values from 0-F, corresponding with # points declared by vertex command.
 * Triangles with all points set to 0 are not drawn.
 *
 * upper word
 * 0000F000	z4
 * 00000F00	z3
 * 000000F0	z2
 * 0000000F	z1
 *
 * lower word
 * F0000000	y4
 * 0F000000	x4
 * 00F00000	y3
 * 000F0000	x3
 * 0000F000	y2
 * 00000F00	x2
 * 000000F0	y1
 * 0000000F	x1
 */
#define	gSPTri4(pkt, x1, y1, z1, x2, y2, z2, x3, y3, z3, x4, y4, z4) \
{                                                                    \
    Gfx *_g = (Gfx *)(pkt);                                          \
    _g->words.w0 = (_SHIFTL(G_TRI4, 24, 8)                           \
            | _SHIFTL(z4, 12, 4)                                     \
            | _SHIFTL(z3, 8, 4)                                      \
            | _SHIFTL(z2, 4, 4)                                      \
            | _SHIFTL(z1, 0, 4));                                    \
    _g->words.w1 = (_SHIFTL(y4, 28, 4)                               \
            | _SHIFTL(x4, 24, 4)                                     \
            | _SHIFTL(y3, 20, 4)                                     \
            | _SHIFTL(x3, 16, 4)                                     \
            | _SHIFTL(y2, 12, 4)                                     \
            | _SHIFTL(x2, 8, 4)                                      \
            | _SHIFTL(y1, 4, 4)                                      \
            | _SHIFTL(x1, 0, 4));                                    \
}

#define gSPTri3(pkt, x1, y1, z1, x2, y2, z2, x3, y3, z3)      \
    gSPTri4(pkt, x1, y1, z1, x2, y2, z2, x3, y3, z3, 0, 0, 0)

#define gSPTri2(pkt, x1, y1, z1, x2, y2, z2)               \
    gSPTri4(pkt, x1, y1, z1, x2, y2, z2, 0, 0, 0, 0, 0, 0)

#define gSPTri1(pkt, x1, y1, z1)                        \
    gSPTri4(pkt, x1, y1, z1, 0, 0, 0, 0, 0, 0, 0, 0, 0)

#define	gDPLoadTLUT06(pkt, a, b, c, d)				                                        \
{                                                                                           \
	Gfx *_g = (Gfx *)pkt;                                                                   \
	_g->words.w0 = _SHIFTL(G_LOADTLUT, 24, 8) | _SHIFTL((a), 14, 10) | _SHIFTL((b), 2, 10); \
	_g->words.w1 = _SHIFTL(0x06, 24, 8) | _SHIFTL((c), 14, 10) | _SHIFTL((d), 2, 10);       \
}

/**
 * Like gDPSetPrimColor, but is useful when the input colour is already in
 * RGBA format. It avoids unnecessary bitshifting and masking.
 */
#define	gDPSetPrimColorViaWord(pkt, m, l, rgba)     \
{                                                   \
	Gfx *_g = (Gfx *)(pkt);                         \
	_g->words.w0 =	(_SHIFTL(G_SETPRIMCOLOR, 24, 8) \
			| _SHIFTL(m, 8, 8)                      \
			| _SHIFTL(l, 0, 8));                    \
	_g->words.w1 =  (rgba);                         \
}

#define	gDPSetEnvColorViaWord(pkt, rgba) gDPSetColor(pkt, G_SETENVCOLOR, rgba)
#define	gDPSetFogColorViaWord(pkt, rgba) gDPSetColor(pkt, G_SETFOGCOLOR, rgba)

/**
 * gDPFillRectangleScaled - a wrapper around gDPFillRectangle which applies
 * g_UiScaleX to the X coordinates.
 *
 * g_UiScaleX is normally 1, but 2 when using hi-res.
 */
#define gDPFillRectangleScaled(pkt, x1, y1, x2, y2) gDPFillRectangle(pkt, (x1) * g_UiScaleX, y1, (x2) * g_UiScaleX, y2)

#define gDPHudRectangle(pkt, x1, y1, x2, y2) gDPFillRectangle(pkt, (x1) * g_UiScaleX, y1, ((x2 + 1)) * g_UiScaleX, (y2) + 1)

/**
 * Custom combiner modes.
 *
 * Modes 6-10 are copies of the following but they replace
 * SHADE with ENVIRONMENT in the alpha half:
 *
 * 06: G_CC_MODULATEIA2
 * 07: G_CC_MODULATEIA
 * 08: G_CC_MODULATEI2
 * 09: G_CC_MODULATEI
 * 10: G_CC_SHADE
 *
 * Modes 12-16 are copies of the following but they replace
 * SHADE with SCALE in the colour half:
 *
 * 12: G_CC_MODULATEI
 * 13: G_CC_MODULATEIA
 * 14: G_CC_SHADE
 * 15: G_CC_MODULATEI2
 * 16: G_CC_MODULATEIA2
 *
 * Summary of modes:
 * 00:  T0*EV          T0*EV
 * 01:  CM             (1-CM)*PR+CM
 * 02:  PR             T0*PR
 * 03:  CM*EV          CM*EV
 * 04:  SH             T0*SH
 * 05:  T1*SC          T1*PL
 * 06:  CM*SH          CM*EV
 * 07:  T0*SH          T0*EV
 * 08:  CM*SH          EV
 * 09:  T0*SH          EV
 * 10:  SH             EV
 * 11:  (T1-T0)*LF+T0  T1
 * 12:  T0*SC          SH
 * 13:  T0*SC          T0*SH
 * 14:  SC             SH
 * 15:  CM*SC          SH
 * 16:  CM*SC          CM*SH
 * 17:  (T0-EV)*SA+EV  (T0-EV)*SH+EV
 * 18:  CM*SH          CM
 * 19:  (T0-EV)*SA+EV  T0*EV
 * 20:  CM*SH          CM*SH+PR
 * 21:  (T1-T0)*LF+T0  SH+EV
 * 22:  (T1-T0)*LF+T0  (SH-EV)*T0
 * 23:  CM*SH          PR+CM
 * 24:  (T1-T0)*LF+T0  (1-SH)*EV
 * 25:  (T1-T0)*LF+T0  EV
 * 26:  (T1-T0)*LF+T0  T0*EV
 * 27:  (PR-EV)*T0+EV  (PR-EV)*T0+EV
 */
#define G_CC_CUSTOM_00  TEXEL0,    0,           ENVIRONMENT,  0,           TEXEL0,    0,           ENVIRONMENT,   0
#define G_CC_CUSTOM_01  0,         0,           0,            COMBINED,    1,         COMBINED,    PRIMITIVE,     COMBINED
#define G_CC_CUSTOM_02  0,         0,           0,            PRIMITIVE,   TEXEL0,    0,           PRIMITIVE,     0
#define G_CC_CUSTOM_03  COMBINED,  0,           ENVIRONMENT,  0,           COMBINED,  0,           ENVIRONMENT,   0
#define G_CC_CUSTOM_04  0,         0,           0,            SHADE,       TEXEL0,    0,           SHADE,         0
#define G_CC_CUSTOM_05  TEXEL1,    0,           SCALE,        0,           TEXEL1,    0,           PRIM_LOD_FRAC, 0
#define G_CC_CUSTOM_06  COMBINED,  0,           SHADE,        0,           COMBINED,  0,           ENVIRONMENT,   0
#define G_CC_CUSTOM_07  TEXEL0,    0,           SHADE,        0,           TEXEL0,    0,           ENVIRONMENT,   0
#define G_CC_CUSTOM_08  COMBINED,  0,           SHADE,        0,           0,         0,           0,             ENVIRONMENT
#define G_CC_CUSTOM_09  TEXEL0,    0,           SHADE,        0,           0,         0,           0,             ENVIRONMENT
#define G_CC_CUSTOM_10  0,         0,           0,            SHADE,       0,         0,           0,             ENVIRONMENT
#define G_CC_CUSTOM_11  TEXEL1,    TEXEL0,      LOD_FRACTION, TEXEL0,      1,         0,           TEXEL1,        0
#define G_CC_CUSTOM_12  TEXEL0,    0,           SCALE,        0,           0,         0,           0,             SHADE
#define G_CC_CUSTOM_13  TEXEL0,    0,           SCALE,        0,           TEXEL0,    0,           SHADE,         0
#define G_CC_CUSTOM_14  1,         0,           SCALE,        0,           0,         0,           0,             SHADE
#define G_CC_CUSTOM_15  COMBINED,  0,           SCALE,        0,           0,         0,           0,             SHADE
#define G_CC_CUSTOM_16  COMBINED,  0,           SCALE,        0,           COMBINED,  0,           SHADE,         0
#define G_CC_CUSTOM_17  TEXEL0,    ENVIRONMENT, SHADE_ALPHA,  ENVIRONMENT, TEXEL0,    ENVIRONMENT, SHADE,         ENVIRONMENT
#define G_CC_CUSTOM_18  COMBINED,  0,           SHADE,        0,           0,         0,           0,             COMBINED
#define G_CC_CUSTOM_19  TEXEL0,    ENVIRONMENT, SHADE_ALPHA,  ENVIRONMENT, TEXEL0,    0,           ENVIRONMENT,   0
#define G_CC_CUSTOM_20  COMBINED,  0,           SHADE,        0,           COMBINED,  0,           SHADE,         PRIMITIVE
#define G_CC_CUSTOM_21  TEXEL1,    TEXEL0,      LOD_FRACTION, TEXEL0,      1,         0,           SHADE,         ENVIRONMENT
#define G_CC_CUSTOM_22  TEXEL1,    TEXEL0,      LOD_FRACTION, TEXEL0,      SHADE,     ENVIRONMENT, TEXEL0,        0
#define G_CC_CUSTOM_23  COMBINED,  0,           SHADE,        0,           1,         0,           PRIMITIVE,     COMBINED
#define G_CC_CUSTOM_24  TEXEL1,    TEXEL0,      LOD_FRACTION, TEXEL0,      1,         SHADE,       ENVIRONMENT,   0
#define G_CC_CUSTOM_25  TEXEL1,    TEXEL0,      LOD_FRACTION, TEXEL0,      1,         0,           ENVIRONMENT,   0
#define G_CC_CUSTOM_26  TEXEL1,    TEXEL0,      LOD_FRACTION, TEXEL0,      TEXEL0,    0,           ENVIRONMENT,   0
#define G_CC_CUSTOM_27  PRIMITIVE, ENVIRONMENT, TEXEL0,       ENVIRONMENT, PRIMITIVE, ENVIRONMENT, TEXEL0,        ENVIRONMENT

#endif
