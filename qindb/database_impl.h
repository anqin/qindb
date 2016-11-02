// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//


#ifndef QINDB_DATABASE_IMPL_H
#define QINDB_DATABASE_IMPL_H

#include <stdint.h>
#include <string>

#include "thirdparty/leveldb/db.h"
#include "thirdparty/leveldb/cleaner.h"
#include "toft/base/scoped_ptr.h"
#include "toft/system/atomic/atomic.h"
#include "toft/system/threading/mutex.h"

#include "qindb/arranger.h"
#include "qindb/log_file.h"
#include "qindb/options.h"
#include "qindb/status.h"
#include "qindb/recover.h"

namespace qindb {

class DatabaseImpl {
public:
    DatabaseImpl(const Options& options);
    ~DatabaseImpl();

    Status Put(const std::string& key, const std::string& value);
    Status Get(const std::string& key, std::string* value);
    Status Delete(const std::string& key);

    void SaveDatabase();

private:
    bool LoadDatabase(const std::string& db_path, leveldb::DB** db_handler,
                      leveldb::Cleaner* cleaner = NULL);
    void RecoverDatabase();

private:
    Options m_options;
    toft::Atomic<uint64_t> m_cur_sequence_no;
    leveldb::DB* m_index_db;
    leveldb::DB* m_garbage_db;
    leveldb::Cleaner* m_cleaner;

    toft::Mutex m_mutex;

    toft::scoped_ptr<Recover> m_recover;
    toft::scoped_ptr<LogFile> m_log_file;
    toft::scoped_ptr<Arranger> m_arranger;
};

} // namespace qindb

#endif // QINDB_DATABASE_IMPL_H
