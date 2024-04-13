#!/bin/bash
touch /tmp/file1
chmod 644 /tmp/file1
./testopen /tmp/file1
if [ $? -eq 0 ]
then
    echo "testopen Pass"
else
    echo "testopen Fail"
fi
