#include "Castor3D/Scene/SceneFileParser_Parsers.hpp"

#include "Castor3D/Engine.hpp"
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
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Material/Texture/Sampler.hpp"
#include "Castor3D/Material/Texture/Animation/TextureAnimation.hpp"
#include "Castor3D/Miscellaneous/LoadingScreen.hpp"
#include "Castor3D/Model/Mesh/Importer.hpp"
#include "Castor3D/Model/Mesh/ImporterFactory.hpp"
#include "Castor3D/Model/Mesh/Mesh.hpp"
#include "Castor3D/Model/Mesh/MeshFactory.hpp"
#include "Castor3D/Model/Mesh/MeshGenerator.hpp"
#include "Castor3D/Model/Mesh/Subdivider.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimation.hpp"
#include "Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/Skeleton.hpp"
#include "Castor3D/Overlay/Overlay.hpp"
#include "Castor3D/Overlay/BorderPanelOverlay.hpp"
#include "Castor3D/Overlay/PanelOverlay.hpp"
#include "Castor3D/Overlay/TextOverlay.hpp"
#include "Castor3D/Render/RenderLoop.hpp"
#include "Castor3D/Render/RenderTarget.hpp"
#include "Castor3D/Render/RenderWindow.hpp"
#include "Castor3D/Scene/BillboardList.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Geometry.hpp"
#include "Castor3D/Scene/Scene.hpp"
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
#include <CastorUtils/FileParser/ParserParameter.hpp>

#pragma GCC diagnostic ignored "-Wnull-dereference"

using namespace castor;

