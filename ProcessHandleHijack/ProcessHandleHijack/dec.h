/*#pragma once

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>

#include <malloc.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


typedef uint64_t(*decrypt_func)(uint64_t);

struct tsl {
	union {
		decrypt_func func;
		uint8_t *arr;
	};
};

int tsl_init(struct tsl *tsl);
void tsl_finit(struct tsl *tsl);
uint64_t tsl_decrypt_world(struct tsl *tsl, uint64_t world);
uint64_t tsl_decrypt_actor(struct tsl *tsl, uint64_t actor);
uint64_t tsl_decrypt_prop(struct tsl *tsl, uint64_t prop);*/