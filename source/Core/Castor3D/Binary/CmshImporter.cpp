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

namespace c3d
{
	//*********************************************************************************************

	namespace cmshimp
	{
		static String cleanName( String name )
		{
			static String const seps = cuT( ",?;.:/\\!§*$£¤^¨&\"'([-|_@)°]=+} \t" );

			while ( !name.empty()
				&& seps.find( name[0] ) != String::npos )
			{
				name = name.substr( 1 );
			}

			while ( !name.empty()
				&& seps.find( name[name.size() - 1u] ) != String::npos )
			{
				name = name.substr( 1 );
			}

			return name;
		}
	}

	//*********************************************************************************************

	CmshImporterFile::CmshImporterFile( Engine & engine
		, Scene * scene
		, Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
		: ImporterFile{ engine, scene, path, parameters, progress }
	{
	}

	StringArray CmshImporterFile::listMaterials()
	{
		return StringArray{};
	}

	Vector< ImporterFile::MeshData > CmshImporterFile::listMeshes()
	{
		Vector< MeshData > result;

		if ( getExtension() == CmshMeshImporter::Type )
		{
			result.emplace_back( getInternalName( getName() ), String{} );
		}

		return result;
	}

	StringArray CmshImporterFile::listSkeletons()
	{
		StringArray result;

		if ( getExtension() == CmshSkeletonImporter::Type )
		{
			result.push_back( getInternalName( getName() ) );
		}

		return result;
	}

	Vector< ImporterFile::NodeData > CmshImporterFile::listSceneNodes()
	{
		return Vector< NodeData >{};
	}

	Vector< ImporterFile::LightData > CmshImporterFile::listLights()
	{
		return Vector< LightData >{};
	}

	Vector< ImporterFile::LightGroupData > CmshImporterFile::listLightGroups()
	{
		return Vector< LightGroupData >{};
	}

	Vector< CmshImporterFile::GeometryData > CmshImporterFile::listGeometries()
	{
		return Vector< GeometryData >{};
	}

	Vector< ImporterFile::CameraData > CmshImporterFile::listCameras()
	{
		return Vector< CameraData >{};
	}

	StringArray CmshImporterFile::listMeshAnimations( Mesh const & mesh )
	{
		StringArray result;

		if ( getExtension() == CmshAnimationImporter::MeshAnimType )
		{
			String const & meshName = mesh.getName();
			auto animName = getInternalName( getName() );

			if ( animName.find( meshName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( meshName.size() ) );
				result.emplace_back( animName );
			}
		}

		return result;
	}

	StringArray CmshImporterFile::listSkeletonAnimations( Skeleton const & skeleton )
	{
		StringArray result;

		if ( getExtension() == CmshAnimationImporter::SkeletonAnimType )
		{
			String const & skeletonName = skeleton.getName();
			auto animName = getInternalName( getName() );

			if ( animName.find( skeletonName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( skeletonName.size() ) );
				result.emplace_back( animName );
			}
		}

		return result;
	}

	StringArray CmshImporterFile::listSceneNodeAnimations( SceneNode const & node )
	{
		StringArray result;

		if ( getExtension() == CmshAnimationImporter::NodeAnimType )
		{
			String const & nodeName = node.getName();
			auto animName = getInternalName( getName() );

			if ( animName.find( nodeName ) == 0u )
			{
				animName = cmshimp::cleanName( animName.substr( nodeName.size() ) );
				result.emplace_back( animName );
			}
		}

		return result;
	}

	Vector< uint32_t > CmshImporterFile::listTextureAnimations( Material const & material
		, uint32_t pass )
	{
		Vector< uint32_t > result;
		return result;
	}

	uint32_t CmshImporterFile::countAllMeshAnimations()const
	{
		uint32_t result{};

		if ( getExtension() == CmshAnimationImporter::MeshAnimType )
		{
			++result;
		}

		return result;
	}

	uint32_t CmshImporterFile::countAllSkeletonAnimations()const
	{
		uint32_t result{};

		if ( getExtension() == CmshAnimationImporter::SkeletonAnimType )
		{
			++result;
		}

		return result;
	}

	uint32_t CmshImporterFile::countAllSceneNodeAnimations()const
	{
		uint32_t result{};

		if ( getExtension() == CmshAnimationImporter::NodeAnimType )
		{
			++result;
		}

		return result;
	}

	uint32_t CmshImporterFile::countAllTextureAnimations()const
	{
		return 0u;
	}

	MaterialImporterUPtr CmshImporterFile::createMaterialImporter()
	{
		return nullptr;
	}

	AnimationImporterUPtr CmshImporterFile::createAnimationImporter()
	{
		return makeUniqueDerived< AnimationImporter, CmshAnimationImporter >( *getOwner() );
	}

	SkeletonImporterUPtr CmshImporterFile::createSkeletonImporter()
	{
		return makeUniqueDerived< SkeletonImporter, CmshSkeletonImporter >( *getOwner() );
	}

	MeshImporterUPtr CmshImporterFile::createMeshImporter()
	{
		return makeUniqueDerived< MeshImporter, CmshMeshImporter >( *getOwner() );
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
		, Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
	{
		return ImporterFileUPtr( new CmshImporterFile{ engine, scene, path, parameters, progress } );
	}

	//*********************************************************************************************

	String const CmshMeshImporter::Type = cuT( "cmsh" );

	CmshMeshImporter::CmshMeshImporter( Engine & engine )
		: MeshImporter{ engine, "Castor" }
	{
	}

	MeshImporterUPtr CmshMeshImporter::create( Engine & engine )
	{
		return makeUniqueDerived< MeshImporter, CmshMeshImporter >( engine );
	}

	bool CmshMeshImporter::doImportMesh( Mesh & mesh, uint32_t submeshIndex )
	{
		BinaryFile meshFile{ m_file->getFileName(), File::OpenMode::eRead };
		return BinaryParser< Mesh >{ submeshIndex }.parse( mesh, meshFile );
	}

	//*********************************************************************************************

	String const CmshSkeletonImporter::Type = cuT( "cskl" );

	CmshSkeletonImporter::CmshSkeletonImporter( Engine & engine )
		: SkeletonImporter{ engine, "Castor" }
	{
	}

	SkeletonImporterUPtr CmshSkeletonImporter::create( Engine & engine )
	{
		return makeUniqueDerived< SkeletonImporter, CmshSkeletonImporter >( engine );
	}

	bool CmshSkeletonImporter::doImportSkeleton( Skeleton & skeleton )
	{
		BinaryFile skelFile{ m_file->getFileName(), File::OpenMode::eRead };
		return BinaryParser< Skeleton >{}.parse( skeleton, skelFile );
	}

	//*********************************************************************************************

	String const CmshAnimationImporter::MeshAnimType = cuT( "cmsa" );
	String const CmshAnimationImporter::SkeletonAnimType = cuT( "cska" );
	String const CmshAnimationImporter::NodeAnimType = cuT( "csna" );

	CmshAnimationImporter::CmshAnimationImporter( Engine & engine )
		: AnimationImporter{ engine, "Castor" }
	{
	}

	AnimationImporterUPtr CmshAnimationImporter::create( Engine & engine )
	{
		return makeUniqueDerived< AnimationImporter, CmshAnimationImporter >( engine );
	}

	bool CmshAnimationImporter::doImportSkeleton( SkeletonAnimation & animation )
	{
		BinaryFile animFile{ m_file->getFileName(), File::OpenMode::eRead };
		return BinaryParser< SkeletonAnimation >{}.parse( animation, animFile );
	}

	bool CmshAnimationImporter::doImportMesh( MeshAnimation & animation )
	{
		BinaryFile animFile{ m_file->getFileName(), File::OpenMode::eRead };
		return BinaryParser< MeshAnimation >{}.parse( animation, animFile );
	}

	bool CmshAnimationImporter::doImportNode( SceneNodeAnimation & animation )
	{
		BinaryFile animFile{ m_file->getFileName(), File::OpenMode::eRead };
		return BinaryParser< SceneNodeAnimation >{}.parse( animation, animFile );
	}

	bool CmshAnimationImporter::doImportTexture( TextureAnimation & animation )
	{
		return true;
	}

	//*********************************************************************************************
}
