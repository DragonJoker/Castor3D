# - Try to find CgToolkit
# Once done this will define
#
#  CGTOOLKIT_FOUND - System has CgToolkit
#  CgToolkit_INCLUDE_DIR      - The CgToolkit include directories
#  CgToolkit_LIBRARIES 	      - The libraries needed to use CgToolkit
#  CgToolkit_<C>_FOUND        - True if component <C> was found (<C> is upper-case)
#  CgToolkit_<C>_LIBRARY      - Libraries to link for component <C> (may include
#                               target_link_libraries debug/optimized keywords)
#
# You can set specifics libs as required :
#  GL => cgGL
#  D3D9 => cgD3D9
#  D3D10 => cgD3D10
#  D3D11 => cgD3D11

# Find CgToolkit root directory
find_path(CgToolkit_ROOT_DIR include/Cg/cg.h 
  HINTS
  PATH_SUFFIXES Cg
  PATHS
  /usr/include
  /usr/local/include
)

# Find CgToolkit include directory, from root directory
find_path(CgToolkit_INCLUDE_DIR Cg/cg.h 
  HINTS
    ENV CG_INC_PATH
  PATH_SUFFIXES include
  PATHS
  ${CgToolkit_ROOT_DIR}
  /usr/local
  /usr
)

find_path(CgToolkit_LIBRARY_DIR cg.lib Cg.lib libcg.so libCg.so
  HINTS
    ENV CG_LIB_PATH
  PATH_SUFFIXES lib64 lib
  PATHS
  ${CgToolkit_ROOT_DIR}
  /usr/local
  /usr
)

find_library(CgToolkit_LIBRARY
  NAMES cg.lib Cg.lib libcg.so libCg.so
  HINTS
  PATH_SUFFIXES lib64 lib
  PATHS
  ${CgToolkit_LIBRARY_DIR}
  /usr/local
  /usr
)

if( CgToolkit_LIBRARY )
	mark_as_advanced( CgToolkit_LIBRARY_DIR CgToolkit_LIBRARY )
	set( CgToolkit_FOUND 1 )
else()
	set( CgToolkit_FOUND 0 )
endif()

if( CgToolkit_FOUND )
	set( _CgToolkit_LIBRARIES ${CgToolkit_LIBRARY} )
	set( _CgToolkit_MISSING_COMPONENTS "" )
	foreach( COMPONENT ${CgToolkit_FIND_COMPONENTS} )
		set( CgToolkit_${UPPERCOMPONENT}_FOUND 0)
		string( TOUPPER ${COMPONENT} UPPERCOMPONENT )
		find_library( CgToolkit_${UPPERCOMPONENT}_LIBRARY
		  NAMES cg${UPPERCOMPONENT}.lib Cg${UPPERCOMPONENT}.lib Cg${UPPERCOMPONENT}.lib libcg${UPPERCOMPONENT}.so libCg${UPPERCOMPONENT}.so
		  HINTS
		  PATH_SUFFIXES lib64 lib
		  PATHS
		  ${CgToolkit_LIBRARY_DIR}
		)
		if( CgToolkit_${UPPERCOMPONENT}_LIBRARY )
			set( CgToolkit_${UPPERCOMPONENT}_FOUND 1 )
			list( APPEND _CgToolkit_LIBRARIES ${CgToolkit_${UPPERCOMPONENT}_LIBRARY} )
			mark_as_advanced( CgToolkit_${UPPERCOMPONENT}_LIBRARY )
		else()
			list( APPEND _CgToolkit_MISSING_COMPONENTS ${UPPERCOMPONENT} )
			set( CgToolkit_FOUND 0 )
		endif()
	endforeach()
endif()

if( CgToolkit_FOUND )
	if( NOT CgToolkit_FIND_QUIETLY )
		if( CgToolkit_FIND_COMPONENTS )
			message( STATUS "Found the following CgToolkit libraries:" )
		endif()
	endif()
	foreach( LIBRARY ${_CgToolkit_LIBRARIES} )
		if( NOT CgToolkit_FIND_QUIETLY )
			message( STATUS "  ${LIBRARY}" )
		endif()
		list( APPEND CgToolkit_LIBRARIES ${LIBRARY} )
	endforeach()
else()
	if( CgToolkit_FIND_REQUIRED )
		message( SEND_ERROR "Unable to find the following requested CgToolkit libraries :\n${_CgToolkit_MISSING_COMPONENTS}" )
	else()
		if( NOT CgToolkit_FIND_QUIETLY )
			message( STATUS "Could NOT find CgToolkit" )
		endif()
	endif()
endif()
