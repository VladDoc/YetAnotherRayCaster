#include "randomBS.h"

thread_local std::mt19937 random_;
thread_local std::uniform_int_distribution<int> dist{0, 255};
