FIND_PACKAGE( PackageHandleStandardArgs )

FIND_PATH(ASTYLE_ROOT_DIR
  NAMES
  	bin/AStyle.exe
  	bin/astyle
  HINTS
	PATH_SUFFIXES AStyle
  PATHS
  	/usr
  	/usr/local
	C:/
	Z:/
)

SET( ASTYLE_BINARY_DIR ${ASTYLE_ROOT_DIR}/bin )

FIND_PROGRAM( ASTYLE_BINARY
  NAMES 
  	AStyle.exe
	astyle
  PATHS
  	${ASTYLE_BINARY_DIR}
)

MARK_AS_ADVANCED( ASTYLE_LIBRARY_DIR ASTYLE_BINARY )

find_package_handle_standard_args( ASTYLE DEFAULT_MSG ASTYLE_BINARY )
