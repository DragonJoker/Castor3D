/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Dx9_Buffer___
#define ___Dx9_Buffer___

#include "Module_Dx9Render.h"
#include "Dx9ShaderProgram.h"

namespace Castor3D
{
	//**************************************************************************
	// Vertex Buffer Objects
	//**************************************************************************

	class Dx9BufferObjectBase
	{
	protected:
		static size_t BuffersCount;
		IDirect3DDevice9 * m_pDevice;

	public:
		Dx9BufferObjectBase()
		{
			BuffersCount++;
		}
		virtual ~Dx9BufferObjectBase()
		{
		}
	};

	template <typename T, class D3dBufferObject>
	class Dx9VertexBufferObject : public Dx9BufferObjectBase
	{
	protected:
		D3dBufferObject * m_pBufferObject;
		T *& m_pBuffer;
		unsigned int & m_uiFilled;
		const BufferDeclaration & m_declaration;
		IDirect3DVertexDeclaration9 * m_pDx9Declaration;

	protected:
		unsigned int m_uiIndex;

	public:
		Dx9VertexBufferObject( unsigned int & p_uiFilled, T *& p_pBuffer, const BufferDeclaration & p_declaration);
		virtual ~Dx9VertexBufferObject();

		virtual void CleanupBufferObject();
		virtual void InitialiseBufferObject();
		virtual void ActivateBufferObject();
	};

	class Dx9IndexBuffer : public IndexBuffer, public Dx9BufferObjectBase
	{
	protected:
		IDirect3DIndexBuffer9 * m_pBufferObject;
		unsigned int *& m_pBuffer;
		unsigned int & m_uiFilled;
		unsigned int m_uiIndex;

	public:
		Dx9IndexBuffer();
		virtual ~Dx9IndexBuffer();

		virtual void Cleanup();
		virtual void Initialise( eBUFFER_MODE p_eMode);
		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();

	protected:
		virtual void _cleanupBufferObject();
	};

	class Dx9VertexBuffer : public VertexBuffer, public Dx9VertexBufferObject<real, IDirect3DVertexBuffer9>
	{
	public:
		Dx9VertexBuffer( const BufferElementDeclaration * p_pElements, size_t p_uiNbElements);
		virtual ~Dx9VertexBuffer();

		virtual void Cleanup();
		virtual void Initialise( eBUFFER_MODE p_eMode);
		virtual void Activate();
		virtual void Deactivate();
		virtual void * Lock( size_t p_uiOffset, size_t p_uiSize, size_t p_uiFlags);
		virtual void Unlock();

	protected:
		virtual void _cleanupBufferObject();
	};

#	include "Dx9Buffer.inl"
}

#endif
