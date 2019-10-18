import json
from mrjob.job import MRJob

class SNPParser(MRJob):
    '''
    extract protein accession, variant amino acid position,
    and variant frequency.
    '''

    def mapper(self, _, line):
        rs_obj = json.loads(line)
        if self.is_missense(rs_obj):
            print(line)

    
    def get_variant_protein_info(self, rs_obj):
        '''
        extract variant amino acid position.
        '''
        spdis=[] # empty list for return
        if 'primary_snapshot_data' in rs_obj:
            primary_snapshot_data = rs_obj['primary_snapshot_data']
            for annot in primary_snapshot_data['allele_annotations']:
                for asm_annot in annot['assembly_annotation']:
                    for gene in asm_annot['genes']:
                        for rna in gene['rnas']:
                            if 'protein' in rna.keys():
                                spdi=rna['protein']['variant']['spdi']
                                if spdi['inserted_sequence'] != spdi['deleted_sequence']:
                                    (ref, alt, pos, seq_id) = (spdi['deleted_sequence'],
                                               spdi['inserted_sequence'],
                                               spdi['position'],
                                               spdi['seq_id'])
                                    if seq_id.startswith("NP_"):
                                        # add to return list
                                        # change the position to 1-based
                                        spdis.append((ref,alt,pos+1,seq_id))


#                             break
        return set(spdis)

    def get_variant_frequency(self, rs_obj):
        allelefreqs=[]
        if 'primary_snapshot_data' in rs_obj:
            primary_snapshot_data = rs_obj['primary_snapshot_data']
            for annot in primary_snapshot_data['allele_annotations']:
                for frequency in annot['frequency']:
                    observation=frequency['observation']
                    if observation['deleted_sequence']!=observation['inserted_sequence']:
                        allelefreqs.append({observation['inserted_sequence']: (frequency['allele_count'], frequency['total_count'])})
        return allelefreqs

    def get_clinical_significances(self, rs_obj):
        '''
        Get the clinical significances for each allele.
        '''
        significances=dict()

        if 'primary_snapshot_data' in rs_obj:
            primary_refsnp= rs_obj['primary_snapshot_data']
            for annot in primary_refsnp['allele_annotations']:
                # find the allele alternative
                allele=""
                for frequency in annot['frequency']:
                    observation=frequency['observation']
                    if observation['deleted_sequence']!=observation['inserted_sequence']:
                        allele=observation['inserted_sequence']
                        break

                sigs=set()
                if allele !="":
                    for clininfo in annot['clinical']:
                        if(clininfo['clinical_significances']):
                            for sig in clininfo['clinical_significances']:
                                sigs.add(sig)

                    significances[allele]= sigs;

        return significances


    def is_missense(self, rs_obj):

        if 'primary_snapshot_data' in rs_obj:
            for allele_annotation in rs_obj['primary_snapshot_data']['allele_annotations'][1:]:
                for assm_annot in allele_annotation['assembly_annotation']:
                    if 'genes' in assm_annot:
                        for gene in assm_annot['genes']:
                            for rna in gene['rnas']:
                                if 'protein' in rna:
                                    for ontology in rna['protein']['sequence_ontology']:
                                        if ontology['name'] == 'missense_variant':
                                            return True

        return False


if __name__ == "__main__":

    SNPParser.run()
    
    # json_cgi = 'https://api.ncbi.nlm.nih.gov/variation/v0/beta/refsnp/'

    # parser = argparse.ArgumentParser(description='Retrieving PSSM score for a SNP')
    # parser.add_argument('-r', dest='rs', required=True,
    #                     help='The snp_id')
    # args = parser.parse_args()
    # url = json_cgi + str(args.rs)
    # req = urllib.request.Request(url)
    # r = urllib.request.urlopen(req).read()
    # rs = json.loads(r.decode('utf-8'))

    # snp_parser = SNPParser()
    # print(snp_parser.is_missense(rs))
