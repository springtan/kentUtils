# mapSts.sql was originally generated by the autoSql program, which also 
# generated mapSts.c and mapSts.h.  This creates the database representation of
# an object which can be loaded and saved from RAM in a fairly 
# automatic way.

#An STS based map in relationship to golden path
CREATE TABLE mapGenethon (
    chrom varchar(255) not null,	# Chromosome
    chromStart int unsigned not null,	# Start position in chrom
    chromEnd int unsigned not null,	# End position in chrom
    name varchar(255) not null,	# Name of STS marker
    score int unsigned not null,	# Score of a marker - depends on how many contigs it hits
    identNo int unsigned not null,	# Identification number of STS
    rhChrom varchar(255) not null,	# Chromosome number from the RH map - 0 if there is none
    distance float not null,	# Distance from the RH map
    ctgAcc varchar(255) not null,	# Contig accession number
    otherAcc varchar(255) not null,	# Accession number of other contigs that the marker hits
              #Indices
    INDEX(name(16)),
    INDEX(chrom(16),chromStart)
);

CREATE TABLE mapGm99Gb4 (
    chrom varchar(255) not null,	# Chromosome
    chromStart int unsigned not null,	# Start position in chrom
    chromEnd int unsigned not null,	# End position in chrom
    name varchar(255) not null,	# Name of STS marker
    score int unsigned not null,	# Score of a marker - depends on how many contigs it hits
    identNo int unsigned not null,	# Identification number of STS
    rhChrom varchar(255) not null,	# Chromosome number from the RH map - 0 if there is none
    distance float not null,	# Distance from the RH map
    ctgAcc varchar(255) not null,	# Contig accession number
    otherAcc varchar(255) not null,	# Accession number of other contigs that the marker hits
              #Indices
    INDEX(name(16)),
    INDEX(chrom(16),chromStart)
);

CREATE TABLE mapMarshfield (
    chrom varchar(255) not null,	# Chromosome
    chromStart int unsigned not null,	# Start position in chrom
    chromEnd int unsigned not null,	# End position in chrom
    name varchar(255) not null,	# Name of STS marker
    score int unsigned not null,	# Score of a marker - depends on how many contigs it hits
    identNo int unsigned not null,	# Identification number of STS
    rhChrom varchar(255) not null,	# Chromosome number from the RH map - 0 if there is none
    distance float not null,	# Distance from the RH map
    ctgAcc varchar(255) not null,	# Contig accession number
    otherAcc varchar(255) not null,	# Accession number of other contigs that the marker hits
              #Indices
    INDEX(name(16)),
    INDEX(chrom(16),chromStart)
);

CREATE TABLE mapShgcG3 (
    chrom varchar(255) not null,	# Chromosome
    chromStart int unsigned not null,	# Start position in chrom
    chromEnd int unsigned not null,	# End position in chrom
    name varchar(255) not null,	# Name of STS marker
    score int unsigned not null,	# Score of a marker - depends on how many contigs it hits
    identNo int unsigned not null,	# Identification number of STS
    rhChrom varchar(255) not null,	# Chromosome number from the RH map - 0 if there is none
    distance float not null,	# Distance from the RH map
    ctgAcc varchar(255) not null,	# Contig accession number
    otherAcc varchar(255) not null,	# Accession number of other contigs that the marker hits
              #Indices
    INDEX(name(16)),
    INDEX(chrom(16),chromStart)
);

CREATE TABLE mapShgcTng (
    chrom varchar(255) not null,	# Chromosome
    chromStart int unsigned not null,	# Start position in chrom
    chromEnd int unsigned not null,	# End position in chrom
    name varchar(255) not null,	# Name of STS marker
    score int unsigned not null,	# Score of a marker - depends on how many contigs it hits
    identNo int unsigned not null,	# Identification number of STS
    rhChrom varchar(255) not null,	# Chromosome number from the RH map - 0 if there is none
    distance float not null,	# Distance from the RH map
    ctgAcc varchar(255) not null,	# Contig accession number
    otherAcc varchar(255) not null,	# Accession number of other contigs that the marker hits
              #Indices
    INDEX(name(16)),
    INDEX(chrom(16),chromStart)
);

CREATE TABLE mapWiJac (
    chrom varchar(255) not null,	# Chromosome
    chromStart int unsigned not null,	# Start position in chrom
    chromEnd int unsigned not null,	# End position in chrom
    name varchar(255) not null,	# Name of STS marker
    score int unsigned not null,	# Score of a marker - depends on how many contigs it hits
    identNo int unsigned not null,	# Identification number of STS
    rhChrom varchar(255) not null,	# Chromosome number from the RH map - 0 if there is none
    distance float not null,	# Distance from the RH map
    ctgAcc varchar(255) not null,	# Contig accession number
    otherAcc varchar(255) not null,	# Accession number of other contigs that the marker hits
              #Indices
    INDEX(name(16)),
    INDEX(chrom(16),chromStart)
);

