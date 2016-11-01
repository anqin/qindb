// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#include "qindb/database.h"

#include <iostream>
#include <string>
#include <vector>

#include "thirdparty/gflags/gflags.h"
#include "thirdparty/glog/logging.h"
#include "thirdparty/leveldb/db.h"
#include "thirdparty/leveldb/options.h"
#include "toft/base/string/number.h"
#include "toft/container/counter.h"
#include "toft/crypto/random/true_random.h"
#include "toft/system/threading/this_thread.h"
#include "toft/system/threading/thread.h"

bool g_quit = false;

DEFINE_uint64(qindb_stat_key_size, 20, "");
DEFINE_uint64(qindb_stat_value_size, 20 * 1024, "");
DEFINE_bool(qindb_stat_full_print_enabled, false, "");
DEFINE_string(qindb_db_path, "my_db", "");

DECLARE_bool(qindb_is_activated);

DECLARE_COUNTER(Rate, qindb_io_user_write);
DECLARE_COUNTER(Rate, qindb_io_user_read);
DECLARE_COUNTER(Rate, qindb_io_db_write);
DECLARE_COUNTER(Rate, qindb_io_db_read);
DECLARE_COUNTER(Rate, qindb_io_arrange_write);
DECLARE_COUNTER(Rate, qindb_io_arrange_read);
DECLARE_COUNTER(Rate, qindb_io_lsm_write);
DECLARE_COUNTER(Rate, qindb_io_lsm_read);


static void SignalIntHandler(int sig) {
    LOG(INFO) << "receive interrupt signal from user, will stop";
    g_quit = true;
    FLAGS_qindb_is_activated = false;
}

void PrintStat(bool is_header) {
    if (is_header) {
        std::cout << "U_w_avg\t\tU_r_avg\t\t";
        std::cout << "S_w_avg\t\tS_r_avg\t\t";

        if (FLAGS_qindb_stat_full_print_enabled) {
            std::cout << "D_w_avg\t\tD_r_avg\t\t";
            std::cout << "A_w_avg\t\tA_r_avg\t\t";
            std::cout << "L_w_avg\t\tL_r_avg\t\t";
        }
        std::cout << std::endl;
    } else {
        uint64_t uw_avg, ur_avg;
        uint64_t dw_avg, dr_avg;
        uint64_t aw_avg, ar_avg;
        uint64_t lw_avg, lr_avg;

        COUNTER_qindb_io_user_write.GetAndReset(&uw_avg);
        COUNTER_qindb_io_user_read.GetAndReset(&ur_avg);
        COUNTER_qindb_io_db_write.GetAndReset(&dw_avg);
        COUNTER_qindb_io_db_read.GetAndReset(&dr_avg);
        COUNTER_qindb_io_arrange_write.GetAndReset(&aw_avg);
        COUNTER_qindb_io_arrange_read.GetAndReset(&ar_avg);
        COUNTER_qindb_io_lsm_write.GetAndReset(&lw_avg);
        COUNTER_qindb_io_lsm_read.GetAndReset(&lr_avg);

        std::cout
            << toft::FormatBinaryMeasure(uw_avg, " B/s") << "\t\t"
            << toft::FormatBinaryMeasure(ur_avg, " B/s") << "\t\t"
            << toft::FormatBinaryMeasure(dw_avg + lw_avg, " B/s") << "\t\t"
            << toft::FormatBinaryMeasure(dr_avg + lr_avg, " B/s") << "\t\t";

        if (FLAGS_qindb_stat_full_print_enabled) {
            std::cout
            << toft::FormatBinaryMeasure(dw_avg, " B/s") << "\t\t"
            << toft::FormatBinaryMeasure(dr_avg, " B/s") << "\t\t"
            << toft::FormatBinaryMeasure(aw_avg, " B/s") << "\t\t"
            << toft::FormatBinaryMeasure(ar_avg, " B/s") << "\t\t"
            << toft::FormatBinaryMeasure(lw_avg, " B/s") << "\t\t"
            << toft::FormatBinaryMeasure(lr_avg, " B/s") << "\t\t";
        }
        std::cout << std::endl;
    }
}

