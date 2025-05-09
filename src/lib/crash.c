#include <ultra64.h>
#include "os_internal.h"
#include "constants.h"
#include "bss.h"
#include "lib/tlb.h"
#include "lib/crash.h"
#include "lib/dma.h"
#include "lib/rmon.h"
#include "lib/sched.h"
#include "lib/str.h"
#include "lib/vi.h"
#include "data.h"
#include "types.h"

#define MSG_FAULT 0x10

#if VERSION >= VERSION_NTSC_1_0
#define MAX_LINES 29
#else
#define MAX_LINES 31
#endif

#define CHAR_W 4
#define CHAR_H 7

#if VERSION < VERSION_NTSC_1_0
char g_CrashMessage[70];
#endif

OSThread g_FaultThread;
u8 g_FaultStack[STACKSIZE_FAULT];
OSMesgQueue g_FaultMesgQueue;
OSMesg g_FaultMesg;

#if VERSION == VERSION_NTSC_BETA
u8 g_CrashHasMessage = false;
#else
bool g_CrashEnabled = false;
#endif

s16 g_CrashCurX = 0;
s16 g_CrashCurY = 0;

struct crashdescription {
	u32 mask;
	u32 value;
	const char *text;
};

struct crashdescription g_CrashCauseDescriptions[] = {
	{ CAUSE_BD,      CAUSE_BD,    "BD"                                              },
	{ CAUSE_IP8,     CAUSE_IP8,   "IP8"                                             },
	{ CAUSE_IP7,     CAUSE_IP7,   "IP7"                                             },
	{ CAUSE_IP6,     CAUSE_IP6,   "IP6"                                             },
	{ CAUSE_IP5,     CAUSE_IP5,   "IP5"                                             },
	{ CAUSE_IP4,     CAUSE_IP4,   "IP4"                                             },
	{ CAUSE_IP3,     CAUSE_IP3,   "IP3"                                             },
	{ CAUSE_SW2,     CAUSE_SW2,   "IP2"                                             },
	{ CAUSE_SW1,     CAUSE_SW1,   "IP1"                                             },
	{ CAUSE_EXCMASK, EXC_INT,     "Int"                                             },
	{ CAUSE_EXCMASK, EXC_MOD,     "TLBmod"                                          },
	{ CAUSE_EXCMASK, EXC_RMISS,   "TLBload"                                         },
	{ CAUSE_EXCMASK, EXC_WMISS,   "TLBstore"                                        },
	{ CAUSE_EXCMASK, EXC_RADE,    "Address error on load or instruction fetch"      },
	{ CAUSE_EXCMASK, EXC_WADE,    "Address error on store"                          },
	{ CAUSE_EXCMASK, EXC_IBE,     "Bus error exception on instruction fetch"        },
	{ CAUSE_EXCMASK, EXC_DBE,     "Bus error exception on data reference"           },
	{ CAUSE_EXCMASK, EXC_SYSCALL, "Syscall"                                         },
	{ CAUSE_EXCMASK, EXC_BREAK,   "Brk"                                             },
	{ CAUSE_EXCMASK, EXC_II,      "Reserved instruction"                            },
	{ CAUSE_EXCMASK, EXC_CPU,     "Cop unusable"                                    },
	{ CAUSE_EXCMASK, EXC_OV,      "Overflow"                                        },
	{ CAUSE_EXCMASK, EXC_TRAP,    "Trap"                                            },
	{ CAUSE_EXCMASK, EXC_VCEI,    "Virtual coherency exception on intruction fetch" },
	{ CAUSE_EXCMASK, EXC_FPE,     "Fp exception"                                    },
	{ CAUSE_EXCMASK, EXC_WATCH,   "Watchpoint"                                      },
	{ CAUSE_EXCMASK, EXC_VCED,    "Virtual coherency exception on data reference"   },
	{ 0,             0,           ""                                                },
};

