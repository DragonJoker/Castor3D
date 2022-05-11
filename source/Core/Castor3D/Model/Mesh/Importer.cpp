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
#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TangentsComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/TexcoordsComponent.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Graphics/HeightMapToNormalMap.hpp>

namespace castor3d
{
	namespace mshimp
	{
		static bool findImage( castor::Path const & path
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

		static void transformMesh( castor::Matrix4x4f const & transform
			, Mesh & mesh )
		{
			for ( auto submesh : mesh )
			{
				for ( auto & vertex : submesh->getPositions() )
				{
					vertex = transform * vertex;
				}

				static castor::Point3fArray tan;
				static castor::Point3fArray tex;
				castor::Point3fArray * tangents = &tan;
				castor::Point3fArray const * texcoords = &tex;

				if ( auto tanComp = submesh->getComponent< TangentsComponent >() )
				{
					tangents = &tanComp->getData();
				}

				if ( auto texComp = submesh->getComponent< TexcoordsComponent >() )
				{
					texcoords = &texComp->getData();
				}

				SubmeshUtils::computeNormals( submesh->getPositions()
					, *texcoords
					, submesh->getNormals()
					, *tangents
					, static_cast< TriFaceMapping const & >( *submesh->getIndexMapping() )
					, true );
			}
		}

		static void transformMeshAnimations( castor::Matrix4x4f const & transform
			, Mesh & mesh )
		{
			for ( auto & animIt : mesh.getAnimations() )
			{
				for ( auto & keyFrame : *animIt.second )
				{
					for ( auto & submeshIt : static_cast< MeshAnimationKeyFrame & >( *keyFrame ) )
					{
						for ( auto & vertex : submeshIt.second.positions )
						{
							vertex = transform * vertex;
						}

						SubmeshUtils::computeNormals( submeshIt.second.positions
							, submeshIt.second.texcoords
							, submeshIt.second.normals
							, submeshIt.second.tangents
							, static_cast< TriFaceMapping const & >( *mesh.getSubmesh( submeshIt.first )->getIndexMapping() )
							, true );
					}
				}
			}
		}

		static void transformSkeleton( castor::Matrix4x4f const & transform
			, Skeleton & skeleton )
		{
			auto invTransform = transform.getInverse();
			skeleton.setGlobalInverseTransform( transform * skeleton.getGlobalInverseTransform() * invTransform );

			for ( auto bone : skeleton.getBones() )
			{
				bone->setInverseTransform( transform * bone->getInverseTransform() * invTransform );
			}
		}

		static void transformSkeletonAnimations( castor::Matrix4x4f const & transform
			, Skeleton & skeleton )
		{
			//for ( auto & animIt : skeleton.getAnimations() )
			//{
			//	for ( auto & keyFrame : *animIt.second )
			//	{
			//		for ( auto & transformIt : static_cast< SkeletonAnimationKeyFrame & >( *keyFrame ).getTransforms() )
			//		{
			//			transformIt.second = transform * transformIt.second;
			//		}

			//		keyFrame->initialise();
			//	}
			//}
		}
	}

	MeshImporter::MeshImporter( Engine & engine )
		: OwnedBy< Engine >( engine )
		, m_fileName()
	{
	}

	bool MeshImporter::import( Skeleton & skeleton
		, castor::Path const & pathFile
		, Parameters const & parameters )
	{
		m_fileName = pathFile;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_animsOnly = false;
		bool result = true;

		if ( skeleton.getNodes().empty() )
		{
			result = doImportSkeleton( skeleton );

			if ( result )
			{
				log::info << "Loaded skeleton [" << skeleton.getName() << "]"
					<< " " << skeleton.getNodesCount() << " Node(s)"
					<< ", " << skeleton.getBonesCount() << " Bones(s)" << std::endl;
			}
		}

		return result;
	}

