project( Dx9RenderSystem )

find_package( Direct3D9 )

if( DIRECT3D9_FOUND )
	message( STATUS "+ Found Direct3D9" )

    include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
    include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
    include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
	include_directories( ${D3D9_INCLUDE_DIR} )

	set( D3d9Libraries "")
	foreach( Lib ${D3D9_LIBRARIES} )
		if( D3d9Libraries )
			set( D3d9Libraries "${D3d9Libraries}|${Lib}" )
		else()
			set( D3d9Libraries "${Lib}" )
		endif()
	endforeach()

	set( CastorBinsDependencies 
		${CastorBinsDependencies}
		Dx9RenderSystem
		PARENT_SCOPE
	)

    set( DX9_VERSION_MAJOR	0 )
    set( DX9_VERSION_MINOR	6 )
    set( DX9_VERSION_BUILD	5 )

	add_target(
		Dx9RenderSystem
		plugin
		"Castor3D"
		"${CastorMinLibraries};${D3d9Libraries}"
		"Dx9RenderSystemPch.hpp"
		"Dx9RenderSystemPch.cpp"
	)

	file( GLOB DX9RenderSystem_FBO_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.inl
	)
	file( GLOB DX9RenderSystem_FBO_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.cpp
	)
	source_group( "Header Files\\FrameBuffer" FILES ${DX9RenderSystem_FBO_HEADER_FILES} )
	source_group( "Source Files\\FrameBuffer" FILES ${DX9RenderSystem_FBO_SOURCE_FILES} )

	file( GLOB DX9RenderSystem_VBO_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.inl
	)
	file( GLOB DX9RenderSystem_VBO_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.cpp
	)
	source_group( "Header Files\\VertexBuffer" FILES ${DX9RenderSystem_VBO_HEADER_FILES} )
	source_group( "Source Files\\VertexBuffer" FILES ${DX9RenderSystem_VBO_SOURCE_FILES} )

	file( GLOB DX9RenderSystem_Render_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.hpp
	)
	file( GLOB DX9RenderSystem_Render_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Ray*.cpp
	)
	source_group( "Header Files\\Render" FILES ${DX9RenderSystem_Render_HEADER_FILES} )
	source_group( "Source Files\\Render" FILES ${DX9RenderSystem_Render_SOURCE_FILES} )

	file( GLOB DX9RenderSystem_Renderer_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.inl
	)
	file( GLOB DX9RenderSystem_Renderer_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.cpp
	)
	source_group( "Header Files\\Render\\Renderer" FILES ${DX9RenderSystem_Renderer_HEADER_FILES} )
	source_group( "Source Files\\Render\\Renderer" FILES ${DX9RenderSystem_Renderer_SOURCE_FILES} )

	file( GLOB DX9RenderSystem_Shader_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.inl
	)
	file( GLOB DX9RenderSystem_Shader_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.cpp
	)
	source_group( "Header Files\\Shader" FILES ${DX9RenderSystem_Shader_HEADER_FILES} )
	source_group( "Source Files\\Shader" FILES ${DX9RenderSystem_Shader_SOURCE_FILES} )

	file( GLOB DX9RenderSystem_Texture_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.inl
	)
	file( GLOB DX9RenderSystem_Texture_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.cpp
	)
	source_group( "Header Files\\Texture" FILES ${DX9RenderSystem_Texture_HEADER_FILES} )
	source_group( "Source Files\\Texture" FILES ${DX9RenderSystem_Texture_SOURCE_FILES} )

	file( GLOB DX9RenderSystem_State_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.inl
	)
	file( GLOB DX9RenderSystem_State_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.cpp
	)
	source_group( "Header Files\\State" FILES ${DX9RenderSystem_State_HEADER_FILES} )
	source_group( "Source Files\\State" FILES ${DX9RenderSystem_State_SOURCE_FILES} )

	file( GLOB DX9RenderSystem_Buffer_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.inl
	)
	file( GLOB DX9RenderSystem_Buffer_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.cpp
	)
	source_group( "Header Files\\Buffer" FILES ${DX9RenderSystem_Buffer_HEADER_FILES} )
	source_group( "Source Files\\Buffer" FILES ${DX9RenderSystem_Buffer_SOURCE_FILES} )

	set_property( TARGET Dx9RenderSystem PROPERTY FOLDER "Renderers" )
	add_target_astyle( Dx9RenderSystem ".h;.hpp;.inl;.cpp" )
    
    if ( CMAKE_COMPILER_IS_GNUCXX )
        add_target_compilation_flags( Dx9RenderSystem "-fcheck-new" )
    endif ()
    
	set( Build "yes (version ${DX9_VERSION_MAJOR}.${DX9_VERSION_MINOR}.${DX9_VERSION_BUILD})" PARENT_SCOPE )
else()
	set( Build "no (Direct3D9 SDK not found)" PARENT_SCOPE )
endif()
