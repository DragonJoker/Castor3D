#include <DiffImageLib/DiffImageLib.hpp>

#include <CastorUtils/Multithreading/ThreadPool.hpp>

#include <vector>
#include <string>

void printUsage()
{
	std::cout << "Diff Image is a tool that allows you to compare images issued from CastorTestLauncher to a reference image." << std::endl;
	std::cout << "Usage:" << std::endl;
	std::cout << "DiffImage {RENDERERS} -f FILE" << std::endl;
	std::cout << "  FILE must be a .cscn file." << std::endl;
	std::cout << "  RENDERERS is a list of Castor3D renderers for which resulting images will be compared to the reference image." << std::endl;
	std::cout << "  Can be vk, gl, or d3d11." << std::endl;
}

bool doParseArgs( int argc
	, char * argv[]
	, diffimg::Options & options )
{
	castor::StringArray args{ argv + 1, argv + argc };

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

int main( int argc, char * argv[] )
{
	diffimg::Options options;

	if ( !doParseArgs( argc, argv, options ) )
	{
		return -1;
	}

	try
	{
		diffimg::Config config{ options };
		castor::ThreadPool pool{ options.outputs.size() };
		std::atomic_int failures = 0;

		for ( auto & output : options.outputs )
		{
			pool.pushJob( [&options, &output, &config, &failures]()
				{
					auto compare = diffimg::compareImages( options, config, output );
					failures += ( compare == diffimg::DiffResult::eUnacceptable
						? 1
						: 0 );
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
