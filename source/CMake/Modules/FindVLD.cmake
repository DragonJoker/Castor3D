FIND_PACKAGE( PackageHandleStandardArgs )

if( MSVC )
	FIND_PATH(VLD_ROOT_DIR include/vld.h 
	  HINTS
	  PATH_SUFFIXES include vld "Visual Leak Detector"
	  PATHS
	  /usr/local
	  /usr
	  C:/ Z:/
	)

	FIND_PATH(VLD_INCLUDE_DIR vld.h
	  HINTS
	  PATH_SUFFIXES include
	  PATHS
	  ${VLD_ROOT_DIR}
	)

	FIND_PATH(VLD_LIBRARY_DIR vld.lib
	  HINTS
	  PATH_SUFFIXES lib/Win32
	  PATHS
	  ${VLD_ROOT_DIR}
	)

	FIND_LIBRARY(VLD_LIBRARY
	  NAMES vld.lib
	  HINTS
	  PATH_SUFFIXES lib/Win32
	  PATHS
	  ${VLD_LIBRARY_DIR}
	)

	MARK_AS_ADVANCED( VLD_LIBRARY_DIR VLD_LIBRARY )
endif()

find_package_handle_standard_args( VLD DEFAULT_MSG VLD_LIBRARY VLD_INCLUDE_DIR )