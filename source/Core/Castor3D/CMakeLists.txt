project( Castor3D )

set( C3D_VERSION_MAJOR	0 )
set( C3D_VERSION_MINOR	7 )
set( C3D_VERSION_BUILD	1 )

configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/Src/RequiredVersion.hpp.in
	${CMAKE_CURRENT_BINARY_DIR}/Src/RequiredVersion.hpp
	NEWLINE_STYLE LF
)

set( CastorBinsDependencies
	${CastorBinsDependencies}
	Castor3D
	PARENT_SCOPE
)

include_directories( ${CMAKE_CURRENT_BINARY_DIR} )
include_directories( ${CMAKE_CURRENT_BINARY_DIR}/Src )
include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )

add_target(
	Castor3D
	api_dll
	"CastorUtils"
	"${CastorMinLibraries}"
	"Castor3DPch.hpp"
	"Castor3DPch.cpp"
)

file( GLOB CASTOR3D_Plugin_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plugin*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plugin*.inl
)
file( GLOB CASTOR3D_Plugin_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plugin*.cpp
)
source_group( "Header Files\\Plugin" FILES ${CASTOR3D_Plugin_HEADER_FILES} )
source_group( "Source Files\\Plugin" FILES ${CASTOR3D_Plugin_SOURCE_FILES} )

