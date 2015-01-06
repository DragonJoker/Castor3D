project( Dx11RenderSystem )

find_package( Direct3D9 )
find_package( Direct3D11 )

if( DIRECT3D11_FOUND AND DIRECT3D9_FOUND )
	message( STATUS "+ Found Direct3D11" )

    include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
    include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
    include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
	include_directories( ${D3D11_INCLUDE_DIR} )

	set( D3d9Libraries "" )
	foreach( Lib ${D3D9_LIBRARIES} )
		if( D3d9Libraries )
			set( D3d9Libraries "${D3d9Libraries}|${Lib}" )
		else()
			set( D3d9Libraries "${Lib}" )
		endif()
	endforeach()

	set( D3d11Libraries "" )
	foreach( Lib ${D3D11_LIBRARIES} )
		if( D3d11Libraries )
			set( D3d11Libraries "${D3d11Libraries}|${Lib}" )
		else()
			set( D3d11Libraries "${Lib}" )
		endif()
	endforeach()

	SET( CastorBinsDependencies 
		${CastorBinsDependencies}
		Dx11RenderSystem
		PARENT_SCOPE
	)

    set( DX11_VERSION_MAJOR	0 )
    set( DX11_VERSION_MINOR	7 )
    set( DX11_VERSION_BUILD	0 )

	add_target(
		Dx11RenderSystem
		plugin
		"Castor3D"
		"${CastorMinLibraries};${D3d9Libraries};${D3d11Libraries}"
		"Dx11RenderSystemPch.hpp"
		"Dx11RenderSystemPch.cpp"
	)

	file( GLOB DX11RenderSystem_FBO_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.inl
	)
	file( GLOB DX11RenderSystem_FBO_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.cpp
	)
	source_group( "Header Files\\FrameBuffer" FILES ${DX11RenderSystem_FBO_HEADER_FILES} )
	source_group( "Source Files\\FrameBuffer" FILES ${DX11RenderSystem_FBO_SOURCE_FILES} )

	file( GLOB DX11RenderSystem_VBO_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.inl
	)
	file( GLOB DX11RenderSystem_VBO_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.cpp
	)
	source_group( "Header Files\\VertexBuffer" FILES ${DX11RenderSystem_VBO_HEADER_FILES} )
	source_group( "Source Files\\VertexBuffer" FILES ${DX11RenderSystem_VBO_SOURCE_FILES} )

	file( GLOB DX11RenderSystem_Render_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.hpp
	)
	file( GLOB DX11RenderSystem_Render_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Ray*.cpp
	)
	source_group( "Header Files\\Render" FILES ${DX11RenderSystem_Render_HEADER_FILES} )
	source_group( "Source Files\\Render" FILES ${DX11RenderSystem_Render_SOURCE_FILES} )

	file( GLOB DX11RenderSystem_Renderer_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.inl
	)
	file( GLOB DX11RenderSystem_Renderer_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.cpp
	)
	source_group( "Header Files\\Render\\Renderer" FILES ${DX11RenderSystem_Renderer_HEADER_FILES} )
	source_group( "Source Files\\Render\\Renderer" FILES ${DX11RenderSystem_Renderer_SOURCE_FILES} )

	file( GLOB DX11RenderSystem_Shader_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.inl
	)
	file( GLOB DX11RenderSystem_Shader_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.cpp
	)
	source_group( "Header Files\\Shader" FILES ${DX11RenderSystem_Shader_HEADER_FILES} )
	source_group( "Source Files\\Shader" FILES ${DX11RenderSystem_Shader_SOURCE_FILES} )

	file( GLOB DX11RenderSystem_Texture_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.inl
	)
	file( GLOB DX11RenderSystem_Texture_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.cpp
	)
	source_group( "Header Files\\Texture" FILES ${DX11RenderSystem_Texture_HEADER_FILES} )
	source_group( "Source Files\\Texture" FILES ${DX11RenderSystem_Texture_SOURCE_FILES} )

	file( GLOB DX11RenderSystem_State_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.inl
	)
	file( GLOB DX11RenderSystem_State_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.cpp
	)
	source_group( "Header Files\\State" FILES ${DX11RenderSystem_State_HEADER_FILES} )
	source_group( "Source Files\\State" FILES ${DX11RenderSystem_State_SOURCE_FILES} )

	file( GLOB DX11RenderSystem_Buffer_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.inl
	)
	file( GLOB DX11RenderSystem_Buffer_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.cpp
	)
	source_group( "Header Files\\Buffer" FILES ${DX11RenderSystem_Buffer_HEADER_FILES} )
	source_group( "Source Files\\Buffer" FILES ${DX11RenderSystem_Buffer_SOURCE_FILES} )

	set_property( TARGET Dx11RenderSystem PROPERTY FOLDER "Renderers" )
	add_target_astyle( Dx11RenderSystem ".h;.hpp;.inl;.cpp" )
    
    if ( CMAKE_COMPILER_IS_GNUCXX )
        add_target_compilation_flags( Dx11RenderSystem "-fcheck-new" )
    endif ()
    
	set( Build "yes (version ${DX11_VERSION_MAJOR}.${DX11_VERSION_MINOR}.${DX11_VERSION_BUILD})" PARENT_SCOPE )
else()
	set( Build "no (Direct3D11 SDK not found)" PARENT_SCOPE )
endif()
