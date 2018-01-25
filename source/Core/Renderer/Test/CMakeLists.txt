find_package( wxWidgets COMPONENTS core base )
if( NOT WIN32 )
	find_package( GTK2 )
else()
	set( GTK2_FOUND TRUE )
endif()

if( wxWidgets_FOUND AND GTK2_FOUND )
	string( COMPARE EQUAL "${wxWidgets_USE_DEBUG}" "ON" IsWxDebug )

	add_definitions(
		-D_FILE_OFFSET_BITS=64
		-D_LARGE_FILES
		-D_UNICODE
	)

	if( NOT WIN32 )
		add_definitions(
			-D__WXGTK__
		)
		set( BinaryLibraries
			${BinaryLibraries}
			dl
		)
	endif()

	if( IsWxDebug )
		add_definitions(
			-D__WXDEBUG__
		)
	endif()

	include_directories(
		${CMAKE_SOURCE_DIR}/Core
		${CMAKE_BINARY_DIR}/Renderer/Renderer/Src
		${CMAKE_SOURCE_DIR}/Renderer/Renderer/Src
		${wxWidgets_INCLUDE_DIRS}
		${GTK2_INCLUDE_DIRS}
	)

	add_subdirectory( 00-Common )
	add_subdirectory( 01-DeviceCreation )
	add_subdirectory( 02-SwapChainCreation )
	add_subdirectory( 03-VertexLayout )
	add_subdirectory( 04-StagingBuffer )
	add_subdirectory( 05-Texture2D )
	add_subdirectory( 06-UniformBuffers )
	add_subdirectory( 07-Texture1D )
	add_subdirectory( 08-TextureBuffer )
	add_subdirectory( 09-FrameBuffer )
	add_subdirectory( 10-SpinningCube )
	add_subdirectory( 11-Texture3D )
	add_subdirectory( 12-TextureCube )
endif ()
