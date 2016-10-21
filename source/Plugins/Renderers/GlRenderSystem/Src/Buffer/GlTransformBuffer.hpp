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
#ifndef ___GL_TRANSFORM_BUFFER_H___
#define ___GL_TRANSFORM_BUFFER_H___

#include "Buffer/GlBufferBase.hpp"

#include <Mesh/Buffer/GpuTransformBuffer.hpp>

namespace GlRender
{
	/*!
	\author		Sylvain DOREMUS
	\version 	0.9.0
	\date 		19/10/2016
	\~english
	\brief 		Class implementing OpenGL Transform feedback buffers.
	\~french
	\brief 		Classe implémentant les tampons de transform feedback OpenGL.
	*/
	class GlTransformBuffer
		: public Castor3D::GpuTransformBuffer
		, public Bindable< std::function< bool( int, uint32_t * ) >
			, std::function< bool( int, uint32_t const * ) >
			, std::function< bool( uint32_t ) > >
	{
		using BindableType = Bindable< std::function< bool( int, uint32_t * ) >
			, std::function< bool( int, uint32_t const * ) >
			, std::function< bool( uint32_t ) > >;

	public:
		GlTransformBuffer( OpenGl & p_gl, GlRenderSystem & p_renderSystem, Castor3D::ShaderProgram & p_program, Castor3D::TransformBufferDeclaration const & p_declaration );
		virtual ~GlTransformBuffer();
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Initialise
		 */
		bool Create()override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Cleanup
		 */
		void Destroy()override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Initialise
		 */
		bool Initialise()override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Cleanup
		 */
		void Cleanup()override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Update
		 */
		void Update()override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Bind
		 */
		bool Bind()override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Unbind
		 */
		void Unbind()override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Fill
		 */
		bool Fill( uint8_t const * p_buffer, ptrdiff_t p_size, Castor3D::BufferAccessType p_type, Castor3D::BufferAccessNature p_nature )override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Lock
		 */
		uint8_t * Lock( uint32_t p_offset, uint32_t p_count, Castor3D::AccessType p_flags )override;
		/**
		 *\copydoc		Castor3D::GpuTransformBuffer::Unlock
		 */
		void Unlock()override;

		inline uint32_t GetGlName()const
		{
			return m_glBuffer.GetGlName();
		}

	private:
		GlAttributeBaseSPtr DoCreateAttribute( Castor3D::TransformBufferElementDeclaration const & p_element );
		bool DoCreateAttributes();

	private:
		GlBufferBase< uint8_t > m_glBuffer;
		//! The transform attributes.
		GlAttributePtrArray m_attributes;
	};
}

#endif
