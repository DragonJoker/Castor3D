/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
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
			std::function< bool( int, uint32_t * ) >,
			std::function< bool( int, uint32_t const * ) >,
			std::function< bool( uint32_t ) > >
	{
		using BindableType = Bindable<
			std::function< bool( int, uint32_t * ) >,
			std::function< bool( int, uint32_t const * ) >,
			std::function< bool( uint32_t ) > >;

	public:
		GlBufferBase( OpenGl & p_gl, GlBufferTarget p_target );
		virtual ~GlBufferBase();

		bool Copy( GlBufferBase< T > const & p_src, uint32_t p_size )const;
		bool InitialiseStorage( uint32_t p_count, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )const;
		bool Upload( uint32_t p_offset, uint32_t p_count, T const * p_buffer )const;
		bool Download( uint32_t p_offset, uint32_t p_count, T * p_buffer )const;
		T * Lock( uint32_t p_offset, uint32_t p_count, Castor::FlagCombination< Castor3D::AccessType > const & p_flags )const;
		T * Lock( GlAccessType p_access )const;
		bool Unlock()const;
		void SetBindingPoint( uint32_t p_point )const;

		inline uint32_t GetBindingPoint()const
		{
			return m_bindingPoint;
		}

	private:
		GlBufferTarget m_target;
		mutable uint32_t m_bindingPoint{ 0u };
	};
}

#include "GlBufferBase.inl"

#endif
