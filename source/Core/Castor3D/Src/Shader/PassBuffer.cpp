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

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & engine
		, uint32_t count
		, uint32_t size )
		: m_buffer{ engine }
		, m_passCount{ count }
	{
		m_buffer.Resize( size * count );
		m_buffer.Initialise( BufferAccessType::eDynamic, BufferAccessNature::eDraw );
	}

	PassBuffer::~PassBuffer()
	{
		m_buffer.Cleanup();
	}

	uint32_t PassBuffer::AddPass( Pass & pass )
	{
		REQUIRE( pass.GetId() == 0u );
		REQUIRE( m_passes.size() < GLSL::MaxMaterialsCount );
		m_passes.emplace_back( &pass );
		pass.SetId( m_passID++ );
		m_connections.emplace_back( pass.onChanged.connect( [this]( Pass const & pass )
		{
			m_dirty.emplace_back( &pass );
		} ) );
		m_dirty.emplace_back( &pass );
		return pass.GetId();
	}

	void PassBuffer::RemovePass( Pass & pass )
	{
		auto id = pass.GetId() - 1u;
		REQUIRE( id < m_passes.size() );
		REQUIRE( &pass == m_passes[id] );
		auto it = m_passes.erase( m_passes.begin() + id );

		for ( it; it != m_passes.end(); ++it )
		{
			( *it )->SetId( id );
			++id;
		}

		m_connections.erase( m_connections.begin() + id );
		pass.SetId( 0u );
		m_passID--;
	}

	void PassBuffer::Update()
	{
		if ( !m_dirty.empty() )
		{
			std::vector< Pass const * > dirty;
			std::swap( m_dirty, dirty );
			auto end = std::unique( dirty.begin(), dirty.end() );

			std::for_each( dirty.begin(), end, [this]( Pass const * p_pass )
			{
				p_pass->Accept( *this );
			} );

			m_buffer.Upload();
			m_buffer.BindTo( 0u );
		}
	}

	void PassBuffer::Bind()const
	{
		m_buffer.BindTo( 0u );
	}

	void PassBuffer::Visit( LegacyPass const & pass )
	{
		CASTOR_EXCEPTION( "This pass buffer can't hold legacy pass data" );
	}

	void PassBuffer::Visit( MetallicRoughnessPbrPass const & pass )
	{
		CASTOR_EXCEPTION( "This pass buffer can't hold metallic/roughness pass data" );
	}

	void PassBuffer::Visit( SpecularGlossinessPbrPass const & pass )
	{
		CASTOR_EXCEPTION( "This pass buffer can't hold specular/glossiness pass data" );
	}
}
