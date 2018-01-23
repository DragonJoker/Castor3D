#include "RenderPipeline.hpp"

#include "Engine.hpp"
#include "Event/Frame/FunctorEvent.hpp"
#include "Mesh/Buffer/GeometryBuffers.hpp"
#include "Mesh/Submesh.hpp"
#include "Render/RenderSystem.hpp"
#include "Scene/BillboardList.hpp"
#include "Scene/Camera.hpp"
#include "Shader/UniformBufferBinding.hpp"
#include "Shader/ShaderProgram.hpp"

#include <GlslSource.hpp>
#include "Shader/Shaders/GlslShadow.hpp"

using namespace castor;

namespace castor3d
{
	//*************************************************************************************************

	const String RenderPipeline::MtxTexture[C3D_MAX_TEXTURE_MATRICES] =
	{
		cuT( "c3d_mtxTexture0" ),
		cuT( "c3d_mtxTexture1" ),
		cuT( "c3d_mtxTexture2" ),
		cuT( "c3d_mtxTexture3" ),
	};

	//*************************************************************************************************

	RenderPipeline::RenderPipeline( RenderSystem & renderSystem
		, DepthStencilState && p_dsState
		, RasteriserState && p_rsState
		, BlendState && p_blState
		, MultisampleState && p_msState
		, ShaderProgram & p_program
		, PipelineFlags const & p_flags )
		: OwnedBy< RenderSystem >{ renderSystem }
		, m_dsState{ std::move( p_dsState ) }
		, m_rsState{ std::move( p_rsState ) }
		, m_blState{ std::move( p_blState ) }
		, m_msState{ std::move( p_msState ) }
		, m_program{ p_program }
		, m_flags( p_flags )
	{
		auto textures = m_flags.m_textureFlags & uint16_t( TextureChannel::eAll );

		while ( textures )
		{
			m_textureCount++;

			while ( !( textures & 0x01 ) )
			{
				textures >>= 1;
			}

			textures >>= 1;
		}

		if ( m_program.hasObject( ShaderType::ePixel ) )
		{
			m_directionalShadowMaps = m_program.findUniform< UniformType::eSampler >( shader::Shadow::MapShadowDirectional, ShaderType::ePixel );
			m_spotShadowMaps = m_program.findUniform< UniformType::eSampler >( shader::Shadow::MapShadowSpot, ShaderType::ePixel );
			m_pointShadowMaps = m_program.findUniform< UniformType::eSampler >( shader::Shadow::MapShadowPoint, ShaderType::ePixel );
			m_environmentMap = m_program.findUniform< UniformType::eSampler >( ShaderProgram::MapEnvironment, ShaderType::ePixel );

			if ( ( checkFlag( m_flags.m_passFlags, PassFlag::ePbrMetallicRoughness )
					|| checkFlag( m_flags.m_passFlags, PassFlag::ePbrSpecularGlossiness ) )
				&& checkFlag( m_flags.m_programFlags, ProgramFlag::eLighting ) )
			{
				m_irradianceMap = m_program.findUniform< UniformType::eSampler >( ShaderProgram::MapIrradiance, ShaderType::ePixel );
				m_prefilteredMap = m_program.findUniform< UniformType::eSampler >( ShaderProgram::MapPrefiltered, ShaderType::ePixel );
				m_brdfMap = m_program.findUniform< UniformType::eSampler >( ShaderProgram::MapBrdf, ShaderType::ePixel );
			}
		}
	}

	RenderPipeline::~RenderPipeline()
	{
	}

	void RenderPipeline::cleanup()
	{
		m_program.cleanup();
		m_meshGeometryBuffers.clear();
		m_billboardGeometryBuffers.clear();
	}

	void RenderPipeline::addUniformBuffer( UniformBuffer & p_ubo )
	{
		m_bindings.push_back( std::ref( p_ubo.createBinding( m_program ) ) );
	}

	GeometryBuffersSPtr RenderPipeline::getGeometryBuffers( Submesh & submesh )
	{
		GeometryBuffersSPtr geometryBuffers;
		auto it = m_meshGeometryBuffers.find( &submesh );

		if ( it == m_meshGeometryBuffers.end() )
		{
			geometryBuffers = getRenderSystem()->createGeometryBuffers( submesh.getTopology()
				, m_program );
			m_meshGeometryBuffers.emplace( &submesh, geometryBuffers );
			doInitialiseGeometryBuffers( submesh, geometryBuffers );
		}
		else
		{
			geometryBuffers = it->second;
		}

		return geometryBuffers;
	}

	GeometryBuffersSPtr RenderPipeline::getGeometryBuffers( BillboardBase & billboard )
	{
		GeometryBuffersSPtr geometryBuffers;
		auto it = m_billboardGeometryBuffers.find( &billboard );

		if ( it == m_billboardGeometryBuffers.end() )
		{
			geometryBuffers = getRenderSystem()->createGeometryBuffers( Topology::eTriangleFan
				, m_program );
			m_billboardGeometryBuffers.emplace( &billboard, geometryBuffers );
			doInitialiseGeometryBuffers( billboard, geometryBuffers );
		}
		else
		{
			geometryBuffers = it->second;
		}

		return geometryBuffers;
	}

	void RenderPipeline::doInitialiseGeometryBuffers( Submesh & submesh
		, GeometryBuffersSPtr geometryBuffers )
	{
		VertexBufferArray buffers;
		submesh.gatherBuffers( buffers );

		if ( getRenderSystem()->getCurrentContext() )
		{
			geometryBuffers->initialise( buffers, &submesh.getIndexBuffer() );
		}
		else
		{
			getRenderSystem()->getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [geometryBuffers, &submesh, buffers]()
			{
				geometryBuffers->initialise( buffers, &submesh.getIndexBuffer() );
			} ) );
		}
	}

	void RenderPipeline::doInitialiseGeometryBuffers( BillboardBase & billboard
		, GeometryBuffersSPtr geometryBuffers )
	{
		VertexBufferArray buffers;
		billboard.gatherBuffers( buffers );

		if ( getRenderSystem()->getCurrentContext() )
		{
			geometryBuffers->initialise( buffers, nullptr );
		}
		else
		{
			getRenderSystem()->getEngine()->postEvent( makeFunctorEvent( EventType::ePreRender
				, [geometryBuffers, buffers]()
			{
				geometryBuffers->initialise( buffers, nullptr );
			} ) );
		}
	}

	//*************************************************************************************************
}
