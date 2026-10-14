#ifndef SLCLI_ELF_LOADED_LINUX_H
#define SLCLI_ELF_LOADED_LINUX_H
#include <stddef.h>
#include <stdint.h>
struct slcli_elf64_header_original {
 uint64_t file_bytes,entry,program_offset,program_count,section_offset;
 uint16_t type;uint32_t flags;
};
struct slcli_elf64_program_original {
 uint64_t index,file_header_offset,offset,virtual_address,physical_address,file_bytes,memory_bytes,alignment;
 uint32_t type,flags;
};
/* Pure exact window decoding, no target pointer dereference or native grant.
 * Output disjoint from every supplied input; refusal leaves it unchanged. */
int slcli_elf64_header_decode_original(const unsigned char *,size_t,uint64_t,
 const unsigned char *,size_t,struct slcli_elf64_header_original *);
int slcli_elf64_program_decode_original(const struct slcli_elf64_header_original *,
 uint64_t,const unsigned char *,size_t,struct slcli_elf64_program_original *);
#endif