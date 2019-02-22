#!/usr/bin/env python3

import os, sys 
from os import listdir
from os.path import isfile, join, abspath

source_dir = abspath(join("..", ".."))
make_args = "-j8" if "--threads" in sys.argv else ""
if not "--no-build" in sys.argv:
    os.system("cd {} ; make {} ; cd {}/tests/ast".format(source_dir, make_args, source_dir))
onlyfiles = [f for f in listdir('c_files') if isfile(join('c_files', f))]
count_all = 0
count_failures = 0
count_succeded = 0
for files in onlyfiles:
    os.system("{}/build/debug/c4 --print-ast ./c_files/".format(source_dir) + files + " > output/" + files + " 2> output/" + files[:-2] + ".err")
    with open('output/' + files, 'r', encoding='ISO-8859-1') as f1, open('exOutput/' + files[:-2] + '.txt', 'r', encoding='ISO-8859-1') as f2, open('results/' + files[:-2] + '.txt', 'w', encoding='ISO-8859-1') as outfile:
        count_all += 1
        failed = 0
        line_nr = 0
        num_lines_f1 = sum(1 for line in f1)
        num_lines_f2 = sum(1 for line in f2)
        if (num_lines_f1 != num_lines_f2):
                
                count_failures += 1
                failed = 1
                print("\033[31mERROR\033[0m in File: ./c_files/{}".format(files))
                print("\tNumber of Tokens not equal.")
        else:
            f1.seek(0)
            f2.seek(0)
            for line1, line2 in zip(f1, f2):
                line_nr += 1
                if line1 != line2:
                    count_failures += 1
                    failed = 1
                    print("\033[31mERROR\033[0m in File: ./c_files/{} in line {}".format(files, line_nr))
                    print("\tExpected: {}".format(line2), end="")
                    print("\tGot     : {}".format(line1))
                    print(line1, file=outfile)
                    break
        if (failed == 0):
                count_succeded += 1
                print("\033[32mSUCCESS\033[0m for ./c_files/{}".format(files))
print("-------------------------------------------------------------------")
print("----")
print("----")
if(count_all == count_succeded):
    print("\033[32m ALL Tests succesfull.\033[0m")
else:
    print("\033[31m{}\033[0m".format(str(count_failures) + " out of " + str(count_all) + " Tests failed."))
print("----")
print("----")               
print("-------------------------------------------------------------------")
