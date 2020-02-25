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

    def test_02(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test_002.fa', "test_func_01", 60, './bin/fastafs', 'tmp/mnt')
        self.assertEqual(difference['diff'] , False)

    def test_03(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test_003.fa', "test_func_01", 60, './bin/fastafs', 'tmp/mnt')
        self.assertEqual(difference['diff'] , False)

    def test_04(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test_004.fa', "test_func_01", 32, './bin/fastafs', 'tmp/mnt')
        self.assertEqual(difference['diff'] , False)

    def test_05(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test_005.fa', "test_func_01", 80, './bin/fastafs', 'tmp/mnt')
        self.assertEqual(difference['diff'] , False)
  
    def test_06(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test_006.fa', "test_func_01", 10, './bin/fastafs', 'tmp/mnt')
        self.assertEqual(difference['diff'] , False)

    def test_07(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test_007.fa', "test_func_01", 72, './bin/fastafs', 'tmp/mnt')
        self.assertEqual(difference['diff'] , False)

    def test_08(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test_008.fa', "test_func_01", 72, './bin/fastafs', 'tmp/mnt')
        self.assertEqual(difference['diff'] , False)

    def test_09(self):
        difference = diff_fasta_with_mounted(TEST_DIR + 'test_009.fa', "test_func_09", 24, './bin/fastafs', 'tmp/mnt')
        self.assertEqual(difference['diff'] , False)


def main():
    unittest.main()

if __name__ == '__main__':
    main()
