project( PyCastor3D )

set( PYC_VERSION_MAJOR 0 )
set( PYC_VERSION_MINOR 7 )
set( PYC_VERSION_BUILD 0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D )
include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${PYTHON_INCLUDE_DIR} )

link_directories( ${Boost_LIBRARY_DIR} )

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
		FOLDER "Bindings"
)

add_target_astyle( PyCastor3D ".h;.hpp;.inl;.cpp" )
set( Build "yes (version ${PYC_VERSION_MAJOR}.${PYC_VERSION_MINOR}.${PYC_VERSION_BUILD})" PARENT_SCOPE )
