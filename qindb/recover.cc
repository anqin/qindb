// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#include "qindb/recover.h"

#include "thirdparty/glog/logging.h"
#include "toft/storage/path/path_ext.h"

namespace qindb {

Recover::Recover(const Options& options)
    : m_options(options) {
    m_meta_info.set_last_log_file_no(0);
    m_meta_info.set_last_log_file_size(0);
    m_meta_info.set_last_seq_no(0);
    Restart();
    InitFile();
}

Recover::~Recover() {}

void Recover::InitFile() {
//     m_file.reset(toft::File::Open(
//             m_options.base_dir + "/recover.db", "a"));
//     m_writer.reset(new toft::ReverseRecordWriter(m_file.get()));
}

bool Recover::Checkpoint(const MetaInfo& meta_info) {
    toft::scoped_ptr<toft::File> file(toft::File::Open(
            m_options.base_dir + "/recover.db", "w"));
    toft::scoped_ptr<toft::RecordWriter> writer(new toft::RecordWriter(file.get()));
    MetaInfo info_data = meta_info;
    if (!writer->WriteMessage(info_data)) {
        LOG(ERROR) << "fail to write meta info";
        return false;
    }
//     file->Close();
    return true;
}

void Recover::Restart() {
    toft::scoped_ptr<toft::File> file(toft::File::Open(
            m_options.base_dir + "/recover.db", "r"));
    if (!file.get()) {
        LOG(WARNING) << "recover file not exist, try full-recovering";
        return;
    }
    toft::RecordReader reader(file.get());

//     if (reader.Next() != 1) {
//         LOG(WARNING) << "not meta info in recover file, try full-recovering";
//         return;
//     }

    m_meta_info.Clear();
    if (!reader.ReadNextMessage(&m_meta_info)) {
        LOG(ERROR) << "fail to read meta info";
        return;
    }
    file->Close();
}

MetaInfo Recover::GetMetaInfo() {
    return m_meta_info;
}

bool Recover::RepairMetaInfo() {
    return false;
}

} // namespace qindb
