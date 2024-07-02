#include <iostream>
#include <cassert>

#ifdef HAS_WIN32_VLD
#include "vld.h"
#endif

#include "test/base/tstbase.h"
#include "test/lsblk/tstlsblk.h"

int main() {
#ifdef _WIN32
   system("chcp 65001");
#endif

    TstBase::test();
    TstLsblk::test();

#ifdef _WIN32
#ifdef _MSC_VER
	system("pause");
#endif
#endif
    return 0;
}
