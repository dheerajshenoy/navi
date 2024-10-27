#pragma once

#include <QString>

template <typename T = bool>
class Result {

public:
    Result(const T &result, const QString &reason = "") : m_result(result), m_reason(reason) {}
    T result() noexcept { return m_result; }
    QString reason() noexcept { return m_reason; }

private : T m_result;
    QString m_reason;
};
