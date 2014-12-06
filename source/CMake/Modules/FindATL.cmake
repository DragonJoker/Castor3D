find_package( PackageHandleStandardArgs )

if ( MSVC )
    if ( NOT VC_DIR )
        if ( MSVC12 )
            get_filename_component( VS_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\12.0\\Setup\\VS;ProductDir]" REALPATH )
        elseif ( MSVC11 )
            get_filename_component( VS_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\11.0\\Setup\\VS;ProductDir]" REALPATH )
        elseif ( MSVC10 )
            get_filename_component( VS_DIR "[HKEY_LOCAL_MACHINE\\SOFTWARE\\Microsoft\\VisualStudio\\10.0\\Setup\\VS;ProductDir]" REALPATH )
        else()
            message( STATUS "!! Could not find a compatible Visual Studio directory!" )
        endif()
        if ( VS_DIR )
            set( VC_DIR ${VS_DIR}/VC )
        endif ()
    endif()

    if ( VC_DIR )
        FIND_PATH( ATL_ROOT_DIR include/atlbase.h 
            HINTS
            PATH_SUFFIXES
                atlmfc
            PATHS
                ${VC_DIR}
        )

        FIND_PATH( ATL_INCLUDE_DIR atlbase.h 
            HINTS
            PATH_SUFFIXES
                include
            PATHS
                ${ATL_ROOT_DIR}
        )

        FIND_LIBRARY( ATL_LIBRARY_RELEASE
            NAMES
                atls.lib
            HINTS
            PATH_SUFFIXES
                lib
            PATHS
                ${ATL_ROOT_DIR}
        )

        FIND_LIBRARY( ATL_LIBRARY_DEBUG
            NAMES
                atlsd.lib
            HINTS
            PATH_SUFFIXES
                lib
            PATHS
                ${ATL_ROOT_DIR}
        )
		
		if ( NOT ATL_LIBRARY_DEBUG )
			SET( ATL_LIBRARIES ${ATL_LIBRARY_RELEASE} CACHE STRING "ATL libraries" )
		else ()
			SET( ATL_LIBRARIES optimized ${ATL_LIBRARY_RELEASE} debug ${ATL_LIBRARY_DEBUG} CACHE STRING "ATL libraries" )
		endif ()
    endif()

    mark_as_advanced( ATL_ROOT_DIR ATL_INCLUDE_DIR ATL_LIBRARIES )
    find_package_handle_standard_args( ATL DEFAULT_MSG ATL_LIBRARIES ATL_INCLUDE_DIR )
endif()