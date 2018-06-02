cmake_minimum_required (VERSION 3.5)

project(ServerPortfolio C CXX)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++17")

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/bin)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/lib)

find_package(Boost REQUIRED COMPONENTS system)

file(GLOB_RECURSE CORE_CPP_FILES ${PROJECT_SOURCE_DIR}/Core/src/*.cpp)
add_library(Core ${CORE_CPP_FILES})
target_link_libraries(Core ${Boost_LIBRARIES})
target_include_directories(Core PRIVATE
  ${PROJECT_SOURCE_DIR}/Core/include
)

file(GLOB_RECURSE CORETEST_CPP_FILES ${PROJECT_SOURCE_DIR}/CoreTest/src/*.cpp)
add_executable(CoreTest ${CORETEST_CPP_FILES})
add_dependencies(CoreTest Core)
target_link_libraries(CoreTest Core)
target_include_directories(CoreTest PRIVATE
  ${PROJECT_SOURCE_DIR}/Core/include
  ${PROJECT_SOURCE_DIR}/CoreTest/include
)

file(GLOB_RECURSE TESTSERVER_CPP_FILES ${PROJECT_SOURCE_DIR}/TestServer/src/*.cpp)
add_executable(TestServer ${TESTSERVER_CPP_FILES})
add_dependencies(TestServer Core)
target_link_libraries(TestServer Core)
target_include_directories(TestServer PRIVATE
  ${PROJECT_SOURCE_DIR}/Core/include
  ${PROJECT_SOURCE_DIR}/TestServer/include
)

file(GLOB_RECURSE TESTCLIENT_CPP_FILES ${PROJECT_SOURCE_DIR}/TestClient/src/*.cpp)
add_executable(TestClient ${TESTCLIENT_CPP_FILES})
add_dependencies(TestClient Core)
target_link_libraries(TestClient Core)
target_include_directories(TestClient PRIVATE
  ${PROJECT_SOURCE_DIR}/Core/include
  ${PROJECT_SOURCE_DIR}/TestClient/include
)
