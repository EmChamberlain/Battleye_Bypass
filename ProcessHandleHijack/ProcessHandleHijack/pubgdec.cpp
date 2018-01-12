
// all code from https://github.com/nmodnation/pubgdec

#include <stdarg.h>
#include <stdio.h>

#include "pubgdec.hpp"

// https://github.com/calccrypto/uint128_t
#ifdef __cplusplus
#include "uint128_t.h"
#endif

#ifdef _MSC_VER

#pragma warning(disable: 4244)
#pragma warning(disable: 4293)

#endif

#ifdef __cplusplus
#define EXTERN extern "C"
#else
#define EXTERN
#endif

/*
InterlockedCompareExchange128
*/

#ifdef __GNUC__

static unsigned char _InterlockedCompareExchange128(int64 volatile *Destination, int64 ExchangeHigh, int64 ExchangeLow, int64* ComparandResult) {
	unsigned char Equal;
	__asm__ __volatile__
	(
		"lock cmpxchg16b %1\n\t"
		"setz %0"
		: "=q" (Equal), "+m" (*Destination), "+d" (ComparandResult[1]), "+a" (ComparandResult[0])
		: "c" (ExchangeHigh), "b" (ExchangeLow)
		: "cc"
	);
	return Equal;
}

#else

EXTERN unsigned char __cdecl _InterlockedCompareExchange128(int64 volatile *Destination, int64 ExchangeHigh, int64 ExchangeLow, int64 *ComparandResult);

#endif
/*
rpm
*/
/*
#define DUMMY_PID 1
#define DUMMY_BASE_ADDR 2
#define DUMMY_READ 3
#define DUMMY_WRITE 4

typedef struct dummy {
	void *handle;
	uint64 user_pid;
	uint64 game_pid;
	uint64 game_base_addr;
} dummy;

typedef struct dummy_request {
	uint64 msg;
	uint64 user_pid;
	uint64 user_addr;
	uint64 game_pid;
	uint64 game_addr;
	uint64 size;
} dummy_request;

EXTERN int __stdcall WriteFile(void *, const void *, uint, uint *, void *);

static int dummy_read(dummy *dummy, void *game_addr, void *user_addr, uint64 size) {
	dummy_request request;
	uint written;
	request.msg = DUMMY_READ;
	request.user_pid = dummy->user_pid;
	request.user_addr = (uint64)user_addr;
	request.game_pid = dummy->game_pid;
	request.game_addr = (uint64)game_addr;
	request.size = size;
	return WriteFile(dummy->handle, &request, sizeof(dummy_request), &written, (void *)0);
}
*/
static uint8 dummy_read8(KReader *dummy, int64 game_addr) {
	return dummy->readType8(game_addr, PROTO_NORMAL_READ);
}
/*
static uint16 dummy_read16(dummy *dummy, void *game_addr) {
	uint16 ret = 0;
	dummy_read(dummy, game_addr, &ret, 2);
	return ret;
}
*/
static uint32 dummy_read32(KReader *dummy, int64 game_addr) {
	return dummy->readType32(game_addr, PROTO_NORMAL_READ);
}

static uint64 dummy_read64(KReader *dummy, int64 game_addr) {
	return dummy->readType64(game_addr, PROTO_NORMAL_READ);
}
static int128 dummy_read128(KReader *dummy, int64 game_addr) {
	return dummy->readType128(game_addr, PROTO_NORMAL_READ);
}

//#define READ(addr, dest, size) dummy_read(dummy, (void *)(addr), dest, size)
#define READ8(addr) dummy_read8(dummy, (int64)(addr))
//#define READ16(addr) dummy_read16(dummy, (void *)(addr))
#define READ32(addr) dummy_read32(dummy, (int64)(addr))
#define READ64(addr) dummy_read64(dummy, (int64)(addr))
#define READ128(addr) dummy_read128(dummy, (int64)(addr))

/*
dec
*/

/*typedef struct int128 {
	int64 low;
	int64 high;
} int128;*/

#ifdef __cplusplus
typedef uint128_t uint128;
#else
#ifdef __GNUC__
typedef unsigned __int128 uint128;
#else
#error uint128
#endif
#endif

#define GET_ADDR(addr) (dummy->getPUBase() + (addr))

