/*
See LICENSE file in root folder
*/
#ifndef ___GL_BUFFER_H___
#define ___GL_BUFFER_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "Common/GlBindable.hpp"

#include <Mesh/Buffer/GpuBuffer.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version 	0.6.1.0
	\date 		03/01/2011
	\~english
	\brief 		Class implementing OpenGL VBO.
	\~french
	\brief 		Classe implémentant les VBO OpenGL.
	*/
	class GlBuffer
		: public castor3d::GpuBuffer
		, public Bindable<
			std::function< void( int, uint32_t * ) >,
			std::function< void( int, uint32_t const * ) >,
			std::function< void( uint32_t ) > >
	{
		using BindableType = Bindable<
			std::function< void( int, uint32_t * ) >,
			std::function< void( int, uint32_t const * ) >,
			std::function< void( uint32_t ) > >;

	public:
		GlBuffer( GlRenderSystem & renderSystem
			, OpenGl & gl
			, GlBufferTarget target );
		virtual ~GlBuffer();
		/**
		 *\copydoc		castor3d::GpuBuffer::create
		 */
		bool create()override;
		/**
		 *\copydoc		castor3d::GpuBuffer::destroy
		 */
		void destroy()override;
		/**
		 *\copydoc		castor3d::GpuBuffer::setBindingPoint
		 */
		void setBindingPoint( uint32_t index )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::getBindingPoint
		 */
		uint32_t getBindingPoint()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::bind
		 */
		void bind()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::unbind
		 */
		void unbind()const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::copy
		 */
		void copy( castor3d::GpuBuffer const & src
			, uint32_t srcOffset
			, uint32_t dstOffset
			, uint32_t size )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::upload
		 */
		void upload( uint32_t offset
			, uint32_t count
			, uint8_t const * buffer )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::download
		 */
		void download( uint32_t offset
			, uint32_t count
			, uint8_t * buffer )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::lock
		 */
		uint8_t * lock( uint32_t offset
			, uint32_t count
			, castor3d::AccessTypes const & flags )const override;
		/**
		 *\copydoc		castor3d::GpuBuffer::unlock
		 */
		void unlock()const override;

		uint8_t * lock( GlAccessType access )const;
		/**
		*\copydoc		castor3d::GpuBuffer::doInitialiseStorage
		*/
		void doInitialiseStorage( uint32_t count
			, castor3d::BufferAccessType type
			, castor3d::BufferAccessNature nature )const override;

	private:
		GlBufferTarget m_target;
		mutable uint32_t m_bindingPoint{ 0u };
		mutable uint32_t m_allocatedSize{ 0u };
	};
}

#endif
