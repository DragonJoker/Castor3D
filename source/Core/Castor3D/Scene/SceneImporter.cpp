#include "Castor3D/Scene/SceneImporter.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/AnimationImporter.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/MaterialImporter.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
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

CU_ImplementSmartPtr( c3d, SceneImporter )

namespace c3d
{
	//*********************************************************************************************

	namespace scnimp
	{
		static Point3f getCameraPosition( BoundingBox const & aabb
			, float & farPlane )
		{
			auto maxComp = std::max( { aabb.getMax()->z, aabb.getMax()->x, aabb.getMax()->y } );
			auto z = ( maxComp * 3.0f );
			farPlane = std::abs( z ) + aabb.getMax()->z;
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

	bool SceneImporter::importData( Scene & scene
		, ImporterFile * file
		, Parameters const & parameters
		, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		m_file = file;
		m_file->setScene( scene );
		incProgressBarGlobalRange( m_file->getProgressBar(), 7u );
		doImportMaterials( scene, parameters, textureRemaps );
		auto skeletons = doImportSkeletons( scene );
		auto meshes = doImportMeshes( scene, skeletons );
		auto nodes = doImportNodes( scene );
		doImportLights( scene );
		doImportLightGroups( scene );
		doImportCameras( scene );
		doCreateGeometries( scene, meshes, nodes );
		importAnimationsData( scene, file, parameters );

		if ( !nodes.empty() )
		{
			incProgressBarGlobalRange( m_file->getProgressBar(), 1u );
			doTransformScene( scene, parameters, nodes );
		}

		doCenterCamera( scene, parameters );
		return true;
	}

	bool SceneImporter::importData( Scene & scene
		, Path const & path
		, Parameters const & parameters
		, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps
		, ProgressBar * progress )
	{
		Engine const & engine = *scene.getEngine();
		auto extension = string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		incProgressBarGlobalRange( progress, 1u );
		stepProgressBarGlobalStartLocal( progress
			, cuT( "Creating import file" )
			, 1u);
		String preferredImporter = cuT( "any" );
		parameters.get( cuT( "preferred_importer" ), preferredImporter );
		auto file = engine.getImporterFileFactory().create( extension
			, preferredImporter
			, scene
			, path
			, parameters
			, progress );
		return importData( scene, file.get(), parameters, textureRemaps );
	}

	bool SceneImporter::importAnimationsData( Scene & scene
		, ImporterFile * file
		, Parameters const & /*parameters*/ )
	{
		m_file = file;

		if ( auto importer = file->createAnimationImporter() )
		{
			incProgressBarGlobalRange( m_file->getProgressBar(), 4u );
			StringMap< AnimObjects > anims;
			doImportSkeletonsAnims( scene, *importer, anims );
			doImportMeshesAnims( scene, *importer, anims );
			doImportNodesAnims( scene, *importer, anims );
			doImportTexturesAnims( scene, *importer );
			doCreateAnimationGroups( scene, anims );
		}

		return true;
	}

	bool SceneImporter::importAnimationsData( Scene & scene
		, Path const & path
		, Parameters const & parameters
		, ProgressBar * progress )
	{
		auto & engine = *scene.getEngine();
		auto extension = string::lowerCase( path.getExtension() );

		if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
		{
			log::error << cuT( "Importer for [" ) << extension << cuT( "] files is not registered, make sure you've got the matching plug-in installed." );
			return false;
		}

		incProgressBarGlobalRange( progress, 1u );
		stepProgressBarGlobalStartLocal( progress
			, cuT( "Creating import file" )
			, 1u );
		String preferredImporter = cuT( "any" );
		parameters.get( cuT( "preferred_importer" ), preferredImporter );
		auto file = engine.getImporterFileFactory().create( extension
			, preferredImporter
			, engine
			, path
			, parameters
			, progress );
		return importAnimationsData( scene, file.get(), parameters );
	}

	void SceneImporter::doImportMaterials( Scene & scene
		, Parameters const & parameters
		, Map< PassComponentTextureFlag, TextureConfiguration > const & textureRemaps )
	{
		if ( auto materialImporter = m_file->createMaterialImporter() )
		{
			if ( auto toImport = m_file->listMaterials();
				!toImport.empty() )
			{
				materialImporter->prepareImport( m_file, parameters, textureRemaps );
				Vector< MaterialPtr > imported;
				auto total = uint32_t( toImport.size() );
				stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, cuT( "Importing scene materials" )
					, total );
				uint32_t index{};

				for ( auto const & name : toImport )
				{
					log::info << materialImporter->getPrefix() << cuT( "Loading Material [" ) << name << cuT( "]" ) << std::endl;
					++index;
					stepProgressBarLocal( m_file->getProgressBar()
						, string::toString( index ) + cuT( " / " ) + string::toString( total ) );
					if ( auto material = materialImporter->createMaterial( name );
						materialImporter->importMaterial( *material ) )
						imported.emplace_back( c3d::move( material ) );
					log::info << materialImporter->getPrefix() << cuT( "Loaded Material [" ) << name << cuT( "]" ) << std::endl;
				}

				for ( auto & material : imported )
				{
					if ( !getOwner()->hasMaterial( material->getName() ) )
						scene.getMaterialView().add( material->getName(), material, true );
				}
			}
		}
	}