static uint64 dec1(KReader *dummy, int128 *_RCX22) {
	uint64 v1;
	int128 v2;
	uint8 v3;
	uint v4;
	int16 v5;
	uint v6;
	uint64 v7;
	uint64 v8;
	uint64 v9;
	int64 v10;
	uint8 v11;
	int64 v12;
	uint16 v13;
	uint v14;
	int64 v15;
	char v16;
	int64 v17;
	int v19;
	int64 v20[4];
	int64 v24[4];
	v24[0] = GET_ADDR(0x3AFF120);
	// dummy code: v24[1] = GET_ADDR(0x3AFF120);
	// dummy code: v24[2] = GET_ADDR(0x3AFF120);
	// dummy code: v24[3] = GET_ADDR(0x3AFF120);
	int128 *copy_RCX22 = new int128;
	copy_RCX22->high = _RCX22->high;
	copy_RCX22->low = _RCX22->low;

	do
	{
		v2 = *copy_RCX22;
		v3 = _InterlockedCompareExchange128((int64 *)copy_RCX22, v2.high, v2.low, (int64 *)&v2);
		v1 = v2.high;
		v19 = DWORD1(v2);
		v4 = v2.low;
		v5 = WORD4(v2);
	} while (!v3);
	v20[0] = GET_ADDR(0x3AFF120);
	// dummy code: v20[1] = GET_ADDR(0x3AFF120);
	// dummy code: v20[2] = GET_ADDR(0x3AFF120);
	// dummy code: v20[3] = GET_ADDR(0x3AFF120);
	v6 = 2067041945;
	v7 = ((_DWORD)v1
		+ v4
		+ HIDWORD(v1)
		- 2145172163
		* (uint)((uint64)(((v1 + v4 + (v1 >> 32)) * (uint128)0x469DEF623F2C51u >> 64)
			+ ((uint64)(v1
				+ v4
				+ (v1 >> 32)
				- ((v1 + v4 + (v1 >> 32))
					* (uint128)0x469DEF623F2C51u >> 64)) >> 1)) >> 30)) ^ 0xFEA07C43;
	v8 = 0;
	v9 = 0;
	do
	{
		v10 = v20[0]; // dummy code: v20[READ8((uint8)(v7 + v9) + v20) & 3];
		v11 = v6 + v9++;
		v12 = v20[0]; // dummy code: v20[READ8(v11 + v20) & 3];
		LODWORD(v7) = ((READ8(BYTE2(v7) + v10) | (((READ8((uint8)v7 + v10) << 8) | READ8(BYTE1(v7) + v12)) << 8)) << 8) | READ8((v7 >> 24) + v10);
		v6 = READ8(((uint64)v6 >> 24) + v12) | ((READ8(BYTE2(v6) + v10) | (((READ8((uint8)v6 + v12) << 8) | READ8(BYTE1(v6) + v12)) << 8)) << 8);
	} while (v9 < 3);
	if ((v6 ^ (uint)v7) != v19)
	{
		// error
		delete(copy_RCX22);
		return 0;
	}
	v13 = v5 ^ ~(_WORD)v4 ^ 0xD25;
	do
	{
		v14 = v13;
		v15 = v24[0]; // dummy code: v24[READ8(READ8((uint8)(v8 ^ v13 ^ 0x1C) + v20) + v20) & 3];
		v16 = v8++ + 2;
		v17 = v24[0]; // dummy code: v24[READ8(READ8((uint8)(v16 ^ 0x1C) + v20) + v20) & 3];
		v13 = READ8(READ8(((v14 ^ 0x4400u) >> 8) + v15) + v17) | (uint16)(READ8(READ8((uint8)(v14 ^ 0x55) + v15) + v17) << 8);
	} while (v8 < 5);
	delete(copy_RCX22);
	return ~(READ32((uint64)(4) * (uint8)(v13 ^ 0xBC) + GET_ADDR(0x3DEBE90)) ^
		READ32(4 * ((uint64)(v13 ^ 0xD7AF5ABC) >> 24) + GET_ADDR(0x3DEB290)) ^
		(READ32((uint64)(4) * (uint8)(HIBYTE(v13) ^ 0x5A) + GET_ADDR(0x3DEBA90)) ^ READ32(GET_ADDR(0x3DEB690) + 700))) % 0x2B;
}

