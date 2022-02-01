project( CastorTest )

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} library" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 3 )
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
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::CastorUtils
)
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		${CMAKE_SOURCE_DIR}/test/${PROJECT_NAME}
		${CMAKE_BINARY_DIR}/test/${PROJECT_NAME}
)
set_target_properties( ${PROJECT_NAME}
	PROPERTIES
		VERSION ${PROJECT_VERSION}
		CXX_STANDARD 20
		CXX_EXTENSIONS OFF
		FOLDER "Tests/Castor"
)
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
