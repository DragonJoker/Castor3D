#include "Castor3D/Binary/CmshImporter.hpp"

#include "Castor3D/Binary/BinaryMesh.hpp"
#include "Castor3D/Binary/BinaryMeshAnimation.hpp"
#include "Castor3D/Binary/BinarySkeleton.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimation.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"

#include <CastorUtils/Data/BinaryFile.hpp>

namespace castor3d
{
	namespace binmshimp
	{
		static castor::String cleanName( castor::String name )
		{
			static castor::String const seps = cuT( ",?;.:/\\!§*$£¤^¨&\"'([-|_@)°]=+} \t" );

			while ( !name.empty()
				&& seps.find( name[0] ) != castor::String::npos )
			{
				name = name.substr( 1 );
			}

			while ( !name.empty()
				&& seps.find( name[name.size() - 1u] ) != castor::String::npos )
			{
				name = name.substr( 1 );
			}

			return name;
		}
	}

	castor::String const CmshImporter::Type = cuT( "cmsh" );

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
		castor::BinaryFile meshFile{ m_fileName, castor::File::OpenMode::eRead };
		auto result = BinaryParser< Mesh >{}.parse( mesh, meshFile );

		castor::PathArray files;
		castor::File::listDirectoryFiles( m_fileName.getPath(), files );
		auto meshName = m_fileName.getFileName();

		for ( auto & fileName : files )
		{
			if ( fileName.getExtension() == "cskl"
				&& fileName.getFileName() == meshName )
			{
				auto skeleton = std::make_shared< Skeleton >( *mesh.getScene() );
				castor::BinaryFile skelFile{ m_fileName.getPath() / ( meshName + cuT( ".cskl" ) )
					, castor::File::OpenMode::eRead };
				result = BinaryParser< Skeleton >{}.parse( *skeleton, skelFile );

				if ( result )
				{
					mesh.setSkeleton( skeleton );
				}

				for ( auto & animFileName : files )
				{
					if ( animFileName.getExtension() == "cska"
						&& animFileName.getFileName().find( meshName ) == 0u )
					{
						auto animName = binmshimp::cleanName( animFileName.getFileName().substr( meshName.size() ) );
						auto animation = std::make_unique< SkeletonAnimation >( *skeleton, animName );
						castor::BinaryFile animFile{ animFileName, castor::File::OpenMode::eRead };
						result = BinaryParser< SkeletonAnimation >{}.parse( *animation, animFile );

						if ( result )
						{
							skeleton->addAnimation( std::move( animation ) );
						}
					}
				}
			}

			if ( fileName.getExtension() == "cmsa"
				&& fileName.getFileName().find( meshName ) == 0u )
			{
				auto animName = binmshimp::cleanName( fileName.getFileName().substr( meshName.size() ) );
				auto animation = std::make_unique< MeshAnimation >( mesh, animName );
				castor::BinaryFile animFile{ fileName, castor::File::OpenMode::eRead };
				result = BinaryParser< MeshAnimation >{}.parse( *animation, animFile );

				if ( result )
				{
					mesh.addAnimation( std::move( animation ) );
				}
			}
		}

		return result;
	}
}
