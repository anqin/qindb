// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#ifndef STORAGE_LEVELDB_INCLUDE_CLEANER_H_
#define STORAGE_LEVELDB_INCLUDE_CLEANER_H_

namespace leveldb {

class Slice;

class Cleaner {
public:
    virtual ~Cleaner() {}

    virtual bool Clean(const Slice& key, const Slice& value) const {}
};

} // namespace leveldb

#endif // STORAGE_LEVELDB_INCLUDE_CLEANER_H_
