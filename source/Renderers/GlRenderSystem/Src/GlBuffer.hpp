/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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

#include <GpuBuffer.hpp>

namespace GlRender
{
	//! Vertex Buffer Object class
	/*!
	Implements OpenGl VBO
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template< typename T >
	class GlBuffer
		:	public Castor3D::GpuBuffer< T >
	{
	protected:
		typedef typename Castor3D::GpuBuffer< T >::HardwareBufferPtr HardwareBufferPtr;

	public:
		GlBuffer( OpenGl & p_gl, eGL_BUFFER_TARGET p_eTarget, HardwareBufferPtr p_pBuffer );
		virtual ~GlBuffer();

		virtual bool Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );

		inline uint32_t GetGlIndex()const
		{
			return m_glBuffer.GetGlIndex();
		}
		HardwareBufferPtr GetCpuBuffer()const
		{
			return m_pBuffer;
		}

	protected:
		virtual bool DoCreate();
		virtual void DoDestroy();
		virtual bool DoInitialise( Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		virtual void DoCleanup();
		virtual bool DoBind();
		virtual void DoUnbind();
		virtual bool DoFill( T * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_type, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );
		virtual T * DoLock( uint32_t p_uiOffset, uint32_t p_uiCount, uint32_t p_uiFlags );
		virtual T * DoLock( eGL_LOCK p_access );
		virtual void DoUnlock();

	protected:
		GlBufferBase< T > m_glBuffer;
		HardwareBufferPtr m_pBuffer;
		OpenGl & m_gl;
	};
}

#include "GlBuffer.inl"

#endif
