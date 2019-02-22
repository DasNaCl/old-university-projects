#!/usr/bin/env python3

import random
import subprocess
import os
import re
from sys import argv
import threading

# number of terms to generate
TERM_NUM = 50
PATH_TO_C4 = os.path.join("..","build","debug", "c4")
EXECUTION_COMMAND = PATH_TO_C4 + " --parse {}"

FILTER_FUNC_RETURN = True
FILTER_VOID_FIELD = True
ONLY_NEGATIVE = False
NO_SEGMANTION_FAULT = False
NO_ASSERTION = False
NO_STRUCT = True
NO_KEYWORDS = True

KEYWORD_FILTER = ["auto", "break", "case", "default", "do", "double", "else", "enum", "float", "for", "goto", "if", "inline"
            "long", "register", "restrict", "return", "short", "signed", "sizeof", "static", "switch", "typedef", "union", 
            "unsigned", "volitale", "while", "_Alignas", "_Alignof", "_Atomic", "_Bool", "_Complex", "_Generic", "_Imaginary", 
            "_Noreturn", "_Static_assert", "_Thread_local"]


ident_reg = re.compile("[A-Za-z_]")
num_reg = re.compile("-n ([0-9]+)")
term_colours = True

print_lock = threading.Lock()
START_SYMBOL = "$START"
MAX_SYMBOLS = 20
MAX_TRIES = 100000
grammar = {
    "$START"             : ["$TOP-DECLARATION"],
    "$TOP-DECLARATION"       : ["$DECLARATION-SPECIFIERS;", "$DECLARATION-SPECIFIERS $DECLARATOR;"],
    "$DECLARATOR"        : ["$POINTER$DIRECT-DECLARATOR", "$DIRECT-DECLARATOR"],
    "$DIRECT-DECLARATOR" : ["$IDENTIFIER", "($DECLARATOR)", 
                          "$DIRECT-DECLARATOR ($PARAM-TYPE-LIST)"],
    "$POINTER"           : ["*", "*$POINTER"],
    "$PARAM-TYPE-LIST"   : ["$PARAM-LIST"],
    "$PARAM-LIST"        : ["$PARAM-DECLARATION", "$PARAM-LIST, $PARAM-DECLARATION"],
    "$PARAM-DECLARATION" : ["$DECLARATION-SPECIFIERS $DECLARATOR", 
                            "$DECLARATION-SPECIFIERS",
                            "$DECLARATION-SPECIFIERS $ABSTRACT-DECLARATOR"],
    "$IDENTIFIER"        : ["$IDENT-NONDIGIT", "$IDENTIFIER$IDENT-NONDIGIT", 
                            "$IDENTIFIER$DIGIT"],
    "$IDENT-NONDIGIT"    : [a for a in "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ_"],
    "$DIGIT"             : [a for a in "0123456789"],
    "$DECLARATION-SPECIFIERS" : ["$TYPE-SPECIFIER"],
    "$TYPE-SPECIFIER"         : ["void", "char", "int"] + (["$STRUCT"] if "--struct" in argv else []),
    "$STRUCT"                 : ["struct $IDENTIFIER", "struct $IDENTIFIER {$STR-DECL-LIST}"],
    "$STR-DECL-LIST"          : ["$STRU-DECL", "$STR-DECL-LIST $STRU-DECL"],
    "$STRU-DECL"              : ["$TYPE-SPECIFIER;", "$TYPE-SPECIFIER $ST-DECLARATOR-LIST;"],
    "$ST-DECLARATOR-LIST"     : ["$S-DECLARATOR", "$ST-DECLARATOR-LIST, $S-DECLARATOR"],
    "$S-DECLARATOR"           : ["$DECLARATOR"],
    "$ABSTRACT-DECLARATOR"    : ["$POINTER", "$POINTER$DIRECT-ABS-DECLARATOR", "$DIRECT-ABS-DECLARATOR"],
    "$DIRECT-ABS-DECLARATOR"  : ["($ABSTRACT-DECLARATOR)", "$DIRECT-ABS-DECLARATOR ($PARAM-TYPE-LIST)",
                                 "()", "$DIRECT-ABS-DECLARATOR()", "($PARAM-TYPE-LIST)"] 
}