namespace castor3d
{
	namespace
	{
		void fillMeshImportParameters( castor::FileParserContext & context
			, String const & meshParams
			, Parameters & parameters )
		{
			StringArray paramArray = string::split( meshParams, cuT( "-" ), 20, false );

			for ( auto param : paramArray )
			{
				if ( param.find( cuT( "smooth_normals" ) ) == 0 )
				{
					String strNml = cuT( "smooth" );
					parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
				}
				else if ( param.find( cuT( "flat_normals" ) ) == 0 )
				{
					String strNml = cuT( "flat" );
					parameters.add( cuT( "normals" ), strNml.c_str(), uint32_t( strNml.size() ) );
				}
				else if ( param.find( cuT( "tangent_space" ) ) == 0 )
				{
					parameters.add( cuT( "tangent_space" ), true );
				}
				else if ( param.find( cuT( "pitch" ) ) == 0 )
				{
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != String::npos )
					{
						float value;
						string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "pitch" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -pitch=<degrees>." ) );
					}
				}
				else if ( param.find( cuT( "yaw" ) ) == 0 )
				{
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != String::npos )
					{
						float value;
						string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "yaw" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -yaw=<degrees>." ) );
					}
				}
				else if ( param.find( cuT( "roll" ) ) == 0 )
				{
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != String::npos )
					{
						float value;
						string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "roll" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -roll=<degrees>." ) );
					}
				}
				else if ( param.find( cuT( "split_mesh" ) ) == 0 )
				{
					parameters.add( cuT( "split_mesh" ), true );
				}
				else if ( param.find( cuT( "rescale" ) ) == 0 )
				{
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != String::npos )
					{
						float value;
						string::parse< float >( param.substr( eqIndex + 1 ), value );
						parameters.add( cuT( "rescale" ), value );
					}
					else
					{
						CU_ParsingError( cuT( "Malformed parameter -rescale=<float>." ) );
					}
				}
				else if ( param.find( cuT( "no_optimisations" ) ) == 0 )
				{
					parameters.add( cuT( "no_optimisations" ), true );
				}
			}
		}
	}

	CU_ImplementAttributeParser( parserMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.strName );
			parsingContext.material = parsingContext.parser->getEngine()->getMaterialCache().tryFind( parsingContext.strName ).lock().get();
			parsingContext.passIndex = 0u;
			parsingContext.createMaterial = parsingContext.material == nullptr;

			if ( parsingContext.createMaterial )
			{
				parsingContext.ownMaterial = parsingContext.parser->getEngine()->getMaterialCache().create( parsingContext.strName
					, *parsingContext.parser->getEngine()
					, parsingContext.parser->getEngine()->getPassesType() );
				parsingContext.material = parsingContext.ownMaterial.get();
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eMaterial )
		
	CU_ImplementAttributeParser( parserSamplerState )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext.sampler = parsingContext.parser->getEngine()->getSamplerCache().tryFind( params[0]->get( name ) );

			if ( !parsingContext.sampler.lock() )
			{
				parsingContext.ownSampler = parsingContext.parser->getEngine()->getSamplerCache().create( name
					, *parsingContext.parser->getEngine() );
				parsingContext.sampler = parsingContext.ownSampler;
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eSampler )

	CU_ImplementAttributeParser( parserRootScene )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			params[0]->get( name );
			parsingContext.scene = parsingContext.parser->getEngine()->getSceneCache().tryFind( name ).lock();

			if ( !parsingContext.scene )
			{
				parsingContext.scene = std::make_shared< Scene >( name
					, *parsingContext.parser->getEngine() );
			}

			parsingContext.sceneNode = parsingContext.scene->getRootNode();
			parsingContext.mapScenes.insert( std::make_pair( name, parsingContext.scene ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootLoadingScreen )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.scene = std::make_shared< Scene >( LoadingScreen::SceneName
			, *parsingContext.parser->getEngine() );
		parsingContext.sceneNode = parsingContext.scene->getRootNode();
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootFont )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.path.clear();

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.strName );
			parsingContext.fontHeight = 0u;
		}
	}
	CU_EndAttributePush( CSCNSection::eFont )

	CU_ImplementAttributeParser( parserRootMaterials )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext.parser->getEngine()->setPassesType( PassTypeID( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRootPanelOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext.parentOverlays.push_back( parsingContext.overlay );
			auto parent = parsingContext.parentOverlays.back().get();
			parsingContext.overlay = parsingContext.parser->getEngine()->getOverlayCache().tryFind( params[0]->get( name ) ).lock();

			if ( !parsingContext.overlay )
			{
				parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::ePanel
					, nullptr
					, parent );
				parsingContext.overlay->rename( name );
			}

			parsingContext.overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserRootBorderPanelOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext.parentOverlays.push_back( parsingContext.overlay );
			auto parent = parsingContext.parentOverlays.back().get();
			parsingContext.overlay = parsingContext.parser->getEngine()->getOverlayCache().tryFind( params[0]->get( name ) ).lock();

			if ( !parsingContext.overlay )
			{
				parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::eBorderPanel
					, nullptr
					, parent );
				parsingContext.overlay->rename( name );
			}

			parsingContext.overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserRootTextOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			parsingContext.parentOverlays.push_back( parsingContext.overlay );
			auto parent = parsingContext.parentOverlays.back().get();
			parsingContext.overlay = parsingContext.parser->getEngine()->getOverlayCache().tryFind( params[0]->get( name ) ).lock();

			if ( !parsingContext.overlay )
			{
				parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::eText
					, nullptr
					, parent );
				parsingContext.overlay->rename( name );
			}

			parsingContext.overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserRootDebugOverlays )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			bool value;
			parsingContext.parser->getEngine()->getRenderLoop().showDebugOverlays( params[0]->get( value ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootWindow )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.window.name );
		}
	}
	CU_EndAttributePush( CSCNSection::eWindow )

	CU_ImplementAttributeParser( parserRootLpvGridSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [count] parameter." ) );
		}
		else
		{
			uint32_t count;
			params[0]->get( count );
			parsingContext.parser->getEngine()->setLpvGridSize( count );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWindowRenderTarget )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.targetType = TargetType::eWindow;
		parsingContext.size = { 1u, 1u };
		parsingContext.pixelFormat = castor::PixelFormat::eUNDEFINED;
	}
	CU_EndAttributePush( CSCNSection::eRenderTarget )

	CU_ImplementAttributeParser( parserWindowVSync )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !params.empty() )
		{
			params[0]->get( parsingContext.window.enableVSync );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWindowFullscreen )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !params.empty() )
		{

			params[0]->get( parsingContext.window.fullscreen );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetScene )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( !params.empty() )
		{
			ScenePtrStrMap::iterator it = parsingContext.mapScenes.find( params[0]->get( name ) );

			if ( it != parsingContext.mapScenes.end() )
			{
				parsingContext.renderTarget->setScene( it->second );
			}
			else
			{
				CU_ParsingError( cuT( "No scene found with name : [" ) + name + cuT( "]." ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetCamera )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.renderTarget->getScene() )
			{
				parsingContext.renderTarget->setCamera( parsingContext.renderTarget->getScene()->getCameraCache().find( params[0]->get( name ) ).lock() );
			}
			else
			{
				CU_ParsingError( cuT( "No scene initialised for this window, set scene before camera." ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !params.empty() )
		{
			params[0]->get( parsingContext.size );

			if ( parsingContext.pixelFormat != castor::PixelFormat::eUNDEFINED )
			{
				parsingContext.renderTarget = parsingContext.parser->getEngine()->getRenderTargetCache().add( parsingContext.targetType
					, parsingContext.size
					, parsingContext.pixelFormat );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetFormat )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !params.empty() )
		{
			params[0]->get( parsingContext.pixelFormat );

			if ( parsingContext.pixelFormat < PixelFormat::eD16_UNORM )
			{
				if ( parsingContext.size != castor::Size{ 1u, 1u } )
				{
					parsingContext.renderTarget = parsingContext.parser->getEngine()->getRenderTargetCache().add( parsingContext.targetType
						, parsingContext.size
						, parsingContext.pixelFormat );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Wrong format for colour" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetStereo )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( !params.empty() )
		{
			float rIntraOcularDistance;
			params[0]->get( rIntraOcularDistance );

			if ( rIntraOcularDistance > 0 )
			{
				//l_parsingContext->renderTarget->setStereo( true );
				//l_parsingContext->renderTarget->setIntraOcularDistance( rIntraOcularDistance );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetPostEffect )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			Parameters parameters;

			if ( params.size() > 1 )
			{
				String tmp;
				parameters.parse( params[1]->get( tmp ) );
			}

			Engine * engine = parsingContext.parser->getEngine();

			if ( !engine->getPostEffectFactory().isTypeRegistered( string::lowerCase( name ) ) )
			{
				CU_ParsingError( cuT( "PostEffect [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				PostEffectSPtr effect = engine->getPostEffectFactory().create( name, *parsingContext.renderTarget, *engine->getRenderSystem(), parameters );
				parsingContext.renderTarget->addPostEffect( effect );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetToneMapping )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			params[0]->get( name );
			Parameters parameters;

			if ( params.size() > 1 )
			{
				String tmp;
				parameters.parse( params[1]->get( tmp ) );
			}

			parsingContext.renderTarget->setToneMappingType( name, parameters );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetSsao )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSsao )

	CU_ImplementAttributeParser( parserRenderTargetEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( parsingContext.renderTarget->getTargetType() == TargetType::eTexture )
		{
			parsingContext.textureUnit->setRenderTarget( parsingContext.renderTarget );
		}
		else
		{
			parsingContext.window.renderTarget = parsingContext.renderTarget;
		}

		parsingContext.renderTarget.reset();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSamplerMinFilter )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			sampler->setMinFilter( VkFilter( uiMode ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMagFilter )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			sampler->setMagFilter( VkFilter( uiMode ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMipFilter )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			sampler->setMipFilter( VkSamplerMipmapMode( uiMode ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMinLod )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			float rValue = -1000;
			params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				sampler->setMinLod( rValue );
			}
			else
			{
				CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMaxLod )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			float rValue = 1000;
			params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				sampler->setMaxLod( rValue );
			}
			else
			{
				CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerLodBias )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			float rValue = 1000;
			params[0]->get( rValue );

			if ( rValue >= -1000 && rValue <= 1000 )
			{
				sampler->setLodBias( rValue );
			}
			else
			{
				CU_ParsingError( cuT( "LOD out of bounds [-1000,1000] : " ) + string::toString( rValue ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerUWrapMode )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			sampler->setWrapS( VkSamplerAddressMode( uiMode ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerVWrapMode )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			sampler->setWrapT( VkSamplerAddressMode( uiMode ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerWWrapMode )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			sampler->setWrapR( VkSamplerAddressMode( uiMode ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerBorderColour )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t colour;
			params[0]->get( colour );
			sampler->setBorderColour( VkBorderColor( colour ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerAnisotropicFiltering )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			bool value;
			params[0]->get( value );
			sampler->enableAnisotropicFiltering( value );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerMaxAnisotropy )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
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

	CU_ImplementAttributeParser( parserSamplerComparisonMode )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t mode;
			params[0]->get( mode );
			sampler->enableCompare( bool( mode ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerComparisonFunc )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( auto sampler = parsingContext.sampler.lock() )
		{
			uint32_t uiMode;
			params[0]->get( uiMode );
			sampler->setCompareOp( VkCompareOp( uiMode ) );
		}
		else
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSamplerEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto sampler = parsingContext.sampler.lock();

		if ( !parsingContext.ownSampler
			&& !sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else
		{
			if ( parsingContext.ownSampler )
			{
				parsingContext.parser->getEngine()->getSamplerCache().add( parsingContext.ownSampler->getName()
					, parsingContext.ownSampler
					, true );
			}

			parsingContext.sampler.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSceneBkColour )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			RgbColour clrBackground;
			params[0]->get( clrBackground );
			parsingContext.scene->setBackgroundColour( clrBackground );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBkImage )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			auto imgBackground = std::make_shared< ImageBackground >( *parsingContext.parser->getEngine()
				, *parsingContext.scene );
			Path path;
			imgBackground->loadImage( context.file.getPath(), params[0]->get( path ) );
			parsingContext.scene->setBackground( imgBackground );
			parsingContext.skybox.reset();
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneFont )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			parsingContext.path.clear();
			params[0]->get( parsingContext.strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eFont )

	CU_ImplementAttributeParser( parserSceneCamera )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.viewport.reset();
		parsingContext.point2f = { 2.2f, 1.0f };
		parsingContext.sceneNode = parsingContext.scene->getCameraRootNode();

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( parsingContext.strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eCamera )

	CU_ImplementAttributeParser( parserSceneLight )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.light.reset();
		parsingContext.strName.clear();
		parsingContext.sceneNode = parsingContext.scene->getObjectRootNode();

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( parsingContext.strName );
		}
	}
	CU_EndAttributePush( CSCNSection::eLight )

	CU_ImplementAttributeParser( parserSceneCameraNode )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			parsingContext.sceneNode = std::make_shared< SceneNode >( name
				, *parsingContext.scene );
			parsingContext.sceneNode->attachTo( *parsingContext.scene->getCameraRootNode() );
		}
	}
	CU_EndAttributePush( CSCNSection::eNode )

	CU_ImplementAttributeParser( parserSceneSceneNode )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			parsingContext.sceneNode = std::make_shared< SceneNode >( name
				, *parsingContext.scene );
			parsingContext.sceneNode->attachTo( *parsingContext.scene->getObjectRootNode() );
		}
	}
	CU_EndAttributePush( CSCNSection::eNode )

	CU_ImplementAttributeParser( parserSceneObject )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.geometry.reset();
		String name;

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( name );
			parsingContext.geometry = std::make_shared< Geometry >( name
				, *parsingContext.scene );
		}
	}
	CU_EndAttributePush( CSCNSection::eObject )

	CU_ImplementAttributeParser( parserSceneAmbientLight )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			RgbColour clColour;
			params[0]->get( clColour );
			parsingContext.scene->setAmbientLight( clColour );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImport )
	{
		//auto & parsingContext = static_cast< SceneFileContext & >( context );
		//Path path;
		//Path pathFile = context.file.getPath() / params[0]->get( path );

		//Engine * engine = parsingContext.parser->getEngine();
		//auto extension = string::lowerCase( pathFile.getExtension() );

		//if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
		//{
		//	CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
		//}
		//else
		//{
		//	auto importer = engine->getImporterFactory().create( extension, *engine );
		//	parsingContext.scene->importExternal( pathFile, *importer );
		//}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBillboard )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			parsingContext.billboards = std::make_shared< BillboardList >( name, *parsingContext.scene );
		}
	}
	CU_EndAttributePush( CSCNSection::eBillboard )

	CU_ImplementAttributeParser( parserSceneAnimatedObjectGroup )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			parsingContext.animGroup = parsingContext.scene->getAnimatedObjectGroupCache().add( name, *parsingContext.scene ).lock();
		}
	}
	CU_EndAttributePush( CSCNSection::eAnimGroup )

	CU_ImplementAttributeParser( parserScenePanelOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			parsingContext.parentOverlays.push_back( parsingContext.overlay );
			auto parent = parsingContext.parentOverlays.back().get();
			parsingContext.overlay = parsingContext.scene->getOverlayView().tryFind( params[0]->get( name ) ).lock();

			if ( !parsingContext.overlay )
			{
				parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::ePanel
					, parsingContext.scene.get()
					, parent );
				parsingContext.overlay->rename( name );
			}

			parsingContext.overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserSceneBorderPanelOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			parsingContext.parentOverlays.push_back( parsingContext.overlay );
			auto parent = parsingContext.parentOverlays.back().get();
			parsingContext.overlay = parsingContext.scene->getOverlayView().tryFind( params[0]->get( name ) ).lock();

			if ( !parsingContext.overlay )
			{
				parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::eBorderPanel
					, parsingContext.scene.get()
					, parent );
				parsingContext.overlay->rename( name );
			}

			parsingContext.overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserSceneTextOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			parsingContext.parentOverlays.push_back( parsingContext.overlay );
			auto parent = parsingContext.parentOverlays.back().get();
			parsingContext.overlay = parsingContext.scene->getOverlayView().tryFind( params[0]->get( name ) ).lock();

			if ( !parsingContext.overlay )
			{
				parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::eText
					, parsingContext.scene.get()
					, parent );
				parsingContext.overlay->rename( name );
			}

			parsingContext.overlay->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserSceneSkybox )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext.skybox = std::make_shared< SkyboxBackground >( *parsingContext.parser->getEngine()
					, *parsingContext.scene );
		}
	}
	CU_EndAttributePush( CSCNSection::eSkybox )

	CU_ImplementAttributeParser( parserSceneFogType )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext.scene->getFog().setType( FogType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneFogDensity )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.scene->getFog().setDensity( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneParticleSystem )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			params[0]->get( value );
			parsingContext.strName = value;
			parsingContext.particleCount = 0u;
			parsingContext.sceneNode = parsingContext.scene->getObjectRootNode();
			parsingContext.material = {};
		}
	}
	CU_EndAttributePush( CSCNSection::eParticleSystem )

	CU_ImplementAttributeParser( parserVoxelConeTracing )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eVoxelConeTracing )

	CU_ImplementAttributeParser( parserSceneEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			if ( parsingContext.scene->getName() == LoadingScreen::SceneName )
			{
				parsingContext.parser->getEngine()->setLoadingScene( std::move( parsingContext.scene ) );
			}
			else
			{
				parsingContext.parser->getEngine()->getSceneCache().add( parsingContext.scene->getName()
					, parsingContext.scene
					, true );
				parsingContext.scene.reset();
			}
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMesh )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;
		params[0]->get( name );

		if ( parsingContext.scene )
		{
			parsingContext.mesh = parsingContext.scene->getMeshCache().tryFind( name );

			if ( !parsingContext.mesh.lock() )
			{
				parsingContext.ownMesh = parsingContext.scene->getMeshCache().create( name
					, *parsingContext.scene );
				parsingContext.mesh = parsingContext.ownMesh;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No scene initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eMesh )

	CU_ImplementAttributeParser( parserDirectionalShadowCascades )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext.scene->setDirectionalShadowCascades( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLpvIndirectAttenuation )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else
		{
			float value{ 0u };
			params[0]->get( value );
			parsingContext.scene->setLpvIndirectAttenuation( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemParent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & cache = parsingContext.scene->getSceneNodeCache();
			String value;
			params[0]->get( value );

			if ( auto node = cache.tryFind( value ).lock() )
			{
				parsingContext.sceneNode = node;
			}
			else
			{
				CU_ParsingError( cuT( "No scene node named " ) + value );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemCount )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext.particleCount = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & cache = parsingContext.parser->getEngine()->getMaterialCache();
			String name;
			params[0]->get( name );
			auto material = cache.tryFind( name ).lock().get();

			if ( material )
			{
				parsingContext.material = material;
			}
			else
			{
				CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

		CU_ImplementAttributeParser( parserParticleSystemDimensions )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.point2f );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemParticle )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		
		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( parsingContext.particleCount == 0 )
		{
			CU_ParsingError( cuT( "particles_count has not been specified." ) );
		}
		else if ( parsingContext.point2f[0] == 0 || parsingContext.point2f[1] == 0 )
		{
			CU_ParsingError( cuT( "one component of the particles dimensions is 0." ) );
		}
		else
		{
			if ( !parsingContext.material )
			{
				parsingContext.material = parsingContext.parser->getEngine()->getMaterialCache().getDefaultMaterial();
			}

			parsingContext.particleSystem = parsingContext.scene->getParticleSystemCache().tryFind( parsingContext.strName ).lock();

			if ( !parsingContext.particleSystem )
			{
				parsingContext.particleSystem = std::make_shared< ParticleSystem >( parsingContext.strName
					, *parsingContext.scene
					, *parsingContext.sceneNode
					, parsingContext.particleCount );
			}

			parsingContext.particleSystem->setMaterial( parsingContext.material );
			parsingContext.particleSystem->setDimensions( parsingContext.point2f );
		}
	}
	CU_EndAttributePush( CSCNSection::eParticle )

	CU_ImplementAttributeParser( parserParticleSystemCSShader )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.shaderProgram.reset();
		parsingContext.shaderStage = VkShaderStageFlagBits( 0u );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext.shaderProgram = parsingContext.parser->getEngine()->getShaderProgramCache().getNewProgram( parsingContext.strName, true );
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderProgram )

	CU_ImplementAttributeParser( parserParticleSystemEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.shaderProgram.reset();
		parsingContext.shaderStage = VkShaderStageFlagBits( 0u );
		
		if ( !parsingContext.particleSystem )
		{
			CU_ParsingError( cuT( "No particle system initialised." ) );
		}
		else
		{
			parsingContext.scene->getParticleSystemCache().add( parsingContext.particleSystem->getName()
				, parsingContext.particleSystem
				, true );
			parsingContext.particleSystem.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserParticleType )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.particleSystem )
		{
			CU_ParsingError( cuT( "No particle system initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String value;
			params[0]->get( value );
			Engine * engine = parsingContext.parser->getEngine();

			if ( !engine->getParticleFactory().isTypeRegistered( string::lowerCase( value ) ) )
			{
				CU_ParsingError( cuT( "Particle type [" ) + value + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				parsingContext.particleSystem->setParticleType( value );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleVariable )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.particleSystem )
		{
			CU_ParsingError( cuT( "No particle system initialised." ) );
		}
		else if ( params.size() < 2 )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			String name;
			uint32_t type;
			String value;
			params[0]->get( name );
			params[1]->get( type );

			if ( params.size() > 2 )
			{
				params[2]->get( value );
			}

			parsingContext.particleSystem->addParticleVariable( name, ParticleFormat( type ), value );
		}
	}
	CU_EndAttributePush( CSCNSection::eUBOVariable )

	CU_ImplementAttributeParser( parserLightParent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;

			if ( auto parent = parsingContext.scene->getSceneNodeCache().find( params[0]->get( name ) ).lock() )
			{
				parsingContext.sceneNode = parent;

				if ( parsingContext.light )
				{
					parsingContext.light->detach();
					parsingContext.sceneNode->attachObject( *parsingContext.light );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else
		{
			parsingContext.scene->getLightCache().add( parsingContext.light->getName()
				, parsingContext.light
				, true );
			parsingContext.light.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserLightType )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiType;
			params[0]->get( uiType );
			parsingContext.lightType = LightType( uiType );
			auto & cache = parsingContext.scene->getLightCache();
			parsingContext.light = cache.tryFind( parsingContext.strName ).lock();

			if ( !parsingContext.light )
			{
				parsingContext.light = std::make_shared< Light >( parsingContext.strName
					, *parsingContext.scene
					, *parsingContext.sceneNode
					, parsingContext.scene->getLightsFactory()
					, parsingContext.lightType );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightColour )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext.light->setColour( value.ptr() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightIntensity )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			Point2f value;
			params[0]->get( value );
			parsingContext.light->setIntensity( value.ptr() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightAttenuation )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );

			if ( parsingContext.lightType == LightType::ePoint )
			{
				parsingContext.light->getPointLight()->setAttenuation( value );
			}
			else if ( parsingContext.lightType == LightType::eSpot )
			{
				parsingContext.light->getSpotLight()->setAttenuation( value );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong type of light to apply attenuation components, needs spotlight or pointlight" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightCutOff )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			float fFloat;
			params[0]->get( fFloat );

			if ( parsingContext.lightType == LightType::eSpot )
			{
				parsingContext.light->getSpotLight()->setCutOff( Angle::fromDegrees( fFloat ) );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightExponent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			float fFloat;
			params[0]->get( fFloat );

			if ( parsingContext.lightType == LightType::eSpot )
			{
				parsingContext.light->getSpotLight()->setExponent( fFloat );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong type of light to apply an exponent, needs spotlight" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightShadows )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eShadows )

	CU_ImplementAttributeParser( parserLightShadowProducer )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext.light->setShadowProducer( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsProducer )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext.light->setShadowProducer( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsFilter )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext.light->setShadowType( ShadowType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsGlobalIllumination )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext.light->setGlobalIlluminationType( GlobalIlluminationType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVolumetricSteps )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext.light->setVolumetricSteps( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVolumetricScatteringFactor )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			float value;
			params[0]->get( value );
			parsingContext.light->setVolumetricScatteringFactor( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsRawConfig )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eRaw )

	CU_ImplementAttributeParser( parserShadowsPcfConfig )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::ePcf )

	CU_ImplementAttributeParser( parserShadowsVsmConfig )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eVsm )

	CU_ImplementAttributeParser( parserShadowsRsmConfig )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eRsm )

	CU_ImplementAttributeParser( parserShadowsLpvConfig )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eLpv )

	CU_ImplementAttributeParser( parserShadowsRawMinOffset )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.light->setRawMinOffset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsRawMaxSlopeOffset )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.light->setRawMaxSlopeOffset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsPcfMinOffset )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.light->setPcfMinOffset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsPcfMaxSlopeOffset )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.light->setPcfMaxSlopeOffset( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVsmVarianceMax )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.light->setVsmMaxVariance( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVsmVarianceBias )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.light->setVsmVarianceBias( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRsmIntensity )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else
		{
			float value{ 0.0f };
			params[0]->get( value );
			parsingContext.light->getRsmConfig().intensity = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRsmMaxRadius )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else
		{
			float value{ 0.0f };
			params[0]->get( value );
			parsingContext.light->getRsmConfig().maxRadius = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRsmSampleCount )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else
		{
			uint32_t value{ 0u };
			params[0]->get( value );
			auto range = parsingContext.light->getRsmConfig().sampleCount.value();
			range = value;
			parsingContext.light->getRsmConfig().sampleCount = range;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLpvTexelAreaModifier )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else
		{
			float value{ 0u };
			params[0]->get( value );
			parsingContext.light->getLpvConfig().texelAreaModifier = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeParent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			SceneNodeSPtr parent;

			if ( name == Scene::ObjectRootNode )
			{
				parent = parsingContext.scene->getObjectRootNode();
			}
			else if ( name == Scene::CameraRootNode )
			{
				parent = parsingContext.scene->getCameraRootNode();
			}
			else if ( name == Scene::RootNode )
			{
				parent = parsingContext.scene->getRootNode();
			}
			else
			{
				parent = parsingContext.scene->getSceneNodeCache().find( name ).lock();
			}

			if ( parent )
			{
				parsingContext.sceneNode->attachTo( *parent );
			}
			else
			{
				CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodePosition )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext.sceneNode->setPosition( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeOrientation )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f axis;
			float angle;
			params[0]->get( axis );
			params[1]->get( angle );
			parsingContext.sceneNode->setOrientation( Quaternion::fromAxisAngle( axis, Angle::fromDegrees( angle ) ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeDirection )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f direction;
			params[0]->get( direction );
			Point3f up{ 0, 1, 0 };
			Point3f right{ point::cross( direction, up ) };
			parsingContext.sceneNode->setOrientation( Quaternion::fromAxes( right, up, direction ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeScale )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext.sceneNode->setScale( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.sceneNode )
		{
			CU_ParsingError( cuT( "No Scene node initialised." ) );
		}
		else
		{
			parsingContext.scene->getSceneNodeCache().add( parsingContext.sceneNode->getName()
				, parsingContext.sceneNode );
			parsingContext.sceneNode.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserObjectParent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			params[0]->get( name );
			SceneNodeSPtr parent;

			if ( name == Scene::ObjectRootNode )
			{
				parent = parsingContext.scene->getObjectRootNode();
			}
			else if ( name == Scene::CameraRootNode )
			{
				parent = parsingContext.scene->getCameraRootNode();
			}
			else if ( name == Scene::RootNode )
			{
				parent = parsingContext.scene->getRootNode();
			}
			else
			{
				parent = parsingContext.scene->getSceneNodeCache().find( name ).lock();
			}

			if ( parent )
			{
				parent->attachObject( *parsingContext.geometry );
			}
			else
			{
				CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "Geometry not initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.geometry->getMesh().lock() )
			{
				auto & cache = parsingContext.parser->getEngine()->getMaterialCache();
				String name;
				auto material = cache.tryFind( params[0]->get( name ) ).lock().get();

				if ( material )
				{
					for ( auto submesh : *parsingContext.geometry->getMesh().lock() )
					{
						parsingContext.geometry->setMaterial( *submesh, material );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Geometry's mesh not initialised" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectMaterials )
	{
	}
	CU_EndAttributePush( CSCNSection::eObjectMaterials )

	CU_ImplementAttributeParser( parserObjectCastShadows )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext.geometry->setShadowCaster( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectReceivesShadows )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext.geometry->setShadowReceiver( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.scene->getGeometryCache().add( std::move( parsingContext.geometry ) );
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserObjectMaterialsMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.geometry->getMesh().lock() )
			{
				auto & cache = parsingContext.parser->getEngine()->getMaterialCache();
				String name;
				uint16_t index;
				auto material = cache.tryFind( params[1]->get( name ) ).lock().get();

				if ( material )
				{
					if ( parsingContext.geometry->getMesh().lock()->getSubmeshCount() > params[0]->get( index ) )
					{
						SubmeshSPtr submesh = parsingContext.geometry->getMesh().lock()->getSubmesh( index );
						parsingContext.geometry->setMaterial( *submesh, material );
					}
					else
					{
						CU_ParsingError( cuT( "Submesh index is too high" ) );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Geometry's mesh not initialised" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectMaterialsEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMeshType )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String type;
		params[0]->get( type );

		Parameters parameters;

		if ( params.size() > 1 )
		{
			String tmp;
			parameters.parse( params[1]->get( tmp ) );
		}

		if ( parsingContext.scene )
		{
			auto & factory = parsingContext.scene->getEngine()->getMeshFactory();
			factory.create( type )->generate( *parsingContext.mesh.lock(), parameters );
		}
		else
		{
			CU_ParsingError( cuT( "No scene initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshSubmesh )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.face1 = -1;
		parsingContext.face2 = -1;
		parsingContext.submesh.reset();

		if ( !parsingContext.mesh.lock() )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else
		{
			parsingContext.submesh = parsingContext.mesh.lock()->createSubmesh();
		}
	}
	CU_EndAttributePush( CSCNSection::eSubmesh )

	CU_ImplementAttributeParser( parserMeshImport )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			Path path;
			Path pathFile = context.file.getPath() / params[0]->get( path );
			Parameters parameters;

			if ( params.size() > 1 )
			{
				String meshParams;
				params[1]->get( meshParams );
				fillMeshImportParameters( context, meshParams, parameters );
			}

			Engine * engine = parsingContext.parser->getEngine();
			auto extension = string::lowerCase( pathFile.getExtension() );

			if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
			{
				CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto importer = engine->getImporterFactory().create( extension, *engine );

				if ( !importer->import( *parsingContext.mesh.lock(), pathFile, parameters, true ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
					parsingContext.mesh.reset();
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshMorphImport )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.mesh.lock() )
		{
			CU_ParsingError( cuT( "No mesh initialised." ) );
		}
		else
		{
			float timeIndex;
			params[1]->get( timeIndex );
			Path path;
			Path pathFile = context.file.getPath() / params[0]->get( path );
			Parameters parameters;
			parameters.add( "no_optimisations", true );

			if ( params.size() > 2 )
			{
				String meshParams;
				params[2]->get( meshParams );
				fillMeshImportParameters( context, meshParams, parameters );
			}

			Engine * engine = parsingContext.parser->getEngine();
			auto extension = string::lowerCase( pathFile.getExtension() );

			if ( !engine->getImporterFactory().isTypeRegistered( extension ) )
			{
				CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto importer = engine->getImporterFactory().create( extension, *engine );
				Mesh mesh{ cuT( "MorphImport" ), *parsingContext.scene };

				if ( !importer->import( mesh, pathFile, parameters, false ) )
				{
					CU_ParsingError( cuT( "Mesh Import failed" ) );
				}
				else if ( mesh.getSubmeshCount() == parsingContext.mesh.lock()->getSubmeshCount() )
				{
					String animName{ "Morph" };

					if ( !parsingContext.mesh.lock()->hasAnimation( animName ) )
					{
						auto & animation = parsingContext.mesh.lock()->createAnimation( animName );

						for ( auto submesh : *parsingContext.mesh.lock() )
						{
							animation.addChild( MeshAnimationSubmesh{ animation, *submesh } );
						}
					}

					MeshAnimation & animation{ static_cast< MeshAnimation & >( parsingContext.mesh.lock()->getAnimation( animName ) ) };
					uint32_t index = 0u;
					MeshAnimationKeyFrameUPtr keyFrame = std::make_unique< MeshAnimationKeyFrame >( animation
						, Milliseconds{ int64_t( timeIndex * 1000ll ) } );

					for ( auto & submesh : mesh )
					{
						auto & submeshAnim = animation.getSubmesh( index );
						std::clog << "Source: " << submeshAnim.getSubmesh().getPointsCount() << " - Anim: " << submesh->getPointsCount() << std::endl;


						if ( submesh->getPointsCount() == submeshAnim.getSubmesh().getPointsCount() )
						{
							keyFrame->addSubmeshBuffer( *submesh, submesh->getPoints() );
						}

						++index;
					}

					mesh.cleanup();
					animation.addKeyFrame( std::move( keyFrame ) );
				}
				else
				{
					CU_ParsingError( cuT( "The new mesh doesn't match the original mesh" ) );
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshDivide )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		Engine * engine = parsingContext.parser->getEngine();

		if ( !parsingContext.mesh.lock() )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String name;
			uint16_t count;
			params[0]->get( name );
			params[1]->get( count );

			if ( !engine->getSubdividerFactory().isTypeRegistered( string::lowerCase( name ) ) )
			{
				CU_ParsingError( cuT( "Subdivider [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else if ( auto mesh = parsingContext.mesh.lock() )
			{
				auto divider = engine->getSubdividerFactory().create( name );
				mesh->computeContainers();

				for ( auto submesh : *mesh )
				{
					divider->subdivide( submesh, count, false );
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshDefaultMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.mesh.lock() )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			auto & cache = parsingContext.parser->getEngine()->getMaterialCache();
			String name;
			auto material = cache.find( params[0]->get( name ) ).lock().get();

			if ( material )
			{
				for ( auto submesh : *parsingContext.mesh.lock() )
				{
					submesh->setDefaultMaterial( material );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshDefaultMaterials )
	{
	}
	CU_EndAttributePush( CSCNSection::eMeshDefaultMaterials )

	CU_ImplementAttributeParser( parserMeshEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Scene initialised." ) );
		}
		else if ( !parsingContext.mesh.lock() )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else
		{
			if ( parsingContext.ownMesh )
			{
				parsingContext.scene->getMeshCache().add( parsingContext.mesh.lock()->getName()
					, parsingContext.ownMesh
					, true );
			}

			if ( parsingContext.geometry )
			{
				parsingContext.geometry->setMesh( parsingContext.mesh );
			}

			parsingContext.mesh.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMeshDefaultMaterialsMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.mesh.lock() )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			auto & cache = parsingContext.parser->getEngine()->getMaterialCache();
			String name;
			uint16_t index;
			auto material = cache.find( params[1]->get( name ) ).lock().get();

			if ( material )
			{
				if ( parsingContext.mesh.lock()->getSubmeshCount() > params[0]->get( index ) )
				{
					SubmeshSPtr submesh = parsingContext.mesh.lock()->getSubmesh( index );
					submesh->setDefaultMaterial( material );
				}
				else
				{
					CU_ParsingError( cuT( "Submesh index is too high" ) );
				}
			}
			else
			{
				CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshDefaultMaterialsEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSubmeshVertex )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext.vertexPos.push_back( value[0] );
			parsingContext.vertexPos.push_back( value[1] );
			parsingContext.vertexPos.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshUV )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point2f value;
			params[0]->get( value );
			parsingContext.vertexTex.push_back( value[0] );
			parsingContext.vertexTex.push_back( value[1] );
			parsingContext.vertexTex.push_back( 0.0 );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshUVW )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext.vertexTex.push_back( value[0] );
			parsingContext.vertexTex.push_back( value[1] );
			parsingContext.vertexTex.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshNormal )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext.vertexNml.push_back( value[0] );
			parsingContext.vertexNml.push_back( value[1] );
			parsingContext.vertexNml.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshTangent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			Point3f value;
			params[0]->get( value );
			parsingContext.vertexTan.push_back( value[0] );
			parsingContext.vertexTan.push_back( value[1] );
			parsingContext.vertexTan.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFace )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ) );
			parsingContext.face1 = -1;
			parsingContext.face2 = -1;

			if ( arrayValues.size() >= 4 )
			{
				Point4i pt4Indices;

				if ( castor::parseValues( *parsingContext.logger, strParams, pt4Indices ) )
				{
					parsingContext.face1 = int( parsingContext.faces.size() );
					parsingContext.faces.push_back( uint32_t( pt4Indices[0] ) );
					parsingContext.faces.push_back( uint32_t( pt4Indices[1] ) );
					parsingContext.faces.push_back( uint32_t( pt4Indices[2] ) );
					parsingContext.face2 = int( parsingContext.faces.size() );
					parsingContext.faces.push_back( uint32_t( pt4Indices[0] ) );
					parsingContext.faces.push_back( uint32_t( pt4Indices[2] ) );
					parsingContext.faces.push_back( uint32_t( pt4Indices[3] ) );
				}
			}
			else if ( castor::parseValues( *parsingContext.logger, strParams, pt3Indices ) )
			{
				parsingContext.face1 = int( parsingContext.faces.size() );
				parsingContext.faces.push_back( uint32_t( pt3Indices[0] ) );
				parsingContext.faces.push_back( uint32_t( pt3Indices[1] ) );
				parsingContext.faces.push_back( uint32_t( pt3Indices[2] ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceUV )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext.submesh;

			if ( parsingContext.vertexTex.empty() )
			{
				parsingContext.vertexTex.resize( parsingContext.vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 6 && parsingContext.face1 != -1 )
			{
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[0] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[1] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[2] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[3] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[4] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[5] );
			}

			if ( arrayValues.size() >= 8 && parsingContext.face2 != -1 )
			{
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[0] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[1] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = string::toFloat( arrayValues[4] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = string::toFloat( arrayValues[5] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[6] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[7] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceUVW )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext.submesh;

			if ( parsingContext.vertexTex.empty() )
			{
				parsingContext.vertexTex.resize( parsingContext.vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext.face1 != -1 )
			{
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[0] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[1] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[2] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[3] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[4] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[5] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[6] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[7] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext.face2 != -1 )
			{
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[0] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[1] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[2] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[6] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[7] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[8] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[ 9] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[10] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceNormals )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext.submesh;

			if ( parsingContext.vertexNml.empty() )
			{
				parsingContext.vertexNml.resize( parsingContext.vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext.face1 != -1 )
			{
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[0] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[1] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[2] );
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[3] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[4] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[5] );
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[6] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[7] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext.face2 != -1 )
			{
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[ 0] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[ 1] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[ 2] );
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = string::toFloat( arrayValues[ 6] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = string::toFloat( arrayValues[ 7] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = string::toFloat( arrayValues[ 8] );
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[ 9] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[10] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceTangents )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			String strParams;
			params[0]->get( strParams );
			SubmeshSPtr submesh = parsingContext.submesh;

			if ( parsingContext.vertexTan.empty() )
			{
				parsingContext.vertexTan.resize( parsingContext.vertexPos.size() );
			}

			Point3i pt3Indices;
			StringArray arrayValues = string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext.face1 != -1 )
			{
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[0] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[1] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = string::toFloat( arrayValues[2] );
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[3] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[4] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = string::toFloat( arrayValues[5] );
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[6] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[7] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext.face2 != -1 )
			{
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[ 0] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[ 1] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = string::toFloat( arrayValues[ 2] );
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = string::toFloat( arrayValues[ 6] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = string::toFloat( arrayValues[ 7] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = string::toFloat( arrayValues[ 8] );
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[ 9] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[10] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.vertexPos.empty() )
		{
			std::vector< InterleavedVertex > vertices{ parsingContext.vertexPos.size() / 3 };
			uint32_t index{ 0u };

			for ( auto & vertex : vertices )
			{
				std::memcpy( vertex.pos.ptr(), &parsingContext.vertexPos[index], sizeof( vertex.pos ) );

				if ( !parsingContext.vertexNml.empty() )
				{
					std::memcpy( vertex.nml.ptr(), &parsingContext.vertexNml[index], sizeof( vertex.nml ) );
				}

				if ( !parsingContext.vertexTan.empty() )
				{
					std::memcpy( vertex.tan.ptr(), &parsingContext.vertexTan[index], sizeof( vertex.tan ) );
				}

				if ( !parsingContext.vertexTex.empty() )
				{
					std::memcpy( vertex.tex.ptr(), &parsingContext.vertexTex[index], sizeof( vertex.tex ) );
				}

				index += 3;
			}

			parsingContext.submesh->addPoints( vertices );

			if ( !parsingContext.faces.empty() )
			{
				auto indices = reinterpret_cast< FaceIndices * >( &parsingContext.faces[0] );
				auto mapping = std::make_shared< TriFaceMapping >( *parsingContext.submesh );
				mapping->addFaceGroup( indices, indices + ( parsingContext.faces.size() / 3 ) );

				if ( !parsingContext.vertexNml.empty() )
				{
					if ( parsingContext.vertexTan.empty() )
					{
						mapping->computeTangentsFromNormals();
					}
				}
				else
				{
					mapping->computeNormals();
				}

				parsingContext.submesh->setIndexMapping( mapping );
			}

			parsingContext.vertexPos.clear();
			parsingContext.vertexNml.clear();
			parsingContext.vertexTan.clear();
			parsingContext.vertexTex.clear();
			parsingContext.faces.clear();
			parsingContext.submesh->getParent().getScene()->getListener().postEvent( makeGpuInitialiseEvent( *parsingContext.submesh ) );
			parsingContext.submesh.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParserBlock( parserMaterialPass, CSCNSection::ePass )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.strName.clear();

		if ( parsingContext.material )
		{
			if ( parsingContext.createMaterial
				|| parsingContext.material->getPassCount() < parsingContext.passIndex )
			{
				parsingContext.pass = parsingContext.material->createPass();
				parsingContext.createPass = true;

			}
			else
			{
				parsingContext.pass = parsingContext.material->getPass( parsingContext.passIndex );
				parsingContext.createPass = false;
			}

			++parsingContext.passIndex;
			parsingContext.unitIndex = 0u;
			parsingContext.createUnit = true;
			sectionName = parsingContext.pass->getPassSectionID();
		}
		else
		{
			CU_ParsingError( cuT( "Material not initialised" ) );
		}
	}
	CU_EndAttributeBlock()

	CU_ImplementAttributeParser( parserMaterialEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.ownMaterial
			&& !parsingContext.material )
		{
			CU_ParsingError( cuT( "Material not initialised" ) );
		}
		else if ( parsingContext.ownMaterial )
		{
			if ( parsingContext.scene )
			{
				parsingContext.scene->getMaterialView().add( parsingContext.material->getName()
					, parsingContext.ownMaterial
					, true );
			}
			else
			{
				parsingContext.parser->getEngine()->getMaterialCache().add( parsingContext.material->getName()
					, parsingContext.ownMaterial
					, true );
			}
		}

		parsingContext.material = {};
		parsingContext.createMaterial = false;
		parsingContext.passIndex = 0u;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserTexTransformRotate )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.textureUnit )
		{
			CU_ParsingError( cuT( "TextureUnit not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.textureTransform.rotate = Angle::fromDegrees( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexTransformTranslate )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.textureUnit )
		{
			CU_ParsingError( cuT( "TextureUnit not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			parsingContext.textureTransform.translate = Point4f{ value->x, value->y, 0.0f, 0.0f };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexTransformScale )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.textureUnit )
		{
			CU_ParsingError( cuT( "TextureUnit not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			parsingContext.textureTransform.scale = Point4f{ value->x, value->y, 1.0f, 0.0f };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexAnimRotate )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.textureAnimation )
		{
			CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.textureAnimation->setRotateSpeed( TextureRotateSpeed{ Angle::fromDegrees( value ) } );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexAnimTranslate )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.textureAnimation )
		{
			CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			parsingContext.textureAnimation->setTranslateSpeed( TextureTranslateSpeed{ value } );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexAnimScale )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.textureAnimation )
		{
			CU_ParsingError( cuT( "TextureAnimation not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			parsingContext.textureAnimation->setScaleSpeed( TextureScaleSpeed{ value } );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVertexShader )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserPixelShader )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserGeometryShader )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserHullShader )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserDomainShader )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserComputeShader )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserConstantsBuffer )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( parsingContext.strName );

			if ( parsingContext.strName.empty() )
			{
				CU_ParsingError( cuT( "Invalid empty name" ) );
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderUBO )

	CU_ImplementAttributeParser( parserShaderEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else
		{
			if ( parsingContext.particleSystem )
			{
				parsingContext.particleSystem->setCSUpdateProgram( parsingContext.shaderProgram );
			}

			parsingContext.shaderProgram.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserShaderProgramFile )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.shaderStage != VkShaderStageFlagBits( 0u ) )
			{
				Path path;
				params[0]->get( path );
				parsingContext.shaderProgram->setFile( parsingContext.shaderStage
					, context.file.getPath() / path );
			}
			else
			{
				CU_ParsingError( cuT( "Shader not initialised" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderGroupSizes )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.shaderStage != VkShaderStageFlagBits( 0u ) )
			{
				Point3i sizes;
				params[0]->get( sizes );
				parsingContext.particleSystem->setCSGroupSizes( sizes );
			}
			else
			{
				CU_ParsingError( cuT( "Shader not initialised" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderUboShaders )
	{
		if ( !params.empty() )
		{
			uint32_t value;
			params[0]->get( value );

			if ( value )
			{
				//auto & parsingContext = static_cast< SceneFileContext & >( context );
				//parsingContext.uniformBuffer = std::make_unique< UniformBuffer >( parsingContext.strName
				//	, *parsingContext.shaderProgram->getRenderSystem()
				//	, 1u );
			}
			else
			{
				CU_ParsingError( cuT( "Unsupported shader type" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderUboVariable )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		params[0]->get( parsingContext.strName2 );

		if ( !parsingContext.uniformBuffer )
		{
			CU_ParsingError( cuT( "Shader constants buffer not initialised" ) );
		}
		else if ( parsingContext.strName2.empty() )
		{
			CU_ParsingError( cuT( "Invalid empty name" ) );
		}
		else
		{
			parsingContext.particleCount = 1;
		}
	}
	CU_EndAttributePush( CSCNSection::eUBOVariable )

	CU_ImplementAttributeParser( parserShaderVariableCount )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		uint32_t param;
		params[0]->get( param );

		if ( parsingContext.uniformBuffer )
		{
			parsingContext.particleCount = param;
		}
		else
		{
			CU_ParsingError( cuT( "Shader constants buffer not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderVariableType )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		uint32_t uiType;
		params[0]->get( uiType );

		if ( parsingContext.uniformBuffer )
		{
			//if ( !parsingContext.uniform )
			//{
			//	parsingContext.uniform = parsingContext.uniformBuffer->createUniform( UniformType( uiType ), parsingContext.strName2, parsingContext.uiUInt32 );
			//}
			//else
			//{
			//	CU_ParsingError( cuT( "Variable type already set" ) );
			//}
		}
		else
		{
			CU_ParsingError( cuT( "Shader constants buffer not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShaderVariableValue )
	{
		//auto & parsingContext = static_cast< SceneFileContext & >( context );
		//String strParams;
		//params[0]->get( strParams );

		//if ( parsingContext.uniform )
		//{
		//	parsingContext.uniform->setStrValue( strParams );
		//}
		//else
		//{
		//	CU_ParsingError( cuT( "Variable not initialised" ) );
		//}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontFile )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		params[0]->get( parsingContext.path );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontHeight )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		params[0]->get( parsingContext.fontHeight );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.strName.empty() && !parsingContext.path.empty() )
		{
			if ( parsingContext.scene )
			{
				parsingContext.scene->getFontView().add( parsingContext.strName
					, uint32_t( parsingContext.fontHeight )
					, context.file.getPath() / parsingContext.path );
			}
			else
			{
				parsingContext.parser->getEngine()->getFontCache().add( parsingContext.strName
					, uint32_t( parsingContext.fontHeight )
					, context.file.getPath() / parsingContext.path );
			}
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserOverlayPosition )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.overlay )
		{
			Point2d ptPosition;
			params[0]->get( ptPosition );
			parsingContext.overlay->setPosition( ptPosition );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlaySize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.overlay )
		{
			Point2d ptSize;
			params[0]->get( ptSize );
			parsingContext.overlay->setSize( ptSize );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPixelSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.overlay )
		{
			Size size;
			params[0]->get( size );
			parsingContext.overlay->setPixelSize( size );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPixelPosition )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.overlay )
		{
			Position position;
			params[0]->get( position );
			parsingContext.overlay->setPixelPosition( position );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.overlay )
		{
			String name;
			params[0]->get( name );
			auto & cache = parsingContext.parser->getEngine()->getMaterialCache();
			parsingContext.overlay->setMaterial( cache.find( name ).lock().get() );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPanelOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;
		parsingContext.parentOverlays.push_back( parsingContext.overlay );
		auto parent = parsingContext.parentOverlays.back().get();
		parsingContext.overlay = parsingContext.parser->getEngine()->getOverlayCache().tryFind( params[0]->get( name ) ).lock();

		if ( !parsingContext.overlay )
		{
			parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
				, OverlayType::ePanel
				, parent ? parent->getScene() : parsingContext.scene.get()
				, parent );
			parsingContext.overlay->rename( name );
		}

		parsingContext.overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserOverlayBorderPanelOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;
		parsingContext.parentOverlays.push_back( parsingContext.overlay );
		auto parent = parsingContext.parentOverlays.back().get();
		parsingContext.overlay = parsingContext.parser->getEngine()->getOverlayCache().tryFind( params[0]->get( name ) ).lock();

		if ( !parsingContext.overlay )
		{
			parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
				, OverlayType::eBorderPanel
				, parent ? parent->getScene() : parsingContext.scene.get()
				, parent );
			parsingContext.overlay->rename( name );
		}

		parsingContext.overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserOverlayTextOverlay )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;
		parsingContext.parentOverlays.push_back( parsingContext.overlay );
		auto parent = parsingContext.parentOverlays.back().get();
		parsingContext.overlay = parsingContext.parser->getEngine()->getOverlayCache().tryFind( params[0]->get( name ) ).lock();

		if ( !parsingContext.overlay )
		{
			parsingContext.overlay = std::make_shared< Overlay >( *parsingContext.parser->getEngine()
				, OverlayType::eText
				, parent ? parent->getScene() : parsingContext.scene.get()
				, parent );
			parsingContext.overlay->rename( name );
		}

		parsingContext.overlay->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserOverlayEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.overlay->getType() == OverlayType::eText )
		{
			auto textOverlay = parsingContext.overlay->getTextOverlay();

			if ( textOverlay->getFontTexture() )
			{
				parsingContext.overlay->setVisible( true );
			}
			else
			{
				parsingContext.overlay->setVisible( false );
				CU_ParsingError( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
			}
		}
		else
		{
			parsingContext.overlay->setVisible( true );
		}

		if ( parsingContext.scene )
		{
			parsingContext.scene->getOverlayView().add( parsingContext.overlay->getName()
				, parsingContext.overlay
				, true );
		}
		else
		{
			parsingContext.parser->getEngine()->getOverlayCache().add( parsingContext.overlay->getName()
				, parsingContext.overlay
				, true );
		}

		CU_Require( !parsingContext.parentOverlays.empty() );
		parsingContext.overlay = parsingContext.parentOverlays.back();
		parsingContext.parentOverlays.pop_back();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserPanelOverlayUvs )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::ePanel )
		{
			Point4d uvs;
			params[0]->get( uvs );
			overlay->getPanelOverlay()->setUV( uvs );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlaySizes )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d ptSize;
			params[0]->get( ptSize );
			overlay->getBorderPanelOverlay()->setBorderSize( ptSize );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayPixelSizes )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			castor::Rectangle size;
			params[0]->get( size );
			overlay->getBorderPanelOverlay()->setBorderPixelSize( size );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			String name;
			params[0]->get( name );
			auto & cache = parsingContext.parser->getEngine()->getMaterialCache();
			overlay->getBorderPanelOverlay()->setBorderMaterial( cache.find( name ).lock().get() );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayPosition )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			uint32_t position;
			params[0]->get( position );
			overlay->getBorderPanelOverlay()->setBorderPosition( BorderPosition( position ) );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayCenterUvs )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			params[0]->get( uvs );
			overlay->getBorderPanelOverlay()->setUV( uvs );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayOuterUvs )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			params[0]->get( uvs );
			overlay->getBorderPanelOverlay()->setBorderOuterUV( uvs );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayInnerUvs )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			Point4d uvs;
			params[0]->get( uvs );
			overlay->getBorderPanelOverlay()->setBorderInnerUV( uvs );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayFont )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			auto & cache = parsingContext.parser->getEngine()->getFontCache();
			String name;
			params[0]->get( name );

			if ( cache.find( name ).lock() )
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

	CU_ImplementAttributeParser( parserTextOverlayTextWrapping )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setTextWrappingMode( TextWrappingMode( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayVerticalAlign )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setVAlign( VAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayHorizontalAlign )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setHAlign( HAlign( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayTexturingMode )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setTexturingMode( TextTexturingMode( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayLineSpacingMode )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			uint32_t value;
			params[0]->get( value );

			overlay->getTextOverlay()->setLineSpacingMode( TextLineSpacingMode( value ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTextOverlayText )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		auto overlay = parsingContext.overlay;
		String strParams;
		params[0]->get( strParams );

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			string::replace( strParams, cuT( "\\n" ), cuT( "\n" ) );
			overlay->getTextOverlay()->setCaption( strParams );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraParent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;
		SceneNodeSPtr parent = parsingContext.scene->getSceneNodeCache().find( params[0]->get( name ) ).lock();

		if ( parent )
		{
			parsingContext.sceneNode = parent;

			while ( parent->getParent()
				&& parent->getParent() != parsingContext.scene->getObjectRootNode().get()
				&& parent->getParent() != parsingContext.scene->getCameraRootNode().get() )
			{
				parent = parent->getParent()->shared_from_this();
			}

			if ( !parent->getParent()
				|| parent->getParent() == parsingContext.scene->getObjectRootNode().get() )
			{
				parent->attachTo( *parsingContext.scene->getCameraRootNode() );
			}
		}
		else
		{
			CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraViewport )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.viewport = std::make_shared< Viewport >( *parsingContext.parser->getEngine() );
		parsingContext.viewport->setPerspective( 0.0_degrees, 1, 0, 1 );
	}
	CU_EndAttributePush( CSCNSection::eViewport )
		
	CU_ImplementAttributeParser( parserCameraHdrConfig )
	{
	}
	CU_EndAttributePush( CSCNSection::eHdrConfig )

	CU_ImplementAttributeParser( parserCameraPrimitive )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !params.empty() )
		{
			uint32_t uiType;
			parsingContext.primitiveType = VkPrimitiveTopology( params[0]->get( uiType ) );
		}
		else
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.sceneNode && parsingContext.viewport )
		{
			auto camera = parsingContext.scene->getCameraCache().add( parsingContext.strName
				, *parsingContext.scene
				, *parsingContext.sceneNode
				, std::move( *parsingContext.viewport ) ).lock();
			camera->setGamma( parsingContext.point2f[0] );
			camera->setExposure( parsingContext.point2f[1] );
			parsingContext.viewport.reset();
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserViewportType )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !params.empty() )
		{
			uint32_t uiType;
			parsingContext.viewport->updateType( ViewportType( params[0]->get( uiType ) ) );
		}
		else
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportLeft )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateLeft( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportRight )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateRight( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportTop )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateTop( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportBottom )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateBottom( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportNear )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateNear( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportFar )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateFar( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		Size size;
		params[0]->get( size );
		parsingContext.viewport->resize( size );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportFovY )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float fFovY;
		params[0]->get( fFovY );
		parsingContext.viewport->updateFovY( Angle::fromDegrees( fFovY ) );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportAspectRatio )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float fRatio;
		params[0]->get( fRatio );
		parsingContext.viewport->updateRatio( fRatio );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardParent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.billboards )
		{
			String name;
			params[0]->get( name );
			SceneNodeSPtr pParent = parsingContext.scene->getSceneNodeCache().find( name ).lock();

			if ( pParent )
			{
				pParent->attachObject( *parsingContext.billboards );
			}
			else
			{
				CU_ParsingError( cuT( "Node " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "Geometry not initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardType )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.billboards )
		{
			CU_ParsingError( cuT( "Billboard not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );

			parsingContext.billboards->setBillboardType( BillboardType( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.billboards )
		{
			CU_ParsingError( cuT( "Billboard not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );

			parsingContext.billboards->setBillboardSize( BillboardSize( value ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardPositions )
	{
	}
	CU_EndAttributePush( CSCNSection::eBillboardList )

	CU_ImplementAttributeParser( parserBillboardMaterial )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.billboards )
		{
			auto & cache = parsingContext.parser->getEngine()->getMaterialCache();
			String name;
			auto material = cache.tryFind( params[0]->get( name ) ).lock().get();

			if ( material )
			{
				parsingContext.billboards->setMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material " ) + name + cuT( " does not exist" ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "Billboard not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardDimensions )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		Point2f dimensions;
		params[0]->get( dimensions );
		parsingContext.billboards->setDimensions( dimensions );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		parsingContext.scene->getBillboardListCache().add( parsingContext.billboards->getName()
			, parsingContext.billboards 
			, true );
		parsingContext.billboards.reset();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserBillboardPoint )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		Point3f position;
		params[0]->get( position );
		parsingContext.billboards->addPoint( position );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimatedObject )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;
		params[0]->get( name );

		if ( parsingContext.animGroup )
		{
			GeometrySPtr geometry = parsingContext.scene->getGeometryCache().find( name ).lock();

			if ( geometry )
			{
				if ( !geometry->getAnimations().empty() )
				{
					parsingContext.animMovable = parsingContext.animGroup->addObject( *geometry
						, geometry->getName() + cuT( "_Movable" ) );
				}

				auto mesh = geometry->getMesh().lock();

				if ( mesh )
				{
					if ( !mesh->getAnimations().empty() )
					{
						parsingContext.animMesh = parsingContext.animGroup->addObject( *mesh
							, *geometry
							, geometry->getName() + cuT( "_Mesh" ) );
					}

					auto skeleton = mesh->getSkeleton();

					if ( skeleton )
					{
						if ( !skeleton->getAnimations().empty() )
						{
							parsingContext.animSkeleton = parsingContext.animGroup->addObject( *skeleton
								, *mesh
								, *geometry
								, geometry->getName() + cuT( "_Skeleton" ) );
						}
					}
				}
			}
			else
			{
				CU_ParsingError( cuT( "No geometry with name " ) + name );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimation )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		params[0]->get( parsingContext.strName2 );

		if ( parsingContext.animGroup )
		{
			parsingContext.animGroup->addAnimation( parsingContext.strName2 );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eAnimation )

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimationStart )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;
		params[0]->get( name );

		if ( parsingContext.animGroup )
		{
			parsingContext.animGroup->startAnimation( name );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimationPause )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		String name;
		params[0]->get( name );

		if ( parsingContext.animGroup )
		{
			parsingContext.animGroup->pauseAnimation( name );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.animGroup )
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}

		parsingContext.animGroup.reset();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserAnimationLooped )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		bool value;
		params[0]->get( value );

		if ( parsingContext.animGroup )
		{
			parsingContext.animGroup->setAnimationLooped( parsingContext.strName2, value );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimationScale )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float value;
		params[0]->get( value );

		if ( parsingContext.animGroup )
		{
			parsingContext.animGroup->setAnimationScale( parsingContext.strName2, value );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimationStartAt )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float value;
		params[0]->get( value );

		if ( parsingContext.animGroup )
		{
			parsingContext.animGroup->setAnimationStartingPoint( parsingContext.strName2
				, castor::Milliseconds{ uint64_t( value * 1000.0f ) } );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimationStopAt )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );
		float value;
		params[0]->get( value );

		if ( parsingContext.animGroup )
		{
			parsingContext.animGroup->setAnimationStoppingPoint( parsingContext.strName2
				, castor::Milliseconds{ uint64_t( value * 1000.0f ) } );
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimationEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSkyboxEqui )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.size() <= 1 )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( !parsingContext.skybox )
		{
			CU_ParsingError( cuT( "No skybox initialised." ) );
		}
		else
		{
			Path path;
			Path filePath = context.file.getPath();
			params[0]->get( path );

			if ( File::fileExists( filePath / path ) )
			{
				uint32_t size;
				params[1]->get( size );
				parsingContext.skybox->loadEquiTexture( filePath, path, size );
			}
			else
			{
				CU_ParsingError( cuT( "Couldn't load the image" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxCross )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.size() < 1 )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( !parsingContext.skybox )
		{
			CU_ParsingError( cuT( "No skybox initialised." ) );
		}
		else
		{
			Path path;
			Path filePath = context.file.getPath();
			params[0]->get( path );

			if ( File::fileExists( filePath / path ) )
			{
				parsingContext.skybox->loadCrossTexture( filePath, path );
			}
			else
			{
				CU_ParsingError( cuT( "Couldn't load the image" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxLeft )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.skybox )
		{
			Path path;
			parsingContext.skybox->loadLeftImage( context.file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxRight )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.skybox )
		{
			Path path;
			parsingContext.skybox->loadRightImage( context.file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxTop )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.skybox )
		{
			Path path;
			parsingContext.skybox->loadTopImage( context.file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxBottom )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.skybox )
		{
			Path path;
			parsingContext.skybox->loadBottomImage( context.file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxFront )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.skybox )
		{
			Path path;
			parsingContext.skybox->loadFrontImage( context.file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxBack )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.skybox )
		{
			Path path;
			parsingContext.skybox->loadBackImage( context.file.getPath(), params[0]->get( path ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.skybox )
		{
			parsingContext.scene->setBackground( std::move( parsingContext.skybox ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSsaoEnabled )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext.ssaoConfig.enabled = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoHighQuality )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext.ssaoConfig.highQuality = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoUseNormalsBuffer )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext.ssaoConfig.useNormalsBuffer = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoRadius )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.ssaoConfig.radius = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoMinRadius )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.ssaoConfig.minRadius = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBias )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.ssaoConfig.bias = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoIntensity )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.ssaoConfig.intensity = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoNumSamples )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext.ssaoConfig.numSamples = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoEdgeSharpness )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.ssaoConfig.edgeSharpness = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBlurStepSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext.ssaoConfig.blurStepSize = { value, parsingContext.ssaoConfig.blurStepSize.value().range() };
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBlurHighQuality )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				bool value;
				params[0]->get( value );
				parsingContext.ssaoConfig.blurHighQuality = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBlurRadius )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext.ssaoConfig.blurRadius = { int32_t( value ), parsingContext.ssaoConfig.blurRadius.value().range() };
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBendStepCount )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				uint32_t value;
				params[0]->get( value );
				parsingContext.ssaoConfig.bendStepCount = int32_t( value );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoBendStepSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			if ( params.empty() )
			{
				CU_ParsingError( cuT( "Missing parameter." ) );
			}
			else
			{
				float value;
				params[0]->get( value );
				parsingContext.ssaoConfig.bendStepSize = value;
			}
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSsaoEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( parsingContext.renderTarget )
		{
			parsingContext.renderTarget->setSsaoConfig( parsingContext.ssaoConfig );
			parsingContext.ssaoConfig = {};
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSubsurfaceScatteringStrength )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.subsurfaceScattering->setStrength( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubsurfaceScatteringGaussianWidth )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.subsurfaceScattering->setGaussianWidth( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubsurfaceScatteringTransmittanceProfile )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eTransmittanceProfile )

	CU_ImplementAttributeParser( parserSubsurfaceScatteringEnd )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else
		{
			parsingContext.pass->setSubsurfaceScattering( std::move( parsingContext.subsurfaceScattering ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserTransmittanceProfileFactor )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.subsurfaceScattering )
		{
			CU_ParsingError( cuT( "No SubsurfaceScattering initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			Point4f value;
			params[0]->get( value );
			parsingContext.subsurfaceScattering->addProfileFactor( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserHdrExponent )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.point2f[1] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserHdrGamma )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.point2f[0] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctEnabled )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			params[0]->get( vctConfig.enabled );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctGridSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			uint32_t value;
			params[0]->get( value );
			vctConfig.gridSize = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctNumCones )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			uint32_t value;
			params[0]->get( value );
			vctConfig.numCones = value;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctMaxDistance )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			params[0]->get( vctConfig.maxDistance );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctRayStepSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			params[0]->get( vctConfig.rayStepSize );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctVoxelSize )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			params[0]->get( vctConfig.voxelSizeFactor );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctConservativeRasterization )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			params[0]->get( vctConfig.enableConservativeRasterization );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctTemporalSmoothing )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			params[0]->get( vctConfig.enableTemporalSmoothing );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctOcclusion )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			params[0]->get( vctConfig.enableOcclusion );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVctSecondaryBounce )
	{
		auto & parsingContext = static_cast< SceneFileContext & >( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			auto & vctConfig = parsingContext.scene->getVoxelConeTracingConfig();
			params[0]->get( vctConfig.enableSecondaryBounce );
		}
	}
	CU_EndAttribute()
}
