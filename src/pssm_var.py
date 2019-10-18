import json
import subprocess
import os
from snp_info import *
import urllib.request
import getopt
import gzip
import sys

class PSSMVar(object):

    pssm_bin = os.path.join(os.path.dirname(__file__), 'c++/build/getpssmforgipos')

    
    prot_acc_gi_file = os.path.join(os.path.dirname(__file__), '../data/gi_accn_38K')
    prot_acc_gi = {}    

    def __init__(self, *args, **kwargs):
        super(PSSMVar, self).__init__(*args, **kwargs)    
        with open(self.prot_acc_gi_file) as f:
            for l in f:
                l = l.strip()
                temp = l.split('\t')
                self.prot_acc_gi[temp[0]] = temp[1]


    def get_pssm_score(self, rs):

        snp_id = rs['refsnp_id']
        # only cares about GnomAD to simplify
        gnomad = False
        for freq in rs['primary_snapshot_data']['allele_annotations'][1]['frequency']:
            if freq['study_name'] == 'GnomAD':
                gnomad = True
                break

        if not gnomad:
            return
                
        snp_parser = SNPParser()
        sigs= snp_parser.get_clinical_significances(rs)
        freq = snp_parser.get_variant_frequency(rs)        
        protein_info = snp_parser.get_variant_protein_info(rs)
        prot_var = {'snp_id': snp_id, 'pssm': [], 'clin_sig': sigs, 'freq': freq, 'protein_info': protein_info}
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

    try:
        opts, args = getopt.getopt(sys.argv[1:], "hr:f:d", ["help", "rs=", "file="])
        
    except getopt.GetoptError as e:
        print(e)
        sys.exit(1)

    rs = None
    input_file = None
    for opt, arg in opts:

        if opt in ("-h", "--help"):
            usage()
            sys.exit()
        
        elif opt == '-d':
            study_info['debug'] = 1

        elif opt in ("-r", "--rs"):
            rs = arg

        elif opt in ("-f", "--file"):
            input_file = os.path.abspath(arg)

    calculator = PSSMVar()


    if rs:
        json_cgi = 'https://api.ncbi.nlm.nih.gov/variation/v0/beta/refsnp/'
        url = json_cgi + args.rs
        req = urllib.request.Request(url)
        r = urllib.request.urlopen(req).read()
        rs = json.loads(r.decode('utf-8'))
    
        calculator.get_pssm_score(rs)
        #calculator.get_pssm_score(63751109)
        #calculator.get_pssm_score(429358)
        #calculator.get_pssm_score(139209733)

    elif input_file:
        input = gzip.GzipFile(input_file, 'rb')
        for line in input:
            line = line.decode()
            rs = json.loads(line)
            calculator.get_pssm_score(rs)
