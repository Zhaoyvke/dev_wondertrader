
#1. 确定CMake的最低版本需求
CMAKE_MINIMUM_REQUIRED(VERSION 3.0.0)

#2. 确定工程名
PROJECT(ParserInsight LANGUAGES CXX)
SET(CMAKE_CXX_STANDARD 11)

SET(SRC  
	${PROJECT_SOURCE_DIR}/ParserInsight.cpp
	${PROJECT_SOURCE_DIR}/ParserInsight.h
)

aux_source_directory(. DIR_LIB_SRCS)
aux_source_directory(protobuf DIR_LIB_SRCS)

add_definitions(-D_GLIBCXX_USE_CXX11_ABI=0)
add_definitions("-Wall -g")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
     message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

# 链接库目录
link_directories ("${PROJECT_SOURCE_DIR}/../API/insight_tcp_gateway_3.2.7/build")

# 添加包含目录
include_directories("${PROJECT_SOURCE_DIR}"
	"${PROJECT_SOURCE_DIR}/../API/insight_tcp_gateway_3.2.7/include"
	"${PROJECT_SOURCE_DIR}/../API/insight_tcp_gateway_3.2.7/include/mdc_gateway_client/include"
	"${PROJECT_SOURCE_DIR}/../API/insight_tcp_gateway_3.2.7/include/mdc_gateway_client/protobuf"
	"${PROJECT_SOURCE_DIR}/../API/insight_tcp_gateway_3.2.7/include/protobuf-3.1.0/include"
	"${PROJECT_SOURCE_DIR}/../API/insight_tcp_gateway_3.2.7/include/protobuf-3.1.0/include/google"
	)

SET(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/build_${PLATFORM}/${CMAKE_BUILD_TYPE}/bin)

INCLUDE_DIRECTORIES(${INCS})
LINK_DIRECTORIES(${LNKS})
ADD_LIBRARY(ParserInsight SHARED ${SRC})

#IF(MSVC)
#	SET(LIBS
#		ws2_32
#	)
#ELSE(GNUCC)
#	IF (WIN32)
#		SET(LIBS
#			boost_thread
#			ws2_32
#		)
#	ELSE(UNIX)
#		SET(LIBS
#			boost_thread
#			"-Wl,--whole-archive"
#			libmdc_gateway_client.a
#			libACE_SSL.a
#			libACE.a	
#			libssl.a
#			libcrypto.a
#			libprotobuf.a	
#			"-Wl,--no-whole-archive"
#		)
#	ENDIF()
#ENDIF()

SET(LIBS
	boost_thread
	ACE.so
	ACE_SSL.so
	protobuf.so	
	mdc_gateway_client.so
)
TARGET_LINK_LIBRARIES(ParserInsight ${LIBS})

IF (MSVC)
ELSE (GNUCC)
	SET_TARGET_PROPERTIES(ParserInsight PROPERTIES
		CXX_VISIBILITY_PRESET hidden
		C_VISIBILITY_PRESET hidden
		VISIBILITY_INLINES_HIDDEN 1
        LINK_FLAGS_RELEASE -s)
ENDIF ()

