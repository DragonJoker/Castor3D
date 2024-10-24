#include "Castor3D/Model/Skeleton/SkeletonImporter.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"

CU_ImplementSmartPtr( castor3d, SkeletonImporter )

namespace castor3d
{
	namespace skelimp
	{
		static void transformSkeleton( castor::Matrix4x4f const & transform
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
		, castor::String const & prefix )
		: OwnedBy< Engine >{ engine }
		, m_prefix{ prefix + cuT( " - " ) }
	{
	}

	bool SkeletonImporter::import( Skeleton & skeleton
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		m_parameters = parameters;
		bool result = true;

		if ( skeleton.getNodes().empty() )
		{
			log::info << getPrefix() << cuT( "Loading Skeleton [" ) << skeleton.getName() << cuT( "]" ) << std::endl;
			result = doImportSkeleton( skeleton );

			if ( result )
			{
				castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
				castor::Quaternion orientation{ castor::Quaternion::identity() };

				if ( parseImportParameters( m_parameters, scale, orientation ) )
				{
					castor::Matrix4x4f transform;
					castor::matrix::setRotate( transform, orientation );
					castor::matrix::scale( transform, scale );
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

	bool SkeletonImporter::import( Skeleton & skeleton
		, castor::Path const & path
		, Parameters const & parameters )
	{
		auto & engine = *skeleton.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		castor::String preferredImporter = cuT( "any" );
		parameters.get( cuT( "preferred_importer" ), preferredImporter );
		auto file = engine.getImporterFileFactory().create( extension
			, preferredImporter
			, engine
			, path
			, parameters );

		if ( auto importer = file->createSkeletonImporter() )
		{
			return importer->import( skeleton, file.get(), parameters );
		}

		return false;
	}
}
