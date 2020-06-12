#ifndef RANDOMBS_H_INCLUDED
#define RANDOMBS_H_INCLUDED

#include <random>

extern thread_local std::mt19937 random_;
extern thread_local std::uniform_int_distribution<int> dist;

#endif // RANDOMBS_H_INCLUDED
