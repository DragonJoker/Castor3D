/*
See LICENSE file in root folder
*/
#ifndef ___GL_BUFFER_BASE_H___
#define ___GL_BUFFER_BASE_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "Common/GlBindable.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		OpenGL buffer objects base class
	\~french
	\brief		Classe de base des buffers objects OpenGL
	*/
	template< typename T >
	class GlBufferBase
		: public Bindable<
			std::function< void( int, uint32_t * ) >,
			std::function< void( int, uint32_t const * ) >,
			std::function< void( uint32_t ) > >
	{
		using BindableType = Bindable<
			std::function< void( int, uint32_t * ) >,
			std::function< void( int, uint32_t const * ) >,
			std::function< void( uint32_t ) > >;

	public:
		GlBufferBase( OpenGl & p_gl, GlBufferTarget p_target );
		virtual ~GlBufferBase();

		void copy( GlBufferBase< T > const & p_src, uint32_t p_size )const;
		void initialiseStorage( uint32_t p_count, castor3d::BufferAccessType p_type, castor3d::BufferAccessNature p_nature )const;
		void upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const;
		void download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const;
		T * lock( uint32_t p_offset, uint32_t p_count, castor3d::AccessTypes const & p_flags )const;
		T * lock( GlAccessType p_access )const;
		void unlock()const;
		void setBindingPoint( uint32_t p_point )const;

		inline uint32_t getBindingPoint()const
		{
			return m_bindingPoint;
		}

	private:
		GlBufferTarget m_target;
		mutable uint32_t m_bindingPoint{ 0u };
		mutable uint32_t m_allocatedSize{ 0u };
	};
}

#include "GlBufferBase.inl"

#endif
