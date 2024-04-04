#include "utils.h"

void traversal(const khash_t(traversal) *h, const khash_t(reg) *map, int taxon);
void print_lineage();

static kstring_t kv = {0, 0, 0};
static kstring_t kl = {0, 0, 0};

int translate_main(int argc, char *argv[]){

    int c, column = 1;
    int node = 0;

    while ((c = getopt(argc, argv, "c:n")) >= 0) {
        if (c == 'c') column = atoi(optarg) - 1;
        else if (c == 'n') node = 1;
    }

    if ( optind == argc || argc != optind + 2) {
        fprintf(stderr, "\nUsage: taxon-utils translate [options] <taxon.map> <tab|taxonIds>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -c INT   column for taxon translate, default: [2]\n");
        fprintf(stderr, "  -n       print lineage for specified node, using ',' for multi taxonId.\n\n");
        return 1;
    }

    khash_t(traversal) *h;
    h = kh_init(traversal);

    khash_t(reg) *map;
    map = kh_init(reg);
    kh_taxon_init(map);

    khint_t    k;
    kstream_t *ks;

    int *fields, n;
    kstring_t kt = {0, 0, 0};
    
    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 
    if (fp) {
       
       ks = ks_init(fp);  
       int ret, node1, node2;

       while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
        
            fields = ksplit(&kt, '\t', &n);
            
            node1  = atoi( kt.s + fields[0] );
            node2  = atoi( kt.s + fields[1] );

            k = kh_put(traversal, h, node1, &ret);
            if(ret){

                kh_key(h, k) = node1;
                node_t *node   = (node_t *)malloc( sizeof(node_t) );
                
                if( node != NULL ){
                    node->parent  = node2;
                    node->rank    = strdup( kt.s + fields[2] );
                    node->name    = strdup( kt.s + fields[3] );
                    kh_val(h, k) = node;
                }
            }
        }
        ks_destroy(ks);
        gzclose(fp);
    }else{
     
        fprintf(stderr, "[ERR]: %s :file streaming error\n", argv[optind]);
    
    }
    
    int taxon = 0;
    if(node){
    
       kputs(argv[optind + 1], &kt);
       fields = ksplit(&kt, ',', &n);
       int i;
       for (i = 0; i < n; ++i){
           kv.l = kl.l = 0;
           taxon = atoi( kt.s + fields[i] );
           traversal(h, map,  taxon);
           print_lineage();
       }

    }else{

        fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");  
        if (fp) {
           ks = ks_init(fp);
           
           while( ks_getuntil( ks, '\n', &kt, 0) >=  0){            
                
                fputs(kt.s, stdout);
                fputc('\t', stdout);

                if(kt.s[0] == '#'){
                    printf("taxon_lineage\ttaxid_lineage\n"); 
                    continue;
                }

                fields = ksplit(&kt, '\t', &n);
                if(column >=  n){
                    fprintf(stderr, "[ERR]: specified target column [%d] exceed filelds: [%d] \n", column + 1, n);
                    return 0;
                }

                taxon = atoi( kt.s + fields[column] );
                kv.l = 0;
                kl.l = 0;

                traversal(h, map, taxon);
                print_lineage();
            
            }

            ks_destroy(ks);
            gzclose(fp);
        }else{        
            fprintf(stderr, "[ERR]: %s :file streaming error\n", argv[optind + 1]);       
        }

        free(kt.s);
        free(kv.s);
        free(kl.s);
    }

    kh_reg_destroy( map );
    kh_traversal_destroy( h );

    return 0;
}

void traversal(const khash_t(traversal) *h, const khash_t(reg) *map, int taxon){
    
    khint_t k, t;

    k  = kh_get(traversal, h, taxon); 
    if(k == kh_end(h) ){ 
        kputs("Unclassified", &kv);
        kputs("0", &kl);
        return;
    }

    while(1){

        t  = kh_get(reg, map, kh_val(h, k)->rank);   
        if(t != kh_end(map)){
            if(kv.l != 0) kputc(0x01, &kv);
            if(kl.l != 0) kputc(0x01, &kl);
            ksprintf(&kv, "%s:%s", kh_val(map, t), kh_val(h, k)->name);
            ksprintf(&kl, "%d", kh_key(h, k));
        }

        if(kh_key(h, k) == 1) return;
        
        k = kh_get(traversal, h, kh_val(h, k)->parent);
        if( k == kh_end( h ) ) return;

    }
}

void print_lineage(){
    
    if(kv.l != 0 ){
        int *fields, i, n;
        fields = ksplit(&kv, 0x01, &n);
        for (i = n - 1; i >= 0; i--){
            if(i != n - 1) putchar(',');
            fputs(kv.s + fields[i], stdout);
        }
    }else  fputs("root", stdout);

    putchar('\t');

    if(kl.l != 0 ){
        int *fields, i, n;
        fields = ksplit(&kl, 0x01, &n);
        for (i = n - 1; i >= 0; i--){
            if(i != n - 1) putchar(',');
            fputs(kl.s + fields[i], stdout);
        }
    }else  fputs("1", stdout);

    putchar('\n');

}