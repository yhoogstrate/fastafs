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
        difference = diff_fasta_with_mounted(TEST_DIR + 'test.fa', "test_func_01", 100, './bin/fastafs', 'tmp/mnt')
        
        self.assertEqual(difference['diff'] , False)




def main():
    unittest.main()

if __name__ == '__main__':
    main()
