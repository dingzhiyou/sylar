cmake_minimum_required(VERSION 2.8)
project(sylar)

set(CMAKE_VERBOSE_MAKEFILE on)
set(CMAKE_CXX_FLAGS "$ENV{CXXFLAGS} -rdynamic -O0 -ggdb -std=c++11 -Wall -Wno-deprecated -Werror -Wno-unused-function")

set(LIB_SRC 
	sylar/application.cc
	sylar/util.cc
	sylar/config.cc
	http/resourceServlet.cc
	sylar/log.cc
	http/http_session.cc
	http/http_connection.cc
	http/http_server.cc
	sylar/daemon.cc
	sylar/uri.rl.cc
	sylar/env.cc
	http/servlet.cc
	sylar/env.cc
	sylar/stream.cc
	sylar/sockstream.cc
	http/http.cc
	sylar/hook.cc
	sylar/tcp_server.cc
	http/http_parser.cc
	http/http11_parser.rl.cc
	http/httpclient_parser.rl.cc
	sylar/socket.cc
	sylar/bytearray.cc
	sylar/ipaddr.cc
	sylar/thread.cc
	sylar/timer.cc
	sylar/fiber.cc
	sylar/scheduler.cc
	sylar/iomanager.cc
	sylar/fd_manager.cc
	)

add_library(sylar SHARED ${LIB_SRC})
link_directories(/home/dada/workspace/yaml-cpp/build)
include_directories(.)
include_directories(/home/dada/workspace/yaml-cpp/include)
set(LIB_LIB
	sylar
	pthread
	yaml-cpp
	dl
	)

add_executable(test_application  tests/test_application.cc)
add_dependencies(test_application sylar)
target_link_libraries(test_application ${LIB_LIB})


add_executable(test_env  tests/test_env.cc)
add_dependencies(test_env sylar)
target_link_libraries(test_env ${LIB_LIB})

add_executable(test_daemo  tests/test_daemo.cc)
add_dependencies(test_daemo sylar)
target_link_libraries(test_daemo ${LIB_LIB})

add_executable(test_uri  tests/test_uri.cc)
add_dependencies(test_uri sylar)
target_link_libraries(test_uri ${LIB_LIB})


add_executable(test_http_connection  tests/test_http_connection.cc)
add_dependencies(test_http_connection sylar)
target_link_libraries(test_http_connection  ${LIB_LIB})

add_executable(test_http_server  tests/test_http_server.cc)
add_dependencies(test_http_server sylar)
target_link_libraries(test_http_server  ${LIB_LIB})

add_executable(echoServer  example/echoServer.cc)
add_dependencies(echoServer sylar)
target_link_libraries(echoServer  ${LIB_LIB})


add_executable(test_tcp_server  tests/test_tcp_server.cc)
add_dependencies(test_tcp_server sylar)
target_link_libraries(test_tcp_server ${LIB_LIB})

add_executable(test_http_parser  tests/test_http_parser.cc)
add_dependencies(test_http_parser sylar)
target_link_libraries(test_http_parser ${LIB_LIB})


add_executable(test_http  tests/test_http.cc)
add_dependencies(test_http sylar)
target_link_libraries(test_http ${LIB_LIB})


add_executable(test_bytearray  tests/test_bytearray.cc)
add_dependencies(test_bytearray sylar)
target_link_libraries(test_bytearray ${LIB_LIB})

add_executable(test_iomanager  tests/test_iomanager.cc)
add_dependencies(test_iomanager sylar)
target_link_libraries(test_iomanager ${LIB_LIB})

add_executable(test_addr tests/test_addr.cc)
add_dependencies(test_addr sylar)
target_link_libraries(test_addr ${LIB_LIB})


add_executable(test_my tests/test_my.cc)
add_dependencies(test_my sylar)
target_link_libraries(test_my ${LIB_LIB})


add_executable(test_sock tests/test_socket.cc)
add_dependencies(test_sock sylar)
target_link_libraries(test_sock ${LIB_LIB})

add_executable(test_hook  tests/test_hook.cc)
add_dependencies(test_hook sylar)
target_link_libraries(test_hook ${LIB_LIB})

add_executable(test_mutex  tests/test_mutex.cc)
add_dependencies(test_mutex sylar)
target_link_libraries(test_mutex ${LIB_LIB})


add_executable(test_scheduler tests/test_scheduler.cc)
add_dependencies(test_scheduler sylar)
target_link_libraries(test_scheduler ${LIB_LIB})

add_executable(test tests/test.cc)
add_dependencies(test sylar )
target_link_libraries(test ${LIB_LIB})

add_executable(test_config tests/test_config.cc)
add_dependencies(test_config sylar)
target_link_libraries(test_config ${LIB_LIB})

add_executable(test_fiber tests/test_fiber.cc)
add_dependencies(test_fiber sylar)
target_link_libraries(test_fiber ${LIB_LIB})

add_executable(testassert tests/testassert.cc)
add_dependencies(testassert sylar )
target_link_libraries(testassert ${LIB_LIB})

add_executable(test_thread tests/test_thread.cc)
add_dependencies(test_thread sylar )
target_link_libraries(test_thread ${LIB_LIB})

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/bin)
SET(LIBRARY_OUTPUT_PATH  ${PROJECT_SOUCE_DIR}/bin)





