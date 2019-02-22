LEXTEST=false
PARTEST=false
ASTTEST=true
SEMTEST=false
FUZZTEST=false

CDIR=$PWD
ARGS=""
if [ "$LEXTEST" = false ]; then
    ARGS=$ARGS" --no-lex"
fi
if [ "$PARTEST" = false ]; then
    ARGS=$ARGS" --no-par"
fi
if [ "$SEMTEST" = false ]; then
    ARGS=$ARGS" --no-sem"
fi
python3 select_tests.py $ARGS
cd build
cmake ..
make -j8
echo "\n#----- Executing unit tests -----#\n"
make test
if [ "$ASTTEST" = true ]; then
    cd $CDIR/tests/ast
    echo "\n#----- Executing AST tests -----#\n"
    python3 execute_tests.py --threads
fi
if [ "$FUZZTEST" = true ]; then
    cd $CDIR/tests
    echo "\n#----- Execution FUZZ tests -----#\n"
    python3 fuzzer.py
fi
cd $CDIR
python3 select_tests.py
echo "done"
