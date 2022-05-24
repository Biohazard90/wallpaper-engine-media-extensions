cmake_minimum_required (VERSION 3.4)

#set(CMAKE_HOST_SYSTEM "Windows-10.0.18362")
set(CMAKE_HOST_SYSTEM "Windows-10.0.22000")

#set(CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION "10.0.17763" CACHE STRING INTERNAL FORCE)
#set(CMAKE_HOST_SYSTEM "Windows-10.0.17763" CACHE STRING INTERNAL FORCE)
#set(CMAKE_SYSTEM_VERSION "10.0.17763" CACHE STRING INTERNAL FORCE)

project (WEMediaExtensions)
math(EXPR BITS "8*${CMAKE_SIZEOF_VOID_P}")

if(CMAKE_CONFIGURATION_TYPES)      
	set(CMAKE_CONFIGURATION_TYPES Debug Release)
endif()
project ("WEMediaExtensions${BITS}")

#message(STATUS "Windows SDK target ${CMAKE_VS_WINDOWS_TARGET_PLATFORM_VERSION}")
#message(STATUS "Windows SDK target ${CMAKE_HOST_SYSTEM}")
#message(STATUS "Windows SDK target ${CMAKE_SYSTEM_VERSION}")
# Configure global paths

# Options
# SFML SHOULD LINK OPENAL STATICALLY
# USE OPENAL-SOFT AND MAKE SURE THAT WORKS OUT
set(OP_SFML2_INC "D:/dev/welibs/sfml_2.5.1/src/include" CACHE STRING "Path to SFML 2 include files")
set(OP_BINARY_OUTPUT_DIRECTORY "D:/dev/we/windows/bin/bin" CACHE STRING "Output directory (wallpaper_engine/bin)")

if ("${BITS}" EQUAL "32")
set(OP_SFML2_LIB "D:/dev/welibs/sfml_2.5.1/x86" CACHE STRING "Path to SFML 2 static libraries")
set(OP_SFML2_EXTERNAL_LIB "D:/dev/welibs/sfml_2.5.1/x86_external" CACHE STRING "Path to SFML 2 external libraries")
else ("${BITS}" EQUAL "32")
set(OP_SFML2_LIB "D:/dev/welibs/sfml_2.5.1/x64" CACHE STRING "Path to SFML 2 static libraries")
set(OP_SFML2_EXTERNAL_LIB "D:/dev/welibs/sfml_2.5.1/x64_external" CACHE STRING "Path to SFML 2 external libraries")
endif()

# Derived options, do not edit

