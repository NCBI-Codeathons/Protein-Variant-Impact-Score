 
![](./possimpactor.png)

## Motivation
There are over 8 million human missense protein variations in dbSNP. These are the changes that may affect protein structure and function. However fewer than 2% of these changes have known human phenotypes. Predicting the potential impact of these uncharacterized variants is an important for bettier understanding of genetic influences on human health and disease.

## Project goal
This project aims to produce a pipeline for calculating a protein variant impact score for missense variants in human proteins that map to conserved domains. The impact score is based on substitution scores from Position Specific Score Matrices (PSSMs) from NCBI's conserved domain database. Substitutions with  negative impact scores are those least tolerated based on conservation and are therefore most likely to disrupt the structure and function of the protein. The ultimate goal is to predict the likelihood of a pathogenic phenotype for a particular variant based on the impact score.  Similar software exists such as PolyPhen-2 (http://genetics.bwh.harvard.edu/pph2/) and SIFT (https://sift.bii.a-star.edu.sg/), but niether of these methods takes advantage of the information on conserved amino acids available for NCBI's Conserved Domain database.
## Diagram of the Concept
![alt text](./product.png)
Known pathogenic changes in the TP53 protein show up with negative scores (shaded red) in the PSSM.


## Outline of Method
We started with a file with 38,299 gi numbers for representative human RefSeq proteins and matched these to 38,277 current RefSeq accessions in the NCBI ID database and mapped scores from missense variants to the conserved domains of the protein. The display will highlight the predicted impact on the protein.

![alt txt](./Flowchart_figures1.png)


## Software
### Python Applications
**snp_info.py** a python class that gets all missense snps, frequency data from GnomAD_exome, phenotype compatible with Hadoop cluster.

 **Input** compressed (bzip2) json files from the [dbSNP FTP site](https://ftp.ncbi.nlm.nih.gov/snp/latest_release/JSON/)S

 **Sample commandline:**

 ```
 python snp_info.py -r hadoop hdfs:///user/wangq2/r69_json -o hdfs:///user/zhahua/missense_b153 --no-output --jobconf  mapreduce.job.name=get.missense --jobconf mapreduce.job.reduces=100 --jobconf mapreduce.job.queuename=prod.dbsnp -c mrjob.conf
 ```
**pssm_var.py** uses snp_info.py class to combine scores from PSSM data with snp_info

**Sample commandlines**


File mode
```
python pssm_var.py -f missense_snp_json_file.gz
```
Single SNP mode
```
python pssm_var.py -r rs429358
```


## Dependencies
* Python 3.6
* NCBI C++ toolkit

## Planned Features

## People
* Hua Zhang NCBI/IEB
* Peter Cooper NCBI/IEB
* Sema Kachalo NCBI/IEB
* Rich McVeigh NCBI/IEB
* Jialin (Charlie) Xiang NCBI/IEB
* Jiyao Wang NCBI/IEB

