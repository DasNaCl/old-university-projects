from os import path
import sys

CMAKEPATH = path.join(".", "CMakeLists.txt")

LEXTEST = False if "--no-lex" in sys.argv else True
PARTEST = False if "--no-par" in sys.argv else True
SEMTEST = False if "--no-sem" in sys.argv else True

cmake_lines = str()

with open(CMAKEPATH, "r") as cmake_file:
    for line in cmake_file:
        if line.startswith("#"):
            if (LEXTEST and "lexer" in line) or (
                PARTEST and "parser" in line) or (
                SEMTEST and "semantic" in line):
                cmake_lines += line[1:]
            else:
                cmake_lines += line
        else:
            if (not LEXTEST and "lexer" in line) or (
                not PARTEST and "parser" in line) or (
                not SEMTEST and "semantic" in line):
                cmake_lines += "#" + line
            else:
                cmake_lines += line
    
with open(CMAKEPATH, "w") as cmake_file:
    cmake_file.write(cmake_lines)
