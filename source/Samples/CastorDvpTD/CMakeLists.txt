project( CastorDvpTD )

set( ${PROJECT_NAME}_WXWIDGET 1 )
set( ${PROJECT_NAME}_DESCRIPTION "Castor Tower Defense" )
set( ${PROJECT_NAME}_VERSION_MAJOR	1 )
set( ${PROJECT_NAME}_VERSION_MINOR	1 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories(
	${Castor3DIncludeDirs}
	${CASTOR_SOURCE_DIR}/include/Samples
	${CASTOR_BINARY_DIR}/include/Samples
	${CASTOR_SOURCE_DIR}/source/Samples
	${CASTOR_BINARY_DIR}/source/Samples
)

message( STATUS "${CASTOR_SOURCE_DIR}/include/Samples" )
message( STATUS "${CASTOR_BINARY_DIR}/include/Samples" )
message( STATUS "${CASTOR_SOURCE_DIR}/source/Samples" )
message( STATUS "${CASTOR_BINARY_DIR}/source/Samples" )
add_target(
	${PROJECT_NAME}
	bin
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}
	"${Castor3DBinsDependencies}"
	"${CastorBinLibraries}"
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
	"${wxWidgets_CFLAGS} ${GUICOMMON_C_FLAGS}"
	"${wxWidgets_CXXFLAGS} ${GUICOMMON_CXX_FLAGS}"
)
set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

file(
	GLOB
		DataFiles
		${CASTOR_SOURCE_DIR}/data/${PROJECT_NAME}/*.*
)

copy_target_files( ${PROJECT_NAME} "" ${DataFiles} )
