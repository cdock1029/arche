cmake_minimum_required(VERSION 3.28)

project(Arche LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_definitions(QT_NO_CAST_FROM_ASCII)

find_package(Qt6 REQUIRED COMPONENTS Core HttpServer Sql)

include(FetchContent)

FetchContent_Declare(
  webxx
  GIT_REPOSITORY https://github.com/rthrfrd/webxx
  GIT_TAG 40b99f345ac8092ebabeb8a8faf0f8fc84ac3e3a)
FetchContent_MakeAvailable(webxx)

add_executable(
  Arche
  main.cpp
  includes/routes.h
  includes/routes.cpp
  includes/components.h
  includes/htmx.h
  includes/database.h
  includes/database.cpp
  includes/htmx.cpp
  README.md)

target_include_directories(Arche PRIVATE ${PROJECT_SOURCE_DIR}/includes)
target_link_libraries(Arche webxx Qt6::Core Qt6::HttpServer Qt6::Sql)

include(GNUInstallDirs)
install(
  TARGETS Arche
  LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
  RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
