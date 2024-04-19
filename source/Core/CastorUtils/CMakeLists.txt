project( CastorUtils )

set( unofficial-convectionkernels_DIR ${VCPKG_SHARE_DIR}/unofficial-convectionkernels )
set( gli_DIR ${VCPKG_SHARE_DIR}/gli )
find_package( unofficial-convectionkernels CONFIG REQUIRED )
find_package( gli CONFIG REQUIRED )

if ( VCPKG_TOOLCHAIN )
	set( unofficial-minizip_DIR ${VCPKG_SHARE_DIR}/unofficial-minizip )
	find_package( unofficial-minizip CONFIG REQUIRED )
	set( minizip_LIB unofficial::minizip::minizip )
else ()
	find_package( minizip CONFIG REQUIRED )
	set( minizip_LIB minizip::minizip )
endif ()

include( Doxygen )
include( AndroidNdkModules )

set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Utils" )
set( ${PROJECT_NAME}_BRIEF "Multiplatform utilities library" )
set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/data/Images/castor-dark-small.png" )
set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
set( ${PROJECT_NAME}_VERSION_MINOR 16 )
set( ${PROJECT_NAME}_VERSION_BUILD 0 )

if ( ZLIB_FOUND AND FreeType_FOUND )
	if ( "Android" STREQUAL ${PROJECTS_OS} )
		include_directories( ${CMAKE_ANDROID_NDK}/sources )
		if( NOT DEFINED ANDROID_NDK )
			set( ANDROID_NDK ${CMAKE_ANDROID_NDK} )
		endif ()
		android_ndk_import_module_cpufeatures()
		set( PLATFORM_LIBRARIES
			${PLATFORM_LIBRARIES}
			m
			EGL
			cpufeatures
		)
	elseif( APPLE )
		set( PLATFORM_LIBRARIES
			${PLATFORM_LIBRARIES}
			"-framework CoreGraphics"
		)
		set( PROJECTS_USE_PRECOMPILED_HEADERS OFF )
	elseif( WIN32 )
	else ()
		find_package( X11 )

		set( CASTOR_HAS_XINERAMA 0 )
		if ( EXISTS "/usr/lib/X11/extensions/Xinerama.h" )
			set( CASTOR_HAS_XINERAMA 1 )
			set( PLATFORM_LIBRARIES
				${PLATFORM_LIBRARIES}
				Xinerama
			)
			set( ${PROJECT_NAME}_DEFINITIONS
				${${PROJECT_NAME}_DEFINITIONS}
				CU_HasXinerama
			)
		elseif ( EXISTS "/usr/include/X11/extensions/Xinerama.h" )
			set( CASTOR_HAS_XINERAMA 1 )
			set( PLATFORM_LIBRARIES
				${PLATFORM_LIBRARIES}
				Xinerama
			)
			set( ${PROJECT_NAME}_DEFINITIONS
				${${PROJECT_NAME}_DEFINITIONS}
				CU_HasXinerama
			)
		endif ()

		set( PLATFORM_LIBRARIES
			${PLATFORM_LIBRARIES}
			${X11_LIBRARIES}
		)
	endif ()

	option( CASTOR_USE_TRACK "Enable function tracking" OFF )

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		${PROJECT_NAME}
		PARENT_SCOPE
	)

	if( PROJECTS_USE_PRECOMPILED_HEADERS )
		set( CASTOR_USE_PCH 1 )
	else()
		set( CASTOR_USE_PCH 0 )
	endif()
	if( CASTOR_USE_TRACK )
		set( CASTOR_USE_TRACK 1 )
	else()
		set( CASTOR_USE_TRACK 0 )
	endif()

	set( ${PROJECT_NAME}_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/${PROJECT_NAME}.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/${PROJECT_NAME}Prerequisites.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/${PROJECT_NAME}Prerequisites.cpp
	)
	set( ${PROJECT_NAME}_NVS_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/${PROJECT_NAME}.natvis
	)
	source_group( "Header Files" FILES ${${PROJECT_NAME}_HDR_FILES} )
	source_group( "Source Files" FILES ${${PROJECT_NAME}_SRC_FILES} )
	source_group( "Natvis Files" FILES ${${PROJECT_NAME}_NVS_FILES} )
	set( ${TARGET_NAME}_HDR_FILES
		${${TARGET_NAME}_HDR_FILES}
		${${PROJECT_NAME}_NVS_FILES}
	)

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Align/AlignModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Align/Aligned.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Align/AlignedMemoryAllocator.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Align/NonAlignedMemoryAllocator.hpp
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Align" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )

	configure_file(
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/config.hpp.in
		${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/config.hpp
		@ONLY
		NEWLINE_STYLE LF
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/config.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/BeginExternHeaderGuard.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/CompilerConfig.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/ConfigModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/EndExternHeaderGuard.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/Macros.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/MultiThreadConfig.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/PlatformConfig.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/SmartPtr.hpp
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Config" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/BinaryFile.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/File.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/Path.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/TextFile.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/TextWriter.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/ZipArchive.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/BinaryFile.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/BinaryFile.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/BinaryLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/BinaryWriter.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/DataModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Endianness.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/File.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Loader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/LoaderException.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Path.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextFile.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextFile.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextWriter.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextWriter.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Writer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/ZipArchive.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Data" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Data" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/Text/TextFont.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/Text/TextPosition.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/Text/TextSize.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextCoords.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextCoords.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextFont.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextPoint.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextPoint.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextPosition.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextQuaternion.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextQuaternion.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextRgbaColour.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextRgbaColour.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextRgbColour.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextRgbColour.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Text/TextSize.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Data\\Text" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Data\\Text" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ArrayView.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/BlockGuard.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ChangeTracked.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/DataHolder.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/DesignModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/DynamicBitset.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/DynamicBitset.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Factory.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/FlagCombination.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/GroupChangeTracked.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Named.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/NonCopyable.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/OwnedBy.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Resource.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Resource.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ResourceCache.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ResourceCacheBase.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ScopeGuard.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Signal.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Templates.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ThreadSafeSignal.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/UnicityException.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Unique.hpp
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Source Files\\Design" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )
	source_group( "Header Files\\Design" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Exception/Assertion.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Exception/Exception.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Exception/ExceptionModule.hpp
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Exception" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/FileParser/FileParser.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/FileParser/FileParserContext.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/FileParser/ParserParameterBase.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/FileParser/ParserParameterHelpers.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/FileParser/ParserParameterTypeException.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/AttributeParsersBySection.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/FileParser.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/FileParserContext.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/FileParserModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/ParserParameter.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/ParserParameter.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/ParserParameterBase.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/ParserParameterBase.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/ParserParameterHelpers.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/FileParser/ParserParameterTypeException.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\FileParser" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\FileParser" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/BoundingBox.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/BoundingSphere.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ColourComponent.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/DataImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ExrImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Font.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/FontCache.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/FreeImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/GliImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/GliImageWriter.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Glyph.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Grid.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/HdrColourComponent.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/HeightMapToNormalMap.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Image.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ImageCache.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ImageLayout.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ImageWriter.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/PixelBufferBase.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/PixelDefinitions.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/PixelFormat.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/PixelFormatExtract.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/PxBufferCompression.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Position.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Rectangle.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Size.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/StbImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/StbImageWriter.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/XpmImageLoader.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_ENM_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelFormat.enum
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/BoundingBox.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/BoundingContainer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/BoundingSphere.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/BoxFilterKernel.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/BoxFilterKernel.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Colour.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Colour.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ColourComponent.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/DataImageLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ExrImageLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Font.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/FontCache.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/FreeImageLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/GliImageLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/GliImageWriter.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Glyph.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/GraphicsModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Grid.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/HdrColourComponent.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/HeightMapToNormalMap.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Image.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ImageCache.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ImageLayout.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ImageLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ImageWriter.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/LanczosFilterKernel.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/LanczosFilterKernel.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Pixel.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Pixel.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelBuffer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelBuffer.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelBufferBase.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelComponents.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelComponents.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelConstIterator.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelDefinitions.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelDefinitions.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelFormat.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelFormat.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelIterator.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Position.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Rectangle.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/RgbaColour.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/RgbaColour.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/RgbColour.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/RgbColour.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Size.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/StbImageLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/StbImageWriter.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/UnsupportedFormatException.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/XpmImageLoader.hpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/PxBufferCompression.hpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/stb_image.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/stb_image_resize.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/stb_image_write.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/tinyexr.h
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_ENM_FILES}
	)
	source_group( "Header Files\\Graphics" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Graphics" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )
	source_group( "Source Files\\Enum" FILES ${${PROJECT_NAME}_FOLDER_ENM_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Log/Logger.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Log/LoggerConsole.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Log/LoggerImpl.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Log/LoggerInstance.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Log/ELogType.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Log/Logger.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Log/LoggerConsole.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Log/LoggerImpl.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Log/LoggerInstance.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Log/LoggerStream.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Log/LoggerStreambuf.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Log/LogModule.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Log" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Log" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Math/PlaneEquation.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Math/SphericalVertex.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Angle.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Angle.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Coords.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Coords.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Length.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Length.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Line2D.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Line3D.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Math.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/MathModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Matrix.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Matrix.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/PlaneEquation.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Point.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Point.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/PointData.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/PointOperators.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/PointOperators.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Quaternion.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Quaternion.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Range.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/RangeSequence.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/RangedValue.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Simd.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Simd.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Speed.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/SphericalVertex.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/SquareMatrix.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/SquareMatrix.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/TransformationMatrix.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/TransformationMatrix.inl
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Math" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Math" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Noise/BlueNoise.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Noise/FractalNoise.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Noise/NoiseModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Noise/PerlinNoise.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Noise/SimplexNoise.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Noise/WhiteNoise.hpp
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Noise" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/BlockTimer.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/BlockTracker.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/CpuInformations.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/Debug.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/DynamicLibrary.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/PreciseTimer.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/StringUtils.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Miscellaneous/Utils.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/BitSize.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/BlockTimer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/BlockTracker.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/CpuInformations.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Debug.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/DynamicLibrary.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Hash.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/MiscellaneousModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/PreciseTimer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/StringUtils.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/StringUtils.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Miscellaneous/Utils.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Miscellaneous" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Miscellaneous" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Multithreading/AsyncJobQueue.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Multithreading/SpinMutex.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Multithreading/ThreadPool.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Multithreading/WorkerThread.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Multithreading/AsyncJobQueue.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Multithreading/MultithreadingModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Multithreading/SpinMutex.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Multithreading/ThreadPool.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Multithreading/WorkerThread.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Multithreading" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Multithreading" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Android/AndroidAligned.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Android/AndroidCpuInformations.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Android/AndroidDebug.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Android/AndroidDynamicLibrary.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Android/AndroidFile.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Android/AndroidLoggerConsole.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Android/AndroidUtils.cpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	source_group( "Source Files\\Platform\\Android" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Linux/LinuxAligned.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Linux/LinuxCpuInformations.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Linux/LinuxDebug.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Linux/LinuxDynamicLibrary.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Linux/LinuxFile.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Linux/LinuxLoggerConsole.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Linux/LinuxUtils.cpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	source_group( "Source Files\\Platform\\Linux" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/MacOSAligned.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/MacOSCpuInformations.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/MacOSDebug.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/MacOSDynamicLibrary.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/MacOSFile.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/MacOSLoggerConsole.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/MacOSUtils.cpp
	)
	if ( APPLE )
		set( ${PROJECT_NAME}_FOLDER_SRC_FILES
			${${PROJECT_NAME}_FOLDER_SRC_FILES}
			${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/DisplaySize.mm
		)
	endif ()
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	source_group( "Source Files\\Platform\\MacOS" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Win32/Win32Aligned.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Win32/Win32CpuInformations.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Win32/Win32Debug.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Win32/Win32DynamicLibrary.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Win32/Win32File.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Win32/Win32LoggerConsole.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/Win32/Win32Utils.cpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	source_group( "Source Files\\Platform\\Win32" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/BuddyAllocator.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/BuddyAllocator.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/PoolModule.hpp
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Pool" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamBaseManipulators.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamIndentBuffer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamIndentBufferManager.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamIndentManipulators.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamModule.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamPrefixBuffer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamPrefixBufferManager.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamPrefixManipulators.hpp
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Stream" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )

	add_target_min(
		${PROJECT_NAME}
		api_dll
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.hpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/${PROJECT_NAME}Pch.cpp
	)
	add_library( castor::${PROJECT_NAME}
		ALIAS
		${PROJECT_NAME}
	)
	target_sources( ${PROJECT_NAME} 
		PRIVATE
			${CASTOR_EDITORCONFIG_FILE}
	)
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			_UNICODE
			UNICODE
			CU_UseCVTT
		PRIVATE
			${${PROJECT_NAME}_DEFINITIONS}
	)
	target_include_directories( ${PROJECT_NAME}
		PUBLIC
			$<BUILD_INTERFACE:${Vulkan_INCLUDE_DIRS}>
			$<BUILD_INTERFACE:${CASTOR_SOURCE_DIR}/include>
			$<BUILD_INTERFACE:${CASTOR_BINARY_DIR}/include>
			$<BUILD_INTERFACE:${CASTOR_SOURCE_DIR}/include/Core>
			$<BUILD_INTERFACE:${CASTOR_BINARY_DIR}/include/Core>
			$<INSTALL_INTERFACE:include>
		PRIVATE
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Core
			${CASTOR_BINARY_DIR}/source/Core
	)
	target_include_directories( ${PROJECT_NAME} SYSTEM
		PRIVATE
			${FREETYPE_INCLUDE_DIR_ft2build}
			${FREETYPE_INCLUDE_DIR_freetype2}
	)
	if ( CMAKE_CXX_COMPILER_ID MATCHES "MSVC" )
		target_compile_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/Zi>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/DEBUG>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:REF>" )
		target_link_options( ${PROJECT_NAME} PRIVATE "$<$<CONFIG:Release>:/OPT:ICF>" )
	endif ()
	target_link_libraries( ${PROJECT_NAME}
		PUBLIC
			ashes::ashespp
		PRIVATE
			${PLATFORM_LIBRARIES}
			${Freetype_LIBRARIES}
			${minizip_LIB}
			unofficial::convectionkernels::convectionkernels
			gli
	)
	if ( WIN32 )
		target_link_libraries( ${PROJECT_NAME}
			PUBLIC
				Dbghelp
		)
	endif ()
	if ( ANDROID )
		target_link_libraries( ${PROJECT_NAME}
			PUBLIC
				log
		)
	endif ()

	option( CASTOR_USE_FREEIMAGE "Look for FreeImage to use it to decode images" OFF )

	if ( CASTOR_USE_FREEIMAGE )
		if ( VCPKG_TOOLCHAIN )
			set( libpng_DIR ${VCPKG_SHARE_DIR}/libpng )
			set( libraw_DIR ${VCPKG_SHARE_DIR}/libraw )
			set( lcms2_DIR ${VCPKG_SHARE_DIR}/lcms2 )
			set( OpenJPEG_DIR ${VCPKG_SHARE_DIR}/openjpeg )
			set( WebP_DIR ${VCPKG_SHARE_DIR}/WebP )
			set( freeimage_DIR ${VCPKG_SHARE_DIR}/freeimage )
			find_package( JXR REQUIRED )
			find_package( libpng CONFIG REQUIRED )
			find_package( OpenJPEG CONFIG REQUIRED )
			find_package( WebP CONFIG REQUIRED )
			find_package( JXR REQUIRED )
			find_package( libraw CONFIG REQUIRED )
			find_package( freeimage CONFIG REQUIRED )
		else ()
			find_package( FreeImage REQUIRED )
			if ( FreeImage_FOUND )
				set( freeimage_FOUND FreeImage_FOUND )
			endif ()
		endif ()

		if ( freeimage_FOUND )
			if ( VCPKG_TOOLCHAIN AND APPLE )
				set( OpenJPEG_DIR ${VCPKG_SHARE_DIR}/openjpeg )
				set( TIFF_DIR ${VCPKG_SHARE_DIR}/tiff )
				set( JPEG_DIR ${VCPKG_SHARE_DIR}/jpeg )
				set( WebP_DIR ${VCPKG_SHARE_DIR}/webp )
				find_package( OpenJPEG CONFIG QUIET )
				find_package( TIFF CONFIG QUIET )
				find_package( JPEG CONFIG QUIET )
				find_package( WebP CONFIG QUIET )
				target_link_libraries( ${PROJECT_NAME}
					PRIVATE
						WebP::libwebpmux
				)
			endif ()
			target_compile_definitions( ${PROJECT_NAME}
				PRIVATE
					C3D_UseFreeImage
			)
			target_link_libraries( ${PROJECT_NAME}
				PRIVATE
					freeimage::FreeImage
			)
			if ( WIN32 )
				get_property( freeimageLibPathRel TARGET freeimage::FreeImage PROPERTY IMPORTED_LOCATION_RELEASE )
				set( ADDITIONAL_DEPS_REL
					${ADDITIONAL_DEPS_REL}
					${freeimageLibPathRel}
				)
				get_property( freeimageLibPathDbg TARGET freeimage::FreeImage PROPERTY IMPORTED_LOCATION_DEBUG )
				set( ADDITIONAL_DEPS_DBG
					${ADDITIONAL_DEPS_DBG}
					${freeimageLibPathDbg}
				)
			endif ()
		endif ()
	endif ()

	target_compile_options( ${PROJECT_NAME}
		PRIVATE
			$<$<CXX_COMPILER_ID:MSVC>:/bigobj>
	)
	set_target_properties( ${PROJECT_NAME}
		PROPERTIES
			FOLDER "Core/${MAIN_PROJECT_NAME}"
	)

	install(
		FILES ${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/config.hpp
		DESTINATION include/${PROJECT_NAME}
		COMPONENT Castor3D
		CONFIGURATIONS Release
	)
	install(
		FILES ${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelFormat.enum
		DESTINATION include/${PROJECT_NAME}/Graphics
		COMPONENT Castor3D
		CONFIGURATIONS Release
	)
	c3d_release_pdbs( ${PROJECT_NAME} )
	install_target_ex( ${PROJECT_NAME}
		Castor3D
		Castor3D
		api_dll
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}
	)
	set( CASTOR_INSTEXP_LIST
		${CASTOR_INSTEXP_LIST}
		${PROJECT_NAME}
		PARENT_SCOPE
	)
	set( PLATFORM_LIBRARIES
		${PLATFORM_LIBRARIES}
		PARENT_SCOPE
	)
	if ( WIN32 )
		set( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
		set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin/$<$<CONFIG:Debug>:Debug/> )
		if ( VCPKG_TOOLCHAIN )
			find_dll_config( zlibLibPathRel ${ZLIB_LIBRARY_RELEASE} "1.dll" )
			get_property( freetypeLibPathRel TARGET freetype PROPERTY IMPORTED_LOCATION_RELEASE )
			set( ADDITIONAL_DEPS_REL
				${ADDITIONAL_DEPS_REL}
				${zlibLibPathRel}
				${freetypeLibPathRel}
			)
			find_dll_config( zlibLibPathDbg ${ZLIB_LIBRARY_DEBUG} "1.dll" )
			get_property( freetypeLibPathDbg TARGET freetype PROPERTY IMPORTED_LOCATION_DEBUG )
			set( ADDITIONAL_DEPS_DBG
				${ADDITIONAL_DEPS_DBG}
				${zlibLibPathDbg}
				${freetypeLibPathDbg}
			)
		elseif ()
			if ( ${PROJECTS_PLATFORM} STREQUAL "x86" )
				get_filename_component( ZLibLibPath ${ZLIB_LIBRARY} PATH )
				set( ZLibLibPath ${ZLibLibPath}/zlib1.dll )
				copy_dll( ${PROJECT_NAME} ${ZLibLibPath} ${ZLibLibPath} )
			elseif ( ZLIB_LIBRARY_RELEASE OR ZLIB_LIBRARY_DEBUG )
				if ( ZLIB_LIBRARY_RELEASE AND ZLIB_LIBRARY_DEBUG )
					copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_DEBUG} ${ZLIB_LIBRARY_RELEASE} )
				elseif ( ZLIB_LIBRARY_RELEASE )
					copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_RELEASE} ${ZLIB_LIBRARY_RELEASE} )
				else ()
					copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY_DEBUG} ${ZLIB_LIBRARY_DEBUG} )
				endif ()
			else ()
				copy_dll( ${PROJECT_NAME} ${ZLIB_LIBRARY} ${ZLIB_LIBRARY} )
			endif ()
		endif ()
		set( ADDITIONAL_DEPS_REL
			${ADDITIONAL_DEPS_REL}
			PARENT_SCOPE
		)
		set( ADDITIONAL_DEPS_DBG
			${ADDITIONAL_DEPS_DBG}
			PARENT_SCOPE
		)
	endif ()

	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )

	set( Build "yes (version ${${PROJECT_NAME}_VERSION_MAJOR}.${${PROJECT_NAME}_VERSION_MINOR}.${${PROJECT_NAME}_VERSION_BUILD})" PARENT_SCOPE )
else ()
	set( msg "no (missing :" )
	if( NOT FreeType_FOUND )
		set( msg "${msg} Freetype" )
	endif()
	if( NOT ZLIB_FOUND )
		set( msg "${msg} Zlib" )
	endif()
	set( Build "${msg})" PARENT_SCOPE )
	set( Error TRUE PARENT_SCOPE )
endif ()