static uint64 dec2(KReader *dummy, int128 *_RCX23) {
	uint64 v1;
	int128 v2;
	uint8 v3;
	uint v4;
	int16 v5;
	uint v6;
	uint64 v7;
	uint64 v8;
	uint64 v9;
	int64 v10;
	uint8 v11;
	int64 v12;
	uint16 v13;
	uint v14;
	int64 v15;
	char v16;
	int64 v17;
	int v19;
	int64 v20[4];
	int64 v24[4];
	v24[0] = GET_ADDR(0x3AFF120);
	// dummy code: v24[1] = GET_ADDR(0x3AFF120);
	// dummy code: v24[2] = GET_ADDR(0x3AFF120);
	// dummy code: v24[3] = GET_ADDR(0x3AFF120);


	int128 *copy_RCX23 = new int128;
	copy_RCX23->high = _RCX23->high;
	copy_RCX23->low = _RCX23->low;


	do
	{
		v2 = *copy_RCX23;
		v3 = _InterlockedCompareExchange128((int64 *)copy_RCX23, v2.high, v2.low, (int64 *)&v2);
		v1 = v2.high;
		v19 = DWORD1(v2);
		v4 = v2.low;
		v5 = WORD4(v2);
	} while (!v3);
	v20[0] = GET_ADDR(0x3AFF120);
	// dummy code: v20[1] = GET_ADDR(0x3AFF120);
	// dummy code: v20[2] = GET_ADDR(0x3AFF120);
	// dummy code: v20[3] = GET_ADDR(0x3AFF120);
	v6 = 2067041945;
	v7 = ((_DWORD)v1
		+ v4
		+ HIDWORD(v1)
		- 2145172163
		* (uint)((uint64)(((v1 + v4 + (v1 >> 32)) * (uint128)0x469DEF623F2C51u >> 64)
			+ ((uint64)(v1
				+ v4
				+ (v1 >> 32)
				- ((v1 + v4 + (v1 >> 32))
					* (uint128)0x469DEF623F2C51u >> 64)) >> 1)) >> 30)) ^ 0xFEA07C43;
	v8 = 0;
	v9 = 0;
	do
	{
		v10 = v20[0]; // dummy code: v20[READ8((uint8)(v7 + v9) + v20) & 3];
		v11 = v6 + v9++;
		v12 = v20[0]; // dummy code: v20[READ8(v11 + v20) & 3];
		LODWORD(v7) = ((READ8(BYTE2(v7) + v10) | (((READ8((uint8)v7 + v10) << 8) | READ8(BYTE1(v7) + v12)) << 8)) << 8) | READ8((v7 >> 24) + v10);
		v6 = READ8(((uint64)v6 >> 24) + v12) | ((READ8(BYTE2(v6) + v10) | (((READ8((uint8)v6 + v12) << 8) | READ8(BYTE1(v6) + v12)) << 8)) << 8);
	} while (v9 < 3);
	if ((v6 ^ (uint)v7) != v19)
	{
		// error
		delete(copy_RCX23);
		return 0;
	}
	v13 = v5 ^ ~(_WORD)v4 ^ 0xD25;
	do
	{
		v14 = v13;
		v15 = v24[0]; // dummy code: v24[READ8(READ8((uint8)(v8 ^ v13 ^ 0xAC) + v20) + v20) & 3];
		v16 = v8++ + 2;
		v17 = v24[0]; // dummy code: v24[READ8(READ8((uint8)(v16 ^ 0xAC) + v20) + v20) & 3];
		v13 = READ8(READ8(((v14 ^ 0x4400u) >> 8) + v17) + v15) | (uint16)(READ8(READ8((uint8)(v14 ^ 0x55) + v17) + v15) << 8);
	} while (v8 < 5);
	delete(copy_RCX23);
	return ~(READ32((uint64)(4) * (uint8)(v13 ^ 0xC) + GET_ADDR(0x3DEBE90)) ^
		READ32(4 * ((uint64)(v13 ^ 0x5CE7E30Cu) >> 24) + GET_ADDR(0x3DEB290)) ^
		(READ32((uint64)(4) * (uint8)(HIBYTE(v13) ^ 0xE3) + GET_ADDR(0x3DEBA90)) ^ READ32(GET_ADDR(0x3DEB690) + 924))) % 0x2B;
}

uint64 pubgdec::decptr(KReader *dummy, int64 x) {
	int128 rcx22 = READ128(x + (22*16));
	int128 rcx23 = READ128(x + (23*16));
	uint64 decrypted1 = dec1(dummy, &rcx22);
	uint64 xor1 = READ64(x + (decrypted1*8));
	uint64 xor2 = dec2(dummy, &rcx23);
	return xor1 ^ xor2;
}