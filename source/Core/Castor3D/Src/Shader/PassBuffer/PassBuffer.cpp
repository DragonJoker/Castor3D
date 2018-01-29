#include "PassBuffer.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderSystem.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"

#include <Design/ArrayView.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & engine
		, uint32_t count
		, uint32_t size )
		: m_buffer{ engine, count * size }
		, m_passCount{ count }
	{
	}

	uint32_t PassBuffer::addPass( Pass & pass )
	{
		REQUIRE( pass.getId() == 0u );
		REQUIRE( m_passes.size() < shader::MaxMaterialsCount );
		m_passes.emplace_back( &pass );
		pass.setId( m_passID++ );
		m_connections.emplace_back( pass.onChanged.connect( [this]( Pass const & pass )
		{
			m_dirty.emplace_back( &pass );
		} ) );
		m_dirty.emplace_back( &pass );
		return pass.getId();
	}

	void PassBuffer::removePass( Pass & pass )
	{
		auto id = pass.getId() - 1u;
		REQUIRE( id < m_passes.size() );
		REQUIRE( &pass == m_passes[id] );
		auto it = m_passes.erase( m_passes.begin() + id );

		for ( it; it != m_passes.end(); ++it )
		{
			( *it )->setId( id );
			++id;
		}

		m_connections.erase( m_connections.begin() + id );
		pass.setId( 0u );
		m_passID--;
	}

	void PassBuffer::update()
	{
		if ( !m_dirty.empty() )
		{
			std::vector< Pass const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin(), end, [this]( Pass const * p_pass )
			{
				p_pass->accept( *this );
			} );

			m_buffer.update();
		}
	}

	renderer::DescriptorSetLayoutBinding PassBuffer::createLayoutBinding()const
	{
		return m_buffer.createLayoutBinding( PassBufferIndex );
	}

	renderer::DescriptorSetBinding const & PassBuffer::createBinding( renderer::DescriptorSet & descriptorSet
		, renderer::DescriptorSetLayoutBinding const & binding )const
	{
		return m_buffer.createBinding( descriptorSet, binding );
	}

	void PassBuffer::visit( LegacyPass const & pass )
	{
		CASTOR_EXCEPTION( "This pass buffer can't hold legacy pass data" );
	}

	void PassBuffer::visit( MetallicRoughnessPbrPass const & pass )
	{
		CASTOR_EXCEPTION( "This pass buffer can't hold metallic/roughness pass data" );
	}

	void PassBuffer::visit( SpecularGlossinessPbrPass const & pass )
	{
		CASTOR_EXCEPTION( "This pass buffer can't hold specular/glossiness pass data" );
	}

	void PassBuffer::doVisitExtended( Pass const & pass
		, ExtendedData & data )
	{
		auto index = pass.getId () - 1;
		
		if ( pass.hasSubsurfaceScattering() )
		{
			doVisit( pass.getSubsurfaceScattering()
				, index
				, data );
		}
		else
		{
			data.sssInfo.r = 0.0f;
			data.sssInfo.g = 0.0f;
			data.sssInfo.b = 0.0f;
			data.sssInfo.a = 0.0f;
		}
	}

	void PassBuffer::doVisit( SubsurfaceScattering const & subsurfaceScattering
		, uint32_t index
		, ExtendedData & data )
	{
		data.sssInfo.r = 1.0f;
		data.sssInfo.g = subsurfaceScattering.getGaussianWidth();
		data.sssInfo.b = subsurfaceScattering.getStrength();
		data.sssInfo.a = float( subsurfaceScattering.getProfileSize() );

		auto i = 0u;

		for ( auto & factor : subsurfaceScattering )
		{
			data.transmittanceProfile[i].r = factor[0];
			data.transmittanceProfile[i].g = factor[1];
			data.transmittanceProfile[i].b = factor[2];
			data.transmittanceProfile[i].a = factor[3];
			++i;
		}
	}
}
