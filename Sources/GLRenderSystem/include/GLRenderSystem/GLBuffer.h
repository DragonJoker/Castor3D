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
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___CS3D_GL_Buffer___
#define ___CS3D_GL_Buffer___

#include "Module_GLRender.h"

namespace Castor3D
{
	//**************************************************************************
	// Vertex Arrays
	//**************************************************************************

	class GLVBVertexBuffer : public VertexBuffer
	{
	public:
		GLVBVertexBuffer()
			:	VertexBuffer()
		{}

		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVBNormalsBuffer : public NormalsBuffer
	{
	public:
		GLVBNormalsBuffer()
			:	NormalsBuffer()
		{}

		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVBVertexAttribsBuffer : public VertexAttribsBuffer
	{
	public:
		GLVBVertexAttribsBuffer()
			:	VertexAttribsBuffer()
		{}

		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVBTextureBuffer : public TextureBuffer
	{
	public:
		GLVBTextureBuffer()
			:	TextureBuffer()
		{}

		virtual void Activate( Pass * p_pass);
		virtual void Deactivate();
	};

	//**************************************************************************
	// Vertex Buffer Objects
	//**************************************************************************

	class GLVBOVertexBuffer : public VertexBuffer
	{
	public:
		unsigned int m_index;

	public:
		GLVBOVertexBuffer()
			:	VertexBuffer(),
				m_index( 0)
		{}
		virtual ~GLVBOVertexBuffer(){ Cleanup(); }

		virtual void Cleanup();

		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVBONormalsBuffer : public NormalsBuffer
	{
	public:
		unsigned int m_index;

	public:
		GLVBONormalsBuffer()
			:	NormalsBuffer(),
				m_index( 0)
		{}
		virtual ~GLVBONormalsBuffer(){ Cleanup(); }

		virtual void Cleanup();

		virtual void Initialise();
		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVBOVertexAttribsBuffer : public VertexAttribsBuffer
	{
	public:
		unsigned int m_index;

	public:
		GLVBOVertexAttribsBuffer()
			:	VertexAttribsBuffer(),
				m_index( 0)
		{}
//		virtual ~GLVBOVertexAttribsBuffer(){ Cleanup(); }

//		virtual void Cleanup(){}

//		virtual void Initialise(){}
		virtual void Activate();
		virtual void Deactivate();
	};

	class GLVBOTextureBuffer : public TextureBuffer
	{
	public:
		unsigned int m_index;

	public:
		GLVBOTextureBuffer()
			:	TextureBuffer(),
				m_index( 0)
		{}
		virtual ~GLVBOTextureBuffer(){ Cleanup(); }

		virtual void Cleanup();

		virtual void Initialise();
		virtual void Activate( Pass * p_pass);
		virtual void Deactivate();
	};
}

#endif
