FIND_PATH(Cg_ROOT_DIR include/Cg/cg.h 
  HINTS
  PATH_SUFFIXES Cg include
  PATHS
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_PATH(Cg_INCLUDE_DIR Cg/cg.h 
  HINTS
  PATH_SUFFIXES include
  PATHS
  ${Cg_ROOT_DIR}
  /usr/local/X11R6/include
  /usr/local/X11/include
  /usr/X11/include
  /sw/include
  /opt/local/include
  /usr/freeware/include
)

FIND_PATH(Cg_LIBRARY_DIR cg.lib Cg.lib libcg.so libCg.so
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  ${Cg_ROOT_DIR}
  /usr/local/X11R6
  /usr/local/X11
  /usr/X11
  /sw
  /usr/freeware
)

FIND_LIBRARY(Cg_LIBRARY
  NAMES cg.lib Cg.lib libcg.so libCg.so
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  ${Cg_LIBRARY_DIR}
  /usr/local/X11R6
  /usr/local/X11
  /usr/X11
  /sw
  /usr/freeware
)

FIND_LIBRARY(Cg_GL_LIBRARY
  NAMES cgGL.lib CgGL.lib CgGl.lib libcgGL.so libCgGl.so libCgGL.so
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  ${Cg_LIBRARY_DIR}
  /usr/local/X11R6
  /usr/local/X11
  /usr/X11
  /sw
  /usr/freeware
)

if (WIN32)
	FIND_LIBRARY(Cg_D3D9_LIBRARY
	  NAMES cgD3D9.lib
	  HINTS
	  PATH_SUFFIXES lib64 lib
	  PATHS
	  ${Cg_LIBRARY_DIR}
	)
endif (WIN32)
