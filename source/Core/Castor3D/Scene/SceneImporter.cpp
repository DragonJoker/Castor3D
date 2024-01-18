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
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/CameraImporter.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/SceneNodeImporter.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/LightImporter.hpp"

CU_ImplementSmartPtr( castor3d, SceneImporter )

namespace castor3d
{
	//*********************************************************************************************

	namespace scnimp
	{
		static castor::Point3f getCameraPosition( castor::BoundingBox const & aabb
			, float & farPlane )
		{
			auto maxComp = std::max( { aabb.getMax()->z, aabb.getMax()->x, aabb.getMax()->y } );
			auto z = ( maxComp * 3.0f );
			farPlane = std::abs( z );
			return { aabb.getCenter()->x
				, aabb.getCenter()->y
				, z };
		}
	}

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
		m_file->setScene( scene );
		castor3d::incProgressBarGlobalRange( m_file->getProgressBar(), 7u );
		doImportMaterials( scene, parameters, textureRemaps );
		auto skeletons = doImportSkeletons( scene );
		auto meshes = doImportMeshes( scene, skeletons );
		auto nodes = doImportNodes( scene );
		doImportLights( scene );
		doImportCameras( scene );
		doCreateGeometries( scene, meshes, nodes );
		importAnimations( scene, file, parameters );

		if ( !nodes.empty() )
		{
			castor3d::incProgressBarGlobalRange( m_file->getProgressBar(), 1u );
			doTransformScene( scene, parameters, nodes );
		}

