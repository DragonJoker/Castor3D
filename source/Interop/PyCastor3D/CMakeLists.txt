project( PyCastor3D )

link_directories( ${BOOST_LIBRARY_DIR} )

set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR	9 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D )
include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${PYTHON_INCLUDE_DIR} )

link_directories( ${Boost_LIBRARY_DIR} )
#link_directories( ${Boost_LIBRARY_DIR_RELEASE} )
#link_directories( ${Boost_LIBRARY_DIR_DEBUG} )

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

add_target( 
	PyCastor3D
	dll
	"CastorUtils;Castor3D"
	"CastorUtils;Castor3D;${PythonLibraries}"
	"PyCastor3DPch.hpp"
	"PyCastor3DPch.cpp"
)

set_target_properties( PyCastor3D
	PROPERTIES
		PREFIX ""
		POSTFIX ""
		LIBRARY_OUTPUT_NAME_DEBUG castor
		RUNTIME_OUTPUT_NAME_DEBUG castor
		ARCHIVE_OUTPUT_NAME_DEBUG castor
		LIBRARY_OUTPUT_NAME_RELEASE castor
		RUNTIME_OUTPUT_NAME_RELEASE castor
		ARCHIVE_OUTPUT_NAME_RELEASE castor
		FOLDER "Interop"
)

add_target_astyle( PyCastor3D ".h;.hpp;.inl;.cpp" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
