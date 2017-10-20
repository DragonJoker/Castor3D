/* See LICENSE file in root folder */
#ifndef ___C3DGLES3_BUFFER_BASE_H___
#define ___C3DGLES3_BUFFER_BASE_H___

#include "GlES3RenderSystemPrerequisites.hpp"
#include "Common/GlES3Bindable.hpp"

namespace GlES3Render
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
	class GlES3BufferBase
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
		GlES3BufferBase( OpenGlES3 & p_gl, GlES3BufferTarget p_target );
		virtual ~GlES3BufferBase();

		void Copy( GlES3BufferBase< T > const & p_src, uint32_t p_size )const;
		void InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const;
		void Upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const;
		void Download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const;
		T * Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessTypes const & p_flags )const;
		T * Lock( Castor3D::AccessTypes const & p_flags )const;
		void Unlock()const;
		void SetBindingPoint( uint32_t p_point )const;

		inline uint32_t GetBindingPoint()const
		{
			return m_bindingPoint;
		}

	private:
		GlES3BufferTarget m_target;
		mutable uint32_t m_bindingPoint{ 0u };
		mutable uint32_t m_allocatedSize{ 0u };
	};
}

#include "GlES3BufferBase.inl"

#endif
