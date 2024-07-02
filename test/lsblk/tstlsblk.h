#ifndef TSTLSBLK_H
#define TSTLSBLK_H

#include "../../utils/json/cxxJson.h"

/**
 * @brief The TstBase class
 * 使用cxxJson解析lsblk -abOJ的输出结果
 */

class TstLsblk
{
public:
    static void test();
};

#endif // TSTLSBLK_H
