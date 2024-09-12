#!/bin/bash

../build/parser ../testdata/testdata01.in "\n\([A-Z]+\);\([A-Za-z ()/0-9]+\);\([0-9]+\)" > ../testdata/testdata01.tmp
echo -n "testdata01 "
[[ -z $(diff ../testdata/testdata01.out ../testdata/testdata01.tmp) ]] && echo "[ PASS ]" || echo "[ FAIL ]"

../build/parser ../testdata/testdata02.in "\n\([A-Z?]+\);\([0-9A-Za-z ]+\);\([0-9]+\);\([0-9]+\);\([A-Za-z0-9,+-: ]+\);\([A-Za-z0-9 -+<=,.\}\{]+\);\([A-Za-z ()/0-9]+\);\(\d\)" > ../testdata/testdata02.tmp
echo -n "testdata02 "
[[ -z $(diff ../testdata/testdata02.out ../testdata/testdata02.tmp) ]] && echo "[ PASS ]" || echo "[ FAIL ]"

../build/parser ../testdata/testdata03.in "\n\([a-zA-Z(),]+\);\([A-Z]+\);\([a-zA-Z,.()#]+\);\([0-9A-Z]+\);\([0-9]+\);\([0-9]+\);\([A-Z.]+\);\([A-Za-z- ]+\)" > ../testdata/testdata03.tmp
echo -n "testdata03 "
[[ -z $(diff ../testdata/testdata03.out ../testdata/testdata03.tmp) ]] && echo "[ PASS ]" || echo "[ FAIL ]"

../build/parser ../testdata/testdata04.in "^lease \(\d+.\d+.\d+.\d+\) {\n\s+starts \(\d\) \(\d+/\d+/\d+\) \(\d+:\d+:\d+\);\n\s+ends \(\d\) \(\d+/\d+/\d+\) \(\d+:\d+:\d+\);\n\*cltt \(\d\) \(\d+/\d+/\d+\) \(\d+:\d+:\d+\);\n\*hardware ethernet \(\h+:\h+:\h+:\h+:\h+:\h+\);\n\*}" > ../testdata/testdata04.tmp
echo -n "testdata04 "
[[ -z $(diff ../testdata/testdata04.out ../testdata/testdata04.tmp) ]] && echo "[ PASS ]" || echo "[ FAIL ]"

rm -rf ../testdata/*.tmp

