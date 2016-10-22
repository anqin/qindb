// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//


#ifndef QINDEXDB_DB_H
#define QINDEXDB_DB_H

#include <stdint.h>
#include <string>

#include "thirdparty/leveldb/db.h"
#include "thirdparty/leveldb/cleaner.h"
#include "toft/base/scoped_ptr.h"
#include "toft/system/atomic/atomic.h"
#include "toft/system/threading/mutex.h"

#include "qindexdb/arranger.h"
#include "qindexdb/log_file.h"
#include "qindexdb/options.h"
#include "qindexdb/status.h"

namespace qindexdb {

class Database {
public:
    Database(const Options& options);
    ~Database();

    Status Put(const std::string& key, const std::string& value);
    Status Get(const std::string& key, std::string* value);
    Status Delete(const std::string& key);

private:
    bool LoadDatabase(const std::string& db_path, leveldb::DB** db_handler,
                      leveldb::Cleaner* cleaner = NULL);

private:
    Options m_options;
    toft::Atomic<uint64_t> m_cur_sequence_no;
    leveldb::DB* m_index_db;
    leveldb::DB* m_garbage_db;
    leveldb::Cleaner* m_cleaner;

    toft::Mutex m_mutex;
    LogFile m_log_file;

    toft::scoped_ptr<Arranger> m_arranger;
};

} // namespace qindexdb

#endif // QINDEXDB_DB_H
