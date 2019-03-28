#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/ImageLoader.hpp>
#include <CastorUtils/Graphics/ImageWriter.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>
#include <CastorUtils/Graphics/StbImageWriter.hpp>
#include <CastorUtils/Multithreading/ThreadPool.hpp>

#include <vector>
#include <string>

using StringArray = std::vector< std::string >;
using PathArray = std::vector< castor::Path >;

static double constexpr acceptableThreshold = 0.1;
static double constexpr negligibleThreshold = 0.001;

struct Options
{
	PathArray outputs;
	castor::Path input;
};

void printUsage()
{
	std::cout << "Diff Image is a tool that allows you to compare images issued from CastorTestLauncher to a reference image." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "DiffImage {RENDERERS} -f FILE" << std::endl;
	std::cout << "  FILE must be a .cscn file." << std::endl;
	std::cout << "  RENDERERS is a list of Castor3D renderers for which resulting images will be compared to the reference image." << std::endl;
}

bool doParseArgs( int argc
	, char * argv[]
	, Options & options )
{
	StringArray args{ argv + 1, argv + argc };

	if ( args.empty() )
	{
		std::cerr << "Missing parameters." << std::endl << std::endl;
		printUsage();
		return false;
	}

	auto it = std::find( args.begin(), args.end(), "-h" );

	if ( it == args.end() )
	{
		it = std::find( args.begin(), args.end(), "--help" );
	}

	if ( it != args.end() )
	{
		args.erase( it );
		printUsage();
		return false;
	}

	it = std::find( args.begin(), args.end(), "-f" );

	if ( std::distance( args.begin(), it ) == args.size() - 1 )
	{
		std::cerr << "-f option is missing file parameter" << std::endl << std::endl;
		printUsage();
		return false;
	}

	auto fileIt = it;
	++fileIt;
	auto file = castor::Path{ castor::string::stringCast< xchar >( *fileIt ) };
	options.input = file.getPath() / ( file.getFileName() + cuT( "_ref.png" ) );
	args.erase( fileIt );
	args.erase( it );

	if ( args.empty() )
	{
		std::cerr << "No renderer was given." << std::endl << std::endl;
		printUsage();
		return false;
	}

	for ( auto & arg : args )
	{
		options.outputs.emplace_back( file.getPath() / cuT( "Compare" ) / ( file.getFileName() + cuT( "_" ) + arg + cuT( ".png" ) ) );
	}

	return true;
}

enum class DiffResult
{
	eNegligible,
	eAcceptable,
	eUnacceptable,
};

struct Config
{
	castor::ImageUPtr reference;
	castor::Path unprocessedDir;
	castor::Path unacceptableDir;
	castor::Path acceptableDir;
	castor::Path negligibleDir;
	castor::ImageLoader loader;
	castor::ImageWriter writer;
};

DiffResult doCompareImages( Config const & config
	, castor::Path const & compFile )
{
	castor::Image toTest{ castor::cuEmptyString, compFile, config.loader };
	bool carryOn = config.reference->getDimensions() == toTest.getDimensions();

	if ( carryOn )
	{
		carryOn = config.reference->getPixelFormat() == toTest.getPixelFormat();

		if ( !carryOn )
		{
			std::cerr << "Output image [" << compFile << "]'s format don't match reference image's format." << std::endl;
		}
	}
	else
	{
		std::cerr << "Output image [" << compFile << "]'s dimensions don't match reference image's dimensions." << std::endl;
	}

	DiffResult result = DiffResult::eUnacceptable;

	if ( carryOn )
	{
		auto src = std::static_pointer_cast< castor::PxBuffer< castor::PixelFormat::eR8G8B8A8_UNORM > >( castor::PxBufferBase::create( config.reference->getDimensions()
			, castor::PixelFormat::eR8G8B8A8_UNORM
			, config.reference->getPixels()->getConstPtr()
			, config.reference->getPixels()->getFormat() ) );
		auto dst = std::static_pointer_cast< castor::PxBuffer< castor::PixelFormat::eR8G8B8A8_UNORM > >( castor::PxBufferBase::create( toTest.getDimensions()
			, castor::PixelFormat::eR8G8B8A8_UNORM
			, toTest.getPixels()->getConstPtr()
			, toTest.getPixels()->getFormat() ) );
		auto diffBuffer = std::static_pointer_cast< castor::PxBuffer< castor::PixelFormat::eR8G8B8A8_UNORM > >( castor::PxBufferBase::create( toTest.getDimensions()
			, castor::PixelFormat::eR8G8B8A8_UNORM ) );
		auto srcIt = src->begin();
		auto dstIt = dst->begin();
		auto diffIt = diffBuffer->begin();
		uint32_t diff{ 0u };
		static castor::Pixel< castor::PixelFormat::eR8G8B8A8_UNORM > const diffPixel = []()
		{
			std::array< uint8_t, 4u > data{ 0xFF, 0x00, 0xFF, 0xFF };
			castor::Pixel< castor::PixelFormat::eR8G8B8A8_UNORM > result{ true };
			auto it = result.begin();
			*it = data[0]; ++it;
			*it = data[1]; ++it;
			*it = data[2]; ++it;
			*it = data[3]; ++it;
			return result;
		}();

		while ( srcIt != src->end() )
		{
			carryOn = *srcIt == *dstIt;

			if ( !carryOn )
			{
				++diff;
				*diffIt = diffPixel;
			}
			else
			{
				*diffIt = *srcIt;
			}

			++srcIt;
			++dstIt;
			++diffIt;
		}

		auto ratio = ( double( diff ) / src->getSize() );
		result = ( ratio < acceptableThreshold
			? ( ratio < negligibleThreshold
				? DiffResult::eNegligible
				: DiffResult::eAcceptable )
			: DiffResult::eUnacceptable );
		auto buffer = castor::PxBufferBase::create( toTest.getDimensions()
			, toTest.getPixelFormat()
			, diffBuffer->getConstPtr()
			, diffBuffer->getFormat() );
		castor::Image const diffImage{ castor::cuEmptyString, *buffer };

		switch ( result )
		{
		case DiffResult::eAcceptable:
			config.writer.write( config.acceptableDir / ( compFile.getFileName() + cuT( ".diff.png" ) )
				, *diffImage.getPixels() );
			break;
		case DiffResult::eUnacceptable:
			config.writer.write( config.unacceptableDir / ( compFile.getFileName() + cuT( ".diff.png" ) )
				, *diffImage.getPixels() );
			break;
		case DiffResult::eNegligible:
			config.writer.write( config.negligibleDir / ( compFile.getFileName() + cuT( ".diff.png" ) )
				, *diffImage.getPixels() );
			break;
		default:
			break;
		}
	}

	return result;
}

