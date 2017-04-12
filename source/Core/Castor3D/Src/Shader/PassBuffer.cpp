﻿#include "PassBuffer.hpp"

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
		auto l_texture = p_engine.GetRenderSystem()->CreateTexture( TextureType::eBuffer
			, AccessType::eWrite
			, AccessType::eRead
			, PixelFormat::eRGBA32F
			, Size( GLSL::MaxMaterialsCount, 1 ) );
		l_texture->GetImage().InitialiseSource();
		SamplerSPtr l_sampler = p_engine.GetLightsSampler();
		m_texture.SetAutoMipmaps( false );
		m_texture.SetSampler( l_sampler );
		m_texture.SetTexture( l_texture );
		m_buffer = l_texture->GetImage().GetBuffer();
		m_texture.Initialise();
	}

	PassBuffer::~PassBuffer()
	{
		m_texture.Cleanup();
	}

	uint32_t PassBuffer::AddPass( Pass & p_pass )
	{
		m_passes.emplace_back( &p_pass );
		p_pass.SetId( m_passID++ );
		return p_pass.GetId();
	}

	void PassBuffer::Update( Scene & p_scene )
	{
		for ( auto & l_pass : m_passes )
		{
			l_pass->Update( *this, p_scene );
		}
	}

	void PassBuffer::SetComponents( uint32_t p_index
		, uint32_t p_components
		, float p_r
		, float p_g
		, float p_b
		, float p_a )
	{
		auto l_buffer = reinterpret_cast< float * >( m_buffer->ptr() ) + ( p_components + p_index * m_passSize ) * 4u;
		*l_buffer++ = p_r;
		*l_buffer++ = p_g;
		*l_buffer++ = p_b;
		*l_buffer++ = p_a;
	}

	void PassBuffer::SetComponents( uint32_t p_index
		, uint32_t p_components
		, Castor::Colour const & p_rgb
		, float p_a )
	{
		auto l_rgb = rgb_float( p_rgb );
		SetComponents( p_index, p_components, l_rgb[0], l_rgb[1], l_rgb[2], p_a );
	}

	void PassBuffer::SetComponents( uint32_t p_index
		, uint32_t p_components
		, float p_r
		, Castor::Colour const & p_gba )
	{
		auto l_gba = rgb_float( p_gba );
		SetComponents( p_index, p_components, p_r, l_gba[0], l_gba[1], l_gba[2] );
	}
}
