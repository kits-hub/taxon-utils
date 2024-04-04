#include <stdio.h>
#include <string.h>
#include <zlib.h>
#include <stdlib.h>

#include "khash.h"
#include "kstring.h"

#include "kseq.h"

typedef struct{
    int   parent;
    char *rank;
    char *name;
} node_t;

KSTREAM_INIT(gzFile, gzread, 16384)

KHASH_MAP_INIT_STR(reg, char*)
void kh_reg_destroy(khash_t(reg) *h);

KHASH_MAP_INIT_STR(kreg, kstring_t)
void kh_kreg_destroy(khash_t(kreg) *h);

KHASH_MAP_INIT_INT(int32, char *)
void kh_int32_destroy(khash_t(int32) *h);

KHASH_SET_INIT_STR(set)
void kh_set_destroy(khash_t(set) *h);

KHASH_MAP_INIT_INT(khint, int)
void kh_khint_destroy(khash_t(khint) *h);

KHASH_MAP_INIT_INT(int, int)

void kh_taxon_init(khash_t(reg) *h);

KHASH_MAP_INIT_INT(traversal, node_t *);

void kh_taxon_init(khash_t(reg) *h);

void kh_traversal_destroy(khash_t(traversal) *h);