project( CastorTest )

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} library" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 4 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )
set( PROJECT_VERSION "${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}" )

set( ${PROJECT_NAME}_HDR_FILES
	${CASTOR_SOURCE_DIR}/test/${PROJECT_NAME}/BenchManager.hpp
	${CASTOR_SOURCE_DIR}/test/${PROJECT_NAME}/Benchmark.hpp
	${CASTOR_SOURCE_DIR}/test/${PROJECT_NAME}/CastorTestPrerequisites.hpp
	${CASTOR_SOURCE_DIR}/test/${PROJECT_NAME}/UnitTest.hpp
)
set( ${PROJECT_NAME}_SRC_FILES
	${CASTOR_SOURCE_DIR}/test/${PROJECT_NAME}/BenchManager.cpp
	${CASTOR_SOURCE_DIR}/test/${PROJECT_NAME}/Benchmark.cpp
	${CASTOR_SOURCE_DIR}/test/${PROJECT_NAME}/UnitTest.cpp
)
source_group( "Header Files"
	FILES
		${${PROJECT_NAME}_HDR_FILES}
)
source_group( "Source Files"
	FILES
		${${PROJECT_NAME}_SRC_FILES}
)
add_target_min(
	${PROJECT_NAME}
	lib
)
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_sources( ${PROJECT_NAME} 
	PRIVATE
		${CASTOR_EDITORCONFIG_FILE}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::CastorUtils
)
if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
	target_compile_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
	target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
endif ()
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		$<BUILD_INTERFACE:${CMAKE_SOURCE_DIR}/test/${PROJECT_NAME}>
		$<BUILD_INTERFACE:${CMAKE_BINARY_DIR}/test/${PROJECT_NAME}>
		$<INSTALL_INTERFACE:include>
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF
		FOLDER "Tests/Castor"
)
install_target_ex( ${PROJECT_NAME}
	Castor3D
	Test
	lib
	${CASTOR_SOURCE_DIR}/test/${PROJECT_NAME}
)
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

set( CASTOR_INSTEXP_LIST
	${CASTOR_INSTEXP_LIST}
	${PROJECT_NAME}
	PARENT_SCOPE
)
