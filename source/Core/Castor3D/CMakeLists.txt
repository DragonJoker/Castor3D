project( Castor3D )

set( ${PROJECT_NAME}_DESCRIPTION "Castor3D Core engine" )
set( ${PROJECT_NAME}_VERSION_MAJOR	0 )
set( ${PROJECT_NAME}_VERSION_MINOR	8 )
set( ${PROJECT_NAME}_VERSION_BUILD	0 )

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Src/RequiredVersion.hpp.in
	${CMAKE_CURRENT_BINARY_DIR}/Src/RequiredVersion.hpp
	NEWLINE_STYLE LF
)

set( CastorBinsDependencies
	${CastorBinsDependencies}
	${PROJECT_NAME}
	PARENT_SCOPE
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	${PROJECT_NAME}
	api_dll
	"CastorUtils"
	"${CastorMinLibraries}"
	"Castor3DPch.hpp"
	"Castor3DPch.cpp"
)

file( GLOB ${PROJECT_NAME}_Main_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Engine.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Engine.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Manager*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Manager*.inl
)
file( GLOB ${PROJECT_NAME}_Main_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/Engine.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Manager*.cpp
)
source_group( "Header Files\\Main" FILES ${${PROJECT_NAME}_Main_HEADER_FILES} )
source_group( "Source Files\\Main" FILES ${${PROJECT_NAME}_Main_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Plugin_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plugin*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plugin*.inl
)
file( GLOB ${PROJECT_NAME}_Plugin_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plugin*.cpp
)
source_group( "Header Files\\Plugin" FILES ${${PROJECT_NAME}_Plugin_HEADER_FILES} )
source_group( "Source Files\\Plugin" FILES ${${PROJECT_NAME}_Plugin_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Animation_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Anim*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Anim*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Bone*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Bone*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Moving*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Moving*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Skeleton*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Skeleton*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/KeyFrame*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/KeyFrame*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Interpolator*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Interpolator*.inl
)
file( GLOB ${PROJECT_NAME}_Animation_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Anim*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Bone*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Moving*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Skeleton*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/KeyFrame*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Interpolator*.cpp
)
source_group( "Header Files\\Animation" FILES ${${PROJECT_NAME}_Animation_HEADER_FILES} )
source_group( "Source Files\\Animation" FILES ${${PROJECT_NAME}_Animation_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_VBO_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Buffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Buffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/CpuBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/CpuBuffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/GpuBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/GpuBuffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/GeometryBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/GeometryBuffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/IndexBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/IndexBuffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/MatrixBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/MatrixBuffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/VertexBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/VertexBuffer*.inl
)
file( GLOB ${PROJECT_NAME}_VBO_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Buffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/CpuBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/GpuBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/GeometryBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/IndexBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/MatrixBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/VertexBuffer*.cpp
)
source_group( "Header Files\\VertexBuffer" FILES ${${PROJECT_NAME}_VBO_HEADER_FILES} )
source_group( "Source Files\\VertexBuffer" FILES ${${PROJECT_NAME}_VBO_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_FBO_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*BackBuffers*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*BackBuffers*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.inl
)
file( GLOB ${PROJECT_NAME}_FBO_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*BackBuffers*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.cpp
)
source_group( "Header Files\\FrameBuffer" FILES ${${PROJECT_NAME}_FBO_HEADER_FILES} )
source_group( "Source Files\\FrameBuffer" FILES ${${PROJECT_NAME}_FBO_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Overlay_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Font*.hpp
)
file( GLOB ${PROJECT_NAME}_Overlay_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Font*.cpp
)
source_group( "Header Files\\Overlay" FILES ${${PROJECT_NAME}_Overlay_HEADER_FILES} )
source_group( "Source Files\\Overlay" FILES ${${PROJECT_NAME}_Overlay_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Render_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Render*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Render*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Viewport*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Ray*.hpp
)
file( GLOB ${PROJECT_NAME}_Render_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Render*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Viewport*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Ray*.cpp
)
source_group( "Header Files\\Render" FILES ${${PROJECT_NAME}_Render_HEADER_FILES} )
source_group( "Source Files\\Render" FILES ${${PROJECT_NAME}_Render_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Renderer_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.inl
)
file( GLOB ${PROJECT_NAME}_Renderer_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.cpp
)
source_group( "Header Files\\Render\\Renderer" FILES ${${PROJECT_NAME}_Renderer_HEADER_FILES} )
source_group( "Source Files\\Render\\Renderer" FILES ${${PROJECT_NAME}_Renderer_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_State_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.inl
)
file( GLOB ${PROJECT_NAME}_State_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.cpp
)
source_group( "Header Files\\State" FILES ${${PROJECT_NAME}_State_HEADER_FILES} )
source_group( "Source Files\\State" FILES ${${PROJECT_NAME}_State_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Event_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Event*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Event*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Listener*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Listener*.inl
)
file( GLOB ${PROJECT_NAME}_Event_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Event*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Listener*.cpp
)
source_group( "Header Files\\Event" FILES ${${PROJECT_NAME}_Event_HEADER_FILES} )
source_group( "Source Files\\Event" FILES ${${PROJECT_NAME}_Event_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Shader_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.inl
)
file( GLOB ${PROJECT_NAME}_Shader_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.cpp
)
source_group( "Header Files\\Shader" FILES ${${PROJECT_NAME}_Shader_HEADER_FILES} )
source_group( "Source Files\\Shader" FILES ${${PROJECT_NAME}_Shader_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Technique_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Technique*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Technique*.inl
)
file( GLOB ${PROJECT_NAME}_Technique_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Technique*.cpp
)
source_group( "Header Files\\Technique" FILES ${${PROJECT_NAME}_Technique_HEADER_FILES} )
source_group( "Source Files\\Technique" FILES ${${PROJECT_NAME}_Technique_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Material_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Material*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Material*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pass*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pass*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sampler*.hpp
)
file( GLOB ${PROJECT_NAME}_Material_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Material*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pass*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sampler*.cpp
)
source_group( "Header Files\\Material" FILES ${${PROJECT_NAME}_Material_HEADER_FILES} )
source_group( "Source Files\\Material" FILES ${${PROJECT_NAME}_Material_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Mesh_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Mesh*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Mesh*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Submesh*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Submesh*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Face*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Vertex*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Divider*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Divider*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Importer*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Importer*.inl
)
file( GLOB ${PROJECT_NAME}_Mesh_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Mesh*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Submesh*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Face*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Vertex*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Divider*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Importer*.cpp
)
source_group( "Header Files\\Mesh" FILES ${${PROJECT_NAME}_Mesh_HEADER_FILES} )
source_group( "Source Files\\Mesh" FILES ${${PROJECT_NAME}_Mesh_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_MeshCategory_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cone*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cube*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cylinder*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Icosahedron*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plane*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Projection*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sphere*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Torus*.hpp
)
file( GLOB ${PROJECT_NAME}_MeshCategory_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cone*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cube*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cylinder*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Icosahedron*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plane*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Projection*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sphere*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Torus*.cpp
)
source_group( "Header Files\\Mesh\\Generator" FILES ${${PROJECT_NAME}_MeshCategory_HEADER_FILES} )
source_group( "Source Files\\Mesh\\Generator" FILES ${${PROJECT_NAME}_MeshCategory_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Scene_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Scene*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Scene*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Camera*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Geometry*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*MovableObject*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Billboard*.hpp
)
file( GLOB ${PROJECT_NAME}_Scene_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Scene*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Camera*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Geometry*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*MovableObject*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Billboard*.cpp
)
source_group( "Header Files\\Scene" FILES ${${PROJECT_NAME}_Scene_HEADER_FILES} )
source_group( "Source Files\\Scene" FILES ${${PROJECT_NAME}_Scene_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_SceneLight_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Light*.hpp
)
file( GLOB ${PROJECT_NAME}_SceneLight_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Light*.cpp
)
source_group( "Header Files\\Scene\\Light" FILES ${${PROJECT_NAME}_SceneLight_HEADER_FILES} )
source_group( "Source Files\\Scene\\Light" FILES ${${PROJECT_NAME}_SceneLight_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Binary_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Chunk*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Chunk*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Binary*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Binary*.inl
)
file( GLOB ${PROJECT_NAME}_Binary_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Chunk*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Binary*.cpp
)
source_group( "Header Files\\Binary" FILES ${${PROJECT_NAME}_Binary_HEADER_FILES} )
source_group( "Source Files\\Binary" FILES ${${PROJECT_NAME}_Binary_SOURCE_FILES} )

file( GLOB ${PROJECT_NAME}_Miscellaneous_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Generator*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Parameter*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pattern*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Handle*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Effect*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Version*.hpp
)
file( GLOB ${PROJECT_NAME}_Miscellaneous_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Generator*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Parameter*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pattern*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Handle*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Effect*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Version*.cpp
)
source_group( "Header Files\\Miscellaneous" FILES ${${PROJECT_NAME}_Miscellaneous_HEADER_FILES} )
source_group( "Source Files\\Miscellaneous" FILES ${${PROJECT_NAME}_Miscellaneous_SOURCE_FILES} )

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)

set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
add_target_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
add_target_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )
	
# Copying core zip data files
file(
	GLOB
		CoreZipFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Data/*.zip
)
copy_target_files( ${PROJECT_NAME} "" ${CoreZipFiles} )
	
# Copying shader languages files
file(
	GLOB
		ShaderLanguagesFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Languages/*.lang
)
copy_target_files( ${PROJECT_NAME} "" ${ShaderLanguagesFiles} )

# Preparing sample scene files installation
file(
	GLOB
		SampleSceneFiles
		${CMAKE_SOURCE_DIR}/../data/Scene/*.cscn
		${CMAKE_SOURCE_DIR}/../data/Scene/*.cmtl
		${CMAKE_SOURCE_DIR}/../data/Scene/*.zip
)
install(
	FILES ${SampleSceneFiles}
	DESTINATION share/${PROJECT_NAME}/samples
	COMPONENT ${PROJECT_NAME}_Samples
)

set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
