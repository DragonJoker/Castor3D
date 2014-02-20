if( WIN32 )
	
	FIND_PATH(D3D9_ROOT_DIR Include/d3d9.h
	  HINTS
	    ENV DXSDK_DIR
	  PATH_SUFFIXES  DxSDK_Jun2010
	)

	FIND_PATH(D3D9_INCLUDE_DIR d3d9.h 
	  HINTS
	  PATH_SUFFIXES Include/um Include
	  PATHS
	  ${D3D9_ROOT_DIR}
	)

	if( CMAKE_CL_64 )
		FIND_PATH(D3D9_LIBRARY_DIR d3d9.lib
		  HINTS
		  PATH_SUFFIXES Lib/x64
		  PATHS
		  ${D3D9_ROOT_DIR}
		)

		FIND_LIBRARY(D3D9_LIBRARY
		  NAMES d3d9.lib
		  HINTS
		  PATH_SUFFIXES Lib/x64
		  PATHS
		  ${D3D9_LIBRARY_DIR}
		)

		FIND_LIBRARY(D3D9_X_LIBRARY_RELEASE
		  NAMES d3dx9.lib
		  HINTS
		  PATH_SUFFIXES Lib/x64
		  PATHS
		  ${D3D9_LIBRARY_DIR}
		)

		FIND_LIBRARY(D3D9_X_LIBRARY_DEBUG
		  NAMES d3dx9d.lib
		  HINTS
		  PATH_SUFFIXES Lib/x64
		  PATHS
		  ${D3D9_LIBRARY_DIR}
		)
	else()
		FIND_PATH(D3D9_LIBRARY_DIR d3d9.lib
		  HINTS
		  PATH_SUFFIXES Lib/x86
		  PATHS
		  ${D3D9_ROOT_DIR}
		)

		FIND_LIBRARY(D3D9_LIBRARY
		  NAMES d3d9.lib
		  HINTS
		  PATH_SUFFIXES Lib/x86
		  PATHS
		  ${D3D9_LIBRARY_DIR}
		)

		FIND_LIBRARY(D3D9_X_LIBRARY_RELEASE
		  NAMES d3dx9.lib
		  HINTS
		  PATH_SUFFIXES Lib/x86
		  PATHS
		  ${D3D9_LIBRARY_DIR}
		)

		FIND_LIBRARY(D3D9_X_LIBRARY_DEBUG
		  NAMES d3dx9d.lib
		  HINTS
		  PATH_SUFFIXES Lib/x86
		  PATHS
		  ${D3D9_LIBRARY_DIR}
		)
	endif()

	set(D3D9_LIBRARIES optimized ${D3D9_LIBRARY} ${D3D9_X_LIBRARY_RELEASE} debug ${D3D9_LIBRARY} ${D3D9_X_LIBRARY_DEBUG} )
	MARK_AS_ADVANCED( D3D9_LIBRARY_DIR D3D9_LIBRARY D3D9_X_LIBRARY )
	find_package_handle_standard_args( Direct3D9  DEFAULT_MSG D3D9_LIBRARY D3D9_X_LIBRARY_RELEASE D3D9_X_LIBRARY_DEBUG D3D9_INCLUDE_DIR )
endif()