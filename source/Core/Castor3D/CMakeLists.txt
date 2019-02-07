project( Castor3D )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D Core engine" )
set( ${PROJECT_NAME}_BRIEF "Multiplatform 3D engine" )
set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/../data/Images/castor-small.png" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( GlslangLibraries 
	glslang
	OGLCompiler
	OSDependent
	SPVRemapper
	SPIRV )

include_directories(
	${CMAKE_SOURCE_DIR}/external/glslang
)

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Src/RequiredVersion.hpp.in
	${CMAKE_CURRENT_BINARY_DIR}/Src/RequiredVersion.hpp
	NEWLINE_STYLE LF
)

set( Castor3DBinsDependencies
	${Castor3DBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

include_directories( ${Castor3DIncludeDirs} )

add_target(
	${PROJECT_NAME}
	api_dll
	"CastorUtils;ShaderAST;ShaderWriter;Ashes"
	"${CastorMinLibraries};ShaderWriter;Ashes;${GlslangLibraries}"
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Castor3DPch.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Castor3DPch.cpp
)

file( GLOB ${PROJECT_NAME}_Prerequisites_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Prerequisites*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Prerequisites*.inl
)
file( GLOB ${PROJECT_NAME}_Prerequisites_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Prerequisites*.cpp
)
source_group( "Header Files\\Prerequisites" FILES ${${PROJECT_NAME}_Prerequisites_HEADER_FILES} )
source_group( "Source Files\\Prerequisites" FILES ${${PROJECT_NAME}_Prerequisites_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Cache_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cache*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cache*.inl
)
file( GLOB ${PROJECT_NAME}_Cache_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cache*.cpp
)
source_group( "Header Files\\Cache" FILES ${${PROJECT_NAME}_Cache_HEADER_FILES} )
source_group( "Source Files\\Cache" FILES ${${PROJECT_NAME}_Cache_SOURCE_FILES} )

parse_subdir_files( Src/Animation "Animation" )
parse_subdir_files( Src/Animation/Skeleton "Animation\\\\Skeleton" )
parse_subdir_files( Src/Animation/Mesh "Animation\\\\Mesh" )
parse_subdir_files( Src/Binary "Binary" )
parse_subdir_files( Src/Buffer "Buffer" )
parse_subdir_files( Src/Cache "Cache" )
parse_subdir_files( Src/EnvironmentMap "EnvironmentMap" )
parse_subdir_files( Src/Event "Event" )
parse_subdir_files( Src/Event/Frame "Event\\\\Frame" )
parse_subdir_files( Src/Event/UserInput "Event\\\\UserInput" )
parse_subdir_files( Src/FrameBuffer "FrameBuffer" )
parse_subdir_files( Src/HDR "HDR" )
parse_subdir_files( Src/Material "Material" )
parse_subdir_files( Src/Mesh "Mesh" )
parse_subdir_files( Src/Mesh/Generator "Mesh\\\\Generator" )
parse_subdir_files( Src/Mesh/Skeleton "Mesh\\\\Skeleton" )
parse_subdir_files( Src/Mesh/SubmeshComponent "Mesh\\\\SubmeshComponent" )
parse_subdir_files( Src/Miscellaneous "Miscellaneous" )
parse_subdir_files( Src/Overlay "Overlay" )
parse_subdir_files( Src/PBR "PBR" )
parse_subdir_files( Src/Plugin "Plugin" )
parse_subdir_files( Src/Prerequisites "Prerequisites" )
parse_subdir_files( Src/PostEffect "PostEffect" )
parse_subdir_files( Src/RenderToTexture "RenderToTexture" )
parse_subdir_files( Src/Render "Render" )
parse_subdir_files( Src/Render/RenderNode "Render\\\\RenderNode" )
parse_subdir_files( Src/Render/Culling "Render\\\\Culling" )
parse_subdir_files( Src/Scene "Scene" )
parse_subdir_files( Src/Scene/Animation "Scene\\\\Animation" )
parse_subdir_files( Src/Scene/Animation/Skeleton "Scene\\\\Animation\\\\Skeleton" )
parse_subdir_files( Src/Scene/Animation/Mesh "Scene\\\\Animation\\\\Mesh" )
parse_subdir_files( Src/Scene/Background "Scene\\\\Background" )
parse_subdir_files( Src/Scene/Light "Scene\\\\Light" )
parse_subdir_files( Src/Scene/ParticleSystem "Scene\\\\ParticleSystem" )
parse_subdir_files( Src/Shader "Shader" )
parse_subdir_files( Src/Shader/PassBuffer "Shader\\\\PassBuffer" )
parse_subdir_files( Src/Shader/Shaders "Shader\\\\Shaders" )
parse_subdir_files( Src/Shader/Ubos "Shader\\\\Ubos" )
parse_subdir_files( Src/Shader/Uniform "Shader\\\\Uniform" )
parse_subdir_files( Src/ShadowMap "ShadowMap" )
parse_subdir_files( Src/State "State" )
parse_subdir_files( Src/Technique "Technique" )
parse_subdir_files( Src/Technique/Opaque "Technique\\\\Opaque" )
parse_subdir_files( Src/Technique/Opaque/Ssao "Technique\\\\Opaque\\\\Ssao" )
parse_subdir_files( Src/Technique/Transparent "Technique\\\\Transparent" )
parse_subdir_files( Src/Texture "Texture" )

set( CastorMinLibraries
	${CastorMinLibraries}
	ShaderWriter
	Ashes
	${PROJECT_NAME}
	PARENT_SCOPE
)

set( CastorBinLibraries
	${CastorBinLibraries}
	ShaderWriter
	Ashes
	${PROJECT_NAME}
	PARENT_SCOPE
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp *.dox" )
target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp *.dox" )

# Copying core zip data files
file(
	GLOB
		CoreZipFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Data/*.zip
)
copy_target_files( ${PROJECT_NAME} "" ${CoreZipFiles} )

# Copying shader languages files
file(
	GLOB
		ShaderLanguagesFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Languages/*.lang
)
copy_target_files( ${PROJECT_NAME} "" ${ShaderLanguagesFiles} )

# Preparing sample scene files installation
file(
	GLOB
		SampleSceneFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/Scene/*.cscn
		${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/Scene/*.cmtl
		${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/Scene/*.cmsh
		${CMAKE_CURRENT_SOURCE_DIR}/Test/Data/Scene/*.zip
)
install(
	FILES ${SampleSceneFiles}
	DESTINATION share/${PROJECT_NAME}/samples
	COMPONENT ${PROJECT_NAME}_Samples
)

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

