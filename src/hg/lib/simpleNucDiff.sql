# singleNucDiff.sql was originally generated by the autoSql program, which also 
# generated singleNucDiff.c and singleNucDiff.h.  This creates the database representation of
# an object which can be loaded and saved from RAM in a fairly 
# automatic way.

#Simple nucleotide difference
CREATE TABLE simpleNucDiff (
    bin smallint not null,	# Index field
    chrom varchar(255) not null,	# Target species chromosome
    chromStart int unsigned not null,	# Start position in chromosome
    chromEnd int unsigned not null,	# End position in chromosome
    tSeq varchar(255) not null,	# Sequence in target species
    qSeq varchar(255) not null,	# Sequence in other (query) species
              #Indices
    INDEX(chrom(8),bin),
    INDEX(chrom(8),chromStart)
);
