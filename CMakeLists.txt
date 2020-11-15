CMAKE_MINIMUM_REQUIRED ( VERSION 3.2)
cmake_policy(SET CMP0091 NEW)
project(FakeSensor VERSION 0.1.4)

add_library (FakeSensor SHARED source/dccmtr.cpp source/dccmtrx.cpp source/dllmain.cpp source/pch.cpp source/Process.cpp)
set_property(TARGET FakeSensor PROPERTY MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

add_executable(sensortest source/sensortest.cpp)
target_link_libraries(sensortest FakeSensor)

add_executable(parametertest source/parametertest.cpp source/Process.cpp)

add_custom_command(
        TARGET FakeSensor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/autocal.ini
                ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)/autocal.ini)

add_custom_command(
        TARGET FakeSensor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/README.md
                ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)/README.md)

add_custom_command(
        TARGET FakeSensor POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy
                ${CMAKE_SOURCE_DIR}/LICENSE
                ${CMAKE_CURRENT_BINARY_DIR}/$(Configuration)/LICENSE)

add_custom_target(
        create_zip
        COMMAND ${CMAKE_COMMAND} -E tar "cfv" "../${CMAKE_PROJECT_NAME}-${CMAKE_VS_PLATFORM_NAME}-v${CMAKE_PROJECT_VERSION}.zip" --format=zip
                "FakeSensor.dll"
                "autocal.ini"
                "LICENSE"
                "README.md"
                "sensortest.exe"
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/Release)
