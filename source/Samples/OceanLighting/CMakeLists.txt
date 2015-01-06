project( OceanLighting )

set( OL_VERSION_MAJOR	0 )
set( OL_VERSION_MINOR	7 )
set( OL_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	OceanLighting
	bin
	"${CastorBinsDependencies}"
	"${CastorBinLibraries}"
	"OceanLightingPch.hpp"
	"OceanLightingPch.cpp"
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
	)
	
# Copying data files
file(
	GLOB
		OceanLighting_datas1
		${CMAKE_CURRENT_SOURCE_DIR}/rsc/*.*
)

file(
    COPY
        ${OceanLighting_datas1}
    DESTINATION
        ${PROJECTS_BINARIES_OUTPUT_PATH_RELEASE}/share/OceanLighting
)

file(
    COPY
        ${OceanLighting_datas1}
    DESTINATION
        ${PROJECTS_BINARIES_OUTPUT_PATH_DEBUG}/share/OceanLighting
)

install(
	FILES ${OceanLighting_datas1}
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/OceanLighting/
	COMPONENT OceanLighting
)

file(
	GLOB
		OceanLighting_datas2
		${CMAKE_CURRENT_SOURCE_DIR}/rsc/data/*.*
)

file(
    COPY
        ${OceanLighting_datas2}
    DESTINATION
        ${PROJECTS_BINARIES_OUTPUT_PATH_RELEASE}/share/OceanLighting/data
)

file(
    COPY
        ${OceanLighting_datas2}
    DESTINATION
        ${PROJECTS_BINARIES_OUTPUT_PATH_DEBUG}/share/OceanLighting/data
)

install(
	FILES ${OceanLighting_datas2}
	DESTINATION ${CMAKE_INSTALL_PREFIX}/share/OceanLighting/data/
	COMPONENT OceanLighting
)

set_property( TARGET OceanLighting PROPERTY FOLDER "Samples" )
set( Build "yes (version ${OL_VERSION_MAJOR}.${OL_VERSION_MINOR}.${OL_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( OceanLighting ".h;.hpp;.inl;.cpp" )
