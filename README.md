# Quick-Index Databse (Q-IndexDB)

 Quick-Index Databse (Q-IndexDB) is the next-generation storage engine used in low-latency key-value storage, to shoot the read/write amplification in Log-Structured Merge-Tree (LSM Tree) engine. Now, it has back in close-source development and be launched in online databases in our real-life product system. Thanks for your attention.


## Build the codes

This repo version is base one.

1. Install the BLADE 

    $ git clone https://github.com/chen3feng/typhoon-blade

2. Download the dependant libraries

    $ cd qindb/
    
    $ ./prepare_deps.sh

3. Build the qindb

    $ blade build -pdebug ...

4. Run the loop test

    $ cd qindb/qindb/benkmark
    
    $ a
    
    $ ./loop_main qindb 10 0 3
