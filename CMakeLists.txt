cmake_minimum_required(VERSION 3.25.0)

project(MoeQ)
set(TARGET_NAME MoeQ)

set(CMAKE_CXX_STANDARD 23)

if (MSVC)
    add_compile_options("$<$<C_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
    add_definitions(-D _WIN_PLATFORM_)
    add_definitions(-D UNICODE)

    link_libraries(ws2_32 wsock32)
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Linux")
    add_definitions(-D _LINUX_PLATFORM_)
    set(CMAKE_CXX_FLAGS "-fvisibility=hidden ${CMAKE_CXX_FLAGS}")
endif()

if ("${CMAKE_SYSTEM_NAME}" STREQUAL "Darwin")
    add_definitions(-D _LINUX_PLATFORM_)
    set(CMAKE_CXX_FLAGS "-fvisibility=hidden ${CMAKE_CXX_FLAGS}")
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_definitions(-D DEBUG)
endif()

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Release")
    add_definitions(-D RELEASE)
endif()

add_definitions(-D _AMD64_)

find_package(ZLIB REQUIRED)
find_package(OpenSSL REQUIRED)
# find_package(unofficial-sqlite3 CONFIG REQUIRED)
find_package(SQLite3 REQUIRED)

file(GLOB_RECURSE SOURCES "Main/*.cpp" "Protocol/*.cpp" "PluginSystem/*.cpp" "Utils/*.cpp")
add_executable(${TARGET_NAME} ${SOURCES} "MoeQ.cpp")

target_link_libraries(${TARGET_NAME} PRIVATE ZLIB::ZLIB)
# target_link_libraries(${TARGET_NAME} PRIVATE unofficial::sqlite3::sqlite3)
target_link_libraries(${TARGET_NAME} PRIVATE SQLite3)
target_link_libraries(${TARGET_NAME} PRIVATE OpenSSL::SSL OpenSSL::Crypto)

message(STATUS ${TARGET_NAME})