#include "Castor3D/Scene/SceneImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/AnimationImporter.hpp"
#include "Castor3D/Animation/AnimationImporterFactory.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/MaterialImporter.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshImporter.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/SkeletonImporter.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/SceneNodeImporter.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/LightImporter.hpp"

namespace castor3d
{
	//*********************************************************************************************

	SceneImporter::SceneImporter( Engine & engine )
		: OwnedBy< Engine >{ engine }
	{
	}

	bool SceneImporter::import( Scene & scene
		, ImporterFile * file
		, Parameters const & parameters
		, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		m_file = file;
		doImportMaterials( scene, parameters, textureRemaps );
		auto skeletons = doImportSkeletons( scene );
		auto meshes = doImportMeshes( scene, skeletons );
		auto nodes = doImportNodes( scene );
		doImportLights( scene );
		doCreateGeometries( scene, meshes, nodes );
		importAnimations( scene, file, parameters );
		doTransformScene( scene, parameters, nodes );
		return true;
	}

	bool SceneImporter::import( Scene & scene
		, castor::Path const & path
		, Parameters const & parameters
		, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		auto & engine = *scene.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, parameters );
		return import( scene, file.get(), parameters, textureRemaps );
	}

	bool SceneImporter::importAnimations( Scene & scene
		, ImporterFile * file
		, Parameters const & parameters )
	{
		m_file = file;
		auto importer = file->createAnimationImporter();

		if ( importer )
		{
			std::map< castor::String, AnimObjects > anims;
			doImportSkeletonsAnims( scene, *importer, anims );
			doImportMeshesAnims( scene, *importer, anims );
			doImportNodesAnims( scene, *importer, anims );
			doCreateAnimationGroups( scene, anims );
		}

		return true;
	}

	bool SceneImporter::importAnimations( Scene & scene
		, castor::Path const & path
		, Parameters const & parameters )
	{
		auto & engine = *scene.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		auto file = engine.getImporterFileFactory().create( extension
			, engine
			, path
			, parameters );
		return importAnimations( scene, file.get(), parameters );
	}

	void SceneImporter::doImportMaterials( Scene & scene
		, Parameters const & parameters
		, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		if ( auto materialImporter = m_file->createMaterialImporter() )
		{
			for ( auto name : m_file->listMaterials() )
			{
				if ( !getOwner()->hasMaterial( name ) )
				{
					auto material = getOwner()->createMaterial( name
						, *getOwner()
						, getOwner()->getPassesType() );

					if ( materialImporter->import( *material
						, m_file
						, parameters
						, textureRemaps ) )
					{
						scene.getMaterialView().add( name, material, true );
					}
				}
			}
		}
	}

	std::map< castor::String, SkeletonRPtr > SceneImporter::doImportSkeletons( Scene & scene )
	{
		Parameters emptyParams;
		std::map< castor::String, SkeletonRPtr > result;

		if ( auto skeletonImporter = m_file->createSkeletonImporter() )
		{
			for ( auto name : m_file->listSkeletons() )
			{
				auto skeleton = scene.createSkeleton( name, scene );

				if ( skeletonImporter->import( *skeleton
					, m_file
					, emptyParams ) )
				{
					result.emplace( name, skeleton.get() );
					scene.addSkeleton( name, skeleton, true );
				}
			}
		}

		return result;
	}

	std::map< castor::String, MeshResPtr > SceneImporter::doImportMeshes( Scene & scene
		, std::map< castor::String, SkeletonRPtr > const & skeletons )
	{
		Parameters emptyParams;
		std::map< castor::String, MeshResPtr > result;

		if ( auto meshImporter = m_file->createMeshImporter() )
		{
			for ( auto [meshName, skeletonName] : m_file->listMeshes() )
			{
				auto mesh = scene.createMesh( meshName, scene );

				if ( meshImporter->import( *mesh
					, m_file
					, emptyParams
					, true ) )
				{
					if ( !skeletonName.empty() )
					{
						auto skelIt = skeletons.find( skeletonName );
						CU_Require( skelIt != skeletons.end() );
						mesh->setSkeleton( skelIt->second );
					}

					result.emplace( meshName, mesh );
					scene.addMesh( meshName, mesh, true );
				}
			}
		}

		return result;
	}

	std::map< castor::String, SceneNodeSPtr > SceneImporter::doImportNodes( Scene & scene )
	{
		Parameters emptyParams;
		std::map< castor::String, SceneNodeSPtr > result;

		if ( auto nodeImporter = m_file->createSceneNodeImporter() )
		{
			for ( auto name : m_file->listSceneNodes() )
			{
				if ( !scene.hasSceneNode( name ) )
				{
					auto node = scene.createSceneNode( name, scene );

					if ( nodeImporter->import( *node
						, m_file
						, emptyParams ) )
					{
						result.emplace( name, node );
						scene.addSceneNode( name, node, true );
					}
				}
			}
		}

		return result;
	}

	void SceneImporter::doImportLights( Scene & scene )
	{
		Parameters emptyParams;

		if ( auto lightImporter = m_file->createLightImporter() )
		{
			for ( auto name : m_file->listLights() )
			{
				auto light = scene.createLight( name.first
					, scene
					, *scene.getObjectRootNode()
					, scene.getLightsFactory()
					, name.second );

				if ( lightImporter->import( *light
					, m_file
					, emptyParams ) )
				{
					scene.addLight( name.first, light, true );
				}
			}
		}
	}

	void SceneImporter::doCreateGeometries( Scene & scene
		, std::map< castor::String, MeshResPtr > const & meshes
		, std::map< castor::String, SceneNodeSPtr > const & nodes )
	{
		for ( auto geom : m_file->listGeometries() )
		{
			auto meshIt = meshes.find( geom.mesh );
			CU_Require( meshIt != meshes.end() );
			auto nodeIt = nodes.find( geom.node );
			CU_Require( nodeIt != nodes.end() );
			auto geometry = scene.createGeometry( geom.name
				, scene
				, *nodeIt->second
				, meshIt->second );
			scene.addGeometry( geometry );
		}
	}

	void SceneImporter::doImportSkeletonsAnims( Scene & scene
		, AnimationImporter & importer
		, std::map< castor::String, AnimObjects > & anims )
	{
		Parameters emptyParams;

		for ( auto & skelIt : scene.getSkeletonCache() )
		{
			auto & skeleton = skelIt.second;

			for ( auto animName : m_file->listSkeletonAnimations( *skeleton ) )
			{
				auto animation = std::make_unique< SkeletonAnimation >( *skeleton, animName );

				if ( importer.import( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.emplace( animName, AnimObjects{} ).first->second;
					anim.skeletons.push_back( skeleton.get() );
					skeleton->addAnimation( std::move( animation ) );
				}
			}
		}
	}

	void SceneImporter::doImportMeshesAnims( Scene & scene
		, AnimationImporter & importer
		, std::map< castor::String, AnimObjects > & anims )
	{
		Parameters emptyParams;

		for ( auto & meshIt : scene.getMeshCache() )
		{
			auto & mesh = meshIt.second;

			for ( auto animName : m_file->listMeshAnimations( *mesh ) )
			{
				auto animation = std::make_unique< MeshAnimation >( *mesh, animName );

				if ( importer.import( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.emplace( animName, AnimObjects{} ).first->second;
					anim.meshes.push_back( mesh.get() );
					mesh->addAnimation( std::move( animation ) );
				}
			}
		}
	}

	void SceneImporter::doImportNodesAnims( Scene & scene
		, AnimationImporter & importer
		, std::map< castor::String, AnimObjects > & anims )
	{
		Parameters emptyParams;

		for ( auto & nodeIt : scene.getSceneNodeCache() )
		{
			auto & node = nodeIt.second;

			for ( auto animName : m_file->listSceneNodeAnimations( *node ) )
			{
				auto animation = std::make_unique< SceneNodeAnimation >( *node, animName );

				if ( importer.import( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.emplace( animName, AnimObjects{} ).first->second;
					anim.nodes.push_back( node.get() );
					node->addAnimation( std::move( animation ) );
				}
			}
		}
	}

	void SceneImporter::doCreateAnimationGroups( Scene & scene
		, std::map< castor::String, AnimObjects > & anims )
	{
		for ( auto & animIt : anims )
		{
			for ( auto geometry : scene.getGeometryCache() )
			{
				auto & mesh = *geometry.second->getMesh().lock();
				auto nodeIt = std::find( animIt.second.nodes.begin()
					, animIt.second.nodes.end()
					, geometry.second->getParent() );
				auto meshIt = std::find( animIt.second.meshes.begin()
					, animIt.second.meshes.end()
					, &mesh );
				auto skelIt = ( mesh.getSkeleton()
					? std::find( animIt.second.skeletons.begin()
						, animIt.second.skeletons.end()
						, mesh.getSkeleton() )
					: animIt.second.skeletons.end() );

				if ( nodeIt != animIt.second.nodes.end()
					|| meshIt != animIt.second.meshes.end()
					|| skelIt != animIt.second.skeletons.end() )
				{
					auto animGroup = ( scene.hasAnimatedObjectGroup( geometry.first )
						? scene.findAnimatedObjectGroup( geometry.first ).lock()
						: scene.addNewAnimatedObjectGroup( geometry.first, scene ).lock() );

					if ( animGroup->addAnimation( animIt.first ) )
					{
						animGroup->setAnimationLooped( animIt.first, true );
					}

					if ( nodeIt != animIt.second.nodes.end() )
					{
						animGroup->addObject( **nodeIt, ( *nodeIt )->getName() );
					}

					if ( meshIt != animIt.second.meshes.end() )
					{
						animGroup->addObject( **meshIt, *geometry.second, geometry.first );
					}

					if ( skelIt != animIt.second.skeletons.end() )
					{
						animGroup->addObject( **skelIt, mesh, *geometry.second, geometry.first );
					}
				}
			}
		}
	}

	void SceneImporter::doTransformScene( Scene & scene
		, Parameters const & parameters
		, std::map< castor::String, SceneNodeSPtr > const & nodes )
	{
		castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
		castor::Quaternion orientation{ castor::Quaternion::identity() };

		if ( parseImportParameters( parameters, scale, orientation ) )
		{
			auto transformNode = scene.addNewSceneNode( m_file->getName() + "TransformNode" ).lock();
			transformNode->setScale( scale );
			transformNode->setOrientation( orientation );
			transformNode->attachTo( *scene.getObjectRootNode() );

			for ( auto & nodeIt : nodes )
			{
				if ( nodeIt.second->getParent() == scene.getObjectRootNode().get() )
				{
					nodeIt.second->attachTo( *transformNode );
				}
			}
		}
	}

	//*********************************************************************************************
}
