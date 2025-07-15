#include "Castor3D/Scene/Scene.hpp"

#include "Castor3D/DebugDefines.hpp"
#include "Castor3D/Engine.hpp"
#include "Castor3D/Limits.hpp"
#include "Castor3D/Cache/OverlayCache.hpp"
#include "Castor3D/Event/Frame/CpuFunctorEvent.hpp"
#include "Castor3D/Event/Frame/FrameListener.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Gui/ControlsManager.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/PassFactory.hpp"
#include "Castor3D/Miscellaneous/LoadingScreen.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Render/RenderInfo.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/Culling/DummyCuller.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMap.hpp"
#include "Castor3D/Render/EnvironmentMap/EnvironmentMapPass.hpp"
#include "Castor3D/Render/Node/BillboardRenderNode.hpp"
#include "Castor3D/Render/Node/SceneRenderNodes.hpp"
#include "Castor3D/Render/Node/SubmeshRenderNode.hpp"
#include "Castor3D/Scene/Background/Image.hpp"
#include "Castor3D/Scene/Background/Skybox.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"
#include "Castor3D/Scene/Background/Background.hpp"
#include "Castor3D/Scene/Background/Colour.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/LightFactory.hpp"
#include "Castor3D/Scene/Light/LightGroup.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/LightingModelFactory.hpp"
#include "Castor3D/Shader/ShaderBuffers/PassBuffer.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Graphics/Font.hpp>
#include <CastorUtils/Graphics/FontCache.hpp>

CU_ImplementSmartPtr( c3d, Scene )

namespace c3d
{
	//*************************************************************************************************

