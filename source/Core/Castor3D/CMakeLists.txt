project( Castor3D )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D Core engine" )
set( ${PROJECT_NAME}_BRIEF "Multiplatform 3D engine" )
set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/data/Images/castor-small.png" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR 11 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

set( ExternalIncludes )

if ( CASTOR_USE_GLSLANG )
	set( Castor3DDefinitions
		${Castor3DDefinitions}
		C3D_HasGlslang
	)
endif ()

if ( EXISTS ${CASTOR_SOURCE_DIR}/${ASHES_DIR}/external/spirv_cross/spirv_glsl.cpp )
	set( SPIRV_CROSS_DIR
		${CASTOR_SOURCE_DIR}/${ASHES_DIR}/external/spirv_cross
	)
	set( SPIRV_HEADER_FILES
		${SPIRV_CROSS_DIR}/GLSL.std.450.h
		${SPIRV_CROSS_DIR}/spirv_common.hpp
		${SPIRV_CROSS_DIR}/spirv.hpp
		${SPIRV_CROSS_DIR}/spirv_cross.hpp
		${SPIRV_CROSS_DIR}/spirv_parser.hpp
		${SPIRV_CROSS_DIR}/spirv_cross_parsed_ir.hpp
		${SPIRV_CROSS_DIR}/spirv_cfg.hpp
		${SPIRV_CROSS_DIR}/spirv_glsl.hpp
	)
	set( SPIRV_SOURCE_FILES
		${SPIRV_CROSS_DIR}/spirv_cross.cpp
		${SPIRV_CROSS_DIR}/spirv_cfg.cpp
		${SPIRV_CROSS_DIR}/spirv_glsl.cpp
		${SPIRV_CROSS_DIR}/spirv_parser.cpp
		${SPIRV_CROSS_DIR}/spirv_cross_parsed_ir.cpp
	)
	set( SPIRV_FILES
		${SPIRV_HEADER_FILES}
		${SPIRV_SOURCE_FILES}
	)
	set( Castor3DDefinitions
		${Castor3DDefinitions}
		C3D_HasSPIRVCross
	)
	set( ExternalIncludes
		${ExternalIncludes}
		${SPIRV_CROSS_DIR}
	)
endif ()

configure_file(
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp.in
	${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/RequiredVersion.hpp
	NEWLINE_STYLE LF
)

set( Castor3DBinsDependencies
	${Castor3DBinsDependencies}
	castor::${PROJECT_NAME}
	PARENT_SCOPE
)

add_target(
	${PROJECT_NAME}
	api_dll
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}
	""
	""
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
	""
	""
	""
	"${SPIRV_FILES}"
)
source_group( "Header Files\\SPIRVCross" FILES ${SPIRV_HEADER_FILES} )
source_group( "Source Files\\SPIRVCross" FILES ${SPIRV_SOURCE_FILES} )
add_library( castor::${PROJECT_NAME}
	ALIAS
	${PROJECT_NAME}
)
target_include_directories( ${PROJECT_NAME}
	PUBLIC
		${Vulkan_INCLUDE_DIRS}
		${ASHES_SOURCE_DIR}/include
		${ASHES_CONFIG_DIR}/include
	PRIVATE
		${CASTOR_SOURCE_DIR}/include/Core
		${CASTOR_BINARY_DIR}/include/Core
		${CASTOR_SOURCE_DIR}/source/Core
		${CASTOR_BINARY_DIR}/source/Core
		${glslang_INCLUDE_DIR}
		${ExternalIncludes}
)
target_compile_definitions( ${PROJECT_NAME}
	PUBLIC
		${Castor3DDefinitions}
)
target_link_libraries( ${PROJECT_NAME}
	PUBLIC
		castor::CastorUtils
		sdw::ShaderWriter
		ashes::ashespp
	PRIVATE
		sdw::CompilerSpirV
		${glslang_LIBRARIES}
)