struct crashdescription g_CrashSrDescriptions[] = {
	{ SR_CU3,      SR_CU3,     "CU3" },
	{ SR_CU2,      SR_CU2,     "CU2" },
	{ SR_CU1,      SR_CU1,     "CU1" },
	{ SR_CU0,      SR_CU0,     "CU0" },
	{ SR_RP,       SR_RP,      "RP"  },
	{ SR_FR,       SR_FR,      "FR"  },
	{ SR_RE,       SR_RE,      "RE"  },
	{ SR_BEV,      SR_BEV,     "BEV" },
	{ SR_TS,       SR_TS,      "TS"  },
	{ SR_SR,       SR_SR,      "SR"  },
	{ SR_CH,       SR_CH,      "CH"  },
	{ SR_CE,       SR_CE,      "CE"  },
	{ SR_DE,       SR_DE,      "DE"  },
	{ SR_IBIT8,    SR_IBIT8,   "IM8" },
	{ SR_IBIT7,    SR_IBIT7,   "IM7" },
	{ SR_IBIT6,    SR_IBIT6,   "IM6" },
	{ SR_IBIT5,    SR_IBIT5,   "IM5" },
	{ SR_IBIT4,    SR_IBIT4,   "IM4" },
	{ SR_IBIT3,    SR_IBIT3,   "IM3" },
	{ SR_IBIT2,    SR_IBIT2,   "IM2" },
	{ SR_IBIT1,    SR_IBIT1,   "IM1" },
	{ SR_KX,       SR_KX,      "KX"  },
	{ SR_SX,       SR_SX,      "SX"  },
	{ SR_UX,       SR_UX,      "UX"  },
	{ SR_KSU_MASK, SR_KSU_USR, "USR" },
	{ SR_KSU_MASK, SR_KSU_SUP, "SUP" },
	{ SR_KSU_MASK, SR_KSU_KER, "KER" },
	{ SR_ERL,      SR_ERL,     "ERL" },
	{ SR_EXL,      SR_EXL,     "EXL" },
	{ SR_IE,       SR_IE,      "IE"  },
	{ 0,           0,          ""    },
};

struct crashdescription g_CrashFpcsrDescriptions[] = {
	{ FPCSR_FS,      FPCSR_FS,    "FS"            },
	{ FPCSR_C,       FPCSR_C,     "C"             },
	{ FPCSR_CE,      FPCSR_CE,    "Unimplemented" },
	{ FPCSR_CV,      FPCSR_CV,    "Invalid op"    },
	{ FPCSR_CZ,      FPCSR_CZ,    "/ by 0.0"      },
	{ FPCSR_CO,      FPCSR_CO,    "Overflow"      },
	{ FPCSR_CU,      FPCSR_CU,    "Underflow"     },
	{ FPCSR_CI,      FPCSR_CI,    "Inexact op"    },
	{ FPCSR_EV,      FPCSR_EV,    "EV"            },
	{ FPCSR_EZ,      FPCSR_EZ,    "EZ"            },
	{ FPCSR_EO,      FPCSR_EO,    "EO"            },
	{ FPCSR_EU,      FPCSR_EU,    "EU"            },
	{ FPCSR_EI,      FPCSR_EI,    "EI"            },
	{ FPCSR_FV,      FPCSR_FV,    "FV"            },
	{ FPCSR_FZ,      FPCSR_FZ,    "FZ"            },
	{ FPCSR_FO,      FPCSR_FO,    "FO"            },
	{ FPCSR_FU,      FPCSR_FU,    "FU"            },
	{ FPCSR_FI,      FPCSR_FI,    "FI"            },
	{ FPCSR_RM_MASK, FPCSR_RM_RN, "RN"            },
	{ FPCSR_RM_MASK, FPCSR_RM_RZ, "RZ"            },
	{ FPCSR_RM_MASK, FPCSR_RM_RP, "RP"            },
	{ FPCSR_RM_MASK, FPCSR_RM_RM, "RM"            },
	{ 0,             0,           ""              },
};

char (*g_CrashCharBuffer)[71] = NULL;

