#include "Castor3D/Scene/SceneImporter.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Animation/AnimationImporter.hpp"
#include "Castor3D/Animation/AnimationImporterFactory.hpp"
#include "Castor3D/Binary/CmshImporter.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/MaterialImporter.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/TextureConfiguration.hpp"
#include "Castor3D/Material/Texture/TextureLayout.hpp"
#include "Castor3D/Material/Texture/TextureUnit.hpp"
#include "Castor3D/Material/Texture/TextureView.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshImporter.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/SubmeshUtils.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/SkeletonImporter.hpp"
#include "Castor3D/Model/Skeleton/BoneNode.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimationObject.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

#include <CastorUtils/Graphics/HeightMapToNormalMap.hpp>

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
		, std::map< TextureFlag, TextureConfiguration > const & textureRemaps )
	{
		m_file = file;
		auto animationImporter = file->createAnimationImporter();
		Parameters emptyParams;
		std::map< castor::String, SkeletonRPtr > skeletons;
		std::map< castor::String, MeshResPtr > meshes;
		std::map< castor::String, SceneNodeSPtr > nodes;

		if ( auto materialImporter = file->createMaterialImporter() )
		{
			for ( auto name : file->listMaterials() )
			{
				auto material = getOwner()->createMaterial( name
					, *getOwner()
					, getOwner()->getPassesType() );

				if ( materialImporter->import( *material
					, file
					, emptyParams
					, textureRemaps ) )
				{
					scene.getMaterialView().add( name, material, true );
				}
			}
		}

		if ( auto skeletonImporter = file->createSkeletonImporter() )
		{
			for ( auto name : file->listSkeletons() )
			{
				auto skeleton = scene.createSkeleton( name, scene );

				if ( skeletonImporter->import( *skeleton
					, file
					, emptyParams ) )
				{
					if ( animationImporter )
					{
						for ( auto animName : file->listSkeletonAnimations( *skeleton ) )
						{
							auto animation = std::make_unique< SkeletonAnimation >( *skeleton, animName );

							if ( animationImporter->import( *animation, m_file, emptyParams ) )
							{
								skeleton->addAnimation( std::move( animation ) );
							}
						}
					}

					skeletons.emplace( name, skeleton.get() );
					scene.addSkeleton( name, skeleton, true );
				}
			}
		}

		if ( auto meshImporter = file->createMeshImporter() )
		{
			for ( auto [meshName, skeletonName] : file->listMeshes() )
			{
				auto mesh = scene.createMesh( meshName, scene );

				if ( meshImporter->import( *mesh
					, file
					, emptyParams
					, true ) )
				{

					if ( animationImporter )
					{
						for ( auto animName : file->listMeshAnimations( *mesh ) )
						{
							auto animation = std::make_unique< MeshAnimation >( *mesh, animName );

							if ( animationImporter->import( *animation, m_file, emptyParams ) )
							{
								mesh->addAnimation( std::move( animation ) );
							}
						}
					}

					if ( !skeletonName.empty() )
					{
						auto skelIt = skeletons.find( skeletonName );
						CU_Require( skelIt != skeletons.end() );
						mesh->setSkeleton( skelIt->second );
					}

					meshes.emplace( meshName, mesh );
					scene.addMesh( meshName, mesh, true );
				}
			}
		}

		std::map< castor::String, std::vector< SceneNodeRPtr > > nodesAnims;

		if ( auto nodeImporter = file->createSceneNodeImporter() )
		{
			for ( auto name : file->listSceneNodes() )
			{
				if ( !scene.hasSceneNode( name ) )
				{
					auto node = scene.createSceneNode( name, scene );

					if ( nodeImporter->import( *node
						, file
						, emptyParams ) )
					{
						if ( animationImporter )
						{
							for ( auto animName : file->listSceneNodeAnimations( *node ) )
							{
								auto animation = std::make_unique< SceneNodeAnimation >( *node, animName );

								if ( animationImporter->import( *animation, m_file, emptyParams ) )
								{
									auto & nodesAnim = nodesAnims.emplace( animName, std::vector< SceneNodeRPtr >{} ).first->second;
									nodesAnim.push_back( node.get() );
									node->addAnimation( std::move( animation ) );
								}
							}
						}

						nodes.emplace( name, node );
						scene.addSceneNode( name, node, true );
					}
				}
			}

			for ( auto & nodesIt : nodesAnims )
			{
				auto animGroup = scene.addNewAnimatedObjectGroup( cuT( "Node" ) + nodesIt.first, scene ).lock();

				if ( animGroup->addAnimation( nodesIt.first ) )
				{
					animGroup->setAnimationLooped( nodesIt.first, true );
				}

				for ( auto node : nodesIt.second )
				{
					animGroup->addObject( *node, node->getName() );
				}
			}
		}

		if ( auto lightImporter = file->createLightImporter() )
		{
			for ( auto name : file->listLights() )
			{
				auto light = scene.createLight( name.first
					, scene
					, *scene.getObjectRootNode()
					, scene.getLightsFactory()
					, name.second );

				if ( lightImporter->import( *light
					, file
					, emptyParams ) )
				{
					scene.addLight( name.first, light, true );
				}
			}
		}

		for ( auto geom : file->listGeometries() )
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
			doAddAnimationGroup( *geometry );
		}

		doTransformScene( scene, parameters, nodes );
		return true;
	}

	bool SceneImporter::import( Scene & scene
		, castor::Path const & path
		, Parameters const & parameters
		, std::map< TextureFlag, TextureConfiguration > const & textureRemaps )
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
		bool result{ true };
		auto animationImporter = file->createAnimationImporter();
		Parameters emptyParams;

		if ( animationImporter )
		{
			std::map< MeshRPtr, castor::StringArray > meshAnims;
			std::map< SkeletonRPtr, castor::StringArray > skelAnims;

			for ( auto & meshIt : scene.getMeshCache() )
			{
				auto & mesh = meshIt.second;

				for ( auto animName : file->listMeshAnimations( *mesh ) )
				{
					auto it = meshAnims.emplace( mesh.get(), castor::StringArray{} ).first;
					auto animation = std::make_unique< MeshAnimation >( *mesh, animName );

					if ( animationImporter->import( *animation, m_file, emptyParams ) )
					{
						it->second.push_back( animation->getName() );
						mesh->addAnimation( std::move( animation ) );
					}
				}
			}

			for ( auto & skelIt : scene.getSkeletonCache() )
			{
				auto & skeleton = skelIt.second;

				for ( auto animName : file->listSkeletonAnimations( *skeleton ) )
				{
					auto it = skelAnims.emplace( skeleton.get(), castor::StringArray{} ).first;
					auto animation = std::make_unique< SkeletonAnimation >( *skeleton, animName );

					if ( animationImporter->import( *animation, m_file, emptyParams ) )
					{
						it->second.push_back( animation->getName() );
						skeleton->addAnimation( std::move( animation ) );
					}
				}
			}

			for ( auto geometry : scene.getGeometryCache() )
			{
				auto & mesh = *geometry.second->getMesh().lock();
				auto meshIt = meshAnims.find( &mesh );
				auto skelIt = mesh.getSkeleton()
					? skelAnims.find( mesh.getSkeleton() )
					: skelAnims.end();

				if ( meshIt != meshAnims.end()
					|| skelIt != skelAnims.end() )
				{
					doAddAnimationGroup( *geometry.second );
				}
			}
		}

		return result;
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

	void SceneImporter::doAddAnimationGroup( Geometry & geometry )
	{
		auto mesh = geometry.getMesh().lock();
		auto skeleton = mesh->getSkeleton();

		if ( mesh->hasAnimation()
			|| ( skeleton && skeleton->hasAnimation() ) )
		{
			auto & scene = *geometry.getScene();
			auto animGroup = ( scene.hasAnimatedObjectGroup( geometry.getName() )
				? scene.findAnimatedObjectGroup( geometry.getName() ).lock()
				: scene.addNewAnimatedObjectGroup( geometry.getName(), scene ).lock() );

			if ( !mesh->getAnimations().empty() )
			{
				for ( auto & animation : mesh->getAnimations() )
				{
					if ( animGroup->addAnimation( animation.first ) )
					{
						animGroup->setAnimationLooped( animation.first, true );
					}
				}

				if ( !animGroup->findObject( geometry.getName() + "_Mesh" ) )
				{
					animGroup->addObject( *mesh, geometry, geometry.getName() );
				}
			}

			if ( skeleton && !skeleton->getAnimations().empty() )
			{
				for ( auto & animation : skeleton->getAnimations() )
				{
					if ( animGroup->addAnimation( animation.first ) )
					{
						animGroup->setAnimationLooped( animation.first, true );
					}
				}

				if ( !animGroup->findObject( geometry.getName() + "_Skeleton" ) )
				{
					animGroup->addObject( *skeleton, *mesh, geometry, geometry.getName() );
				}
			}
		}
	}

	//*********************************************************************************************
}
