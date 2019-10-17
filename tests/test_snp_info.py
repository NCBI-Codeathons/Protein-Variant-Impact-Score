from __future__ import with_statement
import unittest
import json
import sys
import os
sys.path.append(os.path.join(os.path.dirname(os.path.realpath(__file__)), '../..'))
from snp_info import SNPParser

class SNPParserTestCase(unittest.TestCase):

    def __init__(self, *args, **kwargs):
        super(SNPParserTestCase, self).__init__(*args, **kwargs)


    def test_get_variant_protein_info(self):
        pass

    def test_get_variant_frequency(self):
        pass
    
