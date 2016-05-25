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
		${PROJECT_NAME}
		PARENT_SCOPE
	)
	
	set( ${PROJECT_NAME}_DESCRIPTION "OpenGL Renderer" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 9 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )
	
	include_directories( ${CMAKE_SOURCE_DIR}/Core/CastorUtils/Src )
	include_directories( ${CMAKE_SOURCE_DIR}/Core/GlslWriter/Src )
	include_directories( ${CMAKE_SOURCE_DIR}/Core/Castor3D/Src )
	include_directories( ${CMAKE_SOURCE_DIR}/Plugins/Renderers/GlRenderSystem/Src )
	include_directories( ${CMAKE_BINARY_DIR}/Core/CastorUtils/Src )
	
	add_target(
		${PROJECT_NAME}
		plugin
		"Castor3D"
		"${CastorMinLibraries};${OpenGlLibraries}"
		"GlRenderSystemPch.hpp"
		"GlRenderSystemPch.cpp"
	)

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

	file( GLOB ${PROJECT_NAME}_VBO_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.inl
	)
	file( GLOB ${PROJECT_NAME}_VBO_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*BufferObject*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*GeometryBuffer*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Attribute*.cpp
	)
	source_group( "Header Files\\VertexBuffer" FILES ${${PROJECT_NAME}_VBO_HEADER_FILES} )
	source_group( "Source Files\\VertexBuffer" FILES ${${PROJECT_NAME}_VBO_SOURCE_FILES} )

	file( GLOB ${PROJECT_NAME}_Overlay_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.inl
	)
	file( GLOB ${PROJECT_NAME}_Overlay_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Overlay*.cpp
	)
	source_group( "Header Files\\Overlay" FILES ${${PROJECT_NAME}_Overlay_HEADER_FILES} )
	source_group( "Source Files\\Overlay" FILES ${${PROJECT_NAME}_Overlay_SOURCE_FILES} )

	file( GLOB ${PROJECT_NAME}_Render_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderTarget.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderTarget.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderWindow.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderWindow.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.inl
	)
	file( GLOB ${PROJECT_NAME}_Render_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderSystem.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderTarget.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*RenderWindow.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pipeline*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Context*.cpp
	)
	source_group( "Header Files\\Render" FILES ${${PROJECT_NAME}_Render_HEADER_FILES} )
	source_group( "Source Files\\Render" FILES ${${PROJECT_NAME}_Render_SOURCE_FILES} )

	file( GLOB ${PROJECT_NAME}_Shader_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Program*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Program*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.inl
	)
	file( GLOB ${PROJECT_NAME}_Shader_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Shader*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Program*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Variable*.cpp
	)
	source_group( "Header Files\\Shader" FILES ${${PROJECT_NAME}_Shader_HEADER_FILES} )
	source_group( "Source Files\\Shader" FILES ${${PROJECT_NAME}_Shader_SOURCE_FILES} )

	file( GLOB ${PROJECT_NAME}_Texture_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sampler*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sampler*.inl
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.inl
	)
	file( GLOB ${PROJECT_NAME}_Texture_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Texture*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Sampler*.cpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Pixel*.cpp
	)
	source_group( "Header Files\\Texture" FILES ${${PROJECT_NAME}_Texture_HEADER_FILES} )
	source_group( "Source Files\\Texture" FILES ${${PROJECT_NAME}_Texture_SOURCE_FILES} )

	file( GLOB ${PROJECT_NAME}_State_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.inl
	)
	file( GLOB ${PROJECT_NAME}_State_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*State*.cpp
	)
	source_group( "Header Files\\State" FILES ${${PROJECT_NAME}_State_HEADER_FILES} )
	source_group( "Source Files\\State" FILES ${${PROJECT_NAME}_State_SOURCE_FILES} )

	file( GLOB ${PROJECT_NAME}_Scene_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Billboard*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Billboard*.inl
	)
	file( GLOB ${PROJECT_NAME}_Scene_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Billboard*.cpp
	)
	source_group( "Header Files\\Scene" FILES ${${PROJECT_NAME}_Scene_HEADER_FILES} )
	source_group( "Source Files\\Scene" FILES ${${PROJECT_NAME}_Scene_SOURCE_FILES} )

	file( GLOB ${PROJECT_NAME}_Buffer_HEADER_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.hpp
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.inl
	)
	file( GLOB ${PROJECT_NAME}_Buffer_SOURCE_FILES
		${CMAKE_CURRENT_SOURCE_DIR}/Src/*Buffer*.cpp
	)
	source_group( "Header Files\\Buffer" FILES ${${PROJECT_NAME}_Buffer_HEADER_FILES} )
	source_group( "Source Files\\Buffer" FILES ${${PROJECT_NAME}_Buffer_SOURCE_FILES} )
	
	set( Ogl "yes" PARENT_SCOPE )
	
	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Plugins/Renderers" )
	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
else()
	set( Build "no (OpenGL libraries not found)" PARENT_SCOPE )
endif()
