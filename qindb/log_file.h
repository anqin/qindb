// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//


#ifndef QINDB_QINDB_LOG_FILE_H
#define QINDB_QINDB_LOG_FILE_H

#include <stdint.h>
#include <string>

#include "toft/base/scoped_ptr.h"
#include "toft/storage/file/file.h"
#include "toft/storage/recordio/recordio.h"
#include "toft/system/threading/mutex.h"

#include "qindb/options.h"
#include "qindb/status.h"
#include "qindb/schema.pb.h"

namespace qindb {

class DatabaseImpl;

class LogFile {
public:
    LogFile(const Options& options, DatabaseImpl* database);
    ~LogFile();

    bool Append(const LogRecord& log_record, RecordLocation* location,
                Status* status);

    bool ReadRecord(const RecordLocation& location, LogRecord* record,
                    Status* status);

    void NeedCheckpoint(MetaInfo* meta_info);
    void Recover(const MetaInfo& meta_info);

private:
    bool InitFile(uint64_t seq_no, int64_t file_size = 0);

private:
    toft::Mutex m_mutex;
    toft::scoped_ptr<toft::File> m_cur_file;
    toft::scoped_ptr<toft::RecordReader> m_reader;
    toft::scoped_ptr<toft::RecordWriter> m_writer;

    Options m_options;
    DatabaseImpl* m_database;
    uint64_t m_cur_file_no;
    int64_t m_cur_file_size;
};


} // namespace qindb

#endif // QINDB_QINDB_LOG_FILE_H
