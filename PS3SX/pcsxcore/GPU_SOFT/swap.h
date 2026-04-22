#ifndef __SWAP_H__
#define __SWAP_H__

#include <stdint.h>

#if defined(__PPC__) || defined(__ppc__)
#include <ppu_intrinsics.h>
#include "ppu_asm_intrinsics.h"
#endif

// byteswappings

#define SWAP16(x) (((uint16_t)(x)>>8 & 0xff) | ((uint16_t)(x)<<8 & 0xff00))
#define SWAP32(x) (((uint32_t)(x)>>24 & 0xfful) | ((uint32_t)(x)>>8 & 0xff00ul) | ((uint32_t)(x)<<8 & 0xff0000ul) | ((uint32_t)(x)<<24 & 0xff000000ul))

#if defined(__BIGENDIAN__) || defined(_BIG_ENDIAN) || defined(BIG_ENDIAN)

// big endian config
#define HOST2LE32(x) SWAP32(x)
#define HOST2BE32(x) (x)
#define LE2HOST32(x) SWAP32(x)
#define BE2HOST32(x) (x)

#define HOST2LE16(x) SWAP16(x)
#define HOST2BE16(x) (x)
#define LE2HOST16(x) SWAP16(x)
#define BE2HOST16(x) (x)

#else

// little endian config
#define HOST2LE32(x) (x)
#define HOST2BE32(x) SWAP32(x)
#define LE2HOST32(x) (x)
#define BE2HOST32(x) SWAP32(x)

#define HOST2LE16(x) (x)
#define HOST2BE16(x) SWAP16(x)
#define LE2HOST16(x) (x)
#define BE2HOST16(x) SWAP16(x)

#endif

#if (defined(__PPC__) || defined(__ppc__)) && (defined(__BIGENDIAN__) || defined(_BIG_ENDIAN) || defined(BIG_ENDIAN))

// Use SDK intrinsics
static __inline__ uint16_t GETLE16(void *ptr) {
    return __lhbrx(ptr);
}
static __inline__ uint32_t GETLE32(void *ptr) {
    return __lwbrx(ptr);
}
static __inline__ uint32_t GETLE16D(void *ptr) {
    uint32_t ret = __lwbrx(ptr);
	return (ret << 16) | (ret >> 16);
}

static __inline__ void PUTLE16(void *ptr, uint16_t val) {
    __sthbrx(ptr, val);
}
static __inline__ void PUTLE32(void *ptr, uint32_t val) {
    __stwbrx(ptr, val);
}

#else
#define GETLE16(X) LE2HOST16(*(uint16_t *)(X))
#define GETLE32(X) LE2HOST32(*(uint32_t *)(X))
static __inline__ uint32_t GETLE16D(void *X) {
	uint32_t val = GETLE32(X);
	return (val<<16 | val >> 16);
}
#define PUTLE16(X, Y) do{*((uint16_t *)(X))=HOST2LE16((uint16_t)(Y));}while(0)
#define PUTLE32(X, Y) do{*((uint32_t *)(X))=HOST2LE32((uint32_t)(Y));}while(0)
#endif

#define GETLEs16(X) ((int16_t)GETLE16(X))
#define GETLEs32(X) ((int32_t)GETLE32(X))

#endif
