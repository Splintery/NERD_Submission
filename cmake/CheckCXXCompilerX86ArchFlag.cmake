include(CheckCXXCompilerFlag)

macro(check_cxx_compiler_x86_arch_flag flag enablevar supportvar)
	if (NOT DEFINED ${enablevar})
		if (CMAKE_SYSTEM_NAME MATCHES "Linux" AND NOT CMAKE_CROSSCOMPILING)
			# on linux hosts, try to enable SIMD extensions that are supported by the host CPU
			# this is assuming the GCC flag can map to the cpuinfo flag by replacing dots
			# with underscores, which seems to hold some truth
			if (NOT DEFINED CPUFLAGS)
				file(STRINGS "/proc/cpuinfo" CPUFLAGS REGEX "flags.*:")
				list(REMOVE_DUPLICATES CPUFLAGS)
				string(REGEX REPLACE "flags.*:" "" CPUFLAGS "${CPUFLAGS}")
				string(STRIP "${CPUFLAGS}" CPUFLAGS)
				string(REPLACE " " ";" CPUFLAGS "${CPUFLAGS}")
			endif()
			string(REPLACE "." "_" FLAGC "${flag}")
			list(FIND CPUFLAGS "${FLAGC}" index)
			unset(FLAGC)
			if ("${index}" GREATER_EQUAL 0)
				message(STATUS "host processor supports ${flag}")
				set(should_test_compiler TRUE)
			else()
				message(STATUS "host processor doesn't support ${flag}, disabling associated -m${flag} option")
				set(should_test_compiler FALSE)
			endif()
		else()
			# On non linux host assume that it's possible to enable and run code
			# generated with any flag
			set(should_test_compiler TRUE)
		endif()
	
		# Create an option to enable/disable use of specific arch flags
		option(${enablevar} "Enable compiler flag ${flag}" ${should_test_compiler})
	endif()
	
	if (${enablevar})
		check_cxx_compiler_flag("-m${flag}" "${supportvar}")
		if (${${supportvar}})
			add_compile_options("$<$<COMPILE_LANGUAGE:C,CXX>:-m${flag}>")
		endif()
	endif()
endmacro()