#ifdef DEBUG
u32 var8005f138nb[] = {
	0x00000000, 0x22220200, 0x55000000, 0x05f5f500,
	0x27427200, 0x05124500, 0x34255300, 0x22000000,
	0x24444420, 0x42222240, 0x06f6f600, 0x00272000,
	0x00000240, 0x00070000, 0x00000200, 0x11224480,
	0x25555200, 0x26222700, 0x25125700, 0x61211600,
	0x33557300, 0x64611600, 0x24655200, 0x71112200,
	0x25755200, 0x25531600, 0x00200200, 0x00200640,
	0x01242100, 0x00707000, 0x04212400, 0x07120200,
	0x25ff5700, 0x02557d00, 0x06575e00, 0x07445300,
	0x07555600, 0x07565700, 0x07564400, 0x07c95700,
	0x05575500, 0x07222700, 0x03111600, 0x05665500,
	0x04445f00, 0x0dff9d00, 0x0f777d00, 0x07dd5700,
	0x07564600, 0x07995770, 0x07565500, 0x07461e00,
	0x07222200, 0x0d999600, 0x0d552200, 0x0df77500,
	0x0d625500, 0x05622600, 0x07125700, 0x32222230,
	0x44222110, 0x62222260, 0x25000000, 0x00000700,
	0x42200000, 0x0067d700, 0x44755700, 0x00788600,
	0x117dd700, 0x006fc700, 0x32722700, 0x007dd730,
	0x44755500, 0x02622700, 0x02711130, 0x44766500,
	0x62222700, 0x00ffff00, 0x00755d00, 0x006dd600,
	0x00755740, 0x00799710, 0x00744600, 0x00775700,
	0x02722300, 0x00555700, 0x00552200, 0x00577500,
	0x00562500, 0x00552220, 0x00703700, 0x12242210,
	0x02222220, 0x42212240, 0x005a0000,
};
#endif

u16 *g_CrashFrameBuffer = NULL;

extern u32 _libSegmentStart;
extern u32 _libSegmentEnd;

void faultproc(void *arg0);
u32 crash_generate(OSThread *thread, u32 *callstack, s32 *tracelen);
void crash_print_description(u32 mask, char *label, struct crashdescription *descriptions);
void crash_scroll(s32 numlines);

#if VERSION < VERSION_NTSC_1_0
void crash_set_message(char *string)
{
	strncpy(g_CrashMessage, string, sizeof(g_CrashMessage));
	g_CrashHasMessage = true;
}
#endif

void crash_create_thread(void)
{
	osCreateMesgQueue(&g_FaultMesgQueue, &g_FaultMesg, 1);
	osCreateThread(&g_FaultThread, THREAD_FAULT, faultproc, NULL, &g_FaultStack[1024], THREADPRI_FAULT);
	osStartThread(&g_FaultThread);
}

void faultproc(void *arg0)
{
	OSMesg msg = 0;
	OSIntMask mask;
	u32 callstack[10];
	s32 tracelen;
	static OSThread *curr;
	static OSThread *last;

	osSetEventMesg(OS_EVENT_FAULT, &g_FaultMesgQueue, (OSMesg) MSG_FAULT);
	last = NULL;

	while (true) {
		osRecvMesg(&g_FaultMesgQueue, &msg, OS_MESG_BLOCK);
		mask = osSetIntMask(1);
		curr = __osGetCurrFaultedThread();

		if (1);

		if (!curr) {
			continue;
		}

		osSetIntMask(mask);

#if VERSION == VERSION_PAL_BETA
		if (!g_CrashEnabled) {
			continue;
		}
#endif

#ifdef DEBUG
		crash_generate(curr, callstack, &tracelen);
		sched_set_crashed_unexpectedly(true);
#endif
	}
}

#if VERSION == VERSION_NTSC_BETA
u32 var8009710cnb;
char *var80097110nb;
char *var80097114nb;
u32 var80097118nb[24];
#endif

#ifdef DEBUG
char var80097178nb[MAX_LINES + 1][71];
#endif

/**
 * Given a pointer to an instruction and a stack frame pointer, attempt to find
 * the calling function. Return the address of the caller's stack frame and
 * populate regs with stack addresses where that register was saved. This can be
 * used to retrieve the RA value and invoke crash_get_parent_stack_frame again to
 * build a backtrace.
 *
 * origptr is a pointer to an instruction. This should be either the value of
 *     the PC register of the faulted thread, or an RA register if searching a
 *     parent.
 * minaddr is the memory address of the start of the code segment,
 *     ie. 0x70001050. This is used to prevent the function from walking out of
 *     the code segment.
 * origsp is the address of the stack frame for the given origptr.
 * regs is a pointer to an array of 32 words.
 *
 * The function works by walking backwards one instruction at a time, looking
 * for stack frame adjustments and stores of $ra to the stack. Once both of
 * these are are found, the function returns with this information.
 *
 * The function will return 0 if it can't reliably find the caller. This will
 * happen if the function being analysed didn't adjust the stack pointer or
 * store $ra in the stack. It can also fail if the function being analysed uses
 * returns within branches.
 */
