### taxon-utils: utils for NCBI taxonomy manipulation
<hr>

#### install
git clone https://github.com/kits-hub/taxon-utils
cd taxon-utils && make
```

```

#### Usage

```
Usage:   taxon-utils <command> <arguments>
Version: 0.0.1

Command:
      translate   translate the taxon_id map to lineage.
      lineage     translate the taxon_id map to full lineage.
      name        translate the taxon_id map to name.
      rank        translate the taxon_id map to rank.
      lca         perform lca for given taxon match per query.
      bin         bin reads for specify level. default: [species] .
      clade       get clade taxon id from NCBI taxonomy tree.
      filter      filter the entries with specified taxonIds.
      level       bin taxon to higher level. [pcofgs] .
      taxid       convert specified level taxon name to taxid. [kpcofgst] .

Licenced:
(c) 2018-2024 - LEI ZHANG
Logic Informatics Co.,Ltd.
zhanglei@logicinformatics.com
```