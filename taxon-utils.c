/*****************************************************************************
  taxon-utils.c

  centrifuge-utils command line interface.  

  (c) 2018-2024 - LEI ZHANG
  Logic Informatics Co.,Ltd.
  zhanglei@logicinformatics.com
  
  Licenced under The MIT License.
******************************************************************************/

#include <stdio.h>
#include <string.h>

int translate_main(int argc, char *argv[]);
int lineage_main(int argc, char *argv[]);
int name_main(int argc, char *argv[]);
int lca_main(int argc, char *argv[]);
int clade_main(int argc, char *argv[]);
int bin_main(int argc, char *argv[]);
int filter_main(int argc, char *argv[]);
int level_main(int argc, char *argv[]);
int rank_main(int argc, char *argv[]);
int taxid_main(int argc, char *argv[]);

static int usage(){
    
    fprintf(stderr, "\n");
    fprintf(stderr, "Usage:   taxon-utils <command> <arguments>\n");
    fprintf(stderr, "Version: 0.0.1\n\n");

    fprintf(stderr, "Command:\n");
    fprintf(stderr, "      translate   translate the taxon_id map to lineage.\n");
    fprintf(stderr, "      lineage     translate the taxon_id map to full lineage.\n");
    fprintf(stderr, "      name        translate the taxon_id map to name.\n");
    fprintf(stderr, "      rank        translate the taxon_id map to rank.\n");
    fprintf(stderr, "      lca         perform lca for given taxon match per query.\n");
    fprintf(stderr, "      bin         bin reads for specify level. default: [species] .\n");
    fprintf(stderr, "      clade       get clade taxon id from NCBI taxonomy tree.\n");
    fprintf(stderr, "      filter      filter the entries with specified taxonIds.\n");
    fprintf(stderr, "      level       bin taxon to higher level. [pcofgs] .\n");
    fprintf(stderr, "      taxid       convert specified level taxon name to taxid. [kpcofgst] .\n");
    fprintf(stderr, "\n");

    fprintf(stderr, "Licenced:\n");
    fprintf(stderr, "(c) 2018-2024 - LEI ZHANG\n");
    fprintf(stderr, "Logic Informatics Co.,Ltd.\n");
    fprintf(stderr, "zhanglei@logicinformatics.com\n");
    fprintf(stderr, "\n");

    return 1;
}

int main(int argc, char *argv[]){

    if (argc < 2) return usage();
    
    if (strcmp(argv[1], "translate") == 0)      translate_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "lineage") == 0)   lineage_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "name") == 0)      name_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "rank") == 0)      rank_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "lca") == 0)       lca_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "clade") == 0)     clade_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "bin") == 0)       bin_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "filter") == 0)    filter_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "level") == 0)     level_main(argc - 1, argv + 1);
    else if (strcmp(argv[1], "taxid") == 0)     taxid_main(argc - 1, argv + 1);
    else {
        fprintf(stderr, "[main] unrecognized command '%s'. Abort!\n", argv[1]);
        return 1;
    }
    return 0;

}