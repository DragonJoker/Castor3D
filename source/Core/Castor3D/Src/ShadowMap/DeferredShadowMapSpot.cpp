#include "DeferredShadowMapSpot.hpp"

#include <Engine.hpp>

#include <FrameBuffer/FrameBuffer.hpp>
#include <FrameBuffer/TextureAttachment.hpp>
#include <Render/RenderSystem.hpp>
#include <Scene/Light/Light.hpp>
#include <Scene/Light/SpotLight.hpp>
#include <Shader/ShaderProgram.hpp>
#include <ShadowMap/ShadowMapPassSpot.hpp>
#include <Texture/Sampler.hpp>
#include <Texture/TextureImage.hpp>
#include <Texture/TextureLayout.hpp>

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>
#include <Miscellaneous/BlockTracker.hpp>

using namespace castor;
using namespace castor3d;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseSpot( Engine & engine, Size const & p_size )
		{
			auto sampler = engine.getSamplerCache().add( cuT( "ShadowMap_Spot" ) );
			sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
			sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
			sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
			sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
			sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
			sampler->setComparisonMode( ComparisonMode::eRefToTexture );
			sampler->setComparisonFunc( ComparisonFunc::eLEqual );

			auto texture = engine.getRenderSystem()->createTexture( TextureType::eTwoDimensions
				, AccessType::eNone
				, AccessType::eRead | AccessType::eWrite
				, PixelFormat::eD32F
				, p_size );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}

		void doSavePic( TextureLayout & p_texture
			, String const & p_name )
		{
			auto data = p_texture.lock( AccessType::eRead );

			if ( data )
			{
				auto buffer = PxBuffer< PixelFormat::eA8R8G8B8 >{ p_texture.getDimensions()
					, data
					, p_texture.getPixelFormat() };

				for ( auto & pixel : buffer )
				{
					auto value = uint8_t( 255.0f - ( 255.0f - *pixel.begin() ) * 25.0f );

					for ( auto & PixelComponents : pixel )
					{
						PixelComponents = value;
					}
				}

				Image::BinaryWriter()( Image{ cuT( "tmp" ), buffer }
					, Engine::getEngineDirectory() / p_name );
				p_texture.unlock( false );
			}
		}
	}

	DeferredShadowMapSpot::DeferredShadowMapSpot( Engine & engine )
		: ShadowMap{ engine }
		, m_shadowMap{ doInitialiseSpot( engine, Size{ 1024, 1024 } ) }
	{
	}

	DeferredShadowMapSpot::~DeferredShadowMapSpot()
	{
	}
	
	void DeferredShadowMapSpot::update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		for ( auto & pass : m_passes )
		{
			pass.second->update( p_queues, 0 );
		}
	}

	void DeferredShadowMapSpot::render( SpotLight const & p_light )
	{
		auto it = m_passes.find( &p_light.getLight() );
		REQUIRE( it != m_passes.end() && "Light not found, call addLight..." );
		m_frameBuffer->bind( FrameBufferTarget::eDraw );
		m_frameBuffer->clear( BufferComponent::eDepth );
		it->second->render();
		m_frameBuffer->unbind();
	}

	int32_t DeferredShadowMapSpot::doGetMaxPasses()const
	{
		return 1;
	}

	Size DeferredShadowMapSpot::doGetSize()const
	{
		return m_shadowMap.getTexture()->getDimensions();
	}

	void DeferredShadowMapSpot::doInitialise()
	{
		m_shadowMap.initialise();
		m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );
		auto texture = m_shadowMap.getTexture();
		m_depthAttach = m_frameBuffer->createAttachment( texture );
		m_frameBuffer->bind();
		m_frameBuffer->attach( AttachmentPoint::eDepth, m_depthAttach, m_shadowMap.getTexture()->getType() );
		ENSURE( m_frameBuffer->isComplete() );
		m_frameBuffer->unbind();
	}

	void DeferredShadowMapSpot::doCleanup()
	{
		m_depthAttach.reset();
		m_shadowMap.cleanup();
	}

	ShadowMapPassSPtr DeferredShadowMapSpot::doCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassSpot >( *getEngine(), p_light, *this );
	}

	void DeferredShadowMapSpot::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapSpot );
	}

	GLSL::Shader DeferredShadowMapSpot::doGetPixelShaderSource( TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ComparisonFunc alphaFunc )const
	{
		using namespace GLSL;
		GlslWriter writer = getEngine()->getRenderSystem()->createGlslWriter();

		// Fragment Intputs
		auto vtx_texture = writer.declInput< Vec3 >( cuT( "vtx_texture" ) );
		auto c3d_mapOpacity( writer.declUniform< Sampler2D >( ShaderProgram::MapOpacity, checkFlag( textureFlags, TextureChannel::eOpacity ) ) );
		auto gl_FragCoord( writer.declBuiltin< Vec4 >( cuT( "gl_FragCoord" ) ) );

		// Fragment Outputs
		auto pxl_fFragDepth( writer.declFragData< Float >( cuT( "pxl_fFragDepth" ), 0 ) );

		writer.implementFunction< void >( cuT( "main" ), [&]()
		{
			if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
			{
				auto alpha = writer.declLocale( cuT( "alpha" ), texture( c3d_mapOpacity, vtx_texture.xy() ).r() );

				IF( writer, alpha < 0.2_f )
				{
					writer.discard();
				}
				FI;
			}

			pxl_fFragDepth = gl_FragCoord.z();
		} );

		return writer.finalise();
	}
}