u32 crash_get_parent_stack_frame(u32 *origptr, u32 *minaddr, u32 origsp, u32 *regs)
{
	u32 sp = origsp;
	u32 *ptr;
	bool foundaddsp = false;
	bool foundra = false;
	s32 regid;
	u32 instruction;
	s16 value;

	// Clear the regs array (note: reusing the instruction variable here)
	for (instruction = 0; instruction < 32; instruction++) regs[instruction] = 0;

	// Walk backwards through the instructions
	for (ptr = origptr; ptr >= minaddr; ptr--) {
		instruction = *ptr;
		value = instruction & 0xffff;

		if ((instruction & 0xffff0000) == 0x27bd0000) {
			// Found an addiu $sp, $sp, <amount> instruction, which adjusts the
			// stack pointer. These can exist near the start and end of any
			// function. The "add" at the start is done with a negative value
			// which increases the size of the stack, as the stack expands to
			// the left. This function is interested in these negative adds,
			// because it needs to reverse it and move the sp variable forward
			// to the next stack frame (the frame of the caller).

			foundaddsp = true;

			if (value > 0) {
				// Found the addiu sp at the end of the function. It's pretty
				// rare to crash (or jump elsewhere) after restoring the sp,
				// so this situation is not supported by this function.
				break;
			}

			// Change sp to point to the caller's stack frame
			sp -= (value >> 2) * 4;

			if (foundra) {
				break;
			}
		} else if ((instruction & 0xffe00000) == 0xafa00000) {
			// Found a store word instruction that stores to the stack.
			// The encoding of this instruction is:
			//
			//     oooooodd dddsssss iiiiiiii iiiiiiii
			//
			// Where:
			//     o is the opcode (already known to be sw).
			//     d is the destination register (already known to be $sp)
			//     s is the source register
			//     i is the offset to store to
			//
			// This looks for a store from $ra to the stack, so the value can
			// be read from the stack and used to find the caller's address.

			regid = (instruction >> 16) & 0x1f;
			regs[regid] = sp + (value >> 2) * 4;

			if (regid == 31) {
				foundra = true;
			}

			if (foundaddsp && foundra) {
				break;
			}
		} else if (instruction == 0x03e00008) {
			// Found a jr $ra statement, which is a return. This will happen if
			// this loop has walked out of the current function and into the one
			// prior to it, so bail.

			// It can also happen if the function has return statements within
			// branches, but handling these correctly would involve a lot of
			// complexity so that's unsupported. Because of this, this function
			// can end the backtrace prematurely if it encounters a function
			// that does this.
			break;
		}
	}

	if (foundaddsp && foundra) {
		return sp;
	}

	return 0;
}

bool crash_is_return_address(u32 *instruction)
{
	if (((uintptr_t)instruction % 4) == 0
			&& (uintptr_t)instruction >= (uintptr_t)vm_boot
			&& (uintptr_t)instruction <= (uintptr_t)&_libSegmentEnd) {
		// This condition can never pass because 9 is masked out
		if ((instruction[-2] & 0xfc00003c) == 9) {
			return true;
		}

		// If 2 instructions earlier was a jal
		if ((instruction[-2] & 0xfc000000) == 0x0c000000) {
			return true;
		}
	}

	return false;
}

#if VERSION < VERSION_NTSC_1_0
s32 crash_get_str_len(char *str)
{
	s32 i = 0;
	char c = *str++;

	while (c != '\0') {
		i++;

		if (i >= 256) {
			break;
		}

		c = *str++;
	}

	return i;
}
#endif

#if VERSION < VERSION_NTSC_1_0
u32 crash0000c52cnb(u32 romaddr)
{
	u32 addr;

	dma_exec(var80097118nb, romaddr, 0x60);

	var8009710cnb = var80097118nb[0];
	var80097110nb = (char *)&var80097118nb[1];
	var80097114nb = (char *)(crash_get_str_len(var80097110nb) + (uintptr_t)var80097110nb + 1);

	addr = romaddr + crash_get_str_len(var80097110nb) + crash_get_str_len(var80097114nb) + 6;

	if (addr % 4) {
		addr = (addr | 3) + 1;
	}

	return addr;
}
#endif

