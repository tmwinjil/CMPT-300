#!/bin/sh
#*************** YOU SHOULD NOT MODIFY ANYTHING IN THIS FILE ***************

rm -rf ./testdata
mkdir -p ./testdata/1/2/3/4/5/6/7/8/9/10/11/12/13/14/15/16/17/18/19/20
echo -n "Hello" > ./testdata/hello.txt
echo -n "OldFile" > ./testdata/oldfile.txt

cp ./testdata/hello.txt ./testdata/1/2/3/4/5/6/7/8/9/10/
cp ./testdata/oldfile.txt ./testdata/1/2/3/4/5/6/7/8/9/

touch  -m -d 20050101 ./testdata/oldfile.txt

touch  -a -d 20050103 ./testdata/oldfile.txt

if [ ! -f /tmp/bigtestscratchfile2forLMP1 ] ; then
	echo "Creating large file  (first time only)"
	cat /usr/lib/*.so >/tmp/bigtestscratchfile2forLMP1
	chmod 644 /tmp/bigtestscratchfile2forLMP1
fi

ln -s /tmp/bigtestscratchfile2forLMP1  ./testdata/bigfile 

exit 0
