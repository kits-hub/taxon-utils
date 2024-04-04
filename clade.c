#include "utils.h"

static int clade;
int clade_traversal(const khash_t(khint) *h, int node, khint_t k);

int clade_main(int argc, char *argv[]){

    if(argc != 3) {
        fprintf(stderr, "Usage: taxon-utils  clade  <taxon.map>  <clade: NCBI Taxonomy ID, ie: 10239(viruses)>\n\n");
        exit(1);
    }

    clade = atoi(argv[2]);

    khash_t(khint) *h;
    h = kh_init(khint);
    khint_t  k;

    kstring_t kt = {0, 0, 0};
    int *fields, n, ret;

    kstream_t  *kseq;
    gzFile fp;
    fp = strcmp(argv[1], "-")? gzopen(argv[1], "r") : gzdopen(fileno(stdin), "r");

    if (fp) {
   
        int node1, node2;
        kseq = ks_init(fp);

        while (ks_getuntil(kseq, '\n', &kt, 0) >= 0){

            fields = ksplit(&kt, '\t', &n);

            node1  = atoi( kt.s + fields[0] );
            node2  = atoi( kt.s + fields[1] );

            k = kh_put(khint, h, node1, &ret);
            kh_key(h, k) = node1;
            kh_val(h, k) = node2;
       }

       gzclose(fp);
       ks_destroy(kseq);
  
    }else{
        fprintf(stderr, "[ERR]: open %s file failed, exit!\n",  argv[1]);
        exit(-1);
    }
    
    for (k = kh_begin(h); k != kh_end(h); ++k){
        if (kh_exist(h, k)) {
           int key = kh_key(h, k);
           if( clade_traversal( h, key, k) == 1 ){
                printf("%d\n", key);
           }
        }
    }

    free(kt.s);
    kh_khint_destroy(h);

    return 0;
}

int clade_traversal(const khash_t(khint) *h, int node, khint_t k){

    while(1){

        if(node == clade) {
           return 1;
        }else if(node == 1) return 0;

        k = kh_get(khint, h, kh_val(h, k));
        node = kh_key(h, k);
        
        if( ! kh_exist(h, k) ){
           fprintf(stderr,"[ERR]: %d\n", node);
           return 0;
        }
    
    }

}