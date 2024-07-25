#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Animation/AnimationImporter.hpp"
#include "Castor3D/Animation/AnimationImporterFactory.hpp"
#include "Castor3D/Binary/BinarySceneNodeAnimation.hpp"
#include "Castor3D/Cache/AnimatedObjectGroupCache.hpp"
#include "Castor3D/Cache/BillboardCache.hpp"
#include "Castor3D/Cache/CacheView.hpp"
#include "Castor3D/Cache/GeometryCache.hpp"
#include "Castor3D/Cache/LightCache.hpp"
#include "Castor3D/Cache/MaterialCache.hpp"
#include "Castor3D/Cache/ObjectCache.hpp"
#include "Castor3D/Cache/ShaderCache.hpp"
#include "Castor3D/Cache/TargetCache.hpp"
#include "Castor3D/Event/Frame/GpuFunctorEvent.hpp"
#include "Castor3D/Material/Material.hpp"
#include "Castor3D/Material/MaterialImporter.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Pass/Component/PassComponentRegister.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Miscellaneous/LoadingScreen.hpp"
#include "Castor3D/Model/Mesh/MeshImporter.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshFactory.hpp"
#include "Castor3D/Model/Mesh/MeshGenerator.hpp"
#include "Castor3D/Model/Mesh/MeshPreparer.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp"
#include "Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Model/Skeleton/SkeletonImporter.hpp"
#include "Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Render/PostEffect/PostEffect.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/SceneFileParserData.hpp"
#include "Castor3D/Scene/SceneImporter.hpp"
#include "Castor3D/Scene/SceneNode.hpp"
#include "Castor3D/Scene/Animation/AnimatedObjectGroup.hpp"
#include "Castor3D/Scene/Animation/AnimatedTexture.hpp"
#include "Castor3D/Scene/Background/Colour.hpp"
#include "Castor3D/Scene/Background/Image.hpp"
#include "Castor3D/Scene/Background/Skybox.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Scene/Light/PointLight.hpp"
#include "Castor3D/Scene/Light/SpotLight.hpp"
#include "Castor3D/Scene/ParticleSystem/ComputeParticleSystem.hpp"
#include "Castor3D/Scene/ParticleSystem/CpuParticleSystem.hpp"
#include "Castor3D/Scene/ParticleSystem/ParticleSystem.hpp"
#include "Castor3D/Shader/Program.hpp"

#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/FileParser/FileParser.hpp>

using castor::operator<<;

namespace castor3d
{
	namespace scnprs
	{
		static SceneRPtr getScene( castor::FileParserContext const & context
			, OverlayContext const * blockContext
			, OverlayContext::OverlayPtr const & overlay )
		{
			if ( overlay.rptr )
			{
				return overlay.rptr->getScene();
			}

			if ( !blockContext || !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene context in overlay context" ) );
				CU_Failure( "No scene context in overlay context" );
				CU_Exception( "No scene context in overlay context" );
			}

			return blockContext->scene->scene;
		}

