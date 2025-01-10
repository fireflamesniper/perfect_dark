#include <ultra64.h>
#include "lib/tlb.h"
#include "lib/boot.h"
#include "lib/reset.h"
#include "lib/segments.h"
#include "constants.h"
#include "game/menugfx.h"
#include "bss.h"
#include "lib/args.h"
#include "lib/rzip.h"
#include "lib/crash.h"
#include "lib/main.h"
#include "lib/snd.h"
#include "lib/pimgr.h"
#include "lib/videbug.h"
#include "lib/rmon.h"
#include "lib/lib_48150.h"
#include "data.h"
#include "types.h"

OSThread g_RmonThread;
OSThread g_IdleThread;
OSThread g_MainThread;
OSThread g_SchedThread;
OSMesgQueue g_MainMesgQueue;
OSMesg g_MainMesgBuf[32];
OSMesgQueue *g_SchedCmdQ;
u32 var8008dbcc;
OSSched g_Sched;
OSScClient g_MainSchedClient;
#if VERSION >= VERSION_NTSC_1_0
u32 g_SavedOsMemSize;
#else
u16 *var800902e4;
u16 var800902e8;
#endif

u32 var8005ce00 = 0;
u32 var8005ce04 = 0;
u32 var8005ce08 = 0;
u32 var8005ce0c = 0;
u8 *g_StackLeftAddrs[NUM_THREADS] = {0};
u8 *g_StackRightAddrs[NUM_THREADS] = {0};
u8 *g_StackAllocatedPos = (u8 *) K0BASE + 4 * 1024 * 1024;
u32 var8005ce4c = 0x00000002;
u32 var8005ce50 = 0x10000000;

extern u8 *_libSegmentStart;
extern u8 *_datazipSegmentRomStart;
extern u8 *_datazipSegmentRomEnd;
extern u8 *_dataSegmentStart;
extern u8 *_inflateSegmentStart;
extern u8 *_inflateSegmentRomStart;
extern u8 *_inflateSegmentRomEnd;

#if VERSION >= VERSION_NTSC_1_0
s32 osGetMemSize(void)
{
	return g_SavedOsMemSize;
}
#endif

u32 __osGetFpcCsr(void);
u32 __osSetFpcCsr(u32 arg0);

void boot_create_threads(void *arg);

/**
 * Prepares the inflate, .data and lib segments, then creates and starts the
 * main thread.
 *
 * This function relies on the fact that the N64 BIOS loads the first 1MB of ROM
 * data into memory at power on. The inflate, .data and lib segments are already
 * in RAM thanks to this but need to be relocated, and .data and lib need to be
 * unzipped too.
 */
void boot(void)
{
	s32 datacomplen;
	s32 inflatelen;
	u8 *dataziprom;
	u8 *datazipram;
	u32 *libram;
	u32 *libzipram;
	u32 dataram;
	u32 copylen;
	s32 i;
	s32 numlibwords;
	u32 flags;

#if VERSION >= VERSION_NTSC_1_0
	// In NTSC beta, libultra's osGetMemSize is used several times during boot.
	// This function writes to the expansion area but this shouldn't be a problem...
	// In NTSC 1.0 and later, libultra's osGetMemSize is not linked and is replaced
	// with one which returns osMemSize. But it appears as though osMemSize is only
	// reliable on a cold boot, because it's saved and reloaded here on a reset.
	if (osResetType == RESETTYPE_WARM) {
		g_SavedOsMemSize = *(u32 *) STACK_START;
	} else {
		*(u32 *) STACK_START = g_SavedOsMemSize = osMemSize;
	}
#endif

	// Copy compressed .data and inflate segments
	// .data is copied from ROM to 0x701eb000 - 0x70200000
	// inflate is copied from ROM to 0x70200000 - 0x702013f0
	datacomplen = (romptr_t) &_datazipSegmentRomEnd - (romptr_t) &_datazipSegmentRomStart;
#if VERSION >= VERSION_NTSC_1_0
	if (1);
#endif
	inflatelen = (romptr_t) &_inflateSegmentRomEnd - (romptr_t) &_inflateSegmentRomStart;
	copylen = datacomplen + inflatelen;
	libram = (u32 *) ((romptr_t) &_libSegmentStart + 0x2000);
	libzipram = (u32 *) 0x70280000;
	dataziprom = (u8 *) ((romptr_t) &_datazipSegmentRomStart | 0x70000000);
	datazipram = (u8 *) (0x70200000 - datacomplen);
	dataram = (romptr_t) &_dataSegmentStart;

	for (i = copylen - 1; i >= 0; i--) {
		datazipram[i] = dataziprom[i];
	}

	// Copy the compressed part of lib to 0x70280000.
	// lib is compressed from 2KB onwards.
	// It's assumed that lib's compressed length is less than 75000 words.
	// This is fine, as it's about half that.
	numlibwords = 75000;

	for (i = 0; i < numlibwords; i++) {
		libzipram[i] = libram[i];
	}

	// Inflate compressed part of lib
	seg_inflate((void *) libzipram, (void *) libram, (void *) 0x80300000);

	// Inflate .data
	seg_inflate((void *) datazipram, (void *) dataram, (void *) 0x80300000);

#if PIRACYCHECKS
	if (IO_READ(0xa00002e8) != 0xc86e2000) {
		while (1);
	}
#endif

	vm_unmap_range(1, NTLBENTRIES);

	// Clear the stack allocation pointers
	for (i = 0; i < ARRAYCOUNT(g_StackLeftAddrs); i++) {
		g_StackLeftAddrs[i] = NULL;
		g_StackRightAddrs[i] = NULL;
	}

	osInitialize();

	// Write all data memory cache into physical memory
	osWritebackDCacheAll();

	// Invalidate all instruction cache
	osInvalICache((void *) K0BASE, ICACHE_SIZE);

	// Configure the floating point unit
	flags = __osGetFpcCsr();

	flags |= FPCSR_EI; // enable inexact operation
	flags |= FPCSR_EO; // enable overflow
	flags |= FPCSR_EZ; // enable division by zero
	flags |= FPCSR_EV; // enable invalid operation

	__osSetFpcCsr(flags);

#if VERSION < VERSION_NTSC_1_0
	var800902e4 = (void *) 0xbc000c02;
	var800902e8 = 0x4040;
	*(s16 *) 0xbc000c02 = 0x4040;
#endif

	// Create and start the main thread
	osCreateThread(&g_MainThread, THREAD_MAIN, boot_create_threads, NULL, boot_allocate_stack(THREAD_MAIN, STACKSIZE_MAIN), THREADPRI_MAIN);
	osStartThread(&g_MainThread);
}

