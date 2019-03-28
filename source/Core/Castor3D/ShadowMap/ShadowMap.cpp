#include "Castor3D/ShadowMap/ShadowMap.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Event/Frame/FunctorEvent.hpp"
#include "Castor3D/Mesh/Submesh.hpp"
#include "Castor3D/Render/RenderPipeline.hpp"
#include "Castor3D/Scene/Light/Light.hpp"
#include "Castor3D/Shader/Program.hpp"
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"
#include "Castor3D/Shader/Ubos/MatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelMatrixUbo.hpp"
#include "Castor3D/Shader/Ubos/ModelUbo.hpp"
#include "Castor3D/Shader/Ubos/MorphingUbo.hpp"
#include "Castor3D/Shader/Ubos/SceneUbo.hpp"
#include "Castor3D/Shader/Ubos/SkinningUbo.hpp"
#include "Castor3D/ShadowMap/ShadowMapPass.hpp"
#include "Castor3D/Texture/TextureLayout.hpp"
#include "Castor3D/Texture/TextureUnit.hpp"

#include <Ashes/Image/Texture.hpp>
#include <Ashes/Image/TextureView.hpp>
#include <Ashes/RenderPass/RenderPass.hpp>
#include <Ashes/RenderPass/RenderPassCreateInfo.hpp>

#include <ShaderWriter/Source.hpp>
#include "Castor3D/Shader/Shaders/GlslMaterial.hpp"

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
			auto & device = getCurrentDevice( *this );

			{
				auto cmdBuffer = device.getGraphicsCommandPool().createCommandBuffer();
				cmdBuffer->begin( ashes::CommandBufferUsageFlag::eOneTimeSubmit );
				static ashes::ClearColorValue const clearColour = []()
				{
					ashes::ClearColorValue result;
					result.float32 = { 1.0f, 1.0f, 1.0f, 1.0f };
					return result;
				}();

				for ( auto & view : *m_shadowMap.getTexture() )
				{
					cmdBuffer->memoryBarrier( ashes::PipelineStageFlag::eTopOfPipe
						, ashes::PipelineStageFlag::eTransfer
						, view->getView().makeTransferDestination( ashes::ImageLayout::eUndefined
							, 0u ) );
					cmdBuffer->clear( view->getView(), clearColour );
					cmdBuffer->memoryBarrier( ashes::PipelineStageFlag::eTransfer
						, ashes::PipelineStageFlag::eFragmentShader
						, view->getView().makeShaderInputResource( ashes::ImageLayout::eTransferDstOptimal
							, 0u ) );
				}

				for ( auto & view : *m_linearMap.getTexture() )
				{
					cmdBuffer->memoryBarrier( ashes::PipelineStageFlag::eTopOfPipe
						, ashes::PipelineStageFlag::eTransfer
						, view->getView().makeTransferDestination( ashes::ImageLayout::eUndefined
							, 0u ) );
					cmdBuffer->clear( view->getView(), clearColour );
					cmdBuffer->memoryBarrier( ashes::PipelineStageFlag::eTransfer
						, ashes::PipelineStageFlag::eFragmentShader
						, view->getView().makeShaderInputResource( ashes::ImageLayout::eTransferDstOptimal
							, 0u ) );
				}

				cmdBuffer->end();
				auto fence = device.createFence();
				device.getGraphicsQueue().submit( *cmdBuffer, fence.get() );
				fence->wait( ashes::FenceTimeout );
			}

			auto size = m_shadowMap.getTexture()->getDimensions();

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

	void ShadowMap::updateFlags( PipelineFlags & flags )const
	{
		remFlag( flags.programFlags, ProgramFlag::eLighting );
		remFlag( flags.passFlags, PassFlag::eAlphaBlending );
		remFlag( flags.textures, TextureFlag::eAllButOpacity );
		flags.texturesCount = checkFlag( flags.textures, TextureFlag::eOpacity )
			? 1u
			: 0u;
		doUpdateFlags( flags );
	}

	ShaderPtr ShadowMap::getVertexShaderSource( PipelineFlags const & flags )const
	{
		return doGetVertexShaderSource( flags );
	}

	ShaderPtr ShadowMap::getGeometryShaderSource( PipelineFlags const & flags )const
	{
		return doGetGeometryShaderSource( flags );
	}

	ShaderPtr ShadowMap::getPixelShaderSource( PipelineFlags const & flags )const
	{
		return doGetPixelShaderSource( flags );
	}

	ashes::Sampler const & ShadowMap::getLinearSampler()const
	{
		return m_linearMap.getSampler()->getSampler();
	}

	ashes::Sampler const & ShadowMap::getVarianceSampler()const
	{
		return m_shadowMap.getSampler()->getSampler();
	}

	ashes::TextureView const & ShadowMap::getLinearView()const
	{
		return m_linearMap.getTexture()->getDefaultView();
	}

	ashes::TextureView const & ShadowMap::getVarianceView()const
	{
		return m_shadowMap.getTexture()->getDefaultView();
	}

	ashes::TextureView const & ShadowMap::getLinearView( uint32_t index )const
	{
		return m_linearMap.getTexture()->getDefaultView();
	}

	ashes::TextureView const & ShadowMap::getVarianceView( uint32_t index )const
	{
		return m_shadowMap.getTexture()->getDefaultView();
	}

	ShaderPtr ShadowMap::doGetGeometryShaderSource( PipelineFlags const & flags )const
	{
		return ShaderPtr{};
	}
}