		static void fillMeshImportParameters( castor::FileParserContext & context
			, castor::String const & meshParams
			, Parameters & parameters )
		{
			for ( auto const & param : castor::string::split( meshParams, cuT( "-" ), 20, false ) )
			{
				if ( param.find( cuT( "smooth_normals" ) ) == 0 )
				{
					castor::String strNml = cuT( "smooth" );
					parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
				}
				else if ( param.find( cuT( "flat_normals" ) ) == 0 )
				{
					castor::String strNml = cuT( "flat" );
					parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
				}
				else if ( param.find( cuT( "tangent_space" ) ) == 0 )
				{
					parameters.add( cuT( "tangent_space" ), true );
				}
				else if ( param.find( cuT( "pitch" ) ) == 0 )
				{
					if ( auto eqIndex = param.find( cuT( '=' ) );
						eqIndex != castor::String::npos )
					{
						float value;
						castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "pitch" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -pitch=<degrees>." ) );
					}
				}
				else if ( param.find( cuT( "yaw" ) ) == 0 )
				{
					if ( auto eqIndex = param.find( cuT( '=' ) );
						eqIndex != castor::String::npos )
					{
						float value;
						castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "yaw" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -yaw=<degrees>." ) );
					}
				}
				else if ( param.find( cuT( "roll" ) ) == 0 )
				{
					if ( auto eqIndex = param.find( cuT( '=' ) );
						eqIndex != castor::String::npos )
					{
						float value;
						castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "roll" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -roll=<degrees>." ) );
					}
				}
				else if ( param.find( cuT( "emissive_mult" ) ) == 0 )
				{
					if ( auto eqIndex = param.find( cuT( '=' ) );
						eqIndex != castor::String::npos )
					{
						float value;
						castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "emissive_mult" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -emissive_mult=<float>." ) );
					}
				}
				else if ( param.find( cuT( "split_mesh" ) ) == 0 )
				{
					parameters.add( cuT( "split_mesh" ), true );
				}
				else if ( param.find( cuT( "rescale" ) ) == 0 )
				{
					if ( auto eqIndex = param.find( cuT( '=' ) );
						eqIndex != castor::String::npos )
					{
						float value;
						castor::string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "rescale" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -rescale=<float>." ) );
					}
				}
				else if ( param.find( cuT( "prefix" ) ) == 0 )
				{
					if ( auto eqIndex = param.find( cuT( '=' ) );
						eqIndex != castor::String::npos )
					{
						if ( castor::String value = param.substr( eqIndex + 1 );
							value.size() > 2 && value.front() == '\"' && value.back() == '\"' )
						{
							parameters.add( cuT( "prefix" ), value.substr( 1, value.size() - 2 ) );
						}
						else
						{
							CU_ParsingError( cuT( "Malformed parameter -prefix=\"name\"." ) );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -prefix=\"name\"." ) );
					}
				}
				else if ( param.find( cuT( "no_optimisations" ) ) == 0 )
				{
					parameters.add( cuT( "no_optimisations" ), true );
				}
				else if ( param.find( cuT( "invert_normals" ) ) == 0 )
				{
					parameters.add( cuT( "invert_normals" ), true );
				}
				else if ( param.find( cuT( "preferred_importer" ) ) == 0 )
				{
					if ( auto eqIndex = param.find( cuT( '=' ) );
						eqIndex != castor::String::npos )
					{
						if ( castor::String value = param.substr( eqIndex + 1 );
							value.size() > 2 && value.front() == '\"' && value.back() == '\"' )
						{
							parameters.add( cuT( "preferred_importer" ), value.substr( 1, value.size() - 2 ) );
						}
						else
						{
							CU_ParsingError( cuT( "Malformed parameter -preferred_importer=\"name\"." ) );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -preferred_importer=\"name\"." ) );
					}
				}
			}
		}

		template< typename ContextT >
		static CU_ImplementAttributeParserNewBlock( parserSamplerState, ContextT, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				newBlockContext->sampler = getEngine( *blockContext )->tryFindSampler( params[0]->get( name ) );

				if ( !newBlockContext->sampler )
				{
					newBlockContext->ownSampler = getEngine( *blockContext )->createSampler( name
						, *getEngine( *blockContext ) );
					newBlockContext->sampler = newBlockContext->ownSampler.get();
				}
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSampler )

		static CU_ImplementAttributeParserNewBlock( parserRootScene, RootContext, SceneContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				newBlockContext->root = blockContext;
				newBlockContext->scene = newBlockContext->root->engine->tryFindScene( params[0]->get( name ) );

				if ( !newBlockContext->scene )
				{
					newBlockContext->ownScene = castor::makeUnique< Scene >( name
						, *newBlockContext->root->engine );
					newBlockContext->scene = newBlockContext->ownScene.get();
				}

				newBlockContext->root->mapScenes.try_emplace( name, newBlockContext->scene );
				newBlockContext->overlays = castor::makeUnique< OverlayContext >();
				newBlockContext->overlays->root = blockContext;
				newBlockContext->overlays->scene = newBlockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eScene )

		static CU_ImplementAttributeParserNewBlock( parserRootLoadingScreen, RootContext, SceneContext )
		{
			newBlockContext->root = blockContext;
			newBlockContext->ownScene = castor::makeUnique< Scene >( LoadingScreen::SceneName
				, *newBlockContext->root->engine );
			newBlockContext->scene = newBlockContext->ownScene.get();
			newBlockContext->overlays = castor::makeUnique< OverlayContext >();
			newBlockContext->overlays->root = blockContext;
			newBlockContext->overlays->scene = newBlockContext;
		}
		CU_EndAttributePushNewBlock( CSCNSection::eScene )

		static CU_ImplementAttributeParserNewBlock( parserRootFont, RootContext, FontContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( newBlockContext->name );
				newBlockContext->root = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eFont )

		static CU_ImplementAttributeParserNewBlock( parserRootSdfFont, RootContext, FontContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( newBlockContext->name );
				newBlockContext->root = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSdfFont )

		static CU_ImplementAttributeParserBlock( parserRootPanelOverlay, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->engine->tryFindOverlay( params[0]->get( name ) );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *blockContext->engine
						, OverlayType::ePanel
						, nullptr
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::ePanelOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserRootBorderPanelOverlay, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->engine->tryFindOverlay( params[0]->get( name ) );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *blockContext->engine
						, OverlayType::eBorderPanel
						, nullptr
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eBorderPanelOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserRootTextOverlay, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->engine->tryFindOverlay( params[0]->get( name ) );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *blockContext->engine
						, OverlayType::eText
						, nullptr
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eTextOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserRootDebugOverlays, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->engine->getRenderLoop().showDebugOverlays( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRootDebugTargets, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->engine->enableDebugTargets( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserRootWindow, RootContext, WindowContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				newBlockContext->root = blockContext;
				params[0]->get( newBlockContext->window.name );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eWindow )

		static CU_ImplementAttributeParserBlock( parserRootMaxImageSize, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [count] parameter." ) );
			}
			else
			{
				blockContext->engine->setMaxImageSize( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRootDebugMaxImageSize, RootContext )
		{
	#if !defined( NDEBUG )
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [count] parameter." ) );
			}
			else
			{
				blockContext->engine->setMaxImageSize( params[0]->get< uint32_t >() );
			}
	#endif
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRootLpvGridSize, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [count] parameter." ) );
			}
			else
			{
				blockContext->engine->setLpvGridSize( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRootDefaultUnit, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [unit] parameter." ) );
			}
			else
			{
				blockContext->engine->setLengthUnit( castor::LengthUnit( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRootFullLoading, RootContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [enable] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->enableFullLoading );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserWindowRenderTarget, WindowContext, TargetContext )
		{
			newBlockContext->window = blockContext;
			newBlockContext->targetType = TargetType::eWindow;
			newBlockContext->size = { 1u, 1u };
			newBlockContext->srgbPixelFormat = castor::PixelFormat::eUNDEFINED;
			newBlockContext->hdrPixelFormat = castor::PixelFormat::eUNDEFINED;
		}
		CU_EndAttributePushNewBlock( CSCNSection::eRenderTarget )

		static CU_ImplementAttributeParserBlock( parserWindowVSync, WindowContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->window.enableVSync );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWindowFullscreen, WindowContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->window.fullscreen );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWindowAllowHdr, WindowContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->window.allowHdr );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserWindowEnd, WindowContext )
		{
			log::info << "Loaded window [" << blockContext->window.name
				<< ", HDR(" << blockContext->window.allowHdr << ")"
				<< ", VSYNC(" << blockContext->window.enableVSync << ")"
				<< ", FS(" << blockContext->window.fullscreen << ")]" << std::endl;
			blockContext->root->window = castor::move( blockContext->window );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserRenderTargetScene, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				ScenePtrStrMap::iterator it = getRootContext( *blockContext )->mapScenes.find( params[0]->get( name ) );

				if ( it != getRootContext( *blockContext )->mapScenes.end() )
				{
					blockContext->renderTarget->setScene( *it->second );
				}
				else
				{
					CU_ParsingError( cuT( "No scene found with name : [" ) + name + cuT( "]." ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetCamera, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->renderTarget->getScene() )
				{
					castor::String name;

					if ( auto camera = blockContext->renderTarget->getScene()->findCamera( params[0]->get( name ) ) )
					{
						blockContext->renderTarget->setCamera( *camera );
					}
					else
					{
						CU_ParsingError( cuT( "Camera [" ) + name + cuT( "] was not found." ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No scene initialised for this window, set scene before camera." ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetSize, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->size );

				if ( blockContext->srgbPixelFormat != castor::PixelFormat::eUNDEFINED
					&& blockContext->hdrPixelFormat != castor::PixelFormat::eUNDEFINED )
				{
					bool allowHdr = blockContext->window
						? blockContext->window->window.allowHdr
						: true;
					blockContext->renderTarget = getEngine( *blockContext )->getRenderTargetCache().add( blockContext->targetType
						, blockContext->size
						, allowHdr ? blockContext->hdrPixelFormat : blockContext->srgbPixelFormat );
					blockContext->renderTarget->enableFullLoading( getRootContext( *blockContext )->enableFullLoading );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetFormat, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->srgbPixelFormat );
				blockContext->hdrPixelFormat = blockContext->srgbPixelFormat;

				if ( blockContext->srgbPixelFormat < castor::PixelFormat::eD16_UNORM )
				{
					if ( blockContext->size != castor::Size{ 1u, 1u }
						&& blockContext->size != castor::Size{} )
					{
						bool allowHdr = blockContext->window
							? blockContext->window->window.allowHdr
							: true;
						blockContext->renderTarget = getEngine( *blockContext )->getRenderTargetCache().add( blockContext->targetType
							, blockContext->size
							, allowHdr ? blockContext->hdrPixelFormat : blockContext->srgbPixelFormat );
						blockContext->renderTarget->enableFullLoading( getRootContext( *blockContext )->enableFullLoading );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Wrong format for colour" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetSRGBFormat, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->srgbPixelFormat );

				if ( blockContext->srgbPixelFormat < castor::PixelFormat::eD16_UNORM )
				{
					if ( blockContext->size != castor::Size{ 1u, 1u }
						&& blockContext->size != castor::Size{}
						&& blockContext->hdrPixelFormat != castor::PixelFormat::eUNDEFINED )
					{
						bool allowHdr = blockContext->window
							? blockContext->window->window.allowHdr
							: true;
						blockContext->renderTarget = getEngine( *blockContext )->getRenderTargetCache().add( blockContext->targetType
							, blockContext->size
							, allowHdr ? blockContext->hdrPixelFormat : blockContext->srgbPixelFormat );
						blockContext->renderTarget->enableFullLoading( getRootContext( *blockContext )->enableFullLoading );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Wrong format for colour" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetHDRFormat, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->hdrPixelFormat );

				if ( blockContext->hdrPixelFormat < castor::PixelFormat::eD16_UNORM )
				{
					if ( blockContext->size != castor::Size{ 1u, 1u }
						&& blockContext->size != castor::Size{}
						&& blockContext->srgbPixelFormat != castor::PixelFormat::eUNDEFINED )
					{
						bool allowHdr = blockContext->window
							? blockContext->window->window.allowHdr
							: true;
						blockContext->renderTarget = getEngine( *blockContext )->getRenderTargetCache().add( blockContext->targetType
							, blockContext->size
							, allowHdr ? blockContext->hdrPixelFormat : blockContext->srgbPixelFormat );
						blockContext->renderTarget->enableFullLoading( getRootContext( *blockContext )->enableFullLoading );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Wrong format for colour" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetStereo, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float rIntraOcularDistance;
				params[0]->get( rIntraOcularDistance );

				if ( rIntraOcularDistance > 0 )
				{
					//! blockContext->renderTarget->setStereo( true );
					//! blockContext->renderTarget->setIntraOcularDistance( rIntraOcularDistance );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetPostEffect, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				Parameters parameters;

				if ( params.size() > 1 )
				{
					castor::String tmp;
					parameters.parse( params[1]->get( tmp ) );
				}

				castor::String name;
				auto effect = blockContext->renderTarget->getPostEffect( params[0]->get( name ) );

				if ( !effect )
				{
					CU_ParsingError( cuT( "PostEffect [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
				}
				else
				{
					effect->enable( true );
					effect->setParameters( castor::move( parameters ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetToneMapping, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->renderTarget->setToneMappingType( params[0]->get< castor::String >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetFullLoading, TargetContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [enable] parameter." ) );
			}
			else
			{
				blockContext->renderTarget->enableFullLoading( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserRenderTargetEnd, TargetContext )
		{
			if ( !blockContext->renderTarget )
			{
				CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
			}
			else
			{
				auto target = blockContext->renderTarget;
				log::info << "Loaded target [" << target->getName()
					<< ", FMT(" << castor::makeString( ashes::getName( VkFormat( target->getPixelFormat() ) ) ) << ")"
					<< ", DIM(" << target->getSize() << ")]" << std::endl;

				if ( blockContext->window )
				{
					blockContext->window->window.renderTarget = castor::move( blockContext->renderTarget );
				}
				else
				{
					blockContext->texture->renderTarget = castor::move( blockContext->renderTarget );
				}
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSamplerMinFilter, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setMinFilter( VkFilter( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerMagFilter, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setMagFilter( VkFilter( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerMipFilter, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setMipFilter( VkSamplerMipmapMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerMinLod, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				float rValue = -1000;
				params[0]->get( rValue );

				if ( rValue >= -1000 && rValue <= 1000 )
				{
					sampler->setMinLod( rValue );
				}
				else
				{
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + castor::string::toString( rValue ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerMaxLod, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				float rValue = 1000;
				params[0]->get( rValue );

				if ( rValue >= -1000 && rValue <= 1000 )
				{
					sampler->setMaxLod( rValue );
				}
				else
				{
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + castor::string::toString( rValue ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerLodBias, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				float rValue = 1000;
				params[0]->get( rValue );

				if ( rValue >= -1000 && rValue <= 1000 )
				{
					sampler->setLodBias( rValue );
				}
				else
				{
					CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + castor::string::toString( rValue ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerUWrapMode, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setWrapS( VkSamplerAddressMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerVWrapMode, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setWrapT( VkSamplerAddressMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerWWrapMode, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setWrapR( VkSamplerAddressMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerBorderColour, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setBorderColour( VkBorderColor( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerAnisotropicFiltering, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->enableAnisotropicFiltering( params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerMaxAnisotropy, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				float rValue = 1000;
				params[0]->get( rValue );
				sampler->setMaxAnisotropy( rValue );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerComparisonMode, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->enableCompare( bool( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerComparisonFunc, SamplerContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( auto sampler = blockContext->sampler )
			{
				sampler->setCompareOp( VkCompareOp( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSamplerEnd, SamplerContext )
		{
			auto sampler = blockContext->sampler;

			if ( !blockContext->ownSampler
				&& !sampler )
			{
				CU_ParsingError( cuT( "No sampler initialised." ) );
			}
			else
			{
				log::info << "Loaded sampler [" << blockContext->sampler->getName() << "]" << std::endl;

				if ( blockContext->ownSampler )
				{
					getEngine( *blockContext )->addSampler( blockContext->ownSampler->getName()
						, blockContext->ownSampler
						, true );
				}

				blockContext->sampler = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSceneBkColour, SceneContext )
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
				blockContext->scene->setBackgroundColour( params[0]->get< castor::RgbColour >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneBkImage, SceneContext )
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
				auto imgBackground = castor::makeUnique< ImageBackground >( *getEngine( *blockContext )
					, *blockContext->scene );
				imgBackground->setImage( context.file.getPath(), params[0]->get< castor::Path >() );
				blockContext->scene->setBackground( castor::ptrRefCast< SceneBackground >( imgBackground ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserSceneFont, SceneContext, FontContext )
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

		static CU_ImplementAttributeParserNewBlock( parserSceneSdfFont, SceneContext, FontContext )
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

		static CU_ImplementAttributeParserNewBlock( parserSceneCamera, SceneContext, CameraContext )
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
		CU_EndAttributePushNewBlock( CSCNSection::eCamera )

		static CU_ImplementAttributeParserNewBlock( parserSceneLight, SceneContext, LightContext )
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
		CU_EndAttributePushNewBlock( CSCNSection::eLight )

		static CU_ImplementAttributeParserNewBlock( parserSceneCameraNode, SceneContext, NodeContext )
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
				params[0]->get( newBlockContext->name );
				newBlockContext->scene = blockContext;
				newBlockContext->isCameraNode = true;
				newBlockContext->parentNode = blockContext->scene->getCameraRootNode();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eNode )

		static CU_ImplementAttributeParserNewBlock( parserSceneSceneNode, SceneContext, NodeContext )
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
				params[0]->get( newBlockContext->name );
				newBlockContext->scene = blockContext;
				newBlockContext->isCameraNode = false;
				newBlockContext->parentNode = blockContext->scene->getObjectRootNode();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eNode )

		static CU_ImplementAttributeParserNewBlock( parserSceneObject, SceneContext, ObjectContext )
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
				newBlockContext->ownGeometry = blockContext->scene->createGeometry( params[0]->get< castor::String >()
					, *blockContext->scene );
				newBlockContext->geometry = newBlockContext->ownGeometry.get();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eObject )

		static CU_ImplementAttributeParserBlock( parserSceneAmbientLight, SceneContext )
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
				blockContext->scene->setAmbientLight( params[0]->get< castor::RgbColour >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserSceneImport, SceneContext, SceneImportContext )
		{
			newBlockContext->scene = blockContext;
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSceneImport )

		static CU_ImplementAttributeParserNewBlock( parserSceneBillboard, SceneContext, BillboardsContext )
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
				newBlockContext->ownBillboards = castor::makeUnique< BillboardList >( params[0]->get< castor::String >()
					, *blockContext->scene );
				newBlockContext->billboards = newBlockContext->ownBillboards.get();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eBillboard )

		static CU_ImplementAttributeParserNewBlock( parserSceneAnimatedObjectGroup, SceneContext, AnimGroupContext )
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
				newBlockContext->animGroup = blockContext->scene->addNewAnimatedObjectGroup( params[0]->get< castor::String >()
					, *blockContext->scene );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eAnimGroup )

		static CU_ImplementAttributeParserBlock( parserScenePanelOverlay, SceneContext )
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
				castor::String name;
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->scene->tryFindOverlay( params[0]->get( name ) );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *getEngine( *blockContext )
						, OverlayType::ePanel
						, blockContext->scene
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::ePanelOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserSceneBorderPanelOverlay, SceneContext )
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
				castor::String name;
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->scene->tryFindOverlay( params[0]->get( name ) );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *getEngine( *blockContext )
						, OverlayType::eBorderPanel
						, blockContext->scene
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eBorderPanelOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserBlock( parserSceneTextOverlay, SceneContext )
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
				castor::String name;
				blockContext->overlays->parentOverlays.push_back( castor::move( blockContext->overlays->overlay ) );
				auto & parent = blockContext->overlays->parentOverlays.back();
				blockContext->overlays->overlay.rptr = blockContext->scene->tryFindOverlay( params[0]->get( name ) );

				if ( !blockContext->overlays->overlay.rptr )
				{
					blockContext->overlays->overlay.uptr = castor::makeUnique< Overlay >( *getEngine( *blockContext )
						, OverlayType::eText
						, blockContext->scene
						, parent.rptr );
					blockContext->overlays->overlay.rptr = blockContext->overlays->overlay.uptr.get();
					blockContext->overlays->overlay.rptr->rename( name );
				}

				blockContext->overlays->overlay.rptr->setVisible( false );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eTextOverlay, blockContext->overlays.get() )

		static CU_ImplementAttributeParserNewBlock( parserSceneSkybox, SceneContext, SkyboxContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else
			{
				newBlockContext->skybox = castor::makeUnique< SkyboxBackground >( *getEngine( *blockContext )
					, *blockContext->scene );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSkybox )

		static CU_ImplementAttributeParserBlock( parserSceneFogType, SceneContext )
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

		static CU_ImplementAttributeParserBlock( parserSceneFogDensity, SceneContext )
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

		static CU_ImplementAttributeParserNewBlock( parserSceneParticleSystem, SceneContext, ParticleSystemContext )
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

		static CU_ImplementAttributeParserBlock( parserSceneEnd, SceneContext )
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
					getEngine( *blockContext )->setLoadingScene( castor::move( blockContext->ownScene ) );
				}
				else
				{
					getEngine( *blockContext )->addScene( blockContext->scene->getName()
						, blockContext->ownScene
						, true );
				}
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSceneImportFile, SceneImportContext )
		{
			castor::Path path;
			castor::Path pathFile = context.file.getPath() / params[0]->get( path );
			blockContext->files.push_back( pathFile );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportAnimFile, SceneImportContext )
		{
			castor::Path path;
			castor::Path pathFile = context.file.getPath() / params[0]->get( path );
			blockContext->animFiles.push_back( pathFile );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportPrefix, SceneImportContext )
		{
			params[0]->get( blockContext->prefix );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportRescale, SceneImportContext )
		{
			params[0]->get( blockContext->rescale );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportPitch, SceneImportContext )
		{
			params[0]->get( blockContext->pitch );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportYaw, SceneImportContext )
		{
			params[0]->get( blockContext->yaw );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportRoll, SceneImportContext )
		{
			params[0]->get( blockContext->roll );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportNoOptimisations, SceneImportContext )
		{
			params[0]->get( blockContext->noOptimisations );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportEmissiveMult, SceneImportContext )
		{
			params[0]->get( blockContext->emissiveMult );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSceneImportTexRemap, SceneImportContext )
		{
			blockContext->textureRemaps.clear();
			blockContext->textureRemapIt = blockContext->textureRemaps.end();
		}
		CU_EndAttributePushBlock( CSCNSection::eTextureRemap, blockContext )

		static CU_ImplementAttributeParserBlock( parserSceneImportCenterCamera, SceneImportContext )
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

		static CU_ImplementAttributeParserBlock( parserSceneImportPreferredImporter, SceneImportContext )
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

		static CU_ImplementAttributeParserBlock( parserSceneImportIgnoreVertexColour, SceneImportContext )
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

		static CU_ImplementAttributeParserBlock( parserSceneImportEnd, SceneImportContext )
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

				SceneImporter importer{ *engine };

				for ( auto const & file : blockContext->files )
				{
					if ( !importer.import( *blockContext->scene->scene
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
					if ( !importer.importAnimations( *blockContext->scene->scene
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

		static CU_ImplementAttributeParserNewBlock( parserSceneMesh, SceneContext, MeshContext )
		{
			if ( blockContext->scene )
			{
				castor::String name;
				newBlockContext->scene = blockContext;
				newBlockContext->root = blockContext->root;
				newBlockContext->mesh = blockContext->scene->tryFindMesh( params[0]->get( name ) );

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
				castor::String name;
				newBlockContext->scene = blockContext;
				newBlockContext->skeleton = blockContext->scene->tryFindSkeleton( params[0]->get( name ) );

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

		static CU_ImplementAttributeParserBlock( parserParticleSystemParent, ParticleSystemContext )
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
				castor::String value;

				if ( auto node = blockContext->scene->scene->tryFindSceneNode( params[0]->get( value ) ) )
				{
					blockContext->parentNode = node;
				}
				else
				{
					CU_ParsingError( cuT( "No scene node named " ) + value );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserParticleSystemCount, ParticleSystemContext )
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
				params[0]->get( blockContext->particleCount );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserParticleSystemMaterial, ParticleSystemContext )
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
				castor::String name;

				if ( auto material = getEngine( *blockContext )->tryFindMaterial( params[0]->get( name ) ) )
				{
					blockContext->material = material;
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserParticleSystemDimensions, ParticleSystemContext )
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
				params[0]->get( blockContext->dimensions );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserParticleSystemParticle, ParticleSystemContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( blockContext->particleCount == 0 )
			{
				CU_ParsingError( cuT( "particles_count has not been specified." ) );
			}
			else if ( blockContext->dimensions[0] == 0 || blockContext->dimensions[1] == 0 )
			{
				CU_ParsingError( cuT( "one component of the particles dimensions is 0." ) );
			}
			else
			{
				if ( !blockContext->material )
				{
					blockContext->material = getEngine( *blockContext )->getMaterialCache().getDefaultMaterial();
				}

				blockContext->particleSystem = blockContext->scene->scene->tryFindParticleSystem( blockContext->name );

				if ( !blockContext->particleSystem )
				{
					auto node = blockContext->parentNode;

					if ( !node )
					{
						node = blockContext->scene->scene->getObjectRootNode();
					}

					blockContext->parentNode = nullptr;
					blockContext->ownParticleSystem = blockContext->scene->scene->createParticleSystem( blockContext->name
						, *blockContext->scene->scene
						, *node
						, blockContext->particleCount );
					blockContext->particleSystem = blockContext->ownParticleSystem.get();
				}

				blockContext->particleSystem->setMaterial( blockContext->material );
				blockContext->particleSystem->setDimensions( blockContext->dimensions );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eParticle, blockContext )

		static CU_ImplementAttributeParserNewBlock( parserParticleSystemCSShader, ParticleSystemContext, ProgramContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else
			{
				newBlockContext->shaderProgram = getEngine( *blockContext )->getShaderProgramCache().getNewProgram( blockContext->name, true );
				newBlockContext->particleSystem = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eShaderProgram )

		static CU_ImplementAttributeParserBlock( parserParticleSystemEnd, ParticleSystemContext )
		{
			if ( !blockContext->particleSystem )
			{
				CU_ParsingError( cuT( "No particle system initialised." ) );
			}
			else
			{
				blockContext->parentNode = nullptr;
				log::info << "Loaded sampler [" << blockContext->particleSystem->getName() << "]" << std::endl;

				if ( blockContext->ownParticleSystem )
				{
					blockContext->scene->scene->addParticleSystem( blockContext->particleSystem->getName()
						, blockContext->ownParticleSystem
						, true );
				}

				blockContext->particleSystem = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserParticleType, ParticleSystemContext )
		{
			if ( !blockContext->particleSystem )
			{
				CU_ParsingError( cuT( "No particle system initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String type;

				if ( Engine const * engine = getEngine( *blockContext );
					engine && !engine->getParticleFactory().isTypeRegistered( castor::string::lowerCase( params[0]->get( type ) ) ) )
				{
					CU_ParsingError( cuT( "Particle type [" ) + type + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
				}
				else
				{
					blockContext->particleSystem->setParticleType( type );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserParticleVariable, ParticleSystemContext )
		{
			if ( !blockContext->particleSystem )
			{
				CU_ParsingError( cuT( "No particle system initialised." ) );
			}
			else if ( params.size() < 2 )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String value;

				if ( params.size() > 2 )
				{
					params[2]->get( value );
				}

				blockContext->particleSystem->addParticleVariable( params[0]->get< castor::String >(), ParticleFormat( params[1]->get< uint32_t >() ), value );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightParent, LightContext )
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
				castor::String name;

				if ( auto parent = blockContext->scene->scene->findSceneNode( params[0]->get( name ) ) )
				{
					blockContext->parentNode = parent;

					if ( blockContext->light )
					{
						blockContext->light->detach();
						blockContext->parentNode->attachObject( *blockContext->light );
						blockContext->parentNode = nullptr;
					}
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightType, LightContext )
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
				blockContext->lightType = LightType( params[0]->get< uint32_t >() );
				blockContext->light = blockContext->scene->scene->tryFindLight( blockContext->name );

				if ( !blockContext->light )
				{
					auto node = blockContext->parentNode;

					if ( !node )
					{
						node = blockContext->scene->scene->getObjectRootNode();
					}

					blockContext->parentNode = nullptr;
					blockContext->ownLight = blockContext->scene->scene->createLight( blockContext->name
						, *blockContext->scene->scene
						, *node
						, blockContext->scene->scene->getLightsFactory()
						, blockContext->lightType );
					blockContext->light = blockContext->ownLight.get();
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightColour, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->light->setColour( params[0]->get< castor::Point3f >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightIntensity, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->light->setIntensity( params[0]->get< castor::Point2f >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightAttenuation, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::ePoint )
				{
					blockContext->light->getPointLight()->setAttenuation( params[0]->get< castor::Point3f >() );
				}
				else if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setAttenuation( params[0]->get< castor::Point3f >() );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightRange, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::ePoint )
				{
					blockContext->light->getPointLight()->setRange( params[0]->get< float >() );
				}
				else if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setRange( params[0]->get< float >() );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightCutOff, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto angle = params[0]->get< float >();

				if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setInnerCutOff( castor::Angle::fromDegrees( angle / 2.0f ) );
					blockContext->light->getSpotLight()->setOuterCutOff( castor::Angle::fromDegrees( angle ) );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightInnerCutOff, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setInnerCutOff( castor::Angle::fromDegrees( params[0]->get< float >() ) );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightOuterCutOff, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setOuterCutOff( castor::Angle::fromDegrees( params[0]->get< float >() ) );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightExponent, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->lightType == LightType::eSpot )
				{
					blockContext->light->getSpotLight()->setExponent( params[0]->get< float >() );
				}
				else
				{
					CU_ParsingError( cuT( "Wrong type of light to apply an exponent, needs spotlight" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserLightEnd, LightContext )
		{
			if ( !blockContext->light )
			{
				CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
			}
			else
			{
				log::info << "Loaded light [" << blockContext->light->getName() << "]" << std::endl;
				blockContext->parentNode = nullptr;

				if ( blockContext->ownLight )
				{
					blockContext->scene->scene->addLight( blockContext->light->getName()
						, blockContext->ownLight
						, true );
				}

				blockContext->light = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserNodeStatic, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [static] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->isStatic );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNodeParent, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [parent] parameter." ) );
			}
			else
			{
				auto name = params[0]->get< castor::String >();
				SceneNodeRPtr parent;

				if ( name == Scene::ObjectRootNode )
				{
					parent = blockContext->scene->scene->getObjectRootNode();
				}
				else if ( name == Scene::CameraRootNode )
				{
					parent = blockContext->scene->scene->getCameraRootNode();
				}
				else if ( name == Scene::RootNode )
				{
					parent = blockContext->scene->scene->getRootNode();
				}
				else
				{
					parent = blockContext->scene->scene->findSceneNode( name );
				}

				if ( parent )
				{
					blockContext->parentNode = parent;
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNodePosition, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [position] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->position );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNodeOrientation, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [orientation] parameter." ) );
			}
			else
			{
				blockContext->orientation = castor::Quaternion::fromAxisAngle( params[0]->get< castor::Point3f >()
					, castor::Angle::fromDegrees( params[1]->get< float >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNodeRotate, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [orientation] parameter." ) );
			}
			else
			{
				blockContext->orientation *= castor::Quaternion::fromAxisAngle( params[0]->get< castor::Point3f >()
					, castor::Angle::fromDegrees( params[1]->get< float >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNodeDirection, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [direction] parameter." ) );
			}
			else
			{
				castor::Point3f direction;
				params[0]->get( direction );
				castor::Point3f up{ 0, 1, 0 };
				castor::Point3f right{ castor::point::cross( direction, up ) };
				blockContext->orientation = castor::Quaternion::fromAxes( right, up, direction );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNodeScale, NodeContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing [direction] parameter." ) );
			}
			else
			{
				params[0]->get( blockContext->scale );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserNodeEnd, NodeContext )
		{
			auto sceneNode = blockContext->scene->scene->createSceneNode( blockContext->name
				, *blockContext->scene->scene
				, blockContext->parentNode
				, blockContext->position
				, blockContext->orientation
				, blockContext->scale
				, blockContext->isStatic );

			auto name = sceneNode->getName();
			auto node = blockContext->scene->scene->addSceneNode( name, sceneNode, true );
			sceneNode.reset();

			if ( !blockContext->isStatic )
			{
				for ( auto const & fileName : blockContext->scene->root->csnaFiles )
				{
					auto fName = fileName.getFileName();

					if ( auto pos = fName.find( name );
						pos == 0u && fName[name.size()] == '-' )
					{
						if ( auto animName = fName.substr( name.size() + 1u );
							!animName.empty() )
						{
							auto & animation = node->createAnimation( animName );
							BinaryParser< SceneNodeAnimation > parser;
							castor::BinaryFile animFile{ fileName, castor::File::OpenMode::eRead };
							parser.parse( animation, animFile );
						}
					}
				}
			}

			log::info << "Loaded scene node [" << name << "]" << std::endl;
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserObjectParent, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				auto name = params[0]->get< castor::String >();
				SceneNodeRPtr parent;

				if ( name == Scene::ObjectRootNode )
				{
					parent = blockContext->scene->scene->getObjectRootNode();
				}
				else if ( name == Scene::CameraRootNode )
				{
					parent = blockContext->scene->scene->getCameraRootNode();
				}
				else if ( name == Scene::RootNode )
				{
					parent = blockContext->scene->scene->getRootNode();
				}
				else
				{
					parent = blockContext->scene->scene->findSceneNode( name );
				}

				if ( parent )
				{
					parent->attachObject( *blockContext->geometry );
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserObjectMaterial, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->geometry->getMesh() )
				{
					castor::String name;
					auto material = getEngine( *blockContext )->tryFindMaterial( params[0]->get( name ) );

					if ( material )
					{
						for ( auto const & submesh : *blockContext->geometry->getMesh() )
						{
							blockContext->geometry->setMaterial( *submesh, material );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Geometry's mesh not initialised" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserObjectMesh, ObjectContext, MeshContext )
		{
			if ( blockContext->geometry )
			{
				castor::String name;
				auto scene = blockContext->geometry->getScene();
				newBlockContext->geometry = blockContext;
				newBlockContext->scene = blockContext->scene;
				newBlockContext->root = blockContext->scene->root;
				newBlockContext->mesh = scene->tryFindMesh( params[0]->get( name ) );

				if ( !newBlockContext->mesh )
				{
					newBlockContext->ownMesh = scene->createMesh( name, *scene );
					newBlockContext->mesh = newBlockContext->ownMesh.get();
				}
			}
			else
			{
				CU_ParsingError( cuT( "No scene initialised" ) );
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eMesh )

		static CU_ImplementAttributeParserBlock( parserObjectMaterials, ObjectContext )
		{
			// Only push the block
		}
		CU_EndAttributePushBlock( CSCNSection::eObjectMaterials, blockContext )

		static CU_ImplementAttributeParserBlock( parserObjectCastShadows, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->geometry->setShadowCaster( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserObjectReceivesShadows, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->geometry->setShadowReceiver( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserObjectCullable, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->geometry->setCullable( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserObjectEnd, ObjectContext )
		{
			blockContext->parentNode = nullptr;
			log::info << "Loaded geometry [" << blockContext->geometry->getName() << "]" << std::endl;

			if ( blockContext->ownGeometry )
			{
				blockContext->scene->scene->addGeometry( castor::move( blockContext->ownGeometry ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserObjectMaterialsMaterial, ObjectContext )
		{
			if ( !blockContext->geometry )
			{
				CU_ParsingError( cuT( "No Geometry initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->geometry->getMesh() )
			{
				if ( castor::String name;
					auto material = getEngine( *blockContext )->tryFindMaterial( params[1]->get( name ) ) )
				{
					uint16_t index;

					if ( blockContext->geometry->getMesh()->getSubmeshCount() > params[0]->get( index ) )
					{
						auto submesh = blockContext->geometry->getMesh()->getSubmesh( index );
						blockContext->geometry->setMaterial( *submesh, material );
					}
					else
					{
						CU_ParsingError( cuT( "Submesh index is too high" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Geometry's mesh not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserObjectMaterialsEnd, ObjectContext )
		{
			// Only push the block
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSkeletonImport, SkeletonContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( !blockContext->skeleton )
			{
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			}
			else
			{
				castor::Path path;
				castor::Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					castor::String meshParams;
					params[1]->get( meshParams );
					scnprs::fillMeshImportParameters( context, meshParams, parameters );
				}

				if ( !SkeletonImporter::import( *blockContext->skeleton
					, pathFile
					, parameters ) )
				{
					CU_ParsingError( cuT( "Skeleton Import failed" ) );
					blockContext->skeleton = nullptr;
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkeletonAnimImport, SkeletonContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( !blockContext->skeleton )
			{
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			}
			else
			{
				castor::Path path;
				castor::Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					castor::String meshParams;
					params[1]->get( meshParams );
					scnprs::fillMeshImportParameters( context, meshParams, parameters );
				}

				auto const & engine = *getEngine( *blockContext );
				auto extension = castor::string::lowerCase( path.getExtension() );

				if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
				{
					CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
				}
				else
				{
					castor::String preferredImporter = cuT( "any" );
					parameters.get( cuT( "preferred_importer" ), preferredImporter );
					auto file = engine.getImporterFileFactory().create( extension
						, preferredImporter
						, *blockContext->scene->scene
						, pathFile
						, parameters );

					if ( auto importer = file->createAnimationImporter() )
					{
						for ( auto animName : file->listSkeletonAnimations( *blockContext->skeleton ) )
						{
							auto animation = castor::makeUnique< SkeletonAnimation >( *blockContext->skeleton
								, animName );

							if ( !importer->import( *animation
								, file.get()
								, parameters ) )
							{
								CU_ParsingError( cuT( "Skeleton animation Import failed" ) );
							}
							else
							{
								blockContext->skeleton->addAnimation( castor::ptrRefCast< Animation >( animation ) );
							}
						}
					}
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkeletonSingleAnimImport, SkeletonContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No scene initialised." ) );
			}
			else if ( !blockContext->skeleton )
			{
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			}
			else
			{
				auto animRename = params[0]->get< castor::String >();
				auto path = params[1]->get< castor::Path >();
				castor::Path pathFile = context.file.getPath() / path;
				Parameters parameters;

				if ( params.size() > 2 )
				{
					castor::String meshParams;
					params[2]->get( meshParams );
					scnprs::fillMeshImportParameters( context, meshParams, parameters );
				}

				auto const & engine = *getEngine( *blockContext );
				auto extension = castor::string::lowerCase( path.getExtension() );

				if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
				{
					CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
				}
				else
				{
					castor::String preferredImporter = cuT( "any" );
					parameters.get( cuT( "preferred_importer" ), preferredImporter );
					auto file = engine.getImporterFileFactory().create( extension
						, preferredImporter
						, *blockContext->scene->scene
						, pathFile
						, parameters );

					if ( auto importer = file->createAnimationImporter() )
					{
						auto animations = file->listSkeletonAnimations( *blockContext->skeleton );

						if ( animations.empty() )
						{
							CU_ParsingError( cuT( "File [" ) + path + cuT( "] contains no skeleton animation for [" ) + blockContext->skeleton->getName() + cuT( "]." ) );
						}
						else if ( animations.size() > 1 )
						{
							CU_ParsingError( cuT( "File [" ) + path + cuT( "] contains too many skeleton animations, use `import_anim`." ) );
						}
						else
						{
							auto animation = castor::makeUnique< SkeletonAnimation >( *blockContext->skeleton
								, *animations.begin() );

							if ( !importer->import( *animation
								, file.get()
								, parameters ) )
							{
								CU_ParsingError( cuT( "Skeleton animation Import failed" ) );
							}
							else
							{
								animation->rename( animRename );
								blockContext->skeleton->addAnimation( castor::ptrRefCast< Animation >( animation ) );
							}
						}
					}
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkeletonEnd, SkeletonContext )
		{
			if ( !blockContext->scene )
			{
				CU_ParsingError( cuT( "No Scene initialised." ) );
			}
			else if ( !blockContext->skeleton )
			{
				CU_ParsingError( cuT( "No Skeleton initialised." ) );
			}
			else
			{
				log::info << "Loaded skeleton [" << blockContext->skeleton->getName() << "]" << std::endl;
				blockContext->skeleton = nullptr;
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserMeshType, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				Parameters parameters;

				if ( params.size() > 1 )
				{
					castor::String tmp;
					parameters.parse( params[1]->get( tmp ) );
				}

				auto const & factory = getEngine( *blockContext )->getMeshFactory();
				factory.create( params[0]->get< castor::String >() )->generate( *blockContext->mesh, parameters );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserNewBlock( parserMeshSubmesh, MeshContext, SubmeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				newBlockContext->mesh = blockContext;
				newBlockContext->submesh = blockContext->mesh->createSubmesh();
				newBlockContext->submesh->createComponent< DefaultRenderComponent >();
			}
		}
		CU_EndAttributePushNewBlock( CSCNSection::eSubmesh )

		static CU_ImplementAttributeParserBlock( parserMeshImport, MeshContext )
		{
			if ( auto mesh = blockContext->mesh )
			{
				castor::Path path;
				castor::Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					scnprs::fillMeshImportParameters( context, params[1]->get< castor::String >(), parameters );
				}

				if ( !MeshImporter::import( *mesh
					, pathFile
					, parameters
					, true ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
					blockContext->mesh = {};
				}
			}
			else
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshAnimImport, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				castor::Path path;
				castor::Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					scnprs::fillMeshImportParameters( context, params[1]->get< castor::String >(), parameters );
				}

				auto animation = castor::makeUnique< MeshAnimation >( *blockContext->mesh
					, pathFile.getFileName() );

				if ( !AnimationImporter::import( *animation
					, pathFile
					, parameters ) )
				{
					CU_ParsingError( cuT( "Mesh animation Import failed" ) );
				}
				else
				{
					blockContext->mesh->addAnimation( castor::ptrRefCast< Animation >( animation ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshSingleAnimImport, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				auto animName = params[0]->get< castor::String >();
				auto path = params[1]->get< castor::Path >();
				castor::Path pathFile = context.file.getPath() / path;
				Parameters parameters;

				if ( params.size() > 2 )
				{
					scnprs::fillMeshImportParameters( context, params[2]->get< castor::String >(), parameters );
				}

				auto animation = castor::makeUnique< MeshAnimation >( *blockContext->mesh
					, animName );

				if ( !AnimationImporter::import( *animation
					, pathFile
					, parameters ) )
				{
					CU_ParsingError( cuT( "Mesh animation Import failed" ) );
				}
				else
				{
					blockContext->mesh->addAnimation( castor::ptrRefCast< Animation >( animation ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshMorphTargetImport, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No mesh initialised." ) );
			}
			else
			{
				castor::Path path;
				castor::Path pathFile = context.file.getPath() / params[0]->get( path );
				Parameters parameters;

				if ( params.size() > 1 )
				{
					scnprs::fillMeshImportParameters( context, params[1]->get< castor::String >(), parameters );
				}

				Mesh mesh{ cuT( "MorphImport" ), *blockContext->mesh->getScene() };

				if ( !MeshImporter::import( mesh
					, pathFile
					, parameters
					, false ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
				}
				else if ( mesh.getSubmeshCount() == blockContext->mesh->getSubmeshCount() )
				{
					for ( auto & morphSubmesh : mesh )
					{
						auto id = morphSubmesh->getId();
						auto submesh = blockContext->mesh->getSubmesh( id );
						auto component = submesh->hasComponent( MorphComponent::TypeName )
							? submesh->getComponent< MorphComponent >()
							: submesh->createComponent< MorphComponent >();
						castor3d::SubmeshAnimationBuffer buffer;
					
						if ( morphSubmesh->hasComponent( PositionsComponent::TypeName ) )
						{
							buffer.positions = morphSubmesh->getPositions();
							uint32_t index = 0u;

							for ( auto & position : buffer.positions )
							{
								position -= submesh->getPositions()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( NormalsComponent::TypeName ) )
						{
							buffer.normals = morphSubmesh->getNormals();
							uint32_t index = 0u;

							for ( auto & normal : buffer.normals )
							{
								normal -= submesh->getNormals()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( TangentsComponent::TypeName ) )
						{
							buffer.tangents = morphSubmesh->getTangents();
							uint32_t index = 0u;

							for ( auto & tangent : buffer.tangents )
							{
								tangent -= submesh->getTangents()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( BitangentsComponent::TypeName ) )
						{
							buffer.bitangents = morphSubmesh->getBitangents();
							uint32_t index = 0u;

							for ( auto & bitangent : buffer.bitangents )
							{
								bitangent -= submesh->getBitangents()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( Texcoords0Component::TypeName ) )
						{
							buffer.texcoords0 = morphSubmesh->getTexcoords0();
							uint32_t index = 0u;

							for ( auto & texcoord : buffer.texcoords0 )
							{
								texcoord -= submesh->getTexcoords0()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( Texcoords1Component::TypeName ) )
						{
							buffer.texcoords1 = morphSubmesh->getTexcoords1();
							uint32_t index = 0u;

							for ( auto & texcoord : buffer.texcoords1 )
							{
								texcoord -= submesh->getTexcoords1()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( Texcoords2Component::TypeName ) )
						{
							buffer.texcoords2 = morphSubmesh->getTexcoords2();
							uint32_t index = 0u;

							for ( auto & texcoord : buffer.texcoords2 )
							{
								texcoord -= submesh->getTexcoords2()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( Texcoords3Component::TypeName ) )
						{
							buffer.texcoords3 = morphSubmesh->getTexcoords3();
							uint32_t index = 0u;

							for ( auto & texcoord : buffer.texcoords3 )
							{
								texcoord -= submesh->getTexcoords3()[index++];
							}
						}

						if ( morphSubmesh->hasComponent( ColoursComponent::TypeName ) )
						{
							buffer.colours = morphSubmesh->getColours();
							uint32_t index = 0u;

							for ( auto & colour : buffer.colours )
							{
								colour -= submesh->getColours()[index++];
							}
						}

						component->getData().addMorphTarget( castor::move( buffer ) );
					}

					mesh.cleanup();
				}
				else
				{
					CU_ParsingError( cuT( "The new mesh doesn't match the original mesh" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshDefaultMaterial, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;

				if ( auto material = getEngine( *blockContext )->findMaterial( params[0]->get( name ) ) )
				{
					for ( auto const & submesh : *blockContext->mesh )
					{
						submesh->setDefaultMaterial( material );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshDefaultMaterials, MeshContext )
		{
			// Only push the block
		}
		CU_EndAttributePushBlock( CSCNSection::eMeshDefaultMaterials, blockContext )

		static CU_ImplementAttributeParserBlock( parserMeshSkeleton, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				castor::String name;

				if ( auto skeleton = blockContext->mesh->getScene()->findSkeleton( params[0]->get( name ) ) )
				{
					blockContext->mesh->setSkeleton( skeleton );
				}
				else
				{
					CU_ParsingError( cuT( "Skeleton [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshMorphAnimation, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else
			{
				blockContext->morphAnimation = castor::makeUnique< MeshAnimation >( *blockContext->mesh, params[0]->get< castor::String >() );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eMorphAnimation, blockContext )

		static CU_ImplementAttributeParserBlock( parserMeshEnd, MeshContext )
		{
			if ( auto mesh = blockContext->mesh )
			{
				blockContext->mesh = {};

				if ( blockContext->ownMesh )
				{
					mesh->getScene()->addMesh( mesh->getName()
						, blockContext->ownMesh
						, true );
				}

				if ( blockContext->geometry )
				{
					blockContext->geometry->geometry->setMesh( mesh );
				}

				for ( auto const & submesh : *mesh )
				{
					if ( !submesh->hasRenderComponent() )
					{
						submesh->createComponent< DefaultRenderComponent >();
					}

					mesh->getScene()->getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserMeshMorphTargetWeight, MeshContext )
		{
			if ( !blockContext->morphAnimation )
			{
				CU_ParsingError( cuT( "No Morph Animation initialised." ) );
			}
			else if ( params.size() < 3u )
			{
				CU_ParsingError( cuT( "Invalid parameters." ) );
			}
			else if ( auto mesh = blockContext->mesh )
			{
				float timeIndex{};
				params[0]->get( timeIndex );
				uint32_t targetIndex{};
				params[1]->get( targetIndex );
				float targetWeight{};
				params[2]->get( targetWeight );
				auto & animation = *blockContext->morphAnimation;

				for ( auto const & submesh : *mesh )
				{
					MeshAnimationSubmesh animSubmesh{ animation, *submesh };
					animation.addChild( castor::move( animSubmesh ) );
					auto time = castor::Milliseconds{ uint64_t( timeIndex * 1000 ) };
					auto kfit = animation.find( time );
					castor3d::MeshMorphTarget * kf{};

					if ( kfit == animation.end() )
					{
						auto keyFrame = castor::makeUnique< MeshMorphTarget >( animation, time );
						kf = keyFrame.get();
						animation.addKeyFrame( castor::ptrRefCast< AnimationKeyFrame >( keyFrame ) );
					}
					else
					{
						kf = &static_cast< MeshMorphTarget & >( **kfit );
					}

					kf->setTargetWeight( *submesh, targetIndex, targetWeight );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshMorphAnimationEnd, MeshContext )
		{
			if ( !blockContext->morphAnimation )
			{
				CU_ParsingError( cuT( "No Morph Animation initialised." ) );
			}
			else if ( auto mesh = blockContext->mesh )
			{
				log::info << "Loaded morp animation [" << blockContext->morphAnimation->getName() << "]" << std::endl;
				mesh->addAnimation( castor::ptrRefCast< Animation >( blockContext->morphAnimation ) );
			}
			else
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserMeshDefaultMaterialsMaterial, MeshContext )
		{
			if ( !blockContext->mesh )
			{
				CU_ParsingError( cuT( "No Mesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String name;
				uint16_t index;

				if (  auto material = getEngine( *blockContext )->findMaterial( params[1]->get( name ) ))
				{
					if ( blockContext->mesh->getSubmeshCount() > params[0]->get( index ) )
					{
						auto submesh = blockContext->mesh->getSubmesh( index );
						submesh->setDefaultMaterial( material );
					}
					else
					{
						CU_ParsingError( cuT( "Submesh index is too high" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserMeshDefaultMaterialsEnd, MeshContext )
		{
			// Only push the block
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSubmeshVertex, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point3f value;
				params[0]->get( value );
				blockContext->vertexPos.push_back( value[0] );
				blockContext->vertexPos.push_back( value[1] );
				blockContext->vertexPos.push_back( value[2] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshUV, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point2f value;
				params[0]->get( value );
				blockContext->vertexTex.push_back( value[0] );
				blockContext->vertexTex.push_back( value[1] );
				blockContext->vertexTex.push_back( 0.0 );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshUVW, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point3f value;
				params[0]->get( value );
				blockContext->vertexTex.push_back( value[0] );
				blockContext->vertexTex.push_back( value[1] );
				blockContext->vertexTex.push_back( value[2] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshNormal, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point3f value;
				params[0]->get( value );
				blockContext->vertexNml.push_back( value[0] );
				blockContext->vertexNml.push_back( value[1] );
				blockContext->vertexNml.push_back( value[2] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshTangent, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::Point4f value;
				params[0]->get( value );
				blockContext->vertexTan.push_back( value[0] );
				blockContext->vertexTan.push_back( value[1] );
				blockContext->vertexTan.push_back( value[2] );
				blockContext->vertexTan.push_back( value[3] );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshFace, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				castor::String strParams;
				params[0]->get( strParams );
				castor::Point3i pt3Indices;
				auto arrayValues = castor::string::split( strParams, cuT( " " ) );
				blockContext->face1 = -1;
				blockContext->face2 = -1;

				if ( arrayValues.size() >= 4 )
				{
					castor::Point4i pt4Indices;

					if ( castor::parseValues( *blockContext->mesh->root->logger, strParams, pt4Indices ) )
					{
						blockContext->face1 = int( blockContext->faces.size() );
						blockContext->faces.push_back( uint32_t( pt4Indices[0] ) );
						blockContext->faces.push_back( uint32_t( pt4Indices[1] ) );
						blockContext->faces.push_back( uint32_t( pt4Indices[2] ) );
						blockContext->face2 = int( blockContext->faces.size() );
						blockContext->faces.push_back( uint32_t( pt4Indices[0] ) );
						blockContext->faces.push_back( uint32_t( pt4Indices[2] ) );
						blockContext->faces.push_back( uint32_t( pt4Indices[3] ) );
					}
				}
				else if ( castor::parseValues( *blockContext->mesh->root->logger, strParams, pt3Indices ) )
				{
					blockContext->face1 = int( blockContext->faces.size() );
					blockContext->faces.push_back( uint32_t( pt3Indices[0] ) );
					blockContext->faces.push_back( uint32_t( pt3Indices[1] ) );
					blockContext->faces.push_back( uint32_t( pt3Indices[2] ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshFaceUV, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->vertexTex.empty() )
				{
					blockContext->vertexTex.resize( blockContext->vertexPos.size() );
				}

				auto arrayValues = castor::string::split( params[0]->get< castor::String >(), cuT( " " ), 20 );

				if ( arrayValues.size() >= 6 && blockContext->face1 != -1 )
				{
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[2] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[3] );
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[4] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[5] );
				}

				if ( arrayValues.size() >= 8 && blockContext->face2 != -1 )
				{
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[4] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[5] );
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[6] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[7] );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshFaceUVW, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->vertexTex.empty() )
				{
					blockContext->vertexTex.resize( blockContext->vertexPos.size() );
				}

				auto arrayValues = castor::string::split( params[0]->get< castor::String >(), cuT( " " ), 20 );

				if ( arrayValues.size() >= 9 && blockContext->face1 != -1 )
				{
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
					blockContext->vertexTex[2 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[2] );
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[3] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[4] );
					blockContext->vertexTex[2 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[5] );
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[6] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[7] );
					blockContext->vertexTex[2 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[8] );
				}

				if ( arrayValues.size() >= 12 && blockContext->face2 != -1 )
				{
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 0] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 1] );
					blockContext->vertexTex[2 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 2] );
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 6] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 7] );
					blockContext->vertexTex[2 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 8] );
					blockContext->vertexTex[0 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 9] );
					blockContext->vertexTex[1 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[10] );
					blockContext->vertexTex[2 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[11] );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshFaceNormals, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->vertexNml.empty() )
				{
					blockContext->vertexNml.resize( blockContext->vertexPos.size() );
				}

				auto arrayValues = castor::string::split( params[0]->get< castor::String >(), cuT( " " ), 20 );

				if ( arrayValues.size() >= 9 && blockContext->face1 != -1 )
				{
					blockContext->vertexNml[0 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
					blockContext->vertexNml[1 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
					blockContext->vertexNml[2 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[2] );
					blockContext->vertexNml[0 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[3] );
					blockContext->vertexNml[1 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[4] );
					blockContext->vertexNml[2 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[5] );
					blockContext->vertexNml[0 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[6] );
					blockContext->vertexNml[1 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[7] );
					blockContext->vertexNml[2 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[8] );
				}

				if ( arrayValues.size() >= 12 && blockContext->face2 != -1 )
				{
					blockContext->vertexNml[0 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 0] );
					blockContext->vertexNml[1 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 1] );
					blockContext->vertexNml[2 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 2] );
					blockContext->vertexNml[0 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 6] );
					blockContext->vertexNml[1 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 7] );
					blockContext->vertexNml[2 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 8] );
					blockContext->vertexNml[0 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 9] );
					blockContext->vertexNml[1 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[10] );
					blockContext->vertexNml[2 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[11] );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshFaceTangents, SubmeshContext )
		{
			if ( !blockContext->submesh )
			{
				CU_ParsingError( cuT( "No Submesh initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->vertexTan.empty() )
				{
					blockContext->vertexTan.resize( blockContext->vertexPos.size() );
				}

				auto arrayValues = castor::string::split( params[0]->get< castor::String >(), cuT( " " ), 20 );

				if ( arrayValues.size() >= 12 && blockContext->face1 != -1 )
				{
					blockContext->vertexTan[0 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 0] );
					blockContext->vertexTan[1 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 1] );
					blockContext->vertexTan[2 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 2] );
					blockContext->vertexTan[3 + blockContext->faces[size_t( blockContext->face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 3] );
					blockContext->vertexTan[0 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 4] );
					blockContext->vertexTan[1 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 5] );
					blockContext->vertexTan[2 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 6] );
					blockContext->vertexTan[3 + blockContext->faces[size_t( blockContext->face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 7] );
					blockContext->vertexTan[0 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 8] );
					blockContext->vertexTan[1 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 9] );
					blockContext->vertexTan[2 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[10] );
					blockContext->vertexTan[3 + blockContext->faces[size_t( blockContext->face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[11] );
				}

				if ( arrayValues.size() >= 16 && blockContext->face2 != -1 )
				{
					blockContext->vertexTan[0 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 0] );
					blockContext->vertexTan[1 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 1] );
					blockContext->vertexTan[2 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 2] );
					blockContext->vertexTan[3 + blockContext->faces[size_t( blockContext->face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 3] );
					blockContext->vertexTan[0 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 8] );
					blockContext->vertexTan[1 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 9] );
					blockContext->vertexTan[2 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[10] );
					blockContext->vertexTan[3 + blockContext->faces[size_t( blockContext->face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[11] );
					blockContext->vertexTan[0 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[12] );
					blockContext->vertexTan[1 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[13] );
					blockContext->vertexTan[2 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[14] );
					blockContext->vertexTan[3 + blockContext->faces[size_t( blockContext->face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[15] );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSubmeshEnd, SubmeshContext )
		{
			if ( !blockContext->vertexPos.empty() )
			{
				if ( !blockContext->submesh->hasComponent( PositionsComponent::TypeName ) )
				{
					blockContext->submesh->createComponent< PositionsComponent >();
				}

				if ( !blockContext->vertexNml.empty()
					&& !blockContext->submesh->hasComponent( NormalsComponent::TypeName ) )
				{
					blockContext->submesh->createComponent< NormalsComponent >();
				}

				if ( !blockContext->vertexTan.empty()
					&& !blockContext->submesh->hasComponent( TangentsComponent::TypeName ) )
				{
					blockContext->submesh->createComponent< TangentsComponent >();
				}

				if ( !blockContext->vertexTex.empty()
					&& !blockContext->submesh->hasComponent( Texcoords0Component::TypeName ) )
				{
					blockContext->submesh->createComponent< Texcoords0Component >();
				}

				castor::Vector< InterleavedVertex > vertices{ blockContext->vertexPos.size() / 3 };
				uint32_t index{ 0u };

				for ( auto & vertex : vertices )
				{
					std::memcpy( vertex.pos.ptr(), &blockContext->vertexPos[index], sizeof( vertex.pos ) );

					if ( !blockContext->vertexNml.empty() )
					{
						std::memcpy( vertex.nml.ptr(), &blockContext->vertexNml[index], sizeof( vertex.nml ) );
					}

					if ( !blockContext->vertexTan.empty() )
					{
						std::memcpy( vertex.tan.ptr(), &blockContext->vertexTan[index], sizeof( vertex.tan ) );
					}

					if ( !blockContext->vertexTex.empty() )
					{
						std::memcpy( vertex.tex.ptr(), &blockContext->vertexTex[index], sizeof( vertex.tex ) );
					}

					index += 3;
				}

				blockContext->submesh->addPoints( vertices );

				if ( !blockContext->faces.empty() )
				{
					auto indices = reinterpret_cast< FaceIndices * >( &blockContext->faces[0] );
					auto mapping = blockContext->submesh->createComponent< TriFaceMapping >();
					mapping->getData().addFaceGroup( indices, indices + ( blockContext->faces.size() / 3 ) );

					if ( !blockContext->vertexNml.empty() )
					{
						if ( !blockContext->vertexTex.empty()
							&& blockContext->vertexTan.empty() )
						{
							mapping->computeTangents();
						}
					}
					else
					{
						mapping->computeNormals();
						mapping->computeTangents();
					}
				}

				MeshPreparer::prepare( *blockContext->submesh
					, Parameters{} );
				blockContext->vertexPos.clear();
				blockContext->vertexNml.clear();
				blockContext->vertexTan.clear();
				blockContext->vertexTex.clear();
				blockContext->faces.clear();
				blockContext->submesh->getParent().getScene()->getListener().postEvent( makeGpuInitialiseEvent( *blockContext->submesh ) );
				blockContext->submesh = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserComputeShader, ProgramContext )
		{
			blockContext->shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
		}
		CU_EndAttributePushBlock( CSCNSection::eShaderStage, blockContext )

		static CU_ImplementAttributeParserBlock( parserShaderProgramEnd, ProgramContext )
		{
			if ( !blockContext->shaderProgram )
			{
				CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
			}
			else
			{
				if ( blockContext->particleSystem )
				{
					blockContext->particleSystem->particleSystem->setCSUpdateProgram( blockContext->shaderProgram );
				}

				blockContext->shaderProgram = {};
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserShaderFile, ProgramContext )
		{
			if ( !blockContext->shaderProgram )
			{
				CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->shaderStage != VkShaderStageFlagBits( 0u ) )
				{
					blockContext->shaderProgram->setFile( blockContext->shaderStage
						, context.file.getPath() / params[0]->get< castor::Path >() );
				}
				else
				{
					CU_ParsingError( cuT( "Shader not initialised" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserShaderGroupSizes, ProgramContext )
		{
			if ( !blockContext->shaderProgram )
			{
				CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				if ( blockContext->particleSystem
					&& blockContext->shaderStage != VkShaderStageFlagBits( 0u ) )
				{
					blockContext->particleSystem->particleSystem->setCSGroupSizes( params[0]->get< castor::Point3i >() );
				}
				else
				{
					CU_ParsingError( cuT( "Shader not initialised" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFontFile, FontContext )
		{
			params[0]->get( blockContext->path );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFontHeight, FontContext )
		{
			params[0]->get( blockContext->height );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserFontEnd, FontContext )
		{
			if ( !blockContext->name.empty() && !blockContext->path.empty() )
			{
				if ( blockContext->scene )
				{
					blockContext->scene->scene->getFontView().add( blockContext->name
						, uint32_t( blockContext->height )
						, context.file.getPath() / blockContext->path );
				}
				else
				{
					getEngine( *blockContext )->addNewFont( blockContext->name
						, uint32_t( blockContext->height )
						, context.file.getPath() / blockContext->path );
				}

				log::info << "Loaded font [" << blockContext->name << "]" << std::endl;
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSdfFontFile, FontContext )
		{
			params[0]->get( blockContext->path );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSdfFontEnd, FontContext )
		{
			if ( !blockContext->name.empty() && !blockContext->path.empty() )
			{
				if ( blockContext->scene )
				{
					blockContext->scene->scene->getFontView().add( blockContext->name
						, context.file.getPath() / blockContext->path );
				}
				else
				{
					getEngine( *blockContext )->addNewSdfFont( blockContext->name
						, context.file.getPath() / blockContext->path );
				}

				log::info << "Loaded font [" << blockContext->name << "]" << std::endl;
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserOverlayPosition, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setRelativePosition( params[0]->get< castor::Point2d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserOverlaySize, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setRelativeSize( params[0]->get< castor::Point2d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserOverlayPixelSize, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setPixelSize( params[0]->get< castor::Size >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserOverlayPixelPosition, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setPixelPosition( params[0]->get< castor::Position >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserOverlayMaterial, OverlayContext )
		{
			if ( blockContext->overlay.rptr )
			{
				blockContext->overlay.rptr->setMaterial( getEngine( *blockContext )->findMaterial( params[0]->get< castor::String >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserOverlayPanelOverlay, OverlayContext )
		{
			auto engine = getEngine( *blockContext );
			castor::String name;
			blockContext->parentOverlays.push_back( castor::move( blockContext->overlay ) );
			auto & parent = blockContext->parentOverlays.back();
			blockContext->overlay.rptr = blockContext->scene
				? blockContext->scene->scene->tryFindOverlay( params[0]->get( name ) )
				: engine->tryFindOverlay( params[0]->get( name ) );

			if ( !blockContext->overlay.rptr )
			{
				blockContext->overlay.uptr = castor::makeUnique< Overlay >( *engine
					, OverlayType::ePanel
					, scnprs::getScene( context, blockContext, parent )
					, parent.rptr );
				blockContext->overlay.rptr = blockContext->overlay.uptr.get();
				blockContext->overlay.rptr->rename( name );
			}

			blockContext->overlay.rptr->setVisible( false );
		}
		CU_EndAttributePushBlock( CSCNSection::ePanelOverlay, blockContext )

		static CU_ImplementAttributeParserBlock( parserOverlayBorderPanelOverlay, OverlayContext )
		{
			auto engine = getEngine( *blockContext );
			castor::String name;
			blockContext->parentOverlays.push_back( castor::move( blockContext->overlay ) );
			auto & parent = blockContext->parentOverlays.back();
			blockContext->overlay.rptr = blockContext->scene
				? blockContext->scene->scene->tryFindOverlay( params[0]->get( name ) )
				: engine->tryFindOverlay( params[0]->get( name ) );

			if ( !blockContext->overlay.rptr )
			{
				blockContext->overlay.uptr = castor::makeUnique< Overlay >( *engine
					, OverlayType::eBorderPanel
					, scnprs::getScene( context, blockContext, parent )
					, parent.rptr );
				blockContext->overlay.rptr = blockContext->overlay.uptr.get();
				blockContext->overlay.rptr->rename( name );
			}

			blockContext->overlay.rptr->setVisible( false );
		}
		CU_EndAttributePushBlock( CSCNSection::eBorderPanelOverlay, blockContext )

		static CU_ImplementAttributeParserBlock( parserOverlayTextOverlay, OverlayContext )
		{
			auto engine = getEngine( *blockContext );
			castor::String name;
			blockContext->parentOverlays.push_back( castor::move( blockContext->overlay ) );
			auto & parent = blockContext->parentOverlays.back();
			blockContext->overlay.rptr = blockContext->scene
				? blockContext->scene->scene->tryFindOverlay( params[0]->get( name ) )
				: engine->tryFindOverlay( params[0]->get( name ) );

			if ( !blockContext->overlay.rptr )
			{
				blockContext->overlay.uptr = castor::makeUnique< Overlay >( *engine
					, OverlayType::eText
					, scnprs::getScene( context, blockContext, parent )
					, parent.rptr );
				blockContext->overlay.rptr = blockContext->overlay.uptr.get();
				blockContext->overlay.rptr->rename( name );
			}

			blockContext->overlay.rptr->setVisible( false );
		}
		CU_EndAttributePushBlock( CSCNSection::eTextOverlay, blockContext )

		static CU_ImplementAttributeParserBlock( parserOverlayEnd, OverlayContext )
		{
			log::info << "Loaded overlay [" << blockContext->overlay.rptr->getName() << "]" << std::endl;

			if ( blockContext->overlay.rptr->getType() == OverlayType::eText )
			{
				auto textOverlay = blockContext->overlay.rptr->getTextOverlay();

				if ( auto fontTexture = textOverlay->getFontTexture() )
				{
					if ( fontTexture->getFont()->isSDF()
						&& textOverlay->getSDFHeight() == uint32_t{} )
					{
						blockContext->overlay.rptr->setVisible( false );
						CU_ParsingError( cuT( "TextOverlay's font is an SDF font, and the overlay text_height has not been set, the overlay will not be rendered" ) );
					}
					else
					{
						blockContext->overlay.rptr->setVisible( true );
					}
				}
				else
				{
					blockContext->overlay.rptr->setVisible( false );
					CU_ParsingError( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
				}
			}
			else
			{
				blockContext->overlay.rptr->setVisible( true );
			}

			if ( blockContext->overlay.uptr )
			{
				if ( blockContext->scene )
				{
					blockContext->scene->scene->addOverlay( blockContext->overlay.rptr->getName()
						, blockContext->overlay.uptr
						, true );
				}
				else
				{
					getEngine( *blockContext )->addOverlay( blockContext->overlay.rptr->getName()
						, blockContext->overlay.uptr
						, true );
				}
			}

			CU_Require( !blockContext->parentOverlays.empty() );
			blockContext->overlay = castor::move( blockContext->parentOverlays.back() );
			blockContext->parentOverlays.pop_back();
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserPanelOverlayUvs, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::ePanel )
			{
				overlay->getPanelOverlay()->setUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlaySizes, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setRelativeBorderSize( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayPixelSizes, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setPixelBorderSize( params[0]->get< castor::Point4ui >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayMaterial, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setBorderMaterial( getEngine( *blockContext )->findMaterial( params[0]->get< castor::String >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayPosition, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setBorderPosition( BorderPosition( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayCenterUvs, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayOuterUvs, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setBorderOuterUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBorderPanelOverlayInnerUvs, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
			{
				overlay->getBorderPanelOverlay()->setBorderInnerUV( params[0]->get< castor::Point4d >() );
			}
			else
			{
				CU_ParsingError( cuT( "Overlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayFont, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				castor::String name;

				if ( getEngine( *blockContext )->hasFont( params[0]->get( name ) ) )
				{
					overlay->getTextOverlay()->setFont( name );
				}
				else
				{
					CU_ParsingError( cuT( "Unknown font" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayTextHeight, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setSDFHeight( params[0]->get< uint32_t >() );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayTextWrapping, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setTextWrappingMode( TextWrappingMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayVerticalAlign, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setVAlign( VAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayHorizontalAlign, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setHAlign( HAlign( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayTexturingMode, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setTexturingMode( TextTexturingMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayLineSpacingMode, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				overlay->getTextOverlay()->setLineSpacingMode( TextLineSpacingMode( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserTextOverlayText, OverlayContext )
		{
			auto overlay = blockContext->overlay.rptr;

			if ( overlay && overlay->getType() == OverlayType::eText )
			{
				castor::String strParams;
				params[0]->get( strParams );
				castor::string::replace( strParams, cuT( "\\a" ), cuT( "\a" ) );
				castor::string::replace( strParams, cuT( "\\b" ), cuT( "\b" ) );
				castor::string::replace( strParams, cuT( "\\f" ), cuT( "\f" ) );
				castor::string::replace( strParams, cuT( "\\n" ), cuT( "\n" ) );
				castor::string::replace( strParams, cuT( "\\r" ), cuT( "\r" ) );
				castor::string::replace( strParams, cuT( "\\t" ), cuT( "\t" ) );
				castor::string::replace( strParams, cuT( "\\v" ), cuT( "\v" ) );
				overlay->getTextOverlay()->setCaption( castor::toUtf8U32String( strParams ) );
			}
			else
			{
				CU_ParsingError( cuT( "TextOverlay not initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCameraParent, CameraContext )
		{
			castor::String name;
			SceneNodeRPtr parent = blockContext->scene->scene->findSceneNode( params[0]->get( name ) );

			if ( parent )
			{
				while ( parent->getParent()
					&& parent->getParent() != blockContext->scene->scene->getObjectRootNode()
					&& parent->getParent() != blockContext->scene->scene->getCameraRootNode() )
				{
					parent = parent->getParent();
				}

				if ( !parent->getParent()
					|| parent->getParent() == blockContext->scene->scene->getObjectRootNode() )
				{
					parent->attachTo( *blockContext->scene->scene->getCameraRootNode() );
				}

				blockContext->parentNode = parent;
			}
			else
			{
				CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCameraViewport, CameraContext )
		{
			blockContext->viewport = castor::makeUnique< Viewport >( *getEngine( *blockContext ) );
			blockContext->viewport->setPerspective( 0.0_degrees, 1, 0, 1 );
		}
		CU_EndAttributePushBlock( CSCNSection::eViewport, blockContext )

		static CU_ImplementAttributeParserBlock( parserCameraPrimitive, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->primitiveType = VkPrimitiveTopology( params[0]->get< uint32_t >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserCameraEnd, CameraContext )
		{
			if ( blockContext->viewport )
			{
				auto node = blockContext->parentNode;

				if ( !node )
				{
					node = blockContext->scene->scene->getCameraRootNode();
				}

				auto camera = blockContext->scene->scene->addNewCamera( blockContext->name
					, *blockContext->scene->scene
					, *node
					, castor::move( *blockContext->viewport.release() ) );
				camera->setHdrConfig( castor::move( blockContext->hdrConfig ) );
				camera->setColourGradingConfig( castor::move( blockContext->colourGradingConfig ) );
				log::info << "Loaded camera [" << camera->getName() << "]" << std::endl;
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserViewportType, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateType( ViewportType( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportLeft, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateLeft( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportRight, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateRight( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportTop, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateTop( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportBottom, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateBottom( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportNear, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateNear( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportFar, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateFar( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportSize, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->resize( params[0]->get< castor::Size >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportFovY, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateFovY( castor::Angle::fromDegrees( params[0]->get< float >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserViewportAspectRatio, CameraContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				blockContext->viewport->updateRatio( params[0]->get< float >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBillboardParent, BillboardsContext )
		{
			if ( blockContext->billboards )
			{
				castor::String name;

				if ( auto parent = blockContext->scene->scene->findSceneNode( params[0]->get( name ) ) )
				{
					parent->attachObject( *blockContext->billboards );
				}
				else
				{
					CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Geometry not initialised." ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBillboardType, BillboardsContext )
		{
			if ( !blockContext->billboards )
			{
				CU_ParsingError( cuT( "Billboard not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->billboards->setBillboardType( BillboardType( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBillboardSize, BillboardsContext )
		{
			if ( !blockContext->billboards )
			{
				CU_ParsingError( cuT( "Billboard not initialised" ) );
			}
			else if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->billboards->setBillboardSize( BillboardSize( params[0]->get< uint32_t >() ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBillboardPositions, BillboardsContext )
		{
			// Only push the block
		}
		CU_EndAttributePushBlock( CSCNSection::eBillboardList, blockContext )

		static CU_ImplementAttributeParserBlock( parserBillboardMaterial, BillboardsContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->billboards )
			{
				CU_ParsingError( cuT( "Billboard not initialised" ) );
			}
			else
			{
				castor::String name;

				if ( auto material = getEngine( *blockContext )->tryFindMaterial( params[0]->get( name ) ) )
				{
					blockContext->billboards->setMaterial( material );
				}
				else
				{
					CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBillboardDimensions, BillboardsContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->billboards->setDimensions( params[0]->get< castor::Point2f >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserBillboardEnd, BillboardsContext )
		{
			log::info << "Loaded billboards [" << blockContext->billboards->getName() << "]" << std::endl;

			if ( blockContext->ownBillboards )
			{
				blockContext->scene->scene->addBillboardList( blockContext->billboards->getName()
					, blockContext->ownBillboards
					, true );
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserBillboardPoint, BillboardsContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else
			{
				blockContext->billboards->addPoint( params[0]->get< castor::Point3f >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimatedObject, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group not initialised" ) );
			}
			else
			{
				castor::String name;

				if ( auto geometry = blockContext->scene->scene->findGeometry( params[0]->get( name ) ) )
				{
					if ( auto node = geometry->getParent();
						node && node->hasAnimation() )
					{
						blockContext->animNode = blockContext->animGroup->addObject( *node
							, node->getName() );
					}

					if ( auto mesh = geometry->getMesh() )
					{
						if ( mesh->hasAnimation() )
						{
							blockContext->animMesh = blockContext->animGroup->addObject( *mesh
								, *geometry
								, geometry->getName() );
						}

						if ( auto skeleton = mesh->getSkeleton() )
						{
							if ( skeleton->hasAnimation() )
							{
								blockContext->animSkeleton = blockContext->animGroup->addObject( *skeleton
									, *mesh
									, *geometry
									, geometry->getName() );
							}
						}
					}
				}
				else
				{
					if ( auto node = blockContext->scene->scene->findSceneNode( name ) )
					{
						if ( node->hasAnimation() )
						{
							blockContext->animNode = blockContext->animGroup->addObject( *node
								, node->getName() );
						}
					}
					else
					{
						CU_ParsingError( cuT( "No geometry or node with name " ) + name );
					}
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimatedMesh, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group not initialised" ) );
			}
			else
			{
				castor::String name;

				if ( auto geometry = blockContext->scene->scene->findGeometry( params[0]->get( name ) ) )
				{
					if ( auto mesh = geometry->getMesh() )
					{
						if ( mesh->hasAnimation() )
						{
							blockContext->animMesh = blockContext->animGroup->addObject( *mesh
								, *geometry
								, geometry->getName() );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Geometry [" ) + name + cuT( "] has no mesh" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No geometry with name [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimatedSkeleton, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group not initialised" ) );
			}
			else
			{
				castor::String name;

				if ( auto geometry = blockContext->scene->scene->findGeometry( params[0]->get( name ) ) )
				{
					if ( auto mesh = geometry->getMesh() )
					{
						if ( auto skeleton = mesh->getSkeleton() )
						{
							if ( skeleton->hasAnimation() )
							{
								blockContext->animSkeleton = blockContext->animGroup->addObject( *skeleton
									, *mesh
									, *geometry
									, geometry->getName() );
							}
						}
						else
						{
							CU_ParsingError( cuT( "Geometry [" ) + name + cuT( "]'s mesh has no skeleton" ) );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Geometry [" ) + name + cuT( "] has no mesh" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No geometry with name [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimatedNode, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group not initialised" ) );
			}
			else
			{
				castor::String name;

				if ( auto node = blockContext->scene->scene->findSceneNode( params[0]->get( name ) ) )
				{
					if ( node->hasAnimation() )
					{
						blockContext->animNode = blockContext->animGroup->addObject( *node
							, node->getName() );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No node with name [" ) + name + cuT( "]" ) );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimation, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				params[0]->get( blockContext->animName );
				blockContext->animGroup->addAnimation( blockContext->animName );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttributePushBlock( CSCNSection::eAnimation, blockContext )

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimationStart, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->startAnimation( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupAnimationPause, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->pauseAnimation( params[0]->get< castor::String >() );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimatedObjectGroupEnd, AnimGroupContext )
		{
			if ( !blockContext->animGroup )
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
			else
			{
				log::info << "Loaded animated object group [" << blockContext->animGroup->getName() << "]" << std::endl;
			}
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserAnimationLooped, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationLooped( blockContext->animName, params[0]->get< bool >() );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationScale, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationScale( blockContext->animName, params[0]->get< float >() );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationStartAt, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationStartingPoint( blockContext->animName
					, castor::Milliseconds{ uint64_t( params[0]->get< float >() * 1000.0f ) } );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationStopAt, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationStoppingPoint( blockContext->animName
					, castor::Milliseconds{ uint64_t( params[0]->get< float >() * 1000.0f ) } );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationInterpolation, AnimGroupContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter" ) );
			}
			else if ( blockContext->animGroup )
			{
				blockContext->animGroup->setAnimationInterpolation( blockContext->animName
					, InterpolatorType( params[0]->get< uint32_t >() ) );
			}
			else
			{
				CU_ParsingError( cuT( "No animated object group initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserAnimationEnd, AnimGroupContext )
		{
			// Only push the block
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserSkyboxVisible, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->skybox )
			{
				CU_ParsingError( cuT( "No skybox initialised." ) );
			}
			else
			{
				blockContext->skybox->setVisible( params[0]->get< bool >() );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxEqui, SkyboxContext )
		{
			if ( params.size() <= 1 )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->skybox )
			{
				CU_ParsingError( cuT( "No skybox initialised." ) );
			}
			else
			{
				auto path = params[0]->get< castor::Path >();
				auto filePath = context.file.getPath();

				if ( castor::File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setEquiTexture( filePath, path, params[1]->get< uint32_t >() );
				}
				else
				{
					blockContext->skybox.reset();
					castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxCross, SkyboxContext )
		{
			if ( params.size() < 1 )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( !blockContext->skybox )
			{
				CU_ParsingError( cuT( "No skybox initialised." ) );
			}
			else
			{
				auto path = params[0]->get< castor::Path >();
				auto filePath = context.file.getPath();

				if ( castor::File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setCrossTexture( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxLeft, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< castor::Path >();
				auto filePath = context.file.getPath();

				if ( castor::File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setLeftImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxRight, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< castor::Path >();
				auto filePath = context.file.getPath();

				if ( castor::File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setRightImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxTop, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< castor::Path >();
				auto filePath = context.file.getPath();

				if ( castor::File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setTopImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxBottom, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< castor::Path >();
				auto filePath = context.file.getPath();

				if ( castor::File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setBottomImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxFront, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< castor::Path >();
				auto filePath = context.file.getPath();

				if ( castor::File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setFrontImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxBack, SkyboxContext )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else if ( blockContext->skybox )
			{
				auto path = params[0]->get< castor::Path >();
				auto filePath = context.file.getPath();

				if ( castor::File::fileExists( filePath / path ) )
				{
					blockContext->skybox->setBackImage( filePath, path );
				}
				else
				{
					blockContext->skybox.reset();
					castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
					CU_ParsingError( err );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserSkyboxEnd, SkyboxContext )
		{
			if ( blockContext->skybox )
			{
				log::info << "Loaded skybox" << std::endl;
				blockContext->skybox->getScene().setBackground( castor::ptrRefCast< SceneBackground >( blockContext->skybox ) );
			}
			else
			{
				CU_ParsingError( cuT( "No skybox initialised" ) );
			}
		}
		CU_EndAttributePop()

		using namespace castor;

		static void addRootParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< RootContext > context{ result, CSCNSection::eRoot };
			context.addParser( cuT( "debug_overlays" ), parserRootDebugOverlays, { makeParameter< ParameterType::eBool >() } );
			context.addParser( cuT( "debug_targets" ), parserRootDebugTargets, { makeParameter< ParameterType::eBool >() } );
			context.addParser( cuT( "max_image_size" ), parserRootMaxImageSize, { makeParameter< ParameterType::eUInt32 >() } );
			context.addParser( cuT( "debug_max_image_size" ), parserRootDebugMaxImageSize, { makeParameter< ParameterType::eUInt32 >() } );
			context.addParser( cuT( "lpv_grid_size" ), parserRootLpvGridSize, { makeParameter< ParameterType::eUInt32 >() } );
			context.addParser( cuT( "default_unit" ), parserRootDefaultUnit, { makeParameter< ParameterType::eCheckedText, castor::LengthUnit >() } );
			context.addParser( cuT( "enable_full_loading" ), parserRootFullLoading, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			context.addPushParser( cuT( "scene" ), CSCNSection::eScene, parserRootScene, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "loading_screen" ), CSCNSection::eScene, parserRootLoadingScreen, {} );
			context.addPushParser( cuT( "font" ), CSCNSection::eFont, parserRootFont, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "sdf_font" ), CSCNSection::eSdfFont, parserRootSdfFont, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, parserRootPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, parserRootBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, parserRootTextOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "sampler" ), CSCNSection::eSampler, parserSamplerState< RootContext >, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "window" ), CSCNSection::eWindow, parserRootWindow, { makeParameter< ParameterType::eName >() } );
		}

		static void addWindowParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< WindowContext > context{ result, CSCNSection::eWindow, CSCNSection::eRoot };
			context.addParser( cuT( "vsync" ), parserWindowVSync, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			context.addParser( cuT( "fullscreen" ), parserWindowFullscreen, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			context.addParser( cuT( "allow_hdr" ), parserWindowAllowHdr, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			context.addPushParser( cuT( "render_target" ), CSCNSection::eRenderTarget, parserWindowRenderTarget );
			context.addPopParser( cuT( "}" ), parserWindowEnd );
		}

		static void addRenderTargetParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< TargetContext > context{ result, CSCNSection::eRenderTarget };
			context.addParser( cuT( "scene" ), parserRenderTargetScene, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "camera" ), parserRenderTargetCamera, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "size" ), parserRenderTargetSize, { makeParameter< ParameterType::eSize >() } );
			context.addParser( cuT( "format" ), parserRenderTargetFormat, { makeParameter< ParameterType::ePixelFormat >() } );
			context.addParser( cuT( "hdr_format" ), parserRenderTargetHDRFormat, { makeParameter< ParameterType::ePixelFormat >() } );
			context.addParser( cuT( "srgb_format" ), parserRenderTargetSRGBFormat, { makeParameter< ParameterType::ePixelFormat >() } );
			context.addParser( cuT( "stereo" ), parserRenderTargetStereo, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "postfx" ), parserRenderTargetPostEffect, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "tone_mapping" ), parserRenderTargetToneMapping, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "enable_full_loading" ), parserRenderTargetFullLoading, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			context.addPopParser( cuT( "}" ), parserRenderTargetEnd );
		}

		static void addSamplerParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< SamplerContext > context{ result, CSCNSection::eSampler };
			context.addParser( cuT( "min_filter" ), parserSamplerMinFilter, { makeParameter< ParameterType::eCheckedText, VkFilter >() } );
			context.addParser( cuT( "mag_filter" ), parserSamplerMagFilter, { makeParameter< ParameterType::eCheckedText, VkFilter >() } );
			context.addParser( cuT( "mip_filter" ), parserSamplerMipFilter, { makeParameter< ParameterType::eCheckedText, VkSamplerMipmapMode >() } );
			context.addParser( cuT( "min_lod" ), parserSamplerMinLod, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "max_lod" ), parserSamplerMaxLod, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "lod_bias" ), parserSamplerLodBias, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "u_wrap_mode" ), parserSamplerUWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
			context.addParser( cuT( "v_wrap_mode" ), parserSamplerVWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
			context.addParser( cuT( "w_wrap_mode" ), parserSamplerWWrapMode, { makeParameter< ParameterType::eCheckedText, VkSamplerAddressMode >() } );
			context.addParser( cuT( "border_colour" ), parserSamplerBorderColour, { makeParameter< ParameterType::eCheckedText, VkBorderColor >() } );
			context.addParser( cuT( "anisotropic_filtering" ), parserSamplerAnisotropicFiltering, { makeParameter< ParameterType::eBool >() } );
			context.addParser( cuT( "max_anisotropy" ), parserSamplerMaxAnisotropy, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "comparison_mode" ), parserSamplerComparisonMode, { makeParameter< ParameterType::eCheckedText, LimitedType< VkCompareOp > >() } );
			context.addParser( cuT( "comparison_func" ), parserSamplerComparisonFunc, { makeParameter< ParameterType::eCheckedText, VkCompareOp >() } );
			context.addPopParser( cuT( "}" ), parserSamplerEnd );
		}

		static void addSceneParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< SceneContext > context{ result, CSCNSection::eScene, CSCNSection::eRoot };
			context.addParser( cuT( "background_colour" ), parserSceneBkColour, { makeParameter< ParameterType::eRgbColour >() } );
			context.addParser( cuT( "background_image" ), parserSceneBkImage, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "ambient_light" ), parserSceneAmbientLight, { makeParameter< ParameterType::eRgbColour >() } );
			context.addParser( cuT( "fog_type" ), parserSceneFogType, { makeParameter< ParameterType::eCheckedText, FogType >() } );
			context.addParser( cuT( "fog_density" ), parserSceneFogDensity, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "directional_shadow_cascades" ), parserDirectionalShadowCascades, { makeParameter< ParameterType::eUInt32 >( castor::makeRange( 0u, MaxDirectionalCascadesCount ) ) } );
			context.addPushParser( cuT( "font" ), CSCNSection::eFont, parserSceneFont, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "sdf_font" ), CSCNSection::eSdfFont, parserSceneSdfFont, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "sampler" ), CSCNSection::eSampler, parserSamplerState< SceneContext >, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "camera" ), CSCNSection::eCamera, parserSceneCamera, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "light" ), CSCNSection::eLight, parserSceneLight, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "camera_node" ), CSCNSection::eNode, parserSceneCameraNode, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "scene_node" ), CSCNSection::eNode, parserSceneSceneNode, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "object" ), CSCNSection::eObject, parserSceneObject, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "import" ), CSCNSection::eSceneImport, parserSceneImport );
			context.addPushParser( cuT( "billboard" ), CSCNSection::eBillboard, parserSceneBillboard, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "animated_object_group" ), CSCNSection::eAnimGroup, parserSceneAnimatedObjectGroup, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, parserScenePanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, parserSceneBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, parserSceneTextOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "skybox" ), CSCNSection::eSkybox, parserSceneSkybox );
			context.addPushParser( cuT( "particle_system" ), CSCNSection::eParticleSystem, parserSceneParticleSystem, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "skeleton" ), CSCNSection::eSkeleton, parserSkeleton, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "mesh" ), CSCNSection::eMesh, parserSceneMesh, { makeParameter< ParameterType::eName >() } );
			context.addPopParser( cuT( "}" ), parserSceneEnd );
		}

		static void addSceneImportParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< SceneImportContext > context{ result, CSCNSection::eSceneImport, CSCNSection::eScene };
			context.addParser( cuT( "file" ), parserSceneImportFile, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "file_anim" ), parserSceneImportAnimFile, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "prefix" ), parserSceneImportPrefix, { makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "rescale" ), parserSceneImportRescale, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "pitch" ), parserSceneImportPitch, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "yaw" ), parserSceneImportYaw, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "roll" ), parserSceneImportRoll, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "no_optimisations" ), parserSceneImportNoOptimisations, { makeParameter< ParameterType::eBool >() } );
			context.addParser( cuT( "emissive_mult" ), parserSceneImportEmissiveMult, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "recenter_camera" ), parserSceneImportCenterCamera, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "preferred_importer" ), parserSceneImportPreferredImporter, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "ignore_vertex_colour" ), parserSceneImportIgnoreVertexColour, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			context.addPushParser( cuT( "texture_remap_config" ), CSCNSection::eTextureRemap, parserSceneImportTexRemap );
			context.addPopParser( cuT( "}" ), parserSceneImportEnd );
		}

		static void addParticleSystemParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< ParticleSystemContext > context{ result, CSCNSection::eParticleSystem, CSCNSection::eScene };
			context.addParser( cuT( "parent" ), parserParticleSystemParent, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "particles_count" ), parserParticleSystemCount, { makeParameter< ParameterType::eUInt32 >() } );
			context.addParser( cuT( "material" ), parserParticleSystemMaterial, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "dimensions" ), parserParticleSystemDimensions, { makeParameter< ParameterType::ePoint2F >() } );
			context.addPushParser( cuT( "particle" ), CSCNSection::eParticle, parserParticleSystemParticle );
			context.addPushParser( cuT( "cs_shader_program" ), CSCNSection::eShaderProgram, parserParticleSystemCSShader );
			context.addPopParser( cuT( "}" ), parserParticleSystemEnd );
		}

		static void addParticleParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< ParticleSystemContext > context{ result, CSCNSection::eParticle, CSCNSection::eParticleSystem };
			context.addParser( cuT( "variable" ), parserParticleVariable, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eCheckedText, ParticleFormat >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "type" ), parserParticleType, { makeParameter< ParameterType::eName >() } );
			context.addDefaultPopParser();
		}

		static void addLightParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< LightContext > context{ result, CSCNSection::eLight, CSCNSection::eScene };
			context.addParser( cuT( "parent" ), parserLightParent, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "type" ), parserLightType, { makeParameter< ParameterType::eCheckedText, LightType >() } );
			context.addParser( cuT( "colour" ), parserLightColour, { makeParameter< ParameterType::ePoint3F >() } );
			context.addParser( cuT( "intensity" ), parserLightIntensity, { makeParameter< ParameterType::ePoint2F >() } );
			context.addParser( cuT( "attenuation" ), parserLightAttenuation, { makeParameter< ParameterType::ePoint3F >() } );
			context.addParser( cuT( "range" ), parserLightRange, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "cut_off" ), parserLightCutOff, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "inner_cut_off" ), parserLightInnerCutOff, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "outer_cut_off" ), parserLightOuterCutOff, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "exponent" ), parserLightExponent, { makeParameter< ParameterType::eFloat >() } );
			context.addPopParser( cuT( "}" ), parserLightEnd );
		}

		static void addNodeParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< NodeContext > context{ result, CSCNSection::eNode, CSCNSection::eScene };
			context.addParser( cuT( "static" ), parserNodeStatic, { makeParameter< ParameterType::eBool >() } );
			context.addParser( cuT( "parent" ), parserNodeParent, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "position" ), parserNodePosition, { makeParameter< ParameterType::ePoint3F >() } );
			context.addParser( cuT( "orientation" ), parserNodeOrientation, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "rotate" ), parserNodeRotate, { makeParameter< ParameterType::ePoint3F >(), makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "direction" ), parserNodeDirection, { makeParameter< ParameterType::ePoint3F >() } );
			context.addParser( cuT( "scale" ), parserNodeScale, { makeParameter< ParameterType::ePoint3F >() } );
			context.addPopParser( cuT( "}" ), parserNodeEnd );
		}

		static void addObjectParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< ObjectContext > context{ result, CSCNSection::eObject, CSCNSection::eScene };
			context.addParser( cuT( "parent" ), parserObjectParent, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "material" ), parserObjectMaterial, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "cast_shadows" ), parserObjectCastShadows, { makeParameter< ParameterType::eBool >() } );
			context.addParser( cuT( "receive_shadows" ), parserObjectReceivesShadows, { makeParameter< ParameterType::eBool >() } );
			context.addParser( cuT( "cullable" ), parserObjectCullable, { makeParameter< ParameterType::eBool >() } );
			context.addPushParser( cuT( "mesh" ), CSCNSection::eMesh, parserObjectMesh, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "materials" ), CSCNSection::eObjectMaterials, parserObjectMaterials );
			context.addPopParser( cuT( "}" ), parserObjectEnd );
		}

		static void addObjectMaterialsParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< ObjectContext > context{ result, CSCNSection::eObjectMaterials, CSCNSection::eObject };
			context.addParser( cuT( "material" ), parserObjectMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
			context.addPopParser( cuT( "}" ), parserObjectMaterialsEnd );
		}

		static void addSkeletonParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< SkeletonContext > context{ result, CSCNSection::eSkeleton, CSCNSection::eScene };
			context.addParser( cuT( "import" ), parserSkeletonImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "import_anim" ), parserSkeletonAnimImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "import_single_anim" ), parserSkeletonSingleAnimImport, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			context.addPopParser( cuT( "}" ), parserSkeletonEnd );
		}

		static void addMeshParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< MeshContext > context{ result, CSCNSection::eMesh };
			context.addParser( cuT( "type" ), parserMeshType, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "import" ), parserMeshImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "import_anim" ), parserMeshAnimImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "import_single_anim" ), parserMeshSingleAnimImport, { makeParameter< ParameterType::eName >(), makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "import_morph_target" ), parserMeshMorphTargetImport, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "default_material" ), parserMeshDefaultMaterial, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "skeleton" ), parserMeshSkeleton, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "morph_animation" ), CSCNSection::eMorphAnimation, parserMeshMorphAnimation, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "submesh" ), CSCNSection::eSubmesh, parserMeshSubmesh );
			context.addPushParser( cuT( "default_materials" ), CSCNSection::eMeshDefaultMaterials, parserMeshDefaultMaterials );
			context.addPopParser( cuT( "}" ), parserMeshEnd );
		}

		static void addMorphAnimationParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< MeshContext > context{ result, CSCNSection::eMorphAnimation, CSCNSection::eMesh };
			context.addParser( cuT( "target_weight" ), parserMeshMorphTargetWeight, ParserParameterArray{ makeParameter< ParameterType::eFloat >(), makeParameter< ParameterType::eUInt32 >(), makeParameter< ParameterType::eFloat >() } );
			context.addPopParser( cuT( "}" ), parserMeshMorphAnimationEnd );
		}

		static void addMeshDefaultMaterialsParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< MeshContext > context{ result, CSCNSection::eMeshDefaultMaterials, CSCNSection::eMesh };
			context.addParser( cuT( "material" ), parserMeshDefaultMaterialsMaterial, { makeParameter< ParameterType::eUInt16 >(), makeParameter< ParameterType::eName >() } );
			context.addPopParser( cuT( "}" ), parserMeshDefaultMaterialsEnd );
		}

		static void addSubmeshParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< SubmeshContext > context{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };
			context.addParser( cuT( "vertex" ), parserSubmeshVertex, { makeParameter< ParameterType::ePoint3F >() } );
			context.addParser( cuT( "face" ), parserSubmeshFace, { makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "face_uv" ), parserSubmeshFaceUV, { makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "face_uvw" ), parserSubmeshFaceUVW, { makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "face_normals" ), parserSubmeshFaceNormals, { makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "face_tangents" ), parserSubmeshFaceTangents, { makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "uv" ), parserSubmeshUV, { makeParameter< ParameterType::ePoint2F >() } );
			context.addParser( cuT( "uvw" ), parserSubmeshUVW, { makeParameter< ParameterType::ePoint3F >() } );
			context.addParser( cuT( "normal" ), parserSubmeshNormal, { makeParameter< ParameterType::ePoint3F >() } );
			context.addParser( cuT( "tangent" ), parserSubmeshTangent, { makeParameter< ParameterType::ePoint4F >() } );
			context.addPopParser( cuT( "}" ), parserSubmeshEnd );
		}

		static void addShaderProgramParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< ProgramContext > context{ result, CSCNSection::eShaderProgram };
			context.addPushParser( cuT( "compute_program" ), CSCNSection::eShaderStage, parserComputeShader );
			context.addPopParser( cuT( "}" ), parserShaderProgramEnd );
		}

		static void addShaderStageParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< ProgramContext > context{ result, CSCNSection::eShaderStage, CSCNSection::eShaderProgram };
			context.addParser( cuT( "file" ), parserShaderFile, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "group_sizes" ), parserShaderGroupSizes, { makeParameter< ParameterType::ePoint3I >() } );
			context.addDefaultPopParser();
		}

		static void addFontParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< FontContext > context{ result, CSCNSection::eFont };
			context.addParser( cuT( "file" ), parserFontFile, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "height" ), parserFontHeight, { makeParameter< ParameterType::eInt16 >() } );
			context.addPopParser( cuT( "}" ), parserFontEnd );
		}

		static void addSdfFontParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< FontContext > context{ result, CSCNSection::eSdfFont };
			context.addParser( cuT( "file" ), parserSdfFontFile, { makeParameter< ParameterType::ePath >() } );
			context.addPopParser( cuT( "}" ), parserSdfFontEnd );
		}

		static void addPanelOverlayParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< OverlayContext > context{ result, CSCNSection::ePanelOverlay };
			context.addParser( cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			context.addParser( cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			context.addParser( cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			context.addParser( cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			context.addParser( cuT( "uv" ), parserPanelOverlayUvs, { makeParameter< ParameterType::ePoint4D >() } );
			context.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPopParser( cuT( "}" ), parserOverlayEnd );
		}

		static void addBorderPanelOverlayParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< OverlayContext > context{ result, CSCNSection::eBorderPanelOverlay };
			context.addParser( cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			context.addParser( cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			context.addParser( cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			context.addParser( cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			context.addParser( cuT( "border_material" ), parserBorderPanelOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "border_size" ), parserBorderPanelOverlaySizes, { makeParameter< ParameterType::ePoint4D >() } );
			context.addParser( cuT( "pxl_border_size" ), parserBorderPanelOverlayPixelSizes, { makeParameter< ParameterType::ePoint4U >() } );
			context.addParser( cuT( "border_position" ), parserBorderPanelOverlayPosition, { makeParameter< ParameterType::eCheckedText, BorderPosition >() } );
			context.addParser( cuT( "center_uv" ), parserBorderPanelOverlayCenterUvs, { makeParameter< ParameterType::ePoint4D >() } );
			context.addParser( cuT( "border_inner_uv" ), parserBorderPanelOverlayInnerUvs, { makeParameter< ParameterType::ePoint4D >() } );
			context.addParser( cuT( "border_outer_uv" ), parserBorderPanelOverlayOuterUvs, { makeParameter< ParameterType::ePoint4D >() } );
			context.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPopParser( cuT( "}" ), parserOverlayEnd );
		}

		static void addTextOverlayParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< OverlayContext > context{ result, CSCNSection::eTextOverlay };
			context.addParser( cuT( "material" ), parserOverlayMaterial, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "position" ), parserOverlayPosition, { makeParameter< ParameterType::ePoint2D >() } );
			context.addParser( cuT( "size" ), parserOverlaySize, { makeParameter< ParameterType::ePoint2D >() } );
			context.addParser( cuT( "pxl_size" ), parserOverlayPixelSize, { makeParameter< ParameterType::eSize >() } );
			context.addParser( cuT( "pxl_position" ), parserOverlayPixelPosition, { makeParameter< ParameterType::ePosition >() } );
			context.addParser( cuT( "font" ), parserTextOverlayFont, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "text_height" ), parserTextOverlayTextHeight, { makeParameter< ParameterType::eUInt32 >() } );
			context.addParser( cuT( "text" ), parserTextOverlayText, { makeParameter< ParameterType::eText >() } );
			context.addParser( cuT( "text_wrapping" ), parserTextOverlayTextWrapping, { makeParameter< ParameterType::eCheckedText, TextWrappingMode >() } );
			context.addParser( cuT( "vertical_align" ), parserTextOverlayVerticalAlign, { makeParameter< ParameterType::eCheckedText, VAlign >() } );
			context.addParser( cuT( "horizontal_align" ), parserTextOverlayHorizontalAlign, { makeParameter< ParameterType::eCheckedText, HAlign >() } );
			context.addParser( cuT( "texturing_mode" ), parserTextOverlayTexturingMode, { makeParameter< ParameterType::eCheckedText, TextTexturingMode >() } );
			context.addParser( cuT( "line_spacing_mode" ), parserTextOverlayLineSpacingMode, { makeParameter< ParameterType::eCheckedText, TextLineSpacingMode >() } );
			context.addPushParser( cuT( "panel_overlay" ), CSCNSection::ePanelOverlay, parserOverlayPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "border_panel_overlay" ), CSCNSection::eBorderPanelOverlay, parserOverlayBorderPanelOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "text_overlay" ), CSCNSection::eTextOverlay, parserOverlayTextOverlay, { makeParameter< ParameterType::eName >() } );
			context.addPopParser( cuT( "}" ), parserOverlayEnd );
		}

		static void addCameraParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< CameraContext > context{ result, CSCNSection::eCamera, CSCNSection::eScene };
			context.addParser( cuT( "parent" ), parserCameraParent, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "primitive" ), parserCameraPrimitive, { makeParameter< ParameterType::eCheckedText, VkPrimitiveTopology >() } );
			context.addPushParser( cuT( "viewport" ), CSCNSection::eViewport, parserCameraViewport );
			context.addPopParser( cuT( "}" ), parserCameraEnd );
		}

		static void addViewportParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< CameraContext > context{ result, CSCNSection::eViewport, CSCNSection::eCamera };
			context.addParser( cuT( "type" ), parserViewportType, { makeParameter< ParameterType::eCheckedText, ViewportType >() } );
			context.addParser( cuT( "left" ), parserViewportLeft, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "right" ), parserViewportRight, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "top" ), parserViewportTop, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "bottom" ), parserViewportBottom, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "near" ), parserViewportNear, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "far" ), parserViewportFar, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "size" ), parserViewportSize, { makeParameter< ParameterType::eSize >() } );
			context.addParser( cuT( "fov_y" ), parserViewportFovY, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "aspect_ratio" ), parserViewportAspectRatio, { makeParameter< ParameterType::eFloat >() } );
			context.addDefaultPopParser();
		}

		static void addBillboardParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< BillboardsContext > context{ result, CSCNSection::eBillboard, CSCNSection::eScene };
			context.addParser( cuT( "parent" ), parserBillboardParent, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "type" ), parserBillboardType, { makeParameter < ParameterType::eCheckedText, BillboardType >() } );
			context.addParser( cuT( "size" ), parserBillboardSize, { makeParameter < ParameterType::eCheckedText, BillboardSize >() } );
			context.addParser( cuT( "material" ), parserBillboardMaterial, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "dimensions" ), parserBillboardDimensions, { makeParameter< ParameterType::ePoint2F >() } );
			context.addPushParser( cuT( "positions" ), CSCNSection::eBillboardList, parserBillboardPositions );
			context.addPopParser( cuT( "}" ), parserBillboardEnd );
		}

		static void addBillboardListParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< BillboardsContext > context{ result, CSCNSection::eBillboardList, CSCNSection::eBillboard };
			context.addParser( cuT( "pos" ), parserBillboardPoint, { makeParameter< ParameterType::ePoint3F >() } );
			context.addDefaultPopParser();
		}

		static void addAnimGroupParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< AnimGroupContext > context{ result, CSCNSection::eAnimGroup, CSCNSection::eScene };
			context.addParser( cuT( "animated_object" ), parserAnimatedObjectGroupAnimatedObject, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "animated_mesh" ), parserAnimatedObjectGroupAnimatedMesh, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "animated_skeleton" ), parserAnimatedObjectGroupAnimatedSkeleton, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "animated_node" ), parserAnimatedObjectGroupAnimatedNode, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "start_animation" ), parserAnimatedObjectGroupAnimationStart, { makeParameter< ParameterType::eName >() } );
			context.addParser( cuT( "pause_animation" ), parserAnimatedObjectGroupAnimationPause, { makeParameter< ParameterType::eName >() } );
			context.addPushParser( cuT( "animation" ), CSCNSection::eAnimation, parserAnimatedObjectGroupAnimation, { makeParameter< ParameterType::eName >() } );
			context.addPopParser( cuT( "}" ), parserAnimatedObjectGroupEnd );
		}

		static void addAnimationParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< AnimGroupContext > context{ result, CSCNSection::eAnimation, CSCNSection::eAnimGroup };
			context.addParser( cuT( "looped" ), parserAnimationLooped, { makeParameter< ParameterType::eBool >() } );
			context.addParser( cuT( "scale" ), parserAnimationScale, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "start_at" ), parserAnimationStartAt, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "stop_at" ), parserAnimationStopAt, { makeParameter< ParameterType::eFloat >() } );
			context.addParser( cuT( "interpolation" ), parserAnimationInterpolation, { makeParameter< ParameterType::eCheckedText, InterpolatorType >() } );
			context.addPopParser( cuT( "}" ), parserAnimationEnd );
		}

		static void addSkyboxParsers( castor::AttributeParsers & result )
		{
			BlockParserContextT< SkyboxContext > context{ result, CSCNSection::eSkybox, CSCNSection::eScene };
			context.addParser( cuT( "visible" ), parserSkyboxVisible, { makeDefaultedParameter< ParameterType::eBool >( true ) } );
			context.addParser( cuT( "equirectangular" ), parserSkyboxEqui, { makeParameter< ParameterType::ePath >(), makeParameter< ParameterType::eUInt32 >() } );
			context.addParser( cuT( "cross" ), parserSkyboxCross, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "left" ), parserSkyboxLeft, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "right" ), parserSkyboxRight, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "top" ), parserSkyboxTop, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "bottom" ), parserSkyboxBottom, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "front" ), parserSkyboxFront, { makeParameter< ParameterType::ePath >() } );
			context.addParser( cuT( "back" ), parserSkyboxBack, { makeParameter< ParameterType::ePath >() } );
			context.addPopParser( cuT( "}" ), parserSkyboxEnd );
		}

		static castor::AttributeParsers registerParsers( Engine const & engine )
		{
			using namespace castor;
			AttributeParsers result;
			auto & textureChannels = engine.getPassComponentsRegister().getTextureChannels();

			addRootParsers( result );
			addSceneParsers( result );
			addWindowParsers( result );
			addSamplerParsers( result );
			addCameraParsers( result );
			addViewportParsers( result );
			addLightParsers( result );
			addNodeParsers( result );
			addObjectParsers( result );
			addObjectMaterialsParsers( result );
			addFontParsers( result );
			addSdfFontParsers( result );
			addPanelOverlayParsers( result );
			addBorderPanelOverlayParsers( result );
			addTextOverlayParsers( result );
			addMeshParsers( result );
			addSubmeshParsers( result );
			addRenderTargetParsers( result );
			addShaderProgramParsers( result );
			addShaderStageParsers( result );
			addBillboardParsers( result );
			addBillboardListParsers( result );
			addAnimGroupParsers( result );
			addAnimationParsers( result );
			addSkyboxParsers( result );
			addParticleSystemParsers( result );
			addParticleParsers( result );
			addMeshDefaultMaterialsParsers( result );
			addSceneImportParsers( result );
			addSkeletonParsers( result );
			addMorphAnimationParsers( result );
			ClustersConfig::addParsers( result );
			HdrConfig::addParsers( result );
			ShadowConfig::addParsers( result );
			SsaoConfig::addParsers( result );
			VctConfig::addParsers( result );
			Material::addParsers( result, textureChannels );

			return result;
		}
	}

	castor::AdditionalParsers createSceneFileParsers( Engine const & engine )
	{
		return { scnprs::registerParsers( engine )
			, registerSceneFileSections()
			, nullptr };
	}

	castor::StrUInt32Map registerSceneFileSections()
	{
		return { { uint32_t( CSCNSection::eRoot ), castor::String{} }
			, { uint32_t( CSCNSection::eScene ), cuT( "scene" ) }
			, { uint32_t( CSCNSection::eWindow ), cuT( "window" ) }
			, { uint32_t( CSCNSection::eSampler ), cuT( "sampler" ) }
			, { uint32_t( CSCNSection::eCamera ), cuT( "camera" ) }
			, { uint32_t( CSCNSection::eViewport ), cuT( "viewport" ) }
			, { uint32_t( CSCNSection::eLight ), cuT( "light" ) }
			, { uint32_t( CSCNSection::eNode ), cuT( "scene_node" ) }
			, { uint32_t( CSCNSection::eObject ), cuT( "object" ) }
			, { uint32_t( CSCNSection::eObjectMaterials ), cuT( "materials" ) }
			, { uint32_t( CSCNSection::eFont ), cuT( "font" ) }
			, { uint32_t( CSCNSection::ePanelOverlay ), cuT( "panel_overlay" ) }
			, { uint32_t( CSCNSection::eBorderPanelOverlay ), cuT( "border_panel_overlay" ) }
			, { uint32_t( CSCNSection::eTextOverlay ), cuT( "text_overlay" ) }
			, { uint32_t( CSCNSection::eMesh ), cuT( "mesh" ) }
			, { uint32_t( CSCNSection::eSubmesh ), cuT( "submesh" ) }
			, { uint32_t( CSCNSection::eMaterial ), cuT( "material" ) }
			, { uint32_t( CSCNSection::ePass ), cuT( "pass" ) }
			, { uint32_t( CSCNSection::eTextureUnit ), cuT( "texture_unit" ) }
			, { uint32_t( CSCNSection::eRenderTarget ), cuT( "render_target" ) }
			, { uint32_t( CSCNSection::eShaderProgram ), cuT( "shader_program" ) }
			, { uint32_t( CSCNSection::eShaderStage ), cuT( "shader_object" ) }
			, { uint32_t( CSCNSection::eUBOVariable ), cuT( "variable" ) }
			, { uint32_t( CSCNSection::eBillboard ), cuT( "billboard" ) }
			, { uint32_t( CSCNSection::eBillboardList ), cuT( "positions" ) }
			, { uint32_t( CSCNSection::eAnimGroup ), cuT( "animated_object_group" ) }
			, { uint32_t( CSCNSection::eAnimation ), cuT( "animation" ) }
			, { uint32_t( CSCNSection::eSkybox ), cuT( "skybox" ) }
			, { uint32_t( CSCNSection::eParticleSystem ), cuT( "particle_system" ) }
			, { uint32_t( CSCNSection::eParticle ), cuT( "particle" ) }
			, { uint32_t( CSCNSection::eSsao ), cuT( "ssao" ) }
			, { uint32_t( CSCNSection::eHdrConfig ), cuT( "hdr_config" ) }
			, { uint32_t( CSCNSection::eShadows ), cuT( "shadows" ) }
			, { uint32_t( CSCNSection::eMeshDefaultMaterials ), cuT( "default_materials" ) }
			, { uint32_t( CSCNSection::eLpv ), cuT( "lpv_config" ) }
			, { uint32_t( CSCNSection::eRaw ), cuT( "raw_config" ) }
			, { uint32_t( CSCNSection::ePcf ), cuT( "pcf_config" ) }
			, { uint32_t( CSCNSection::eVsm ), cuT( "vsm_config" ) }
			, { uint32_t( CSCNSection::eRsm ), cuT( "rsm_config" ) }
			, { uint32_t( CSCNSection::eTextureAnimation ), cuT( "texture_animation" ) }
			, { uint32_t( CSCNSection::eVoxelConeTracing ), cuT( "voxel_cone_tracing" ) }
			, { uint32_t( CSCNSection::eTextureTransform ), cuT( "texture_transform" ) }
			, { uint32_t( CSCNSection::eSceneImport ), cuT( "import" ) }
			, { uint32_t( CSCNSection::eSkeleton ), cuT( "skeleton" ) }
			, { uint32_t( CSCNSection::eMorphAnimation ), cuT( "morph_animation" ) }
			, { uint32_t( CSCNSection::eTextureRemapChannel ), cuT( "texture_remap_channel" ) }
			, { uint32_t( CSCNSection::eTextureRemap ), cuT( "texture_remap" ) }
			, { uint32_t( CSCNSection::eClusters ), cuT( "clusters" ) }
			, { uint32_t( CSCNSection::eTexture ), cuT( "texture" ) }
			, { uint32_t( CSCNSection::eColourGrading ), cuT( "colour_grading" ) } };
	}

	uint32_t getSceneFileRootSection()
	{
		return uint32_t( CSCNSection::eRoot );
	}
}