file( GLOB ${PROJECT_NAME}_Prerequisites_HEADER_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/*Prerequisites*.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/*Prerequisites*.inl
)
file( GLOB ${PROJECT_NAME}_Prerequisites_SOURCE_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/*Prerequisites*.cpp
)
source_group( "Header Files\\Prerequisites" FILES ${${PROJECT_NAME}_Prerequisites_HEADER_FILES} )
source_group( "Source Files\\Prerequisites" FILES ${${PROJECT_NAME}_Prerequisites_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Cache_HEADER_FILES
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/*Cache*.hpp
	${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/*Cache*.inl
)
file( GLOB ${PROJECT_NAME}_Cache_SOURCE_FILES
	${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/*Cache*.cpp
)
source_group( "Header Files\\Cache" FILES ${${PROJECT_NAME}_Cache_HEADER_FILES} )
source_group( "Source Files\\Cache" FILES ${${PROJECT_NAME}_Cache_SOURCE_FILES} )

c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Animation "Animation" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Animation/Skeleton "Animation\\\\\\\\Skeleton" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Animation/Mesh "Animation\\\\\\\\Mesh" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Binary "Binary" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Buffer "Buffer" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Cache "Cache" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core EnvironmentMap "EnvironmentMap" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Event "Event" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Event/Frame "Event\\\\\\\\Frame" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Event/UserInput "Event\\\\\\\\UserInput" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core FrameBuffer "FrameBuffer" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core HDR "HDR" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Material "Material" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Mesh "Mesh" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Mesh/Generator "Mesh\\\\\\\\Generator" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Mesh/Skeleton "Mesh\\\\\\\\Skeleton" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Mesh/SubmeshComponent "Mesh\\\\\\\\SubmeshComponent" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Miscellaneous "Miscellaneous" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Overlay "Overlay" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core PBR "PBR" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Plugin "Plugin" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Prerequisites "Prerequisites" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core PostEffect "PostEffect" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core RenderToTexture "RenderToTexture" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Render "Render" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Render/RenderNode "Render\\\\\\\\RenderNode" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Render/Culling "Render\\\\\\\\Culling" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Scene "Scene" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Scene/Animation "Scene\\\\\\\\Animation" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Scene/Animation/Skeleton "Scene\\\\\\\\Animation\\\\\\\\Skeleton" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Scene/Animation/Mesh "Scene\\\\\\\\Animation\\\\\\\\Mesh" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Scene/Background "Scene\\\\\\\\Background" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Scene/Light "Scene\\\\\\\\Light" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Scene/ParticleSystem "Scene\\\\\\\\ParticleSystem" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Shader "Shader" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Shader/PassBuffer "Shader\\\\\\\\PassBuffer" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Shader/Shaders "Shader\\\\\\\\Shaders" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Shader/TextureConfigurationBuffer "Shader\\\\\\\\TextureConfigurationBuffer" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Shader/Ubos "Shader\\\\\\\\Ubos" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Shader/Uniform "Shader\\\\\\\\Uniform" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core ShadowMap "ShadowMap" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core State "State" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Technique "Technique" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Technique/Opaque "Technique\\\\\\\\Opaque" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Technique/Opaque/Ssao "Technique\\\\\\\\Opaque\\\\\\\\Ssao" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Technique/Transparent "Technique\\\\\\\\Transparent" )
c3d_parse_subdir_files( ${CASTOR_SOURCE_DIR} Core Texture "Texture" )

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp *.dox" )
target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp *.dox" )

# Copying core zip data files
file(
	GLOB
		CoreZipFiles
		${CASTOR_SOURCE_DIR}/data/${PROJECT_NAME}/*.zip
)
copy_target_files( ${PROJECT_NAME} "" ${CoreZipFiles} )

# Copying shader languages files
file(
	GLOB
		ShaderLanguagesFiles
		${CASTOR_SOURCE_DIR}/languages/${PROJECT_NAME}/*.lang
)
copy_target_files( ${PROJECT_NAME} "" ${ShaderLanguagesFiles} )

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )

