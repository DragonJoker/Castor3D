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
		m_meshes.clear();
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

	bool MeshImporter::import( Scene & scene
		, castor::Path const & fileName
		, Parameters const & parameters
		, std::map< TextureFlag, TextureConfiguration > const & textureRemaps
		, bool initialise )
	{
		m_textureRemaps = textureRemaps;
		m_fileName = fileName;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_nodes.clear();
		m_geometries.clear();
		m_meshes.clear();
		bool result = doImportScene( scene );

		if ( result && initialise )
		{
			float value = 1.0f;

			if ( m_parameters.get( cuT( "rescale" ), value )
				&& std::abs( value ) > std::numeric_limits< float >::epsilon()
				&& std::abs( value - 1.0f ) > std::numeric_limits< float >::epsilon() )
			{
				auto scaleNode = scene.getSceneNodeCache().add( fileName.getFileName() + "ScaleNode", scene ).lock();
				scaleNode->setScale( { value, value, value } );
				scaleNode->attachTo( *scene.getObjectRootNode() );

				for ( auto & node : m_nodes )
				{
					if ( node->getParent() == scene.getObjectRootNode().get() )
					{
						node->attachTo( *scaleNode );
					}
				}
			}

			if ( m_parameters.get( cuT( "pitch" ), value )
				&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
			{
				auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }
					, castor::Angle::fromDegrees( value ) );

				for ( auto meshIt : m_meshes )
				{
					for ( auto submesh : *meshIt.second.lock() )
					{
						for ( auto & vertex : submesh->getPoints() )
						{
							rot.transform( vertex.pos, vertex.pos );
						}
					}
				}
			}

			if ( m_parameters.get( cuT( "yaw" ), value )
				&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
			{
				auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }
					, castor::Angle::fromDegrees( value ) );

				for ( auto meshIt : m_meshes )
				{
					for ( auto submesh : *meshIt.second.lock() )
					{
						for ( auto & vertex : submesh->getPoints() )
						{
							rot.transform( vertex.pos, vertex.pos );
						}
					}
				}
			}

			if ( m_parameters.get( cuT( "roll" ), value )
				&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
			{
				auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 0.0f, 1.0f }
					, castor::Angle::fromDegrees( value ) );

				for ( auto meshIt : m_meshes )
				{
					for ( auto submesh : *meshIt.second.lock() )
					{
						for ( auto & vertex : submesh->getPoints() )
						{
							rot.transform( vertex.pos, vertex.pos );
						}
					}
				}
			}

			for ( auto meshIt : m_meshes )
			{
				auto mesh = meshIt.second.lock();
				mesh->computeContainers();
				log::info << "Loaded mesh [" << mesh->getName() << "]"
					<< " AABB (" << mesh->getBoundingBox() << ")"
					<< ", " << mesh->getVertexCount() << " vertices"
					<< ", " << mesh->getSubmeshCount() << " submeshes" << std::endl;
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
					, { false, true, true } );
				result = castor::makeUnique< castor::Image >( relative.getFileName()
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

	TextureSourceInfo MeshImporter::loadTexture( castor3d::SamplerRes sampler
		, castor::Path const & path
		, TextureConfiguration const & config )const
	{
		castor::Path relative;
		castor::Path folder;

		if ( !findImage( path, m_filePath, folder, relative ) )
		{
			CU_Exception( "Couldn't find image at path [" + path + "]" );
		}

		bool allowCompression = config.normalMask[0] == 0;
		return TextureSourceInfo{ sampler
			, folder
			, relative
			, { allowCompression, true, true } };
	}

	TextureSourceInfo MeshImporter::loadTexture( castor3d::SamplerRes sampler
		, castor::String name
		, castor::String type
		, castor::ByteArray data
		, TextureConfiguration const & config )const
	{
		bool allowCompression = config.normalMask[0] == 0;
		return TextureSourceInfo{ sampler
			, std::move( name )
			, std::move( type )
			, std::move( data )
			, { allowCompression, true, true } };
	}

	void MeshImporter::loadTexture( castor3d::SamplerRes sampler
		, castor::Path const & path
		, PassTextureConfig const & config
		, Pass & pass )const
	{
		try
		{
			pass.registerTexture( loadTexture( sampler, path, config.config )
				, config );
		}
		catch ( std::exception & exc )
		{
			log::error << exc.what() << std::endl;
		}
	}

	void MeshImporter::loadTexture( castor3d::SamplerRes sampler
		, castor::String name
		, castor::String type
		, castor::ByteArray data
		, PassTextureConfig const & config
		, Pass & pass )const
	{
		try
		{
			pass.registerTexture( loadTexture( sampler
					, std::move( name )
					, std::move( type )
					, std::move( data )
					, config.config )
				, config );
		}
		catch ( std::exception & exc )
		{
			log::error << exc.what() << std::endl;
		}
	}

	bool MeshImporter::convertToNormalMap( castor::Path & path
		, TextureConfiguration & config )const
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
