FIND_PATH(FreeImage_ROOT_DIR include/FreeImage.h 
  HINTS
  PATH_SUFFIXES FreeImage include
  PATHS
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_PATH(FreeImage_INCLUDE_DIR FreeImage.h 
  HINTS
  PATH_SUFFIXES Dist dist include
  PATHS
  ${FreeImage_ROOT_DIR}
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_PATH(FreeImage_LIBRARY_DIR libfreeimage.so libFreeImage.so FreeImage.lib
  HINTS
  PATH_SUFFIXES Dist dist lib64 lib
  PATHS
  ${FreeImage_ROOT_DIR}
  /usr/local/X11R6
  /usr/local/X11
  /usr/X11
  /sw
  /usr/freeware
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
