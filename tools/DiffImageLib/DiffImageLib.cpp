#include "DiffImageLib.hpp"

#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/ImageLoader.hpp>
#include <CastorUtils/Graphics/ImageWriter.hpp>
#include <CastorUtils/Graphics/FreeImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageWriter.hpp>
#include <CastorUtils/Multithreading/ThreadPool.hpp>

#include <vector>
#include <string>

namespace diffimg
{
	//*********************************************************************************************

	namespace
	{
		castor::Path initialiseDir( castor::Path const & basePath
			, castor::String const & name )
		{
			auto result = basePath / cuT( "Compare" ) / name;

			if ( !castor::File::directoryExists( result ) )
			{
				castor::File::directoryCreate( result );
			}

			return result;
		}

		void processLog( castor::Path const & file
			, castor::Path const & directory
			, bool moveLog )
		{
			auto logFile = file.getPath() / ( file.getFileName() + cuT( ".log" ) );

			if ( castor::File::fileExists( logFile ) )
			{
				if ( moveLog )
				{
					castor::File::copyFile( logFile, directory );
				}

				castor::File::deleteFile( logFile );
			}
		}

		void moveOutput( castor::Path const & file
			, castor::Path const & directory
			, bool moveLog )
		{
			if ( castor::File::fileExists( file ) )
			{
				castor::File::copyFile( file, directory );
				castor::File::deleteFile( file );
				processLog( file, directory, moveLog );
			}
		}
	}

	//*********************************************************************************************

	Config::Config( Options const & options )
	{
		dirs[size_t( diffimg::DiffResult::eUnprocessed )] = initialiseDir( options.input.getPath(), cuT( "Unprocessed" ) );
		dirs[size_t( diffimg::DiffResult::eUnacceptable )] = initialiseDir( options.input.getPath(), cuT( "Unacceptable" ) );
		dirs[size_t( diffimg::DiffResult::eAcceptable )] = initialiseDir( options.input.getPath(), cuT( "Acceptable" ) );
		dirs[size_t( diffimg::DiffResult::eNegligible )] = initialiseDir( options.input.getPath(), cuT( "Negligible" ) );

		castor::StbImageLoader::registerLoader( loader );
		castor::FreeImageLoader::registerLoader( loader );
		castor::StbImageWriter::registerWriter( writer );

		if ( !castor::File::fileExists( options.input ) )
		{
			std::cout << "Reference image [" << options.input << "] does not exist." << std::endl << std::endl;

			for ( auto & output : options.outputs )
			{
				moveOutput( output, dirs[size_t( diffimg::DiffResult::eUnprocessed )], false );
			}

			throw std::runtime_error{ "Reference image does not exist." };
		}

		reference = std::make_unique< castor::Image >( loader.load( castor::cuEmptyString
			, options.input
			, false
			, false ) );
	}

	//*********************************************************************************************

	DiffResult compareImages( Options const & options
		, Config const & config
		, castor::Path const & compFile )
	{
		if ( !castor::File::fileExists( compFile ) )
		{
			std::cerr << "Output image [" << compFile << "] does not exist." << std::endl;
			processLog( compFile, config.dirs[size_t( diffimg::DiffResult::eUnacceptable )], true );
			return DiffResult::eUnprocessed;
		}

		castor::Image toTest = config.loader.load( castor::cuEmptyString
			, compFile
			, false
			, false );
		bool carryOn = config.reference->getDimensions() == toTest.getDimensions();
		DiffResult result = DiffResult::eUnacceptable;

		if ( !carryOn )
		{
			std::cerr << "Output image [" << compFile << "]'s dimensions don't match reference image's dimensions." << std::endl;
		}
		else
		{
			carryOn = config.reference->getPixelFormat() == toTest.getPixelFormat();

			if ( !carryOn )
			{
				std::cerr << "Output image [" << compFile << "]'s format don't match reference image's format." << std::endl;
			}
			else
			{
				auto src = castor::PxBufferBase::create( config.reference->getDimensions()
					, castor::PixelFormat::eR8G8B8A8_UNORM
					, config.reference->getPixels()->getConstPtr()
					, config.reference->getPixels()->getFormat() );
				auto dst = castor::PxBufferBase::create( toTest.getDimensions()
					, castor::PixelFormat::eR8G8B8A8_UNORM
					, toTest.getPixels()->getConstPtr()
					, toTest.getPixels()->getFormat() );
				auto diffBuffer = castor::PxBufferBase::create( toTest.getDimensions()
					, castor::PixelFormat::eR8G8B8A8_UNORM );
				struct Pixel
				{
					uint8_t r, g, b, a;
				};
				auto srcIt = reinterpret_cast< Pixel const * >( src->getConstPtr() );
				auto end = reinterpret_cast< Pixel const * >( src->getConstPtr() + src->getSize() );
				auto dstIt = reinterpret_cast< Pixel * >( dst->getPtr() );
				auto diffIt = reinterpret_cast< Pixel * >( diffBuffer->getPtr() );
				uint32_t diff{ 0u };

				while ( srcIt != end )
				{
					int16_t dr = int16_t( dstIt->r - srcIt->r );
					int16_t dg = int16_t( dstIt->g - srcIt->g );
					int16_t db = int16_t( dstIt->b - srcIt->b );
					int16_t da = int16_t( dstIt->a - srcIt->a );

					if ( dr || dg || db || da )
					{
						++diff;
					}

					*diffIt = { uint8_t( std::min( 255, ( dr * 4 + srcIt->r / 4 ) / 2 ) )
						, uint8_t( std::min( 255, ( dg * 4 + srcIt->g / 4 ) / 2 ) )
						, uint8_t( std::min( 255, ( db * 4 + srcIt->b / 4 ) / 2 ) )
						, uint8_t( std::min( 255, ( da * 4 + srcIt->a / 4 ) / 2 ) ) };

					++srcIt;
					++dstIt;
					++diffIt;
				}

				auto ratio = ( double( diff ) / src->getSize() );
				result = ( ratio < options.acceptableThreshold
					? ( ratio < options.negligibleThreshold
						? DiffResult::eNegligible
						: DiffResult::eAcceptable )
					: DiffResult::eUnacceptable );
				auto buffer = castor::PxBufferBase::create( toTest.getDimensions()
					, toTest.getPixelFormat()
					, diffBuffer->getConstPtr()
					, diffBuffer->getFormat() );
				castor::Image const diffImage{ castor::cuEmptyString, *buffer };
				config.writer.write( config.dirs[size_t( result )] / ( compFile.getFileName() + cuT( ".diff.png" ) )
					, *diffImage.getPixels() );

				if ( result == DiffResult::eUnacceptable )
				{
					std::cerr << "Output image [" << compFile.getFileName( true ) << "] doesn't match reference image [" << options.input.getFileName( true ) << "]." << std::endl;
				}
			}
		}

		moveOutput( compFile, config.dirs[size_t( result )]
			, result == diffimg::DiffResult::eUnacceptable );
		return result;
	}

	//*********************************************************************************************
}
