# Locate CastorUtils library
# This module defines :
#  	CastorUtils_LIBRARY, the library to link against
#  	CastorUtils_FOUND, if false, do not try to link to CastorUtils
#  	CastorUtils_INCLUDE_DIR, where to find headers.
#	CastorUtils_LIBRARY_DIR, where to find the library
# You just have to set CastorUtils_ROOT_DIR in your CMakeCache.txt if it is not found.
# If CastorUtils was built in a special folder, you can specify it in CastorUtils_LIBRARY_DIR

function( Castor_COMPILER_DUMPVERSION OUTPUT_VERSION)
	exec_program(${CMAKE_CXX_COMPILER} ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion OUTPUT_VARIABLE Castor_COMPILER_VERSION)
	string(REGEX REPLACE "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2" Castor_COMPILER_VERSION ${Castor_COMPILER_VERSION})
	set(${OUTPUT_VERSION} ${Castor_COMPILER_VERSION} PARENT_SCOPE)
endfunction()

FIND_PATH(CastorUtils_ROOT_DIR include/CastorUtils/CastorUtils.hpp
  HINTS
  PATHS
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

if (CastorUtils_ROOT_DIR)
	FIND_PATH(CastorUtils_INCLUDE_DIR CastorUtils/CastorUtils.hpp
	  HINTS
	  PATH_SUFFIXES include
	  PATHS
	  ${CastorUtils_ROOT_DIR}
	  /usr/local/X11R6/include
	  /usr/local/X11/include
	  /usr/X11/include
	  /sw/include
	  /opt/local/include
	  /usr/freeware/include
	)

	if (NOT CastorUtils_LIBRARY_DIR)
		if (CMAKE_BUILD_TYPE)
			set( CastorUtils_LIBRARY_DIR "${CastorUtils_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE}")
		else ()
			set( CastorUtils_LIBRARY_DIR "${CastorUtils_ROOT_DIR}/lib")
		endif ()
	endif ()

	set( CastorUtils_LIBRARY_DIR ${CastorUtils_LIBRARY_DIR} CACHE FILEPATH "CastorUtils library directory")

	if (WIN32)
		if (MSVC)
			set( CastorUtils_LIBRARY_NAME "CastorUtils")
		elseif (BORLAND)
			set( CastorUtils_LIBRARY_NAME "CastorUtils")
		elseif (CYGWIN)
			set( CastorUtils_LIBRARY_NAME "libCastorUtils")
		else (CYGWIN)
			set( CastorUtils_LIBRARY_NAME "libCastorUtils")
		endif (MSVC)
	elseif (UNIX)
		set( CastorUtils_LIBRARY_NAME "libCastorUtils")
	elseif (APPLE)
		set( CastorUtils_LIBRARY_NAME "libCastorUtils")
	endif (WIN32)
	
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" OR "${CMAKE_CXX_COMPILER}" MATCHES "icl" OR "${CMAKE_CXX_COMPILER}" MATCHES "icpc")
		if(WIN32)
			set( CastorUtils_ABI_Name "-iw")
		else()
			set( CastorUtils_ABI_Name "-il")
		endif()
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (MSVC90)
		set( CastorUtils_ABI_Name "-vc90")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (MSVC10)
		set( CastorUtils_ABI_Name "-vc100")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (MSVC80)
		set( CastorUtils_ABI_Name "-vc80")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (MSVC71)
		set( CastorUtils_ABI_Name "-vc71")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (MSVC70)
		set( CastorUtils_ABI_Name "-vc7")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (MSVC60)
		set( CastorUtils_ABI_Name "-vc6")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (BORLAND)
		set( CastorUtils_ABI_Name "-bcb")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "SunPro")
		set( CastorUtils_ABI_Name "-sw")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (MINGW)
		Castor_COMPILER_DUMPVERSION(Castor_COMPILER_VERSION)
		set( CastorUtils_ABI_Name "-mgw${Castor_COMPILER_VERSION}")
		set( CastorUtils_ABI_Name_Debug "-g")
	elseif (UNIX)
		if (CMAKE_COMPILER_IS_GNUCXX)
			Castor_COMPILER_DUMPVERSION(Castor_COMPILER_VERSION)
			if(APPLE)
				set( CastorUtils_ABI_Name "-xgcc${Castor_COMPILER_VERSION}")
				set( CastorUtils_ABI_Name_Debug "-g")
			else()
				set( CastorUtils_ABI_Name "-gcc${Castor_COMPILER_VERSION}")
				set( CastorUtils_ABI_Name_Debug "-g")
			endif()
		endif (CMAKE_COMPILER_IS_GNUCXX)
	endif()

	set( CastorUtils_LIBRARY_DEBUG "${CastorUtils_LIBRARY_NAME}${CastorUtils_ABI_Name}${CastorUtils_ABI_Name_Debug}")
	set( CastorUtils_LIBRARY_RELEASE "${CastorUtils_LIBRARY_NAME}${CastorUtils_ABI_Name}")
	set( CastorUtils_LIBRARY optimized ${CastorUtils_LIBRARY_RELEASE} debug ${CastorUtils_LIBRARY_DEBUG})
#	set( CastorUtils_LIBRARY "${CastorUtils_LIBRARY_NAME}${CastorUtils_ABI_Name}")

	set( CastorUtils_LIBRARY_DEBUG ${CastorUtils_LIBRARY_DEBUG} CACHE FILE "CastorUtils library - Debug")
	set( CastorUtils_LIBRARY_RELEASE ${CastorUtils_LIBRARY_RELEASE} CACHE FILE "CastorUtils library - Release")
	set( CastorUtils_LIBRARY ${CastorUtils_LIBRARY} CACHE FILE "CastorUtils library")
	
	MARK_AS_ADVANCED(CastorUtils_INCLUDE_DIR CastorUtils_LIBRARY_DIR CastorUtils_LIBRARY CastorUtils_LIBRARY_DEBUG CastorUtils_LIBRARY_RELEASE)
endif ()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CastorUtils  DEFAULT_MSG  CastorUtils_LIBRARY CastorUtils_LIBRARY_DIR CastorUtils_INCLUDE_DIR)
