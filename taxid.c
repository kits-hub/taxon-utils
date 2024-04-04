#include "utils.h"

int taxid_main(int argc, char *argv[]){
    
    int  c;
    int  target      = 1;
    char *placehold  = "-";
    int  remove      = 0;
    char level       = 's';

    while ((c = getopt(argc, argv, "c:rs:l:")) >= 0) {       
        
        if(c == 'r') remove = 1;
        else if (c == 'c') target = atoi(optarg) - 1;
        else if (c == 's') placehold  = optarg;
        else if (c == 'l') level  =  optarg[0];
  
    }

    if ( optind == argc || argc != optind + 2 ) {
 
        fprintf(stderr, "\nUsage: taxon-utils taxid  <taxon.map> <maps>\n\n");
        fprintf(stderr, "Options:\n");
        fprintf(stderr, "  -c INT   column for taxid translate, default: [2]\n");
        fprintf(stderr, "  -r       remove items without match.\n");
        fprintf(stderr, "  -l       level for query taxonomy name. default: [s]\n");
        fprintf(stderr, "  -s STR   display strings if without match, default: [-]\n\n");
        return 1;
    
    }

    kstring_t kt    = {0, 0, 0};
    int *fields, ret, n;
    kstring_t kswp  = {0, 0, 0};

    khash_t(reg) *h;
    h = kh_init(reg);
    khint_t k;

    kstream_t *ks;
    gzFile fp;
    fp = strcmp(argv[optind], "-")? gzopen(argv[optind], "r") : gzdopen(fileno(stdin), "r"); 
    char q, p;

    if(fp){
        ks  = ks_init(fp);
        while( ks_getuntil( ks, '\n', &kt, 0) >=  0){

            fields = ksplit(&kt, '\t', &n);
            q= (kt.s + fields[2])[0];
            if(q == 's'){
                p = (kt.s + fields[2])[2];
                if(p == 'b')      q = 't';
                else if(p == 'p') q = 'k';
                else if(p == 'e') q = 's';
            }

            kswp.l = 0;
            ksprintf(&kswp, "%c:%s", q, kt.s + fields[3] );
            k = kh_put(reg, h, kswp.s, &ret);
            kh_key(h, k)   = strdup(kswp.s);
            kh_val(h, k)   = strdup(kt.s);

        }
        ks_destroy(ks);
        gzclose(fp);
    
    }else{
        fprintf(stderr, "[ERR]: can't open file %s\n", argv[optind]);
        exit(-1);
    }

    fp = strcmp(argv[optind + 1], "-")? gzopen(argv[optind + 1], "r") : gzdopen(fileno(stdin), "r");  
    if (fp) {
       
       ks = ks_init(fp);
       kstring_t kp = {0,0,0};

       while( ks_getuntil( ks, '\n', &kt, 0) >=  0){
            
            if(kt.s[0] == '#'){
                printf("%s\ttaxid\n", kt.s); 
                continue;
            }

            kp.l   = 0;
            kputs(kt.s , &kp);
            fields = ksplit( &kt, '\t', &n);
            
            kswp.l = 0;
            ksprintf(&kswp, "%c:%s", level, kt.s + fields[target] );
            k = kh_get(reg, h, kswp.s);
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

    kh_reg_destroy( h );
    free(kt.s);
    free(kswp.s);

    return 0;
}