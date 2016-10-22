// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//

#ifndef QINDEXDB_QIN_CLEANER_H
#define QINDEXDB_QIN_CLEANER_H

#include "thirdparty/leveldb/cleaner.h"
#include "thirdparty/leveldb/db.h"
#include "thirdparty/leveldb/slice.h"

namespace qindexdb {

class QinCleaner : public leveldb::Cleaner {
public:
    QinCleaner(leveldb::DB* garbage_db);
    virtual ~QinCleaner();

    virtual bool Clean(const leveldb::Slice& key, const leveldb::Slice& value);

private:
    leveldb::DB* m_garbage_db;
};

} // namespace qindexdb

#endif // QINDEXDB_QIN_CLEANER_H
