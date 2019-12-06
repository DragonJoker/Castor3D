project( OceanLighting )

set( ${PROJECT_NAME}_DESCRIPTION "Eric Bruneton's OceanLightingFFT demo adapted for ${MAIN_PROJECT_NAME}" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 10 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

add_target(
	${PROJECT_NAME}
	bin
	"${Castor3DBinsDependencies}"
	"${CastorMinLibraries};${wxWidgetsLibraries}"
	${CMAKE_CURRENT_SOURCE_DIR}/Src/OceanLightingPch.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/OceanLightingPch.cpp
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)
target_include_directories( ${PROJECT_NAME}
	PRIVATE
		${Castor3DIncludeDirs}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::Castor3D
)
# Copying data files
file(
	GLOB
		${PROJECT_NAME}_datas1
		${CMAKE_CURRENT_SOURCE_DIR}/rsc/*.*
)
copy_target_files( ${PROJECT_NAME} "" ${${PROJECT_NAME}_datas1} )
copy_target_directory( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/rsc/data data )
copy_target_directory( ${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/rsc/Glsl130_Shaders Glsl130_Shaders )

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples" )
set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
