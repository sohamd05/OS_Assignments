#!/bin/bash

# Compile C programs
gcc read100.c -o read100
gcc read510.c -o read510
gcc write10end.c -o write10end
gcc copy.c -o copy
gcc rename.c -o rename

# Run tests
./read100
if [ $? -eq 0 ]; then
    echo "read100 pass"
else
    echo "read100 fail"
fi

./read510
if [ $? -eq 0  ]; then
    echo "read510 pass"
else
    echo "read510 fail"
fi

./write10end
if [ $? -eq 0 ]; then
    echo "write10end pass"
else
    echo "write10end fail"
fi

./copy
if diff /tmp/testfile /tmp/testfile_copy >/dev/null; then
    echo "copy pass"
else
    echo "copy fail"
fi

./rename
if [ -e "/tmp/renamed_testfile" ]; then
    echo "rename pass"
else
    echo "rename fail"
fi

# Clean up
rm  /tmp/testfile_copy /tmp/renamed_testfile read100 read510 write10end copy rename

