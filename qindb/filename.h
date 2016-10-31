// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#ifndef QINDB_FILENAME_H
#define QINDB_FILENAME_H

#include <stdint.h>
#include <string>

namespace qindb {
namespace utils {

std::string LogFileName(uint64_t file_no);

} // namespace utils
} // namespace qindb

#endif // QINDB_FILENAME_H
