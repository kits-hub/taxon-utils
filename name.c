#include "utils.h"

int name_main(int argc, char *argv[]){
    
    int c;
    int target      = 1;
    char *placehold = "-";
    int remove      = 0;
    int node        = 0;

    while ((c = getopt(argc, argv, "c:nrs:")) >= 0) {       
        
        if(c == 'r') remove = 1;
        else if (c == 'c') target = atoi(optarg) - 1;
        else if (c == 's') placehold  = optarg;
        else if (c == 'n') node = 1;
  
    }

    if ( optind == argc || argc != optind + 2 ) {
 
        fprintf(stderr, "\nUsage: taxon-utils name  <taxon.map> <maps|taxonIds>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -c INT   column for taxon translate, default: [2]\n");
        fprintf(stderr, "  -n       print lineage for specified node, using ',' for multi taxonId.\n");
        fprintf(stderr, "  -R       remove items without match.\n");
        fprintf(stderr, "  -s STR   display strings if without match, default: [-]\n\n");
        return 1;
    
    }

    kstring_t kt    = {0, 0, 0};
    int *fields, ret, n;

    khash_t(reg) *h;
    h = kh_init(reg);
    khint_t k;

    kstream_t *ks;
    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 

    if(fp){
        
        ks  = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n);
            k = kh_put(reg, h, kt.s, &ret);
            kh_key(h, k)   = strdup(kt.s);
            kh_val(h, k)   = strdup(kt.s + fields[3]);

        }
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(-1);
    }
    
    if(node){

       kputs(argv[optind + 1], &kt);
       fields = ksplit(&kt, ',', &n);
       int i;
       for (i = 0; i < n; ++i){
           k = kh_get(reg, h, kt.s +fields[i]);
           if(k != kh_end(h)){
               printf("%s\t%s\n",  kt.s +fields[i], kh_val(h, k));
           }else{
               if(!remove) printf("%s\t%s\n",  kt.s +fields[i], placehold);
           }
       }

    }else{

        fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");  
        if (fp) {
           
           ks = ks_init(fp);
           kstring_t kp = {0,0,0};

           while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
                
                if(kt.s[0] == '#'){
                    printf("%s\tname\n", kt.s); 
                    continue;
                }

                kp.l   = 0;
                kputs(kt.s , &kp);
                fields = ksplit( &kt, '\t', &n);
                k = kh_get(reg, h, kt.s + fields[target]);

                if(k != kh_end(h)){
                   printf("%s\t%s\n", kp.s, kh_val(h, k));
                }else{
                   if(!remove) printf("%s\t%s\n", kp.s, placehold);
                }       

            }
            free(kp.s);
            ks_destroy(ks);
            gzclose(fp);

        }else{        
            fprintf(stderr, "[ERR]: %s :file streaming error\n", argv[optind + 1]);       
        }
    }

    kh_reg_destroy( h );
    free(kt.s);
    return 0;
}