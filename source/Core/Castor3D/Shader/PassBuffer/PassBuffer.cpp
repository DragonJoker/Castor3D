#include "Castor3D/Shader/PassBuffer/PassBuffer.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Material/Pass/Pass.hpp"
#include "Castor3D/Shader/Shaders/SdwModule.hpp"

namespace castor3d
{
	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & engine
		, RenderDevice const & device
		, uint32_t count
		, uint32_t size )
		: m_buffer{ engine, device, count * size, cuT( "PassBuffer" ) }
		, m_passCount{ count }
	{
	}

	uint32_t PassBuffer::addPass( Pass & pass )
	{
		CU_Require( pass.getId() == 0u );
		CU_Require( m_passes.size() < shader::MaxMaterialsCount );
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
		auto id = pass.getId();
		CU_Require( id < m_passes.size() );
		CU_Require( &pass == m_passes[id] );

		for ( auto it = m_passes.erase( m_passes.begin() + id ); it != m_passes.end(); ++it )
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

	VkDescriptorSetLayoutBinding PassBuffer::createLayoutBinding()const
	{
		return m_buffer.createLayoutBinding( getPassBufferIndex() );
	}

	void PassBuffer::createBinding( ashes::DescriptorSet & descriptorSet
		, VkDescriptorSetLayoutBinding const & binding )const
	{
		m_buffer.createBinding( descriptorSet, binding );
	}

	void PassBuffer::visit( PhongPass const & pass )
	{
		CU_Exception( "This pass buffer can't hold phong pass data" );
	}

	void PassBuffer::visit( MetallicRoughnessPbrPass const & pass )
	{
		CU_Exception( "This pass buffer can't hold metallic/roughness pass data" );
	}

	void PassBuffer::visit( SpecularGlossinessPbrPass const & pass )
	{
		CU_Exception( "This pass buffer can't hold specular/glossiness pass data" );
	}

	void PassBuffer::doVisitExtended( Pass const & pass
		, ExtendedData & data )
	{
		auto index = pass.getId() - 1;

#if C3D_MaterialsStructOfArrays

		if ( pass.hasSubsurfaceScattering() )
		{
			doVisit( pass.getSubsurfaceScattering()
				, index
				, data );
		}
		else
		{
			data.sssInfo[index].r = 0.0f;
			data.sssInfo[index].g = 0.0f;
			data.sssInfo[index].b = 0.0f;
			data.sssInfo[index].a = 0.0f;
		}

#else

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

#endif
	}

	void PassBuffer::doVisit( SubsurfaceScattering const & subsurfaceScattering
		, uint32_t index
		, ExtendedData & data )
	{
#if C3D_MaterialsStructOfArrays

		data.sssInfo[index].r = 1.0f;
		data.sssInfo[index].g = subsurfaceScattering.getGaussianWidth();
		data.sssInfo[index].b = subsurfaceScattering.getStrength();
		data.sssInfo[index].a = float( subsurfaceScattering.getProfileSize() );

		auto i = 0u;

		for ( auto & factor : subsurfaceScattering )
		{
			data.transmittanceProfile[index][i].r = factor[0];
			data.transmittanceProfile[index][i].g = factor[1];
			data.transmittanceProfile[index][i].b = factor[2];
			data.transmittanceProfile[index][i].a = factor[3];
			++i;
		}

#else

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

#endif
	}
}
