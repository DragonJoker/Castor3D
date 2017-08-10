#include "GlBuffer.hpp"
#include "Render/GlRenderSystem.hpp"

namespace GlRender
{
	GlBuffer::GlBuffer( GlRenderSystem & renderSystem
		, OpenGl & p_gl
		, GlBufferTarget p_target )
		: castor3d::GpuBuffer( renderSystem )
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

	bool GlBuffer::create()
	{
		return BindableType::create();
	}

	void GlBuffer::destroy()
	{
		BindableType::destroy();
	}

	void GlBuffer::initialiseStorage( uint32_t p_count
		, castor3d::BufferAccessType p_type
		, castor3d::BufferAccessNature p_nature )const
	{
		REQUIRE( p_count > 0 );
		REQUIRE( this->getGlName() != GlInvalidIndex );
		bind();
		getOpenGl().BufferData( m_target
			, p_count
			, nullptr
			, BindableType::getOpenGl().getBufferFlags( uint32_t( p_nature ) | uint32_t( p_type ) ) );
		m_allocatedSize = p_count;
		unbind();
	}

	void GlBuffer::setBindingPoint( uint32_t p_index )const
	{
		m_bindingPoint = p_index;
		getOpenGl().BindBufferBase( m_target
			, p_index
			, getGlName() );
	}

	uint32_t GlBuffer::getBindingPoint()const
	{
		return m_bindingPoint;
	}

	void GlBuffer::bind()const
	{
		BindableType::bind();
	}

	void GlBuffer::unbind()const
	{
		BindableType::unbind();
	}

	void GlBuffer::copy( castor3d::GpuBuffer const & p_src
		, uint32_t p_size )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( p_size > 0 );
		REQUIRE( m_allocatedSize >= p_size );
		REQUIRE( static_cast< GlBuffer const & >( p_src ).m_allocatedSize >= p_size );
		getOpenGl().BindBuffer( GlBufferTarget::eRead
			, static_cast< GlBuffer const & >( p_src ).getGlName() );
		getOpenGl().BindBuffer( GlBufferTarget::eWrite, getGlName() );
		BindableType::getOpenGl().CopyBufferSubData( GlBufferTarget::eRead
			, GlBufferTarget::eWrite
			, 0
			, 0
			, p_size );
		getOpenGl().BindBuffer( GlBufferTarget::eWrite, 0 );
		getOpenGl().BindBuffer( GlBufferTarget::eRead, 0 );
	}

	void GlBuffer::upload( uint32_t p_offset
		, uint32_t p_count
		, uint8_t const * p_buffer )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( p_count > 0 );
		REQUIRE( m_allocatedSize >= p_count + p_offset );
		bind();
		static auto const provider = BindableType::getOpenGl().getProvider();

		if ( provider == GlProvider::eNvidia || provider == GlProvider::eATI )
		{
			auto buffer = lock( p_offset
				, p_count
				, castor3d::AccessType::eWrite );

			if ( buffer )
			{
				std::memcpy( buffer, p_buffer, p_count );
				unlock();
			}
		}
		else
		{
			BindableType::getOpenGl().BufferSubData( m_target
				, p_offset
				, p_count
				, p_buffer );
		}

		unbind();
	}

	void GlBuffer::download( uint32_t p_offset
		, uint32_t p_count
		, uint8_t * p_buffer )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( p_count > 0 );
		REQUIRE( m_allocatedSize >= p_count + p_offset );
		bind();
		auto buffer = lock( p_offset
			, p_count
			, castor3d::AccessType::eRead );

		if ( buffer )
		{
			std::memcpy( p_buffer, buffer, p_count );
			unlock();
		}

		unbind();
	}

	uint8_t * GlBuffer::lock( uint32_t p_offset
		, uint32_t p_count
		, castor3d::AccessTypes const & p_flags )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( p_count > 0 );
		REQUIRE( m_allocatedSize >= p_count + p_offset );
		return reinterpret_cast< uint8_t * >( BindableType::getOpenGl().MapBufferRange( m_target
			, p_offset
			, p_count
			, BindableType::getOpenGl().getBitfieldFlags( p_flags ) ) );
	}

	void GlBuffer::unlock()const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		getOpenGl().UnmapBuffer( m_target );
	}

	uint8_t * GlBuffer::lock( GlAccessType p_access )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		return reinterpret_cast< uint8_t * >( BindableType::getOpenGl().MapBuffer( m_target
			, p_access ) );
	}
}
