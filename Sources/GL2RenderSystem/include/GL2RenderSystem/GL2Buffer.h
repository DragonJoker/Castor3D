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
#ifndef ___GL2_Buffer___
#define ___GL2_Buffer___

#include "Module_GLRender.h"
#include "GL2RenderSystem.h"

namespace Castor3D
{
	//**************************************************************************
	// Vertex Arrays
	//**************************************************************************

	class C3D_GL2_API GLVBVertexBuffer : public VertexBuffer
	{
	public:
		GLVBVertexBuffer()
			:	VertexBuffer()
		{
		}

		virtual void Activate();
		virtual void Deactivate();
	};

	class C3D_GL2_API GLVBNormalsBuffer : public NormalsBuffer
	{
	public:
		GLVBNormalsBuffer()
			:	NormalsBuffer()
		{
		}

		virtual void Activate();
		virtual void Deactivate();
	};

	class C3D_GL2_API GLVBTextureBuffer : public TextureBuffer
	{
	public:
		GLVBTextureBuffer()
			:	TextureBuffer()
		{
		}

		virtual void Activate( PassPtr p_pass);
		virtual void Deactivate();
	};

	//**************************************************************************
	// Vertex Buffer Objects / Vertex Attribs
	//**************************************************************************

	class C3D_GL2_API GLVBOTextureBuffer : public TextureBuffer
	{
	public:
		unsigned int m_index;
		std::vector <GLVertexAttribsBuffer2rPtr> m_arrayBuffers;

	public:
		GLVBOTextureBuffer();
		virtual ~GLVBOTextureBuffer();

		virtual void Cleanup();
		virtual void Initialise( PassPtr p_pass);
		virtual void Activate( PassPtr p_pass);
		virtual void Deactivate();
		void SetShaderProgram( ShaderProgram * p_pProgram);
	};

	//**************************************************************************
	// Texture Buffer Object
	//**************************************************************************

	class C3D_GL2_API GLTextureBufferObject : public TextureBufferObject
	{
	public:
		unsigned int m_uiIndex;

	public:
		GLTextureBufferObject();
		virtual ~GLTextureBufferObject();

		virtual void Cleanup();
		virtual void Initialise( const Resource::PixelFormat & p_format, size_t p_uiSize, const unsigned char * p_pBytes);

		inline unsigned int	GetIndex	()const { return m_uiIndex; }
	};
}

#endif