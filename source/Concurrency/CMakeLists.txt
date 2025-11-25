#-------------------------------------------------------------------------------
# Project: Recruitment
# File: ConcurrencyExercise/CMakeLists.txt
#
# Copyright (C) 2025 Nintendo, All rights reserved.
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
	"${CMAKE_CURRENT_SOURCE_DIR}/all.h" 
	"context.h"
)

set_source_files_properties("${CMAKE_CURRENT_SOURCE_DIR}/all.h" PROPERTIES GENERATED TRUE)

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/test_main.cpp")
	list(APPEND SRC_FILES "test_main.cpp")
else()
	list(APPEND SRC_FILES "main.cpp")
	add_definitions("-DIS_CANDIDATE")
endif()

source_group("Source Files" FILES ${SRC_FILES})

build_src_files_list(${CMAKE_CURRENT_SOURCE_DIR} TRUE)

add_executable(ConcurrencyExercise
	${SRC_FILES}
)
target_include_directories(ConcurrencyExercise PRIVATE ".")

target_compile_features(ConcurrencyExercise PRIVATE cxx_std_20)
set_target_properties(ConcurrencyExercise PROPERTIES CXX_EXTENSIONS OFF)

target_link_libraries(ConcurrencyExercise PRIVATE Helpers)

set(SUBMISSION_FILES "${SUBMISSION_FILES}" PARENT_SCOPE)
