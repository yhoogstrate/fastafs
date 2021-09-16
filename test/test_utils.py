#!/usr/bn/env python

from multiprocessing import Process
from tqdm import tqdm
import datetime
import multiprocessing
import os
import re
import subprocess
import time
import wget
import sys
import logging




def get_git_revision():
    git_branch = subprocess.check_output(['git', 'log', '-n', '1']).decode("utf-8").strip().split("\n")[0].replace("commit ","git-commit:")
    git_status = "|".join([_.strip().replace(" ","_") for _ in subprocess.check_output(['git', 'status', '-s', '-b']).decode("utf-8").strip().split("\n")])

    return (git_branch, git_status)

def get_machine_id():
    with open('/etc/machine-id') as fh:
        return "system:" + fh.read().strip()

def get_curtime():
    return str(datetime.datetime.now().strftime('%Y-%m-%d %X'))

def get_fastafs_rev():
    return subprocess.check_output(['./bin/fastafs', '--version']).decode("utf-8").split("\n")[0].replace('fastafs','').strip()


def check_cache_file_plain(fastafs_binary, filename_in, filename_out):
    logging.info("Checking if plain fastafs file exists: " + filename_out)
    
    if not os.path.exists(filename_out):
        logging.info("Creating new fastafs archive: " + filename_in)
        cmd = [fastafs_binary, 'cache', '--fastafs-only', '-o', filename_out, filename_in]

        logging.info("Running: " + ' '.join(cmd))
        p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)#:# as p:
        stdout, stderr = p.communicate()


def check_cache_file_zstd(fastafs_binary, filename_in, filename_out):
    logging.info("Checking if ZSTD fastafs file exists: " + filename_out)
    
    if not os.path.exists(filename_out):
        logging.info("Creating new zstd archive" + filename_in)
        
        cmd = [fastafs_binary, 'cache', '-o', re.sub('.zst$', '', filename_out) , filename_in]
        logging.info("Running: " + ' '.join(cmd))
        p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)#:# as p:
        stdout, stderr = p.communicate()


def check_benchmark_file(RESULTS_FILE):
    logging.info("Checking presence of file: " + RESULTS_FILE)
    
    if not os.path.exists(RESULTS_FILE):
        logging.info("Creating benchmark file: " + RESULTS_FILE)
        with open(RESULTS_FILE,'w') as fh:
            fh.write(
                "\t".join(
                ["timestamp", "fastafs-version", "git-commit", "perf:cycles", "perf:total_time", "perf:user_time", "perf:sys_time", "cmd", "git-mod-status"]
                ) + "\n"
            )
    

def write_benchmark_file(RESULTS_FILE, difference, TIMESTAMP, FASTAFS_REV, GIT_REV):
    with open(RESULTS_FILE, 'a') as fh:
        #print(difference)
        if(difference['diff']):
            raise Exception("ERROR - DIFFERENCE DETECTED")
            import sys
            sys.exit(1)
        else:
            logging.info("Appending benchmark results to file\n  - cycles:    %s sec\n  - total-time: %s sec\n  - user-time: %s sec\n  - sys-time:   %s sec",
                           str(difference['perf']['cycles']),
                           str(difference['perf']['total_time']),
                           str(difference['perf']['user_time']),
                           str(difference['perf']['sys_time']))
            fh.write(
                "\t".join([TIMESTAMP,
                           FASTAFS_REV,
                           GIT_REV[0].replace('git-commit:',''),
                           str(difference['perf']['cycles']),
                           str(difference['perf']['total_time']),
                           str(difference['perf']['user_time']),
                           str(difference['perf']['sys_time']),
                           " ".join(difference['cmd']),
                           GIT_REV[1]]) + "\n"
            )





"""
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
"""


def run_mount_bg(fastafs_binary, args, return_dict):
    #cmd = ['perf', 'stat' , fastafs_binary, 'mount',  '-d', '-f'] + args  #+ ['>' + '/dev/null']
    cmd = ['perf', 'stat', '-e', 'cycles', fastafs_binary, 'mount',  '-d', '-f'] + args  #+ ['>' + '/dev/null']
    
    return_dict['cmd'] = cmd

    o = ''
    e = ''

    with subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE) as p:
        while p.poll() is None:
            stdout, stderr = p.communicate()

            stdout = stdout.decode("utf-8")
            stderr = stderr.decode("utf-8")

            # i believe return_dict values can be set only once, so first cache and store later
            o += "\n---\n" + str(stdout)#[0:50] + "..." + str(stdout)[-50:]
            e += "\n---\n" + str(stderr)#[0:50] + "..." + str(stderr)[-50:]
        p.terminate()

    return_dict['stdout'] = o[0:100]
    return_dict['stderr'] = e[0:100]

    return_dict['perf'] = {'cycles': int(re.search('([0-9,]+)[ \t]+cycles' , e, re.IGNORECASE).group(1).replace(",","")),
             'total_time': float(re.search('([0-9\.]+)[ \t]+seconds time elapsed' , e, re.IGNORECASE).group(1)),
             'user_time': float(re.search('([0-9\.]+)[ \t]+seconds user', e, re.IGNORECASE).group(1)), 
             'sys_time': float(re.search('([0-9\.]+)[ \t]+seconds sys', e, re.IGNORECASE).group(1)) }



