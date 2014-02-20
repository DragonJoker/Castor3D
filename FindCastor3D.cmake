# Locate Castor3D library
# This module defines :
#  	Castor3D_LIBRARIES :	The library to link against
#  	Castor3D_FOUND :		If false, do not try to link to Castor3D
#  	Castor3D_INCLUDE_DIR :	Where to find headers.
#	Castor3D_LIBRARY_DIR :	Where to find the library
# You just have to set Castor3D_ROOT_DIR in your CMakeCache.txt if it is not found.
# If Castor3D was built in a special folder, you can specify it in Castor3D_LIBRARY_DIR

function( Castor_COMPILER_DUMPVERSION OUTPUT_VERSION)
	exec_program(${CMAKE_CXX_COMPILER} ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion OUTPUT_VARIABLE Castor_COMPILER_VERSION)
	string(REGEX REPLACE "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2" Castor_COMPILER_VERSION ${Castor_COMPILER_VERSION})
	set(${OUTPUT_VERSION} ${Castor_COMPILER_VERSION} PARENT_SCOPE)
endfunction()

FIND_PATH(Castor3D_ROOT_DIR include/Castor3D/Prerequisites.hpp
  HINTS
  PATHS
  C: D: E: F: G: H: I: J: K: L: M: N: O: P: Q: R: S: T: U: V: W: X: Y: Z:
  /usr/local/include
  /usr/include
)

if (Castor3D_ROOT_DIR)
	FIND_PATH(Castor3D_INCLUDE_DIR Castor3D/Prerequisites.hpp
	  HINTS
	  PATH_SUFFIXES include
	  PATHS
	  ${Castor3D_ROOT_DIR}
	)

	if (NOT Castor3D_LIBRARY_DIR)
		if (CMAKE_BUILD_TYPE)
			set( Castor3D_LIBRARY_DIR "${Castor3D_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE}")
		else ()
			set( Castor3D_LIBRARY_DIR "${Castor3D_ROOT_DIR}/lib")
		endif ()
	endif ()

	set( Castor3D_LIBRARY_DIR ${Castor3D_LIBRARY_DIR} CACHE FILEPATH "Castor3D library directory")

	if (WIN32)
		if (MSVC)
			set( Castor3D_LIBRARY_NAME "Castor3D")
		elseif (BORLAND)
			set( Castor3D_LIBRARY_NAME "Castor3D")
		elseif (CYGWIN)
			set( Castor3D_LIBRARY_NAME "libCastor3D")
		else (CYGWIN)
			set( Castor3D_LIBRARY_NAME "libCastor3D")
		endif (MSVC)
	elseif (UNIX)
		set( Castor3D_LIBRARY_NAME "libCastor3D")
	elseif (APPLE)
		set( Castor3D_LIBRARY_NAME "libCastor3D")
	endif (WIN32)
	
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" OR "${CMAKE_CXX_COMPILER}" MATCHES "icl" OR "${CMAKE_CXX_COMPILER}" MATCHES "icpc")
		if(WIN32)
			set( Castor3D_ABI_Name "-iw")
		else()
			set( Castor3D_ABI_Name "-il")
		endif()
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (MSVC90)
		set( Castor3D_ABI_Name "-vc90")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (MSVC10)
		set( Castor3D_ABI_Name "-vc100")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (MSVC80)
		set( Castor3D_ABI_Name "-vc80")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (MSVC71)
		set( Castor3D_ABI_Name "-vc71")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (MSVC70)
		set( Castor3D_ABI_Name "-vc7")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (MSVC60)
		set( Castor3D_ABI_Name "-vc6")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (BORLAND)
		set( Castor3D_ABI_Name "-bcb")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "SunPro")
		set( Castor3D_ABI_Name "-sw")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (MINGW)
		Castor_COMPILER_DUMPVERSION(Castor_COMPILER_VERSION)
		set( Castor3D_ABI_Name "-mgw${Castor_COMPILER_VERSION}")
		set( Castor3D_ABI_Name_Debug "-d")
	elseif (UNIX)
		if (CMAKE_COMPILER_IS_GNUCXX)
			Castor_COMPILER_DUMPVERSION(Castor_COMPILER_VERSION)
			if(APPLE)
				set( Castor3D_ABI_Name "-xgcc${Castor_COMPILER_VERSION}")
				set( Castor3D_ABI_Name_Debug "-d")
			else()
				set( Castor3D_ABI_Name "-gcc${Castor_COMPILER_VERSION}")
				set( Castor3D_ABI_Name_Debug "-d")
			endif()
		endif (CMAKE_COMPILER_IS_GNUCXX)
	endif()

	set( Castor3D_LIBRARY_RELEASE 	"${Castor3D_LIBRARY_NAME}${Castor3D_ABI_Name}"							)
	set( Castor3D_LIBRARY_DEBUG 	"${Castor3D_LIBRARY_RELEASE}${Castor3D_ABI_Name_Debug}"					)
	set( Castor3D_LIBRARIES			optimized ${Castor3D_LIBRARY_RELEASE} debug ${Castor3D_LIBRARY_DEBUG}	)

	set( Castor3D_LIBRARY_DEBUG 	${Castor3D_LIBRARY_DEBUG} 	CACHE FILE "Castor3D library - Debug"	)
	set( Castor3D_LIBRARY_RELEASE 	${Castor3D_LIBRARY_RELEASE}	CACHE FILE "Castor3D library - Release"	)
	set( Castor3D_LIBRARIES			${Castor3D_LIBRARIES} 		CACHE FILE "Castor3D library"			)
	
	MARK_AS_ADVANCED(Castor3D_LIBRARY_DEBUG Castor3D_LIBRARY_RELEASE)
endif()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(Castor3D DEFAULT_MSG Castor3D_LIBRARIES Castor3D_LIBRARY_DIR Castor3D_INCLUDE_DIR)
