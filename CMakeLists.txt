cmake_minimum_required(VERSION 3.9)
project(async)


set(CMAKE_CXX_STANDARD 17)
set(PROCESS_SRC            "lib/processor/threadpool.h" 
                       "lib/processor/utils.h")

set(ASYNC_LIB_SRC "lib/async.h" "lib/async.cpp" ${PROCESS_SRC})

add_library(libasync_static STATIC ${ASYNC_LIB_SRC})
add_library(libasync SHARED ${ASYNC_LIB_SRC})

set_target_properties (
	libasync
	libasync_static
	PROPERTIES 
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON
	OUTPUT_NAME async
)



add_executable(async_cli "cli/main.cpp" )

set_target_properties (
	async_cli PROPERTIES 
	CXX_STANDARD 17
	CXX_STANDARD_REQUIRED ON	
)


target_include_directories(			
	async_cli
	PRIVATE
	${PROJECT_SOURCE_DIR}
	${PROJECT_SOURCE_DIR}/lib/processor
)

target_link_libraries(async_cli libasync_static)

