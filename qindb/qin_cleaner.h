// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#ifndef QINDB_QIN_CLEANER_H
#define QINDB_QIN_CLEANER_H

#include "thirdparty/leveldb/cleaner.h"
#include "thirdparty/leveldb/db.h"
#include "thirdparty/leveldb/slice.h"

namespace qindb {

class QinCleaner : public leveldb::Cleaner {
public:
    QinCleaner(leveldb::DB* garbage_db);
    virtual ~QinCleaner();

    virtual bool Clean(const leveldb::Slice& key, const leveldb::Slice& value) const;

private:
    leveldb::DB* m_garbage_db;
};

} // namespace qindb

#endif // QINDB_QIN_CLEANER_H
