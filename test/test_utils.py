#!/usr/bn/env python

import subprocess

def run_perf_mount(i, rdict):
    cmd = ['perf', 'stat', 'fastafs', 'mount', '-f', 'hg19', '/mnt/tmp']
    with subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE) as p:
        stdout, stderr = p.communicate()
        stderr = stderr.decode("utf-8")
        stdout = stdout.decode("utf-8")
        
        for _ in stderr.split("\n"):
            if _.find("nstructions") > -1:
                rdict['instructions'] = _.strip().split(" ",1)[0].replace(",","")
                p.terminate()


def get_ins_per_reading_mounted_suffix(size):
    manager = multiprocessing.Manager()
    return_dict = manager.dict()
    parallel_thread = Process(target=run_perf_mount, args=(1, return_dict))#, args=cmd), kwargs={'save_stdout': tmp_filename}
    parallel_thread.start()

    import time
    time.sleep(1)


    #print("Tailing content")
    cmd = ['bash', '-c', 'tail -c '+str(size)+' /mnt/tmp/hg19.fa > /dev/null']
    with subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE) as p1:
        stdout1, stderr1 = p1.communicate()
        p1.terminate()

        #print("Umounting")
        cmd2 = ['fusermount', '-u', '/mnt/tmp']
        with subprocess.Popen(cmd2,  stderr=subprocess.PIPE, stdout = subprocess.PIPE) as p2:
            stdout2, stderr2 = p2.communicate()
            #print("Obtain thread data back")
            
            p2.terminate()


    parallel_thread.join()
    return return_dict['instructions']





def run_mount(fastafs_binary, mountpoint):
    cmd = [fastafs_binary, 'mount', '-f', 'hg19', '/mnt/tmp']
    with subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE) as p:
        stdout, stderr = p.communicate()
        stderr = stderr.decode("utf-8")
        stdout = stdout.decode("utf-8")
        
        for _ in stderr.split("\n"):
            if _.find("nstructions") > -1:
                rdict['instructions'] = _.strip().split(" ",1)[0].replace(",","")
                p.terminate()



def diff_fasta_with_mounted(fasta_file, fastafs_tmp_file, padding, fastafs_binary, mountpoint):
    """
    Do a diff with an original fasta and one fastafs converted and mounted
    """
    
    # 1. fasta to FASTAFS:
    subprocess.Popen([fastafs_binary, 'cache', '-o', fastafs_tmp_file , fasta_file],  stderr=subprocess.PIPE, stdout = subprocess.PIPE)


    # 2. mount in the background
    """
    manager = multiprocessing.Manager()
    return_dict = manager.dict()
    parallel_thread = Process(target=run_mount, args=(1, return_dict))#, args=cmd), kwargs={'save_stdout': tmp_filename}
    parallel_thread.start()

    import time
    time.sleep(1)


    #print("Tailing content")
    cmd = ['bash', '-c', 'tail -c '+str(size)+' /mnt/tmp/hg19.fa > /dev/null']
    with subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE) as p1:
        stdout1, stderr1 = p1.communicate()
        p1.terminate()

        #print("Umounting")
        cmd2 = ['fusermount', '-u', '/mnt/tmp']
        with subprocess.Popen(cmd2,  stderr=subprocess.PIPE, stdout = subprocess.PIPE) as p2:
            stdout2, stderr2 = p2.communicate()
            #print("Obtain thread data back")
            
            p2.terminate()


    parallel_thread.join()
    return return_dict['instructions']
    """

