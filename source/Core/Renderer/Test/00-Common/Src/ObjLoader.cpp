#include "ObjLoader.hpp"

#include <stdlib.h>
#include <fstream>
#include <sstream>

#include <Utils/StringUtils.hpp>

namespace common
{
	bool doPreloadFile( std::string const & fileContent
		, uint16_t & nv
		, uint16_t & nvt
		, uint16_t & nvn
		, uint16_t & nf
		, uint16_t & ntf
		, renderer::UInt16Array & faces )
	{
		std::istringstream file( fileContent );
		std::string line;
		std::string mtlfile;

		while ( std::getline( file, line ) )
		{
			utils::trimLeft( line );
			std::stringstream stream( line );
			std::string ident;
			stream >> ident;

			if ( ident == "v" )
			{
				if ( ntf )
				{
					faces.push_back( ntf );
					ntf = 0u;
				}

				++nv;
			}
			else if ( ident == "vt" )
			{
				++nvt;
			}
			else if ( ident == "vn" )
			{
				++nvn;
			}
			else if ( ident == "f" )
			{
				++nf;
				++ntf;
			}
			else if ( ident == "g" || ident == "usemtl" )
			{
				if ( ntf )
				{
					faces.push_back( ntf );
					ntf = 0u;
				}
			}
			else if ( ident == "mtllib" )
			{
				mtlfile = line.substr( line.find_first_of( " " ) + 1 );
				utils::trim( mtlfile );
			}
		}

		if ( ntf )
		{
			faces.push_back( ntf );
		}

		std::clog << "    Vertex count: " << nv << std::endl;
		std::clog << "    TexCoord count: " << nvt << std::endl;
		std::clog << "    Normal count: " << nvn << std::endl;
		std::clog << "    Group count: " << int( faces.size() ) << std::endl;

		return true;
	}

	void loadObjFile( std::string const & fileContent
		, std::vector< TexturedVertexData > & vboData
		, renderer::UInt16Array & iboData )
	{
		uint16_t nv = 0u;
		uint16_t nvt = 0u;
		uint16_t nvn = 0u;
		uint16_t nf = 0u;
		uint16_t ntf = 0u;
		renderer::UInt16Array faces;

		if ( !doPreloadFile( fileContent
			, nv
			, nvt
			, nvn
			, nf
			, ntf
			, faces ) )
		{
			std::cerr << "Failed to load the OBJ file" << std::endl;
			return;
		}

		std::istringstream file( fileContent );
		std::string line;
		renderer::Vec3Array allvtx( nv );
		renderer::Vec2Array alltex( nvt );
		renderer::Vec3Array allnml( nvn );
		auto vtxit = allvtx.begin();
		auto texit = alltex.begin();
		auto nmlit = allnml.begin();

		while ( std::getline( file, line ) )
		{
			utils::trim( line );
			std::stringstream stream( line );
			std::string ident;
			stream >> ident;

			if ( ident == "v" )
			{
				stream >> vtxit->x >> vtxit->y >> vtxit->z;
				++vtxit;
			}
			else if ( ident == "vt" )
			{
				stream >> texit->x >> texit->y;
				++texit;
			}
			else if ( ident == "vn" )
			{
				stream >> nmlit->x >> nmlit->y >> nmlit->z;
				++nmlit;
			}
		}

		file.clear();
		file.seekg( 0, std::ios::beg );

		renderer::UInt16Array index( nf * 3 );
		renderer::Vec3Array vertex( nf * 3 );
		renderer::Vec3Array normal( nf * 3 );
		renderer::Vec2Array texcoord( nf * 3 );
		auto idxit = index.begin();
		vtxit = vertex.begin();
		texit = texcoord.begin();
		nmlit = normal.begin();
		auto facesit = faces.end();
		std::string mtlname;
		uint16_t normalsCount = 0u;

		while ( std::getline( file, line ) )
		{
			utils::trim( line );
			std::stringstream stream( line );
			std::string ident;
			stream >> ident;

			if ( ident == "g" )
			{
				if ( facesit == faces.end() )
				{
					facesit = faces.begin();
				}
				else
				{
					iboData.insert( iboData.end(), index.begin(), idxit );
					idxit = index.begin();
					++facesit;
				}

				std::clog << "    Group faces count: " << *facesit << std::endl;
			}
			else if ( ident == "usemtl" )
			{
				if ( facesit == faces.end() )
				{
					facesit = faces.begin();
				}
				else if ( index.begin() != idxit )
				{
					iboData.insert( iboData.end(), index.begin(), idxit );
					idxit = index.begin();
					++facesit;
				}

				stream >> mtlname;
				utils::trim( mtlname );
			}
			else if ( ident == "f" )
			{
				for ( uint32_t i = 0u; i < 3u; i++ )
				{
					std::string face;
					stream >> face;

					size_t index1 = face.find( '/' );
					std::string component = face.substr( 0, index1 );
					uint16_t iv = atoi( component.c_str() ) - 1;
					*vtxit++ = allvtx[iv];

					++index1;
					size_t index2 = face.find( '/', index1 );

					if ( index2 > index1 )
					{
						component = face.substr( index1, index2 - index1 );
						uint32_t ivt = atoi( component.c_str() ) - 1;
						texit->x = alltex[ivt].x;
						texit->y = 1.0f - alltex[ivt].y;
						++texit;
					}

					++index2;
					component = face.substr( index2 );
					uint32_t ivn = atoi( component.c_str() ) - 1;
					*nmlit++ = allnml[ivn];

					*idxit++ = normalsCount;
					normalsCount++;
				}
			}
		}

		if ( idxit != index.begin()
			&& facesit != faces.end() )
		{
			iboData.insert( iboData.end(), index.begin(), idxit );
			auto size = size_t( std::distance( vertex.begin(), vtxit ) );

			for ( size_t i = 0; i < size; ++i )
			{
				auto & vtx = vertex[i];
				auto & tex = texcoord[i];
				vboData.push_back( { { vtx[0], vtx[1], vtx[2] - 3.0f, 1.0f }, { tex[0], tex[1] } } );
			}
		}
	}
}