void doProcessLog( castor::Path const & file
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

void doMoveOutput( castor::Path const & file
	, castor::Path const & directory
	, bool moveLog )
{
	if ( castor::File::fileExists( file ) )
	{
		castor::File::copyFile( file, directory );
		castor::File::deleteFile( file );
		doProcessLog( file, directory, moveLog );
	}
}

void doRemoveOutput( castor::Path const & file )
{
	if ( castor::File::fileExists( file ) )
	{
		castor::File::deleteFile( file );
	}

	auto logFile = file.getPath() / ( file.getFileName() + cuT( ".log" ) );

	if ( castor::File::fileExists( logFile ) )
	{
		castor::File::deleteFile( logFile );
	}
}

castor::Path doInitialiseDir( castor::Path const & basePath
	, castor::String const & name )
{
	auto result = basePath / cuT( "Compare" ) / name;

	if ( !castor::File::directoryExists( result ) )
	{
		castor::File::directoryCreate( result );
	}

	return result;
}

int main( int argc, char * argv[] )
{
	int result = EXIT_SUCCESS;
	Options options;

	if ( doParseArgs( argc, argv, options ) )
	{
		Config config;
		config.unprocessedDir = doInitialiseDir( options.input.getPath(), cuT( "Unprocessed" ) );
		config.unacceptableDir = doInitialiseDir( options.input.getPath(), cuT( "Unacceptable" ) );
		config.acceptableDir = doInitialiseDir( options.input.getPath(), cuT( "Acceptable" ) );
		config.negligibleDir = doInitialiseDir( options.input.getPath(), cuT( "Negligible" ) );
		castor::StbImageLoader::registerLoader( config.loader );
		castor::StbImageWriter::registerWriter( config.writer );

		if ( !castor::File::fileExists( options.input ) )
		{
			std::cout << "Reference image [" << options.input << "] does not exist." << std::endl << std::endl;

			for ( auto & output : options.outputs )
			{
				doMoveOutput( output, config.unprocessedDir, false );
			}

			return result;
		}

		try
		{
			config.reference = std::make_unique< castor::Image >( castor::cuEmptyString
				, options.input
				, config.loader );
			castor::ThreadPool pool{ options.outputs.size() };

			for ( auto & output : options.outputs )
			{
				pool.pushJob( [&options, &output, &config]()
					{
						if ( !castor::File::fileExists( output ) )
						{
							std::cerr << "Output image [" << output << "] does not exist." << std::endl;
							doProcessLog( output, config.unacceptableDir, true );
						}
						else
						{
							auto compare = doCompareImages( config, output );

							switch ( compare )
							{
							case DiffResult::eNegligible:
								doMoveOutput( output, config.negligibleDir, false );
								break;
							case DiffResult::eAcceptable:
								doMoveOutput( output, config.acceptableDir, false );
								break;
							case DiffResult::eUnacceptable:
								std::cerr << "Output image [" << output.getFileName( true ) << "] doesn't match reference image [" << options.input.getFileName( true ) << "]." << std::endl;
								doMoveOutput( output, config.unacceptableDir, true );
								break;
							default:
								break;
							}
						}
					} );
			}

			pool.waitAll( castor::Milliseconds::max() );
		}
		catch ( std::exception & exc )
		{
			std::cerr << "Failure " << exc.what() << std::endl;
		}
	}

	return result;
}

//******************************************************************************
