cmake_minimum_required (VERSION 3.5)

project (sqltutor-ratings)

# set (CMAKE_CXX_STANDARD 11)
find_library(PQXX_LIB pqxx)

set (FILES sqltutor-ratings.cpp rating.cpp problem.cpp)

add_executable(${PROJECT_NAME} ${FILES})
target_link_libraries (${PROJECT_NAME} ${PQXX_LIB})
