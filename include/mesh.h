#pragma once
#include <vector>
#include "CDT/CDT.h"

namespace utils 
{
    struct Mesh
    {
        // Vector to store our mesh triangles in.
        std::vector<CDT::Triangle> triangles;
        // And also the set of triangle corners.
        std::vector<CDT::V2d<double>> points;
    };
};