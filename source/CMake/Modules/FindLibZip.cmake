# FindLibZip
# ------------
#
# Locate libzip library
#
# This module defines
#
# ::
#
#   LIBZIP_LIBRARY, the library to link against
#   LIBZIP_FOUND, if false, do not try to link to libzip
#   LIBZIP_INCLUDE_DIR, where to find headers.
#

FIND_PATH( LIBZIP_ROOT_DIR include/zip.h 
	HINTS
	PATHS
		/usr/local
		/usr
)

FIND_PATH( LIBZIP_INCLUDE_DIR zip.h 
	HINTS
	PATH_SUFFIXES
		include
	PATHS
		${LIBZIP_ROOT_DIR}
)

FIND_PATH( LIBZIP_LIBRARY_DIR libzip.so zip.lib
	HINTS
	PATH_SUFFIXES
		lib
	PATHS
		${LIBZIP_ROOT_DIR}
)

if ( WIN32 )
	FIND_PATH( LIBZIP_ZIPCONF_DIR zipconf.h 
		HINTS
		PATH_SUFFIXES
			libzip/include
		PATHS
			${LIBZIP_LIBRARY_DIR}
	)
else ()
	FIND_PATH( LIBZIP_ZIPCONF_DIR zipconf.h 
		HINTS
		PATH_SUFFIXES
			include
		PATHS
			${LIBZIP_INCLUDE_DIR}
	)
endif ()

FIND_LIBRARY( LIBZIP_LIBRARY
	NAMES
		zip
	HINTS
	PATHS
		${LIBZIP_LIBRARY_DIR}
)
MARK_AS_ADVANCED( LIBZIP_ROOT_DIR )
MARK_AS_ADVANCED( LIBZIP_LIBRARY )
MARK_AS_ADVANCED( LIBZIP_LIBRARY_DIR )
find_package_handle_standard_args( LIBZIP DEFAULT_MSG LIBZIP_LIBRARY LIBZIP_INCLUDE_DIR LIBZIP_ZIPCONF_DIR )

if ( LIBZIP_FOUND )
	set( LIBZIP_INCLUDE_DIRS
		${LIBZIP_INCLUDE_DIR}
		${LIBZIP_ZIPCONF_DIR}
	)
endif ()