static void PrintStatRoutine() {
    PrintStat(true);
    while (!g_quit) {
        PrintStat(false);
        toft::ThisThread::Sleep(1000);
    }
}

std::string GenRandomString(uint64_t size) {
    static const char alphanum[] =
        "0123456789"
        "ABCDEFGHIJKLMNOPQRSTUVWXYZ"
        "abcdefghijklmnopqrstuvwxyz";
    char* buf = new char[size + 1];
    for (uint32_t i = 0; i < size; ++i) {
        buf[i] = alphanum[rand() % (sizeof(alphanum) - 1)];
    }
    buf[size] = '\0';
    std::string ret(buf);
    delete buf;

    return ret;
}

namespace qindb {

static void DataFillRoutine(Database* database) {
    while (!g_quit) {
        std::string random_key = GenRandomString(FLAGS_qindb_stat_key_size);
        std::string random_value = GenRandomString(FLAGS_qindb_stat_value_size);
        CHECK(random_key.size() == FLAGS_qindb_stat_key_size);
        CHECK(random_value.size() == FLAGS_qindb_stat_value_size);
        Status status = database->Put(random_key, random_value);
        if (!status.IsOk()) {
            LOG(ERROR) << "fail to put <"
                << random_key << "> to database: "
                << status.GetReason();
            continue;
        }
    }
}


static void DataFetchRoutine(Database* database) {
    while (!g_quit) {
        std::string random_key = GenRandomString(FLAGS_qindb_stat_key_size);
        CHECK(random_key.size() == FLAGS_qindb_stat_key_size);
        std::string value;
        Status status = database->Get(random_key, &value);
        if (!status.IsOk()) {
            LOG(ERROR) << "fail to get vaue of <"
                << random_key << "> to database";
            continue;
        }
        if (value.size() != FLAGS_qindb_stat_value_size) {
            LOG(ERROR) << "data mismatch";
        }
    }
}

static void DataRemoveRoutine(Database* database) {
    while (!g_quit) {
        std::string random_key = GenRandomString(FLAGS_qindb_stat_key_size);
        CHECK(random_key.size() == FLAGS_qindb_stat_key_size);
        Status status = database->Delete(random_key);
        if (!status.IsOk()) {
            LOG(ERROR) << "fail to delete <"
                << random_key << "> to database";
            continue;
        }
    }
}

void RunWorkers(Database* database, uint32_t fill_thread_num,
                uint32_t fetch_thread_num, uint32_t remove_thread_num) {
    std::cout << "===== Benchmark for QinDB =====" << std::endl;
    std::vector<toft::Thread*> threads;
    for (uint32_t i = 0; i < fill_thread_num; ++i) {
        toft::Thread* thread = new toft::Thread(std::bind(DataFillRoutine, database));
        threads.push_back(thread);
    }

    for (uint32_t i = 0; i < fetch_thread_num; ++i) {
        toft::Thread* thread = new toft::Thread(std::bind(DataFetchRoutine, database));
        threads.push_back(thread);
    }

    for (uint32_t i = 0; i < remove_thread_num; ++i) {
        toft::Thread* thread = new toft::Thread(std::bind(DataRemoveRoutine, database));
        threads.push_back(thread);
    }

    LOG(INFO) << "total started threads: " << threads.size();

    for (uint32_t i = 0; i < threads.size(); ++i) {
        threads[i]->Join();
    }
}

} // namespace qindb

