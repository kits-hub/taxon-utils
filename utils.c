#include "utils.h"

void kh_reg_destroy(khash_t(reg) *h){
    
    khint_t k;
    if  (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            free((char*)kh_key(h, k));
            free((char*)kh_val(h, k));
        }
    }
    kh_destroy(reg, h);

}

void kh_kreg_destroy(khash_t(kreg) *h){
    
    khint_t k;
    if  (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            free((char*)kh_key(h, k));
            free((char*)kh_val(h, k).s);
        }
    }
    kh_destroy(kreg, h);

}

void kh_set_destroy(khash_t(set) *h){

    khint_t k;
    if (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) {
            free((char*)kh_key(h, k));
        }
    }
    kh_destroy(set, h);
}

void kh_khint_destroy(khash_t(khint) *h){
    kh_destroy(khint, h);
}

void kh_traversal_destroy(khash_t(traversal) *h){
    
    khint_t k;
    if (h == 0) return;
    
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)){
            node_t *node = kh_val(h, k);
            free(node->name);
            free(node->rank);
            free(node);
        }
    }
    
    kh_destroy(traversal, h);
}

void kh_taxon_init(khash_t(reg) *h){
    
    kstring_t ks = {0, 0, 0};
    ksprintf(&ks, "strain t isolate t subspecies t species s genus g family f order o class c phylum p superkingdom k");

    int *fields, i, n, ret;
    khint_t  k;
    fields = ksplit(&ks, ' ', &n);

    for (i = 0; i < n - 1; i += 2){
        k = kh_put(reg, h, ks.s + fields[i] , &ret);
        if(ret){
            kh_key(h, k) = strdup(ks.s + fields[i]);
            kh_val(h, k) = strdup(ks.s + fields[i + 1]);
        }
    }

}

void kh_int32_destroy(khash_t(int32) *h){
    
    khint_t k;
    if  (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)) free((char *)kh_val(h, k));
    }
    kh_destroy(int32, h);
}