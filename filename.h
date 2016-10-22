// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#ifndef QINDEXDB_FILENAME_H
#define QINDEXDB_FILENAME_H

#include <stdint.h>
#include <string>

namespace qindexdb {
namespace utils {

std::string LogFileName(uint64_t file_no);

} // namespace utils
} // namespace qindexdb

#endif // QINDB_FILENAME_H
