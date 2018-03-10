#include "Pipeline/GlPipeline.hpp"

#include "Buffer/GlBuffer.hpp"
#include "Command/Commands/GlBindPipelineCommand.hpp"
#include "Core/GlDevice.hpp"
#include "Core/GlRenderer.hpp"
#include "Core/GlRenderingResources.hpp"
#include "Miscellaneous/GlValidator.hpp"
#include "Pipeline/GlPipelineLayout.hpp"
#include "RenderPass/GlRenderPass.hpp"
#include "Shader/GlShaderModule.hpp"

#include <algorithm>

#if defined( interface )
#	undef interface
#endif

namespace gl_renderer
{
	namespace
	{
		template<typename T>
		void doHashCombine( size_t & seed, T const & v )
		{
			const uint64_t kMul = 0x9ddfea08eb382d69ULL;

			std::hash< T > hasher;
			uint64_t a = ( hasher( v ) ^ seed ) * kMul;
			a ^= ( a >> 47 );

			uint64_t b = ( seed ^ a ) * kMul;
			b ^= ( b >> 47 );

			seed = static_cast< std::size_t >( b * kMul );
		}

		size_t doHash( VboBindings const & vbos
			, IboBinding const & ibo )
		{
			size_t result{ 0u };

			for ( auto & binding : vbos )
			{
				auto & vbo = binding.second;
				doHashCombine( result, vbo.bo );
				doHashCombine( result, vbo.offset );
			}

			if ( bool( ibo ) )
			{
				doHashCombine( result, ibo.value().bo );
				doHashCombine( result, ibo.value().offset );
			}

			return result;
		}

		size_t doHash( renderer::VertexInputAttributeDescription const & desc )
		{
			size_t result = 0u;
			doHashCombine( result, desc.binding );
			doHashCombine( result, desc.format );
			doHashCombine( result, desc.location );
			doHashCombine( result, desc.offset );
			return result;
		}

		size_t doHash( renderer::VertexInputBindingDescription const & desc )
		{
			size_t result = 0u;
			doHashCombine( result, desc.binding );
			doHashCombine( result, desc.inputRate );
			doHashCombine( result, desc.stride );
			return result;
		}

		size_t doHash( renderer::VertexInputState const & state )
		{
			size_t result = 0u;

			for ( auto & desc : state.vertexAttributeDescriptions )
			{
				doHashCombine( result, doHash( desc ) );
			}

			for ( auto & desc : state.vertexBindingDescriptions )
			{
				doHashCombine( result, doHash( desc ) );
			}

			return result;
		}
	}

	Pipeline::Pipeline( Device const & device
		, PipelineLayout const & layout
		, renderer::GraphicsPipelineCreateInfo && createInfo )
		: renderer::Pipeline{ device
			, layout
			, std::move( createInfo ) }
		, m_device{ device }
		, m_layout{ layout }
		, m_ssState{ std::move( m_createInfo.stages ) }
		, m_vertexInputState{ m_createInfo.vertexInputState }
		, m_renderPass{ m_createInfo.renderPass.get() }
		, m_iaState{ m_createInfo.inputAssemblyState }
		, m_cbState{ m_createInfo.colourBlendState }
		, m_rsState{ m_createInfo.rasterisationState }
		, m_dsState{ 0u, false, true, renderer::CompareOp::eLess,  }
		, m_msState{ m_createInfo.multisampleState }
		, m_viewport{ m_createInfo.viewport }
		, m_scissor{ m_createInfo.scissor }
		, m_vertexInputStateHash{ doHash( m_vertexInputState ) }
		, m_program{ m_ssState }
	{
		if ( m_createInfo.depthStencilState )
		{
			m_dsState = m_createInfo.depthStencilState.value();
		}

		if ( m_createInfo.tessellationState )
		{
			m_tsState = m_createInfo.tessellationState.value();
		}

		for ( auto & stage : m_ssState )
		{
			if ( !static_cast< ShaderModule const & >( *stage.module ).isSpirV()
				&& stage.specialisationInfo )
			{
				m_constantsPcbs.push_back( convert( stage.module->getStage(), *stage.specialisationInfo ) );
			}
		}

		apply( m_device, m_cbState );
		apply( m_device, m_rsState
			, hasDynamicState( renderer::DynamicState::eLineWidth )
			, hasDynamicState( renderer::DynamicState::eDepthBias ) );
		apply( m_device, m_dsState );
		apply( m_device, m_msState );
		apply( m_device, m_tsState );
		m_program.link();

		if ( m_device.getRenderer().isValidationEnabled() )
		{
			validatePipeline( m_layout
				, m_program.getProgram()
				, m_vertexInputState
				, m_renderPass );
		}
	}

	Pipeline::~Pipeline()
	{
	}

	GeometryBuffers * Pipeline::findGeometryBuffers( VboBindings const & vbos
		, IboBinding const & ibo )const
	{
		size_t hash = doHash( vbos, ibo );
		auto it = std::find_if( m_geometryBuffers.begin()
			, m_geometryBuffers.end()
			, [&hash]( std::pair< size_t, GeometryBuffersPtr > const & pair )
		{
			return pair.first == hash;
		} );
		return it == m_geometryBuffers.end()
			? nullptr
			: it->second.get();
	}

	GeometryBuffersRef Pipeline::createGeometryBuffers( VboBindings vbos
		, IboBinding const & ibo
		, renderer::IndexType type )const
	{
		size_t hash = doHash( vbos, ibo );
		m_geometryBuffers.emplace_back( hash, std::make_unique< GeometryBuffers >( vbos, ibo, m_vertexInputState, type ) );

		for ( auto & binding : vbos )
		{
			auto & vbo = binding.second;
			m_connections[vbo.bo] = vbo.buffer->onDestroy.connect( [this]( GLuint name )
			{
				auto it = std::remove_if( m_geometryBuffers.begin()
					, m_geometryBuffers.end()
					, [&name]( std::pair< size_t, GeometryBuffersPtr > const & pair )
				{
					bool result = false;

					for ( auto & vbo : pair.second->getVbos() )
					{
						if ( !result )
						{
							result = vbo.vbo == name;
						}
					}

					if ( !result && bool( pair.second->hasIbo() ) )
					{
						result = pair.second->getIbo().ibo == name;
					}

					return result;
				} );

				if ( it != m_geometryBuffers.end() )
				{
					m_geometryBuffers.erase( it, m_geometryBuffers.end() );
				}
			} );
		}

		return *m_geometryBuffers.back().second;
	}
}
