project( DiffImage )

set( ${PROJECT_NAME}_DESCRIPTION "Diffs two images issued from Castor3D" )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	0 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories(
	${CASTOR_SOURCE_DIR}/include/Core
	${CASTOR_BINARY_DIR}/include/Core
	${CASTOR_BINARY_DIR}/source/Core
)

add_target(
	${PROJECT_NAME}
	bin_dos
	${CASTOR_SOURCE_DIR}/include/Tools/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Tools/${PROJECT_NAME}
	"CastorUtils"
	"CastorUtils"
	""
	""
)
target_compile_definitions( ${PROJECT_NAME} PRIVATE
	${wxWidgets_DEFINITIONS}
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Tools" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
