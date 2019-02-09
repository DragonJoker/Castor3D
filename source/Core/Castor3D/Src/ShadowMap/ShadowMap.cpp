#include "ShadowMap.hpp"

#include "Engine.hpp"

#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderPipeline.hpp"
#include "Scene/Light/Light.hpp"
#include "Shader/Program.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Shader/Ubos/MatrixUbo.hpp"
#include "Shader/Ubos/ModelMatrixUbo.hpp"
#include "Shader/Ubos/ModelUbo.hpp"
#include "Shader/Ubos/MorphingUbo.hpp"
#include "Shader/Ubos/SceneUbo.hpp"
#include "Shader/Ubos/SkinningUbo.hpp"
#include "ShadowMap/ShadowMapPass.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Image/Texture.hpp>
#include <Image/TextureView.hpp>
#include <RenderPass/RenderPass.hpp>
#include <RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include "Shader/Shaders/GlslMaterial.hpp"

using namespace castor;

namespace castor3d
{
	ShadowMap::ShadowMap( Engine & engine
		, TextureUnit && shadowMap
		, TextureUnit && linearMap
		, std::vector< PassData > && passes
		, uint32_t count )
		: OwnedBy< Engine >{ engine }
		, m_shadowMap{ std::move( shadowMap ) }
		, m_linearMap{ std::move( linearMap ) }
		, m_passes{ std::move( passes ) }
		, m_count{ count }
	{
	}

	ShadowMap::~ShadowMap()
	{
	}

	bool ShadowMap::initialise()
	{
		bool result = true;

		if ( !m_initialised )
		{
			m_shadowMap.initialise();
			m_linearMap.initialise();
			auto size = m_shadowMap.getTexture()->getDimensions();
			auto & device = getCurrentDevice( *this );

			for ( auto & pass : m_passes )
			{
				pass.matrixUbo->initialise();
				result &= pass.pass->initialise( { size.width, size.height } );
			}

			if ( result )
			{
				m_finished = device.createSemaphore();
				doInitialise();
				m_initialised = true;
			}
		}

		return result;
	}

	void ShadowMap::cleanup()
	{
		m_finished.reset();

		for ( auto & pass : m_passes )
		{
			pass.pass->cleanup();
			pass.matrixUbo->cleanup();
		}

		if ( m_initialised )
		{
			m_initialised = false;
			doCleanup();
			m_shadowMap.cleanup();
			m_linearMap.cleanup();
		}
	}

	void ShadowMap::updateFlags( PassFlags & passFlags
		, TextureChannels & textureFlags
		, ProgramFlags & programFlags
		, SceneFlags & sceneFlags )const
	{
		remFlag( programFlags, ProgramFlag::eLighting );
		remFlag( passFlags, PassFlag::eAlphaBlending );
		remFlag( textureFlags, TextureChannel( uint16_t( TextureChannel::eAll )
			& ~uint16_t( TextureChannel::eOpacity ) ) );
		doUpdateFlags( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	ShaderPtr ShadowMap::getVertexShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, bool invertNormals )const
	{
		return doGetVertexShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, invertNormals );
	}

	ShaderPtr ShadowMap::getGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return doGetGeometryShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags );
	}

	ShaderPtr ShadowMap::getPixelShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags
		, ashes::CompareOp alphaFunc )const
	{
		return doGetPixelShaderSource( passFlags
			, textureFlags
			, programFlags
			, sceneFlags
			, alphaFunc );
	}

	ashes::Sampler const & ShadowMap::getSampler()const
	{
		return m_shadowMap.getSampler()->getSampler();
	}

	ashes::TextureView const & ShadowMap::getView()const
	{
		return m_shadowMap.getTexture()->getDefaultView();
	}

	ashes::TextureView const & ShadowMap::getView( uint32_t index )const
	{
		return m_shadowMap.getTexture()->getDefaultView();
	}

	ShaderPtr ShadowMap::doGetGeometryShaderSource( PassFlags const & passFlags
		, TextureChannels const & textureFlags
		, ProgramFlags const & programFlags
		, SceneFlags const & sceneFlags )const
	{
		return ShaderPtr{};
	}

	void ShadowMap::doDiscardAlpha( sdw::ShaderWriter & writer
		, TextureChannels const & textureFlags
		, ashes::CompareOp alphaFunc
		, sdw::UInt const & materialIndex
		, shader::Materials const & materials )const
	{
		using namespace sdw;

		if ( checkFlag( textureFlags, TextureChannel::eOpacity ) )
		{
			auto material = materials.getBaseMaterial( materialIndex );
			auto alpha = writer.declLocale( "alpha"
				, material->m_opacity );
			auto alphaRef = writer.declLocale( "alphaRef"
				, material->m_alphaRef );

			auto c3d_mapOpacity = writer.getVariable< sdw::SampledImage2DRgba32 >( "c3d_mapOpacity" );
			auto vtx_texture = writer.getVariable< sdw::Vec3 >( "vtx_texture" );
			alpha *= texture( c3d_mapOpacity, vtx_texture.xy() ).r();
			shader::applyAlphaFunc( writer
				, alphaFunc
				, alpha
				, alphaRef );
		}
		else if ( alphaFunc != ashes::CompareOp::eAlways )
		{
			auto material = materials.getBaseMaterial( materialIndex );
			auto alpha = writer.declLocale( "alpha"
				, material->m_opacity );
			auto alphaRef = writer.declLocale( "alphaRef"
				, material->m_alphaRef );

			shader::applyAlphaFunc( writer
				, alphaFunc
				, alpha
				, alphaRef );
		}
	}
}
