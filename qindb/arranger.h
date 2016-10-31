// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#ifndef QINDB_ARRANGER_H
#define QINDB_ARRANGER_H

#include "thirdparty/leveldb/db.h"
#include "toft/system/threading/base_thread.h"
#include "toft/system/threading/event.h"

#include "qindb/options.h"
#include "qindb/log_file.h"

namespace qindb {

class Arranger : public toft::BaseThread {
public:
    Arranger(const Options& options, LogFile* log_file,
             leveldb::DB* index_db, leveldb::DB* garbage_db);
    ~Arranger();

protected:
    void Entry();

private:
    void ArrangeFiles();
    void ArrangeSingleFile(uint32_t file_no);
    void RemoveFile(uint32_t file_no);

private:
    Options m_options;
    LogFile* m_log_file;
    leveldb::DB* m_index_db;
    leveldb::DB* m_garbage_db;

    toft::AutoResetEvent m_schedule_event;
};

} // namespace qindb

#endif // QINDB_ARRANGER_H
