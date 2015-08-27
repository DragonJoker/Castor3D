project( GlRenderSystem )

find_package( OpenGL )

if( OPENGL_FOUND )
	set( OpenGlLibraries "" )
	foreach( Lib ${OPENGL_gl_LIBRARY} )
		if( OpenGlLibraries )
			set( OpenGlLibraries "${OpenGlLibraries}|${Lib}" )
		else()
			set( OpenGlLibraries "${Lib}" )
		endif()
	endforeach()
	
	SET( CastorBinsDependencies 
		${CastorBinsDependencies}
		GlRenderSystem
		PARENT_SCOPE
	)
	
	set( GL_VERSION_MAJOR	0 )
	set( GL_VERSION_MINOR	7 )
	set( GL_VERSION_BUILD	0 )
	
	include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
	include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
	include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
	
	add_target(
		GlRenderSystem
		plugin
		"Castor3D"
		"${CastorMinLibraries};${OpenGlLibraries}"
		"GlRenderSystemPch.hpp"
		"GlRenderSystemPch.cpp"
	)

	file( GLOB GLRenderSystem_FBO_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.inl
	)
	file( GLOB GLRenderSystem_FBO_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*FrameBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attachment*.cpp
	)
	source_group( "Header Files\\FrameBuffer" FILES ${GLRenderSystem_FBO_HEADER_FILES} )
	source_group( "Source Files\\FrameBuffer" FILES ${GLRenderSystem_FBO_SOURCE_FILES} )

	file( GLOB GLRenderSystem_VBO_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.inl
	)
	file( GLOB GLRenderSystem_VBO_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Array*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.cpp
	)
	source_group( "Header Files\\VertexBuffer" FILES ${GLRenderSystem_VBO_HEADER_FILES} )
	source_group( "Source Files\\VertexBuffer" FILES ${GLRenderSystem_VBO_SOURCE_FILES} )

	file( GLOB GLRenderSystem_Render_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.hpp
	)
	file( GLOB GLRenderSystem_Render_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Ray*.cpp
	)
	source_group( "Header Files\\Render" FILES ${GLRenderSystem_Render_HEADER_FILES} )
	source_group( "Source Files\\Render" FILES ${GLRenderSystem_Render_SOURCE_FILES} )

	file( GLOB GLRenderSystem_Renderer_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.inl
	)
	file( GLOB GLRenderSystem_Renderer_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Renderer*.cpp
	)
	source_group( "Header Files\\Render\\Renderer" FILES ${GLRenderSystem_Renderer_HEADER_FILES} )
	source_group( "Source Files\\Render\\Renderer" FILES ${GLRenderSystem_Renderer_SOURCE_FILES} )

	file( GLOB GLRenderSystem_Shader_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.inl
	)
	file( GLOB GLRenderSystem_Shader_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.cpp
	)
	source_group( "Header Files\\Shader" FILES ${GLRenderSystem_Shader_HEADER_FILES} )
	source_group( "Source Files\\Shader" FILES ${GLRenderSystem_Shader_SOURCE_FILES} )

	file( GLOB GLRenderSystem_Texture_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.inl
	)
	file( GLOB GLRenderSystem_Texture_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.cpp
	)
	source_group( "Header Files\\Texture" FILES ${GLRenderSystem_Texture_HEADER_FILES} )
	source_group( "Source Files\\Texture" FILES ${GLRenderSystem_Texture_SOURCE_FILES} )

	file( GLOB GLRenderSystem_State_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.inl
	)
	file( GLOB GLRenderSystem_State_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.cpp
	)
	source_group( "Header Files\\State" FILES ${GLRenderSystem_State_HEADER_FILES} )
	source_group( "Source Files\\State" FILES ${GLRenderSystem_State_SOURCE_FILES} )

	file( GLOB GLRenderSystem_Buffer_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.inl
	)
	file( GLOB GLRenderSystem_Buffer_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.cpp
	)
	source_group( "Header Files\\Buffer" FILES ${GLRenderSystem_Buffer_HEADER_FILES} )
	source_group( "Source Files\\Buffer" FILES ${GLRenderSystem_Buffer_SOURCE_FILES} )
	
	set( Ogl "yes" PARENT_SCOPE )
	
	set_property( TARGET GlRenderSystem PROPERTY FOLDER "Renderers" )
	set( Build "yes (version ${GL_VERSION_MAJOR}.${GL_VERSION_MINOR}.${GL_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( GlRenderSystem ".h;.hpp;.inl;.cpp" )
else()
	set( Build "no (OpenGL libraries not found)" PARENT_SCOPE )
endif()
