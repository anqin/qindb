// Copyright (C) 2016, Baidu Inc.
// Author: An Qin (qinan@baidu.com)
//
// Description:
//

#include "qindb/status.h"

namespace qindb {

Status::Status()
    : m_type(kOk) {}

void Status::SetFailed(Type type, const std::string& reason) {
    m_type = type;
    m_reason = reason;
}

std::string Status::GetReason() const {
    return m_reason;
}

Status::Type Status::GetType() const {
    return m_type;
}

std::string Status::ToString() const {
    switch (m_type) {
    case kOk:
        return "kOk";
    case kNotFound:
        return "kNotFound";
    case kSystemError:
        return "kSystemError";
    case kTimeout:
        return "kTimeout";
    case kBusy:
        return "kBusy";
    case kNoAuth:
        return "kNoAuth";
    case kNotImpl:
        return "kNotImpl";
    case kUnknown:
    default:
        return "kUnknown";
    }
}

bool Status::IsOk() const {
    return m_type == kOk;
}
} // namespace qindb
