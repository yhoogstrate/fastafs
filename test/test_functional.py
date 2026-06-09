#!/usr/bin/env python3
# *- coding: utf-8 -*-

import os
import shutil
import subprocess
import sys
import unittest

sys.path.insert(0, os.path.dirname(__file__))
from test_utils import diff_fasta_with_mounted


TEST_DIR  = "test/data/"
TMP_DIR   = "tmp/test_functional"
MNT_DIR   = "tmp/mnt"
FASTAFS   = "./build-release/bin/fastafs"


def _cache_and_mount(fasta, alias, padding, use_zstd=False):
    fastafs_out = os.path.join(TMP_DIR, alias + ".fastafs")
    return diff_fasta_with_mounted(
        fasta, fastafs_out, alias, padding,
        FASTAFS, use_zstd, MNT_DIR,
    )


class FunctionalTest(unittest.TestCase):

    @classmethod
    def setUpClass(cls):
        os.makedirs(TMP_DIR, exist_ok=True)
        os.makedirs(MNT_DIR, exist_ok=True)

    @classmethod
    def tearDownClass(cls):
        shutil.rmtree(TMP_DIR, ignore_errors=True)

    # --- plain (.fastafs) ---

    def test_01_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test.fa',     'func_01', 100)
        self.assertFalse(r['diff'])

    def test_02_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_002.fa', 'func_02',  60)
        self.assertFalse(r['diff'])

    def test_03_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_003.fa', 'func_03',  60)
        self.assertFalse(r['diff'])

    def test_04_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_004.fa', 'func_04',  32)
        self.assertFalse(r['diff'])

    def test_05_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_005.fa', 'func_05',  80)
        self.assertFalse(r['diff'])

    def test_06_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_006.fa', 'func_06',  10)
        self.assertFalse(r['diff'])

    def test_07_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_007.fa', 'func_07',  72)
        self.assertFalse(r['diff'])

    def test_08_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_008.fa', 'func_08',  72)
        self.assertFalse(r['diff'])

    def test_09_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_009.fa', 'func_09',  24)
        self.assertFalse(r['diff'])

    def test_10_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_010.fa', 'func_10',  60)
        self.assertFalse(r['diff'])

    def test_11_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_011.fa', 'func_11',  60)
        self.assertFalse(r['diff'])

    def test_12_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_012.fa', 'func_12',  60)
        self.assertFalse(r['diff'])

    def test_13_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_013.fa', 'func_13',  60)
        self.assertFalse(r['diff'])

    def test_14_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_014.fa', 'func_14',  60)
        self.assertFalse(r['diff'])

    def test_15_plain(self):
        r = _cache_and_mount(TEST_DIR + 'test_015.fa', 'func_15',  60)
        self.assertFalse(r['diff'])

    # --- zstd (.fastafs.zst) ---

    def test_01_zstd(self):
        r = _cache_and_mount(TEST_DIR + 'test.fa',     'func_01_z', 100, use_zstd=True)
        self.assertFalse(r['diff'])

    def test_02_zstd(self):
        r = _cache_and_mount(TEST_DIR + 'test_002.fa', 'func_02_z',  60, use_zstd=True)
        self.assertFalse(r['diff'])

    def test_05_zstd(self):
        r = _cache_and_mount(TEST_DIR + 'test_005.fa', 'func_05_z',  80, use_zstd=True)
        self.assertFalse(r['diff'])

    def test_09_zstd(self):
        r = _cache_and_mount(TEST_DIR + 'test_009.fa', 'func_09_z',  24, use_zstd=True)
        self.assertFalse(r['diff'])


if __name__ == '__main__':
    unittest.main()
