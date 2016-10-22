// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//

#ifndef QINDEXDB_PROTO_HELPER_H
#define QINDEXDB_PROTO_HELPER_H

#include <stdint.h>

#include <string>

#include "qindexdb/schema.pb.h"

namespace qindexdb {


template<typename PbMessage>
bool PBToString(const PbMessage& message, std::string* output) {
    if (!message.IsInitialized()) {
        LOG(ERROR) << "missing required fields: "
                << message.InitializationErrorString();
        return false;
    }
    if (!message.AppendToString(output)) {
        LOG(ERROR) << "fail to convert to string";
        return false;
    }

    return true;
}

template<typename PbMessage>
bool StringToPB(const std::string& str, PbMessage* message) {
    if (!message->ParseFromArray(str.c_str(), str.size())) {
        LOG(WARNING) << "missing required fields: "
            << message->InitializationErrorString();;
        return false;
    }
    return true;
}


} // namespace qindexdb

#endif // QINDEXDB_PROTO_HELPER_H
