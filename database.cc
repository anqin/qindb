// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#include "qindexdb/database.h"

#include "toft/storage/path/path_ext.h"

#include "qindexdb/proto_helper.h"
#include "qindexdb/qin_cleaner.h"
#include "qindexdb/schema.pb.h"

namespace qindexdb {


Database::Database(const Options& options)
    : m_options(options), m_cur_sequence_no(0),
      m_index_db(NULL), m_garbage_db(NULL), m_log_file(options) {
    if (!toft::IsExist(m_options.base_dir)) {
        CHECK(toft::CreateDirWithRetry(m_options.base_dir));
    }
    LoadDatabase(options.base_dir + "/garbage_db", &m_garbage_db);
    m_cleaner = new QinCleaner(m_garbage_db);
    LoadDatabase(options.base_dir + "/index_db", &m_index_db, m_cleaner);
    m_arranger.reset(new Arranger(options, &m_log_file, m_index_db, m_garbage_db));

    m_arranger->Start();
}

Database::~Database() {
    delete m_index_db;
    delete m_garbage_db;
    delete m_cleaner;
}

Status Database::Put(const std::string& key, const std::string& value) {
    toft::MutexLocker lock(&m_mutex);
    LogRecord log_record;
    log_record.set_seq_no(m_cur_sequence_no++);
    log_record.set_key(key);
    log_record.set_value(value);

    RecordLocation location;
    Status status;
    if (!m_log_file.Append(log_record, &location, &status)) {
        LOG(ERROR) << "fail to append log";
        return status;
    }

    std::string buffer_str;
    if (!PBToString<RecordLocation>(location, &buffer_str)) {
        LOG(ERROR) << "fail to serialize pb record";
        status.SetFailed(Status::kSystemError, "fail to serialize pb");
        return status;
    }

    leveldb::Status db_status =
        m_index_db->Put(leveldb::WriteOptions(), key, buffer_str);
    if (!db_status.ok()) {
        LOG(ERROR) << "fail to put into index db";
        status.SetFailed(Status::kSystemError, "fail to put into index db");
        return status;
    }
    return status;
}

Status Database::Get(const std::string& key, std::string* value) {
    Status status;
    std::string buffer_str;
    leveldb::Status db_status =
        m_index_db->Get(leveldb::ReadOptions(), key, &buffer_str);
    if (!db_status.ok()) {
        LOG(ERROR) << "fail to get from index db";
        status.SetFailed(Status::kSystemError, "fail to get from index db");
        return status;
    }

    RecordLocation location;
    if (!StringToPB<RecordLocation>(buffer_str, &location)) {
        LOG(ERROR) << "fail to parse pb from string";
        status.SetFailed(Status::kSystemError, "fail to parse pb from string");
        return status;
    }

    LogRecord log_record;
    if (!m_log_file.ReadRecord(location, &log_record, &status)) {
        LOG(ERROR) << "fail to read record from log file";
        status.SetFailed(Status::kSystemError, "fail to read record from log file");
        return status;
    }

    *value = log_record.value();
    return status;
}

Status Database::Delete(const std::string& key) {
    Status status;
    leveldb::Status db_status =
        m_index_db->Delete(leveldb::WriteOptions(), key);
    if (!db_status.ok()) {
        LOG(ERROR) << "fail to delete from index db";
        status.SetFailed(Status::kSystemError, "fail to delete from index db");
        return status;
    }

    return status;
}

bool Database::LoadDatabase(const std::string& db_path, leveldb::DB** db_handler,
                            leveldb::Cleaner* cleaner) {
    leveldb::Options options;
    options.create_if_missing = true;
    options.cleaner = cleaner;
    leveldb::Status status = leveldb::DB::Open(options, db_path, db_handler);
    CHECK(status.ok()) << ", fail to open db: " << db_path
        << ", status: " << status.ToString();
    return true;
}

} // namespace qindexdb
