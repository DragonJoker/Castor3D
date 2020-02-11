#include "Castor3D/Binary/CmshImporter.hpp"

#include "Castor3D/Binary/BinaryMesh.hpp"
#include "Castor3D/Binary/BinarySkeleton.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"

#include <CastorUtils/Data/BinaryFile.hpp>

using namespace castor;

namespace castor3d
{
	String const CmshImporter::Type = cuT( "cmsh" );

	CmshImporter::CmshImporter( Engine & engine )
		: MeshImporter{ engine }
	{
	}

	MeshImporterUPtr CmshImporter::create( Engine & engine )
	{
		return std::make_unique< CmshImporter >( engine );
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