/**
 * Allocate stack space for the given thread ID.
 *
 * Each allocation is aligned to 16 bytes.
 *
 * Allocations start from the end of onboard memory (0x80400000) and are
 * allocated right to left.
 *
 * The returned address leaves 8 bytes free on the right side of the stack,
 * presumably for identification purposes. So the actual stack space available
 * can be 8 bytes above or below what was requested.
 *
 * The stack is initialised with the thread's ID. This makes it easier to
 * identify in memory and detect when a stack overflow has occurred.
 */
void *boot_allocate_stack(s32 threadid, s32 size)
{
	u8 *ptr8;
	u32 *ptr32;
	s32 i;
	s32 j;
	u8 *tmp;

	g_StackRightAddrs[threadid] = g_StackAllocatedPos;
	size = (size + 0xf) & 0xfffffff0;

	g_StackAllocatedPos -= size;
	g_StackLeftAddrs[threadid] = (tmp = g_StackAllocatedPos);

	ptr8 = g_StackAllocatedPos;

	for (i = 0; i < size; i++) {
		ptr8[i] = ((0xf - (threadid & 0xf)) << 4) | (threadid & 0xf);
	}

#if VERSION < VERSION_NTSC_1_0
	// Mark the first 8 words specially
	ptr32 = (u32 *)g_StackLeftAddrs[threadid];

	for (j = 0; j < 8; j++) {
		*ptr32 = 0xdeadbabe;
		ptr32++;
	}
#endif

	return g_StackAllocatedPos + size - 8;
}

#if VERSION < VERSION_NTSC_1_0
u8 *boot_get_stack_pos(void)
{
	return g_StackAllocatedPos;
}
#endif

#if VERSION < VERSION_NTSC_1_0
void func00001978(void)
{
	var8005ce4c = 1;
	var8005ce50 = 0x10000000;
}
#endif

void idleproc(void *data)
{
	while (true);
}

void boot_create_idle_thread(void)
{
	osCreateThread(&g_IdleThread, THREAD_IDLE, idleproc, NULL, boot_allocate_stack(THREAD_IDLE, STACKSIZE_IDLE), THREADPRI_IDLE);
	osStartThread(&g_IdleThread);
}

void boot_create_rmon_thread(void)
{
	osCreateThread(&g_RmonThread, THREAD_RMON, rmonproc, NULL, boot_allocate_stack(THREAD_RMON, STACKSIZE_RMON), THREADPRI_RMON);
	osStartThread(&g_RmonThread);
}

void boot_create_sched_thread(void)
{
	osCreateMesgQueue(&g_MainMesgQueue, g_MainMesgBuf, ARRAYCOUNT(g_MainMesgBuf));

	if (osTvType == OS_TV_MPAL) {
		osCreateScheduler(&g_Sched, &g_SchedThread, OS_VI_MPAL_LAN1, 1);
	} else {
		osCreateScheduler(&g_Sched, &g_SchedThread, OS_VI_NTSC_LAN1, 1);
	}

	osScAddClient(&g_Sched, &g_MainSchedClient, &g_MainMesgQueue, false);
	g_SchedCmdQ = osScGetCmdQ(&g_Sched);
}

void boot_create_threads(void *arg)
{
	boot_create_idle_thread();
	videbug_create();
	pimgr_create();
	boot_create_rmon_thread();

	if (args_parse_debug_args()) {
		osStopThread(NULL);
	}

	osSetThreadPri(0, THREADPRI_MAIN);
	boot_create_sched_thread();
	main_proc();
}

#if VERSION < VERSION_NTSC_1_0
void boot_count_unused_stack(void)
{
	s32 threadid;

	for (threadid = 0; threadid < NUM_THREADS; threadid++) {
		u8 *left = g_StackLeftAddrs[threadid];
		u8 *right = g_StackRightAddrs[threadid];

		if (left != NULL) {
			u32 byte = ((0xf - (threadid & 0xf)) << 4) | (threadid & 0xf);

			left += 0x20;

			while (*left == byte && left < right) {
				left++;
			}
		}
	}
}

void boot_check_stack_overflow(void)
{
	s32 threadid;

	for (threadid = 0; threadid < NUM_THREADS; threadid++) {
		if (g_StackLeftAddrs[threadid] != NULL) {
			u32 *ptr = (u32 *) g_StackLeftAddrs[threadid];
			s32 i;

			for (i = 0; i < 8; i++) {
				if (*ptr != 0xdeadbabe) {
					char message[128];

					boot_count_unused_stack();

					sprintf(message, "Stack overflow thread %d", threadid);
					crash_set_message(message);
					CRASH();
				}

				ptr++;
			}
		}
	}
}
#endif
