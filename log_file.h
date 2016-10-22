// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//


#ifndef QINDB_QINDEXDB_LOG_FILE_H
#define QINDB_QINDEXDB_LOG_FILE_H

#include <stdint.h>
#include <string>

#include "toft/base/scoped_ptr.h"
#include "toft/storage/file/file.h"
#include "toft/storage/recordio/recordio.h"

#include "qindexdb/options.h"
#include "qindexdb/status.h"
#include "qindexdb/schema.pb.h"

namespace qindexdb {

class LogFile {
public:
    LogFile(const Options& options);
    ~LogFile();

    bool Append(const LogRecord& log_record, RecordLocation* location,
                Status* status);

    bool ReadRecord(const RecordLocation& location, LogRecord* record,
                    Status* status);

private:
    bool InitFile(uint64_t seq_no);

private:
    toft::scoped_ptr<toft::File> m_cur_file;
    toft::scoped_ptr<toft::RecordReader> m_reader;
    toft::scoped_ptr<toft::RecordWriter> m_writer;

    Options m_options;
    uint64_t m_cur_file_no;
    int64_t m_cur_file_size;
};


} // namespace qindexdb

#endif // QINDB_QINDEXDB_LOG_FILE_H
