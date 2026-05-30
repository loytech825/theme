cmake_minimum_required(VERSION 3.20)

project(themer)

set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/**" )
add_executable(theme)

target_include_directories(theme PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_sources(theme PRIVATE ${SOURCES})

install(TARGETS theme RUNTIME DESTINATION "${CMAKE_INSTALL_PREFIX}/bin")