#ifndef VIRTUAL_ARENA
#define VIRTUAL_ARENA

#include <stdio.h>
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

#define ARENA_BASE_POS (sizeof(virtual_arena))
#define ARENA_ALIGN (sizeof(void*))

typedef struct {
	u64 reserve_size;
	u64 commit_size;
	u64 position;
	u64 commit_position;
} virtual_arena;

virtual_arena *arena_create(u64 reserve_size, u64 commit_size);
void arena_destroy(virtual_arena *arena);
void *arena_push(virtual_arena *arena, u64 size, bool non_zero);
void arena_pop(virtual_arena *arena, u64 size);
void arena_pop_to(virtual_arena *arena, u64 pos);
void arena_clear(virtual_arena *arena);

#define PUSH_STRUCT(arena, type) (type*)arena_push((arena), sizeof(type), false) // will zero the memory
#define PUSH_STRUCT_NZ(arena, type) (type*)arena_push((arena), sizeof(type), true) // will leave the memory empty
#define PUSH_ARRAY(arena, type, n) (type*)arena_push((arena), sizeof(type) * (n), false)
#define PUSH_ARRAY_NZ(arena, type, n) (type*)arena_push((arena), sizeof(type) * (n), true)

u32 get_pagesize(void);

void *mem_reserve(u64 size);
bool mem_commit(void *ptr, u64 size);
bool mem_decommit(void *ptr, u64 size);
bool mem_release(void *ptr, u64 size);

#if defined(VIRTUAL_ARENA_IMPLEMENTATION)

virtual_arena *arena_create(u64 reserve_size, u64 commit_size)
{
	u32 pagesize = get_pagesize();
	reserve_size = ALIGN_UP_POW2(reserve_size, pagesize);
	commit_size = ALIGN_UP_POW2(commit_size, pagesize);
	
	virtual_arena *arena = mem_reserve(reserve_size);
	
	if (!mem_commit(arena, commit_size)) 
		return NULL;
	
	arena->reserve_size = reserve_size;
	arena->commit_size = commit_size;
	arena->position = ARENA_BASE_POS;
	arena->commit_position = commit_size;
	
	return arena;
}

void arena_destroy(virtual_arena *arena)
{
	mem_release(arena, arena->reserve_size);
}

void *arena_push(virtual_arena *arena, u64 size, bool non_zero)
{
	u64 position_aligned = ALIGN_UP_POW2(arena->position, ARENA_ALIGN);
	u64 new_position = position_aligned + size;
	
	if (new_position > arena->reserve_size)
		return NULL;
	
	if (new_position > arena->commit_position) {
		u64 new_commit_position = new_position;
		new_commit_position += arena->commit_size - 1;
		new_commit_position -= new_commit_position % arena->commit_size;
		new_commit_position = MIN(new_commit_position, arena->reserve_size);
		
		u8 *mem = (u8*)arena + arena->commit_position;
		u64 commit_size = new_commit_position - arena->commit_position;
		
		if (!mem_commit(mem, commit_size))
			return NULL;
		
		arena->commit_position = new_commit_position;
	}
	
	arena->position = new_position;
	
	u8 *out = (u8*)arena + position_aligned;
	
	if (!non_zero)
		memset(out, 0, size);
	
	return out;
}

void arena_pop(virtual_arena *arena, u64 size)
{
	size = MIN(size, arena->position - ARENA_BASE_POS);
	arena->position -= size;
}

void arena_pop_to(virtual_arena *arena, u64 position)
{
	u64 size = position < arena->position ? arena->position - position : 0;
	arena_pop(arena, size);
}

void arena_clear(virtual_arena *arena)
{
	arena_pop_to(arena, ARENA_BASE_POS);
}

#if defined(_WIN32)

#include <windows.h>

u32 plat_get_pagesize(void)
{
	SYSTEM_INFO sysinfo = { 0 };
	GetSystemInfo(&sysinfo);
	return sysinfo.dwPageSize;
}

void *mem_reserve(u64 size)
{
	return VirtualAlloc(NULL, size, MEM_RESERVE, PAGE_READWRITE);
}

bool mem_commit(void *ptr, u64 size)
{
	void *ret = VirtualAlloc(ptr, size, MEM_COMMIT, PAGE_READWRITE);
	return ret != NULL;
}

bool plat_mem_decommit(void *ptr, u64 size)
{
	return VirtualFree(ptr, size, MEM_DECOMMIT);
}

bool plat_mem_release(void *ptr, u64 size)
{
	return VirtualFree(ptr, size, MEM_RELEASE);
}

#elif defined(__linux__)

#include <unistd.h>
#include <sys/mman.h>

u32 get_pagesize(void)
{
	return (u32)sysconf(_SC_PAGESIZE);
}

void *mem_reserve(u64 size)
{
	void *out = mmap(NULL, size, PROT_NONE, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
	if (out == MAP_FAILED)
		return NULL;
	
	return out;
}

bool mem_commit(void *ptr, u64 size)
{
	s32 ret = mprotect(ptr, size, PROT_READ | PROT_WRITE);
	return ret == 0;
}

bool mem_decommit(void *ptr, u64 size)
{
	s32 ret = mprotect(ptr, size, PROT_NONE);
	if (ret != 0) return false;
	ret = madvise(ptr, size, MADV_DONTNEED);
	return ret == 0;
}

bool mem_release(void *ptr, u64 size)
{
	s32 ret = munmap(ptr, size);
	return ret == 0;
}

#endif // Windows or Linux
#endif // defined(VIRTUAL_ARENA_IMPLEMENTATION)
#endif // VIRTUAL_ARENA
