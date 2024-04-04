#include "utils.h"

KHASH_MAP_INIT_INT(bin, node_t*)
static char* level;

void kh_bin_destroy(khash_t(bin) *h){
    
    khint_t k;
    if (h == 0) return;
    for (k = kh_begin(h); k != kh_end(h); ++k){
        if (kh_exist(h, k)) {           
           node_t *node = kh_val(h, k);
           free(node->name);
           free(node->rank);
           free(node);
        }
    }
    kh_destroy(bin, h);

}

int bin_traversal(const khash_t(bin) *h, int taxon){
    
    khint_t k;

    while(1){
        k  = kh_get(bin, h, taxon);
        if(k == kh_end(h) ) return 0;
        if(strcmp (kh_val(h, k)->rank, level ) == 0){
            return taxon;
        }else if(taxon == 1){
            return 0;
        }else{
            taxon = kh_val(h, k)->parent;
        }
    }

}

int bin_main(int argc, char *argv[]){

    int c;
    level = "species";

    while ((c = getopt(argc, argv, "l:")) >= 0) {
        if (c == 'l') level = optarg;
    }

    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\nUsage: taxon-utils bin [options] <taxon.map> <taxon>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -l STR   taxon levels [phylum|order|class|family|order|genus|species>] default: [species]\n\n");
        return 1;
    }

    kstream_t *ks;
    kstring_t kt = {0, 0, 0};
    int *fields, n, ret;
    int node1, node2;

    khash_t(bin) *h;
    h = kh_init(bin);
    khint_t  k;

    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");  

    if (fp) {

       ks = ks_init(fp);
       while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n);
            
            node1  = atoi( kt.s + fields[0] );
            node2  = atoi( kt.s + fields[1] );

            k = kh_put(bin, h, node1, &ret);
            kh_key(h, k) = node1;

            node_t *node   = (node_t *)malloc( sizeof(node_t) );
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

        fprintf(stderr, "[ERR] %s :file streaming error\n", argv[optind]);
    
    }
    
    fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");  
    
    if (fp) {
       ks = ks_init(fp);  
       while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields     = ksplit(&kt, '\t', &n);            
            int taxon  = atoi( kt.s + fields[1] );         
            int   hit  = bin_traversal(h, taxon);
                    k  = kh_get(bin, h, hit);
            
            if(hit != 0)
                printf("%s\t%d\t%s\n", kt.s, hit, kh_val(h, k)->name);
            else
                printf("%s\t%d\tOthers\n", kt.s, hit);

        }

        ks_destroy(ks);
        gzclose(fp);
    
    }else{

        fprintf(stderr, "[ERR] %s :file streaming error\n", argv[optind + 1]);
    
    }
   
    free(kt.s);
    kh_bin_destroy( h );
    return 0;
}