	bool MeshImporter::import( Mesh & mesh
		, castor::Path const & pathFile
		, Parameters const & parameters
		, bool forceImport )
	{
		bool splitSubmeshes = false;
		m_parameters.get( cuT( "split_mesh" ), splitSubmeshes );
		m_fileName = pathFile;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_animsOnly = false;
		bool result = true;

		if ( !mesh.getSubmeshCount() || forceImport )
		{
			result = doImportMesh( mesh );

			if ( result )
			{
				float value = 1.0f;
				castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
				castor::Quaternion orientation{ castor::Quaternion::identity() };
				bool needsTransform = false;

				if ( m_parameters.get( cuT( "rescale" ), value )
					&& std::abs( value ) > std::numeric_limits< float >::epsilon()
					&& std::abs( value - 1.0f ) > std::numeric_limits< float >::epsilon() )
				{
					scale = { value, value, value };
					needsTransform = true;
				}

				if ( m_parameters.get( cuT( "pitch" ), value )
					&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
				{
					auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }
						, castor::Angle::fromDegrees( value ) );
					orientation *= rot;
					needsTransform = true;
				}

				if ( m_parameters.get( cuT( "yaw" ), value )
					&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
				{
					auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }
						, castor::Angle::fromDegrees( value ) );
					orientation *= rot;
					needsTransform = true;
				}

				if ( m_parameters.get( cuT( "roll" ), value )
					&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
				{
					auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 0.0f, 1.0f }
						, castor::Angle::fromDegrees( value ) );
					orientation *= rot;
					needsTransform = true;
				}

				if ( needsTransform )
				{
					castor::Matrix4x4f transform;
					castor::matrix::setTransform( transform
						, castor::Point3f{}
						, scale
						, orientation );

					if ( !mesh.getSkeleton() )
					{
						mshimp::transformMesh( transform, mesh );
						mshimp::transformMeshAnimations( transform, mesh );

						if ( auto skeleton = mesh.getSkeleton() )
						{
							mshimp::transformSkeleton( transform, *skeleton );
							mshimp::transformSkeletonAnimations( transform, *skeleton );
						}
					}
				}

				mesh.computeContainers();
				log::info << "Loaded mesh [" << mesh.getName() << "]"
					<< " AABB (" << print( mesh.getBoundingBox() ) << ")"
					<< ", " << mesh.getVertexCount() << " vertices"
					<< ", " << mesh.getSubmeshCount() << " submeshes" << std::endl;
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
		, castor::Path const & pathFile
		, Parameters const & parameters
		, std::map< TextureFlag, TextureConfiguration > const & textureRemaps )
	{
		m_textureRemaps = textureRemaps;
		m_fileName = pathFile;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_animsOnly = false;
		bool result = doImportScene( scene );

		if ( result )
		{
			float value = 1.0f;
			castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
			castor::Quaternion orientation{ castor::Quaternion::identity() };
			bool needsTransform = false;

			if ( m_parameters.get( cuT( "rescale" ), value )
				&& std::abs( value ) > std::numeric_limits< float >::epsilon()
				&& std::abs( value - 1.0f ) > std::numeric_limits< float >::epsilon() )
			{
				scale = { value, value, value };
				needsTransform = true;
			}

			if ( m_parameters.get( cuT( "pitch" ), value )
				&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
			{
				auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 1.0f, 0.0f, 0.0f }
					, castor::Angle::fromDegrees( value ) );
				orientation *= rot;
				needsTransform = true;
			}

			if ( m_parameters.get( cuT( "yaw" ), value )
				&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
			{
				auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }
					, castor::Angle::fromDegrees( value ) );
				orientation *= rot;
				needsTransform = true;
			}

