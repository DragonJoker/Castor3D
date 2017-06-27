#include "GlBuffer.hpp"
#include "Render/GlRenderSystem.hpp"

namespace GlRender
{
	GlBuffer::GlBuffer( GlRenderSystem & p_renderSystem
		, OpenGl & p_gl
		, GlBufferTarget p_target )
		: Castor3D::GpuBuffer( p_renderSystem )
		, BindableType{ p_gl,
			"GlBuffer",
			std::bind( &OpenGl::GenBuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
			std::bind( &OpenGl::DeleteBuffers, std::ref( p_gl ), std::placeholders::_1, std::placeholders::_2 ),
			std::bind( &OpenGl::IsBuffer, std::ref( p_gl ), std::placeholders::_1 ),
			[&p_gl, p_target]( uint32_t p_glName )
			{
				return p_gl.BindBuffer( p_target, p_glName );
			} }
		, m_target{ p_target }
	{
	}

	GlBuffer::~GlBuffer()
	{
	}

	bool GlBuffer::Create()
	{
		return BindableType::Create();
	}

	void GlBuffer::Destroy()
	{
		BindableType::Destroy();
	}

	void GlBuffer::InitialiseStorage( uint32_t p_count
		, Castor3D::BufferAccessType p_type
		, Castor3D::BufferAccessNature p_nature )const
	{
		REQUIRE( p_count > 0 );
		REQUIRE( this->GetGlName() != GlInvalidIndex );
		Bind();
		GetOpenGl().BufferData( m_target
			, p_count
			, nullptr
			, BindableType::GetOpenGl().GetBufferFlags( uint32_t( p_nature ) | uint32_t( p_type ) ) );
		m_allocatedSize = p_count;
		Unbind();
	}

	void GlBuffer::SetBindingPoint( uint32_t p_index )const
	{
		m_bindingPoint = p_index;
		GetOpenGl().BindBufferBase( m_target
			, p_index
			, GetGlName() );
	}

	void GlBuffer::Bind()const
	{
		BindableType::Bind();
	}

	void GlBuffer::Unbind()const
	{
		BindableType::Unbind();
	}

	void GlBuffer::Copy( Castor3D::GpuBuffer const & p_src
		, uint32_t p_size )const
	{
		REQUIRE( this->GetGlName() != GlInvalidIndex );
		REQUIRE( p_size > 0 );
		REQUIRE( m_allocatedSize >= p_size );
		REQUIRE( p_src.m_allocatedSize >= p_size );
		GetOpenGl().BindBuffer( GlBufferTarget::eRead
			, static_cast< GlBuffer const & >( p_src ).GetGlName() );
		GetOpenGl().BindBuffer( GlBufferTarget::eWrite, GetGlName() );
		BindableType::GetOpenGl().CopyBufferSubData( GlBufferTarget::eRead
			, GlBufferTarget::eWrite
			, 0
			, 0
			, p_size );
		GetOpenGl().BindBuffer( GlBufferTarget::eWrite, 0 );
		GetOpenGl().BindBuffer( GlBufferTarget::eRead, 0 );
	}

	void GlBuffer::Upload( uint32_t p_offset
		, uint32_t p_count
		, uint8_t const * p_buffer )const
	{
		REQUIRE( this->GetGlName() != GlInvalidIndex );
		REQUIRE( p_size > 0 );
		REQUIRE( m_allocatedSize >= p_count + p_offset );
		Bind();
		auto l_provider = BindableType::GetOpenGl().GetProvider();

		if ( l_provider == GlProvider::eNvidia || l_provider == GlProvider::eATI )
		{
			auto l_buffer = Lock( p_offset
				, p_count
				, Castor3D::AccessType::eWrite );

			if ( l_buffer )
			{
				std::memcpy( l_buffer, p_buffer, p_count );
				Unlock();
			}
		}
		else
		{
			BindableType::GetOpenGl().BufferSubData( m_target
				, p_offset
				, p_count
				, p_buffer );
		}

		Unbind();
	}

	void GlBuffer::Download( uint32_t p_offset
		, uint32_t p_count
		, uint8_t * p_buffer )const
	{
		REQUIRE( this->GetGlName() != GlInvalidIndex );
		REQUIRE( l_size > 0 );
		REQUIRE( m_allocatedSize >= p_count + p_offset );
		Bind();
		auto l_buffer = Lock( p_offset
			, p_count
			, Castor3D::AccessType::eRead );

		if ( l_buffer )
		{
			std::memcpy( p_buffer, l_buffer, p_count );
			Unlock();
		}

		Unbind();
	}

	uint8_t * GlBuffer::Lock( uint32_t p_offset
		, uint32_t p_count
		, Castor3D::AccessTypes const & p_flags )const
	{
		REQUIRE( this->GetGlName() != GlInvalidIndex );
		REQUIRE( p_count > 0 );
		REQUIRE( m_allocatedSize >= p_count + p_offset );
		return reinterpret_cast< uint8_t * >( BindableType::GetOpenGl().MapBufferRange( m_target
			, p_offset
			, p_count
			, BindableType::GetOpenGl().GetBitfieldFlags( p_flags ) ) );
	}

	void GlBuffer::Unlock()const
	{
		REQUIRE( this->GetGlName() != GlInvalidIndex );
		GetOpenGl().UnmapBuffer( m_target );
	}

	uint8_t * GlBuffer::Lock( GlAccessType p_access )const
	{
		REQUIRE( this->GetGlName() != GlInvalidIndex );
		return reinterpret_cast< uint8_t * >( BindableType::GetOpenGl().MapBuffer( m_target
			, p_access ) );
	}
}
