#pragma once

#include <string>


namespace constants {
/*
* Configuration contract constants
*/
    const static int REQUIRED_ARGS_COUNT = 2;
    const static std::string INDEX_ALG_KEY = "IA";
    const static std::string MODIFIED_INDEX_ALG_KEY = "MIA";
    const static std::string ALL_TASK_KEY = "ALL";
    const static long double DEFAULT_RELIABILITY = 3;
    const static long double DEFAULT_ACCURACY = 0.01;
    const static long double DEFAULT_EPSILON_RESERVED = 0.001;
    const static int DEFAULT_MAPD_M = 10;
    const static int DEFAULT_MAPD_KEY = 1;

/*
* Data constants
*/
    const static std::string algorithmPointsPath = "../data/algorithm_points/";
    const static std::string functionPointsPath = "../data/function_points/";
    const static std::string invalidPointsPath = "../data/invalid_points/";
    const static std::string pointFileName = "points_";
    const static std::string pointFileExtension = ".txt";

/*
* Internal constants
*/
    const static int MIN_R = 1;
    const static int BIDIMENSIONAL_POINT_SIZE = 1;
    const static long double MIN_PEANO_POINT = 0.0;
    const static long double MAX_PEANO_POINT = 1.0;
}