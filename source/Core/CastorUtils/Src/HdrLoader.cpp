#include "HdrLoader.hpp"

#include "PixelBufferBase.hpp"

#include <cmath>
#include <memory>
#include <cstdio>

namespace Castor
{
	typedef unsigned char RGBE[4];
	static uint32_t const R = 0;
	static uint32_t const G = 1;
	static uint32_t const B = 2;
	static uint32_t const E = 3;

	static uint16_t const MINELEN = 8; // minimum scanline length for encoding
	static uint16_t const MAXELEN = 0x7fff; // maximum scanline length for encoding

	namespace
	{
		float DoConvertComponent( int p_expo, int p_val )
		{
			return ( p_val / 256.0f ) * float( pow( 2, p_expo ) );
		}

		void DoWorkOnRGBE( RGBE * p_scan, int p_len, float * p_cols )
		{
			while ( p_len-- > 0 )
			{
				int l_expo = p_scan[0][E] - 128;
				p_cols[0] = DoConvertComponent( l_expo, p_scan[0][R] );
				p_cols[1] = DoConvertComponent( l_expo, p_scan[0][G] );
				p_cols[2] = DoConvertComponent( l_expo, p_scan[0][B] );
				p_cols += 3;
				p_scan++;
			}
		}

		bool DoOldDecrunch( RGBE * p_scanline, int p_len, FILE * p_file )
		{
			int l_rshift = 0;

			while ( p_len > 0 )
			{
				p_scanline[0][R] = fgetc( p_file );
				p_scanline[0][G] = fgetc( p_file );
				p_scanline[0][B] = fgetc( p_file );
				p_scanline[0][E] = fgetc( p_file );

				if ( feof( p_file ) )
				{
					return false;
				}

				if ( p_scanline[0][R] == 1
					 && p_scanline[0][G] == 1
					 && p_scanline[0][B] == 1 )
				{
					for ( uint32_t i = p_scanline[0][E] << l_rshift; i > 0; i-- )
					{
						memcpy( &p_scanline[0][0], &p_scanline[-1][0], 4 );
						p_scanline++;
						p_len--;
					}

					l_rshift += 8;
				}
				else
				{
					p_scanline++;
					p_len--;
					l_rshift = 0;
				}
			}
			return true;
		}

		bool DoDecrunch( RGBE * p_scanline, uint16_t p_len, FILE * p_file )
		{
			if ( p_len < MINELEN || p_len > MAXELEN )
			{
				return DoOldDecrunch( p_scanline, p_len, p_file );
			}

			int i = fgetc( p_file );

			if ( i != 2 )
			{
				fseek( p_file, -1, SEEK_CUR );
				return DoOldDecrunch( p_scanline, p_len, p_file );
			}

			p_scanline[0][G] = fgetc( p_file );
			p_scanline[0][B] = fgetc( p_file );
			i = fgetc( p_file );

			if ( p_scanline[0][G] != 2 || p_scanline[0][B] & 128 )
			{
				p_scanline[0][R] = 2;
				p_scanline[0][E] = i;
				return DoOldDecrunch( p_scanline + 1, p_len - 1, p_file );
			}

			// read each component
			for ( uint8_t i = 0; i < 4; i++ )
			{
				for ( uint16_t j = 0; j < p_len; )
				{
					uint8_t l_code = fgetc( p_file );

					if ( l_code > 128 )
					{ // run
						l_code &= 127;
						uint8_t val = fgetc( p_file );

						while ( l_code-- )
						{
							p_scanline[j++][i] = val;
						}
					}
					else
					{	// non-run
						while ( l_code-- )
						{
							p_scanline[j++][i] = fgetc( p_file );
						}
					}
				}
			}

			return feof( p_file ) ? false : true;
		}

	}

	PxBufferBaseSPtr HdrLoader::Load( Path const & p_path )
	{
		FILE * l_file = nullptr;
		FOpen( l_file, p_path.c_str(), "rb" );

		if ( !l_file )
		{
			return nullptr;
		}

		char l_str[200];
		fread( l_str, 10, 1, l_file );

		if ( memcmp( l_str, "#?RADIANCE", 10 ) )
		{
			fclose( l_file );
			return nullptr;
		}

		fseek( l_file, 1, SEEK_CUR );

		char l_cmd[200];
		uint32_t i = 0;
		char l_curc = 0;
		char l_oldc = 0;

		while ( true )
		{
			l_oldc = l_curc;
			l_curc = fgetc( l_file );

			if ( l_curc == 0xa && l_oldc == 0xa )
			{
				break;
			}

			l_cmd[i++] = l_curc;
		}

		char l_reso[200];
		i = 0;

		while ( true )
		{
			l_curc = fgetc( l_file );
			l_reso[i++] = l_curc;

			if ( l_curc == 0xa )
			{
				break;
			}
		}

		uint32_t l_width{ 0 };
		uint32_t l_height{ 0 };

		if ( !sscanf( l_reso, "-Y %ld +X %ld", &l_height, &l_width ) )
		{
			fclose( l_file );
			return nullptr;
		}

		Size l_size{ l_width, l_height };
		auto l_pxBuffer = PxBufferBase::create( l_size, ePIXEL_FORMAT_ARGB32F );
		std::vector< RGBE > l_scanline( l_width );
		float * l_buffer = reinterpret_cast< float * >( l_pxBuffer->ptr() );

		// convert image 
		for ( int y = l_height - 1; y >= 0; y-- )
		{
			if ( DoDecrunch( l_scanline.data(), l_width, l_file ) == false )
			{
				break;
			}

			DoWorkOnRGBE( l_scanline.data(), l_width, l_buffer );
			l_buffer[3] = 1.0f;
			l_buffer += l_width * 4;
		}

		fclose( l_file );
		return l_pxBuffer;
	}
}
