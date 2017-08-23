#include "GlBuffer.hpp"
#include "Render/GlRenderSystem.hpp"

namespace GlRender
{
	GlBuffer::GlBuffer( GlRenderSystem & renderSystem
		, OpenGl & gl
		, GlBufferTarget target )
		: castor3d::GpuBuffer( renderSystem )
		, BindableType{ gl,
			"GlBuffer",
			std::bind( &OpenGl::GenBuffers, std::ref( gl ), std::placeholders::_1, std::placeholders::_2 ),
			std::bind( &OpenGl::DeleteBuffers, std::ref( gl ), std::placeholders::_1, std::placeholders::_2 ),
			std::bind( &OpenGl::IsBuffer, std::ref( gl ), std::placeholders::_1 ),
			[&gl, target]( uint32_t glName )
			{
				return gl.BindBuffer( target, glName );
			} }
		, m_target{ target }
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

	void GlBuffer::setBindingPoint( uint32_t index )const
	{
		m_bindingPoint = index;
		getOpenGl().BindBufferBase( m_target
			, m_bindingPoint
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

	void GlBuffer::copy( castor3d::GpuBuffer const & src
		, uint32_t srcOffset
		, uint32_t dstOffset
		, uint32_t size )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( size > 0 );
		REQUIRE( m_allocatedSize >= size + dstOffset );
		REQUIRE( static_cast< GlBuffer const & >( src ).m_allocatedSize >= size + srcOffset );
		getOpenGl().BindBuffer( GlBufferTarget::eRead
			, static_cast< GlBuffer const & >( src ).getGlName() );
		getOpenGl().BindBuffer( GlBufferTarget::eWrite, getGlName() );
		BindableType::getOpenGl().CopyBufferSubData( GlBufferTarget::eRead
			, GlBufferTarget::eWrite
			, srcOffset
			, dstOffset
			, size );
		getOpenGl().BindBuffer( GlBufferTarget::eWrite, 0 );
		getOpenGl().BindBuffer( GlBufferTarget::eRead, 0 );
	}

	void GlBuffer::upload( uint32_t offset
		, uint32_t count
		, uint8_t const * buffer )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( count > 0 );
		REQUIRE( m_allocatedSize >= count + offset );
		bind();
		static auto const provider = BindableType::getOpenGl().getProvider();

		if ( provider == GlProvider::eNvidia || provider == GlProvider::eATI )
		{
			auto data = lock( offset
				, count
				, castor3d::AccessType::eWrite );

			if ( buffer )
			{
				std::memcpy( data, buffer, count );
				unlock();
			}
		}
		else
		{
			BindableType::getOpenGl().BufferSubData( m_target
				, offset
				, count
				, buffer );
		}

		unbind();
	}

	void GlBuffer::download( uint32_t offset
		, uint32_t count
		, uint8_t * buffer )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( count > 0 );
		REQUIRE( m_allocatedSize >= count + offset );
		bind();
		auto data = lock( offset
			, count
			, castor3d::AccessType::eRead );

		if ( buffer )
		{
			std::memcpy( buffer, data, count );
			unlock();
		}

		unbind();
	}

	uint8_t * GlBuffer::lock( uint32_t offset
		, uint32_t count
		, castor3d::AccessTypes const & flags )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		REQUIRE( count > 0 );
		REQUIRE( m_allocatedSize >= count + offset );
		return reinterpret_cast< uint8_t * >( BindableType::getOpenGl().MapBufferRange( m_target
			, offset
			, count
			, BindableType::getOpenGl().getBitfieldFlags( flags ) ) );
	}

	void GlBuffer::unlock()const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		getOpenGl().UnmapBuffer( m_target );
	}

	uint8_t * GlBuffer::lock( GlAccessType access )const
	{
		REQUIRE( this->getGlName() != GlInvalidIndex );
		return reinterpret_cast< uint8_t * >( BindableType::getOpenGl().MapBuffer( m_target
			, access ) );
	}

	void GlBuffer::doInitialiseStorage( uint32_t count
		, castor3d::BufferAccessType type
		, castor3d::BufferAccessNature nature )const
	{
		REQUIRE( count > 0 );
		REQUIRE( this->getGlName() != GlInvalidIndex );
		bind();
		getOpenGl().BufferData( m_target
			, count
			, nullptr
			, BindableType::getOpenGl().getBufferFlags( uint32_t( nature ) | uint32_t( type ) ) );
		m_allocatedSize = count;
		unbind();
	}
}