#if VERSION < VERSION_NTSC_1_0
bool crash0000c5d8nb(u32 arg0)
{
	u32 this = 0x00e00004;
	u32 prev = 0x00e00004;

	while (true) {
		u32 next = crash0000c52cnb(this);

		if (arg0 >= var8009710cnb) {
			prev = this;

			if (var8009710cnb == 0) {
				return false;
			}

			this = next;
		} else {
			break;
		}
	}

	crash0000c52cnb(prev);

	return true;
}
#endif

#if VERSION < VERSION_NTSC_1_0
bool crash0000c660nb(void)
{
	crash0000c52cnb(0xe00000);

	return var8009710cnb == 0x826475be;
}
#endif

#if VERSION < VERSION_NTSC_1_0
void crash0000c694nb(void)
{
	s32 numvalid = 0;
	s32 i;

	for (i = 0; i < 4; i++) {
		if (var80097110nb[i] >= '!' && var80097110nb[i] <= 0x7f) {
			numvalid++;
		}
	}

	if (numvalid == 4) {
		rmon_printf("%.49s", var80097110nb);
	} else {
		rmon_printf("???");
	}
}
#endif

#if VERSION < VERSION_NTSC_1_0
void crash0000c714nb(void)
{
	s32 numvalid = 0;
	s32 i;

	for (i = 0; i < 4; i++) {
		if (var80097114nb[i] >= '!' && var80097114nb[i] <= 0x7f) {
			numvalid++;
		}
	}

	if (numvalid == 4) {
		rmon_printf("%.41s", var80097114nb);
	} else {
		rmon_printf("???");
	}
}
#endif

#if VERSION < VERSION_NTSC_1_0
void crash0000c794nb(void)
{
	rmon_printf("%08x", var8009710cnb);
}
#endif

#if VERSION < VERSION_NTSC_1_0
u32 crash0000c7c0nb(void)
{
	return var8009710cnb;
}
#endif

u32 crash_get_stack_end(u32 sp, s32 tid)
{
	u32 start;
	u32 end;

	if (tid <= 0 || tid > 6U) {
		rmon_printf("Bad tid\n");
		return 0;
	}

	start = (uintptr_t)g_StackLeftAddrs[tid];
	end = (uintptr_t)g_StackRightAddrs[tid];

	if (sp >= K0BASE) {
		return end;
	}

	return (sp & 0xf0000000) | (end - start);
}

u32 crash_get_stack_start(u32 sp, s32 tid)
{
	u32 start;

	if (tid <= 0 || tid > 6U) {
		rmon_printf("Bad tid\n");
		return 0;
	}

	start = (uintptr_t)g_StackLeftAddrs[tid];

	if (sp >= K0BASE) {
		return start;
	}

	return sp & 0xf0000000;
}

bool crash_is_double(f32 value)
{
	u32 bits = *(u32*)&value;
	u32 fraction = bits & 0x7fffff;
	u8 exponent = (u8)(bits >> 23);

	return fraction == 0 || (exponent != 0 && exponent != 0xff);
}

void crash_print_float(s32 index, f32 value)
{
	if (crash_is_double(value)) {
		rmon_printf("%s%s%02d: % .7e ", "", "", index, (f64)value);
	} else {
		u32 bits = *(u32 *)&value;
		rmon_printf("%02d: I%d.%03d.%07d ", index, (bits & 0x80000000) >> 31, (bits & 0x7f800000) >> 23, bits & 0x7fffff);
	}
}

void crash_print_2_floats(s32 index, f32 value1, f32 value2)
{
	crash_print_float(index, value1);
	rmon_printf(" ");

	crash_print_float(index + 1, value2);
	rmon_printf("\n");
}

void crash_print_3_floats(s32 index, f32 value1, f32 value2, f32 value3)
{
	crash_print_float(index, value1);
	rmon_printf(" ");

	crash_print_float(index + 1, value2);
	rmon_printf(" ");

	crash_print_float(index + 2, value3);
	rmon_printf("\n");
}

