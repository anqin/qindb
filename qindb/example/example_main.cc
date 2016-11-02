// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//

#include "qindb/database.h"

#include <string>
#include <iostream>

namespace qindb {


int32_t BaseOp(Database* database, int32_t argc, char** argv, Status* status) {
    std::string cmd = argv[1];
    if (cmd == "put") {
        if (argc < 4) {
            return -1;
        }
        *status = database->Put(argv[2], argv[3]);
    } else if (cmd == "get") {
        if (argc < 3) {
            return -1;
        }
        std::string value;
        *status = database->Get(argv[2], &value);
        if (status->IsOk()) {
            std::cout << value << std::endl;
        }
    } else if (cmd == "delete") {
        if (argc < 3) {
            return -1;
        }
        *status = database->Delete(argv[2]);
    }

    if (!status->IsOk()) {
        std::cerr << status->ToString() << ": "
            << status->GetReason() << std::endl;
        return -1;
    }
    return 0;
}


} // namespace qindb


void Usage(const std::string& prg_name) {
    std::cout << "$ " << prg_name << "    [OPERATION]    [OPTIONS]" << std::endl;
    std::cout << "examples: " << std::endl;
    std::cout << "    fill  <key num>" << std::endl;
    std::cout << "    fetch  <key num>" << std::endl;
    std::cout << "    remove  <key num>" << std::endl;
    std::cout << "    put  <key>  <value>" << std::endl;
    std::cout << "    get  <key>  " << std::endl;
    std::cout << "    delete  <key>  " << std::endl;
}


int main(int argc, char* argv[]) {
//     ::google::ParseCommandLineFlags(&argc, &argv, true);
//     ::google::InitGoogleLogging(argv[0]);
    if (argc < 2) {
        Usage(argv[0]);
        return -1;
    }

    int ret = 0;
    qindb::Status status;
    qindb::Options options;
    options.base_dir = "my_db";
    options.file_size = (8 << 10);
    qindb::Database database(options);
    std::string cmd = argv[1];
    if (cmd == "put" || cmd == "get" || cmd == "delete") {
        ret = qindb::BaseOp(&database, argc, argv, &status);
    } else {
        Usage(argv[0]);
    }
    return ret;
}

