#pragma once

#include <string>
#include <QString>

class Result {

public:
    Result(const bool &ok, const QString &value = QString()) : ok(ok), value(value) {}
    std::string toStdStringValue() noexcept { return value.toStdString(); }
    bool ok;
    QString value;
};
