#ifndef FIXED_ARENA
#define FIXED_ARENA

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

#define KiB(n) ((u64)(n) << 10)
#define MiB(n) ((u64)(n) << 20)
#define GiB(n) ((u64)(n) << 30)

#define MIN(a, b) (((a) < (b)) ? (a) : (b))
#define MAX(a, b) (((a) > (b)) ? (a) : (b))
#define ALIGN_UP_POW2(n, p) (((u64)(n) + ((u64)(p) - 1)) & (~((u64)(p) - 1)))

#define ARENA_BASE_POS (sizeof(fixed_arena))
#define ARENA_ALIGN (sizeof(void*))

typedef struct {
	u64 capacity;
	u64 position;
} fixed_arena;

fixed_arena *arena_create(u64 capacity);
void arena_destroy(fixed_arena *arena);
void *arena_push(fixed_arena *arena, u64 size, bool non_zero);
void arena_pop(fixed_arena *arena, u64 size);
void arena_pop_to(fixed_arena *arena, u64 position);
void arena_clear(fixed_arena *arena);

#define PUSH_STRUCT(arena, type) (type*)arena_push((arena), sizeof(type), false) // will zero the memory
#define PUSH_STRUCT_NZ(arena, type) (type*)arena_push((arena), sizeof(type), true) // will leave the memory empty
#define PUSH_ARRAY(arena, type, n) (type*)arena_push((arena), sizeof(type) * (n), false)
#define PUSH_ARRAY_NZ(arena, type, n) (type*)arena_push((arena), sizeof(type) * (n), true)

#if defined(FIXED_ARENA_IMPLEMENTATION)

fixed_arena *arena_create(u64 capacity)
{
	u64 total_allocation = capacity + ARENA_BASE_POS;
	fixed_arena *arena = (fixed_arena*)malloc(total_allocation);
	if (arena == NULL)
		return NULL;
	arena->capacity = total_allocation;
	arena->position = ARENA_BASE_POS;
	return arena;
}

void arena_destroy(fixed_arena *arena)
{
	free(arena);
}

void *arena_push(fixed_arena *arena, u64 size, bool non_zero)
{
	u64 pos_aligned = ALIGN_UP_POW2(arena->position, ARENA_ALIGN);
	u64 new_pos = pos_aligned + size;
	
	if (new_pos > arena->capacity) {
		return NULL;
	}
	
	arena->position = new_pos;
	
	u8 *out = (u8*)arena + pos_aligned;
	
	if (!non_zero) {
		memset(out, 0, size);
	}
	
	return out;
}

void arena_pop(fixed_arena *arena, u64 size)
{
	size = MIN(size, arena->position - ARENA_BASE_POS);
	arena->position -= size;
}

void arena_pop_to(fixed_arena *arena, u64 position)
{
	u64 size = position < arena->position ? arena->position - position : 0;
	arena_pop(arena, size);
}


void arena_clear(fixed_arena *arena)
{
	arena_pop_to(arena, ARENA_BASE_POS);
}

#endif // defined(FIXED_ARENA_IMPLEMENTATION)
#endif // FIXED_ARENA
