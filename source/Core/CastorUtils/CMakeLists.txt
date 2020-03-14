project( CastorUtils )

include( Doxygen )
include( AndroidNdkModules )

if ( CASTOR_USE_SYSTEM_LIBS OR APPLE )
	if ( VCPKG_TOOLCHAIN )
		find_package( freetype CONFIG )
		set( Freetype_LIBRARIES freetype )
		set( FreeType_FOUND ( freetype_FOUND OR FREETYPE_FOUND ) )
	else ()
		find_package( Freetype )
		set( Freetype_LIBRARIES Freetype::Freetype )
	endif ()
else ()
	set( FreeType_FOUND ON )
	set( Freetype_LIBRARIES Freetype::Freetype )
endif ()
if ( FreeType_FOUND OR FREETYPE_FOUND )
	set( FreeType_FOUND FREETYPE_FOUND )
endif ()
if ( CASTOR_USE_SYSTEM_LIBS )
	find_package( ZLIB 1.2.7 )
else ()
	set( ZLIB_FOUND ON )
endif ()

if ( ZLIB_FOUND AND FreeType_FOUND )
	if ( ANDROID )
		include_directories( ${ANDROID_NDK}/sources )
		android_ndk_import_module_cpufeatures()
		set( PLATFORM_LIBRARIES
			m
			EGL
			cpufeatures
		)
	elseif( APPLE )
		set( PLATFORM_LIBRARIES
			"-framework CoreGraphics"
		)
	elseif( WIN32 )
	else ()
		find_package( X11 )

		set( CASTOR_HAS_XINERAMA 0 )
		if ( EXISTS "/usr/lib/X11/extensions/Xinerama.h" )
			set( CASTOR_HAS_XINERAMA 1 )
			set( PLATFORM_LIBRARIES
				Xinerama
			)
			set( ${PROJECT_NAME}_DEFINITIONS
				${${PROJECT_NAME}_DEFINITIONS}
				CU_HasXinerama
			)
		elseif ( EXISTS "/usr/include/X11/extensions/Xinerama.h" )
			set( CASTOR_HAS_XINERAMA 1 )
			set( PLATFORM_LIBRARIES
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

	option( CASTOR_USE_TRACK "Enable function tracking" FALSE )

	set( CastorBinsDependencies
		${CastorBinsDependencies}
		${PROJECT_NAME}
		PARENT_SCOPE
	)

	set( ${PROJECT_NAME}_DESCRIPTION "${MAIN_PROJECT_NAME} Utils" )
	set( ${PROJECT_NAME}_BRIEF "Multiplatform utilities library" )
	set( ${PROJECT_NAME}_ICON "${CMAKE_SOURCE_DIR}/../data/Images/castor-dark-small.png" )
	set( ${PROJECT_NAME}_VERSION_MAJOR 0 )
	set( ${PROJECT_NAME}_VERSION_MINOR 11 )
	set( ${PROJECT_NAME}_VERSION_BUILD 0 )

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
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Config/CompilerConfig.hpp
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
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/ZipArchive.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/BinaryFile.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/BinaryFile.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/BinaryLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/BinaryWriter.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Endianness.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Loader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/LoaderException.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/Path.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextFile.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextFile.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Data/TextWriter.hpp
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

	set( ${PROJECT_NAME}_FOLDER_SRC_C_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/ioapi.c
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/iowin32.c
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/mztools.c
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/unzip.c
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/zip.c
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/crypt.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/ioapi.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/iowin32.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/mztools.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/unzip.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Data/MiniZip/zip.h
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Data\\Minizip" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Data\\Minizip" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )
	source_group( "Source Files\\Data\\Minizip" FILES ${${PROJECT_NAME}_FOLDER_SRC_C_FILES} )

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ArrayView.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/BlockGuard.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ChangeTracked.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Collection.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Collection.inl
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
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/ScopeGuard.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Signal.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Templates.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/UnicityException.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Design/Unique.hpp
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Design" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Exception/Assertion.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Exception/Exception.hpp
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
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/DdsImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Font.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/FontCache.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Glyph.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/HdrColourComponent.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Image.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ImageCache.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/ImageWriter.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/PixelBufferBase.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/PixelFormat.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Position.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Rectangle.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/Size.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/StbImageLoader.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/StbImageWriter.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/XpmImageLoader.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/BoundingBox.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/BoundingContainer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/BoundingSphere.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Colour.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Colour.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ColourComponent.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/DdsImageLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Font.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/FontCache.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Glyph.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/HdrColourComponent.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Image.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ImageCache.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ImageLoader.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/ImageWriter.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Pixel.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/Pixel.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelBuffer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelBuffer.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelBufferBase.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Graphics/PixelConstIterator.hpp
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
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/stb_image.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/stb_image_resize.h
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Graphics/stb_image_write.h
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Graphics" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Graphics" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

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
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Line2D.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Line3D.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Math.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Matrix.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Matrix.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/PlaneEquation.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Point.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Point.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/PointOperators.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/PointOperators.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Quaternion.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Quaternion.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Range.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/RangedValue.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Simd.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Math/Simd.inl
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
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Multithreading/ThreadPool.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Multithreading/WorkerThread.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
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

	if ( ANDROID )
		set( _PLATFORM Android )
	elseif ( WIN32 )
		set( _PLATFORM Win32 )
	elseif ( APPLE )
		set( _PLATFORM MacOS )
		set( ${PROJECT_NAME}_FOLDER_SRC_FILES
			${${PROJECT_NAME}_FOLDER_SRC_FILES}
			${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/MacOS/DisplaySize.mm
		)
	else ()
		set( _PLATFORM Linux )
	endif ()
	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/${_PLATFORM}/Aligned.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/${_PLATFORM}/CpuInformations.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/${_PLATFORM}/Debug.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/${_PLATFORM}/DynamicLibrary.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/${_PLATFORM}/File.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/${_PLATFORM}/LoggerConsole.cpp
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Platform/${_PLATFORM}/Utils.cpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	source_group( "Source Files\\Platform" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_SRC_FILES
		${CASTOR_SOURCE_DIR}/source/Core/${PROJECT_NAME}/Pool/PoolException.cpp
	)
	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/BuddyAllocator.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/BuddyAllocator.inl
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/FixedGrowingSizeMarkedMemoryData.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/FixedGrowingSizeMemoryData.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/FixedSizeMarkedMemoryData.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/FixedSizeMemoryData.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/MemoryDataTyper.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/ObjectPool.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/PoolException.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/PoolManagedObject.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Pool/UniqueObjectPool.hpp
	)
	set( ${PROJECT_NAME}_SRC_FILES
		${${PROJECT_NAME}_SRC_FILES}
		${${PROJECT_NAME}_FOLDER_SRC_FILES}
	)
	set( ${PROJECT_NAME}_HDR_FILES
		${${PROJECT_NAME}_HDR_FILES}
		${${PROJECT_NAME}_FOLDER_HDR_FILES}
	)
	source_group( "Header Files\\Pool" FILES ${${PROJECT_NAME}_FOLDER_HDR_FILES} )
	source_group( "Source Files\\Pool" FILES ${${PROJECT_NAME}_FOLDER_SRC_FILES} )

	set( ${PROJECT_NAME}_FOLDER_HDR_FILES
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamBaseManipulators.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamIndentBuffer.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamIndentBufferManager.hpp
		${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME}/Stream/StreamIndentManipulators.hpp
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
	target_compile_definitions( ${PROJECT_NAME}
		PUBLIC
			_UNICODE
			UNICODE
		PRIVATE
			${${PROJECT_NAME}_DEFINITIONS}
	)
	target_include_directories( ${PROJECT_NAME}
		PUBLIC
			${CASTOR_SOURCE_DIR}/include
			${CASTOR_BINARY_DIR}/include
		PRIVATE
			${CASTOR_SOURCE_DIR}/include/Core
			${CASTOR_BINARY_DIR}/include/Core
			${CASTOR_SOURCE_DIR}/source/Core
			${CASTOR_BINARY_DIR}/source/Core
			${FREETYPE_INCLUDE_DIR_ft2build}
			${FREETYPE_INCLUDE_DIR_freetype2}
	)

	if ( WIN32 )
		target_link_libraries( ${PROJECT_NAME}
			PUBLIC
				Dbghelp
			PRIVATE
				${PLATFORM_LIBRARIES}
				${Freetype_LIBRARIES}
				ZLIB::ZLIB
		)
	else ()
		target_link_libraries( ${PROJECT_NAME}
			PUBLIC
				${CMAKE_DL_LIBRARY}
			PRIVATE
				${PLATFORM_LIBRARIES}
				${Freetype_LIBRARIES}
				ZLIB::ZLIB
		)
	endif ()

	install(
		FILES ${CASTOR_BINARY_DIR}/include/Core/${PROJECT_NAME}/config.hpp
		DESTINATION include/${PROJECT_NAME}
		COMPONENT ${PROJECT_NAME}_dev
	)

	if ( WIN32 )
		set( CMAKE_INSTALL_DEBUG_LIBRARIES TRUE )
		set( CMAKE_INSTALL_SYSTEM_RUNTIME_DESTINATION bin/$<$<CONFIG:Debug>:Debug/> )
		include( InstallRequiredSystemLibraries )
		if ( CASTOR_USE_SYSTEM_LIBS )
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
	endif ()

	install_target( ${PROJECT_NAME} api_dll ${CASTOR_SOURCE_DIR}/include/Core/${PROJECT_NAME} )
	set_property( TARGET ${PROJECT_NAME} PROPERTY FOLDER "Core" )
	add_target_astyle( ${PROJECT_NAME} ".h;.hpp;.inl;.cpp" )
	target_add_doc( ${PROJECT_NAME} "French" "*.h *.hpp" )
	target_add_doc( ${PROJECT_NAME} "English" "*.h *.hpp" )

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

set( CastorMinLibraries
	${CastorMinLibraries}
	${PROJECT_NAME}
	PARENT_SCOPE
)