u32 crash_generate(OSThread *thread, u32 *callstack, s32 *tracelen)
{
	s32 i;
	u32 ptr;
	u32 *sp;
	u32 regs[32];
#if VERSION == VERSION_NTSC_BETA
	s32 j;
	u32 *stackstart;
	u32 *stackend;
#else
	u32 *stackend;
	u32 *stackstart;
#endif
	__OSThreadContext *ctx = &thread->context;
	bool done;
	u32 *tmpsp;
#if VERSION == VERSION_NTSC_BETA
	s32 len;
#endif

	rmon_printf("\n\nFAULT-\n");

#if VERSION >= VERSION_NTSC_1_0
	if (!g_Vars.fourmeg2player)
#endif
	{
		// Print a stack trace in a dodgy way.
		// It works by iterating through the stack allocation, looking for any
		// values which could potentially be a return address, and prints them.
		u32 *stackend = (u32 *) crash_get_stack_end(ctx->sp, thread->id);
		rmon_printf("DodgyStackTrace: %08llx ", ctx->ra & 0xffffffff);
		tmpsp = (u32 *) ctx->sp;

		while (tmpsp < stackend) {
			if (crash_is_return_address((u32 *)*tmpsp)) {
				rmon_printf("%08x ", *tmpsp);
			}

			tmpsp++;
		}

		rmon_printf(".\n");
	}

#if VERSION >= VERSION_NTSC_1_0
	rmon_printf("%H#@! Another Perfect Crash (tm)\n");
#else
	rmon_printf("\nPerfect Crash (tm)\n\n");
#endif

#if VERSION >= VERSION_NTSC_1_0
	if (!g_Vars.fourmeg2player)
#endif
	{
		// Print floating point registers
		crash_print_2_floats(0, ctx->fp0.f.f_odd, ctx->fp0.f.f_even);
		crash_print_3_floats(2, ctx->fp2.f.f_odd, ctx->fp2.f.f_even, ctx->fp4.f.f_odd);
		crash_print_3_floats(5, ctx->fp4.f.f_even, ctx->fp6.f.f_odd, ctx->fp6.f.f_even);
		crash_print_3_floats(8, ctx->fp8.f.f_odd, ctx->fp8.f.f_even, ctx->fp10.f.f_odd);
		crash_print_3_floats(11, ctx->fp10.f.f_even, ctx->fp12.f.f_odd, ctx->fp12.f.f_even);
		crash_print_3_floats(14, ctx->fp14.f.f_odd, ctx->fp14.f.f_even, ctx->fp16.f.f_odd);
		crash_print_3_floats(17, ctx->fp16.f.f_even, ctx->fp18.f.f_odd, ctx->fp18.f.f_even);
		crash_print_3_floats(20, ctx->fp20.f.f_odd, ctx->fp20.f.f_even, ctx->fp22.f.f_odd);
		crash_print_3_floats(23, ctx->fp22.f.f_even, ctx->fp24.f.f_odd, ctx->fp24.f.f_even);
		crash_print_3_floats(26, ctx->fp26.f.f_odd, ctx->fp26.f.f_even, ctx->fp28.f.f_odd);
		crash_print_3_floats(29, ctx->fp28.f.f_even, ctx->fp30.f.f_odd, ctx->fp30.f.f_even);
	}

	// Print integer registers
	rmon_printf("at 0x%016llx v0 0x%016llx v1 0x%016llx\n", ctx->at, ctx->v0, ctx->v1);
	rmon_printf("a0 0x%016llx a1 0x%016llx a2 0x%016llx\n", ctx->a0, ctx->a1, ctx->a2);
	rmon_printf("a3 0x%016llx t0 0x%016llx t1 0x%016llx\n", ctx->a3, ctx->t0, ctx->t1);
	rmon_printf("t2 0x%016llx t3 0x%016llx t4 0x%016llx\n", ctx->t2, ctx->t3, ctx->t4);
	rmon_printf("t5 0x%016llx t6 0x%016llx t7 0x%016llx\n", ctx->t5, ctx->t6, ctx->t7);
	rmon_printf("s0 0x%016llx s1 0x%016llx s2 0x%016llx\n", ctx->s0, ctx->s1, ctx->s2);
	rmon_printf("s3 0x%016llx s4 0x%016llx s5 0x%016llx\n", ctx->s3, ctx->s4, ctx->s5);
	rmon_printf("s6 0x%016llx s7 0x%016llx t8 0x%016llx\n", ctx->s6, ctx->s7, ctx->t8);
	rmon_printf("t9 0x%016llx gp 0x%016llx sp 0x%016llx\n", ctx->t9, ctx->gp, ctx->sp);
	rmon_printf("s8 0x%016llx ra 0x%016llx\n", ctx->s8, ctx->ra);

#if VERSION >= VERSION_NTSC_1_0
	rmon_printf("TID %d epc %08x caus %08x fp %08x badv %08x sr %08x\n",
			thread->id, ctx->pc, ctx->cause, ctx->fpcsr, ctx->badvaddr, ctx->sr);
#else
	rmon_printf("TID %d epc %08x cause %08x fp %08x badv %08x sr %08x\n",
			thread->id, ctx->pc, ctx->cause, ctx->fpcsr, ctx->badvaddr, ctx->sr);
#endif

	// Print the address of the faulted instruction, along with the instruction
	// itself and the next three - presumably to help the developer locate it.
	rmon_printf("dshex -a %08x %08x %08x %08x %08x\n", ctx->pc,
			((u32 *)ctx->pc)[0],
			((u32 *)ctx->pc)[1],
			((u32 *)ctx->pc)[2],
			((u32 *)ctx->pc)[3]);

	crash_print_description(ctx->cause, "cause", g_CrashCauseDescriptions);
	rmon_printf(" : ");
	crash_print_description(ctx->fpcsr, "fpcsr", g_CrashFpcsrDescriptions);
	rmon_printf("\n");

	// Print a proper stack trace
	i = 0;
	done = false;
	sp = (u32 *)ctx->sp;
	stackend = (u32 *) crash_get_stack_end((uintptr_t)sp, thread->id);
	stackstart = (u32 *) crash_get_stack_start((uintptr_t)sp, thread->id);
	ptr = ctx->pc;
	*tracelen = 0;
	rmon_printf("nearl: ");

	while (!done) {
		sp = (u32 *) crash_get_parent_stack_frame((u32 *) ptr, &_libSegmentStart, (uintptr_t)sp, regs);
		rmon_printf(" %08x ", ptr);

		callstack[*tracelen] = ptr;
		*tracelen = *tracelen + 1;

		if (i == 4) {
			rmon_printf("\n       ");
		}

		if (sp == NULL) {
			sp = (u32 *)ctx->sp;
		}

		if (regs[31] == 0) {
			ptr = ctx->ra;
		} else {
			ptr = *(u32 *)(regs[31]);
		}

		if (sp < stackstart || sp >= stackend || stackend == &sp[4] || ptr == 0) {
			break;
		}

		done = i >= 9;
		i++;
	}

#if VERSION == VERSION_NTSC_BETA
	g_CrashCurY = 31;

	if (g_CrashHasMessage) {
		g_CrashCurX = 0;

		for (len = 0; len < 71U; len++) {
			if (g_CrashMessage[len] == '\0') {
				break;
			}
		}

		g_CrashCurX = (71 - len) / 2;

		for (j = 0; j < len; j++) {
			crash_append_char(g_CrashMessage[j]);
		}
	} else {
		g_CrashCurX = 32;

		crash_append_char('C');
		crash_append_char('R');
		crash_append_char('A');
		crash_append_char('S');
		crash_append_char('H');
		crash_append_char('E');
		crash_append_char('D');
	}
#endif

	rmon_printf("\n");
	rmon_printf("\n");

	return 0;
}

