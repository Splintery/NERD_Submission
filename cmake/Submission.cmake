# ###########################################################################
# Non built projects may need to list explicitely their files
# ###########################################################################

file(GLOB_RECURSE NES_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/source/NesPatching/*")
foreach(file ${NES_FILES})
	list(APPEND SUBMISSION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach()
file(GLOB_RECURSE DEEP_FILES RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}/source/DeepPlanet/*")
foreach(file ${DEEP_FILES})
	list(APPEND SUBMISSION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
endforeach()

# ###########################################################################
# If the candidate application package includes a GIT repository
# and GIT is found on the host machine, we may want to include everything
# versioned too, including the repository itself to see the commit styles
# of the candidate
# ###########################################################################

find_package(Git QUIET)
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/.git/" AND Git_FOUND)
	file(GLOB_RECURSE GIT_REPO_FILES "${CMAKE_CURRENT_SOURCE_DIR}/.git/" "${CMAKE_CURRENT_SOURCE_DIR}/.git/*")
	foreach(file IN LISTS GIT_REPO_FILES)
		list(APPEND SUBMISSION_FILES "${file}")
	endforeach()
	execute_process(COMMAND git -C "${CMAKE_CURRENT_SOURCE_DIR}" ls-tree -r HEAD --name-only OUTPUT_VARIABLE VERSIONNED_FILES)
	string(STRIP "${VERSIONNED_FILES}" VERSIONNED_FILES)
	string(REPLACE "\n" ";" VERSIONNED_FILES "${VERSIONNED_FILES}")
	foreach(file IN LISTS VERSIONNED_FILES)
		list(APPEND SUBMISSION_FILES "${CMAKE_CURRENT_SOURCE_DIR}/${file}")
	endforeach()
endif()

list(REMOVE_DUPLICATES SUBMISSION_FILES)
file(WRITE "${CMAKE_CURRENT_SOURCE_DIR}/manifest.submission.txt")
foreach(file IN LISTS SUBMISSION_FILES)
	file(APPEND "${CMAKE_CURRENT_SOURCE_DIR}/manifest.submission.txt" "${file}\n")
endforeach()
if (NOT "${NERD_CANDIDATE}" STREQUAL "NONE")
	set(SUBMISSION_SUFFIX "-${NERD_CANDIDATE}")
endif()
add_custom_target(Submission
	"${CMAKE_COMMAND}" -E tar cf "${CMAKE_CURRENT_SOURCE_DIR}/Submission${SUBMISSION_SUFFIX}.zip" "--format=zip" "--files-from=${CMAKE_CURRENT_SOURCE_DIR}/manifest.submission.txt"
	WORKING_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}"
	COMMENT "Packaging files for submission"
)