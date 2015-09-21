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
#ifndef ___DX11_BUFFER_OBJECT_H___
#define ___DX11_BUFFER_OBJECT_H___

#include "Dx11BufferObjectBase.hpp"

#include <GpuBuffer.hpp>
#include <CpuBuffer.hpp>

namespace Dx11Render
{
	template< typename T, class D3dBufferObject >
	class DxBufferObject
		:	public DxBufferObjectBase
		,	public Castor3D::GpuBuffer< T >
	{
	public:
		DxBufferObject( HardwareBufferPtr p_pBuffer );
		virtual ~DxBufferObject();

		virtual bool Fill( T const * p_pBuffer, ptrdiff_t p_iSize, Castor3D::eBUFFER_ACCESS_TYPE p_eType, Castor3D::eBUFFER_ACCESS_NATURE p_eNature );

		HardwareBufferPtr GetCpuBuffer()const
		{
			return m_pBuffer;
		}

	protected:
		void DoDestroy();
		void DoCleanup();
		T * DoLock( uint32_t p_uiOffset, uint32_t p_uiSize, uint32_t p_uiFlags );
		void DoUnlock();

	protected:
		D3dBufferObject	* m_pBufferObject;
		HardwareBufferPtr m_pBuffer;
		uint32_t m_uiIndex;
	};
}

#include "Dx11BufferObject.inl"

#endif
