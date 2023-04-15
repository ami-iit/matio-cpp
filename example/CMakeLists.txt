cmake_minimum_required(VERSION 3.5)
project(testMatioCpp CXX)

find_package(matioCpp REQUIRED)
find_package(Eigen3 REQUIRED)

add_executable(matiocpp_example main.cpp)
target_link_libraries(matiocpp_example PRIVATE matioCpp::matioCpp)

install(TARGETS matiocpp_example DESTINATION bin)
