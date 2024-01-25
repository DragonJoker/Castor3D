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
		, Parameters const & parameters
		, ProgressBar * progress )
		: ImporterFile{ engine, scene, path, parameters, progress }
	{
	}

	castor::StringArray CmshImporterFile::listMaterials()
	{
		return castor::StringArray{};
	}

	castor::Vector< ImporterFile::MeshData > CmshImporterFile::listMeshes()
	{
		castor::Vector< MeshData > result;

		if ( getExtension() == CmshMeshImporter::Type )
		{
			result.emplace_back( getName(), castor::String{} );
		}

		return result;
	}

	castor::StringArray CmshImporterFile::listSkeletons()
	{
		castor::StringArray result;

		if ( getExtension() == CmshSkeletonImporter::Type )
		{
			result.push_back( getName() );
		}

		return result;
	}

	castor::Vector< ImporterFile::NodeData > CmshImporterFile::listSceneNodes()
	{
		return castor::Vector< NodeData >{};
	}

	castor::Vector< ImporterFile::LightData > CmshImporterFile::listLights()
	{
		return castor::Vector< LightData >{};
	}

	castor::Vector< CmshImporterFile::GeometryData > CmshImporterFile::listGeometries()
	{
		return castor::Vector< GeometryData >{};
	}

	castor::Vector< ImporterFile::CameraData > CmshImporterFile::listCameras()
	{
		return castor::Vector< CameraData >{};
	}

	castor::StringArray CmshImporterFile::listMeshAnimations( Mesh const & mesh )
	{
		castor::StringArray result;

		if ( getExtension() == CmshAnimationImporter::MeshAnimType )
		{
			auto meshName = mesh.getName();
			auto animName = getName();

			if ( getName().find( meshName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( meshName.size() ) );
				result.emplace_back( animName );
			}
		}

		return result;
	}

	castor::StringArray CmshImporterFile::listSkeletonAnimations( Skeleton const & skeleton )
	{
		castor::StringArray result;

		if ( getExtension() == CmshAnimationImporter::SkeletonAnimType )
		{
			auto skeletonName = skeleton.getName();
			auto animName = getName();

			if ( getName().find( skeletonName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( skeletonName.size() ) );
				result.emplace_back( animName );
			}
		}

		return result;
	}

	castor::StringArray CmshImporterFile::listSceneNodeAnimations( castor3d::SceneNode const & node )
	{
		castor::StringArray result;

		if ( getExtension() == CmshAnimationImporter::NodeAnimType )
		{
			auto nodeName = node.getName();
			auto animName = getName();

			if ( getName().find( nodeName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( nodeName.size() ) );
				result.emplace_back( animName );
			}
		}

		return result;
	}

	castor::StringArray CmshImporterFile::listAllMeshAnimations()
	{
		castor::StringArray result;

		if ( getExtension() == CmshAnimationImporter::MeshAnimType )
		{
			result.emplace_back( getName() );
		}

		return result;
	}

	castor::StringArray CmshImporterFile::listAllSkeletonAnimations()
	{
		castor::StringArray result;

		if ( getExtension() == CmshAnimationImporter::SkeletonAnimType )
		{
			result.emplace_back( getName() );
		}

		return result;
	}

	castor::StringArray CmshImporterFile::listAllSceneNodeAnimations()
	{
		castor::StringArray result;

		if ( getExtension() == CmshAnimationImporter::NodeAnimType )
		{
			result.emplace_back( getName() );
		}

		return result;
	}

	MaterialImporterUPtr CmshImporterFile::createMaterialImporter()
	{
		return nullptr;
	}

	AnimationImporterUPtr CmshImporterFile::createAnimationImporter()
	{
		return castor::makeUniqueDerived< AnimationImporter, CmshAnimationImporter >( *getOwner() );
	}

	SkeletonImporterUPtr CmshImporterFile::createSkeletonImporter()
	{
		return castor::makeUniqueDerived< SkeletonImporter, CmshSkeletonImporter >( *getOwner() );
	}

	MeshImporterUPtr CmshImporterFile::createMeshImporter()
	{
		return castor::makeUniqueDerived< MeshImporter, CmshMeshImporter >( *getOwner() );
	}

	SceneNodeImporterUPtr CmshImporterFile::createSceneNodeImporter()
	{
		return nullptr;
	}

	LightImporterUPtr CmshImporterFile::createLightImporter()
	{
		return nullptr;
	}

	CameraImporterUPtr CmshImporterFile::createCameraImporter()
	{
		return nullptr;
	}

	ImporterFileUPtr CmshImporterFile::create( Engine & engine
		, Scene * scene
		, castor::Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
	{
		return ImporterFileUPtr( new CmshImporterFile{ engine, scene, path, parameters, progress } );
	}

	//*********************************************************************************************

	castor::String const CmshMeshImporter::Type = cuT( "cmsh" );

	CmshMeshImporter::CmshMeshImporter( Engine & engine )
		: MeshImporter{ engine }
	{
	}

	MeshImporterUPtr CmshMeshImporter::create( Engine & engine )
	{
		return castor::makeUniqueDerived< MeshImporter, CmshMeshImporter >( engine );
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
		return castor::makeUniqueDerived< SkeletonImporter, CmshSkeletonImporter >( engine );
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
		return castor::makeUniqueDerived< AnimationImporter, CmshAnimationImporter >( engine );
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
