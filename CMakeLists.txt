cmake_minimum_required(VERSION 3.16)
project(plotjuggler_json_parser)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)

find_package(Qt5 REQUIRED COMPONENTS Widgets Xml)
find_package(nlohmann_json REQUIRED)
find_package(plotjuggler REQUIRED)

if(NOT DEFINED PJ_PLUGIN_INSTALL_DIRECTORY)
  set(PJ_PLUGIN_INSTALL_DIRECTORY lib/plotjuggler)
endif()

add_library(ParserROS2StringJson SHARED
  src/parser_ros2_string_json.cpp
  src/parser_ros2_string_json.h
)

target_include_directories(ParserROS2StringJson PRIVATE /opt/ros/humble/include)

target_link_directories(ParserROS2StringJson PRIVATE /opt/ros/humble/lib)

target_link_libraries(ParserROS2StringJson
  PRIVATE
    Qt5::Widgets
    Qt5::Xml
    plotjuggler_base
    nlohmann_json::nlohmann_json
)

target_compile_definitions(ParserROS2StringJson PRIVATE QT_PLUGIN)

install(TARGETS ParserROS2StringJson DESTINATION ${PJ_PLUGIN_INSTALL_DIRECTORY})
