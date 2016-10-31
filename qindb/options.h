// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#ifndef QINDB_QINDB_OPTIONS_H
#define QINDB_QINDB_OPTIONS_H

#include <stdint.h>
#include <string>

namespace qindb {

struct Options {
    uint32_t hash_num;
    uint32_t channel_num;
    uint32_t block_num;
    uint32_t page_in_block_num;

    uint32_t page_size;
    uint32_t block_size;

    uint32_t garbage_thread_num;

    // new
    int64_t file_size;
    int64_t arrange_schedule_period;
    std::string base_dir;

    Options();
};

} // namespace qindb

#endif // QINDB_QINDB_OPTIONS_H
