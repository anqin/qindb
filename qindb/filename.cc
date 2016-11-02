// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//

#include "qindb/filename.h"

#include "toft/base/string/format.h"

namespace qindb {
namespace utils {

std::string LogFileName(uint64_t file_no) {
    return toft::StringPrint("logfile_%06d", file_no);
}

} // namespace utils
} // namespace qindb
