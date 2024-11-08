#pragma once

#include <QString>

struct StorageDevice {
    QString name;
    QString size;
    QString type;
    QString fstype;
    QString mountPoint;
    QString label;
};
