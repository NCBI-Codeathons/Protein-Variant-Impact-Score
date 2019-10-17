# Protein-Variant-Impact-Score
This project aims to produce a pipeline for calculating a protein variant impact score for missense variants in human proteins that map to conserved domains. The impact score is based on substitution scores from Position Specific Score Matrices (PSSM) from NCBI's conserved domain database. Substitutions with  negative impact scores are those least tolerated based on conservation and are therefore most likely to disrupt the structure and function of the protein. The ultimate goal is to predict the likelihood of a pathogenic phenotype for a particular variant based on the impact score.  Similar software exists such as PolyPhen-2 (http://genetics.bwh.harvard.edu/pph2/) and SIFT (https://sift.bii.a-star.edu.sg/), but niether of these methods takes advantage of the information on conserved amino acids available for NCBI's Conserved Domain database.
## Diagram of the Concept
![alt text](https://github.com/NCBI-Codeathons/Protein-Variant-Impact-Score/blob/master/product.png)
Known pathogenic changes in the TP53 protein show up with negative scores in the PSSM.


## Outline of Method
We started with a file with 38,299 gi numbers for representative human RefSeq proteins and matched these to 38,277 current RefSeq accessions in the NCBI ID database and mapped scores from missense variants to the conserved domains of the protein. The display highlight the predicted impact on the protein.

![alt txt](https://github.com/NCBI-Codeathons/Protein-Variant-Impact-Score/blob/master/Flowchart_figures1.png)

## Preliminary Display
![alt txt](https://github.com/NCBI-Codeathons/Protein-Variant-Impact-Score/blob/master/display.png)
## Planned Features

## People
