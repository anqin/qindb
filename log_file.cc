// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#include "qindexdb/log_file.h"

#include "qindexdb/filename.h"

namespace qindexdb {


LogFile::LogFile(const Options& options)
    : m_cur_file(NULL), m_reader(NULL), m_writer(NULL),
      m_options(options), m_cur_file_size(0) {}

LogFile::~LogFile() {}

bool LogFile::InitFile(uint64_t seq_no) {
    std::string filename = utils::LogFileName(seq_no);
    m_cur_file.reset(toft::File::Open(m_options.base_dir + "/" + filename, "a"));
    m_writer.reset(new toft::RecordWriter(m_cur_file.get()));
    m_cur_file_no = seq_no;
    m_cur_file_size = 0;
    return true;
}

bool LogFile::Append(const LogRecord& log_record, RecordLocation* location,
                     Status* status) {
    if (!m_cur_file.get()) {
        InitFile(log_record.seq_no());
        CHECK(m_cur_file.get() != NULL);
    }
    if (m_writer->WriteMessage(log_record)) {
        location->set_log_file_no(m_cur_file_no);
        location->set_offset(m_cur_file_size);
        location->set_record_size(log_record.ByteSize());

        m_cur_file_size += (log_record.ByteSize() + sizeof(uint32_t));
        if (m_cur_file_size > m_options.file_size) {
            m_cur_file->Close();
            m_cur_file.reset();
        }
    }

    return true;
}


bool LogFile::ReadRecord(const RecordLocation& location, LogRecord* record,
                         Status* status) {
    std::string filename = utils::LogFileName(location.log_file_no());
    toft::scoped_ptr<toft::File> file(toft::File::Open(
            m_options.base_dir + "/" + filename, "r"));
    if (!file->Seek(location.offset(), SEEK_SET)) {
        LOG(ERROR) << "fail to seek [" << filename << "] to offset: " << location.offset();
        status->SetFailed(Status::kSystemError, "seek record fail");
        return false;
    }
    toft::scoped_array<char> m_buffer(new char[location.record_size() + sizeof(uint32_t)]);
    if (!file->Read(m_buffer.get(), location.record_size() + sizeof(uint32_t))) {
        LOG(ERROR) << "fail to read from file: " << filename;
        status->SetFailed(Status::kSystemError, "read record fail");
        return false;
    }

    if (!record->ParseFromArray(m_buffer.get() + sizeof(uint32_t), location.record_size())) {
        LOG(ERROR) << "fail to parse record from buffer";
        status->SetFailed(Status::kSystemError, "parse record fail");
        return false;
    }
    return true;
}



} // namespace qindexdb
