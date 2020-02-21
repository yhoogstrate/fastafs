#!/usr/bin/env python

#!/usr/bin/env python
# *- coding: utf-8 -*-
# vim: set expandtab tabstop=4 shiftwidth=4 softtabstop=4 textwidth=79:


import unittest
import filecmp
import os



from test_utils import *


TEST_DIR = "test/data/"
T_TEST_DIR = "tmp/"


# Nosetests doesn't use main()
if not os.path.exists(T_TEST_DIR):
    os.makedirs(T_TEST_DIR)
    os.makedirs(T_TEST_DIR + "/mnt")



class FunctionalTest(unittest.TestCase):
    def test_01(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test.fa',T_TEST_DIR + 'functional_test.fastafs', 100, './bin/fastafs', 'tmp/mnt')
        
        # assert  diff =  0




def main():
    unittest.main()

if __name__ == '__main__':
    main()
