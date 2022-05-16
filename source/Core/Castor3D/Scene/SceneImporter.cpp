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
#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"
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
		bool result = true;
		auto animationImporter = file->createAnimationImporter();
		Parameters emptyParams;

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
					scene.addSkeleton( name, skeleton, true );

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
				}
			}
		}

		if ( auto meshImporter = file->createMeshImporter() )
		{
			for ( auto name : file->listMeshes() )
			{
				auto mesh = scene.createMesh( name, scene );

				if ( meshImporter->import( *mesh
					, file
					, emptyParams
					, true ) )
				{
					scene.addMesh( name, mesh, true );

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
				}
			}
		}

		if ( auto nodeImporter = file->createSceneNodeImporter() )
		{
			for ( auto name : file->listSceneNodes() )
			{
				auto node = scene.createSceneNode( name, scene );

				if ( nodeImporter->import( *node
					, file
					, emptyParams ) )
				{
					scene.addSceneNode( name, node, true );

					if ( animationImporter )
					{
						for ( auto animName : file->listSceneNodeAnimations( *node ) )
						{
							auto animation = std::make_unique< SceneNodeAnimation >( *node, animName );

							if ( animationImporter->import( *animation, m_file, emptyParams ) )
							{
								node->addAnimation( std::move( animation ) );
							}
						}
					}
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

		if ( result )
		{
			doTransformScene( scene, parameters );
		}

		return result;
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
		bool result{ true };
		auto animationImporter = file->createAnimationImporter();
		Parameters emptyParams;

		if ( animationImporter )
		{
			for ( auto & meshIt : scene.getMeshCache() )
			{
				auto & mesh = meshIt.second;

				for ( auto animName : file->listMeshAnimations( *mesh ) )
				{
					auto animation = std::make_unique< MeshAnimation >( *mesh, animName );

					if ( animationImporter->import( *animation, m_file, emptyParams ) )
					{
						mesh->addAnimation( std::move( animation ) );
					}
				}
			}

			for ( auto & skelIt : scene.getSkeletonCache() )
			{
				auto & skeleton = skelIt.second;

				for ( auto animName : file->listSkeletonAnimations( *skeleton ) )
				{
					auto animation = std::make_unique< SkeletonAnimation >( *skeleton, animName );

					if ( animationImporter->import( *animation, m_file, emptyParams ) )
					{
						skeleton->addAnimation( std::move( animation ) );
					}
				}
			}
		}

		if ( result )
		{
			doTransformScene( scene, parameters );
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
		, Parameters const & parameters )
	{
		castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
		castor::Quaternion orientation{ castor::Quaternion::identity() };

		if ( parseImportParameters( parameters, scale, orientation ) )
		{
			auto transformNode = scene.addNewSceneNode( m_file->getName() + "TransformNode" ).lock();
			transformNode->setScale( scale );
			transformNode->setOrientation( orientation );
			transformNode->attachTo( *scene.getObjectRootNode() );

			for ( auto & node : m_nodes )
			{
				if ( node->getParent() == scene.getObjectRootNode().get() )
				{
					node->attachTo( *transformNode );
				}
			}
		}
	}

	//*********************************************************************************************
}
