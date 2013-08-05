table encodeStanfordPromoters
"Stanford Promoter Activity in ENCODE Regions (bed 9+)"
    (
    string chrom;      "Reference sequence chromosome or scaffold"
    uint   chromStart; "Start position in chromosome"
    uint   chromEnd;   "End position in chromosome"
    string name;       "accession of mRNA used to predict the promoter"
    uint   score;      "Score from 0-1000"
    char[1] strand;    "+ or -"
    uint thickStart;   "Placeholder for BED9 format -- same as chromStart"
    uint thickEnd;     "Placeholder for BED9 format -- same as chromEnd"
    uint reserved;     "Used as itemRgb"
    string geneModel;  "Gene model ID; same ID may have multiple predicted promoters"
    string description; "Gene description"
    int lucA;          "Luciferase signal A"
    int renA;          "Renilla signal A"
    int lucB;          "Luciferase signal B"
    int renB;          "Renilla signal B"
    float avgRatio;    "Average Luciferase/Renilla Ratio"
    float normRatio;   "Normalized Luciferase/Renilla Ratio"
    float normLog2Ratio; "Normalized and log2 transformed Luciferase Renilla Ratio"
    )