def diff_fasta_with_mounted(fasta_file, fastafs_tmp_filename, fastafs_mount_alias, padding, fastafs_binary, use_zstd, mountpoint = "tmp/mnt"):
    """
    Do a diff with an original fasta and one fastafs converted and mounted
    """
    output = {
        'cmd':{},
        'stdout': {},
        'stderr': {},
        'retcode': {},
        'diff': False,
        'perf': None
    }

    mountpoint = mountpoint.rstrip('/') + '/'

    # 1. fasta to FASTAFS:
    prog = 'cache'
    if use_zstd:
        cmd = [fastafs_binary, prog, '-o', fastafs_tmp_filename, fasta_file]
        fastafs_tmp_filename = fastafs_tmp_filename + ".zst"
    else:
        fastafs_tmp_filename = fastafs_tmp_filename + ".fastafs"
        cmd = [fastafs_binary, prog, '--fastafs-only', '-o', fastafs_tmp_filename, fasta_file]


    if not os.path.exists(fastafs_tmp_filename):
        print(' '.join(cmd))

        p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)#:# as p:
        stdout, stderr = p.communicate()
        output['cmd'][prog] = cmd
        output['stdout'][prog] = stdout.decode("utf-8")
        output['stderr'][prog] = stderr.decode("utf-8")
        output['retcode'][prog] = p.returncode


    # 2. check integrity:
    """ # uncomment when supported with chunked zstd(!!!)
    prog = 'check'
    cmd = [fastafs_binary, 'check', '-f', fastafs_tmp_filename]
    print(cmd)
    p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)#:# as p:
    stdout, stderr = p.communicate()
    output['cmd'][prog] = cmd
    output['stdout'][prog] = stdout.decode("utf-8")
    output['stderr'][prog] = stderr.decode("utf-8")
    output['retcode'][prog] = p.returncode
    """

    # 3. run active mount process and push it to the background
    manager = multiprocessing.Manager()
    return_dict = manager.dict()
    cmd = [fastafs_binary, "mount", '-p', str(padding), '-g', '-f', fastafs_tmp_filename, mountpoint]
    print(' '.join(cmd))
    parallel_thread = Process(target=run_mount_bg, args=(cmd[0], cmd[2:], return_dict))
    parallel_thread.start()

    #print(" --- checkpoint 03 --- ")
    #basename = fastafs_tmp_filename.replace('.fastafs.zstd','').replace('.fastafs.zst','').replace('.fastafs.zstd','').split('/')[-1]

    fn = mountpoint + fastafs_mount_alias + ".fa"
    i = 0
    while not os.path.exists(fn):
        print("waiting for file [" + fn + "] to come online")
        time.sleep(0.1) # there always is some time before invoking the mount and having the mount point up and running
        if i > 15:
            sys.exit(1)
        else:
            i += 1

    matches = 0
    try:
        with open(fn) as fh_mnt, open(fasta_file) as fh_orig:
            for line1, line2 in tqdm(zip(fh_mnt, fh_orig)):
                if line1.strip() != line2.strip():
                    #print("[" + line1.strip() + "] == [" + line2.strip() + "]")
                    output['diff'] = True
                    break
                else:
                    matches += 1

    except Exception as e:
        output['diff'] = True


    # 2. check integrity:
    cmd = ['fusermount', '-u', mountpoint]
    p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)
    #print("pt 1: ", parallel_thread.is_alive() )
    stdout, stderr = p.communicate()
    #print("pt 2: ", parallel_thread.is_alive() )
    while p.poll() is None:
        print("waiting for p (umount)")
    #print("p: ", p.is_alive() )
    
    #print(" --- checkpoint 06 --- ")
    
    #print("pt 3: ", parallel_thread.is_alive() )
    #while parallel_thread.is_alive() is None:
     #   print("waiting for pt (thread)")
     #   time.sleep(0.1)
    parallel_thread.join()
    #print("pt 4: ", parallel_thread.is_alive() )
    
    return_dict['diff'] = output['diff']
    
    #print(" --- checkpoint 07 --- ")

    #output['perf'] = return_dict['perf']
    

    return return_dict




