#pragma once

#include "base.h"
#include <chrono>

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

std::wstring crypt(std::wstring s);

struct timer
{
    using clock = std::chrono::steady_clock;
    clock::time_point start = clock::now();

    double  s() const { return std::chrono::duration<double>( clock::now() - start).count(); }
    double ms() const { return std::chrono::duration<double, std::milli>(clock::now() - start).count(); }
    double us() const { return std::chrono::duration<double, std::micro>(clock::now() - start).count(); }
    double ns() const { return std::chrono::duration<double, std::nano>(clock::now() - start).count(); }
};