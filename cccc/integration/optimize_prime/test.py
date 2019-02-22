import os, sys 
from os import listdir
from os.path import isfile, join
os.system("cd ../.. ; make ; cd integration/optimize_prime")
onlyfiles = [f for f in listdir('./cFiles') if isfile(join('./cFiles', f))]
count_all = 0
count_failures = 0
count_succeded = 0
for files in onlyfiles:
    os.system("../../build/debug/c4 --tokenize ./cFiles/" + files + " > output/" + files + " 2> output/" + files[:-2] + ".err")
    with open('./output/' + files, 'r', encoding='ISO-8859-1') as f1, open('./exOutput/' + files[:-2] + '.txt', 'r', encoding='ISO-8859-1') as f2, open('./results/' + files[:-2] + '.txt', 'w', encoding='ISO-8859-1') as outfile:
        count_all += 1
        failed = 0
        num_lines_f1 = sum(1 for line in f1)
        num_lines_f2 = sum(1 for line in f2)
        if (num_lines_f1 != num_lines_f2):
                count_failures += 1
                failed = 1
                output = "\033[31mERROR\033[0m in File: ./c Files/" + files + ""
                output2 = "\tNumber of Tokens not equal."
                os.system("echo \"" + output + "\"")
                os.system("echo \"" + output2 + "\"")
        else:        
            for line1, line2 in zip(f1, f2):
                if line1 != line2:
                    count_failures += 1
                    failed = 1
                    output = "\033[31mERROR\033[0m in File: ./c Files/" + files + ""
                    output2 = "\tExpected: " + line1 + ""
                    os.system("echo \"" + output + "\"")
                    os.system("echo  \"" + output2 + "\"")
                    print(line1, file=outfile)
        if (failed == 0):
                count_succeded += 1
                output = "\033[32mSUCCESS\033[0m for ./c Files/" + files + ""
                os.system("echo \"" + output + "\"")
os.system("echo -------------------------------------------------------------------")
os.system("echo ----")
os.system("echo ----")
if(count_all == count_succeded):
    os.system("echo \"\033[32m ALL Tests succesfull.\033[0m \"")
else:
    output = "" + str(count_failures) + " out of " + str(count_all) + " Tests failed."
    os.system("echo \"\033[31m" + output + "\033[0m\"")
os.system("echo ----")
os.system("echo ----")               
os.system("echo -------------------------------------------------------------------")
