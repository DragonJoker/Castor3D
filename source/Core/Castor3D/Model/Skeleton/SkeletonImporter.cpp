#include "Castor3D/Model/Skeleton/SkeletonImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/ImporterFile.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Scene/Scene.hpp"

CU_ImplementSmartPtr( c3d, SkeletonImporter )

namespace c3d
{
	namespace skelimp
	{
		static void transformSkeleton( Matrix4x4f const & transform
			, Skeleton & skeleton )
		{
			auto invTransform = transform.getInverse();
			skeleton.setGlobalInverseTransform( transform * ( skeleton.getGlobalInverseTransform() * invTransform ) );

			for ( auto bone : skeleton.getBones() )
			{
				bone->setInverseTransform( transform * ( bone->getInverseTransform() * invTransform ) );
			}
		}
	}

	SkeletonImporter::SkeletonImporter( Engine & engine
		, String const & prefix )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
	{
	}

	SkeletonRes SkeletonImporter::importData( String const & name
		, Scene & scene
		, ImporterFile * file
		, ImportConfig const & config )
	{
		if ( !m_file )
		{
			m_file = file;
		}

		auto result = doCreateSkeleton( name, scene );

		if ( !result
			|| !importData( *result, file, config ) )
		{
			return nullptr;
		}

		return result;
	}

	bool SkeletonImporter::importData( Skeleton & skeleton
		, ImporterFile * file
		, ImportConfig const & config )
	{
		m_file = file;
		bool result = true;

		if ( skeleton.getNodes().empty() )
		{
			log::info << getPrefix() << cuT( "Loading Skeleton [" ) << skeleton.getName() << cuT( "]" ) << std::endl;
			result = doImportSkeleton( skeleton );

			if ( result )
			{
				Point3f scale{ 1.0f, 1.0f, 1.0f };
				Quaternion orientation{ Quaternion::identity() };

				if ( parseImportParameters( config, scale, orientation ) )
				{
					Matrix4x4f transform;
					matrix::setRotate( transform, orientation );
					matrix::scale( transform, scale );
					skelimp::transformSkeleton( transform, skeleton );
				}

				log::info << getPrefix() << "Loaded Skeleton [" << skeleton.getName() << "]"
					<< " " << skeleton.getNodesCount() << " Node(s)"
					<< ", " << skeleton.getBonesCount() << " Bones(s)"
					<< ", " << cuT( "Root Node [" ) << skeleton.getRootNode()->getName() << cuT( "]" ) << std::endl;
			}
			else
			{
				log::info << getPrefix() << cuT( "Couldn't load Skeleton [" ) << skeleton.getName() << cuT( "]" ) << std::endl;
			}
		}

		return result;
	}

	bool SkeletonImporter::importData( Skeleton & skeleton
		, Path const & path
		, ImportConfig const & config )
	{
		auto & engine = *skeleton.getEngine();
		auto extension = string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, config );

		if ( auto importer = file->createSkeletonImporter() )
		{
			return importer->importData( skeleton, file.get(), config );
		}

		return false;
	}

	SkeletonRes SkeletonImporter::doCreateSkeleton( String const & name
			, Scene & scene )
	{
		return scene.createSkeleton( name, scene );
	}
}