		doCenterCamera( scene, parameters );
		return true;
	}

	bool SceneImporter::import( Scene & scene
		, castor::Path const & path
		, Parameters const & parameters
		, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps
		, ProgressBar * progress )
	{
		auto & engine = *scene.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		castor3d::incProgressBarGlobalRange( progress, 1u );
		castor3d::stepProgressBarGlobalStartLocal( progress
			, "Creating import file"
			, 1u);
		castor::String preferredImporter = cuT( "any" );
		parameters.get( "preferred_importer", preferredImporter );
		auto file = engine.getImporterFileFactory().create( extension
			, preferredImporter
			, engine
			, path
			, parameters
			, progress );
		return import( scene, file.get(), parameters, textureRemaps );
	}

	bool SceneImporter::importAnimations( Scene & scene
		, ImporterFile * file
		, Parameters const & /*parameters*/ )
	{
		m_file = file;

		if ( auto importer = file->createAnimationImporter() )
		{
			castor3d::incProgressBarGlobalRange( m_file->getProgressBar(), 4u );
			castor::StringMap< AnimObjects > anims;
			doImportSkeletonsAnims( scene, *importer, anims );
			doImportMeshesAnims( scene, *importer, anims );
			doImportNodesAnims( scene, *importer, anims );
			doCreateAnimationGroups( scene, anims );
		}

		return true;
	}

	bool SceneImporter::importAnimations( Scene & scene
		, castor::Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
	{
		auto & engine = *scene.getEngine();
		auto extension = castor::string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		castor3d::incProgressBarGlobalRange( progress, 1u );
		castor3d::stepProgressBarGlobalStartLocal( progress
			, "Creating import file"
			, 1u );
		castor::String preferredImporter = cuT( "any" );
		parameters.get( "preferred_importer", preferredImporter );
		auto file = engine.getImporterFileFactory().create( extension
			, preferredImporter
			, engine
			, path
			, parameters
			, progress );
		return importAnimations( scene, file.get(), parameters );
	}

	void SceneImporter::doImportMaterials( Scene & scene
		, Parameters const & parameters
		, std::map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		if ( auto materialImporter = m_file->createMaterialImporter() )
		{
			auto toImport = m_file->listMaterials();

			if ( !toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, "Importing scene materials"
					, total );
				uint32_t index{};

				for ( auto const & name : toImport )
				{
					++index;
					castor3d::stepProgressBarLocal( m_file->getProgressBar()
						, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

					if ( !getOwner()->hasMaterial( name ) )
					{
						if ( auto material = getOwner()->createMaterial( name
								, *getOwner()
								, getOwner()->getDefaultLightingModel() );
							materialImporter->import( *material
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
	}

	castor::StringMap< SkeletonRPtr > SceneImporter::doImportSkeletons( Scene & scene )
	{
		Parameters emptyParams;
		castor::StringMap< SkeletonRPtr > result;

		if ( auto skeletonImporter = m_file->createSkeletonImporter() )
		{
			auto toImport = m_file->listSkeletons();

			if ( !toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, "Importing scene skeletons"
					, total );
				uint32_t index{};

				for ( auto & name : toImport )
				{
					++index;
					castor3d::stepProgressBarLocal( m_file->getProgressBar()
						, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

					if ( auto skeleton = scene.createSkeleton( name, scene );
						skeletonImporter->import( *skeleton
							, m_file
							, emptyParams ) )
					{
						result.try_emplace( name, skeleton.get() );
						scene.addSkeleton( name, skeleton, true );
					}
				}
			}
		}

		return result;
	}

	castor::StringMap< MeshResPtr > SceneImporter::doImportMeshes( Scene & scene
		, castor::StringMap< SkeletonRPtr > const & skeletons )
	{
		Parameters emptyParams;
		castor::StringMap< MeshResPtr > result;

		if ( auto meshImporter = m_file->createMeshImporter() )
		{
			auto toImport = m_file->listMeshes();

			if ( !toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, "Importing scene meshes"
					, total );
				uint32_t index{};

				for ( auto & data : toImport )
				{
					++index;
					castor3d::stepProgressBarLocal( m_file->getProgressBar()
						, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

					if ( auto mesh = scene.createMesh( data.name, scene );
						meshImporter->import( *mesh
							, m_file
							, emptyParams
							, true ) )
					{
						if ( !data.skeleton.empty() )
						{
							auto skelIt = skeletons.find( data.skeleton );
							CU_Require( skelIt != skeletons.end() );
							mesh->setSkeleton( skelIt->second );
						}

						result.try_emplace( data.name, mesh.get() );
						scene.addMesh( data.name, mesh, true );
					}
				}
			}
		}

		return result;
	}

	castor::StringMap< SceneNodeRPtr > SceneImporter::doImportNodes( Scene & scene )
	{
		Parameters emptyParams;
		castor::StringMap< SceneNodeRPtr > result;

		if ( auto nodeImporter = m_file->createSceneNodeImporter() )
		{
			auto toImport = m_file->listSceneNodes();

			if ( !toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, "Importing scene nodes"
					, total );
				uint32_t index{};

				for ( auto & data : toImport )
				{
					++index;
					castor3d::stepProgressBarLocal( m_file->getProgressBar()
						, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

					if ( !scene.hasSceneNode( data.name ) )
					{
						if ( auto node = scene.createSceneNode( data.name, scene );
							nodeImporter->import( *node
								, m_file
								, emptyParams ) )
						{
							if ( auto parent = scene.tryFindSceneNode( data.parent ) )
							{
								node->attachTo( *parent );
							}
							else if ( data.isCamera )
							{
								node->attachTo( *scene.getCameraRootNode() );
							}
							else
							{
								node->attachTo( *scene.getObjectRootNode() );
							}

							result.try_emplace( data.name, node.get() );
							scene.addSceneNode( data.name, node, true );
						}
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
			auto toImport = m_file->listLights();

			if ( !toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, "Importing scene lights"
					, total );
				uint32_t index{};

				for ( auto & data : toImport )
				{
					++index;
					castor3d::stepProgressBarLocal( m_file->getProgressBar()
						, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

					if ( auto light = scene.createLight( data.name
							, scene
							, *scene.getObjectRootNode()
							, scene.getLightsFactory()
							, data.type );
						lightImporter->import( *light
							, m_file
							, emptyParams ) )
					{
						scene.addLight( data.name, light, true );
					}
				}
			}
		}
	}

	void SceneImporter::doImportCameras( Scene & scene )
	{
		Parameters emptyParams;

		if ( auto cameraImporter = m_file->createCameraImporter() )
		{
			auto toImport = m_file->listCameras();

			if ( !toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, "Importing scene cameras"
					, total );
				uint32_t index{};

				for ( auto & data : toImport )
				{
					++index;
					castor3d::stepProgressBarLocal( m_file->getProgressBar()
						, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

					if ( auto camera = scene.createCamera( data.name
							, scene
							, *scene.getCameraRootNode() );
						cameraImporter->import( *camera
							, m_file
							, emptyParams ) )
					{
						scene.addCamera( data.name, camera, true );
					}
				}
			}
		}
	}

	void SceneImporter::doCreateGeometries( Scene & scene
		, castor::StringMap< MeshResPtr > const & meshes
		, castor::StringMap< SceneNodeRPtr > const & nodes )
	{
		auto toImport = m_file->listGeometries();

		if ( !toImport.empty() )
		{
			auto total = uint32_t( toImport.size() );
			castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
				, "Creating scene objects"
				, total );
			uint32_t index{};

			for ( auto & geom : toImport )
			{
				++index;
				castor3d::stepProgressBarLocal( m_file->getProgressBar()
					, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

				auto meshIt = meshes.find( geom.mesh );
				CU_Require( meshIt != meshes.end() );
				auto nodeIt = nodes.find( geom.node );
				CU_Require( nodeIt != nodes.end() );
				scene.addGeometry( scene.createGeometry( geom.name
					, scene
					, *nodeIt->second
					, meshIt->second ) );
			}
		}
	}

	void SceneImporter::doImportSkeletonsAnims( Scene & scene
		, AnimationImporter & importer
		, castor::StringMap< AnimObjects > & anims )
	{
		Parameters emptyParams;
		auto total = uint32_t( m_file->listAllSkeletonAnimations().size() );
		castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, "Importing scene skeletons animations"
			, total );
		uint32_t index{};

		for ( auto & [_, skeleton] : scene.getSkeletonCache() )
		{
			for ( auto animName : m_file->listSkeletonAnimations( *skeleton ) )
			{
				++index;
				castor3d::stepProgressBarLocal( m_file->getProgressBar()
					, castor::string::toString( index ) + " / " + castor::string::toString( total ) );
				auto animation = castor::makeUnique< SkeletonAnimation >( *skeleton, animName );

				if ( importer.import( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.try_emplace( animName ).first->second;
					anim.skeletons.push_back( skeleton.get() );
					skeleton->addAnimation( castor::ptrRefCast< Animation >( animation ) );
				}
			}
		}
	}

	void SceneImporter::doImportMeshesAnims( Scene & scene
		, AnimationImporter & importer
		, castor::StringMap< AnimObjects > & anims )
	{
		Parameters emptyParams;
		auto total = uint32_t( m_file->listAllMeshAnimations().size() );
		castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, "Importing scene meshes animations"
			, total );
		uint32_t index{};

		for ( auto & [_, mesh] : scene.getMeshCache() )
		{
			for ( auto animName : m_file->listMeshAnimations( *mesh ) )
			{
				++index;
				castor3d::stepProgressBarLocal( m_file->getProgressBar()
					, castor::string::toString( index ) + " / " + castor::string::toString( total ) );
				auto animation = castor::makeUnique< MeshAnimation >( *mesh, animName );

				if ( importer.import( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.try_emplace( animName ).first->second;
					anim.meshes.push_back( mesh.get() );
					mesh->addAnimation( castor::ptrRefCast< Animation >( animation ) );
				}
			}
		}
	}

	void SceneImporter::doImportNodesAnims( Scene & scene
		, AnimationImporter & importer
		, castor::StringMap< AnimObjects > & anims )
	{
		Parameters emptyParams;
		auto total = uint32_t( m_file->listAllSceneNodeAnimations().size() );
		castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, "Importing scene nodes animations"
			, total );
		uint32_t index{};

		for ( auto & [_, node] : scene.getSceneNodeCache() )
		{
			for ( auto animName : m_file->listSceneNodeAnimations( *node ) )
			{
				++index;
				castor3d::stepProgressBarLocal( m_file->getProgressBar()
					, castor::string::toString( index ) + " / " + castor::string::toString( total ) );
				auto animation = castor::makeUnique< SceneNodeAnimation >( *node, animName );

				if ( importer.import( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.try_emplace( animName ).first->second;
					anim.nodes.push_back( node.get() );
					node->addAnimation( castor::ptrRefCast< Animation >( animation ) );
				}
			}
		}
	}

	void SceneImporter::doCreateAnimationGroups( Scene & scene
		, castor::StringMap< AnimObjects > & anims )
	{
		auto total = uint32_t( anims.size() );
		castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, "Creating scene animations groups"
			, total );
		uint32_t index{};

		for ( auto & [animName, animObjects] : anims )
		{
			++index;
			castor3d::stepProgressBarLocal( m_file->getProgressBar()
				, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

			for ( auto & [name, geometry] : scene.getGeometryCache() )
			{
				auto & mesh = *geometry->getMesh();
				auto nodeIt = std::find( animObjects.nodes.begin()
					, animObjects.nodes.end()
					, geometry->getParent() );
				auto meshIt = std::find( animObjects.meshes.begin()
					, animObjects.meshes.end()
					, &mesh );
				auto skelIt = ( mesh.getSkeleton()
					? std::find( animObjects.skeletons.begin()
						, animObjects.skeletons.end()
						, mesh.getSkeleton() )
					: animObjects.skeletons.end() );

				if ( nodeIt != animObjects.nodes.end()
					|| meshIt != animObjects.meshes.end()
					|| skelIt != animObjects.skeletons.end() )
				{
					auto animGroup = ( scene.hasAnimatedObjectGroup( name )
						? scene.findAnimatedObjectGroup( name )
						: scene.addNewAnimatedObjectGroup( name, scene ) );

					if ( animGroup->addAnimation( animName ) )
					{
						animGroup->setAnimationLooped( animName, true );
					}

					if ( nodeIt != animObjects.nodes.end() )
					{
						animGroup->addObject( **nodeIt, ( *nodeIt )->getName() );
					}

					if ( meshIt != animObjects.meshes.end() )
					{
						animGroup->addObject( **meshIt, *geometry, name );
					}

					if ( skelIt != animObjects.skeletons.end() )
					{
						animGroup->addObject( **skelIt, mesh, *geometry, name );
					}
				}
			}
		}
	}

	void SceneImporter::doTransformScene( Scene & scene
		, Parameters const & parameters
		, castor::StringMap< SceneNodeRPtr > const & nodes )
	{
		castor::Point3f scale{ 1.0f, 1.0f, 1.0f };
		castor::Quaternion orientation{ castor::Quaternion::identity() };
		auto total = uint32_t( nodes.size() );
		castor3d::stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, "Transforming scene nodes"
			, total );
		uint32_t index{};

		if ( parseImportParameters( parameters, scale, orientation ) )
		{
			auto transformNode = scene.addNewSceneNode( m_file->getName() + "TransformNode" );
			transformNode->setScale( scale );
			transformNode->setOrientation( orientation );
			transformNode->attachTo( *scene.getObjectRootNode() );

			for ( auto & [_, node] : nodes )
			{
				++index;
				castor3d::stepProgressBarLocal( m_file->getProgressBar()
					, castor::string::toString( index ) + " / " + castor::string::toString( total ) );

				if ( node->getParent() == scene.getObjectRootNode() )
				{
					node->attachTo( *transformNode );
				}
			}
		}
	}

	void SceneImporter::doCenterCamera( Scene & scene
		, Parameters const & parameters )
	{
		castor::String centerCamera;

		if ( parameters.get( cuT( "center_camera" ), centerCamera )
			&& !centerCamera.empty() )
		{
			if ( auto camera = scene.getCameraCache().tryFind( centerCamera ) )
			{
				scene.getSceneNodeCache().forEach( []( SceneNode & node )
					{
						node.update();
					} );
				scene.updateBoundingBox();
				auto cameraNode = camera->getParent();
				float farPlane = 0.0f;
				cameraNode->setPosition( scnimp::getCameraPosition( scene.getBoundingBox(), farPlane ) );
				cameraNode->setOrientation( castor::Quaternion::fromAxisAngle( castor::Point3f{ 0.0f, 1.0f, 0.0f }, 180.0_degrees ) );
				auto & vp = camera->getViewport();
				camera->getViewport().setPerspective( vp.getFovY()
					, vp.getRatio()
					, std::max( 0.01f, farPlane / 1000.0f )
					, std::max( farPlane, 1000.0f ) );
			}
		}
	}

	Engine * getEngine( SceneImportContext const & context )
	{
		return getEngine( *context.scene );
	}

	//*********************************************************************************************
}
