#pragma once

#ifndef A_STAR_H_INCLUDED
#define A_STAR_H_INCLUDED

#include <vector>
#include <stack>
#include <utility>
#include <set>
#include <cfloat>


namespace a_star {
    typedef std::pair<int, int> Pair;

    typedef std::pair<double, std::pair<int, int>> pPair;

    struct cell
    {
        int parent_i, parent_j;
        // f = g + h
        double f, g, h;
    };

    std::stack<Pair> aStarSearch(const std::vector<std::vector<int>>& grid, Pair src, Pair dest);
}

#endif // A_STAR_H_INCLUDED
