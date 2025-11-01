cmake_minimum_required(VERSION 3.8)

# set the project name
project(Sample_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# Preprocessor
add_executable(Geo_processor
        src/preprocess.cpp
        src/preprocess_main.cpp
)

# Sample_app
add_executable(Sample_app
        src/sample_app.cpp
        src/database.cpp)
