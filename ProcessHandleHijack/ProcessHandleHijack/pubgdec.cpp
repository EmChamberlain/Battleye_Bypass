/*
pubgdec 1.01
*/
// all code from https://github.com/nmodnation/pubgdec

// https://github.com/calccrypto/uint128_t

#include "pubgdec.hpp"

#include <iterator>

#if !defined(__GNUC__) && defined(__cplusplus)
#include "uint128_t.h"
#endif

#ifdef _MSC_VER
#pragma warning(disable: 4244)
#pragma warning(disable: 4293)
#endif

/*
hexrays stuff
*/

typedef unsigned int uint;
typedef unsigned char uchar;
typedef unsigned short ushort;
typedef unsigned long ulong;

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef int int32;
typedef unsigned int uint32;
typedef long long int64;
typedef unsigned long long uint64;

#define _BYTE uint8
#define _WORD uint16
#define _DWORD uint32
#define _QWORD uint64

#define LAST_IND(x, part_type) (sizeof(x) / sizeof(part_type) - 1)
#define HIGH_IND(x, part_type) LAST_IND(x, part_type)
#define LOW_IND(x, part_type) 0

#define BYTEn(x, n)   (*((_BYTE*)&(x) + n))
#define WORDn(x, n)   (*((_WORD*)&(x) + n))
#define DWORDn(x, n)  (*((_DWORD*)&(x) + n))

#define LOBYTE(x) BYTEn(x, LOW_IND(x, _BYTE))
#define LOWORD(x) WORDn(x, LOW_IND(x, _WORD))
#define LODWORD(x) DWORDn(x, LOW_IND(x, _DWORD))
#define HIBYTE(x) BYTEn(x, HIGH_IND(x, _BYTE))
#define HIWORD(x) WORDn(x, HIGH_IND(x, _WORD))
#define HIDWORD(x) DWORDn(x, HIGH_IND(x, _DWORD))
#define BYTE1(x) BYTEn(x, 1)
#define BYTE2(x) BYTEn(x, 2)
#define BYTE3(x) BYTEn(x, 3)
#define BYTE4(x) BYTEn(x, 4)
#define BYTE5(x) BYTEn(x, 5)
#define BYTE6(x) BYTEn(x, 6)
#define BYTE7(x) BYTEn(x, 7)
#define BYTE8(x) BYTEn(x, 8)
#define BYTE9(x) BYTEn(x, 9)
#define BYTE10(x) BYTEn(x, 10)
#define BYTE11(x) BYTEn(x, 11)
#define BYTE12(x) BYTEn(x, 12)
#define BYTE13(x) BYTEn(x, 13)
#define BYTE14(x) BYTEn(x, 14)
#define BYTE15(x) BYTEn(x, 15)
#define WORD1(x) WORDn(x, 1)
#define WORD2(x) WORDn(x, 2)
#define WORD3(x) WORDn(x, 3)
#define WORD4(x) WORDn(x, 4)
#define WORD5(x) WORDn(x, 5)
#define WORD6(x) WORDn(x, 6)
#define WORD7(x) WORDn(x, 7)
#define DWORD1(x) DWORDn(x, 1)

/*
rpm
*/


/*static int dummy_read(dummy *dummy, void *game_addr, void *user_addr, uint64 size) {
	dummy_request request;
	uint written;
	request.msg = DUMMY_READ;
	request.user_pid = dummy->user_pid;
	request.user_addr = (uint64)user_addr;
	request.game_pid = dummy->game_pid;
	request.game_addr = (uint64)game_addr;
	request.size = size;
	return WriteFile(dummy->handle, &request, sizeof(dummy_request), &written, (void *)0);
}*/

static uint8 dummy_read8(KReader *dummy, int64 game_addr) {
	return dummy->readType8(game_addr, PROTO_NORMAL_READ);
}

/*static uint16 dummy_read16(dummy *dummy, void *game_addr) {
	uint16 ret = 0;
	dummy_read(dummy, game_addr, &ret, 2);
	return ret;
}*/

static uint32 dummy_read32(KReader *dummy, int64 game_addr) {
	return dummy->readType32(game_addr, PROTO_NORMAL_READ);
}

static uint64 dummy_read64(KReader *dummy, int64 game_addr) {
	return dummy->readType64(game_addr, PROTO_NORMAL_READ);
}

static int128 dummy_read128(KReader *dummy, int64 game_addr) {
	return dummy->readType128(game_addr, PROTO_NORMAL_READ);
}

#define GET_ADDR(addr) (dummy->getPUBase() + (addr))

//#define READ(addr, dest, size) dummy_read(dummy, (void *)(addr), dest, size)
#define READ8(addr) dummy_read8(dummy, addr)
//#define READ16(addr) dummy_read16(dummy, (void *)(addr))
#define READ32(addr) dummy_read32(dummy, addr)
#define READ64(addr) dummy_read64(dummy, addr)
#define READ128(addr) dummy_read128(dummy, addr)


/*
dec
*/

/*typedef struct int128 {
	int64 low;
	int64 high;
} int128;*/

#ifdef __GNUC__
typedef unsigned __int128 uint128;
#else
#ifdef __cplusplus
typedef uint128_t uint128;
#else
#error uint128
#endif
#endif

#define TABLE_SIZE 0x100

