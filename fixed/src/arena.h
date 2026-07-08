#ifndef MARENA
#define MARENA

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <stdbool.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;
typedef s8 b8;
typedef s32 b32;

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ALIGN_UP_POW2(n, p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))

#define ARENA_BASE_POS (sizeof(marena))
#define ARENA_ALIGN (sizeof(void*))

typedef struct {
	u64 capacity;
	u64 pos;
} marena;

marena* arena_create(u64 capacity);
void arena_destroy(marena* arena);
void* arena_push(marena* arena, u64 size, b32 non_zero);
void arena_pop(marena* arena, u64 size);
void arena_pop_to(marena* arena, u64 pos);
void arena_clear(marena* arena);

#define PUSH_STRUCT(arena, type) (type*)arena_push((arena), sizeof(type), false) // will zero the memory
#define PUSH_STRUCT_NZ(arena, type) (type*)arena_push((arena), sizeof(type), true) // will leave the memory empty
#define PUSH_ARRAY(arena, type, n) (type*)arena_push((arena), sizeof(type) * (n), false)
#define PUSH_ARRAY_NZ(arena, type, n) (type*)arena_push((arena), sizeof(type) * (n), true)

#if defined(MARENA_IMPLEMENTATION)

marena* arena_create(u64 capacity)
{
	marena* arena = (marena*)malloc(capacity);
	arena->capacity = capacity;
	arena->pos = ARENA_BASE_POS;
	return arena;
}

void arena_destroy(marena* arena)
{
	free(arena);
}

void* arena_push(marena* arena, u64 size, b32 non_zero)
{
	u64 pos_aligned = ALIGN_UP_POW2(arena->pos, ARENA_ALIGN);
	u64 new_pos = pos_aligned + size;
	
	if (new_pos > arena->capacity) {
		return NULL;
	}
	
	arena->pos = new_pos;
	
	u8* out = (u8*)arena + pos_aligned;
	
	if (!non_zero) {
		memset(out, 0, size);
	}
	
	return out;
}

void arena_pop(marena* arena, u64 size)
{
	size = MIN(size, arena->pos - ARENA_BASE_POS);
	arena->pos -= size;
}

void arena_pop_to(marena* arena, u64 pos)
{
	u64 size = pos < arena->pos ? arena->pos - pos : 0;
	arena_pop(arena, size);
}


void arena_clear(marena* arena)
{
	arena_pop_to(arena, ARENA_BASE_POS);
}

#endif // defined(MARENA_IMPLEMENTATION)
#endif // MARENA
