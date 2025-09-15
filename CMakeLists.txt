cmake_minimum_required(VERSION 3.10)
project(TopoAnalyzer)

# Включаем C++17
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем MPI
find_package(MPI REQUIRED)
include_directories(${MPI_INCLUDE_PATH})

# Подключаем OpenMP
find_package(OpenMP REQUIRED)

# Добавляем исходники
add_executable(TopoAnalyzer src/main.cpp)

# Линкуем библиотеки
target_link_libraries(TopoAnalyzer ${MPI_CXX_LIBRARIES} OpenMP::OpenMP_CXX)
