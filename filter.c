#include "utils.h"

int filter_main(int argc, char *argv[]){
    
    if ( optind == argc || argc != optind + 2 ) {
        
        fprintf(stderr, "\nUsage: taxon-utils filter <taxon> <taxonId:list>\n\n");
        fprintf(stderr, "Example: taxon-utils filter lineage.txt 3,2759\n\n");
        return 1;
    
    }

    khash_t(set) *h;
    h = kh_init(set);
    khint_t k;

    int i,*fields, n, ret;
    kstring_t  kt  = {0, 0, 0};

    kputs( argv[optind + 1], &kt);
    fields = ksplit(&kt, ',', &n);
    for (i = 0; i < n; ++i){
        k = kh_put(set, h, kt.s + fields[i], &ret);
        if(ret) kh_key(h, k) = strdup(kt.s + fields[i]); 
    }

    gzFile     fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r");
    if(fp){
        
        kstream_t  *ks;
        ks       = ks_init(fp);
        kt.l     = 0;
        char *p;
        kstring_t lineage = {0, 0, 0};

        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            lineage.l = 0;
            ks_tokaux_t aux;
            
            int flag,i,j;
            flag = 1;

            for (p = kstrtok(kt.s, "\t", &aux), i = 0; p; p = kstrtok(0, 0, &aux), ++i){
                
                if(i == 3){
                    kputs(p, &lineage);
                    
                    if(strcmp("1", lineage.s) == 0){
                         flag = 0; break;
                    }
                    
                    fields = ksplit(&lineage, ',', &n);
                    for (j = 0; j < n; ++j){
                        k = kh_get(set, h, lineage.s + fields[j]);
                        if ( kh_exist(h, k) ) {
                            flag = 0; break;
                        }
                    }
                
                }
            }
            if(flag) printf("%s\n", kt.s);

        }

        ks_destroy(ks);
        gzclose(fp);

    }else{
        fprintf(stderr, "[ERR]can't open file %s\n", argv[optind]);
        exit(1);
    }

    free(kt.s);
    kh_set_destroy( h );
    return 0;
}