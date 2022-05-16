#include "Castor3D/Animation/AnimationImporterFactory.hpp"

#include "Castor3D/Binary/CmshImporter.hpp"

CU_ImplementCUSmartPtr( castor3d, AnimationImporterFactory )

namespace castor3d
{
	AnimationImporterFactory::AnimationImporterFactory()
		: Factory< AnimationImporter
			, castor::String
			, AnimationImporterUPtr
			, std::function< AnimationImporterUPtr( Engine & ) > >{}
	{
		registerType( CmshAnimationImporter::MeshAnimType, CmshAnimationImporter::create );
		registerType( CmshAnimationImporter::SkeletonAnimType, CmshAnimationImporter::create );
		registerType( CmshAnimationImporter::NodeAnimType, CmshAnimationImporter::create );
	}
}
