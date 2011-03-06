FIND_PATH(Glew_ROOT_DIR include/glew.h 
  HINTS
  PATH_SUFFIXES Glew include
  PATHS
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_PATH(Glew_INCLUDE_DIR GL/glew.h  glew.h
  HINTS
  PATH_SUFFIXES include
  PATHS
  ${Glew_ROOT_DIR}
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_PATH(Glew_LIBRARY_DIR glew libglew glew32.lib
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  ${Glew_ROOT_DIR}
  /usr/local/X11R6
  /usr/local/X11
  /usr/X11
  /sw
  /usr/freeware
)

FIND_LIBRARY(Glew_LIBRARY
  NAMES glew libglew glew32.lib
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  ${Glew_LIBRARY_DIR}
  /usr/local/X11R6
  /usr/local/X11
  /usr/X11
  /sw
  /usr/freeware
)