FIND_PATH(FreeImage_ROOT_DIR include/FreeImage.h Dist/FreeImage.h
  HINTS
  PATH_SUFFIXES FreeImage
  PATHS
  /usr/local/include
  /usr/include
)

FIND_PATH(FreeImage_INCLUDE_DIR FreeImage.h 
  HINTS
  PATH_SUFFIXES Dist dist include
  PATHS
  ${FreeImage_ROOT_DIR}
  /usr/local
  /usr
)

FIND_PATH(FreeImage_LIBRARY_DIR libfreeimage.so libFreeImage.so FreeImage.lib
  HINTS
  PATH_SUFFIXES Dist dist lib64 lib
  PATHS
  ${FreeImage_ROOT_DIR}
  /usr/local
  /usr
)

FIND_LIBRARY(FreeImage_LIBRARY
  NAMES libfreeimage.so libFreeImage.so FreeImage.lib
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  ${FreeImage_LIBRARY_DIR}
  /usr/local/X11R6
  /usr/local/X11
  /usr/X11
  /sw
  /usr/freeware
)

MARK_AS_ADVANCED( FreeImage_LIBRARY_DIR )
MARK_AS_ADVANCED( FreeImage_LIBRARY )

SET( FreeImage_LIBRARY_DIRS ${FreeImage_LIBRARY_DIR} )
SET( FreeImage_LIBRARIES ${FreeImage_LIBRARY} )

find_package_handle_standard_args( FreeImage DEFAULT_MSG FreeImage_LIBRARIES FreeImage_INCLUDE_DIR )
