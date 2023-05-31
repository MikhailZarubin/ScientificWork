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
    const static std::string KEY_PRINT_LEVEL = "PRINT_LEVEL";
    const static std::string KEY_ITERATION_LIMIT = "ITER_LIMIT";
    const static std::string KEY_CUSTOM_TASK = "CUSTOM_TASK";

    /*
    * Configuration contract: Defined Values
    */
    const static std::string DEFINE_GLOBAL_SEARCH_ALG = "GSA";
    const static std::string DEFINE_INDEX_ALG = "IA";
    const static std::string DEFINE_MODIFIED_INDEX_ALG = "MIA";
    enum class PrintLevel {
        PRINT_NOT_ANYTHING = 0,
        PRINT_ONLY_TRIAL_POINT = 1,
        PRINT_ALL_POINTS = 2
    };

    /*
    * Configuration contract: Default Values
    */
    const static double DEFAULT_TASK_RELIABILITY = 3;
    const static double DEFAULT_TASK_ACCURACY = 0.01;
    const static double DEFAULT_TASK_EPSILON_RESERVED = 0;
    const static int DEFAULT_SCAN_DENSITY = 10;
    const static int DEFAULT_SCAN_KEY = 1;
    const static PrintLevel DEFAULT_PRINT_LEVEL = PrintLevel::PRINT_ONLY_TRIAL_POINT;
    const static int DEFAULT_ITERATION_LIMIT = 5000;

    /*
    * Data constants
    */
    const static std::string API_DIR = "../api/";
    const static std::string NAME_CONTRACT_FILE = "name_contract.txt";
    const static std::string DATA_PATHS_FILE = "data_paths.txt";
    const static std::string CONFIG_PATH_FILE = "config_path.txt";

    /*
    * Internal constants
    */
    const static int MIN_R = 1;
    const static int BIDIMENSIONAL_POINT_SIZE = 1;
    const static long double MIN_PEANO_POINT = 0.0;
    const static long double MAX_PEANO_POINT = 1.0;
    const static int DATA_PATHS_COUNT = 3;
    const static long double STEP_PRINT_POINTS = 0.001;
    const static int CUSTOM_TASK_NUMBER = -1;
}