			if ( m_parameters.get( cuT( "roll" ), value )
				&& std::abs( value ) > std::numeric_limits< float >::epsilon() )
			{
				auto rot = castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 0.0f, 1.0f }
					, castor::Angle::fromDegrees( value ) );
				orientation *= rot;
				needsTransform = true;
			}

			if ( needsTransform )
			{
				auto transformNode = scene.addNewSceneNode( pathFile.getFileName() + "TransformNode" ).lock();
				transformNode->setScale( scale );
				transformNode->setOrientation( orientation );
				transformNode->attachTo( *scene.getObjectRootNode() );

				for ( auto & node : m_nodes )
				{
					if ( node->getParent() == scene.getObjectRootNode().get() )
					{
						node->attachTo( *transformNode );
					}
				}
			}

			for ( auto meshIt : m_meshes )
			{
				auto mesh = meshIt.second.lock();
				mesh->computeContainers();
				log::info << "Loaded mesh [" << mesh->getName() << "]"
					<< " AABB (" << print( mesh->getBoundingBox() ) << ")"
					<< ", " << mesh->getVertexCount() << " vertices"
					<< ", " << mesh->getSubmeshCount() << " submeshes" << std::endl;
			}
		}

		return result;
	}

	bool MeshImporter::importAnimations( Skeleton & skeleton
		, castor::Path const & pathFile
		, Parameters const & parameters )
	{
		m_fileName = pathFile;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_animsOnly = true;
		return doImportSkeleton( skeleton );
	}

	bool MeshImporter::importAnimations( Mesh & mesh
		, castor::Path const & pathFile
		, Parameters const & parameters )
	{
		m_fileName = pathFile;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_animsOnly = true;
		return doImportMesh( mesh );
	}

	bool MeshImporter::importAnimations( Scene & scene
		, castor::Path const & pathFile
		, Parameters const & parameters )
	{
		m_fileName = pathFile;
		m_filePath = m_fileName.getPath();
		m_parameters = parameters;
		m_animsOnly = true;
		return doImportScene( scene );
	}

	castor::ImageSPtr MeshImporter::loadImage( castor::String const & name
		, castor::ImageCreateParams const & params )const
	{
		castor::ImageSPtr result;

		try
		{
			auto & cache = getEngine()->getImageCache();
			auto image = cache.tryFind( name );

			if ( !image.lock() )
			{
				image = cache.add( name, params );
			}

			result = image.lock();
		}
		catch ( castor::Exception & exc )
		{
			log::error << cuT( "Error encountered while loading image file [" ) << name << cuT( "]: " ) << exc.what() << std::endl;
		}
		catch ( std::exception & exc )
		{
			log::error << cuT( "Error encountered while loading image file [" ) << name << cuT( "]: " ) << exc.what() << std::endl;
		}
		catch ( ... )
		{
			log::error << cuT( "Error encountered while loading image file [" ) << name << cuT( "]: Unknown error" ) << std::endl;
		}

		return result;
	}

	castor::ImageSPtr MeshImporter::loadImage( castor::Path const & path )const
	{
		castor::ImageSPtr result;
		castor::Path relative;
		castor::Path folder;

		if ( mshimp::findImage( path, m_filePath, folder, relative ) )
		{
			result = loadImage( relative.getFileName()
				, castor::ImageCreateParams{ folder / relative
					, { false, false, false } } );
		}

		return result;
	}

	castor::ImageSPtr MeshImporter::loadImage( castor::String name
		, castor::String type
		, castor::ByteArray data )const
	{
		return loadImage( name
			, castor::ImageCreateParams{ type
				, data
				, { false, false, false } } );
	}

	TextureSourceInfo MeshImporter::loadTexture( castor3d::SamplerRes sampler
		, castor::Path const & path
		, TextureConfiguration const & config )const
	{
		auto image = loadImage( path );

		if ( !image )
		{
			CU_Exception( "Couldn't find image at path [" + path + "]" );
		}

		bool allowCompression = config.normalMask[0] == 0;
		return TextureSourceInfo{ sampler
			, image->getPath().getPath()
			, image->getPath().getFileName( true )
			, { allowCompression, true, true } };
	}

	TextureSourceInfo MeshImporter::loadTexture( castor3d::SamplerRes sampler
		, castor::String name
		, castor::String type
		, castor::ByteArray data
		, TextureConfiguration const & config )const
	{
		auto image = loadImage( name, type, data );

		if ( !image )
		{
			CU_Exception( "Couldn't load image [" + name + "]" );
		}

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
