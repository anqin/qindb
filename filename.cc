// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#include "qindexdb/filename.h"

#include "toft/base/string/format.h"

namespace qindexdb {
namespace utils {

std::string LogFileName(uint64_t file_no) {
    return toft::StringPrint("logfile_%06d", file_no);
}

} // namespace utils
} // namespace qindexdb
