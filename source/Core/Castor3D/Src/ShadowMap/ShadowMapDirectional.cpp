#include "ShadowMapDirectional.hpp"

#include "Engine.hpp"

#include "FrameBuffer/FrameBuffer.hpp"
#include "FrameBuffer/TextureAttachment.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/ShaderProgram.hpp"
#include "ShadowMap/ShadowMapPassDirectional.hpp"
#include "Texture/Sampler.hpp"
#include "Texture/TextureImage.hpp"
#include "Texture/TextureLayout.hpp"

#include <GlslSource.hpp>

#include <Graphics/Image.hpp>

using namespace castor;

namespace castor3d
{
	namespace
	{
		TextureUnit doInitialiseDirectional( Engine & engine, Size const & p_size )
		{
			SamplerSPtr sampler;
			String const name = cuT( "ShadowMap_Directional" );

			if ( engine.getSamplerCache().has( name ) )
			{
				sampler = engine.getSamplerCache().find( name );
			}
			else
			{
				sampler = engine.getSamplerCache().add( name );
				sampler->setInterpolationMode( InterpolationFilter::eMin, InterpolationMode::eLinear );
				sampler->setInterpolationMode( InterpolationFilter::eMag, InterpolationMode::eLinear );
				sampler->setWrappingMode( TextureUVW::eU, WrapMode::eClampToBorder );
				sampler->setWrappingMode( TextureUVW::eV, WrapMode::eClampToBorder );
				sampler->setWrappingMode( TextureUVW::eW, WrapMode::eClampToBorder );
				sampler->setComparisonMode( ComparisonMode::eRefToTexture );
				sampler->setComparisonFunc( ComparisonFunc::eLEqual );
			}

			auto texture = engine.getRenderSystem()->createTexture(
				TextureType::eTwoDimensions,
				AccessType::eNone,
				AccessType::eRead | AccessType::eWrite,
				PixelFormat::eD32F, p_size );
			TextureUnit unit{ engine };
			unit.setTexture( texture );
			unit.setSampler( sampler );

			for ( auto & image : *texture )
			{
				image->initialiseSource();
			}

			return unit;
		}
	}

	ShadowMapDirectional::ShadowMapDirectional( Engine & engine )
		: ShadowMap{ engine }
		, m_shadowMap{ doInitialiseDirectional( engine, Size{ 4096, 4096 } ) }
	{
	}

	ShadowMapDirectional::~ShadowMapDirectional()
	{
	}

	void ShadowMapDirectional::update( Camera const & p_camera
		, RenderQueueArray & p_queues )
	{
		if ( !m_passes.empty() )
		{
			for ( auto & it : m_passes )
			{
				it.second->update( p_queues, 0u );
			}
		}
	}

	void ShadowMapDirectional::render()
	{
		if ( !m_passes.empty() )
		{
			m_frameBuffer->bind( FrameBufferTarget::eDraw );
			m_depthAttach->attach( AttachmentPoint::eDepth );
			m_frameBuffer->clear( BufferComponent::eDepth );
			m_passes.begin()->second->render();
			m_depthAttach->detach();
			m_frameBuffer->unbind();
		}
	}

	int32_t ShadowMapDirectional::doGetMaxPasses()const
	{
		return 1;
	}

	Size ShadowMapDirectional::doGetSize()const
	{
		return m_shadowMap.getTexture()->getDimensions();
	}

	void ShadowMapDirectional::doInitialise()
	{
		m_shadowMap.initialise();
		m_frameBuffer->setClearColour( Colour::fromPredefined( PredefinedColour::eOpaqueBlack ) );

		auto texture = m_shadowMap.getTexture();
		m_depthAttach = m_frameBuffer->createAttachment( texture );
		m_depthAttach->setTarget( texture->getType() );
	}

	void ShadowMapDirectional::doCleanup()
	{
		m_depthAttach.reset();
		m_shadowMap.cleanup();
	}

	ShadowMapPassSPtr ShadowMapDirectional::doCreatePass( Light & p_light )const
	{
		return std::make_shared< ShadowMapPassDirectional >( *getEngine(), p_light, *this );
	}

	void ShadowMapDirectional::doUpdateFlags( TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		addFlag( programFlags, ProgramFlag::eShadowMapDirectional );
	}

	GLSL::Shader ShadowMapDirectional::doGetPixelShaderSource( TextureChannels const & textureFlags
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
