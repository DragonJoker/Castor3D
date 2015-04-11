# FindASSIMP
# ------------
#
# Locate assimp library
#
# This module defines
#
# ::
#
#   ASSIMP_LIBRARIES, the libraries to link against
#   ASSIMP_FOUND, if false, do not try to link to assimp
#   ASSIMP_INCLUDE_DIR, where to find headers.
#

FIND_PATH(ASSIMP_ROOT_DIR include/assimp/ai_assert.h 
    HINTS
    PATH_SUFFIXES
        assimp
    PATHS
        /usr/local
        /usr
)

FIND_PATH(ASSIMP_INCLUDE_DIR assimp/ai_assert.h 
    HINTS
    PATH_SUFFIXES
        include
    PATHS
        ${ASSIMP_ROOT_DIR}
        /usr/local/include
        /usr/include
)

if (CMAKE_CL_64 OR CMAKE_GENERATOR MATCHES Win64)
	if( MSVC )
		FIND_PATH(ASSIMP_LIBRARY_RELEASE_DIR assimp.lib
            HINTS
            PATH_SUFFIXES
                lib/x64
                lib/assimp_release-dll_x64
                lib/x64/Release
                lib/Release/x64
            PATHS
                ${ASSIMP_ROOT_DIR}
		)

		FIND_PATH(ASSIMP_LIBRARY_DEBUG_DIR assimpD.lib assimpd.lib
            HINTS
            PATH_SUFFIXES
                lib/x64
                lib/assimp_debug-dll_x64
                lib/x64/Debug
                lib/Debug/x64
            PATHS
                ${ASSIMP_ROOT_DIR}
				${ASSIMP_LIBRARY_RELEASE_DIR}
		)

        FIND_LIBRARY(ASSIMP_LIBRARY_RELEASE
            NAMES
                assimp.lib
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_RELEASE_DIR}
        )

        FIND_LIBRARY(ASSIMP_LIBRARY_DEBUG
            NAMES
                assimpD.lib
				assimpd.lib
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_DEBUG_DIR}
                ${ASSIMP_LIBRARY_RELEASE_DIR}
        )
	else()
		FIND_PATH(ASSIMP_LIBRARY_RELEASE_DIR libassimp.so libassimp.lib
			HINTS
			PATH_SUFFIXES
				lib64
				lib/assimp_release-dll_win64
				lib/x64/Release
				lib/Release/x64
			PATHS
				${ASSIMP_ROOT_DIR}
				/usr/local
				/usr
		)

		FIND_PATH(ASSIMP_LIBRARY_DEBUG_DIR libassimp.so libassimp.lib
			HINTS
			PATH_SUFFIXES
				lib64
				lib/assimp_debug-dll_win64
				lib/x64/Debug
				lib/Debug/x64
			PATHS
				${ASSIMP_ROOT_DIR}
				/usr/local
				/usr
		)

        FIND_LIBRARY(ASSIMP_LIBRARY_RELEASE
            NAMES
                libassimp.so
                libassimp.dll.a
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_RELEASE_DIR}
        )

        FIND_LIBRARY(ASSIMP_LIBRARY_DEBUG
            NAMES
                libassimp.so
                libassimp.dll.a
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_DEBUG_DIR}
        )
	endif()
