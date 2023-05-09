#pragma once

#include <string>


namespace constants {
/*
* Configuration contract: Required Keys
*/
    const static std::string KEY_ALG_TYPE = "ALG_TYPE";

/*
* Configuration contract: Optional Keys
*/
    const static std::string KEY_TASK_NUMBER = "TASK_NUM";
    const static std::string KEY_TASK_RELIABILITY = "TASK_R";
    const static std::string KEY_TASK_ACCURACY = "TASK_ACC";
    const static std::string KEY_TASK_EPSILON_RESERVED = "TASK_EPS";
    const static std::string KEY_SCAN_DENSITY = "SCAN_DEN";
    const static std::string KEY_SCAN_KEY = "SCAN_KEY";
    const static std::string KEY_IS_PRINT = "IS_PRINT";

/*
* Configuration contract: Defined Values
*/
    const static std::string DEFINE_INDEX_ALG = "IA";
    const static std::string DEFINE_MODIFIED_INDEX_ALG = "MIA";

/*
* Configuration contract: Default Values
*/
    const static double DEFAULT_TASK_RELIABILITY = 3;
    const static double DEFAULT_TASK_ACCURACY = 0.01;
    const static double DEFAULT_TASK_EPSILON_RESERVED = 0;
    const static int DEFAULT_SCAN_DENSITY = 10;
    const static int DEFAULT_SCAN_KEY = 1;
    const static bool DEFAULT_IS_PRINT = false;

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