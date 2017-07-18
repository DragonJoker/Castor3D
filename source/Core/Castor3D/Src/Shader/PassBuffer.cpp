#include "PassBuffer.hpp"

#include "Engine.hpp"
#include "Material/Pass.hpp"
#include "Render/RenderSystem.hpp"
#include "Render/RenderNode/PassRenderNode.hpp"
#include "Shader/ShaderProgram.hpp"
#include "Texture/TextureLayout.hpp"
#include "Texture/TextureUnit.hpp"

#include <GlslMaterial.hpp>

using namespace Castor;

namespace Castor3D
{
	//*********************************************************************************************

	PassBuffer::PassBuffer( Engine & p_engine
		, uint32_t p_size )
		: m_texture{ p_engine }
		, m_passSize{ p_size }
	{
		auto texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eBuffer
			, AccessType::eWrite
			, AccessType::eRead
			, PixelFormat::eRGBA32F
			, Size( GLSL::MaxMaterialsCount, 1 ) );
		texture->GetImage().InitialiseSource();
		SamplerSPtr sampler = p_engine.GetLightsSampler();
		m_texture.SetAutoMipmaps( false );
		m_texture.SetSampler( sampler );
		m_texture.SetTexture( texture );
		m_texture.SetIndex( Pass::PassBufferIndex );
		m_buffer = texture->GetImage().GetBuffer();
		m_texture.Initialise();
	}

	PassBuffer::~PassBuffer()
	{
		m_texture.Cleanup();
	}

	uint32_t PassBuffer::AddPass( Pass & p_pass )
	{
		REQUIRE( p_pass.GetId() == 0u );
		REQUIRE( m_passes.size() < GLSL::MaxMaterialsCount );
		m_passes.emplace_back( &p_pass );
		p_pass.SetId( m_passID++ );
		m_connections.emplace_back( p_pass.onChanged.connect( [this]( Pass const & p_pass )
		{
			m_dirty.emplace_back( &p_pass );
		} ) );
		m_dirty.emplace_back( &p_pass );
		return p_pass.GetId();
	}

	void PassBuffer::RemovePass( Pass & p_pass )
	{
		auto id = p_pass.GetId() - 1u;
		REQUIRE( id < m_passes.size() );
		REQUIRE( &p_pass == m_passes[id] );
		auto it = m_passes.erase( m_passes.begin() + id );

		for ( it; it != m_passes.end(); ++it )
		{
			( *it )->SetId( id );
			++id;
		}

		m_connections.erase( m_connections.begin() + id );
		p_pass.SetId( 0u );
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
				p_pass->Update( *this );
			} );

			auto layout = m_texture.GetTexture();
			auto locked = layout->Lock( AccessType::eWrite );

			if ( locked )
			{
				memcpy( locked, m_buffer->const_ptr(), m_buffer->size() );
			}

			layout->Unlock( true );
		}
	}

	void PassBuffer::Bind()const
	{
		m_texture.Bind();
	}

	void PassBuffer::Unbind()const
	{
		m_texture.Unbind();
	}

	void PassBuffer::SetComponents( uint32_t p_index
		, uint32_t p_components
		, float p_r
		, float p_g
		, float p_b
		, float p_a )
	{
		REQUIRE( p_components < m_passSize );
		auto buffer = reinterpret_cast< float * >( m_buffer->ptr() ) + ( p_components + p_index * m_passSize ) * 4u;
		*buffer++ = p_r;
		*buffer++ = p_g;
		*buffer++ = p_b;
		*buffer++ = p_a;
	}

	void PassBuffer::SetComponents( uint32_t p_index
		, uint32_t p_components
		, Castor::Colour const & p_rgb
		, float p_a )
	{
		auto rgb = rgb_float( p_rgb );
		SetComponents( p_index, p_components, rgb[0], rgb[1], rgb[2], p_a );
	}

	void PassBuffer::SetComponents( uint32_t p_index
		, uint32_t p_components
		, Castor::HdrColour const & p_rgb
		, float p_a )
	{
		auto rgb = rgb_float( p_rgb );
		SetComponents( p_index, p_components, rgb[0], rgb[1], rgb[2], p_a );
	}

	void PassBuffer::SetComponents( uint32_t p_index
		, uint32_t p_components
		, float p_r
		, Castor::Colour const & p_gba )
	{
		auto gba = rgb_float( p_gba );
		SetComponents( p_index, p_components, p_r, gba[0], gba[1], gba[2] );
	}
}