else()
	if( MSVC )
		FIND_PATH(ASSIMP_LIBRARY_RELEASE_DIR libassimp.so assimp.lib
            HINTS
            PATH_SUFFIXES
                lib/x86
                lib/assimp_release-dll_win32
                lib/x86/Release
                lib/Release/x86
            PATHS
                ${ASSIMP_ROOT_DIR}
                /usr/local
                /usr
		)

		FIND_PATH(ASSIMP_LIBRARY_DEBUG_DIR libassimp.so assimpD.lib
            HINTS
            PATH_SUFFIXES
                lib/x86
                lib/assimp_debug-dll_win32
                lib/x86/Debug
                lib/Debug/x86
            PATHS
                ${ASSIMP_ROOT_DIR}
                /usr/local
                /usr
		)

        FIND_LIBRARY(ASSIMP_LIBRARY_RELEASE
            NAMES
                assimp.lib
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_RELEASE_DIR}
        )

        FIND_LIBRARY(ASSIMP_LIBRARY_DEBUG
            NAMES
                assimpD.lib
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_DEBUG_DIR}
        )
	elseif( MINGW )
		FIND_PATH(ASSIMP_LIBRARY_RELEASE_DIR libassimp.dll.a
            HINTS
            PATH_SUFFIXES
                lib
                lib/mingw
                lib/x86/Debug
                lib/Debug/x86
                lib/x86
            PATHS
                ${ASSIMP_ROOT_DIR}
                /usr/local
                /usr
		)

		FIND_PATH(ASSIMP_LIBRARY_DEBUG_DIR libassimp.dll.a
            HINTS
            PATH_SUFFIXES
                lib
                lib/mingw
                lib/x86/Debug
                lib/Debug/x86
                lib/x86
            PATHS
                ${ASSIMP_ROOT_DIR}
                /usr/local
                /usr
		)

        FIND_LIBRARY(ASSIMP_LIBRARY_RELEASE
            NAMES
                libassimp.dll.a
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_RELEASE_DIR}
        )

        FIND_LIBRARY(ASSIMP_LIBRARY_DEBUG
            NAMES
                libassimpd.dll.a
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_DEBUG_DIR}
        )
	else()
		FIND_PATH(ASSIMP_LIBRARY_RELEASE_DIR libassimp.so
            HINTS
            PATH_SUFFIXES
                lib
                lib/assimp_release-dll_win32
                lib/x86/Release
                lib/Release/x86
            PATHS
                ${ASSIMP_ROOT_DIR}
                /usr/local
                /usr
		)

		FIND_PATH(ASSIMP_LIBRARY_DEBUG_DIR libassimp.so
            HINTS
            PATH_SUFFIXES
                lib
                lib/assimp_debug-dll_win32
                lib/x86/Debug
                lib/Debug/x86
            PATHS
                ${ASSIMP_ROOT_DIR}
                /usr/local
                /usr
		)

        FIND_LIBRARY(ASSIMP_LIBRARY_RELEASE
            NAMES
                libassimp.so
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_RELEASE_DIR}
        )

        FIND_LIBRARY(ASSIMP_LIBRARY_DEBUG
            NAMES
                libassimp.so
            HINTS
            PATHS
                ${ASSIMP_LIBRARY_DEBUG_DIR}
        )
	endif()
endif()

MARK_AS_ADVANCED( ASSIMP_LIBRARY_RELEASE_DIR )
MARK_AS_ADVANCED( ASSIMP_LIBRARY_DEBUG_DIR )
MARK_AS_ADVANCED( ASSIMP_LIBRARY_RELEASE )
MARK_AS_ADVANCED( ASSIMP_LIBRARY_DEBUG_DIR )
find_package_handle_standard_args( ASSIMP DEFAULT_MSG ASSIMP_LIBRARY_RELEASE ASSIMP_INCLUDE_DIR )

IF ( ASSIMP_FOUND )
	IF (MSVC)
        if ( ASSIMP_LIBRARY_DEBUG )
            SET(ASSIMP_LIBRARIES optimized ${ASSIMP_LIBRARY_RELEASE} debug ${ASSIMP_LIBRARY_DEBUG} CACHE STRING "Assimp libraries")
            SET(ASSIMP_LIBRARY_DIRS ${ASSIMP_LIBRARY_RELEASE_DIR} ${ASSIMP_LIBRARY_DEBUG_DIR})
        else()
            SET(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY_RELEASE} CACHE STRING "Assimp libraries")
            SET(ASSIMP_LIBRARY_DIRS ${ASSIMP_LIBRARY_RELEASE_DIR})
        endif()
	ELSE ()
        if ( ASSIMP_LIBRARY_DEBUG )
            SET(ASSIMP_LIBRARIES optimized ${ASSIMP_LIBRARY_RELEASE} debug ${ASSIMP_LIBRARY_DEBUG} CACHE STRING "Assimp libraries")
            SET(ASSIMP_LIBRARY_DIRS ${ASSIMP_LIBRARY_RELEASE_DIR} ${ASSIMP_LIBRARY_DEBUG_DIR})
        else()
            SET(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY_RELEASE} CACHE STRING "Assimp libraries")
            SET(ASSIMP_LIBRARY_DIRS ${ASSIMP_LIBRARY_RELEASE_DIR})
        endif()
	ENDIF ()
ENDIF ()