void crash_print_description(u32 mask, char *label, struct crashdescription *description)
{
	bool first = true;
	s32 i;

	rmon_printf("%s <", label);

	while (description->mask != 0) {
		if ((description->mask & mask) == description->value) {
			if (first) {
				first = false;
			} else {
				rmon_printf(",");
			}

			rmon_printf("%s", description->text);
		}

		description++;
	}

	rmon_printf(">");
}

void crash_put_char(s32 x, s32 y, char c)
{
	if (c == '\t' || c == '\n') {
		c = '\0';
	}

	if ((c > '\0' && c < ' ') || c > '~') {
		c = '?';
	}

	if (x >= 0 && x < 72 && y >= 0 && y <= MAX_LINES && g_CrashCharBuffer != NULL) {
		g_CrashCharBuffer[y][x] = c;
	}
}

void crash_append_char(char c)
{
	if (c == '\0') {
		return;
	}

	if (c == '\t') {
		do {
			crash_append_char(' ');
		} while (g_CrashCurX & 7);

		return;
	}

	if (c == '\n') {
		g_CrashCurY++;
		g_CrashCurX = 0;
	}

	if (g_CrashCurY >= MAX_LINES) {
		crash_scroll(g_CrashCurY - MAX_LINES + 1);
		g_CrashCurY = MAX_LINES - 1;
	}

	if (c != '\n') {
		crash_put_char(g_CrashCurX, g_CrashCurY, c);
		g_CrashCurX++;

		if (g_CrashCurX >= 71) {
			g_CrashCurX = 0;
			g_CrashCurY++;
		}
	}
}

