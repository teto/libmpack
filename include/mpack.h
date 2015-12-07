#ifndef MPACK_H
#define MPACK_H

#include <limits.h>

#if UINT_MAX == 0xffffffff
typedef int mpack_int32_t;
typedef unsigned int mpack_uint32_t;
#elif ULONG_MAX == 0xffffffff
typedef long mpack_int32_t;
typedef unsigned long mpack_uint32_t;
#else
# error "can't find unsigned 32-bit integer type"
#endif

#if ULLONG_MAX == 0xffffffffffffffff
# define MPACK_USE_64INT
typedef long long mpack_int64_t;
typedef unsigned long long mpack_uint64_t;
#elif UINT64_MAX == 0xffffffffffffffff
# define MPACK_USE_64INT
typedef int64_t mpack_int64_t;
typedef uint64_t mpack_uint64_t;
#endif

#define MPACK_BIG_ENDIAN (*(mpack_uint32_t *)"\xff\0\0\0" == 0xff000000)

typedef union {
  double f64;
#ifdef MPACK_USE_64INT
  mpack_uint64_t u64;
  mpack_int64_t s64;
#endif
  mpack_uint32_t u32;
  mpack_int32_t s32;
  struct {
    mpack_uint32_t lo, hi;
  } components;
} mpack_value_t;

#include <stddef.h>

#ifndef MPACK_DEFAULT_STACK_SIZE
# define MPACK_DEFAULT_STACK_SIZE 32
#endif  /* MPACK_STACK_MAX_SIZE */

typedef enum {
  MPACK_TOKEN_NIL,
  MPACK_TOKEN_BOOLEAN,
  MPACK_TOKEN_UINT,
  MPACK_TOKEN_SINT,
  MPACK_TOKEN_FLOAT,
  MPACK_TOKEN_CHUNK,
  MPACK_TOKEN_BIN,
  MPACK_TOKEN_STR,
  MPACK_TOKEN_EXT,
  MPACK_TOKEN_ARRAY,
  MPACK_TOKEN_MAP
} mpack_token_type_t;


typedef struct mpack_token_s {
  mpack_token_type_t type;  /* Type of token */
  size_t length;            /* Byte length for str/bin/ext/chunk/float/int/uint.
                               Item count for array/map. */
  union {
    mpack_value_t value;    /* 32-bit parts of primitives (bool,int,float) */
    const char *chunk_ptr;  /* Chunk of data from str/bin/ext */
    int ext_type;           /* Type field for ext tokens */
  } data;
} mpack_token_t;

typedef struct mpack_unpacker_s mpack_unpacker_t;

void mpack_unpacker_init(mpack_unpacker_t *unpacker);
mpack_token_t *mpack_unpack(mpack_unpacker_t *unpacker, const char **buf,
    size_t *buflen);

typedef struct mpack_unpack_state_s {
  int code;
  mpack_token_t token;
  /* number of bytes remaining when unpacking values or byte arrays.
   * number of items remaining when unpacking arrays or maps. */
  size_t remaining;
} mpack_unpack_state_t;

struct mpack_unpacker_s {
  void *data;
  mpack_token_t *result;
  size_t stackpos;
  int error_code;
  mpack_unpack_state_t stack[MPACK_DEFAULT_STACK_SIZE];
};

#endif  /* MPACK_H */
