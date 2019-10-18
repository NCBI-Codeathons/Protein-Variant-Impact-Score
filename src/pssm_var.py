import json
import subprocess
import os
from snp_info import *
import urllib.request
import argparse

class PSSMVar(object):

    pssm_bin = os.path.join(os.path.dirname(__file__), 'c++/build/getpssmforgipos')
    json_cgi = 'https://api.ncbi.nlm.nih.gov/variation/v0/beta/refsnp/'
    
    prot_acc_gi_file = os.path.join(os.path.dirname(__file__), '../data/gi_accn_38K')
    prot_acc_gi = {}    

    def __init__(self, *args, **kwargs):
        super(PSSMVar, self).__init__(*args, **kwargs)    
        with open(self.prot_acc_gi_file) as f:
            for l in f:
                l = l.strip()
                temp = l.split('\t')
                self.prot_acc_gi[temp[0]] = temp[1]


    def get_pssm_score(self, snp_id):

        url = self.json_cgi + str(snp_id)
        req = urllib.request.Request(url)
        r = urllib.request.urlopen(req).read()
        rs = json.loads(r.decode('utf-8'))
        snp_parser = SNPParser()
        sigs= snp_parser.get_clinical_significances(rs)
        freq = snp_parser.get_variant_frequency(rs)        
        protein_info = snp_parser.get_variant_protein_info(rs)
        prot_var = {'snp_id': snp_id, 'pssm': [], 'clin_sig': sigs, 'freq': freq}
        for info in protein_info:
            acc = info[3].split('.')[0]
            if acc in self.prot_acc_gi:
                proc = subprocess.Popen(' '.join([self.pssm_bin, self.prot_acc_gi[acc], str(info[2])]), \
                                        shell=True, stdout=subprocess.PIPE, stderr=subprocess.STDOUT)

                pssm = ''
                for l in proc.stdout.readlines():
                    pssm += l.decode().strip()

                proc.communicate()
                pssm = json.loads(pssm)
                prot_var['pssm'].append({acc: pssm['data']['pssmlist']})
                
        print(prot_var)

            

if __name__ == "__main__":

    parser = argparse.ArgumentParser(description='Retrieving PSSM score for a SNP')
    parser.add_argument('-r', dest='rs', required=True,
                        help='The snp_id')
    args = parser.parse_args()
    
    calculator = PSSMVar()
    calculator.get_pssm_score(args.rs)
    #calculator.get_pssm_score(63751109)
    #calculator.get_pssm_score(139209733)
