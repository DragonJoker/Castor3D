project( GuiCommon )

set( CastorBinsDependencies
	${CastorBinsDependencies}
	GuiCommon
	PARENT_SCOPE
)

set( CastorBinLibraries
	${CastorMinLibraries}
	${wxWidgetsLibraries}
	GuiCommon
	PARENT_SCOPE
)

set( GC_VERSION_MAJOR	0 )
set( GC_VERSION_MINOR	7 )
set( GC_VERSION_BUILD	0 )

include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	GuiCommon
	lib
	"Castor3D"
	"${CastorBinLibraries}"
	"GuiCommonPch.hpp"
	"GuiCommonPch.cpp"
	"${wxWidgets_CFLAGS}"
	"${wxWidgets_CXXFLAGS}"
)

file( GLOB ${PROJECT_NAME}_PROPERTIES_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Properties*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Properties*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Property*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Property*.inl
)
file( GLOB ${PROJECT_NAME}_PROPERTIES_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Properties*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Property*.cpp
)
source_group( "Header Files\\Properties" FILES ${${PROJECT_NAME}_PROPERTIES_HEADER_FILES} )
source_group( "Source Files\\Properties" FILES ${${PROJECT_NAME}_PROPERTIES_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_SHADER_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Language*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Language*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Lexer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Lexer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stc*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stc*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Style*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Style*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameVariable*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameVariable*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.inl
)
file( GLOB ${PROJECT_NAME}_SHADER_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Language*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Lexer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Stc*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Style*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameVariable*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.cpp
)
source_group( "Header Files\\Shader" FILES ${${PROJECT_NAME}_SHADER_HEADER_FILES} )
source_group( "Source Files\\Shader" FILES ${${PROJECT_NAME}_SHADER_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_SYSTEM_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Screen*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Screen*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*List*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*List*.inl
)
file( GLOB ${PROJECT_NAME}_SYSTEM_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Loader*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Screen*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*List*.cpp
)
source_group( "Header Files\\System" FILES ${${PROJECT_NAME}_SYSTEM_HEADER_FILES} )
source_group( "Source Files\\System" FILES ${${PROJECT_NAME}_SYSTEM_SOURCE_FILES} )

# Copying language files for GLSL and HLSL coloration in wxShaderDialog
file(
	GLOB_RECURSE
		Parser_languages tools/resource/GuiCommon/languages/*.lang
)

set_property( TARGET GuiCommon PROPERTY FOLDER "Samples" )
add_target_astyle( GuiCommon ".h;.hpp;.inl;.cpp" )

set( Build "yes (version ${GC_VERSION_MAJOR}.${GC_VERSION_MINOR}.${GC_VERSION_BUILD})" PARENT_SCOPE )

if ( {${CASTOR_PACKAGE_DEB} )
	set( CPACK_DEBIAN_PACKAGE_DEPENDS "wxWidgets >= ${wxWidgetsVersion}" )
endif ()