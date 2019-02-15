#include "Castor3D/Mesh/CmshImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Binary/BinaryMesh.hpp"
#include "Castor3D/Binary/BinarySkeleton.hpp"
#include "Castor3D/Mesh/Mesh.hpp"
#include "Castor3D/Mesh/Skeleton/Skeleton.hpp"
#include "Castor3D/Scene/Scene.hpp"

using namespace castor;

namespace castor3d
{
	String const CmshImporter::Type = cuT( "cmsh" );

	CmshImporter::CmshImporter( Engine & engine )
		: Importer{ engine }
	{
	}

	ImporterUPtr CmshImporter::create( Engine & engine )
	{
		return std::make_unique< CmshImporter >( engine );
	}

	bool CmshImporter::doImportScene( Scene & scene )
	{
		CU_Exception( "WTF are you trying to do? There is no scene, in a cmsh file." );
	}

	bool CmshImporter::doImportMesh( Mesh & mesh )
	{
		BinaryFile file{ m_fileName, File::OpenMode::eRead };
		auto result = BinaryParser< Mesh >{}.parse( mesh, file );

		if ( result && File::fileExists( m_fileName.getPath() / ( m_fileName.getFileName() + cuT( ".cskl" ) ) ) )
		{
			auto skeleton = std::make_shared< Skeleton >( *mesh.getScene() );
			BinaryFile file{ m_fileName.getPath() / ( m_fileName.getFileName() + cuT( ".cskl" ) )
				, File::OpenMode::eRead };
			result = BinaryParser< Skeleton >{}.parse( *skeleton, file );

			if ( result )
			{
				mesh.setSkeleton( skeleton );
			}
		}

		return result;
	}
}