typedef struct decrypt_struct {
	uint8 table[TABLE_SIZE];
	uint32 xor1;
	uint32 xor2;
} decrypt_struct;

static decrypt_struct g_decrypt;

#define READ_TABLE(index) g_decrypt.table[index]

// export
void pubgdec::decinit(KReader *dummy) {
	//READ(GET_ADDR(0x3AFF120), g_decrypt.table, TABLE_SIZE);
	int64 base = GET_ADDR(0x3AFF120);
	for(int i = 0; i < TABLE_SIZE; i++)
	{
		g_decrypt.table[i] = dummy_read8(dummy, base + (i * sizeof(uint8)));
	}
	g_decrypt.xor1 = READ32(700 + GET_ADDR(0x3DEB690));
	g_decrypt.xor2 = READ32(924 + GET_ADDR(0x3DEB690));
}

static uint64 dec1(KReader *dummy, int128 rcx22) {
	uint64 v1;
	uint v4;
	int16 v5;
	uint v6;
	uint64 v7;
	uint64 v8;
	uint64 v9;
	uint8 v11;
	uint16 v13;
	uint v14;
	char v16;
	int v19;
	v1 = rcx22.high;
	v19 = DWORD1(rcx22);
	v4 = rcx22.low;
	v5 = WORD4(rcx22);
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
		v11 = v6 + v9++;
		LODWORD(v7) = ((READ_TABLE(BYTE2(v7)) | (((READ_TABLE((uint8)v7) << 8) | READ_TABLE(BYTE1(v7))) << 8)) << 8) | READ_TABLE((v7 >> 24));
		v6 = READ_TABLE((uint64)v6 >> 24) | ((READ_TABLE(BYTE2(v6)) | (((READ_TABLE((uint8)v6) << 8) | READ_TABLE(BYTE1(v6))) << 8)) << 8);
	} while (v9 < 3);
	if ((v6 ^ (uint)v7) != v19)
	{
		return 0;
	}
	v13 = v5 ^ ~(_WORD)v4 ^ 0xD25;
	do
	{
		v14 = v13;
		v16 = v8++ + 2;
		v13 = READ_TABLE(READ_TABLE((v14 ^ 0x4400u) >> 8)) | (uint16)(READ_TABLE(READ_TABLE((uint8)(v14 ^ 0x55))) << 8);
	} while (v8 < 5);
	return ~(
		READ32((uint64)(4) * (uint8)(v13 ^ 0xBC) + GET_ADDR(0x3DEBE90)) ^
		READ32(4 * ((uint64)(v13 ^ 0xD7AF5ABC) >> 24) + GET_ADDR(0x3DEB290)) ^
		(READ32((uint64)(4) * (uint8)(HIBYTE(v13) ^ 0x5A) + GET_ADDR(0x3DEBA90)) ^ g_decrypt.xor1)) % 0x2B;
}

static uint64 dec2(KReader *dummy, int128 rcx23) {
	uint64 v1;
	uint8 v3;
	uint v4;
	int16 v5;
	uint v6;
	uint64 v7;
	uint64 v8;
	uint64 v9;
	uint8 v11;
	uint16 v13;
	uint v14;
	char v16;
	int v19;
	v1 = rcx23.high;
	v19 = DWORD1(rcx23);
	v4 = rcx23.low;
	v5 = WORD4(rcx23);
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
		v11 = v6 + v9++;
		LODWORD(v7) = ((READ_TABLE(BYTE2(v7)) | (((READ_TABLE((uint8)v7) << 8) | READ_TABLE(BYTE1(v7))) << 8)) << 8) | READ_TABLE(v7 >> 24);
		v6 = READ_TABLE((uint64)v6 >> 24) | ((READ_TABLE(BYTE2(v6)) | (((READ_TABLE((uint8)v6) << 8) | READ_TABLE(BYTE1(v6))) << 8)) << 8);
	} while (v9 < 3);
	if ((v6 ^ (uint)v7) != v19)
	{
		return 0;
	}
	v13 = v5 ^ ~(_WORD)v4 ^ 0xD25;
	do
	{
		v14 = v13;
		v16 = v8++ + 2;
		v13 = READ_TABLE(READ_TABLE((v14 ^ 0x4400u) >> 8)) | (uint16)(READ_TABLE(READ_TABLE((uint8)(v14 ^ 0x55))) << 8);
	} while (v8 < 5);
	return ~(
		READ32((uint64)(4) * (uint8)(v13 ^ 0xC) + GET_ADDR(0x3DEBE90)) ^
		READ32(4 * ((uint64)(v13 ^ 0x5CE7E30Cu) >> 24) + GET_ADDR(0x3DEB290)) ^
		(READ32((uint64)(4) * (uint8)(HIBYTE(v13) ^ 0xE3) + GET_ADDR(0x3DEBA90)) ^ g_decrypt.xor2)) % 0x2B;
}

uint64 pubgdec::decptr(KReader *dummy, int64 x) {
	int128 rcx22 = READ128(x + (22 * 16));
	int128 rcx23 = READ128(x + (23 * 16));
	uint64 decrypted1 = dec1(dummy, rcx22);
	uint64 xor1 = READ64(x + (decrypted1*8));
	uint64 xor2 = dec2(dummy, rcx23);
	return xor1 ^ xor2;
}
