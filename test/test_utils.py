#!/usr/bn/env python

import subprocess
import time
import multiprocessing
from multiprocessing import Process




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



def run_mount_bg(fastafs_binary, args, return_dict):
    cmd = [fastafs_binary, 'mount',  '-d', '-f'] + args
    #return_dict[cmd] = cmd
    return_dict['stdout'] = ''
    return_dict['stderr'] = ''
    with subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE) as p:
        stdout, stderr = p.communicate()

        stdout = stdout.decode("utf-8")
        stderr = stderr.decode("utf-8")

        return_dict['stdout'] = stdout
        return_dict['stderr'] = stderr

    #pass



def diff_fasta_with_mounted(fasta_file, fastafs_tmp_name, padding, fastafs_binary, mountpoint):
    """
    Do a diff with an original fasta and one fastafs converted and mounted
    """
    output = {
        'cmd':{},
        'stdout': {},
        'stderr': {},
        'retcode': {},
        'diff': False
    }
    
    # 1. fasta to FASTAFS:
    prog = 'cache'
    #cmd = [fastafs_binary, prog, '-o', fastafs_tmp_file , fasta_file]
    cmd = [fastafs_binary, prog, fastafs_tmp_name , fasta_file]
    p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)#:# as p:
    stdout, stderr = p.communicate()
    output['cmd'][prog] = cmd
    output['stdout'][prog] = stdout.decode("utf-8")
    output['stderr'][prog] = stderr.decode("utf-8")
    output['retcode'][prog] = p.returncode
    
    # 2. check integrity:
    prog = 'check'
    cmd = [fastafs_binary, 'check', fastafs_tmp_name, fastafs_tmp_name]
    p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)#:# as p:
    stdout, stderr = p.communicate()
    output['cmd'][prog] = cmd
    output['stdout'][prog] = stdout.decode("utf-8")
    output['stderr'][prog] = stderr.decode("utf-8")
    output['retcode'][prog] = p.returncode

    # 3. run active mount process and push it to the background
    manager = multiprocessing.Manager()
    return_dict = manager.dict()
    parallel_thread = Process(target=run_mount_bg, args=(fastafs_binary, ['-p', str(padding), fastafs_tmp_name, 'tmp/mnt'], return_dict))
    parallel_thread.start()
    # see how long this runs
    
    time.sleep(0.1)
    with open("tmp/mnt/" + fastafs_tmp_name + ".fa") as fh_mnt, open(fasta_file) as fh_orig:
        for line1, line2 in zip(fh_mnt, fh_orig):
            if line1.strip() != line2.strip():
                print("[" + line1.strip() + "] == [" + line2.strip() + "]")
                output['diff'] = True
                #return output
                break

    # 2. check integrity:
    cmd = ['fusermount', '-u', "tmp/mnt"]
    p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)
    stdout, stderr = p.communicate()

    return output
