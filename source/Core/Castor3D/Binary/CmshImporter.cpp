#include "Castor3D/Binary/CmshImporter.hpp"

#include "Castor3D/Binary/BinaryMesh.hpp"
#include "Castor3D/Binary/BinaryMeshAnimation.hpp"
#include "Castor3D/Binary/BinarySceneNodeAnimation.hpp"
#include "Castor3D/Binary/BinarySkeleton.hpp"
#include "Castor3D/Binary/BinarySkeletonAnimation.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

#include <CastorUtils/Data/BinaryFile.hpp>

namespace castor3d
{
	//*********************************************************************************************

	namespace cmshimp
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

	//*********************************************************************************************

	CmshImporterFile::CmshImporterFile( Engine & engine
		, Scene * scene
		, castor::Path const & path
		, Parameters const & parameters )
		: ImporterFile{ engine, scene, path, parameters }
	{
	}

	std::vector< castor::String > CmshImporterFile::listMaterials()
	{
		return std::vector< castor::String >{};
	}

	std::vector< std::pair< castor::String, castor::String > > CmshImporterFile::listMeshes()
	{
		std::vector< std::pair< castor::String, castor::String > > result;

		if ( getExtension() == CmshMeshImporter::Type )
		{
			result.emplace_back( getName(), castor::String{} );
		}

		return result;
	}

	std::vector< castor::String > CmshImporterFile::listSkeletons()
	{
		std::vector< castor::String > result;

		if ( getExtension() == CmshSkeletonImporter::Type )
		{
			result.push_back( getName() );
		}

		return result;
	}

	std::vector< castor::String > CmshImporterFile::listSceneNodes()
	{
		return std::vector< castor::String >{};
	}

	std::vector< std::pair< castor::String, LightType > > CmshImporterFile::listLights()
	{
		return std::vector< std::pair< castor::String, LightType > >{};
	}

	std::vector< CmshImporterFile::GeometryData > CmshImporterFile::listGeometries()
	{
		return std::vector< GeometryData >{};
	}

	std::vector< castor::String > CmshImporterFile::listMeshAnimations( Mesh const & mesh )
	{
		std::vector< castor::String > result;

		if ( getExtension() == CmshAnimationImporter::MeshAnimType )
		{
			auto meshName = mesh.getName();
			auto animName = getName();

			if ( getName().find( meshName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( meshName.size() ) );
				result.push_back( animName );
			}
		}

		return result;
	}

	std::vector< castor::String > CmshImporterFile::listSkeletonAnimations( Skeleton const & skeleton )
	{
		std::vector< castor::String > result;

		if ( getExtension() == CmshAnimationImporter::SkeletonAnimType )
		{
			auto skeletonName = skeleton.getName();
			auto animName = getName();

			if ( getName().find( skeletonName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( skeletonName.size() ) );
				result.push_back( animName );
			}
		}

		return result;
	}

	std::vector< castor::String > CmshImporterFile::listSceneNodeAnimations( castor3d::SceneNode const & node )
	{
		std::vector< castor::String > result;

		if ( getExtension() == CmshAnimationImporter::NodeAnimType )
		{
			auto nodeName = node.getName();
			auto animName = getName();

			if ( getName().find( nodeName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( nodeName.size() ) );
				result.push_back( animName );
			}
		}

		return result;
	}

	MaterialImporterUPtr CmshImporterFile::createMaterialImporter()
	{
		return nullptr;
	}

	AnimationImporterUPtr CmshImporterFile::createAnimationImporter()
	{
		return std::make_unique< CmshAnimationImporter >( *getOwner() );
	}

	SkeletonImporterUPtr CmshImporterFile::createSkeletonImporter()
	{
		return std::make_unique< CmshSkeletonImporter >( *getOwner() );
	}

	MeshImporterUPtr CmshImporterFile::createMeshImporter()
	{
		return std::make_unique< CmshMeshImporter >( *getOwner() );
	}

	SceneNodeImporterUPtr CmshImporterFile::createSceneNodeImporter()
	{
		return nullptr;
	}

