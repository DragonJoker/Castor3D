#include "PassBuffer.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderNode/PassRenderNode.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Shader/Shaders/GlslMaterial.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <Design/ArrayView.hpp>

using namespace castor;

namespace castor3d
{
	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & engine
		, uint32_t count
		, uint32_t size )
		: m_buffer{ engine }
		, m_passCount{ count }
	{
		m_buffer.resize( size * count );
		m_buffer.initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
	}

	PassBuffer::~PassBuffer()
	{
		m_buffer.cleanup();
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

			m_buffer.upload();
			m_buffer.bindTo( 0u );
		}
	}

	void PassBuffer::bind()const
	{
		m_buffer.bindTo( 0u );
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
		
#if GLSL_MATERIALS_STRUCT_OF_ARRAY
		
		if ( pass.hasSubsurfaceScattering() )
		{
			data.sssInfo[index].r = 1.0f;
			doVisit( pass.getSubsurfaceScattering()
				, index
				, data);
		}
		else
		{
			data.sssInfo[index].r = 0.0f;
		}

#else
		
		if ( pass.hasSubsurfaceScattering() )
		{
			data.sssInfo.r = 1.0f;
			doVisit( pass.getSubsurfaceScattering()
				, index
				, data);
		}
		else
		{
			data.sssInfo.r = 0.0f;
		}

#endif
	}

	void PassBuffer::doVisit( SubsurfaceScattering const & subsurfaceScattering
		, uint32_t index
		, ExtendedData & data )
	{
#if GLSL_MATERIALS_STRUCT_OF_ARRAY

		data.transmittance[index].r = subsurfaceScattering.getTransmittanceCoefficients()[0];
		data.transmittance[index].g = subsurfaceScattering.getTransmittanceCoefficients()[1];
		data.transmittance[index].b = subsurfaceScattering.getTransmittanceCoefficients()[2];
		data.transmittance[index].a = subsurfaceScattering.isDistanceBasedTransmittanceEnabled() ? 1.0f : 0.0f;

#else

		data.transmittance.r = subsurfaceScattering.getTransmittanceCoefficients()[0];
		data.transmittance.g = subsurfaceScattering.getTransmittanceCoefficients()[1];
		data.transmittance.b = subsurfaceScattering.getTransmittanceCoefficients()[2];
		data.transmittance.a = subsurfaceScattering.isDistanceBasedTransmittanceEnabled() ? 1.0f : 0.0f;

#endif
	}
}
