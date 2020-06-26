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
	std::cout << "  Can be vk, gl4, or d3d11." << std::endl;
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
	auto file = castor::Path{ castor::string::stringCast< castor::xchar >( *fileIt ) };
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
	eUnprocessed,
	eNegligible,
	eAcceptable,
	eUnacceptable,
	eCount,
};

struct Config
{
	castor::ImageUPtr reference;
	std::array< castor::Path, size_t( DiffResult::eCount ) > dirs;
	castor::ImageLoader loader;
	castor::ImageWriter writer;
};

DiffResult doCompareImages( Options const & options
	, Config const & config
	, castor::Path const & compFile )
{
	castor::Image toTest = config.loader.load( castor::cuEmptyString, compFile );
	bool carryOn = config.reference->getDimensions() == toTest.getDimensions();

	if ( !carryOn )
	{
		std::cerr << "Output image [" << compFile << "]'s dimensions don't match reference image's dimensions." << std::endl;
		return DiffResult::eUnacceptable;
	}

	carryOn = config.reference->getPixelFormat() == toTest.getPixelFormat();

	if ( !carryOn )
	{
		std::cerr << "Output image [" << compFile << "]'s format don't match reference image's format." << std::endl;
		return DiffResult::eUnacceptable;
	}

	DiffResult result = DiffResult::eUnacceptable;
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
	auto srcIt = reinterpret_cast< Pixel const* >( src->getConstPtr() );
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
	config.writer.write( config.dirs[size_t( result )] / ( compFile.getFileName() + cuT( ".diff.png" ) )
		, *diffImage.getPixels() );

	if ( result == DiffResult::eUnacceptable )
	{
		std::cerr << "Output image [" << compFile.getFileName( true ) << "] doesn't match reference image [" << options.input.getFileName( true ) << "]." << std::endl;
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
	Options options;

	if ( !doParseArgs( argc, argv, options ) )
	{
		return -1;
	}

	Config config;
	config.dirs[size_t( DiffResult::eUnprocessed )] = doInitialiseDir( options.input.getPath(), cuT( "Unprocessed" ) );
	config.dirs[size_t( DiffResult::eUnacceptable )] = doInitialiseDir( options.input.getPath(), cuT( "Unacceptable" ) );
	config.dirs[size_t( DiffResult::eAcceptable )] = doInitialiseDir( options.input.getPath(), cuT( "Acceptable" ) );
	config.dirs[size_t( DiffResult::eNegligible )] = doInitialiseDir( options.input.getPath(), cuT( "Negligible" ) );
	castor::StbImageLoader::registerLoader( config.loader );
	castor::StbImageWriter::registerWriter( config.writer );

	if ( !castor::File::fileExists( options.input ) )
	{
		std::cout << "Reference image [" << options.input << "] does not exist." << std::endl << std::endl;

		for ( auto & output : options.outputs )
		{
			doMoveOutput( output, config.dirs[size_t( DiffResult::eUnprocessed )], false );
		}

		return EXIT_SUCCESS;
	}

	try
	{
		config.reference = std::make_unique< castor::Image >( config.loader.load( castor::cuEmptyString
			, options.input ) );
		castor::ThreadPool pool{ options.outputs.size() };
		std::atomic_int failures = 0;

		for ( auto & output : options.outputs )
		{
			pool.pushJob( [&options, &output, &config, &failures]()
				{
					if ( !castor::File::fileExists( output ) )
					{
						std::cerr << "Output image [" << output << "] does not exist." << std::endl;
						doProcessLog( output, config.dirs[size_t( DiffResult::eUnacceptable )], true );
					}
					else
					{
						auto compare = doCompareImages( options, config, output );
						doMoveOutput( output, config.dirs[size_t( compare )]
							, compare == DiffResult::eUnacceptable );
						failures += ( compare == DiffResult::eUnacceptable
							? 1
							: 0 );
					}
				} );
		}

		pool.waitAll( castor::Milliseconds::max() );
		return failures;
	}
	catch ( std::exception & exc )
	{
		std::cerr << "Failure " << exc.what() << std::endl;
		return -1;
	}
	catch ( ... )
	{
		std::cerr << "Unknown Failure" << std::endl;
		return -1;
	}
}

//******************************************************************************
