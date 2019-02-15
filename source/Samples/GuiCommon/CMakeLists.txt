project( GuiCommon )

set( Castor3DBinsDependencies
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( CastorBinLibraries
	${CastorMinLibraries}
	${wxWidgetsLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)

if ( ${CASTOR_SAMPLE_CASTOR_VIEWER_RECORDS} )
	set( CastorBinLibraries
		${CastorMinLibraries}
		${FFMpegLibraries}
		${OpenCVLibraries}
		PARENT_SCOPE
	)
endif ()

set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Common GUI tools" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

include_directories(
	${Castor3DIncludeDirs}
	${CASTOR_SOURCE_DIR}/include/Samples
	${CASTOR_BINARY_DIR}/include/Samples
	${CASTOR_SOURCE_DIR}/source/Samples
	${CASTOR_BINARY_DIR}/source/Samples
)

add_target(
	${PROJECT_NAME}
	lib
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}
	"Castor3D"
	"${CastorBinLibraries}"
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
	"${wxWidgets_CFLAGS} ${GUICOMMON_C_FLAGS}"
	"${wxWidgets_CXXFLAGS} ${GUICOMMON_CXX_FLAGS}"
)

file( GLOB ${PROJECT_NAME}_TREE_ITEMS_HEADER_FILES
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*TreeItem*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*TreeItem*.inl
)
file( GLOB ${PROJECT_NAME}_TREE_ITEMS_SOURCE_FILES
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*TreeItem*.cpp
)
source_group( "Header Files\\Properties\\Tree Items" FILES ${${PROJECT_NAME}_TREE_ITEMS_HEADER_FILES} )
source_group( "Source Files\\Properties\\Tree Items" FILES ${${PROJECT_NAME}_TREE_ITEMS_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_PROPERTIES_MATH_HEADER_FILES
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Properties*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Properties*.inl
)
file( GLOB ${PROJECT_NAME}_PROPERTIES_MATH_SOURCE_FILES
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Properties*.cpp
)
source_group( "Header Files\\Properties\\Math" FILES ${${PROJECT_NAME}_PROPERTIES_MATH_HEADER_FILES} )
source_group( "Source Files\\Properties\\Math" FILES ${${PROJECT_NAME}_PROPERTIES_MATH_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_SHADER_HEADER_FILES
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Language*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Language*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Lexer*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Lexer*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Stc*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Stc*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Style*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Style*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Uniform*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Uniform*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Shader*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Shader*.inl
)
file( GLOB ${PROJECT_NAME}_SHADER_SOURCE_FILES
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Language*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Lexer*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Stc*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Style*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Uniform*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Shader*.cpp
)
source_group( "Header Files\\Shader" FILES ${${PROJECT_NAME}_SHADER_HEADER_FILES} )
source_group( "Source Files\\Shader" FILES ${${PROJECT_NAME}_SHADER_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_SYSTEM_HEADER_FILES
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Loader*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Loader*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Screen*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Screen*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Renderer*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Renderer*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*List*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*List*.inl
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Application*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Application*.inl
)
file( GLOB ${PROJECT_NAME}_SYSTEM_SOURCE_FILES
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Loader*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Screen*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Renderer*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*List*.cpp
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Application*.cpp
)
source_group( "Header Files\\System" FILES ${${PROJECT_NAME}_SYSTEM_HEADER_FILES} )
source_group( "Source Files\\System" FILES ${${PROJECT_NAME}_SYSTEM_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_AUI_HEADER_FILES
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Aui*.hpp
	${CASTOR_SOURCE_DIR}/include/Samples/${PROJECT_NAME}/*Aui*.inl
)
file( GLOB ${PROJECT_NAME}_AUI_SOURCE_FILES
	${CASTOR_SOURCE_DIR}/source/Samples/${PROJECT_NAME}/*Aui*.cpp
)
source_group( "Header Files\\Aui" FILES ${${PROJECT_NAME}_AUI_HEADER_FILES} )
source_group( "Source Files\\Aui" FILES ${${PROJECT_NAME}_AUI_SOURCE_FILES} )

# Copying language files for GLSL and HLSL coloration in ShaderDialog
file(
	GLOB_RECURSE
		Parser_languages tools/resource/${PROJECT_NAME}/languages/*.lang
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Samples" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

if ( {${CASTOR_PACKAGE_DEB} )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "wxWidgets >= ${wxWidgetsVersion}" )
endif ()
