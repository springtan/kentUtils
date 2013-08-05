# peptideMapping.sql was originally generated by the autoSql program, which also 
# generated peptideMapping.c and peptideMapping.h.  This creates the database representation of
# an object which can be loaded and saved from RAM in a fairly 
# automatic way.

#Format for genomic mappings of mass spec proteogenomic hits
CREATE TABLE peptideMapping (
    chrom varchar(255) not null,	# Reference sequence chromosome or scaffold
    chromStart int unsigned not null,	# Start position in chromosome
    chromEnd int unsigned not null,	# End position in chromosome
    name varchar(255) not null,	# Peptide sequence of the hit
    score int unsigned not null,	# Log e-value scaled to a score of 0 (worst) to 1000 (best)
    strand char(1) not null,	# + or -
    rawScore float not null,	# Raw score for this hit, as estimated through HMM analysis
    spectrumId varchar(255) not null,	# Non-unique identifier for the spectrum file
    peptideRank int unsigned not null,	# Rank of this hit, for peptides with multiple genomic hits
    peptideRepeatCount int unsigned not null,	# Indicates how many times this same hit was observed
              #Indices
    KEY(name),
    KEY(chrom, chromStart, spectrumId)
);
