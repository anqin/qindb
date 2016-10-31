// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//

#include "qindb/arranger.h"

#include <algorithm>
#include <map>
#include <vector>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "toft/base/scoped_ptr.h"
#include "toft/container/counter.h"
#include "toft/storage/file/file.h"
#include "toft/storage/path/path_ext.h"

#include "qindb/filename.h"
#include "qindb/proto_helper.h"

DEFINE_bool(qindb_is_activated, true, "");

DECLARE_COUNTER(Rate, qindb_io_arrange_write);
DECLARE_COUNTER(Rate, qindb_io_arrange_read);

namespace qindb {

Arranger::Arranger(const Options& options, LogFile* log_file,
                   leveldb::DB* index_db, leveldb::DB* garbage_db)
    : m_options(options), m_log_file(log_file),
      m_index_db(index_db), m_garbage_db(garbage_db) {}

Arranger::~Arranger() {
    SendStopRequest();
    if (IsJoinable()) {
        Join();
    } else {
        VLOG(20) << "thread is not started";
    }

}

void Arranger::Entry() {
    while (!IsStopRequested()) {
        ArrangeFiles();
        if (!m_schedule_event.TimedWait(m_options.arrange_schedule_period)) {
        }
        if (FLAGS_qindb_is_activated) {
            SendStopRequest();
        }
    }
}

typedef std::pair<uint32_t, uint32_t> KeyValuePair;
bool CmpFunc(const KeyValuePair& lhs, const KeyValuePair& rhs) {
    return lhs.second > rhs.second;
}

void Arranger::ArrangeFiles() {
    VLOG(10) << "ArrangeFiles()";
    std::map<uint32_t, uint32_t> stat;
    leveldb::Iterator* db_iter = m_garbage_db->NewIterator(leveldb::ReadOptions());
    db_iter->SeekToFirst();
    for (; db_iter->Valid() && FLAGS_qindb_is_activated; db_iter->Next()) {
        uint32_t log_file_no;
#if 0
        leveldb::Slice key_slice = db_iter->key();
        leveldb::Slice log_file_no_slice;
        CHECK(GetLengthPrefixedSlice(&key_slice, &log_file_no_slice));
        CHECK(GetVarint64(&log_file_no_slice, &log_file_no));
#else
        RecordLocation location;
        if (!StringToPB<RecordLocation>(db_iter->value().ToString(), &location)) {
            LOG(ERROR) << "fail to parse record locatio in arranger";
            continue;
        }
        log_file_no = location.log_file_no();
#endif
        std::map<uint32_t, uint32_t>::iterator it =  stat.find(log_file_no);
        if (it == stat.end()) {
            stat[log_file_no] = 1;
        } else {
            stat[log_file_no] = stat[log_file_no] + 1;
        }
    }
    delete db_iter;

    std::vector<KeyValuePair> clean_list(stat.begin(), stat.end());
    std::sort(clean_list.begin(), clean_list.end(), CmpFunc);

    uint64_t total_arranged_key_num = 0;
    for (uint32_t i = 0; i < clean_list.size()
         && FLAGS_qindb_is_activated; ++i) {
        ArrangeSingleFile(clean_list[i].first);
        total_arranged_key_num += clean_list[i].second;
        if (!FLAGS_qindb_is_activated) {
            RemoveFile(clean_list[i].first);
        }
    }
    VLOG(10) << "total arranged key num: " << total_arranged_key_num;
}

void Arranger::ArrangeSingleFile(uint32_t file_no) {
    VLOG(10) << "ArrangeSingleFile(): " << file_no;
    std::string filename = utils::LogFileName(file_no);
    toft::scoped_ptr<toft::File> file(toft::File::Open(
            m_options.base_dir + "/" + filename, "r"));
    if (file.get() == NULL) {
        LOG(WARNING) << "not exit: " << filename;
        return;
    }
    toft::scoped_ptr<toft::RecordReader> record_reader(new toft::RecordReader(file.get()));

    LogRecord log_record;
    while (record_reader->ReadNextMessage(&log_record)
           && FLAGS_qindb_is_activated) {
        COUNTER_qindb_io_arrange_read.AddCount(log_record.ByteSize());
        std::string value;
        leveldb::Status db_status = m_garbage_db->Get(leveldb::ReadOptions(),
                                                      log_record.key(), &value);
        if (db_status.ok()) {
            // key is in garbage db, should be removed
            continue;
        }

        RecordLocation new_location;
        Status status;
        if (!m_log_file->Append(log_record, &new_location, &status)) {
            LOG(ERROR) << "fail to append to log file in arranger";
            continue;
        }
        COUNTER_qindb_io_arrange_write.AddCount(log_record.ByteSize());

        value = "";
        if (!PBToString<RecordLocation>(new_location, &value)) {
            LOG(ERROR) << "fail to serialize pb in arranger";
            continue;
        }

        db_status = m_index_db->Put(leveldb::WriteOptions(), log_record.key(), value);
        if (!db_status.ok()) {
            LOG(ERROR) << "fail to update the index in arranger";
            continue;
        }
        db_status = m_garbage_db->Delete(leveldb::WriteOptions(), log_record.key());
        if (!db_status.ok()) {
            LOG(ERROR) << "fail to remove the key from garbage db in arranger";
        }
    }
}

void Arranger::RemoveFile(uint32_t file_no) {
    VLOG(10) << "RemoveFile(): " << file_no;
    std::string filename = utils::LogFileName(file_no);
    if (!toft::RemoveLocalFile(m_options.base_dir + "/" + filename)) {
        LOG(ERROR) << "fail to remvoe file: " << filename;
    }
}

} // namespace qindb
