# Locate CastorUtils library
# This module defines :
#  	CastorUtils_LIBRARIES :		The library to link against
#  	CastorUtils_FOUND :			If false, do not try to link to CastorUtils
#  	CastorUtils_INCLUDE_DIR :	Where to find headers.
#	CastorUtils_LIBRARY_DIR :	Where to find the library
# You just have to set CastorUtils_ROOT_DIR in your CMakeCache.txt if it is not found.
# If CastorUtils was built in a special folder, you can specify it in CastorUtils_LIBRARY_DIR

function( Castor_COMPILER_DUMPVERSION OUTPUT_VERSION )
	exec_program(${CMAKE_CXX_COMPILER} ARGS ${CMAKE_CXX_COMPILER_ARG1} -dumpversion OUTPUT_VARIABLE Castor_COMPILER_VERSION)
	string(REGEX REPLACE "([0-9])\\.([0-9])(\\.[0-9])?" "\\1\\2" Castor_COMPILER_VERSION ${Castor_COMPILER_VERSION})
	set(${OUTPUT_VERSION} ${Castor_COMPILER_VERSION} PARENT_SCOPE)
endfunction()

FIND_PATH(CastorUtils_ROOT_DIR CastorUtils/CastorUtils.hpp
  HINTS
  PATHS
  C: D: E: F: G: H: I: J: K: L: M: N: O: P: Q: R: S: T: U: V: W: X: Y: Z:
  /usr/local/include
  /usr/include
)

if (CastorUtils_ROOT_DIR)
	FIND_PATH(CastorUtils_INCLUDE_DIR CastorUtils/CastorUtils.hpp
	  HINTS
	  PATH_SUFFIXES include
	  PATHS
	  ${CastorUtils_ROOT_DIR}
	)

	if (NOT CastorUtils_LIBRARY_DIR)
		if (CMAKE_BUILD_TYPE)
			set( CastorUtils_LIBRARY_DIR "${CastorUtils_ROOT_DIR}/lib/${CMAKE_BUILD_TYPE}")
		else ()
			set( CastorUtils_LIBRARY_DIR "${CastorUtils_ROOT_DIR}/lib")
		endif ()
	endif ()

	set( CastorUtils_LIBRARY_DIR ${CastorUtils_LIBRARY_DIR} CACHE FILEPATH "CastorUtils library directory")

	set( CastorUtils_LIBRARY_NAME "CastorUtils")
	if(WIN32)
		if(NOT (MSVC OR BORLAND))
			set( CastorUtils_LIBRARY_NAME "lib${CastorUtils_LIBRARY_NAME}")
		endif()
	else()
		set( CastorUtils_LIBRARY_NAME "lib${CastorUtils_LIBRARY_NAME}")
	endif()
	if("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel" OR "${CMAKE_CXX_COMPILER}" MATCHES "icl" OR "${CMAKE_CXX_COMPILER}" MATCHES "icpc")
		#Intel compiler
		if(WIN32)
			set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-iw")
		else()
			set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-il")
		endif()
	elseif (MSVC12)
		#Visual Studio 2013
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-vc120")
	elseif (MSVC11)
		#Visual Studio 2012
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-vc110")
	elseif (MSVC10)
		#Visual Studio 2010
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-vc100")
	elseif (MSVC90)
		#Visual Studio 2008
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-vc90")
	elseif (MSVC80)
		#Visual Studio 2005
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-vc80")
	elseif (MSVC71)
		#Visual Studio 2003
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-vc71")
	elseif (MSVC70)
		#Visual Studio 7
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-vc7")
	elseif (MSVC60)
		#Visual Studio 6
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-vc6")
	elseif (BORLAND)
		#Borland compiler
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-bcb")
	elseif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "SunPro")
		#Sun compiler
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-sw")
	elseif (MINGW)
		#MinGW compiler
		COMPILER_DUMPVERSION(COMPILER_VERSION)
		set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-mgw${COMPILER_VERSION}")
	elseif (UNIX)
		#Unix system
		if (CMAKE_COMPILER_IS_GNUCXX)
			#GNU C/C++ compiler (can be gcc, g++, clang, ...)
			COMPILER_DUMPVERSION(COMPILER_VERSION)
			if(APPLE)
				#Apple system
				set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-xgcc${COMPILER_VERSION}")
			else()
				#GNU/Linux system
				set( CastorUtils_LIBRARY_NAME "${CastorUtils_LIBRARY_NAME}-gcc${COMPILER_VERSION}")
			endif()
		endif (CMAKE_COMPILER_IS_GNUCXX)
	endif()
	set( CastorUtils_LIBRARY_RELEASE 	"${CastorUtils_LIBRARY_NAME}"	)
	set( CastorUtils_LIBRARY_DEBUG 		"${CastorUtils_LIBRARY_NAME}-d"	)
	if (WIN32)
		set( CastorUtils_LIBRARY_RELEASE 	"${CastorUtils_LIBRARY_RELEASE}.lib")
		set( CastorUtils_LIBRARY_DEBUG 		"${CastorUtils_LIBRARY_NAME}.lib")
	else()
		set( CastorUtils_LIBRARY_RELEASE 	"${CastorUtils_LIBRARY_RELEASE}.a")
		set( CastorUtils_LIBRARY_DEBUG	 	"${CastorUtils_LIBRARY_DEBUG}.a")
	endif()

	set( CastorUtils_LIBRARIES			optimized ${CastorUtils_LIBRARY_RELEASE} debug ${CastorUtils_LIBRARY_DEBUG}	)

	set( CastorUtils_LIBRARY_DEBUG		${CastorUtils_LIBRARY_DEBUG}	CACHE FILE "CastorUtils library - Debug"	)
	set( CastorUtils_LIBRARY_RELEASE	${CastorUtils_LIBRARY_RELEASE}	CACHE FILE "CastorUtils library - Release"	)
	set( CastorUtils_LIBRARIES			${CastorUtils_LIBRARIES}		CACHE FILE "CastorUtils library"			)
	
	MARK_AS_ADVANCED(CastorUtils_LIBRARY_DEBUG CastorUtils_LIBRARY_RELEASE)
endif ()

FIND_PACKAGE_HANDLE_STANDARD_ARGS(CastorUtils  DEFAULT_MSG  CastorUtils_LIBRARIES CastorUtils_LIBRARY_DIR CastorUtils_INCLUDE_DIR)
