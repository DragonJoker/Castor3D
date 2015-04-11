# FindFreeImage
# ------------
#
# Locate FreeIMage library
#
# This module defines
#
# ::
#
#   FreeImage_LIBRARIES, the library to link against
#   FREEIMAGE_FOUND, if false, do not try to link to FreeImage
#   FreeImage_INCLUDE_DIR, where to find headers.
#   FreeImage_VERSION_STRING, the version of FreeImage found.
#

set( FreeImage_PLATFORM "x86" )

if(MSVC)
	if( (CMAKE_CL_64 OR CMAKE_GENERATOR MATCHES Win64) )
		set( FreeImage_PLATFORM "x64" )
	endif()
else()
	if( (${CMAKE_SIZEOF_VOID_P} EQUAL 8) AND NOT MINGW )
		set( FreeImage_PLATFORM "x64" )
	endif()
endif()

if (WIN32)
	FIND_PATH(FreeImage_ROOT_DIR Dist/FreeImage.h
		HINTS
		PATH_SUFFIXES
			FreeImage
	)

	FIND_PATH(FreeImage_INCLUDE_DIR FreeImage.h 
		HINTS
		PATH_SUFFIXES
            Dist
		PATHS
			${FreeImage_ROOT_DIR}
	)

    FIND_PATH(FreeImage_LIBRARY_DIR FreeImage.lib
        HINTS
        PATH_SUFFIXES
            Dist/${FreeImage_PLATFORM}
        PATHS
            ${FreeImage_ROOT_DIR}
    )

    FIND_LIBRARY(FreeImage_LIBRARY
        NAMES FreeImage.lib
        HINTS
        PATHS
            ${FreeImage_LIBRARY_DIR}
    )
else()
	FIND_PATH(FreeImage_ROOT_DIR include/FreeImage.h Dist/FreeImage.h
		HINTS
		PATH_SUFFIXES
			FreeImage
		PATHS
			/usr/local/include
			/usr/include
	)

	FIND_PATH(FreeImage_INCLUDE_DIR FreeImage.h 
		HINTS
		PATH_SUFFIXES
            include
		PATHS
			${FreeImage_ROOT_DIR}
	)

	FIND_PATH(FreeImage_LIBRARY_DIR libfreeimage.so libFreeImage.so
		HINTS
		PATH_SUFFIXES
			lib64
			lib
		PATHS
			${FreeImage_ROOT_DIR}
	)

	FIND_LIBRARY(FreeImage_LIBRARY
		NAMES libfreeimage.so libFreeImage.so
		HINTS
		PATH_SUFFIXES
			lib64
			lib
		PATHS
			${FreeImage_LIBRARY_DIR}
	)
endif()

if ( EXISTS "${FreeImage_INCLUDE_DIR}/FreeImage.h" )
  set( FreeImage_H "${FreeImage_INCLUDE_DIR}/FreeImage.h" )
endif ()

if ( FreeImage_INCLUDE_DIR AND FreeImage_H )
    file( STRINGS "${FreeImage_H}" freeimage_version_str REGEX "^#[\t ]*define[\t ]+FREEIMAGE_(MAJOR_VERSION|MINOR_VERSION|RELEASE_SERIAL)[\t ]+[0-9]+$" )
    unset( FreeImage_VERSION_STRING )
    foreach ( VPART MAJOR_VERSION MINOR_VERSION RELEASE_SERIAL )
        foreach ( VLINE ${freeimage_version_str} )
            if ( VLINE MATCHES "^#[\t ]*define[\t ]+FREEIMAGE_${VPART}" )
                string( REGEX REPLACE "^#[\t ]*define[\t ]+FREEIMAGE_${VPART}[\t ]+([0-9]+)$" "\\1" FreeImage_VERSION_PART "${VLINE}" )
                if ( FreeImage_VERSION_STRING )
                    set( FreeImage_VERSION_STRING "${FreeImage_VERSION_STRING}.${FreeImage_VERSION_PART}" )
                else()
                    set( FreeImage_VERSION_STRING "${FreeImage_VERSION_PART}" )
                endif()
                unset( FreeImage_VERSION_PART )
            endif()
        endforeach()
    endforeach()
endif()

SET( FreeImage_LIBRARY_DIRS ${FreeImage_LIBRARY_DIR} )

if ( FreeImage_LIBRARY )
    SET( FreeImage_LIBRARIES ${FreeImage_LIBRARY} )
endif ()

MARK_AS_ADVANCED( FreeImage_LIBRARY_DIR )
MARK_AS_ADVANCED( FreeImage_LIBRARY )
MARK_AS_ADVANCED( FreeImage_LIBRARIES )

find_package_handle_standard_args( FreeImage DEFAULT_MSG FreeImage_LIBRARIES FreeImage_INCLUDE_DIR )