	namespace scene
	{
		static CU_ImplementAttributeParserBlock( parserBkColour, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->scene->setBackgroundColour( params[0]->get< RgbColour >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBkImage, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto imgBackground = makeUnique< ImageBackground >( *getEngine( *blockContext )
					, *blockContext->scene );
				imgBackground->setImage( context.file.getPath(), params[0]->get< Path >() );
				blockContext->scene->setBackground( ptrRefCast< SceneBackground >( imgBackground ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserFont, SceneContext, FontContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->scene = blockContext;
				newBlockContext->root = blockContext->root;
				params[0]->get( newBlockContext->name );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eFont )

		static CU_ImplementAttributeParserNewBlock( parserSdfFont, SceneContext, FontContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->scene = blockContext;
				newBlockContext->root = blockContext->root;
				params[0]->get( newBlockContext->name );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSdfFont )

		static CU_ImplementAttributeParserNewBlock( parserSamplerState, SceneContext, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				newBlockContext->sampler = getEngine( *blockContext )->tryFindSampler( name );

				if ( !newBlockContext->sampler )
				{
					newBlockContext->ownSampler = getEngine( *blockContext )->createSampler( name
						, *getEngine( *blockContext ) );
					newBlockContext->sampler = newBlockContext->ownSampler.get();
				}
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSampler )

		static CU_ImplementAttributeParserNewBlock( parserCamera, SceneContext, CameraContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->scene = blockContext;
				newBlockContext->name = getPrefixedName( params[0]->get< String >(), *blockContext );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eCamera )

		static CU_ImplementAttributeParserNewBlock( parserCameraNode, SceneContext, NodeContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->name = getPrefixedName( params[0]->get< String >(), *blockContext );
				newBlockContext->scene = blockContext;
				newBlockContext->isCameraNode = true;
				newBlockContext->parentNode = blockContext->scene->getCameraRootNode();
				newBlockContext->currentNode = blockContext->scene->tryFindSceneNode( newBlockContext->name );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eNode )

		static CU_ImplementAttributeParserNewBlock( parserNode, SceneContext, NodeContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->name = getPrefixedName( params[0]->get< String >(), *blockContext );
				newBlockContext->scene = blockContext;
				newBlockContext->isCameraNode = false;
				newBlockContext->parentNode = blockContext->scene->getObjectRootNode();
				newBlockContext->currentNode = blockContext->scene->tryFindSceneNode( newBlockContext->name );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eNode )

		static CU_ImplementAttributeParserNewBlock( parserObject, SceneContext, ObjectContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->name = getPrefixedName( params[0]->get< String >(), *blockContext );
				newBlockContext->scene = blockContext;
				newBlockContext->ownGeometry = blockContext->scene->createGeometry( newBlockContext->name
					, *blockContext->scene );
				newBlockContext->geometry = newBlockContext->ownGeometry.get();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eObject )

		static CU_ImplementAttributeParserBlock( parserAmbientLight, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->scene->setAmbientLight( params[0]->get< RgbColour >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserImport, SceneContext, SceneImportContext )
		{
			newBlockContext->scene = blockContext;
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSceneImport )

		static CU_ImplementAttributeParserNewBlock( parserBillboard, SceneContext, BillboardsContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				newBlockContext->scene = blockContext;
				newBlockContext->ownBillboards = makeUnique< BillboardList >( name
					, *blockContext->scene );
				newBlockContext->billboards = newBlockContext->ownBillboards.get();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eBillboard )

		static CU_ImplementAttributeParserNewBlock( parserAnimatedObjectGroup, SceneContext, AnimGroupContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				newBlockContext->scene = blockContext;
				newBlockContext->animGroup = blockContext->scene->addNewAnimatedObjectGroup( name
					, *blockContext->scene );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eAnimGroup )

		static CU_ImplementAttributeParserBlock( parserPanelOverlay, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				blockContext->overlays->parentOverlays.push_back( c3d::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->scene->tryFindOverlay( name );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = getEngine( *blockContext )->createOverlay( name
						, *getEngine( *blockContext )
						, OverlayType::ePanel
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::ePanelOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlay, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				blockContext->overlays->parentOverlays.push_back( c3d::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->scene->tryFindOverlay( name );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = getEngine( *blockContext )->createOverlay( name
						, *getEngine( *blockContext )
						, OverlayType::eBorderPanel
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eBorderPanelOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserTextOverlay, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				blockContext->overlays->parentOverlays.push_back( c3d::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->scene->tryFindOverlay( name );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = getEngine( *blockContext )->createOverlay( name
						, *getEngine( *blockContext )
						, OverlayType::eText
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eTextOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserNewBlock( parserSkybox, SceneContext, SkyboxContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else
			{
				newBlockContext->skybox = makeUnique< SkyboxBackground >( *getEngine( *blockContext )
					, *blockContext->scene );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSkybox )

		static CU_ImplementAttributeParserBlock( parserFogType, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else
			{
				blockContext->scene->getFog().setType( FogType( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFogDensity, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else
			{
				blockContext->scene->getFog().setDensity( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserDirectionalShadowCascades, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->scene->setDirectionalShadowCascades( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserGlobalIndirectAttenuation, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No Scene initialised." ) );
			}
			else
			{
				float value{ 0u };
				params[0]->get( value );
				blockContext->scene->setLpvIndirectAttenuation( value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserParticleSystem, SceneContext, ParticleSystemContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->scene = blockContext;
				params[0]->get( newBlockContext->name );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eParticleSystem )

		static CU_ImplementAttributeParserNewBlock( parserMesh, SceneContext, MeshContext )
		{
			if ( blockContext->scene )
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				newBlockContext->scene = blockContext;
				newBlockContext->root = blockContext->root;
				newBlockContext->mesh = blockContext->scene->tryFindMesh( name );

				if ( !newBlockContext->mesh )
				{
					newBlockContext->ownMesh = blockContext->scene->createMesh( name
						, *blockContext->scene );
					newBlockContext->mesh = newBlockContext->ownMesh.get();
				}
			}
			else
			{
				CU_ParsingError( cuT( "No scene initialised" ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eMesh )

		static CU_ImplementAttributeParserNewBlock( parserSkeleton, SceneContext, SkeletonContext )
		{
			if ( blockContext->scene )
			{
				auto name = getPrefixedName( params[0]->get< String >(), *blockContext );
				newBlockContext->scene = blockContext;
				newBlockContext->skeleton = blockContext->scene->tryFindSkeleton( name );

				if ( !newBlockContext->skeleton )
				{
					newBlockContext->skeleton = blockContext->scene->addNewSkeleton( name
						, *blockContext->scene );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No scene initialised" ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSkeleton )

		static CU_ImplementAttributeParserBlock( parserEnd, SceneContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else
			{
				log::info << "Loaded scene [" << blockContext->scene->getName() << "]" << std::endl;

				if ( blockContext->scene->getName() == LoadingScreen::SceneName )
				{
					getEngine( *blockContext )->setLoadingScene( c3d::move( blockContext->ownScene ) );
				}
				else if ( blockContext->ownScene )
				{
					getEngine( *blockContext )->addScene( blockContext->scene->getName()
						, blockContext->ownScene
						, true );
				}
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserImportFile, SceneImportContext )
		{
			Path path;
			Path pathFile = context.file.getPath() / params[0]->get( path );
			blockContext->files.push_back( pathFile );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportAnimFile, SceneImportContext )
		{
			Path path;
			Path pathFile = context.file.getPath() / params[0]->get( path );
			blockContext->animFiles.push_back( pathFile );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportPrefix, SceneImportContext )
		{
			params[0]->get( blockContext->prefix );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportRescale, SceneImportContext )
		{
			params[0]->get( blockContext->rescale );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportPitch, SceneImportContext )
		{
			params[0]->get( blockContext->pitch );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportYaw, SceneImportContext )
		{
			params[0]->get( blockContext->yaw );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportRoll, SceneImportContext )
		{
			params[0]->get( blockContext->roll );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportNoOptimisations, SceneImportContext )
		{
			params[0]->get( blockContext->noOptimisations );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportNoValidation, SceneImportContext )
		{
			params[0]->get( blockContext->noValidation );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportEmissiveMult, SceneImportContext )
		{
			params[0]->get( blockContext->emissiveMult );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportTexRemap, SceneImportContext )
		{
			blockContext->textureRemaps.clear();
			blockContext->textureRemapIt = blockContext->textureRemaps.end();
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemap, blockContext )

		static CU_ImplementAttributeParserBlock( parserImportCenterCamera, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing name parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->centerCamera );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportPreferredImporter, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing name parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->preferredImporter );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportIgnoreVertexColour, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing name parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->ignoreVertexColour );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportDisableImageCompression, SceneImportContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing name parameter" ) );
			}
			else
			{
				params[0]->get( blockContext->disableImageCompression );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserImportEnd, SceneImportContext )
		{
			Engine * engine = getEngine( *blockContext );

			if ( blockContext->files.empty() )
			{
				CU_ParsingError( cuT( "No file chosen to import" ) );
			}
			else
			{
				Parameters parameters;

				if ( blockContext->rescale != 1.0f )
				{
					parameters.add( cuT( "rescale" ), blockContext->rescale );
				}

				if ( blockContext->pitch != 0.0f )
				{
					parameters.add( cuT( "pitch" ), blockContext->pitch );
				}

				if ( blockContext->yaw != 0.0f )
				{
					parameters.add( cuT( "yaw" ), blockContext->yaw );
				}

				if ( blockContext->roll != 0.0f )
				{
					parameters.add( cuT( "roll" ), blockContext->roll );
				}

				if ( !blockContext->prefix.empty() )
				{
					parameters.add( cuT( "prefix" ), blockContext->prefix );
				}

				if ( blockContext->noOptimisations )
				{
					parameters.add( cuT( "no_optimisations" ), blockContext->noOptimisations );
				}

				if ( blockContext->noValidation )
				{
					parameters.add( cuT( "no_validation" ), blockContext->noValidation );
				}

				if ( blockContext->ignoreVertexColour )
				{
					parameters.add( cuT( "ignore_vertex_colour" ), blockContext->ignoreVertexColour );
				}

				if ( blockContext->emissiveMult != 1.0f )
				{
					parameters.add( cuT( "emissive_mult" ), blockContext->emissiveMult );
				}

				if ( !blockContext->centerCamera.empty() )
				{
					parameters.add( cuT( "center_camera" ), blockContext->centerCamera );
				}

				if ( !blockContext->preferredImporter.empty() )
				{
					parameters.add( cuT( "preferred_importer" ), blockContext->preferredImporter );
				}

				if ( blockContext->disableImageCompression )
				{
					parameters.add( cuT( "disable_image_compression" ), blockContext->disableImageCompression );
				}

				SceneImporter importer{ *engine };

				for ( auto const & file : blockContext->files )
				{
					if ( !importer.importData( *blockContext->scene->scene
						, file
						, parameters
						, blockContext->textureRemaps
						, blockContext->scene->root->progress ) )
					{
						CU_ParsingError( cuT( "External scene Import failed" ) );
					}
				}

				for ( auto const & file : blockContext->animFiles )
				{
					if ( !importer.importAnimationsData( *blockContext->scene->scene
						, file
						, parameters
						, blockContext->scene->root->progress ) )
					{
						CU_ParsingError( cuT( "External scene Import failed" ) );
					}
				}
			}
		}
		CU_EndAttributePop()
	}

	//*************************************************************************************************

	String print( Point3f const & obj )
	{
		auto stream = makeStringStream();
		stream << std::setprecision( 4 ) << obj->x
			<< ", " << std::setprecision( 4 ) << obj->y
			<< ", " << std::setprecision( 4 ) << obj->z;
		return stream.str();
	}

	String print( BoundingBox const & obj )
	{
		auto stream = makeStringStream();
		stream << "min: " << print( obj.getMin() ) << ", max: " << print( obj.getMax() );
		return stream.str();
	}

	//*************************************************************************************************

	String Scene::RootNode = cuT( "C3D.RootNode" );
	String Scene::CameraRootNode = cuT( "C3D.CameraRootNode" );
	String Scene::ObjectRootNode = cuT( "C3D.ObjectRootNode" );

	Scene::Scene( String const & name, Engine & engine )
		: OwnedBy< Engine >{ engine }
		, Named{ name }
		, m_resources{ engine.getGraphResourceHandler() }
		, m_sceneNodeCache{ makeObjectCache< SceneNode, String, SceneNodeCacheTraits >( *this
			, SceneNodeInitialiserT< SceneNodeCache >{}
			, DummyFunctorT< SceneNodeCache >{}
			, SceneNodeMergerT< SceneNodeCache >{ getName() }
			, SceneNodeAttacherT< SceneNodeCache >{}
			, SceneNodeDetacherT< SceneNodeCache >{} ) }
		, m_rootNode{ m_sceneNodeCache->find( RootNode ) }
		, m_rootCameraNode{ m_sceneNodeCache->find( CameraRootNode ) }
		, m_rootObjectNode{ m_sceneNodeCache->find( ObjectRootNode ) }
		, m_background{ makeUniqueDerived< SceneBackground, ColourBackground >( engine, *this ) }
		, m_lightFactory{ makeUnique< LightFactory >() }
		, m_listener{ engine.addNewFrameListener( cuT( "Scene_" ) + name + string::toString( intptr_t( this ) ) ) }
		, m_renderNodes{ makeUnique< SceneRenderNodes >( *this ) }
		, m_sceneUbo{ *engine.getRenderDevice() }
	{
		m_rootNode->setSerialisable( false );
		m_rootCameraNode->setSerialisable( false );
		m_rootObjectNode->setSerialisable( false );
		m_billboardCache = makeObjectCache< BillboardList, String, BillboardListCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_cameraCache = makeObjectCache< Camera, String, CameraCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, DummyFunctorT< CameraCache >{}
			, DummyFunctorT< CameraCache >{}
			, MovableMergerT< CameraCache >{ getName() }
			, MovableAttacherT< CameraCache >{}
			, MovableDetacherT< CameraCache >{} );
		m_geometryCache = makeObjectCache< Geometry, String, GeometryCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_lightCache = makeObjectCache< Light, String, LightCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode );
		m_lightGroupCache = makeCache< LightGroup, String, LightGroupCacheTraits >( *this );
		m_particleSystemCache = makeObjectCache< ParticleSystem, String, ParticleSystemCacheTraits >( *this
			, m_rootNode
			, m_rootCameraNode
			, m_rootObjectNode
			, [this]( ParticleSystem & element )
			{
				auto & nodes = getRenderNodes();

				getListener().postEvent( makeGpuFunctorEvent( GpuEventType::ePreUpload
					, [&element, &nodes]( RenderDevice const & device
						, QueueData const & )
					{
						element.initialise( device );
						auto material = element.getMaterial();

						for ( auto const & pass : *material )
						{
							nodes.createNode( *pass
								, *element.getBillboards() );
						}
					} ) );
			}
			, GpuEventCleanerT< ParticleSystemCache >{ getListener() }
			, MovableMergerT< ParticleSystemCache >{ getName() }
			, MovableAttacherT< ParticleSystemCache >{}
			, MovableDetacherT< ParticleSystemCache >{} );
		m_animatedObjectGroupCache = makeCache< AnimatedObjectGroup, String, AnimatedObjectGroupCacheTraits >( *this );
		m_meshCache = makeCache< Mesh, String, MeshCacheTraits >( getLogger( engine )
			, CpuEventInitialiserT< MeshCache >{ getListener() }
			, CpuEventCleanerT< MeshCache >{ getListener() }
			, ResourceMergerT< MeshCache >{ getName() } );
		m_skeletonCache = makeCache< Skeleton, String, SkeletonCacheTraits >( getLogger( engine )
			, DummyFunctorT< SkeletonCache >{}
			, DummyFunctorT< SkeletonCache >{}
			, ResourceMergerT< SkeletonCache >{ getName() } );

		m_materialCacheView = makeCacheView( getName()
			, getEngine()->getMaterialCache()
			, [this]( MaterialCache::ElementT & element )
			{
				getListener().postEvent( makeCpuInitialiseEvent( element ) );
				m_materialsListeners.try_emplace( &element
					, element.onChanged.connect( [this]( Material const & material )
						{
							onMaterialChanged( material );
						} ) );
				m_dirtyMaterials = true;
			}
			, [this]( MaterialCache::ElementT & element )
			{
				m_dirtyMaterials = true;
				m_materialsListeners.erase( &element );
				element.cleanup();
			} );
		m_samplerCacheView = makeCacheView( getName()
			, getEngine()->getSamplerCache()
			, [this]( SamplerCache::ElementT & element )
			{
				element.initialise( getOwner()->getRenderSystem()->getRenderDevice() );
			}
			, []( SamplerCache::ElementT & element )
			{
				element.cleanup();
			} );
		m_overlayCache = makeCache< Overlay, String, OverlayCacheTraits >( *getEngine() );
		m_fontCacheView = makeCacheView( getName()
			, getEngine()->getFontCache( {} )
			, DummyFunctorT< FontCache >{}
			, DummyFunctorT< FontCache >{} );

		addNewAnimatedObjectGroup( cuT( "C3D_Textures" ), *this );
		auto & device = engine.getRenderSystem()->getRenderDevice();
		auto data = device.graphicsData();
		m_reflectionMap = makeUnique< EnvironmentMap >( m_resources
			, device
			, *data
			, *this );
	}

	Scene::~Scene()noexcept
	{
		if ( auto animTextures = m_animatedObjectGroupCache->tryFind( cuT( "C3D_Textures" ) ) )
		{
			for ( auto & [_, object] : animTextures->getObjects() )
			{
				if ( object->getKind() == AnimationType::eTexture )
				{
					getOwner()->getMaterialCache().unregisterTexture( static_cast< AnimatedTexture const & >( *object ) );
				}
			}
		}

		if ( m_cleanBackground )
		{
			m_cleanBackground->skip();
			m_cleanBackground = nullptr;
		}

		m_reflectionMap->cleanup();
		m_onSceneNodeChanged.disconnect();
		m_onGeometryChanged.disconnect();
		m_onBillboardListChanged.disconnect();
		m_onParticleSystemChanged.disconnect();

		m_meshCache->clear();
		m_overlayCache->clear();

		m_reflectionMap.reset();

		m_background.reset();
		m_animatedObjectGroupCache.reset();
		m_billboardCache.reset();
		m_particleSystemCache.reset();
		m_cameraCache.reset();
		m_geometryCache.reset();
		m_lightGroupCache.reset();
		m_lightCache.reset();

		m_meshCache.reset();
		m_skeletonCache.reset();
		m_materialCacheView.reset();
		m_samplerCacheView.reset();
		m_overlayCache.reset();
		m_fontCacheView.reset();

		if ( m_rootCameraNode )
		{
			m_rootCameraNode->detach( true );
			m_rootCameraNode = {};
		}

		if ( m_rootObjectNode )
		{
			m_rootObjectNode->detach( true );
			m_rootObjectNode = {};
		}

		if ( m_rootNode )
		{
			m_rootNode->detach( true );
			m_rootNode = {};
		}

		m_sceneNodeCache.reset();
	}

	void Scene::initialise()
	{
		auto & engine = *getEngine();
		auto const & device = engine.getRenderSystem()->getRenderDevice();
		auto mbName = toUtf8( getName() );
		m_timerParticlesGpu = makeUnique< crg::FramePassTimer >( device.makeContext(), mbName + "/ParticlesGPU", crg::TimerScope::eUpdate );
		engine.registerTimer( getName() + cuT( "/ParticlesGPU" ), *m_timerParticlesGpu );
#if C3D_DebugTimers
		m_timerSceneNodes = makeUnique< crg::FramePassTimer >( device.makeContext(), mbName + "/SceneNodes", crg::TimerScope::eUpdate );
		engine.registerTimer( getName() + cuT( "/SceneNodes" ), *m_timerSceneNodes );
		m_timerBoundingBox = makeUnique< crg::FramePassTimer >( device.makeContext(), mbName + "/BoundingBoxes", crg::TimerScope::eUpdate );
		engine.registerTimer( getName() + cuT( "/BoundingBoxes" ), *m_timerBoundingBox );
		m_timerMaterials = makeUnique< crg::FramePassTimer >( device.makeContext(), mbName + "/Materials", crg::TimerScope::eUpdate );
		engine.registerTimer( getName() + cuT( "/Materials" ), *m_timerMaterials );
		m_timerLights = makeUnique< crg::FramePassTimer >( device.makeContext(), mbName + "/Lights", crg::TimerScope::eUpdate );
		engine.registerTimer( getName() + cuT( "/Lights" ), *m_timerLights );
		m_timerParticlesCpu = makeUnique< crg::FramePassTimer >( device.makeContext(), mbName + "/ParticlesCPU", crg::TimerScope::eUpdate );
		engine.registerTimer( getName() + cuT( "/ParticlesCPU" ), *m_timerParticlesCpu );
		m_timerGpuUpdate = makeUnique< crg::FramePassTimer >( device.makeContext(), mbName + "/GPUUpdate", crg::TimerScope::eUpdate );
		engine.registerTimer( getName() + cuT( "/GPUUpdate" ), *m_timerGpuUpdate );
		m_timerMovables = makeUnique< crg::FramePassTimer >( device.makeContext(), mbName + "/Movables", crg::TimerScope::eUpdate );
		engine.registerTimer( getName() + cuT( "/Movables" ), *m_timerMovables );
#endif

		m_animatedObjectGroupCache->initialise( device );
		m_lightCache->initialise( device );
		m_lightGroupCache->initialise( device );
		m_background->initialise( device );
		doUpdateLightsDependent();
		updateBoundingBox();
		doUpdateMaterials();
		log::info << "Initialised scene [" << getName() << "], AABB: " << print( m_boundingBox ) << std::endl;
		m_initialised = true;
	}

	void Scene::updateBoundingBox()
	{
#if C3D_DebugTimers
		auto block( m_timerBoundingBox ? makeRawUnique< crg::FramePassTimerBlock >( m_timerBoundingBox->start() ) : nullptr );
#endif
		auto & cache = *m_geometryCache;
		auto lock( makeUniqueLock( cache ) );

		if ( !cache.isEmptyNoLock() )
		{
			constexpr float fmin = std::numeric_limits< float >::max();
			constexpr float fmax = std::numeric_limits< float >::lowest();
			Point3f min{ fmin, fmin, fmin };
			Point3f max{ fmax, fmax, fmax };

			for ( auto const & [_, geometry] : cache )
			{
				auto node = geometry->getParent();
				auto mesh = geometry->getMesh();

				if ( node && mesh )
				{
					auto bbox = mesh->getBoundingBox().getAxisAligned( geometry->getGlobalTransform() );

					for ( auto i = 0u; i < 3u; ++i )
					{
						min[i] = std::min( min[i], bbox.getMin()[i] );
						max[i] = std::max( max[i], bbox.getMax()[i] );
					}
				}
			}

			m_boundingBox.load( min, max );
		}
		else
		{
			m_boundingBox = BoundingBox{};
		}
	}

	void Scene::cleanup()
	{
		m_initialised = false;
		m_dirtyNodes.clear();
		m_dirtyBillboards.clear();
		m_dirtyObjects.clear();
		m_dirtyLightGroups.clear();

		getEngine()->getControlsManager()->destroyControls( *this );

		m_animatedObjectGroupCache->cleanup();
		m_geometryCache->cleanup();
		m_cameraCache->cleanup();
		m_lightGroupCache->cleanup();
		m_lightCache->cleanup();
		m_billboardCache->cleanup();
		m_particleSystemCache->cleanup();
		m_sceneNodeCache->cleanup();
		m_skeletonCache->cleanup();
		m_overlayCache->cleanup();

		m_materialCacheView->clear();
		m_samplerCacheView->clear();
		m_fontCacheView->clear();

		m_renderNodes->clear();

		m_meshCache->cleanup();

		if ( m_cleanBackground )
		{
			m_cleanBackground->skip();
			m_cleanBackground = nullptr;
		}

		m_cleanBackground = getListener().postEvent( makeCpuFunctorEvent( CpuEventType::ePreGpuStep
			, [this]()
			{
				m_background->cleanup();
				m_cleanBackground = nullptr;
			} ) );

		auto & engine = *getEngine();
#if C3D_DebugTimers
		engine.unregisterTimer( getName() + cuT( "/SceneNodes" ), *m_timerSceneNodes );
		m_timerSceneNodes.reset();
		engine.unregisterTimer( getName() + cuT( "/BoundingBoxes" ), *m_timerBoundingBox );
		m_timerBoundingBox.reset();
		engine.unregisterTimer( getName() + cuT( "/Materials" ), *m_timerMaterials );
		m_timerMaterials.reset();
		engine.unregisterTimer( getName() + cuT( "/Lights" ), *m_timerLights );
		m_timerLights.reset();
		engine.unregisterTimer( getName() + cuT( "/ParticlesCPU" ), *m_timerParticlesCpu );
		m_timerParticlesCpu.reset();
		engine.unregisterTimer( getName() + cuT( "/GPUUpdate" ), *m_timerGpuUpdate );
		m_timerGpuUpdate.reset();
		engine.unregisterTimer( getName() + cuT( "/Movables" ), *m_timerMovables );
		m_timerMovables.reset();
#endif
		engine.unregisterTimer( getName() + cuT( "/ParticlesGPU" ), *m_timerParticlesGpu );
		m_timerParticlesGpu.reset();

		{
			auto lock( makeUniqueLock( getEngine()->getRenderTargetCache() ) );
			for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eTexture ) )
			{
				if ( target->getScene() == this )
				{
					target->cleanup( *getEngine()->getRenderDevice() );
				}
			}

			for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eWindow ) )
			{
				if ( target->getScene() == this )
				{
					target->cleanup( *getEngine()->getRenderDevice() );
				}
			}
		}
	}

	void Scene::update( CpuUpdater & updater )
	{
		if ( m_initialised )
		{
			onUpdate( *this );
			updater.scene = this;
			auto & sceneObjs = updater.dirtyScenes.try_emplace( this ).first->second;
			doGatherDirty( sceneObjs );
			doUpdateSceneNodes( sceneObjs );
			m_animatedObjectGroupCache->update( updater );
			doUpdateMovables( sceneObjs );

			if ( !sceneObjs.dirtyGeometries.empty()
				|| !sceneObjs.dirtyNodes.empty() )
			{
				updateBoundingBox();
			}

			doUpdateMaterials();
			doUpdateLights( updater, sceneObjs );
			m_renderNodes->update( updater );
			doUpdateParticles( updater );
			doUpdateLightsDependent();

			m_sceneUbo.cpuUpdate( *this );

			m_changed = false;
		}
	}

	void Scene::upload( UploadData & uploader )
	{
		getLightCache().upload( uploader );
		m_meshCache->forEach( [&uploader]( Mesh const & mesh )
			{
				for ( auto const & submesh : mesh )
				{
					submesh->upload( uploader );
				}
			} );
		m_background->upload( uploader );
	}

	void Scene::update( GpuUpdater & updater )
	{
#if C3D_DebugTimers
		auto block( m_timerGpuUpdate->start() );
#endif
		updater.scene = this;
		updater.info.visible.lightsCount += getLightCache().getLightsBufferCount( LightType::eDirectional );
		updater.info.visible.lightsCount += getLightCache().getLightsBufferCount( LightType::eSpot );
		updater.info.visible.lightsCount += getLightCache().getLightsBufferCount( LightType::ePoint );
		doUpdateParticles( updater );
		m_renderNodes->update( updater );
	}

	void Scene::setBackground( SceneBackgroundUPtr value )
	{
		m_background = c3d::move( value );
		m_background->initialise( getEngine()->getRenderSystem()->getRenderDevice() );
		onSetBackground( *m_background );
	}

	void Scene::merge( Scene & scene )
	{
		scene.getAnimatedObjectGroupCache().mergeInto( *m_animatedObjectGroupCache );
		scene.getCameraCache().mergeInto( *m_cameraCache );
		scene.getBillboardListCache().mergeInto( *m_billboardCache );
		scene.getParticleSystemCache().mergeInto( *m_particleSystemCache );
		scene.getGeometryCache().mergeInto( *m_geometryCache );
		scene.getLightCache().mergeInto( *m_lightCache );
		scene.getLightGroupCache().mergeInto( *m_lightGroupCache );
		scene.getSceneNodeCache().mergeInto( *m_sceneNodeCache );
		m_ambientLight = scene.getAmbientLight();
		scene.cleanup();
	}

	uint32_t Scene::getVertexCount()const
	{
		uint32_t result = 0;
		using LockType = UniqueLock< GeometryCache >;
		LockType lock{ makeUniqueLock( *m_geometryCache ) };

		for ( auto const & [_, geometry] : *m_geometryCache )
		{
			if ( auto mesh = geometry->getMesh() )
			{
				result += mesh->getVertexCount();
			}
		}

		return result;
	}

	uint32_t Scene::getFaceCount()const
	{
		uint32_t result = 0;
		using LockType = UniqueLock< GeometryCache >;
		LockType lock{ makeUniqueLock( *m_geometryCache ) };

		for ( auto const & [_, geometry] : *m_geometryCache )
		{
			if ( auto mesh = geometry->getMesh() )
			{
				result += mesh->getFaceCount();
			}
		}

		return result;
	}

	SceneFlags Scene::getFlags()const
	{
		SceneFlags result{};

		switch ( m_fog.getType() )
		{
		case FogType::eLinear:
			result |= SceneFlag::eFogLinear;
			break;

		case FogType::eExponential:
			result |= SceneFlag::eFogExponential;
			break;

		case FogType::eSquaredExponential:
			result |= SceneFlag::eFogSquaredExponential;
			break;

		default:
			break;
		}

		if ( m_hasShadows[size_t( LightType::eDirectional )] )
		{
			result |= SceneFlag::eShadowDirectional;
		}

		if ( m_hasShadows[size_t( LightType::ePoint )] )
		{
			result |= SceneFlag::eShadowPoint;
		}

		if ( m_hasShadows[size_t( LightType::eSpot )] )
		{
			result |= SceneFlag::eShadowSpot;
		}

		if ( m_voxelConfig.enabled )
		{
			result |= SceneFlag::eVoxelConeTracing;
		}
		else
		{
			if ( needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eRsm )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eRsm )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eRsm ) )
			{
				result |= SceneFlag::eRsmGI;
			}

			if ( needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLpvG )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLpvG )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLpv )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLpvG ) )
			{
				result |= SceneFlag::eLpvGI;
			}

			if ( needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::eDirectional, GlobalIlluminationType::eLayeredLpvG )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::ePoint, GlobalIlluminationType::eLayeredLpvG )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLayeredLpv )
				|| needsGlobalIllumination( LightType::eSpot, GlobalIlluminationType::eLayeredLpvG ) )
			{
				if ( getEngine()->getRenderSystem()->hasLLPV() )
				{
					result |= SceneFlag::eLayeredLpvGI;
				}
				else
				{
					result |= SceneFlag::eLpvGI;
				}
			}
		}

		return result;
	}

	bool Scene::hasShadows()const
	{
		return m_hasAnyShadows;
	}

	bool Scene::hasShadows( LightType lightType )const
	{
		return m_hasShadows[size_t( lightType )];
	}

	void Scene::addEnvironmentMap( SceneNode & node )
	{
		m_reflectionMap->addNode( node );
		markDirty( node );
	}

	void Scene::removeEnvironmentMap( SceneNode & node )
	{
		m_reflectionMap->removeNode( node );
		markDirty( node );
	}

	bool Scene::hasEnvironmentMap( SceneNode & node )const
	{
		return m_reflectionMap->hasNode( node );
	}

	EnvironmentMap & Scene::getEnvironmentMap()const
	{
		return *m_reflectionMap;
	}

	uint32_t Scene::getEnvironmentMapIndex( SceneNode const & node )const
	{
		return m_reflectionMap->getIndex( node );
	}

	String const & Scene::getBackgroundModel()const
	{
		return m_background->getModelName();
	}

	Vector< LightingModelID > Scene::getLightingModelsID()const
	{
		return getEngine()->getLightingModelFactory().getLightingModelsID();
	}

	AnimatedObjectRPtr Scene::addAnimatedTexture( TextureSourceInfo const & sourceInfo
		, TextureConfiguration const & config
		, Pass & pass )
	{
		auto const & cache = getAnimatedObjectGroupCache();
		auto group = cache.find( cuT( "C3D_Textures" ) );
		return group->addObject( sourceInfo, config, pass );
	}

	void Scene::markDirty( SceneNode & node )
	{
		if ( !node.isDisplayable() )
		{
			return;
		}

		Vector< SceneNode * > work;
		work.push_back( &node );

		while ( !work.empty() )
		{
			auto & curNode = *work.back();
			work.pop_back();
			auto it = std::find( m_dirtyNodes.begin()
				, m_dirtyNodes.end()
				, &curNode );

			if ( it != m_dirtyNodes.end() )
			{
				m_dirtyNodes.erase( it );
			}

			if ( auto parent = curNode.getParent();
				!parent )
			{
				it = m_dirtyNodes.begin();
			}
			else
			{
				do
				{
					it = std::find( m_dirtyNodes.begin()
						, m_dirtyNodes.end()
						, parent );
					parent = parent->getParent();

				}
				while ( parent && it == m_dirtyNodes.end() );

				if ( it == m_dirtyNodes.end() )
				{
					it = m_dirtyNodes.begin();
				}
				else
				{
					it = std::next( it );
				}
			}

			m_dirtyNodes.insert( it, &curNode );

			for ( auto & object : curNode.getObjects() )
			{
				markDirty( object.get() );
			}

			for ( auto const & [_, nd] : curNode.getChildren() )
			{
				if ( nd )
				{
					work.push_back( nd );
				}
			}
		}
	}

	void Scene::markDirty( BillboardBase & object )
	{
		if ( !object.getNode()->isDisplayable() )
		{
			return;
		}

		auto it = std::find( m_dirtyBillboards.begin()
			, m_dirtyBillboards.end()
			, &object );

		if ( it == m_dirtyBillboards.end() )
		{
			m_dirtyBillboards.emplace_back( &object );
		}
	}

	void Scene::markDirty( MovableObject & object )
	{
		if ( !object.getParent()->isDisplayable() )
		{
			return;
		}

		auto it = std::find( m_dirtyObjects.begin()
			, m_dirtyObjects.end()
			, &object );

		if ( it == m_dirtyObjects.end() )
		{
			m_dirtyObjects.emplace_back( &object );
		}
	}

	void Scene::markDirty( LightGroup & object )
	{
		auto it = std::find( m_dirtyLightGroups.begin()
			, m_dirtyLightGroups.end()
			, &object );

		if ( it == m_dirtyLightGroups.end() )
		{
			m_dirtyLightGroups.emplace_back( &object );
		}
	}

	void Scene::addParsers( AttributeParsers & result )
	{
		BlockParserContextT< SceneContext > sceneCtx{ result, CSCNSection::eScene, CSCNSection::eRoot };
		BlockParserContextT< SceneImportContext > importCtx{ result, CSCNSection::eSceneImport, CSCNSection::eScene };

		sceneCtx.addParser( cuT( "background_colour" ), scene::parserBkColour, { makeParameter< ParameterType::eRgbColour >() } );
		sceneCtx.addParser( cuT( "background_image" ), scene::parserBkImage, { makeParameter< ParameterType::ePath >() } );
		sceneCtx.addParser( cuT( "ambient_light" ), scene::parserAmbientLight, { makeParameter< ParameterType::eRgbColour >() } );
		sceneCtx.addParser( cuT( "fog_type" ), scene::parserFogType, { makeParameter< ParameterType::eCheckedText, FogType >() } );
		sceneCtx.addParser( cuT( "fog_density" ), scene::parserFogDensity, { makeParameter< ParameterType::eFloat >() } );
		sceneCtx.addParser( cuT( "directional_shadow_cascades" ), scene::parserDirectionalShadowCascades, { makeParameter< ParameterType::eUInt32 >( makeRange( 0u, MaxDirectionalCascadesCount ) ) } );
		sceneCtx.addParser( cuT( "lpv_indirect_attenuation" ), scene::parserGlobalIndirectAttenuation, { makeParameter< ParameterType::eFloat >() } );
		sceneCtx.addPushParser( cuT( "font" ), CSCNSection::eFont, scene::parserFont, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "sdf_font" ), CSCNSection::eSdfFont, scene::parserSdfFont, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "sampler" ), CSCNSection::eSampler, scene::parserSamplerState, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "camera" ), CSCNSection::eCamera, scene::parserCamera, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "camera_node" ), CSCNSection::eNode, scene::parserCameraNode, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "scene_node" ), CSCNSection::eNode, scene::parserNode, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "object" ), CSCNSection::eObject, scene::parserObject, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "import" ), CSCNSection::eSceneImport, scene::parserImport );
		sceneCtx.addPushParser( cuT( "billboard" ), CSCNSection::eBillboard, scene::parserBillboard, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "animated_object_group" ), CSCNSection::eAnimGroup, scene::parserAnimatedObjectGroup, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, scene::parserPanelOverlay, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, scene::parserBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, scene::parserTextOverlay, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "skybox" ), CSCNSection::eSkybox, scene::parserSkybox );
		sceneCtx.addPushParser( cuT( "particle_system" ), CSCNSection::eParticleSystem, scene::parserParticleSystem, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "skeleton" ), CSCNSection::eSkeleton, scene::parserSkeleton, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPushParser( cuT( "mesh" ), CSCNSection::eMesh, scene::parserMesh, { makeParameter< ParameterType::eName >() } );
		sceneCtx.addPopParser( cuT( "}" ), scene::parserEnd );

		importCtx.addParser( cuT( "file" ), scene::parserImportFile, { makeParameter< ParameterType::ePath >() } );
		importCtx.addParser( cuT( "file_anim" ), scene::parserImportAnimFile, { makeParameter< ParameterType::ePath >() } );
		importCtx.addParser( cuT( "prefix" ), scene::parserImportPrefix, { makeParameter< ParameterType::eText >() } );
		importCtx.addParser( cuT( "rescale" ), scene::parserImportRescale, { makeParameter< ParameterType::eFloat >() } );
		importCtx.addParser( cuT( "pitch" ), scene::parserImportPitch, { makeParameter< ParameterType::eFloat >() } );
		importCtx.addParser( cuT( "yaw" ), scene::parserImportYaw, { makeParameter< ParameterType::eFloat >() } );
		importCtx.addParser( cuT( "roll" ), scene::parserImportRoll, { makeParameter< ParameterType::eFloat >() } );
		importCtx.addParser( cuT( "no_optimisations" ), scene::parserImportNoOptimisations, { makeParameter< ParameterType::eBool >() } );
		importCtx.addParser( cuT( "no_validation" ), scene::parserImportNoValidation, { makeParameter< ParameterType::eBool >() } );
		importCtx.addParser( cuT( "emissive_mult" ), scene::parserImportEmissiveMult, { makeParameter< ParameterType::eFloat >() } );
		importCtx.addParser( cuT( "recenter_camera" ), scene::parserImportCenterCamera, { makeParameter< ParameterType::eName >() } );
		importCtx.addParser( cuT( "preferred_importer" ), scene::parserImportPreferredImporter, { makeParameter< ParameterType::eName >() } );
		importCtx.addParser( cuT( "ignore_vertex_colour" ), scene::parserImportIgnoreVertexColour, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		importCtx.addParser( cuT( "disable_image_compression" ), scene::parserImportDisableImageCompression, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
		importCtx.addPushParser( cuT( "texture_remap_config" ), CSCNSection::eTextureRemap, scene::parserImportTexRemap );
		importCtx.addPopParser( cuT( "}" ), scene::parserImportEnd );
	}

	BackgroundModelID Scene::getBackgroundModelId()const
	{
		return m_background
			? m_background->getModelID()
			: BackgroundModelID( 0u );
	}

	LightingModelID Scene::getDefaultLightingModel()const
	{
		return getEngine()->getDefaultLightingModel();
	}

	String Scene::getDefaultLightingModelName()const
	{
		return getEngine()->getDefaultLightingModelName();
	}

	bool Scene::needsGlobalIllumination()const
	{
		return m_needsGlobalIllumination;
	}

	bool Scene::needsGlobalIllumination( GlobalIlluminationType giType )const
	{
		return std::any_of( m_giTypes.begin()
			, m_giTypes.end()
			, [&giType]( Set< GlobalIlluminationType > const & lookup )
			{
				return lookup.end() != lookup.find( giType );
			} );
	}

	bool Scene::needsGlobalIllumination( LightType ltType
		, GlobalIlluminationType giType )const
	{
		return m_giTypes[size_t( ltType )].end() != m_giTypes[size_t( ltType )].find( giType );
	}

	void Scene::setDefaultLightingModel( LightingModelID value )const
	{
		getEngine()->setDefaultLightingModel( value );
	}

	SemaphoreWaitArray Scene::getRenderTargetsSemaphores()const
	{
		SemaphoreWaitArray result;
		auto lock( makeUniqueLock( getEngine()->getRenderTargetCache() ) );

		for ( auto & target : getEngine()->getRenderTargetCache().getRenderTargets( TargetType::eTexture ) )
		{
			if ( target->getScene() != this )
			{
				auto toWait = target->getSemaphore();
				result.insert( result.end()
					, toWait.begin()
					, toWait.end() );
			}
		}

		return result;
	}

	uint32_t Scene::getLpvGridSize()const
	{
		return getEngine()->getLpvGridSize();
	}

	bool Scene::hasBindless()const
	{
		auto device = getEngine()->getRenderDevice();
		CU_Require( getEngine()->getRenderDevice() );
		return device->hasBindless();
	}

	ashes::DescriptorSetLayout * Scene::getBindlessTexDescriptorLayout()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorLayout();
	}

	ashes::DescriptorPool * Scene::getBindlessTexDescriptorPool()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorPool();
	}

	ashes::DescriptorSet * Scene::getBindlessTexDescriptorSet()const
	{
		return getEngine()->getTextureUnitCache().getDescriptorSet();
	}

	ashes::Buffer< ModelBufferConfiguration > const & Scene::getModelBuffer()const
	{
		CU_Require( m_renderNodes );
		return m_renderNodes->getModelBuffer();
	}

	ashes::Buffer< BillboardUboConfiguration > const & Scene::getBillboardsBuffer()const
	{
		CU_Require( m_renderNodes );
		return m_renderNodes->getBillboardsBuffer();
	}

	bool Scene::hasObjects( LightingModelID lightingModelId )const
	{
		return m_renderNodes->hasNodes( lightingModelId );
	}

	void Scene::setDirectionalShadowCascades( uint32_t value )
	{
		CU_Require( value <= MaxDirectionalCascadesCount );
		m_directionalShadowCascades = value;
	}

	void Scene::setLpvIndirectAttenuation( float value )
	{
		m_lpvIndirectAttenuation = value;
	}

	void Scene::doGatherDirty( CpuUpdater::DirtyObjects & sceneObjs )
	{
		sceneObjs.dirtyNodes.insert( sceneObjs.dirtyNodes.end()
			, m_dirtyNodes.begin()
			, m_dirtyNodes.end() );

		for ( auto movable : m_dirtyObjects )
		{
			switch ( movable->getMovableType() )
			{
			case MovableType::eGeometry:
				sceneObjs.dirtyGeometries.emplace_back( static_cast< Geometry * >( movable ) );
				break;
			case MovableType::eBillboard:
				markDirty( *static_cast< BillboardBase * >( static_cast< BillboardList * >( movable ) ) );
				break;
			case MovableType::eParticleEmitter:
				if ( auto billboards = static_cast< ParticleSystem & >( *movable ).getBillboards() )
				{
					markDirty( *billboards );
				}
				break;
			case MovableType::eLight:
				sceneObjs.dirtyLights.emplace_back( static_cast< Light * >( movable )->getInstance() );
				break;
			case MovableType::eCamera:
				sceneObjs.dirtyCameras.emplace_back( static_cast< Camera * >( movable ) );
				break;
			default:
				break;
			}
		}

		for ( auto group : m_dirtyLightGroups )
		{
			for ( auto & instance : *group )
			{
				sceneObjs.dirtyLights.emplace_back( instance.get() );
			}
		}

		if ( !sceneObjs.dirtyCameras.empty() )
		{
			for ( auto const & [_, light] : getLightCache() )
			{
				if ( light->getLightType() == LightType::eDirectional )
				{
					sceneObjs.dirtyLights.push_back( light->getInstance() );
				}
			}
		}

		sceneObjs.dirtyBillboards.insert( sceneObjs.dirtyBillboards.end()
			, m_dirtyBillboards.begin()
			, m_dirtyBillboards.end() );
		m_dirtyBillboards.clear();
		m_dirtyLightGroups.clear();
		m_dirtyObjects.clear();
		m_dirtyNodes.clear();
	}

	void Scene::doUpdateSceneNodes( CpuUpdater::DirtyObjects const & sceneObjs )const
	{
#if C3D_DebugTimers
		auto block( m_timerSceneNodes->start() );
#endif

		for ( auto & node : sceneObjs.dirtyNodes )
		{
			node->update();
		}
	}

	void Scene::doUpdateMovables( CpuUpdater::DirtyObjects const & sceneObjs )
	{
#if C3D_DebugTimers
		auto block( m_timerMovables->start() );
#endif

		for ( auto camera : sceneObjs.dirtyCameras )
		{
			camera->update();
		}

		for ( auto object : sceneObjs.dirtyGeometries )
		{
			bool dirty = false;
			auto & geometry = *object;

			for ( auto const & [pass, submeshes] : geometry.getIds() )
			{
				for ( auto & [_, rendered] : submeshes )
				{
					auto const & submesh = rendered.second->data;

					if ( submesh.isInitialised() )
					{
						geometry.fillEntry( rendered.first
							, *pass
							, geometry
							, submesh.getMeshletsCount()
							, submesh.getIndexCount()
							, submesh.getPointsCount()
							, rendered.second->modelData );
						geometry.fillEntryOffsets( rendered.first
							, submesh.getVertexOffset( geometry, *pass )
							, submesh.getIndexOffset()
							, submesh.getMeshletOffset() );
					}
					else
					{
						markDirty( geometry );
					}
				}

				dirty = dirty || pass->getId() == 0;
			}

			if ( dirty )
			{
				markDirty( geometry );
			}
		}

		for ( auto object : sceneObjs.dirtyBillboards )
		{
			bool dirty = false;

			for ( auto & [pass, billboard] : object->getIds() )
			{
				object->fillEntry( billboard.first
					, *pass
					, *object
					, 0u
					, 0u
					, 0u
					, billboard.second->modelData );
				object->fillEntryOffsets( billboard.first
					, 0u
					, 0u
					, 0u );
				object->fillData( billboard.second->billboardData );
				dirty = dirty || pass->getId() == 0;
			}

			if ( dirty )
			{
				markDirty( *object );
			}
		}
	}

	void Scene::doUpdateLights( CpuUpdater & updater
		, CpuUpdater::DirtyObjects const & sceneObjs )
	{
#if C3D_DebugTimers
		auto block( m_timerLights->start() );
#endif

		for ( auto const & light : sceneObjs.dirtyLights )
		{
			doUpdateLightDependent( light->getLightType()
				, light->isShadowProducer()
				, light->getExpectedGlobalIlluminationType() );
		}

		m_lightCache->update( updater );
	}

	void Scene::doUpdateParticles( CpuUpdater & updater )
	{
#if C3D_DebugTimers
		auto block( m_timerParticlesCpu->start() );
#endif
		auto & cache = getParticleSystemCache();
		auto lock( makeUniqueLock( cache ) );
		updater.index = 0u;

		for ( auto const & [_, particleSystem] : cache )
		{
			particleSystem->update( updater );
		}
	}

	void Scene::doUpdateParticles( GpuUpdater & updater )
	{
		auto & cache = getParticleSystemCache();
		auto lock( makeUniqueLock( cache ) );

		if ( !m_timerParticlesGpu )
		{
			return;
		}

		auto timerBlock( m_timerParticlesGpu->start() );
		updater.index = 0u;
		updater.timer = m_timerParticlesGpu.get();

		for ( auto const & [_, particleSystem] : cache )
		{
			particleSystem->update( updater );
		}
	}

	void Scene::doUpdateMaterials()
	{
#if C3D_DebugTimers
		auto block( m_timerMaterials->start() );
#endif

		if ( m_dirtyMaterials )
		{
			auto const & cache = getEngine()->getMaterialCache();
			cache.lock();
			m_needsSubsurfaceScattering = false;
			m_hasTransparentObjects = false;
			m_hasOpaqueObjects = false;

			for ( auto & matName : *m_materialCacheView )
			{
				if ( cache.hasNoLock( matName ) )
				{
					if ( auto material = cache.findNoLock( matName ) )
					{
						m_needsSubsurfaceScattering |= material->hasSubsurfaceScattering();

						for ( auto const & pass : *material )
						{
							m_hasTransparentObjects |= pass->hasAlphaBlending();
							m_hasOpaqueObjects |= !pass->hasOnlyAlphaBlending();
						}
					}
				}
			}

			cache.unlock();
			m_dirtyMaterials = false;
		}
	}

	bool Scene::doUpdateLightDependent( LightType lightType
		, bool shadowProducer
		, GlobalIlluminationType globalIllumination )
	{
		auto changed = m_hasShadows[size_t( lightType )] != shadowProducer
			|| ( globalIllumination != GlobalIlluminationType::eNone
				&& m_giTypes[size_t( lightType )].find( globalIllumination ) == m_giTypes[size_t( lightType )].end() );

		if ( changed )
		{
			m_hasShadows[size_t( lightType )] = shadowProducer;

			if ( globalIllumination != GlobalIlluminationType::eNone )
			{
				m_giTypes[size_t( lightType )].insert( globalIllumination );
			}
		}

		return changed;
	}

	bool Scene::doUpdateLightsDependent()
	{
		bool needsGI = false;
		bool hasAnyShadows = false;
		Array< bool, size_t( LightType::eCount ) > hasShadows{};
		Array< Set< GlobalIlluminationType >, size_t( LightType::eCount ) > giTypes{};

		m_lightCache->forEach( [&giTypes, &needsGI, &hasAnyShadows, &hasShadows]( Light const & light )
			{
				if ( light.getExpectedGlobalIlluminationType() != GlobalIlluminationType::eNone )
				{
					giTypes[uint32_t( light.getLightType() )].insert( light.getExpectedGlobalIlluminationType() );
					needsGI = true;
				}

				if ( light.isExpectedShadowProducer() )
				{
					hasAnyShadows = true;
					hasShadows[size_t( light.getLightType() )] = true;
				}
			} );

		m_lightGroupCache->forEach( [&giTypes, &needsGI, &hasAnyShadows, &hasShadows]( LightGroup const & light )
			{
				if ( light.getExpectedGlobalIlluminationType() != GlobalIlluminationType::eNone )
				{
					giTypes[uint32_t( light.getLightType() )].insert( light.getExpectedGlobalIlluminationType() );
					needsGI = true;
				}

				if ( light.isExpectedShadowProducer() )
				{
					hasAnyShadows = true;
					hasShadows[size_t( light.getLightType() )] = true;
				}
			} );

		size_t i = 0u;
		bool changed = false;
		for ( auto & shadows : m_hasShadows )
		{
			changed = shadows.exchange( hasShadows[i] ) != hasShadows[i]
				|| changed;
			++i;
		}

		i = 0u;
		for ( auto & types : m_giTypes )
		{
			changed = types != giTypes[i]
				|| changed;
			types = giTypes[i];
			++i;
		}

		if ( !needsGI )
		{
			needsGI = getVoxelConeTracingConfig().enabled;
		}

		changed = m_hasAnyShadows.exchange( hasAnyShadows ) != hasAnyShadows
			|| changed;
		changed = m_needsGlobalIllumination.exchange( needsGI ) != needsGI
			|| changed;
		return changed;
	}

	void Scene::onMaterialChanged( Material const & )
	{
		m_dirtyMaterials = true;
	}

	String getPrefix( SceneContext const & context )
	{
		return context.prefix;
	}

	Engine * getEngine( SceneContext const & context )
	{
		return getEngine( *context.root );
	}
}
