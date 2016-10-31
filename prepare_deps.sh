#!/bin/bash

git clone https://github.com/anqin/thirdparty
git clone https://github.com/anqin/toft
rm -rf thirdparty/leveldb*
cp -rf leveldb_modified/* thirdparty/