file( GLOB CASTOR3D_Animation_HEADER_FILES
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
file( GLOB CASTOR3D_Animation_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Anim*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Bone*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Moving*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Skeleton*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/KeyFrame*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*Interpolator*.cpp
)
source_group( "Header Files\\Animation" FILES ${CASTOR3D_Animation_HEADER_FILES} )
source_group( "Source Files\\Animation" FILES ${CASTOR3D_Animation_SOURCE_FILES} )

file( GLOB CASTOR3D_VBO_HEADER_FILES
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
file( GLOB CASTOR3D_VBO_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/Buffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/CpuBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/GpuBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/GeometryBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/IndexBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/MatrixBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/VertexBuffer*.cpp
)
source_group( "Header Files\\VertexBuffer" FILES ${CASTOR3D_VBO_HEADER_FILES} )
source_group( "Source Files\\VertexBuffer" FILES ${CASTOR3D_VBO_SOURCE_FILES} )

file( GLOB CASTOR3D_FBO_HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.inl
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.inl
)
file( GLOB CASTOR3D_FBO_SOURCE_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.cpp
)
source_group( "Header Files\\FrameBuffer" FILES ${CASTOR3D_FBO_HEADER_FILES} )
source_group( "Source Files\\FrameBuffer" FILES ${CASTOR3D_FBO_SOURCE_FILES} )

file( GLOB CASTOR3D_Render_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderable*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderable*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderTarget*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderTarget*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderWindow*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderWindow*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Viewport*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Ray*.hpp
)
file( GLOB CASTOR3D_Render_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderable*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderTarget*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderWindow*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Viewport*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Ray*.cpp
)
source_group( "Header Files\\Render" FILES ${CASTOR3D_Render_HEADER_FILES} )
source_group( "Source Files\\Render" FILES ${CASTOR3D_Render_SOURCE_FILES} )

file( GLOB CASTOR3D_Renderer_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.inl
)
file( GLOB CASTOR3D_Renderer_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.cpp
)
source_group( "Header Files\\Render\\Renderer" FILES ${CASTOR3D_Renderer_HEADER_FILES} )
source_group( "Source Files\\Render\\Renderer" FILES ${CASTOR3D_Renderer_SOURCE_FILES} )

file( GLOB CASTOR3D_State_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.inl
)
file( GLOB CASTOR3D_State_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.cpp
)
source_group( "Header Files\\State" FILES ${CASTOR3D_State_HEADER_FILES} )
source_group( "Source Files\\State" FILES ${CASTOR3D_State_SOURCE_FILES} )

file( GLOB CASTOR3D_Overlay_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Font*.hpp
)
file( GLOB CASTOR3D_Overlay_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Font*.cpp
)
source_group( "Header Files\\Overlay" FILES ${CASTOR3D_Overlay_HEADER_FILES} )
source_group( "Source Files\\Overlay" FILES ${CASTOR3D_Overlay_SOURCE_FILES} )

file( GLOB CASTOR3D_Event_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Event*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Event*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Listener*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Listener*.inl
)
file( GLOB CASTOR3D_Event_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Event*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Listener*.cpp
)
source_group( "Header Files\\Event" FILES ${CASTOR3D_Event_HEADER_FILES} )
source_group( "Source Files\\Event" FILES ${CASTOR3D_Event_SOURCE_FILES} )

file( GLOB CASTOR3D_Shader_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.inl
)
file( GLOB CASTOR3D_Shader_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.cpp
)
source_group( "Header Files\\Shader" FILES ${CASTOR3D_Shader_HEADER_FILES} )
source_group( "Source Files\\Shader" FILES ${CASTOR3D_Shader_SOURCE_FILES} )

file( GLOB CASTOR3D_Technique_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Technique*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Technique*.inl
)
file( GLOB CASTOR3D_Technique_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Technique*.cpp
)
source_group( "Header Files\\Technique" FILES ${CASTOR3D_Technique_HEADER_FILES} )
source_group( "Source Files\\Technique" FILES ${CASTOR3D_Technique_SOURCE_FILES} )

file( GLOB CASTOR3D_Material_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Material*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Material*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pass*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pass*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sampler*.hpp
)
file( GLOB CASTOR3D_Material_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Material*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pass*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sampler*.cpp
)
source_group( "Header Files\\Material" FILES ${CASTOR3D_Material_HEADER_FILES} )
source_group( "Source Files\\Material" FILES ${CASTOR3D_Material_SOURCE_FILES} )

file( GLOB CASTOR3D_Mesh_HEADER_FILES
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
file( GLOB CASTOR3D_Mesh_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Mesh*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Submesh*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Face*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Vertex*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Divider*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Importer*.cpp
)
source_group( "Header Files\\Mesh" FILES ${CASTOR3D_Mesh_HEADER_FILES} )
source_group( "Source Files\\Mesh" FILES ${CASTOR3D_Mesh_SOURCE_FILES} )

file( GLOB CASTOR3D_MeshCategory_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cone*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cube*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cylinder*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Icosahedron*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plane*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Projection*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sphere*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Torus*.hpp
)
file( GLOB CASTOR3D_MeshCategory_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cone*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cube*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Cylinder*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Icosahedron*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Plane*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Projection*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sphere*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Torus*.cpp
)
source_group( "Header Files\\Mesh\\Category" FILES ${CASTOR3D_MeshCategory_HEADER_FILES} )
source_group( "Source Files\\Mesh\\Category" FILES ${CASTOR3D_MeshCategory_SOURCE_FILES} )

file( GLOB CASTOR3D_Scene_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Scene*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Scene*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Camera*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Geometry*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*MovableObject*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Billboard*.hpp
)
file( GLOB CASTOR3D_Scene_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Scene*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Camera*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Geometry*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*MovableObject*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Billboard*.cpp
)
source_group( "Header Files\\Scene" FILES ${CASTOR3D_Scene_HEADER_FILES} )
source_group( "Source Files\\Scene" FILES ${CASTOR3D_Scene_SOURCE_FILES} )

file( GLOB CASTOR3D_SceneLight_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Light*.hpp
)
file( GLOB CASTOR3D_SceneLight_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Light*.cpp
)
source_group( "Header Files\\Scene\\Light" FILES ${CASTOR3D_SceneLight_HEADER_FILES} )
source_group( "Source Files\\Scene\\Light" FILES ${CASTOR3D_SceneLight_SOURCE_FILES} )

file( GLOB CASTOR3D_Binary_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Chunk*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Chunk*.inl
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Binary*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Binary*.inl
)
file( GLOB CASTOR3D_Binary_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Chunk*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Binary*.cpp
)
source_group( "Header Files\\Binary" FILES ${CASTOR3D_Binary_HEADER_FILES} )
source_group( "Source Files\\Binary" FILES ${CASTOR3D_Binary_SOURCE_FILES} )

file( GLOB CASTOR3D_Miscellaneous_HEADER_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Generator*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Parameter*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pattern*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Handle*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Effect*.hpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Version*.hpp
)
file( GLOB CASTOR3D_Miscellaneous_SOURCE_FILES
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Generator*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Parameter*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pattern*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Handle*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Effect*.cpp
	${CMAKE_CURRENT_SOURCE_DIR}/Src/*Version*.cpp
)
source_group( "Header Files\\Miscellaneous" FILES ${CASTOR3D_Miscellaneous_HEADER_FILES} )
source_group( "Source Files\\Miscellaneous" FILES ${CASTOR3D_Miscellaneous_SOURCE_FILES} )

set( CastorMinLibraries
	${CastorMinLibraries}
	Castor3D
	PARENT_SCOPE
)

set_property( TARGET Castor3D PROPERTY FOLDER "Core" )
add_target_astyle( Castor3D ".h;.hpp;.inl;.cpp" )
add_target_doc( Castor3D "French" "*.h *.hpp" )
add_target_doc( Castor3D "English" "*.h *.hpp" )
	
# Copying shader languages files
file(
	GLOB
		ShaderLanguagesFiles
		${CMAKE_CURRENT_SOURCE_DIR}/Src/Languages/*.lang
)

file(
	COPY
		${ShaderLanguagesFiles}
	DESTINATION
		${PROJECTS_BINARIES_OUTPUT_DIR_RELEASE}/share/Castor3D
)

file(
	COPY
		${ShaderLanguagesFiles}
	DESTINATION
		${PROJECTS_BINARIES_OUTPUT_DIR_DEBUG}/share/Castor3D
)

# Preparing shader languages files installation
install(
	FILES ${ShaderLanguagesFiles}
	DESTINATION share/Castor3D
	COMPONENT Castor3D
)

# Preparing sample scene files installation
file(
	GLOB
		SampleSceneFiles
		${CMAKE_SOURCE_DIR}/../data/Scene/*.cscn
		${CMAKE_SOURCE_DIR}/../data/Scene/*.cmtl
)
install(
	FILES ${SampleSceneFiles}
	DESTINATION share/Castor3D/samples
	COMPONENT Castor3D_Samples
)
# And their data
file( GLOB SUB_DIRS ${CMAKE_SOURCE_DIR}/../data/Scene/* )
foreach( DIR ${SUB_DIRS} )
	if( IS_DIRECTORY ${DIR} )
		get_filename_component( DIR ${DIR} NAME )
		file(
			GLOB
				DataFiles
				${CMAKE_SOURCE_DIR}/../data/Scene/${DIR}/*.*
	 )
		install(
			FILES ${DataFiles}
			DESTINATION share/Castor3D/samples/${DIR}
			COMPONENT Castor3D_Samples
	 )
	endif()
endforeach()

set( Build "yes (version ${C3D_VERSION_MAJOR}.${C3D_VERSION_MINOR}.${C3D_VERSION_BUILD})" PARENT_SCOPE )