This is the repo for Quick-Index Databse (Q-IndexDB)


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
