// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//

#ifndef QINDB_RECOVER_H
#define QINDB_RECOVER_H

#include "toft/system/threading/mutex.h"
#include "toft/base/scoped_ptr.h"
#include "toft/storage/recordio/recordio.h"
#include "toft/storage/file/file.h"

#include "qindb/schema.pb.h"
#include "qindb/options.h"

namespace qindb {

class Recover {
public:
    Recover(const Options& options);
    ~Recover();

    bool Checkpoint(const MetaInfo& meta_info);
    void Restart();

    MetaInfo GetMetaInfo();

private:
    bool RepairMetaInfo();
    void InitFile();

private:
    toft::Mutex m_mutex;
    Options m_options;
    MetaInfo m_meta_info;

//     toft::scoped_ptr<toft::File> m_file;
//     toft::scoped_ptr<toft::ReverseRecordWriter> m_writer;
};

} // namespace qindb

#endif // QINDB_RECOVER_H
