#include "Castor3D/Model/Mesh/Importer.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/HeightMapToNormalMap.hpp>

namespace castor3d
{
	namespace
	{
		static float constexpr normalStrength = 8.0f;

		std::ostream & operator<<( std::ostream & stream, castor::Point3f const & obj )
		{
			stream << std::setprecision( 4 ) << obj->x
				<< ", " << std::setprecision( 4 ) << obj->y
				<< ", " << std::setprecision( 4 ) << obj->z;
			return stream;
		}

		std::ostream & operator<<( std::ostream & stream, castor::BoundingBox const & obj )
		{
			stream << "min: " << obj.getMin() << ", max: " << obj.getMax();
			return stream;
		}

		bool findImage( castor::Path const & path
			, castor::Path const & filePath
			, castor::Path & folder
			, castor::Path & relative )
		{
			if ( castor::File::fileExists( path ) )
			{
				relative = path;
			}
			else if ( castor::File::fileExists( filePath / path ) )
			{
				auto fullPath = filePath / path;
				folder = fullPath.getPath();
				relative = fullPath.getFileName( true );;
			}
			else
			{
				castor::PathArray files;
				castor::String fileName = path.getFileName( true );
				castor::File::listDirectoryFiles( filePath, files, true );
				auto it = std::find_if( files.begin()
					, files.end()
					, [&fileName]( castor::Path const & file )
					{
						return file.getFileName( true ) == fileName
							|| file.getFileName( true ).find( fileName ) == 0;
					} );

				folder = filePath;

				if ( it != files.end() )
				{
					relative = *it;
					relative = castor::Path{ relative.substr( folder.size() + 1 ) };
				}
				else
				{
					relative = castor::Path{ fileName };
				}
			}

			if ( !castor::File::fileExists( folder / relative ) )
			{
				log::error << cuT( "Couldn't load texture file [" ) << path << cuT( "]: File does not exist." ) << std::endl;
				return false;
			}

			return true;
		}

		int clamp( int value, int max )
		{
			if ( value >= max )
			{
				value %= max;
			}

			while ( value < 0 )
			{
				value += max;
			}

			return value;
		}

		size_t getIndex( int x, int y, int width, int height )
		{
			return size_t( y * width + x );
		}

		float getHeight( castor::ArrayView< uint8_t const > const & data
			, int width
			, int height
			, int x
			, int y )
		{
			x = clamp( x, width );
			y = clamp( y, height );
			return float( 0xFF - data[getIndex( x, y, width, height )] ) / 255.0f;
		}

		uint8_t textureCoordinateToRgb( float value )
		{
			return uint8_t( ( value * 0.5 + 0.5 ) * 255.0 );
		}

		castor::Point4f calculateNormal( castor::ArrayView< uint8_t const > const & data
			, int width
			, int height
			, int x
			, int y )
		{
			// surrounding pixels
			float tl = getHeight( data, width, height, x - 1, y - 1 ); // top left
			float  l = getHeight( data, width, height, x - 1, y );   // left
			float bl = getHeight( data, width, height, x - 1, y + 1 ); // bottom left
			float  t = getHeight( data, width, height, x, y - 1 );   // top
			float  c = getHeight( data, width, height, x, y );   // center
			float  b = getHeight( data, width, height, x, y + 1 );   // bottom
			float tr = getHeight( data, width, height, x + 1, y - 1 ); // top right
			float  r = getHeight( data, width, height, x + 1, y );   // right
			float br = getHeight( data, width, height, x + 1, y + 1 ); // bottom right

			// sobel filter
			const float dX = float( ( tr + 2.0 * r + br ) - ( tl + 2.0 * l + bl ) );
			const float dY = float( ( bl + 2.0 * b + br ) - ( tl + 2.0 * t + tr ) );
			const float dZ = float( 1.0 / normalStrength );

			castor::Point3f n{ dX, dY, dZ };
			castor::point::normalise( n );
			return { n->x, n->y, n->z, 1.0f - c };
		}

		castor::ByteArray calculateNormals( castor::ArrayView< uint8_t const > const & data
			, castor::Size const & size )
		{
			castor::ByteArray result;
			result.resize( data.size() * 4u );
			auto width = int( size.getWidth() );
			auto height = int( size.getHeight() );

			for ( int x = 0; x < width; ++x )
			{
				for ( int y = 0; y < height; ++y )
				{
					auto n = calculateNormal( data, width, height, x, y );

					 // convert to rgb
					auto coord = getIndex( x, y, width, height ) * 4;
					result[coord + 0] = textureCoordinateToRgb( n->x );
					result[coord + 1] = textureCoordinateToRgb( n->y );
					result[coord + 2] = textureCoordinateToRgb( n->z );
					result[coord + 3] = uint8_t( n->w * 255.0f );
				}
			}

			return result;
		}
	}

