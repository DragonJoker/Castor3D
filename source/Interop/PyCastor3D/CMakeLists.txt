project( PyCastor3D )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D Python binding DLL." )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

if (MSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /bigobj")
endif()

add_target( 
	PyCastor3D
	dll
	${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}
	""
	""
	${CASTOR_SOURCE_DIR}/include/Interop/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Interop/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( PyCastor3D
	PUBLIC
		${CASTOR_SOURCE_DIR}/include/Interop
		${CASTOR_BINARY_DIR}/include/Interop
		${Boost_INCLUDE_DIR}
		${PYTHON_INCLUDE_DIR}
	PRIVATE
		${Castor3DIncludeDirs}
)
target_link_directories( PyCastor3D
	PUBLIC
		${Boost_LIBRARY_DIR}
		${Boost_LIBRARY_DIR_RELEASE}
		${Boost_LIBRARY_DIR_DEBUG}
)
target_link_libraries( PyCastor3D
	PUBLIC
		${Boost_PYTHON_LIBRARY}
		${PYTHON_LIBRARY}
		castor::Castor3D
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
