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
#include <CastorUtils/FileParser/ParserParameter.hpp>

using castor::operator<<;

namespace castor3d
{
	namespace scnprs
	{
		static void fillMeshImportParameters( castor::FileParserContext & context
			, castor::String const & meshParams
			, Parameters & parameters )
		{
			auto paramArray = castor::string::split( meshParams, cuT( "-" ), 20, false );

			for ( auto param : paramArray )
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
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != castor::String::npos )
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
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != castor::String::npos )
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
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != castor::String::npos )
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
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != castor::String::npos )
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
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != castor::String::npos )
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
					auto eqIndex = param.find( cuT( '=' ) );

					if ( eqIndex != castor::String::npos )
					{
						castor::String value = param.substr( eqIndex + 1 );

						if ( value.size() > 2
							&& value.front() == '\"'
							&& value.back() == '\"' )
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
			}
		}

		static MorphFlags getMorphFlags( SubmeshFlags submeshFlags )
		{
			MorphFlags result{};

			if ( checkFlag( submeshFlags, SubmeshFlag::ePositions ) )
			{
				result |= MorphFlag::ePositions;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
			{
				result |= MorphFlag::eNormals;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
			{
				result |= MorphFlag::eTangents;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) )
			{
				result |= MorphFlag::eTexcoords0;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) )
			{
				result |= MorphFlag::eTexcoords1;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) )
			{
				result |= MorphFlag::eTexcoords2;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) )
			{
				result |= MorphFlag::eTexcoords3;
			}

			if ( checkFlag( submeshFlags, SubmeshFlag::eColours ) )
			{
				result |= MorphFlag::eColours;
			}

			return result;
		}
	}

	SceneFileContext & getParserContext( castor::FileParserContext & context )
	{
		return *static_cast< SceneFileContext * >( context.getUserContext( "c3d.scene" ) );
	}

	CU_ImplementAttributeParser( parserMaterial )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.strName );
			parsingContext.material = parsingContext.parser->getEngine()->tryFindMaterial( parsingContext.strName );
			parsingContext.passIndex = 0u;
			parsingContext.createMaterial = parsingContext.material == nullptr;

			if ( parsingContext.createMaterial )
			{
				parsingContext.ownMaterial = parsingContext.parser->getEngine()->createMaterial( parsingContext.strName
					, *parsingContext.parser->getEngine()
					, parsingContext.parser->getEngine()->getDefaultLightingModel() );
				parsingContext.material = parsingContext.ownMaterial.get();
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eMaterial )
		
	CU_ImplementAttributeParser( parserSamplerState )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			castor::String name;
			parsingContext.sampler = parsingContext.parser->getEngine()->tryFindSampler( params[0]->get( name ) );

			if ( !parsingContext.sampler )
			{
				parsingContext.ownSampler = parsingContext.parser->getEngine()->createSampler( name
					, *parsingContext.parser->getEngine() );
				parsingContext.sampler = parsingContext.ownSampler.get();
			}
		}
	}
	CU_EndAttributePush( CSCNSection::eSampler )

	CU_ImplementAttributeParser( parserRootScene )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			castor::String name;
			params[0]->get( name );
			parsingContext.scene = parsingContext.parser->getEngine()->tryFindScene( name );

			if ( !parsingContext.scene )
			{
				parsingContext.ownScene = castor::makeUnique< Scene >( name
					, *parsingContext.parser->getEngine() );
				parsingContext.scene = parsingContext.ownScene.get();
			}

			parsingContext.mapScenes.insert( std::make_pair( name, parsingContext.scene ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootLoadingScreen )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.ownScene = castor::makeUnique< Scene >( LoadingScreen::SceneName
			, *parsingContext.parser->getEngine() );
		parsingContext.scene = parsingContext.ownScene.get();
	}
	CU_EndAttributePush( CSCNSection::eScene )

	CU_ImplementAttributeParser( parserRootFont )
	{
		auto & parsingContext = getParserContext( context );
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

	CU_ImplementAttributeParser( parserRootPanelOverlay )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			castor::String name;
			parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
			auto & parent = parsingContext.parentOverlays.back();
			parsingContext.overlay.rptr = parsingContext.parser->getEngine()->tryFindOverlay( params[0]->get( name ) );

			if ( !parsingContext.overlay.rptr )
			{
				parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::ePanel
					, nullptr
					, parent.rptr );
				parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
				parsingContext.overlay.rptr->rename( name );
			}

			parsingContext.overlay.rptr->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserRootBorderPanelOverlay )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			castor::String name;
			parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
			auto & parent = parsingContext.parentOverlays.back();
			parsingContext.overlay.rptr = parsingContext.parser->getEngine()->tryFindOverlay( params[0]->get( name ) );

			if ( !parsingContext.overlay.rptr )
			{
				parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::eBorderPanel
					, nullptr
					, parent.rptr );
				parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
				parsingContext.overlay.rptr->rename( name );
			}

			parsingContext.overlay.rptr->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserRootTextOverlay )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			castor::String name;
			parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
			auto & parent = parsingContext.parentOverlays.back();
			parsingContext.overlay.rptr = parsingContext.parser->getEngine()->tryFindOverlay( params[0]->get( name ) );

			if ( !parsingContext.overlay.rptr )
			{
				parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::eText
					, nullptr
					, parent.rptr );
				parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
				parsingContext.overlay.rptr->rename( name );
			}

			parsingContext.overlay.rptr->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserRootDebugOverlays )
	{
		auto & parsingContext = getParserContext( context );

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
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRootWindow )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			parsingContext.inWindow = true;
			params[0]->get( parsingContext.window.name );
		}
	}
	CU_EndAttributePush( CSCNSection::eWindow )

	CU_ImplementAttributeParser( parserRootMaxImageSize )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [count] parameter." ) );
		}
		else
		{
			uint32_t value;
			params[0]->get( value );
			parsingContext.parser->getEngine()->setMaxImageSize( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRootLpvGridSize )
	{
		auto & parsingContext = getParserContext( context );

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

	CU_ImplementAttributeParser( parserRootDefaultUnit )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [unit] parameter." ) );
		}
		else
		{
			parsingContext.parser->getEngine()->setLengthUnit( castor::LengthUnit( params[0]->get< uint32_t >() ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRootFullLoading )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [enable] parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.enableFullLoading );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWindowRenderTarget )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.targetType = TargetType::eWindow;
		parsingContext.size = { 1u, 1u };
		parsingContext.pixelFormat = castor::PixelFormat::eUNDEFINED;
	}
	CU_EndAttributePush( CSCNSection::eRenderTarget )

	CU_ImplementAttributeParser( parserWindowVSync )
	{
		auto & parsingContext = getParserContext( context );

		if ( !params.empty() )
		{
			params[0]->get( parsingContext.window.enableVSync );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWindowFullscreen )
	{
		auto & parsingContext = getParserContext( context );

		if ( !params.empty() )
		{
			params[0]->get( parsingContext.window.fullscreen );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWindowAllowHdr )
	{
		auto & parsingContext = getParserContext( context );

		if ( !params.empty() )
		{
			params[0]->get( parsingContext.window.allowHdr );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserWindowEnd )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.inWindow = false;
		log::info << "Loaded window [" << parsingContext.window.name
			<< ", HDR(" << parsingContext.window.allowHdr << ")"
			<< ", VSYNC(" << parsingContext.window.enableVSync << ")"
			<< ", FS" << parsingContext.window.fullscreen << ")]" << std::endl;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserRenderTargetScene )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( !params.empty() )
		{
			ScenePtrStrMap::iterator it = parsingContext.mapScenes.find( params[0]->get( name ) );

			if ( it != parsingContext.mapScenes.end() )
			{
				parsingContext.renderTarget->setScene( *it->second );
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
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.renderTarget->getScene() )
			{
				castor::String name;

				if ( auto camera = parsingContext.renderTarget->getScene()->findCamera( params[0]->get( name ) ) )
				{
					parsingContext.renderTarget->setCamera( *camera );
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

	CU_ImplementAttributeParser( parserRenderTargetSize )
	{
		auto & parsingContext = getParserContext( context );

		if ( !params.empty() )
		{
			params[0]->get( parsingContext.size );

			if ( parsingContext.pixelFormat != castor::PixelFormat::eUNDEFINED )
			{
				parsingContext.renderTarget = parsingContext.parser->getEngine()->getRenderTargetCache().add( parsingContext.targetType
					, parsingContext.size
					, parsingContext.pixelFormat );
				parsingContext.renderTarget->enableFullLoading( parsingContext.enableFullLoading );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetFormat )
	{
		auto & parsingContext = getParserContext( context );

		if ( !params.empty() )
		{
			params[0]->get( parsingContext.pixelFormat );

			if ( parsingContext.pixelFormat < castor::PixelFormat::eD16_UNORM )
			{
				if ( parsingContext.size != castor::Size{ 1u, 1u } )
				{
					parsingContext.renderTarget = parsingContext.parser->getEngine()->getRenderTargetCache().add( parsingContext.targetType
						, parsingContext.size
						, parsingContext.pixelFormat );
					parsingContext.renderTarget->enableFullLoading( parsingContext.enableFullLoading );
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
		auto & parsingContext = getParserContext( context );

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
				//parsingContext->renderTarget->setStereo( true );
				//parsingContext->renderTarget->setIntraOcularDistance( rIntraOcularDistance );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetPostEffect )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			params[0]->get( name );
			Parameters parameters;

			if ( params.size() > 1 )
			{
				castor::String tmp;
				parameters.parse( params[1]->get( tmp ) );
			}

			auto effect = parsingContext.renderTarget->getPostEffect( name );

			if ( !effect )
			{
				CU_ParsingError( cuT( "PostEffect [" ) + name + cuT( "] is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				effect->enable( true );
				effect->setParameters( std::move( parameters ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetToneMapping )
	{
		auto & parsingContext = getParserContext( context );

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
			castor::String name;
			params[0]->get( name );
			Parameters parameters;

			if ( params.size() > 1 )
			{
				castor::String tmp;
				parameters.parse( params[1]->get( tmp ) );
			}

			parsingContext.renderTarget->setToneMappingType( name, parameters );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetSsao )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSsao )

	CU_ImplementAttributeParser( parserRenderTargetFullLoading )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [enable] parameter." ) );
		}
		else
		{
			parsingContext.renderTarget->enableFullLoading( params[0]->get< bool >() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRenderTargetEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.renderTarget )
		{
			CU_ParsingError( cuT( "No target initialised. (Did you forget to set its size and format ?)" ) );
		}
		else
		{
			auto target = parsingContext.renderTarget;
			log::info << "Loaded target [" << target->getName()
				<< ", FMT(" << ashes::getName( VkFormat( target->getPixelFormat() ) ) << ")"
				<< ", VSYNC(" << target->getSize() << ")]" << std::endl;

			if ( parsingContext.inWindow )
			{
				parsingContext.window.renderTarget = std::move( parsingContext.renderTarget );
			}
			else
			{
				parsingContext.textureRenderTarget = std::move( parsingContext.renderTarget );
			}
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSamplerMinFilter )
	{
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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

	CU_ImplementAttributeParser( parserSamplerMaxLod )
	{
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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

	CU_ImplementAttributeParser( parserSamplerLodBias )
	{
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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

	CU_ImplementAttributeParser( parserSamplerUWrapMode )
	{
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );

		if ( auto sampler = parsingContext.sampler )
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
		auto & parsingContext = getParserContext( context );
		auto sampler = parsingContext.sampler;

		if ( !parsingContext.ownSampler
			&& !sampler )
		{
			CU_ParsingError( cuT( "No sampler initialised." ) );
		}
		else
		{
			log::info << "Loaded sampler [" << parsingContext.sampler->getName() << "]" << std::endl;

			if ( parsingContext.ownSampler )
			{
				parsingContext.parser->getEngine()->addSampler( parsingContext.ownSampler->getName()
					, parsingContext.ownSampler
					, true );
			}

			parsingContext.sampler = {};
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSceneBkColour )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::RgbColour clrBackground;
			params[0]->get( clrBackground );
			parsingContext.scene->setBackgroundColour( clrBackground );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneBkImage )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			auto imgBackground = castor::makeUnique< ImageBackground >( *parsingContext.parser->getEngine()
				, *parsingContext.scene );
			castor::Path path;
			imgBackground->setImage( context.file.getPath(), params[0]->get( path ) );
			parsingContext.scene->setBackground( castor::ptrRefCast< SceneBackground >( imgBackground ) );
			parsingContext.skybox.reset();
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneFont )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );
		parsingContext.viewport.reset();
		parsingContext.point2f = { 2.2f, 1.0f };

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
		auto & parsingContext = getParserContext( context );
		parsingContext.light = {};
		parsingContext.strName.clear();

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
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			parsingContext.nodeConfig = {};
			params[0]->get( parsingContext.nodeConfig.name );
			parsingContext.nodeConfig.isCameraNode = true;
			parsingContext.nodeConfig.parent = parsingContext.scene->getCameraRootNode();
		}
	}
	CU_EndAttributePush( CSCNSection::eNode )

	CU_ImplementAttributeParser( parserSceneSceneNode )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			parsingContext.nodeConfig = {};
			params[0]->get( parsingContext.nodeConfig.name );
			parsingContext.nodeConfig.isCameraNode = false;
			parsingContext.nodeConfig.parent = parsingContext.scene->getObjectRootNode();
		}
	}
	CU_EndAttributePush( CSCNSection::eNode )

	CU_ImplementAttributeParser( parserSceneObject )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.geometry = {};
		castor::String name;

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			params[0]->get( name );
			parsingContext.ownGeometry = parsingContext.scene->createGeometry( name
				, *parsingContext.scene );
			parsingContext.geometry = parsingContext.ownGeometry.get();
		}
	}
	CU_EndAttributePush( CSCNSection::eObject )

	CU_ImplementAttributeParser( parserSceneAmbientLight )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::RgbColour clColour;
			params[0]->get( clColour );
			parsingContext.scene->setAmbientLight( clColour );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImport )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.sceneImportConfig = {};
	}
	CU_EndAttributePush( CSCNSection::eSceneImport )

	CU_ImplementAttributeParser( parserSceneBillboard )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			params[0]->get( name );
			parsingContext.ownBillboards = castor::makeUnique< BillboardList >( name, *parsingContext.scene );
			parsingContext.billboards = parsingContext.ownBillboards.get();
		}
	}
	CU_EndAttributePush( CSCNSection::eBillboard )

	CU_ImplementAttributeParser( parserSceneAnimatedObjectGroup )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			params[0]->get( name );
			parsingContext.animGroup = parsingContext.scene->addNewAnimatedObjectGroup( name, *parsingContext.scene );
		}
	}
	CU_EndAttributePush( CSCNSection::eAnimGroup )

	CU_ImplementAttributeParser( parserScenePanelOverlay )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
			auto & parent = parsingContext.parentOverlays.back();
			parsingContext.overlay.rptr = parsingContext.scene->tryFindOverlay( params[0]->get( name ) );

			if ( !parsingContext.overlay.rptr )
			{
				parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::ePanel
					, parsingContext.scene
					, parent.rptr );
				parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
				parsingContext.overlay.rptr->rename( name );
			}

			parsingContext.overlay.rptr->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserSceneBorderPanelOverlay )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
			auto & parent = parsingContext.parentOverlays.back();
			parsingContext.overlay.rptr = parsingContext.scene->tryFindOverlay( params[0]->get( name ) );

			if ( !parsingContext.overlay.rptr )
			{
				parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::eBorderPanel
					, parsingContext.scene
					, parent.rptr );
				parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
				parsingContext.overlay.rptr->rename( name );
			}

			parsingContext.overlay.rptr->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserSceneTextOverlay )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
			auto & parent = parsingContext.parentOverlays.back();
			parsingContext.overlay.rptr = parsingContext.scene->tryFindOverlay( params[0]->get( name ) );

			if ( !parsingContext.overlay.rptr )
			{
				parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
					, OverlayType::eText
					, parsingContext.scene
					, parent.rptr );
				parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
				parsingContext.overlay.rptr->rename( name );
			}

			parsingContext.overlay.rptr->setVisible( false );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserSceneSkybox )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			parsingContext.skybox = castor::makeUnique< SkyboxBackground >( *parsingContext.parser->getEngine()
					, *parsingContext.scene );
		}
	}
	CU_EndAttributePush( CSCNSection::eSkybox )

	CU_ImplementAttributeParser( parserSceneFogType )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
			castor::String value;
			params[0]->get( value );
			parsingContext.strName = value;
			parsingContext.particleCount = 0u;
			parsingContext.material = {};
		}
	}
	CU_EndAttributePush( CSCNSection::eParticleSystem )

	CU_ImplementAttributeParser( parserVoxelConeTracing )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eVoxelConeTracing )

	CU_ImplementAttributeParser( parserSceneEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else
		{
			log::info << "Loaded scene [" << parsingContext.scene->getName() << "]" << std::endl;

			if ( parsingContext.scene->getName() == LoadingScreen::SceneName )
			{
				parsingContext.parser->getEngine()->setLoadingScene( std::move( parsingContext.ownScene ) );
			}
			else
			{
				parsingContext.parser->getEngine()->addScene( parsingContext.scene->getName()
					, parsingContext.ownScene
					, true );
				parsingContext.scene = {};
			}
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSceneImportFile )
	{
		auto & parsingContext = getParserContext( context );
		castor::Path path;
		castor::Path pathFile = context.file.getPath() / params[0]->get( path );
		parsingContext.sceneImportConfig.files.push_back( pathFile );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportAnimFile )
	{
		auto & parsingContext = getParserContext( context );
		castor::Path path;
		castor::Path pathFile = context.file.getPath() / params[0]->get( path );
		parsingContext.sceneImportConfig.animFiles.push_back( pathFile );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportPrefix )
	{
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.sceneImportConfig.prefix );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportRescale )
	{
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.sceneImportConfig.rescale );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportPitch )
	{
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.sceneImportConfig.pitch );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportYaw )
	{
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.sceneImportConfig.yaw );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportRoll )
	{
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.sceneImportConfig.roll );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportNoOptimisations )
	{
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.sceneImportConfig.noOptimisations );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportEmissiveMult )
	{
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.sceneImportConfig.emissiveMult );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSceneImportTexRemap )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.sceneImportConfig.textureRemaps.clear();
		parsingContext.sceneImportConfig.textureRemapIt = parsingContext.sceneImportConfig.textureRemaps.end();
	}
	CU_EndAttributePush( CSCNSection::eTextureRemap )

	CU_ImplementAttributeParser( parserSceneImportCenterCamera )
	{
		auto & parsingContext = getParserContext( context );
		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing name parameter" ) );
		}
		else
		{
			parsingContext.sceneImportConfig.centerCamera = params[0]->get< castor::String >();
		}
	}
	CU_EndAttributePush( CSCNSection::eTextureRemap )

	CU_ImplementAttributeParser( parserSceneImportEnd )
	{
		auto & parsingContext = getParserContext( context );
		Engine * engine = parsingContext.parser->getEngine();

		if ( parsingContext.sceneImportConfig.files.empty() )
		{
			CU_ParsingError( cuT( "No file chosen to import" ) );
		}
		else
		{
			Parameters parameters;

			if ( parsingContext.sceneImportConfig.rescale != 1.0f )
			{
				parameters.add( cuT( "rescale" ), parsingContext.sceneImportConfig.rescale );
			}

			if ( parsingContext.sceneImportConfig.pitch != 0.0f )
			{
				parameters.add( cuT( "pitch" ), parsingContext.sceneImportConfig.pitch );
			}

			if ( parsingContext.sceneImportConfig.yaw != 0.0f )
			{
				parameters.add( cuT( "yaw" ), parsingContext.sceneImportConfig.yaw );
			}

			if ( parsingContext.sceneImportConfig.roll != 0.0f )
			{
				parameters.add( cuT( "roll" ), parsingContext.sceneImportConfig.roll );
			}

			if ( !parsingContext.sceneImportConfig.prefix.empty() )
			{
				parameters.add( cuT( "prefix" ), parsingContext.sceneImportConfig.prefix );
			}

			if ( parsingContext.sceneImportConfig.noOptimisations )
			{
				parameters.add( cuT( "no_optimisations" ), parsingContext.sceneImportConfig.noOptimisations );
			}

			if ( parsingContext.sceneImportConfig.emissiveMult != 1.0f )
			{
				parameters.add( cuT( "emissive_mult" ), parsingContext.sceneImportConfig.emissiveMult );
			}

			if ( !parsingContext.sceneImportConfig.centerCamera.empty() )
			{
				parameters.add( cuT( "center_camera" ), parsingContext.sceneImportConfig.centerCamera );
			}

			SceneImporter importer{ *engine };

			for ( auto & file : parsingContext.sceneImportConfig.files )
			{
				if ( !importer.import( *parsingContext.scene
					, file
					, parameters
					, parsingContext.sceneImportConfig.textureRemaps ) )
				{
					CU_ParsingError( cuT( "External scene Import failed" ) );
				}
			}

			for ( auto & file : parsingContext.sceneImportConfig.animFiles )
			{
				if ( !importer.importAnimations( *parsingContext.scene
					, file
					, parameters ) )
				{
					CU_ParsingError( cuT( "External scene Import failed" ) );
				}
			}
		}

		parsingContext.sceneImportConfig = {};
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMesh )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		params[0]->get( name );

		if ( parsingContext.scene )
		{
			parsingContext.mesh = parsingContext.scene->tryFindMesh( name );

			if ( !parsingContext.mesh )
			{
				parsingContext.ownMesh = parsingContext.scene->createMesh( name
					, *parsingContext.scene );
				parsingContext.mesh = parsingContext.ownMesh.get();
			}
		}
		else
		{
			CU_ParsingError( cuT( "No scene initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eMesh )

	CU_ImplementAttributeParser( parserSkeleton )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		params[0]->get( name );

		if ( parsingContext.scene )
		{
			parsingContext.skeleton = parsingContext.scene->tryFindSkeleton( name );

			if ( !parsingContext.skeleton )
			{
				parsingContext.skeleton = parsingContext.scene->addNewSkeleton( name
					, *parsingContext.scene );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No scene initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eSkeleton )

	CU_ImplementAttributeParser( parserDirectionalShadowCascades )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
			castor::String value;
			params[0]->get( value );

			if ( auto node = parsingContext.scene->tryFindSceneNode( value ) )
			{
				parsingContext.parentNode = node;
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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
			castor::String name;
			params[0]->get( name );
			auto material = parsingContext.parser->getEngine()->tryFindMaterial( name );

			if ( material )
			{
				parsingContext.material = material;
			}
			else
			{
				CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserParticleSystemDimensions )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );
		
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

			parsingContext.particleSystem = parsingContext.scene->tryFindParticleSystem( parsingContext.strName );

			if ( !parsingContext.particleSystem )
			{
				auto node = parsingContext.parentNode;

				if ( !node )
				{
					node = parsingContext.scene->getObjectRootNode();
				}

				parsingContext.parentNode = nullptr;
				parsingContext.ownParticleSystem = parsingContext.scene->createParticleSystem( parsingContext.strName
					, *parsingContext.scene
					, *node
					, parsingContext.particleCount );
				parsingContext.particleSystem = parsingContext.ownParticleSystem.get();
			}

			parsingContext.particleSystem->setMaterial( parsingContext.material );
			parsingContext.particleSystem->setDimensions( parsingContext.point2f );
		}
	}
	CU_EndAttributePush( CSCNSection::eParticle )

	CU_ImplementAttributeParser( parserParticleSystemCSShader )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderProgram = {};
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
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderProgram = {};
		parsingContext.shaderStage = VkShaderStageFlagBits( 0u );
		
		if ( !parsingContext.particleSystem )
		{
			CU_ParsingError( cuT( "No particle system initialised." ) );
		}
		else
		{
			parsingContext.parentNode = nullptr;
			log::info << "Loaded sampler [" << parsingContext.particleSystem->getName() << "]" << std::endl;

			if ( parsingContext.ownParticleSystem )
			{
				parsingContext.scene->addParticleSystem( parsingContext.particleSystem->getName()
					, parsingContext.ownParticleSystem
					, true );
			}

			parsingContext.particleSystem = {};
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserParticleType )
	{
		auto & parsingContext = getParserContext( context );

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
			castor::String value;
			params[0]->get( value );
			Engine * engine = parsingContext.parser->getEngine();

			if ( !engine->getParticleFactory().isTypeRegistered( castor::string::lowerCase( value ) ) )
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
		auto & parsingContext = getParserContext( context );

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
			castor::String name;
			uint32_t type;
			castor::String value;
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
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;

			if ( auto parent = parsingContext.scene->findSceneNode( params[0]->get( name ) ) )
			{
				parsingContext.parentNode = parent;

				if ( parsingContext.light )
				{
					parsingContext.light->detach();
					parsingContext.parentNode->attachObject( *parsingContext.light );
					parsingContext.parentNode = nullptr;
				}
			}
			else
			{
				CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else
		{
			log::info << "Loaded light [" << parsingContext.light->getName() << "]" << std::endl;
			parsingContext.parentNode = nullptr;

			if ( parsingContext.ownLight )
			{
				parsingContext.scene->addLight( parsingContext.light->getName()
					, parsingContext.ownLight
					, true );
			}

			parsingContext.light = {};
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserLightType )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !params.empty() )
		{
			uint32_t uiType;
			params[0]->get( uiType );
			parsingContext.lightType = LightType( uiType );
			parsingContext.light = parsingContext.scene->tryFindLight( parsingContext.strName );

			if ( !parsingContext.light )
			{
				auto node = parsingContext.parentNode;

				if ( !node )
				{
					node = parsingContext.scene->getObjectRootNode();
				}

				parsingContext.parentNode = nullptr;
				parsingContext.ownLight = parsingContext.scene->createLight( parsingContext.strName
					, *parsingContext.scene
					, *node
					, parsingContext.scene->getLightsFactory()
					, parsingContext.lightType );
				parsingContext.light = parsingContext.ownLight.get();
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightColour )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			castor::Point3f value;
			params[0]->get( value );
			parsingContext.light->setColour( value.ptr() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightIntensity )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			castor::Point2f value;
			params[0]->get( value );
			parsingContext.light->setIntensity( value.ptr() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightAttenuation )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			castor::Point3f value;
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

	CU_ImplementAttributeParser( parserLightRange )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
		else if ( !params.empty() )
		{
			auto value = params[0]->get< float >();

			if ( parsingContext.lightType == LightType::ePoint )
			{
				parsingContext.light->getPointLight()->setRange( value );
			}
			else if ( parsingContext.lightType == LightType::eSpot )
			{
				parsingContext.light->getSpotLight()->setRange( value );
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
		auto & parsingContext = getParserContext( context );

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
				parsingContext.light->getSpotLight()->setInnerCutOff( castor::Angle::fromDegrees( fFloat / 2.0f ) );
				parsingContext.light->getSpotLight()->setOuterCutOff( castor::Angle::fromDegrees( fFloat ) );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightInnerCutOff )
	{
		auto & parsingContext = getParserContext( context );

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
				parsingContext.light->getSpotLight()->setInnerCutOff( castor::Angle::fromDegrees( fFloat ) );
			}
			else
			{
				CU_ParsingError( cuT( "Wrong type of light to apply a cut off, needs spotlight" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserLightOuterCutOff )
	{
		auto & parsingContext = getParserContext( context );

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
				parsingContext.light->getSpotLight()->setOuterCutOff( castor::Angle::fromDegrees( fFloat ) );
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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eShadows )

	CU_ImplementAttributeParser( parserLightShadowProducer )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eRaw )

	CU_ImplementAttributeParser( parserShadowsPcfConfig )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::ePcf )

	CU_ImplementAttributeParser( parserShadowsVsmConfig )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eVsm )

	CU_ImplementAttributeParser( parserShadowsRsmConfig )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eRsm )

	CU_ImplementAttributeParser( parserShadowsLpvConfig )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.light )
		{
			CU_ParsingError( cuT( "No Light initialised. Have you set it's type?" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eLpv )

	CU_ImplementAttributeParser( parserShadowsRawMinOffset )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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

	CU_ImplementAttributeParser( parserShadowsPcfFilterSize )
	{
		auto & parsingContext = getParserContext( context );

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
			uint32_t value;
			params[0]->get( value );
			parsingContext.light->setPcfFilterSize( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsPcfSampleCount )
	{
		auto & parsingContext = getParserContext( context );

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
			uint32_t value;
			params[0]->get( value );
			parsingContext.light->setPcfSampleCount( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVsmMinVariance )
	{
		auto & parsingContext = getParserContext( context );

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
			parsingContext.light->setVsmMinVariance( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserShadowsVsmLightBleedingReduction )
	{
		auto & parsingContext = getParserContext( context );

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
			parsingContext.light->setVsmLightBleedingReduction( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserRsmIntensity )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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

	CU_ImplementAttributeParser( parserNodeStatic )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [static] parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.nodeConfig.isStatic );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeParent )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [parent] parameter." ) );
		}
		else
		{
			castor::String name;
			params[0]->get( name );
			SceneNodeRPtr parent;

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
				parent = parsingContext.scene->findSceneNode( name );
			}

			if ( parent )
			{
				parsingContext.nodeConfig.parent = parent;
			}
			else
			{
				CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodePosition )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [position] parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.nodeConfig.position );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeOrientation )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [orientation] parameter." ) );
		}
		else
		{
			castor::Point3f axis;
			float angle;
			params[0]->get( axis );
			params[1]->get( angle );
			parsingContext.nodeConfig.orientation = castor::Quaternion::fromAxisAngle( axis
				, castor::Angle::fromDegrees( angle ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeRotate )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [orientation] parameter." ) );
		}
		else
		{
			castor::Point3f axis;
			float angle;
			params[0]->get( axis );
			params[1]->get( angle );
			parsingContext.nodeConfig.orientation *= castor::Quaternion::fromAxisAngle( axis
				, castor::Angle::fromDegrees( angle ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeDirection )
	{
		auto & parsingContext = getParserContext( context );

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
			parsingContext.nodeConfig.orientation = castor::Quaternion::fromAxes( right, up, direction );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeScale )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing [direction] parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.nodeConfig.scale );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserNodeEnd )
	{
		auto & parsingContext = getParserContext( context );
		auto & nodeConfig = parsingContext.nodeConfig;

		auto sceneNode = parsingContext.scene->createSceneNode( nodeConfig.name
			, *parsingContext.scene
			, nodeConfig.parent
			, nodeConfig.position
			, nodeConfig.orientation
			, nodeConfig.scale
			, nodeConfig.isStatic );

		auto name = sceneNode->getName();
		auto node = parsingContext.scene->addSceneNode( name, sceneNode );
		sceneNode.reset();

		if ( !nodeConfig.isStatic )
		{
			for ( auto fileName : parsingContext.csnaFiles )
			{
				auto fName = fileName.getFileName();
				auto pos = fName.find( name );

				if ( pos == 0u
					&& fName[name.size()] == '-' )
				{
					auto animName = fName.substr( name.size() + 1u );

					if ( !animName.empty() )
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

	CU_ImplementAttributeParser( parserObjectParent )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			params[0]->get( name );
			SceneNodeRPtr parent;

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
				parent = parsingContext.scene->findSceneNode( name );
			}

			if ( parent )
			{
				parent->attachObject( *parsingContext.geometry );
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

	CU_ImplementAttributeParser( parserObjectMaterial )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.geometry->getMesh() )
			{
				castor::String name;
				auto material = parsingContext.parser->getEngine()->tryFindMaterial( params[0]->get( name ) );

				if ( material )
				{
					for ( auto & submesh : *parsingContext.geometry->getMesh() )
					{
						parsingContext.geometry->setMaterial( *submesh, material );
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

	CU_ImplementAttributeParser( parserObjectMaterials )
	{
	}
	CU_EndAttributePush( CSCNSection::eObjectMaterials )

	CU_ImplementAttributeParser( parserObjectCastShadows )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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

	CU_ImplementAttributeParser( parserObjectCullable )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			bool value;
			params[0]->get( value );
			parsingContext.geometry->setCullable( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectEnd )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.parentNode = nullptr;
		log::info << "Loaded geometry [" << parsingContext.geometry->getName() << "]" << std::endl;

		if ( parsingContext.ownGeometry )
		{
			parsingContext.scene->addGeometry( std::move( parsingContext.ownGeometry ) );
		}

		parsingContext.geometry = nullptr;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserObjectMaterialsMaterial )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.geometry )
		{
			CU_ParsingError( cuT( "No Geometry initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.geometry->getMesh() )
			{
				castor::String name;
				uint16_t index;
				auto material = parsingContext.parser->getEngine()->tryFindMaterial( params[1]->get( name ) );

				if ( material )
				{
					if ( parsingContext.geometry->getMesh()->getSubmeshCount() > params[0]->get( index ) )
					{
						auto submesh = parsingContext.geometry->getMesh()->getSubmesh( index );
						parsingContext.geometry->setMaterial( *submesh, material );
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
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserObjectMaterialsEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSkeletonImport )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !parsingContext.skeleton )
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

			if ( !SkeletonImporter::import( *parsingContext.skeleton
				, pathFile
				, parameters ) )
			{
				CU_ParsingError( cuT( "Skeleton Import failed" ) );
				parsingContext.skeleton = nullptr;
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkeletonAnimImport )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised." ) );
		}
		else if ( !parsingContext.skeleton )
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

			auto & engine = *parsingContext.scene->getEngine();
			auto extension = castor::string::lowerCase( path.getExtension() );

			if ( !engine.getImporterFileFactory().isTypeRegistered( extension ) )
			{
				CU_ParsingError( cuT( "Importer for [" ) + extension + cuT( "] files is not registered, make sure you've got the matching plug-in installed." ) );
			}
			else
			{
				auto file = engine.getImporterFileFactory().create( extension
					, *parsingContext.scene
					, pathFile
					, parameters );

				if ( auto importer = file->createAnimationImporter() )
				{
					for ( auto animName : file->listSkeletonAnimations( *parsingContext.skeleton ) )
					{
						auto animation = castor::makeUnique< SkeletonAnimation >( *parsingContext.skeleton
							, animName );

						if ( !importer->import( *animation
							, file.get()
							, parameters ) )
						{
							CU_ParsingError( cuT( "Skeleton animation Import failed" ) );
						}
						else
						{
							parsingContext.skeleton->addAnimation( castor::ptrRefCast< Animation >( animation ) );
						}
					}
				}
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkeletonEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Scene initialised." ) );
		}
		else if ( !parsingContext.skeleton )
		{
			CU_ParsingError( cuT( "No Skeleton initialised." ) );
		}
		else
		{
			log::info << "Loaded skeleton [" << parsingContext.skeleton->getName() << "]" << std::endl;
			parsingContext.importer.reset();
			parsingContext.skeleton = nullptr;
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMeshType )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No scene initialised" ) );
		}
		else if ( !parsingContext.mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else
		{
			castor::String type;
			params[0]->get( type );

			Parameters parameters;

			if ( params.size() > 1 )
			{
				castor::String tmp;
				parameters.parse( params[1]->get( tmp ) );
			}

			auto & factory = parsingContext.scene->getEngine()->getMeshFactory();
			factory.create( type )->generate( *parsingContext.mesh, parameters );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshSubmesh )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.face1 = -1;
		parsingContext.face2 = -1;
		parsingContext.submesh = {};

		if ( !parsingContext.mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else
		{
			parsingContext.submesh = parsingContext.mesh->createSubmesh();
		}
	}
	CU_EndAttributePush( CSCNSection::eSubmesh )

	CU_ImplementAttributeParser( parserMeshImport )
	{
		auto & parsingContext = getParserContext( context );

		if ( auto mesh = parsingContext.mesh )
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

			if ( !MeshImporter::import( *mesh
				, pathFile
				, parameters
				, true ) )
			{
				CU_ParsingError( cuT( "Mesh Import failed" ) );
				parsingContext.mesh = {};
			}
		}
		else
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshAnimImport )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.mesh )
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
				castor::String meshParams;
				params[1]->get( meshParams );
				scnprs::fillMeshImportParameters( context, meshParams, parameters );
			}

			auto animation = castor::makeUnique< MeshAnimation >( *parsingContext.mesh
				, pathFile.getFileName() );

			if ( !AnimationImporter::import( *animation
				, pathFile
				, parameters ) )
			{
				CU_ParsingError( cuT( "Mesh animation Import failed" ) );
			}
			else
			{
				parsingContext.mesh->addAnimation( castor::ptrRefCast< Animation >( animation ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshMorphTargetImport )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.mesh )
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
				castor::String meshParams;
				params[1]->get( meshParams );
				scnprs::fillMeshImportParameters( context, meshParams, parameters );
			}

			Mesh mesh{ cuT( "MorphImport" ), *parsingContext.scene };

			if ( !MeshImporter::import( mesh
				, pathFile
				, parameters
				, false ) )
			{
				CU_ParsingError( cuT( "Mesh Import failed" ) );
			}
			else if ( mesh.getSubmeshCount() == parsingContext.mesh->getSubmeshCount() )
			{
				for ( auto & morphSubmesh : mesh )
				{
					auto id = morphSubmesh->getId();
					auto submesh = parsingContext.mesh->getSubmesh( id );
					auto submeshFlags = morphSubmesh->getSubmeshFlags( nullptr );
					auto component = submesh->hasComponent( MorphComponent::Name )
						? submesh->getComponent< MorphComponent >()
						: submesh->createComponent< MorphComponent >( scnprs::getMorphFlags( submeshFlags ) );
					castor3d::SubmeshAnimationBuffer buffer;

					if ( checkFlag( submeshFlags, SubmeshFlag::ePositions ) )
					{
						buffer.positions = morphSubmesh->getPositions();
						uint32_t index = 0u;

						for ( auto & position : buffer.positions )
						{
							position -= submesh->getPositions()[index++];
						}
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eNormals ) )
					{
						buffer.normals = morphSubmesh->getNormals();
						uint32_t index = 0u;

						for ( auto & normal : buffer.normals )
						{
							normal -= submesh->getNormals()[index++];
						}
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTangents ) )
					{
						buffer.tangents = morphSubmesh->getTangents();
						uint32_t index = 0u;

						for ( auto & tangent : buffer.tangents )
						{
							tangent -= submesh->getTangents()[index++];
						}
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords0 ) )
					{
						buffer.texcoords0 = morphSubmesh->getTexcoords0();
						uint32_t index = 0u;

						for ( auto & texcoord : buffer.texcoords0 )
						{
							texcoord -= submesh->getTexcoords0()[index++];
						}
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords1 ) )
					{
						buffer.texcoords1 = morphSubmesh->getTexcoords1();
						uint32_t index = 0u;

						for ( auto & texcoord : buffer.texcoords1 )
						{
							texcoord -= submesh->getTexcoords1()[index++];
						}
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords2 ) )
					{
						buffer.texcoords2 = morphSubmesh->getTexcoords2();
						uint32_t index = 0u;

						for ( auto & texcoord : buffer.texcoords2 )
						{
							texcoord -= submesh->getTexcoords2()[index++];
						}
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eTexcoords3 ) )
					{
						buffer.texcoords3 = morphSubmesh->getTexcoords3();
						uint32_t index = 0u;

						for ( auto & texcoord : buffer.texcoords3 )
						{
							texcoord -= submesh->getTexcoords3()[index++];
						}
					}

					if ( checkFlag( submeshFlags, SubmeshFlag::eColours ) )
					{
						buffer.colours = morphSubmesh->getColours();
						uint32_t index = 0u;

						for ( auto & colour : buffer.colours )
						{
							colour -= submesh->getColours()[index++];
						}
					}

					component->addMorphTarget( std::move( buffer ) );
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

	CU_ImplementAttributeParser( parserMeshDefaultMaterial )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			auto material = parsingContext.parser->getEngine()->findMaterial( params[0]->get( name ) );

			if ( material )
			{
				for ( auto & submesh : *parsingContext.mesh )
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

	CU_ImplementAttributeParser( parserMeshDefaultMaterials )
	{
	}
	CU_EndAttributePush( CSCNSection::eMeshDefaultMaterials )

	CU_ImplementAttributeParser( parserMeshSkeleton )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Scene initialised." ) );
		}
		else if ( auto mesh = parsingContext.mesh )
		{
			castor::String name;
			auto skeleton = parsingContext.scene->findSkeleton( params[0]->get( name ) );

			if ( skeleton )
			{
				mesh->setSkeleton( skeleton );
			}
			else
			{
				CU_ParsingError( cuT( "Skeleton [" ) + name + cuT( "] does not exist" ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMeshMorphAnimation )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Scene initialised." ) );
		}
		else if ( auto mesh = parsingContext.mesh )
		{
			castor::String name;
			params[0]->get( name );
			parsingContext.morphAnimation = castor::makeUnique< MeshAnimation >( *mesh, name );
		}
		else
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eMorphAnimation )

	CU_ImplementAttributeParser( parserMeshEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Scene initialised." ) );
		}
		else if ( auto mesh = parsingContext.mesh )
		{
			if ( parsingContext.ownMesh )
			{
				parsingContext.scene->addMesh( mesh->getName()
					, parsingContext.ownMesh
					, true );
			}

			if ( parsingContext.geometry )
			{
				parsingContext.geometry->setMesh( mesh );
			}

			parsingContext.importer.reset();
			parsingContext.mesh = {};

			for ( auto & submesh : *mesh )
			{
				mesh->getScene()->getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMeshMorphTargetWeight )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Scene initialised." ) );
		}
		else if ( !parsingContext.morphAnimation )
		{
			CU_ParsingError( cuT( "No Morph Animation initialised." ) );
		}
		else if ( params.size() < 3u )
		{
			CU_ParsingError( cuT( "Invalid parameters." ) );
		}
		else if ( auto mesh = parsingContext.mesh )
		{
			float timeIndex{};
			params[0]->get( timeIndex );
			uint32_t targetIndex{};
			params[1]->get( targetIndex );
			float targetWeight{};
			params[2]->get( targetWeight );
			auto & animation = *parsingContext.morphAnimation;

			for ( auto & submesh : *mesh )
			{
				MeshAnimationSubmesh animSubmesh{ animation, *submesh };
				animation.addChild( std::move( animSubmesh ) );
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

	CU_ImplementAttributeParser( parserMeshMorphAnimationEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.scene )
		{
			CU_ParsingError( cuT( "No Scene initialised." ) );
		}
		else if ( !parsingContext.morphAnimation )
		{
			CU_ParsingError( cuT( "No Morph Animation initialised." ) );
		}
		else if ( auto mesh = parsingContext.mesh )
		{
			log::info << "Loaded morp animation [" << parsingContext.morphAnimation->getName() << "]" << std::endl;
			mesh->addAnimation( castor::ptrRefCast< Animation >( parsingContext.morphAnimation ) );
		}
		else
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMeshDefaultMaterialsMaterial )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.mesh )
		{
			CU_ParsingError( cuT( "No Mesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String name;
			uint16_t index;
			auto material = parsingContext.parser->getEngine()->findMaterial( params[1]->get( name ) );

			if ( material )
			{
				if ( parsingContext.mesh->getSubmeshCount() > params[0]->get( index ) )
				{
					auto submesh = parsingContext.mesh->getSubmesh( index );
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

	CU_ImplementAttributeParser( parserMeshDefaultMaterialsEnd )
	{
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSubmeshVertex )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::Point3f value;
			params[0]->get( value );
			parsingContext.vertexPos.push_back( value[0] );
			parsingContext.vertexPos.push_back( value[1] );
			parsingContext.vertexPos.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshUV )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::Point2f value;
			params[0]->get( value );
			parsingContext.vertexTex.push_back( value[0] );
			parsingContext.vertexTex.push_back( value[1] );
			parsingContext.vertexTex.push_back( 0.0 );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshUVW )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::Point3f value;
			params[0]->get( value );
			parsingContext.vertexTex.push_back( value[0] );
			parsingContext.vertexTex.push_back( value[1] );
			parsingContext.vertexTex.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshNormal )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::Point3f value;
			params[0]->get( value );
			parsingContext.vertexNml.push_back( value[0] );
			parsingContext.vertexNml.push_back( value[1] );
			parsingContext.vertexNml.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshTangent )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::Point3f value;
			params[0]->get( value );
			parsingContext.vertexTan.push_back( value[0] );
			parsingContext.vertexTan.push_back( value[1] );
			parsingContext.vertexTan.push_back( value[2] );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFace )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String strParams;
			params[0]->get( strParams );
			castor::Point3i pt3Indices;
			auto arrayValues = castor::string::split( strParams, cuT( " " ) );
			parsingContext.face1 = -1;
			parsingContext.face2 = -1;

			if ( arrayValues.size() >= 4 )
			{
				castor::Point4i pt4Indices;

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
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String strParams;
			params[0]->get( strParams );

			if ( parsingContext.vertexTex.empty() )
			{
				parsingContext.vertexTex.resize( parsingContext.vertexPos.size() );
			}

			auto arrayValues = castor::string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 6 && parsingContext.face1 != -1 )
			{
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[2] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[3] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[4] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[5] );
			}

			if ( arrayValues.size() >= 8 && parsingContext.face2 != -1 )
			{
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[4] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[5] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[6] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[7] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceUVW )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String strParams;
			params[0]->get( strParams );

			if ( parsingContext.vertexTex.empty() )
			{
				parsingContext.vertexTex.resize( parsingContext.vertexPos.size() );
			}

			auto arrayValues = castor::string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext.face1 != -1 )
			{
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[2] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[3] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[4] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[5] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[6] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[7] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext.face2 != -1 )
			{
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[2] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[6] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[7] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[8] );
				parsingContext.vertexTex[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 9] );
				parsingContext.vertexTex[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[10] );
				parsingContext.vertexTex[2 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceNormals )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String strParams;
			params[0]->get( strParams );

			if ( parsingContext.vertexNml.empty() )
			{
				parsingContext.vertexNml.resize( parsingContext.vertexPos.size() );
			}

			auto arrayValues = castor::string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext.face1 != -1 )
			{
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[2] );
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[3] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[4] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[5] );
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[6] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[7] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext.face2 != -1 )
			{
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 0] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 1] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 2] );
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 6] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 7] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 8] );
				parsingContext.vertexNml[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 9] );
				parsingContext.vertexNml[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[10] );
				parsingContext.vertexNml[2 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshFaceTangents )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.submesh )
		{
			CU_ParsingError( cuT( "No Submesh initialised." ) );
		}
		else if ( !params.empty() )
		{
			castor::String strParams;
			params[0]->get( strParams );

			if ( parsingContext.vertexTan.empty() )
			{
				parsingContext.vertexTan.resize( parsingContext.vertexPos.size() );
			}

			auto arrayValues = castor::string::split( strParams, cuT( " " ), 20 );

			if ( arrayValues.size() >= 9 && parsingContext.face1 != -1 )
			{
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[0] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[1] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face1 + 0 )] * 3] = castor::string::toFloat( arrayValues[2] );
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[3] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[4] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face1 + 1 )] * 3] = castor::string::toFloat( arrayValues[5] );
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[6] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[7] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face1 + 2 )] * 3] = castor::string::toFloat( arrayValues[8] );
			}

			if ( arrayValues.size() >= 12 && parsingContext.face2 != -1 )
			{
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 0] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 1] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face2 + 0 )] * 3] = castor::string::toFloat( arrayValues[ 2] );
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 6] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 7] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face2 + 1 )] * 3] = castor::string::toFloat( arrayValues[ 8] );
				parsingContext.vertexTan[0 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[ 9] );
				parsingContext.vertexTan[1 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[10] );
				parsingContext.vertexTan[2 + parsingContext.faces[size_t( parsingContext.face2 + 2 )] * 3] = castor::string::toFloat( arrayValues[11] );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSubmeshEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.vertexPos.empty() )
		{
			if ( !parsingContext.submesh->hasComponent( PositionsComponent::Name ) )
			{
				parsingContext.submesh->createComponent< PositionsComponent >();
			}

			if ( !parsingContext.vertexNml.empty()
				&& !parsingContext.submesh->hasComponent( NormalsComponent::Name ) )
			{
				parsingContext.submesh->createComponent< NormalsComponent >();
			}

			if ( !parsingContext.vertexTan.empty()
				&& !parsingContext.submesh->hasComponent( TangentsComponent::Name ) )
			{
				parsingContext.submesh->createComponent< TangentsComponent >();
			}

			if ( !parsingContext.vertexTex.empty()
				&& !parsingContext.submesh->hasComponent( Texcoords0Component::Name ) )
			{
				parsingContext.submesh->createComponent< Texcoords0Component >();
			}

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
				auto mapping = parsingContext.submesh->createComponent< TriFaceMapping >();
				mapping->addFaceGroup( indices, indices + ( parsingContext.faces.size() / 3 ) );

				if ( !parsingContext.vertexNml.empty() )
				{
					if ( !parsingContext.vertexTex.empty()
						&& parsingContext.vertexTan.empty() )
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

			MeshPreparer::prepare( *parsingContext.submesh
				, Parameters{} );
			parsingContext.vertexPos.clear();
			parsingContext.vertexNml.clear();
			parsingContext.vertexTan.clear();
			parsingContext.vertexTex.clear();
			parsingContext.faces.clear();
			parsingContext.submesh->getParent().getScene()->getListener().postEvent( makeGpuInitialiseEvent( *parsingContext.submesh ) );
			parsingContext.submesh = {};
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserMaterialPass )
	{
		auto & parsingContext = getParserContext( context );
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
		}
		else
		{
			CU_ParsingError( cuT( "Material not initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::ePass )

	CU_ImplementAttributeParser( parserMaterialRenderPass )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.strName.clear();

		if ( !parsingContext.material )
		{
			CU_ParsingError( cuT( "Material not initialised" ) );
		}
		else if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameters" ) );
		}
		else
		{
			castor::String typeName;
			params[0]->get( typeName );
			parsingContext.material->setRenderPassInfo( parsingContext.parser->getEngine()->getRenderPassInfo( typeName ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserMaterialEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.ownMaterial
			&& !parsingContext.material )
		{
			CU_ParsingError( cuT( "Material not initialised" ) );
		}
		else if ( parsingContext.ownMaterial )
		{
			log::info << "Loaded material [" << parsingContext.material->getName() << "]" << std::endl;

			if ( parsingContext.scene )
			{
				parsingContext.scene->addMaterial( parsingContext.material->getName()
					, parsingContext.ownMaterial
					, true );
			}
			else
			{
				parsingContext.parser->getEngine()->addMaterial( parsingContext.material->getName()
					, parsingContext.ownMaterial
					, true );
			}
		}

		parsingContext.material = {};
		parsingContext.createMaterial = false;
		parsingContext.passIndex = 0u;
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserPassTextureUnit )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.pass )
		{
			parsingContext.textureTransform = TextureTransform{};

			if ( parsingContext.createPass
				|| parsingContext.pass->getTextureUnitsCount() <= parsingContext.unitIndex )
			{
				parsingContext.imageInfo->mipLevels = ashes::RemainingArrayLayers;
			}
			else
			{
				parsingContext.textureConfiguration = parsingContext.pass->getTextureUnit( parsingContext.unitIndex )->getConfiguration();
			}

			++parsingContext.unitIndex;
		}
		else
		{
			CU_ParsingError( cuT( "Pass not initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextureUnit )

	CU_ImplementAttributeParser( parserPassShader )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderProgram = {};
		parsingContext.shaderStage = VkShaderStageFlagBits( 0u );

		if ( parsingContext.pass )
		{
			auto & cache = parsingContext.parser->getEngine()->getShaderProgramCache();
			parsingContext.shaderProgram = cache.getNewProgram( parsingContext.material->getName() + castor::string::toString( parsingContext.pass->getId() )
				, true );
		}
		else
		{
			CU_ParsingError( cuT( "Pass not initialised" ) );
		}
	}
	CU_EndAttributePush( CSCNSection::eShaderProgram )

	CU_ImplementAttributeParser( parserPassEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.pass )
		{
			CU_ParsingError( cuT( "No Pass initialised." ) );
		}
		else
		{
			parsingContext.pass->prepareTextures();
			log::info << "Loaded pass [" << parsingContext.material->getName()
				<< ", " << parsingContext.pass->getIndex() << "]" << std::endl;
			parsingContext.pass = {};
			parsingContext.passComponent = nullptr;
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserUnitChannel )
	{
		auto & parsingContext = getParserContext( context );

		if ( !params.empty() )
		{
			PassComponentTextureFlag textures;
			params[0]->get( textures );
			auto & engine = *parsingContext.parser->getEngine();
			engine.getPassComponentsRegister().fillChannels( textures, parsingContext );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitImage )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			castor::Path folder;
			castor::Path relative;
			params[0]->get( relative );

			if ( castor::File::fileExists( context.file.getPath() / relative ) )
			{
				folder = context.file.getPath();
				auto & engine = *parsingContext.parser->getEngine();
				parsingContext.image = engine.tryFindImage( relative.getFileName() );

				if ( !parsingContext.image )
				{
					auto img = engine.createImage( relative.getFileName()
						, castor::ImageCreateParams{ folder / relative
							, { false, false, false } } );
					parsingContext.image = engine.addImage( relative.getFileName(), img );
				}
			}
			else if ( !castor::File::fileExists( relative ) )
			{
				CU_ParsingError( cuT( "File [" ) + relative + cuT( "] not found, check the relativeness of the path" ) );
				relative.clear();
			}

			if ( !relative.empty() )
			{
				parsingContext.folder = folder;
				parsingContext.relative = relative;
				parsingContext.strName.clear();
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitLevelsCount )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			params[0]->get( parsingContext.imageInfo->mipLevels );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitRenderTarget )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.targetType = TargetType::eTexture;
		parsingContext.size = { 1u, 1u };
		parsingContext.pixelFormat = castor::PixelFormat::eUNDEFINED;
	}
	CU_EndAttributePush( CSCNSection::eRenderTarget )

	CU_ImplementAttributeParser( parserUnitSampler )
	{
		auto & parsingContext = getParserContext( context );

		if ( !params.empty() )
		{
			castor::String name;
			auto sampler = parsingContext.parser->getEngine()->findSampler( params[0]->get( name ) );

			if ( sampler )
			{
				parsingContext.sampler = sampler;
			}
			else
			{
				CU_ParsingError( cuT( "Unknown sampler : [" ) + name + cuT( "]" ) );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitInvertY )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else
		{
			bool value;
			params[0]->get( value );
			parsingContext.textureConfiguration.needsYInversion = value
				? 1u
				: 0u;
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitTransform )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.pass )
		{
		}
	}
	CU_EndAttributePush( CSCNSection::eTextureTransform )

	CU_ImplementAttributeParser( parserUnitTileSet )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.pass )
		{
			castor::Point2i value;
			params[0]->get( value );
			parsingContext.textureConfiguration.tileSet->z = uint32_t( value->x );
			parsingContext.textureConfiguration.tileSet->w = uint32_t( value->y );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitTiles )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.pass )
		{
			params[0]->get( parsingContext.textureConfiguration.tiles );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitAnimation )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.pass )
		{
			parsingContext.textureAnimation = castor::makeUnique< TextureAnimation >( *parsingContext.parser->getEngine()
				, "Default" );
		}
	}
	CU_EndAttributePush( CSCNSection::eTextureAnimation )

	CU_ImplementAttributeParser( parserUnitTexcoordSet )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.pass )
		{
			params[0]->get( parsingContext.texcoordSet );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserUnitEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.pass )
		{
			if ( !parsingContext.sampler )
			{
				parsingContext.sampler = parsingContext.parser->getEngine()->getDefaultSampler();
			}

			TextureSourceInfo sourceInfo = ( parsingContext.textureRenderTarget
				? TextureSourceInfo{ parsingContext.sampler
					, parsingContext.textureRenderTarget }
				: TextureSourceInfo{ parsingContext.sampler
					, parsingContext.folder
					, parsingContext.relative } );
			parsingContext.textureConfiguration.transform = parsingContext.textureTransform;

			if ( parsingContext.textureAnimation && parsingContext.scene )
			{
				auto animated = parsingContext.scene->addAnimatedTexture( sourceInfo
					, parsingContext.textureConfiguration
					, *parsingContext.pass );
				parsingContext.textureAnimation->addPendingAnimated( *animated );
			}

			if ( parsingContext.textureRenderTarget )
			{
				parsingContext.pass->registerTexture( std::move( sourceInfo )
					, { std::move( parsingContext.textureConfiguration )
						, parsingContext.texcoordSet }
					, std::move( parsingContext.textureAnimation ) );
				parsingContext.textureRenderTarget = {};
			}
			else if ( parsingContext.folder.empty() && parsingContext.relative.empty() )
			{
				CU_ParsingError( cuT( "Texture unit's image not initialised" ) );
			}
			else
			{
				parsingContext.pass->registerTexture( std::move( sourceInfo )
					, { std::move( parsingContext.textureConfiguration )
						, parsingContext.texcoordSet }
					, std::move( parsingContext.textureAnimation ) );
			}

			parsingContext.imageInfo =
			{
				0u,
				VK_IMAGE_TYPE_2D,
				VK_FORMAT_UNDEFINED,
				{ 1u, 1u, 1u },
				1u,
				1u,
				VK_SAMPLE_COUNT_1_BIT,
				VK_IMAGE_TILING_OPTIMAL,
				VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT
			};
			parsingContext.image = {};
			parsingContext.textureConfiguration = TextureConfiguration{};
			parsingContext.texcoordSet = 0u;
			parsingContext.passComponent = nullptr;
		}
		else
		{
			CU_ParsingError( cuT( "Pass not initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserTexTransformRotate )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			float value;
			params[0]->get( value );
			parsingContext.textureTransform.rotate = castor::Angle::fromDegrees( value );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexTransformTranslate )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			parsingContext.textureTransform.translate = castor::Point3f{ value->x, value->y, 0.0f };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexTransformScale )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			castor::Point2f value;
			params[0]->get( value );
			parsingContext.textureTransform.scale = castor::Point3f{ value->x, value->y, 1.0f };
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexTile )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			castor::Point2i value;
			params[0]->get( value );
			parsingContext.textureConfiguration.tileSet->x = uint32_t( value->x );
			parsingContext.textureConfiguration.tileSet->y = uint32_t( value->y );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexAnimRotate )
	{
		auto & parsingContext = getParserContext( context );

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
			parsingContext.textureAnimation->setRotateSpeed( TextureRotateSpeed{ castor::Angle::fromDegrees( value ) } );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserTexAnimTranslate )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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

	CU_ImplementAttributeParser( parserTexAnimTile )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter" ) );
		}
		else
		{
			bool value;
			params[0]->get( value );

			if ( value )
			{
				parsingContext.textureAnimation->enableTileAnim();
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserVertexShader )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_VERTEX_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserPixelShader )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserGeometryShader )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserHullShader )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserDomainShader )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserComputeShader )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.shaderStage = VK_SHADER_STAGE_COMPUTE_BIT;
	}
	CU_EndAttributePush( CSCNSection::eShaderStage )

	CU_ImplementAttributeParser( parserConstantsBuffer )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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

			parsingContext.shaderProgram = {};
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserShaderProgramFile )
	{
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.shaderStage != VkShaderStageFlagBits( 0u ) )
			{
				castor::Path path;
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
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.shaderProgram )
		{
			CU_ParsingError( cuT( "No ShaderProgram initialised." ) );
		}
		else if ( !params.empty() )
		{
			if ( parsingContext.shaderStage != VkShaderStageFlagBits( 0u ) )
			{
				castor::Point3i sizes;
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
				//auto & parsingContext = getParserContext( context );
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
		auto & parsingContext = getParserContext( context );
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
		auto & parsingContext = getParserContext( context );
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
		auto & parsingContext = getParserContext( context );
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
		//auto & parsingContext = getParserContext( context );
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
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.path );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontHeight )
	{
		auto & parsingContext = getParserContext( context );
		params[0]->get( parsingContext.fontHeight );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserFontEnd )
	{
		auto & parsingContext = getParserContext( context );

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

			log::info << "Loaded font [" << parsingContext.strName << "]" << std::endl;
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserOverlayPosition )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.overlay.rptr )
		{
			castor::Point2d ptPosition;
			params[0]->get( ptPosition );
			parsingContext.overlay.rptr->setRelativePosition( ptPosition );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlaySize )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.overlay.rptr )
		{
			castor::Point2d ptSize;
			params[0]->get( ptSize );
			parsingContext.overlay.rptr->setRelativeSize( ptSize );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPixelSize )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.overlay.rptr )
		{
			castor::Size size;
			params[0]->get( size );
			parsingContext.overlay.rptr->setPixelSize( size );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPixelPosition )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.overlay.rptr )
		{
			castor::Position position;
			params[0]->get( position );
			parsingContext.overlay.rptr->setPixelPosition( position );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayMaterial )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.overlay.rptr )
		{
			castor::String name;
			params[0]->get( name );
			parsingContext.overlay.rptr->setMaterial( parsingContext.parser->getEngine()->findMaterial( name ) );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserOverlayPanelOverlay )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
		auto & parent = parsingContext.parentOverlays.back();
		parsingContext.overlay.rptr = parsingContext.scene
			? parsingContext.scene->tryFindOverlay( params[0]->get( name ) )
			: parsingContext.parser->getEngine()->tryFindOverlay( params[0]->get( name ) );

		if ( !parsingContext.overlay.rptr )
		{
			parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
				, OverlayType::ePanel
				, parent.rptr ? parent.rptr->getScene() : parsingContext.scene
				, parent.rptr );
			parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
			parsingContext.overlay.rptr->rename( name );
		}

		parsingContext.overlay.rptr->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::ePanelOverlay )

	CU_ImplementAttributeParser( parserOverlayBorderPanelOverlay )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
		auto & parent = parsingContext.parentOverlays.back();
		parsingContext.overlay.rptr = parsingContext.scene
			? parsingContext.scene->tryFindOverlay( params[0]->get( name ) )
			: parsingContext.parser->getEngine()->tryFindOverlay( params[0]->get( name ) );

		if ( !parsingContext.overlay.rptr )
		{
			parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
				, OverlayType::eBorderPanel
				, parent.rptr ? parent.rptr->getScene() : parsingContext.scene
				, parent.rptr );
			parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
			parsingContext.overlay.rptr->rename( name );
		}

		parsingContext.overlay.rptr->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::eBorderPanelOverlay )

	CU_ImplementAttributeParser( parserOverlayTextOverlay )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		parsingContext.parentOverlays.push_back( std::move( parsingContext.overlay ) );
		auto & parent = parsingContext.parentOverlays.back();
		parsingContext.overlay.rptr = parsingContext.scene
			? parsingContext.scene->tryFindOverlay( params[0]->get( name ) )
			: parsingContext.parser->getEngine()->tryFindOverlay( params[0]->get( name ) );

		if ( !parsingContext.overlay.rptr )
		{
			parsingContext.overlay.uptr = castor::makeUnique< Overlay >( *parsingContext.parser->getEngine()
				, OverlayType::eText
				, parent.rptr ? parent.rptr->getScene() : parsingContext.scene
				, parent.rptr );
			parsingContext.overlay.rptr = parsingContext.overlay.uptr.get();
			parsingContext.overlay.rptr->rename( name );
		}

		parsingContext.overlay.rptr->setVisible( false );
	}
	CU_EndAttributePush( CSCNSection::eTextOverlay )

	CU_ImplementAttributeParser( parserOverlayEnd )
	{
		auto & parsingContext = getParserContext( context );
		log::info << "Loaded overlay [" << parsingContext.overlay.rptr->getName() << "]" << std::endl;

		if ( parsingContext.overlay.rptr->getType() == OverlayType::eText )
		{
			auto textOverlay = parsingContext.overlay.rptr->getTextOverlay();

			if ( textOverlay->getFontTexture() )
			{
				parsingContext.overlay.rptr->setVisible( true );
			}
			else
			{
				parsingContext.overlay.rptr->setVisible( false );
				CU_ParsingError( cuT( "TextOverlay's font has not been set, it will not be rendered" ) );
			}
		}
		else
		{
			parsingContext.overlay.rptr->setVisible( true );
		}

		if ( parsingContext.overlay.uptr )
		{
			if ( parsingContext.scene )
			{
				parsingContext.scene->addOverlay( parsingContext.overlay.rptr->getName()
					, parsingContext.overlay.uptr
					, true );
			}
			else
			{
				parsingContext.parser->getEngine()->addOverlay( parsingContext.overlay.rptr->getName()
					, parsingContext.overlay.uptr
					, true );
			}
		}

		CU_Require( !parsingContext.parentOverlays.empty() );
		parsingContext.overlay = std::move( parsingContext.parentOverlays.back() );
		parsingContext.parentOverlays.pop_back();
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserPanelOverlayUvs )
	{
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

		if ( overlay && overlay->getType() == OverlayType::ePanel )
		{
			castor::Point4d uvs;
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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			castor::Point4d ptSize;
			params[0]->get( ptSize );
			overlay->getBorderPanelOverlay()->setRelativeBorderSize( ptSize );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayPixelSizes )
	{
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			castor::Point4ui size;
			params[0]->get( size );
			overlay->getBorderPanelOverlay()->setPixelBorderSize( size );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayMaterial )
	{
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			castor::String name;
			params[0]->get( name );
			overlay->getBorderPanelOverlay()->setBorderMaterial( parsingContext.parser->getEngine()->findMaterial( name ) );
		}
		else
		{
			CU_ParsingError( cuT( "Overlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBorderPanelOverlayPosition )
	{
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			castor::Point4d uvs;
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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			castor::Point4d uvs;
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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

		if ( overlay && overlay->getType() == OverlayType::eBorderPanel )
		{
			castor::Point4d uvs;
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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			auto & cache = parsingContext.parser->getEngine()->getFontCache();
			castor::String name;
			params[0]->get( name );

			if ( cache.find( name ) )
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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;

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
		auto & parsingContext = getParserContext( context );
		auto overlay = parsingContext.overlay.rptr;
		castor::String strParams;
		params[0]->get( strParams );

		if ( overlay && overlay->getType() == OverlayType::eText )
		{
			castor::string::replace( strParams, cuT( "\\n" ), cuT( "\n" ) );
			overlay->getTextOverlay()->setCaption( castor::string::stringCast< char32_t >( strParams ) );
		}
		else
		{
			CU_ParsingError( cuT( "TextOverlay not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraParent )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		SceneNodeRPtr parent = parsingContext.scene->findSceneNode( params[0]->get( name ) );

		if ( parent )
		{
			while ( parent->getParent()
				&& parent->getParent() != parsingContext.scene->getObjectRootNode()
				&& parent->getParent() != parsingContext.scene->getCameraRootNode() )
			{
				parent = parent->getParent();
			}

			if ( !parent->getParent()
				|| parent->getParent() == parsingContext.scene->getObjectRootNode() )
			{
				parent->attachTo( *parsingContext.scene->getCameraRootNode() );
			}

			parsingContext.parentNode = parent;
		}
		else
		{
			CU_ParsingError( cuT( "Node [" ) + name + cuT( "] does not exist" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserCameraViewport )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.viewport = castor::makeUnique< Viewport >( *parsingContext.parser->getEngine() );
		parsingContext.viewport->setPerspective( 0.0_degrees, 1, 0, 1 );
	}
	CU_EndAttributePush( CSCNSection::eViewport )
		
	CU_ImplementAttributeParser( parserCameraHdrConfig )
	{
	}
	CU_EndAttributePush( CSCNSection::eHdrConfig )

	CU_ImplementAttributeParser( parserCameraPrimitive )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.viewport )
		{
			auto node = parsingContext.parentNode;

			if ( !node )
			{
				node = parsingContext.scene->getCameraRootNode();
			}

			parsingContext.parentNode = nullptr;
			auto camera = parsingContext.scene->addNewCamera( parsingContext.strName
				, *parsingContext.scene
				, *node
				, std::move( *parsingContext.viewport.release() ) );
			camera->setGamma( parsingContext.point2f[0] );
			camera->setExposure( parsingContext.point2f[1] );
			log::info << "Loaded camera [" << camera->getName() << "]" << std::endl;
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserViewportType )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateLeft( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportRight )
	{
		auto & parsingContext = getParserContext( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateRight( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportTop )
	{
		auto & parsingContext = getParserContext( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateTop( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportBottom )
	{
		auto & parsingContext = getParserContext( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateBottom( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportNear )
	{
		auto & parsingContext = getParserContext( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateNear( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportFar )
	{
		auto & parsingContext = getParserContext( context );
		float rValue;
		params[0]->get( rValue );
		parsingContext.viewport->updateFar( rValue );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportSize )
	{
		auto & parsingContext = getParserContext( context );
		castor::Size size;
		params[0]->get( size );
		parsingContext.viewport->resize( size );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportFovY )
	{
		auto & parsingContext = getParserContext( context );
		float fFovY;
		params[0]->get( fFovY );
		parsingContext.viewport->updateFovY( castor::Angle::fromDegrees( fFovY ) );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserViewportAspectRatio )
	{
		auto & parsingContext = getParserContext( context );
		float fRatio;
		params[0]->get( fRatio );
		parsingContext.viewport->updateRatio( fRatio );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardParent )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.billboards )
		{
			castor::String name;
			params[0]->get( name );

			if ( auto parent = parsingContext.scene->findSceneNode( name ) )
			{
				parent->attachObject( *parsingContext.billboards );
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

	CU_ImplementAttributeParser( parserBillboardType )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.billboards )
		{
			castor::String name;
			auto material = parsingContext.parser->getEngine()->tryFindMaterial( params[0]->get( name ) );

			if ( material )
			{
				parsingContext.billboards->setMaterial( material );
			}
			else
			{
				CU_ParsingError( cuT( "Material [" ) + name + cuT( "] does not exist" ) );
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
		auto & parsingContext = getParserContext( context );
		castor::Point2f dimensions;
		params[0]->get( dimensions );
		parsingContext.billboards->setDimensions( dimensions );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserBillboardEnd )
	{
		auto & parsingContext = getParserContext( context );
		parsingContext.parentNode = nullptr;
		log::info << "Loaded billboards [" << parsingContext.billboards->getName() << "]" << std::endl;

		if ( parsingContext.ownBillboards )
		{
			parsingContext.scene->addBillboardList( parsingContext.billboards->getName()
				, parsingContext.ownBillboards
				, true );
		}

		parsingContext.billboards = {};
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserBillboardPoint )
	{
		auto & parsingContext = getParserContext( context );
		castor::Point3f position;
		params[0]->get( position );
		parsingContext.billboards->addPoint( position );
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimatedObject )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		params[0]->get( name );

		if ( parsingContext.animGroup )
		{
			if ( auto geometry = parsingContext.scene->findGeometry( name ) )
			{
				auto node = geometry->getParent();

				if ( node && node->hasAnimation() )
				{
					parsingContext.animNode = parsingContext.animGroup->addObject( *node
						, node->getName() );
				}

				if ( auto mesh = geometry->getMesh() )
				{
					if ( mesh->hasAnimation() )
					{
						parsingContext.animMesh = parsingContext.animGroup->addObject( *mesh
							, *geometry
							, geometry->getName() );
					}

					if ( auto skeleton = mesh->getSkeleton() )
					{
						if ( skeleton->hasAnimation() )
						{
							parsingContext.animSkeleton = parsingContext.animGroup->addObject( *skeleton
								, *mesh
								, *geometry
								, geometry->getName() );
						}
					}
				}
			}
			else
			{
				if ( auto node = parsingContext.scene->findSceneNode( name ) )
				{
					if ( node->hasAnimation() )
					{
						parsingContext.animNode = parsingContext.animGroup->addObject( *node
							, node->getName() );
					}
				}
				else
				{
					CU_ParsingError( cuT( "No geometry or node with name " ) + name );
				}
			}
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimatedMesh )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		params[0]->get( name );

		if ( parsingContext.animGroup )
		{
			if ( auto geometry = parsingContext.scene->findGeometry( name ) )
			{
				if ( auto mesh = geometry->getMesh() )
				{
					if ( mesh->hasAnimation() )
					{
						parsingContext.animMesh = parsingContext.animGroup->addObject( *mesh
							, *geometry
							, geometry->getName() );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Geometry [" ) + name + "] has no mesh" );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No geometry with name [" ) + name + "]" );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimatedSkeleton )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		params[0]->get( name );

		if ( parsingContext.animGroup )
		{
			if ( auto geometry = parsingContext.scene->findGeometry( name ) )
			{
				if ( auto mesh = geometry->getMesh() )
				{
					if ( auto skeleton = mesh->getSkeleton() )
					{
						if ( skeleton->hasAnimation() )
						{
							parsingContext.animSkeleton = parsingContext.animGroup->addObject( *skeleton
								, *mesh
								, *geometry
								, geometry->getName() );
						}
					}
					else
					{
						CU_ParsingError( cuT( "Geometry [" ) + name + "]'s mesh has no skeleton" );
					}
				}
				else
				{
					CU_ParsingError( cuT( "Geometry [" ) + name + "] has no mesh" );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No geometry with name [" ) + name + "]" );
			}
		}
		else
		{
			CU_ParsingError( cuT( "No animated object group not initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserAnimatedObjectGroupAnimatedNode )
	{
		auto & parsingContext = getParserContext( context );
		castor::String name;
		params[0]->get( name );

		if ( parsingContext.animGroup )
		{
			if ( auto node = parsingContext.scene->findSceneNode( name ) )
			{
				if ( node->hasAnimation() )
				{
					parsingContext.animNode = parsingContext.animGroup->addObject( *node
						, node->getName() );
				}
			}
			else
			{
				CU_ParsingError( cuT( "No node with name [" ) + name + "]" );
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
		auto & parsingContext = getParserContext( context );
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
		auto & parsingContext = getParserContext( context );
		castor::String name;
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
		auto & parsingContext = getParserContext( context );
		castor::String name;
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
		auto & parsingContext = getParserContext( context );

		if ( !parsingContext.animGroup )
		{
			CU_ParsingError( cuT( "No animated object group initialised" ) );
		}

		log::info << "Loaded animated object group [" << parsingContext.animGroup->getName() << "]" << std::endl;
		parsingContext.animGroup = {};
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserAnimationLooped )
	{
		auto & parsingContext = getParserContext( context );
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
		auto & parsingContext = getParserContext( context );
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
		auto & parsingContext = getParserContext( context );
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
		auto & parsingContext = getParserContext( context );
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

	CU_ImplementAttributeParser( parserAnimationInterpolation )
	{
		auto & parsingContext = getParserContext( context );
		uint32_t value;
		params[0]->get( value );

		if ( parsingContext.animGroup )
		{
			parsingContext.animGroup->setAnimationInterpolation( parsingContext.strName2
				, InterpolatorType( value ) );
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

	CU_ImplementAttributeParser( parserSkyboxVisible )
	{
		auto & parsingContext = getParserContext( context );

		if ( params.empty() )
		{
			CU_ParsingError( cuT( "Missing parameter." ) );
		}
		else if ( !parsingContext.skybox )
		{
			CU_ParsingError( cuT( "No skybox initialised." ) );
		}
		else
		{
			parsingContext.skybox->setVisible( params[0]->get< bool >() );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxEqui )
	{
		auto & parsingContext = getParserContext( context );

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
			auto path = params[0]->get< castor::Path >();
			auto filePath = context.file.getPath();

			if ( castor::File::fileExists( filePath / path ) )
			{
				uint32_t size;
				params[1]->get( size );
				parsingContext.skybox->setEquiTexture( filePath, path, size );
			}
			else
			{
				parsingContext.skybox.reset();
				castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
				CU_ParsingError( err );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxCross )
	{
		auto & parsingContext = getParserContext( context );

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
			auto path = params[0]->get< castor::Path >();
			auto filePath = context.file.getPath();

			if ( castor::File::fileExists( filePath / path ) )
			{
				parsingContext.skybox->setCrossTexture( filePath, path );
			}
			else
			{
				parsingContext.skybox.reset();
				castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
				CU_ParsingError( err );
			}
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxLeft )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.skybox )
		{
			auto path = params[0]->get< castor::Path >();
			auto filePath = context.file.getPath();

			if ( castor::File::fileExists( filePath / path ) )
			{
				parsingContext.skybox->setLeftImage( filePath, path );
			}
			else
			{
				parsingContext.skybox.reset();
				castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
				CU_ParsingError( err );
			}
		}
		else
		{
			parsingContext.skybox.reset();
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxRight )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.skybox )
		{
			auto path = params[0]->get< castor::Path >();
			auto filePath = context.file.getPath();

			if ( castor::File::fileExists( filePath / path ) )
			{
				parsingContext.skybox->setRightImage( filePath, path );
			}
			else
			{
				parsingContext.skybox.reset();
				castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
				CU_ParsingError( err );
			}
		}
		else
		{
			parsingContext.skybox.reset();
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxTop )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.skybox )
		{
			auto path = params[0]->get< castor::Path >();
			auto filePath = context.file.getPath();

			if ( castor::File::fileExists( filePath / path ) )
			{
				parsingContext.skybox->setTopImage( filePath, path );
			}
			else
			{
				parsingContext.skybox.reset();
				castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
				CU_ParsingError( err );
			}
		}
		else
		{
			parsingContext.skybox.reset();
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxBottom )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.skybox )
		{
			auto path = params[0]->get< castor::Path >();
			auto filePath = context.file.getPath();

			if ( castor::File::fileExists( filePath / path ) )
			{
				parsingContext.skybox->setBottomImage( filePath, path );
			}
			else
			{
				parsingContext.skybox.reset();
				castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
				CU_ParsingError( err );
			}
		}
		else
		{
			parsingContext.skybox.reset();
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxFront )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.skybox )
		{
			auto path = params[0]->get< castor::Path >();
			auto filePath = context.file.getPath();

			if ( castor::File::fileExists( filePath / path ) )
			{
				parsingContext.skybox->setFrontImage( filePath, path );
			}
			else
			{
				parsingContext.skybox.reset();
				castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
				CU_ParsingError( err );
			}
		}
		else
		{
			parsingContext.skybox.reset();
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxBack )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.skybox )
		{
			auto path = params[0]->get< castor::Path >();
			auto filePath = context.file.getPath();

			if ( castor::File::fileExists( filePath / path ) )
			{
				parsingContext.skybox->setBackImage( filePath, path );
			}
			else
			{
				parsingContext.skybox.reset();
				castor::String err = cuT( "Couldn't load the image file [" ) + path + cuT( "] (file does not exist)" );
				CU_ParsingError( err );
			}
		}
		else
		{
			parsingContext.skybox.reset();
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttribute()

	CU_ImplementAttributeParser( parserSkyboxEnd )
	{
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.skybox )
		{
			log::info << "Loaded skybox" << std::endl;
			parsingContext.scene->setBackground( castor::ptrRefCast< SceneBackground >( parsingContext.skybox ) );
		}
		else
		{
			CU_ParsingError( cuT( "No skybox initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserSsaoEnabled )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

		if ( parsingContext.renderTarget )
		{
			log::info << "Loaded SSAO configuration" << std::endl;
			parsingContext.renderTarget->setSsaoConfig( parsingContext.ssaoConfig );
			parsingContext.ssaoConfig = {};
		}
		else
		{
			CU_ParsingError( cuT( "No render target initialised" ) );
		}
	}
	CU_EndAttributePop()

	CU_ImplementAttributeParser( parserHdrExponent )
	{
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
		auto & parsingContext = getParserContext( context );

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
