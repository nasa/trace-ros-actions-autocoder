cmake_minimum_required(VERSION 2.8.3)
project(trace_ros_msgs_autocoder)

# TODO: List all packages with action server msgs here
set(PROJECT_MSGS_PKGS example_msgs)

# Specifies part of the autocoded C++ files' destination path
set(AUTOCODED_HEADER_DEST autocoded_proj_files)

find_package(catkin REQUIRED COMPONENTS actionlib ${PROJECT_MSGS_PKGS} trace_executive)

catkin_package(
   INCLUDE_DIRS include 
   LIBRARIES ${PROJECT_NAME}  
   CATKIN_DEPENDS actionlib ${PROJECT_MSGS_PKGS} trace_executive  
)

# CMAKE_BINARY_DIR/include is the destination for the new auto-generated
# header files that will be created by msg_gen.py
include_directories(include ${catkin_INCLUDE_DIRS} ${CMAKE_BINARY_DIR}/include)

foreach(MSG_PKG ${PROJECT_MSGS_PKGS})

    message("MSG_PKG: ${MSG_PKG}")
    message("MSG_PKG_DEVEL_PREFIX: ${${MSG_PKG}_DEVEL_PREFIX}")
    message("MSG_PKG_SOURCE_PREFIX: ${${MSG_PKG}_SOURCE_PREFIX}")

    list(APPEND PROJECT_MSGS_PKGS_DEVEL_PREFIXES
        ${${MSG_PKG}_DEVEL_PREFIX})

    list(APPEND PROJECT_MSGS_PKGS_SOURCE_PREFIXES
        ${${MSG_PKG}_SOURCE_PREFIX})

endforeach()

execute_process(
    COMMAND ${CMAKE_CURRENT_SOURCE_DIR}/msg_gen.py
    -a "${AUTOCODED_HEADER_DEST}"
    -n "${PROJECT_MSGS_PKGS}"
    -d "${PROJECT_MSGS_PKGS_DEVEL_PREFIXES}"
    -s "${PROJECT_MSGS_PKGS_SOURCE_PREFIXES}"
    -p /opt/ros/noetic
    -t ${CMAKE_CURRENT_SOURCE_DIR}/templates
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    OUTPUT_FILE ${CMAKE_BINARY_DIR}/msg_gen.log
)

file(GLOB ACTION_TRANSLATORS "${CMAKE_BINARY_DIR}/src/*.cc")
message("action_translators: ${ACTION_TRANSLATORS}")

# Based on trace_executive
add_library(${PROJECT_NAME} SHARED
    ${ACTION_TRANSLATORS}
    src/action_client.cc)

