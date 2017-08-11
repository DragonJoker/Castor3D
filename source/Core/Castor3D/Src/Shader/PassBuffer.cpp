#include "PassBuffer.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderNode/PassRenderNode.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslMaterial.hpp>

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
		REQUIRE( m_passes.size() < glsl::MaxMaterialsCount );
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
}
