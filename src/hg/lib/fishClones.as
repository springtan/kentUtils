table fishClones
"Describes the positions of fishClones in the assembly"
   (
   string chrom;                   "Reference sequence chromosome or scaffold"
   uint   chromStart;              "Start position in chromosome"
   uint   chromEnd;                "End position in chromosome"
   string name;                    "Name of clone"
   uint score;                     "Always 1000"
   uint placeCount;                "Number of times FISH'd"
   string[placeCount] bandStarts;  "Start FISH band"
   string[placeCount] bandEnds;    "End FISH band"
   string[placeCount] labs;        "Lab where clone FISH'd"
   string placeType;               "How clone was placed on the sequence assembly"
   uint accCount;                  "Number of accessions associated with the clone"
   string[accCount] accNames;      "Accession associated with clone"
   uint stsCount;                  "Number of STS markers associated with this clone"
   string[stsCount] stsNames;      "Names of STS  markers"
   uint beCount;                   "Number of BAC end sequences associated with this clone"
   string[beCount] beNames;        "Accessions of BAC ends"
   )
