cmake_minimum_required(VERSION 3.10.0)
project(lab3_1 VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Настройка Git версии
find_package(Git)
if(GIT_FOUND)
    execute_process(
        COMMAND ${GIT_EXECUTABLE} rev-list --all --count
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
        OUTPUT_VARIABLE GIT_COMMIT_COUNT
        OUTPUT_STRIP_TRAILING_WHITESPACE
        ERROR_QUIET
    )
else()
    set(GIT_COMMIT_COUNT "0")
endif()

add_definitions(-DVERSION_PROJECT="${GIT_COMMIT_COUNT}")

# Исполняемый файл
add_executable(lab3 
    main.cpp
)

# Заголовочные файлы
target_include_directories(lab3 PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})

# Настройка CPack
set(CPACK_PACKAGE_NAME "lab3_1")
set(CPACK_PACKAGE_VERSION ${PROJECT_VERSION})
set(CPACK_PACKAGE_VENDOR "GitHub Actions")
set(CPACK_PACKAGE_DESCRIPTION "C++ Containers")

if(WIN32)
    set(CPACK_GENERATOR "ZIP")
    set(CPACK_PACKAGE_FILE_NAME "lab3_1-${PROJECT_VERSION}-windows")
endif()

if(UNIX AND NOT APPLE)
    set(CPACK_GENERATOR "DEB")
    set(CPACK_DEBIAN_PACKAGE_MAINTAINER "GitHub Actions <actions@github.com>")
    set(CPACK_DEBIAN_PACKAGE_NAME "lab3_1")
    set(CPACK_DEBIAN_PACKAGE_VERSION ${PROJECT_VERSION})
    set(CPACK_DEBIAN_FILE_NAME "lab31-${PROJECT_VERSION}-linux.deb")
endif()

install(TARGETS lab3 DESTINATION bin)

include(CPack)