// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#include "qindb/database.h"

#include "qindb/database_impl.h"

namespace qindb {


Database::Database(const Options& options) {
    m_impl = new DatabaseImpl(options);
}

Database::~Database() {
    delete m_impl;
}

Status Database::Put(const std::string& key, const std::string& value) {
    return m_impl->Put(key, value);
}

Status Database::Get(const std::string& key, std::string* value) {
    return m_impl->Get(key, value);
}

Status Database::Delete(const std::string& key) {
    return m_impl->Delete(key);
}

void Database::SaveDatabase() {
    return m_impl->SaveDatabase();
}

} // namespace qindb
