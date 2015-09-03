project( OceanLighting )

set( ${PROJECT_NAME}_DESCRIPTION "Eric Bruneton's OceanLightingFFT demo adapted for ${MAIN_PROJECT_NAME}" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR	8 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	${PROJECT_NAME}
	bin
	"${CastorBinsDependencies}"
	"${CastorMinLibraries};${wxWidgetsLibraries}"
	"OceanLightingPch.hpp"
	"OceanLightingPch.cpp"
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
	)
	
# Copying data files
file(
	GLOB
		${PROJECT_NAME}_datas1
		${CMAKE_CURRENT_SOURCE_DIR}/rsc/*.*
)

file(
    COPY
        ${${PROJECT_NAME}_datas1}
    DESTINATION
        ${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/share/${PROJECT_NAME}
)

file(
    COPY
        ${${PROJECT_NAME}_datas1}
    DESTINATION
        ${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/share/${PROJECT_NAME}
)

install(
	FILES ${${PROJECT_NAME}_datas1}
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/
	COMPONENT ${PROJECT_NAME}
)

file(
	GLOB
		${PROJECT_NAME}_datas2
		${CMAKE_CURRENT_SOURCE_DIR}/rsc/data/*.*
)

file(
    COPY
        ${${PROJECT_NAME}_datas2}
    DESTINATION
        ${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/share/${PROJECT_NAME}/data
)

file(
    COPY
        ${${PROJECT_NAME}_datas2}
    DESTINATION
        ${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/share/${PROJECT_NAME}/data
)

install(
	FILES ${${PROJECT_NAME}_datas2}
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/${PROJECT_NAME}/data/
	COMPONENT ${PROJECT_NAME}
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
