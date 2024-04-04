#include "utils.h"

typedef struct{
    int   parent;
    char *rank;
    char *name;
} Node;

KHASH_MAP_INIT_INT(krona, Node *)

void kh_krona_destroy(khash_t(krona) *h);
void krona_traversal(const khash_t(krona) *h, const khash_t(reg) *map, int taxon);
void krona_reg_init(khash_t(reg) *h);
inline void krona_lineage();

static kstring_t kv = {0, 0, 0};

int krona_main(int argc, char *argv[]){

    if (argc != 3) {
        fprintf(stderr, "\nUsage: taxon-utils krona <taxon.map> <kreport>\n\n");
        return 1;
    }

    khash_t(krona) *h;
    h = kh_init(krona);

    khash_t(reg) *map;
    map = kh_init(reg);
    krona_reg_init(map);

    khint_t    k;
    kstream_t *ks;

    int *fields, n, ret;

    gzFile fp;
    fp = strcmp(argv[1], "-")? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r"); 
    kstring_t kt = {0, 0, 0};
    
    if (fp) {        
        int node1, node2;
        ks  = ks_init(fp);
        
        while(ks_getuntil( ks, '\n', &kt, 0) >= 0 ){

            fields = ksplit(&kt, '\t', &n);            
            node1  = atoi( kt.s );
            node2  = atoi( kt.s + fields[1] );

            k = kh_put(krona, h, node1, &ret);
            kh_key(h, k) = node1;

            Node *node   = (Node *)malloc( sizeof(Node) );
            if( node != NULL ){
                node->parent  = node2;
                node->rank    = strdup( kt.s + fields[2] );
                node->name    = strdup( kt.s + fields[3] );
                kh_val(h, k) = node;
            }
        }
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR] can't open file %s\n", argv[1]);
        exit(1);
    }

    
    fp = strcmp(argv[2], "-")? gzopen(argv[2], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {
        
       ks  = ks_init(fp);
       ks_getuntil( ks, '\n', &kt, 0);

       while(ks_getuntil( ks, '\n', &kt, 0) >= 0 ){
            
            fields     = ksplit(&kt, '\t', &n);
            int taxon  = atoi( kt.s + fields[1] );
            
            kv.l = 0;
            krona_traversal(h, map, taxon);
            printf("%s\t", kt.s + fields[6]);
            krona_lineage();
        
        }

        ks_destroy(ks);
        gzclose(fp);
    }else{
        fprintf(stderr, "[ERR]can't open file %s\n", argv[2]);
        exit(1);
    }

    free(kt.s);
    kh_krona_destroy( h );
    return 0;
}

void kh_krona_destroy(khash_t(krona) *h){
    
    khint_t k;
    if (h == 0) return;
    for (k = 0; k < kh_end(h); ++k) {
        if (kh_exist(h, k)){
            Node *node = kh_val(h, k);
            free(node->name);
            free(node->rank);
            free(node);
        }
    }
    kh_destroy(krona, h);
}

inline void krona_lineage(){
    
    if(kv.l != 0 ){
        int *fields, i, n;
        fields = ksplit(&kv, ';', &n);
        for (i = n - 1; i >= 0; i--){
            if(i != n - 1) putchar('\t');
            fputs(kv.s + fields[i], stdout);
        }
        putchar('\n');
    }else  fputs("Unclassified\n", stdout);

}

void krona_traversal(const khash_t(krona) *h, const khash_t(reg) *map, int taxon){
    
    khint_t k, t;

    k  = kh_get(krona, h, taxon); 
    if(k == kh_end(h) ){ 
        kputs("Unclassified", &kv);
        return;
    }

    while(1){

        t  = kh_get(reg, map, kh_val(h, k)->rank);   
        if(t != kh_end(map)){
            if(kv.l != 0) kputc(';', &kv);
            ksprintf(&kv, "%s:%s", kh_val(map, t), kh_val(h, k)->name);
        }

        if(kh_key(h, k) == 1) return;
        
        k = kh_get(krona, h, kh_val(h, k)->parent);
        if( k == kh_end( h ) ) return;

    }
}

void krona_reg_init(khash_t(reg) *h){
    
    kstring_t ks = {0, 0, 0};
    ksprintf(&ks, "species s genus g family f order o class c phylum p superkingdom d");

    int *fields, i, n, ret;
    khint_t  k;
    fields = ksplit(&ks, ' ', &n);

    for (i = 0; i < n - 1; i += 2){
        k = kh_put(reg, h, ks.s + fields[i] , &ret);
        kh_key(h, k) = strdup(ks.s + fields[i]);
        kh_val(h, k) = strdup(ks.s + fields[i + 1]);
    }

}