void crash_scroll(s32 numlines)
{
	s32 i;
	s32 y;
	s32 x;

	if (g_CrashCharBuffer == NULL) {
		return;
	}

	while (numlines-- > 0) {
		for (y = 0; y < MAX_LINES; y++) {
			for (x = 0; x < 71; x++) {
				g_CrashCharBuffer[y][x] = g_CrashCharBuffer[y + 1][x];
			}
		}
	}
}

/**
 * Render a character to the crash buffer.
 */
void crash_render_char(s32 x, s32 y, char c)
{
	s32 i;
	s32 j;
	s32 width;
	u16 *fbpos;
	bool hires;
	s32 tmp;
	u32 a2;

	width = vi_get_width();

	if (c == '\0') {
		c = ' ';
	}

	if (c < ' ' || c > '~') {
		return;
	}

	hires = (width == 640);

	if (hires) {
		fbpos = g_CrashFrameBuffer + x * 2 + y * width;
	} else {
		fbpos = g_CrashFrameBuffer + x + y * width;
	}

#ifdef DEBUG
	a2 = var8005f138nb[c - ' '];
#else
	a2 = 0;
#endif

	for (i = 0; i < CHAR_H; i++) {
		for (j = 0; j < CHAR_W; j++) {
			u32 gray = a2 & 0x80000000;

			if (gray) {
#if VERSION == VERSION_NTSC_BETA
				fbpos[0] = GPACK_RGBA5551(0xff, 0xff, 0xff, 1);
#else
				fbpos[0] = GPACK_RGBA5551(0x78, 0x78, 0x78, 1);
#endif
			} else {
				fbpos[0] = GPACK_RGBA5551(0, 0, 0, 1);
			}

			if (hires) {
				if (gray) {
#if VERSION == VERSION_NTSC_BETA
					fbpos[1] = GPACK_RGBA5551(0xff, 0xff, 0xff, 1);
#else
					fbpos[1] = GPACK_RGBA5551(0x78, 0x78, 0x78, 1);
#endif
				} else {
					fbpos[1] = GPACK_RGBA5551(0, 0, 0, 1);
				}
			}

			fbpos++;

			if (hires) {
				fbpos++;
			}

			a2 *= 2;
		}

		if (hires) {
			fbpos += width;
			fbpos -= 8;
		} else {
			fbpos += width;
			fbpos -= 4;
		}
	}
}

void crash_reset(void)
{
#ifdef DEBUG
	g_CrashCharBuffer = var80097178nb;
#else
	g_CrashCharBuffer = NULL;
#endif

	if (g_CrashCharBuffer) {
		// Unreachable
		s32 x;
		s32 y;

		for (y = 0; y < MAX_LINES + 1; y++) {
			for (x = 0; x < 71; x++) {
				g_CrashCharBuffer[y][x] = '\0';
			}
		}
	}
}

void crash_render_frame(u16 *fb)
{
	s32 width;
	s32 height;
	s32 x;
	s32 y;

	g_CrashFrameBuffer = (u16 *) PHYS_TO_K1(fb);

	width = (vi_get_width() - 13) / CHAR_W;
	height = (vi_get_height() - 10) / CHAR_H - 1;

	if (g_CrashCharBuffer != NULL) {
		for (y = 0; y < height && y < MAX_LINES; y++) {
			for (x = 0; x < width - 5 && x < 71; x++) {
				crash_render_char(20 + x * CHAR_W, CHAR_H + y * CHAR_H, g_CrashCharBuffer[y][x]);
			}
		}
	}
}