# Precompiled headers
MACRO(ADD_MSVC_PRECOMPILED_HEADER PrecompiledHeader PrecompiledSource SourcesVar)
  IF(MSVC)
    GET_FILENAME_COMPONENT(PrecompiledBasename ${PrecompiledHeader} NAME_WE)
    SET(PrecompiledBinary "${CMAKE_CURRENT_BINARY_DIR}/${PrecompiledBasename}.pch")
    #SET(Sources ${${SourcesVar}})

	IF(${SourcesVar})
		#SET(Sources ${SourcesVar})
		#SET(Sources "a" "b")
		LIST(APPEND Sources ${${SourcesVar}})
		#MESSAGE( STATUS ${Sources} )
		LIST(FILTER Sources INCLUDE REGEX cpp$)
		#MESSAGE( STATUS ${${Sources}} )
		SET(Sources ${Sources})
		SET_SOURCE_FILES_PROPERTIES(${Sources}
                                PROPERTIES COMPILE_FLAGS "/Yu\"${PrecompiledHeader}\" /FI\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_DEPENDS "${PrecompiledBinary}")
		#MESSAGE( STATUS ${Sources} )
	ENDIF()

    SET_SOURCE_FILES_PROPERTIES(${PrecompiledSource}
                                PROPERTIES COMPILE_FLAGS "/Yc\"${PrecompiledHeader}\" /Fp\"${PrecompiledBinary}\""
                                           OBJECT_OUTPUTS "${PrecompiledBinary}")

    # Add precompiled header to SourcesVar
    LIST(APPEND ${SourcesVar} ${PrecompiledSource})
  ENDIF()
ENDMACRO()

# Visual Studio filters
FUNCTION(ADD_SOURCE_DIRS SourcesVar)
	#foreach(source IN LISTS ${SourcesVar})
	#	MESSAGE( STATUS ${source} )
	#endforeach()
	foreach(source IN LISTS ${SourcesVar})
		get_filename_component(source_path "${source}" PATH)
		
		if (source_path STREQUAL "")
		else (source_path STREQUAL "")
		string(FIND ${source_path} "include" foundInclude REVERSE)
		string(FIND ${source_path} "src" foundSrc REVERSE)
		string(FIND ${source_path} "thirdparty" foundThirdparty)
		if (foundInclude GREATER 0)
			string(SUBSTRING ${source_path} ${foundInclude} -1 source_path)
		endif ()
		if (foundSrc GREATER 0)
			string(SUBSTRING ${source_path} ${foundSrc} -1 source_path)
		endif ()
		if (foundThirdparty GREATER 0)
			string(SUBSTRING ${source_path} ${foundThirdparty} -1 source_path)
		endif ()
		
		string(REPLACE "/" "\\" source_path_msvc "${source_path}")
		string(REPLACE "include" "Header Files" source_path_msvc "${source_path_msvc}")
		string(REPLACE "src" "Source Files" source_path_msvc "${source_path_msvc}")
		source_group("${source_path_msvc}" FILES "${source}")
		endif (source_path STREQUAL "")
	endforeach()
ENDFUNCTION()

# Copy output directory
MACRO(ADD_TARGET_OUTPUT_DIR projoutputname subdirectory)
	add_custom_command(TARGET ${projoutputname}
		POST_BUILD
		COMMAND xcopy /Y \"$(OutputPath)$(TargetFileName)\" \"${OP_BINARY_OUTPUT_DIRECTORY}${subdirectory}\"\n
		if exist \"$(OutputPath)$(TargetName).pdb\" xcopy /Y \"$(OutputPath)$(TargetName).pdb\" \"${OP_BINARY_OUTPUT_DIRECTORY}${subdirectory}\"
	)
ENDMACRO()
MACRO(ADD_TARGET_OUTPUT projoutputname)
	ADD_TARGET_OUTPUT_DIR(${projoutputname} "")
ENDMACRO()

add_definitions(-DPLATFORM_WINDOWS)
add_definitions(-D_HAS_EXCEPTIONS=0 -D_STATIC_CPPLIB)
add_definitions(-DUNICODE -D_UNICODE)
add_definitions(-DBOOST_ALL_NO_LIB -DBOOST_NO_EXCEPTION)
add_definitions(-DPSAPI_VERSION=1) # https://msdn.microsoft.com/en-us/library/windows/desktop/ms682623(v=vs.85).aspx

if ("${BITS}" EQUAL "64")
	add_definitions(-DWIN64)
	add_definitions(-DPLATFORM_X64)
endif ("${BITS}" EQUAL "64")

set(common_build_flags "/MP32 /arch:SSE2 /std:c++17 /std:c17")
set(common_linker_flags "/LTCG /pdbaltpath:%_PDB%")

if(EXISTS "D:/dev/we/buildtools/msdosstub.exe")
set(common_linker_flags "${common_linker_flags} /STUB:D:\\dev\\we\\buildtools\\msdosstub.exe")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "${common_build_flags} ") # /fp:fast
set(CMAKE_CXX_FLAGS_RELEASE "/MP32 /MT /GF /GR- /Gy /Oi /O2 /Os /Zi /GL /GS- ${common_build_flags} ") # /fp:fast
#set(CMAKE_CXX_FLAGS_MINSIZEREL "/MD /GF /GR- /Gy /Oi /O2 /Zi")
set(CMAKE_CXX_FLAGS_DEBUG "/MP32 /MTd /GR- /Od /Zi ${common_build_flags} ")
set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} CACHE STRING "Flags used by the compiler during all build types (Forced version)." FORCE)
set(CMAKE_C_FLAGS_DEBUG "${common_build_flags} ") # /fp:fast
set(CMAKE_C_FLAGS_RELEASE "/MT /GF /GR- /Gy /Oi /O2 /Os /Zi /GL /GS- ${common_build_flags} ") # /fp:fast
#set(CMAKE_CXX_FLAGS_MINSIZEREL "/MD /GF /GR- /Gy /Oi /O2 /Zi")
set(CMAKE_C_FLAGS_DEBUG "/MTd /GR- /Od /Zi ${common_build_flags} ")

# set(CMAKE_CXX_STANDARD_LIBRARIES "${CMAKE_CXX_STANDARD_LIBRARIES} libcmt.lib")

# 4996 deprecated API
# 4530 exception handler used, but unwind semantics are not enabled
# 4577 'noexcept' used with no exception handling mode specified; termination on exception is not guaranteed
# 4091 'keyword' : ignored on left of 'type' when no variable is declared
# 4351 new behavior: elements of array '' will be default initialized
# 4838 conversion from 'const uint32' to 'const float' requires a narrowing conversion
set(CMAKE_CXX_FLAGS "/wd4996 /wd4530 /wd4577 /wd4091 /wd4351 /wd4838")
set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /DEBUG ${common_linker_flags}")
set(CMAKE_STATIC_LINKER_FLAGS_RELEASE "${CMAKE_STATIC_LINKER_FLAGS_RELEASE} ")
set(CMAKE_SHARED_LINKER_FLAGS_RELEASE "${CMAKE_SHARED_LINKER_FLAGS_RELEASE} /INCREMENTAL:NO /DEBUG ${common_linker_flags}")
#set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} /DEBUG")
#set(CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL "${CMAKE_STATIC_LINKER_FLAGS_MINSIZEREL}")
#set(CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL "${CMAKE_SHARED_LINKER_FLAGS_MINSIZEREL} /DEBUG")
string(REPLACE "/EHsc" "" CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS}")

# static libraries
add_subdirectory(mediaextensions)