	MeshImporter::MeshImporter( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_fileName()
	{
	}

	bool MeshImporter::import( Mesh & mesh
		, castor::Path const & fileName
		, Parameters const & parameters
		, bool initialise )
	{
		bool splitSubmeshes = false;
		m_parameters.get( cuT( "split_mesh" ), splitSubmeshes );
		m_fileName = fileName;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_nodes.clear();
		m_geometries.clear();
		bool result = true;

		if ( !mesh.getSubmeshCount() )
		{
			result = doImportMesh( mesh );

			if ( result && initialise )
			{
				float value = 1.0f;

				if ( m_parameters.get( cuT( "rescale" ), value )
					&& std::abs( value - 1.0f ) > 0.01f )
				{
					for ( auto submesh : mesh )
					{
						for ( auto & vertex : submesh->getPoints() )
						{
							vertex.pos *= value;
						}
					}
				}

				if ( m_parameters.get( cuT( "pitch" ), value )
					&& std::abs( value ) > 0.01f )
				{
					auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }
						, castor::Angle::fromDegrees( value ) );

					for ( auto submesh : mesh )
					{
						for ( auto & vertex : submesh->getPoints() )
						{
							rot.transform( vertex.pos, vertex.pos );
						}
					}
				}

				if ( m_parameters.get( cuT( "yaw" ), value )
					&& std::abs( value ) > 0.01f )
				{
					auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }
						, castor::Angle::fromDegrees( value ) );

					for ( auto submesh : mesh )
					{
						for ( auto & vertex : submesh->getPoints() )
						{
							rot.transform( vertex.pos, vertex.pos );
						}
					}
				}

				if ( m_parameters.get( cuT( "roll" ), value )
					&& std::abs( value ) > 0.01f )
				{
					auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 0.0f, 1.0f }
						, castor::Angle::fromDegrees( value ) );

					for ( auto submesh : mesh )
					{
						for ( auto & vertex : submesh->getPoints() )
						{
							rot.transform( vertex.pos, vertex.pos );
						}
					}
				}

				mesh.computeContainers();
				log::info << "Loaded mesh [" << mesh.getName() << "]"
					<< " AABB (" << mesh.getBoundingBox() << ")"
					<< ", " << mesh.getVertexCount() << " vertices"
					<< ", " << mesh.getSubmeshCount() << " submeshes" << std::endl;

				for ( auto submesh : mesh )
				{
					mesh.getScene()->getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
				}
			}
		}
		else
		{
			for ( auto submesh : mesh )
			{
				submesh->setMaterial( {}, submesh->getDefaultMaterial(), false );
			}
		}

		return result;
	}

	castor::ImageUPtr MeshImporter::loadImage( castor::Path const & path )const
	{
		castor::ImageUPtr result;
		castor::Path relative;
		castor::Path folder;

		if ( findImage( path, m_filePath, folder, relative ) )
		{
			try
			{
				auto image = getEngine()->getImageLoader().load( relative.getFileName()
					, folder / relative
					, false
					, true );
				result = std::make_unique< castor::Image >( relative.getFileName()
					, folder / relative
					, *image.getPixels() );
			}
			catch ( castor::Exception & exc )
			{
				log::error << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: " ) << exc.what() << std::endl;
			}
			catch ( std::exception & exc )
			{
				log::error << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: " ) << exc.what() << std::endl;
			}
			catch ( ... )
			{
				log::error << cuT( "Error encountered while loading image file [" ) << path << cuT( "]: Unknown error" ) << std::endl;
			}
		}

		return result;
	}

	TextureUnitSPtr MeshImporter::loadTexture( castor::Path const & path
		, TextureConfiguration const & config )const
	{
		TextureUnitSPtr result;
		castor::Path relative;
		castor::Path folder;

		if ( findImage( path, m_filePath, folder, relative ) )
		{
			try
			{
				result = std::make_shared< TextureUnit >( *getEngine() );
				result->setTexture( createTextureLayout( *getEngine()
					, relative
					, folder
					, config.normalMask[0] == 0 ) );
				result->setConfiguration( config );
				return result;
			}
			catch ( castor::Exception & exc )
			{
				log::error << cuT( "Error encountered while loading texture file [" ) << path << cuT( "]: " ) << exc.what() << std::endl;
			}
			catch ( std::exception & exc )
			{
				log::error << cuT( "Error encountered while loading texture file [" ) << path << cuT( "]: " ) << exc.what() << std::endl;
			}
			catch ( ... )
			{
				log::error << cuT( "Error encountered while loading texture file [" ) << path << cuT( "]: Unknown error" ) << std::endl;
			}
		}

		return nullptr;
	}

	TextureUnitSPtr MeshImporter::loadTexture( castor::Path const & path
		, TextureConfiguration const & config
		, Pass & pass )const
	{
		auto result = loadTexture( path
			, config );

		if ( result )
		{
			pass.addTextureUnit( result );
		}

		return result;
	}

	bool MeshImporter::convertToNormalMap( castor::Path & path
		, castor3d::TextureConfiguration & config )const
	{
		auto result = false;

		if ( !path.empty() )
		{
			if ( auto image = loadImage( path ) )
			{
				log::info << "Converting height map to normal map." << std::endl;

				if ( castor::convertToNormalMap( 3.0f, *image ) )
				{
					config.normalMask[0] = 0x00FFFFFF;
					config.heightMask[0] = 0xFF000000;
					path = image->getPath();
					path = path.getPath() / ( "N_" + path.getFileName() + ".png" );
					getEngine()->getImageWriter().write( path, image->getPxBuffer() );
				}
			}
		}

		return result;
	}
}
