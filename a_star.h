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

    bool isValid(int row, int col, int rows, int cols)
    {
        return (row >= 0) && (row < rows) &&
               (col >= 0) && (col < cols);
    }

    bool isUnBlocked(std::vector<std::vector<int>> grid, int row, int col)
    {
        if (grid[row][col] == 1)
            return (true);
        else
            return (false);
    }

    bool isDestination(int row, int col, Pair dest)
    {
        if (row == dest.first && col == dest.second)
            return (true);
        else
            return (false);
    }

    double calculateHValue(int row, int col, Pair dest)
    {
        return ((double)sqrt ((row-dest.first)*(row-dest.first)
                            + (col-dest.second)*(col-dest.second)));
    }

    std::stack<Pair> tracePath(std::vector<std::vector<cell>> cellDetails, Pair dest)
    {
        printf ("\nThe Path is ");
        int row = dest.first;
        int col = dest.second;

        std::stack<Pair> Path;

        while (!(cellDetails[row][col].parent_i == row &&
                 cellDetails[row][col].parent_j == col ))
        {
            Path.push (std::make_pair (row, col));
            int temp_row = cellDetails[row][col].parent_i;
            int temp_col = cellDetails[row][col].parent_j;
            row = temp_row;
            col = temp_col;
        }

        Path.push (std::make_pair (row, col));

        return Path;
    }


    std::stack<Pair> aStarSearch(std::vector<std::vector<int>> grid, Pair src, Pair dest)
    {
        const size_t rows = grid.size();
        const size_t cols = grid[0].size();
        // If the source is out of range
        if (isValid (src.first, src.second, rows, cols) == false)
        {
            printf ("Source is invalid\n");
            return std::stack<Pair>();
        }

        // If the destination is out of range
        if (isValid (dest.first, dest.second, rows, cols) == false)
        {
            printf ("Destination is invalid\n");
            return std::stack<Pair>();
        }

        // Either the source or the destination is blocked
        if (isUnBlocked(grid, src.first, src.second) == false ||
            isUnBlocked(grid, dest.first, dest.second) == false)
        {
            printf ("Source or the destination is blocked\n");
            return std::stack<Pair>();
        }

        // If the destination cell is the same as source cell
        if (isDestination(src.first, src.second, dest) == true)
        {
            printf ("We are already at the destination\n");
            return std::stack<Pair>();
        }

        std::vector<std::vector<bool>> closedList{rows, std::vector<bool>(cols, false)};
        std::vector<std::vector<cell>> cellDetails{rows, std::vector<cell>{cols, {0,0,0,0,0}}};

        size_t i, j;

        for (i=0; i<rows; i++)
        {
            for (j=0; j<cols; j++)
            {
                cellDetails[i][j].f = FLT_MAX;
                cellDetails[i][j].g = FLT_MAX;
                cellDetails[i][j].h = FLT_MAX;
                cellDetails[i][j].parent_i = -1;
                cellDetails[i][j].parent_j = -1;
            }
        }

        // Initialising the parameters of the starting node
        i = src.first, j = src.second;
        cellDetails[i][j].f = 0.0;
        cellDetails[i][j].g = 0.0;
        cellDetails[i][j].h = 0.0;
        cellDetails[i][j].parent_i = i;
        cellDetails[i][j].parent_j = j;

        std::set<pPair> openList;


        openList.insert(std::make_pair (0.0, std::make_pair (i, j)));

        bool foundDest = false;

        while (!openList.empty())
        {
            pPair p = *openList.begin();

            openList.erase(openList.begin());

            i = p.second.first;
            j = p.second.second;
            closedList[i][j] = true;

            double gNew, hNew, fNew;


            if (isValid(i-1, j, rows, cols))
            {
                if (isDestination(i-1, j, dest))
                {
                    cellDetails[i-1][j].parent_i = i;
                    cellDetails[i-1][j].parent_j = j;
                    printf ("The destination cell is found\n");
                    foundDest = true;
                    return tracePath (cellDetails, dest);
                }
                else if (closedList[i-1][j] == false &&
                         isUnBlocked(grid, i-1, j) == true)
                {
                    gNew = cellDetails[i][j].g + 1.0;
                    hNew = calculateHValue (i-1, j, dest);
                    fNew = gNew + hNew;
                    if (cellDetails[i-1][j].f == FLT_MAX ||
                        cellDetails[i-1][j].f > fNew)
                    {
                        openList.insert( std::make_pair(fNew,
                                         std::make_pair(i-1, j)));

                        cellDetails[i-1][j].f = fNew;
                        cellDetails[i-1][j].g = gNew;
                        cellDetails[i-1][j].h = hNew;
                        cellDetails[i-1][j].parent_i = i;
                        cellDetails[i-1][j].parent_j = j;
                    }
                }
            }

            // SOUTH
            if (isValid(i+1, j, rows, cols) == true)
            {
                if (isDestination(i+1, j, dest) == true)
                {
                    cellDetails[i+1][j].parent_i = i;
                    cellDetails[i+1][j].parent_j = j;
                    printf("The destination cell is found\n");
                    foundDest = true;
                    return tracePath (cellDetails, dest);;
                }
                else if (closedList[i+1][j] == false &&
                         isUnBlocked(grid, i+1, j) == true)
                {
                    gNew = cellDetails[i][j].g + 1.0;
                    hNew = calculateHValue(i+1, j, dest);
                    fNew = gNew + hNew;

                    if (cellDetails[i+1][j].f == FLT_MAX ||
                        cellDetails[i+1][j].f > fNew)
                    {
                        openList.insert( std::make_pair(fNew,
                                         std::make_pair(i+1, j)));
                        cellDetails[i+1][j].f = fNew;
                        cellDetails[i+1][j].g = gNew;
                        cellDetails[i+1][j].h = hNew;
                        cellDetails[i+1][j].parent_i = i;
                        cellDetails[i+1][j].parent_j = j;
                    }
                }
            }

            // EAST
            if (isValid(i, j+1, rows, cols))
            {
                if (isDestination(i, j+1, dest))
                {
                    cellDetails[i][j+1].parent_i = i;
                    cellDetails[i][j+1].parent_j = j;
                    printf ("The destination cell is found\n");
                    foundDest = true;
                    return tracePath (cellDetails, dest);
                }
                else if (closedList[i][j+1] == false &&
                         isUnBlocked(grid, i, j+1) == true)
                {
                    gNew = cellDetails[i][j].g + 1.0;
                    hNew = calculateHValue (i, j+1, dest);
                    fNew = gNew + hNew;
                    if (cellDetails[i][j+1].f == FLT_MAX ||
                            cellDetails[i][j+1].f > fNew)
                    {
                        openList.insert( std::make_pair(fNew,
                                         std::make_pair(i, j+1)));

                        cellDetails[i][j+1].f = fNew;
                        cellDetails[i][j+1].g = gNew;
                        cellDetails[i][j+1].h = hNew;
                        cellDetails[i][j+1].parent_i = i;
                        cellDetails[i][j+1].parent_j = j;
                    }
                }
            }

            // WEST
            if (isValid(i, j-1, rows, cols))
            {
                if (isDestination(i, j-1, dest))
                {
                    cellDetails[i][j-1].parent_i = i;
                    cellDetails[i][j-1].parent_j = j;
                    printf ("The destination cell is found\n");
                    foundDest = true;
                    return tracePath (cellDetails, dest);
                }
                else if (closedList[i][j-1] == false &&
                         isUnBlocked(grid, i, j-1) == true)
                {
                    gNew = cellDetails[i][j].g + 1.0;
                    hNew = calculateHValue (i, j-1, dest);
                    fNew = gNew + hNew;
                    if (cellDetails[i][j-1].f == FLT_MAX ||
                        cellDetails[i][j-1].f > fNew)
                    {
                        openList.insert( std::make_pair(fNew,
                                         std::make_pair(i, j-1)));

                        cellDetails[i][j-1].f = fNew;
                        cellDetails[i][j-1].g = gNew;
                        cellDetails[i][j-1].h = hNew;
                        cellDetails[i][j-1].parent_i = i;
                        cellDetails[i][j-1].parent_j = j;
                    }
                }
            }
        }

        if (foundDest == false)
            printf("Failed to find the Destination Cell\n");

        return std::stack<Pair>();
    }
}

#endif // A_STAR_H_INCLUDED
