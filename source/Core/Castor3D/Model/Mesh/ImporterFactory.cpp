#include "Castor3D/Model/Mesh/ImporterFactory.hpp"

#include "Castor3D/Binary/CmshImporter.hpp"

CU_ImplementCUSmartPtr( castor3d, MeshImporterFactory )
CU_ImplementCUSmartPtr( castor3d, MeshSubdividerFactory )

namespace castor3d
{
	MeshImporterFactory::MeshImporterFactory()
		: Factory< MeshImporter, castor::String, MeshImporterUPtr, std::function< MeshImporterUPtr( Engine & ) > >{}
	{
		registerType( CmshImporter::MeshType, CmshImporter::create );
		registerType( CmshImporter::MeshAnimType, CmshImporter::create );
		registerType( CmshImporter::SkeletonType, CmshImporter::create );
		registerType( CmshImporter::SkeletonAnimType, CmshImporter::create );
		registerType( CmshImporter::NodeAnimType, CmshImporter::create );
	}

	MeshImporterFactory::~MeshImporterFactory()
	{
	}
}
