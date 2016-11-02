// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
// Description:
//

#ifndef QINDB_PROTO_HELPER_H
#define QINDB_PROTO_HELPER_H

#include <stdint.h>

#include <string>

#include "qindb/schema.pb.h"

namespace qindb {


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


} // namespace qindb

#endif // QINDB_PROTO_HELPER_H