class colour():
    def __init__(self):
        self.green = "\033[92m"
        self.red   = "\033[91m"
        self.yellow = "\033[93m"
        self.default = "\033[0m"
    
    def enable(self):
        self.__init__()
    
    def disable(self):
        self.green = ""
        self.red = ""
        self.yellow = ""
        self.default = ""

c = colour()

def apply_subst(term, rule):
    old, new = rule
    return term.replace(old, new, 1)

def create_term(grammar):
    term = START_SYMBOL
    tries = 0
    while term.count("$") > 0:
        to_subst = random.choice(list(grammar.keys()))
        subst = random.choice(grammar[to_subst])
        new_term = apply_subst(term, (to_subst, subst))
        if term != new_term and term.count("$") < MAX_SYMBOLS:
            term = new_term
            tries = 0
        else:
            tries += 1
            if tries > MAX_TRIES:
                raise RuntimeError
    return term

def term_filter(term):
    reduced_term = term.replace("int", "").replace("void", "").replace("char", "")
    return bool(ident_reg.search(reduced_term))

def output_filter(output):
    if FILTER_FUNC_RETURN and "Function may not return another function!" in output:
        return False
    if FILTER_VOID_FIELD and "variable or field declared void" in output:
        return False
    if NO_SEGMANTION_FAULT and "Segmentation" in output:
        return False
    if NO_ASSERTION and "Assertion" in output:
        return False
    if NO_KEYWORDS:
        for keyword in KEYWORD_FILTER:
            if "\"{}\"".format(keyword) in output:
                return False
    return True

def test_term(id):
    work = True
    term = str()
    while work:
        try:
            term = create_term(grammar)
        except RuntimeError:
            continue
        if not term_filter(term):
            continue
        path_to_content = os.path.join(".", "cnt{}.tmp".format(id))
        with open(path_to_content, "w") as cnt:
            cnt.write(term)
        process = subprocess.Popen(EXECUTION_COMMAND.format(path_to_content), stderr=subprocess.PIPE, shell=True)
        output, err = process.communicate()
        err = err.decode("utf8")
        exit_code = process.wait()
        if ONLY_NEGATIVE and exit_code == 0:
            continue
        if not output_filter(err):
            continue
        print_lock.acquire()
        try:
            print("{}: {}".format(c.green + "SUCCESS" + c.default if exit_code==0 else c.red + "FAILURE" + c.default, term))
            if err:
                print(c.yellow + "ERROR MESSAGE:" + c.default +" {}".format(err))
        finally:
            print_lock.release()
            try:
                os.remove(path_to_content)
                os.remove(path_to_content[:-3] + "ll")
            except:
                pass
        break

if __name__ == "__main__":
    # disables colours
    if "--no-colour" in argv:
        c.disable()
    # enables structs
    if "--struct" in argv:
        NO_STRUCTS = False
    # enables report for errors for function return types
    if "--func-return" in argv:
        FILTER_FUNC_RETURN = False
    # enables report for void field errors
    if "--void-field" in argv:
        FILTER_VOID_FIELD = False
    # disables report for segmentation faults
    if "--no-seg" in argv:
        NO_SEGMANTION_FAULT = True
    # disables report for assertion fails
    if "--no-assert" in argv:
        NO_ASSERTION = True
    # only shows failed test cases
    if "--only-neg" in argv:
        ONLY_NEGATIVE = True
    # disables filtering of keywords
    if "--keywords" in argv:
        NO_KEYWORDS = False
    # -n for number of finished tests
    for i in range(len(argv)):
        if argv[i] == "-n" and i+1 < len(argv):
            if argv[i + 1].isdecimal():
                TERM_NUM = int(argv[i+1])
    threads = list()
    for i in range(TERM_NUM):
        threads.append(threading.Thread(target=test_term, args=[i]))
        threads[i].start()
    for thread in threads:
        thread.join()
