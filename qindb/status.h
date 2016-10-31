// Copyright (C) 2016, for QinDB's authors.
// Author: An Qin (anqin.qin@gmail.com)
//
//

#ifndef QINDB_QINDB_STATUS_H
#define QINDB_QINDB_STATUS_H

#include <string>

namespace qindb {

class Status {
public:
    enum Type {
        kOk = 0,
        kNotFound,
        kSystemError,
        kTimeout,
        kBusy,
        kNoAuth,
        kUnknown,
        kNotImpl
    };

    Status();

    void SetFailed(Type type, const std::string& reason = "");
    std::string GetReason() const;
    Type GetType() const;
    std::string ToString() const;
    bool IsOk() const;

private:
    Type m_type;
    std::string m_reason;
};

} // namespace qindb

#endif // QINDB_QINDB_STATUS_H
