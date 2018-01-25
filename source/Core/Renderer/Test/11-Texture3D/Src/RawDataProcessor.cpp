#include "RawDataProcessor.hpp"

#include <cstdio>

namespace vkapp
{
	void readFile( std::string const & name
		, renderer::UIVec3 const & size
		, renderer::ByteArray & data )
	{
		FILE * file = fopen( name.c_str(), "rb" );

		if ( !file )
		{
			throw std::runtime_error{ "Couldn't load file." };
		}

		// Holds the luminance buffer
		renderer::ByteArray luminance( size[0] * size[1] * size[2] );
		fread( luminance.data(), 1u, luminance.size(), file );
		fclose( file );

		// Convert the data to RGBA data.
		data.resize( size[0] * size[1] * size[2] * 4 );
		auto buffer = data.data();

		for ( auto & c : luminance )
		{
			buffer[0] = c;
			buffer[1] = c;
			buffer[2] = c;
			buffer[3] = c;
			buffer += 4;
		}
	}
}
