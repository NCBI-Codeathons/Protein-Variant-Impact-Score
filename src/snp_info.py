import json

class SNPParser(object):
    '''
    extract protein accession, variant amino acid position,
    and variant frequency.
    '''
    
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
                                        print("\t".join([ref,alt,str(pos),seq_id]))
                                        # add to return list
                                        spdis.append((seq_id,pos,alt,ref))
      

#                             break 
        return spdis

    def get_variant_frequency(self, rs_obj):
        allelefreqs=[]
        if 'primary_snapshot_data' in rs_obj:
            for annot in primary_snapshot_data['allele_annotations']:
                for frequency in annot['frequency']:
		    observation=frequency['observation']
                    if observation['deleted_sequence']!=observation['inserted_sequence']:     
                        print (observation['deleted_sequence'],frequency['allele_count'],frequency['total_count'])
                        allelefreqs.append((observation['deleted_sequence'],frequency['allele_count'],frequency['total_count']))
        return set(allelefreqs)


    
