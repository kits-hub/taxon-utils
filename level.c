#include "utils.h"

static unsigned int level_tab[] = {
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,  
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,
      0,   0,   0,   0,   0,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,
      0,   0,   0,   6,   0,   0,   4,   3,   0,   0,  0,  8,  0,  0,  0,  5,
      7,   0,   0,   2,   1,   0,   0,   0,   0,   0,  0,  0,  0,  0,  0,  0,
};

static unsigned int val   = 0;
static int clade = 0;

int level_traversal(const khash_t(traversal) *h, const khash_t(reg) *map, int taxon);
int level_main(int argc, char *argv[]){

    int  c, column = 1;
    int  node      = 0;
    int  remove    = 0;

    while ((c = getopt(argc, argv, "c:nr")) >= 0) {
        if (c == 'c') column = atoi(optarg) - 1;
        else if (c == 'n') node   = 1;
        else if (c == 'r') remove = 1;

    }

    if ( optind == argc || argc != optind + 3) {
        fprintf(stderr, "\nUsage: taxon-utils level [options] <taxon.map> <tab|taxonIds> <level>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -c INT   column for taxon translate, default: [2]\n");
        fprintf(stderr, "  -n       print lineage for specified node, using ',' for multi taxonId.\n");
        fprintf(stderr, "  -r       remove non-mapped entries.\n\n");
        return 1;
    }

    clade = (int)argv[optind + 2][0];
    val  = level_tab[clade];
 
    if(val == 0){
        fprintf(stderr, "[ERR]: specified levels: in [kpcofgst] %s.\n", argv[optind + 2]);
        return 0;
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
                    
                    if( (kt.s + fields[2])[0] == 's'){
                        switch( (kt.s + fields[2])[2] ){
                            case 'b': 
                                        node->rank    = strdup( "typing" );
                                        break;
                            case 'r':    
                                        node->rank    = strdup( "typing" );
                                        break;
                            case 'e':    
                                        node->rank    = strdup( "species" );
                                        break;
                            case 'p': 
                                        node->rank    = strdup( "kingdom" );
                                        break;
                            default:
                                        node->rank    = strdup( kt.s + fields[2] );
                                        break;
                        }
                    }else
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

    int lev   = 0;
    int taxon = 0;
    if(node){

       kputs(argv[optind + 1], &kt);
       fields = ksplit(&kt, ',', &n);
       int i;
       for (i = 0; i < n; ++i){
           taxon = atoi( kt.s + fields[i] );
           lev = level_traversal(h, map,  taxon);
           printf("%d\t%d\n", taxon, lev);
       }

    }else{

        fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");  
        if (fp) {
           ks = ks_init(fp);
           kstring_t kv = {0, 0, 0};
           while( ks_getuntil( ks, '\n', &kt, 0) >=  0){            
                
                if(kt.l == 0) continue;                
                if(kt.s[0] == '#'){
                    printf("%s\tlevel\n", kt.s);
                    continue;
                }
                kv.l = 0;
                kputs(kt.s, &kv);
                fields = ksplit(&kt, '\t', &n);
                if(column >=  n){
                    fprintf(stderr, "[ERR]: specified target column [%d] exceed filelds: [%d] \n", column + 1, n);
                    return 0;
                }

                taxon = atoi( kt.s + fields[column] );
                lev = level_traversal(h, map, taxon);

                if(lev != 0)
                   printf("%s\t%d\n", kv.s, lev);
                else
                   if(! remove) printf("%s\t%d\n", kv.s, lev);


            }

            free(kv.s);
            ks_destroy(ks);
            gzclose(fp);
        }else{        
            fprintf(stderr, "[ERR]: %s :file streaming error\n", argv[optind + 1]);       
        }

        free(kt.s);

    }

    kh_reg_destroy( map );
    kh_traversal_destroy( h );

    return 0;
}

int level_traversal(const khash_t(traversal) *h, const khash_t(reg) *map, int taxon){
    
    khint_t k;

    k  = kh_get(traversal, h, taxon); 
    if(k == kh_end(h) ) return 0;

    while(1){
        
        if( kh_val(h, k)->rank[0] == clade)
            return kh_key(h, k);
        else if(level_tab[ (int)kh_val(h, k)->rank[0] ] > val)
            return 0;
        
        if(kh_key(h, k) == 1) return  0;
        
        k = kh_get(traversal, h, kh_val(h, k)->parent);
        if( k == kh_end( h ) ) return 0;

    }
}