	StringMap< SkeletonRPtr > SceneImporter::doImportSkeletons( Scene & scene )
	{
		Parameters emptyParams;
		StringMap< SkeletonRPtr > result;

		if ( auto skeletonImporter = m_file->createSkeletonImporter() )
		{
			if ( auto toImport = m_file->listSkeletons();
				!toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, cuT( "Importing scene skeletons" )
					, total );
				uint32_t index{};

				for ( auto const & name : toImport )
				{
					++index;
					stepProgressBarLocal( m_file->getProgressBar()
						, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

					if ( auto skeleton = skeletonImporter->importData( name
						, scene
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

	StringMap< MeshResPtr > SceneImporter::doImportMeshes( Scene & scene
		, StringMap< SkeletonRPtr > const & skeletons )
	{
		Parameters emptyParams;
		StringMap< MeshResPtr > result;

		if ( auto meshImporter = m_file->createMeshImporter() )
		{
			if ( auto toImport = m_file->listMeshes();
				!toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, cuT( "Importing scene meshes" )
					, total );
				uint32_t index{};

				for ( auto const & data : toImport )
				{
					++index;
					stepProgressBarLocal( m_file->getProgressBar()
						, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

					if ( auto mesh = meshImporter->importData( data.name, scene
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

						if ( mesh )
						{
							mesh->cleanup();
						}
					}
				}
			}
		}

		return result;
	}

	StringMap< SceneNodeRPtr > SceneImporter::doImportNodes( Scene & scene )
	{
		Parameters emptyParams;
		StringMap< SceneNodeRPtr > result;

		if ( auto nodeImporter = m_file->createSceneNodeImporter() )
		{
			if ( auto toImport = m_file->listSceneNodes();
				!toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, cuT( "Importing scene nodes" )
					, total );
				uint32_t index{};

				for ( auto const & data : toImport )
				{
					++index;
					stepProgressBarLocal( m_file->getProgressBar()
						, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

					if ( !scene.hasSceneNode( data.name ) )
					{
						if ( auto node = nodeImporter->importData( data.name
							, { &scene }
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
			if ( auto toImport = m_file->listLights();
				!toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, cuT( "Importing scene lights" )
					, total );
				uint32_t index{};

				for ( auto const & data : toImport )
				{
					++index;
					stepProgressBarLocal( m_file->getProgressBar()
						, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

					if ( auto light = lightImporter->importData( data.name
						, { &scene, scene.getObjectRootNode(), &scene.getLightsFactory(), data.type }
						, m_file
						, emptyParams ) )
					{
						scene.addLight( data.name, light, true );
					}
				}
			}
		}
	}

	void SceneImporter::doImportLightGroups( Scene & scene )
	{
		Parameters emptyParams;

		if ( auto lightImporter = m_file->createLightImporter() )
		{
			if ( auto toImport = m_file->listLightGroups();
				!toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, cuT( "Importing scene light groups" )
					, total );
				uint32_t index{};

				for ( auto const & data : toImport )
				{
					++index;
					stepProgressBarLocal( m_file->getProgressBar()
						, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

					if ( auto light = lightImporter->importData( data.name
						, { &scene, &scene.getLightsFactory(), data.type }
						, m_file
						, emptyParams ) )
					{
						scene.addLightGroup( data.name, light, true );
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
			if ( auto toImport = m_file->listCameras();
				!toImport.empty() )
			{
				auto total = uint32_t( toImport.size() );
				stepProgressBarGlobalStartLocal( m_file->getProgressBar()
					, cuT( "Importing scene cameras" )
					, total );
				uint32_t index{};

				for ( auto const & data : toImport )
				{
					++index;
					stepProgressBarLocal( m_file->getProgressBar()
						, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

					if ( auto camera = cameraImporter->importData( data.name
						, { &scene, scene.getCameraRootNode() }
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
		, StringMap< MeshResPtr > const & meshes
		, StringMap< SceneNodeRPtr > const & nodes )
	{
		auto toImport = m_file->listGeometries();

		if ( !toImport.empty() )
		{
			auto total = uint32_t( toImport.size() );
			stepProgressBarGlobalStartLocal( m_file->getProgressBar()
				, cuT( "Creating scene objects" )
				, total );
			uint32_t index{};

			for ( auto const & geom : toImport )
			{
				++index;
				stepProgressBarLocal( m_file->getProgressBar()
					, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

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
		, StringMap< AnimObjects > & anims )
	{
		Parameters emptyParams;
		auto total = m_file->countAllSkeletonAnimations();
		stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, cuT( "Importing scene skeletons animations" )
			, total );
		uint32_t index{};

		for ( auto const & [_, skeleton] : scene.getSkeletonCache() )
		{
			for ( auto animName : m_file->listSkeletonAnimations( *skeleton ) )
			{
				++index;
				stepProgressBarLocal( m_file->getProgressBar()
					, string::toString( index ) + cuT( " / " ) + string::toString( total ) );
				auto animation = makeUnique< SkeletonAnimation >( *skeleton, animName );

				if ( importer.importData( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.try_emplace( animName ).first->second;
					anim.skeletons.push_back( skeleton.get() );
					skeleton->addAnimation( ptrRefCast< Animation >( animation ) );
				}
			}
		}
	}

	void SceneImporter::doImportMeshesAnims( Scene & scene
		, AnimationImporter & importer
		, StringMap< AnimObjects > & anims )
	{
		Parameters emptyParams;
		auto total = m_file->countAllMeshAnimations();
		stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, cuT( "Importing scene meshes animations" )
			, total );
		uint32_t index{};

		for ( auto const & [_, mesh] : scene.getMeshCache() )
		{
			for ( auto animName : m_file->listMeshAnimations( *mesh ) )
			{
				++index;
				stepProgressBarLocal( m_file->getProgressBar()
					, string::toString( index ) + cuT( " / " ) + string::toString( total ) );
				auto animation = makeUnique< MeshAnimation >( *mesh, animName );

				if ( importer.importData( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.try_emplace( animName ).first->second;
					anim.meshes.push_back( mesh.get() );
					mesh->addAnimation( ptrRefCast< Animation >( animation ) );
				}
			}
		}
	}

	void SceneImporter::doImportNodesAnims( Scene & scene
		, AnimationImporter & importer
		, StringMap< AnimObjects > & anims )
	{
		Parameters emptyParams;
		auto total = m_file->countAllSceneNodeAnimations();
		stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, cuT( "Importing scene nodes animations" )
			, total );
		uint32_t index{};

		for ( auto const & [_, node] : scene.getSceneNodeCache() )
		{
			for ( auto animName : m_file->listSceneNodeAnimations( *node ) )
			{
				++index;
				stepProgressBarLocal( m_file->getProgressBar()
					, string::toString( index ) + cuT( " / " ) + string::toString( total ) );
				auto animation = makeUnique< SceneNodeAnimation >( *node, animName );

				if ( importer.importData( *animation, m_file, emptyParams ) )
				{
					auto & anim = anims.try_emplace( animName ).first->second;
					anim.nodes.push_back( node.get() );
					node->addAnimation( ptrRefCast< Animation >( animation ) );
				}
			}
		}
	}

	void SceneImporter::doImportTexturesAnims( Scene & scene
		, AnimationImporter & importer )
	{
		Parameters emptyParams;
		auto total = m_file->countAllTextureAnimations();
		stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, cuT( "Importing scene nodes animations" )
			, total );
		uint32_t index{};

		for ( auto const & materialName : scene.getMaterialView() )
		{
			if ( auto material = getOwner()->findMaterial( materialName ) )
			{
				for ( uint32_t passId = 0u; passId < material->getPassCount(); ++passId )
				{
					auto pass = material->getPass( passId );
					for ( auto unitId : m_file->listTextureAnimations( *material, passId ) )
					{
						++index;
						stepProgressBarLocal( m_file->getProgressBar()
							, string::toString( index ) + cuT( " / " ) + string::toString( total ) );
						auto unit = pass->getTextureUnit( unitId );
						auto & animation = unit->createAnimation();

						if ( !importer.importData( animation, m_file, emptyParams ) )
						{
							unit->removeAnimation();
						}
					}
				}
			}
		}
	}

	void SceneImporter::doCreateAnimationGroups( Scene & scene
		, StringMap< AnimObjects > & anims )const
	{
		auto hasNodeAnim = [&anims]( SceneNode const & node, StringView animName )
			{
				for ( auto & [currAnimName, animObjects] : anims )
				{
					if ( animName != currAnimName )
					{
						if ( auto nodeIt = std::find( animObjects.nodes.begin(), animObjects.nodes.end(), &node );
							nodeIt != animObjects.nodes.end() )
							return true;
					}
				}

				return false;
			};

		auto total = uint32_t( anims.size() );
		stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, cuT( "Creating scene animations groups" )
			, total );
		uint32_t index{};

		for ( auto & [animName, animObjects] : anims )
		{
			++index;
			stepProgressBarLocal( m_file->getProgressBar()
				, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

			for ( auto const & [objectName, geometry] : scene.getGeometryCache() )
			{
				auto node = geometry->getParent();
				Vector< SceneNode * > nodes;

				while ( node && !hasNodeAnim( *node, animName ) )
				{
					if ( auto nodeIt = std::find( animObjects.nodes.begin(), animObjects.nodes.end(), node );
						nodeIt != animObjects.nodes.end() )
					{
						nodes.push_back( *nodeIt );
						// Prevent processing this node twice
						animObjects.nodes.erase( nodeIt );
					}

					node = node->getParent();
				}

				auto & mesh = *geometry->getMesh();
				auto meshIt = std::find( animObjects.meshes.begin()
					, animObjects.meshes.end()
					, &mesh );
				auto skelIt = ( mesh.getSkeleton()
					? std::find( animObjects.skeletons.begin()
						, animObjects.skeletons.end()
						, mesh.getSkeleton() )
					: animObjects.skeletons.end() );

				if ( !nodes.empty()
					|| meshIt != animObjects.meshes.end()
					|| skelIt != animObjects.skeletons.end() )
				{
					auto animGroup = ( scene.hasAnimatedObjectGroup( objectName )
						? scene.findAnimatedObjectGroup( objectName )
						: scene.addNewAnimatedObjectGroup( objectName, scene ) );

					if ( animGroup->addAnimation( animName ) )
					{
						animGroup->setAnimationLooped( animName, true );
					}

					for ( auto animNode : nodes )
					{
						animGroup->addObject( *animNode, animNode->getName() );
					}

					if ( meshIt != animObjects.meshes.end() )
					{
						animGroup->addObject( **meshIt, *geometry, objectName );
					}

					if ( skelIt != animObjects.skeletons.end() )
					{
						animGroup->addObject( **skelIt, mesh, *geometry, objectName );
					}
				}
			}

			for ( auto const & [nodeName, node] : scene.getSceneNodeCache() )
			{
				auto nodeIt = std::find( animObjects.nodes.begin()
					, animObjects.nodes.end()
					, node.get() );

				if ( nodeIt != animObjects.nodes.end() )
				{
					auto animGroup = ( scene.hasAnimatedObjectGroup( nodeName )
						? scene.findAnimatedObjectGroup( nodeName )
						: scene.addNewAnimatedObjectGroup( nodeName, scene ) );

					if ( animGroup->addAnimation( animName ) )
					{
						animGroup->setAnimationLooped( animName, true );
					}

					animGroup->addObject( **nodeIt, ( *nodeIt )->getName() );
				}
			}
		}
	}

	void SceneImporter::doTransformScene( Scene & scene
		, Parameters const & parameters
		, StringMap< SceneNodeRPtr > const & nodes )const
	{
		Point3f scale{ 1.0f, 1.0f, 1.0f };
		Quaternion orientation{ Quaternion::identity() };
		auto total = uint32_t( nodes.size() );
		stepProgressBarGlobalStartLocal( m_file->getProgressBar()
			, cuT( "Transforming scene nodes" )
			, total );

		if ( parseImportParameters( parameters, scale, orientation ) )
		{
			uint32_t index{};
			auto transformNode = scene.addNewSceneNode( m_file->getName() + cuT( "TransformNode" ) );
			transformNode->setScale( scale );
			transformNode->setOrientation( orientation );
			transformNode->attachTo( *scene.getObjectRootNode() );

			for ( auto const & [_, node] : nodes )
			{
				++index;
				stepProgressBarLocal( m_file->getProgressBar()
					, string::toString( index ) + cuT( " / " ) + string::toString( total ) );

				if ( node->getParent() == scene.getObjectRootNode() )
				{
					node->attachTo( *transformNode );
				}
			}
		}
	}

	void SceneImporter::doCenterCamera( Scene & scene
		, Parameters const & parameters )const
	{
		String centerCamera;

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
				cameraNode->setOrientation( Quaternion::fromAxisAngle( Point3f{ 0.0f, 1.0f, 0.0f }, 180.0_degrees ) );
				Viewport const & vp = camera->getViewport();
				camera->getViewport().setPerspective( vp.getFovY()
					, vp.getRatio()
					, std::max( 0.01f, farPlane / 10000.0f )
					, std::max( farPlane, 1000.0f ) );
			}
		}
	}

	String getPrefix( SceneImportContext const & context )
	{
		auto parentPrefix = getPrefix( *context.scene );
		return parentPrefix.empty()
			? context.prefix
			: parentPrefix + context.prefix;
	}

	Engine * getEngine( SceneImportContext const & context )
	{
		return getEngine( *context.scene );
	}

	//*********************************************************************************************
}
