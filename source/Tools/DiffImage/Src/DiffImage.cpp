#include <Graphics/Image.hpp>

#include <vector>
#include <string>

using StringArray = std::vector< std::string >;
using PathArray = std::vector< castor::Path >;

static double constexpr threshold = 0.1;

struct Options
{
	PathArray outputs;
	castor::Path input;
};

void printUsage()
{
	std::cout << "Diff Image is a tool that allows you to compare images issued from CastorTestLauncher to a reference image." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "DiffImage {RENDERERS} FILE" << std::endl;
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
		options.outputs.emplace_back( file.getPath() / ( file.getFileName() + cuT( "_" ) + arg + cuT( ".png" ) ) );
	}

	return true;
}

bool doCompareImages( castor::Image const & reference, castor::Path const & compFile )
{
	castor::Image toTest{ castor::cuEmptyString, compFile };
	bool result = reference.getDimensions() == toTest.getDimensions();

	if ( result )
	{
		result = reference.getPixelFormat() == toTest.getPixelFormat();

		if ( !result )
		{
			std::cerr << "Output image [" << compFile << "]'s format don't match reference image's format." << std::endl;
		}
	}
	else
	{
		std::cerr << "Output image [" << compFile << "]'s dimensions don't match reference image's dimensions." << std::endl;
	}

	if ( result )
	{
		auto src = std::static_pointer_cast< castor::PxBuffer< castor::PixelFormat::eA8R8G8B8 > >( castor::PxBufferBase::create( reference.getDimensions()
			, castor::PixelFormat::eA8R8G8B8
			, reference.getPixels()->constPtr()
			, reference.getPixels()->format() ) );
		auto dst = std::static_pointer_cast< castor::PxBuffer< castor::PixelFormat::eA8R8G8B8 > >( castor::PxBufferBase::create( toTest.getDimensions()
			, castor::PixelFormat::eA8R8G8B8
			, toTest.getPixels()->constPtr()
			, toTest.getPixels()->format() ) );
		auto diffBuffer = std::static_pointer_cast< castor::PxBuffer< castor::PixelFormat::eA8R8G8B8 > >( castor::PxBufferBase::create( toTest.getDimensions()
			, castor::PixelFormat::eA8R8G8B8 ) );
		auto srcIt = src->begin();
		auto dstIt = dst->begin();
		auto diffIt = diffBuffer->begin();
		uint32_t diff{ 0u };
		static castor::Pixel< castor::PixelFormat::eA8R8G8B8 > const diffPixel = []()
		{
			std::array< uint8_t, 4u > data{ 0xFF, 0x00, 0xFF, 0xFF };
			castor::Pixel< castor::PixelFormat::eA8R8G8B8 > result{ true };
			auto it = result.begin();
			*it = data[0]; ++it;
			*it = data[1]; ++it;
			*it = data[2]; ++it;
			*it = data[3]; ++it;
			return result;
		}();

		while ( srcIt != src->end() )
		{
			result = *srcIt == *dstIt;

			if ( !result )
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

		result = ( double( diff ) / src->size() ) < threshold;

		if ( diff > 1000u )
		{
			auto buffer = castor::PxBufferBase::create( toTest.getDimensions()
				, toTest.getPixelFormat()
				, diffBuffer->constPtr()
				, diffBuffer->format() );
			castor::Image const diffImage{ castor::cuEmptyString, *buffer };
			castor::Image::BinaryWriter{}( diffImage, compFile.getPath() / cuT( "Diff" ) / ( compFile.getFileName() + cuT( ".diff.png" ) ) );
		}
	}

	return result;
}

int main( int argc, char * argv[] )
{
	int result = EXIT_SUCCESS;
	Options options;

	if ( doParseArgs( argc, argv, options ) )
	{
		if ( !castor::File::fileExists( options.input ) )
		{
			std::cout << "Reference image [" << options.input << "] does not exist." << std::endl << std::endl;

			for ( auto & file : options.outputs )
			{
				castor::File::deleteFile( file );
				auto logFile = file.getPath() / ( file.getFileName() + cuT( ".log" ) );

				if ( castor::File::fileExists( logFile ) )
				{
					castor::File::deleteFile( logFile );
				}
			}

			return result;
		}

		auto diffDir = options.input.getPath() / cuT( "Diff" );

		if ( !castor::File::directoryExists( diffDir ) )
		{
			castor::File::directoryCreate( diffDir );
		}

		try
		{
			castor::Image reference{ castor::cuEmptyString, options.input };
			PathArray toRemove;

			for ( auto & output : options.outputs )
			{
				if ( !castor::File::fileExists( output ) )
				{
					std::cerr << "Output image [" << output.getFileName( true ) << "] does not exist." << std::endl;
				}
				else
				{
					if ( !doCompareImages( reference, output ) )
					{
						std::cerr << "Output image [" << output.getFileName( true ) << "] doesn't match reference image [" << options.input.getFileName( true ) << "]." << std::endl;
						castor::File::copyFile( output, diffDir );
						castor::File::deleteFile( output );
						auto logFile = output.getPath() / ( output.getFileName() + cuT( ".log" ) );

						if ( castor::File::fileExists( logFile ) )
						{
							castor::File::copyFile( logFile, diffDir );
							castor::File::deleteFile( logFile );
						}
					}
					else
					{
						toRemove.push_back( output );
					}
				}
			}

			for ( auto & file : toRemove )
			{
				castor::File::deleteFile( file );
				auto logFile = file.getPath() / ( file.getFileName() + cuT( ".log" ) );

				if ( castor::File::fileExists( logFile ) )
				{
					castor::File::deleteFile( logFile );
				}
			}
		}
		catch ( std::exception & exc )
		{
			std::cerr << "Failure " << exc.what() << std::endl;
		}
	}

	return result;
}

//******************************************************************************
