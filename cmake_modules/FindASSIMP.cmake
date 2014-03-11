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
	FIND_PATH(ASSIMP_LIBRARY_RELEASE_DIR libassimp.so assimp.lib
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

	FIND_PATH(ASSIMP_LIBRARY_DEBUG_DIR libassimp.so assimpD.lib
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
else()
	if( MSVC )
		FIND_PATH(ASSIMP_LIBRARY_RELEASE_DIR libassimp.so assimp.lib
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

		FIND_PATH(ASSIMP_LIBRARY_DEBUG_DIR libassimp.so assimpD.lib
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
	elseif( MINGW )
		FIND_PATH(ASSIMP_LIBRARY_RELEASE_DIR libassimp.dll.a
            HINTS
            PATH_SUFFIXES
                lib
                lib/mingw
            PATHS
                ${ASSIMP_ROOT_DIR}
                /usr/local
                /usr
		)

		FIND_PATH(ASSIMP_LIBRARY_DEBUG_DIR libassimpd.dll.a
            HINTS
            PATH_SUFFIXES
                lib
                lib/mingw
            PATHS
                ${ASSIMP_ROOT_DIR}
                /usr/local
                /usr
		)
	endif()
endif()

FIND_LIBRARY(ASSIMP_LIBRARY_RELEASE
    NAMES
        libassimp.so
        assimp.lib
        libassimp.dll.a
    HINTS
    PATHS
        ${ASSIMP_LIBRARY_RELEASE_DIR}
        /usr/local
        /usr
)

FIND_LIBRARY(ASSIMP_LIBRARY_DEBUG
    NAMES
        libassimp.so
        assimpD.lib
        libassimpd.dll.a
    HINTS
    PATHS
        ${ASSIMP_LIBRARY_DEBUG_DIR}
        /usr/local
        /usr
)

SET(ASSIMP_LIBRARIES ${ASSIMP_LIBRARY_RELEASE})
SET(ASSIMP_LIBRARY_DIRS ${ASSIMP_LIBRARY_RELEASE_DIR})

IF (MSVC)
	SET(ASSIMP_LIBRARIES optimized ${ASSIMP_LIBRARY_RELEASE} debug ${ASSIMP_LIBRARY_DEBUG})
	SET(ASSIMP_LIBRARY_DIRS ${ASSIMP_LIBRARY_RELEASE_DIR} ${ASSIMP_LIBRARY_DEBUG_DIR})
ENDIF ()

MARK_AS_ADVANCED( ASSIMP_LIBRARY_RELEASE_DIR )
MARK_AS_ADVANCED( ASSIMP_LIBRARY_DEBUG_DIR )
MARK_AS_ADVANCED( ASSIMP_LIBRARY_RELEASE )
MARK_AS_ADVANCED( ASSIMP_LIBRARY_DEBUG_DIR )
find_package_handle_standard_args( ASSIMP DEFAULT_MSG ASSIMP_LIBRARIES ASSIMP_INCLUDE_DIR )
