project( CastorTest )

set( ${PROJECT_NAME}_DESCRIPTION "${PROJECT_NAME} library" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 3 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

include_directories(
	${CMAKE_SOURCE_DIR}/include/Core
	${CMAKE_BINARY_DIR}/include/Core
	${CMAKE_SOURCE_DIR}/test/CastorTest
	${CMAKE_BINARY_DIR}/test/CastorTest
)

add_target(
	${PROJECT_NAME}
	lib
	${CMAKE_SOURCE_DIR}/test/${PROJECT_NAME}
	${CMAKE_SOURCE_DIR}/test/${PROJECT_NAME}
	"CastorUtils"
	"CastorUtils;${CastorBinsDependencies}"
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tests/Castor" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
