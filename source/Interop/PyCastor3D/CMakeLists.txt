project( PyCastor3D )

link_directories( ${BOOST_LIBRARY_DIR} )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D Python binding DLL." )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )


include_directories( ${Castor3DIncludeDirs} )
include_directories( ${CASTOR_SOURCE_DIR}/include/Interop )
include_directories( ${CASTOR_BINARY_DIR}/include/Interop )
include_directories( ${Boost_INCLUDE_DIR} )
include_directories( ${PYTHON_INCLUDE_DIR} )

link_directories( ${Boost_LIBRARY_DIR} )
link_directories( ${Boost_LIBRARY_DIR_RELEASE} )
link_directories( ${Boost_LIBRARY_DIR_DEBUG} )

if (MSVC)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

add_target( 
	PyCastor3D
	dll
	${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}
	"CastorUtils;Castor3D"
	"CastorUtils;Castor3D;${PythonLibraries}"
	${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
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
