#ifndef SLCLI_ARENA_LAYOUT_H
#define SLCLI_ARENA_LAYOUT_H
#include <stdint.h>
/* SAME native allocator header, shared by the fixed prebirth initializer and
 * ordinary arena. No header supplies source/native ownership admission. */
struct slcli_arena_block_header { uint64_t bytes,requested,next,state; };
_Static_assert(sizeof(struct slcli_arena_block_header)==32,"Original arena header stride");
enum { SLCLI_ARENA_BLOCK_FREE=1,SLCLI_ARENA_BLOCK_LIVE=2 };
#endif
