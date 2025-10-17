cmake_minimum_required(VERSION 3.8)

# set the project name
project(Sample_app)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

# add the executable
add_executable(Sample_app src/sample_app.cpp src/database.cpp)
