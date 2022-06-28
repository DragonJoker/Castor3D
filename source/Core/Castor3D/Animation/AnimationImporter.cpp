#include "Castor3D/Animation/AnimationImporter.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

namespace castor3d
{
	namespace animimp
	{
		static void transformMeshAnimation( castor::Matrix4x4f const & transform
			, MeshAnimation & animation )
		{
			auto mesh = static_cast< Mesh * >( animation.getAnimable() );

			for ( auto submesh : *mesh )
			{
				if ( auto component = submesh->getComponent< MorphComponent >() )
				{
					for ( auto & target : component->getMorphTargetsBuffers() )
					{
						for ( auto & vertex : target.positions )
						{
							vertex = transform * vertex;
						}

						SubmeshUtils::computeNormals( target.positions
							, target.texcoords0
							, target.normals
							, target.tangents
							, static_cast< TriFaceMapping const & >( *submesh->getIndexMapping() ).getFaces()
							, true );
					}
				}
			}
		}

		static void transformPosition( castor::Point3f const & translate
			, castor::Point3f const & scale
			, castor::Quaternion const & rotation
			, castor::Point3f & result )
		{
			rotation.transform( scale * result, result );
			result += translate;
		}

		static void transformRotation( castor::Point3f const & scale
			, castor::Quaternion const & rotation
			, castor::Quaternion & result )
		{
			float x = std::copysign( 1.0f, scale->x );
			float y = std::copysign( 1.0f, scale->y );
			float z = std::copysign( 1.0f, scale->z );

			result.quat.x = y * z * result.quat.x;
			result.quat.y = x * z * result.quat.y;
			result.quat.z = x * y * result.quat.z;

			result *= rotation;
		}

		static void transformSkeletonAnimation( castor::Point3f const & scale
			, castor::Quaternion const & rotation
			, SkeletonAnimation & animation )
		{
			auto ident = castor::Quaternion::identity();

			for ( auto & keyFrame : animation )
			{
				for ( auto & transformIt : static_cast< SkeletonAnimationKeyFrame & >( *keyFrame ) )
				{
					if ( transformIt.object->getParent() )
					{
						transformPosition( {}, scale, ident, transformIt.transform.translate );
					}
					else
					{
						transformPosition( {}, scale, rotation, transformIt.transform.translate );
						transformRotation( scale, rotation, transformIt.transform.rotate );
					}
				}

				keyFrame->initialise();
			}
		}
	}

	AnimationImporter::AnimationImporter( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	bool AnimationImporter::import( SkeletonAnimation & skeleton
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		m_parameters = parameters;
		auto result = doImportSkeleton( skeleton );

		if ( result )
		{
			castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
			castor::Quaternion orientation{ castor::Quaternion::identity() };

			if ( parseImportParameters( m_parameters, scale, orientation ) )
			{
				animimp::transformSkeletonAnimation( scale
					, orientation
					, skeleton );
			}
		}

		return result;
	}

	bool AnimationImporter::import( SkeletonAnimation & animation
		, castor::Path const & path
		, Parameters const & parameters )
	{
		auto & engine = *animation.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, parameters );

		if ( auto importer = file->createAnimationImporter() )
		{
			return importer->import( animation, file.get(), parameters );
		}

		return false;
	}

	bool AnimationImporter::import( MeshAnimation & animation
		, ImporterFile * file
		, Parameters const & parameters )
	{
		bool splitSubmeshes = false;
		m_parameters.get( cuT( "split_mesh" ), splitSubmeshes );
		m_file = file;
		m_parameters = parameters;
		auto result = doImportMesh( animation );

		if ( result )
		{
			castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
			castor::Quaternion orientation{ castor::Quaternion::identity() };

			if ( parseImportParameters( m_parameters, scale, orientation ) )
			{
				castor::Matrix4x4f transform;
				castor::matrix::setRotate( transform, orientation );
				castor::matrix::scale( transform, scale );
				animimp::transformMeshAnimation( transform, animation );
			}
		}

		return result;
	}

	bool AnimationImporter::import( MeshAnimation & animation
		, castor::Path const & path
		, Parameters const & parameters )
	{
		auto & engine = *animation.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, parameters );

		if ( auto importer = file->createAnimationImporter() )
		{
			return importer->import( animation, file.get(), parameters );
		}

		return false;
	}

	bool AnimationImporter::import( SceneNodeAnimation & animation
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		m_parameters = parameters;
		return doImportNode( animation );
	}

	bool AnimationImporter::import( SceneNodeAnimation & animation
		, castor::Path const & path
		, Parameters const & parameters )
	{
		auto & engine = *animation.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, parameters );

		if ( auto importer = file->createAnimationImporter() )
		{
			return importer->import( animation, file.get(), parameters );
		}

		return false;
	}
}
