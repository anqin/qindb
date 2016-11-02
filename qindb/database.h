// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//


#ifndef QINDB_DATABASE_H
#define QINDB_DATABASE_H

#include <stdint.h>
#include <string>

#include "qindb/status.h"
#include "qindb/options.h"

namespace qindb {

class DatabaseImpl;

class Database {
public:
    Database(const Options& options);
    ~Database();

    Status Put(const std::string& key, const std::string& value);
    Status Get(const std::string& key, std::string* value);
    Status Delete(const std::string& key);

    void SaveDatabase();

private:
    DatabaseImpl* m_impl;
};

} // namespace qindb

#endif // QINDB_DATABASE_H