def diff_fasta_with_view(fasta_file, fastafs_tmp_filename, padding, fastafs_binary, use_zstd, mountpoint = "tmp/mnt"):
    """
    Do a diff with an original fasta with 'fastafs view' command
    """
    output = {
        'cmd':{},
        'stdout': {},
        'stderr': {},
        'retcode': {},
        'diff': False,
        'perf-cycles': None
    }

    mountpoint = mountpoint.rstrip('/') + '/'

    # 1. fasta to FASTAFS:
    prog = 'cache'
    if use_zstd:
        #cmd = [fastafs_binary, prog, '-o', fastafs_tmp_filename, fasta_file]
        fastafs_tmp_filename = fastafs_tmp_filename + ".zst"
    else:
        fastafs_tmp_filename = fastafs_tmp_filename + ".fastafs"
        #cmd = [fastafs_binary, prog, '--fastafs-only', '-o', fastafs_tmp_filename, fasta_file]
    
    
    # if not os.path.exists(fastafs_tmp_filename):
        # print('$ ' + ' '.join(cmd))

        # p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)
        # stdout, stderr = p.communicate()
        # output['cmd'][prog] = cmd
        # output['stdout'][prog] = stdout.decode("utf-8")
        # output['stderr'][prog] = stderr.decode("utf-8")
        # output['retcode'][prog] = p.returncode
        # p.terminate()
    
    # # 2. check integrity:
    # prog = 'check'
    # cmd = [fastafs_binary, 'check', fastafs_tmp_name, fastafs_tmp_name]
    # p = subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.PIPE)
    # stdout, stderr = p.communicate()
    # output['cmd'][prog] = cmd
    # output['stdout'][prog] = stdout.decode("utf-8")
    # output['stderr'][prog] = stderr.decode("utf-8")
    # output['retcode'][prog] = p.returncode
    # p.terminate()

    # 3. run view and test outcome
    """
    logging.info("Run view from beginning to end and check file consistency")
    prog = 'view'
    cmd = [fastafs_binary, prog, '-p', str(padding), '-f' , fastafs_tmp_filename]
    print('$ ' + ' '.join(cmd))
    
    os.environ['PYTHONUNBUFFERED'] = "1"
    p = subprocess.Popen(cmd,  stdout=subprocess.PIPE, bufsize=10) # bufsize=1, universal_newlines=True

    with open(fasta_file) as fh_orig:
        while p.poll() is None:
            line1 = fh_orig.readline().strip()
            line2 = p.stdout.readline().decode("utf-8").strip()
            
            if line1 != "" or line2 != "":
                if line1 != line2:
                    output['diff'] = True
                    print("[" + line1 + "] == [" + line2 + "]")
                    break

    p.stdout.close()
    """

    # 4. perf view
    cmd = ['perf', 'stat', '-e', 'cycles', fastafs_binary, 'view', '-f' , fastafs_tmp_filename]
    
    return_dict = {'cmd': cmd, 'diff': False}

    o = ''
    e = ''

    with subprocess.Popen(cmd,  stderr=subprocess.PIPE, stdout = subprocess.DEVNULL) as p:
        while p.poll() is None:
            stdout, stderr = p.communicate()

            #stdout = stdout.decode("utf-8")
            stderr = stderr.decode("utf-8")

            # i believe return_dict values can be set only once, so first cache and store later
            #o += "\n---\n" + str(stdout)#[0:50] + "..." + str(stdout)[-50:]
            e += "\n---\n" + str(stderr)#[0:50] + "..." + str(stderr)[-50:]
        p.terminate()

    #return_dict['stdout'] = o[0:100]
    return_dict['stderr'] = e[0:100]

    return_dict['perf'] = {'cycles': int(re.search('([0-9,]+)[ \t]+cycles' , e, re.IGNORECASE).group(1).replace(",","")),
             'total_time': float(re.search('([0-9\.]+)[ \t]+seconds time elapsed' , e, re.IGNORECASE).group(1)),
             'user_time': float(re.search('([0-9\.]+)[ \t]+seconds user', e, re.IGNORECASE).group(1)), 
             'sys_time': float(re.search('([0-9\.]+)[ \t]+seconds sys', e, re.IGNORECASE).group(1)) }

    
    return return_dict