namespace leveldb {

static void DataFillRoutine(DB* database) {
    while (!g_quit) {
        std::string random_key = GenRandomString(FLAGS_qindb_stat_key_size);
        std::string random_value = GenRandomString(FLAGS_qindb_stat_value_size);
        CHECK(random_key.size() == FLAGS_qindb_stat_key_size);
        CHECK(random_value.size() == FLAGS_qindb_stat_value_size);
        Status status = database->Put(WriteOptions(), random_key, random_value);
        if (!status.ok()) {
            LOG(ERROR) << "fail to put <"
                << random_key << "> to database: "
                << status.ToString();
            continue;
        }
        COUNTER_qindb_io_user_write.AddCount(random_key.size() + random_value.size());
    }
}


static void DataFetchRoutine(DB* database) {
    while (!g_quit) {
        std::string random_key = GenRandomString(FLAGS_qindb_stat_key_size);
        CHECK(random_key.size() == FLAGS_qindb_stat_key_size);
        std::string value;
        Status status = database->Get(ReadOptions(), random_key, &value);
        if (!status.ok()) {
            LOG(ERROR) << "fail to get vaue of <"
                << random_key << "> to database";
            continue;
        }
        COUNTER_qindb_io_user_read.AddCount(random_key.size() + value.size());

        if (value.size() != FLAGS_qindb_stat_value_size) {
            LOG(ERROR) << "data mismatch";
        }
    }
}

static void DataRemoveRoutine(DB* database) {
    while (!g_quit) {
        std::string random_key = GenRandomString(FLAGS_qindb_stat_key_size);
        CHECK(random_key.size() == FLAGS_qindb_stat_key_size);
        Status status = database->Delete(WriteOptions(), random_key);
        if (!status.ok()) {
            LOG(ERROR) << "fail to delete <"
                << random_key << "> to database";
            continue;
        }
    }
}

void RunWorkers(DB* database, uint32_t fill_thread_num,
                uint32_t fetch_thread_num, uint32_t remove_thread_num) {
    std::cout << "===== Benchmark for LevelDB =====" << std::endl;
    std::vector<toft::Thread*> threads;
    for (uint32_t i = 0; i < fill_thread_num; ++i) {
        toft::Thread* thread = new toft::Thread(std::bind(DataFillRoutine, database));
        threads.push_back(thread);
    }

    for (uint32_t i = 0; i < fetch_thread_num; ++i) {
        toft::Thread* thread = new toft::Thread(std::bind(DataFetchRoutine, database));
        threads.push_back(thread);
    }

    for (uint32_t i = 0; i < remove_thread_num; ++i) {
        toft::Thread* thread = new toft::Thread(std::bind(DataRemoveRoutine, database));
        threads.push_back(thread);
    }

    LOG(INFO) << "total started threads: " << threads.size();

    for (uint32_t i = 0; i < threads.size(); ++i) {
        threads[i]->Join();
    }
}

} // namespace qindb

void Usage(const std::string& prg_name) {
    std::cout << "$ " << prg_name << "    [OPERATION]    [OPTIONS]" << std::endl;
    std::cout << "examples: " << std::endl;
    std::cout << "    qindb  <fill threads> <fetch threads> <remove threads>" << std::endl;
    std::cout << "    leveldb  <fill threads> <fetch threads> <remove threads> " << std::endl;
}


int main(int argc, char* argv[]) {
    ::google::ParseCommandLineFlags(&argc, &argv, true);
    ::google::InitGoogleLogging(argv[0]);

    if (argc < 5) {
        Usage(argv[0]);
        return -1;
    }
    toft::Thread print_thread;
    print_thread.Start(PrintStatRoutine);

    signal(SIGINT, SignalIntHandler);
    signal(SIGTERM, SignalIntHandler);

    int ret = 0;
    std::string cmd = argv[1];
    uint32_t fill_num, fetch_num, remove_num;
    toft::StringToNumber(argv[2], &fill_num);
    toft::StringToNumber(argv[3], &fetch_num);
    toft::StringToNumber(argv[4], &remove_num);
    if (cmd == "qindb") {
        qindb::Options options;
        options.base_dir = FLAGS_qindb_db_path;
        options.file_size = (32 << 20);
        qindb::Database database(options);
        qindb::RunWorkers(&database, fill_num, fetch_num, remove_num);
    } else if (cmd == "leveldb") {
        leveldb::Options options;
        options.create_if_missing = true;
        leveldb::DB* database = NULL;
        leveldb::Status status =
            leveldb::DB::Open(options, FLAGS_qindb_db_path, &database);
        CHECK(status.ok()) << ", fail to open db: " << FLAGS_qindb_db_path
            << ", status: " << status.ToString();
        leveldb::RunWorkers(database, fill_num, fetch_num, remove_num);
        delete database;
    } else {
        Usage(argv[0]);
    }

    print_thread.Join();

    return ret;
}

