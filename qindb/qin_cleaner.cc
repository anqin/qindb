// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//

#include "qindb/qin_cleaner.h"

#include "thirdparty/leveldb/status.h"
#include "thirdparty/leveldb-1.9.0/util/coding.h"

#include "qindb/proto_helper.h"
#include "qindb/schema.pb.h"

namespace qindb {


QinCleaner::QinCleaner(leveldb::DB* garbage_db)
    : m_garbage_db(garbage_db) {}

QinCleaner::~QinCleaner() {}

bool QinCleaner::Clean(const leveldb::Slice& key, const leveldb::Slice& value) const {
    VLOG(20) << "clean() is activated: value size = " << value.ToString().size();
    if (value.ToString().size() == 0) {
        return true;
    }
    RecordLocation location;
    if (!StringToPB<RecordLocation>(value.ToString(), &location)) {
        LOG(ERROR) << "fail to parse record in leveldb";
    }
#if 0
    RecordLocation location;
    if (!StringToPB<RecordLocation>(value.ToString(), &location)) {
        LOG(ERROR) << "fail to parse record in leveldb";
        return false;
    }
    std::string file_no_str;
    leveldb::PutVarint64(&file_no_str, location.log_file_no());
    leveldb::Slice garbage_key;
    leveldb::PutLengthPrefixedSlice(&garbage_key, file_no_str);
    leveldb::PutLengthPrefixedSlice(&garbage_key, key);
    leveldb::Status db_status = m_garbage_db->Put(leveldb::WriteOptions(), garbage_key, value);
#else
    leveldb::Status db_status = m_garbage_db->Put(leveldb::WriteOptions(), key, value);
#endif

    if (!db_status.ok()) {
        LOG(ERROR) << "fail to put into garbage db in leveldb";
    }
    return db_status.ok();
}


} // namespace qindb
