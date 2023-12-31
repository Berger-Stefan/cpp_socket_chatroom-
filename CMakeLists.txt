#  cmake -DCMAKE_TOOLCHAIN_FILE=/opt/vcpkg/scripts/buildsystems/vcpkg.cmake ..
cmake_minimum_required(VERSION 3.27)

project(
    CPP_WEBSOCKET_CHATROOM
	VERSION 0.1
	LANGUAGES CXX)

# If you set any CMAKE_ variables, that can go here.
set(CMAKE_CXX_COMPILER "/usr/bin/clang++" CACHE STRING "C++ compiler" FORCE)
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_Flags "-O3")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

find_package(mongocxx CONFIG REQUIRED)
find_package(bsoncxx CONFIG REQUIRED)

set(SERVER_FILES 
  "${PROJECT_SOURCE_DIR}/src/server.cpp" 
  "${PROJECT_SOURCE_DIR}/include/ServerClass.hpp" 
  "${PROJECT_SOURCE_DIR}/src/ServerClass.cpp" 
  )

set(CLIENT_FILES 
  "${PROJECT_SOURCE_DIR}/src/client.cpp" 
  "${PROJECT_SOURCE_DIR}/include/ClientClass.hpp" 
  "${PROJECT_SOURCE_DIR}/src/ClientClass.cpp" 
  )

add_executable(server ${SERVER_FILES})
add_executable(client ${CLIENT_FILES})

target_link_libraries(server PRIVATE $<IF:$<TARGET_EXISTS:mongo::bsoncxx_static>,mongo::bsoncxx_static,mongo::bsoncxx_shared>)
target_link_libraries(server PRIVATE $<IF:$<TARGET_EXISTS:mongo::mongocxx_static>,mongo::mongocxx_static,mongo::mongocxx_shared>)

target_include_directories(server PRIVATE ${PROJECT_SOURCE_DIR}/include)
target_include_directories(client PRIVATE ${PROJECT_SOURCE_DIR}/include)
