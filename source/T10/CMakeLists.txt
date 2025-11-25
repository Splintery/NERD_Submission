#-------------------------------------------------------------------------------
# Project: Recruitment
# File: T10/CMakeLists.txt
#
# Copyright (C) 2018 Nintendo, All rights reserved.
#
# These coded instructions, statements, and computer programs contain proprietary
# information of Nintendo and/or its licensed developers and are protected by
# national and international copyright laws. They may not be disclosed to third
# parties or copied or duplicated in any form, in whole or in part, without the
# prior written consent of Nintendo.
#
# The content herein is highly confidential and should be handled accordingly.
#-------------------------------------------------------------------------------

include(../helpers.cmake)

set(SRC_FILES
	"main.cpp"
	"${CMAKE_CURRENT_SOURCE_DIR}/all.h"
)

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/all.h" PROPERTIES GENERATED TRUE)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/check.cpp")
	list(APPEND SRC_FILES "check.cpp" "check.h")
	add_definitions("-DHAVE_CHECK")
endif()

add_definitions(-D_SILENCE_TR2_SYS_NAMESPACE_DEPRECATION_WARNING)

source_group("Source Files" FILES ${SRC_FILES})

build_src_files_list(${CMAKE_CURRENT_SOURCE_DIR} TRUE)

file(GLOB_RECURSE DATA_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/Data/*")
source_group("Data" FILES ${DATA_FILES})

add_executable(T10
	${SRC_FILES}
)

target_link_libraries(T10 PRIVATE Helpers)
if (WIN32)
	# nothing for now
else()
	target_link_libraries(T10 PRIVATE stdc++fs)
endif()


set(SUBMISSION_FILES "${SUBMISSION_FILES}" PARENT_SCOPE)
