#include "log.hpp"
#include <chrono>
#include <iomanip>
#include <iostream>

namespace vnsee::log
{

template<typename String>
std::ostream& print(const String& kind)
{
    // Prefix log messages with a simple header. Keep it minimal and stream-friendly.
    std::cout << '[' << kind << "] ";
    return std::cout;
}

} // namespace vnsee::log