	LightImporterUPtr CmshImporterFile::createLightImporter()
	{
		return nullptr;
	}

	ImporterFileUPtr CmshImporterFile::create( Engine & engine
		, Scene * scene
		, castor::Path const & path
		, Parameters const & parameters )
	{
		return ImporterFileUPtr( new CmshImporterFile{ engine, scene, path, parameters } );
	}

	//*********************************************************************************************

	castor::String const CmshMeshImporter::Type = cuT( "cmsh" );

	CmshMeshImporter::CmshMeshImporter( Engine & engine )
		: MeshImporter{ engine }
	{
	}

	MeshImporterUPtr CmshMeshImporter::create( Engine & engine )
	{
		return std::make_unique< CmshMeshImporter >( engine );
	}

	bool CmshMeshImporter::doImportMesh( Mesh & mesh )
	{
		castor::BinaryFile meshFile{ m_file->getFileName(), castor::File::OpenMode::eRead };
		return BinaryParser< Mesh >{}.parse( mesh, meshFile );
	}

	//*********************************************************************************************

	castor::String const CmshSkeletonImporter::Type = cuT( "cskl" );

	CmshSkeletonImporter::CmshSkeletonImporter( Engine & engine )
		: SkeletonImporter{ engine }
	{
	}

	SkeletonImporterUPtr CmshSkeletonImporter::create( Engine & engine )
	{
		return std::make_unique< CmshSkeletonImporter >( engine );
	}

	bool CmshSkeletonImporter::doImportSkeleton( Skeleton & skeleton )
	{
		castor::BinaryFile skelFile{ m_file->getFileName(), castor::File::OpenMode::eRead };
		return BinaryParser< Skeleton >{}.parse( skeleton, skelFile );
	}

	//*********************************************************************************************

	castor::String const CmshAnimationImporter::MeshAnimType = cuT( "cmsa" );
	castor::String const CmshAnimationImporter::SkeletonAnimType = cuT( "cska" );
	castor::String const CmshAnimationImporter::NodeAnimType = cuT( "csna" );

	CmshAnimationImporter::CmshAnimationImporter( Engine & engine )
		: AnimationImporter{ engine }
	{
	}

	AnimationImporterUPtr CmshAnimationImporter::create( Engine & engine )
	{
		return std::make_unique< CmshAnimationImporter >( engine );
	}

	bool CmshAnimationImporter::doImportSkeleton( SkeletonAnimation & animation )
	{
		castor::BinaryFile animFile{ m_file->getFileName(), castor::File::OpenMode::eRead };
		auto result = BinaryParser< SkeletonAnimation >{}.parse( animation, animFile );

		if ( result )
		{
			castor3d::log::info << cuT( "Loaded skeleton animation [" ) << animation.getName() << cuT( "] " )
				<< animation.getLength().count() << cuT( " ms, " )
				<< animation.size() << cuT( " Keyframes" ) << std::endl;
		}

		return result;
	}

	bool CmshAnimationImporter::doImportMesh( MeshAnimation & animation )
	{
		castor::BinaryFile animFile{ m_file->getFileName(), castor::File::OpenMode::eRead };
		auto result = BinaryParser< MeshAnimation >{}.parse( animation, animFile );

		if ( result )
		{
			castor3d::log::info << cuT( "Loaded mesh animation [" ) << animation.getName() << cuT( "] " )
				<< animation.getLength().count() << cuT( " ms, " )
				<< animation.size() << cuT( " Keyframes" ) << std::endl;
		}

		return result;
	}

	bool CmshAnimationImporter::doImportNode( SceneNodeAnimation & animation )
	{
		castor::BinaryFile animFile{ m_file->getFileName(), castor::File::OpenMode::eRead };
		auto result = BinaryParser< SceneNodeAnimation >{}.parse( animation, animFile );

		if ( result )
		{
			castor3d::log::info << cuT( "Loaded scene node animation [" ) << animation.getName() << cuT( "] " )
				<< animation.getLength().count() << cuT( " ms, " )
				<< animation.size() << cuT( " Keyframes" ) << std::endl;
		}

		return result;
	}

	//*********************************************************************************************
}
