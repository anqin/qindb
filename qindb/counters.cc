// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//


#include "toft/container/counter.h"

DEFINE_COUNTER(Rate, qindb_io_user_write, "write io bandwidth from user space");
DEFINE_COUNTER(Rate, qindb_io_user_read, "read io bandwidth from user space");
DEFINE_COUNTER(Rate, qindb_io_db_write, "write io bandwidth from system space");
DEFINE_COUNTER(Rate, qindb_io_db_read, "read io bandwidth from system space");
DEFINE_COUNTER(Rate, qindb_io_arrange_write, "read io bandwidth from arranger component");
DEFINE_COUNTER(Rate, qindb_io_arrange_read, "read io bandwidth from arranger component");
DEFINE_COUNTER(Rate, qindb_io_lsm_write, "write io bandwidth from LSM component");
DEFINE_COUNTER(Rate, qindb_io_lsm_read, "read io bandwidth from LSM component");
DEFINE_COUNTER(Rate, qindb_io_lsm_index_write, "write io bandwidth from LSM-index component");
DEFINE_COUNTER(Rate, qindb_io_lsm_index_read, "read io bandwidth from LSM-index component");
DEFINE_COUNTER(Rate, qindb_io_lsm_garbage_write, "write io bandwidth from LSM-garbage component");
DEFINE_COUNTER(Rate, qindb_io_lsm_garbage_read, "read io bandwidth from LSM-garbage component");
