/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GL_BUFFER_H___
#define ___GL_BUFFER_H___

#include "GlBufferBase.hpp"

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
	template< typename T >
	class GlBuffer
		: public Castor3D::GpuBuffer< T >
		, public Holder
	{
	protected:
		typedef typename Castor3D::GpuBuffer< T >::HardwareBufferPtr HardwareBufferPtr;

	public:
		GlBuffer( GlRenderSystem & p_renderSystem, OpenGl & p_gl, eGL_BUFFER_TARGET p_target, HardwareBufferPtr p_buffer );
		virtual ~GlBuffer();

		virtual bool Fill( T const * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );

		inline uint32_t GetGlName()const
		{
			return m_glBuffer.GetGlName();
		}
		HardwareBufferPtr GetCpuBuffer()const
		{
			return m_pBuffer;
		}

	protected:
		virtual bool DoCreate();
		virtual void DoDestroy();
		virtual bool DoInitialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );
		virtual void DoCleanup();
		virtual bool DoBind();
		virtual void DoUnbind();
		virtual bool DoFill( T * p_buffer, ptrdiff_t p_size, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_nature );
		virtual T * DoLock( uint32_t p_offset, uint32_t p_count, uint32_t p_flags );
		virtual T * DoLock( eGL_LOCK p_access );
		virtual void DoUnlock();

	protected:
		GlBufferBase< T > m_glBuffer;
		HardwareBufferPtr m_pBuffer;
	};
}

#include "GlBuffer.inl"

#endif
