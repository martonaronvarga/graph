
// We need to define _GNU_SOURCE before
// _any_ headers files are imported to get
// the usage statistics of a thread (i.e. have RUSAGE_THREAD) on GNU/Linux
// https://manpages.courier-mta.org/htmlman2/getrusage.2.html
#ifndef _GNU_SOURCE // Avoid possible double-definition warning.
#define _GNU_SOURCE
#endif

#ifdef __clang__
#pragma clang diagnostic ignored "-Wunused-function"
#pragma clang diagnostic ignored "-Wunused-variable"
#pragma clang diagnostic ignored "-Wunused-const-variable"
#pragma clang diagnostic ignored "-Wparentheses"
#pragma clang diagnostic ignored "-Wunused-label"
#pragma clang diagnostic ignored "-Wunused-but-set-variable"
#elif __GNUC__
#pragma GCC diagnostic ignored "-Wunused-function"
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wunused-const-variable"
#pragma GCC diagnostic ignored "-Wparentheses"
#pragma GCC diagnostic ignored "-Wunused-label"
#pragma GCC diagnostic ignored "-Wunused-but-set-variable"
#endif

// Headers
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdio.h>
#include <float.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialisation
struct futhark_context_config;
struct futhark_context_config *futhark_context_config_new(void);
void futhark_context_config_free(struct futhark_context_config *cfg);
int futhark_context_config_set_tuning_param(struct futhark_context_config *cfg, const char *param_name, size_t new_value);
struct futhark_context;
struct futhark_context *futhark_context_new(struct futhark_context_config *cfg);
void futhark_context_free(struct futhark_context *ctx);
void futhark_context_config_set_debugging(struct futhark_context_config *cfg, int flag);
void futhark_context_config_set_profiling(struct futhark_context_config *cfg, int flag);
void futhark_context_config_set_logging(struct futhark_context_config *cfg, int flag);
int futhark_get_tuning_param_count(void);
const char *futhark_get_tuning_param_name(int);
const char *futhark_get_tuning_param_class(int);

// Arrays
struct futhark_i32_1d;
struct futhark_i32_1d *futhark_new_i32_1d(struct futhark_context *ctx, const int32_t *data, int64_t dim0);
struct futhark_i32_1d *futhark_new_raw_i32_1d(struct futhark_context *ctx, unsigned char *data, int64_t dim0);
int futhark_free_i32_1d(struct futhark_context *ctx, struct futhark_i32_1d *arr);
int futhark_values_i32_1d(struct futhark_context *ctx, struct futhark_i32_1d *arr, int32_t *data);
int futhark_index_i32_1d(struct futhark_context *ctx, int32_t *out, struct futhark_i32_1d *arr, int64_t i0);
unsigned char *futhark_values_raw_i32_1d(struct futhark_context *ctx, struct futhark_i32_1d *arr);
const int64_t *futhark_shape_i32_1d(struct futhark_context *ctx, struct futhark_i32_1d *arr);
struct futhark_i64_1d;
struct futhark_i64_1d *futhark_new_i64_1d(struct futhark_context *ctx, const int64_t *data, int64_t dim0);
struct futhark_i64_1d *futhark_new_raw_i64_1d(struct futhark_context *ctx, unsigned char *data, int64_t dim0);
int futhark_free_i64_1d(struct futhark_context *ctx, struct futhark_i64_1d *arr);
int futhark_values_i64_1d(struct futhark_context *ctx, struct futhark_i64_1d *arr, int64_t *data);
int futhark_index_i64_1d(struct futhark_context *ctx, int64_t *out, struct futhark_i64_1d *arr, int64_t i0);
unsigned char *futhark_values_raw_i64_1d(struct futhark_context *ctx, struct futhark_i64_1d *arr);
const int64_t *futhark_shape_i64_1d(struct futhark_context *ctx, struct futhark_i64_1d *arr);

// Opaque values



// Entry points
int futhark_entry_graph_orientation_solve(struct futhark_context *ctx, bool *out0, struct futhark_i64_1d **out1, struct futhark_i64_1d **out2, struct futhark_i64_1d **out3, const int32_t in0, const int32_t in1, const int32_t in2, const struct futhark_i64_1d *in3, const struct futhark_i32_1d *in4, const struct futhark_i32_1d *in5);

// Miscellaneous
int futhark_context_sync(struct futhark_context *ctx);
void futhark_context_config_set_cache_file(struct futhark_context_config *cfg, const char *f);
char *futhark_context_get_error(struct futhark_context *ctx);
void futhark_context_set_logging_file(struct futhark_context *ctx, FILE *f);
void futhark_context_pause_profiling(struct futhark_context *ctx);
void futhark_context_unpause_profiling(struct futhark_context *ctx);
char *futhark_context_report(struct futhark_context *ctx);
int futhark_context_clear_caches(struct futhark_context *ctx);
#define FUTHARK_BACKEND_c
#define FUTHARK_SUCCESS 0
#define FUTHARK_PROGRAM_ERROR 2
#define FUTHARK_OUT_OF_MEMORY 3

#ifdef __cplusplus
}
#endif

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <math.h>
#include <stdint.h>
// If NDEBUG is set, the assert() macro will do nothing. Since Futhark
// (unfortunately) makes use of assert() for error detection (and even some
// side effects), we want to avoid that.
#undef NDEBUG
#include <assert.h>
#include <stdarg.h>
#define SCALAR_FUN_ATTR static inline
// Start of util.h.
//
// Various helper functions that are useful in all generated C code.

#include <errno.h>
#include <string.h>

static const char *fut_progname = "(embedded Futhark)";

static void futhark_panic(int eval, const char *fmt, ...) __attribute__((noreturn));
static char* msgprintf(const char *s, ...);
static void* slurp_file(const char *filename, size_t *size);
static int dump_file(const char *file, const void *buf, size_t n);
struct str_builder;
static void str_builder_init(struct str_builder *b);
static void str_builder(struct str_builder *b, const char *s, ...);
static char *strclone(const char *str);

static void futhark_panic(int eval, const char *fmt, ...) {
  va_list ap;
  va_start(ap, fmt);
  fprintf(stderr, "%s: ", fut_progname);
  vfprintf(stderr, fmt, ap);
  va_end(ap);
  exit(eval);
}

// For generating arbitrary-sized error messages.  It is the callers
// responsibility to free the buffer at some point.
static char* msgprintf(const char *s, ...) {
  va_list vl;
  va_start(vl, s);
  size_t needed = 1 + (size_t)vsnprintf(NULL, 0, s, vl);
  char *buffer = (char*) malloc(needed);
  va_start(vl, s); // Must re-init.
  vsnprintf(buffer, needed, s, vl);
  return buffer;
}

static inline void check_err(int errval, int sets_errno, const char *fun, int line,
                             const char *msg, ...) {
  if (errval) {
    char errnum[10];

    va_list vl;
    va_start(vl, msg);

    fprintf(stderr, "ERROR: ");
    vfprintf(stderr, msg, vl);
    fprintf(stderr, " in %s() at line %d with error code %s\n",
            fun, line,
            sets_errno ? strerror(errno) : errnum);
    exit(errval);
  }
}

#define CHECK_ERR(err, ...) check_err(err, 0, __func__, __LINE__, __VA_ARGS__)
#define CHECK_ERRNO(err, ...) check_err(err, 1, __func__, __LINE__, __VA_ARGS__)

// Read the rest of an open file into a NUL-terminated string; returns
// NULL on error.
static void* fslurp_file(FILE *f, size_t *size) {
  long start = ftell(f);
  fseek(f, 0, SEEK_END);
  long src_size = ftell(f)-start;
  fseek(f, start, SEEK_SET);
  unsigned char *s = (unsigned char*) malloc((size_t)src_size + 1);
  if (fread(s, 1, (size_t)src_size, f) != (size_t)src_size) {
    free(s);
    s = NULL;
  } else {
    s[src_size] = '\0';
  }

  if (size) {
    *size = (size_t)src_size;
  }

  return s;
}

// Read a file into a NUL-terminated string; returns NULL on error.
static void* slurp_file(const char *filename, size_t *size) {
  FILE *f = fopen(filename, "rb"); // To avoid Windows messing with linebreaks.
  if (f == NULL) return NULL;
  unsigned char *s = fslurp_file(f, size);
  fclose(f);
  return s;
}

// Dump 'n' bytes from 'buf' into the file at the designated location.
// Returns 0 on success.
static int dump_file(const char *file, const void *buf, size_t n) {
  FILE *f = fopen(file, "w");

  if (f == NULL) {
    return 1;
  }

  if (fwrite(buf, sizeof(char), n, f) != n) {
    return 1;
  }

  if (fclose(f) != 0) {
    return 1;
  }

  return 0;
}

struct str_builder {
  char *str;
  size_t capacity; // Size of buffer.
  size_t used; // Bytes used, *not* including final zero.
};

static void str_builder_init(struct str_builder *b) {
  b->capacity = 10;
  b->used = 0;
  b->str = malloc(b->capacity);
  b->str[0] = 0;
}

static void str_builder(struct str_builder *b, const char *s, ...) {
  va_list vl;
  va_start(vl, s);
  size_t needed = (size_t)vsnprintf(NULL, 0, s, vl);

  while (b->capacity < b->used + needed + 1) {
    b->capacity *= 2;
    b->str = realloc(b->str, b->capacity);
  }

  va_start(vl, s); // Must re-init.
  vsnprintf(b->str+b->used, b->capacity-b->used, s, vl);
  b->used += needed;
}

static void str_builder_str(struct str_builder *b, const char *s) {
  size_t needed = strlen(s);
  if (b->capacity < b->used + needed + 1) {
    b->capacity *= 2;
    b->str = realloc(b->str, b->capacity);
  }
  strcpy(b->str+b->used, s);
  b->used += needed;
}

static void str_builder_char(struct str_builder *b, char c) {
  size_t needed = 1;
  if (b->capacity < b->used + needed + 1) {
    b->capacity *= 2;
    b->str = realloc(b->str, b->capacity);
  }
  b->str[b->used] = c;
  b->str[b->used+1] = 0;
  b->used += needed;
}

static void str_builder_json_str(struct str_builder* sb, const char* s) {
  str_builder_char(sb, '"');
  for (int j = 0; s[j]; j++) {
    char c = s[j];
    switch (c) {
    case '\n':
      str_builder_str(sb, "\\n");
      break;
    case '"':
      str_builder_str(sb, "\\\"");
      break;
    default:
      str_builder_char(sb, c);
    }
  }
  str_builder_char(sb, '"');
}

static char *strclone(const char *str) {
  size_t size = strlen(str) + 1;
  char *copy = (char*) malloc(size);
  if (copy == NULL) {
    return NULL;
  }

  memcpy(copy, str, size);
  return copy;
}

// Assumes NULL-terminated.
static char *strconcat(const char *src_fragments[]) {
  size_t src_len = 0;
  const char **p;

  for (p = src_fragments; *p; p++) {
    src_len += strlen(*p);
  }

  char *src = (char*) malloc(src_len + 1);
  size_t n = 0;
  for (p = src_fragments; *p; p++) {
    strcpy(src + n, *p);
    n += strlen(*p);
  }

  return src;
}

// End of util.h.
// Start of cache.h

#define CACHE_HASH_SIZE 8 // In 32-bit words.

struct cache_hash {
  uint32_t hash[CACHE_HASH_SIZE];
};

// Initialise a blank cache.
static void cache_hash_init(struct cache_hash *c);

// Hash some bytes and add them to the accumulated hash.
static void cache_hash(struct cache_hash *out, const char *in, size_t n);

// Try to restore cache contents from a file with the given name.
// Assumes the cache is invalid if it contains the given hash.
// Allocates memory and reads the cache conents, which is returned in
// *buf with size *buflen.  If the cache is successfully loaded, this
// function returns 0.  Otherwise it returns nonzero.  Errno is set if
// the failure to load the cache is due to anything except invalid
// cache conents.  Note that failing to restore the cache is not
// necessarily a problem: it might just be invalid or not created yet.
static int cache_restore(const char *fname, const struct cache_hash *hash,
                         unsigned char **buf, size_t *buflen);

// Store cache contents in the given file, with the given hash.
static int cache_store(const char *fname, const struct cache_hash *hash,
                       const unsigned char *buf, size_t buflen);

// Now for the implementation.

static void cache_hash_init(struct cache_hash *c) {
  memset(c->hash, 0, CACHE_HASH_SIZE * sizeof(uint32_t));
}

static void cache_hash(struct cache_hash *out, const char *in, size_t n) {
  // Adaptation of djb2 for larger output size by storing intermediate
  // states.
  uint32_t hash = 5381;
  for (size_t i = 0; i < n; i++) {
    hash = ((hash << 5) + hash) + in[i];
    out->hash[i % CACHE_HASH_SIZE] ^= hash;
  }
}

#define CACHE_HEADER_SIZE 8
static const char cache_header[CACHE_HEADER_SIZE] = "FUTHARK\0";

static int cache_restore(const char *fname, const struct cache_hash *hash,
                         unsigned char **buf, size_t *buflen) {
  FILE *f = fopen(fname, "rb");

  if (f == NULL) {
    return 1;
  }

  char f_header[CACHE_HEADER_SIZE];

  if (fread(f_header, sizeof(char), CACHE_HEADER_SIZE, f) != CACHE_HEADER_SIZE) {
    goto error;
  }

  if (memcmp(f_header, cache_header, CACHE_HEADER_SIZE) != 0) {
    goto error;
  }

  if (fseek(f, 0, SEEK_END) != 0) {
    goto error;
  }
  int64_t f_size = (int64_t)ftell(f);
  if (fseek(f, CACHE_HEADER_SIZE, SEEK_SET) != 0) {
    goto error;
  }

  int64_t expected_size;

  if (fread(&expected_size, sizeof(int64_t), 1, f) != 1) {
    goto error;
  }

  if (f_size != expected_size) {
    errno = 0;
    goto error;
  }

  int32_t f_hash[CACHE_HASH_SIZE];

  if (fread(f_hash, sizeof(int32_t), CACHE_HASH_SIZE, f) != CACHE_HASH_SIZE) {
    goto error;
  }

  if (memcmp(f_hash, hash->hash, CACHE_HASH_SIZE) != 0) {
    errno = 0;
    goto error;
  }

  *buflen = f_size - CACHE_HEADER_SIZE - sizeof(int64_t) - CACHE_HASH_SIZE*sizeof(int32_t);
  *buf = malloc(*buflen);
  if (fread(*buf, sizeof(char), *buflen, f) != *buflen) {
    free(*buf);
    goto error;
  }

  fclose(f);

  return 0;

 error:
  fclose(f);
  return 1;
}

static int cache_store(const char *fname, const struct cache_hash *hash,
                       const unsigned char *buf, size_t buflen) {
  FILE *f = fopen(fname, "wb");

  if (f == NULL) {
    return 1;
  }

  if (fwrite(cache_header, CACHE_HEADER_SIZE, 1, f) != 1) {
    goto error;
  }

  int64_t size = CACHE_HEADER_SIZE + sizeof(int64_t) + CACHE_HASH_SIZE*sizeof(int32_t) + buflen;

  if (fwrite(&size, sizeof(size), 1, f) != 1) {
    goto error;
  }

  if (fwrite(hash->hash, sizeof(int32_t), CACHE_HASH_SIZE, f) != CACHE_HASH_SIZE) {
    goto error;
  }

  if (fwrite(buf, sizeof(unsigned char), buflen, f) != buflen) {
    goto error;
  }

  fclose(f);

  return 0;

 error:
  fclose(f);
  return 1;
}

// End of cache.h
// Start of half.h.

// Conversion functions are from http://half.sourceforge.net/, but
// translated to C.
//
// Copyright (c) 2012-2021 Christian Rau
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#ifndef __OPENCL_VERSION__
#define __constant
#endif

__constant static const uint16_t base_table[512] = {
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000,
  0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0000, 0x0001, 0x0002, 0x0004, 0x0008, 0x0010, 0x0020, 0x0040, 0x0080, 0x0100,
  0x0200, 0x0400, 0x0800, 0x0C00, 0x1000, 0x1400, 0x1800, 0x1C00, 0x2000, 0x2400, 0x2800, 0x2C00, 0x3000, 0x3400, 0x3800, 0x3C00,
  0x4000, 0x4400, 0x4800, 0x4C00, 0x5000, 0x5400, 0x5800, 0x5C00, 0x6000, 0x6400, 0x6800, 0x6C00, 0x7000, 0x7400, 0x7800, 0x7C00,
  0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00,
  0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00,
  0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00,
  0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00,
  0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00,
  0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00,
  0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00, 0x7C00,
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000,
  0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8000, 0x8001, 0x8002, 0x8004, 0x8008, 0x8010, 0x8020, 0x8040, 0x8080, 0x8100,
  0x8200, 0x8400, 0x8800, 0x8C00, 0x9000, 0x9400, 0x9800, 0x9C00, 0xA000, 0xA400, 0xA800, 0xAC00, 0xB000, 0xB400, 0xB800, 0xBC00,
  0xC000, 0xC400, 0xC800, 0xCC00, 0xD000, 0xD400, 0xD800, 0xDC00, 0xE000, 0xE400, 0xE800, 0xEC00, 0xF000, 0xF400, 0xF800, 0xFC00,
  0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00,
  0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00,
  0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00,
  0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00,
  0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00,
  0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00,
  0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00, 0xFC00 };

__constant static const unsigned char shift_table[512] = {
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 13,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 23, 22, 21, 20, 19, 18, 17, 16, 15, 14, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13,
  13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 13, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24,
  24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 24, 13 };

__constant static const uint32_t mantissa_table[2048] = {
  0x00000000, 0x33800000, 0x34000000, 0x34400000, 0x34800000, 0x34A00000, 0x34C00000, 0x34E00000, 0x35000000, 0x35100000, 0x35200000, 0x35300000, 0x35400000, 0x35500000, 0x35600000, 0x35700000,
  0x35800000, 0x35880000, 0x35900000, 0x35980000, 0x35A00000, 0x35A80000, 0x35B00000, 0x35B80000, 0x35C00000, 0x35C80000, 0x35D00000, 0x35D80000, 0x35E00000, 0x35E80000, 0x35F00000, 0x35F80000,
  0x36000000, 0x36040000, 0x36080000, 0x360C0000, 0x36100000, 0x36140000, 0x36180000, 0x361C0000, 0x36200000, 0x36240000, 0x36280000, 0x362C0000, 0x36300000, 0x36340000, 0x36380000, 0x363C0000,
  0x36400000, 0x36440000, 0x36480000, 0x364C0000, 0x36500000, 0x36540000, 0x36580000, 0x365C0000, 0x36600000, 0x36640000, 0x36680000, 0x366C0000, 0x36700000, 0x36740000, 0x36780000, 0x367C0000,
  0x36800000, 0x36820000, 0x36840000, 0x36860000, 0x36880000, 0x368A0000, 0x368C0000, 0x368E0000, 0x36900000, 0x36920000, 0x36940000, 0x36960000, 0x36980000, 0x369A0000, 0x369C0000, 0x369E0000,
  0x36A00000, 0x36A20000, 0x36A40000, 0x36A60000, 0x36A80000, 0x36AA0000, 0x36AC0000, 0x36AE0000, 0x36B00000, 0x36B20000, 0x36B40000, 0x36B60000, 0x36B80000, 0x36BA0000, 0x36BC0000, 0x36BE0000,
  0x36C00000, 0x36C20000, 0x36C40000, 0x36C60000, 0x36C80000, 0x36CA0000, 0x36CC0000, 0x36CE0000, 0x36D00000, 0x36D20000, 0x36D40000, 0x36D60000, 0x36D80000, 0x36DA0000, 0x36DC0000, 0x36DE0000,
  0x36E00000, 0x36E20000, 0x36E40000, 0x36E60000, 0x36E80000, 0x36EA0000, 0x36EC0000, 0x36EE0000, 0x36F00000, 0x36F20000, 0x36F40000, 0x36F60000, 0x36F80000, 0x36FA0000, 0x36FC0000, 0x36FE0000,
  0x37000000, 0x37010000, 0x37020000, 0x37030000, 0x37040000, 0x37050000, 0x37060000, 0x37070000, 0x37080000, 0x37090000, 0x370A0000, 0x370B0000, 0x370C0000, 0x370D0000, 0x370E0000, 0x370F0000,
  0x37100000, 0x37110000, 0x37120000, 0x37130000, 0x37140000, 0x37150000, 0x37160000, 0x37170000, 0x37180000, 0x37190000, 0x371A0000, 0x371B0000, 0x371C0000, 0x371D0000, 0x371E0000, 0x371F0000,
  0x37200000, 0x37210000, 0x37220000, 0x37230000, 0x37240000, 0x37250000, 0x37260000, 0x37270000, 0x37280000, 0x37290000, 0x372A0000, 0x372B0000, 0x372C0000, 0x372D0000, 0x372E0000, 0x372F0000,
  0x37300000, 0x37310000, 0x37320000, 0x37330000, 0x37340000, 0x37350000, 0x37360000, 0x37370000, 0x37380000, 0x37390000, 0x373A0000, 0x373B0000, 0x373C0000, 0x373D0000, 0x373E0000, 0x373F0000,
  0x37400000, 0x37410000, 0x37420000, 0x37430000, 0x37440000, 0x37450000, 0x37460000, 0x37470000, 0x37480000, 0x37490000, 0x374A0000, 0x374B0000, 0x374C0000, 0x374D0000, 0x374E0000, 0x374F0000,
  0x37500000, 0x37510000, 0x37520000, 0x37530000, 0x37540000, 0x37550000, 0x37560000, 0x37570000, 0x37580000, 0x37590000, 0x375A0000, 0x375B0000, 0x375C0000, 0x375D0000, 0x375E0000, 0x375F0000,
  0x37600000, 0x37610000, 0x37620000, 0x37630000, 0x37640000, 0x37650000, 0x37660000, 0x37670000, 0x37680000, 0x37690000, 0x376A0000, 0x376B0000, 0x376C0000, 0x376D0000, 0x376E0000, 0x376F0000,
  0x37700000, 0x37710000, 0x37720000, 0x37730000, 0x37740000, 0x37750000, 0x37760000, 0x37770000, 0x37780000, 0x37790000, 0x377A0000, 0x377B0000, 0x377C0000, 0x377D0000, 0x377E0000, 0x377F0000,
  0x37800000, 0x37808000, 0x37810000, 0x37818000, 0x37820000, 0x37828000, 0x37830000, 0x37838000, 0x37840000, 0x37848000, 0x37850000, 0x37858000, 0x37860000, 0x37868000, 0x37870000, 0x37878000,
  0x37880000, 0x37888000, 0x37890000, 0x37898000, 0x378A0000, 0x378A8000, 0x378B0000, 0x378B8000, 0x378C0000, 0x378C8000, 0x378D0000, 0x378D8000, 0x378E0000, 0x378E8000, 0x378F0000, 0x378F8000,
  0x37900000, 0x37908000, 0x37910000, 0x37918000, 0x37920000, 0x37928000, 0x37930000, 0x37938000, 0x37940000, 0x37948000, 0x37950000, 0x37958000, 0x37960000, 0x37968000, 0x37970000, 0x37978000,
  0x37980000, 0x37988000, 0x37990000, 0x37998000, 0x379A0000, 0x379A8000, 0x379B0000, 0x379B8000, 0x379C0000, 0x379C8000, 0x379D0000, 0x379D8000, 0x379E0000, 0x379E8000, 0x379F0000, 0x379F8000,
  0x37A00000, 0x37A08000, 0x37A10000, 0x37A18000, 0x37A20000, 0x37A28000, 0x37A30000, 0x37A38000, 0x37A40000, 0x37A48000, 0x37A50000, 0x37A58000, 0x37A60000, 0x37A68000, 0x37A70000, 0x37A78000,
  0x37A80000, 0x37A88000, 0x37A90000, 0x37A98000, 0x37AA0000, 0x37AA8000, 0x37AB0000, 0x37AB8000, 0x37AC0000, 0x37AC8000, 0x37AD0000, 0x37AD8000, 0x37AE0000, 0x37AE8000, 0x37AF0000, 0x37AF8000,
  0x37B00000, 0x37B08000, 0x37B10000, 0x37B18000, 0x37B20000, 0x37B28000, 0x37B30000, 0x37B38000, 0x37B40000, 0x37B48000, 0x37B50000, 0x37B58000, 0x37B60000, 0x37B68000, 0x37B70000, 0x37B78000,
  0x37B80000, 0x37B88000, 0x37B90000, 0x37B98000, 0x37BA0000, 0x37BA8000, 0x37BB0000, 0x37BB8000, 0x37BC0000, 0x37BC8000, 0x37BD0000, 0x37BD8000, 0x37BE0000, 0x37BE8000, 0x37BF0000, 0x37BF8000,
  0x37C00000, 0x37C08000, 0x37C10000, 0x37C18000, 0x37C20000, 0x37C28000, 0x37C30000, 0x37C38000, 0x37C40000, 0x37C48000, 0x37C50000, 0x37C58000, 0x37C60000, 0x37C68000, 0x37C70000, 0x37C78000,
  0x37C80000, 0x37C88000, 0x37C90000, 0x37C98000, 0x37CA0000, 0x37CA8000, 0x37CB0000, 0x37CB8000, 0x37CC0000, 0x37CC8000, 0x37CD0000, 0x37CD8000, 0x37CE0000, 0x37CE8000, 0x37CF0000, 0x37CF8000,
  0x37D00000, 0x37D08000, 0x37D10000, 0x37D18000, 0x37D20000, 0x37D28000, 0x37D30000, 0x37D38000, 0x37D40000, 0x37D48000, 0x37D50000, 0x37D58000, 0x37D60000, 0x37D68000, 0x37D70000, 0x37D78000,
  0x37D80000, 0x37D88000, 0x37D90000, 0x37D98000, 0x37DA0000, 0x37DA8000, 0x37DB0000, 0x37DB8000, 0x37DC0000, 0x37DC8000, 0x37DD0000, 0x37DD8000, 0x37DE0000, 0x37DE8000, 0x37DF0000, 0x37DF8000,
  0x37E00000, 0x37E08000, 0x37E10000, 0x37E18000, 0x37E20000, 0x37E28000, 0x37E30000, 0x37E38000, 0x37E40000, 0x37E48000, 0x37E50000, 0x37E58000, 0x37E60000, 0x37E68000, 0x37E70000, 0x37E78000,
  0x37E80000, 0x37E88000, 0x37E90000, 0x37E98000, 0x37EA0000, 0x37EA8000, 0x37EB0000, 0x37EB8000, 0x37EC0000, 0x37EC8000, 0x37ED0000, 0x37ED8000, 0x37EE0000, 0x37EE8000, 0x37EF0000, 0x37EF8000,
  0x37F00000, 0x37F08000, 0x37F10000, 0x37F18000, 0x37F20000, 0x37F28000, 0x37F30000, 0x37F38000, 0x37F40000, 0x37F48000, 0x37F50000, 0x37F58000, 0x37F60000, 0x37F68000, 0x37F70000, 0x37F78000,
  0x37F80000, 0x37F88000, 0x37F90000, 0x37F98000, 0x37FA0000, 0x37FA8000, 0x37FB0000, 0x37FB8000, 0x37FC0000, 0x37FC8000, 0x37FD0000, 0x37FD8000, 0x37FE0000, 0x37FE8000, 0x37FF0000, 0x37FF8000,
  0x38000000, 0x38004000, 0x38008000, 0x3800C000, 0x38010000, 0x38014000, 0x38018000, 0x3801C000, 0x38020000, 0x38024000, 0x38028000, 0x3802C000, 0x38030000, 0x38034000, 0x38038000, 0x3803C000,
  0x38040000, 0x38044000, 0x38048000, 0x3804C000, 0x38050000, 0x38054000, 0x38058000, 0x3805C000, 0x38060000, 0x38064000, 0x38068000, 0x3806C000, 0x38070000, 0x38074000, 0x38078000, 0x3807C000,
  0x38080000, 0x38084000, 0x38088000, 0x3808C000, 0x38090000, 0x38094000, 0x38098000, 0x3809C000, 0x380A0000, 0x380A4000, 0x380A8000, 0x380AC000, 0x380B0000, 0x380B4000, 0x380B8000, 0x380BC000,
  0x380C0000, 0x380C4000, 0x380C8000, 0x380CC000, 0x380D0000, 0x380D4000, 0x380D8000, 0x380DC000, 0x380E0000, 0x380E4000, 0x380E8000, 0x380EC000, 0x380F0000, 0x380F4000, 0x380F8000, 0x380FC000,
  0x38100000, 0x38104000, 0x38108000, 0x3810C000, 0x38110000, 0x38114000, 0x38118000, 0x3811C000, 0x38120000, 0x38124000, 0x38128000, 0x3812C000, 0x38130000, 0x38134000, 0x38138000, 0x3813C000,
  0x38140000, 0x38144000, 0x38148000, 0x3814C000, 0x38150000, 0x38154000, 0x38158000, 0x3815C000, 0x38160000, 0x38164000, 0x38168000, 0x3816C000, 0x38170000, 0x38174000, 0x38178000, 0x3817C000,
  0x38180000, 0x38184000, 0x38188000, 0x3818C000, 0x38190000, 0x38194000, 0x38198000, 0x3819C000, 0x381A0000, 0x381A4000, 0x381A8000, 0x381AC000, 0x381B0000, 0x381B4000, 0x381B8000, 0x381BC000,
  0x381C0000, 0x381C4000, 0x381C8000, 0x381CC000, 0x381D0000, 0x381D4000, 0x381D8000, 0x381DC000, 0x381E0000, 0x381E4000, 0x381E8000, 0x381EC000, 0x381F0000, 0x381F4000, 0x381F8000, 0x381FC000,
  0x38200000, 0x38204000, 0x38208000, 0x3820C000, 0x38210000, 0x38214000, 0x38218000, 0x3821C000, 0x38220000, 0x38224000, 0x38228000, 0x3822C000, 0x38230000, 0x38234000, 0x38238000, 0x3823C000,
  0x38240000, 0x38244000, 0x38248000, 0x3824C000, 0x38250000, 0x38254000, 0x38258000, 0x3825C000, 0x38260000, 0x38264000, 0x38268000, 0x3826C000, 0x38270000, 0x38274000, 0x38278000, 0x3827C000,
  0x38280000, 0x38284000, 0x38288000, 0x3828C000, 0x38290000, 0x38294000, 0x38298000, 0x3829C000, 0x382A0000, 0x382A4000, 0x382A8000, 0x382AC000, 0x382B0000, 0x382B4000, 0x382B8000, 0x382BC000,
  0x382C0000, 0x382C4000, 0x382C8000, 0x382CC000, 0x382D0000, 0x382D4000, 0x382D8000, 0x382DC000, 0x382E0000, 0x382E4000, 0x382E8000, 0x382EC000, 0x382F0000, 0x382F4000, 0x382F8000, 0x382FC000,
  0x38300000, 0x38304000, 0x38308000, 0x3830C000, 0x38310000, 0x38314000, 0x38318000, 0x3831C000, 0x38320000, 0x38324000, 0x38328000, 0x3832C000, 0x38330000, 0x38334000, 0x38338000, 0x3833C000,
  0x38340000, 0x38344000, 0x38348000, 0x3834C000, 0x38350000, 0x38354000, 0x38358000, 0x3835C000, 0x38360000, 0x38364000, 0x38368000, 0x3836C000, 0x38370000, 0x38374000, 0x38378000, 0x3837C000,
  0x38380000, 0x38384000, 0x38388000, 0x3838C000, 0x38390000, 0x38394000, 0x38398000, 0x3839C000, 0x383A0000, 0x383A4000, 0x383A8000, 0x383AC000, 0x383B0000, 0x383B4000, 0x383B8000, 0x383BC000,
  0x383C0000, 0x383C4000, 0x383C8000, 0x383CC000, 0x383D0000, 0x383D4000, 0x383D8000, 0x383DC000, 0x383E0000, 0x383E4000, 0x383E8000, 0x383EC000, 0x383F0000, 0x383F4000, 0x383F8000, 0x383FC000,
  0x38400000, 0x38404000, 0x38408000, 0x3840C000, 0x38410000, 0x38414000, 0x38418000, 0x3841C000, 0x38420000, 0x38424000, 0x38428000, 0x3842C000, 0x38430000, 0x38434000, 0x38438000, 0x3843C000,
  0x38440000, 0x38444000, 0x38448000, 0x3844C000, 0x38450000, 0x38454000, 0x38458000, 0x3845C000, 0x38460000, 0x38464000, 0x38468000, 0x3846C000, 0x38470000, 0x38474000, 0x38478000, 0x3847C000,
  0x38480000, 0x38484000, 0x38488000, 0x3848C000, 0x38490000, 0x38494000, 0x38498000, 0x3849C000, 0x384A0000, 0x384A4000, 0x384A8000, 0x384AC000, 0x384B0000, 0x384B4000, 0x384B8000, 0x384BC000,
  0x384C0000, 0x384C4000, 0x384C8000, 0x384CC000, 0x384D0000, 0x384D4000, 0x384D8000, 0x384DC000, 0x384E0000, 0x384E4000, 0x384E8000, 0x384EC000, 0x384F0000, 0x384F4000, 0x384F8000, 0x384FC000,
  0x38500000, 0x38504000, 0x38508000, 0x3850C000, 0x38510000, 0x38514000, 0x38518000, 0x3851C000, 0x38520000, 0x38524000, 0x38528000, 0x3852C000, 0x38530000, 0x38534000, 0x38538000, 0x3853C000,
  0x38540000, 0x38544000, 0x38548000, 0x3854C000, 0x38550000, 0x38554000, 0x38558000, 0x3855C000, 0x38560000, 0x38564000, 0x38568000, 0x3856C000, 0x38570000, 0x38574000, 0x38578000, 0x3857C000,
  0x38580000, 0x38584000, 0x38588000, 0x3858C000, 0x38590000, 0x38594000, 0x38598000, 0x3859C000, 0x385A0000, 0x385A4000, 0x385A8000, 0x385AC000, 0x385B0000, 0x385B4000, 0x385B8000, 0x385BC000,
  0x385C0000, 0x385C4000, 0x385C8000, 0x385CC000, 0x385D0000, 0x385D4000, 0x385D8000, 0x385DC000, 0x385E0000, 0x385E4000, 0x385E8000, 0x385EC000, 0x385F0000, 0x385F4000, 0x385F8000, 0x385FC000,
  0x38600000, 0x38604000, 0x38608000, 0x3860C000, 0x38610000, 0x38614000, 0x38618000, 0x3861C000, 0x38620000, 0x38624000, 0x38628000, 0x3862C000, 0x38630000, 0x38634000, 0x38638000, 0x3863C000,
  0x38640000, 0x38644000, 0x38648000, 0x3864C000, 0x38650000, 0x38654000, 0x38658000, 0x3865C000, 0x38660000, 0x38664000, 0x38668000, 0x3866C000, 0x38670000, 0x38674000, 0x38678000, 0x3867C000,
  0x38680000, 0x38684000, 0x38688000, 0x3868C000, 0x38690000, 0x38694000, 0x38698000, 0x3869C000, 0x386A0000, 0x386A4000, 0x386A8000, 0x386AC000, 0x386B0000, 0x386B4000, 0x386B8000, 0x386BC000,
  0x386C0000, 0x386C4000, 0x386C8000, 0x386CC000, 0x386D0000, 0x386D4000, 0x386D8000, 0x386DC000, 0x386E0000, 0x386E4000, 0x386E8000, 0x386EC000, 0x386F0000, 0x386F4000, 0x386F8000, 0x386FC000,
  0x38700000, 0x38704000, 0x38708000, 0x3870C000, 0x38710000, 0x38714000, 0x38718000, 0x3871C000, 0x38720000, 0x38724000, 0x38728000, 0x3872C000, 0x38730000, 0x38734000, 0x38738000, 0x3873C000,
  0x38740000, 0x38744000, 0x38748000, 0x3874C000, 0x38750000, 0x38754000, 0x38758000, 0x3875C000, 0x38760000, 0x38764000, 0x38768000, 0x3876C000, 0x38770000, 0x38774000, 0x38778000, 0x3877C000,
  0x38780000, 0x38784000, 0x38788000, 0x3878C000, 0x38790000, 0x38794000, 0x38798000, 0x3879C000, 0x387A0000, 0x387A4000, 0x387A8000, 0x387AC000, 0x387B0000, 0x387B4000, 0x387B8000, 0x387BC000,
  0x387C0000, 0x387C4000, 0x387C8000, 0x387CC000, 0x387D0000, 0x387D4000, 0x387D8000, 0x387DC000, 0x387E0000, 0x387E4000, 0x387E8000, 0x387EC000, 0x387F0000, 0x387F4000, 0x387F8000, 0x387FC000,
  0x38000000, 0x38002000, 0x38004000, 0x38006000, 0x38008000, 0x3800A000, 0x3800C000, 0x3800E000, 0x38010000, 0x38012000, 0x38014000, 0x38016000, 0x38018000, 0x3801A000, 0x3801C000, 0x3801E000,
  0x38020000, 0x38022000, 0x38024000, 0x38026000, 0x38028000, 0x3802A000, 0x3802C000, 0x3802E000, 0x38030000, 0x38032000, 0x38034000, 0x38036000, 0x38038000, 0x3803A000, 0x3803C000, 0x3803E000,
  0x38040000, 0x38042000, 0x38044000, 0x38046000, 0x38048000, 0x3804A000, 0x3804C000, 0x3804E000, 0x38050000, 0x38052000, 0x38054000, 0x38056000, 0x38058000, 0x3805A000, 0x3805C000, 0x3805E000,
  0x38060000, 0x38062000, 0x38064000, 0x38066000, 0x38068000, 0x3806A000, 0x3806C000, 0x3806E000, 0x38070000, 0x38072000, 0x38074000, 0x38076000, 0x38078000, 0x3807A000, 0x3807C000, 0x3807E000,
  0x38080000, 0x38082000, 0x38084000, 0x38086000, 0x38088000, 0x3808A000, 0x3808C000, 0x3808E000, 0x38090000, 0x38092000, 0x38094000, 0x38096000, 0x38098000, 0x3809A000, 0x3809C000, 0x3809E000,
  0x380A0000, 0x380A2000, 0x380A4000, 0x380A6000, 0x380A8000, 0x380AA000, 0x380AC000, 0x380AE000, 0x380B0000, 0x380B2000, 0x380B4000, 0x380B6000, 0x380B8000, 0x380BA000, 0x380BC000, 0x380BE000,
  0x380C0000, 0x380C2000, 0x380C4000, 0x380C6000, 0x380C8000, 0x380CA000, 0x380CC000, 0x380CE000, 0x380D0000, 0x380D2000, 0x380D4000, 0x380D6000, 0x380D8000, 0x380DA000, 0x380DC000, 0x380DE000,
  0x380E0000, 0x380E2000, 0x380E4000, 0x380E6000, 0x380E8000, 0x380EA000, 0x380EC000, 0x380EE000, 0x380F0000, 0x380F2000, 0x380F4000, 0x380F6000, 0x380F8000, 0x380FA000, 0x380FC000, 0x380FE000,
  0x38100000, 0x38102000, 0x38104000, 0x38106000, 0x38108000, 0x3810A000, 0x3810C000, 0x3810E000, 0x38110000, 0x38112000, 0x38114000, 0x38116000, 0x38118000, 0x3811A000, 0x3811C000, 0x3811E000,
  0x38120000, 0x38122000, 0x38124000, 0x38126000, 0x38128000, 0x3812A000, 0x3812C000, 0x3812E000, 0x38130000, 0x38132000, 0x38134000, 0x38136000, 0x38138000, 0x3813A000, 0x3813C000, 0x3813E000,
  0x38140000, 0x38142000, 0x38144000, 0x38146000, 0x38148000, 0x3814A000, 0x3814C000, 0x3814E000, 0x38150000, 0x38152000, 0x38154000, 0x38156000, 0x38158000, 0x3815A000, 0x3815C000, 0x3815E000,
  0x38160000, 0x38162000, 0x38164000, 0x38166000, 0x38168000, 0x3816A000, 0x3816C000, 0x3816E000, 0x38170000, 0x38172000, 0x38174000, 0x38176000, 0x38178000, 0x3817A000, 0x3817C000, 0x3817E000,
  0x38180000, 0x38182000, 0x38184000, 0x38186000, 0x38188000, 0x3818A000, 0x3818C000, 0x3818E000, 0x38190000, 0x38192000, 0x38194000, 0x38196000, 0x38198000, 0x3819A000, 0x3819C000, 0x3819E000,
  0x381A0000, 0x381A2000, 0x381A4000, 0x381A6000, 0x381A8000, 0x381AA000, 0x381AC000, 0x381AE000, 0x381B0000, 0x381B2000, 0x381B4000, 0x381B6000, 0x381B8000, 0x381BA000, 0x381BC000, 0x381BE000,
  0x381C0000, 0x381C2000, 0x381C4000, 0x381C6000, 0x381C8000, 0x381CA000, 0x381CC000, 0x381CE000, 0x381D0000, 0x381D2000, 0x381D4000, 0x381D6000, 0x381D8000, 0x381DA000, 0x381DC000, 0x381DE000,
  0x381E0000, 0x381E2000, 0x381E4000, 0x381E6000, 0x381E8000, 0x381EA000, 0x381EC000, 0x381EE000, 0x381F0000, 0x381F2000, 0x381F4000, 0x381F6000, 0x381F8000, 0x381FA000, 0x381FC000, 0x381FE000,
  0x38200000, 0x38202000, 0x38204000, 0x38206000, 0x38208000, 0x3820A000, 0x3820C000, 0x3820E000, 0x38210000, 0x38212000, 0x38214000, 0x38216000, 0x38218000, 0x3821A000, 0x3821C000, 0x3821E000,
  0x38220000, 0x38222000, 0x38224000, 0x38226000, 0x38228000, 0x3822A000, 0x3822C000, 0x3822E000, 0x38230000, 0x38232000, 0x38234000, 0x38236000, 0x38238000, 0x3823A000, 0x3823C000, 0x3823E000,
  0x38240000, 0x38242000, 0x38244000, 0x38246000, 0x38248000, 0x3824A000, 0x3824C000, 0x3824E000, 0x38250000, 0x38252000, 0x38254000, 0x38256000, 0x38258000, 0x3825A000, 0x3825C000, 0x3825E000,
  0x38260000, 0x38262000, 0x38264000, 0x38266000, 0x38268000, 0x3826A000, 0x3826C000, 0x3826E000, 0x38270000, 0x38272000, 0x38274000, 0x38276000, 0x38278000, 0x3827A000, 0x3827C000, 0x3827E000,
  0x38280000, 0x38282000, 0x38284000, 0x38286000, 0x38288000, 0x3828A000, 0x3828C000, 0x3828E000, 0x38290000, 0x38292000, 0x38294000, 0x38296000, 0x38298000, 0x3829A000, 0x3829C000, 0x3829E000,
  0x382A0000, 0x382A2000, 0x382A4000, 0x382A6000, 0x382A8000, 0x382AA000, 0x382AC000, 0x382AE000, 0x382B0000, 0x382B2000, 0x382B4000, 0x382B6000, 0x382B8000, 0x382BA000, 0x382BC000, 0x382BE000,
  0x382C0000, 0x382C2000, 0x382C4000, 0x382C6000, 0x382C8000, 0x382CA000, 0x382CC000, 0x382CE000, 0x382D0000, 0x382D2000, 0x382D4000, 0x382D6000, 0x382D8000, 0x382DA000, 0x382DC000, 0x382DE000,
  0x382E0000, 0x382E2000, 0x382E4000, 0x382E6000, 0x382E8000, 0x382EA000, 0x382EC000, 0x382EE000, 0x382F0000, 0x382F2000, 0x382F4000, 0x382F6000, 0x382F8000, 0x382FA000, 0x382FC000, 0x382FE000,
  0x38300000, 0x38302000, 0x38304000, 0x38306000, 0x38308000, 0x3830A000, 0x3830C000, 0x3830E000, 0x38310000, 0x38312000, 0x38314000, 0x38316000, 0x38318000, 0x3831A000, 0x3831C000, 0x3831E000,
  0x38320000, 0x38322000, 0x38324000, 0x38326000, 0x38328000, 0x3832A000, 0x3832C000, 0x3832E000, 0x38330000, 0x38332000, 0x38334000, 0x38336000, 0x38338000, 0x3833A000, 0x3833C000, 0x3833E000,
  0x38340000, 0x38342000, 0x38344000, 0x38346000, 0x38348000, 0x3834A000, 0x3834C000, 0x3834E000, 0x38350000, 0x38352000, 0x38354000, 0x38356000, 0x38358000, 0x3835A000, 0x3835C000, 0x3835E000,
  0x38360000, 0x38362000, 0x38364000, 0x38366000, 0x38368000, 0x3836A000, 0x3836C000, 0x3836E000, 0x38370000, 0x38372000, 0x38374000, 0x38376000, 0x38378000, 0x3837A000, 0x3837C000, 0x3837E000,
  0x38380000, 0x38382000, 0x38384000, 0x38386000, 0x38388000, 0x3838A000, 0x3838C000, 0x3838E000, 0x38390000, 0x38392000, 0x38394000, 0x38396000, 0x38398000, 0x3839A000, 0x3839C000, 0x3839E000,
  0x383A0000, 0x383A2000, 0x383A4000, 0x383A6000, 0x383A8000, 0x383AA000, 0x383AC000, 0x383AE000, 0x383B0000, 0x383B2000, 0x383B4000, 0x383B6000, 0x383B8000, 0x383BA000, 0x383BC000, 0x383BE000,
  0x383C0000, 0x383C2000, 0x383C4000, 0x383C6000, 0x383C8000, 0x383CA000, 0x383CC000, 0x383CE000, 0x383D0000, 0x383D2000, 0x383D4000, 0x383D6000, 0x383D8000, 0x383DA000, 0x383DC000, 0x383DE000,
  0x383E0000, 0x383E2000, 0x383E4000, 0x383E6000, 0x383E8000, 0x383EA000, 0x383EC000, 0x383EE000, 0x383F0000, 0x383F2000, 0x383F4000, 0x383F6000, 0x383F8000, 0x383FA000, 0x383FC000, 0x383FE000,
  0x38400000, 0x38402000, 0x38404000, 0x38406000, 0x38408000, 0x3840A000, 0x3840C000, 0x3840E000, 0x38410000, 0x38412000, 0x38414000, 0x38416000, 0x38418000, 0x3841A000, 0x3841C000, 0x3841E000,
  0x38420000, 0x38422000, 0x38424000, 0x38426000, 0x38428000, 0x3842A000, 0x3842C000, 0x3842E000, 0x38430000, 0x38432000, 0x38434000, 0x38436000, 0x38438000, 0x3843A000, 0x3843C000, 0x3843E000,
  0x38440000, 0x38442000, 0x38444000, 0x38446000, 0x38448000, 0x3844A000, 0x3844C000, 0x3844E000, 0x38450000, 0x38452000, 0x38454000, 0x38456000, 0x38458000, 0x3845A000, 0x3845C000, 0x3845E000,
  0x38460000, 0x38462000, 0x38464000, 0x38466000, 0x38468000, 0x3846A000, 0x3846C000, 0x3846E000, 0x38470000, 0x38472000, 0x38474000, 0x38476000, 0x38478000, 0x3847A000, 0x3847C000, 0x3847E000,
  0x38480000, 0x38482000, 0x38484000, 0x38486000, 0x38488000, 0x3848A000, 0x3848C000, 0x3848E000, 0x38490000, 0x38492000, 0x38494000, 0x38496000, 0x38498000, 0x3849A000, 0x3849C000, 0x3849E000,
  0x384A0000, 0x384A2000, 0x384A4000, 0x384A6000, 0x384A8000, 0x384AA000, 0x384AC000, 0x384AE000, 0x384B0000, 0x384B2000, 0x384B4000, 0x384B6000, 0x384B8000, 0x384BA000, 0x384BC000, 0x384BE000,
  0x384C0000, 0x384C2000, 0x384C4000, 0x384C6000, 0x384C8000, 0x384CA000, 0x384CC000, 0x384CE000, 0x384D0000, 0x384D2000, 0x384D4000, 0x384D6000, 0x384D8000, 0x384DA000, 0x384DC000, 0x384DE000,
  0x384E0000, 0x384E2000, 0x384E4000, 0x384E6000, 0x384E8000, 0x384EA000, 0x384EC000, 0x384EE000, 0x384F0000, 0x384F2000, 0x384F4000, 0x384F6000, 0x384F8000, 0x384FA000, 0x384FC000, 0x384FE000,
  0x38500000, 0x38502000, 0x38504000, 0x38506000, 0x38508000, 0x3850A000, 0x3850C000, 0x3850E000, 0x38510000, 0x38512000, 0x38514000, 0x38516000, 0x38518000, 0x3851A000, 0x3851C000, 0x3851E000,
  0x38520000, 0x38522000, 0x38524000, 0x38526000, 0x38528000, 0x3852A000, 0x3852C000, 0x3852E000, 0x38530000, 0x38532000, 0x38534000, 0x38536000, 0x38538000, 0x3853A000, 0x3853C000, 0x3853E000,
  0x38540000, 0x38542000, 0x38544000, 0x38546000, 0x38548000, 0x3854A000, 0x3854C000, 0x3854E000, 0x38550000, 0x38552000, 0x38554000, 0x38556000, 0x38558000, 0x3855A000, 0x3855C000, 0x3855E000,
  0x38560000, 0x38562000, 0x38564000, 0x38566000, 0x38568000, 0x3856A000, 0x3856C000, 0x3856E000, 0x38570000, 0x38572000, 0x38574000, 0x38576000, 0x38578000, 0x3857A000, 0x3857C000, 0x3857E000,
  0x38580000, 0x38582000, 0x38584000, 0x38586000, 0x38588000, 0x3858A000, 0x3858C000, 0x3858E000, 0x38590000, 0x38592000, 0x38594000, 0x38596000, 0x38598000, 0x3859A000, 0x3859C000, 0x3859E000,
  0x385A0000, 0x385A2000, 0x385A4000, 0x385A6000, 0x385A8000, 0x385AA000, 0x385AC000, 0x385AE000, 0x385B0000, 0x385B2000, 0x385B4000, 0x385B6000, 0x385B8000, 0x385BA000, 0x385BC000, 0x385BE000,
  0x385C0000, 0x385C2000, 0x385C4000, 0x385C6000, 0x385C8000, 0x385CA000, 0x385CC000, 0x385CE000, 0x385D0000, 0x385D2000, 0x385D4000, 0x385D6000, 0x385D8000, 0x385DA000, 0x385DC000, 0x385DE000,
  0x385E0000, 0x385E2000, 0x385E4000, 0x385E6000, 0x385E8000, 0x385EA000, 0x385EC000, 0x385EE000, 0x385F0000, 0x385F2000, 0x385F4000, 0x385F6000, 0x385F8000, 0x385FA000, 0x385FC000, 0x385FE000,
  0x38600000, 0x38602000, 0x38604000, 0x38606000, 0x38608000, 0x3860A000, 0x3860C000, 0x3860E000, 0x38610000, 0x38612000, 0x38614000, 0x38616000, 0x38618000, 0x3861A000, 0x3861C000, 0x3861E000,
  0x38620000, 0x38622000, 0x38624000, 0x38626000, 0x38628000, 0x3862A000, 0x3862C000, 0x3862E000, 0x38630000, 0x38632000, 0x38634000, 0x38636000, 0x38638000, 0x3863A000, 0x3863C000, 0x3863E000,
  0x38640000, 0x38642000, 0x38644000, 0x38646000, 0x38648000, 0x3864A000, 0x3864C000, 0x3864E000, 0x38650000, 0x38652000, 0x38654000, 0x38656000, 0x38658000, 0x3865A000, 0x3865C000, 0x3865E000,
  0x38660000, 0x38662000, 0x38664000, 0x38666000, 0x38668000, 0x3866A000, 0x3866C000, 0x3866E000, 0x38670000, 0x38672000, 0x38674000, 0x38676000, 0x38678000, 0x3867A000, 0x3867C000, 0x3867E000,
  0x38680000, 0x38682000, 0x38684000, 0x38686000, 0x38688000, 0x3868A000, 0x3868C000, 0x3868E000, 0x38690000, 0x38692000, 0x38694000, 0x38696000, 0x38698000, 0x3869A000, 0x3869C000, 0x3869E000,
  0x386A0000, 0x386A2000, 0x386A4000, 0x386A6000, 0x386A8000, 0x386AA000, 0x386AC000, 0x386AE000, 0x386B0000, 0x386B2000, 0x386B4000, 0x386B6000, 0x386B8000, 0x386BA000, 0x386BC000, 0x386BE000,
  0x386C0000, 0x386C2000, 0x386C4000, 0x386C6000, 0x386C8000, 0x386CA000, 0x386CC000, 0x386CE000, 0x386D0000, 0x386D2000, 0x386D4000, 0x386D6000, 0x386D8000, 0x386DA000, 0x386DC000, 0x386DE000,
  0x386E0000, 0x386E2000, 0x386E4000, 0x386E6000, 0x386E8000, 0x386EA000, 0x386EC000, 0x386EE000, 0x386F0000, 0x386F2000, 0x386F4000, 0x386F6000, 0x386F8000, 0x386FA000, 0x386FC000, 0x386FE000,
  0x38700000, 0x38702000, 0x38704000, 0x38706000, 0x38708000, 0x3870A000, 0x3870C000, 0x3870E000, 0x38710000, 0x38712000, 0x38714000, 0x38716000, 0x38718000, 0x3871A000, 0x3871C000, 0x3871E000,
  0x38720000, 0x38722000, 0x38724000, 0x38726000, 0x38728000, 0x3872A000, 0x3872C000, 0x3872E000, 0x38730000, 0x38732000, 0x38734000, 0x38736000, 0x38738000, 0x3873A000, 0x3873C000, 0x3873E000,
  0x38740000, 0x38742000, 0x38744000, 0x38746000, 0x38748000, 0x3874A000, 0x3874C000, 0x3874E000, 0x38750000, 0x38752000, 0x38754000, 0x38756000, 0x38758000, 0x3875A000, 0x3875C000, 0x3875E000,
  0x38760000, 0x38762000, 0x38764000, 0x38766000, 0x38768000, 0x3876A000, 0x3876C000, 0x3876E000, 0x38770000, 0x38772000, 0x38774000, 0x38776000, 0x38778000, 0x3877A000, 0x3877C000, 0x3877E000,
  0x38780000, 0x38782000, 0x38784000, 0x38786000, 0x38788000, 0x3878A000, 0x3878C000, 0x3878E000, 0x38790000, 0x38792000, 0x38794000, 0x38796000, 0x38798000, 0x3879A000, 0x3879C000, 0x3879E000,
  0x387A0000, 0x387A2000, 0x387A4000, 0x387A6000, 0x387A8000, 0x387AA000, 0x387AC000, 0x387AE000, 0x387B0000, 0x387B2000, 0x387B4000, 0x387B6000, 0x387B8000, 0x387BA000, 0x387BC000, 0x387BE000,
  0x387C0000, 0x387C2000, 0x387C4000, 0x387C6000, 0x387C8000, 0x387CA000, 0x387CC000, 0x387CE000, 0x387D0000, 0x387D2000, 0x387D4000, 0x387D6000, 0x387D8000, 0x387DA000, 0x387DC000, 0x387DE000,
  0x387E0000, 0x387E2000, 0x387E4000, 0x387E6000, 0x387E8000, 0x387EA000, 0x387EC000, 0x387EE000, 0x387F0000, 0x387F2000, 0x387F4000, 0x387F6000, 0x387F8000, 0x387FA000, 0x387FC000, 0x387FE000 };
__constant static const uint32_t exponent_table[64] = {
  0x00000000, 0x00800000, 0x01000000, 0x01800000, 0x02000000, 0x02800000, 0x03000000, 0x03800000, 0x04000000, 0x04800000, 0x05000000, 0x05800000, 0x06000000, 0x06800000, 0x07000000, 0x07800000,
  0x08000000, 0x08800000, 0x09000000, 0x09800000, 0x0A000000, 0x0A800000, 0x0B000000, 0x0B800000, 0x0C000000, 0x0C800000, 0x0D000000, 0x0D800000, 0x0E000000, 0x0E800000, 0x0F000000, 0x47800000,
  0x80000000, 0x80800000, 0x81000000, 0x81800000, 0x82000000, 0x82800000, 0x83000000, 0x83800000, 0x84000000, 0x84800000, 0x85000000, 0x85800000, 0x86000000, 0x86800000, 0x87000000, 0x87800000,
  0x88000000, 0x88800000, 0x89000000, 0x89800000, 0x8A000000, 0x8A800000, 0x8B000000, 0x8B800000, 0x8C000000, 0x8C800000, 0x8D000000, 0x8D800000, 0x8E000000, 0x8E800000, 0x8F000000, 0xC7800000 };
__constant static const unsigned short offset_table[64] = {
  0, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024,
  0, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024, 1024 };

SCALAR_FUN_ATTR uint16_t float2halfbits(float value) {
  union { float x; uint32_t y; } u;
  u.x = value;
  uint32_t bits = u.y;

  uint16_t hbits = base_table[bits>>23] + (uint16_t)((bits&0x7FFFFF)>>shift_table[bits>>23]);;

  return hbits;
}

SCALAR_FUN_ATTR float halfbits2float(uint16_t value) {
  uint32_t bits = mantissa_table[offset_table[value>>10]+(value&0x3FF)] + exponent_table[value>>10];

  union { uint32_t x; float y; } u;
  u.x = bits;
  return u.y;
}

SCALAR_FUN_ATTR uint16_t halfbitsnextafter(uint16_t from, uint16_t to) {
  int fabs = from & 0x7FFF, tabs = to & 0x7FFF;
  if(fabs > 0x7C00 || tabs > 0x7C00) {
    return ((from&0x7FFF)>0x7C00) ? (from|0x200) : (to|0x200);
  }
  if(from == to || !(fabs|tabs)) {
    return to;
  }
  if(!fabs) {
    return (to&0x8000)+1;
  }
  unsigned int out =
    from +
    (((from>>15)^(unsigned int)((from^(0x8000|(0x8000-(from>>15))))<(to^(0x8000|(0x8000-(to>>15))))))<<1)
    - 1;
  return out;
}

// End of half.h.
// Start of timing.h.

// The function get_wall_time() returns the wall time in microseconds
// (with an unspecified offset).

#ifdef _WIN32

#include <windows.h>

static int64_t get_wall_time(void) {
  LARGE_INTEGER time,freq;
  assert(QueryPerformanceFrequency(&freq));
  assert(QueryPerformanceCounter(&time));
  return ((double)time.QuadPart / freq.QuadPart) * 1000000;
}

static int64_t get_wall_time_ns(void) {
  return get_wall_time() * 1000;
}

#else
// Assuming POSIX

#include <time.h>
#include <sys/time.h>

static int64_t get_wall_time_ns(void) {
  struct timespec time;
  assert(clock_gettime(CLOCK_MONOTONIC, &time) == 0);
  return time.tv_sec * 1000000000 + time.tv_nsec;
}

static int64_t get_wall_time(void) {
  return get_wall_time_ns() / 1000;
}


#endif

// End of timing.h.
// Start of lock.h.

// A very simple cross-platform implementation of locks.  Uses
// pthreads on Unix and some Windows thing there.  Futhark's
// host-level code is not multithreaded, but user code may be, so we
// need some mechanism for ensuring atomic access to API functions.
// This is that mechanism.  It is not exposed to user code at all, so
// we do not have to worry about name collisions.

#ifdef _WIN32

typedef HANDLE lock_t;

static void create_lock(lock_t *lock) {
  *lock = CreateMutex(NULL,  // Default security attributes.
                      FALSE, // Initially unlocked.
                      NULL); // Unnamed.
}

static void lock_lock(lock_t *lock) {
  assert(WaitForSingleObject(*lock, INFINITE) == WAIT_OBJECT_0);
}

static void lock_unlock(lock_t *lock) {
  assert(ReleaseMutex(*lock));
}

static void free_lock(lock_t *lock) {
  CloseHandle(*lock);
}

#else
// Assuming POSIX

#include <pthread.h>

typedef pthread_mutex_t lock_t;

static void create_lock(lock_t *lock) {
  int r = pthread_mutex_init(lock, NULL);
  assert(r == 0);
}

static void lock_lock(lock_t *lock) {
  int r = pthread_mutex_lock(lock);
  assert(r == 0);
}

static void lock_unlock(lock_t *lock) {
  int r = pthread_mutex_unlock(lock);
  assert(r == 0);
}

static void free_lock(lock_t *lock) {
  // Nothing to do for pthreads.
  (void)lock;
}

#endif

// End of lock.h.
// Start of free_list.h.

typedef uintptr_t fl_mem;

// An entry in the free list.  May be invalid, to avoid having to
// deallocate entries as soon as they are removed.  There is also a
// tag, to help with memory reuse.
struct free_list_entry {
  size_t size;
  fl_mem mem;
  const char *tag;
  unsigned char valid;
};

struct free_list {
  struct free_list_entry *entries; // Pointer to entries.
  int capacity;                    // Number of entries.
  int used;                        // Number of valid entries.
  lock_t lock;                     // Thread safety.
};

static void free_list_init(struct free_list *l) {
  l->capacity = 30; // Picked arbitrarily.
  l->used = 0;
  l->entries = (struct free_list_entry*) malloc(sizeof(struct free_list_entry) * l->capacity);
  for (int i = 0; i < l->capacity; i++) {
    l->entries[i].valid = 0;
  }
  create_lock(&l->lock);
}

// Remove invalid entries from the free list.
static void free_list_pack(struct free_list *l) {
  lock_lock(&l->lock);
  int p = 0;
  for (int i = 0; i < l->capacity; i++) {
    if (l->entries[i].valid) {
      l->entries[p] = l->entries[i];
      if (i > p) {
        l->entries[i].valid = 0;
      }
      p++;
    }
  }

  // Now p is the number of used elements.  We don't want it to go
  // less than the default capacity (although in practice it's OK as
  // long as it doesn't become 1).
  if (p < 30) {
    p = 30;
  }
  l->entries = realloc(l->entries, p * sizeof(struct free_list_entry));
  l->capacity = p;
  lock_unlock(&l->lock);
}

static void free_list_destroy(struct free_list *l) {
  assert(l->used == 0);
  free(l->entries);
  free_lock(&l->lock);
}

// Not part of the interface, so no locking.
static int free_list_find_invalid(struct free_list *l) {
  int i;
  for (i = 0; i < l->capacity; i++) {
    if (!l->entries[i].valid) {
      break;
    }
  }
  return i;
}

static void free_list_insert(struct free_list *l, size_t size, fl_mem mem, const char *tag) {
  lock_lock(&l->lock);
  int i = free_list_find_invalid(l);

  if (i == l->capacity) {
    // List is full; so we have to grow it.
    int new_capacity = l->capacity * 2 * sizeof(struct free_list_entry);
    l->entries = realloc(l->entries, new_capacity);
    for (int j = 0; j < l->capacity; j++) {
      l->entries[j+l->capacity].valid = 0;
    }
    l->capacity *= 2;
  }

  // Now 'i' points to the first invalid entry.
  l->entries[i].valid = 1;
  l->entries[i].size = size;
  l->entries[i].mem = mem;
  l->entries[i].tag = tag;

  l->used++;
  lock_unlock(&l->lock);
}

// Determine whether this entry in the free list is acceptable for
// satisfying the request.  Not public, so no locking.
static bool free_list_acceptable(size_t size, const char* tag, struct free_list_entry *entry) {
  // We check not just the hard requirement (is the entry acceptable
  // and big enough?) but also put a cap on how much wasted space
  // (internal fragmentation) we allow.  This is necessarily a
  // heuristic, and a crude one.

  if (!entry->valid) {
    return false;
  }

  if (size > entry->size) {
    return false;
  }

  // We know the block fits.  Now the question is whether it is too
  // big.  Our policy is as follows:
  //
  // 1) We don't care about wasted space below 4096 bytes (to avoid
  // churn in tiny allocations).
  //
  // 2) If the tag matches, we allow _any_ amount of wasted space.
  //
  // 3) Otherwise we allow up to 50% wasted space.

  if (entry->size < 4096) {
    return true;
  }

  if (entry->tag == tag) {
    return true;
  }

  if (entry->size < size * 2) {
    return true;
  }

  return false;
}

// Find and remove a memory block of the indicated tag, or if that
// does not exist, another memory block with exactly the desired size.
// Returns 0 on success.
static int free_list_find(struct free_list *l, size_t size, const char *tag,
                          size_t *size_out, fl_mem *mem_out) {
  lock_lock(&l->lock);
  int size_match = -1;
  int i;
  int ret = 1;
  for (i = 0; i < l->capacity; i++) {
    if (free_list_acceptable(size, tag, &l->entries[i]) &&
        (size_match < 0 || l->entries[i].size < l->entries[size_match].size)) {
      // If this entry is valid, has sufficient size, and is smaller than the
      // best entry found so far, use this entry.
      size_match = i;
    }
  }

  if (size_match >= 0) {
    l->entries[size_match].valid = 0;
    *size_out = l->entries[size_match].size;
    *mem_out = l->entries[size_match].mem;
    l->used--;
    ret = 0;
  }
  lock_unlock(&l->lock);
  return ret;
}

// Remove the first block in the free list.  Returns 0 if a block was
// removed, and nonzero if the free list was already empty.
static int free_list_first(struct free_list *l, fl_mem *mem_out) {
  lock_lock(&l->lock);
  int ret = 1;
  for (int i = 0; i < l->capacity; i++) {
    if (l->entries[i].valid) {
      l->entries[i].valid = 0;
      *mem_out = l->entries[i].mem;
      l->used--;
      ret = 0;
      break;
    }
  }
  lock_unlock(&l->lock);
  return ret;
}

// End of free_list.h.
// Start of event_list.h

typedef int (*event_report_fn)(struct str_builder*, void*);

// A collection of key-value associations. Used to associate extra data with
// events.
struct kvs {
  // A buffer that contains all value data. Must be freed when the struct kvs is
  // no longer used.
  char *buf;

  // Size of buf in bytes.
  size_t buf_size;

  // Number of bytes used in buf.
  size_t buf_used;

  // Number of associations stored.
  size_t n;

  // Capacity of vals.
  size_t vals_capacity;

  // An array of keys.
  const char* *keys;

  // Indexes into 'buf' that contains the values as zero-terminated strings.
  size_t *vals;
};

static const size_t KVS_INIT_BUF_SIZE = 128;
static const size_t KVS_INIT_NUMKEYS = 8;

void kvs_init(struct kvs* kvs) {
  kvs->buf = malloc(KVS_INIT_BUF_SIZE);
  kvs->buf_size = KVS_INIT_BUF_SIZE;
  kvs->buf_used = 0;
  kvs->vals_capacity = KVS_INIT_NUMKEYS;
  kvs->keys = calloc(kvs->vals_capacity, sizeof(const char*));
  kvs->vals = calloc(kvs->vals_capacity, sizeof(size_t));
  kvs->n = 0;
}

struct kvs* kvs_new(void) {
  struct kvs *kvs = malloc(sizeof(struct kvs));
  kvs_init(kvs);
  return kvs;
}

void kvs_printf(struct kvs* kvs, const char* key, const char* fmt, ...) {
  va_list vl;
  va_start(vl, fmt);

  size_t needed = 1 + (size_t)vsnprintf(NULL, 0, fmt, vl);

  while (kvs->buf_used+needed > kvs->buf_size) {
    kvs->buf_size *= 2;
    kvs->buf = realloc(kvs->buf, kvs->buf_size * sizeof(const char*));
  }

  if (kvs->n == kvs->vals_capacity) {
    kvs->vals_capacity *= 2;
    kvs->vals = realloc(kvs->vals, kvs->vals_capacity * sizeof(size_t));
    kvs->keys = realloc(kvs->keys, kvs->vals_capacity * sizeof(char*));
  }

  kvs->keys[kvs->n] = key;
  kvs->vals[kvs->n] = kvs->buf_used;
  kvs->buf_used += needed;

  va_start(vl, fmt); // Must re-init.
  vsnprintf(&kvs->buf[kvs->vals[kvs->n]], needed, fmt, vl);

  kvs->n++;
}

void kvs_free(struct kvs* kvs) {
  free(kvs->vals);
  free(kvs->keys);
  free(kvs->buf);
}

// Assumes all of the values are valid JSON objects.
void kvs_json(const struct kvs* kvs, struct str_builder *sb) {
  str_builder_char(sb, '{');
  for (size_t i = 0; i < kvs->n; i++) {
    if (i != 0) {
      str_builder_str(sb, ",");
    }
    str_builder_json_str(sb, kvs->keys[i]);
    str_builder_str(sb, ":");
    str_builder_str(sb, &kvs->buf[kvs->vals[i]]);
  }
  str_builder_char(sb, '}');
}

void kvs_log(const struct kvs* kvs, const char* prefix, FILE* f) {
  for (size_t i = 0; i < kvs->n; i++) {
    fprintf(f, "%s%s: %s\n",
            prefix,
            kvs->keys[i],
            &kvs->buf[kvs->vals[i]]);
  }
}

struct event {
  void* data;
  event_report_fn f;
  const char* name;
  const char *provenance;
  // Key-value information that is also to be printed.
  struct kvs *kvs;
};

struct event_list {
  struct event *events;
  int num_events;
  int capacity;
};

static void event_list_init(struct event_list *l) {
  l->capacity = 100;
  l->num_events = 0;
  l->events = calloc(l->capacity, sizeof(struct event));
}

static void event_list_free(struct event_list *l) {
  free(l->events);
}

static void add_event_to_list(struct event_list *l,
                              const char* name,
                              const char* provenance,
                              struct kvs *kvs,
                              void* data,
                              event_report_fn f) {
  if (l->num_events == l->capacity) {
    l->capacity *= 2;
    l->events = realloc(l->events, l->capacity * sizeof(struct event));
  }
  l->events[l->num_events].name = name;
  l->events[l->num_events].provenance =
    provenance ? provenance : "unknown";
  l->events[l->num_events].kvs = kvs;
  l->events[l->num_events].data = data;
  l->events[l->num_events].f = f;
  l->num_events++;
}

static int report_events_in_list(struct event_list *l,
                                 struct str_builder* sb) {
  int ret = 0;
  for (int i = 0; i < l->num_events; i++) {
    if (i != 0) {
      str_builder_str(sb, ",");
    }
    str_builder_str(sb, "{\"name\":");
    str_builder_json_str(sb, l->events[i].name);
    str_builder_str(sb, ",\"provenance\":");
    str_builder_json_str(sb, l->events[i].provenance);
    if (l->events[i].f(sb, l->events[i].data) != 0) {
      ret = 1;
      break;
    }

    str_builder_str(sb, ",\"details\":");
    if (l->events[i].kvs) {
      kvs_json(l->events[i].kvs, sb);
      kvs_free(l->events[i].kvs);
    } else {
      str_builder_str(sb, "{}");
    }

    str_builder(sb, "}");
  }
  event_list_free(l);
  event_list_init(l);
  return ret;
}

// End of event_list.h
#include <getopt.h>
#include <ctype.h>
#include <inttypes.h>
#include <unistd.h>
// Start of values.h.

//// Text I/O

typedef int (*writer)(FILE*, const void*);
typedef int (*bin_reader)(void*);
typedef int (*str_reader)(const char *, void*);

struct array_reader {
  char* elems;
  int64_t n_elems_space;
  int64_t elem_size;
  int64_t n_elems_used;
  int64_t *shape;
  str_reader elem_reader;
};

static void skipspaces(FILE *f) {
  int c;
  do {
    c = getc(f);
  } while (isspace(c));

  if (c != EOF) {
    ungetc(c, f);
  }
}

static int constituent(char c) {
  return isalnum(c) || c == '.' || c == '-' || c == '+' || c == '_';
}

// Produces an empty token only on EOF.
static void next_token(FILE *f, char *buf, int bufsize) {
 start:
  skipspaces(f);

  int i = 0;
  while (i < bufsize) {
    int c = getc(f);
    buf[i] = (char)c;

    if (c == EOF) {
      buf[i] = 0;
      return;
    } else if (c == '-' && i == 1 && buf[0] == '-') {
      // Line comment, so skip to end of line and start over.
      for (; c != '\n' && c != EOF; c = getc(f));
      goto start;
    } else if (!constituent((char)c)) {
      if (i == 0) {
        // We permit single-character tokens that are not
        // constituents; this lets things like ']' and ',' be
        // tokens.
        buf[i+1] = 0;
        return;
      } else {
        ungetc(c, f);
        buf[i] = 0;
        return;
      }
    }

    i++;
  }

  buf[bufsize-1] = 0;
}

static int next_token_is(FILE *f, char *buf, int bufsize, const char* expected) {
  next_token(f, buf, bufsize);
  return strcmp(buf, expected) == 0;
}

static void remove_underscores(char *buf) {
  char *w = buf;

  for (char *r = buf; *r; r++) {
    if (*r != '_') {
      *w++ = *r;
    }
  }

  *w++ = 0;
}

static int read_str_elem(char *buf, struct array_reader *reader) {
  int ret;
  if (reader->n_elems_used == reader->n_elems_space) {
    reader->n_elems_space *= 2;
    reader->elems = (char*) realloc(reader->elems,
                                    (size_t)(reader->n_elems_space * reader->elem_size));
  }

  ret = reader->elem_reader(buf, reader->elems + reader->n_elems_used * reader->elem_size);

  if (ret == 0) {
    reader->n_elems_used++;
  }

  return ret;
}

static int read_str_array_elems(FILE *f,
                                char *buf, int bufsize,
                                struct array_reader *reader, int64_t dims) {
  int ret = 1;
  int expect_elem = 1;
  char *knows_dimsize = (char*) calloc((size_t)dims, sizeof(char));
  int cur_dim = (int)dims-1;
  int64_t *elems_read_in_dim = (int64_t*) calloc((size_t)dims, sizeof(int64_t));

  while (1) {
    next_token(f, buf, bufsize);
    if (strcmp(buf, "]") == 0) {
      expect_elem = 0;
      if (knows_dimsize[cur_dim]) {
        if (reader->shape[cur_dim] != elems_read_in_dim[cur_dim]) {
          ret = 1;
          break;
        }
      } else {
        knows_dimsize[cur_dim] = 1;
        reader->shape[cur_dim] = elems_read_in_dim[cur_dim];
      }
      if (cur_dim == 0) {
        ret = 0;
        break;
      } else {
        cur_dim--;
        elems_read_in_dim[cur_dim]++;
      }
    } else if (!expect_elem && strcmp(buf, ",") == 0) {
      expect_elem = 1;
    } else if (expect_elem) {
      if (strcmp(buf, "[") == 0) {
        if (cur_dim == dims - 1) {
          ret = 1;
          break;
        }
        cur_dim++;
        elems_read_in_dim[cur_dim] = 0;
      } else if (cur_dim == dims - 1) {
        ret = read_str_elem(buf, reader);
        if (ret != 0) {
          break;
        }
        expect_elem = 0;
        elems_read_in_dim[cur_dim]++;
      } else {
        ret = 1;
        break;
      }
    } else {
      ret = 1;
      break;
    }
  }

  free(knows_dimsize);
  free(elems_read_in_dim);
  return ret;
}

static int read_str_empty_array(FILE *f, char *buf, int bufsize,
                                const char *type_name, int64_t *shape, int64_t dims) {
  if (strlen(buf) == 0) {
    // EOF
    return 1;
  }

  if (strcmp(buf, "empty") != 0) {
    return 1;
  }

  if (!next_token_is(f, buf, bufsize, "(")) {
    return 1;
  }

  for (int i = 0; i < dims; i++) {
    if (!next_token_is(f, buf, bufsize, "[")) {
      return 1;
    }

    next_token(f, buf, bufsize);

    if (sscanf(buf, "%"SCNu64, (uint64_t*)&shape[i]) != 1) {
      return 1;
    }

    if (!next_token_is(f, buf, bufsize, "]")) {
      return 1;
    }
  }

  if (!next_token_is(f, buf, bufsize, type_name)) {
    return 1;
  }


  if (!next_token_is(f, buf, bufsize, ")")) {
    return 1;
  }

  // Check whether the array really is empty.
  for (int i = 0; i < dims; i++) {
    if (shape[i] == 0) {
      return 0;
    }
  }

  // Not an empty array!
  return 1;
}

static int read_str_array(FILE *f,
                          int64_t elem_size, str_reader elem_reader,
                          const char *type_name,
                          void **data, int64_t *shape, int64_t dims) {
  int ret;
  struct array_reader reader;
  char buf[100];

  int dims_seen;
  for (dims_seen = 0; dims_seen < dims; dims_seen++) {
    if (!next_token_is(f, buf, sizeof(buf), "[")) {
      break;
    }
  }

  if (dims_seen == 0) {
    return read_str_empty_array(f, buf, sizeof(buf), type_name, shape, dims);
  }

  if (dims_seen != dims) {
    return 1;
  }

  reader.shape = shape;
  reader.n_elems_used = 0;
  reader.elem_size = elem_size;
  reader.n_elems_space = 16;
  reader.elems = (char*) realloc(*data, (size_t)(elem_size*reader.n_elems_space));
  reader.elem_reader = elem_reader;

  ret = read_str_array_elems(f, buf, sizeof(buf), &reader, dims);

  *data = reader.elems;

  return ret;
}

#define READ_STR(MACRO, PTR, SUFFIX)                                   \
  remove_underscores(buf);                                              \
  int j;                                                                \
  if (sscanf(buf, "%"MACRO"%n", (PTR*)dest, &j) == 1) {                 \
    return !(strcmp(buf+j, "") == 0 || strcmp(buf+j, SUFFIX) == 0);     \
  } else {                                                              \
    return 1;                                                           \
  }

static int read_str_i8(char *buf, void* dest) {
  // Some platforms (WINDOWS) does not support scanf %hhd or its
  // cousin, %SCNi8.  Read into int first to avoid corrupting
  // memory.
  //
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63417
  remove_underscores(buf);
  int j, x;
  if (sscanf(buf, "%i%n", &x, &j) == 1) {
    *(int8_t*)dest = (int8_t)x;
    return !(strcmp(buf+j, "") == 0 || strcmp(buf+j, "i8") == 0);
  } else {
    return 1;
  }
}

static int read_str_u8(char *buf, void* dest) {
  // Some platforms (WINDOWS) does not support scanf %hhd or its
  // cousin, %SCNu8.  Read into int first to avoid corrupting
  // memory.
  //
  // https://gcc.gnu.org/bugzilla/show_bug.cgi?id=63417
  remove_underscores(buf);
  int j, x;
  if (sscanf(buf, "%i%n", &x, &j) == 1) {
    *(uint8_t*)dest = (uint8_t)x;
    return !(strcmp(buf+j, "") == 0 || strcmp(buf+j, "u8") == 0);
  } else {
    return 1;
  }
}

static int read_str_i16(char *buf, void* dest) {
  READ_STR(SCNi16, int16_t, "i16");
}

static int read_str_u16(char *buf, void* dest) {
  READ_STR(SCNi16, int16_t, "u16");
}

static int read_str_i32(char *buf, void* dest) {
  READ_STR(SCNi32, int32_t, "i32");
}

static int read_str_u32(char *buf, void* dest) {
  READ_STR(SCNi32, int32_t, "u32");
}

static int read_str_i64(char *buf, void* dest) {
  READ_STR(SCNi64, int64_t, "i64");
}

static int read_str_u64(char *buf, void* dest) {
  // FIXME: This is not correct, as SCNu64 only permits decimal
  // literals.  However, SCNi64 does not handle very large numbers
  // correctly (it's really for signed numbers, so that's fair).
  READ_STR(SCNu64, uint64_t, "u64");
}

static int read_str_f16(char *buf, void* dest) {
  remove_underscores(buf);
  if (strcmp(buf, "f16.nan") == 0) {
    *(uint16_t*)dest = float2halfbits(NAN);
    return 0;
  } else if (strcmp(buf, "f16.inf") == 0) {
    *(uint16_t*)dest = float2halfbits(INFINITY);
    return 0;
  } else if (strcmp(buf, "-f16.inf") == 0) {
    *(uint16_t*)dest = float2halfbits(-INFINITY);
    return 0;
  } else {
    int j;
    float x;
    if (sscanf(buf, "%f%n", &x, &j) == 1) {
      if (strcmp(buf+j, "") == 0 || strcmp(buf+j, "f16") == 0) {
        *(uint16_t*)dest = float2halfbits(x);
        return 0;
      }
    }
    return 1;
  }
}

static int read_str_f32(char *buf, void* dest) {
  remove_underscores(buf);
  if (strcmp(buf, "f32.nan") == 0) {
    *(float*)dest = (float)NAN;
    return 0;
  } else if (strcmp(buf, "f32.inf") == 0) {
    *(float*)dest = (float)INFINITY;
    return 0;
  } else if (strcmp(buf, "-f32.inf") == 0) {
    *(float*)dest = (float)-INFINITY;
    return 0;
  } else {
    READ_STR("f", float, "f32");
  }
}

static int read_str_f64(char *buf, void* dest) {
  remove_underscores(buf);
  if (strcmp(buf, "f64.nan") == 0) {
    *(double*)dest = (double)NAN;
    return 0;
  } else if (strcmp(buf, "f64.inf") == 0) {
    *(double*)dest = (double)INFINITY;
    return 0;
  } else if (strcmp(buf, "-f64.inf") == 0) {
    *(double*)dest = (double)-INFINITY;
    return 0;
  } else {
    READ_STR("lf", double, "f64");
  }
}

static int read_str_bool(char *buf, void* dest) {
  if (strcmp(buf, "true") == 0) {
    *(char*)dest = 1;
    return 0;
  } else if (strcmp(buf, "false") == 0) {
    *(char*)dest = 0;
    return 0;
  } else {
    return 1;
  }
}

static int write_str_i8(FILE *out, int8_t *src) {
  return fprintf(out, "%hhdi8", *src);
}

static int write_str_u8(FILE *out, uint8_t *src) {
  return fprintf(out, "%hhuu8", *src);
}

static int write_str_i16(FILE *out, int16_t *src) {
  return fprintf(out, "%hdi16", *src);
}

static int write_str_u16(FILE *out, uint16_t *src) {
  return fprintf(out, "%huu16", *src);
}

static int write_str_i32(FILE *out, int32_t *src) {
  return fprintf(out, "%di32", *src);
}

static int write_str_u32(FILE *out, uint32_t *src) {
  return fprintf(out, "%uu32", *src);
}

static int write_str_i64(FILE *out, int64_t *src) {
  return fprintf(out, "%"PRIi64"i64", *src);
}

static int write_str_u64(FILE *out, uint64_t *src) {
  return fprintf(out, "%"PRIu64"u64", *src);
}

static int write_str_f16(FILE *out, uint16_t *src) {
  float x = halfbits2float(*src);
  if (isnan(x)) {
    return fprintf(out, "f16.nan");
  } else if (isinf(x) && x >= 0) {
    return fprintf(out, "f16.inf");
  } else if (isinf(x)) {
    return fprintf(out, "-f16.inf");
  } else {
    return fprintf(out, "%.*ff16", FLT_DIG, x);
  }
}

static int write_str_f32(FILE *out, float *src) {
  float x = *src;
  if (isnan(x)) {
    return fprintf(out, "f32.nan");
  } else if (isinf(x) && x >= 0) {
    return fprintf(out, "f32.inf");
  } else if (isinf(x)) {
    return fprintf(out, "-f32.inf");
  } else {
    return fprintf(out, "%.*ff32", FLT_DIG, x);
  }
}

static int write_str_f64(FILE *out, double *src) {
  double x = *src;
  if (isnan(x)) {
    return fprintf(out, "f64.nan");
  } else if (isinf(x) && x >= 0) {
    return fprintf(out, "f64.inf");
  } else if (isinf(x)) {
    return fprintf(out, "-f64.inf");
  } else {
    return fprintf(out, "%.*ff64", DBL_DIG, x);
  }
}

static int write_str_bool(FILE *out, void *src) {
  return fprintf(out, *(char*)src ? "true" : "false");
}

//// Binary I/O

#define BINARY_FORMAT_VERSION 2
#define IS_BIG_ENDIAN (!*(unsigned char *)&(uint16_t){1})

static void flip_bytes(size_t elem_size, unsigned char *elem) {
  for (size_t j=0; j<elem_size/2; j++) {
    unsigned char head = elem[j];
    size_t tail_index = elem_size-1-j;
    elem[j] = elem[tail_index];
    elem[tail_index] = head;
  }
}

// On Windows we need to explicitly set the file mode to not mangle
// newline characters.  On *nix there is no difference.
#ifdef _WIN32
#include <io.h>
#include <fcntl.h>
static void set_binary_mode(FILE *f) {
  setmode(fileno(f), O_BINARY);
}
#else
static void set_binary_mode(FILE *f) {
  (void)f;
}
#endif

static int read_byte(FILE *f, void* dest) {
  size_t num_elems_read = fread(dest, 1, 1, f);
  return num_elems_read == 1 ? 0 : 1;
}

//// Types

struct primtype_info_t {
  const char binname[4]; // Used for parsing binary data.
  const char* type_name; // Same name as in Futhark.
  const int64_t size; // in bytes
  const writer write_str; // Write in text format.
  const str_reader read_str; // Read in text format.
};

static const struct primtype_info_t i8_info =
  {.binname = "  i8", .type_name = "i8",   .size = 1,
   .write_str = (writer)write_str_i8, .read_str = (str_reader)read_str_i8};
static const struct primtype_info_t i16_info =
  {.binname = " i16", .type_name = "i16",  .size = 2,
   .write_str = (writer)write_str_i16, .read_str = (str_reader)read_str_i16};
static const struct primtype_info_t i32_info =
  {.binname = " i32", .type_name = "i32",  .size = 4,
   .write_str = (writer)write_str_i32, .read_str = (str_reader)read_str_i32};
static const struct primtype_info_t i64_info =
  {.binname = " i64", .type_name = "i64",  .size = 8,
   .write_str = (writer)write_str_i64, .read_str = (str_reader)read_str_i64};
static const struct primtype_info_t u8_info =
  {.binname = "  u8", .type_name = "u8",   .size = 1,
   .write_str = (writer)write_str_u8, .read_str = (str_reader)read_str_u8};
static const struct primtype_info_t u16_info =
  {.binname = " u16", .type_name = "u16",  .size = 2,
   .write_str = (writer)write_str_u16, .read_str = (str_reader)read_str_u16};
static const struct primtype_info_t u32_info =
  {.binname = " u32", .type_name = "u32",  .size = 4,
   .write_str = (writer)write_str_u32, .read_str = (str_reader)read_str_u32};
static const struct primtype_info_t u64_info =
  {.binname = " u64", .type_name = "u64",  .size = 8,
   .write_str = (writer)write_str_u64, .read_str = (str_reader)read_str_u64};
static const struct primtype_info_t f16_info =
  {.binname = " f16", .type_name = "f16",  .size = 2,
   .write_str = (writer)write_str_f16, .read_str = (str_reader)read_str_f16};
static const struct primtype_info_t f32_info =
  {.binname = " f32", .type_name = "f32",  .size = 4,
   .write_str = (writer)write_str_f32, .read_str = (str_reader)read_str_f32};
static const struct primtype_info_t f64_info =
  {.binname = " f64", .type_name = "f64",  .size = 8,
   .write_str = (writer)write_str_f64, .read_str = (str_reader)read_str_f64};
static const struct primtype_info_t bool_info =
  {.binname = "bool", .type_name = "bool", .size = 1,
   .write_str = (writer)write_str_bool, .read_str = (str_reader)read_str_bool};

static const struct primtype_info_t* primtypes[] = {
  &i8_info, &i16_info, &i32_info, &i64_info,
  &u8_info, &u16_info, &u32_info, &u64_info,
  &f16_info, &f32_info, &f64_info,
  &bool_info,
  NULL // NULL-terminated
};

// General value interface.  All endian business taken care of at
// lower layers.

static int read_is_binary(FILE *f) {
  skipspaces(f);
  int c = getc(f);
  if (c == 'b') {
    int8_t bin_version;
    int ret = read_byte(f, &bin_version);

    if (ret != 0) { futhark_panic(1, "binary-input: could not read version.\n"); }

    if (bin_version != BINARY_FORMAT_VERSION) {
      futhark_panic(1, "binary-input: File uses version %i, but I only understand version %i.\n",
            bin_version, BINARY_FORMAT_VERSION);
    }

    return 1;
  }
  ungetc(c, f);
  return 0;
}

static const struct primtype_info_t* read_bin_read_type_enum(FILE *f) {
  char read_binname[4];

  int num_matched = fscanf(f, "%4c", read_binname);
  if (num_matched != 1) { futhark_panic(1, "binary-input: Couldn't read element type.\n"); }

  const struct primtype_info_t **type = primtypes;

  for (; *type != NULL; type++) {
    // I compare the 4 characters manually instead of using strncmp because
    // this allows any value to be used, also NULL bytes
    if (memcmp(read_binname, (*type)->binname, 4) == 0) {
      return *type;
    }
  }
  futhark_panic(1, "binary-input: Did not recognize the type '%s'.\n", read_binname);
  return NULL;
}

static void read_bin_ensure_scalar(FILE *f, const struct primtype_info_t *expected_type) {
  int8_t bin_dims;
  int ret = read_byte(f, &bin_dims);
  if (ret != 0) { futhark_panic(1, "binary-input: Couldn't get dims.\n"); }

  if (bin_dims != 0) {
    futhark_panic(1, "binary-input: Expected scalar (0 dimensions), but got array with %i dimensions.\n",
          bin_dims);
  }

  const struct primtype_info_t *bin_type = read_bin_read_type_enum(f);
  if (bin_type != expected_type) {
    futhark_panic(1, "binary-input: Expected scalar of type %s but got scalar of type %s.\n",
          expected_type->type_name,
          bin_type->type_name);
  }
}

//// High-level interface

static int read_bin_array(FILE *f,
                          const struct primtype_info_t *expected_type, void **data, int64_t *shape, int64_t dims) {
  int ret;

  int8_t bin_dims;
  ret = read_byte(f, &bin_dims);
  if (ret != 0) { futhark_panic(1, "binary-input: Couldn't get dims.\n"); }

  if (bin_dims != dims) {
    futhark_panic(1, "binary-input: Expected %i dimensions, but got array with %i dimensions.\n",
          dims, bin_dims);
  }

  const struct primtype_info_t *bin_primtype = read_bin_read_type_enum(f);
  if (expected_type != bin_primtype) {
    futhark_panic(1, "binary-input: Expected %iD-array with element type '%s' but got %iD-array with element type '%s'.\n",
          dims, expected_type->type_name, dims, bin_primtype->type_name);
  }

  int64_t elem_count = 1;
  for (int i=0; i<dims; i++) {
    int64_t bin_shape;
    ret = (int)fread(&bin_shape, sizeof(bin_shape), 1, f);
    if (ret != 1) {
      futhark_panic(1, "binary-input: Couldn't read size for dimension %i of array.\n", i);
    }
    if (IS_BIG_ENDIAN) {
      flip_bytes(sizeof(bin_shape), (unsigned char*) &bin_shape);
    }
    elem_count *= bin_shape;
    shape[i] = bin_shape;
  }

  int64_t elem_size = expected_type->size;
  void* tmp = realloc(*data, (size_t)(elem_count * elem_size));
  if (tmp == NULL) {
    futhark_panic(1, "binary-input: Failed to allocate array of size %i.\n",
          elem_count * elem_size);
  }
  *data = tmp;

  int64_t num_elems_read = (int64_t)fread(*data, (size_t)elem_size, (size_t)elem_count, f);
  if (num_elems_read != elem_count) {
    futhark_panic(1, "binary-input: tried to read %i elements of an array, but only got %i elements.\n",
          elem_count, num_elems_read);
  }

  // If we're on big endian platform we must change all multibyte elements
  // from using little endian to big endian
  if (IS_BIG_ENDIAN && elem_size != 1) {
    flip_bytes((size_t)elem_size, (unsigned char*) *data);
  }

  return 0;
}

static int read_array(FILE *f, const struct primtype_info_t *expected_type, void **data, int64_t *shape, int64_t dims) {
  if (!read_is_binary(f)) {
    return read_str_array(f, expected_type->size, (str_reader)expected_type->read_str, expected_type->type_name, data, shape, dims);
  } else {
    return read_bin_array(f, expected_type, data, shape, dims);
  }
}

static int end_of_input(FILE *f) {
  skipspaces(f);
  char token[2];
  next_token(f, token, sizeof(token));
  if (strcmp(token, "") == 0) {
    return 0;
  } else {
    return 1;
  }
}

static int write_str_array(FILE *out,
                           const struct primtype_info_t *elem_type,
                           const unsigned char *data,
                           const int64_t *shape,
                           int8_t rank) {
  if (rank==0) {
    elem_type->write_str(out, (const void*)data);
  } else {
    int64_t len = (int64_t)shape[0];
    int64_t slice_size = 1;

    int64_t elem_size = elem_type->size;
    for (int8_t i = 1; i < rank; i++) {
      slice_size *= shape[i];
    }

    if (len*slice_size == 0) {
      fprintf(out, "empty(");
      for (int64_t i = 0; i < rank; i++) {
        fprintf(out, "[%"PRIi64"]", shape[i]);
      }
      fprintf(out, "%s", elem_type->type_name);
      fprintf(out, ")");
    } else if (rank==1) {
      fputc('[', out);
      for (int64_t i = 0; i < len; i++) {
        elem_type->write_str(out, (const void*) (data + i * elem_size));
        if (i != len-1) {
          fprintf(out, ", ");
        }
      }
      fputc(']', out);
    } else {
      fputc('[', out);
      for (int64_t i = 0; i < len; i++) {
        write_str_array(out, elem_type, data + i * slice_size * elem_size, shape+1, rank-1);
        if (i != len-1) {
          fprintf(out, ", ");
        }
      }
      fputc(']', out);
    }
  }
  return 0;
}

static int write_bin_array(FILE *out,
                           const struct primtype_info_t *elem_type,
                           const unsigned char *data,
                           const int64_t *shape,
                           int8_t rank) {
  int64_t num_elems = 1;
  for (int64_t i = 0; i < rank; i++) {
    num_elems *= shape[i];
  }

  fputc('b', out);
  fputc((char)BINARY_FORMAT_VERSION, out);
  fwrite(&rank, sizeof(int8_t), 1, out);
  fwrite(elem_type->binname, 4, 1, out);
  if (shape != NULL) {
    fwrite(shape, sizeof(int64_t), (size_t)rank, out);
  }

  if (IS_BIG_ENDIAN) {
    for (int64_t i = 0; i < num_elems; i++) {
      const unsigned char *elem = data+i*elem_type->size;
      for (int64_t j = 0; j < elem_type->size; j++) {
        fwrite(&elem[elem_type->size-j], 1, 1, out);
      }
    }
  } else {
    fwrite(data, (size_t)elem_type->size, (size_t)num_elems, out);
  }

  return 0;
}

static int write_array(FILE *out, int write_binary,
                       const struct primtype_info_t *elem_type,
                       const void *data,
                       const int64_t *shape,
                       const int8_t rank) {
  if (write_binary) {
    return write_bin_array(out, elem_type, data, shape, rank);
  } else {
    return write_str_array(out, elem_type, data, shape, rank);
  }
}

static int read_scalar(FILE *f,
                       const struct primtype_info_t *expected_type, void *dest) {
  if (!read_is_binary(f)) {
    char buf[100];
    next_token(f, buf, sizeof(buf));
    return expected_type->read_str(buf, dest);
  } else {
    read_bin_ensure_scalar(f, expected_type);
    size_t elem_size = (size_t)expected_type->size;
    size_t num_elems_read = fread(dest, elem_size, 1, f);
    if (IS_BIG_ENDIAN) {
      flip_bytes(elem_size, (unsigned char*) dest);
    }
    return num_elems_read == 1 ? 0 : 1;
  }
}

static int write_scalar(FILE *out, int write_binary, const struct primtype_info_t *type, void *src) {
  if (write_binary) {
    return write_bin_array(out, type, src, NULL, 0);
  } else {
    return type->write_str(out, src);
  }
}

// End of values.h.

static int binary_output = 0;
static int print_result = 1;
static FILE *runtime_file;
static int perform_warmup = 0;
static int num_runs = 1;
static const char *entry_point = "main";
// Start of tuning.h.


int is_blank_line_or_comment(const char *s) {
  size_t i = strspn(s, " \t\n");
  return s[i] == '\0' || // Line is blank.
         strncmp(s + i, "--", 2) == 0; // Line is comment.
}

static char* load_tuning_file(const char *fname,
                              void *cfg,
                              int (*set_tuning_param)(void*, const char*, size_t)) {
  const int max_line_len = 1024;
  char* line = (char*) malloc(max_line_len);

  FILE *f = fopen(fname, "r");

  if (f == NULL) {
    snprintf(line, max_line_len, "Cannot open file: %s", strerror(errno));
    return line;
  }

  int lineno = 0;
  while (fgets(line, max_line_len, f) != NULL) {
    lineno++;
    if (is_blank_line_or_comment(line)) {
      continue;
    }
    char *eql = strstr(line, "=");
    if (eql) {
      *eql = 0;
      char *endptr;
      int value = strtol(eql+1, &endptr, 10);
      if (*endptr && *endptr != '\n') {
        snprintf(line, max_line_len, "Invalid line %d (must be of form 'name=int').",
                 lineno);
        return line;
      }
      if (set_tuning_param(cfg, line, (size_t)value) != 0) {
        char* err = (char*) malloc(max_line_len + 50);
        snprintf(err, max_line_len + 50, "Unknown name '%s' on line %d.", line, lineno);
        free(line);
        return err;
      }
    } else {
      snprintf(line, max_line_len, "Invalid line %d (must be of form 'name=int').",
               lineno);
      return line;
    }
  }

  free(line);

  return NULL;
}

// End of tuning.h.

int parse_options(struct futhark_context_config *cfg, int argc, char *const argv[])
{
    int ch;
    static struct option long_options[] = {{"write-runtime-to", required_argument, NULL, 1}, {"runs", required_argument, NULL, 2}, {"debugging", no_argument, NULL, 3}, {"log", no_argument, NULL, 4}, {"profile", no_argument, NULL, 5}, {"entry-point", required_argument, NULL, 6}, {"binary-output", no_argument, NULL, 7}, {"no-print-result", no_argument, NULL, 8}, {"help", no_argument, NULL, 9}, {"print-params", no_argument, NULL, 10}, {"param", required_argument, NULL, 11}, {"tuning", required_argument, NULL, 12}, {"cache-file", required_argument, NULL, 13}, {0, 0, 0, 0}};
    static char *option_descriptions = "  -t/--write-runtime-to FILE Print the time taken to execute the program to the indicated file, an integral number of microseconds.\n  -r/--runs INT              Perform NUM runs of the program.\n  -D/--debugging             Perform possibly expensive internal correctness checks and verbose logging.\n  -L/--log                   Print various low-overhead logging information to stderr while running.\n  -P/--profile               Enable the collection of profiling information.\n  -e/--entry-point NAME      The entry point to run. Defaults to main.\n  -b/--binary-output         Print the program result in the binary output format.\n  -n/--no-print-result       Do not print the program result.\n  -h/--help                  Print help information and exit.\n  --print-params             Print all tuning parameters that can be set with --param or --tuning.\n  --param ASSIGNMENT         Set a tuning parameter to the given value.\n  --tuning FILE              Read size=value assignments from the given file.\n  --cache-file FILE          Store program cache here.\n";
    
    while ((ch = getopt_long(argc, argv, ":t:r:DLPe:bnh", long_options, NULL)) != -1) {
        if (ch == 1 || ch == 't') {
            runtime_file = fopen(optarg, "w");
            if (runtime_file == NULL)
                futhark_panic(1, "Cannot open %s: %s\n", optarg, strerror(errno));
        }
        if (ch == 2 || ch == 'r') {
            num_runs = atoi(optarg);
            perform_warmup = 1;
            if (num_runs <= 0)
                futhark_panic(1, "Need a positive number of runs, not %s\n", optarg);
        }
        if (ch == 3 || ch == 'D')
            futhark_context_config_set_debugging(cfg, 1);
        if (ch == 4 || ch == 'L')
            futhark_context_config_set_logging(cfg, 1);
        if (ch == 5 || ch == 'P')
            futhark_context_config_set_profiling(cfg, 1);
        if (ch == 6 || ch == 'e') {
            if (entry_point != NULL)
                entry_point = optarg;
        }
        if (ch == 7 || ch == 'b')
            binary_output = 1;
        if (ch == 8 || ch == 'n')
            print_result = 0;
        if (ch == 9 || ch == 'h') {
            printf("Usage: %s [OPTION]...\nOptions:\n\n%s\nFor more information, consult the Futhark User's Guide or the man pages.\n", fut_progname, option_descriptions);
            exit(0);
        }
        if (ch == 10) {
            int n = futhark_get_tuning_param_count();
            
            for (int i = 0; i < n; i++)
                printf("%s (%s)\n", futhark_get_tuning_param_name(i), futhark_get_tuning_param_class(i));
            exit(0);
        }
        if (ch == 11) {
            char *name = optarg;
            char *equals = strstr(optarg, "=");
            char *value_str = equals != NULL ? equals + 1 : optarg;
            int value = atoi(value_str);
            
            if (equals != NULL) {
                *equals = 0;
                if (futhark_context_config_set_tuning_param(cfg, name, (size_t) value) != 0)
                    futhark_panic(1, "Unknown size: %s\n", name);
            } else
                futhark_panic(1, "Invalid argument for size option: %s\n", optarg);
        }
        if (ch == 12) {
            char *ret = load_tuning_file(optarg, cfg, (int (*)(void *, const char *, size_t)) futhark_context_config_set_tuning_param);
            
            if (ret != NULL)
                futhark_panic(1, "When loading tuning file '%s': %s\n", optarg, ret);
        }
        if (ch == 13)
            futhark_context_config_set_cache_file(cfg, optarg);
        if (ch == ':')
            futhark_panic(-1, "Missing argument for option %s\n", argv[optind - 1]);
        if (ch == '?') {
            fprintf(stderr, "Usage: %s [OPTIONS]...\nOptions:\n\n%s\n", fut_progname, "  -t/--write-runtime-to FILE Print the time taken to execute the program to the indicated file, an integral number of microseconds.\n  -r/--runs INT              Perform NUM runs of the program.\n  -D/--debugging             Perform possibly expensive internal correctness checks and verbose logging.\n  -L/--log                   Print various low-overhead logging information to stderr while running.\n  -P/--profile               Enable the collection of profiling information.\n  -e/--entry-point NAME      The entry point to run. Defaults to main.\n  -b/--binary-output         Print the program result in the binary output format.\n  -n/--no-print-result       Do not print the program result.\n  -h/--help                  Print help information and exit.\n  --print-params             Print all tuning parameters that can be set with --param or --tuning.\n  --param ASSIGNMENT         Set a tuning parameter to the given value.\n  --tuning FILE              Read size=value assignments from the given file.\n  --cache-file FILE          Store program cache here.\n");
            futhark_panic(1, "Unknown option: %s\n", argv[optind - 1]);
        }
    }
    return optind;
}
static int futrts_cli_entry_graph_orientation_solve(struct futhark_context *ctx)
{
    int64_t t_start, t_end;
    int time_runs = 0, profile_run = 0;
    int retval = 0;
    
    // We do not want to profile all the initialisation.
    futhark_context_pause_profiling(ctx);
    // Declare and read input.
    set_binary_mode(stdin);
    
    int32_t read_value_0;
    
    if (read_scalar(stdin, &i32_info, &read_value_0) != 0)
        futhark_panic(1, "Error when reading input #%d of type %s (errno: %s).\n", 0, "i32", strerror(errno));
    ;
    
    int32_t read_value_1;
    
    if (read_scalar(stdin, &i32_info, &read_value_1) != 0)
        futhark_panic(1, "Error when reading input #%d of type %s (errno: %s).\n", 1, "i32", strerror(errno));
    ;
    
    int32_t read_value_2;
    
    if (read_scalar(stdin, &i32_info, &read_value_2) != 0)
        futhark_panic(1, "Error when reading input #%d of type %s (errno: %s).\n", 2, "i32", strerror(errno));
    ;
    
    struct futhark_i64_1d * read_value_3;
    int64_t read_shape_3[1];
    int64_t *read_arr_3 = NULL;
    
    errno = 0;
    if (read_array(stdin, &i64_info, (void **) &read_arr_3, read_shape_3, 1) != 0)
        futhark_panic(1, "Cannot read input #%d of type %s (errno: %s).\n", 3, "[]i64", strerror(errno));
    
    struct futhark_i32_1d * read_value_4;
    int64_t read_shape_4[1];
    int32_t *read_arr_4 = NULL;
    
    errno = 0;
    if (read_array(stdin, &i32_info, (void **) &read_arr_4, read_shape_4, 1) != 0)
        futhark_panic(1, "Cannot read input #%d of type %s (errno: %s).\n", 4, "[]i32", strerror(errno));
    
    struct futhark_i32_1d * read_value_5;
    int64_t read_shape_5[1];
    int32_t *read_arr_5 = NULL;
    
    errno = 0;
    if (read_array(stdin, &i32_info, (void **) &read_arr_5, read_shape_5, 1) != 0)
        futhark_panic(1, "Cannot read input #%d of type %s (errno: %s).\n", 5, "[]i32", strerror(errno));
    if (end_of_input(stdin) != 0)
        futhark_panic(1, "Expected EOF on stdin after reading input for \"%s\".\n", "graph_orientation_solve");
    
    bool result_0;
    struct futhark_i64_1d * result_1;
    struct futhark_i64_1d * result_2;
    struct futhark_i64_1d * result_3;
    
    if (perform_warmup) {
        int r;
        
        ;
        ;
        ;
        assert((read_value_3 = futhark_new_i64_1d(ctx, read_arr_3, read_shape_3[0])) != NULL);
        assert((read_value_4 = futhark_new_i32_1d(ctx, read_arr_4, read_shape_4[0])) != NULL);
        assert((read_value_5 = futhark_new_i32_1d(ctx, read_arr_5, read_shape_5[0])) != NULL);
        if (futhark_context_sync(ctx) != 0)
            futhark_panic(1, "%s", futhark_context_get_error(ctx));
        ;
        // Only profile last run.
        if (profile_run)
            futhark_context_unpause_profiling(ctx);
        t_start = get_wall_time();
        r = futhark_entry_graph_orientation_solve(ctx, &result_0, &result_1, &result_2, &result_3, read_value_0, read_value_1, read_value_2, read_value_3, read_value_4, read_value_5);
        if (r != 0)
            futhark_panic(1, "%s", futhark_context_get_error(ctx));
        if (futhark_context_sync(ctx) != 0)
            futhark_panic(1, "%s", futhark_context_get_error(ctx));
        ;
        if (profile_run)
            futhark_context_pause_profiling(ctx);
        t_end = get_wall_time();
        
        long elapsed_usec = t_end - t_start;
        
        if (time_runs && runtime_file != NULL) {
            fprintf(runtime_file, "%lld\n", (long long) elapsed_usec);
            fflush(runtime_file);
        }
        ;
        ;
        ;
        assert(futhark_free_i64_1d(ctx, read_value_3) == 0);
        assert(futhark_free_i32_1d(ctx, read_value_4) == 0);
        assert(futhark_free_i32_1d(ctx, read_value_5) == 0);
        ;
        assert(futhark_free_i64_1d(ctx, result_1) == 0);
        assert(futhark_free_i64_1d(ctx, result_2) == 0);
        assert(futhark_free_i64_1d(ctx, result_3) == 0);
    }
    time_runs = 1;
    // Proper run.
    for (int run = 0; run < num_runs; run++) {
        // Only profile last run.
        profile_run = run == num_runs - 1;
        
        int r;
        
        ;
        ;
        ;
        assert((read_value_3 = futhark_new_i64_1d(ctx, read_arr_3, read_shape_3[0])) != NULL);
        assert((read_value_4 = futhark_new_i32_1d(ctx, read_arr_4, read_shape_4[0])) != NULL);
        assert((read_value_5 = futhark_new_i32_1d(ctx, read_arr_5, read_shape_5[0])) != NULL);
        if (futhark_context_sync(ctx) != 0)
            futhark_panic(1, "%s", futhark_context_get_error(ctx));
        ;
        // Only profile last run.
        if (profile_run)
            futhark_context_unpause_profiling(ctx);
        t_start = get_wall_time();
        r = futhark_entry_graph_orientation_solve(ctx, &result_0, &result_1, &result_2, &result_3, read_value_0, read_value_1, read_value_2, read_value_3, read_value_4, read_value_5);
        if (r != 0)
            futhark_panic(1, "%s", futhark_context_get_error(ctx));
        if (futhark_context_sync(ctx) != 0)
            futhark_panic(1, "%s", futhark_context_get_error(ctx));
        ;
        if (profile_run)
            futhark_context_pause_profiling(ctx);
        t_end = get_wall_time();
        
        long elapsed_usec = t_end - t_start;
        
        if (time_runs && runtime_file != NULL) {
            fprintf(runtime_file, "%lld\n", (long long) elapsed_usec);
            fflush(runtime_file);
        }
        ;
        ;
        ;
        assert(futhark_free_i64_1d(ctx, read_value_3) == 0);
        assert(futhark_free_i32_1d(ctx, read_value_4) == 0);
        assert(futhark_free_i32_1d(ctx, read_value_5) == 0);
        if (run < num_runs - 1) {
            ;
            assert(futhark_free_i64_1d(ctx, result_1) == 0);
            assert(futhark_free_i64_1d(ctx, result_2) == 0);
            assert(futhark_free_i64_1d(ctx, result_3) == 0);
        }
    }
    ;
    ;
    ;
    free(read_arr_3);
    free(read_arr_4);
    free(read_arr_5);
    if (print_result) {
        // Print the final result.
        if (binary_output)
            set_binary_mode(stdout);
        write_scalar(stdout, binary_output, &bool_info, &result_0);
        printf("\n");
        {
            int64_t *arr = calloc(futhark_shape_i64_1d(ctx, result_1)[0], i64_info.size);
            
            assert(arr != NULL);
            assert(futhark_values_i64_1d(ctx, result_1, arr) == 0);
            assert(futhark_context_sync(ctx) == 0);
            write_array(stdout, binary_output, &i64_info, arr, futhark_shape_i64_1d(ctx, result_1), 1);
            free(arr);
        }
        printf("\n");
        {
            int64_t *arr = calloc(futhark_shape_i64_1d(ctx, result_2)[0], i64_info.size);
            
            assert(arr != NULL);
            assert(futhark_values_i64_1d(ctx, result_2, arr) == 0);
            assert(futhark_context_sync(ctx) == 0);
            write_array(stdout, binary_output, &i64_info, arr, futhark_shape_i64_1d(ctx, result_2), 1);
            free(arr);
        }
        printf("\n");
        {
            int64_t *arr = calloc(futhark_shape_i64_1d(ctx, result_3)[0], i64_info.size);
            
            assert(arr != NULL);
            assert(futhark_values_i64_1d(ctx, result_3, arr) == 0);
            assert(futhark_context_sync(ctx) == 0);
            write_array(stdout, binary_output, &i64_info, arr, futhark_shape_i64_1d(ctx, result_3), 1);
            free(arr);
        }
        printf("\n");
    }
    
  print_end:
    { }
    ;
    assert(futhark_free_i64_1d(ctx, result_1) == 0);
    assert(futhark_free_i64_1d(ctx, result_2) == 0);
    assert(futhark_free_i64_1d(ctx, result_3) == 0);
    return retval;
}
typedef int entry_point_fun(struct futhark_context *);
struct entry_point_entry {
    const char *name;
    entry_point_fun *fun;
};
int main(int argc, char **argv)
{
    int retval = 0;
    
    fut_progname = argv[0];
    
    struct futhark_context_config *cfg = futhark_context_config_new();
    
    assert(cfg != NULL);
    
    int parsed_options = parse_options(cfg, argc, argv);
    
    argc -= parsed_options;
    argv += parsed_options;
    if (argc != 0)
        futhark_panic(1, "Excess non-option: %s\n", argv[0]);
    
    struct futhark_context *ctx = futhark_context_new(cfg);
    
    assert(ctx != NULL);
    
    char *error = futhark_context_get_error(ctx);
    
    if (error != NULL)
        futhark_panic(1, "%s", error);
    
    struct entry_point_entry entry_points[] = {{.name ="graph_orientation_solve", .fun =futrts_cli_entry_graph_orientation_solve}};
    
    if (entry_point != NULL) {
        int num_entry_points = sizeof(entry_points) / sizeof(entry_points[0]);
        entry_point_fun *entry_point_fun = NULL;
        
        for (int i = 0; i < num_entry_points; i++) {
            if (strcmp(entry_points[i].name, entry_point) == 0) {
                entry_point_fun = entry_points[i].fun;
                break;
            }
        }
        if (entry_point_fun == NULL) {
            fprintf(stderr, "No entry point '%s'.  Select another with --entry-point.  Options are:\n", entry_point);
            for (int i = 0; i < num_entry_points; i++)
                fprintf(stderr, "%s\n", entry_points[i].name);
            return 1;
        }
        if (isatty(fileno(stdin))) {
            fprintf(stderr, "Reading input from TTY.\n");
            fprintf(stderr, "Send EOF (CTRL-d) after typing all input values.\n");
        }
        retval = entry_point_fun(ctx);
        if (runtime_file != NULL)
            fclose(runtime_file);
    }
    futhark_context_free(ctx);
    futhark_context_config_free(cfg);
    return retval;
}

#ifdef _MSC_VER
#define inline __inline
#endif
#include <string.h>
#include <string.h>
#include <errno.h>
#include <assert.h>
#include <ctype.h>



#define FUTHARK_F64_ENABLED

// Start of scalar.h.

// Implementation of the primitive scalar operations.  Very
// repetitive.  This code is inserted directly into both CUDA and
// OpenCL programs, as well as the CPU code, so it has some #ifdefs to
// work everywhere.  Some operations are defined as macros because
// this allows us to use them as constant expressions in things like
// array sizes and static initialisers.

// Some of the #ifdefs are because OpenCL uses type-generic functions
// for some operations (e.g. sqrt), while C and CUDA sensibly use
// distinct functions for different precisions (e.g. sqrtf() and
// sqrt()).  This is quite annoying.  Due to C's unfortunate casting
// rules, it is also really easy to accidentally implement
// floating-point functions in the wrong precision, so be careful.

// Double-precision definitions are only included if the preprocessor
// macro FUTHARK_F64_ENABLED is set.

#ifndef M_PI
#define M_PI 3.141592653589793
#endif

SCALAR_FUN_ATTR int32_t fptobits_f32_i32(float x);
SCALAR_FUN_ATTR float bitstofp_i32_f32(int32_t x);

SCALAR_FUN_ATTR uint8_t   add8(uint8_t x, uint8_t y)   { return x + y; }
SCALAR_FUN_ATTR uint16_t add16(uint16_t x, uint16_t y) { return x + y; }
SCALAR_FUN_ATTR uint32_t add32(uint32_t x, uint32_t y) { return x + y; }
SCALAR_FUN_ATTR uint64_t add64(uint64_t x, uint64_t y) { return x + y; }

SCALAR_FUN_ATTR uint8_t   sub8(uint8_t x, uint8_t y)   { return x - y; }
SCALAR_FUN_ATTR uint16_t sub16(uint16_t x, uint16_t y) { return x - y; }
SCALAR_FUN_ATTR uint32_t sub32(uint32_t x, uint32_t y) { return x - y; }
SCALAR_FUN_ATTR uint64_t sub64(uint64_t x, uint64_t y) { return x - y; }

SCALAR_FUN_ATTR uint8_t   mul8(uint8_t x, uint8_t y)   { return x * y; }
SCALAR_FUN_ATTR uint16_t mul16(uint16_t x, uint16_t y) { return x * y; }
SCALAR_FUN_ATTR uint32_t mul32(uint32_t x, uint32_t y) { return x * y; }
SCALAR_FUN_ATTR uint64_t mul64(uint64_t x, uint64_t y) { return x * y; }

#if defined(ISPC)

SCALAR_FUN_ATTR uint8_t udiv8(uint8_t x, uint8_t y) {
  // This strange pattern is used to prevent the ISPC compiler from
  // causing SIGFPEs and bogus results on divisions where inactive lanes
  // have 0-valued divisors. It ensures that any inactive lane instead
  // has a divisor of 1. https://github.com/ispc/ispc/issues/2292
  uint8_t ys = 1;
  foreach_active(i) { ys = y; }
  return x / ys;
}

SCALAR_FUN_ATTR uint16_t udiv16(uint16_t x, uint16_t y) {
  uint16_t ys = 1;
  foreach_active(i) { ys = y; }
  return x / ys;
}

SCALAR_FUN_ATTR uint32_t udiv32(uint32_t x, uint32_t y) {
  uint32_t ys = 1;
  foreach_active(i) { ys = y; }
  return x / ys;
}

SCALAR_FUN_ATTR uint64_t udiv64(uint64_t x, uint64_t y) {
  uint64_t ys = 1;
  foreach_active(i) { ys = y; }
  return x / ys;
}

SCALAR_FUN_ATTR uint8_t udiv_up8(uint8_t x, uint8_t y) {
  uint8_t ys = 1;
  foreach_active(i) { ys = y; }
  return (x + y - 1) / ys;
}

SCALAR_FUN_ATTR uint16_t udiv_up16(uint16_t x, uint16_t y) {
  uint16_t ys = 1;
  foreach_active(i) { ys = y; }
  return (x + y - 1) / ys;
}

SCALAR_FUN_ATTR uint32_t udiv_up32(uint32_t x, uint32_t y) {
  uint32_t ys = 1;
  foreach_active(i) { ys = y; }
  return (x + y - 1) / ys;
}

SCALAR_FUN_ATTR uint64_t udiv_up64(uint64_t x, uint64_t y) {
  uint64_t ys = 1;
  foreach_active(i) { ys = y; }
  return (x + y - 1) / ys;
}

SCALAR_FUN_ATTR uint8_t umod8(uint8_t x, uint8_t y) {
  uint8_t ys = 1;
  foreach_active(i) { ys = y; }
  return x % ys;
}

SCALAR_FUN_ATTR uint16_t umod16(uint16_t x, uint16_t y) {
  uint16_t ys = 1;
  foreach_active(i) { ys = y; }
  return x % ys;
}

SCALAR_FUN_ATTR uint32_t umod32(uint32_t x, uint32_t y) {
  uint32_t ys = 1;
  foreach_active(i) { ys = y; }
  return x % ys;
}

SCALAR_FUN_ATTR uint64_t umod64(uint64_t x, uint64_t y) {
  uint64_t ys = 1;
  foreach_active(i) { ys = y; }
  return x % ys;
}

SCALAR_FUN_ATTR uint8_t udiv_safe8(uint8_t x, uint8_t y) {
  uint8_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x / ys;
}

SCALAR_FUN_ATTR uint16_t udiv_safe16(uint16_t x, uint16_t y) {
  uint16_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x / ys;
}

SCALAR_FUN_ATTR uint32_t udiv_safe32(uint32_t x, uint32_t y) {
  uint32_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x / ys;
}

SCALAR_FUN_ATTR uint64_t udiv_safe64(uint64_t x, uint64_t y) {
  uint64_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x / ys;
}

SCALAR_FUN_ATTR uint8_t udiv_up_safe8(uint8_t x, uint8_t y) {
  uint8_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : (x + y - 1) / ys;
}

SCALAR_FUN_ATTR uint16_t udiv_up_safe16(uint16_t x, uint16_t y) {
  uint16_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : (x + y - 1) / ys;
}

SCALAR_FUN_ATTR uint32_t udiv_up_safe32(uint32_t x, uint32_t y) {
  uint32_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : (x + y - 1) / ys;
}

SCALAR_FUN_ATTR uint64_t udiv_up_safe64(uint64_t x, uint64_t y) {
  uint64_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : (x + y - 1) / ys;
}

SCALAR_FUN_ATTR uint8_t umod_safe8(uint8_t x, uint8_t y) {
  uint8_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x % ys;
}

SCALAR_FUN_ATTR uint16_t umod_safe16(uint16_t x, uint16_t y) {
  uint16_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x % ys;
}

SCALAR_FUN_ATTR uint32_t umod_safe32(uint32_t x, uint32_t y) {
  uint32_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x % ys;
}

SCALAR_FUN_ATTR uint64_t umod_safe64(uint64_t x, uint64_t y) {
  uint64_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x % ys;
}

SCALAR_FUN_ATTR int8_t sdiv8(int8_t x, int8_t y) {
  int8_t ys = 1;
  foreach_active(i) { ys = y; }
  int8_t q = x / ys;
  int8_t r = x % ys;
  return q - ((r != 0 && r < 0 != y < 0) ? 1 : 0);
}

SCALAR_FUN_ATTR int16_t sdiv16(int16_t x, int16_t y) {
  int16_t ys = 1;
  foreach_active(i) { ys = y; }
  int16_t q = x / ys;
  int16_t r = x % ys;
  return q - ((r != 0 && r < 0 != y < 0) ? 1 : 0);
}

SCALAR_FUN_ATTR int32_t sdiv32(int32_t x, int32_t y) {
  int32_t ys = 1;
  foreach_active(i) { ys = y; }
  int32_t q = x / ys;
  int32_t r = x % ys;
  return q - ((r != 0 && r < 0 != y < 0) ? 1 : 0);
}

SCALAR_FUN_ATTR int64_t sdiv64(int64_t x, int64_t y) {
  int64_t ys = 1;
  foreach_active(i) { ys = y; }
  int64_t q = x / ys;
  int64_t r = x % ys;
  return q - ((r != 0 && r < 0 != y < 0) ? 1 : 0);
}

SCALAR_FUN_ATTR int8_t sdiv_up8(int8_t x, int8_t y) { return sdiv8(x + y - 1, y); }
SCALAR_FUN_ATTR int16_t sdiv_up16(int16_t x, int16_t y) { return sdiv16(x + y - 1, y); }
SCALAR_FUN_ATTR int32_t sdiv_up32(int32_t x, int32_t y) { return sdiv32(x + y - 1, y); }
SCALAR_FUN_ATTR int64_t sdiv_up64(int64_t x, int64_t y) { return sdiv64(x + y - 1, y); }

SCALAR_FUN_ATTR int8_t smod8(int8_t x, int8_t y) {
  int8_t ys = 1;
  foreach_active(i) { ys = y; }
  int8_t r = x % ys;
  return r + (r == 0 || (x > 0 && y > 0) || (x < 0 && y < 0) ? 0 : y);
}

SCALAR_FUN_ATTR int16_t smod16(int16_t x, int16_t y) {
  int16_t ys = 1;
  foreach_active(i) { ys = y; }
  int16_t r = x % ys;
  return r + (r == 0 || (x > 0 && y > 0) || (x < 0 && y < 0) ? 0 : y);
}

SCALAR_FUN_ATTR int32_t smod32(int32_t x, int32_t y) {
  int32_t ys = 1;
  foreach_active(i) { ys = y; }
  int32_t r = x % ys;
  return r + (r == 0 || (x > 0 && y > 0) || (x < 0 && y < 0) ? 0 : y);
}

SCALAR_FUN_ATTR int64_t smod64(int64_t x, int64_t y) {
  int64_t ys = 1;
  foreach_active(i) { ys = y; }
  int64_t r = x % ys;
  return r + (r == 0 || (x > 0 && y > 0) || (x < 0 && y < 0) ? 0 : y);
}

SCALAR_FUN_ATTR int8_t   sdiv_safe8(int8_t x, int8_t y)   { return y == 0 ? 0 : sdiv8(x, y); }
SCALAR_FUN_ATTR int16_t sdiv_safe16(int16_t x, int16_t y) { return y == 0 ? 0 : sdiv16(x, y); }
SCALAR_FUN_ATTR int32_t sdiv_safe32(int32_t x, int32_t y) { return y == 0 ? 0 : sdiv32(x, y); }
SCALAR_FUN_ATTR int64_t sdiv_safe64(int64_t x, int64_t y) { return y == 0 ? 0 : sdiv64(x, y); }

SCALAR_FUN_ATTR int8_t sdiv_up_safe8(int8_t x, int8_t y)     { return sdiv_safe8(x + y - 1, y); }
SCALAR_FUN_ATTR int16_t sdiv_up_safe16(int16_t x, int16_t y) { return sdiv_safe16(x + y - 1, y); }
SCALAR_FUN_ATTR int32_t sdiv_up_safe32(int32_t x, int32_t y) { return sdiv_safe32(x + y - 1, y); }
SCALAR_FUN_ATTR int64_t sdiv_up_safe64(int64_t x, int64_t y) { return sdiv_safe64(x + y - 1, y); }

SCALAR_FUN_ATTR int8_t   smod_safe8(int8_t x, int8_t y)   { return y == 0 ? 0 : smod8(x, y); }
SCALAR_FUN_ATTR int16_t smod_safe16(int16_t x, int16_t y) { return y == 0 ? 0 : smod16(x, y); }
SCALAR_FUN_ATTR int32_t smod_safe32(int32_t x, int32_t y) { return y == 0 ? 0 : smod32(x, y); }
SCALAR_FUN_ATTR int64_t smod_safe64(int64_t x, int64_t y) { return y == 0 ? 0 : smod64(x, y); }

SCALAR_FUN_ATTR int8_t squot8(int8_t x, int8_t y) {
  int8_t ys = 1;
  foreach_active(i) { ys = y; }
  return x / ys;
}

SCALAR_FUN_ATTR int16_t squot16(int16_t x, int16_t y) {
  int16_t ys = 1;
  foreach_active(i) { ys = y; }
  return x / ys;
}

SCALAR_FUN_ATTR int32_t squot32(int32_t x, int32_t y) {
  int32_t ys = 1;
  foreach_active(i) { ys = y; }
  return x / ys;
}

SCALAR_FUN_ATTR int64_t squot64(int64_t x, int64_t y) {
  int64_t ys = 1;
  foreach_active(i) { ys = y; }
  return x / ys;
}

SCALAR_FUN_ATTR int8_t srem8(int8_t x, int8_t y) {
  int8_t ys = 1;
  foreach_active(i) { ys = y; }
  return x % ys;
}

SCALAR_FUN_ATTR int16_t srem16(int16_t x, int16_t y) {
  int16_t ys = 1;
  foreach_active(i) { ys = y; }
  return x % ys;
}

SCALAR_FUN_ATTR int32_t srem32(int32_t x, int32_t y) {
  int32_t ys = 1;
  foreach_active(i) { ys = y; }
  return x % ys;
}

SCALAR_FUN_ATTR int64_t srem64(int64_t x, int64_t y) {
  int8_t ys = 1;
  foreach_active(i) { ys = y; }
  return x % ys;
}

SCALAR_FUN_ATTR int8_t squot_safe8(int8_t x, int8_t y) {
  int8_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x / ys;
}

SCALAR_FUN_ATTR int16_t squot_safe16(int16_t x, int16_t y) {
  int16_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x / ys;
}

SCALAR_FUN_ATTR int32_t squot_safe32(int32_t x, int32_t y) {
  int32_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x / ys;
}

SCALAR_FUN_ATTR int64_t squot_safe64(int64_t x, int64_t y) {
  int64_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x / ys;
}

SCALAR_FUN_ATTR int8_t srem_safe8(int8_t x, int8_t y) {
  int8_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x % ys;
}

SCALAR_FUN_ATTR int16_t srem_safe16(int16_t x, int16_t y) {
  int16_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x % ys;
}

SCALAR_FUN_ATTR int32_t srem_safe32(int32_t x, int32_t y) {
  int32_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x % ys;
}

SCALAR_FUN_ATTR int64_t srem_safe64(int64_t x, int64_t y) {
  int64_t ys = 1;
  foreach_active(i) { ys = y; }
  return y == 0 ? 0 : x % ys;
}

#else

SCALAR_FUN_ATTR uint8_t   udiv8(uint8_t x, uint8_t y)   { return x / y; }
SCALAR_FUN_ATTR uint16_t udiv16(uint16_t x, uint16_t y) { return x / y; }
SCALAR_FUN_ATTR uint32_t udiv32(uint32_t x, uint32_t y) { return x / y; }
SCALAR_FUN_ATTR uint64_t udiv64(uint64_t x, uint64_t y) { return x / y; }

SCALAR_FUN_ATTR uint8_t   udiv_up8(uint8_t x, uint8_t y)   { return (x + y - 1) / y; }
SCALAR_FUN_ATTR uint16_t udiv_up16(uint16_t x, uint16_t y) { return (x + y - 1) / y; }
SCALAR_FUN_ATTR uint32_t udiv_up32(uint32_t x, uint32_t y) { return (x + y - 1) / y; }
SCALAR_FUN_ATTR uint64_t udiv_up64(uint64_t x, uint64_t y) { return (x + y - 1) / y; }

SCALAR_FUN_ATTR uint8_t   umod8(uint8_t x, uint8_t y)   { return x % y; }
SCALAR_FUN_ATTR uint16_t umod16(uint16_t x, uint16_t y) { return x % y; }
SCALAR_FUN_ATTR uint32_t umod32(uint32_t x, uint32_t y) { return x % y; }
SCALAR_FUN_ATTR uint64_t umod64(uint64_t x, uint64_t y) { return x % y; }

SCALAR_FUN_ATTR uint8_t   udiv_safe8(uint8_t x, uint8_t y)   { return y == 0 ? 0 : x / y; }
SCALAR_FUN_ATTR uint16_t udiv_safe16(uint16_t x, uint16_t y) { return y == 0 ? 0 : x / y; }
SCALAR_FUN_ATTR uint32_t udiv_safe32(uint32_t x, uint32_t y) { return y == 0 ? 0 : x / y; }
SCALAR_FUN_ATTR uint64_t udiv_safe64(uint64_t x, uint64_t y) { return y == 0 ? 0 : x / y; }

SCALAR_FUN_ATTR uint8_t   udiv_up_safe8(uint8_t x, uint8_t y)   { return y == 0 ? 0 : (x + y - 1) / y; }
SCALAR_FUN_ATTR uint16_t udiv_up_safe16(uint16_t x, uint16_t y) { return y == 0 ? 0 : (x + y - 1) / y; }
SCALAR_FUN_ATTR uint32_t udiv_up_safe32(uint32_t x, uint32_t y) { return y == 0 ? 0 : (x + y - 1) / y; }
SCALAR_FUN_ATTR uint64_t udiv_up_safe64(uint64_t x, uint64_t y) { return y == 0 ? 0 : (x + y - 1) / y; }

SCALAR_FUN_ATTR uint8_t   umod_safe8(uint8_t x, uint8_t y)   { return y == 0 ? 0 : x % y; }
SCALAR_FUN_ATTR uint16_t umod_safe16(uint16_t x, uint16_t y) { return y == 0 ? 0 : x % y; }
SCALAR_FUN_ATTR uint32_t umod_safe32(uint32_t x, uint32_t y) { return y == 0 ? 0 : x % y; }
SCALAR_FUN_ATTR uint64_t umod_safe64(uint64_t x, uint64_t y) { return y == 0 ? 0 : x % y; }

SCALAR_FUN_ATTR int8_t sdiv8(int8_t x, int8_t y) {
  int8_t q = x / y;
  int8_t r = x % y;
  return q - ((r != 0 && r < 0 != y < 0) ? 1 : 0);
}

SCALAR_FUN_ATTR int16_t sdiv16(int16_t x, int16_t y) {
  int16_t q = x / y;
  int16_t r = x % y;
  return q - ((r != 0 && r < 0 != y < 0) ? 1 : 0);
}

SCALAR_FUN_ATTR int32_t sdiv32(int32_t x, int32_t y) {
  int32_t q = x / y;
  int32_t r = x % y;
  return q - ((r != 0 && r < 0 != y < 0) ? 1 : 0);
}

SCALAR_FUN_ATTR int64_t sdiv64(int64_t x, int64_t y) {
  int64_t q = x / y;
  int64_t r = x % y;
  return q - ((r != 0 && r < 0 != y < 0) ? 1 : 0);
}

SCALAR_FUN_ATTR int8_t   sdiv_up8(int8_t x, int8_t y)   { return sdiv8(x + y - 1, y); }
SCALAR_FUN_ATTR int16_t sdiv_up16(int16_t x, int16_t y) { return sdiv16(x + y - 1, y); }
SCALAR_FUN_ATTR int32_t sdiv_up32(int32_t x, int32_t y) { return sdiv32(x + y - 1, y); }
SCALAR_FUN_ATTR int64_t sdiv_up64(int64_t x, int64_t y) { return sdiv64(x + y - 1, y); }

SCALAR_FUN_ATTR int8_t smod8(int8_t x, int8_t y) {
  int8_t r = x % y;
  return r + (r == 0 || (x > 0 && y > 0) || (x < 0 && y < 0) ? 0 : y);
}

SCALAR_FUN_ATTR int16_t smod16(int16_t x, int16_t y) {
  int16_t r = x % y;
  return r + (r == 0 || (x > 0 && y > 0) || (x < 0 && y < 0) ? 0 : y);
}

SCALAR_FUN_ATTR int32_t smod32(int32_t x, int32_t y) {
  int32_t r = x % y;
  return r + (r == 0 || (x > 0 && y > 0) || (x < 0 && y < 0) ? 0 : y);
}

SCALAR_FUN_ATTR int64_t smod64(int64_t x, int64_t y) {
  int64_t r = x % y;
  return r + (r == 0 || (x > 0 && y > 0) || (x < 0 && y < 0) ? 0 : y);
}

SCALAR_FUN_ATTR int8_t   sdiv_safe8(int8_t x, int8_t y)   { return y == 0 ? 0 : sdiv8(x, y); }
SCALAR_FUN_ATTR int16_t sdiv_safe16(int16_t x, int16_t y) { return y == 0 ? 0 : sdiv16(x, y); }
SCALAR_FUN_ATTR int32_t sdiv_safe32(int32_t x, int32_t y) { return y == 0 ? 0 : sdiv32(x, y); }
SCALAR_FUN_ATTR int64_t sdiv_safe64(int64_t x, int64_t y) { return y == 0 ? 0 : sdiv64(x, y); }

SCALAR_FUN_ATTR int8_t   sdiv_up_safe8(int8_t x, int8_t y)   { return sdiv_safe8(x + y - 1, y);}
SCALAR_FUN_ATTR int16_t sdiv_up_safe16(int16_t x, int16_t y) { return sdiv_safe16(x + y - 1, y); }
SCALAR_FUN_ATTR int32_t sdiv_up_safe32(int32_t x, int32_t y) { return sdiv_safe32(x + y - 1, y); }
SCALAR_FUN_ATTR int64_t sdiv_up_safe64(int64_t x, int64_t y) { return sdiv_safe64(x + y - 1, y); }

SCALAR_FUN_ATTR int8_t   smod_safe8(int8_t x, int8_t y)   { return y == 0 ? 0 : smod8(x, y); }
SCALAR_FUN_ATTR int16_t smod_safe16(int16_t x, int16_t y) { return y == 0 ? 0 : smod16(x, y); }
SCALAR_FUN_ATTR int32_t smod_safe32(int32_t x, int32_t y) { return y == 0 ? 0 : smod32(x, y); }
SCALAR_FUN_ATTR int64_t smod_safe64(int64_t x, int64_t y) { return y == 0 ? 0 : smod64(x, y); }

SCALAR_FUN_ATTR int8_t   squot8(int8_t x, int8_t y)   { return x / y; }
SCALAR_FUN_ATTR int16_t squot16(int16_t x, int16_t y) { return x / y; }
SCALAR_FUN_ATTR int32_t squot32(int32_t x, int32_t y) { return x / y; }
SCALAR_FUN_ATTR int64_t squot64(int64_t x, int64_t y) { return x / y; }

SCALAR_FUN_ATTR int8_t   srem8(int8_t x, int8_t y)   { return x % y; }
SCALAR_FUN_ATTR int16_t srem16(int16_t x, int16_t y) { return x % y; }
SCALAR_FUN_ATTR int32_t srem32(int32_t x, int32_t y) { return x % y; }
SCALAR_FUN_ATTR int64_t srem64(int64_t x, int64_t y) { return x % y; }

SCALAR_FUN_ATTR int8_t   squot_safe8(int8_t x, int8_t y)   { return y == 0 ? 0 : x / y; }
SCALAR_FUN_ATTR int16_t squot_safe16(int16_t x, int16_t y) { return y == 0 ? 0 : x / y; }
SCALAR_FUN_ATTR int32_t squot_safe32(int32_t x, int32_t y) { return y == 0 ? 0 : x / y; }
SCALAR_FUN_ATTR int64_t squot_safe64(int64_t x, int64_t y) { return y == 0 ? 0 : x / y; }

SCALAR_FUN_ATTR int8_t   srem_safe8(int8_t x, int8_t y)   { return y == 0 ? 0 : x % y; }
SCALAR_FUN_ATTR int16_t srem_safe16(int16_t x, int16_t y) { return y == 0 ? 0 : x % y; }
SCALAR_FUN_ATTR int32_t srem_safe32(int32_t x, int32_t y) { return y == 0 ? 0 : x % y; }
SCALAR_FUN_ATTR int64_t srem_safe64(int64_t x, int64_t y) { return y == 0 ? 0 : x % y; }

#endif

SCALAR_FUN_ATTR int8_t   smin8(int8_t x, int8_t y)   { return x < y ? x : y; }
SCALAR_FUN_ATTR int16_t smin16(int16_t x, int16_t y) { return x < y ? x : y; }
SCALAR_FUN_ATTR int32_t smin32(int32_t x, int32_t y) { return x < y ? x : y; }
SCALAR_FUN_ATTR int64_t smin64(int64_t x, int64_t y) { return x < y ? x : y; }

SCALAR_FUN_ATTR uint8_t   umin8(uint8_t x, uint8_t y)   { return x < y ? x : y; }
SCALAR_FUN_ATTR uint16_t umin16(uint16_t x, uint16_t y) { return x < y ? x : y; }
SCALAR_FUN_ATTR uint32_t umin32(uint32_t x, uint32_t y) { return x < y ? x : y; }
SCALAR_FUN_ATTR uint64_t umin64(uint64_t x, uint64_t y) { return x < y ? x : y; }

SCALAR_FUN_ATTR int8_t  smax8(int8_t x, int8_t y)    { return x < y ? y : x; }
SCALAR_FUN_ATTR int16_t smax16(int16_t x, int16_t y) { return x < y ? y : x; }
SCALAR_FUN_ATTR int32_t smax32(int32_t x, int32_t y) { return x < y ? y : x; }
SCALAR_FUN_ATTR int64_t smax64(int64_t x, int64_t y) { return x < y ? y : x; }

SCALAR_FUN_ATTR uint8_t   umax8(uint8_t x, uint8_t y)   { return x < y ? y : x; }
SCALAR_FUN_ATTR uint16_t umax16(uint16_t x, uint16_t y) { return x < y ? y : x; }
SCALAR_FUN_ATTR uint32_t umax32(uint32_t x, uint32_t y) { return x < y ? y : x; }
SCALAR_FUN_ATTR uint64_t umax64(uint64_t x, uint64_t y) { return x < y ? y : x; }

SCALAR_FUN_ATTR uint8_t   shl8(uint8_t x, uint8_t y)   { return (uint8_t)(x << y); }
SCALAR_FUN_ATTR uint16_t shl16(uint16_t x, uint16_t y) { return (uint16_t)(x << y); }
SCALAR_FUN_ATTR uint32_t shl32(uint32_t x, uint32_t y) { return x << y; }
SCALAR_FUN_ATTR uint64_t shl64(uint64_t x, uint64_t y) { return x << y; }

SCALAR_FUN_ATTR uint8_t   lshr8(uint8_t x, uint8_t y)   { return x >> y; }
SCALAR_FUN_ATTR uint16_t lshr16(uint16_t x, uint16_t y) { return x >> y; }
SCALAR_FUN_ATTR uint32_t lshr32(uint32_t x, uint32_t y) { return x >> y; }
SCALAR_FUN_ATTR uint64_t lshr64(uint64_t x, uint64_t y) { return x >> y; }

SCALAR_FUN_ATTR int8_t   ashr8(int8_t x, int8_t y)   { return x >> y; }
SCALAR_FUN_ATTR int16_t ashr16(int16_t x, int16_t y) { return x >> y; }
SCALAR_FUN_ATTR int32_t ashr32(int32_t x, int32_t y) { return x >> y; }
SCALAR_FUN_ATTR int64_t ashr64(int64_t x, int64_t y) { return x >> y; }

SCALAR_FUN_ATTR uint8_t   and8(uint8_t x, uint8_t y)   { return x & y; }
SCALAR_FUN_ATTR uint16_t and16(uint16_t x, uint16_t y) { return x & y; }
SCALAR_FUN_ATTR uint32_t and32(uint32_t x, uint32_t y) { return x & y; }
SCALAR_FUN_ATTR uint64_t and64(uint64_t x, uint64_t y) { return x & y; }

SCALAR_FUN_ATTR uint8_t    or8(uint8_t x, uint8_t y)  { return x | y; }
SCALAR_FUN_ATTR uint16_t or16(uint16_t x, uint16_t y) { return x | y; }
SCALAR_FUN_ATTR uint32_t or32(uint32_t x, uint32_t y) { return x | y; }
SCALAR_FUN_ATTR uint64_t or64(uint64_t x, uint64_t y) { return x | y; }

SCALAR_FUN_ATTR uint8_t   xor8(uint8_t x, uint8_t y)   { return x ^ y; }
SCALAR_FUN_ATTR uint16_t xor16(uint16_t x, uint16_t y) { return x ^ y; }
SCALAR_FUN_ATTR uint32_t xor32(uint32_t x, uint32_t y) { return x ^ y; }
SCALAR_FUN_ATTR uint64_t xor64(uint64_t x, uint64_t y) { return x ^ y; }

SCALAR_FUN_ATTR bool ult8(uint8_t x, uint8_t y)    { return x < y; }
SCALAR_FUN_ATTR bool ult16(uint16_t x, uint16_t y) { return x < y; }
SCALAR_FUN_ATTR bool ult32(uint32_t x, uint32_t y) { return x < y; }
SCALAR_FUN_ATTR bool ult64(uint64_t x, uint64_t y) { return x < y; }

SCALAR_FUN_ATTR bool ule8(uint8_t x, uint8_t y)    { return x <= y; }
SCALAR_FUN_ATTR bool ule16(uint16_t x, uint16_t y) { return x <= y; }
SCALAR_FUN_ATTR bool ule32(uint32_t x, uint32_t y) { return x <= y; }
SCALAR_FUN_ATTR bool ule64(uint64_t x, uint64_t y) { return x <= y; }

SCALAR_FUN_ATTR bool  slt8(int8_t x, int8_t y)   { return x < y; }
SCALAR_FUN_ATTR bool slt16(int16_t x, int16_t y) { return x < y; }
SCALAR_FUN_ATTR bool slt32(int32_t x, int32_t y) { return x < y; }
SCALAR_FUN_ATTR bool slt64(int64_t x, int64_t y) { return x < y; }

SCALAR_FUN_ATTR bool  sle8(int8_t x, int8_t y)   { return x <= y; }
SCALAR_FUN_ATTR bool sle16(int16_t x, int16_t y) { return x <= y; }
SCALAR_FUN_ATTR bool sle32(int32_t x, int32_t y) { return x <= y; }
SCALAR_FUN_ATTR bool sle64(int64_t x, int64_t y) { return x <= y; }

SCALAR_FUN_ATTR uint8_t pow8(uint8_t x, uint8_t y) {
  uint8_t res = 1, rem = y;
  while (rem != 0) {
    if (rem & 1)
      res *= x;
    rem >>= 1;
    x *= x;
  }
  return res;
}

SCALAR_FUN_ATTR uint16_t pow16(uint16_t x, uint16_t y) {
  uint16_t res = 1, rem = y;
  while (rem != 0) {
    if (rem & 1)
      res *= x;
    rem >>= 1;
    x *= x;
  }
  return res;
}

SCALAR_FUN_ATTR uint32_t pow32(uint32_t x, uint32_t y) {
  uint32_t res = 1, rem = y;
  while (rem != 0) {
    if (rem & 1)
      res *= x;
    rem >>= 1;
    x *= x;
  }
  return res;
}

SCALAR_FUN_ATTR uint64_t pow64(uint64_t x, uint64_t y) {
  uint64_t res = 1, rem = y;
  while (rem != 0) {
    if (rem & 1)
      res *= x;
    rem >>= 1;
    x *= x;
  }
  return res;
}

SCALAR_FUN_ATTR bool  itob_i8_bool(int8_t x)  { return x != 0; }
SCALAR_FUN_ATTR bool itob_i16_bool(int16_t x) { return x != 0; }
SCALAR_FUN_ATTR bool itob_i32_bool(int32_t x) { return x != 0; }
SCALAR_FUN_ATTR bool itob_i64_bool(int64_t x) { return x != 0; }

SCALAR_FUN_ATTR int8_t btoi_bool_i8(bool x)   { return x; }
SCALAR_FUN_ATTR int16_t btoi_bool_i16(bool x) { return x; }
SCALAR_FUN_ATTR int32_t btoi_bool_i32(bool x) { return x; }
SCALAR_FUN_ATTR int64_t btoi_bool_i64(bool x) { return x; }

#define sext_i8_i8(x) ((int8_t) (int8_t) (x))
#define sext_i8_i16(x) ((int16_t) (int8_t) (x))
#define sext_i8_i32(x) ((int32_t) (int8_t) (x))
#define sext_i8_i64(x) ((int64_t) (int8_t) (x))
#define sext_i16_i8(x) ((int8_t) (int16_t) (x))
#define sext_i16_i16(x) ((int16_t) (int16_t) (x))
#define sext_i16_i32(x) ((int32_t) (int16_t) (x))
#define sext_i16_i64(x) ((int64_t) (int16_t) (x))
#define sext_i32_i8(x) ((int8_t) (int32_t) (x))
#define sext_i32_i16(x) ((int16_t) (int32_t) (x))
#define sext_i32_i32(x) ((int32_t) (int32_t) (x))
#define sext_i32_i64(x) ((int64_t) (int32_t) (x))
#define sext_i64_i8(x) ((int8_t) (int64_t) (x))
#define sext_i64_i16(x) ((int16_t) (int64_t) (x))
#define sext_i64_i32(x) ((int32_t) (int64_t) (x))
#define sext_i64_i64(x) ((int64_t) (int64_t) (x))
#define zext_i8_i8(x) ((int8_t) (uint8_t) (x))
#define zext_i8_i16(x) ((int16_t) (uint8_t) (x))
#define zext_i8_i32(x) ((int32_t) (uint8_t) (x))
#define zext_i8_i64(x) ((int64_t) (uint8_t) (x))
#define zext_i16_i8(x) ((int8_t) (uint16_t) (x))
#define zext_i16_i16(x) ((int16_t) (uint16_t) (x))
#define zext_i16_i32(x) ((int32_t) (uint16_t) (x))
#define zext_i16_i64(x) ((int64_t) (uint16_t) (x))
#define zext_i32_i8(x) ((int8_t) (uint32_t) (x))
#define zext_i32_i16(x) ((int16_t) (uint32_t) (x))
#define zext_i32_i32(x) ((int32_t) (uint32_t) (x))
#define zext_i32_i64(x) ((int64_t) (uint32_t) (x))
#define zext_i64_i8(x) ((int8_t) (uint64_t) (x))
#define zext_i64_i16(x) ((int16_t) (uint64_t) (x))
#define zext_i64_i32(x) ((int32_t) (uint64_t) (x))
#define zext_i64_i64(x) ((int64_t) (uint64_t) (x))

SCALAR_FUN_ATTR int8_t   abs8(int8_t x)  { return (int8_t)abs(x); }
SCALAR_FUN_ATTR int16_t abs16(int16_t x) { return (int16_t)abs(x); }
SCALAR_FUN_ATTR int32_t abs32(int32_t x) { return abs(x); }
SCALAR_FUN_ATTR int64_t abs64(int64_t x) {
#if defined(__OPENCL_VERSION__) || defined(ISPC)
  return abs(x);
#else
  return llabs(x);
#endif
}

#if defined(__OPENCL_VERSION__)

SCALAR_FUN_ATTR int32_t  futrts_popc8(int8_t x)  { return popcount(x); }
SCALAR_FUN_ATTR int32_t futrts_popc16(int16_t x) { return popcount(x); }
SCALAR_FUN_ATTR int32_t futrts_popc32(int32_t x) { return popcount(x); }
SCALAR_FUN_ATTR int32_t futrts_popc64(int64_t x) { return popcount(x); }

#elif defined(__CUDA_ARCH__)

SCALAR_FUN_ATTR int32_t  futrts_popc8(int8_t x)  { return __popc(zext_i8_i32(x)); }
SCALAR_FUN_ATTR int32_t futrts_popc16(int16_t x) { return __popc(zext_i16_i32(x)); }
SCALAR_FUN_ATTR int32_t futrts_popc32(int32_t x) { return __popc(x); }
SCALAR_FUN_ATTR int32_t futrts_popc64(int64_t x) { return __popcll(x); }

#else // Not OpenCL or CUDA, but plain C.

SCALAR_FUN_ATTR int32_t futrts_popc8(uint8_t x) {
  int c = 0;
  for (; x; ++c) { x &= x - 1; }
  return c;
}

SCALAR_FUN_ATTR int32_t futrts_popc16(uint16_t x) {
  int c = 0;
  for (; x; ++c) { x &= x - 1; }
  return c;
}

SCALAR_FUN_ATTR int32_t futrts_popc32(uint32_t x) {
  int c = 0;
  for (; x; ++c) { x &= x - 1; }
  return c;
}

SCALAR_FUN_ATTR int32_t futrts_popc64(uint64_t x) {
  int c = 0;
  for (; x; ++c) { x &= x - 1; }
  return c;
}
#endif

#if defined(__OPENCL_VERSION__)
SCALAR_FUN_ATTR uint8_t  futrts_umul_hi8 ( uint8_t a,  uint8_t b) { return mul_hi(a, b); }
SCALAR_FUN_ATTR uint16_t futrts_umul_hi16(uint16_t a, uint16_t b) { return mul_hi(a, b); }
SCALAR_FUN_ATTR uint32_t futrts_umul_hi32(uint32_t a, uint32_t b) { return mul_hi(a, b); }
SCALAR_FUN_ATTR uint64_t futrts_umul_hi64(uint64_t a, uint64_t b) { return mul_hi(a, b); }
SCALAR_FUN_ATTR uint8_t  futrts_smul_hi8 ( int8_t a,  int8_t b) { return mul_hi(a, b); }
SCALAR_FUN_ATTR uint16_t futrts_smul_hi16(int16_t a, int16_t b) { return mul_hi(a, b); }
SCALAR_FUN_ATTR uint32_t futrts_smul_hi32(int32_t a, int32_t b) { return mul_hi(a, b); }
SCALAR_FUN_ATTR uint64_t futrts_smul_hi64(int64_t a, int64_t b) { return mul_hi(a, b); }
#elif defined(__CUDA_ARCH__)
SCALAR_FUN_ATTR  uint8_t futrts_umul_hi8(uint8_t a, uint8_t b) { return ((uint16_t)a) * ((uint16_t)b) >> 8; }
SCALAR_FUN_ATTR uint16_t futrts_umul_hi16(uint16_t a, uint16_t b) { return ((uint32_t)a) * ((uint32_t)b) >> 16; }
SCALAR_FUN_ATTR uint32_t futrts_umul_hi32(uint32_t a, uint32_t b) { return __umulhi(a, b); }
SCALAR_FUN_ATTR uint64_t futrts_umul_hi64(uint64_t a, uint64_t b) { return __umul64hi(a, b); }
SCALAR_FUN_ATTR  uint8_t futrts_smul_hi8 ( int8_t a, int8_t b) { return ((int16_t)a) * ((int16_t)b) >> 8; }
SCALAR_FUN_ATTR uint16_t futrts_smul_hi16(int16_t a, int16_t b) { return ((int32_t)a) * ((int32_t)b) >> 16; }
SCALAR_FUN_ATTR uint32_t futrts_smul_hi32(int32_t a, int32_t b) { return __mulhi(a, b); }
SCALAR_FUN_ATTR uint64_t futrts_smul_hi64(int64_t a, int64_t b) { return __mul64hi(a, b); }
#elif defined(ISPC)
SCALAR_FUN_ATTR uint8_t futrts_umul_hi8(uint8_t a, uint8_t b) { return ((uint16_t)a) * ((uint16_t)b) >> 8; }
SCALAR_FUN_ATTR uint16_t futrts_umul_hi16(uint16_t a, uint16_t b) { return ((uint32_t)a) * ((uint32_t)b) >> 16; }
SCALAR_FUN_ATTR uint32_t futrts_umul_hi32(uint32_t a, uint32_t b) { return ((uint64_t)a) * ((uint64_t)b) >> 32; }
SCALAR_FUN_ATTR uint64_t futrts_umul_hi64(uint64_t a, uint64_t b) {
  uint64_t ah = a >> 32;
  uint64_t al = a & 0xffffffff;
  uint64_t bh = b >> 32;
  uint64_t bl = b & 0xffffffff;

  uint64_t p1 = al * bl;
  uint64_t p2 = al * bh;
  uint64_t p3 = ah * bl;
  uint64_t p4 = ah * bh;

  uint64_t p1h = p1 >> 32;
  uint64_t p2h = p2 >> 32;
  uint64_t p3h = p3 >> 32;
  uint64_t p2l = p2 & 0xffffffff;
  uint64_t p3l = p3 & 0xffffffff;

  uint64_t l = p1h + p2l + p3l;
  uint64_t m = (p2 >> 32) + (p3 >> 32);
  uint64_t h = (l >> 32) + m + p4;

  return h;
}
SCALAR_FUN_ATTR  int8_t futrts_smul_hi8 ( int8_t a,  int8_t b) { return ((uint16_t)a) * ((uint16_t)b) >> 8; }
SCALAR_FUN_ATTR int16_t futrts_smul_hi16(int16_t a, int16_t b) { return ((uint32_t)a) * ((uint32_t)b) >> 16; }
SCALAR_FUN_ATTR int32_t futrts_smul_hi32(int32_t a, int32_t b) { return ((uint64_t)a) * ((uint64_t)b) >> 32; }
SCALAR_FUN_ATTR int64_t futrts_smul_hi64(int64_t a, int64_t b) {
  uint64_t ah = a >> 32;
  uint64_t al = a & 0xffffffff;
  uint64_t bh = b >> 32;
  uint64_t bl = b & 0xffffffff;

  uint64_t p1 =  al * bl;
  int64_t  p2 = al * bh;
  int64_t  p3 = ah * bl;
  uint64_t p4 =  ah * bh;

  uint64_t p1h = p1 >> 32;
  uint64_t p2h = p2 >> 32;
  uint64_t p3h = p3 >> 32;
  uint64_t p2l = p2 & 0xffffffff;
  uint64_t p3l = p3 & 0xffffffff;

  uint64_t l = p1h + p2l + p3l;
  uint64_t m = (p2 >> 32) + (p3 >> 32);
  uint64_t h = (l >> 32) + m + p4;

  return h;
}

#else // Not OpenCL, ISPC, or CUDA, but plain C.
SCALAR_FUN_ATTR uint8_t futrts_umul_hi8(uint8_t a, uint8_t b) { return ((uint16_t)a) * ((uint16_t)b) >> 8; }
SCALAR_FUN_ATTR uint16_t futrts_umul_hi16(uint16_t a, uint16_t b) { return ((uint32_t)a) * ((uint32_t)b) >> 16; }
SCALAR_FUN_ATTR uint32_t futrts_umul_hi32(uint32_t a, uint32_t b) { return ((uint64_t)a) * ((uint64_t)b) >> 32; }
SCALAR_FUN_ATTR uint64_t futrts_umul_hi64(uint64_t a, uint64_t b) { return ((__uint128_t)a) * ((__uint128_t)b) >> 64; }
SCALAR_FUN_ATTR int8_t futrts_smul_hi8(int8_t a, int8_t b) { return ((int16_t)a) * ((int16_t)b) >> 8; }
SCALAR_FUN_ATTR int16_t futrts_smul_hi16(int16_t a, int16_t b) { return ((int32_t)a) * ((int32_t)b) >> 16; }
SCALAR_FUN_ATTR int32_t futrts_smul_hi32(int32_t a, int32_t b) { return ((int64_t)a) * ((int64_t)b) >> 32; }
SCALAR_FUN_ATTR int64_t futrts_smul_hi64(int64_t a, int64_t b) { return ((__int128_t)a) * ((__int128_t)b) >> 64; }
#endif

#if defined(__OPENCL_VERSION__)
SCALAR_FUN_ATTR  uint8_t futrts_umad_hi8 ( uint8_t a,  uint8_t b,  uint8_t c) { return mad_hi(a, b, c); }
SCALAR_FUN_ATTR uint16_t futrts_umad_hi16(uint16_t a, uint16_t b, uint16_t c) { return mad_hi(a, b, c); }
SCALAR_FUN_ATTR uint32_t futrts_umad_hi32(uint32_t a, uint32_t b, uint32_t c) { return mad_hi(a, b, c); }
SCALAR_FUN_ATTR uint64_t futrts_umad_hi64(uint64_t a, uint64_t b, uint64_t c) { return mad_hi(a, b, c); }
SCALAR_FUN_ATTR  uint8_t futrts_smad_hi8( int8_t a,  int8_t b,   int8_t c) { return mad_hi(a, b, c); }
SCALAR_FUN_ATTR uint16_t futrts_smad_hi16(int16_t a, int16_t b, int16_t c) { return mad_hi(a, b, c); }
SCALAR_FUN_ATTR uint32_t futrts_smad_hi32(int32_t a, int32_t b, int32_t c) { return mad_hi(a, b, c); }
SCALAR_FUN_ATTR uint64_t futrts_smad_hi64(int64_t a, int64_t b, int64_t c) { return mad_hi(a, b, c); }
#else // Not OpenCL

SCALAR_FUN_ATTR  uint8_t futrts_umad_hi8( uint8_t a,  uint8_t b,  uint8_t c) { return futrts_umul_hi8(a, b) + c; }
SCALAR_FUN_ATTR uint16_t futrts_umad_hi16(uint16_t a, uint16_t b, uint16_t c) { return futrts_umul_hi16(a, b) + c; }
SCALAR_FUN_ATTR uint32_t futrts_umad_hi32(uint32_t a, uint32_t b, uint32_t c) { return futrts_umul_hi32(a, b) + c; }
SCALAR_FUN_ATTR uint64_t futrts_umad_hi64(uint64_t a, uint64_t b, uint64_t c) { return futrts_umul_hi64(a, b) + c; }
SCALAR_FUN_ATTR  uint8_t futrts_smad_hi8 ( int8_t a,  int8_t b,  int8_t c) { return futrts_smul_hi8(a, b) + c; }
SCALAR_FUN_ATTR uint16_t futrts_smad_hi16(int16_t a, int16_t b, int16_t c) { return futrts_smul_hi16(a, b) + c; }
SCALAR_FUN_ATTR uint32_t futrts_smad_hi32(int32_t a, int32_t b, int32_t c) { return futrts_smul_hi32(a, b) + c; }
SCALAR_FUN_ATTR uint64_t futrts_smad_hi64(int64_t a, int64_t b, int64_t c) { return futrts_smul_hi64(a, b) + c; }
#endif

#if defined(__OPENCL_VERSION__)
SCALAR_FUN_ATTR int32_t  futrts_clzz8(int8_t x)  { return clz(x); }
SCALAR_FUN_ATTR int32_t futrts_clzz16(int16_t x) { return clz(x); }
SCALAR_FUN_ATTR int32_t futrts_clzz32(int32_t x) { return clz(x); }
SCALAR_FUN_ATTR int32_t futrts_clzz64(int64_t x) { return clz(x); }

#elif defined(__CUDA_ARCH__)

SCALAR_FUN_ATTR int32_t  futrts_clzz8(int8_t x)  { return __clz(zext_i8_i32(x)) - 24; }
SCALAR_FUN_ATTR int32_t futrts_clzz16(int16_t x) { return __clz(zext_i16_i32(x)) - 16; }
SCALAR_FUN_ATTR int32_t futrts_clzz32(int32_t x) { return __clz(x); }
SCALAR_FUN_ATTR int32_t futrts_clzz64(int64_t x) { return __clzll(x); }

#elif defined(ISPC)

SCALAR_FUN_ATTR int32_t  futrts_clzz8(int8_t x)  { return count_leading_zeros((int32_t)(uint8_t)x)-24; }
SCALAR_FUN_ATTR int32_t futrts_clzz16(int16_t x) { return count_leading_zeros((int32_t)(uint16_t)x)-16; }
SCALAR_FUN_ATTR int32_t futrts_clzz32(int32_t x) { return count_leading_zeros(x); }
SCALAR_FUN_ATTR int32_t futrts_clzz64(int64_t x) { return count_leading_zeros(x); }

#else // Not OpenCL, ISPC or CUDA, but plain C.

SCALAR_FUN_ATTR int32_t futrts_clzz8(int8_t x)
{ return x == 0 ? 8 : __builtin_clz((uint32_t)zext_i8_i32(x)) - 24; }
SCALAR_FUN_ATTR int32_t futrts_clzz16(int16_t x)
{ return x == 0 ? 16 : __builtin_clz((uint32_t)zext_i16_i32(x)) - 16; }
SCALAR_FUN_ATTR int32_t futrts_clzz32(int32_t x)
{ return x == 0 ? 32 : __builtin_clz((uint32_t)x); }
SCALAR_FUN_ATTR int32_t futrts_clzz64(int64_t x)
{ return x == 0 ? 64 : __builtin_clzll((uint64_t)x); }
#endif

#if defined(__OPENCL_VERSION__)
SCALAR_FUN_ATTR int32_t futrts_ctzz8(int8_t x) {
  int i = 0;
  for (; i < 8 && (x & 1) == 0; i++, x >>= 1) ;
  return i;
}

SCALAR_FUN_ATTR int32_t futrts_ctzz16(int16_t x) {
  int i = 0;
  for (; i < 16 && (x & 1) == 0; i++, x >>= 1) ;
  return i;
}

SCALAR_FUN_ATTR int32_t futrts_ctzz32(int32_t x) {
  int i = 0;
  for (; i < 32 && (x & 1) == 0; i++, x >>= 1) ;
  return i;
}

SCALAR_FUN_ATTR int32_t futrts_ctzz64(int64_t x) {
  int i = 0;
  for (; i < 64 && (x & 1) == 0; i++, x >>= 1) ;
  return i;
}

#elif defined(__CUDA_ARCH__)

SCALAR_FUN_ATTR int32_t futrts_ctzz8(int8_t x) {
  int y = __ffs(x);
  return y == 0 ? 8 : y - 1;
}

SCALAR_FUN_ATTR int32_t futrts_ctzz16(int16_t x) {
  int y = __ffs(x);
  return y == 0 ? 16 : y - 1;
}

SCALAR_FUN_ATTR int32_t futrts_ctzz32(int32_t x) {
  int y = __ffs(x);
  return y == 0 ? 32 : y - 1;
}

SCALAR_FUN_ATTR int32_t futrts_ctzz64(int64_t x) {
  int y = __ffsll(x);
  return y == 0 ? 64 : y - 1;
}

#elif defined(ISPC)

SCALAR_FUN_ATTR int32_t futrts_ctzz8(int8_t x) { return x == 0 ? 8 : count_trailing_zeros((int32_t)x); }
SCALAR_FUN_ATTR int32_t futrts_ctzz16(int16_t x) { return x == 0 ? 16 : count_trailing_zeros((int32_t)x); }
SCALAR_FUN_ATTR int32_t futrts_ctzz32(int32_t x) { return count_trailing_zeros(x); }
SCALAR_FUN_ATTR int32_t futrts_ctzz64(int64_t x) { return count_trailing_zeros(x); }

#else // Not OpenCL or CUDA, but plain C.

SCALAR_FUN_ATTR int32_t  futrts_ctzz8(int8_t x)  { return x == 0 ? 8 : __builtin_ctz((uint32_t)x); }
SCALAR_FUN_ATTR int32_t futrts_ctzz16(int16_t x) { return x == 0 ? 16 : __builtin_ctz((uint32_t)x); }
SCALAR_FUN_ATTR int32_t futrts_ctzz32(int32_t x) { return x == 0 ? 32 : __builtin_ctz((uint32_t)x); }
SCALAR_FUN_ATTR int32_t futrts_ctzz64(int64_t x) { return x == 0 ? 64 : __builtin_ctzll((uint64_t)x); }
#endif

SCALAR_FUN_ATTR float fdiv32(float x, float y) { return x / y; }
SCALAR_FUN_ATTR float fadd32(float x, float y) { return x + y; }
SCALAR_FUN_ATTR float fsub32(float x, float y) { return x - y; }
SCALAR_FUN_ATTR float fmul32(float x, float y) { return x * y; }
SCALAR_FUN_ATTR bool cmplt32(float x, float y) { return x < y; }
SCALAR_FUN_ATTR bool cmple32(float x, float y) { return x <= y; }
SCALAR_FUN_ATTR float sitofp_i8_f32(int8_t x)  { return (float) x; }

SCALAR_FUN_ATTR float sitofp_i16_f32(int16_t x) { return (float) x; }
SCALAR_FUN_ATTR float sitofp_i32_f32(int32_t x) { return (float) x; }
SCALAR_FUN_ATTR float sitofp_i64_f32(int64_t x) { return (float) x; }
SCALAR_FUN_ATTR float  uitofp_i8_f32(uint8_t x)  { return (float) x; }
SCALAR_FUN_ATTR float uitofp_i16_f32(uint16_t x) { return (float) x; }
SCALAR_FUN_ATTR float uitofp_i32_f32(uint32_t x) { return (float) x; }
SCALAR_FUN_ATTR float uitofp_i64_f32(uint64_t x) { return (float) x; }

#ifdef __OPENCL_VERSION__
SCALAR_FUN_ATTR float fabs32(float x)          { return fabs(x); }
SCALAR_FUN_ATTR float fmax32(float x, float y) { return fmax(x, y); }
SCALAR_FUN_ATTR float fmin32(float x, float y) { return fmin(x, y); }
SCALAR_FUN_ATTR float fpow32(float x, float y) { return pow(x, y); }

#elif defined(ISPC)

SCALAR_FUN_ATTR float fabs32(float x) { return abs(x); }
SCALAR_FUN_ATTR float fmax32(float x, float y) { return isnan(x) ? y : isnan(y) ? x : max(x, y); }
SCALAR_FUN_ATTR float fmin32(float x, float y) { return isnan(x) ? y : isnan(y) ? x : min(x, y); }
SCALAR_FUN_ATTR float fpow32(float a, float b) {
  float ret;
  foreach_active (i) {
      uniform float r = pow(extract(a, i), extract(b, i));
      ret = insert(ret, i, r);
  }
  return ret;
}

#else // Not OpenCL, but CUDA or plain C.

SCALAR_FUN_ATTR float fabs32(float x)          { return fabsf(x); }
SCALAR_FUN_ATTR float fmax32(float x, float y) { return fmaxf(x, y); }
SCALAR_FUN_ATTR float fmin32(float x, float y) { return fminf(x, y); }
SCALAR_FUN_ATTR float fpow32(float x, float y) { return powf(x, y); }
#endif

SCALAR_FUN_ATTR bool futrts_isnan32(float x) { return isnan(x); }

#if defined(ISPC)

SCALAR_FUN_ATTR bool futrts_isinf32(float x) { return !isnan(x) && isnan(x - x); }

SCALAR_FUN_ATTR bool futrts_isfinite32(float x) { return !isnan(x) && !futrts_isinf32(x); }

#else

SCALAR_FUN_ATTR bool futrts_isinf32(float x) { return isinf(x); }

#endif

SCALAR_FUN_ATTR int8_t fptosi_f32_i8(float x) {
  if (futrts_isnan32(x) || futrts_isinf32(x)) {
    return 0;
  } else {
    return (int8_t) x;
  }
}

SCALAR_FUN_ATTR int16_t fptosi_f32_i16(float x) {
  if (futrts_isnan32(x) || futrts_isinf32(x)) {
    return 0;
  } else {
    return (int16_t) x;
  }
}

SCALAR_FUN_ATTR int32_t fptosi_f32_i32(float x) {
  if (futrts_isnan32(x) || futrts_isinf32(x)) {
    return 0;
  } else {
    return (int32_t) x;
  }
}

SCALAR_FUN_ATTR int64_t fptosi_f32_i64(float x) {
  if (futrts_isnan32(x) || futrts_isinf32(x)) {
    return 0;
  } else {
    return (int64_t) x;
  };
}

SCALAR_FUN_ATTR uint8_t fptoui_f32_i8(float x) {
  if (futrts_isnan32(x) || futrts_isinf32(x)) {
    return 0;
  } else {
    return (uint8_t) (int8_t) x;
  }
}

SCALAR_FUN_ATTR uint16_t fptoui_f32_i16(float x) {
  if (futrts_isnan32(x) || futrts_isinf32(x)) {
    return 0;
  } else {
    return (uint16_t) (int16_t) x;
  }
}

SCALAR_FUN_ATTR uint32_t fptoui_f32_i32(float x) {
  if (futrts_isnan32(x) || futrts_isinf32(x)) {
    return 0;
  } else {
    return (uint32_t) (int32_t) x;
  }
}

SCALAR_FUN_ATTR uint64_t fptoui_f32_i64(float x) {
  if (futrts_isnan32(x) || futrts_isinf32(x)) {
    return 0;
  } else {
    return (uint64_t) (int64_t) x;
  }
}

SCALAR_FUN_ATTR bool ftob_f32_bool(float x) { return x != 0; }
SCALAR_FUN_ATTR float btof_bool_f32(bool x) { return x ? 1 : 0; }

#ifdef __OPENCL_VERSION__
SCALAR_FUN_ATTR float futrts_log32(float x) { return log(x); }
SCALAR_FUN_ATTR float futrts_log2_32(float x) { return log2(x); }
SCALAR_FUN_ATTR float futrts_log10_32(float x) { return log10(x); }
SCALAR_FUN_ATTR float futrts_log1p_32(float x) { return log1p(x); }
SCALAR_FUN_ATTR float futrts_sqrt32(float x) { return sqrt(x); }
SCALAR_FUN_ATTR float futrts_rsqrt32(float x) { return rsqrt(x); }
SCALAR_FUN_ATTR float futrts_cbrt32(float x) { return cbrt(x); }
SCALAR_FUN_ATTR float futrts_exp32(float x) { return exp(x); }
SCALAR_FUN_ATTR float futrts_cos32(float x) { return cos(x); }
SCALAR_FUN_ATTR float futrts_cospi32(float x) { return cospi(x); }
SCALAR_FUN_ATTR float futrts_sin32(float x) { return sin(x); }
SCALAR_FUN_ATTR float futrts_sinpi32(float x) { return sinpi(x); }
SCALAR_FUN_ATTR float futrts_tan32(float x) { return tan(x); }
SCALAR_FUN_ATTR float futrts_tanpi32(float x) { return tanpi(x); }
SCALAR_FUN_ATTR float futrts_acos32(float x) { return acos(x); }
SCALAR_FUN_ATTR float futrts_acospi32(float x) { return acospi(x); }
SCALAR_FUN_ATTR float futrts_asin32(float x) { return asin(x); }
SCALAR_FUN_ATTR float futrts_asinpi32(float x) { return asinpi(x); }
SCALAR_FUN_ATTR float futrts_atan32(float x) { return atan(x); }
SCALAR_FUN_ATTR float futrts_atanpi32(float x) { return atanpi(x); }
SCALAR_FUN_ATTR float futrts_cosh32(float x) { return cosh(x); }
SCALAR_FUN_ATTR float futrts_sinh32(float x) { return sinh(x); }
SCALAR_FUN_ATTR float futrts_tanh32(float x) { return tanh(x); }
SCALAR_FUN_ATTR float futrts_acosh32(float x) { return acosh(x); }
SCALAR_FUN_ATTR float futrts_asinh32(float x) { return asinh(x); }
SCALAR_FUN_ATTR float futrts_atanh32(float x) { return atanh(x); }
SCALAR_FUN_ATTR float futrts_atan2_32(float x, float y) { return atan2(x, y); }
SCALAR_FUN_ATTR float futrts_atan2pi_32(float x, float y) { return atan2pi(x, y); }
SCALAR_FUN_ATTR float futrts_hypot32(float x, float y) { return hypot(x, y); }
SCALAR_FUN_ATTR float futrts_gamma32(float x) { return tgamma(x); }
SCALAR_FUN_ATTR float futrts_lgamma32(float x) { return lgamma(x); }
SCALAR_FUN_ATTR float futrts_erf32(float x) { return erf(x); }
SCALAR_FUN_ATTR float futrts_erfc32(float x) { return erfc(x); }
SCALAR_FUN_ATTR float fmod32(float x, float y) { return fmod(x, y); }
SCALAR_FUN_ATTR float futrts_round32(float x) { return rint(x); }
SCALAR_FUN_ATTR float futrts_floor32(float x) { return floor(x); }
SCALAR_FUN_ATTR float futrts_ceil32(float x) { return ceil(x); }
SCALAR_FUN_ATTR float futrts_nextafter32(float x, float y) { return nextafter(x, y); }
SCALAR_FUN_ATTR float futrts_lerp32(float v0, float v1, float t) { return mix(v0, v1, t); }
SCALAR_FUN_ATTR float futrts_ldexp32(float x, int32_t y) { return ldexp(x, y); }
SCALAR_FUN_ATTR float futrts_copysign32(float x, float y) { return copysign(x, y); }
SCALAR_FUN_ATTR float futrts_mad32(float a, float b, float c) { return mad(a, b, c); }
SCALAR_FUN_ATTR float futrts_fma32(float a, float b, float c) { return fma(a, b, c); }

#elif defined(ISPC)

SCALAR_FUN_ATTR float futrts_log32(float x) { return futrts_isfinite32(x) || (futrts_isinf32(x) && x < 0)? log(x) : x; }
SCALAR_FUN_ATTR float futrts_log2_32(float x) { return futrts_log32(x) / log(2.0f); }
SCALAR_FUN_ATTR float futrts_log10_32(float x) { return futrts_log32(x) / log(10.0f); }

SCALAR_FUN_ATTR float futrts_log1p_32(float x) {
  if(x == -1.0f || (futrts_isinf32(x) && x > 0.0f)) return x / 0.0f;
  float y = 1.0f + x;
  float z = y - 1.0f;
  return log(y) - (z-x)/y;
}

SCALAR_FUN_ATTR float futrts_sqrt32(float x) { return sqrt(x); }
SCALAR_FUN_ATTR float futrts_rsqrt32(float x) { return 1/sqrt(x); }

extern "C" unmasked uniform float cbrtf(uniform float);
SCALAR_FUN_ATTR float futrts_cbrt32(float x) {
  float res;
  foreach_active (i) {
    uniform float r = cbrtf(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

SCALAR_FUN_ATTR float futrts_exp32(float x) { return exp(x); }
SCALAR_FUN_ATTR float futrts_cos32(float x) { return cos(x); }
SCALAR_FUN_ATTR float futrts_cospi32(float x) { return cos((float)M_PI*x); }
SCALAR_FUN_ATTR float futrts_sin32(float x) { return sin(x); }
SCALAR_FUN_ATTR float futrts_sinpi32(float x) { return sin(M_PI*x); }
SCALAR_FUN_ATTR float futrts_tan32(float x) { return tan(x); }
SCALAR_FUN_ATTR float futrts_tanpi32(float x) { return tan((float)M_PI*x); }
SCALAR_FUN_ATTR float futrts_acos32(float x) { return acos(x); }
SCALAR_FUN_ATTR float futrts_acospi32(float x) { return acos(x)/(float)M_PI; }
SCALAR_FUN_ATTR float futrts_asin32(float x) { return asin(x); }
SCALAR_FUN_ATTR float futrts_asinpi32(float x) { return asin(x)/(float)M_PI; }
SCALAR_FUN_ATTR float futrts_atan32(float x) { return atan(x); }
SCALAR_FUN_ATTR float futrts_atanpi32(float x) { return atan(x)/(float)M_PI; }
SCALAR_FUN_ATTR float futrts_cosh32(float x) { return (exp(x)+exp(-x)) / 2.0f; }
SCALAR_FUN_ATTR float futrts_sinh32(float x) { return (exp(x)-exp(-x)) / 2.0f; }
SCALAR_FUN_ATTR float futrts_tanh32(float x) { return futrts_sinh32(x)/futrts_cosh32(x); }

SCALAR_FUN_ATTR float futrts_acosh32(float x) {
  float f = x+sqrt(x*x-1);
  if (futrts_isfinite32(f)) return log(f);
  return f;
}

SCALAR_FUN_ATTR float futrts_asinh32(float x) {
  float f = x+sqrt(x*x+1);
  if (futrts_isfinite32(f)) return log(f);
  return f;
}

SCALAR_FUN_ATTR float futrts_atanh32(float x) {
  float f = (1+x)/(1-x);
  if (futrts_isfinite32(f)) return log(f)/2.0f;
  return f;
}

SCALAR_FUN_ATTR float futrts_atan2_32(float x, float y)
{ return (x == 0.0f && y == 0.0f) ? 0.0f : atan2(x, y); }
SCALAR_FUN_ATTR float futrts_atan2pi_32(float x, float y)
{ return (x == 0.0f && y == 0.0f) ? 0.0f : atan2(x, y) / (float)M_PI; }

SCALAR_FUN_ATTR float futrts_hypot32(float x, float y) {
  if (futrts_isfinite32(x) && futrts_isfinite32(y)) {
    x = abs(x);
    y = abs(y);
    float a;
    float b;
    if (x >= y){
        a = x;
        b = y;
    } else {
        a = y;
        b = x;
    }
    if(b == 0){
      return a;
    }

    int e;
    float an;
    float bn;
    an = frexp (a, &e);
    bn = ldexp (b, - e);
    float cn;
    cn = sqrt (an * an + bn * bn);
    return ldexp (cn, e);
  } else {
    if (futrts_isinf32(x) || futrts_isinf32(y)) return INFINITY;
    else return x + y;
  }

}

extern "C" unmasked uniform float tgammaf(uniform float x);
SCALAR_FUN_ATTR float futrts_gamma32(float x) {
  float res;
  foreach_active (i) {
    uniform float r = tgammaf(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

extern "C" unmasked uniform float lgammaf(uniform float x);
SCALAR_FUN_ATTR float futrts_lgamma32(float x) {
  float res;
  foreach_active (i) {
    uniform float r = lgammaf(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

extern "C" unmasked uniform float erff(uniform float x);
SCALAR_FUN_ATTR float futrts_erf32(float x) {
  float res;
  foreach_active (i) {
    uniform float r = erff(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

extern "C" unmasked uniform float erfcf(uniform float x);
SCALAR_FUN_ATTR float futrts_erfc32(float x) {
  float res;
  foreach_active (i) {
    uniform float r = erfcf(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

SCALAR_FUN_ATTR float fmod32(float x, float y) { return x - y * trunc(x/y); }
SCALAR_FUN_ATTR float futrts_round32(float x) { return round(x); }
SCALAR_FUN_ATTR float futrts_floor32(float x) { return floor(x); }
SCALAR_FUN_ATTR float futrts_ceil32(float x) { return ceil(x); }

extern "C" unmasked uniform float nextafterf(uniform float x, uniform float y);
SCALAR_FUN_ATTR float futrts_nextafter32(float x, float y) {
  float res;
  foreach_active (i) {
    uniform float r = nextafterf(extract(x, i), extract(y, i));
    res = insert(res, i, r);
  }
  return res;
}

SCALAR_FUN_ATTR float futrts_lerp32(float v0, float v1, float t) {
  return v0 + (v1 - v0) * t;
}

SCALAR_FUN_ATTR float futrts_ldexp32(float x, int32_t y) {
  return x * pow((uniform float)2.0, (float)y);
}

SCALAR_FUN_ATTR float futrts_copysign32(float x, float y) {
  int32_t xb = fptobits_f32_i32(x);
  int32_t yb = fptobits_f32_i32(y);
  return bitstofp_i32_f32((xb & ~(1<<31)) | (yb & (1<<31)));
}

SCALAR_FUN_ATTR float futrts_mad32(float a, float b, float c) {
  return a * b + c;
}

SCALAR_FUN_ATTR float futrts_fma32(float a, float b, float c) {
  return a * b + c;
}

#else // Not OpenCL or ISPC, but CUDA or plain C.

SCALAR_FUN_ATTR float futrts_log32(float x) { return logf(x); }
SCALAR_FUN_ATTR float futrts_log2_32(float x) { return log2f(x); }
SCALAR_FUN_ATTR float futrts_log10_32(float x) { return log10f(x); }
SCALAR_FUN_ATTR float futrts_log1p_32(float x) { return log1pf(x); }
SCALAR_FUN_ATTR float futrts_sqrt32(float x) { return sqrtf(x); }
SCALAR_FUN_ATTR float futrts_rsqrt32(float x) { return 1/sqrtf(x); }
SCALAR_FUN_ATTR float futrts_cbrt32(float x) { return cbrtf(x); }
SCALAR_FUN_ATTR float futrts_exp32(float x) { return expf(x); }
SCALAR_FUN_ATTR float futrts_cos32(float x) { return cosf(x); }

SCALAR_FUN_ATTR float futrts_cospi32(float x) {
#if defined(__CUDA_ARCH__)
  return cospif(x);
#else
  return cosf(((float)M_PI)*x);
#endif
}
SCALAR_FUN_ATTR float futrts_sin32(float x) { return sinf(x); }

SCALAR_FUN_ATTR float futrts_sinpi32(float x) {
#if defined(__CUDA_ARCH__)
  return sinpif(x);
#else
  return sinf((float)M_PI*x);
#endif
}

SCALAR_FUN_ATTR float futrts_tan32(float x) { return tanf(x); }
SCALAR_FUN_ATTR float futrts_tanpi32(float x) { return tanf((float)M_PI*x); }
SCALAR_FUN_ATTR float futrts_acos32(float x) { return acosf(x); }
SCALAR_FUN_ATTR float futrts_acospi32(float x) { return acosf(x)/(float)M_PI; }
SCALAR_FUN_ATTR float futrts_asin32(float x) { return asinf(x); }
SCALAR_FUN_ATTR float futrts_asinpi32(float x) { return asinf(x)/(float)M_PI; }
SCALAR_FUN_ATTR float futrts_atan32(float x) { return atanf(x); }
SCALAR_FUN_ATTR float futrts_atanpi32(float x) { return atanf(x)/(float)M_PI; }
SCALAR_FUN_ATTR float futrts_cosh32(float x) { return coshf(x); }
SCALAR_FUN_ATTR float futrts_sinh32(float x) { return sinhf(x); }
SCALAR_FUN_ATTR float futrts_tanh32(float x) { return tanhf(x); }
SCALAR_FUN_ATTR float futrts_acosh32(float x) { return acoshf(x); }
SCALAR_FUN_ATTR float futrts_asinh32(float x) { return asinhf(x); }
SCALAR_FUN_ATTR float futrts_atanh32(float x) { return atanhf(x); }
SCALAR_FUN_ATTR float futrts_atan2_32(float x, float y) { return atan2f(x, y); }
SCALAR_FUN_ATTR float futrts_atan2pi_32(float x, float y) { return atan2f(x, y) / (float)M_PI; }
SCALAR_FUN_ATTR float futrts_hypot32(float x, float y) { return hypotf(x, y); }
SCALAR_FUN_ATTR float futrts_gamma32(float x) { return tgammaf(x); }
SCALAR_FUN_ATTR float futrts_lgamma32(float x) { return lgammaf(x); }
SCALAR_FUN_ATTR float futrts_erf32(float x) { return erff(x); }
SCALAR_FUN_ATTR float futrts_erfc32(float x) { return erfcf(x); }
SCALAR_FUN_ATTR float fmod32(float x, float y) { return fmodf(x, y); }
SCALAR_FUN_ATTR float futrts_round32(float x) { return rintf(x); }
SCALAR_FUN_ATTR float futrts_floor32(float x) { return floorf(x); }
SCALAR_FUN_ATTR float futrts_ceil32(float x) { return ceilf(x); }
SCALAR_FUN_ATTR float futrts_nextafter32(float x, float y) { return nextafterf(x, y); }
SCALAR_FUN_ATTR float futrts_lerp32(float v0, float v1, float t) { return v0 + (v1 - v0) * t; }
SCALAR_FUN_ATTR float futrts_ldexp32(float x, int32_t y) { return ldexpf(x, y); }
SCALAR_FUN_ATTR float futrts_copysign32(float x, float y) { return copysignf(x, y); }
SCALAR_FUN_ATTR float futrts_mad32(float a, float b, float c) { return a * b + c; }
SCALAR_FUN_ATTR float futrts_fma32(float a, float b, float c) { return fmaf(a, b, c); }

#endif

#if defined(ISPC)

SCALAR_FUN_ATTR int32_t fptobits_f32_i32(float x) { return intbits(x); }
SCALAR_FUN_ATTR float bitstofp_i32_f32(int32_t x) { return floatbits(x); }
SCALAR_FUN_ATTR uniform int32_t fptobits_f32_i32(uniform float x) { return intbits(x); }
SCALAR_FUN_ATTR uniform float bitstofp_i32_f32(uniform int32_t x) { return floatbits(x); }

#else

SCALAR_FUN_ATTR int32_t fptobits_f32_i32(float x) {
  union {
    float f;
    int32_t t;
  } p;

  p.f = x;
  return p.t;
}

SCALAR_FUN_ATTR float bitstofp_i32_f32(int32_t x) {
  union {
    int32_t f;
    float t;
  } p;

  p.f = x;
  return p.t;
}
#endif

SCALAR_FUN_ATTR float fsignum32(float x) {
  return futrts_isnan32(x) ? x : (x > 0 ? 1 : 0) - (x < 0 ? 1 : 0);
}

#ifdef FUTHARK_F64_ENABLED

SCALAR_FUN_ATTR double bitstofp_i64_f64(int64_t x);
SCALAR_FUN_ATTR int64_t fptobits_f64_i64(double x);

#if defined(ISPC)

SCALAR_FUN_ATTR bool futrts_isinf64(float x) { return !isnan(x) && isnan(x - x); }
SCALAR_FUN_ATTR bool futrts_isfinite64(float x) { return !isnan(x) && !futrts_isinf64(x); }
SCALAR_FUN_ATTR double fdiv64(double x, double y) { return x / y; }
SCALAR_FUN_ATTR double fadd64(double x, double y) { return x + y; }
SCALAR_FUN_ATTR double fsub64(double x, double y) { return x - y; }
SCALAR_FUN_ATTR double fmul64(double x, double y) { return x * y; }
SCALAR_FUN_ATTR bool cmplt64(double x, double y) { return x < y; }
SCALAR_FUN_ATTR bool cmple64(double x, double y) { return x <= y; }
SCALAR_FUN_ATTR double sitofp_i8_f64(int8_t x) { return (double) x; }
SCALAR_FUN_ATTR double sitofp_i16_f64(int16_t x) { return (double) x; }
SCALAR_FUN_ATTR double sitofp_i32_f64(int32_t x) { return (double) x; }
SCALAR_FUN_ATTR double sitofp_i64_f64(int64_t x) { return (double) x; }
SCALAR_FUN_ATTR double uitofp_i8_f64(uint8_t x) { return (double) x; }
SCALAR_FUN_ATTR double uitofp_i16_f64(uint16_t x) { return (double) x; }
SCALAR_FUN_ATTR double uitofp_i32_f64(uint32_t x) { return (double) x; }
SCALAR_FUN_ATTR double uitofp_i64_f64(uint64_t x) { return (double) x; }
SCALAR_FUN_ATTR double fabs64(double x) { return abs(x); }
SCALAR_FUN_ATTR double fmax64(double x, double y) { return isnan(x) ? y : isnan(y) ? x : max(x, y); }
SCALAR_FUN_ATTR double fmin64(double x, double y) { return isnan(x) ? y : isnan(y) ? x : min(x, y); }

SCALAR_FUN_ATTR double fpow64(double a, double b) {
  float ret;
  foreach_active (i) {
      uniform float r = pow(extract(a, i), extract(b, i));
      ret = insert(ret, i, r);
  }
  return ret;
}
SCALAR_FUN_ATTR double futrts_log64(double x) { return futrts_isfinite64(x) || (futrts_isinf64(x) && x < 0)? log(x) : x; }
SCALAR_FUN_ATTR double futrts_log2_64(double x) { return futrts_log64(x)/log(2.0d); }
SCALAR_FUN_ATTR double futrts_log10_64(double x) { return futrts_log64(x)/log(10.0d); }

SCALAR_FUN_ATTR double futrts_log1p_64(double x) {
  if(x == -1.0d || (futrts_isinf64(x) && x > 0.0d)) return x / 0.0d;
  double y = 1.0d + x;
  double z = y - 1.0d;
  return log(y) - (z-x)/y;
}

SCALAR_FUN_ATTR double futrts_sqrt64(double x) { return sqrt(x); }
SCALAR_FUN_ATTR double futrts_rsqrt64(double x) { return 1/sqrt(x); }

SCALAR_FUN_ATTR double futrts_cbrt64(double x) {
  double res;
  foreach_active (i) {
    uniform double r = cbrtf(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}
SCALAR_FUN_ATTR double futrts_exp64(double x) { return exp(x); }
SCALAR_FUN_ATTR double futrts_cos64(double x) { return cos(x); }
SCALAR_FUN_ATTR double futrts_cospi64(double x) { return cos(M_PI*x); }
SCALAR_FUN_ATTR double futrts_sin64(double x) { return sin(x); }
SCALAR_FUN_ATTR double futrts_sinpi64(double x) { return sin(M_PI*x); }
SCALAR_FUN_ATTR double futrts_tan64(double x) { return tan(x); }
SCALAR_FUN_ATTR double futrts_tanpi64(double x) { return tan(M_PI*x); }
SCALAR_FUN_ATTR double futrts_acos64(double x) { return acos(x); }
SCALAR_FUN_ATTR double futrts_acospi64(double x) { return acos(x)/M_PI; }
SCALAR_FUN_ATTR double futrts_asin64(double x) { return asin(x); }
SCALAR_FUN_ATTR double futrts_asinpi64(double x) { return asin(x)/M_PI; }
SCALAR_FUN_ATTR double futrts_atan64(double x) { return atan(x); }
SCALAR_FUN_ATTR double futrts_atanpi64(double x) { return atan(x)/M_PI; }
SCALAR_FUN_ATTR double futrts_cosh64(double x) { return (exp(x)+exp(-x)) / 2.0d; }
SCALAR_FUN_ATTR double futrts_sinh64(double x) { return (exp(x)-exp(-x)) / 2.0d; }
SCALAR_FUN_ATTR double futrts_tanh64(double x) { return futrts_sinh64(x)/futrts_cosh64(x); }

SCALAR_FUN_ATTR double futrts_acosh64(double x) {
  double f = x+sqrt(x*x-1.0d);
  if(futrts_isfinite64(f)) return log(f);
  return f;
}

SCALAR_FUN_ATTR double futrts_asinh64(double x) {
  double f = x+sqrt(x*x+1.0d);
  if(futrts_isfinite64(f)) return log(f);
  return f;
}

SCALAR_FUN_ATTR double futrts_atanh64(double x) {
  double f = (1.0d+x)/(1.0d-x);
  if(futrts_isfinite64(f)) return log(f)/2.0d;
  return f;
}
SCALAR_FUN_ATTR double futrts_atan2_64(double x, double y) { return atan2(x, y); }

SCALAR_FUN_ATTR double futrts_atan2pi_64(double x, double y) { return atan2(x, y) / M_PI; }

extern "C" unmasked uniform double hypot(uniform double x, uniform double y);
SCALAR_FUN_ATTR double futrts_hypot64(double x, double y) {
  double res;
  foreach_active (i) {
    uniform double r = hypot(extract(x, i), extract(y, i));
    res = insert(res, i, r);
  }
  return res;
}

extern "C" unmasked uniform double tgamma(uniform double x);
SCALAR_FUN_ATTR double futrts_gamma64(double x) {
  double res;
  foreach_active (i) {
    uniform double r = tgamma(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

extern "C" unmasked uniform double lgamma(uniform double x);
SCALAR_FUN_ATTR double futrts_lgamma64(double x) {
  double res;
  foreach_active (i) {
    uniform double r = lgamma(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

extern "C" unmasked uniform double erf(uniform double x);
SCALAR_FUN_ATTR double futrts_erf64(double x) {
  double res;
  foreach_active (i) {
    uniform double r = erf(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

extern "C" unmasked uniform double erfc(uniform double x);
SCALAR_FUN_ATTR double futrts_erfc64(double x) {
  double res;
  foreach_active (i) {
    uniform double r = erfc(extract(x, i));
    res = insert(res, i, r);
  }
  return res;
}

SCALAR_FUN_ATTR double futrts_fma64(double a, double b, double c) { return a * b + c; }
SCALAR_FUN_ATTR double futrts_round64(double x) { return round(x); }
SCALAR_FUN_ATTR double futrts_ceil64(double x) { return ceil(x); }

extern "C" unmasked uniform double nextafter(uniform float x, uniform double y);
SCALAR_FUN_ATTR float futrts_nextafter64(double x, double y) {
  double res;
  foreach_active (i) {
    uniform double r = nextafter(extract(x, i), extract(y, i));
    res = insert(res, i, r);
  }
  return res;
}

SCALAR_FUN_ATTR double futrts_floor64(double x) { return floor(x); }
SCALAR_FUN_ATTR bool futrts_isnan64(double x) { return isnan(x); }

SCALAR_FUN_ATTR int8_t fptosi_f64_i8(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (int8_t) x;
  }
}

SCALAR_FUN_ATTR int16_t fptosi_f64_i16(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (int16_t) x;
  }
}

SCALAR_FUN_ATTR int32_t fptosi_f64_i32(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (int32_t) x;
  }
}

SCALAR_FUN_ATTR int64_t fptosi_f64_i64(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (int64_t) x;
  }
}

SCALAR_FUN_ATTR uint8_t fptoui_f64_i8(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (uint8_t) (int8_t) x;
  }
}

SCALAR_FUN_ATTR uint16_t fptoui_f64_i16(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (uint16_t) (int16_t) x;
  }
}

SCALAR_FUN_ATTR uint32_t fptoui_f64_i32(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (uint32_t) (int32_t) x;
  }
}

SCALAR_FUN_ATTR uint64_t fptoui_f64_i64(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (uint64_t) (int64_t) x;
  }
}

SCALAR_FUN_ATTR bool ftob_f64_bool(double x) { return x != 0.0; }
SCALAR_FUN_ATTR double btof_bool_f64(bool x) { return x ? 1.0 : 0.0; }

SCALAR_FUN_ATTR int64_t fptobits_f64_i64(double x) {
  int64_t res;
  foreach_active (i) {
    uniform double tmp = extract(x, i);
    uniform int64_t r = *((uniform int64_t* uniform)&tmp);
    res = insert(res, i, r);
  }
  return res;
}

SCALAR_FUN_ATTR double bitstofp_i64_f64(int64_t x) {
  double res;
  foreach_active (i) {
    uniform int64_t tmp = extract(x, i);
    uniform double r = *((uniform double* uniform)&tmp);
    res = insert(res, i, r);
  }
  return res;
}

SCALAR_FUN_ATTR uniform int64_t fptobits_f64_i64(uniform double x) {
  return intbits(x);
}

SCALAR_FUN_ATTR uniform double bitstofp_i64_f64(uniform int64_t x) {
  return doublebits(x);
}

SCALAR_FUN_ATTR double fmod64(double x, double y) {
  return x - y * trunc(x/y);
}

SCALAR_FUN_ATTR double fsignum64(double x) {
  return futrts_isnan64(x) ? x : (x > 0 ? 1.0d : 0.0d) - (x < 0 ? 1.0d : 0.0d);
}

SCALAR_FUN_ATTR double futrts_lerp64(double v0, double v1, double t) {
  return v0 + (v1 - v0) * t;
}

SCALAR_FUN_ATTR double futrts_ldexp64(double x, int32_t y) {
  return x * pow((uniform double)2.0, (double)y);
}

SCALAR_FUN_ATTR double futrts_copysign64(double x, double y) {
  int64_t xb = fptobits_f64_i64(x);
  int64_t yb = fptobits_f64_i64(y);
  return bitstofp_i64_f64((xb & ~(((int64_t)1)<<63)) | (yb & (((int64_t)1)<<63)));
}

SCALAR_FUN_ATTR double futrts_mad64(double a, double b, double c) { return a * b + c; }
SCALAR_FUN_ATTR float fpconv_f32_f32(float x) { return (float) x; }
SCALAR_FUN_ATTR double fpconv_f32_f64(float x) { return (double) x; }
SCALAR_FUN_ATTR float fpconv_f64_f32(double x) { return (float) x; }
SCALAR_FUN_ATTR double fpconv_f64_f64(double x) { return (double) x; }

#else

SCALAR_FUN_ATTR double fdiv64(double x, double y) { return x / y; }
SCALAR_FUN_ATTR double fadd64(double x, double y) { return x + y; }
SCALAR_FUN_ATTR double fsub64(double x, double y) { return x - y; }
SCALAR_FUN_ATTR double fmul64(double x, double y) { return x * y; }
SCALAR_FUN_ATTR bool cmplt64(double x, double y) { return x < y; }
SCALAR_FUN_ATTR bool cmple64(double x, double y) { return x <= y; }
SCALAR_FUN_ATTR double sitofp_i8_f64(int8_t x) { return (double) x; }
SCALAR_FUN_ATTR double sitofp_i16_f64(int16_t x) { return (double) x; }
SCALAR_FUN_ATTR double sitofp_i32_f64(int32_t x) { return (double) x; }
SCALAR_FUN_ATTR double sitofp_i64_f64(int64_t x) { return (double) x; }
SCALAR_FUN_ATTR double uitofp_i8_f64(uint8_t x) { return (double) x; }
SCALAR_FUN_ATTR double uitofp_i16_f64(uint16_t x) { return (double) x; }
SCALAR_FUN_ATTR double uitofp_i32_f64(uint32_t x) { return (double) x; }
SCALAR_FUN_ATTR double uitofp_i64_f64(uint64_t x) { return (double) x; }
SCALAR_FUN_ATTR double fabs64(double x) { return fabs(x); }
SCALAR_FUN_ATTR double fmax64(double x, double y) { return fmax(x, y); }
SCALAR_FUN_ATTR double fmin64(double x, double y) { return fmin(x, y); }
SCALAR_FUN_ATTR double fpow64(double x, double y) { return pow(x, y); }
SCALAR_FUN_ATTR double futrts_log64(double x) { return log(x); }
SCALAR_FUN_ATTR double futrts_log2_64(double x) { return log2(x); }
SCALAR_FUN_ATTR double futrts_log10_64(double x) { return log10(x); }
SCALAR_FUN_ATTR double futrts_log1p_64(double x) { return log1p(x); }
SCALAR_FUN_ATTR double futrts_sqrt64(double x) { return sqrt(x); }
SCALAR_FUN_ATTR double futrts_rsqrt64(double x) { return 1/sqrt(x); }
SCALAR_FUN_ATTR double futrts_cbrt64(double x) { return cbrt(x); }
SCALAR_FUN_ATTR double futrts_exp64(double x) { return exp(x); }
SCALAR_FUN_ATTR double futrts_cos64(double x) { return cos(x); }

SCALAR_FUN_ATTR double futrts_cospi64(double x) {
#ifdef __OPENCL_VERSION__
  return cospi(x);
#elif defined(__CUDA_ARCH__)
  return cospi(x);
#else
  return cos(M_PI*x);
#endif
}

SCALAR_FUN_ATTR double futrts_sin64(double x) {
  return sin(x);
}

SCALAR_FUN_ATTR double futrts_sinpi64(double x) {
#ifdef __OPENCL_VERSION__
  return sinpi(x);
#elif defined(__CUDA_ARCH__)
  return sinpi(x);
#else
  return sin(M_PI*x);
#endif
}

SCALAR_FUN_ATTR double futrts_tan64(double x) {
  return tan(x);
}

SCALAR_FUN_ATTR double futrts_tanpi64(double x) {
#ifdef __OPENCL_VERSION__
  return tanpi(x);
#else
  return tan(M_PI*x);
#endif
}

SCALAR_FUN_ATTR double futrts_acos64(double x) {
  return acos(x);
}

SCALAR_FUN_ATTR double futrts_acospi64(double x) {
#ifdef __OPENCL_VERSION__
  return acospi(x);
#else
  return acos(x) / M_PI;
#endif
}

SCALAR_FUN_ATTR double futrts_asin64(double x) {
  return asin(x);
}

SCALAR_FUN_ATTR double futrts_asinpi64(double x) {
#ifdef __OPENCL_VERSION__
  return asinpi(x);
#else
  return asin(x) / M_PI;
#endif
}

SCALAR_FUN_ATTR double futrts_atan64(double x) {
  return atan(x);
}

SCALAR_FUN_ATTR double futrts_atanpi64(double x) {
#ifdef __OPENCL_VERSION__
  return atanpi(x);
#else
  return atan(x) / M_PI;
#endif
}

SCALAR_FUN_ATTR double futrts_cosh64(double x) { return cosh(x); }
SCALAR_FUN_ATTR double futrts_sinh64(double x) { return sinh(x); }
SCALAR_FUN_ATTR double futrts_tanh64(double x) { return tanh(x); }
SCALAR_FUN_ATTR double futrts_acosh64(double x) { return acosh(x); }
SCALAR_FUN_ATTR double futrts_asinh64(double x) { return asinh(x); }
SCALAR_FUN_ATTR double futrts_atanh64(double x) { return atanh(x); }
SCALAR_FUN_ATTR double futrts_atan2_64(double x, double y) { return atan2(x, y); }

SCALAR_FUN_ATTR double futrts_atan2pi_64(double x, double y) {
#ifdef __OPENCL_VERSION__
  return atan2pi(x, y);
#else
  return atan2(x, y) / M_PI;
#endif
}

SCALAR_FUN_ATTR double futrts_hypot64(double x, double y) { return hypot(x, y); }
SCALAR_FUN_ATTR double futrts_gamma64(double x) { return tgamma(x); }
SCALAR_FUN_ATTR double futrts_lgamma64(double x) { return lgamma(x); }
SCALAR_FUN_ATTR double futrts_erf64(double x) { return erf(x); }
SCALAR_FUN_ATTR double futrts_erfc64(double x) { return erfc(x); }
SCALAR_FUN_ATTR double futrts_fma64(double a, double b, double c) { return fma(a, b, c); }
SCALAR_FUN_ATTR double futrts_round64(double x) { return rint(x); }
SCALAR_FUN_ATTR double futrts_ceil64(double x) { return ceil(x); }
SCALAR_FUN_ATTR float futrts_nextafter64(float x, float y) { return nextafter(x, y); }
SCALAR_FUN_ATTR double futrts_floor64(double x) { return floor(x); }
SCALAR_FUN_ATTR bool futrts_isnan64(double x) { return isnan(x); }
SCALAR_FUN_ATTR bool futrts_isinf64(double x) { return isinf(x); }

SCALAR_FUN_ATTR int8_t fptosi_f64_i8(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (int8_t) x;
  }
}

SCALAR_FUN_ATTR int16_t fptosi_f64_i16(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (int16_t) x;
  }
}

SCALAR_FUN_ATTR int32_t fptosi_f64_i32(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (int32_t) x;
  }
}

SCALAR_FUN_ATTR int64_t fptosi_f64_i64(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (int64_t) x;
  }
}

SCALAR_FUN_ATTR uint8_t fptoui_f64_i8(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (uint8_t) (int8_t) x;
  }
}

SCALAR_FUN_ATTR uint16_t fptoui_f64_i16(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (uint16_t) (int16_t) x;
  }
}

SCALAR_FUN_ATTR uint32_t fptoui_f64_i32(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (uint32_t) (int32_t) x;
  }
}

SCALAR_FUN_ATTR uint64_t fptoui_f64_i64(double x) {
  if (futrts_isnan64(x) || futrts_isinf64(x)) {
    return 0;
  } else {
    return (uint64_t) (int64_t) x;
  }
}

SCALAR_FUN_ATTR bool ftob_f64_bool(double x) { return x != 0; }
SCALAR_FUN_ATTR double btof_bool_f64(bool x) { return x ? 1 : 0; }

SCALAR_FUN_ATTR int64_t fptobits_f64_i64(double x) {
  union {
    double f;
    int64_t t;
  } p;

  p.f = x;
  return p.t;
}

SCALAR_FUN_ATTR double bitstofp_i64_f64(int64_t x) {
  union {
    int64_t f;
    double t;
  } p;

  p.f = x;
  return p.t;
}

SCALAR_FUN_ATTR double fmod64(double x, double y) {
  return fmod(x, y);
}

SCALAR_FUN_ATTR double fsignum64(double x) {
  return futrts_isnan64(x) ? x : (x > 0) - (x < 0);
}

SCALAR_FUN_ATTR double futrts_lerp64(double v0, double v1, double t) {
#ifdef __OPENCL_VERSION__
  return mix(v0, v1, t);
#else
  return v0 + (v1 - v0) * t;
#endif
}

SCALAR_FUN_ATTR double futrts_ldexp64(double x, int32_t y) {
  return ldexp(x, y);
}

SCALAR_FUN_ATTR float futrts_copysign64(double x, double y) {
  return copysign(x, y);
}

SCALAR_FUN_ATTR double futrts_mad64(double a, double b, double c) {
#ifdef __OPENCL_VERSION__
  return mad(a, b, c);
#else
  return a * b + c;
#endif
}

SCALAR_FUN_ATTR float fpconv_f32_f32(float x) { return (float) x; }
SCALAR_FUN_ATTR double fpconv_f32_f64(float x) { return (double) x; }
SCALAR_FUN_ATTR float fpconv_f64_f32(double x) { return (float) x; }
SCALAR_FUN_ATTR double fpconv_f64_f64(double x) { return (double) x; }

#endif

#endif

#define futrts_cond_f16(x,y,z) ((x) ? (y) : (z))
#define futrts_cond_f32(x,y,z) ((x) ? (y) : (z))
#define futrts_cond_f64(x,y,z) ((x) ? (y) : (z))

#define futrts_cond_i8(x,y,z) ((x) ? (y) : (z))
#define futrts_cond_i16(x,y,z) ((x) ? (y) : (z))
#define futrts_cond_i32(x,y,z) ((x) ? (y) : (z))
#define futrts_cond_i64(x,y,z) ((x) ? (y) : (z))

#define futrts_cond_bool(x,y,z) ((x) ? (y) : (z))
#define futrts_cond_unit(x,y,z) ((x) ? (y) : (z))

// End of scalar.h.
// Start of scalar_f16.h.

// Half-precision is emulated if needed (e.g. in straight C) with the
// native type used if possible.  The emulation works by typedef'ing
// 'float' to 'f16', and then implementing all operations on single
// precision.  To cut down on duplication, we use the same code for
// those Futhark functions that require just operators or casts.  The
// in-memory representation for arrays will still be 16 bits even
// under emulation, so the compiler will have to be careful when
// generating reads or writes.

#if !defined(cl_khr_fp16) && !(defined(__CUDA_ARCH__) && __CUDA_ARCH__ >= 600) && !(defined(ISPC))
#define EMULATE_F16
#endif

#if !defined(EMULATE_F16) && defined(__OPENCL_VERSION__)
#pragma OPENCL EXTENSION cl_khr_fp16 : enable
#endif

#ifdef EMULATE_F16

// Note that the half-precision storage format is still 16 bits - the
// compiler will have to be real careful!
typedef float f16;

#elif defined(ISPC)
typedef float16 f16;

#else

#ifdef __CUDA_ARCH__
#include <cuda_fp16.h>
#endif

typedef half f16;

#endif

// Some of these functions convert to single precision because half
// precision versions are not available.
SCALAR_FUN_ATTR f16 fadd16(f16 x, f16 y) { return x + y; }
SCALAR_FUN_ATTR f16 fsub16(f16 x, f16 y) { return x - y; }
SCALAR_FUN_ATTR f16 fmul16(f16 x, f16 y) { return x * y; }
SCALAR_FUN_ATTR bool cmplt16(f16 x, f16 y) { return x < y; }
SCALAR_FUN_ATTR bool cmple16(f16 x, f16 y) { return x <= y; }
SCALAR_FUN_ATTR f16 sitofp_i8_f16(int8_t x) { return (f16) x; }
SCALAR_FUN_ATTR f16 sitofp_i16_f16(int16_t x) { return (f16) x; }
SCALAR_FUN_ATTR f16 sitofp_i32_f16(int32_t x) { return (f16) x; }
SCALAR_FUN_ATTR f16 sitofp_i64_f16(int64_t x) { return (f16) x; }
SCALAR_FUN_ATTR f16 uitofp_i8_f16(uint8_t x) { return (f16) x; }
SCALAR_FUN_ATTR f16 uitofp_i16_f16(uint16_t x) { return (f16) x; }
SCALAR_FUN_ATTR f16 uitofp_i32_f16(uint32_t x) { return (f16) x; }
SCALAR_FUN_ATTR f16 uitofp_i64_f16(uint64_t x) { return (f16) x; }
SCALAR_FUN_ATTR int8_t fptosi_f16_i8(f16 x) { return (int8_t) (float) x; }
SCALAR_FUN_ATTR int16_t fptosi_f16_i16(f16 x) { return (int16_t) x; }
SCALAR_FUN_ATTR int32_t fptosi_f16_i32(f16 x) { return (int32_t) x; }
SCALAR_FUN_ATTR int64_t fptosi_f16_i64(f16 x) { return (int64_t) x; }
SCALAR_FUN_ATTR uint8_t fptoui_f16_i8(f16 x) { return (uint8_t) (float) x; }
SCALAR_FUN_ATTR uint16_t fptoui_f16_i16(f16 x) { return (uint16_t) x; }
SCALAR_FUN_ATTR uint32_t fptoui_f16_i32(f16 x) { return (uint32_t) x; }
SCALAR_FUN_ATTR uint64_t fptoui_f16_i64(f16 x) { return (uint64_t) x; }
SCALAR_FUN_ATTR bool ftob_f16_bool(f16 x) { return x != (f16)0; }
SCALAR_FUN_ATTR f16 btof_bool_f16(bool x) { return x ? 1 : 0; }

#ifndef EMULATE_F16

SCALAR_FUN_ATTR bool futrts_isnan16(f16 x) { return isnan((float)x); }

#ifdef __OPENCL_VERSION__

SCALAR_FUN_ATTR f16 fabs16(f16 x) { return fabs(x); }
SCALAR_FUN_ATTR f16 fmax16(f16 x, f16 y) { return fmax(x, y); }
SCALAR_FUN_ATTR f16 fmin16(f16 x, f16 y) { return fmin(x, y); }
SCALAR_FUN_ATTR f16 fpow16(f16 x, f16 y) { return pow(x, y); }

#elif defined(ISPC)

SCALAR_FUN_ATTR f16 fabs16(f16 x) { return abs(x); }
SCALAR_FUN_ATTR f16 fmax16(f16 x, f16 y) { return futrts_isnan16(x) ? y : futrts_isnan16(y) ? x : max(x, y); }
SCALAR_FUN_ATTR f16 fmin16(f16 x, f16 y) { return futrts_isnan16(x) ? y : futrts_isnan16(y) ? x : min(x, y); }
SCALAR_FUN_ATTR f16 fpow16(f16 x, f16 y) { return pow(x, y); }

#else // Assuming CUDA.

SCALAR_FUN_ATTR f16 fabs16(f16 x) { return fabsf(x); }
SCALAR_FUN_ATTR f16 fmax16(f16 x, f16 y) { return fmaxf(x, y); }
SCALAR_FUN_ATTR f16 fmin16(f16 x, f16 y) { return fminf(x, y); }
SCALAR_FUN_ATTR f16 fpow16(f16 x, f16 y) { return powf(x, y); }

#endif

#if defined(ISPC)
SCALAR_FUN_ATTR bool futrts_isinf16(float x) { return !futrts_isnan16(x) && futrts_isnan16(x - x); }
SCALAR_FUN_ATTR bool futrts_isfinite16(float x) { return !futrts_isnan16(x) && !futrts_isinf16(x); }
#else
SCALAR_FUN_ATTR bool futrts_isinf16(f16 x) { return isinf((float)x); }
#endif

#ifdef __OPENCL_VERSION__
SCALAR_FUN_ATTR f16 futrts_log16(f16 x) { return log(x); }
SCALAR_FUN_ATTR f16 futrts_log2_16(f16 x) { return log2(x); }
SCALAR_FUN_ATTR f16 futrts_log10_16(f16 x) { return log10(x); }
SCALAR_FUN_ATTR f16 futrts_log1p_16(f16 x) { return log1p(x); }
SCALAR_FUN_ATTR f16 futrts_sqrt16(f16 x) { return sqrt(x); }
SCALAR_FUN_ATTR f16 futrts_rsqrt16(f16 x) { return rsqrt(x); }
SCALAR_FUN_ATTR f16 futrts_cbrt16(f16 x) { return cbrt(x); }
SCALAR_FUN_ATTR f16 futrts_exp16(f16 x) { return exp(x); }
SCALAR_FUN_ATTR f16 futrts_cos16(f16 x) { return cos(x); }
SCALAR_FUN_ATTR f16 futrts_cospi16(f16 x) { return cospi(x); }
SCALAR_FUN_ATTR f16 futrts_sin16(f16 x) { return sin(x); }
SCALAR_FUN_ATTR f16 futrts_sinpi16(f16 x) { return sinpi(x); }
SCALAR_FUN_ATTR f16 futrts_tan16(f16 x) { return tan(x); }
SCALAR_FUN_ATTR f16 futrts_tanpi16(f16 x) { return tanpi(x); }
SCALAR_FUN_ATTR f16 futrts_acos16(f16 x) { return acos(x); }
SCALAR_FUN_ATTR f16 futrts_acospi16(f16 x) { return acospi(x); }
SCALAR_FUN_ATTR f16 futrts_asin16(f16 x) { return asin(x); }
SCALAR_FUN_ATTR f16 futrts_asinpi16(f16 x) { return asinpi(x); }
SCALAR_FUN_ATTR f16 futrts_atan16(f16 x) { return atan(x); }
SCALAR_FUN_ATTR f16 futrts_atanpi16(f16 x) { return atanpi(x); }
SCALAR_FUN_ATTR f16 futrts_cosh16(f16 x) { return cosh(x); }
SCALAR_FUN_ATTR f16 futrts_sinh16(f16 x) { return sinh(x); }
SCALAR_FUN_ATTR f16 futrts_tanh16(f16 x) { return tanh(x); }
SCALAR_FUN_ATTR f16 futrts_acosh16(f16 x) { return acosh(x); }
SCALAR_FUN_ATTR f16 futrts_asinh16(f16 x) { return asinh(x); }
SCALAR_FUN_ATTR f16 futrts_atanh16(f16 x) { return atanh(x); }
SCALAR_FUN_ATTR f16 futrts_atan2_16(f16 x, f16 y) { return atan2(x, y); }
SCALAR_FUN_ATTR f16 futrts_atan2pi_16(f16 x, f16 y) { return atan2pi(x, y); }
SCALAR_FUN_ATTR f16 futrts_hypot16(f16 x, f16 y) { return hypot(x, y); }
SCALAR_FUN_ATTR f16 futrts_gamma16(f16 x) { return tgamma(x); }
SCALAR_FUN_ATTR f16 futrts_lgamma16(f16 x) { return lgamma(x); }
SCALAR_FUN_ATTR f16 futrts_erf16(f16 x) { return erf(x); }
SCALAR_FUN_ATTR f16 futrts_erfc16(f16 x) { return erfc(x); }
SCALAR_FUN_ATTR f16 fmod16(f16 x, f16 y) { return fmod(x, y); }
SCALAR_FUN_ATTR f16 futrts_round16(f16 x) { return rint(x); }
SCALAR_FUN_ATTR f16 futrts_floor16(f16 x) { return floor(x); }
SCALAR_FUN_ATTR f16 futrts_ceil16(f16 x) { return ceil(x); }
SCALAR_FUN_ATTR f16 futrts_nextafter16(f16 x, f16 y) { return nextafter(x, y); }
SCALAR_FUN_ATTR f16 futrts_lerp16(f16 v0, f16 v1, f16 t) { return mix(v0, v1, t); }
SCALAR_FUN_ATTR f16 futrts_ldexp16(f16 x, int32_t y) { return ldexp(x, y); }
SCALAR_FUN_ATTR f16 futrts_copysign16(f16 x, f16 y) { return copysign(x, y); }
SCALAR_FUN_ATTR f16 futrts_mad16(f16 a, f16 b, f16 c) { return mad(a, b, c); }
SCALAR_FUN_ATTR f16 futrts_fma16(f16 a, f16 b, f16 c) { return fma(a, b, c); }

#elif defined(ISPC)

SCALAR_FUN_ATTR f16 futrts_log16(f16 x) { return futrts_isfinite16(x) || (futrts_isinf16(x) && x < 0) ? log(x) : x; }
SCALAR_FUN_ATTR f16 futrts_log2_16(f16 x) { return futrts_log16(x) / log(2.0f16); }
SCALAR_FUN_ATTR f16 futrts_log10_16(f16 x) { return futrts_log16(x) / log(10.0f16); }
SCALAR_FUN_ATTR f16 futrts_log1p_16(f16 x) {
  if(x == -1.0f16 || (futrts_isinf16(x) && x > 0.0f16)) return x / 0.0f16;
  f16 y = 1.0f16 + x;
  f16 z = y - 1.0f16;
  return log(y) - (z-x)/y;
}
SCALAR_FUN_ATTR f16 futrts_sqrt16(f16 x) { return (float16)sqrt((float)x); }
SCALAR_FUN_ATTR f16 futrts_rsqrt16(f16 x) { return (float16)1/sqrt((float)x); }
SCALAR_FUN_ATTR f16 futrts_exp16(f16 x) { return exp(x); }
SCALAR_FUN_ATTR f16 futrts_cos16(f16 x) { return (float16)cos((float)x); }
SCALAR_FUN_ATTR f16 futrts_cospi16(f16 x) { return (float16)cos((float)M_PI*(float)x); }
SCALAR_FUN_ATTR f16 futrts_sin16(f16 x) { return (float16)sin((float)x); }
SCALAR_FUN_ATTR f16 futrts_sinpi16(f16 x) { return (float16)sin((float)M_PI*(float)x); }
SCALAR_FUN_ATTR f16 futrts_tan16(f16 x) { return (float16)tan((float)x); }
SCALAR_FUN_ATTR f16 futrts_tanpi16(f16 x) { return (float16)(tan((float)M_PI*(float)x)); }
SCALAR_FUN_ATTR f16 futrts_acos16(f16 x) { return (float16)acos((float)x); }
SCALAR_FUN_ATTR f16 futrts_acospi16(f16 x) { return (float16)(acos((float)x)/(float)M_PI); }
SCALAR_FUN_ATTR f16 futrts_asin16(f16 x) { return (float16)asin((float)x); }
SCALAR_FUN_ATTR f16 futrts_asinpi16(f16 x) { return (float16)(asin((float)x)/(float)M_PI); }
SCALAR_FUN_ATTR f16 futrts_atan16(f16 x) { return (float16)atan((float)x); }
SCALAR_FUN_ATTR f16 futrts_atanpi16(f16 x) { return (float16)(atan((float)x)/(float)M_PI); }
SCALAR_FUN_ATTR f16 futrts_cosh16(f16 x) { return (exp(x)+exp(-x)) / 2.0f16; }
SCALAR_FUN_ATTR f16 futrts_sinh16(f16 x) { return (exp(x)-exp(-x)) / 2.0f16; }
SCALAR_FUN_ATTR f16 futrts_tanh16(f16 x) { return futrts_sinh16(x)/futrts_cosh16(x); }
SCALAR_FUN_ATTR f16 futrts_acosh16(f16 x) {
  float16 f = x+(float16)sqrt((float)(x*x-1));
  if(futrts_isfinite16(f)) return log(f);
  return f;
}
SCALAR_FUN_ATTR f16 futrts_asinh16(f16 x) {
  float16 f = x+(float16)sqrt((float)(x*x+1));
  if(futrts_isfinite16(f)) return log(f);
  return f;
}
SCALAR_FUN_ATTR f16 futrts_atanh16(f16 x) {
  float16 f = (1+x)/(1-x);
  if(futrts_isfinite16(f)) return log(f)/2.0f16;
  return f;
}
SCALAR_FUN_ATTR f16 futrts_atan2_16(f16 x, f16 y) { return (float16)atan2((float)x, (float)y); }
SCALAR_FUN_ATTR f16 futrts_atan2pi_16(f16 x, f16 y) { return (float16)(atan2((float)x, (float)y)/(float)M_PI); }
SCALAR_FUN_ATTR f16 futrts_hypot16(f16 x, f16 y) { return (float16)futrts_hypot32((float)x, (float)y); }

extern "C" unmasked uniform float tgammaf(uniform float x);
SCALAR_FUN_ATTR f16 futrts_gamma16(f16 x) {
  f16 res;
  foreach_active (i) {
    uniform f16 r = (f16)tgammaf(extract((float)x, i));
    res = insert(res, i, r);
  }
  return res;
}

extern "C" unmasked uniform float lgammaf(uniform float x);
SCALAR_FUN_ATTR f16 futrts_lgamma16(f16 x) {
  f16 res;
  foreach_active (i) {
    uniform f16 r = (f16)lgammaf(extract((float)x, i));
    res = insert(res, i, r);
  }
  return res;
}
SCALAR_FUN_ATTR f16 futrts_cbrt16(f16 x) { return (f16)futrts_cbrt32((float)x); }
SCALAR_FUN_ATTR f16 futrts_erf16(f16 x) { return (f16)futrts_erf32((float)x); }
SCALAR_FUN_ATTR f16 futrts_erfc16(f16 x) { return (f16)futrts_erfc32((float)x); }
SCALAR_FUN_ATTR f16 fmod16(f16 x, f16 y) { return x - y * (float16)trunc((float) (x/y)); }
SCALAR_FUN_ATTR f16 futrts_round16(f16 x) { return (float16)round((float)x); }
SCALAR_FUN_ATTR f16 futrts_floor16(f16 x) { return (float16)floor((float)x); }
SCALAR_FUN_ATTR f16 futrts_ceil16(f16 x) { return (float16)ceil((float)x); }
SCALAR_FUN_ATTR f16 futrts_nextafter16(f16 x, f16 y) { return (float16)futrts_nextafter32((float)x, (float) y); }
SCALAR_FUN_ATTR f16 futrts_lerp16(f16 v0, f16 v1, f16 t) { return v0 + (v1 - v0) * t; }
SCALAR_FUN_ATTR f16 futrts_ldexp16(f16 x, int32_t y) { return futrts_ldexp32((float)x, y); }
SCALAR_FUN_ATTR f16 futrts_copysign16(f16 x, f16 y) { return futrts_copysign32((float)x, y); }
SCALAR_FUN_ATTR f16 futrts_mad16(f16 a, f16 b, f16 c) { return a * b + c; }
SCALAR_FUN_ATTR f16 futrts_fma16(f16 a, f16 b, f16 c) { return a * b + c; }

#else // Assume CUDA.

SCALAR_FUN_ATTR f16 futrts_log16(f16 x) { return hlog(x); }
SCALAR_FUN_ATTR f16 futrts_log2_16(f16 x) { return hlog2(x); }
SCALAR_FUN_ATTR f16 futrts_log10_16(f16 x) { return hlog10(x); }
SCALAR_FUN_ATTR f16 futrts_log1p_16(f16 x) { return (f16)log1pf((float)x); }
SCALAR_FUN_ATTR f16 futrts_sqrt16(f16 x) { return hsqrt(x); }
SCALAR_FUN_ATTR f16 futrts_rsqrt16(f16 x) { return hrsqrt(x); }
SCALAR_FUN_ATTR f16 futrts_cbrt16(f16 x) { return cbrtf(x); }
SCALAR_FUN_ATTR f16 futrts_exp16(f16 x) { return hexp(x); }
SCALAR_FUN_ATTR f16 futrts_cos16(f16 x) { return hcos(x); }
SCALAR_FUN_ATTR f16 futrts_cospi16(f16 x) { return hcos((f16)M_PI*x); }
SCALAR_FUN_ATTR f16 futrts_sin16(f16 x) { return hsin(x); }
SCALAR_FUN_ATTR f16 futrts_sinpi16(f16 x) { return hsin((f16)M_PI*x); }
SCALAR_FUN_ATTR f16 futrts_tan16(f16 x) { return tanf(x); }
SCALAR_FUN_ATTR f16 futrts_tanpi16(f16 x) { return tanf((f16)M_PI*x); }
SCALAR_FUN_ATTR f16 futrts_acos16(f16 x) { return acosf(x); }
SCALAR_FUN_ATTR f16 futrts_acospi16(f16 x) { return (f16)acosf(x)/(f16)M_PI; }
SCALAR_FUN_ATTR f16 futrts_asin16(f16 x) { return asinf(x); }
SCALAR_FUN_ATTR f16 futrts_asinpi16(f16 x) { return (f16)asinf(x)/(f16)M_PI; }
SCALAR_FUN_ATTR f16 futrts_atan16(f16 x) { return (f16)atanf(x); }
SCALAR_FUN_ATTR f16 futrts_atanpi16(f16 x) { return (f16)atanf(x)/(f16)M_PI; }
SCALAR_FUN_ATTR f16 futrts_cosh16(f16 x) { return coshf(x); }
SCALAR_FUN_ATTR f16 futrts_sinh16(f16 x) { return sinhf(x); }
SCALAR_FUN_ATTR f16 futrts_tanh16(f16 x) { return tanhf(x); }
SCALAR_FUN_ATTR f16 futrts_acosh16(f16 x) { return acoshf(x); }
SCALAR_FUN_ATTR f16 futrts_asinh16(f16 x) { return asinhf(x); }
SCALAR_FUN_ATTR f16 futrts_atanh16(f16 x) { return atanhf(x); }
SCALAR_FUN_ATTR f16 futrts_atan2_16(f16 x, f16 y) { return (f16)atan2f(x, y); }
SCALAR_FUN_ATTR f16 futrts_atan2pi_16(f16 x, f16 y) { return (f16)atan2f(x, y)/(f16)M_PI; }
SCALAR_FUN_ATTR f16 futrts_hypot16(f16 x, f16 y) { return hypotf(x, y); }
SCALAR_FUN_ATTR f16 futrts_gamma16(f16 x) { return tgammaf(x); }
SCALAR_FUN_ATTR f16 futrts_lgamma16(f16 x) { return lgammaf(x); }
SCALAR_FUN_ATTR f16 futrts_erf16(f16 x) { return erff(x); }
SCALAR_FUN_ATTR f16 futrts_erfc16(f16 x) { return erfcf(x); }
SCALAR_FUN_ATTR f16 fmod16(f16 x, f16 y) { return fmodf(x, y); }
SCALAR_FUN_ATTR f16 futrts_round16(f16 x) { return rintf(x); }
SCALAR_FUN_ATTR f16 futrts_floor16(f16 x) { return hfloor(x); }
SCALAR_FUN_ATTR f16 futrts_ceil16(f16 x) { return hceil(x); }
SCALAR_FUN_ATTR f16 futrts_nextafter16(f16 x, f16 y) { return __ushort_as_half(halfbitsnextafter(__half_as_ushort(x), __half_as_ushort(y))); }
SCALAR_FUN_ATTR f16 futrts_lerp16(f16 v0, f16 v1, f16 t) { return v0 + (v1 - v0) * t; }
SCALAR_FUN_ATTR f16 futrts_ldexp16(f16 x, int32_t y) { return futrts_ldexp32((float)x, y); }
SCALAR_FUN_ATTR f16 futrts_copysign16(f16 x, f16 y) { return futrts_copysign32((float)x, y); }
SCALAR_FUN_ATTR f16 futrts_mad16(f16 a, f16 b, f16 c) { return a * b + c; }
SCALAR_FUN_ATTR f16 futrts_fma16(f16 a, f16 b, f16 c) { return fmaf(a, b, c); }

#endif

// The CUDA __half type cannot be put in unions for some reason, so we
// use bespoke conversion functions instead.
#ifdef __CUDA_ARCH__
SCALAR_FUN_ATTR int16_t fptobits_f16_i16(f16 x) { return __half_as_ushort(x); }
SCALAR_FUN_ATTR f16 bitstofp_i16_f16(int16_t x) { return __ushort_as_half(x); }
#elif defined(ISPC)
SCALAR_FUN_ATTR int16_t fptobits_f16_i16(f16 x) { varying int16_t y = *((varying int16_t * uniform)&x); return y;
}
SCALAR_FUN_ATTR f16 bitstofp_i16_f16(int16_t x) { varying f16 y = *((varying f16 * uniform)&x); return y; }
#else
SCALAR_FUN_ATTR int16_t fptobits_f16_i16(f16 x) {
  union {
    f16 f;
    int16_t t;
  } p;

  p.f = x;
  return p.t;
}

SCALAR_FUN_ATTR f16 bitstofp_i16_f16(int16_t x) {
  union {
    int16_t f;
    f16 t;
  } p;

  p.f = x;
  return p.t;
}
#endif

#else // No native f16 - emulate.

SCALAR_FUN_ATTR f16 fabs16(f16 x) { return fabs32(x); }
SCALAR_FUN_ATTR f16 fmax16(f16 x, f16 y) { return fmax32(x, y); }
SCALAR_FUN_ATTR f16 fmin16(f16 x, f16 y) { return fmin32(x, y); }
SCALAR_FUN_ATTR f16 fpow16(f16 x, f16 y) { return fpow32(x, y); }
SCALAR_FUN_ATTR bool futrts_isnan16(f16 x) { return futrts_isnan32(x); }
SCALAR_FUN_ATTR bool futrts_isinf16(f16 x) { return futrts_isinf32(x); }
SCALAR_FUN_ATTR f16 futrts_log16(f16 x) { return futrts_log32(x); }
SCALAR_FUN_ATTR f16 futrts_log2_16(f16 x) { return futrts_log2_32(x); }
SCALAR_FUN_ATTR f16 futrts_log10_16(f16 x) { return futrts_log10_32(x); }
SCALAR_FUN_ATTR f16 futrts_log1p_16(f16 x) { return futrts_log1p_32(x); }
SCALAR_FUN_ATTR f16 futrts_sqrt16(f16 x) { return futrts_sqrt32(x); }
SCALAR_FUN_ATTR f16 futrts_rsqrt16(f16 x) { return futrts_rsqrt32(x); }
SCALAR_FUN_ATTR f16 futrts_cbrt16(f16 x) { return futrts_cbrt32(x); }
SCALAR_FUN_ATTR f16 futrts_exp16(f16 x) { return futrts_exp32(x); }
SCALAR_FUN_ATTR f16 futrts_cos16(f16 x) { return futrts_cos32(x); }
SCALAR_FUN_ATTR f16 futrts_cospi16(f16 x) { return futrts_cospi32(x); }
SCALAR_FUN_ATTR f16 futrts_sin16(f16 x) { return futrts_sin32(x); }
SCALAR_FUN_ATTR f16 futrts_sinpi16(f16 x) { return futrts_sinpi32(x); }
SCALAR_FUN_ATTR f16 futrts_tan16(f16 x) { return futrts_tan32(x); }
SCALAR_FUN_ATTR f16 futrts_tanpi16(f16 x) { return futrts_tanpi32(x); }
SCALAR_FUN_ATTR f16 futrts_acos16(f16 x) { return futrts_acos32(x); }
SCALAR_FUN_ATTR f16 futrts_acospi16(f16 x) { return futrts_acospi32(x); }
SCALAR_FUN_ATTR f16 futrts_asin16(f16 x) { return futrts_asin32(x); }
SCALAR_FUN_ATTR f16 futrts_asinpi16(f16 x) { return futrts_asinpi32(x); }
SCALAR_FUN_ATTR f16 futrts_atan16(f16 x) { return futrts_atan32(x); }
SCALAR_FUN_ATTR f16 futrts_atanpi16(f16 x) { return futrts_atanpi32(x); }
SCALAR_FUN_ATTR f16 futrts_cosh16(f16 x) { return futrts_cosh32(x); }
SCALAR_FUN_ATTR f16 futrts_sinh16(f16 x) { return futrts_sinh32(x); }
SCALAR_FUN_ATTR f16 futrts_tanh16(f16 x) { return futrts_tanh32(x); }
SCALAR_FUN_ATTR f16 futrts_acosh16(f16 x) { return futrts_acosh32(x); }
SCALAR_FUN_ATTR f16 futrts_asinh16(f16 x) { return futrts_asinh32(x); }
SCALAR_FUN_ATTR f16 futrts_atanh16(f16 x) { return futrts_atanh32(x); }
SCALAR_FUN_ATTR f16 futrts_atan2_16(f16 x, f16 y) { return futrts_atan2_32(x, y); }
SCALAR_FUN_ATTR f16 futrts_atan2pi_16(f16 x, f16 y) { return futrts_atan2pi_32(x, y); }
SCALAR_FUN_ATTR f16 futrts_hypot16(f16 x, f16 y) { return futrts_hypot32(x, y); }
SCALAR_FUN_ATTR f16 futrts_gamma16(f16 x) { return futrts_gamma32(x); }
SCALAR_FUN_ATTR f16 futrts_lgamma16(f16 x) { return futrts_lgamma32(x); }
SCALAR_FUN_ATTR f16 futrts_erf16(f16 x) { return futrts_erf32(x); }
SCALAR_FUN_ATTR f16 futrts_erfc16(f16 x) { return futrts_erfc32(x); }
SCALAR_FUN_ATTR f16 fmod16(f16 x, f16 y) { return fmod32(x, y); }
SCALAR_FUN_ATTR f16 futrts_round16(f16 x) { return futrts_round32(x); }
SCALAR_FUN_ATTR f16 futrts_floor16(f16 x) { return futrts_floor32(x); }
SCALAR_FUN_ATTR f16 futrts_ceil16(f16 x) { return futrts_ceil32(x); }
SCALAR_FUN_ATTR f16 futrts_nextafter16(f16 x, f16 y) { return halfbits2float(halfbitsnextafter(float2halfbits(x), float2halfbits(y))); }
SCALAR_FUN_ATTR f16 futrts_lerp16(f16 v0, f16 v1, f16 t) { return futrts_lerp32(v0, v1, t); }
SCALAR_FUN_ATTR f16 futrts_ldexp16(f16 x, int32_t y) { return futrts_ldexp32(x, y); }
SCALAR_FUN_ATTR f16 futrts_copysign16(f16 x, f16 y) { return futrts_copysign32((float)x, y); }
SCALAR_FUN_ATTR f16 futrts_mad16(f16 a, f16 b, f16 c) { return futrts_mad32(a, b, c); }
SCALAR_FUN_ATTR f16 futrts_fma16(f16 a, f16 b, f16 c) { return futrts_fma32(a, b, c); }

// Even when we are using an OpenCL that does not support cl_khr_fp16,
// it must still support vload_half for actually creating a
// half-precision number, which can then be efficiently converted to a
// float.  Similarly for vstore_half.
#ifdef __OPENCL_VERSION__

SCALAR_FUN_ATTR int16_t fptobits_f16_i16(f16 x) {
  int16_t y;
  // Violating strict aliasing here.
  vstore_half((float)x, 0, (half*)&y);
  return y;
}

SCALAR_FUN_ATTR f16 bitstofp_i16_f16(int16_t x) {
  return (f16)vload_half(0, (half*)&x);
}

#else
SCALAR_FUN_ATTR int16_t fptobits_f16_i16(f16 x) { return (int16_t)float2halfbits(x); }
SCALAR_FUN_ATTR f16 bitstofp_i16_f16(int16_t x) { return halfbits2float((uint16_t)x); }
SCALAR_FUN_ATTR f16 fsignum16(f16 x) { return futrts_isnan16(x) ? x : (x > 0 ? 1 : 0) - (x < 0 ? 1 : 0); }

#endif

#endif

SCALAR_FUN_ATTR float fpconv_f16_f16(f16 x) { return x; }
SCALAR_FUN_ATTR float fpconv_f16_f32(f16 x) { return x; }
SCALAR_FUN_ATTR f16 fpconv_f32_f16(float x) { return (f16) x; }

#ifdef FUTHARK_F64_ENABLED
SCALAR_FUN_ATTR double fpconv_f16_f64(f16 x) { return (double) x; }
#if defined(ISPC)
SCALAR_FUN_ATTR f16 fpconv_f64_f16(double x) { return (f16) ((float)x); }
#else
SCALAR_FUN_ATTR f16 fpconv_f64_f16(double x) { return (f16) x; }
#endif
#endif

// End of scalar_f16.h.

// Start of context_prototypes.h
//
// Prototypes for the functions in context.h, or that will be called
// from those functions, that need to be available very early.

struct futhark_context_config;
struct futhark_context;

static void set_error(struct futhark_context* ctx, char *error);

// These are called in context/config new/free functions and contain
// shared setup.  They are generated by the compiler itself.
static int init_constants(struct futhark_context*);
static int free_constants(struct futhark_context*);
static void setup_program(struct futhark_context* ctx);
static void teardown_program(struct futhark_context *ctx);

// Allocate host memory.  Must be freed with host_free().
static void host_alloc(struct futhark_context* ctx, size_t size, const char* tag, size_t* size_out, void** mem_out);
// Allocate memory allocated with host_alloc().
static void host_free(struct futhark_context* ctx, size_t size, const char* tag, void* mem);

// Log that a copy has occurred. The provenance may be NULL, if we do not know
// where this came from.
static void log_copy(struct futhark_context* ctx,
                     const char *kind, const char *provenance,
                     int r,
                     int64_t dst_offset, int64_t dst_strides[r],
                     int64_t src_offset, int64_t src_strides[r],
                     int64_t shape[r]);

static void log_transpose(struct futhark_context* ctx,
                          int64_t k, int64_t m, int64_t n);

static bool lmad_map_tr(int64_t *num_arrays_out, int64_t *n_out, int64_t *m_out,
                        int r,
                        const int64_t dst_strides[r],
                        const int64_t src_strides[r],
                        const int64_t shape[r]);

static bool lmad_contiguous(int r, int64_t strides[r], int64_t shape[r]);

static bool lmad_memcpyable(int r,
                            int64_t dst_strides[r], int64_t src_strides[r], int64_t shape[r]);

static void add_event(struct futhark_context* ctx,
                      const char* name,
                      const char* provenance,
                      struct kvs *kvs,
                      void* data,
                      event_report_fn f);

// Functions that must be defined by the backend.
static void backend_context_config_setup(struct futhark_context_config* cfg);
static void backend_context_config_teardown(struct futhark_context_config* cfg);
static int backend_context_setup(struct futhark_context *ctx);
static void backend_context_teardown(struct futhark_context *ctx);

// End of of context_prototypes.h

struct memblock {
    int *references;
    unsigned char *mem;
    int64_t size;
    const char *desc;
};
struct constants {
    int dummy;
};
struct tuning_params {
    int dummy;
};
static const int num_tuning_params = 0;
static const char *tuning_param_names[] = {NULL};
static const char *tuning_param_vars[] = {NULL};
static const char *tuning_param_classes[] = {NULL};
static int64_t tuning_param_defaults[] = {0};
// Start of backends/c.h

struct futhark_context_config {
  int in_use;
  int debugging;
  int profiling;
  int logging;
  char *cache_fname;
  int num_tuning_params;
  int64_t *tuning_params;
  const char** tuning_param_names;
  const char** tuning_param_vars;
  const char** tuning_param_classes;
};

static void backend_context_config_setup(struct futhark_context_config* cfg) {
  (void)cfg;
}

static void backend_context_config_teardown(struct futhark_context_config* cfg) {
  (void)cfg;
}

int futhark_context_config_set_tuning_param(struct futhark_context_config* cfg, const char *param_name, size_t param_value) {
  (void)cfg; (void)param_name; (void)param_value;
  return 1;
}

struct futhark_context {
  struct futhark_context_config* cfg;
  int detail_memory;
  int debugging;
  int profiling;
  int profiling_paused;
  int logging;
  lock_t lock;
  char *error;
  lock_t error_lock;
  FILE *log;
  struct constants *constants;
  struct free_list free_list;
  struct event_list event_list;
  int64_t peak_mem_usage_default;
  int64_t cur_mem_usage_default;
  struct program* program;
  bool program_initialised;
};

int backend_context_setup(struct futhark_context* ctx) {
  (void)ctx;
  return 0;
}

void backend_context_teardown(struct futhark_context* ctx) {
  (void)ctx;
}

int futhark_context_sync(struct futhark_context* ctx) {
  (void)ctx;
  return 0;
}

// End of backends/c.h

struct program {
    int dummy;
};
static void setup_program(struct futhark_context *ctx)
{
    (void) ctx;
    
    int error = 0;
    
    (void) error;
    ctx->program = malloc(sizeof(struct program));
}
static void teardown_program(struct futhark_context *ctx)
{
    (void) ctx;
    
    int error = 0;
    
    (void) error;
    free(ctx->program);
}
static void set_tuning_params(struct futhark_context *ctx)
{
    (void) ctx;
}
int memblock_unref(struct futhark_context *ctx, struct memblock *block, const char *desc)
{
    if (block->references != NULL) {
        *block->references -= 1;
        if (ctx->detail_memory)
            fprintf(ctx->log, "Unreferencing block %s (allocated as %s) in %s: %d references remaining.\n", desc, block->desc, "default space", *block->references);
        if (*block->references == 0) {
            ctx->cur_mem_usage_default -= block->size;
            host_free(ctx, (size_t) block->size, desc, (void *) block->mem);
            free(block->references);
            if (ctx->detail_memory)
                fprintf(ctx->log, "%lld bytes freed (now allocated: %lld bytes)\n", (long long) block->size, (long long) ctx->cur_mem_usage_default);
        }
        block->references = NULL;
    }
    return 0;
}
int memblock_alloc(struct futhark_context *ctx, struct memblock *block, int64_t size, const char *desc)
{
    if (size < 0)
        futhark_panic(1, "Negative allocation of %lld bytes attempted for %s in %s.\n", (long long) size, desc, "default space", ctx->cur_mem_usage_default);
    
    int ret = memblock_unref(ctx, block, desc);
    
    if (ret != FUTHARK_SUCCESS)
        return ret;
    if (ctx->detail_memory)
        fprintf(ctx->log, "Allocating %lld bytes for %s in %s (currently allocated: %lld bytes).\n", (long long) size, desc, "default space", (long long) ctx->cur_mem_usage_default);
    host_alloc(ctx, (size_t) size, desc, (size_t *) &size, (void *) &block->mem);
    if (ctx->error == NULL) {
        block->references = (int *) malloc(sizeof(int));
        *block->references = 1;
        block->size = size;
        block->desc = desc;
        
        long long new_usage = ctx->cur_mem_usage_default + size;
        
        if (ctx->detail_memory)
            fprintf(ctx->log, "Received block of %lld bytes; now allocated: %lld bytes", (long long) block->size, new_usage);
        ctx->cur_mem_usage_default = new_usage;
        if (new_usage > ctx->peak_mem_usage_default) {
            ctx->peak_mem_usage_default = new_usage;
            if (ctx->detail_memory)
                fprintf(ctx->log, " (new peak).\n");
        } else if (ctx->detail_memory)
            fprintf(ctx->log, ".\n");
        return FUTHARK_SUCCESS;
    } else {
        // We are naively assuming that any memory allocation error is due to OOM.
        lock_lock(&ctx->error_lock);
        
        char *old_error = ctx->error;
        
        ctx->error = msgprintf("Failed to allocate memory in %s.\nAttempted allocation: %12lld bytes\nCurrently allocated:  %12lld bytes\n%s", "default space", (long long) size, (long long) ctx->cur_mem_usage_default, old_error);
        free(old_error);
        lock_unlock(&ctx->error_lock);
        return FUTHARK_OUT_OF_MEMORY;
    }
}
int memblock_set(struct futhark_context *ctx, struct memblock *lhs, struct memblock *rhs, const char *lhs_desc)
{
    int ret = memblock_unref(ctx, lhs, lhs_desc);
    
    if (rhs->references != NULL)
        (*rhs->references)++;
    *lhs = *rhs;
    return ret;
}
char *futhark_context_report(struct futhark_context *ctx)
{
    if (futhark_context_sync(ctx) != 0)
        return NULL;
    
    struct str_builder builder;
    
    str_builder_init(&builder);
    str_builder_char(&builder, '{');
    str_builder_str(&builder, "\"memory\":{");
    str_builder(&builder, "\"default space\": %lld", (long long) ctx->peak_mem_usage_default);
    str_builder_str(&builder, "},\"events\":[");
    if (report_events_in_list(&ctx->event_list, &builder) != 0) {
        free(builder.str);
        return NULL;
    } else {
        str_builder_str(&builder, "]}");
        return builder.str;
    }
}
int futhark_context_clear_caches(struct futhark_context *ctx)
{
    lock_lock(&ctx->lock);
    ctx->peak_mem_usage_default = 0;
    lock_unlock(&ctx->lock);
    return ctx->error != NULL;
}

// Start of context.h

// Internal functions.

static void set_error(struct futhark_context* ctx, char *error) {
  lock_lock(&ctx->error_lock);
  if (ctx->error == NULL) {
    ctx->error = error;
  } else {
    free(error);
  }
  lock_unlock(&ctx->error_lock);
}

// XXX: should be static, but used in ispc_util.h
void lexical_realloc_error(struct futhark_context* ctx, size_t new_size) {
  set_error(ctx,
            msgprintf("Failed to allocate memory.\nAttempted allocation: %12lld bytes\n",
                      (long long) new_size));
}

static int lexical_realloc(struct futhark_context *ctx,
                           unsigned char **ptr,
                           int64_t *old_size,
                           int64_t new_size) {
  unsigned char *new = realloc(*ptr, (size_t)new_size);
  if (new == NULL) {
    lexical_realloc_error(ctx, new_size);
    return FUTHARK_OUT_OF_MEMORY;
  } else {
    *ptr = new;
    *old_size = new_size;
    return FUTHARK_SUCCESS;
  }
}

static void free_all_in_free_list(struct futhark_context* ctx) {
  fl_mem mem;
  free_list_pack(&ctx->free_list);
  while (free_list_first(&ctx->free_list, (fl_mem*)&mem) == 0) {
    free((void*)mem);
  }
}

static int is_small_alloc(size_t size) {
  return size < 1024*1024;
}

static void host_alloc(struct futhark_context* ctx,
                       size_t size, const char* tag, size_t* size_out, void** mem_out) {
  if (is_small_alloc(size) || free_list_find(&ctx->free_list, size, tag, size_out, (fl_mem*)mem_out) != 0) {
    *size_out = size;
    *mem_out = malloc(size);
  }
}

static void host_free(struct futhark_context* ctx,
                      size_t size, const char* tag, void* mem) {
  // Small allocations are handled by malloc()s own free list.  The
  // threshold here is kind of arbitrary, but seems to work OK.
  // Larger allocations are mmap()ed/munmapped() every time, which is
  // very slow, and Futhark programs tend to use a few very large
  // allocations.
  if (is_small_alloc(size)) {
    free(mem);
  } else {
    free_list_insert(&ctx->free_list, size, (fl_mem)mem, tag);
  }
}

static void add_event(struct futhark_context* ctx,
                      const char* name,
                      const char* provenance,
                      struct kvs *kvs,
                      void* data,
                      event_report_fn f) {
  if (provenance == NULL) {
    provenance = "unknown";
  }
  if (ctx->logging) {
    fprintf(ctx->log, "Event: %s\n  at: %s\n", name, provenance);
    if (kvs) {
      kvs_log(kvs, "  ", ctx->log);
    }
  }
  add_event_to_list(&ctx->event_list, name, provenance, kvs, data, f);
}

char *futhark_context_get_error(struct futhark_context *ctx) {
  char *error = ctx->error;
  ctx->error = NULL;
  return error;
}

void futhark_context_config_set_debugging(struct futhark_context_config *cfg, int flag) {
    cfg->profiling = cfg->logging = cfg->debugging = flag;
}

void futhark_context_config_set_profiling(struct futhark_context_config *cfg, int flag) {
    cfg->profiling = flag;
}

void futhark_context_config_set_logging(struct futhark_context_config *cfg, int flag) {
    cfg->logging = flag;
}

void futhark_context_config_set_cache_file(struct futhark_context_config *cfg, const char *f) {
  cfg->cache_fname = strdup(f);
}

int futhark_get_tuning_param_count(void) {
  return num_tuning_params;
}

const char *futhark_get_tuning_param_name(int i) {
  return tuning_param_names[i];
}

const char *futhark_get_tuning_param_class(int i) {
    return tuning_param_classes[i];
}

void futhark_context_set_logging_file(struct futhark_context *ctx, FILE *f){
  ctx->log = f;
}

void futhark_context_pause_profiling(struct futhark_context *ctx) {
  ctx->profiling_paused = 1;
}

void futhark_context_unpause_profiling(struct futhark_context *ctx) {
  ctx->profiling_paused = 0;
}

struct futhark_context_config* futhark_context_config_new(void) {
  struct futhark_context_config* cfg = malloc(sizeof(struct futhark_context_config));
  if (cfg == NULL) {
    return NULL;
  }
  cfg->in_use = 0;
  cfg->debugging = 0;
  cfg->profiling = 0;
  cfg->logging = 0;
  cfg->cache_fname = NULL;
  cfg->num_tuning_params = num_tuning_params;
  cfg->tuning_params = malloc(cfg->num_tuning_params * sizeof(int64_t));
  memcpy(cfg->tuning_params, tuning_param_defaults,
         cfg->num_tuning_params * sizeof(int64_t));
  cfg->tuning_param_names = tuning_param_names;
  cfg->tuning_param_vars = tuning_param_vars;
  cfg->tuning_param_classes = tuning_param_classes;
  backend_context_config_setup(cfg);
  return cfg;
}

void futhark_context_config_free(struct futhark_context_config* cfg) {
  assert(!cfg->in_use);
  backend_context_config_teardown(cfg);
  free(cfg->cache_fname);
  free(cfg->tuning_params);
  free(cfg);
}

struct futhark_context* futhark_context_new(struct futhark_context_config* cfg) {
  struct futhark_context* ctx = malloc(sizeof(struct futhark_context));
  if (ctx == NULL) {
    return NULL;
  }
  assert(!cfg->in_use);
  ctx->cfg = cfg;
  ctx->cfg->in_use = 1;
  ctx->program_initialised = false;
  create_lock(&ctx->error_lock);
  create_lock(&ctx->lock);
  free_list_init(&ctx->free_list);
  event_list_init(&ctx->event_list);
  ctx->peak_mem_usage_default = 0;
  ctx->cur_mem_usage_default = 0;
  ctx->constants = malloc(sizeof(struct constants));
  ctx->debugging = cfg->debugging;
  ctx->logging = cfg->logging;
  ctx->detail_memory = cfg->logging;
  ctx->profiling = cfg->profiling;
  ctx->profiling_paused = 0;
  ctx->error = NULL;
  ctx->log = stderr;
  set_tuning_params(ctx);
  if (backend_context_setup(ctx) == 0) {
    setup_program(ctx);
    init_constants(ctx);
    ctx->program_initialised = true;
    (void)futhark_context_clear_caches(ctx);
    (void)futhark_context_sync(ctx);
  }
  return ctx;
}

void futhark_context_free(struct futhark_context* ctx) {
  if (ctx->program_initialised) {
    free_constants(ctx);
    teardown_program(ctx);
  }
  backend_context_teardown(ctx);
  free_all_in_free_list(ctx);
  free_list_destroy(&ctx->free_list);
  event_list_free(&ctx->event_list);
  free(ctx->constants);
  free(ctx->error);
  free_lock(&ctx->lock);
  free_lock(&ctx->error_lock);
  ctx->cfg->in_use = 0;
  free(ctx);
}

// End of context.h

// Start of copy.h

// Cache-oblivious map-transpose function.
#define GEN_MAP_TRANSPOSE(NAME, ELEM_TYPE)                              \
  static void map_transpose_##NAME                                      \
  (ELEM_TYPE* dst, ELEM_TYPE* src,                                      \
   int64_t k, int64_t m, int64_t n,                                     \
   int64_t cb, int64_t ce, int64_t rb, int64_t re)                      \
  {                                                                     \
  int32_t r = re - rb;                                                  \
  int32_t c = ce - cb;                                                  \
  if (k == 1) {                                                         \
    if (r <= 64 && c <= 64) {                                           \
      for (int64_t j = 0; j < c; j++) {                                 \
        for (int64_t i = 0; i < r; i++) {                               \
          dst[(j + cb) * n + (i + rb)] = src[(i + rb) * m + (j + cb)];  \
        }                                                               \
      }                                                                 \
    } else if (c <= r) {                                                \
      map_transpose_##NAME(dst, src, k, m, n, cb, ce, rb, rb + r/2);    \
      map_transpose_##NAME(dst, src, k, m, n, cb, ce, rb + r/2, re);    \
    } else {                                                            \
      map_transpose_##NAME(dst, src, k, m, n, cb, cb + c/2, rb, re);    \
      map_transpose_##NAME(dst, src, k, m, n, cb + c/2, ce, rb, re);    \
    }                                                                   \
  } else {                                                              \
  for (int64_t i = 0; i < k; i++) {                                     \
    map_transpose_##NAME(dst + i * m * n, src + i * m * n, 1, m, n, cb, ce, rb, re); \
  }\
} \
}

// Straightforward LMAD copy function.
#define GEN_LMAD_COPY_ELEMENTS(NAME, ELEM_TYPE)                         \
  static void lmad_copy_elements_##NAME(int r,                          \
                                        ELEM_TYPE* dst, int64_t dst_strides[r], \
                                        ELEM_TYPE *src, int64_t src_strides[r], \
                                        int64_t shape[r]) {             \
    if (r == 1) {                                                       \
      for (int i = 0; i < shape[0]; i++) {                              \
        dst[i*dst_strides[0]] = src[i*src_strides[0]];                  \
      }                                                                 \
    } else if (r > 1) {                                                 \
      for (int i = 0; i < shape[0]; i++) {                              \
        lmad_copy_elements_##NAME(r-1,                                  \
                                  dst+i*dst_strides[0], dst_strides+1,  \
                                  src+i*src_strides[0], src_strides+1,  \
                                  shape+1);                             \
      }                                                                 \
    }                                                                   \
  }                                                                     \

// Check whether this LMAD can be seen as a transposed 2D array.  This
// is done by checking every possible splitting point.
static bool lmad_is_tr(int64_t *n_out, int64_t *m_out,
                       int r,
                       const int64_t strides[r],
                       const int64_t shape[r]) {
  for (int i = 1; i < r; i++) {
    int n = 1, m = 1;
    bool ok = true;
    int64_t expected = 1;
    // Check strides before 'i'.
    for (int j = i-1; j >= 0; j--) {
      ok = ok && strides[j] == expected;
      expected *= shape[j];
      n *= shape[j];
    }
    // Check strides after 'i'.
    for (int j = r-1; j >= i; j--) {
      ok = ok && strides[j] == expected;
      expected *= shape[j];
      m *= shape[j];
    }
    if (ok) {
      *n_out = n;
      *m_out = m;
      return true;
    }
  }
  return false;
}

// This function determines whether the a 'dst' LMAD is row-major and
// 'src' LMAD is column-major.  Both LMADs are for arrays of the same
// shape.  Both LMADs are allowed to have additional dimensions "on
// top".  Essentially, this function determines whether a copy from
// 'src' to 'dst' is a "map(transpose)" that we know how to implement
// efficiently.  The LMADs can have arbitrary rank, and the main
// challenge here is checking whether the src LMAD actually
// corresponds to a 2D column-major layout by morally collapsing
// dimensions.  There is a lot of looping here, but the actual trip
// count is going to be very low in practice.
//
// Returns true if this is indeed a map(transpose), and writes the
// number of arrays, and moral array size to appropriate output
// parameters.
static bool lmad_map_tr(int64_t *num_arrays_out, int64_t *n_out, int64_t *m_out,
                        int r,
                        const int64_t dst_strides[r],
                        const int64_t src_strides[r],
                        const int64_t shape[r]) {
  int64_t rowmajor_strides[r];
  rowmajor_strides[r-1] = 1;

  for (int i = r-2; i >= 0; i--) {
    rowmajor_strides[i] = rowmajor_strides[i+1] * shape[i+1];
  }

  // map_r will be the number of mapped dimensions on top.
  int map_r = 0;
  int64_t num_arrays = 1;
  for (int i = 0; i < r; i++) {
    if (dst_strides[i] != rowmajor_strides[i] ||
        src_strides[i] != rowmajor_strides[i]) {
      break;
    } else {
      num_arrays *= shape[i];
      map_r++;
    }
  }

  *num_arrays_out = num_arrays;

  if (r==map_r) {
    return false;
  }

  if (memcmp(&rowmajor_strides[map_r],
             &dst_strides[map_r],
             sizeof(int64_t)*(r-map_r)) == 0) {
    return lmad_is_tr(n_out, m_out, r-map_r, src_strides+map_r, shape+map_r);
  } else if (memcmp(&rowmajor_strides[map_r],
                    &src_strides[map_r],
                    sizeof(int64_t)*(r-map_r)) == 0) {
    return lmad_is_tr(m_out, n_out, r-map_r, dst_strides+map_r, shape+map_r);
  }
  return false;
}

// Check if the strides correspond to row-major strides of *any*
// permutation of the shape.  This is done by recursive search with
// backtracking.  This is worst-case exponential, but hopefully the
// arrays we encounter do not have that many dimensions.
static bool lmad_contiguous_search(int checked, int64_t expected,
                                   int r,
                                   int64_t strides[r], int64_t shape[r], bool used[r]) {
  for (int i = 0; i < r; i++) {
    for (int j = 0; j < r; j++) {
      if (!used[j] && strides[j] == expected && strides[j] >= 0) {
        used[j] = true;
        if (checked+1 == r ||
            lmad_contiguous_search(checked+1, expected * shape[j], r, strides, shape, used)) {
          return true;
        }
        used[j] = false;
      }
    }
  }
  return false;
}

// Does this LMAD correspond to an array with positive strides and no
// holes?
static bool lmad_contiguous(int r, int64_t strides[r], int64_t shape[r]) {
  bool used[r];
  for (int i = 0; i < r; i++) {
    used[i] = false;
  }
  return lmad_contiguous_search(0, 1, r, strides, shape, used);
}

// Does this copy correspond to something that could be done with a
// memcpy()-like operation?  I.e. do the LMADs actually represent the
// same in-memory layout and are they contiguous?
static bool lmad_memcpyable(int r,
                            int64_t dst_strides[r], int64_t src_strides[r], int64_t shape[r]) {
  if (!lmad_contiguous(r, dst_strides, shape)) {
    return false;
  }
  for (int i = 0; i < r; i++) {
    if (dst_strides[i] != src_strides[i] && shape[i] != 1) {
      return false;
    }
  }
  return true;
}


static void log_copy(struct futhark_context* ctx,
                     const char *kind, const char *provenance,
                     int r,
                     int64_t dst_offset, int64_t dst_strides[r],
                     int64_t src_offset, int64_t src_strides[r],
                     int64_t shape[r]) {
  if (ctx->logging) {
    fprintf(ctx->log, "\n# Copy %s\n", kind);
    if (provenance) { fprintf(ctx->log, "At: %s\n", provenance); }
    fprintf(ctx->log, "Shape: ");
    for (int i = 0; i < r; i++) { fprintf(ctx->log, "[%ld]", (long int)shape[i]); }
    fprintf(ctx->log, "\n");
    fprintf(ctx->log, "Dst offset: %ld\n", (long int)dst_offset);
    fprintf(ctx->log, "Dst strides:");
    for (int i = 0; i < r; i++) { fprintf(ctx->log, " %ld", (long int)dst_strides[i]); }
    fprintf(ctx->log, "\n");
    fprintf(ctx->log, "Src offset: %ld\n", (long int)src_offset);
    fprintf(ctx->log, "Src strides:");
    for (int i = 0; i < r; i++) { fprintf(ctx->log, " %ld", (long int)src_strides[i]); }
    fprintf(ctx->log, "\n");
  }
}

static void log_transpose(struct futhark_context* ctx,
                          int64_t k, int64_t n, int64_t m) {
  if (ctx->logging) {
    fprintf(ctx->log, "## Transpose\n");
    fprintf(ctx->log, "Arrays     : %ld\n", (long int)k);
    fprintf(ctx->log, "X elements : %ld\n", (long int)m);
    fprintf(ctx->log, "Y elements : %ld\n", (long int)n);
    fprintf(ctx->log, "\n");
  }
}

#define GEN_LMAD_COPY(NAME, ELEM_TYPE)                                  \
  static void lmad_copy_##NAME                                          \
  (struct futhark_context *ctx, int r,                                  \
   ELEM_TYPE* dst, int64_t dst_offset, int64_t dst_strides[r],          \
   ELEM_TYPE *src, int64_t src_offset, int64_t src_strides[r],          \
   int64_t shape[r]) {                                                  \
    log_copy(ctx, "CPU to CPU", NULL, r, dst_offset, dst_strides,       \
             src_offset, src_strides, shape);                           \
    int64_t size = 1;                                                   \
    for (int i = 0; i < r; i++) { size *= shape[i]; }                   \
    if (size == 0) { return; }                                          \
    int64_t k, n, m;                                                    \
    if (lmad_map_tr(&k, &n, &m,                                         \
                    r, dst_strides, src_strides, shape)) {              \
      log_transpose(ctx, k, n, m);                                      \
      map_transpose_##NAME                                              \
        (dst+dst_offset, src+src_offset, k, n, m, 0, n, 0, m);          \
    } else if (lmad_memcpyable(r, dst_strides, src_strides, shape)) {   \
      if (ctx->logging) {fprintf(ctx->log, "## Flat copy\n\n");}          \
      memcpy(dst+dst_offset, src+src_offset, size*sizeof(*dst));        \
    } else {                                                            \
      if (ctx->logging) {fprintf(ctx->log, "## General copy\n\n");}       \
      lmad_copy_elements_##NAME                                         \
        (r,                                                             \
         dst+dst_offset, dst_strides,                                   \
         src+src_offset, src_strides, shape);                           \
    }                                                                   \
  }

GEN_MAP_TRANSPOSE(1b, uint8_t)
GEN_MAP_TRANSPOSE(2b, uint16_t)
GEN_MAP_TRANSPOSE(4b, uint32_t)
GEN_MAP_TRANSPOSE(8b, uint64_t)

GEN_LMAD_COPY_ELEMENTS(1b, uint8_t)
GEN_LMAD_COPY_ELEMENTS(2b, uint16_t)
GEN_LMAD_COPY_ELEMENTS(4b, uint32_t)
GEN_LMAD_COPY_ELEMENTS(8b, uint64_t)

GEN_LMAD_COPY(1b, uint8_t)
GEN_LMAD_COPY(2b, uint16_t)
GEN_LMAD_COPY(4b, uint32_t)
GEN_LMAD_COPY(8b, uint64_t)

// End of copy.h

#define FUTHARK_FUN_ATTR static

FUTHARK_FUN_ATTR int futrts_add_arc_6874(struct futhark_context *ctx, struct memblock *mem_out_p_22882, struct memblock mat_mem_21926, int64_t dz2080U_10516, int64_t dz2081U_10517, int32_t u_10519, int32_t v_10520, int64_t c_10521);
FUTHARK_FUN_ATTR int futrts_entry_graph_orientation_solve(struct futhark_context *ctx, struct memblock *mem_out_p_22883, struct memblock *mem_out_p_22884, struct memblock *mem_out_p_22885, int64_t *out_prim_out_22886, int64_t *out_prim_out_22887, bool *out_prim_out_22888, struct memblock bounds_mem_21926, struct memblock eu_mem_21927, struct memblock ev_mem_21928, int64_t dz2080U_14788, int64_t dz2081U_14789, int64_t dz2082U_14790, int32_t mode_14791, int32_t n_14792, int32_t m_14793);

static int init_constants(struct futhark_context *ctx)
{
    (void) ctx;
    
    int err = 0;
    
    
  cleanup:
    return err;
}
static int free_constants(struct futhark_context *ctx)
{
    (void) ctx;
    return 0;
}
struct futhark_i32_1d {
    struct memblock mem;
    int64_t shape[1];
};
struct futhark_i32_1d *futhark_new_i32_1d(struct futhark_context *ctx, const int32_t *data, int64_t dim0)
{
    int err = 0;
    struct futhark_i32_1d *bad = NULL;
    struct futhark_i32_1d *arr = (struct futhark_i32_1d *) malloc(sizeof(struct futhark_i32_1d));
    
    if (arr == NULL)
        return bad;
    lock_lock(&ctx->lock);
    arr->mem.references = NULL;
    if (memblock_alloc(ctx, &arr->mem, dim0 * 4, "arr->mem"))
        err = 1;
    arr->shape[0] = dim0;
    if ((size_t) dim0 * 4 > 0)
        memmove(arr->mem.mem + 0, (const unsigned char *) data + 0, (size_t) dim0 * 4);
    lock_unlock(&ctx->lock);
    if (err != 0) {
        free(arr);
        return bad;
    }
    return arr;
}
struct futhark_i32_1d *futhark_new_raw_i32_1d(struct futhark_context *ctx, unsigned char *data, int64_t dim0)
{
    int err = 0;
    struct futhark_i32_1d *bad = NULL;
    struct futhark_i32_1d *arr = (struct futhark_i32_1d *) malloc(sizeof(struct futhark_i32_1d));
    
    if (arr == NULL)
        return bad;
    lock_lock(&ctx->lock);
    arr->mem.references = NULL;
    arr->mem.mem = data;
    arr->shape[0] = dim0;
    lock_unlock(&ctx->lock);
    return arr;
}
int futhark_free_i32_1d(struct futhark_context *ctx, struct futhark_i32_1d *arr)
{
    lock_lock(&ctx->lock);
    if (memblock_unref(ctx, &arr->mem, "arr->mem") != 0)
        return 1;
    lock_unlock(&ctx->lock);
    free(arr);
    return 0;
}
int futhark_values_i32_1d(struct futhark_context *ctx, struct futhark_i32_1d *arr, int32_t *data)
{
    int err = 0;
    
    lock_lock(&ctx->lock);
    if ((size_t) arr->shape[0] * 4 > 0)
        memmove((unsigned char *) data + 0, arr->mem.mem + 0, (size_t) arr->shape[0] * 4);
    lock_unlock(&ctx->lock);
    return err;
}
int futhark_index_i32_1d(struct futhark_context *ctx, int32_t *out, struct futhark_i32_1d *arr, int64_t i0)
{
    int err = 0;
    
    if (i0 >= 0 && i0 < arr->shape[0]) {
        lock_lock(&ctx->lock);
        if (4 > 0)
            memmove((unsigned char *) out + 0, arr->mem.mem + 4 * (i0 * 1), 4);
        lock_unlock(&ctx->lock);
    } else {
        err = 1;
        set_error(ctx, strdup("Index out of bounds."));
    }
    return err;
}
unsigned char *futhark_values_raw_i32_1d(struct futhark_context *ctx, struct futhark_i32_1d *arr)
{
    (void) ctx;
    return arr->mem.mem;
}
const int64_t *futhark_shape_i32_1d(struct futhark_context *ctx, struct futhark_i32_1d *arr)
{
    (void) ctx;
    return arr->shape;
}
struct futhark_i64_1d {
    struct memblock mem;
    int64_t shape[1];
};
struct futhark_i64_1d *futhark_new_i64_1d(struct futhark_context *ctx, const int64_t *data, int64_t dim0)
{
    int err = 0;
    struct futhark_i64_1d *bad = NULL;
    struct futhark_i64_1d *arr = (struct futhark_i64_1d *) malloc(sizeof(struct futhark_i64_1d));
    
    if (arr == NULL)
        return bad;
    lock_lock(&ctx->lock);
    arr->mem.references = NULL;
    if (memblock_alloc(ctx, &arr->mem, dim0 * 8, "arr->mem"))
        err = 1;
    arr->shape[0] = dim0;
    if ((size_t) dim0 * 8 > 0)
        memmove(arr->mem.mem + 0, (const unsigned char *) data + 0, (size_t) dim0 * 8);
    lock_unlock(&ctx->lock);
    if (err != 0) {
        free(arr);
        return bad;
    }
    return arr;
}
struct futhark_i64_1d *futhark_new_raw_i64_1d(struct futhark_context *ctx, unsigned char *data, int64_t dim0)
{
    int err = 0;
    struct futhark_i64_1d *bad = NULL;
    struct futhark_i64_1d *arr = (struct futhark_i64_1d *) malloc(sizeof(struct futhark_i64_1d));
    
    if (arr == NULL)
        return bad;
    lock_lock(&ctx->lock);
    arr->mem.references = NULL;
    arr->mem.mem = data;
    arr->shape[0] = dim0;
    lock_unlock(&ctx->lock);
    return arr;
}
int futhark_free_i64_1d(struct futhark_context *ctx, struct futhark_i64_1d *arr)
{
    lock_lock(&ctx->lock);
    if (memblock_unref(ctx, &arr->mem, "arr->mem") != 0)
        return 1;
    lock_unlock(&ctx->lock);
    free(arr);
    return 0;
}
int futhark_values_i64_1d(struct futhark_context *ctx, struct futhark_i64_1d *arr, int64_t *data)
{
    int err = 0;
    
    lock_lock(&ctx->lock);
    if ((size_t) arr->shape[0] * 8 > 0)
        memmove((unsigned char *) data + 0, arr->mem.mem + 0, (size_t) arr->shape[0] * 8);
    lock_unlock(&ctx->lock);
    return err;
}
int futhark_index_i64_1d(struct futhark_context *ctx, int64_t *out, struct futhark_i64_1d *arr, int64_t i0)
{
    int err = 0;
    
    if (i0 >= 0 && i0 < arr->shape[0]) {
        lock_lock(&ctx->lock);
        if (8 > 0)
            memmove((unsigned char *) out + 0, arr->mem.mem + 8 * (i0 * 1), 8);
        lock_unlock(&ctx->lock);
    } else {
        err = 1;
        set_error(ctx, strdup("Index out of bounds."));
    }
    return err;
}
unsigned char *futhark_values_raw_i64_1d(struct futhark_context *ctx, struct futhark_i64_1d *arr)
{
    (void) ctx;
    return arr->mem.mem;
}
const int64_t *futhark_shape_i64_1d(struct futhark_context *ctx, struct futhark_i64_1d *arr)
{
    (void) ctx;
    return arr->shape;
}

FUTHARK_FUN_ATTR int futrts_add_arc_6874(struct futhark_context *ctx, struct memblock *mem_out_p_22882, struct memblock mat_mem_21926, int64_t dz2080U_10516, int64_t dz2081U_10517, int32_t u_10519, int32_t v_10520, int64_t c_10521)
{
    (void) ctx;
    
    int err = 0;
    struct memblock mem_21929;
    
    mem_21929.references = NULL;
    
    struct memblock mem_out_22654;
    
    mem_out_22654.references = NULL;
    // graph_orientation_futhark.fut:21:27-33
    
    int64_t u_10522 = sext_i32_i64(u_10519);
    
    // graph_orientation_futhark.fut:21:27-33
    
    bool x_10523 = sle64((int64_t) 0, u_10522);
    
    // graph_orientation_futhark.fut:21:27-33
    
    bool y_10524 = slt64(u_10522, dz2080U_10516);
    
    // graph_orientation_futhark.fut:21:27-33
    
    bool bounds_check_10525 = x_10523 && y_10524;
    
    // graph_orientation_futhark.fut:21:27-33
    
    bool index_certs_10526;
    
    if (!bounds_check_10525) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_10522, "] out of bounds for array of shape [", (long long) dz2080U_10516, "].", "-> #0  graph_orientation_futhark.fut:21:27-33\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:21:27-36
    
    int64_t v_10529 = sext_i32_i64(v_10520);
    
    // graph_orientation_futhark.fut:21:27-36
    
    bool x_10530 = sle64((int64_t) 0, v_10529);
    
    // graph_orientation_futhark.fut:21:27-36
    
    bool y_10531 = slt64(v_10529, dz2081U_10517);
    
    // graph_orientation_futhark.fut:21:27-36
    
    bool bounds_check_10532 = x_10530 && y_10531;
    
    // graph_orientation_futhark.fut:21:27-36
    
    bool index_certs_10533;
    
    if (!bounds_check_10532) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_10529, "] out of bounds for array of shape [", (long long) dz2081U_10517, "].", "-> #0  graph_orientation_futhark.fut:21:27-36\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:21:3-11
    
    int64_t binop_x_21927 = (int64_t) 8 * dz2080U_10516;
    
    // graph_orientation_futhark.fut:21:3-11
    
    int64_t bytes_21928 = dz2081U_10517 * binop_x_21927;
    
    // graph_orientation_futhark.fut:21:3-41
    
    bool index_ok_10548 = bounds_check_10525 && bounds_check_10532;
    
    // graph_orientation_futhark.fut:21:3-41
    
    bool index_certs_10549;
    
    if (!index_ok_10548) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_10522, ", ", (long long) v_10529, "] out of bounds for array of shape [", (long long) dz2080U_10516, "][", (long long) dz2081U_10517, "].", "-> #0  graph_orientation_futhark.fut:21:3-41\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    
    int64_t zp_lhs_10534 = ((int64_t *) mat_mem_21926.mem)[u_10522 * dz2081U_10517 + v_10529];
    
    // graph_orientation_futhark.fut:21:37-40
    
    int64_t tmp_10536 = add64(c_10521, zp_lhs_10534);
    
    // graph_orientation_futhark.fut:21:3-11
    if (memblock_alloc(ctx, &mem_21929, bytes_21928, "mem_21929")) {
        err = 1;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:21:3-11
    // graph_orientation_futhark.fut:21:3-11
    lmad_copy_8b(ctx, 2, (uint64_t *) mem_21929.mem, (int64_t) 0, (int64_t []) {dz2081U_10517, (int64_t) 1}, (uint64_t *) mat_mem_21926.mem, (int64_t) 0, (int64_t []) {dz2081U_10517, (int64_t) 1}, (int64_t []) {dz2080U_10516, dz2081U_10517});
    // graph_orientation_futhark.fut:21:3-41
    ((int64_t *) mem_21929.mem)[u_10522 * dz2081U_10517 + v_10529] = tmp_10536;
    if (memblock_set(ctx, &mem_out_22654, &mem_21929, "mem_21929") != 0)
        return 1;
    if (memblock_set(ctx, &*mem_out_p_22882, &mem_out_22654, "mem_out_22654") != 0)
        return 1;
    
  cleanup:
    {
        if (memblock_unref(ctx, &mem_21929, "mem_21929") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_out_22654, "mem_out_22654") != 0)
            return 1;
    }
    return err;
}
FUTHARK_FUN_ATTR int futrts_entry_graph_orientation_solve(struct futhark_context *ctx, struct memblock *mem_out_p_22883, struct memblock *mem_out_p_22884, struct memblock *mem_out_p_22885, int64_t *out_prim_out_22886, int64_t *out_prim_out_22887, bool *out_prim_out_22888, struct memblock bounds_mem_21926, struct memblock eu_mem_21927, struct memblock ev_mem_21928, int64_t dz2080U_14788, int64_t dz2081U_14789, int64_t dz2082U_14790, int32_t mode_14791, int32_t n_14792, int32_t m_14793)
{
    (void) ctx;
    
    int err = 0;
    int64_t mem_21965_cached_sizze_22889 = 0;
    unsigned char *mem_21965 = NULL;
    int64_t mem_21967_cached_sizze_22890 = 0;
    unsigned char *mem_21967 = NULL;
    int64_t mem_21981_cached_sizze_22891 = 0;
    unsigned char *mem_21981 = NULL;
    int64_t mem_21983_cached_sizze_22892 = 0;
    unsigned char *mem_21983 = NULL;
    int64_t mem_22010_cached_sizze_22893 = 0;
    unsigned char *mem_22010 = NULL;
    int64_t mem_22012_cached_sizze_22894 = 0;
    unsigned char *mem_22012 = NULL;
    int64_t mem_22026_cached_sizze_22895 = 0;
    unsigned char *mem_22026 = NULL;
    int64_t mem_22099_cached_sizze_22896 = 0;
    unsigned char *mem_22099 = NULL;
    int64_t mem_22101_cached_sizze_22897 = 0;
    unsigned char *mem_22101 = NULL;
    int64_t mem_22115_cached_sizze_22898 = 0;
    unsigned char *mem_22115 = NULL;
    int64_t mem_22117_cached_sizze_22899 = 0;
    unsigned char *mem_22117 = NULL;
    int64_t mem_22150_cached_sizze_22900 = 0;
    unsigned char *mem_22150 = NULL;
    int64_t mem_22152_cached_sizze_22901 = 0;
    unsigned char *mem_22152 = NULL;
    int64_t mem_22166_cached_sizze_22902 = 0;
    unsigned char *mem_22166 = NULL;
    int64_t mem_22168_cached_sizze_22903 = 0;
    unsigned char *mem_22168 = NULL;
    int64_t mem_22416_cached_sizze_22904 = 0;
    unsigned char *mem_22416 = NULL;
    int64_t mem_22469_cached_sizze_22905 = 0;
    unsigned char *mem_22469 = NULL;
    int64_t mem_22471_cached_sizze_22906 = 0;
    unsigned char *mem_22471 = NULL;
    struct memblock mem_22485;
    
    mem_22485.references = NULL;
    
    struct memblock mem_param_tmp_22859;
    
    mem_param_tmp_22859.references = NULL;
    
    struct memblock mem_param_tmp_22858;
    
    mem_param_tmp_22858.references = NULL;
    
    struct memblock mem_param_tmp_22870;
    
    mem_param_tmp_22870.references = NULL;
    
    struct memblock mem_param_tmp_22869;
    
    mem_param_tmp_22869.references = NULL;
    
    struct memblock mem_22449;
    
    mem_22449.references = NULL;
    
    struct memblock mem_22447;
    
    mem_22447.references = NULL;
    
    struct memblock ext_mem_22452;
    
    ext_mem_22452.references = NULL;
    
    struct memblock ext_mem_22455;
    
    ext_mem_22455.references = NULL;
    
    struct memblock mem_param_22446;
    
    mem_param_22446.references = NULL;
    
    struct memblock mem_param_22443;
    
    mem_param_22443.references = NULL;
    
    struct memblock ext_mem_22460;
    
    ext_mem_22460.references = NULL;
    
    struct memblock ext_mem_22461;
    
    ext_mem_22461.references = NULL;
    
    struct memblock mem_param_22440;
    
    mem_param_22440.references = NULL;
    
    struct memblock mem_param_22437;
    
    mem_param_22437.references = NULL;
    
    struct memblock ext_mem_22466;
    
    ext_mem_22466.references = NULL;
    
    struct memblock ext_mem_22467;
    
    ext_mem_22467.references = NULL;
    
    struct memblock mem_22434;
    
    mem_22434.references = NULL;
    
    struct memblock mem_22432;
    
    mem_22432.references = NULL;
    
    struct memblock mem_22486;
    
    mem_22486.references = NULL;
    
    struct memblock ext_mem_22487;
    
    ext_mem_22487.references = NULL;
    
    struct memblock mem_22431;
    
    mem_22431.references = NULL;
    
    struct memblock mem_22429;
    
    mem_22429.references = NULL;
    
    struct memblock mem_param_tmp_22800;
    
    mem_param_tmp_22800.references = NULL;
    
    struct memblock mem_param_tmp_22799;
    
    mem_param_tmp_22799.references = NULL;
    
    struct memblock mem_param_tmp_22798;
    
    mem_param_tmp_22798.references = NULL;
    
    struct memblock mem_param_tmp_22797;
    
    mem_param_tmp_22797.references = NULL;
    
    struct memblock mem_22176;
    
    mem_22176.references = NULL;
    
    struct memblock mem_22179;
    
    mem_22179.references = NULL;
    
    struct memblock ext_mem_22180;
    
    ext_mem_22180.references = NULL;
    
    struct memblock mem_param_tmp_22816;
    
    mem_param_tmp_22816.references = NULL;
    
    struct memblock mem_param_tmp_22815;
    
    mem_param_tmp_22815.references = NULL;
    
    struct memblock mem_param_tmp_22814;
    
    mem_param_tmp_22814.references = NULL;
    
    struct memblock mem_22125;
    
    mem_22125.references = NULL;
    
    struct memblock ext_mem_22128;
    
    ext_mem_22128.references = NULL;
    
    struct memblock mem_param_tmp_22829;
    
    mem_param_tmp_22829.references = NULL;
    
    struct memblock mem_param_tmp_22828;
    
    mem_param_tmp_22828.references = NULL;
    
    struct memblock ext_mem_22079;
    
    ext_mem_22079.references = NULL;
    
    struct memblock ext_mem_22083;
    
    ext_mem_22083.references = NULL;
    
    struct memblock mem_22069;
    
    mem_22069.references = NULL;
    
    struct memblock mem_22067;
    
    mem_22067.references = NULL;
    
    struct memblock ext_mem_22072;
    
    ext_mem_22072.references = NULL;
    
    struct memblock ext_mem_22076;
    
    ext_mem_22076.references = NULL;
    
    struct memblock ext_mem_22086;
    
    ext_mem_22086.references = NULL;
    
    struct memblock ext_mem_22090;
    
    ext_mem_22090.references = NULL;
    
    struct memblock mem_param_22064;
    
    mem_param_22064.references = NULL;
    
    struct memblock mem_param_22061;
    
    mem_param_22061.references = NULL;
    
    struct memblock ext_mem_22096;
    
    ext_mem_22096.references = NULL;
    
    struct memblock ext_mem_22097;
    
    ext_mem_22097.references = NULL;
    
    struct memblock ext_mem_22131;
    
    ext_mem_22131.references = NULL;
    
    struct memblock ext_mem_22134;
    
    ext_mem_22134.references = NULL;
    
    struct memblock ext_mem_22138;
    
    ext_mem_22138.references = NULL;
    
    struct memblock mem_param_22057;
    
    mem_param_22057.references = NULL;
    
    struct memblock mem_param_22054;
    
    mem_param_22054.references = NULL;
    
    struct memblock mem_param_22051;
    
    mem_param_22051.references = NULL;
    
    struct memblock ext_mem_22146;
    
    ext_mem_22146.references = NULL;
    
    struct memblock ext_mem_22147;
    
    ext_mem_22147.references = NULL;
    
    struct memblock ext_mem_22148;
    
    ext_mem_22148.references = NULL;
    
    struct memblock mem_param_22047;
    
    mem_param_22047.references = NULL;
    
    struct memblock mem_param_22044;
    
    mem_param_22044.references = NULL;
    
    struct memblock mem_param_22041;
    
    mem_param_22041.references = NULL;
    
    struct memblock mem_param_22037;
    
    mem_param_22037.references = NULL;
    
    struct memblock ext_mem_22190;
    
    ext_mem_22190.references = NULL;
    
    struct memblock ext_mem_22191;
    
    ext_mem_22191.references = NULL;
    
    struct memblock ext_mem_22192;
    
    ext_mem_22192.references = NULL;
    
    struct memblock ext_mem_22193;
    
    ext_mem_22193.references = NULL;
    
    struct memblock mem_22028;
    
    mem_22028.references = NULL;
    
    struct memblock mem_param_tmp_22793;
    
    mem_param_tmp_22793.references = NULL;
    
    struct memblock mem_22004;
    
    mem_22004.references = NULL;
    
    struct memblock mem_param_22001;
    
    mem_param_22001.references = NULL;
    
    struct memblock ext_mem_22008;
    
    ext_mem_22008.references = NULL;
    
    struct memblock mem_21961;
    
    mem_21961.references = NULL;
    
    struct memblock mem_param_tmp_22787;
    
    mem_param_tmp_22787.references = NULL;
    
    struct memblock mem_21994;
    
    mem_21994.references = NULL;
    
    struct memblock mem_param_21992;
    
    mem_param_21992.references = NULL;
    
    struct memblock ext_mem_21997;
    
    ext_mem_21997.references = NULL;
    
    struct memblock mem_21963;
    
    mem_21963.references = NULL;
    
    struct memblock mem_param_tmp_22712;
    
    mem_param_tmp_22712.references = NULL;
    
    struct memblock mem_param_tmp_22741;
    
    mem_param_tmp_22741.references = NULL;
    
    struct memblock mem_param_tmp_22776;
    
    mem_param_tmp_22776.references = NULL;
    
    struct memblock mem_22296;
    
    mem_22296.references = NULL;
    
    struct memblock mem_param_22293;
    
    mem_param_22293.references = NULL;
    
    struct memblock ext_mem_22300;
    
    ext_mem_22300.references = NULL;
    
    struct memblock ext_mem_22304;
    
    ext_mem_22304.references = NULL;
    
    struct memblock ext_mem_22308;
    
    ext_mem_22308.references = NULL;
    
    struct memblock mem_22283;
    
    mem_22283.references = NULL;
    
    struct memblock mem_22286;
    
    mem_22286.references = NULL;
    
    struct memblock ext_mem_22287;
    
    ext_mem_22287.references = NULL;
    
    struct memblock mem_param_tmp_22764;
    
    mem_param_tmp_22764.references = NULL;
    
    struct memblock mem_22278;
    
    mem_22278.references = NULL;
    
    struct memblock mem_param_22275;
    
    mem_param_22275.references = NULL;
    
    struct memblock ext_mem_22281;
    
    ext_mem_22281.references = NULL;
    
    struct memblock mem_22272;
    
    mem_22272.references = NULL;
    
    struct memblock mem_22288;
    
    mem_22288.references = NULL;
    
    struct memblock ext_mem_22289;
    
    ext_mem_22289.references = NULL;
    
    struct memblock mem_param_tmp_22751;
    
    mem_param_tmp_22751.references = NULL;
    
    struct memblock mem_param_tmp_22750;
    
    mem_param_tmp_22750.references = NULL;
    
    struct memblock mem_22253;
    
    mem_22253.references = NULL;
    
    struct memblock mem_22251;
    
    mem_22251.references = NULL;
    
    struct memblock ext_mem_22256;
    
    ext_mem_22256.references = NULL;
    
    struct memblock ext_mem_22259;
    
    ext_mem_22259.references = NULL;
    
    struct memblock ext_mem_22262;
    
    ext_mem_22262.references = NULL;
    
    struct memblock ext_mem_22265;
    
    ext_mem_22265.references = NULL;
    
    struct memblock mem_param_22249;
    
    mem_param_22249.references = NULL;
    
    struct memblock mem_param_22246;
    
    mem_param_22246.references = NULL;
    
    struct memblock ext_mem_22270;
    
    ext_mem_22270.references = NULL;
    
    struct memblock ext_mem_22271;
    
    ext_mem_22271.references = NULL;
    
    struct memblock mem_param_22239;
    
    mem_param_22239.references = NULL;
    
    struct memblock ext_mem_22312;
    
    ext_mem_22312.references = NULL;
    
    struct memblock mem_22243;
    
    mem_22243.references = NULL;
    
    struct memblock mem_22241;
    
    mem_22241.references = NULL;
    
    struct memblock ext_mem_22316;
    
    ext_mem_22316.references = NULL;
    
    struct memblock mem_param_tmp_22722;
    
    mem_param_tmp_22722.references = NULL;
    
    struct memblock mem_param_tmp_22721;
    
    mem_param_tmp_22721.references = NULL;
    
    struct memblock mem_param_tmp_22733;
    
    mem_param_tmp_22733.references = NULL;
    
    struct memblock mem_param_tmp_22732;
    
    mem_param_tmp_22732.references = NULL;
    
    struct memblock mem_22217;
    
    mem_22217.references = NULL;
    
    struct memblock mem_22215;
    
    mem_22215.references = NULL;
    
    struct memblock ext_mem_22220;
    
    ext_mem_22220.references = NULL;
    
    struct memblock ext_mem_22223;
    
    ext_mem_22223.references = NULL;
    
    struct memblock mem_param_22213;
    
    mem_param_22213.references = NULL;
    
    struct memblock mem_param_22210;
    
    mem_param_22210.references = NULL;
    
    struct memblock ext_mem_22228;
    
    ext_mem_22228.references = NULL;
    
    struct memblock ext_mem_22229;
    
    ext_mem_22229.references = NULL;
    
    struct memblock mem_param_22207;
    
    mem_param_22207.references = NULL;
    
    struct memblock mem_param_22204;
    
    mem_param_22204.references = NULL;
    
    struct memblock ext_mem_22234;
    
    ext_mem_22234.references = NULL;
    
    struct memblock ext_mem_22235;
    
    ext_mem_22235.references = NULL;
    
    struct memblock mem_param_22197;
    
    mem_param_22197.references = NULL;
    
    struct memblock ext_mem_22320;
    
    ext_mem_22320.references = NULL;
    
    struct memblock mem_22201;
    
    mem_22201.references = NULL;
    
    struct memblock mem_22199;
    
    mem_22199.references = NULL;
    
    struct memblock ext_mem_22324;
    
    ext_mem_22324.references = NULL;
    
    struct memblock mem_param_tmp_22666;
    
    mem_param_tmp_22666.references = NULL;
    
    struct memblock mem_param_tmp_22707;
    
    mem_param_tmp_22707.references = NULL;
    
    struct memblock mem_22389;
    
    mem_22389.references = NULL;
    
    struct memblock mem_param_22386;
    
    mem_param_22386.references = NULL;
    
    struct memblock ext_mem_22393;
    
    ext_mem_22393.references = NULL;
    
    struct memblock ext_mem_22397;
    
    ext_mem_22397.references = NULL;
    
    struct memblock ext_mem_22401;
    
    ext_mem_22401.references = NULL;
    
    struct memblock mem_22378;
    
    mem_22378.references = NULL;
    
    struct memblock mem_22381;
    
    mem_22381.references = NULL;
    
    struct memblock ext_mem_22382;
    
    ext_mem_22382.references = NULL;
    
    struct memblock mem_param_tmp_22697;
    
    mem_param_tmp_22697.references = NULL;
    
    struct memblock mem_22373;
    
    mem_22373.references = NULL;
    
    struct memblock mem_param_22370;
    
    mem_param_22370.references = NULL;
    
    struct memblock ext_mem_22376;
    
    ext_mem_22376.references = NULL;
    
    struct memblock mem_22367;
    
    mem_22367.references = NULL;
    
    struct memblock ext_mem_22405;
    
    ext_mem_22405.references = NULL;
    
    struct memblock mem_param_tmp_22676;
    
    mem_param_tmp_22676.references = NULL;
    
    struct memblock mem_param_tmp_22675;
    
    mem_param_tmp_22675.references = NULL;
    
    struct memblock mem_param_tmp_22688;
    
    mem_param_tmp_22688.references = NULL;
    
    struct memblock mem_param_tmp_22687;
    
    mem_param_tmp_22687.references = NULL;
    
    struct memblock mem_22348;
    
    mem_22348.references = NULL;
    
    struct memblock mem_22346;
    
    mem_22346.references = NULL;
    
    struct memblock ext_mem_22351;
    
    ext_mem_22351.references = NULL;
    
    struct memblock ext_mem_22354;
    
    ext_mem_22354.references = NULL;
    
    struct memblock mem_param_22344;
    
    mem_param_22344.references = NULL;
    
    struct memblock mem_param_22341;
    
    mem_param_22341.references = NULL;
    
    struct memblock ext_mem_22359;
    
    ext_mem_22359.references = NULL;
    
    struct memblock ext_mem_22360;
    
    ext_mem_22360.references = NULL;
    
    struct memblock mem_param_22338;
    
    mem_param_22338.references = NULL;
    
    struct memblock mem_param_22335;
    
    mem_param_22335.references = NULL;
    
    struct memblock ext_mem_22365;
    
    ext_mem_22365.references = NULL;
    
    struct memblock ext_mem_22366;
    
    ext_mem_22366.references = NULL;
    
    struct memblock mem_param_22328;
    
    mem_param_22328.references = NULL;
    
    struct memblock ext_mem_22409;
    
    ext_mem_22409.references = NULL;
    
    struct memblock mem_22332;
    
    mem_22332.references = NULL;
    
    struct memblock mem_22330;
    
    mem_22330.references = NULL;
    
    struct memblock ext_mem_22413;
    
    ext_mem_22413.references = NULL;
    
    struct memblock mem_param_tmp_22664;
    
    mem_param_tmp_22664.references = NULL;
    
    struct memblock ext_mem_21955;
    
    ext_mem_21955.references = NULL;
    
    struct memblock mem_21953;
    
    mem_21953.references = NULL;
    
    struct memblock mem_param_21950;
    
    mem_param_21950.references = NULL;
    
    struct memblock ext_mem_21959;
    
    ext_mem_21959.references = NULL;
    
    struct memblock mem_param_tmp_22662;
    
    mem_param_tmp_22662.references = NULL;
    
    struct memblock ext_mem_21942;
    
    ext_mem_21942.references = NULL;
    
    struct memblock ext_mem_21941;
    
    ext_mem_21941.references = NULL;
    
    struct memblock ext_mem_21940;
    
    ext_mem_21940.references = NULL;
    
    struct memblock mem_21938;
    
    mem_21938.references = NULL;
    
    struct memblock mem_param_21935;
    
    mem_param_21935.references = NULL;
    
    struct memblock ext_mem_21946;
    
    ext_mem_21946.references = NULL;
    
    struct memblock mem_21931;
    
    mem_21931.references = NULL;
    
    struct memblock mem_out_22656;
    
    mem_out_22656.references = NULL;
    
    struct memblock mem_out_22655;
    
    mem_out_22655.references = NULL;
    
    struct memblock mem_out_22654;
    
    mem_out_22654.references = NULL;
    
    int64_t prim_out_22657;
    int64_t prim_out_22658;
    bool prim_out_22659;
    
    // graph_orientation_futhark.fut:384:17-26
    
    int64_t i32_res_17102 = sext_i32_i64(n_14792);
    
    // graph_orientation_futhark.fut:372:14-23
    
    int64_t i32_res_17103 = sext_i32_i64(m_14793);
    
    // graph_orientation_futhark.fut:12:40-43
    
    int32_t zp_lhs_17104 = add32(1, m_14793);
    
    // graph_orientation_futhark.fut:12:44-47
    
    int32_t sink_res_17105 = add32(n_14792, zp_lhs_17104);
    
    // graph_orientation_futhark.fut:7:46-49
    
    int32_t zp_lhs_17109 = add32(2, m_14793);
    
    // graph_orientation_futhark.fut:7:50-53
    
    int32_t node_count_res_17110 = add32(n_14792, zp_lhs_17109);
    
    // graph_orientation_futhark.fut:18:37-28:26
    
    int64_t i32_res_17111 = sext_i32_i64(node_count_res_17110);
    
    // graph_orientation_futhark.fut:31:5-39:12
    
    bool bounds_invalid_upwards_17117 = slt64(i32_res_17103, (int64_t) 0);
    
    // graph_orientation_futhark.fut:31:5-39:12
    
    bool valid_17118 = !bounds_invalid_upwards_17117;
    
    // graph_orientation_futhark.fut:31:5-39:12
    
    bool range_valid_c_17119;
    
    if (!valid_17118) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Range ", (long long) (int64_t) 0, "..<", (long long) i32_res_17103, " is invalid.", "-> #0  graph_orientation_futhark.fut:31:5-39:12\n   #1  graph_orientation_futhark.fut:353:13-44\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:31:5-39:12
    
    int64_t binop_x_21929 = (int64_t) 8 * i32_res_17111;
    
    // graph_orientation_futhark.fut:31:5-39:12
    
    int64_t bytes_21930 = i32_res_17111 * binop_x_21929;
    
    // graph_orientation_futhark.fut:31:5-39:12
    if (memblock_alloc(ctx, &mem_21931, bytes_21930, "mem_21931")) {
        err = 1;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:31:5-39:12
    for (int64_t nest_i_22660 = 0; nest_i_22660 < i32_res_17111; nest_i_22660++) {
        for (int64_t nest_i_22661 = 0; nest_i_22661 < i32_res_17111; nest_i_22661++) {
            ((int64_t *) mem_21931.mem)[nest_i_22660 * i32_res_17111 + nest_i_22661] = (int64_t) 0;
        }
    }
    // graph_orientation_futhark.fut:31:5-39:12
    if (memblock_set(ctx, &mem_param_21935, &mem_21931, "mem_21931") != 0)
        return 1;
    for (int64_t i_17126 = 0; i_17126 < i32_res_17103; i_17126++) {
        // graph_orientation_futhark.fut:32:15-26
        
        int32_t i64_res_17128 = sext_i64_i32(i_17126);
        
        // graph_orientation_futhark.fut:34:15-20
        
        int64_t k_17130 = sext_i32_i64(i64_res_17128);
        
        // graph_orientation_futhark.fut:34:15-20
        
        bool x_17131 = sle64((int64_t) 0, k_17130);
        
        // graph_orientation_futhark.fut:34:15-20
        
        bool y_17132 = slt64(k_17130, dz2081U_14789);
        
        // graph_orientation_futhark.fut:34:15-20
        
        bool bounds_check_17133 = x_17131 && y_17132;
        
        // graph_orientation_futhark.fut:34:15-20
        
        bool index_certs_17134;
        
        if (!bounds_check_17133) {
            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) k_17130, "] out of bounds for array of shape [", (long long) dz2081U_14789, "].", "-> #0  graph_orientation_futhark.fut:34:15-20\n   #1  graph_orientation_futhark.fut:353:13-44\n"));
            err = FUTHARK_PROGRAM_ERROR;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:35:15-20
        
        bool y_17138 = slt64(k_17130, dz2082U_14790);
        
        // graph_orientation_futhark.fut:35:15-20
        
        bool bounds_check_17139 = x_17131 && y_17138;
        
        // graph_orientation_futhark.fut:35:15-20
        
        bool index_certs_17140;
        
        if (!bounds_check_17139) {
            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) k_17130, "] out of bounds for array of shape [", (long long) dz2082U_14790, "].", "-> #0  graph_orientation_futhark.fut:35:15-20\n   #1  graph_orientation_futhark.fut:353:13-44\n"));
            err = FUTHARK_PROGRAM_ERROR;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:9:36-39
        
        int32_t edge_node_res_17129 = add32(1, i64_res_17128);
        
        // graph_orientation_futhark.fut:34:15-20
        
        int32_t u_17135 = ((int32_t *) eu_mem_21927.mem)[k_17130];
        
        // graph_orientation_futhark.fut:35:15-20
        
        int32_t v_17141 = ((int32_t *) ev_mem_21928.mem)[k_17130];
        
        // graph_orientation_futhark.fut:36:16-37
        if (memblock_alloc(ctx, &mem_21938, bytes_21930, "mem_21938")) {
            err = 1;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:36:16-37
        // graph_orientation_futhark.fut:36:16-37
        lmad_copy_8b(ctx, 2, (uint64_t *) mem_21938.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (uint64_t *) mem_param_21935.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (int64_t []) {i32_res_17111, i32_res_17111});
        // graph_orientation_futhark.fut:36:16-37
        if (futrts_add_arc_6874(ctx, &ext_mem_21940, mem_21938, i32_res_17111, i32_res_17111, 0, edge_node_res_17129, (int64_t) 1) != 0) {
            err = 1;
            goto cleanup;
        }
        if (memblock_unref(ctx, &mem_21938, "mem_21938") != 0)
            return 1;
        // graph_orientation_futhark.fut:37:34-37
        
        int32_t add_arc_arg2_17143 = add32(zp_lhs_17104, u_17135);
        
        // graph_orientation_futhark.fut:37:16-43
        if (futrts_add_arc_6874(ctx, &ext_mem_21941, ext_mem_21940, i32_res_17111, i32_res_17111, edge_node_res_17129, add_arc_arg2_17143, (int64_t) 1) != 0) {
            err = 1;
            goto cleanup;
        }
        if (memblock_unref(ctx, &ext_mem_21940, "ext_mem_21940") != 0)
            return 1;
        // graph_orientation_futhark.fut:38:34-37
        
        int32_t add_arc_arg2_17145 = add32(zp_lhs_17104, v_17141);
        
        // graph_orientation_futhark.fut:38:16-43
        if (futrts_add_arc_6874(ctx, &ext_mem_21942, ext_mem_21941, i32_res_17111, i32_res_17111, edge_node_res_17129, add_arc_arg2_17145, (int64_t) 1) != 0) {
            err = 1;
            goto cleanup;
        }
        if (memblock_unref(ctx, &ext_mem_21941, "ext_mem_21941") != 0)
            return 1;
        if (memblock_set(ctx, &mem_param_tmp_22662, &ext_mem_21942, "ext_mem_21942") != 0)
            return 1;
        if (memblock_set(ctx, &mem_param_21935, &mem_param_tmp_22662, "mem_param_tmp_22662") != 0)
            return 1;
    }
    if (memblock_set(ctx, &ext_mem_21946, &mem_param_21935, "mem_param_21935") != 0)
        return 1;
    // graph_orientation_futhark.fut:42:5-44:42
    
    bool bounds_invalid_upwards_17147 = slt64(i32_res_17102, (int64_t) 0);
    
    // graph_orientation_futhark.fut:42:5-44:42
    
    bool valid_17148 = !bounds_invalid_upwards_17147;
    
    // graph_orientation_futhark.fut:42:5-44:42
    
    bool range_valid_c_17149;
    
    if (!valid_17148) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Range ", (long long) (int64_t) 0, "..<", (long long) i32_res_17102, " is invalid.", "-> #0  graph_orientation_futhark.fut:42:5-44:42\n   #1  graph_orientation_futhark.fut:353:13-44\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:42:5-44:42
    if (memblock_set(ctx, &mem_param_21950, &ext_mem_21946, "ext_mem_21946") != 0)
        return 1;
    for (int64_t i_17151 = 0; i_17151 < i32_res_17102; i_17151++) {
        // graph_orientation_futhark.fut:43:15-27
        
        int32_t i64_res_17153 = sext_i64_i32(i_17151);
        
        // graph_orientation_futhark.fut:44:33-42
        
        int64_t i_17154 = sext_i32_i64(i64_res_17153);
        
        // graph_orientation_futhark.fut:44:33-42
        
        bool x_17155 = sle64((int64_t) 0, i_17154);
        
        // graph_orientation_futhark.fut:44:33-42
        
        bool y_17156 = slt64(i_17154, dz2080U_14788);
        
        // graph_orientation_futhark.fut:44:33-42
        
        bool bounds_check_17157 = x_17155 && y_17156;
        
        // graph_orientation_futhark.fut:44:33-42
        
        bool index_certs_17158;
        
        if (!bounds_check_17157) {
            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) i_17154, "] out of bounds for array of shape [", (long long) dz2080U_14788, "].", "-> #0  graph_orientation_futhark.fut:44:33-42\n   #1  graph_orientation_futhark.fut:353:13-44\n"));
            err = FUTHARK_PROGRAM_ERROR;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:44:33-42
        
        int64_t add_arc_arg3_17159 = ((int64_t *) bounds_mem_21926.mem)[i_17154];
        
        // graph_orientation_futhark.fut:44:26-29
        
        int32_t add_arc_arg1_17160 = add32(zp_lhs_17104, i64_res_17153);
        
        // graph_orientation_futhark.fut:44:10-42
        if (memblock_alloc(ctx, &mem_21953, bytes_21930, "mem_21953")) {
            err = 1;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:44:10-42
        // graph_orientation_futhark.fut:44:10-42
        lmad_copy_8b(ctx, 2, (uint64_t *) mem_21953.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (uint64_t *) mem_param_21950.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (int64_t []) {i32_res_17111, i32_res_17111});
        // graph_orientation_futhark.fut:44:10-42
        if (futrts_add_arc_6874(ctx, &ext_mem_21955, mem_21953, i32_res_17111, i32_res_17111, add_arc_arg1_17160, sink_res_17105, add_arc_arg3_17159) != 0) {
            err = 1;
            goto cleanup;
        }
        if (memblock_unref(ctx, &mem_21953, "mem_21953") != 0)
            return 1;
        if (memblock_set(ctx, &mem_param_tmp_22664, &ext_mem_21955, "ext_mem_21955") != 0)
            return 1;
        if (memblock_set(ctx, &mem_param_21950, &mem_param_tmp_22664, "mem_param_tmp_22664") != 0)
            return 1;
    }
    if (memblock_set(ctx, &ext_mem_21959, &mem_param_21950, "mem_param_21950") != 0)
        return 1;
    if (memblock_unref(ctx, &ext_mem_21946, "ext_mem_21946") != 0)
        return 1;
    // graph_orientation_futhark.fut:355:5-357:30
    
    bool cond_17162 = mode_14791 == 0;
    
    // graph_orientation_futhark.fut:356:10-357:30
    
    bool cond_17163 = mode_14791 == 1;
    
    // graph_orientation_futhark.fut:272:12-57
    
    bool y_17712 = slt64((int64_t) 0, i32_res_17111);
    bool protect_assert_disj_18344 = cond_17163 || y_17712;
    bool protect_assert_disj_22624 = cond_17162 || protect_assert_disj_18344;
    
    // graph_orientation_futhark.fut:272:12-57
    
    bool index_certs_17714;
    
    if (!protect_assert_disj_22624) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:272:12-57\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:277:18-78
    
    bool cond_17735 = i32_res_17111 == (int64_t) 0;
    
    // graph_orientation_futhark.fut:277:18-78
    
    bool x_17736 = !cond_17735;
    
    // graph_orientation_futhark.fut:277:18-78
    
    int64_t tmp_17737 = sub64(i32_res_17111, (int64_t) 1);
    
    // graph_orientation_futhark.fut:277:18-78
    
    bool x_17738 = sle64((int64_t) 0, tmp_17737);
    
    // graph_orientation_futhark.fut:277:18-78
    
    bool y_17739 = slt64(tmp_17737, i32_res_17111);
    
    // graph_orientation_futhark.fut:277:18-78
    
    bool bounds_check_17740 = x_17738 && y_17739;
    
    // graph_orientation_futhark.fut:277:18-78
    
    bool protect_assert_disj_17741 = cond_17735 || bounds_check_17740;
    bool protect_assert_disj_18346 = cond_17163 || protect_assert_disj_17741;
    bool protect_assert_disj_22626 = cond_17162 || protect_assert_disj_18346;
    
    // graph_orientation_futhark.fut:277:18-78
    
    bool index_certs_17742;
    
    if (!protect_assert_disj_22626) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) tmp_17737, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:277:18-78\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:272:13-31
    
    int64_t bytes_21960 = (int64_t) 4 * i32_res_17111;
    
    // graph_orientation_futhark.fut:255:11-262:42
    
    bool cond_17897 = slt32(0, node_count_res_17110);
    
    // graph_orientation_futhark.fut:305:28-51
    
    bool loop_not_taken_17898 = !cond_17897;
    
    // graph_orientation_futhark.fut:305:28-51
    
    int64_t x_num_elems_17901 = i32_res_17111 * i32_res_17111;
    
    // graph_orientation_futhark.fut:212:13-21
    
    int64_t t_21255 = sext_i32_i64(sink_res_17105);
    
    // graph_orientation_futhark.fut:212:13-21
    
    bool x_21256 = sle64((int64_t) 0, t_21255);
    bool loop_not_taken_22583 = !cond_17163;
    bool protect_assert_disj_22584 = y_17712 || loop_not_taken_22583;
    bool protect_assert_disj_22628 = cond_17162 || protect_assert_disj_22584;
    
    // graph_orientation_futhark.fut:127:16-55
    
    bool index_certs_21257;
    
    if (!protect_assert_disj_22628) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:127:16-55\n   #1  graph_orientation_futhark.fut:211:19-47\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:137:9-144:30
    
    bool bounds_invalid_upwards_21258 = slt32(node_count_res_17110, 0);
    
    // graph_orientation_futhark.fut:137:9-144:30
    
    bool valid_21259 = !bounds_invalid_upwards_21258;
    bool protect_assert_disj_22586 = valid_21259 || loop_not_taken_22583;
    bool protect_assert_disj_22630 = cond_17162 || protect_assert_disj_22586;
    
    // graph_orientation_futhark.fut:137:9-144:30
    
    bool range_valid_c_21260;
    
    if (!protect_assert_disj_22630) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Range ", (long long) (int64_t) 0, "..<", (long long) i32_res_17111, " is invalid.", "-> #0  graph_orientation_futhark.fut:137:9-144:30\n   #1  graph_orientation_futhark.fut:211:19-47\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:212:13-21
    
    bool y_21261 = slt64(t_21255, i32_res_17111);
    
    // graph_orientation_futhark.fut:212:13-21
    
    bool bounds_check_21262 = x_21256 && y_21261;
    bool protect_assert_disj_22588 = bounds_check_21262 || loop_not_taken_22583;
    bool protect_assert_disj_22632 = cond_17162 || protect_assert_disj_22588;
    
    // graph_orientation_futhark.fut:212:13-21
    
    bool index_certs_21263;
    
    if (!protect_assert_disj_22632) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) t_21255, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:212:13-21\n   #1  graph_orientation_futhark.fut:356:31-44\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:182:10-186:33
    
    bool cond_21342 = sink_res_17105 == 0;
    
    // graph_orientation_futhark.fut:182:10-186:33
    
    bool cond_21343 = !cond_21342;
    
    // graph_orientation_futhark.fut:183:36-44
    
    bool loop_cond_t_res_21344 = sink_res_17105 == -1;
    
    // graph_orientation_futhark.fut:183:36-44
    
    bool loop_cond_t_res_21345 = !loop_cond_t_res_21344;
    
    // graph_orientation_futhark.fut:219:36-75
    
    bool x_21346 = cond_21343 && loop_cond_t_res_21345;
    bool loop_not_taken_22534 = !cond_17162;
    bool protect_assert_disj_22535 = y_17712 || loop_not_taken_22534;
    
    // graph_orientation_futhark.fut:51:15-51
    
    bool index_certs_20968;
    
    if (!protect_assert_disj_22535) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:51:15-51\n   #1  graph_orientation_futhark.fut:112:27-43\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    
    bool protect_assert_disj_22537 = valid_21259 || loop_not_taken_22534;
    
    // graph_orientation_futhark.fut:61:9-69:31
    
    bool range_valid_c_20971;
    
    if (!protect_assert_disj_22537) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Range ", (long long) (int64_t) 0, "..<", (long long) i32_res_17111, " is invalid.", "-> #0  graph_orientation_futhark.fut:61:9-69:31\n   #1  graph_orientation_futhark.fut:112:27-43\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    
    int64_t ext_22412;
    int64_t ext_22411;
    int64_t ext_22410;
    int64_t graph_orientation_solve_res_17166;
    
    if (cond_17162) {
        // graph_orientation_futhark.fut:51:16-36
        if (memblock_alloc(ctx, &mem_22330, bytes_21960, "mem_22330")) {
            err = 1;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:52:17-35
        if (memblock_alloc(ctx, &mem_22332, bytes_21960, "mem_22332")) {
            err = 1;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:109:5-121:57
        
        int64_t ext_22408;
        int64_t ext_22407;
        int64_t ext_22406;
        bool edmonds_karp_res_20972;
        int64_t edmonds_karp_res_20974;
        bool loop_while_20975;
        int64_t total_20977;
        int64_t ctx_param_ext_22325;
        int64_t ctx_param_ext_22326;
        int64_t ctx_param_ext_22327;
        
        if (memblock_set(ctx, &mem_param_22328, &mem_21931, "mem_21931") != 0)
            return 1;
        ctx_param_ext_22325 = (int64_t) 0;
        ctx_param_ext_22326 = i32_res_17111;
        ctx_param_ext_22327 = (int64_t) 1;
        loop_while_20975 = 1;
        total_20977 = (int64_t) 0;
        while (loop_while_20975) {
            // graph_orientation_futhark.fut:51:16-36
            for (int64_t nest_i_22673 = 0; nest_i_22673 < i32_res_17111; nest_i_22673++) {
                ((int32_t *) mem_22330.mem)[nest_i_22673] = -1;
            }
            // graph_orientation_futhark.fut:51:15-51
            ((int32_t *) mem_22330.mem)[(int64_t) 0] = 0;
            // graph_orientation_futhark.fut:52:17-35
            for (int64_t nest_i_22674 = 0; nest_i_22674 < i32_res_17111; nest_i_22674++) {
                ((int32_t *) mem_22332.mem)[nest_i_22674] = 0;
            }
            // graph_orientation_futhark.fut:52:16-49
            ((int32_t *) mem_22332.mem)[(int64_t) 0] = 0;
            // graph_orientation_futhark.fut:55:5-70:49
            
            int64_t ext_22364;
            int64_t ext_22363;
            int64_t ext_22362;
            int64_t ext_22361;
            bool bfs_res_20982;
            int32_t bfs_res_20983;
            int32_t bfs_res_20984;
            bool bfs_res_20986;
            bool loop_while_20988;
            int32_t qstart_20989;
            int32_t qend_20990;
            bool found_20992;
            int64_t ctx_param_ext_22333;
            int64_t ctx_param_ext_22334;
            int64_t ctx_param_ext_22336;
            int64_t ctx_param_ext_22337;
            
            if (memblock_set(ctx, &mem_param_22335, &mem_22330, "mem_22330") != 0)
                return 1;
            if (memblock_set(ctx, &mem_param_22338, &mem_22332, "mem_22332") != 0)
                return 1;
            ctx_param_ext_22333 = (int64_t) 0;
            ctx_param_ext_22334 = (int64_t) 1;
            ctx_param_ext_22336 = (int64_t) 0;
            ctx_param_ext_22337 = (int64_t) 1;
            loop_while_20988 = 1;
            qstart_20989 = 0;
            qend_20990 = 1;
            found_20992 = 0;
            while (loop_while_20988) {
                // graph_orientation_futhark.fut:58:15-28
                
                int64_t qstart_20994 = sext_i32_i64(qstart_20989);
                
                // graph_orientation_futhark.fut:58:15-28
                
                bool x_20995 = sle64((int64_t) 0, qstart_20994);
                
                // graph_orientation_futhark.fut:58:15-28
                
                bool y_20996 = slt64(qstart_20994, i32_res_17111);
                
                // graph_orientation_futhark.fut:58:15-28
                
                bool bounds_check_20997 = x_20995 && y_20996;
                
                // graph_orientation_futhark.fut:58:15-28
                
                bool index_certs_20998;
                
                if (!bounds_check_20997) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) qstart_20994, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:58:15-28\n   #1  graph_orientation_futhark.fut:112:27-43\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:58:15-28
                
                int32_t u_20999 = ((int32_t *) mem_param_22338.mem)[ctx_param_ext_22336 + qstart_20994 * ctx_param_ext_22337];
                
                // graph_orientation_futhark.fut:59:28-31
                
                int32_t qstartzq_21000 = add32(1, qstart_20989);
                
                // graph_orientation_futhark.fut:63:33-39
                
                int64_t u_21019 = sext_i32_i64(u_20999);
                
                // graph_orientation_futhark.fut:63:33-39
                
                bool x_21020 = sle64((int64_t) 0, u_21019);
                
                // graph_orientation_futhark.fut:63:33-39
                
                bool y_21021 = slt64(u_21019, i32_res_17111);
                
                // graph_orientation_futhark.fut:63:33-39
                
                bool bounds_check_21022 = x_21020 && y_21021;
                
                // graph_orientation_futhark.fut:61:9-69:31
                
                int64_t ext_22358;
                int64_t ext_22357;
                int64_t ext_22356;
                int64_t ext_22355;
                int32_t loopres_21003;
                bool loopres_21004;
                int32_t qe_21008;
                bool f_21009;
                int64_t ctx_param_ext_22339;
                int64_t ctx_param_ext_22340;
                int64_t ctx_param_ext_22342;
                int64_t ctx_param_ext_22343;
                
                if (memblock_set(ctx, &mem_param_22341, &mem_param_22335, "mem_param_22335") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22344, &mem_param_22338, "mem_param_22338") != 0)
                    return 1;
                ctx_param_ext_22339 = ctx_param_ext_22333;
                ctx_param_ext_22340 = ctx_param_ext_22334;
                ctx_param_ext_22342 = ctx_param_ext_22336;
                ctx_param_ext_22343 = ctx_param_ext_22337;
                qe_21008 = qend_20990;
                f_21009 = found_20992;
                for (int64_t i_21005 = 0; i_21005 < i32_res_17111; i_21005++) {
                    // graph_orientation_futhark.fut:112:27-43
                    
                    int32_t binop_x_21010 = sext_i64_i32(i_21005);
                    
                    // graph_orientation_futhark.fut:63:16-20
                    
                    int64_t v_21011 = sext_i32_i64(binop_x_21010);
                    
                    // graph_orientation_futhark.fut:63:16-20
                    
                    bool x_21012 = sle64((int64_t) 0, v_21011);
                    
                    // graph_orientation_futhark.fut:63:16-20
                    
                    bool y_21013 = slt64(v_21011, i32_res_17111);
                    
                    // graph_orientation_futhark.fut:63:16-20
                    
                    bool bounds_check_21014 = x_21012 && y_21013;
                    
                    // graph_orientation_futhark.fut:63:16-20
                    
                    bool index_certs_21015;
                    
                    if (!bounds_check_21014) {
                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21011, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:63:16-20\n   #1  graph_orientation_futhark.fut:112:27-43\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                        err = FUTHARK_PROGRAM_ERROR;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:63:16-20
                    
                    int32_t zeze_lhs_21016 = ((int32_t *) mem_param_22341.mem)[ctx_param_ext_22339 + v_21011 * ctx_param_ext_22340];
                    
                    // graph_orientation_futhark.fut:63:13-69:31
                    
                    bool cond_21017 = zeze_lhs_21016 == -1;
                    bool loop_not_taken_22524 = !cond_21017;
                    bool protect_assert_disj_22525 = bounds_check_21022 || loop_not_taken_22524;
                    
                    // graph_orientation_futhark.fut:63:33-39
                    
                    bool index_certs_21023;
                    
                    if (!protect_assert_disj_22525) {
                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21019, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:63:33-39\n   #1  graph_orientation_futhark.fut:112:27-43\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                        err = FUTHARK_PROGRAM_ERROR;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:112:27-43
                    
                    int64_t zm_lhs_21024;
                    
                    if (cond_21017) {
                        int64_t x_22526 = ((int64_t *) ext_mem_21959.mem)[u_21019 * i32_res_17111 + v_21011];
                        
                        zm_lhs_21024 = x_22526;
                    } else {
                        zm_lhs_21024 = (int64_t) 0;
                    }
                    // graph_orientation_futhark.fut:112:27-43
                    
                    int64_t zm_rhs_21025;
                    
                    if (cond_21017) {
                        int64_t x_22528 = ((int64_t *) mem_param_22328.mem)[ctx_param_ext_22325 + (u_21019 * ctx_param_ext_22326 + v_21011 * ctx_param_ext_22327)];
                        
                        zm_rhs_21025 = x_22528;
                    } else {
                        zm_rhs_21025 = (int64_t) 0;
                    }
                    // graph_orientation_futhark.fut:63:43-55
                    
                    int64_t zg_lhs_21026 = sub64(zm_lhs_21024, zm_rhs_21025);
                    
                    // graph_orientation_futhark.fut:63:56-62
                    
                    bool cond_t_res_21027 = slt64((int64_t) 0, zg_lhs_21026);
                    bool x_22645 = cond_21017 && cond_t_res_21027;
                    int64_t ext_22353;
                    
                    if (x_22645) {
                        ext_22353 = (int64_t) 0;
                    } else {
                        ext_22353 = ctx_param_ext_22339;
                    }
                    
                    int64_t ext_22352;
                    
                    if (x_22645) {
                        ext_22352 = (int64_t) 1;
                    } else {
                        ext_22352 = ctx_param_ext_22340;
                    }
                    
                    int64_t ext_22350;
                    
                    if (x_22645) {
                        ext_22350 = (int64_t) 0;
                    } else {
                        ext_22350 = ctx_param_ext_22342;
                    }
                    
                    int64_t ext_22349;
                    
                    if (x_22645) {
                        ext_22349 = (int64_t) 1;
                    } else {
                        ext_22349 = ctx_param_ext_22343;
                    }
                    
                    int32_t loopres_21030;
                    bool loopres_21031;
                    
                    if (x_22645) {
                        // graph_orientation_futhark.fut:65:24-44
                        
                        int64_t qe_21035 = sext_i32_i64(qe_21008);
                        
                        // graph_orientation_futhark.fut:65:24-44
                        
                        bool x_21036 = sle64((int64_t) 0, qe_21035);
                        
                        // graph_orientation_futhark.fut:65:24-44
                        
                        bool y_21037 = slt64(qe_21035, i32_res_17111);
                        
                        // graph_orientation_futhark.fut:65:24-44
                        
                        bool bounds_check_21038 = x_21036 && y_21037;
                        
                        // graph_orientation_futhark.fut:65:24-44
                        
                        bool index_certs_21039;
                        
                        if (!bounds_check_21038) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) qe_21035, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:65:24-44\n   #1  graph_orientation_futhark.fut:112:27-43\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:64:24-30
                        if (memblock_alloc(ctx, &mem_22346, bytes_21960, "mem_22346")) {
                            err = 1;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:64:24-30
                        // graph_orientation_futhark.fut:64:24-30
                        lmad_copy_4b(ctx, 1, (uint32_t *) mem_22346.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22341.mem, ctx_param_ext_22339, (int64_t []) {ctx_param_ext_22340}, (int64_t []) {i32_res_17111});
                        // graph_orientation_futhark.fut:64:24-43
                        ((int32_t *) mem_22346.mem)[v_21011] = u_20999;
                        // graph_orientation_futhark.fut:65:24-30
                        if (memblock_alloc(ctx, &mem_22348, bytes_21960, "mem_22348")) {
                            err = 1;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:65:24-30
                        // graph_orientation_futhark.fut:65:24-30
                        lmad_copy_4b(ctx, 1, (uint32_t *) mem_22348.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22344.mem, ctx_param_ext_22342, (int64_t []) {ctx_param_ext_22343}, (int64_t []) {i32_res_17111});
                        // graph_orientation_futhark.fut:65:24-44
                        ((int32_t *) mem_22348.mem)[qe_21035] = binop_x_21010;
                        // graph_orientation_futhark.fut:66:28-31
                        
                        int32_t qezq_21041 = add32(1, qe_21008);
                        
                        // graph_orientation_futhark.fut:67:32-36
                        
                        bool fzq_f_res_21042 = binop_x_21010 == sink_res_17105;
                        
                        // graph_orientation_futhark.fut:112:27-43
                        
                        bool x_21043 = !f_21009;
                        
                        // graph_orientation_futhark.fut:112:27-43
                        
                        bool y_21044 = fzq_f_res_21042 && x_21043;
                        
                        // graph_orientation_futhark.fut:112:27-43
                        
                        bool fzq_21045 = f_21009 || y_21044;
                        
                        if (memblock_set(ctx, &ext_mem_22354, &mem_22346, "mem_22346") != 0)
                            return 1;
                        if (memblock_set(ctx, &ext_mem_22351, &mem_22348, "mem_22348") != 0)
                            return 1;
                        loopres_21030 = qezq_21041;
                        loopres_21031 = fzq_21045;
                    } else {
                        if (memblock_set(ctx, &ext_mem_22354, &mem_param_22341, "mem_param_22341") != 0)
                            return 1;
                        if (memblock_set(ctx, &ext_mem_22351, &mem_param_22344, "mem_param_22344") != 0)
                            return 1;
                        loopres_21030 = qe_21008;
                        loopres_21031 = f_21009;
                    }
                    if (memblock_set(ctx, &mem_param_tmp_22687, &ext_mem_22354, "ext_mem_22354") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_tmp_22688, &ext_mem_22351, "ext_mem_22351") != 0)
                        return 1;
                    
                    int64_t ctx_param_ext_tmp_22689 = ext_22353;
                    int64_t ctx_param_ext_tmp_22690 = ext_22352;
                    int64_t ctx_param_ext_tmp_22691 = ext_22350;
                    int64_t ctx_param_ext_tmp_22692 = ext_22349;
                    int32_t qe_tmp_22695 = loopres_21030;
                    bool f_tmp_22696 = loopres_21031;
                    
                    if (memblock_set(ctx, &mem_param_22341, &mem_param_tmp_22687, "mem_param_tmp_22687") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_22344, &mem_param_tmp_22688, "mem_param_tmp_22688") != 0)
                        return 1;
                    ctx_param_ext_22339 = ctx_param_ext_tmp_22689;
                    ctx_param_ext_22340 = ctx_param_ext_tmp_22690;
                    ctx_param_ext_22342 = ctx_param_ext_tmp_22691;
                    ctx_param_ext_22343 = ctx_param_ext_tmp_22692;
                    qe_21008 = qe_tmp_22695;
                    f_21009 = f_tmp_22696;
                }
                if (memblock_set(ctx, &ext_mem_22360, &mem_param_22341, "mem_param_22341") != 0)
                    return 1;
                if (memblock_set(ctx, &ext_mem_22359, &mem_param_22344, "mem_param_22344") != 0)
                    return 1;
                ext_22358 = ctx_param_ext_22339;
                ext_22357 = ctx_param_ext_22340;
                ext_22356 = ctx_param_ext_22342;
                ext_22355 = ctx_param_ext_22343;
                loopres_21003 = qe_21008;
                loopres_21004 = f_21009;
                // graph_orientation_futhark.fut:56:13-39
                
                bool cond_21046 = slt32(qstartzq_21000, loopres_21003);
                
                // graph_orientation_futhark.fut:56:30-39
                
                bool not_res_21047 = !loopres_21004;
                
                // graph_orientation_futhark.fut:112:27-43
                
                bool x_21048 = cond_21046 && not_res_21047;
                
                if (memblock_set(ctx, &mem_param_tmp_22675, &ext_mem_22360, "ext_mem_22360") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_tmp_22676, &ext_mem_22359, "ext_mem_22359") != 0)
                    return 1;
                
                int64_t ctx_param_ext_tmp_22677 = ext_22358;
                int64_t ctx_param_ext_tmp_22678 = ext_22357;
                int64_t ctx_param_ext_tmp_22679 = ext_22356;
                int64_t ctx_param_ext_tmp_22680 = ext_22355;
                bool loop_while_tmp_22681 = x_21048;
                int32_t qstart_tmp_22682 = qstartzq_21000;
                int32_t qend_tmp_22683 = loopres_21003;
                bool found_tmp_22685 = loopres_21004;
                
                if (memblock_set(ctx, &mem_param_22335, &mem_param_tmp_22675, "mem_param_tmp_22675") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22338, &mem_param_tmp_22676, "mem_param_tmp_22676") != 0)
                    return 1;
                ctx_param_ext_22333 = ctx_param_ext_tmp_22677;
                ctx_param_ext_22334 = ctx_param_ext_tmp_22678;
                ctx_param_ext_22336 = ctx_param_ext_tmp_22679;
                ctx_param_ext_22337 = ctx_param_ext_tmp_22680;
                loop_while_20988 = loop_while_tmp_22681;
                qstart_20989 = qstart_tmp_22682;
                qend_20990 = qend_tmp_22683;
                found_20992 = found_tmp_22685;
            }
            if (memblock_set(ctx, &ext_mem_22366, &mem_param_22335, "mem_param_22335") != 0)
                return 1;
            if (memblock_set(ctx, &ext_mem_22365, &mem_param_22338, "mem_param_22338") != 0)
                return 1;
            ext_22364 = ctx_param_ext_22333;
            ext_22363 = ctx_param_ext_22334;
            ext_22362 = ctx_param_ext_22336;
            ext_22361 = ctx_param_ext_22337;
            bfs_res_20982 = loop_while_20988;
            bfs_res_20983 = qstart_20989;
            bfs_res_20984 = qend_20990;
            bfs_res_20986 = found_20992;
            // graph_orientation_futhark.fut:113:10-121:57
            
            bool not_res_21049 = !bfs_res_20986;
            
            // graph_orientation_futhark.fut:113:10-121:57
            
            bool cond_21050;
            
            if (not_res_21049) {
                cond_21050 = 1;
            } else {
                // graph_orientation_futhark.fut:113:26-33
                
                bool index_certs_21055;
                
                if (!bounds_check_21262) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) t_21255, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:113:26-33\n   #1  graph_orientation_futhark.fut:355:26-46\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:113:26-33
                
                int32_t zeze_lhs_21056 = ((int32_t *) ext_mem_22366.mem)[ext_22364 + t_21255 * ext_22363];
                
                // graph_orientation_futhark.fut:113:34-42
                
                bool cond_f_res_21057 = zeze_lhs_21056 == -1;
                
                cond_21050 = cond_f_res_21057;
            }
            // graph_orientation_futhark.fut:113:10-121:57
            
            int64_t ext_22404;
            int64_t ext_22403;
            int64_t ext_22402;
            bool loopres_21058;
            int64_t loopres_21060;
            
            if (cond_21050) {
                if (memblock_set(ctx, &ext_mem_22405, &mem_param_22328, "mem_param_22328") != 0)
                    return 1;
                ext_22404 = ctx_param_ext_22325;
                ext_22403 = ctx_param_ext_22326;
                ext_22402 = ctx_param_ext_22327;
                loopres_21058 = 1;
                loopres_21060 = total_20977;
            } else {
                // graph_orientation_futhark.fut:75:5-79:25
                if (memblock_alloc(ctx, &mem_22367, (int64_t) 0, "mem_22367")) {
                    err = 1;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:75:5-79:25
                
                int64_t augment_path_res_21062;
                bool augment_path_res_21063;
                int32_t augment_path_res_21064;
                int64_t loop_dz2084U_21066;
                bool loop_while_21067;
                int32_t v_21068;
                
                if (memblock_set(ctx, &mem_param_22370, &mem_22367, "mem_22367") != 0)
                    return 1;
                loop_dz2084U_21066 = (int64_t) 0;
                loop_while_21067 = x_21346;
                v_21068 = sink_res_17105;
                while (loop_while_21067) {
                    // graph_orientation_futhark.fut:78:15-22
                    
                    int64_t v_21070 = sext_i32_i64(v_21068);
                    
                    // graph_orientation_futhark.fut:78:15-22
                    
                    bool x_21071 = sle64((int64_t) 0, v_21070);
                    
                    // graph_orientation_futhark.fut:78:15-22
                    
                    bool y_21072 = slt64(v_21070, i32_res_17111);
                    
                    // graph_orientation_futhark.fut:78:15-22
                    
                    bool bounds_check_21073 = x_21071 && y_21072;
                    
                    // graph_orientation_futhark.fut:78:15-22
                    
                    bool index_certs_21074;
                    
                    if (!bounds_check_21073) {
                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21070, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:78:15-22\n   #1  graph_orientation_futhark.fut:116:23-44\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                        err = FUTHARK_PROGRAM_ERROR;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:79:14-24
                    
                    int64_t conc_tmp_21077 = (int64_t) 1 + loop_dz2084U_21066;
                    
                    // graph_orientation_futhark.fut:79:14-24
                    
                    int64_t bytes_22372 = (int64_t) 4 * conc_tmp_21077;
                    
                    // graph_orientation_futhark.fut:78:15-22
                    
                    int32_t p_21075 = ((int32_t *) ext_mem_22366.mem)[ext_22364 + v_21070 * ext_22363];
                    
                    // graph_orientation_futhark.fut:79:14-24
                    if (memblock_alloc(ctx, &mem_22373, bytes_22372, "mem_22373")) {
                        err = 1;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:116:23-44
                    for (int64_t nest_i_22702 = 0; nest_i_22702 < (int64_t) 1; nest_i_22702++) {
                        ((int32_t *) mem_22373.mem)[nest_i_22702] = v_21068;
                    }
                    // graph_orientation_futhark.fut:79:14-24
                    
                    int64_t tmp_offs_22703 = (int64_t) 0;
                    
                    if (!(tmp_offs_22703 == (int64_t) 0)) {
                        // graph_orientation_futhark.fut:79:14-24
                        lmad_copy_4b(ctx, 1, (uint32_t *) mem_22373.mem, tmp_offs_22703, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_22373.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {(int64_t) 1});
                    }
                    tmp_offs_22703 += (int64_t) 1;
                    // graph_orientation_futhark.fut:79:14-24
                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22373.mem, tmp_offs_22703, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22370.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {loop_dz2084U_21066});
                    tmp_offs_22703 += loop_dz2084U_21066;
                    // graph_orientation_futhark.fut:76:13-33
                    
                    bool cond_21079 = p_21075 == 0;
                    
                    // graph_orientation_futhark.fut:76:13-33
                    
                    bool cond_21080 = !cond_21079;
                    
                    // graph_orientation_futhark.fut:76:25-33
                    
                    bool loop_cond_t_res_21081 = p_21075 == -1;
                    
                    // graph_orientation_futhark.fut:76:25-33
                    
                    bool loop_cond_t_res_21082 = !loop_cond_t_res_21081;
                    
                    // graph_orientation_futhark.fut:116:23-44
                    
                    bool x_21083 = cond_21080 && loop_cond_t_res_21082;
                    
                    if (memblock_set(ctx, &mem_param_tmp_22697, &mem_22373, "mem_22373") != 0)
                        return 1;
                    
                    int64_t loop_dz2084U_tmp_22698 = conc_tmp_21077;
                    bool loop_while_tmp_22699 = x_21083;
                    int32_t v_tmp_22700 = p_21075;
                    
                    if (memblock_set(ctx, &mem_param_22370, &mem_param_tmp_22697, "mem_param_tmp_22697") != 0)
                        return 1;
                    loop_dz2084U_21066 = loop_dz2084U_tmp_22698;
                    loop_while_21067 = loop_while_tmp_22699;
                    v_21068 = v_tmp_22700;
                }
                if (memblock_set(ctx, &ext_mem_22376, &mem_param_22370, "mem_param_22370") != 0)
                    return 1;
                augment_path_res_21062 = loop_dz2084U_21066;
                augment_path_res_21063 = loop_while_21067;
                augment_path_res_21064 = v_21068;
                if (memblock_unref(ctx, &mem_22367, "mem_22367") != 0)
                    return 1;
                // graph_orientation_futhark.fut:80:6-39
                
                bool cond_21084 = augment_path_res_21064 == 0;
                
                // graph_orientation_futhark.fut:80:21-31
                
                int64_t conc_tmp_21085 = (int64_t) 1 + augment_path_res_21062;
                
                // graph_orientation_futhark.fut:355:26-46
                
                int64_t augment_path_res_21086;
                
                if (cond_21084) {
                    augment_path_res_21086 = conc_tmp_21085;
                } else {
                    augment_path_res_21086 = (int64_t) 0;
                }
                // graph_orientation_futhark.fut:355:26-46
                
                int64_t bytes_22377 = (int64_t) 4 * augment_path_res_21086;
                
                // graph_orientation_futhark.fut:355:26-46
                if (cond_21084) {
                    // graph_orientation_futhark.fut:355:26-46
                    if (memblock_alloc(ctx, &mem_22381, bytes_22377, "mem_22381")) {
                        err = 1;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:116:23-44
                    for (int64_t nest_i_22704 = 0; nest_i_22704 < (int64_t) 1; nest_i_22704++) {
                        ((int32_t *) mem_22381.mem)[nest_i_22704] = 0;
                    }
                    // graph_orientation_futhark.fut:355:26-46
                    
                    int64_t tmp_offs_22705 = (int64_t) 0;
                    
                    if (!(tmp_offs_22705 == (int64_t) 0)) {
                        // graph_orientation_futhark.fut:355:26-46
                        lmad_copy_4b(ctx, 1, (uint32_t *) mem_22381.mem, tmp_offs_22705, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_22381.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {(int64_t) 1});
                    }
                    tmp_offs_22705 += (int64_t) 1;
                    // graph_orientation_futhark.fut:355:26-46
                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22381.mem, tmp_offs_22705, (int64_t []) {(int64_t) 1}, (uint32_t *) ext_mem_22376.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {augment_path_res_21062});
                    tmp_offs_22705 += augment_path_res_21062;
                    if (memblock_set(ctx, &ext_mem_22382, &mem_22381, "mem_22381") != 0)
                        return 1;
                } else {
                    // graph_orientation_futhark.fut:355:26-46
                    if (memblock_alloc(ctx, &mem_22378, bytes_22377, "mem_22378")) {
                        err = 1;
                        goto cleanup;
                    }
                    if (memblock_set(ctx, &ext_mem_22382, &mem_22378, "mem_22378") != 0)
                        return 1;
                }
                if (memblock_unref(ctx, &ext_mem_22376, "ext_mem_22376") != 0)
                    return 1;
                // graph_orientation_futhark.fut:84:6-89:72
                
                bool cond_21091 = slt64(augment_path_res_21086, (int64_t) 2);
                
                // graph_orientation_futhark.fut:85:20-23
                
                int64_t k_21092 = sub64(augment_path_res_21086, (int64_t) 1);
                bool x_22512 = !cond_21091;
                
                // graph_orientation_futhark.fut:86:17-28
                
                bool empty_slice_21094 = k_21092 == (int64_t) 0;
                
                // graph_orientation_futhark.fut:86:17-28
                
                int64_t m_21095 = sub64(k_21092, (int64_t) 1);
                
                // graph_orientation_futhark.fut:86:17-28
                
                bool zzero_leq_i_p_m_t_s_21096 = sle64((int64_t) 0, m_21095);
                
                // graph_orientation_futhark.fut:86:17-28
                
                bool i_p_m_t_s_leq_w_21097 = slt64(m_21095, augment_path_res_21086);
                
                // graph_orientation_futhark.fut:86:17-28
                
                bool i_lte_j_21098 = sle64((int64_t) 0, k_21092);
                
                // graph_orientation_futhark.fut:86:17-28
                
                bool y_21099 = zzero_leq_i_p_m_t_s_21096 && i_p_m_t_s_leq_w_21097;
                
                // graph_orientation_futhark.fut:86:17-28
                
                bool forwards_ok_21100 = i_lte_j_21098 && y_21099;
                
                // graph_orientation_futhark.fut:86:17-28
                
                bool ok_or_empty_21101 = empty_slice_21094 || forwards_ok_21100;
                bool protect_assert_disj_22515 = cond_21091 || ok_or_empty_21101;
                
                // graph_orientation_futhark.fut:86:17-28
                
                bool index_certs_21102;
                
                if (!protect_assert_disj_22515) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, ":", (long long) k_21092, "] out of bounds for array of shape [", (long long) augment_path_res_21086, "].", "-> #0  graph_orientation_futhark.fut:86:17-28\n   #1  graph_orientation_futhark.fut:117:20-46\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:87:17-36
                
                bool i_p_m_t_s_leq_w_21103 = slt64(m_21095, k_21092);
                
                // graph_orientation_futhark.fut:87:17-36
                
                bool y_21104 = zzero_leq_i_p_m_t_s_21096 && i_p_m_t_s_leq_w_21103;
                
                // graph_orientation_futhark.fut:87:17-36
                
                bool ok_or_empty_21105 = empty_slice_21094 || y_21104;
                bool protect_assert_disj_22517 = cond_21091 || ok_or_empty_21105;
                
                // graph_orientation_futhark.fut:87:17-36
                
                bool index_certs_21106;
                
                if (!protect_assert_disj_22517) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, ":", (long long) k_21092, "] out of bounds for array of shape [", (long long) k_21092, "].", "-> #0  graph_orientation_futhark.fut:87:17-36\n   #1  graph_orientation_futhark.fut:117:20-46\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:87:25-36
                
                bool i_p_m_t_s_leq_w_21107 = slt64(k_21092, augment_path_res_21086);
                
                // graph_orientation_futhark.fut:87:25-36
                
                bool y_21108 = i_lte_j_21098 && i_p_m_t_s_leq_w_21107;
                
                // graph_orientation_futhark.fut:87:25-36
                
                bool i_lte_j_21109 = sle64((int64_t) 1, augment_path_res_21086);
                
                // graph_orientation_futhark.fut:87:25-36
                
                bool forwards_ok_21110 = y_21108 && i_lte_j_21109;
                
                // graph_orientation_futhark.fut:87:25-36
                
                bool ok_or_empty_21111 = empty_slice_21094 || forwards_ok_21110;
                bool protect_assert_disj_22519 = cond_21091 || ok_or_empty_21111;
                
                // graph_orientation_futhark.fut:87:25-36
                
                bool index_certs_21112;
                
                if (!protect_assert_disj_22519) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 1, ":] out of bounds for array of shape [", (long long) augment_path_res_21086, "].", "-> #0  graph_orientation_futhark.fut:87:25-36\n   #1  graph_orientation_futhark.fut:117:20-46\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:88:11-89:71
                
                int64_t defunc_0_reduce_res_21115;
                
                if (x_22512) {
                    int64_t x_22520;
                    int64_t redout_21768 = (int64_t) 9223372036854775807;
                    
                    for (int64_t i_21769 = 0; i_21769 < k_21092; i_21769++) {
                        int32_t eta_p_21116 = ((int32_t *) ext_mem_22382.mem)[i_21769];
                        int64_t index_primexp_21869 = (int64_t) 1 + i_21769;
                        int32_t eta_p_21117 = ((int32_t *) ext_mem_22382.mem)[index_primexp_21869];
                        
                        // graph_orientation_futhark.fut:89:42-48
                        
                        int64_t u_21118 = sext_i32_i64(eta_p_21116);
                        
                        // graph_orientation_futhark.fut:89:42-48
                        
                        bool x_21119 = sle64((int64_t) 0, u_21118);
                        
                        // graph_orientation_futhark.fut:89:42-48
                        
                        bool y_21120 = slt64(u_21118, i32_res_17111);
                        
                        // graph_orientation_futhark.fut:89:42-48
                        
                        bool bounds_check_21121 = x_21119 && y_21120;
                        
                        // graph_orientation_futhark.fut:89:42-48
                        
                        bool index_certs_21122;
                        
                        if (!bounds_check_21121) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21118, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:89:42-48\n   #1  graph_orientation_futhark.fut:117:20-46\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:89:42-51
                        
                        int64_t v_21123 = sext_i32_i64(eta_p_21117);
                        
                        // graph_orientation_futhark.fut:89:42-51
                        
                        bool x_21124 = sle64((int64_t) 0, v_21123);
                        
                        // graph_orientation_futhark.fut:89:42-51
                        
                        bool y_21125 = slt64(v_21123, i32_res_17111);
                        
                        // graph_orientation_futhark.fut:89:42-51
                        
                        bool bounds_check_21126 = x_21124 && y_21125;
                        
                        // graph_orientation_futhark.fut:89:42-51
                        
                        bool index_certs_21127;
                        
                        if (!bounds_check_21126) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21123, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:89:42-51\n   #1  graph_orientation_futhark.fut:117:20-46\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:89:14-71
                        
                        int64_t zm_lhs_21128 = ((int64_t *) ext_mem_21959.mem)[u_21118 * i32_res_17111 + v_21123];
                        
                        // graph_orientation_futhark.fut:89:14-71
                        
                        int64_t zm_rhs_21129 = ((int64_t *) mem_param_22328.mem)[ctx_param_ext_22325 + (u_21118 * ctx_param_ext_22326 + v_21123 * ctx_param_ext_22327)];
                        
                        // graph_orientation_futhark.fut:89:52-64
                        
                        int64_t lifted_lambda_res_21130 = sub64(zm_lhs_21128, zm_rhs_21129);
                        
                        // graph_orientation_futhark.fut:88:18-25
                        
                        int64_t min_res_21133 = smin64(lifted_lambda_res_21130, redout_21768);
                        int64_t redout_tmp_22706 = min_res_21133;
                        
                        redout_21768 = redout_tmp_22706;
                    }
                    x_22520 = redout_21768;
                    defunc_0_reduce_res_21115 = x_22520;
                } else {
                    defunc_0_reduce_res_21115 = (int64_t) 0;
                }
                // graph_orientation_futhark.fut:84:6-89:72
                
                int64_t min_residual_res_21093;
                
                if (cond_21091) {
                    min_residual_res_21093 = (int64_t) 0;
                } else {
                    min_residual_res_21093 = defunc_0_reduce_res_21115;
                }
                // graph_orientation_futhark.fut:118:39-46
                
                bool cond_f_res_21139 = sle64(min_residual_res_21093, (int64_t) 0);
                
                // graph_orientation_futhark.fut:355:26-46
                
                bool y_21141 = cond_f_res_21139 && x_22512;
                
                // graph_orientation_futhark.fut:355:26-46
                
                bool cond_21142 = cond_21091 || y_21141;
                bool eq_x_zz_22641 = (int64_t) 0 == defunc_0_reduce_res_21115;
                bool p_and_eq_x_y_22644 = x_22512 && eq_x_zz_22641;
                bool cond_f_res_21145 = cond_21091 || p_and_eq_x_y_22644;
                
                // graph_orientation_futhark.fut:121:25-45
                
                bool y_21146 = cond_f_res_21145 && x_22512;
                
                // graph_orientation_futhark.fut:121:25-45
                
                bool cond_21147 = cond_21091 || y_21146;
                bool protect_assert_disj_22499 = ok_or_empty_21101 || cond_21147;
                bool protect_assert_disj_22507 = cond_21142 || protect_assert_disj_22499;
                
                // graph_orientation_futhark.fut:95:17-28
                
                bool index_certs_21157;
                
                if (!protect_assert_disj_22507) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, ":", (long long) k_21092, "] out of bounds for array of shape [", (long long) augment_path_res_21086, "].", "-> #0  graph_orientation_futhark.fut:95:17-28\n   #1  graph_orientation_futhark.fut:121:25-45\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:96:25-36
                
                bool forwards_ok_21161 = y_21108 && i_lte_j_21109;
                
                // graph_orientation_futhark.fut:96:25-36
                
                bool ok_or_empty_21162 = empty_slice_21094 || forwards_ok_21161;
                bool protect_assert_disj_22501 = cond_21147 || ok_or_empty_21162;
                bool protect_assert_disj_22509 = cond_21142 || protect_assert_disj_22501;
                
                // graph_orientation_futhark.fut:96:25-36
                
                bool index_certs_21163;
                
                if (!protect_assert_disj_22509) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 1, ":] out of bounds for array of shape [", (long long) augment_path_res_21086, "].", "-> #0  graph_orientation_futhark.fut:96:25-36\n   #1  graph_orientation_futhark.fut:121:25-45\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                
                bool protect_assert_disj_22503 = ok_or_empty_21105 || cond_21147;
                bool protect_assert_disj_22511 = cond_21142 || protect_assert_disj_22503;
                
                // graph_orientation_futhark.fut:96:17-36
                
                bool index_certs_21167;
                
                if (!protect_assert_disj_22511) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, ":", (long long) k_21092, "] out of bounds for array of shape [", (long long) k_21092, "].", "-> #0  graph_orientation_futhark.fut:96:17-36\n   #1  graph_orientation_futhark.fut:121:25-45\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:355:26-46
                
                int64_t ext_22400;
                int64_t ext_22399;
                int64_t ext_22398;
                int64_t loopres_f_res_21144;
                
                if (cond_21142) {
                    if (memblock_set(ctx, &ext_mem_22401, &mem_param_22328, "mem_param_22328") != 0)
                        return 1;
                    ext_22400 = ctx_param_ext_22325;
                    ext_22399 = ctx_param_ext_22326;
                    ext_22398 = ctx_param_ext_22327;
                    loopres_f_res_21144 = total_20977;
                } else {
                    // graph_orientation_futhark.fut:93:6-103:18
                    
                    int64_t ext_22396;
                    int64_t ext_22395;
                    int64_t ext_22394;
                    
                    if (cond_21147) {
                        if (memblock_set(ctx, &ext_mem_22397, &mem_param_22328, "mem_param_22328") != 0)
                            return 1;
                        ext_22396 = ctx_param_ext_22325;
                        ext_22395 = ctx_param_ext_22326;
                        ext_22394 = ctx_param_ext_22327;
                    } else {
                        // graph_orientation_futhark.fut:97:11-103:18
                        
                        int64_t ext_22392;
                        int64_t ext_22391;
                        int64_t ext_22390;
                        int64_t ctx_param_ext_22383;
                        int64_t ctx_param_ext_22384;
                        int64_t ctx_param_ext_22385;
                        
                        if (memblock_set(ctx, &mem_param_22386, &mem_param_22328, "mem_param_22328") != 0)
                            return 1;
                        ctx_param_ext_22383 = ctx_param_ext_22325;
                        ctx_param_ext_22384 = ctx_param_ext_22326;
                        ctx_param_ext_22385 = ctx_param_ext_22327;
                        for (int64_t i_21169 = 0; i_21169 < k_21092; i_21169++) {
                            // graph_orientation_futhark.fut:98:21-33
                            
                            int32_t i64_res_21171 = sext_i64_i32(i_21169);
                            
                            // graph_orientation_futhark.fut:99:21-26
                            
                            int64_t j_21172 = sext_i32_i64(i64_res_21171);
                            
                            // graph_orientation_futhark.fut:99:21-26
                            
                            bool x_21173 = sle64((int64_t) 0, j_21172);
                            
                            // graph_orientation_futhark.fut:99:21-26
                            
                            bool y_21174 = slt64(j_21172, k_21092);
                            
                            // graph_orientation_futhark.fut:99:21-26
                            
                            bool bounds_check_21175 = x_21173 && y_21174;
                            
                            // graph_orientation_futhark.fut:99:21-26
                            
                            bool index_certs_21176;
                            
                            if (!bounds_check_21175) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) j_21172, "] out of bounds for array of shape [", (long long) k_21092, "].", "-> #0  graph_orientation_futhark.fut:99:21-26\n   #1  graph_orientation_futhark.fut:121:25-45\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:121:25-45
                            
                            int32_t u_21177 = ((int32_t *) ext_mem_22382.mem)[j_21172];
                            
                            // graph_orientation_futhark.fut:121:25-45
                            
                            int64_t index_primexp_21178 = (int64_t) 1 + j_21172;
                            
                            // graph_orientation_futhark.fut:121:25-45
                            
                            int32_t v_21179 = ((int32_t *) ext_mem_22382.mem)[index_primexp_21178];
                            
                            // graph_orientation_futhark.fut:101:44-48
                            
                            int64_t u_21180 = sext_i32_i64(u_21177);
                            
                            // graph_orientation_futhark.fut:101:44-48
                            
                            bool x_21181 = sle64((int64_t) 0, u_21180);
                            
                            // graph_orientation_futhark.fut:101:44-48
                            
                            bool y_21182 = slt64(u_21180, i32_res_17111);
                            
                            // graph_orientation_futhark.fut:101:44-48
                            
                            bool bounds_check_21183 = x_21181 && y_21182;
                            
                            // graph_orientation_futhark.fut:101:44-48
                            
                            bool index_certs_21184;
                            
                            if (!bounds_check_21183) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21180, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:101:44-48\n   #1  graph_orientation_futhark.fut:121:25-45\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:101:44-51
                            
                            int64_t v_21185 = sext_i32_i64(v_21179);
                            
                            // graph_orientation_futhark.fut:101:44-51
                            
                            bool x_21186 = sle64((int64_t) 0, v_21185);
                            
                            // graph_orientation_futhark.fut:101:44-51
                            
                            bool y_21187 = slt64(v_21185, i32_res_17111);
                            
                            // graph_orientation_futhark.fut:101:44-51
                            
                            bool bounds_check_21188 = x_21186 && y_21187;
                            
                            // graph_orientation_futhark.fut:101:44-51
                            
                            bool index_certs_21189;
                            
                            if (!bounds_check_21188) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21185, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:101:44-51\n   #1  graph_orientation_futhark.fut:121:25-45\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:101:22-60
                            
                            bool index_ok_21193 = bounds_check_21183 && bounds_check_21188;
                            
                            // graph_orientation_futhark.fut:101:22-60
                            
                            bool index_certs_21194;
                            
                            if (!index_ok_21193) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21180, ", ", (long long) v_21185, "] out of bounds for array of shape [", (long long) i32_res_17111, "][", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:101:22-60\n   #1  graph_orientation_futhark.fut:121:25-45\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:102:22-62
                            
                            bool index_certs_21199;
                            
                            if (!index_ok_21193) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21185, ", ", (long long) u_21180, "] out of bounds for array of shape [", (long long) i32_res_17111, "][", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:102:22-62\n   #1  graph_orientation_futhark.fut:121:25-45\n   #2  graph_orientation_futhark.fut:355:26-46\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:121:25-45
                            
                            int64_t zp_lhs_21190 = ((int64_t *) mem_param_22386.mem)[ctx_param_ext_22383 + (u_21180 * ctx_param_ext_22384 + v_21185 * ctx_param_ext_22385)];
                            
                            // graph_orientation_futhark.fut:101:52-59
                            
                            int64_t f1_21191 = add64(min_residual_res_21093, zp_lhs_21190);
                            
                            // graph_orientation_futhark.fut:101:22-28
                            if (memblock_alloc(ctx, &mem_22389, bytes_21930, "mem_22389")) {
                                err = 1;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:101:22-28
                            // graph_orientation_futhark.fut:101:22-28
                            lmad_copy_8b(ctx, 2, (uint64_t *) mem_22389.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (uint64_t *) mem_param_22386.mem, ctx_param_ext_22383, (int64_t []) {ctx_param_ext_22384, ctx_param_ext_22385}, (int64_t []) {i32_res_17111, i32_res_17111});
                            // graph_orientation_futhark.fut:101:22-60
                            ((int64_t *) mem_22389.mem)[u_21180 * i32_res_17111 + v_21185] = f1_21191;
                            // graph_orientation_futhark.fut:121:25-45
                            
                            int64_t zm_lhs_21196 = ((int64_t *) mem_22389.mem)[v_21185 * i32_res_17111 + u_21180];
                            
                            // graph_orientation_futhark.fut:102:54-61
                            
                            int64_t f2_21197 = sub64(zm_lhs_21196, min_residual_res_21093);
                            
                            // graph_orientation_futhark.fut:102:22-62
                            ((int64_t *) mem_22389.mem)[v_21185 * i32_res_17111 + u_21180] = f2_21197;
                            if (memblock_set(ctx, &mem_param_tmp_22707, &mem_22389, "mem_22389") != 0)
                                return 1;
                            
                            int64_t ctx_param_ext_tmp_22708 = (int64_t) 0;
                            int64_t ctx_param_ext_tmp_22709 = i32_res_17111;
                            int64_t ctx_param_ext_tmp_22710 = (int64_t) 1;
                            
                            if (memblock_set(ctx, &mem_param_22386, &mem_param_tmp_22707, "mem_param_tmp_22707") != 0)
                                return 1;
                            ctx_param_ext_22383 = ctx_param_ext_tmp_22708;
                            ctx_param_ext_22384 = ctx_param_ext_tmp_22709;
                            ctx_param_ext_22385 = ctx_param_ext_tmp_22710;
                        }
                        if (memblock_set(ctx, &ext_mem_22393, &mem_param_22386, "mem_param_22386") != 0)
                            return 1;
                        ext_22392 = ctx_param_ext_22383;
                        ext_22391 = ctx_param_ext_22384;
                        ext_22390 = ctx_param_ext_22385;
                        if (memblock_set(ctx, &ext_mem_22397, &ext_mem_22393, "ext_mem_22393") != 0)
                            return 1;
                        ext_22396 = ext_22392;
                        ext_22395 = ext_22391;
                        ext_22394 = ext_22390;
                    }
                    // graph_orientation_futhark.fut:121:53-56
                    
                    int64_t tmp_21201 = add64(total_20977, min_residual_res_21093);
                    
                    if (memblock_set(ctx, &ext_mem_22401, &ext_mem_22397, "ext_mem_22397") != 0)
                        return 1;
                    ext_22400 = ext_22396;
                    ext_22399 = ext_22395;
                    ext_22398 = ext_22394;
                    loopres_f_res_21144 = tmp_21201;
                }
                if (memblock_unref(ctx, &ext_mem_22382, "ext_mem_22382") != 0)
                    return 1;
                if (memblock_set(ctx, &ext_mem_22405, &ext_mem_22401, "ext_mem_22401") != 0)
                    return 1;
                ext_22404 = ext_22400;
                ext_22403 = ext_22399;
                ext_22402 = ext_22398;
                loopres_21058 = cond_21142;
                loopres_21060 = loopres_f_res_21144;
            }
            if (memblock_unref(ctx, &ext_mem_22366, "ext_mem_22366") != 0)
                return 1;
            // graph_orientation_futhark.fut:110:13-21
            
            bool not_res_21202 = !loopres_21058;
            
            if (memblock_set(ctx, &mem_param_tmp_22666, &ext_mem_22405, "ext_mem_22405") != 0)
                return 1;
            
            int64_t ctx_param_ext_tmp_22667 = ext_22404;
            int64_t ctx_param_ext_tmp_22668 = ext_22403;
            int64_t ctx_param_ext_tmp_22669 = ext_22402;
            bool loop_while_tmp_22670 = not_res_21202;
            int64_t total_tmp_22672 = loopres_21060;
            
            if (memblock_set(ctx, &mem_param_22328, &mem_param_tmp_22666, "mem_param_tmp_22666") != 0)
                return 1;
            ctx_param_ext_22325 = ctx_param_ext_tmp_22667;
            ctx_param_ext_22326 = ctx_param_ext_tmp_22668;
            ctx_param_ext_22327 = ctx_param_ext_tmp_22669;
            loop_while_20975 = loop_while_tmp_22670;
            total_20977 = total_tmp_22672;
        }
        if (memblock_set(ctx, &ext_mem_22409, &mem_param_22328, "mem_param_22328") != 0)
            return 1;
        ext_22408 = ctx_param_ext_22325;
        ext_22407 = ctx_param_ext_22326;
        ext_22406 = ctx_param_ext_22327;
        edmonds_karp_res_20972 = loop_while_20975;
        edmonds_karp_res_20974 = total_20977;
        if (memblock_unref(ctx, &mem_22330, "mem_22330") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22332, "mem_22332") != 0)
            return 1;
        if (memblock_set(ctx, &ext_mem_22413, &ext_mem_22409, "ext_mem_22409") != 0)
            return 1;
        ext_22412 = ext_22408;
        ext_22411 = ext_22407;
        ext_22410 = ext_22406;
        graph_orientation_solve_res_17166 = edmonds_karp_res_20974;
    } else {
        int64_t ext_22323;
        int64_t ext_22322;
        int64_t ext_22321;
        int64_t graph_orientation_solve_res_f_res_17389;
        
        if (cond_17163) {
            // graph_orientation_futhark.fut:127:17-37
            if (memblock_alloc(ctx, &mem_22199, bytes_21960, "mem_22199")) {
                err = 1;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:128:17-35
            if (memblock_alloc(ctx, &mem_22201, bytes_21960, "mem_22201")) {
                err = 1;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:208:5-226:46
            
            int64_t ext_22319;
            int64_t ext_22318;
            int64_t ext_22317;
            bool dinic_res_21264;
            int64_t dinic_res_21266;
            bool loop_while_21267;
            int64_t tot_21269;
            int64_t ctx_param_ext_22194;
            int64_t ctx_param_ext_22195;
            int64_t ctx_param_ext_22196;
            
            if (memblock_set(ctx, &mem_param_22197, &mem_21931, "mem_21931") != 0)
                return 1;
            ctx_param_ext_22194 = (int64_t) 0;
            ctx_param_ext_22195 = i32_res_17111;
            ctx_param_ext_22196 = (int64_t) 1;
            loop_while_21267 = 1;
            tot_21269 = (int64_t) 0;
            while (loop_while_21267) {
                // graph_orientation_futhark.fut:127:17-37
                for (int64_t nest_i_22719 = 0; nest_i_22719 < i32_res_17111; nest_i_22719++) {
                    ((int32_t *) mem_22199.mem)[nest_i_22719] = -1;
                }
                // graph_orientation_futhark.fut:127:16-55
                ((int32_t *) mem_22199.mem)[(int64_t) 0] = 0;
                // graph_orientation_futhark.fut:128:17-35
                for (int64_t nest_i_22720 = 0; nest_i_22720 < i32_res_17111; nest_i_22720++) {
                    ((int32_t *) mem_22201.mem)[nest_i_22720] = 0;
                }
                // graph_orientation_futhark.fut:128:16-49
                ((int32_t *) mem_22201.mem)[(int64_t) 0] = 0;
                // graph_orientation_futhark.fut:131:5-145:42
                
                int64_t ext_22233;
                int64_t ext_22232;
                int64_t ext_22231;
                int64_t ext_22230;
                bool levelf_21274;
                int32_t levelf_21275;
                int32_t levelf_21276;
                bool loop_while_21279;
                int32_t qstart_21280;
                int32_t qend_21281;
                int64_t ctx_param_ext_22202;
                int64_t ctx_param_ext_22203;
                int64_t ctx_param_ext_22205;
                int64_t ctx_param_ext_22206;
                
                if (memblock_set(ctx, &mem_param_22204, &mem_22199, "mem_22199") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22207, &mem_22201, "mem_22201") != 0)
                    return 1;
                ctx_param_ext_22202 = (int64_t) 0;
                ctx_param_ext_22203 = (int64_t) 1;
                ctx_param_ext_22205 = (int64_t) 0;
                ctx_param_ext_22206 = (int64_t) 1;
                loop_while_21279 = 1;
                qstart_21280 = 0;
                qend_21281 = 1;
                while (loop_while_21279) {
                    // graph_orientation_futhark.fut:134:15-28
                    
                    int64_t qstart_21284 = sext_i32_i64(qstart_21280);
                    
                    // graph_orientation_futhark.fut:134:15-28
                    
                    bool x_21285 = sle64((int64_t) 0, qstart_21284);
                    
                    // graph_orientation_futhark.fut:134:15-28
                    
                    bool y_21286 = slt64(qstart_21284, i32_res_17111);
                    
                    // graph_orientation_futhark.fut:134:15-28
                    
                    bool bounds_check_21287 = x_21285 && y_21286;
                    
                    // graph_orientation_futhark.fut:134:15-28
                    
                    bool index_certs_21288;
                    
                    if (!bounds_check_21287) {
                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) qstart_21284, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:134:15-28\n   #1  graph_orientation_futhark.fut:211:19-47\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                        err = FUTHARK_PROGRAM_ERROR;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:134:15-28
                    
                    int32_t u_21289 = ((int32_t *) mem_param_22207.mem)[ctx_param_ext_22205 + qstart_21284 * ctx_param_ext_22206];
                    
                    // graph_orientation_futhark.fut:135:28-31
                    
                    int32_t qstartzq_21290 = add32(1, qstart_21280);
                    
                    // graph_orientation_futhark.fut:139:35-41
                    
                    int64_t u_21307 = sext_i32_i64(u_21289);
                    
                    // graph_orientation_futhark.fut:139:35-41
                    
                    bool x_21308 = sle64((int64_t) 0, u_21307);
                    
                    // graph_orientation_futhark.fut:139:35-41
                    
                    bool y_21309 = slt64(u_21307, i32_res_17111);
                    
                    // graph_orientation_futhark.fut:139:35-41
                    
                    bool bounds_check_21310 = x_21308 && y_21309;
                    
                    // graph_orientation_futhark.fut:137:9-144:30
                    
                    int64_t ext_22227;
                    int64_t ext_22226;
                    int64_t ext_22225;
                    int64_t ext_22224;
                    int32_t loopres_21293;
                    int32_t qe_21297;
                    int64_t ctx_param_ext_22208;
                    int64_t ctx_param_ext_22209;
                    int64_t ctx_param_ext_22211;
                    int64_t ctx_param_ext_22212;
                    
                    if (memblock_set(ctx, &mem_param_22210, &mem_param_22204, "mem_param_22204") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_22213, &mem_param_22207, "mem_param_22207") != 0)
                        return 1;
                    ctx_param_ext_22208 = ctx_param_ext_22202;
                    ctx_param_ext_22209 = ctx_param_ext_22203;
                    ctx_param_ext_22211 = ctx_param_ext_22205;
                    ctx_param_ext_22212 = ctx_param_ext_22206;
                    qe_21297 = qend_21281;
                    for (int64_t i_21294 = 0; i_21294 < i32_res_17111; i_21294++) {
                        // graph_orientation_futhark.fut:211:19-47
                        
                        int32_t binop_x_21298 = sext_i64_i32(i_21294);
                        
                        // graph_orientation_futhark.fut:139:16-22
                        
                        int64_t v_21299 = sext_i32_i64(binop_x_21298);
                        
                        // graph_orientation_futhark.fut:139:16-22
                        
                        bool x_21300 = sle64((int64_t) 0, v_21299);
                        
                        // graph_orientation_futhark.fut:139:16-22
                        
                        bool y_21301 = slt64(v_21299, i32_res_17111);
                        
                        // graph_orientation_futhark.fut:139:16-22
                        
                        bool bounds_check_21302 = x_21300 && y_21301;
                        
                        // graph_orientation_futhark.fut:139:16-22
                        
                        bool index_certs_21303;
                        
                        if (!bounds_check_21302) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21299, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:139:16-22\n   #1  graph_orientation_futhark.fut:211:19-47\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:139:16-22
                        
                        int32_t zeze_lhs_21304 = ((int32_t *) mem_param_22210.mem)[ctx_param_ext_22208 + v_21299 * ctx_param_ext_22209];
                        
                        // graph_orientation_futhark.fut:139:13-144:30
                        
                        bool cond_21305 = zeze_lhs_21304 == -1;
                        bool loop_not_taken_22571 = !cond_21305;
                        bool protect_assert_disj_22572 = bounds_check_21310 || loop_not_taken_22571;
                        
                        // graph_orientation_futhark.fut:139:35-41
                        
                        bool index_certs_21311;
                        
                        if (!protect_assert_disj_22572) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21307, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:139:35-41\n   #1  graph_orientation_futhark.fut:211:19-47\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:211:19-47
                        
                        int64_t zm_lhs_21312;
                        
                        if (cond_21305) {
                            int64_t x_22573 = ((int64_t *) ext_mem_21959.mem)[u_21307 * i32_res_17111 + v_21299];
                            
                            zm_lhs_21312 = x_22573;
                        } else {
                            zm_lhs_21312 = (int64_t) 0;
                        }
                        // graph_orientation_futhark.fut:211:19-47
                        
                        int64_t zm_rhs_21313;
                        
                        if (cond_21305) {
                            int64_t x_22575 = ((int64_t *) mem_param_22197.mem)[ctx_param_ext_22194 + (u_21307 * ctx_param_ext_22195 + v_21299 * ctx_param_ext_22196)];
                            
                            zm_rhs_21313 = x_22575;
                        } else {
                            zm_rhs_21313 = (int64_t) 0;
                        }
                        // graph_orientation_futhark.fut:139:45-57
                        
                        int64_t zg_lhs_21314 = sub64(zm_lhs_21312, zm_rhs_21313);
                        
                        // graph_orientation_futhark.fut:139:58-64
                        
                        bool cond_t_res_21315 = slt64((int64_t) 0, zg_lhs_21314);
                        bool x_22648 = cond_21305 && cond_t_res_21315;
                        int64_t ext_22222;
                        
                        if (x_22648) {
                            ext_22222 = (int64_t) 0;
                        } else {
                            ext_22222 = ctx_param_ext_22208;
                        }
                        
                        int64_t ext_22221;
                        
                        if (x_22648) {
                            ext_22221 = (int64_t) 1;
                        } else {
                            ext_22221 = ctx_param_ext_22209;
                        }
                        
                        int64_t ext_22219;
                        
                        if (x_22648) {
                            ext_22219 = (int64_t) 0;
                        } else {
                            ext_22219 = ctx_param_ext_22211;
                        }
                        
                        int64_t ext_22218;
                        
                        if (x_22648) {
                            ext_22218 = (int64_t) 1;
                        } else {
                            ext_22218 = ctx_param_ext_22212;
                        }
                        
                        int32_t loopres_21318;
                        
                        if (x_22648) {
                            // graph_orientation_futhark.fut:140:47-53
                            
                            bool index_certs_21323;
                            
                            if (!bounds_check_21310) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21307, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:140:47-53\n   #1  graph_orientation_futhark.fut:211:19-47\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:141:24-44
                            
                            int64_t qe_21329 = sext_i32_i64(qe_21297);
                            
                            // graph_orientation_futhark.fut:141:24-44
                            
                            bool x_21330 = sle64((int64_t) 0, qe_21329);
                            
                            // graph_orientation_futhark.fut:141:24-44
                            
                            bool y_21331 = slt64(qe_21329, i32_res_17111);
                            
                            // graph_orientation_futhark.fut:141:24-44
                            
                            bool bounds_check_21332 = x_21330 && y_21331;
                            
                            // graph_orientation_futhark.fut:141:24-44
                            
                            bool index_certs_21333;
                            
                            if (!bounds_check_21332) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) qe_21329, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:141:24-44\n   #1  graph_orientation_futhark.fut:211:19-47\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:140:47-53
                            
                            int32_t zp_lhs_21324 = ((int32_t *) mem_param_22210.mem)[ctx_param_ext_22208 + u_21307 * ctx_param_ext_22209];
                            
                            // graph_orientation_futhark.fut:140:54-60
                            
                            int32_t lvlzq_21325 = add32(1, zp_lhs_21324);
                            
                            // graph_orientation_futhark.fut:140:26-34
                            if (memblock_alloc(ctx, &mem_22215, bytes_21960, "mem_22215")) {
                                err = 1;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:140:26-34
                            // graph_orientation_futhark.fut:140:26-34
                            lmad_copy_4b(ctx, 1, (uint32_t *) mem_22215.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22210.mem, ctx_param_ext_22208, (int64_t []) {ctx_param_ext_22209}, (int64_t []) {i32_res_17111});
                            // graph_orientation_futhark.fut:140:26-61
                            ((int32_t *) mem_22215.mem)[v_21299] = lvlzq_21325;
                            // graph_orientation_futhark.fut:141:24-30
                            if (memblock_alloc(ctx, &mem_22217, bytes_21960, "mem_22217")) {
                                err = 1;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:141:24-30
                            // graph_orientation_futhark.fut:141:24-30
                            lmad_copy_4b(ctx, 1, (uint32_t *) mem_22217.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22213.mem, ctx_param_ext_22211, (int64_t []) {ctx_param_ext_22212}, (int64_t []) {i32_res_17111});
                            // graph_orientation_futhark.fut:141:24-44
                            ((int32_t *) mem_22217.mem)[qe_21329] = binop_x_21298;
                            // graph_orientation_futhark.fut:142:28-31
                            
                            int32_t qezq_21335 = add32(1, qe_21297);
                            
                            if (memblock_set(ctx, &ext_mem_22223, &mem_22215, "mem_22215") != 0)
                                return 1;
                            if (memblock_set(ctx, &ext_mem_22220, &mem_22217, "mem_22217") != 0)
                                return 1;
                            loopres_21318 = qezq_21335;
                        } else {
                            if (memblock_set(ctx, &ext_mem_22223, &mem_param_22210, "mem_param_22210") != 0)
                                return 1;
                            if (memblock_set(ctx, &ext_mem_22220, &mem_param_22213, "mem_param_22213") != 0)
                                return 1;
                            loopres_21318 = qe_21297;
                        }
                        if (memblock_set(ctx, &mem_param_tmp_22732, &ext_mem_22223, "ext_mem_22223") != 0)
                            return 1;
                        if (memblock_set(ctx, &mem_param_tmp_22733, &ext_mem_22220, "ext_mem_22220") != 0)
                            return 1;
                        
                        int64_t ctx_param_ext_tmp_22734 = ext_22222;
                        int64_t ctx_param_ext_tmp_22735 = ext_22221;
                        int64_t ctx_param_ext_tmp_22736 = ext_22219;
                        int64_t ctx_param_ext_tmp_22737 = ext_22218;
                        int32_t qe_tmp_22740 = loopres_21318;
                        
                        if (memblock_set(ctx, &mem_param_22210, &mem_param_tmp_22732, "mem_param_tmp_22732") != 0)
                            return 1;
                        if (memblock_set(ctx, &mem_param_22213, &mem_param_tmp_22733, "mem_param_tmp_22733") != 0)
                            return 1;
                        ctx_param_ext_22208 = ctx_param_ext_tmp_22734;
                        ctx_param_ext_22209 = ctx_param_ext_tmp_22735;
                        ctx_param_ext_22211 = ctx_param_ext_tmp_22736;
                        ctx_param_ext_22212 = ctx_param_ext_tmp_22737;
                        qe_21297 = qe_tmp_22740;
                    }
                    if (memblock_set(ctx, &ext_mem_22229, &mem_param_22210, "mem_param_22210") != 0)
                        return 1;
                    if (memblock_set(ctx, &ext_mem_22228, &mem_param_22213, "mem_param_22213") != 0)
                        return 1;
                    ext_22227 = ctx_param_ext_22208;
                    ext_22226 = ctx_param_ext_22209;
                    ext_22225 = ctx_param_ext_22211;
                    ext_22224 = ctx_param_ext_22212;
                    loopres_21293 = qe_21297;
                    // graph_orientation_futhark.fut:132:20-26
                    
                    bool loop_cond_21336 = slt32(qstartzq_21290, loopres_21293);
                    
                    if (memblock_set(ctx, &mem_param_tmp_22721, &ext_mem_22229, "ext_mem_22229") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_tmp_22722, &ext_mem_22228, "ext_mem_22228") != 0)
                        return 1;
                    
                    int64_t ctx_param_ext_tmp_22723 = ext_22227;
                    int64_t ctx_param_ext_tmp_22724 = ext_22226;
                    int64_t ctx_param_ext_tmp_22725 = ext_22225;
                    int64_t ctx_param_ext_tmp_22726 = ext_22224;
                    bool loop_while_tmp_22727 = loop_cond_21336;
                    int32_t qstart_tmp_22728 = qstartzq_21290;
                    int32_t qend_tmp_22729 = loopres_21293;
                    
                    if (memblock_set(ctx, &mem_param_22204, &mem_param_tmp_22721, "mem_param_tmp_22721") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_22207, &mem_param_tmp_22722, "mem_param_tmp_22722") != 0)
                        return 1;
                    ctx_param_ext_22202 = ctx_param_ext_tmp_22723;
                    ctx_param_ext_22203 = ctx_param_ext_tmp_22724;
                    ctx_param_ext_22205 = ctx_param_ext_tmp_22725;
                    ctx_param_ext_22206 = ctx_param_ext_tmp_22726;
                    loop_while_21279 = loop_while_tmp_22727;
                    qstart_21280 = qstart_tmp_22728;
                    qend_21281 = qend_tmp_22729;
                }
                if (memblock_set(ctx, &ext_mem_22235, &mem_param_22204, "mem_param_22204") != 0)
                    return 1;
                if (memblock_set(ctx, &ext_mem_22234, &mem_param_22207, "mem_param_22207") != 0)
                    return 1;
                ext_22233 = ctx_param_ext_22202;
                ext_22232 = ctx_param_ext_22203;
                ext_22231 = ctx_param_ext_22205;
                ext_22230 = ctx_param_ext_22206;
                levelf_21274 = loop_while_21279;
                levelf_21275 = qstart_21280;
                levelf_21276 = qend_21281;
                // graph_orientation_futhark.fut:212:13-21
                
                int32_t zl_lhs_21337 = ((int32_t *) ext_mem_22235.mem)[ext_22233 + t_21255 * ext_22232];
                
                // graph_orientation_futhark.fut:212:10-226:46
                
                bool cond_21338 = slt32(zl_lhs_21337, 0);
                
                // graph_orientation_futhark.fut:212:10-226:46
                
                int64_t ext_22315;
                int64_t ext_22314;
                int64_t ext_22313;
                bool loopres_21339;
                int64_t loopres_21341;
                
                if (cond_21338) {
                    if (memblock_set(ctx, &ext_mem_22316, &mem_param_22197, "mem_param_22197") != 0)
                        return 1;
                    ext_22315 = ctx_param_ext_22194;
                    ext_22314 = ctx_param_ext_22195;
                    ext_22313 = ctx_param_ext_22196;
                    loopres_21339 = 1;
                    loopres_21341 = tot_21269;
                } else {
                    // graph_orientation_futhark.fut:151:18-38
                    if (memblock_alloc(ctx, &mem_22241, bytes_21960, "mem_22241")) {
                        err = 1;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:152:17-35
                    if (memblock_alloc(ctx, &mem_22243, bytes_21960, "mem_22243")) {
                        err = 1;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:216:14-224:50
                    
                    int64_t ext_22311;
                    int64_t ext_22310;
                    int64_t ext_22309;
                    bool loopres_f_res_21347;
                    int64_t loopres_f_res_21349;
                    bool loop_while_21350;
                    int64_t ds_21352;
                    int64_t ctx_param_ext_22236;
                    int64_t ctx_param_ext_22237;
                    int64_t ctx_param_ext_22238;
                    
                    if (memblock_set(ctx, &mem_param_22239, &mem_param_22197, "mem_param_22197") != 0)
                        return 1;
                    ctx_param_ext_22236 = ctx_param_ext_22194;
                    ctx_param_ext_22237 = ctx_param_ext_22195;
                    ctx_param_ext_22238 = ctx_param_ext_22196;
                    loop_while_21350 = 1;
                    ds_21352 = (int64_t) 0;
                    while (loop_while_21350) {
                        // graph_orientation_futhark.fut:151:18-38
                        for (int64_t nest_i_22748 = 0; nest_i_22748 < i32_res_17111; nest_i_22748++) {
                            ((int32_t *) mem_22241.mem)[nest_i_22748] = -1;
                        }
                        // graph_orientation_futhark.fut:151:17-53
                        ((int32_t *) mem_22241.mem)[(int64_t) 0] = 0;
                        // graph_orientation_futhark.fut:152:17-35
                        for (int64_t nest_i_22749 = 0; nest_i_22749 < i32_res_17111; nest_i_22749++) {
                            ((int32_t *) mem_22243.mem)[nest_i_22749] = 0;
                        }
                        // graph_orientation_futhark.fut:152:16-49
                        ((int32_t *) mem_22243.mem)[(int64_t) 0] = 0;
                        // graph_orientation_futhark.fut:155:5-176:48
                        
                        int64_t ext_22269;
                        int64_t ext_22268;
                        int64_t ext_22267;
                        int64_t ext_22266;
                        bool dinic_blocking_flow_res_21357;
                        bool dinic_blocking_flow_res_21358;
                        int32_t dinic_blocking_flow_res_21361;
                        bool loop_while_21362;
                        bool found_21363;
                        int32_t top_21366;
                        int64_t ctx_param_ext_22244;
                        int64_t ctx_param_ext_22245;
                        int64_t ctx_param_ext_22247;
                        int64_t ctx_param_ext_22248;
                        
                        if (memblock_set(ctx, &mem_param_22246, &mem_22241, "mem_22241") != 0)
                            return 1;
                        if (memblock_set(ctx, &mem_param_22249, &mem_22243, "mem_22243") != 0)
                            return 1;
                        ctx_param_ext_22244 = (int64_t) 0;
                        ctx_param_ext_22245 = (int64_t) 1;
                        ctx_param_ext_22247 = (int64_t) 0;
                        ctx_param_ext_22248 = (int64_t) 1;
                        loop_while_21362 = 1;
                        found_21363 = 0;
                        top_21366 = 1;
                        while (loop_while_21362) {
                            // graph_orientation_futhark.fut:158:25-28
                            
                            int32_t u_21367 = sub32(top_21366, 1);
                            
                            // graph_orientation_futhark.fut:158:15-29
                            
                            int64_t u_21368 = sext_i32_i64(u_21367);
                            
                            // graph_orientation_futhark.fut:158:15-29
                            
                            bool x_21369 = sle64((int64_t) 0, u_21368);
                            
                            // graph_orientation_futhark.fut:158:15-29
                            
                            bool y_21370 = slt64(u_21368, i32_res_17111);
                            
                            // graph_orientation_futhark.fut:158:15-29
                            
                            bool bounds_check_21371 = x_21369 && y_21370;
                            
                            // graph_orientation_futhark.fut:158:15-29
                            
                            bool index_certs_21372;
                            
                            if (!bounds_check_21371) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21368, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:158:15-29\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:158:15-29
                            
                            int32_t u_21373 = ((int32_t *) mem_param_22249.mem)[ctx_param_ext_22247 + u_21368 * ctx_param_ext_22248];
                            
                            // graph_orientation_futhark.fut:159:10-176:48
                            
                            bool cond_21374 = u_21373 == sink_res_17105;
                            
                            // graph_orientation_futhark.fut:166:31-39
                            
                            int64_t u_21379 = sext_i32_i64(u_21373);
                            
                            // graph_orientation_futhark.fut:166:31-39
                            
                            bool x_21380 = sle64((int64_t) 0, u_21379);
                            
                            // graph_orientation_futhark.fut:166:31-39
                            
                            bool y_21381 = slt64(u_21379, i32_res_17111);
                            
                            // graph_orientation_futhark.fut:166:31-39
                            
                            bool bounds_check_21382 = x_21380 && y_21381;
                            
                            // graph_orientation_futhark.fut:219:36-75
                            
                            bool protect_assert_disj_21384 = loop_not_taken_17898 || bounds_check_21382;
                            bool protect_assert_disj_22559 = cond_21374 || protect_assert_disj_21384;
                            
                            // graph_orientation_futhark.fut:166:31-39
                            
                            bool index_certs_21385;
                            
                            if (!protect_assert_disj_22559) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21379, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:166:31-39\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            
                            bool x_22556 = !cond_21374;
                            bool protect_cond_conj_22560 = cond_17897 && x_22556;
                            
                            // graph_orientation_futhark.fut:166:31-39
                            
                            int32_t zp_lhs_21386;
                            
                            if (protect_cond_conj_22560) {
                                // graph_orientation_futhark.fut:219:36-75
                                
                                int32_t x_21387 = ((int32_t *) ext_mem_22235.mem)[ext_22233 + u_21379 * ext_22232];
                                
                                zp_lhs_21386 = x_21387;
                            } else {
                                zp_lhs_21386 = 0;
                            }
                            // graph_orientation_futhark.fut:166:40-46
                            
                            int32_t zeze_rhs_21388 = add32(1, zp_lhs_21386);
                            int32_t loopres_f_res_21390;
                            bool loopres_f_res_21391;
                            
                            if (x_22556) {
                                bool x_22561;
                                int32_t x_22562;
                                bool x_22563;
                                bool loop_while_21392;
                                int32_t v_21393;
                                bool ok_21394;
                                
                                loop_while_21392 = cond_17897;
                                v_21393 = 0;
                                ok_21394 = 0;
                                while (loop_while_21392) {
                                    // graph_orientation_futhark.fut:166:19-27
                                    
                                    int64_t v_21395 = sext_i32_i64(v_21393);
                                    
                                    // graph_orientation_futhark.fut:166:19-27
                                    
                                    bool x_21396 = sle64((int64_t) 0, v_21395);
                                    
                                    // graph_orientation_futhark.fut:166:19-27
                                    
                                    bool y_21397 = slt64(v_21395, i32_res_17111);
                                    
                                    // graph_orientation_futhark.fut:166:19-27
                                    
                                    bool bounds_check_21398 = x_21396 && y_21397;
                                    
                                    // graph_orientation_futhark.fut:166:19-27
                                    
                                    bool index_certs_21399;
                                    
                                    if (!bounds_check_21398) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21395, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:166:19-27\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:166:19-27
                                    
                                    int32_t zeze_lhs_21400 = ((int32_t *) ext_mem_22235.mem)[ext_22233 + v_21395 * ext_22232];
                                    
                                    // graph_orientation_futhark.fut:166:16-170:35
                                    
                                    bool cond_21401 = zeze_lhs_21400 == zeze_rhs_21388;
                                    
                                    // graph_orientation_futhark.fut:166:16-170:35
                                    
                                    bool cond_21402;
                                    
                                    if (cond_21401) {
                                        // graph_orientation_futhark.fut:167:19-25
                                        
                                        bool index_certs_21403;
                                        
                                        if (!bounds_check_21382) {
                                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21379, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:167:19-25\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                            err = FUTHARK_PROGRAM_ERROR;
                                            goto cleanup;
                                        }
                                        // graph_orientation_futhark.fut:219:36-75
                                        
                                        int64_t zm_lhs_21404 = ((int64_t *) ext_mem_21959.mem)[u_21379 * i32_res_17111 + v_21395];
                                        
                                        // graph_orientation_futhark.fut:219:36-75
                                        
                                        int64_t zm_rhs_21405 = ((int64_t *) mem_param_22239.mem)[ctx_param_ext_22236 + (u_21379 * ctx_param_ext_22237 + v_21395 * ctx_param_ext_22238)];
                                        
                                        // graph_orientation_futhark.fut:167:29-41
                                        
                                        int64_t zg_lhs_21406 = sub64(zm_lhs_21404, zm_rhs_21405);
                                        
                                        // graph_orientation_futhark.fut:167:42-48
                                        
                                        bool cond_t_res_21407 = slt64((int64_t) 0, zg_lhs_21406);
                                        
                                        cond_21402 = cond_t_res_21407;
                                    } else {
                                        cond_21402 = 0;
                                    }
                                    // graph_orientation_futhark.fut:166:16-170:35
                                    
                                    bool cond_21408;
                                    
                                    if (cond_21402) {
                                        // graph_orientation_futhark.fut:168:19-28
                                        
                                        int32_t zeze_lhs_21409 = ((int32_t *) mem_param_22246.mem)[ctx_param_ext_22244 + v_21395 * ctx_param_ext_22245];
                                        
                                        // graph_orientation_futhark.fut:168:29-37
                                        
                                        bool cond_t_res_21410 = zeze_lhs_21409 == -1;
                                        
                                        cond_21408 = cond_t_res_21410;
                                    } else {
                                        cond_21408 = 0;
                                    }
                                    // graph_orientation_futhark.fut:219:36-75
                                    
                                    int32_t loopres_21411;
                                    
                                    if (cond_21408) {
                                        loopres_21411 = v_21393;
                                    } else {
                                        // graph_orientation_futhark.fut:170:24-27
                                        
                                        int32_t tmp_21412 = add32(1, v_21393);
                                        
                                        loopres_21411 = tmp_21412;
                                    }
                                    // graph_orientation_futhark.fut:164:22-37
                                    
                                    bool cond_21413 = slt32(loopres_21411, node_count_res_17110);
                                    
                                    // graph_orientation_futhark.fut:164:31-37
                                    
                                    bool not_res_21414 = !cond_21408;
                                    
                                    // graph_orientation_futhark.fut:219:36-75
                                    
                                    bool x_21415 = cond_21413 && not_res_21414;
                                    bool loop_while_tmp_22761 = x_21415;
                                    int32_t v_tmp_22762 = loopres_21411;
                                    bool ok_tmp_22763 = cond_21408;
                                    
                                    loop_while_21392 = loop_while_tmp_22761;
                                    v_21393 = v_tmp_22762;
                                    ok_21394 = ok_tmp_22763;
                                }
                                x_22561 = loop_while_21392;
                                x_22562 = v_21393;
                                x_22563 = ok_21394;
                                loopres_f_res_21390 = x_22562;
                                loopres_f_res_21391 = x_22563;
                            } else {
                                loopres_f_res_21390 = 0;
                                loopres_f_res_21391 = 0;
                            }
                            
                            int64_t ext_22258;
                            
                            if (loopres_f_res_21391) {
                                ext_22258 = (int64_t) 0;
                            } else {
                                ext_22258 = ctx_param_ext_22244;
                            }
                            
                            int64_t ext_22257;
                            
                            if (loopres_f_res_21391) {
                                ext_22257 = (int64_t) 1;
                            } else {
                                ext_22257 = ctx_param_ext_22245;
                            }
                            
                            int64_t ext_22255;
                            
                            if (loopres_f_res_21391) {
                                ext_22255 = (int64_t) 0;
                            } else {
                                ext_22255 = ctx_param_ext_22247;
                            }
                            
                            int64_t ext_22254;
                            
                            if (loopres_f_res_21391) {
                                ext_22254 = (int64_t) 1;
                            } else {
                                ext_22254 = ctx_param_ext_22248;
                            }
                            
                            int64_t ext_22264;
                            
                            if (cond_21374) {
                                ext_22264 = ctx_param_ext_22244;
                            } else {
                                ext_22264 = ext_22258;
                            }
                            
                            int64_t ext_22263;
                            
                            if (cond_21374) {
                                ext_22263 = ctx_param_ext_22245;
                            } else {
                                ext_22263 = ext_22257;
                            }
                            
                            int64_t ext_22261;
                            
                            if (cond_21374) {
                                ext_22261 = ctx_param_ext_22247;
                            } else {
                                ext_22261 = ext_22255;
                            }
                            
                            int64_t ext_22260;
                            
                            if (cond_21374) {
                                ext_22260 = ctx_param_ext_22248;
                            } else {
                                ext_22260 = ext_22254;
                            }
                            
                            int32_t loopres_21377;
                            
                            if (cond_21374) {
                                if (memblock_set(ctx, &ext_mem_22265, &mem_param_22246, "mem_param_22246") != 0)
                                    return 1;
                                if (memblock_set(ctx, &ext_mem_22262, &mem_param_22249, "mem_param_22249") != 0)
                                    return 1;
                                loopres_21377 = top_21366;
                            } else {
                                int32_t loopres_f_res_21418;
                                
                                if (loopres_f_res_21391) {
                                    // graph_orientation_futhark.fut:172:31-61
                                    
                                    int64_t v_found_21420 = sext_i32_i64(loopres_f_res_21390);
                                    
                                    // graph_orientation_futhark.fut:172:31-61
                                    
                                    bool x_21421 = sle64((int64_t) 0, v_found_21420);
                                    
                                    // graph_orientation_futhark.fut:172:31-61
                                    
                                    bool y_21422 = slt64(v_found_21420, i32_res_17111);
                                    
                                    // graph_orientation_futhark.fut:172:31-61
                                    
                                    bool bounds_check_21423 = x_21421 && y_21422;
                                    
                                    // graph_orientation_futhark.fut:172:31-61
                                    
                                    bool index_certs_21424;
                                    
                                    if (!bounds_check_21423) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_found_21420, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:172:31-61\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:173:30-61
                                    
                                    int64_t top_21427 = sext_i32_i64(top_21366);
                                    
                                    // graph_orientation_futhark.fut:173:30-61
                                    
                                    bool x_21428 = sle64((int64_t) 0, top_21427);
                                    
                                    // graph_orientation_futhark.fut:173:30-61
                                    
                                    bool y_21429 = slt64(top_21427, i32_res_17111);
                                    
                                    // graph_orientation_futhark.fut:173:30-61
                                    
                                    bool bounds_check_21430 = x_21428 && y_21429;
                                    
                                    // graph_orientation_futhark.fut:173:30-61
                                    
                                    bool index_certs_21431;
                                    
                                    if (!bounds_check_21430) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) top_21427, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:173:30-61\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:172:31-42
                                    if (memblock_alloc(ctx, &mem_22251, bytes_21960, "mem_22251")) {
                                        err = 1;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:172:31-42
                                    // graph_orientation_futhark.fut:172:31-42
                                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22251.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22246.mem, ctx_param_ext_22244, (int64_t []) {ctx_param_ext_22245}, (int64_t []) {i32_res_17111});
                                    // graph_orientation_futhark.fut:172:31-61
                                    ((int32_t *) mem_22251.mem)[v_found_21420] = u_21373;
                                    // graph_orientation_futhark.fut:173:30-40
                                    if (memblock_alloc(ctx, &mem_22253, bytes_21960, "mem_22253")) {
                                        err = 1;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:173:30-40
                                    // graph_orientation_futhark.fut:173:30-40
                                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22253.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22249.mem, ctx_param_ext_22247, (int64_t []) {ctx_param_ext_22248}, (int64_t []) {i32_res_17111});
                                    // graph_orientation_futhark.fut:173:30-61
                                    ((int32_t *) mem_22253.mem)[top_21427] = loopres_f_res_21390;
                                    // graph_orientation_futhark.fut:174:49-52
                                    
                                    int32_t tmp_21433 = add32(1, top_21366);
                                    
                                    if (memblock_set(ctx, &ext_mem_22259, &mem_22251, "mem_22251") != 0)
                                        return 1;
                                    if (memblock_set(ctx, &ext_mem_22256, &mem_22253, "mem_22253") != 0)
                                        return 1;
                                    loopres_f_res_21418 = tmp_21433;
                                } else {
                                    if (memblock_set(ctx, &ext_mem_22259, &mem_param_22246, "mem_param_22246") != 0)
                                        return 1;
                                    if (memblock_set(ctx, &ext_mem_22256, &mem_param_22249, "mem_param_22249") != 0)
                                        return 1;
                                    loopres_f_res_21418 = u_21367;
                                }
                                if (memblock_set(ctx, &ext_mem_22265, &ext_mem_22259, "ext_mem_22259") != 0)
                                    return 1;
                                if (memblock_set(ctx, &ext_mem_22262, &ext_mem_22256, "ext_mem_22256") != 0)
                                    return 1;
                                loopres_21377 = loopres_f_res_21418;
                            }
                            // graph_orientation_futhark.fut:156:30-33
                            
                            bool loop_cond_t_res_21435 = slt32(0, loopres_21377);
                            
                            // graph_orientation_futhark.fut:219:36-75
                            
                            bool x_21436 = loop_cond_t_res_21435 && x_22556;
                            
                            if (memblock_set(ctx, &mem_param_tmp_22750, &ext_mem_22265, "ext_mem_22265") != 0)
                                return 1;
                            if (memblock_set(ctx, &mem_param_tmp_22751, &ext_mem_22262, "ext_mem_22262") != 0)
                                return 1;
                            
                            int64_t ctx_param_ext_tmp_22752 = ext_22264;
                            int64_t ctx_param_ext_tmp_22753 = ext_22263;
                            int64_t ctx_param_ext_tmp_22754 = ext_22261;
                            int64_t ctx_param_ext_tmp_22755 = ext_22260;
                            bool loop_while_tmp_22756 = x_21436;
                            bool found_tmp_22757 = cond_21374;
                            int32_t top_tmp_22760 = loopres_21377;
                            
                            if (memblock_set(ctx, &mem_param_22246, &mem_param_tmp_22750, "mem_param_tmp_22750") != 0)
                                return 1;
                            if (memblock_set(ctx, &mem_param_22249, &mem_param_tmp_22751, "mem_param_tmp_22751") != 0)
                                return 1;
                            ctx_param_ext_22244 = ctx_param_ext_tmp_22752;
                            ctx_param_ext_22245 = ctx_param_ext_tmp_22753;
                            ctx_param_ext_22247 = ctx_param_ext_tmp_22754;
                            ctx_param_ext_22248 = ctx_param_ext_tmp_22755;
                            loop_while_21362 = loop_while_tmp_22756;
                            found_21363 = found_tmp_22757;
                            top_21366 = top_tmp_22760;
                        }
                        if (memblock_set(ctx, &ext_mem_22271, &mem_param_22246, "mem_param_22246") != 0)
                            return 1;
                        if (memblock_set(ctx, &ext_mem_22270, &mem_param_22249, "mem_param_22249") != 0)
                            return 1;
                        ext_22269 = ctx_param_ext_22244;
                        ext_22268 = ctx_param_ext_22245;
                        ext_22267 = ctx_param_ext_22247;
                        ext_22266 = ctx_param_ext_22248;
                        dinic_blocking_flow_res_21357 = loop_while_21362;
                        dinic_blocking_flow_res_21358 = found_21363;
                        dinic_blocking_flow_res_21361 = top_21366;
                        // graph_orientation_futhark.fut:178:6-201:29
                        
                        bool not_res_21437 = !dinic_blocking_flow_res_21358;
                        
                        // graph_orientation_futhark.fut:178:27-34
                        
                        bool cond_f_res_21438 = sle32(dinic_blocking_flow_res_21361, 0);
                        
                        // graph_orientation_futhark.fut:219:36-75
                        
                        bool y_21439 = dinic_blocking_flow_res_21358 && cond_f_res_21438;
                        
                        // graph_orientation_futhark.fut:219:36-75
                        
                        bool cond_21440 = not_res_21437 || y_21439;
                        
                        // graph_orientation_futhark.fut:178:6-201:29
                        
                        int64_t dinic_blocking_flow_res_21441;
                        int64_t dinic_blocking_flow_res_21443;
                        
                        if (cond_21440) {
                            // graph_orientation_futhark.fut:179:9-11
                            if (memblock_alloc(ctx, &mem_22288, (int64_t) 0, "mem_22288")) {
                                err = 1;
                                goto cleanup;
                            }
                            if (memblock_set(ctx, &ext_mem_22289, &mem_22288, "mem_22288") != 0)
                                return 1;
                            dinic_blocking_flow_res_21441 = (int64_t) 0;
                            dinic_blocking_flow_res_21443 = (int64_t) 0;
                        } else {
                            // graph_orientation_futhark.fut:182:10-186:33
                            if (memblock_alloc(ctx, &mem_22272, (int64_t) 0, "mem_22272")) {
                                err = 1;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:182:10-186:33
                            
                            int64_t dinic_blocking_flow_res_f_res_21446;
                            bool dinic_blocking_flow_res_f_res_21447;
                            int32_t dinic_blocking_flow_res_f_res_21448;
                            int64_t loop_dz2086Uz2087U_21450;
                            bool loop_while_21451;
                            int32_t node_21452;
                            
                            if (memblock_set(ctx, &mem_param_22275, &mem_22272, "mem_22272") != 0)
                                return 1;
                            loop_dz2086Uz2087U_21450 = (int64_t) 0;
                            loop_while_21451 = x_21346;
                            node_21452 = sink_res_17105;
                            while (loop_while_21451) {
                                // graph_orientation_futhark.fut:185:20-33
                                
                                int64_t node_21454 = sext_i32_i64(node_21452);
                                
                                // graph_orientation_futhark.fut:185:20-33
                                
                                bool x_21455 = sle64((int64_t) 0, node_21454);
                                
                                // graph_orientation_futhark.fut:185:20-33
                                
                                bool y_21456 = slt64(node_21454, i32_res_17111);
                                
                                // graph_orientation_futhark.fut:185:20-33
                                
                                bool bounds_check_21457 = x_21455 && y_21456;
                                
                                // graph_orientation_futhark.fut:185:20-33
                                
                                bool index_certs_21458;
                                
                                if (!bounds_check_21457) {
                                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) node_21454, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:185:20-33\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                    err = FUTHARK_PROGRAM_ERROR;
                                    goto cleanup;
                                }
                                // graph_orientation_futhark.fut:186:19-32
                                
                                int64_t conc_tmp_21461 = (int64_t) 1 + loop_dz2086Uz2087U_21450;
                                
                                // graph_orientation_futhark.fut:186:19-32
                                
                                int64_t bytes_22277 = (int64_t) 4 * conc_tmp_21461;
                                
                                // graph_orientation_futhark.fut:185:20-33
                                
                                int32_t p_21459 = ((int32_t *) ext_mem_22271.mem)[ext_22269 + node_21454 * ext_22268];
                                
                                // graph_orientation_futhark.fut:186:19-32
                                if (memblock_alloc(ctx, &mem_22278, bytes_22277, "mem_22278")) {
                                    err = 1;
                                    goto cleanup;
                                }
                                // graph_orientation_futhark.fut:219:36-75
                                for (int64_t nest_i_22769 = 0; nest_i_22769 < (int64_t) 1; nest_i_22769++) {
                                    ((int32_t *) mem_22278.mem)[nest_i_22769] = node_21452;
                                }
                                // graph_orientation_futhark.fut:186:19-32
                                
                                int64_t tmp_offs_22770 = (int64_t) 0;
                                
                                if (!(tmp_offs_22770 == (int64_t) 0)) {
                                    // graph_orientation_futhark.fut:186:19-32
                                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22278.mem, tmp_offs_22770, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_22278.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {(int64_t) 1});
                                }
                                tmp_offs_22770 += (int64_t) 1;
                                // graph_orientation_futhark.fut:186:19-32
                                lmad_copy_4b(ctx, 1, (uint32_t *) mem_22278.mem, tmp_offs_22770, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22275.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {loop_dz2086Uz2087U_21450});
                                tmp_offs_22770 += loop_dz2086Uz2087U_21450;
                                // graph_orientation_futhark.fut:183:18-44
                                
                                bool cond_21463 = p_21459 == 0;
                                
                                // graph_orientation_futhark.fut:183:18-44
                                
                                bool cond_21464 = !cond_21463;
                                
                                // graph_orientation_futhark.fut:183:36-44
                                
                                bool loop_cond_t_res_21465 = p_21459 == -1;
                                
                                // graph_orientation_futhark.fut:183:36-44
                                
                                bool loop_cond_t_res_21466 = !loop_cond_t_res_21465;
                                
                                // graph_orientation_futhark.fut:219:36-75
                                
                                bool x_21467 = cond_21464 && loop_cond_t_res_21466;
                                
                                if (memblock_set(ctx, &mem_param_tmp_22764, &mem_22278, "mem_22278") != 0)
                                    return 1;
                                
                                int64_t loop_dz2086Uz2087U_tmp_22765 = conc_tmp_21461;
                                bool loop_while_tmp_22766 = x_21467;
                                int32_t node_tmp_22767 = p_21459;
                                
                                if (memblock_set(ctx, &mem_param_22275, &mem_param_tmp_22764, "mem_param_tmp_22764") != 0)
                                    return 1;
                                loop_dz2086Uz2087U_21450 = loop_dz2086Uz2087U_tmp_22765;
                                loop_while_21451 = loop_while_tmp_22766;
                                node_21452 = node_tmp_22767;
                            }
                            if (memblock_set(ctx, &ext_mem_22281, &mem_param_22275, "mem_param_22275") != 0)
                                return 1;
                            dinic_blocking_flow_res_f_res_21446 = loop_dz2086Uz2087U_21450;
                            dinic_blocking_flow_res_f_res_21447 = loop_while_21451;
                            dinic_blocking_flow_res_f_res_21448 = node_21452;
                            if (memblock_unref(ctx, &mem_22272, "mem_22272") != 0)
                                return 1;
                            // graph_orientation_futhark.fut:187:19-58
                            
                            bool cond_21468 = dinic_blocking_flow_res_f_res_21448 == 0;
                            
                            // graph_orientation_futhark.fut:187:38-50
                            
                            int64_t conc_tmp_21469 = (int64_t) 1 + dinic_blocking_flow_res_f_res_21446;
                            
                            // graph_orientation_futhark.fut:219:36-75
                            
                            int64_t path_21470;
                            
                            if (cond_21468) {
                                path_21470 = conc_tmp_21469;
                            } else {
                                path_21470 = (int64_t) 0;
                            }
                            // graph_orientation_futhark.fut:219:36-75
                            
                            int64_t bytes_22282 = (int64_t) 4 * path_21470;
                            
                            // graph_orientation_futhark.fut:219:36-75
                            if (cond_21468) {
                                // graph_orientation_futhark.fut:219:36-75
                                if (memblock_alloc(ctx, &mem_22286, bytes_22282, "mem_22286")) {
                                    err = 1;
                                    goto cleanup;
                                }
                                // graph_orientation_futhark.fut:219:36-75
                                for (int64_t nest_i_22771 = 0; nest_i_22771 < (int64_t) 1; nest_i_22771++) {
                                    ((int32_t *) mem_22286.mem)[nest_i_22771] = 0;
                                }
                                // graph_orientation_futhark.fut:219:36-75
                                
                                int64_t tmp_offs_22772 = (int64_t) 0;
                                
                                if (!(tmp_offs_22772 == (int64_t) 0)) {
                                    // graph_orientation_futhark.fut:219:36-75
                                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22286.mem, tmp_offs_22772, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_22286.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {(int64_t) 1});
                                }
                                tmp_offs_22772 += (int64_t) 1;
                                // graph_orientation_futhark.fut:219:36-75
                                lmad_copy_4b(ctx, 1, (uint32_t *) mem_22286.mem, tmp_offs_22772, (int64_t []) {(int64_t) 1}, (uint32_t *) ext_mem_22281.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {dinic_blocking_flow_res_f_res_21446});
                                tmp_offs_22772 += dinic_blocking_flow_res_f_res_21446;
                                if (memblock_set(ctx, &ext_mem_22287, &mem_22286, "mem_22286") != 0)
                                    return 1;
                            } else {
                                // graph_orientation_futhark.fut:219:36-75
                                if (memblock_alloc(ctx, &mem_22283, bytes_22282, "mem_22283")) {
                                    err = 1;
                                    goto cleanup;
                                }
                                if (memblock_set(ctx, &ext_mem_22287, &mem_22283, "mem_22283") != 0)
                                    return 1;
                            }
                            if (memblock_unref(ctx, &ext_mem_22281, "ext_mem_22281") != 0)
                                return 1;
                            // graph_orientation_futhark.fut:190:10-200:16
                            
                            bool cond_21475 = slt64(path_21470, (int64_t) 2);
                            
                            // graph_orientation_futhark.fut:190:10-200:16
                            
                            int64_t bottleneck_21476;
                            
                            if (cond_21475) {
                                bottleneck_21476 = (int64_t) 0;
                            } else {
                                // graph_orientation_futhark.fut:192:14-199:30
                                
                                bool loop_cond_21477 = slt64((int64_t) 1, path_21470);
                                
                                // graph_orientation_futhark.fut:192:14-199:30
                                
                                bool b_21478;
                                int64_t b_21479;
                                int64_t b_21480;
                                bool loop_while_21481;
                                int64_t i_21482;
                                int64_t b_21483;
                                
                                loop_while_21481 = loop_cond_21477;
                                i_21482 = (int64_t) 1;
                                b_21483 = (int64_t) 9223372036854775807;
                                while (loop_while_21481) {
                                    // graph_orientation_futhark.fut:195:31-34
                                    
                                    int64_t u_21484 = sub64(i_21482, (int64_t) 1);
                                    
                                    // graph_orientation_futhark.fut:195:24-35
                                    
                                    bool x_21485 = sle64((int64_t) 0, u_21484);
                                    
                                    // graph_orientation_futhark.fut:195:24-35
                                    
                                    bool y_21486 = slt64(u_21484, path_21470);
                                    
                                    // graph_orientation_futhark.fut:195:24-35
                                    
                                    bool bounds_check_21487 = x_21485 && y_21486;
                                    
                                    // graph_orientation_futhark.fut:195:24-35
                                    
                                    bool index_certs_21488;
                                    
                                    if (!bounds_check_21487) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21484, "] out of bounds for array of shape [", (long long) path_21470, "].", "-> #0  graph_orientation_futhark.fut:195:24-35\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:195:24-35
                                    
                                    int32_t u_21489 = ((int32_t *) ext_mem_22287.mem)[u_21484];
                                    
                                    // graph_orientation_futhark.fut:196:24-31
                                    
                                    bool x_21490 = sle64((int64_t) 0, i_21482);
                                    
                                    // graph_orientation_futhark.fut:196:24-31
                                    
                                    bool y_21491 = slt64(i_21482, path_21470);
                                    
                                    // graph_orientation_futhark.fut:196:24-31
                                    
                                    bool bounds_check_21492 = x_21490 && y_21491;
                                    
                                    // graph_orientation_futhark.fut:196:24-31
                                    
                                    bool index_certs_21493;
                                    
                                    if (!bounds_check_21492) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) i_21482, "] out of bounds for array of shape [", (long long) path_21470, "].", "-> #0  graph_orientation_futhark.fut:196:24-31\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:196:24-31
                                    
                                    int32_t v_21494 = ((int32_t *) ext_mem_22287.mem)[i_21482];
                                    
                                    // graph_orientation_futhark.fut:197:25-31
                                    
                                    int64_t u_21495 = sext_i32_i64(u_21489);
                                    
                                    // graph_orientation_futhark.fut:197:25-31
                                    
                                    bool x_21496 = sle64((int64_t) 0, u_21495);
                                    
                                    // graph_orientation_futhark.fut:197:25-31
                                    
                                    bool y_21497 = slt64(u_21495, i32_res_17111);
                                    
                                    // graph_orientation_futhark.fut:197:25-31
                                    
                                    bool bounds_check_21498 = x_21496 && y_21497;
                                    
                                    // graph_orientation_futhark.fut:197:25-31
                                    
                                    bool index_certs_21499;
                                    
                                    if (!bounds_check_21498) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21495, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:197:25-31\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:197:25-34
                                    
                                    int64_t v_21500 = sext_i32_i64(v_21494);
                                    
                                    // graph_orientation_futhark.fut:197:25-34
                                    
                                    bool x_21501 = sle64((int64_t) 0, v_21500);
                                    
                                    // graph_orientation_futhark.fut:197:25-34
                                    
                                    bool y_21502 = slt64(v_21500, i32_res_17111);
                                    
                                    // graph_orientation_futhark.fut:197:25-34
                                    
                                    bool bounds_check_21503 = x_21501 && y_21502;
                                    
                                    // graph_orientation_futhark.fut:197:25-34
                                    
                                    bool index_certs_21504;
                                    
                                    if (!bounds_check_21503) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21500, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:197:25-34\n   #1  graph_orientation_futhark.fut:219:36-75\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:219:36-75
                                    
                                    int64_t zm_lhs_21505 = ((int64_t *) ext_mem_21959.mem)[u_21495 * i32_res_17111 + v_21500];
                                    
                                    // graph_orientation_futhark.fut:219:36-75
                                    
                                    int64_t zm_rhs_21506 = ((int64_t *) mem_param_22239.mem)[ctx_param_ext_22236 + (u_21495 * ctx_param_ext_22237 + v_21500 * ctx_param_ext_22238)];
                                    
                                    // graph_orientation_futhark.fut:197:35-47
                                    
                                    int64_t rc_21507 = sub64(zm_lhs_21505, zm_rhs_21506);
                                    
                                    // graph_orientation_futhark.fut:198:25-49
                                    
                                    bool cond_21508 = slt64(rc_21507, b_21483);
                                    
                                    // graph_orientation_futhark.fut:198:25-49
                                    
                                    int64_t bzq_21509;
                                    
                                    if (cond_21508) {
                                        bzq_21509 = rc_21507;
                                    } else {
                                        bzq_21509 = b_21483;
                                    }
                                    // graph_orientation_futhark.fut:199:22-25
                                    
                                    int64_t tmp_21510 = add64((int64_t) 1, i_21482);
                                    
                                    // graph_orientation_futhark.fut:193:24-29
                                    
                                    bool loop_cond_21511 = slt64(tmp_21510, path_21470);
                                    bool loop_while_tmp_22773 = loop_cond_21511;
                                    int64_t i_tmp_22774 = tmp_21510;
                                    int64_t b_tmp_22775 = bzq_21509;
                                    
                                    loop_while_21481 = loop_while_tmp_22773;
                                    i_21482 = i_tmp_22774;
                                    b_21483 = b_tmp_22775;
                                }
                                b_21478 = loop_while_21481;
                                b_21479 = i_21482;
                                b_21480 = b_21483;
                                bottleneck_21476 = b_21480;
                            }
                            if (memblock_set(ctx, &ext_mem_22289, &ext_mem_22287, "ext_mem_22287") != 0)
                                return 1;
                            dinic_blocking_flow_res_21441 = path_21470;
                            dinic_blocking_flow_res_21443 = bottleneck_21476;
                        }
                        if (memblock_unref(ctx, &ext_mem_22271, "ext_mem_22271") != 0)
                            return 1;
                        // graph_orientation_futhark.fut:220:19-224:50
                        
                        bool cond_21512 = sle64(dinic_blocking_flow_res_21443, (int64_t) 0);
                        
                        // graph_orientation_futhark.fut:220:51-54
                        
                        bool cond_f_res_21513 = slt64(dinic_blocking_flow_res_21441, (int64_t) 2);
                        
                        // graph_orientation_futhark.fut:356:31-44
                        
                        bool x_21514 = !cond_21512;
                        
                        // graph_orientation_futhark.fut:356:31-44
                        
                        bool y_21515 = cond_f_res_21513 && x_21514;
                        
                        // graph_orientation_futhark.fut:356:31-44
                        
                        bool cond_21516 = cond_21512 || y_21515;
                        
                        // graph_orientation_futhark.fut:93:26-33
                        
                        bool cond_f_res_21519 = dinic_blocking_flow_res_21443 == (int64_t) 0;
                        
                        // graph_orientation_futhark.fut:223:33-58
                        
                        bool x_21520 = !cond_f_res_21513;
                        
                        // graph_orientation_futhark.fut:223:33-58
                        
                        bool y_21521 = cond_f_res_21519 && x_21520;
                        
                        // graph_orientation_futhark.fut:223:33-58
                        
                        bool cond_21522 = cond_f_res_21513 || y_21521;
                        
                        // graph_orientation_futhark.fut:94:20-23
                        
                        int64_t k_21523 = sub64(dinic_blocking_flow_res_21441, (int64_t) 1);
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        bool empty_slice_21525 = k_21523 == (int64_t) 0;
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        int64_t m_21526 = sub64(k_21523, (int64_t) 1);
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        bool zzero_leq_i_p_m_t_s_21527 = sle64((int64_t) 0, m_21526);
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        bool i_p_m_t_s_leq_w_21528 = slt64(m_21526, dinic_blocking_flow_res_21441);
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        bool i_lte_j_21529 = sle64((int64_t) 0, k_21523);
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        bool y_21530 = zzero_leq_i_p_m_t_s_21527 && i_p_m_t_s_leq_w_21528;
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        bool forwards_ok_21531 = i_lte_j_21529 && y_21530;
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        bool ok_or_empty_21532 = empty_slice_21525 || forwards_ok_21531;
                        bool protect_assert_disj_22543 = cond_21522 || ok_or_empty_21532;
                        bool protect_assert_disj_22551 = cond_21516 || protect_assert_disj_22543;
                        
                        // graph_orientation_futhark.fut:95:17-28
                        
                        bool index_certs_21533;
                        
                        if (!protect_assert_disj_22551) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, ":", (long long) k_21523, "] out of bounds for array of shape [", (long long) dinic_blocking_flow_res_21441, "].", "-> #0  graph_orientation_futhark.fut:95:17-28\n   #1  graph_orientation_futhark.fut:223:33-58\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:96:25-36
                        
                        bool i_p_m_t_s_leq_w_21534 = slt64(k_21523, dinic_blocking_flow_res_21441);
                        
                        // graph_orientation_futhark.fut:96:25-36
                        
                        bool i_lte_j_21535 = sle64((int64_t) 1, dinic_blocking_flow_res_21441);
                        
                        // graph_orientation_futhark.fut:96:25-36
                        
                        bool y_21536 = i_lte_j_21529 && i_p_m_t_s_leq_w_21534;
                        
                        // graph_orientation_futhark.fut:96:25-36
                        
                        bool forwards_ok_21537 = i_lte_j_21535 && y_21536;
                        
                        // graph_orientation_futhark.fut:96:25-36
                        
                        bool ok_or_empty_21538 = empty_slice_21525 || forwards_ok_21537;
                        bool protect_assert_disj_22545 = cond_21522 || ok_or_empty_21538;
                        bool protect_assert_disj_22553 = cond_21516 || protect_assert_disj_22545;
                        
                        // graph_orientation_futhark.fut:96:25-36
                        
                        bool index_certs_21539;
                        
                        if (!protect_assert_disj_22553) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 1, ":] out of bounds for array of shape [", (long long) dinic_blocking_flow_res_21441, "].", "-> #0  graph_orientation_futhark.fut:96:25-36\n   #1  graph_orientation_futhark.fut:223:33-58\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:96:17-36
                        
                        bool i_p_m_t_s_leq_w_21540 = slt64(m_21526, k_21523);
                        
                        // graph_orientation_futhark.fut:96:17-36
                        
                        bool y_21541 = zzero_leq_i_p_m_t_s_21527 && i_p_m_t_s_leq_w_21540;
                        
                        // graph_orientation_futhark.fut:96:17-36
                        
                        bool ok_or_empty_21542 = empty_slice_21525 || y_21541;
                        bool protect_assert_disj_22547 = cond_21522 || ok_or_empty_21542;
                        bool protect_assert_disj_22555 = cond_21516 || protect_assert_disj_22547;
                        
                        // graph_orientation_futhark.fut:96:17-36
                        
                        bool index_certs_21543;
                        
                        if (!protect_assert_disj_22555) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, ":", (long long) k_21523, "] out of bounds for array of shape [", (long long) k_21523, "].", "-> #0  graph_orientation_futhark.fut:96:17-36\n   #1  graph_orientation_futhark.fut:223:33-58\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:356:31-44
                        
                        int64_t ext_22307;
                        int64_t ext_22306;
                        int64_t ext_22305;
                        int64_t loopres_21518;
                        
                        if (cond_21516) {
                            if (memblock_set(ctx, &ext_mem_22308, &mem_param_22239, "mem_param_22239") != 0)
                                return 1;
                            ext_22307 = ctx_param_ext_22236;
                            ext_22306 = ctx_param_ext_22237;
                            ext_22305 = ctx_param_ext_22238;
                            loopres_21518 = ds_21352;
                        } else {
                            // graph_orientation_futhark.fut:93:6-103:18
                            
                            int64_t ext_22303;
                            int64_t ext_22302;
                            int64_t ext_22301;
                            
                            if (cond_21522) {
                                if (memblock_set(ctx, &ext_mem_22304, &mem_param_22239, "mem_param_22239") != 0)
                                    return 1;
                                ext_22303 = ctx_param_ext_22236;
                                ext_22302 = ctx_param_ext_22237;
                                ext_22301 = ctx_param_ext_22238;
                            } else {
                                // graph_orientation_futhark.fut:97:11-103:18
                                
                                int64_t ext_22299;
                                int64_t ext_22298;
                                int64_t ext_22297;
                                int64_t ctx_param_ext_22290;
                                int64_t ctx_param_ext_22291;
                                int64_t ctx_param_ext_22292;
                                
                                if (memblock_set(ctx, &mem_param_22293, &mem_param_22239, "mem_param_22239") != 0)
                                    return 1;
                                ctx_param_ext_22290 = ctx_param_ext_22236;
                                ctx_param_ext_22291 = ctx_param_ext_22237;
                                ctx_param_ext_22292 = ctx_param_ext_22238;
                                for (int64_t i_21545 = 0; i_21545 < k_21523; i_21545++) {
                                    // graph_orientation_futhark.fut:98:21-33
                                    
                                    int32_t i64_res_21547 = sext_i64_i32(i_21545);
                                    
                                    // graph_orientation_futhark.fut:99:21-26
                                    
                                    int64_t j_21548 = sext_i32_i64(i64_res_21547);
                                    
                                    // graph_orientation_futhark.fut:99:21-26
                                    
                                    bool x_21549 = sle64((int64_t) 0, j_21548);
                                    
                                    // graph_orientation_futhark.fut:99:21-26
                                    
                                    bool y_21550 = slt64(j_21548, k_21523);
                                    
                                    // graph_orientation_futhark.fut:99:21-26
                                    
                                    bool bounds_check_21551 = x_21549 && y_21550;
                                    
                                    // graph_orientation_futhark.fut:99:21-26
                                    
                                    bool index_certs_21552;
                                    
                                    if (!bounds_check_21551) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) j_21548, "] out of bounds for array of shape [", (long long) k_21523, "].", "-> #0  graph_orientation_futhark.fut:99:21-26\n   #1  graph_orientation_futhark.fut:223:33-58\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:223:33-58
                                    
                                    int32_t u_21553 = ((int32_t *) ext_mem_22289.mem)[j_21548];
                                    
                                    // graph_orientation_futhark.fut:223:33-58
                                    
                                    int64_t index_primexp_21554 = (int64_t) 1 + j_21548;
                                    
                                    // graph_orientation_futhark.fut:223:33-58
                                    
                                    int32_t v_21555 = ((int32_t *) ext_mem_22289.mem)[index_primexp_21554];
                                    
                                    // graph_orientation_futhark.fut:101:44-48
                                    
                                    int64_t u_21556 = sext_i32_i64(u_21553);
                                    
                                    // graph_orientation_futhark.fut:101:44-48
                                    
                                    bool x_21557 = sle64((int64_t) 0, u_21556);
                                    
                                    // graph_orientation_futhark.fut:101:44-48
                                    
                                    bool y_21558 = slt64(u_21556, i32_res_17111);
                                    
                                    // graph_orientation_futhark.fut:101:44-48
                                    
                                    bool bounds_check_21559 = x_21557 && y_21558;
                                    
                                    // graph_orientation_futhark.fut:101:44-48
                                    
                                    bool index_certs_21560;
                                    
                                    if (!bounds_check_21559) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21556, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:101:44-48\n   #1  graph_orientation_futhark.fut:223:33-58\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:101:44-51
                                    
                                    int64_t v_21561 = sext_i32_i64(v_21555);
                                    
                                    // graph_orientation_futhark.fut:101:44-51
                                    
                                    bool x_21562 = sle64((int64_t) 0, v_21561);
                                    
                                    // graph_orientation_futhark.fut:101:44-51
                                    
                                    bool y_21563 = slt64(v_21561, i32_res_17111);
                                    
                                    // graph_orientation_futhark.fut:101:44-51
                                    
                                    bool bounds_check_21564 = x_21562 && y_21563;
                                    
                                    // graph_orientation_futhark.fut:101:44-51
                                    
                                    bool index_certs_21565;
                                    
                                    if (!bounds_check_21564) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21561, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:101:44-51\n   #1  graph_orientation_futhark.fut:223:33-58\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:101:22-60
                                    
                                    bool index_ok_21569 = bounds_check_21559 && bounds_check_21564;
                                    
                                    // graph_orientation_futhark.fut:101:22-60
                                    
                                    bool index_certs_21570;
                                    
                                    if (!index_ok_21569) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21556, ", ", (long long) v_21561, "] out of bounds for array of shape [", (long long) i32_res_17111, "][", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:101:22-60\n   #1  graph_orientation_futhark.fut:223:33-58\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:102:22-62
                                    
                                    bool index_certs_21575;
                                    
                                    if (!index_ok_21569) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21561, ", ", (long long) u_21556, "] out of bounds for array of shape [", (long long) i32_res_17111, "][", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:102:22-62\n   #1  graph_orientation_futhark.fut:223:33-58\n   #2  graph_orientation_futhark.fut:356:31-44\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:223:33-58
                                    
                                    int64_t zp_lhs_21566 = ((int64_t *) mem_param_22293.mem)[ctx_param_ext_22290 + (u_21556 * ctx_param_ext_22291 + v_21561 * ctx_param_ext_22292)];
                                    
                                    // graph_orientation_futhark.fut:101:52-59
                                    
                                    int64_t f1_21567 = add64(dinic_blocking_flow_res_21443, zp_lhs_21566);
                                    
                                    // graph_orientation_futhark.fut:101:22-28
                                    if (memblock_alloc(ctx, &mem_22296, bytes_21930, "mem_22296")) {
                                        err = 1;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:101:22-28
                                    // graph_orientation_futhark.fut:101:22-28
                                    lmad_copy_8b(ctx, 2, (uint64_t *) mem_22296.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (uint64_t *) mem_param_22293.mem, ctx_param_ext_22290, (int64_t []) {ctx_param_ext_22291, ctx_param_ext_22292}, (int64_t []) {i32_res_17111, i32_res_17111});
                                    // graph_orientation_futhark.fut:101:22-60
                                    ((int64_t *) mem_22296.mem)[u_21556 * i32_res_17111 + v_21561] = f1_21567;
                                    // graph_orientation_futhark.fut:223:33-58
                                    
                                    int64_t zm_lhs_21572 = ((int64_t *) mem_22296.mem)[v_21561 * i32_res_17111 + u_21556];
                                    
                                    // graph_orientation_futhark.fut:102:54-61
                                    
                                    int64_t f2_21573 = sub64(zm_lhs_21572, dinic_blocking_flow_res_21443);
                                    
                                    // graph_orientation_futhark.fut:102:22-62
                                    ((int64_t *) mem_22296.mem)[v_21561 * i32_res_17111 + u_21556] = f2_21573;
                                    if (memblock_set(ctx, &mem_param_tmp_22776, &mem_22296, "mem_22296") != 0)
                                        return 1;
                                    
                                    int64_t ctx_param_ext_tmp_22777 = (int64_t) 0;
                                    int64_t ctx_param_ext_tmp_22778 = i32_res_17111;
                                    int64_t ctx_param_ext_tmp_22779 = (int64_t) 1;
                                    
                                    if (memblock_set(ctx, &mem_param_22293, &mem_param_tmp_22776, "mem_param_tmp_22776") != 0)
                                        return 1;
                                    ctx_param_ext_22290 = ctx_param_ext_tmp_22777;
                                    ctx_param_ext_22291 = ctx_param_ext_tmp_22778;
                                    ctx_param_ext_22292 = ctx_param_ext_tmp_22779;
                                }
                                if (memblock_set(ctx, &ext_mem_22300, &mem_param_22293, "mem_param_22293") != 0)
                                    return 1;
                                ext_22299 = ctx_param_ext_22290;
                                ext_22298 = ctx_param_ext_22291;
                                ext_22297 = ctx_param_ext_22292;
                                if (memblock_set(ctx, &ext_mem_22304, &ext_mem_22300, "ext_mem_22300") != 0)
                                    return 1;
                                ext_22303 = ext_22299;
                                ext_22302 = ext_22298;
                                ext_22301 = ext_22297;
                            }
                            // graph_orientation_futhark.fut:224:42-49
                            
                            int64_t tmp_21577 = add64(ds_21352, dinic_blocking_flow_res_21443);
                            
                            if (memblock_set(ctx, &ext_mem_22308, &ext_mem_22304, "ext_mem_22304") != 0)
                                return 1;
                            ext_22307 = ext_22303;
                            ext_22306 = ext_22302;
                            ext_22305 = ext_22301;
                            loopres_21518 = tmp_21577;
                        }
                        if (memblock_unref(ctx, &ext_mem_22289, "ext_mem_22289") != 0)
                            return 1;
                        // graph_orientation_futhark.fut:217:22-36
                        
                        bool not_res_21578 = !cond_21516;
                        
                        if (memblock_set(ctx, &mem_param_tmp_22741, &ext_mem_22308, "ext_mem_22308") != 0)
                            return 1;
                        
                        int64_t ctx_param_ext_tmp_22742 = ext_22307;
                        int64_t ctx_param_ext_tmp_22743 = ext_22306;
                        int64_t ctx_param_ext_tmp_22744 = ext_22305;
                        bool loop_while_tmp_22745 = not_res_21578;
                        int64_t ds_tmp_22747 = loopres_21518;
                        
                        if (memblock_set(ctx, &mem_param_22239, &mem_param_tmp_22741, "mem_param_tmp_22741") != 0)
                            return 1;
                        ctx_param_ext_22236 = ctx_param_ext_tmp_22742;
                        ctx_param_ext_22237 = ctx_param_ext_tmp_22743;
                        ctx_param_ext_22238 = ctx_param_ext_tmp_22744;
                        loop_while_21350 = loop_while_tmp_22745;
                        ds_21352 = ds_tmp_22747;
                    }
                    if (memblock_set(ctx, &ext_mem_22312, &mem_param_22239, "mem_param_22239") != 0)
                        return 1;
                    ext_22311 = ctx_param_ext_22236;
                    ext_22310 = ctx_param_ext_22237;
                    ext_22309 = ctx_param_ext_22238;
                    loopres_f_res_21347 = loop_while_21350;
                    loopres_f_res_21349 = ds_21352;
                    if (memblock_unref(ctx, &mem_22241, "mem_22241") != 0)
                        return 1;
                    if (memblock_unref(ctx, &mem_22243, "mem_22243") != 0)
                        return 1;
                    // graph_orientation_futhark.fut:225:15-226:46
                    
                    bool cond_21579 = loopres_f_res_21349 == (int64_t) 0;
                    
                    // graph_orientation_futhark.fut:356:31-44
                    
                    int64_t loopres_f_res_21580;
                    
                    if (cond_21579) {
                        loopres_f_res_21580 = tot_21269;
                    } else {
                        // graph_orientation_futhark.fut:226:39-45
                        
                        int64_t tmp_21581 = add64(tot_21269, loopres_f_res_21349);
                        
                        loopres_f_res_21580 = tmp_21581;
                    }
                    if (memblock_set(ctx, &ext_mem_22316, &ext_mem_22312, "ext_mem_22312") != 0)
                        return 1;
                    ext_22315 = ext_22311;
                    ext_22314 = ext_22310;
                    ext_22313 = ext_22309;
                    loopres_21339 = cond_21579;
                    loopres_21341 = loopres_f_res_21580;
                }
                if (memblock_unref(ctx, &ext_mem_22235, "ext_mem_22235") != 0)
                    return 1;
                // graph_orientation_futhark.fut:209:13-27
                
                bool not_res_21582 = !loopres_21339;
                
                if (memblock_set(ctx, &mem_param_tmp_22712, &ext_mem_22316, "ext_mem_22316") != 0)
                    return 1;
                
                int64_t ctx_param_ext_tmp_22713 = ext_22315;
                int64_t ctx_param_ext_tmp_22714 = ext_22314;
                int64_t ctx_param_ext_tmp_22715 = ext_22313;
                bool loop_while_tmp_22716 = not_res_21582;
                int64_t tot_tmp_22718 = loopres_21341;
                
                if (memblock_set(ctx, &mem_param_22197, &mem_param_tmp_22712, "mem_param_tmp_22712") != 0)
                    return 1;
                ctx_param_ext_22194 = ctx_param_ext_tmp_22713;
                ctx_param_ext_22195 = ctx_param_ext_tmp_22714;
                ctx_param_ext_22196 = ctx_param_ext_tmp_22715;
                loop_while_21267 = loop_while_tmp_22716;
                tot_21269 = tot_tmp_22718;
            }
            if (memblock_set(ctx, &ext_mem_22320, &mem_param_22197, "mem_param_22197") != 0)
                return 1;
            ext_22319 = ctx_param_ext_22194;
            ext_22318 = ctx_param_ext_22195;
            ext_22317 = ctx_param_ext_22196;
            dinic_res_21264 = loop_while_21267;
            dinic_res_21266 = tot_21269;
            if (memblock_unref(ctx, &mem_22199, "mem_22199") != 0)
                return 1;
            if (memblock_unref(ctx, &mem_22201, "mem_22201") != 0)
                return 1;
            if (memblock_set(ctx, &ext_mem_22324, &ext_mem_22320, "ext_mem_22320") != 0)
                return 1;
            ext_22323 = ext_22319;
            ext_22322 = ext_22318;
            ext_22321 = ext_22317;
            graph_orientation_solve_res_f_res_17389 = dinic_res_21266;
        } else {
            // graph_orientation_futhark.fut:274:13-31
            if (memblock_alloc(ctx, &mem_21963, binop_x_21929, "mem_21963")) {
                err = 1;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:274:13-31
            for (int64_t nest_i_22781 = 0; nest_i_22781 < i32_res_17111; nest_i_22781++) {
                ((int64_t *) mem_21963.mem)[nest_i_22781] = (int64_t) 0;
            }
            // graph_orientation_futhark.fut:277:18-78
            if (mem_21965_cached_sizze_22889 < binop_x_21929) {
                err = lexical_realloc(ctx, &mem_21965, &mem_21965_cached_sizze_22889, binop_x_21929);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:277:18-78
            if (mem_21967_cached_sizze_22890 < binop_x_21929) {
                err = lexical_realloc(ctx, &mem_21967, &mem_21967_cached_sizze_22890, binop_x_21929);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:277:18-78
            
            int64_t discard_21778;
            int64_t scanacc_21772 = (int64_t) 0;
            
            for (int64_t i_21775 = 0; i_21775 < i32_res_17111; i_21775++) {
                // graph_orientation_futhark.fut:277:48-59
                
                int32_t i64_res_19357 = sext_i64_i32(i_21775);
                
                // graph_orientation_futhark.fut:277:41-60
                
                int64_t zg_lhs_19358 = sext_i32_i64(i64_res_19357);
                
                // graph_orientation_futhark.fut:277:41-60
                
                bool x_19359 = sle64((int64_t) 0, zg_lhs_19358);
                
                // graph_orientation_futhark.fut:277:41-60
                
                bool y_19360 = slt64(zg_lhs_19358, i32_res_17111);
                
                // graph_orientation_futhark.fut:277:41-60
                
                bool bounds_check_19361 = x_19359 && y_19360;
                
                // graph_orientation_futhark.fut:277:41-60
                
                bool index_certs_19362;
                
                if (!bounds_check_19361) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) zg_lhs_19358, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:277:41-60\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:277:18-78
                
                int64_t zg_lhs_19363 = ((int64_t *) ext_mem_21959.mem)[zg_lhs_19358];
                
                // graph_orientation_futhark.fut:277:61-67
                
                bool lifted_lambda_res_19364 = slt64((int64_t) 0, zg_lhs_19363);
                
                // graph_orientation_futhark.fut:277:18-78
                
                int64_t defunc_0_f_res_19365 = btoi_bool_i64(lifted_lambda_res_19364);
                
                // graph_orientation_futhark.fut:277:18-78
                
                int64_t defunc_0_op_res_17734 = add64(defunc_0_f_res_19365, scanacc_21772);
                
                ((int64_t *) mem_21965)[i_21775] = defunc_0_op_res_17734;
                ((int64_t *) mem_21967)[i_21775] = defunc_0_f_res_19365;
                
                int64_t scanacc_tmp_22782 = defunc_0_op_res_17734;
                
                scanacc_21772 = scanacc_tmp_22782;
            }
            discard_21778 = scanacc_21772;
            // graph_orientation_futhark.fut:277:18-78
            
            int64_t m_f_res_17743;
            
            if (x_17736) {
                // graph_orientation_futhark.fut:277:18-78
                
                int64_t x_21583 = ((int64_t *) mem_21965)[tmp_17737];
                
                m_f_res_17743 = x_21583;
            } else {
                m_f_res_17743 = (int64_t) 0;
            }
            // graph_orientation_futhark.fut:277:18-78
            
            int64_t m_17745;
            
            if (cond_17735) {
                m_17745 = (int64_t) 0;
            } else {
                m_17745 = m_f_res_17743;
            }
            // graph_orientation_futhark.fut:277:18-78
            
            int64_t bytes_21980 = (int64_t) 8 * m_17745;
            
            // graph_orientation_futhark.fut:277:18-78
            if (mem_21981_cached_sizze_22891 < bytes_21980) {
                err = lexical_realloc(ctx, &mem_21981, &mem_21981_cached_sizze_22891, bytes_21980);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:277:18-78
            
            bool acc_cert_19327;
            
            // graph_orientation_futhark.fut:277:18-78
            for (int64_t i_21780 = 0; i_21780 < i32_res_17111; i_21780++) {
                int64_t eta_p_19342 = ((int64_t *) mem_21967)[i_21780];
                int64_t eta_p_19343 = ((int64_t *) mem_21965)[i_21780];
                
                // graph_orientation_futhark.fut:277:18-78
                
                bool cond_19346 = eta_p_19342 == (int64_t) 1;
                
                // graph_orientation_futhark.fut:277:18-78
                
                int64_t lifted_lambda_res_19347;
                
                if (cond_19346) {
                    // graph_orientation_futhark.fut:277:18-78
                    
                    int64_t lifted_lambda_res_t_res_21584 = sub64(eta_p_19343, (int64_t) 1);
                    
                    lifted_lambda_res_19347 = lifted_lambda_res_t_res_21584;
                } else {
                    lifted_lambda_res_19347 = (int64_t) -1;
                }
                // graph_orientation_futhark.fut:277:18-78
                // UpdateAcc
                if (sle64((int64_t) 0, lifted_lambda_res_19347) && slt64(lifted_lambda_res_19347, m_17745)) {
                    ((int64_t *) mem_21981)[lifted_lambda_res_19347] = i_21780;
                }
            }
            // graph_orientation_futhark.fut:277:5-79
            
            int64_t bytes_21982 = (int64_t) 4 * m_17745;
            
            // graph_orientation_futhark.fut:277:5-79
            if (mem_21983_cached_sizze_22892 < bytes_21982) {
                err = lexical_realloc(ctx, &mem_21983, &mem_21983_cached_sizze_22892, bytes_21982);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:277:5-79
            for (int64_t i_21783 = 0; i_21783 < m_17745; i_21783++) {
                int64_t eta_p_17762 = ((int64_t *) mem_21981)[i_21783];
                
                // graph_orientation_futhark.fut:277:9-16
                
                int32_t i64_res_17763 = sext_i64_i32(eta_p_17762);
                
                ((int32_t *) mem_21983)[i_21783] = i64_res_17763;
            }
            // graph_orientation_futhark.fut:357:10-30
            
            bool loop_nonempty_17764 = slt64((int64_t) 0, m_17745);
            
            // graph_orientation_futhark.fut:357:10-30
            
            bool loop_not_taken_17765 = !loop_nonempty_17764;
            
            // graph_orientation_futhark.fut:357:10-30
            
            bool protect_assert_disj_17766 = y_17712 || loop_not_taken_17765;
            
            // graph_orientation_futhark.fut:281:45-51
            
            bool index_certs_17767;
            
            if (!protect_assert_disj_17766) {
                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:281:45-51\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                err = FUTHARK_PROGRAM_ERROR;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:274:23-284:15
            if (memblock_set(ctx, &mem_param_21992, &mem_21963, "mem_21963") != 0)
                return 1;
            for (int64_t i_17769 = 0; i_17769 < m_17745; i_17769++) {
                // graph_orientation_futhark.fut:274:23-284:15
                
                int32_t b_17771 = ((int32_t *) mem_21983)[i_17769];
                
                // graph_orientation_futhark.fut:281:37-42
                
                int64_t v_17772 = sext_i32_i64(b_17771);
                
                // graph_orientation_futhark.fut:281:37-42
                
                bool x_17773 = sle64((int64_t) 0, v_17772);
                
                // graph_orientation_futhark.fut:281:37-42
                
                bool y_17774 = slt64(v_17772, i32_res_17111);
                
                // graph_orientation_futhark.fut:281:37-42
                
                bool bounds_check_17775 = x_17773 && y_17774;
                
                // graph_orientation_futhark.fut:281:37-42
                
                bool index_certs_17776;
                
                if (!bounds_check_17775) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_17772, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:281:37-42\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:281:37-42
                
                int64_t zp_lhs_17777 = ((int64_t *) mem_param_21992.mem)[v_17772];
                
                // graph_orientation_futhark.fut:274:23-284:15
                
                int64_t zp_rhs_17781 = ((int64_t *) ext_mem_21959.mem)[v_17772];
                
                // graph_orientation_futhark.fut:281:43-54
                
                int64_t exa_17782 = add64(zp_lhs_17777, zp_rhs_17781);
                
                // graph_orientation_futhark.fut:281:17-24
                if (memblock_alloc(ctx, &mem_21994, binop_x_21929, "mem_21994")) {
                    err = 1;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:281:17-24
                // graph_orientation_futhark.fut:281:17-24
                lmad_copy_8b(ctx, 1, (uint64_t *) mem_21994.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint64_t *) mem_param_21992.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {i32_res_17111});
                // graph_orientation_futhark.fut:281:17-55
                ((int64_t *) mem_21994.mem)[v_17772] = exa_17782;
                // graph_orientation_futhark.fut:282:38-44
                
                int64_t zm_lhs_17785 = ((int64_t *) mem_21994.mem)[(int64_t) 0];
                
                // graph_orientation_futhark.fut:282:45-56
                
                int64_t exb_17786 = sub64(zm_lhs_17785, zp_rhs_17781);
                
                // graph_orientation_futhark.fut:282:17-57
                ((int64_t *) mem_21994.mem)[(int64_t) 0] = exb_17786;
                if (memblock_set(ctx, &mem_param_tmp_22787, &mem_21994, "mem_21994") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_21992, &mem_param_tmp_22787, "mem_param_tmp_22787") != 0)
                    return 1;
            }
            if (memblock_set(ctx, &ext_mem_21997, &mem_param_21992, "mem_param_21992") != 0)
                return 1;
            if (memblock_unref(ctx, &mem_21963, "mem_21963") != 0)
                return 1;
            // graph_orientation_futhark.fut:294:18-297:16
            if (mem_22010_cached_sizze_22893 < binop_x_21929) {
                err = lexical_realloc(ctx, &mem_22010, &mem_22010_cached_sizze_22893, binop_x_21929);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:294:18-297:16
            if (mem_22012_cached_sizze_22894 < binop_x_21929) {
                err = lexical_realloc(ctx, &mem_22012, &mem_22012_cached_sizze_22894, binop_x_21929);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:294:18-297:16
            
            int64_t discard_21793;
            int64_t scanacc_21787 = (int64_t) 0;
            
            for (int64_t i_21790 = 0; i_21790 < i32_res_17111; i_21790++) {
                // graph_orientation_futhark.fut:295:15-26
                
                int32_t i64_res_19310 = sext_i64_i32(i_21790);
                
                // graph_orientation_futhark.fut:296:10-43
                
                bool cond_19311 = i64_res_19310 == 0;
                
                // graph_orientation_futhark.fut:296:10-43
                
                bool cond_19312 = !cond_19311;
                
                // graph_orientation_futhark.fut:296:22-26
                
                bool cond_t_res_19313 = i64_res_19310 == sink_res_17105;
                
                // graph_orientation_futhark.fut:296:22-26
                
                bool cond_t_res_19314 = !cond_t_res_19313;
                
                // graph_orientation_futhark.fut:294:18-297:16
                
                bool x_19315 = cond_19312 && cond_t_res_19314;
                
                // graph_orientation_futhark.fut:296:10-43
                
                bool lifted_lambda_res_19316;
                
                if (x_19315) {
                    // graph_orientation_futhark.fut:296:30-36
                    
                    int64_t v_21587 = sext_i32_i64(i64_res_19310);
                    
                    // graph_orientation_futhark.fut:296:30-36
                    
                    bool x_21588 = sle64((int64_t) 0, v_21587);
                    
                    // graph_orientation_futhark.fut:296:30-36
                    
                    bool y_21589 = slt64(v_21587, i32_res_17111);
                    
                    // graph_orientation_futhark.fut:296:30-36
                    
                    bool bounds_check_21590 = x_21588 && y_21589;
                    
                    // graph_orientation_futhark.fut:296:30-36
                    
                    bool index_certs_21591;
                    
                    if (!bounds_check_21590) {
                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21587, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:296:30-36\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                        err = FUTHARK_PROGRAM_ERROR;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:296:30-36
                    
                    int64_t zg_lhs_21592 = ((int64_t *) ext_mem_21997.mem)[v_21587];
                    
                    // graph_orientation_futhark.fut:296:37-43
                    
                    bool lifted_lambda_res_t_res_21593 = slt64((int64_t) 0, zg_lhs_21592);
                    
                    lifted_lambda_res_19316 = lifted_lambda_res_t_res_21593;
                } else {
                    lifted_lambda_res_19316 = 0;
                }
                // graph_orientation_futhark.fut:294:18-297:16
                
                int64_t defunc_0_f_res_19324 = btoi_bool_i64(lifted_lambda_res_19316);
                
                // graph_orientation_futhark.fut:294:18-297:16
                
                int64_t defunc_0_op_res_17828 = add64(defunc_0_f_res_19324, scanacc_21787);
                
                ((int64_t *) mem_22010)[i_21790] = defunc_0_op_res_17828;
                ((int64_t *) mem_22012)[i_21790] = defunc_0_f_res_19324;
                
                int64_t scanacc_tmp_22789 = defunc_0_op_res_17828;
                
                scanacc_21787 = scanacc_tmp_22789;
            }
            discard_21793 = scanacc_21787;
            // graph_orientation_futhark.fut:294:18-297:16
            
            int64_t m_f_res_17829;
            
            if (x_17736) {
                // graph_orientation_futhark.fut:294:18-297:16
                
                int64_t x_21594 = ((int64_t *) mem_22010)[tmp_17737];
                
                m_f_res_17829 = x_21594;
            } else {
                m_f_res_17829 = (int64_t) 0;
            }
            // graph_orientation_futhark.fut:294:18-297:16
            
            int64_t m_17831;
            
            if (cond_17735) {
                m_17831 = (int64_t) 0;
            } else {
                m_17831 = m_f_res_17829;
            }
            // graph_orientation_futhark.fut:294:18-297:16
            
            int64_t bytes_22025 = (int64_t) 8 * m_17831;
            
            // graph_orientation_futhark.fut:294:5-297:17
            
            int64_t bytes_22027 = (int64_t) 4 * m_17831;
            
            // graph_orientation_futhark.fut:300:5-313:28
            
            bool loop_cond_17850 = slt64((int64_t) 0, m_17831);
            
            // graph_orientation_futhark.fut:357:10-30
            
            bool loop_not_taken_17851 = !loop_cond_17850;
            
            // graph_orientation_futhark.fut:357:10-30
            
            bool protect_assert_disj_17852 = protect_assert_disj_17741 || loop_not_taken_17851;
            
            // graph_orientation_futhark.fut:307:22-310:20
            
            bool index_certs_17853;
            
            if (!protect_assert_disj_17852) {
                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) tmp_17737, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:307:22-310:20\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                err = FUTHARK_PROGRAM_ERROR;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:315:7-13
            
            bool index_certs_18115;
            
            if (!bounds_check_21262) {
                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) t_21255, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:315:7-13\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                err = FUTHARK_PROGRAM_ERROR;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:272:13-31
            if (memblock_alloc(ctx, &mem_21961, bytes_21960, "mem_21961")) {
                err = 1;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:272:13-31
            for (int64_t nest_i_22792 = 0; nest_i_22792 < i32_res_17111; nest_i_22792++) {
                ((int32_t *) mem_21961.mem)[nest_i_22792] = 0;
            }
            // graph_orientation_futhark.fut:272:12-57
            ((int32_t *) mem_21961.mem)[(int64_t) 0] = node_count_res_17110;
            // graph_orientation_futhark.fut:273:38-291:15
            if (memblock_set(ctx, &mem_param_22001, &mem_21931, "mem_21931") != 0)
                return 1;
            for (int64_t i_17789 = 0; i_17789 < m_17745; i_17789++) {
                // graph_orientation_futhark.fut:273:38-291:15
                
                int32_t b_17791 = ((int32_t *) mem_21983)[i_17789];
                
                // graph_orientation_futhark.fut:288:37-46
                
                int64_t v_17792 = sext_i32_i64(b_17791);
                
                // graph_orientation_futhark.fut:288:37-46
                
                bool x_17793 = sle64((int64_t) 0, v_17792);
                
                // graph_orientation_futhark.fut:288:37-46
                
                bool y_17794 = slt64(v_17792, i32_res_17111);
                
                // graph_orientation_futhark.fut:288:37-46
                
                bool bounds_check_17795 = x_17793 && y_17794;
                
                // graph_orientation_futhark.fut:288:37-46
                
                bool index_certs_17796;
                
                if (!bounds_check_17795) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_17792, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:288:37-46\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:288:16-46
                
                bool index_ok_17801 = y_17712 && bounds_check_17795;
                
                // graph_orientation_futhark.fut:288:16-46
                
                bool index_certs_17802;
                
                if (!index_ok_17801) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, ", ", (long long) v_17792, "] out of bounds for array of shape [", (long long) i32_res_17111, "][", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:288:16-46\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:289:16-50
                
                bool index_certs_17805;
                
                if (!index_ok_17801) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_17792, ", ", (long long) (int64_t) 0, "] out of bounds for array of shape [", (long long) i32_res_17111, "][", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:289:16-50\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:273:38-291:15
                
                int64_t f1_17797 = ((int64_t *) ext_mem_21959.mem)[v_17792];
                
                // graph_orientation_futhark.fut:288:16-22
                if (memblock_alloc(ctx, &mem_22004, bytes_21930, "mem_22004")) {
                    err = 1;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:288:16-22
                // graph_orientation_futhark.fut:288:16-22
                lmad_copy_8b(ctx, 2, (uint64_t *) mem_22004.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (uint64_t *) mem_param_22001.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (int64_t []) {i32_res_17111, i32_res_17111});
                // graph_orientation_futhark.fut:288:16-46
                ((int64_t *) mem_22004.mem)[v_17792] = f1_17797;
                // graph_orientation_futhark.fut:289:16-50
                
                int64_t lw_val_17804 = -f1_17797;
                
                // graph_orientation_futhark.fut:289:16-50
                ((int64_t *) mem_22004.mem)[v_17792 * i32_res_17111] = lw_val_17804;
                if (memblock_set(ctx, &mem_param_tmp_22793, &mem_22004, "mem_22004") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22001, &mem_param_tmp_22793, "mem_param_tmp_22793") != 0)
                    return 1;
            }
            if (memblock_set(ctx, &ext_mem_22008, &mem_param_22001, "mem_param_22001") != 0)
                return 1;
            // graph_orientation_futhark.fut:294:18-297:16
            if (mem_22026_cached_sizze_22895 < bytes_22025) {
                err = lexical_realloc(ctx, &mem_22026, &mem_22026_cached_sizze_22895, bytes_22025);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:294:18-297:16
            
            bool acc_cert_19280;
            
            // graph_orientation_futhark.fut:294:18-297:16
            for (int64_t i_21795 = 0; i_21795 < i32_res_17111; i_21795++) {
                int64_t eta_p_19295 = ((int64_t *) mem_22012)[i_21795];
                int64_t eta_p_19296 = ((int64_t *) mem_22010)[i_21795];
                
                // graph_orientation_futhark.fut:294:18-297:16
                
                bool cond_19299 = eta_p_19295 == (int64_t) 1;
                
                // graph_orientation_futhark.fut:294:18-297:16
                
                int64_t lifted_lambda_res_19300;
                
                if (cond_19299) {
                    // graph_orientation_futhark.fut:294:18-297:16
                    
                    int64_t lifted_lambda_res_t_res_21595 = sub64(eta_p_19296, (int64_t) 1);
                    
                    lifted_lambda_res_19300 = lifted_lambda_res_t_res_21595;
                } else {
                    lifted_lambda_res_19300 = (int64_t) -1;
                }
                // graph_orientation_futhark.fut:294:18-297:16
                // UpdateAcc
                if (sle64((int64_t) 0, lifted_lambda_res_19300) && slt64(lifted_lambda_res_19300, m_17831)) {
                    ((int64_t *) mem_22026)[lifted_lambda_res_19300] = i_21795;
                }
            }
            // graph_orientation_futhark.fut:294:5-297:17
            if (memblock_alloc(ctx, &mem_22028, bytes_22027, "mem_22028")) {
                err = 1;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:294:5-297:17
            for (int64_t i_21798 = 0; i_21798 < m_17831; i_21798++) {
                int64_t eta_p_17848 = ((int64_t *) mem_22026)[i_21798];
                
                // graph_orientation_futhark.fut:294:9-16
                
                int32_t i64_res_17849 = sext_i64_i32(eta_p_17848);
                
                ((int32_t *) mem_22028.mem)[i_21798] = i64_res_17849;
            }
            // graph_orientation_futhark.fut:307:22-310:20
            if (mem_22150_cached_sizze_22900 < binop_x_21929) {
                err = lexical_realloc(ctx, &mem_22150, &mem_22150_cached_sizze_22900, binop_x_21929);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:307:22-310:20
            if (mem_22152_cached_sizze_22901 < binop_x_21929) {
                err = lexical_realloc(ctx, &mem_22152, &mem_22152_cached_sizze_22901, binop_x_21929);
                if (err != FUTHARK_SUCCESS)
                    goto cleanup;
            }
            // graph_orientation_futhark.fut:300:5-313:28
            
            int64_t ext_22187;
            int64_t ext_22186;
            int64_t ext_22185;
            int64_t ext_22184;
            int64_t ext_22183;
            int64_t ext_22182;
            int64_t ext_22181;
            int64_t push_relabel_res_17854;
            bool push_relabel_res_17855;
            int64_t loop_dz2088Uz2084U_17860;
            bool loop_while_17861;
            int64_t ctx_param_ext_22038;
            int64_t ctx_param_ext_22039;
            int64_t ctx_param_ext_22040;
            int64_t ctx_param_ext_22042;
            int64_t ctx_param_ext_22043;
            int64_t ctx_param_ext_22045;
            int64_t ctx_param_ext_22046;
            
            if (memblock_set(ctx, &mem_param_22037, &mem_22028, "mem_22028") != 0)
                return 1;
            if (memblock_set(ctx, &mem_param_22041, &ext_mem_22008, "ext_mem_22008") != 0)
                return 1;
            if (memblock_set(ctx, &mem_param_22044, &ext_mem_21997, "ext_mem_21997") != 0)
                return 1;
            if (memblock_set(ctx, &mem_param_22047, &mem_21961, "mem_21961") != 0)
                return 1;
            ctx_param_ext_22038 = (int64_t) 0;
            ctx_param_ext_22039 = i32_res_17111;
            ctx_param_ext_22040 = (int64_t) 1;
            ctx_param_ext_22042 = (int64_t) 0;
            ctx_param_ext_22043 = (int64_t) 1;
            ctx_param_ext_22045 = (int64_t) 0;
            ctx_param_ext_22046 = (int64_t) 1;
            loop_dz2088Uz2084U_17860 = m_17831;
            loop_while_17861 = loop_cond_17850;
            while (loop_while_17861) {
                // graph_orientation_futhark.fut:303:15-21
                
                bool y_17866 = slt64((int64_t) 0, loop_dz2088Uz2084U_17860);
                
                // graph_orientation_futhark.fut:303:15-21
                
                bool index_certs_17867;
                
                if (!y_17866) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, "] out of bounds for array of shape [", (long long) loop_dz2088Uz2084U_17860, "].", "-> #0  graph_orientation_futhark.fut:303:15-21\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:303:15-21
                
                int32_t head_res_17868 = ((int32_t *) mem_param_22037.mem)[(int64_t) 0];
                
                // graph_orientation_futhark.fut:304:18-24
                
                int64_t j_m_i_17869 = sub64(loop_dz2088Uz2084U_17860, (int64_t) 1);
                
                // graph_orientation_futhark.fut:304:18-24
                
                bool empty_slice_17870 = j_m_i_17869 == (int64_t) 0;
                
                // graph_orientation_futhark.fut:304:18-24
                
                bool zzero_leq_i_p_m_t_s_17871 = sle64((int64_t) 0, j_m_i_17869);
                
                // graph_orientation_futhark.fut:304:18-24
                
                bool i_p_m_t_s_leq_w_17872 = slt64(j_m_i_17869, loop_dz2088Uz2084U_17860);
                
                // graph_orientation_futhark.fut:304:18-24
                
                bool i_lte_j_17873 = sle64((int64_t) 1, loop_dz2088Uz2084U_17860);
                
                // graph_orientation_futhark.fut:304:18-24
                
                bool y_17874 = zzero_leq_i_p_m_t_s_17871 && i_p_m_t_s_leq_w_17872;
                
                // graph_orientation_futhark.fut:304:18-24
                
                bool forwards_ok_17875 = i_lte_j_17873 && y_17874;
                
                // graph_orientation_futhark.fut:304:18-24
                
                bool ok_or_empty_17876 = empty_slice_17870 || forwards_ok_17875;
                
                // graph_orientation_futhark.fut:304:18-24
                
                bool index_certs_17877;
                
                if (!ok_or_empty_17876) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 1, ":] out of bounds for array of shape [", (long long) loop_dz2088Uz2084U_17860, "].", "-> #0  graph_orientation_futhark.fut:304:18-24\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:253:10-15
                
                int64_t u_17879 = sext_i32_i64(head_res_17868);
                
                // graph_orientation_futhark.fut:253:10-15
                
                bool x_17880 = sle64((int64_t) 0, u_17879);
                
                // graph_orientation_futhark.fut:253:10-15
                
                bool y_17881 = slt64(u_17879, i32_res_17111);
                
                // graph_orientation_futhark.fut:253:10-15
                
                bool bounds_check_17882 = x_17880 && y_17881;
                
                // graph_orientation_futhark.fut:253:10-15
                
                bool index_certs_17883;
                
                if (!bounds_check_17882) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_17879, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:253:10-15\n   #1  graph_orientation_futhark.fut:305:28-51\n   #2  graph_orientation_futhark.fut:357:10-30\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:305:28-51
                
                bool protect_assert_disj_17899 = bounds_check_17882 || loop_not_taken_17898;
                
                // graph_orientation_futhark.fut:250:5-267:38
                
                int64_t ext_22145;
                int64_t ext_22144;
                int64_t ext_22143;
                int64_t ext_22142;
                int64_t ext_22141;
                int64_t ext_22140;
                int64_t ext_22139;
                bool discharge_res_17884;
                bool loop_while_17888;
                int64_t ctx_param_ext_22048;
                int64_t ctx_param_ext_22049;
                int64_t ctx_param_ext_22050;
                int64_t ctx_param_ext_22052;
                int64_t ctx_param_ext_22053;
                int64_t ctx_param_ext_22055;
                int64_t ctx_param_ext_22056;
                
                if (memblock_set(ctx, &mem_param_22051, &mem_param_22041, "mem_param_22041") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22054, &mem_param_22044, "mem_param_22044") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22057, &mem_param_22047, "mem_param_22047") != 0)
                    return 1;
                ctx_param_ext_22048 = ctx_param_ext_22038;
                ctx_param_ext_22049 = ctx_param_ext_22039;
                ctx_param_ext_22050 = ctx_param_ext_22040;
                ctx_param_ext_22052 = ctx_param_ext_22042;
                ctx_param_ext_22053 = ctx_param_ext_22043;
                ctx_param_ext_22055 = ctx_param_ext_22045;
                ctx_param_ext_22056 = ctx_param_ext_22046;
                loop_while_17888 = 1;
                while (loop_while_17888) {
                    // graph_orientation_futhark.fut:253:10-15
                    
                    int64_t zeze_lhs_17892 = ((int64_t *) mem_param_22054.mem)[ctx_param_ext_22052 + u_17879 * ctx_param_ext_22053];
                    
                    // graph_orientation_futhark.fut:253:7-267:38
                    
                    bool cond_17893 = zeze_lhs_17892 == (int64_t) 0;
                    bool protect_assert_disj_22618 = cond_17893 || protect_assert_disj_17899;
                    
                    // graph_orientation_futhark.fut:258:16-22
                    
                    bool index_certs_17900;
                    
                    if (!protect_assert_disj_22618) {
                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_17879, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:258:16-22\n   #1  graph_orientation_futhark.fut:305:28-51\n   #2  graph_orientation_futhark.fut:357:10-30\n"));
                        err = FUTHARK_PROGRAM_ERROR;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:305:28-51
                    
                    int64_t ext_22137;
                    int64_t ext_22136;
                    int64_t ext_22135;
                    int64_t ext_22133;
                    int64_t ext_22132;
                    int64_t ext_22130;
                    int64_t ext_22129;
                    
                    if (cond_17893) {
                        if (memblock_set(ctx, &ext_mem_22138, &mem_param_22051, "mem_param_22051") != 0)
                            return 1;
                        ext_22137 = ctx_param_ext_22048;
                        ext_22136 = ctx_param_ext_22049;
                        ext_22135 = ctx_param_ext_22050;
                        if (memblock_set(ctx, &ext_mem_22134, &mem_param_22054, "mem_param_22054") != 0)
                            return 1;
                        ext_22133 = ctx_param_ext_22052;
                        ext_22132 = ctx_param_ext_22053;
                        if (memblock_set(ctx, &ext_mem_22131, &mem_param_22057, "mem_param_22057") != 0)
                            return 1;
                        ext_22130 = ctx_param_ext_22055;
                        ext_22129 = ctx_param_ext_22056;
                    } else {
                        // graph_orientation_futhark.fut:255:11-262:42
                        
                        int64_t ext_22095;
                        int64_t ext_22094;
                        int64_t ext_22093;
                        int64_t ext_22092;
                        int64_t ext_22091;
                        bool loopres_f_res_17902;
                        int32_t loopres_f_res_17903;
                        bool loopres_f_res_17904;
                        bool loop_while_17907;
                        int32_t v_17908;
                        bool pushed_17909;
                        int64_t ctx_param_ext_22058;
                        int64_t ctx_param_ext_22059;
                        int64_t ctx_param_ext_22060;
                        int64_t ctx_param_ext_22062;
                        int64_t ctx_param_ext_22063;
                        
                        if (memblock_set(ctx, &mem_param_22061, &mem_param_22051, "mem_param_22051") != 0)
                            return 1;
                        if (memblock_set(ctx, &mem_param_22064, &mem_param_22054, "mem_param_22054") != 0)
                            return 1;
                        ctx_param_ext_22058 = ctx_param_ext_22048;
                        ctx_param_ext_22059 = ctx_param_ext_22049;
                        ctx_param_ext_22060 = ctx_param_ext_22050;
                        ctx_param_ext_22062 = ctx_param_ext_22052;
                        ctx_param_ext_22063 = ctx_param_ext_22053;
                        loop_while_17907 = cond_17897;
                        v_17908 = 0;
                        pushed_17909 = 0;
                        while (loop_while_17907) {
                            // graph_orientation_futhark.fut:258:16-25
                            
                            int64_t v_17912 = sext_i32_i64(v_17908);
                            
                            // graph_orientation_futhark.fut:258:16-25
                            
                            bool x_17913 = sle64((int64_t) 0, v_17912);
                            
                            // graph_orientation_futhark.fut:258:16-25
                            
                            bool y_17914 = slt64(v_17912, i32_res_17111);
                            
                            // graph_orientation_futhark.fut:258:16-25
                            
                            bool bounds_check_17915 = x_17913 && y_17914;
                            
                            // graph_orientation_futhark.fut:258:16-25
                            
                            bool index_certs_17916;
                            
                            if (!bounds_check_17915) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_17912, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:258:16-25\n   #1  graph_orientation_futhark.fut:305:28-51\n   #2  graph_orientation_futhark.fut:357:10-30\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:305:28-51
                            
                            int64_t zm_lhs_17917 = ((int64_t *) ext_mem_21959.mem)[u_17879 * i32_res_17111 + v_17912];
                            
                            // graph_orientation_futhark.fut:305:28-51
                            
                            int64_t zm_rhs_17921 = ((int64_t *) mem_param_22061.mem)[ctx_param_ext_22058 + (u_17879 * ctx_param_ext_22059 + v_17912 * ctx_param_ext_22060)];
                            
                            // graph_orientation_futhark.fut:258:26-37
                            
                            int64_t zg_lhs_17922 = sub64(zm_lhs_17917, zm_rhs_17921);
                            
                            // graph_orientation_futhark.fut:258:13-262:42
                            
                            bool cond_17923 = slt64((int64_t) 0, zg_lhs_17922);
                            
                            // graph_orientation_futhark.fut:258:13-262:42
                            
                            bool cond_17924;
                            
                            if (cond_17923) {
                                // graph_orientation_futhark.fut:258:48-52
                                
                                int32_t zeze_lhs_21598 = ((int32_t *) mem_param_22057.mem)[ctx_param_ext_22055 + u_17879 * ctx_param_ext_22056];
                                
                                // graph_orientation_futhark.fut:258:56-60
                                
                                int32_t zp_lhs_21599 = ((int32_t *) mem_param_22057.mem)[ctx_param_ext_22055 + v_17912 * ctx_param_ext_22056];
                                
                                // graph_orientation_futhark.fut:258:61-67
                                
                                int32_t zeze_rhs_21600 = add32(1, zp_lhs_21599);
                                
                                // graph_orientation_futhark.fut:258:53-67
                                
                                bool cond_t_res_21601 = zeze_lhs_21598 == zeze_rhs_21600;
                                
                                cond_17924 = cond_t_res_21601;
                            } else {
                                cond_17924 = 0;
                            }
                            // graph_orientation_futhark.fut:231:21-27
                            
                            int64_t min_arg0_21625;
                            
                            if (cond_17924) {
                                int64_t x_22607 = ((int64_t *) mem_param_22064.mem)[ctx_param_ext_22062 + u_17879 * ctx_param_ext_22063];
                                
                                min_arg0_21625 = x_22607;
                            } else {
                                min_arg0_21625 = (int64_t) 0;
                            }
                            // graph_orientation_futhark.fut:231:13-50
                            
                            int64_t min_res_21626 = smin64(zg_lhs_17922, min_arg0_21625);
                            
                            // graph_orientation_futhark.fut:232:6-238:20
                            
                            bool cond_21627 = slt64((int64_t) 0, min_res_21626);
                            bool protect_cond_conj_22609 = cond_17924 && cond_21627;
                            
                            // graph_orientation_futhark.fut:232:23-28
                            
                            int32_t zeze_lhs_21629;
                            
                            if (protect_cond_conj_22609) {
                                int32_t x_22601 = ((int32_t *) mem_param_22057.mem)[ctx_param_ext_22055 + u_17879 * ctx_param_ext_22056];
                                
                                zeze_lhs_21629 = x_22601;
                            } else {
                                zeze_lhs_21629 = 0;
                            }
                            // graph_orientation_futhark.fut:232:32-37
                            
                            int32_t zp_lhs_21630;
                            
                            if (protect_cond_conj_22609) {
                                int32_t x_22603 = ((int32_t *) mem_param_22057.mem)[ctx_param_ext_22055 + v_17912 * ctx_param_ext_22056];
                                
                                zp_lhs_21630 = x_22603;
                            } else {
                                zp_lhs_21630 = 0;
                            }
                            // graph_orientation_futhark.fut:232:38-44
                            
                            int32_t zeze_rhs_21631 = add32(1, zp_lhs_21630);
                            
                            // graph_orientation_futhark.fut:232:29-44
                            
                            bool cond_t_res_21632 = zeze_lhs_21629 == zeze_rhs_21631;
                            bool x_22651 = cond_21627 && cond_t_res_21632;
                            
                            // graph_orientation_futhark.fut:233:18-56
                            
                            bool index_ok_21637 = bounds_check_17882 && bounds_check_17915;
                            bool loop_not_taken_22595 = !x_22651;
                            bool protect_assert_disj_22596 = index_ok_21637 || loop_not_taken_22595;
                            bool loop_not_taken_22611 = !cond_17924;
                            bool protect_assert_disj_22612 = protect_assert_disj_22596 || loop_not_taken_22611;
                            
                            // graph_orientation_futhark.fut:233:18-56
                            
                            bool index_certs_21638;
                            
                            if (!protect_assert_disj_22612) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_17879, ", ", (long long) v_17912, "] out of bounds for array of shape [", (long long) i32_res_17111, "][", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:233:18-56\n   #1  graph_orientation_futhark.fut:259:32-54\n   #2  graph_orientation_futhark.fut:305:28-51\n   #3  graph_orientation_futhark.fut:357:10-30\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:234:42-51
                            
                            bool index_certs_21640;
                            
                            if (!protect_assert_disj_22612) {
                                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_17912, ", ", (long long) u_17879, "] out of bounds for array of shape [", (long long) i32_res_17111, "][", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:234:42-51\n   #1  graph_orientation_futhark.fut:259:32-54\n   #2  graph_orientation_futhark.fut:305:28-51\n   #3  graph_orientation_futhark.fut:357:10-30\n"));
                                err = FUTHARK_PROGRAM_ERROR;
                                goto cleanup;
                            }
                            
                            int64_t ext_22075;
                            
                            if (x_22651) {
                                ext_22075 = (int64_t) 0;
                            } else {
                                ext_22075 = ctx_param_ext_22058;
                            }
                            
                            int64_t ext_22074;
                            
                            if (x_22651) {
                                ext_22074 = i32_res_17111;
                            } else {
                                ext_22074 = ctx_param_ext_22059;
                            }
                            
                            int64_t ext_22073;
                            
                            if (x_22651) {
                                ext_22073 = (int64_t) 1;
                            } else {
                                ext_22073 = ctx_param_ext_22060;
                            }
                            
                            int64_t ext_22071;
                            
                            if (x_22651) {
                                ext_22071 = (int64_t) 0;
                            } else {
                                ext_22071 = ctx_param_ext_22062;
                            }
                            
                            int64_t ext_22070;
                            
                            if (x_22651) {
                                ext_22070 = (int64_t) 1;
                            } else {
                                ext_22070 = ctx_param_ext_22063;
                            }
                            // graph_orientation_futhark.fut:233:50-55
                            
                            int64_t fl1_21635 = add64(zm_rhs_17921, min_res_21626);
                            
                            // graph_orientation_futhark.fut:235:46-51
                            
                            int64_t ex1_21644 = sub64(min_arg0_21625, min_res_21626);
                            
                            // graph_orientation_futhark.fut:258:13-262:42
                            
                            int64_t ext_22089;
                            int64_t ext_22088;
                            int64_t ext_22087;
                            int64_t ext_22085;
                            int64_t ext_22084;
                            int32_t loopres_17930;
                            bool loopres_17931;
                            
                            if (cond_17924) {
                                if (x_22651) {
                                    // graph_orientation_futhark.fut:233:18-25
                                    if (memblock_alloc(ctx, &mem_22067, bytes_21930, "mem_22067")) {
                                        err = 1;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:233:18-25
                                    // graph_orientation_futhark.fut:233:18-25
                                    lmad_copy_8b(ctx, 2, (uint64_t *) mem_22067.mem, (int64_t) 0, (int64_t []) {i32_res_17111, (int64_t) 1}, (uint64_t *) mem_param_22061.mem, ctx_param_ext_22058, (int64_t []) {ctx_param_ext_22059, ctx_param_ext_22060}, (int64_t []) {i32_res_17111, i32_res_17111});
                                    // graph_orientation_futhark.fut:233:18-56
                                    ((int64_t *) mem_22067.mem)[u_17879 * i32_res_17111 + v_17912] = fl1_21635;
                                    // graph_orientation_futhark.fut:234:42-51
                                    
                                    int64_t zm_lhs_21641 = ((int64_t *) mem_22067.mem)[v_17912 * i32_res_17111 + u_17879];
                                    
                                    // graph_orientation_futhark.fut:234:52-57
                                    
                                    int64_t fl2_21642 = sub64(zm_lhs_21641, min_res_21626);
                                    
                                    // graph_orientation_futhark.fut:234:18-58
                                    ((int64_t *) mem_22067.mem)[v_17912 * i32_res_17111 + u_17879] = fl2_21642;
                                    // graph_orientation_futhark.fut:235:18-26
                                    if (memblock_alloc(ctx, &mem_22069, binop_x_21929, "mem_22069")) {
                                        err = 1;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:235:18-26
                                    // graph_orientation_futhark.fut:235:18-26
                                    lmad_copy_8b(ctx, 1, (uint64_t *) mem_22069.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint64_t *) mem_param_22064.mem, ctx_param_ext_22062, (int64_t []) {ctx_param_ext_22063}, (int64_t []) {i32_res_17111});
                                    // graph_orientation_futhark.fut:235:18-52
                                    ((int64_t *) mem_22069.mem)[u_17879] = ex1_21644;
                                    // graph_orientation_futhark.fut:236:39-45
                                    
                                    int64_t zp_lhs_21647 = ((int64_t *) mem_22069.mem)[v_17912];
                                    
                                    // graph_orientation_futhark.fut:236:46-51
                                    
                                    int64_t ex2_21648 = add64(min_res_21626, zp_lhs_21647);
                                    
                                    // graph_orientation_futhark.fut:236:18-52
                                    ((int64_t *) mem_22069.mem)[v_17912] = ex2_21648;
                                    if (memblock_set(ctx, &ext_mem_22076, &mem_22067, "mem_22067") != 0)
                                        return 1;
                                    if (memblock_set(ctx, &ext_mem_22072, &mem_22069, "mem_22069") != 0)
                                        return 1;
                                } else {
                                    if (memblock_set(ctx, &ext_mem_22076, &mem_param_22061, "mem_param_22061") != 0)
                                        return 1;
                                    if (memblock_set(ctx, &ext_mem_22072, &mem_param_22064, "mem_param_22064") != 0)
                                        return 1;
                                }
                                // graph_orientation_futhark.fut:305:28-51
                                
                                bool all_equal_21652;
                                bool redout_21800 = 1;
                                
                                for (int64_t i_21801 = 0; i_21801 < x_num_elems_17901; i_21801++) {
                                    int64_t new_index_21910 = squot64(i_21801, i32_res_17111);
                                    int64_t binop_y_21912 = i32_res_17111 * new_index_21910;
                                    int64_t new_index_21913 = i_21801 - binop_y_21912;
                                    int64_t x_21653 = ((int64_t *) ext_mem_22076.mem)[ext_22075 + (new_index_21910 * ext_22074 + new_index_21913 * ext_22073)];
                                    int64_t y_21654 = ((int64_t *) mem_param_22061.mem)[ctx_param_ext_22058 + (new_index_21910 * ctx_param_ext_22059 + new_index_21913 * ctx_param_ext_22060)];
                                    
                                    // graph_orientation_futhark.fut:305:28-51
                                    
                                    bool binlam_res_21655 = x_21653 == y_21654;
                                    
                                    // graph_orientation_futhark.fut:305:28-51
                                    
                                    bool binlam_res_21658 = binlam_res_21655 && redout_21800;
                                    bool redout_tmp_22840 = binlam_res_21658;
                                    
                                    redout_21800 = redout_tmp_22840;
                                }
                                all_equal_21652 = redout_21800;
                                // graph_orientation_futhark.fut:260:18-261:47
                                
                                bool cond_21667 = !all_equal_21652;
                                int64_t ext_22082;
                                
                                if (cond_21667) {
                                    ext_22082 = ext_22075;
                                } else {
                                    ext_22082 = ctx_param_ext_22058;
                                }
                                
                                int64_t ext_22081;
                                
                                if (cond_21667) {
                                    ext_22081 = ext_22074;
                                } else {
                                    ext_22081 = ctx_param_ext_22059;
                                }
                                
                                int64_t ext_22080;
                                
                                if (cond_21667) {
                                    ext_22080 = ext_22073;
                                } else {
                                    ext_22080 = ctx_param_ext_22060;
                                }
                                
                                int64_t ext_22078;
                                
                                if (cond_21667) {
                                    ext_22078 = ext_22071;
                                } else {
                                    ext_22078 = ctx_param_ext_22062;
                                }
                                
                                int64_t ext_22077;
                                
                                if (cond_21667) {
                                    ext_22077 = ext_22070;
                                } else {
                                    ext_22077 = ctx_param_ext_22063;
                                }
                                
                                int32_t loopres_t_res_21668;
                                
                                if (cond_21667) {
                                    // graph_orientation_futhark.fut:260:40-43
                                    
                                    int32_t tmp_21671 = add32(1, v_17908);
                                    
                                    if (memblock_set(ctx, &ext_mem_22083, &ext_mem_22076, "ext_mem_22076") != 0)
                                        return 1;
                                    if (memblock_set(ctx, &ext_mem_22079, &ext_mem_22072, "ext_mem_22072") != 0)
                                        return 1;
                                    loopres_t_res_21668 = tmp_21671;
                                } else {
                                    // graph_orientation_futhark.fut:261:26-29
                                    
                                    int32_t tmp_21672 = add32(1, v_17908);
                                    
                                    if (memblock_set(ctx, &ext_mem_22083, &mem_param_22061, "mem_param_22061") != 0)
                                        return 1;
                                    if (memblock_set(ctx, &ext_mem_22079, &mem_param_22064, "mem_param_22064") != 0)
                                        return 1;
                                    loopres_t_res_21668 = tmp_21672;
                                }
                                if (memblock_unref(ctx, &ext_mem_22072, "ext_mem_22072") != 0)
                                    return 1;
                                if (memblock_unref(ctx, &ext_mem_22076, "ext_mem_22076") != 0)
                                    return 1;
                                if (memblock_set(ctx, &ext_mem_22090, &ext_mem_22083, "ext_mem_22083") != 0)
                                    return 1;
                                ext_22089 = ext_22082;
                                ext_22088 = ext_22081;
                                ext_22087 = ext_22080;
                                if (memblock_set(ctx, &ext_mem_22086, &ext_mem_22079, "ext_mem_22079") != 0)
                                    return 1;
                                ext_22085 = ext_22078;
                                ext_22084 = ext_22077;
                                loopres_17930 = loopres_t_res_21668;
                                loopres_17931 = cond_21667;
                            } else {
                                // graph_orientation_futhark.fut:262:21-24
                                
                                int32_t tmp_17977 = add32(1, v_17908);
                                
                                if (memblock_set(ctx, &ext_mem_22090, &mem_param_22061, "mem_param_22061") != 0)
                                    return 1;
                                ext_22089 = ctx_param_ext_22058;
                                ext_22088 = ctx_param_ext_22059;
                                ext_22087 = ctx_param_ext_22060;
                                if (memblock_set(ctx, &ext_mem_22086, &mem_param_22064, "mem_param_22064") != 0)
                                    return 1;
                                ext_22085 = ctx_param_ext_22062;
                                ext_22084 = ctx_param_ext_22063;
                                loopres_17930 = tmp_17977;
                                loopres_17931 = 0;
                            }
                            // graph_orientation_futhark.fut:256:19-38
                            
                            bool cond_17978 = slt32(loopres_17930, node_count_res_17110);
                            
                            // graph_orientation_futhark.fut:256:28-38
                            
                            bool not_res_17979 = !loopres_17931;
                            
                            // graph_orientation_futhark.fut:357:10-30
                            
                            bool x_17980 = cond_17978 && not_res_17979;
                            
                            if (memblock_set(ctx, &mem_param_tmp_22828, &ext_mem_22090, "ext_mem_22090") != 0)
                                return 1;
                            if (memblock_set(ctx, &mem_param_tmp_22829, &ext_mem_22086, "ext_mem_22086") != 0)
                                return 1;
                            
                            int64_t ctx_param_ext_tmp_22830 = ext_22089;
                            int64_t ctx_param_ext_tmp_22831 = ext_22088;
                            int64_t ctx_param_ext_tmp_22832 = ext_22087;
                            int64_t ctx_param_ext_tmp_22833 = ext_22085;
                            int64_t ctx_param_ext_tmp_22834 = ext_22084;
                            bool loop_while_tmp_22835 = x_17980;
                            int32_t v_tmp_22836 = loopres_17930;
                            bool pushed_tmp_22837 = loopres_17931;
                            
                            if (memblock_set(ctx, &mem_param_22061, &mem_param_tmp_22828, "mem_param_tmp_22828") != 0)
                                return 1;
                            if (memblock_set(ctx, &mem_param_22064, &mem_param_tmp_22829, "mem_param_tmp_22829") != 0)
                                return 1;
                            ctx_param_ext_22058 = ctx_param_ext_tmp_22830;
                            ctx_param_ext_22059 = ctx_param_ext_tmp_22831;
                            ctx_param_ext_22060 = ctx_param_ext_tmp_22832;
                            ctx_param_ext_22062 = ctx_param_ext_tmp_22833;
                            ctx_param_ext_22063 = ctx_param_ext_tmp_22834;
                            loop_while_17907 = loop_while_tmp_22835;
                            v_17908 = v_tmp_22836;
                            pushed_17909 = pushed_tmp_22837;
                        }
                        if (memblock_set(ctx, &ext_mem_22097, &mem_param_22061, "mem_param_22061") != 0)
                            return 1;
                        if (memblock_set(ctx, &ext_mem_22096, &mem_param_22064, "mem_param_22064") != 0)
                            return 1;
                        ext_22095 = ctx_param_ext_22058;
                        ext_22094 = ctx_param_ext_22059;
                        ext_22093 = ctx_param_ext_22060;
                        ext_22092 = ctx_param_ext_22062;
                        ext_22091 = ctx_param_ext_22063;
                        loopres_f_res_17902 = loop_while_17907;
                        loopres_f_res_17903 = v_17908;
                        loopres_f_res_17904 = pushed_17909;
                        // graph_orientation_futhark.fut:357:10-30
                        
                        bool protect_assert_disj_17981 = bounds_check_17882 || loopres_f_res_17904;
                        
                        // graph_orientation_futhark.fut:243:41-48
                        
                        bool index_certs_17982;
                        
                        if (!protect_assert_disj_17981) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_17879, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:243:41-48\n   #1  graph_orientation_futhark.fut:265:23-42\n   #2  graph_orientation_futhark.fut:305:28-51\n   #3  graph_orientation_futhark.fut:357:10-30\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:357:10-30
                        
                        bool protect_assert_disj_17983 = protect_assert_disj_17741 || loopres_f_res_17904;
                        
                        // graph_orientation_futhark.fut:243:18-100
                        
                        bool index_certs_17984;
                        
                        if (!protect_assert_disj_17983) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) tmp_17737, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:243:18-100\n   #1  graph_orientation_futhark.fut:265:23-42\n   #2  graph_orientation_futhark.fut:305:28-51\n   #3  graph_orientation_futhark.fut:357:10-30\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        
                        int64_t ext_22127;
                        
                        if (loopres_f_res_17904) {
                            ext_22127 = ctx_param_ext_22055;
                        } else {
                            ext_22127 = (int64_t) 0;
                        }
                        
                        int64_t ext_22126;
                        
                        if (loopres_f_res_17904) {
                            ext_22126 = ctx_param_ext_22056;
                        } else {
                            ext_22126 = (int64_t) 1;
                        }
                        if (loopres_f_res_17904) {
                            if (memblock_set(ctx, &ext_mem_22128, &mem_param_22057, "mem_param_22057") != 0)
                                return 1;
                        } else {
                            // graph_orientation_futhark.fut:243:18-100
                            if (mem_22099_cached_sizze_22896 < binop_x_21929) {
                                err = lexical_realloc(ctx, &mem_22099, &mem_22099_cached_sizze_22896, binop_x_21929);
                                if (err != FUTHARK_SUCCESS)
                                    goto cleanup;
                            }
                            // graph_orientation_futhark.fut:243:18-100
                            if (mem_22101_cached_sizze_22897 < binop_x_21929) {
                                err = lexical_realloc(ctx, &mem_22101, &mem_22101_cached_sizze_22897, binop_x_21929);
                                if (err != FUTHARK_SUCCESS)
                                    goto cleanup;
                            }
                            // graph_orientation_futhark.fut:243:18-100
                            
                            int64_t inpacc_21706;
                            int64_t inpacc_19674 = (int64_t) 0;
                            
                            for (int64_t i_21822 = 0; i_21822 < i32_res_17111; i_21822++) {
                                // graph_orientation_futhark.fut:243:49-60
                                
                                int32_t i64_res_21881 = sext_i64_i32(i_21822);
                                
                                // graph_orientation_futhark.fut:243:41-61
                                
                                int64_t zm_lhs_21882 = sext_i32_i64(i64_res_21881);
                                
                                // graph_orientation_futhark.fut:243:41-61
                                
                                bool x_21883 = sle64((int64_t) 0, zm_lhs_21882);
                                
                                // graph_orientation_futhark.fut:243:41-61
                                
                                bool y_21884 = slt64(zm_lhs_21882, i32_res_17111);
                                
                                // graph_orientation_futhark.fut:243:41-61
                                
                                bool bounds_check_21885 = x_21883 && y_21884;
                                
                                // graph_orientation_futhark.fut:243:41-61
                                
                                bool index_certs_21886;
                                
                                if (!bounds_check_21885) {
                                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) zm_lhs_21882, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:243:41-61\n   #1  graph_orientation_futhark.fut:265:23-42\n   #2  graph_orientation_futhark.fut:305:28-51\n   #3  graph_orientation_futhark.fut:357:10-30\n"));
                                    err = FUTHARK_PROGRAM_ERROR;
                                    goto cleanup;
                                }
                                // graph_orientation_futhark.fut:243:18-100
                                
                                int64_t zm_lhs_21887 = ((int64_t *) ext_mem_21959.mem)[u_17879 * i32_res_17111 + zm_lhs_21882];
                                
                                // graph_orientation_futhark.fut:243:18-100
                                
                                int64_t zm_rhs_21888 = ((int64_t *) ext_mem_22097.mem)[ext_22095 + (u_17879 * ext_22094 + zm_lhs_21882 * ext_22093)];
                                
                                // graph_orientation_futhark.fut:243:62-82
                                
                                int64_t zg_lhs_21889 = sub64(zm_lhs_21887, zm_rhs_21888);
                                
                                // graph_orientation_futhark.fut:243:83-89
                                
                                bool lifted_lambda_res_21890 = slt64((int64_t) 0, zg_lhs_21889);
                                
                                // graph_orientation_futhark.fut:243:18-100
                                
                                int64_t defunc_0_f_res_21891 = btoi_bool_i64(lifted_lambda_res_21890);
                                
                                // graph_orientation_futhark.fut:243:18-100
                                
                                int64_t defunc_0_op_res_21902 = add64(inpacc_19674, defunc_0_f_res_21891);
                                
                                // graph_orientation_futhark.fut:243:18-100
                                
                                bool cond_21903 = defunc_0_f_res_21891 == (int64_t) 1;
                                
                                // graph_orientation_futhark.fut:243:18-100
                                
                                int64_t lifted_lambda_res_21904;
                                
                                if (cond_21903) {
                                    // graph_orientation_futhark.fut:243:18-100
                                    
                                    int64_t lifted_lambda_res_t_res_21905 = sub64(defunc_0_op_res_21902, (int64_t) 1);
                                    
                                    lifted_lambda_res_21904 = lifted_lambda_res_t_res_21905;
                                } else {
                                    lifted_lambda_res_21904 = (int64_t) -1;
                                }
                                ((int64_t *) mem_22099)[i_21822] = lifted_lambda_res_21904;
                                ((int64_t *) mem_22101)[i_21822] = defunc_0_op_res_21902;
                                
                                int64_t inpacc_tmp_22841 = defunc_0_op_res_21902;
                                
                                inpacc_19674 = inpacc_tmp_22841;
                            }
                            inpacc_21706 = inpacc_19674;
                            // graph_orientation_futhark.fut:243:18-100
                            
                            int64_t m_f_res_18007;
                            
                            if (x_17736) {
                                // graph_orientation_futhark.fut:243:18-100
                                
                                int64_t x_21678 = ((int64_t *) mem_22101)[tmp_17737];
                                
                                m_f_res_18007 = x_21678;
                            } else {
                                m_f_res_18007 = (int64_t) 0;
                            }
                            // graph_orientation_futhark.fut:243:18-100
                            
                            int64_t m_18009;
                            
                            if (cond_17735) {
                                m_18009 = (int64_t) 0;
                            } else {
                                m_18009 = m_f_res_18007;
                            }
                            // graph_orientation_futhark.fut:243:18-100
                            
                            int64_t bytes_22114 = (int64_t) 8 * m_18009;
                            
                            // graph_orientation_futhark.fut:243:18-100
                            if (mem_22115_cached_sizze_22898 < bytes_22114) {
                                err = lexical_realloc(ctx, &mem_22115, &mem_22115_cached_sizze_22898, bytes_22114);
                                if (err != FUTHARK_SUCCESS)
                                    goto cleanup;
                            }
                            // graph_orientation_futhark.fut:357:10-30
                            
                            bool eq_x_zz_18028 = (int64_t) 0 == m_f_res_18007;
                            
                            // graph_orientation_futhark.fut:357:10-30
                            
                            bool p_and_eq_x_y_18029 = x_17736 && eq_x_zz_18028;
                            
                            // graph_orientation_futhark.fut:357:10-30
                            
                            bool cond_18030 = cond_17735 || p_and_eq_x_y_18029;
                            
                            // graph_orientation_futhark.fut:243:5-101
                            
                            int64_t bytes_22116 = (int64_t) 4 * m_18009;
                            
                            // graph_orientation_futhark.fut:244:6-245:78
                            
                            int32_t relabel_res_18031;
                            
                            if (cond_18030) {
                                // graph_orientation_futhark.fut:244:31-36
                                
                                int32_t relabel_res_t_res_21702 = ((int32_t *) mem_param_22057.mem)[ctx_param_ext_22055 + u_17879 * ctx_param_ext_22056];
                                
                                relabel_res_18031 = relabel_res_t_res_21702;
                            } else {
                                // graph_orientation_futhark.fut:243:18-100
                                
                                bool acc_cert_18018;
                                
                                // graph_orientation_futhark.fut:243:18-100
                                for (int64_t i_21826 = 0; i_21826 < i32_res_17111; i_21826++) {
                                    int64_t v_18022 = ((int64_t *) mem_22099)[i_21826];
                                    
                                    // graph_orientation_futhark.fut:243:18-100
                                    // UpdateAcc
                                    if (sle64((int64_t) 0, v_18022) && slt64(v_18022, m_18009)) {
                                        ((int64_t *) mem_22115)[v_18022] = i_21826;
                                    }
                                }
                                // graph_orientation_futhark.fut:243:5-101
                                if (mem_22117_cached_sizze_22899 < bytes_22116) {
                                    err = lexical_realloc(ctx, &mem_22117, &mem_22117_cached_sizze_22899, bytes_22116);
                                    if (err != FUTHARK_SUCCESS)
                                        goto cleanup;
                                }
                                // graph_orientation_futhark.fut:243:5-101
                                for (int64_t i_21829 = 0; i_21829 < m_18009; i_21829++) {
                                    int64_t eta_p_18026 = ((int64_t *) mem_22115)[i_21829];
                                    
                                    // graph_orientation_futhark.fut:243:9-16
                                    
                                    int32_t i64_res_18027 = sext_i64_i32(eta_p_18026);
                                    
                                    ((int32_t *) mem_22117)[i_21829] = i64_res_18027;
                                }
                                // graph_orientation_futhark.fut:245:18-77
                                
                                int32_t defunc_0_reduce_res_21705;
                                int32_t redout_21831 = 2147483647;
                                
                                for (int64_t i_21832 = 0; i_21832 < m_18009; i_21832++) {
                                    int32_t eta_p_19763 = ((int32_t *) mem_22117)[i_21832];
                                    
                                    // graph_orientation_futhark.fut:245:66-71
                                    
                                    int64_t v_19764 = sext_i32_i64(eta_p_19763);
                                    
                                    // graph_orientation_futhark.fut:245:66-71
                                    
                                    bool x_19765 = sle64((int64_t) 0, v_19764);
                                    
                                    // graph_orientation_futhark.fut:245:66-71
                                    
                                    bool y_19766 = slt64(v_19764, i32_res_17111);
                                    
                                    // graph_orientation_futhark.fut:245:66-71
                                    
                                    bool bounds_check_19767 = x_19765 && y_19766;
                                    
                                    // graph_orientation_futhark.fut:245:66-71
                                    
                                    bool index_certs_19768;
                                    
                                    if (!bounds_check_19767) {
                                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_19764, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:245:66-71\n   #1  graph_orientation_futhark.fut:265:23-42\n   #2  graph_orientation_futhark.fut:305:28-51\n   #3  graph_orientation_futhark.fut:357:10-30\n"));
                                        err = FUTHARK_PROGRAM_ERROR;
                                        goto cleanup;
                                    }
                                    // graph_orientation_futhark.fut:245:66-71
                                    
                                    int32_t lifted_lambda_res_19769 = ((int32_t *) mem_param_22057.mem)[ctx_param_ext_22055 + v_19764 * ctx_param_ext_22056];
                                    
                                    // graph_orientation_futhark.fut:245:25-32
                                    
                                    int32_t min_res_18045 = smin32(lifted_lambda_res_19769, redout_21831);
                                    int32_t redout_tmp_22846 = min_res_18045;
                                    
                                    redout_21831 = redout_tmp_22846;
                                }
                                defunc_0_reduce_res_21705 = redout_21831;
                                // graph_orientation_futhark.fut:245:16-78
                                
                                int32_t relabel_res_f_res_18046 = add32(1, defunc_0_reduce_res_21705);
                                
                                relabel_res_18031 = relabel_res_f_res_18046;
                            }
                            // graph_orientation_futhark.fut:266:23-29
                            if (memblock_alloc(ctx, &mem_22125, bytes_21960, "mem_22125")) {
                                err = 1;
                                goto cleanup;
                            }
                            // graph_orientation_futhark.fut:266:23-29
                            // graph_orientation_futhark.fut:266:23-29
                            lmad_copy_4b(ctx, 1, (uint32_t *) mem_22125.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22057.mem, ctx_param_ext_22055, (int64_t []) {ctx_param_ext_22056}, (int64_t []) {i32_res_17111});
                            // graph_orientation_futhark.fut:266:23-43
                            ((int32_t *) mem_22125.mem)[u_17879] = relabel_res_18031;
                            if (memblock_set(ctx, &ext_mem_22128, &mem_22125, "mem_22125") != 0)
                                return 1;
                        }
                        if (memblock_set(ctx, &ext_mem_22138, &ext_mem_22097, "ext_mem_22097") != 0)
                            return 1;
                        ext_22137 = ext_22095;
                        ext_22136 = ext_22094;
                        ext_22135 = ext_22093;
                        if (memblock_set(ctx, &ext_mem_22134, &ext_mem_22096, "ext_mem_22096") != 0)
                            return 1;
                        ext_22133 = ext_22092;
                        ext_22132 = ext_22091;
                        if (memblock_set(ctx, &ext_mem_22131, &ext_mem_22128, "ext_mem_22128") != 0)
                            return 1;
                        ext_22130 = ext_22127;
                        ext_22129 = ext_22126;
                    }
                    // graph_orientation_futhark.fut:251:13-21
                    
                    bool not_res_18049 = !cond_17893;
                    
                    if (memblock_set(ctx, &mem_param_tmp_22814, &ext_mem_22138, "ext_mem_22138") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_tmp_22815, &ext_mem_22134, "ext_mem_22134") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_tmp_22816, &ext_mem_22131, "ext_mem_22131") != 0)
                        return 1;
                    
                    int64_t ctx_param_ext_tmp_22817 = ext_22137;
                    int64_t ctx_param_ext_tmp_22818 = ext_22136;
                    int64_t ctx_param_ext_tmp_22819 = ext_22135;
                    int64_t ctx_param_ext_tmp_22820 = ext_22133;
                    int64_t ctx_param_ext_tmp_22821 = ext_22132;
                    int64_t ctx_param_ext_tmp_22822 = ext_22130;
                    int64_t ctx_param_ext_tmp_22823 = ext_22129;
                    bool loop_while_tmp_22824 = not_res_18049;
                    
                    if (memblock_set(ctx, &mem_param_22051, &mem_param_tmp_22814, "mem_param_tmp_22814") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_22054, &mem_param_tmp_22815, "mem_param_tmp_22815") != 0)
                        return 1;
                    if (memblock_set(ctx, &mem_param_22057, &mem_param_tmp_22816, "mem_param_tmp_22816") != 0)
                        return 1;
                    ctx_param_ext_22048 = ctx_param_ext_tmp_22817;
                    ctx_param_ext_22049 = ctx_param_ext_tmp_22818;
                    ctx_param_ext_22050 = ctx_param_ext_tmp_22819;
                    ctx_param_ext_22052 = ctx_param_ext_tmp_22820;
                    ctx_param_ext_22053 = ctx_param_ext_tmp_22821;
                    ctx_param_ext_22055 = ctx_param_ext_tmp_22822;
                    ctx_param_ext_22056 = ctx_param_ext_tmp_22823;
                    loop_while_17888 = loop_while_tmp_22824;
                }
                if (memblock_set(ctx, &ext_mem_22148, &mem_param_22051, "mem_param_22051") != 0)
                    return 1;
                if (memblock_set(ctx, &ext_mem_22147, &mem_param_22054, "mem_param_22054") != 0)
                    return 1;
                if (memblock_set(ctx, &ext_mem_22146, &mem_param_22057, "mem_param_22057") != 0)
                    return 1;
                ext_22145 = ctx_param_ext_22048;
                ext_22144 = ctx_param_ext_22049;
                ext_22143 = ctx_param_ext_22050;
                ext_22142 = ctx_param_ext_22052;
                ext_22141 = ctx_param_ext_22053;
                ext_22140 = ctx_param_ext_22055;
                ext_22139 = ctx_param_ext_22056;
                discharge_res_17884 = loop_while_17888;
                // graph_orientation_futhark.fut:307:22-310:20
                
                int64_t discard_21841;
                int64_t scanacc_21835 = (int64_t) 0;
                
                for (int64_t i_21838 = 0; i_21838 < i32_res_17111; i_21838++) {
                    // graph_orientation_futhark.fut:308:19-30
                    
                    int32_t i64_res_19413 = sext_i64_i32(i_21838);
                    
                    // graph_orientation_futhark.fut:309:14-64
                    
                    bool cond_19414 = i64_res_19413 == 0;
                    
                    // graph_orientation_futhark.fut:309:14-64
                    
                    bool cond_19415 = !cond_19414;
                    
                    // graph_orientation_futhark.fut:309:26-30
                    
                    bool cond_t_res_19416 = i64_res_19413 == sink_res_17105;
                    
                    // graph_orientation_futhark.fut:309:26-30
                    
                    bool cond_t_res_19417 = !cond_t_res_19416;
                    
                    // graph_orientation_futhark.fut:307:22-310:20
                    
                    bool x_19418 = cond_19415 && cond_t_res_19417;
                    
                    // graph_orientation_futhark.fut:309:14-64
                    
                    bool cond_19419;
                    
                    if (x_19418) {
                        // graph_orientation_futhark.fut:309:34-39
                        
                        int64_t v_21709 = sext_i32_i64(i64_res_19413);
                        
                        // graph_orientation_futhark.fut:309:34-39
                        
                        bool x_21710 = sle64((int64_t) 0, v_21709);
                        
                        // graph_orientation_futhark.fut:309:34-39
                        
                        bool y_21711 = slt64(v_21709, i32_res_17111);
                        
                        // graph_orientation_futhark.fut:309:34-39
                        
                        bool bounds_check_21712 = x_21710 && y_21711;
                        
                        // graph_orientation_futhark.fut:309:34-39
                        
                        bool index_certs_21713;
                        
                        if (!bounds_check_21712) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21709, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:309:34-39\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:309:34-39
                        
                        int64_t zeze_lhs_21714 = ((int64_t *) mem_param_22044.mem)[ctx_param_ext_22042 + v_21709 * ctx_param_ext_22043];
                        
                        // graph_orientation_futhark.fut:309:40-47
                        
                        bool cond_t_res_21715 = zeze_lhs_21714 == (int64_t) 0;
                        
                        cond_19419 = cond_t_res_21715;
                    } else {
                        cond_19419 = 0;
                    }
                    // graph_orientation_futhark.fut:309:14-64
                    
                    bool lifted_lambda_res_19427;
                    
                    if (cond_19419) {
                        // graph_orientation_futhark.fut:309:51-57
                        
                        int64_t v_21716 = sext_i32_i64(i64_res_19413);
                        
                        // graph_orientation_futhark.fut:309:51-57
                        
                        bool x_21717 = sle64((int64_t) 0, v_21716);
                        
                        // graph_orientation_futhark.fut:309:51-57
                        
                        bool y_21718 = slt64(v_21716, i32_res_17111);
                        
                        // graph_orientation_futhark.fut:309:51-57
                        
                        bool bounds_check_21719 = x_21717 && y_21718;
                        
                        // graph_orientation_futhark.fut:309:51-57
                        
                        bool index_certs_21720;
                        
                        if (!bounds_check_21719) {
                            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_21716, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:309:51-57\n   #1  graph_orientation_futhark.fut:357:10-30\n"));
                            err = FUTHARK_PROGRAM_ERROR;
                            goto cleanup;
                        }
                        // graph_orientation_futhark.fut:309:51-57
                        
                        int64_t zg_lhs_21721 = ((int64_t *) ext_mem_22147.mem)[ext_22142 + v_21716 * ext_22141];
                        
                        // graph_orientation_futhark.fut:309:58-64
                        
                        bool lifted_lambda_res_t_res_21722 = slt64((int64_t) 0, zg_lhs_21721);
                        
                        lifted_lambda_res_19427 = lifted_lambda_res_t_res_21722;
                    } else {
                        lifted_lambda_res_19427 = 0;
                    }
                    // graph_orientation_futhark.fut:307:22-310:20
                    
                    int64_t defunc_0_f_res_19435 = btoi_bool_i64(lifted_lambda_res_19427);
                    
                    // graph_orientation_futhark.fut:307:22-310:20
                    
                    int64_t defunc_0_op_res_18079 = add64(defunc_0_f_res_19435, scanacc_21835);
                    
                    ((int64_t *) mem_22150)[i_21838] = defunc_0_op_res_18079;
                    ((int64_t *) mem_22152)[i_21838] = defunc_0_f_res_19435;
                    
                    int64_t scanacc_tmp_22847 = defunc_0_op_res_18079;
                    
                    scanacc_21835 = scanacc_tmp_22847;
                }
                discard_21841 = scanacc_21835;
                // graph_orientation_futhark.fut:307:22-310:20
                
                int64_t m_f_res_18080;
                
                if (x_17736) {
                    // graph_orientation_futhark.fut:307:22-310:20
                    
                    int64_t x_21723 = ((int64_t *) mem_22150)[tmp_17737];
                    
                    m_f_res_18080 = x_21723;
                } else {
                    m_f_res_18080 = (int64_t) 0;
                }
                // graph_orientation_futhark.fut:307:22-310:20
                
                int64_t m_18082;
                
                if (cond_17735) {
                    m_18082 = (int64_t) 0;
                } else {
                    m_18082 = m_f_res_18080;
                }
                // graph_orientation_futhark.fut:307:22-310:20
                
                int64_t bytes_22165 = (int64_t) 8 * m_18082;
                
                // graph_orientation_futhark.fut:307:9-310:21
                
                int64_t bytes_22167 = (int64_t) 4 * m_18082;
                
                // graph_orientation_futhark.fut:307:22-310:20
                if (mem_22166_cached_sizze_22902 < bytes_22165) {
                    err = lexical_realloc(ctx, &mem_22166, &mem_22166_cached_sizze_22902, bytes_22165);
                    if (err != FUTHARK_SUCCESS)
                        goto cleanup;
                }
                // graph_orientation_futhark.fut:307:22-310:20
                
                bool acc_cert_19383;
                
                // graph_orientation_futhark.fut:307:22-310:20
                for (int64_t i_21843 = 0; i_21843 < i32_res_17111; i_21843++) {
                    int64_t eta_p_19398 = ((int64_t *) mem_22152)[i_21843];
                    int64_t eta_p_19399 = ((int64_t *) mem_22150)[i_21843];
                    
                    // graph_orientation_futhark.fut:307:22-310:20
                    
                    bool cond_19402 = eta_p_19398 == (int64_t) 1;
                    
                    // graph_orientation_futhark.fut:307:22-310:20
                    
                    int64_t lifted_lambda_res_19403;
                    
                    if (cond_19402) {
                        // graph_orientation_futhark.fut:307:22-310:20
                        
                        int64_t lifted_lambda_res_t_res_21724 = sub64(eta_p_19399, (int64_t) 1);
                        
                        lifted_lambda_res_19403 = lifted_lambda_res_t_res_21724;
                    } else {
                        lifted_lambda_res_19403 = (int64_t) -1;
                    }
                    // graph_orientation_futhark.fut:307:22-310:20
                    // UpdateAcc
                    if (sle64((int64_t) 0, lifted_lambda_res_19403) && slt64(lifted_lambda_res_19403, m_18082)) {
                        ((int64_t *) mem_22166)[lifted_lambda_res_19403] = i_21843;
                    }
                }
                // graph_orientation_futhark.fut:307:9-310:21
                if (mem_22168_cached_sizze_22903 < bytes_22167) {
                    err = lexical_realloc(ctx, &mem_22168, &mem_22168_cached_sizze_22903, bytes_22167);
                    if (err != FUTHARK_SUCCESS)
                        goto cleanup;
                }
                // graph_orientation_futhark.fut:307:9-310:21
                for (int64_t i_21846 = 0; i_21846 < m_18082; i_21846++) {
                    int64_t eta_p_18099 = ((int64_t *) mem_22166)[i_21846];
                    
                    // graph_orientation_futhark.fut:307:13-20
                    
                    int32_t i64_res_18100 = sext_i64_i32(eta_p_18099);
                    
                    ((int32_t *) mem_22168)[i_21846] = i64_res_18100;
                }
                // graph_orientation_futhark.fut:311:19-25
                
                int64_t zg_lhs_18101 = ((int64_t *) ext_mem_22147.mem)[ext_22142 + u_17879 * ext_22141];
                
                // graph_orientation_futhark.fut:311:16-312:39
                
                bool cond_18102 = slt64((int64_t) 0, zg_lhs_18101);
                
                // graph_orientation_futhark.fut:312:21-39
                
                int64_t conc_tmp_18103 = j_m_i_17869 + m_18082;
                
                // graph_orientation_futhark.fut:311:43-63
                
                int64_t conc_tmp_18104 = (int64_t) 1 + conc_tmp_18103;
                
                // graph_orientation_futhark.fut:357:10-30
                
                int64_t qzq_18105;
                
                if (cond_18102) {
                    qzq_18105 = conc_tmp_18104;
                } else {
                    qzq_18105 = conc_tmp_18103;
                }
                // graph_orientation_futhark.fut:357:10-30
                
                int64_t bytes_22175 = (int64_t) 4 * qzq_18105;
                
                // graph_orientation_futhark.fut:357:10-30
                if (cond_18102) {
                    // graph_orientation_futhark.fut:357:10-30
                    if (memblock_alloc(ctx, &mem_22179, bytes_22175, "mem_22179")) {
                        err = 1;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:357:10-30
                    for (int64_t nest_i_22852 = 0; nest_i_22852 < (int64_t) 1; nest_i_22852++) {
                        ((int32_t *) mem_22179.mem)[j_m_i_17869 + m_18082 + nest_i_22852] = head_res_17868;
                    }
                    // graph_orientation_futhark.fut:357:10-30
                    
                    int64_t tmp_offs_22853 = (int64_t) 0;
                    
                    // graph_orientation_futhark.fut:357:10-30
                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22179.mem, tmp_offs_22853, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22037.mem, (int64_t) 0 + (int64_t) 1, (int64_t []) {(int64_t) 1}, (int64_t []) {j_m_i_17869});
                    tmp_offs_22853 += j_m_i_17869;
                    // graph_orientation_futhark.fut:357:10-30
                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22179.mem, tmp_offs_22853, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_22168, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {m_18082});
                    tmp_offs_22853 += m_18082;
                    if (!(tmp_offs_22853 == j_m_i_17869 + m_18082)) {
                        // graph_orientation_futhark.fut:357:10-30
                        lmad_copy_4b(ctx, 1, (uint32_t *) mem_22179.mem, tmp_offs_22853, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_22179.mem, j_m_i_17869 + m_18082, (int64_t []) {(int64_t) 1}, (int64_t []) {(int64_t) 1});
                    }
                    tmp_offs_22853 += (int64_t) 1;
                    if (memblock_set(ctx, &ext_mem_22180, &mem_22179, "mem_22179") != 0)
                        return 1;
                } else {
                    // graph_orientation_futhark.fut:357:10-30
                    if (memblock_alloc(ctx, &mem_22176, bytes_22175, "mem_22176")) {
                        err = 1;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:357:10-30
                    
                    int64_t tmp_offs_22854 = (int64_t) 0;
                    
                    // graph_orientation_futhark.fut:357:10-30
                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22176.mem, tmp_offs_22854, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22037.mem, (int64_t) 0 + (int64_t) 1, (int64_t []) {(int64_t) 1}, (int64_t []) {j_m_i_17869});
                    tmp_offs_22854 += j_m_i_17869;
                    // graph_orientation_futhark.fut:357:10-30
                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22176.mem, tmp_offs_22854, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_22168, (int64_t) 0, (int64_t []) {(int64_t) 1}, (int64_t []) {m_18082});
                    tmp_offs_22854 += m_18082;
                    if (memblock_set(ctx, &ext_mem_22180, &mem_22176, "mem_22176") != 0)
                        return 1;
                }
                // graph_orientation_futhark.fut:301:22-25
                
                bool loop_cond_18110 = slt64((int64_t) 0, qzq_18105);
                
                if (memblock_set(ctx, &mem_param_tmp_22797, &ext_mem_22180, "ext_mem_22180") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_tmp_22798, &ext_mem_22148, "ext_mem_22148") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_tmp_22799, &ext_mem_22147, "ext_mem_22147") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_tmp_22800, &ext_mem_22146, "ext_mem_22146") != 0)
                    return 1;
                
                int64_t ctx_param_ext_tmp_22801 = ext_22145;
                int64_t ctx_param_ext_tmp_22802 = ext_22144;
                int64_t ctx_param_ext_tmp_22803 = ext_22143;
                int64_t ctx_param_ext_tmp_22804 = ext_22142;
                int64_t ctx_param_ext_tmp_22805 = ext_22141;
                int64_t ctx_param_ext_tmp_22806 = ext_22140;
                int64_t ctx_param_ext_tmp_22807 = ext_22139;
                int64_t loop_dz2088Uz2084U_tmp_22808 = qzq_18105;
                bool loop_while_tmp_22809 = loop_cond_18110;
                
                if (memblock_set(ctx, &mem_param_22037, &mem_param_tmp_22797, "mem_param_tmp_22797") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22041, &mem_param_tmp_22798, "mem_param_tmp_22798") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22044, &mem_param_tmp_22799, "mem_param_tmp_22799") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22047, &mem_param_tmp_22800, "mem_param_tmp_22800") != 0)
                    return 1;
                ctx_param_ext_22038 = ctx_param_ext_tmp_22801;
                ctx_param_ext_22039 = ctx_param_ext_tmp_22802;
                ctx_param_ext_22040 = ctx_param_ext_tmp_22803;
                ctx_param_ext_22042 = ctx_param_ext_tmp_22804;
                ctx_param_ext_22043 = ctx_param_ext_tmp_22805;
                ctx_param_ext_22045 = ctx_param_ext_tmp_22806;
                ctx_param_ext_22046 = ctx_param_ext_tmp_22807;
                loop_dz2088Uz2084U_17860 = loop_dz2088Uz2084U_tmp_22808;
                loop_while_17861 = loop_while_tmp_22809;
            }
            if (memblock_set(ctx, &ext_mem_22193, &mem_param_22037, "mem_param_22037") != 0)
                return 1;
            if (memblock_set(ctx, &ext_mem_22192, &mem_param_22041, "mem_param_22041") != 0)
                return 1;
            if (memblock_set(ctx, &ext_mem_22191, &mem_param_22044, "mem_param_22044") != 0)
                return 1;
            if (memblock_set(ctx, &ext_mem_22190, &mem_param_22047, "mem_param_22047") != 0)
                return 1;
            ext_22187 = ctx_param_ext_22038;
            ext_22186 = ctx_param_ext_22039;
            ext_22185 = ctx_param_ext_22040;
            ext_22184 = ctx_param_ext_22042;
            ext_22183 = ctx_param_ext_22043;
            ext_22182 = ctx_param_ext_22045;
            ext_22181 = ctx_param_ext_22046;
            push_relabel_res_17854 = loop_dz2088Uz2084U_17860;
            push_relabel_res_17855 = loop_while_17861;
            if (memblock_unref(ctx, &mem_21961, "mem_21961") != 0)
                return 1;
            if (memblock_unref(ctx, &ext_mem_21997, "ext_mem_21997") != 0)
                return 1;
            if (memblock_unref(ctx, &ext_mem_22008, "ext_mem_22008") != 0)
                return 1;
            if (memblock_unref(ctx, &mem_22028, "mem_22028") != 0)
                return 1;
            // graph_orientation_futhark.fut:315:7-13
            
            int64_t tmp_18116 = ((int64_t *) ext_mem_22191.mem)[ext_22184 + t_21255 * ext_22183];
            
            if (memblock_unref(ctx, &ext_mem_22191, "ext_mem_22191") != 0)
                return 1;
            if (memblock_set(ctx, &ext_mem_22324, &ext_mem_22192, "ext_mem_22192") != 0)
                return 1;
            ext_22323 = ext_22187;
            ext_22322 = ext_22186;
            ext_22321 = ext_22185;
            graph_orientation_solve_res_f_res_17389 = tmp_18116;
        }
        if (memblock_set(ctx, &ext_mem_22413, &ext_mem_22324, "ext_mem_22324") != 0)
            return 1;
        ext_22412 = ext_22323;
        ext_22411 = ext_22322;
        ext_22410 = ext_22321;
        graph_orientation_solve_res_17166 = graph_orientation_solve_res_f_res_17389;
    }
    if (memblock_unref(ctx, &mem_21931, "mem_21931") != 0)
        return 1;
    // graph_orientation_futhark.fut:359:20-32
    
    bool ok_18117 = graph_orientation_solve_res_17166 == i32_res_17103;
    
    // graph_orientation_futhark.fut:372:8-23
    
    bool range_valid_c_18120;
    
    if (!valid_17118) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Range ", (long long) (int64_t) 0, "..", (long long) (int64_t) 1, "..<", (long long) i32_res_17103, " is invalid.", "-> #0  graph_orientation_futhark.fut:372:8-23\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:362:5-372:24
    
    int64_t binop_x_22414 = (int64_t) 8 * i32_res_17103;
    
    // graph_orientation_futhark.fut:362:5-372:24
    
    int64_t bytes_22415 = (int64_t) 16 * i32_res_17103;
    bool protect_assert_disj_18312 = y_17712 || ok_18117;
    
    // graph_orientation_futhark.fut:320:15-52
    
    bool index_certs_18184;
    
    if (!protect_assert_disj_18312) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) (int64_t) 0, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:320:15-52\n   #1  graph_orientation_futhark.fut:380:16-49\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    
    bool protect_assert_disj_18316 = ok_18117 || valid_21259;
    
    // graph_orientation_futhark.fut:330:9-336:29
    
    bool range_valid_c_18193;
    
    if (!protect_assert_disj_18316) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Range ", (long long) (int64_t) 0, "..<", (long long) i32_res_17111, " is invalid.", "-> #0  graph_orientation_futhark.fut:330:9-336:29\n   #1  graph_orientation_futhark.fut:380:16-49\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    
    bool protect_assert_disj_18318 = valid_17148 || ok_18117;
    
    // graph_orientation_futhark.fut:384:11-26
    
    bool range_valid_c_18261;
    
    if (!protect_assert_disj_18318) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s%lld%s\n\nBacktrace:\n%s", "Range ", (long long) (int64_t) 0, "..", (long long) (int64_t) 1, "..<", (long long) i32_res_17102, " is invalid.", "-> #0  graph_orientation_futhark.fut:384:11-26\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:381:8-384:27
    
    bool cond_18279 = i32_res_17102 == (int64_t) 0;
    
    // graph_orientation_futhark.fut:381:8-384:27
    
    int64_t tmp_18281 = sub64(i32_res_17102, (int64_t) 1);
    
    // graph_orientation_futhark.fut:381:8-384:27
    
    bool x_18282 = sle64((int64_t) 0, tmp_18281);
    
    // graph_orientation_futhark.fut:381:8-384:27
    
    bool y_18283 = slt64(tmp_18281, i32_res_17102);
    
    // graph_orientation_futhark.fut:381:8-384:27
    
    bool bounds_check_18284 = x_18282 && y_18283;
    
    // graph_orientation_futhark.fut:381:8-384:27
    
    bool protect_assert_disj_18285 = cond_18279 || bounds_check_18284;
    bool protect_assert_disj_18320 = ok_18117 || protect_assert_disj_18285;
    
    // graph_orientation_futhark.fut:381:8-384:27
    
    bool index_certs_18286;
    
    if (!protect_assert_disj_18320) {
        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) tmp_18281, "] out of bounds for array of shape [", (long long) i32_res_17102, "].", "-> #0  graph_orientation_futhark.fut:381:8-384:27\n"));
        err = FUTHARK_PROGRAM_ERROR;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:362:5-372:24
    if (mem_22416_cached_sizze_22904 < bytes_22415) {
        err = lexical_realloc(ctx, &mem_22416, &mem_22416_cached_sizze_22904, bytes_22415);
        if (err != FUTHARK_SUCCESS)
            goto cleanup;
    }
    // graph_orientation_futhark.fut:362:5-372:24
    for (int64_t i_21850 = 0; i_21850 < i32_res_17103; i_21850++) {
        // graph_orientation_futhark.fut:363:15-26
        
        int32_t i64_res_18130 = sext_i64_i32(i_21850);
        
        // graph_orientation_futhark.fut:364:15-20
        
        int64_t k_18131 = sext_i32_i64(i64_res_18130);
        
        // graph_orientation_futhark.fut:364:15-20
        
        bool x_18132 = sle64((int64_t) 0, k_18131);
        
        // graph_orientation_futhark.fut:364:15-20
        
        bool y_18133 = slt64(k_18131, dz2081U_14789);
        
        // graph_orientation_futhark.fut:364:15-20
        
        bool bounds_check_18134 = x_18132 && y_18133;
        
        // graph_orientation_futhark.fut:364:15-20
        
        bool index_certs_18135;
        
        if (!bounds_check_18134) {
            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) k_18131, "] out of bounds for array of shape [", (long long) dz2081U_14789, "].", "-> #0  graph_orientation_futhark.fut:364:15-20\n"));
            err = FUTHARK_PROGRAM_ERROR;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:364:15-20
        
        int32_t u_18136 = ((int32_t *) eu_mem_21927.mem)[k_18131];
        
        // graph_orientation_futhark.fut:365:15-20
        
        bool y_18137 = slt64(k_18131, dz2082U_14790);
        
        // graph_orientation_futhark.fut:365:15-20
        
        bool bounds_check_18138 = x_18132 && y_18137;
        
        // graph_orientation_futhark.fut:365:15-20
        
        bool index_certs_18139;
        
        if (!bounds_check_18138) {
            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) k_18131, "] out of bounds for array of shape [", (long long) dz2082U_14790, "].", "-> #0  graph_orientation_futhark.fut:365:15-20\n"));
            err = FUTHARK_PROGRAM_ERROR;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:9:36-39
        
        int32_t edge_node_res_18141 = add32(1, i64_res_18130);
        
        // graph_orientation_futhark.fut:11:58-61
        
        int32_t vertex_node_res_18142 = add32(zp_lhs_17104, u_18136);
        
        // graph_orientation_futhark.fut:368:22-34
        
        int64_t ek_18143 = sext_i32_i64(edge_node_res_18141);
        
        // graph_orientation_futhark.fut:368:22-34
        
        bool x_18144 = sle64((int64_t) 0, ek_18143);
        
        // graph_orientation_futhark.fut:368:22-34
        
        bool y_18145 = slt64(ek_18143, i32_res_17111);
        
        // graph_orientation_futhark.fut:368:22-34
        
        bool bounds_check_18146 = x_18144 && y_18145;
        
        // graph_orientation_futhark.fut:368:22-34
        
        bool index_certs_18147;
        
        if (!bounds_check_18146) {
            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) ek_18143, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:368:22-34\n"));
            err = FUTHARK_PROGRAM_ERROR;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:368:22-38
        
        int64_t vu_18148 = sext_i32_i64(vertex_node_res_18142);
        
        // graph_orientation_futhark.fut:368:22-38
        
        bool x_18149 = sle64((int64_t) 0, vu_18148);
        
        // graph_orientation_futhark.fut:368:22-38
        
        bool y_18150 = slt64(vu_18148, i32_res_17111);
        
        // graph_orientation_futhark.fut:368:22-38
        
        bool bounds_check_18151 = x_18149 && y_18150;
        
        // graph_orientation_futhark.fut:368:22-38
        
        bool index_certs_18152;
        
        if (!bounds_check_18151) {
            set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) vu_18148, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:368:22-38\n"));
            err = FUTHARK_PROGRAM_ERROR;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:365:15-20
        
        int32_t v_18140 = ((int32_t *) ev_mem_21928.mem)[k_18131];
        
        // graph_orientation_futhark.fut:362:5-372:24
        
        int64_t zg_lhs_18153 = ((int64_t *) ext_mem_22413.mem)[ext_22412 + (ek_18143 * ext_22411 + vu_18148 * ext_22410)];
        
        // graph_orientation_futhark.fut:368:39-45
        
        bool chosen_u_18154 = slt64((int64_t) 0, zg_lhs_18153);
        
        if (chosen_u_18154) {
            // graph_orientation_futhark.fut:370:16-25
            
            int64_t i32_res_21738 = sext_i32_i64(v_18140);
            
            // graph_orientation_futhark.fut:370:27-36
            
            int64_t i32_res_21739 = sext_i32_i64(u_18136);
            
            // graph_orientation_futhark.fut:362:5-372:24
            ((int64_t *) mem_22416)[i_21850 * (int64_t) 2] = i32_res_21738;
            ((int64_t *) mem_22416)[i_21850 * (int64_t) 2 + (int64_t) 1] = i32_res_21739;
        } else {
            // graph_orientation_futhark.fut:371:16-25
            
            int64_t i32_res_18159 = sext_i32_i64(u_18136);
            
            // graph_orientation_futhark.fut:371:27-36
            
            int64_t i32_res_18160 = sext_i32_i64(v_18140);
            
            // graph_orientation_futhark.fut:362:5-372:24
            ((int64_t *) mem_22416)[i_21850 * (int64_t) 2] = i32_res_18159;
            ((int64_t *) mem_22416)[i_21850 * (int64_t) 2 + (int64_t) 1] = i32_res_18160;
        }
    }
    // graph_orientation_futhark.fut:374:20-55
    if (memblock_alloc(ctx, &mem_22429, binop_x_22414, "mem_22429")) {
        err = 1;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:374:20-55
    // graph_orientation_futhark.fut:374:20-55
    lmad_copy_8b(ctx, 1, (uint64_t *) mem_22429.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint64_t *) mem_22416, (int64_t) 0, (int64_t []) {(int64_t) 2}, (int64_t []) {i32_res_17103});
    // graph_orientation_futhark.fut:374:31-375:55
    if (memblock_alloc(ctx, &mem_22431, binop_x_22414, "mem_22431")) {
        err = 1;
        goto cleanup;
    }
    // graph_orientation_futhark.fut:374:31-375:55
    // graph_orientation_futhark.fut:374:31-375:55
    lmad_copy_8b(ctx, 1, (uint64_t *) mem_22431.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint64_t *) mem_22416, (int64_t) 0 + (int64_t) 1, (int64_t []) {(int64_t) 2}, (int64_t []) {i32_res_17103});
    // graph_orientation_futhark.fut:381:8-384:27
    
    bool x_18280 = !cond_18279;
    
    // graph_orientation_futhark.fut:381:8-384:27
    
    int64_t bytes_22468 = (int64_t) 8 * i32_res_17102;
    
    // graph_orientation_futhark.fut:378:3-384:28
    
    int64_t violators_18175;
    
    if (ok_18117) {
        // graph_orientation_futhark.fut:378:14-16
        if (memblock_alloc(ctx, &mem_22486, (int64_t) 0, "mem_22486")) {
            err = 1;
            goto cleanup;
        }
        if (memblock_set(ctx, &ext_mem_22487, &mem_22486, "mem_22486") != 0)
            return 1;
        violators_18175 = (int64_t) 0;
    } else {
        // graph_orientation_futhark.fut:320:16-35
        if (memblock_alloc(ctx, &mem_22432, i32_res_17111, "mem_22432")) {
            err = 1;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:320:16-35
        for (int64_t nest_i_22856 = 0; nest_i_22856 < i32_res_17111; nest_i_22856++) {
            ((bool *) mem_22432.mem)[nest_i_22856] = 0;
        }
        // graph_orientation_futhark.fut:320:15-52
        ((bool *) mem_22432.mem)[(int64_t) 0] = 1;
        // graph_orientation_futhark.fut:321:17-35
        if (memblock_alloc(ctx, &mem_22434, bytes_21960, "mem_22434")) {
            err = 1;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:321:17-35
        for (int64_t nest_i_22857 = 0; nest_i_22857 < i32_res_17111; nest_i_22857++) {
            ((int32_t *) mem_22434.mem)[nest_i_22857] = 0;
        }
        // graph_orientation_futhark.fut:321:16-49
        ((int32_t *) mem_22434.mem)[(int64_t) 0] = 0;
        // graph_orientation_futhark.fut:324:5-337:41
        
        int64_t ext_22465;
        int64_t ext_22464;
        int64_t ext_22463;
        int64_t ext_22462;
        bool seenf_18194;
        int32_t seenf_18195;
        int32_t seenf_18196;
        bool loop_while_18199;
        int32_t qstart_18200;
        int32_t qend_18201;
        int64_t ctx_param_ext_22435;
        int64_t ctx_param_ext_22436;
        int64_t ctx_param_ext_22438;
        int64_t ctx_param_ext_22439;
        
        if (memblock_set(ctx, &mem_param_22437, &mem_22432, "mem_22432") != 0)
            return 1;
        if (memblock_set(ctx, &mem_param_22440, &mem_22434, "mem_22434") != 0)
            return 1;
        ctx_param_ext_22435 = (int64_t) 0;
        ctx_param_ext_22436 = (int64_t) 1;
        ctx_param_ext_22438 = (int64_t) 0;
        ctx_param_ext_22439 = (int64_t) 1;
        loop_while_18199 = 1;
        qstart_18200 = 0;
        qend_18201 = 1;
        while (loop_while_18199) {
            // graph_orientation_futhark.fut:327:15-28
            
            int64_t qstart_18204 = sext_i32_i64(qstart_18200);
            
            // graph_orientation_futhark.fut:327:15-28
            
            bool x_18205 = sle64((int64_t) 0, qstart_18204);
            
            // graph_orientation_futhark.fut:327:15-28
            
            bool y_18206 = slt64(qstart_18204, i32_res_17111);
            
            // graph_orientation_futhark.fut:327:15-28
            
            bool bounds_check_18207 = x_18205 && y_18206;
            
            // graph_orientation_futhark.fut:327:15-28
            
            bool index_certs_18208;
            
            if (!bounds_check_18207) {
                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) qstart_18204, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:327:15-28\n   #1  graph_orientation_futhark.fut:380:16-49\n"));
                err = FUTHARK_PROGRAM_ERROR;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:327:15-28
            
            int32_t u_18209 = ((int32_t *) mem_param_22440.mem)[ctx_param_ext_22438 + qstart_18204 * ctx_param_ext_22439];
            
            // graph_orientation_futhark.fut:328:28-31
            
            int32_t qstartzq_18210 = add32(1, qstart_18200);
            
            // graph_orientation_futhark.fut:332:29-35
            
            int64_t u_21742 = sext_i32_i64(u_18209);
            
            // graph_orientation_futhark.fut:332:29-35
            
            bool x_21743 = sle64((int64_t) 0, u_21742);
            
            // graph_orientation_futhark.fut:332:29-35
            
            bool y_21744 = slt64(u_21742, i32_res_17111);
            
            // graph_orientation_futhark.fut:332:29-35
            
            bool bounds_check_21745 = x_21743 && y_21744;
            
            // graph_orientation_futhark.fut:330:9-336:29
            
            int64_t ext_22459;
            int64_t ext_22458;
            int64_t ext_22457;
            int64_t ext_22456;
            int32_t loopres_18213;
            int32_t qe_18217;
            int64_t ctx_param_ext_22441;
            int64_t ctx_param_ext_22442;
            int64_t ctx_param_ext_22444;
            int64_t ctx_param_ext_22445;
            
            if (memblock_set(ctx, &mem_param_22443, &mem_param_22437, "mem_param_22437") != 0)
                return 1;
            if (memblock_set(ctx, &mem_param_22446, &mem_param_22440, "mem_param_22440") != 0)
                return 1;
            ctx_param_ext_22441 = ctx_param_ext_22435;
            ctx_param_ext_22442 = ctx_param_ext_22436;
            ctx_param_ext_22444 = ctx_param_ext_22438;
            ctx_param_ext_22445 = ctx_param_ext_22439;
            qe_18217 = qend_18201;
            for (int64_t i_18214 = 0; i_18214 < i32_res_17111; i_18214++) {
                // graph_orientation_futhark.fut:380:16-49
                
                int32_t binop_x_18218 = sext_i64_i32(i_18214);
                
                // graph_orientation_futhark.fut:332:20-25
                
                int64_t v_18219 = sext_i32_i64(binop_x_18218);
                
                // graph_orientation_futhark.fut:332:20-25
                
                bool x_18220 = sle64((int64_t) 0, v_18219);
                
                // graph_orientation_futhark.fut:332:20-25
                
                bool y_18221 = slt64(v_18219, i32_res_17111);
                
                // graph_orientation_futhark.fut:332:20-25
                
                bool bounds_check_18222 = x_18220 && y_18221;
                
                // graph_orientation_futhark.fut:332:20-25
                
                bool index_certs_18223;
                
                if (!bounds_check_18222) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) v_18219, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:332:20-25\n   #1  graph_orientation_futhark.fut:380:16-49\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:332:20-25
                
                bool not_arg0_18224 = ((bool *) mem_param_22443.mem)[ctx_param_ext_22441 + v_18219 * ctx_param_ext_22442];
                bool protect_assert_disj_22491 = not_arg0_18224 || bounds_check_21745;
                
                // graph_orientation_futhark.fut:332:29-35
                
                bool index_certs_21746;
                
                if (!protect_assert_disj_22491) {
                    set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) u_21742, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:332:29-35\n   #1  graph_orientation_futhark.fut:380:16-49\n"));
                    err = FUTHARK_PROGRAM_ERROR;
                    goto cleanup;
                }
                // graph_orientation_futhark.fut:332:13-336:29
                
                bool not_res_18225 = !not_arg0_18224;
                
                // graph_orientation_futhark.fut:380:16-49
                
                int64_t zm_lhs_21747;
                
                if (not_res_18225) {
                    int64_t x_22492 = ((int64_t *) ext_mem_21959.mem)[u_21742 * i32_res_17111 + v_18219];
                    
                    zm_lhs_21747 = x_22492;
                } else {
                    zm_lhs_21747 = (int64_t) 0;
                }
                // graph_orientation_futhark.fut:380:16-49
                
                int64_t zm_rhs_21748;
                
                if (not_res_18225) {
                    int64_t x_22494 = ((int64_t *) ext_mem_22413.mem)[ext_22412 + (u_21742 * ext_22411 + v_18219 * ext_22410)];
                    
                    zm_rhs_21748 = x_22494;
                } else {
                    zm_rhs_21748 = (int64_t) 0;
                }
                // graph_orientation_futhark.fut:332:39-51
                
                int64_t zg_lhs_21749 = sub64(zm_lhs_21747, zm_rhs_21748);
                
                // graph_orientation_futhark.fut:332:52-58
                
                bool cond_t_res_21750 = slt64((int64_t) 0, zg_lhs_21749);
                bool x_22637 = not_res_18225 && cond_t_res_21750;
                int64_t ext_22454;
                
                if (x_22637) {
                    ext_22454 = (int64_t) 0;
                } else {
                    ext_22454 = ctx_param_ext_22441;
                }
                
                int64_t ext_22453;
                
                if (x_22637) {
                    ext_22453 = (int64_t) 1;
                } else {
                    ext_22453 = ctx_param_ext_22442;
                }
                
                int64_t ext_22451;
                
                if (x_22637) {
                    ext_22451 = (int64_t) 0;
                } else {
                    ext_22451 = ctx_param_ext_22444;
                }
                
                int64_t ext_22450;
                
                if (x_22637) {
                    ext_22450 = (int64_t) 1;
                } else {
                    ext_22450 = ctx_param_ext_22445;
                }
                
                int32_t loopres_18247;
                
                if (x_22637) {
                    // graph_orientation_futhark.fut:334:24-44
                    
                    int64_t qe_21754 = sext_i32_i64(qe_18217);
                    
                    // graph_orientation_futhark.fut:334:24-44
                    
                    bool x_21755 = sle64((int64_t) 0, qe_21754);
                    
                    // graph_orientation_futhark.fut:334:24-44
                    
                    bool y_21756 = slt64(qe_21754, i32_res_17111);
                    
                    // graph_orientation_futhark.fut:334:24-44
                    
                    bool bounds_check_21757 = x_21755 && y_21756;
                    
                    // graph_orientation_futhark.fut:334:24-44
                    
                    bool index_certs_21758;
                    
                    if (!bounds_check_21757) {
                        set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) qe_21754, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:334:24-44\n   #1  graph_orientation_futhark.fut:380:16-49\n"));
                        err = FUTHARK_PROGRAM_ERROR;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:333:25-32
                    if (memblock_alloc(ctx, &mem_22447, i32_res_17111, "mem_22447")) {
                        err = 1;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:333:25-32
                    // graph_orientation_futhark.fut:333:25-32
                    lmad_copy_1b(ctx, 1, (uint8_t *) mem_22447.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint8_t *) mem_param_22443.mem, ctx_param_ext_22441, (int64_t []) {ctx_param_ext_22442}, (int64_t []) {i32_res_17111});
                    // graph_orientation_futhark.fut:333:25-48
                    ((bool *) mem_22447.mem)[v_18219] = 1;
                    // graph_orientation_futhark.fut:334:24-30
                    if (memblock_alloc(ctx, &mem_22449, bytes_21960, "mem_22449")) {
                        err = 1;
                        goto cleanup;
                    }
                    // graph_orientation_futhark.fut:334:24-30
                    // graph_orientation_futhark.fut:334:24-30
                    lmad_copy_4b(ctx, 1, (uint32_t *) mem_22449.mem, (int64_t) 0, (int64_t []) {(int64_t) 1}, (uint32_t *) mem_param_22446.mem, ctx_param_ext_22444, (int64_t []) {ctx_param_ext_22445}, (int64_t []) {i32_res_17111});
                    // graph_orientation_futhark.fut:334:24-44
                    ((int32_t *) mem_22449.mem)[qe_21754] = binop_x_18218;
                    // graph_orientation_futhark.fut:335:31-34
                    
                    int32_t tmp_21760 = add32(1, qe_18217);
                    
                    if (memblock_set(ctx, &ext_mem_22455, &mem_22447, "mem_22447") != 0)
                        return 1;
                    if (memblock_set(ctx, &ext_mem_22452, &mem_22449, "mem_22449") != 0)
                        return 1;
                    loopres_18247 = tmp_21760;
                } else {
                    if (memblock_set(ctx, &ext_mem_22455, &mem_param_22443, "mem_param_22443") != 0)
                        return 1;
                    if (memblock_set(ctx, &ext_mem_22452, &mem_param_22446, "mem_param_22446") != 0)
                        return 1;
                    loopres_18247 = qe_18217;
                }
                if (memblock_set(ctx, &mem_param_tmp_22869, &ext_mem_22455, "ext_mem_22455") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_tmp_22870, &ext_mem_22452, "ext_mem_22452") != 0)
                    return 1;
                
                int64_t ctx_param_ext_tmp_22871 = ext_22454;
                int64_t ctx_param_ext_tmp_22872 = ext_22453;
                int64_t ctx_param_ext_tmp_22873 = ext_22451;
                int64_t ctx_param_ext_tmp_22874 = ext_22450;
                int32_t qe_tmp_22877 = loopres_18247;
                
                if (memblock_set(ctx, &mem_param_22443, &mem_param_tmp_22869, "mem_param_tmp_22869") != 0)
                    return 1;
                if (memblock_set(ctx, &mem_param_22446, &mem_param_tmp_22870, "mem_param_tmp_22870") != 0)
                    return 1;
                ctx_param_ext_22441 = ctx_param_ext_tmp_22871;
                ctx_param_ext_22442 = ctx_param_ext_tmp_22872;
                ctx_param_ext_22444 = ctx_param_ext_tmp_22873;
                ctx_param_ext_22445 = ctx_param_ext_tmp_22874;
                qe_18217 = qe_tmp_22877;
            }
            if (memblock_set(ctx, &ext_mem_22461, &mem_param_22443, "mem_param_22443") != 0)
                return 1;
            if (memblock_set(ctx, &ext_mem_22460, &mem_param_22446, "mem_param_22446") != 0)
                return 1;
            ext_22459 = ctx_param_ext_22441;
            ext_22458 = ctx_param_ext_22442;
            ext_22457 = ctx_param_ext_22444;
            ext_22456 = ctx_param_ext_22445;
            loopres_18213 = qe_18217;
            // graph_orientation_futhark.fut:325:20-26
            
            bool loop_cond_18258 = slt32(qstartzq_18210, loopres_18213);
            
            if (memblock_set(ctx, &mem_param_tmp_22858, &ext_mem_22461, "ext_mem_22461") != 0)
                return 1;
            if (memblock_set(ctx, &mem_param_tmp_22859, &ext_mem_22460, "ext_mem_22460") != 0)
                return 1;
            
            int64_t ctx_param_ext_tmp_22860 = ext_22459;
            int64_t ctx_param_ext_tmp_22861 = ext_22458;
            int64_t ctx_param_ext_tmp_22862 = ext_22457;
            int64_t ctx_param_ext_tmp_22863 = ext_22456;
            bool loop_while_tmp_22864 = loop_cond_18258;
            int32_t qstart_tmp_22865 = qstartzq_18210;
            int32_t qend_tmp_22866 = loopres_18213;
            
            if (memblock_set(ctx, &mem_param_22437, &mem_param_tmp_22858, "mem_param_tmp_22858") != 0)
                return 1;
            if (memblock_set(ctx, &mem_param_22440, &mem_param_tmp_22859, "mem_param_tmp_22859") != 0)
                return 1;
            ctx_param_ext_22435 = ctx_param_ext_tmp_22860;
            ctx_param_ext_22436 = ctx_param_ext_tmp_22861;
            ctx_param_ext_22438 = ctx_param_ext_tmp_22862;
            ctx_param_ext_22439 = ctx_param_ext_tmp_22863;
            loop_while_18199 = loop_while_tmp_22864;
            qstart_18200 = qstart_tmp_22865;
            qend_18201 = qend_tmp_22866;
        }
        if (memblock_set(ctx, &ext_mem_22467, &mem_param_22437, "mem_param_22437") != 0)
            return 1;
        if (memblock_set(ctx, &ext_mem_22466, &mem_param_22440, "mem_param_22440") != 0)
            return 1;
        ext_22465 = ctx_param_ext_22435;
        ext_22464 = ctx_param_ext_22436;
        ext_22463 = ctx_param_ext_22438;
        ext_22462 = ctx_param_ext_22439;
        seenf_18194 = loop_while_18199;
        seenf_18195 = qstart_18200;
        seenf_18196 = qend_18201;
        if (memblock_unref(ctx, &mem_22432, "mem_22432") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22434, "mem_22434") != 0)
            return 1;
        // graph_orientation_futhark.fut:381:8-384:27
        if (mem_22469_cached_sizze_22905 < bytes_22468) {
            err = lexical_realloc(ctx, &mem_22469, &mem_22469_cached_sizze_22905, bytes_22468);
            if (err != FUTHARK_SUCCESS)
                goto cleanup;
        }
        // graph_orientation_futhark.fut:381:8-384:27
        if (mem_22471_cached_sizze_22906 < bytes_22468) {
            err = lexical_realloc(ctx, &mem_22471, &mem_22471_cached_sizze_22906, bytes_22468);
            if (err != FUTHARK_SUCCESS)
                goto cleanup;
        }
        // graph_orientation_futhark.fut:381:8-384:27
        
        int64_t discard_21860;
        int64_t scanacc_21854 = (int64_t) 0;
        
        for (int64_t i_21857 = 0; i_21857 < i32_res_17102; i_21857++) {
            // graph_orientation_futhark.fut:382:18-30
            
            int32_t i64_res_20876 = sext_i64_i32(i_21857);
            
            // graph_orientation_futhark.fut:383:21-24
            
            int32_t tmp_20877 = add32(zp_lhs_17104, i64_res_20876);
            
            // graph_orientation_futhark.fut:383:13-25
            
            int64_t tmp_20878 = sext_i32_i64(tmp_20877);
            
            // graph_orientation_futhark.fut:383:13-25
            
            bool x_20879 = sle64((int64_t) 0, tmp_20878);
            
            // graph_orientation_futhark.fut:383:13-25
            
            bool y_20880 = slt64(tmp_20878, i32_res_17111);
            
            // graph_orientation_futhark.fut:383:13-25
            
            bool bounds_check_20881 = x_20879 && y_20880;
            
            // graph_orientation_futhark.fut:383:13-25
            
            bool index_certs_20882;
            
            if (!bounds_check_20881) {
                set_error(ctx, msgprintf("Error: %s%lld%s%lld%s\n\nBacktrace:\n%s", "Index [", (long long) tmp_20878, "] out of bounds for array of shape [", (long long) i32_res_17111, "].", "-> #0  graph_orientation_futhark.fut:383:13-25\n"));
                err = FUTHARK_PROGRAM_ERROR;
                goto cleanup;
            }
            // graph_orientation_futhark.fut:383:13-25
            
            bool lifted_lambda_res_20883 = ((bool *) ext_mem_22467.mem)[ext_22465 + tmp_20878 * ext_22464];
            
            // graph_orientation_futhark.fut:381:8-384:27
            
            int64_t defunc_0_f_res_20884 = btoi_bool_i64(lifted_lambda_res_20883);
            
            // graph_orientation_futhark.fut:381:8-384:27
            
            int64_t defunc_0_op_res_18278 = add64(defunc_0_f_res_20884, scanacc_21854);
            
            ((int64_t *) mem_22469)[i_21857] = defunc_0_op_res_18278;
            ((int64_t *) mem_22471)[i_21857] = defunc_0_f_res_20884;
            
            int64_t scanacc_tmp_22878 = defunc_0_op_res_18278;
            
            scanacc_21854 = scanacc_tmp_22878;
        }
        discard_21860 = scanacc_21854;
        if (memblock_unref(ctx, &ext_mem_22467, "ext_mem_22467") != 0)
            return 1;
        // graph_orientation_futhark.fut:381:8-384:27
        
        int64_t m_f_res_18287;
        
        if (x_18280) {
            // graph_orientation_futhark.fut:381:8-384:27
            
            int64_t x_21761 = ((int64_t *) mem_22469)[tmp_18281];
            
            m_f_res_18287 = x_21761;
        } else {
            m_f_res_18287 = (int64_t) 0;
        }
        // graph_orientation_futhark.fut:381:8-384:27
        
        int64_t m_18289;
        
        if (cond_18279) {
            m_18289 = (int64_t) 0;
        } else {
            m_18289 = m_f_res_18287;
        }
        // graph_orientation_futhark.fut:381:8-384:27
        
        int64_t bytes_22484 = (int64_t) 8 * m_18289;
        
        // graph_orientation_futhark.fut:381:8-384:27
        if (memblock_alloc(ctx, &mem_22485, bytes_22484, "mem_22485")) {
            err = 1;
            goto cleanup;
        }
        // graph_orientation_futhark.fut:381:8-384:27
        
        bool acc_cert_20846;
        
        // graph_orientation_futhark.fut:381:8-384:27
        for (int64_t i_21862 = 0; i_21862 < i32_res_17102; i_21862++) {
            int64_t eta_p_20861 = ((int64_t *) mem_22471)[i_21862];
            int64_t eta_p_20862 = ((int64_t *) mem_22469)[i_21862];
            
            // graph_orientation_futhark.fut:381:8-384:27
            
            bool cond_20865 = eta_p_20861 == (int64_t) 1;
            
            // graph_orientation_futhark.fut:381:8-384:27
            
            int64_t lifted_lambda_res_20866;
            
            if (cond_20865) {
                // graph_orientation_futhark.fut:381:8-384:27
                
                int64_t lifted_lambda_res_t_res_21762 = sub64(eta_p_20862, (int64_t) 1);
                
                lifted_lambda_res_20866 = lifted_lambda_res_t_res_21762;
            } else {
                lifted_lambda_res_20866 = (int64_t) -1;
            }
            // graph_orientation_futhark.fut:381:8-384:27
            // UpdateAcc
            if (sle64((int64_t) 0, lifted_lambda_res_20866) && slt64(lifted_lambda_res_20866, m_18289)) {
                ((int64_t *) mem_22485.mem)[lifted_lambda_res_20866] = i_21862;
            }
        }
        if (memblock_set(ctx, &ext_mem_22487, &mem_22485, "mem_22485") != 0)
            return 1;
        violators_18175 = m_18289;
    }
    if (memblock_unref(ctx, &ext_mem_21959, "ext_mem_21959") != 0)
        return 1;
    if (memblock_unref(ctx, &ext_mem_22413, "ext_mem_22413") != 0)
        return 1;
    if (memblock_set(ctx, &mem_out_22654, &mem_22429, "mem_22429") != 0)
        return 1;
    if (memblock_set(ctx, &mem_out_22655, &mem_22431, "mem_22431") != 0)
        return 1;
    if (memblock_set(ctx, &mem_out_22656, &ext_mem_22487, "ext_mem_22487") != 0)
        return 1;
    prim_out_22657 = i32_res_17103;
    prim_out_22658 = violators_18175;
    prim_out_22659 = ok_18117;
    if (memblock_set(ctx, &*mem_out_p_22883, &mem_out_22654, "mem_out_22654") != 0)
        return 1;
    if (memblock_set(ctx, &*mem_out_p_22884, &mem_out_22655, "mem_out_22655") != 0)
        return 1;
    if (memblock_set(ctx, &*mem_out_p_22885, &mem_out_22656, "mem_out_22656") != 0)
        return 1;
    *out_prim_out_22886 = prim_out_22657;
    *out_prim_out_22887 = prim_out_22658;
    *out_prim_out_22888 = prim_out_22659;
    
  cleanup:
    {
        free(mem_21965);
        free(mem_21967);
        free(mem_21981);
        free(mem_21983);
        free(mem_22010);
        free(mem_22012);
        free(mem_22026);
        free(mem_22099);
        free(mem_22101);
        free(mem_22115);
        free(mem_22117);
        free(mem_22150);
        free(mem_22152);
        free(mem_22166);
        free(mem_22168);
        free(mem_22416);
        free(mem_22469);
        free(mem_22471);
        if (memblock_unref(ctx, &mem_22485, "mem_22485") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22859, "mem_param_tmp_22859") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22858, "mem_param_tmp_22858") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22870, "mem_param_tmp_22870") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22869, "mem_param_tmp_22869") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22449, "mem_22449") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22447, "mem_22447") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22452, "ext_mem_22452") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22455, "ext_mem_22455") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22446, "mem_param_22446") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22443, "mem_param_22443") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22460, "ext_mem_22460") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22461, "ext_mem_22461") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22440, "mem_param_22440") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22437, "mem_param_22437") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22466, "ext_mem_22466") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22467, "ext_mem_22467") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22434, "mem_22434") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22432, "mem_22432") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22486, "mem_22486") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22487, "ext_mem_22487") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22431, "mem_22431") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22429, "mem_22429") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22800, "mem_param_tmp_22800") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22799, "mem_param_tmp_22799") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22798, "mem_param_tmp_22798") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22797, "mem_param_tmp_22797") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22176, "mem_22176") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22179, "mem_22179") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22180, "ext_mem_22180") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22816, "mem_param_tmp_22816") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22815, "mem_param_tmp_22815") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22814, "mem_param_tmp_22814") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22125, "mem_22125") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22128, "ext_mem_22128") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22829, "mem_param_tmp_22829") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22828, "mem_param_tmp_22828") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22079, "ext_mem_22079") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22083, "ext_mem_22083") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22069, "mem_22069") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22067, "mem_22067") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22072, "ext_mem_22072") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22076, "ext_mem_22076") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22086, "ext_mem_22086") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22090, "ext_mem_22090") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22064, "mem_param_22064") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22061, "mem_param_22061") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22096, "ext_mem_22096") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22097, "ext_mem_22097") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22131, "ext_mem_22131") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22134, "ext_mem_22134") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22138, "ext_mem_22138") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22057, "mem_param_22057") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22054, "mem_param_22054") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22051, "mem_param_22051") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22146, "ext_mem_22146") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22147, "ext_mem_22147") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22148, "ext_mem_22148") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22047, "mem_param_22047") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22044, "mem_param_22044") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22041, "mem_param_22041") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22037, "mem_param_22037") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22190, "ext_mem_22190") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22191, "ext_mem_22191") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22192, "ext_mem_22192") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22193, "ext_mem_22193") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22028, "mem_22028") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22793, "mem_param_tmp_22793") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22004, "mem_22004") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22001, "mem_param_22001") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22008, "ext_mem_22008") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_21961, "mem_21961") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22787, "mem_param_tmp_22787") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_21994, "mem_21994") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_21992, "mem_param_21992") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_21997, "ext_mem_21997") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_21963, "mem_21963") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22712, "mem_param_tmp_22712") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22741, "mem_param_tmp_22741") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22776, "mem_param_tmp_22776") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22296, "mem_22296") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22293, "mem_param_22293") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22300, "ext_mem_22300") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22304, "ext_mem_22304") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22308, "ext_mem_22308") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22283, "mem_22283") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22286, "mem_22286") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22287, "ext_mem_22287") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22764, "mem_param_tmp_22764") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22278, "mem_22278") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22275, "mem_param_22275") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22281, "ext_mem_22281") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22272, "mem_22272") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22288, "mem_22288") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22289, "ext_mem_22289") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22751, "mem_param_tmp_22751") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22750, "mem_param_tmp_22750") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22253, "mem_22253") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22251, "mem_22251") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22256, "ext_mem_22256") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22259, "ext_mem_22259") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22262, "ext_mem_22262") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22265, "ext_mem_22265") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22249, "mem_param_22249") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22246, "mem_param_22246") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22270, "ext_mem_22270") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22271, "ext_mem_22271") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22239, "mem_param_22239") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22312, "ext_mem_22312") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22243, "mem_22243") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22241, "mem_22241") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22316, "ext_mem_22316") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22722, "mem_param_tmp_22722") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22721, "mem_param_tmp_22721") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22733, "mem_param_tmp_22733") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22732, "mem_param_tmp_22732") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22217, "mem_22217") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22215, "mem_22215") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22220, "ext_mem_22220") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22223, "ext_mem_22223") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22213, "mem_param_22213") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22210, "mem_param_22210") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22228, "ext_mem_22228") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22229, "ext_mem_22229") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22207, "mem_param_22207") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22204, "mem_param_22204") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22234, "ext_mem_22234") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22235, "ext_mem_22235") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22197, "mem_param_22197") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22320, "ext_mem_22320") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22201, "mem_22201") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22199, "mem_22199") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22324, "ext_mem_22324") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22666, "mem_param_tmp_22666") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22707, "mem_param_tmp_22707") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22389, "mem_22389") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22386, "mem_param_22386") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22393, "ext_mem_22393") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22397, "ext_mem_22397") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22401, "ext_mem_22401") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22378, "mem_22378") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22381, "mem_22381") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22382, "ext_mem_22382") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22697, "mem_param_tmp_22697") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22373, "mem_22373") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22370, "mem_param_22370") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22376, "ext_mem_22376") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22367, "mem_22367") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22405, "ext_mem_22405") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22676, "mem_param_tmp_22676") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22675, "mem_param_tmp_22675") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22688, "mem_param_tmp_22688") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22687, "mem_param_tmp_22687") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22348, "mem_22348") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22346, "mem_22346") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22351, "ext_mem_22351") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22354, "ext_mem_22354") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22344, "mem_param_22344") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22341, "mem_param_22341") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22359, "ext_mem_22359") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22360, "ext_mem_22360") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22338, "mem_param_22338") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22335, "mem_param_22335") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22365, "ext_mem_22365") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22366, "ext_mem_22366") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_22328, "mem_param_22328") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22409, "ext_mem_22409") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22332, "mem_22332") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_22330, "mem_22330") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_22413, "ext_mem_22413") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22664, "mem_param_tmp_22664") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_21955, "ext_mem_21955") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_21953, "mem_21953") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_21950, "mem_param_21950") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_21959, "ext_mem_21959") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_tmp_22662, "mem_param_tmp_22662") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_21942, "ext_mem_21942") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_21941, "ext_mem_21941") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_21940, "ext_mem_21940") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_21938, "mem_21938") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_param_21935, "mem_param_21935") != 0)
            return 1;
        if (memblock_unref(ctx, &ext_mem_21946, "ext_mem_21946") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_21931, "mem_21931") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_out_22656, "mem_out_22656") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_out_22655, "mem_out_22655") != 0)
            return 1;
        if (memblock_unref(ctx, &mem_out_22654, "mem_out_22654") != 0)
            return 1;
    }
    return err;
}

int futhark_entry_graph_orientation_solve(struct futhark_context *ctx, bool *out0, struct futhark_i64_1d **out1, struct futhark_i64_1d **out2, struct futhark_i64_1d **out3, const int32_t in0, const int32_t in1, const int32_t in2, const struct futhark_i64_1d *in3, const struct futhark_i32_1d *in4, const struct futhark_i32_1d *in5)
{
    int64_t dz2080U_14788 = (int64_t) 0;
    int64_t dz2081U_14789 = (int64_t) 0;
    int64_t dz2082U_14790 = (int64_t) 0;
    int32_t mode_14791 = 0;
    int32_t n_14792 = 0;
    int32_t m_14793 = 0;
    int64_t prim_out_22657 = (int64_t) 0;
    int64_t prim_out_22658 = (int64_t) 0;
    bool prim_out_22659 = 0;
    int ret = 0;
    
    lock_lock(&ctx->lock);
    
    struct memblock mem_out_22656;
    
    mem_out_22656.references = NULL;
    
    struct memblock mem_out_22655;
    
    mem_out_22655.references = NULL;
    
    struct memblock mem_out_22654;
    
    mem_out_22654.references = NULL;
    
    struct memblock ev_mem_21928;
    
    ev_mem_21928.references = NULL;
    
    struct memblock eu_mem_21927;
    
    eu_mem_21927.references = NULL;
    
    struct memblock bounds_mem_21926;
    
    bounds_mem_21926.references = NULL;
    mode_14791 = in0;
    n_14792 = in1;
    m_14793 = in2;
    bounds_mem_21926 = in3->mem;
    dz2080U_14788 = in3->shape[0];
    eu_mem_21927 = in4->mem;
    dz2081U_14789 = in4->shape[0];
    ev_mem_21928 = in5->mem;
    dz2082U_14790 = in5->shape[0];
    if (!(dz2080U_14788 == in3->shape[0] && (dz2081U_14789 == in4->shape[0] && dz2082U_14790 == in5->shape[0]))) {
        ret = 1;
        set_error(ctx, msgprintf("Error: entry point arguments have invalid sizes.\n"));
    }
    if (ret == 0) {
        ret = futrts_entry_graph_orientation_solve(ctx, &mem_out_22654, &mem_out_22655, &mem_out_22656, &prim_out_22657, &prim_out_22658, &prim_out_22659, bounds_mem_21926, eu_mem_21927, ev_mem_21928, dz2080U_14788, dz2081U_14789, dz2082U_14790, mode_14791, n_14792, m_14793);
        if (ret == 0) {
            *out0 = prim_out_22659;
            assert((*out1 = (struct futhark_i64_1d *) malloc(sizeof(struct futhark_i64_1d))) != NULL);
            (*out1)->mem = mem_out_22654;
            (*out1)->shape[0] = prim_out_22657;
            assert((*out2 = (struct futhark_i64_1d *) malloc(sizeof(struct futhark_i64_1d))) != NULL);
            (*out2)->mem = mem_out_22655;
            (*out2)->shape[0] = prim_out_22657;
            assert((*out3 = (struct futhark_i64_1d *) malloc(sizeof(struct futhark_i64_1d))) != NULL);
            (*out3)->mem = mem_out_22656;
            (*out3)->shape[0] = prim_out_22658;
        }
    }
    lock_unlock(&ctx->lock);
    return ret;
}
  
