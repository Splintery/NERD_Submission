set(SRC_FILES
	Helpers.cpp
	Helpers.h
	stb_image.cpp
	stb_image.h
	stb_image_write.cpp
	stb_image_write.h
)
source_group("Source Files" FILES ${SRC_FILES})

if (MSVC)
    set_source_files_properties(stb_image PROPERTIES COMPILE_FLAGS "/wd4100")
endif()

add_library(Helpers STATIC ${SRC_FILES})

target_include_directories(Helpers INTERFACE ".")

find_package(OpenMP)
if (OpenMP_CXX_FOUND)
    target_link_libraries(Helpers PUBLIC OpenMP::OpenMP_CXX)
endif()

# Install .natvis
if (CMAKE_GENERATOR MATCHES "Visual Studio [0-9]* ([0-9]*)")
	set(VS_YEAR "${CMAKE_MATCH_1}")
	get_filename_component(USER_DOC_FOLDER "[HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders;Personal]" ABSOLUTE)
	set(VIZ_FOLDER "${USER_DOC_FOLDER}/Visual Studio ${VS_YEAR}/Visualizers")
	if (NOT EXISTS "${VIZ_FOLDER}")
		file(MAKE_DIRECTORY "${VIZ_FOLDER}")
	endif()
	file(COPY "${CMAKE_CURRENT_SOURCE_DIR}/nerd_recruitment.natvis" DESTINATION "${VIZ_FOLDER}")
endif()
