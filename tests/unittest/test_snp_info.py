from __future__ import with_statement
import unittest
import json
import sys
import os
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../../src'))
from snp_info import SNPParser

class SNPParserTestCase(unittest.TestCase):

    test_set = {}
    
    def __init__(self, *args, **kwargs):
        super(SNPParserTestCase, self).__init__(*args, **kwargs)
        self.make_test_set()
        

    def make_test_set(self):
        input = open('../data/test.case.json', 'r')
        for line in input:
            self.test_set[json.loads(line)['refsnp_id']] = json.loads(line)
        

    def test_get_variant_protein_info(self):
        snp_parser = SNPParser()
        # ref   alt      pos     seq_id
        expected = set([
            ('S', 'Y', 43, 'NP_000240.1'),
            ('S', 'Y', 43, 'NP_001245200.1'),
            ('S', 'Y', 43, 'NP_001341557.1'),
            ('S', 'Y', 43, 'NP_001341558.1'),
            ('S', 'Y', 43, 'NP_001341559.1'),
            ('S', 'F', 43, 'NP_000240.1'),
            ('S', 'F', 43, 'NP_001245200.1'),
            ('S', 'F', 43, 'NP_001341557.1'),
            ('S', 'F', 43, 'NP_001341558.1'),
            ('S', 'F', 43, 'NP_001341559.1'),
        ])
        protein_info = snp_parser.get_variant_protein_info(self.test_set['63751109'])
        self.assertEqual(protein_info, expected , 'protein info test failed.')

        
    def test_get_variant_frequency(self):
        snp_parser = SNPParser()
        freq = snp_parser.get_variant_frequency(self.test_set['63751109'])
        self.assertEqual(freq, set([('T',0, 78698)]) , 'frequency test failed.')

    def test_get_clinical_significances(self):
        snp_parser = SNPParser()
        expected=dict()
        aset=set()
        aset.add('pathogenic')
        expected['T']=aset

        sigs= snp_parser.get_clinical_significances(self.test_set['63751109'])
        self.assertEqual(sigs, expected , 'clinical significances test failed.')
    
    

if __name__ == '__main__':

    unittest.main()
