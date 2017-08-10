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
#ifndef ___C3DGL_GlUniformBufferBinding_H___
#define ___C3DGL_GlUniformBufferBinding_H___

#include <Shader/UniformBufferBinding.hpp>

#include "GlRenderSystemPrerequisites.hpp"
#include "Common/GlHolder.hpp"

namespace GlRender
{
	class GlUniformBufferBinding
		: public castor3d::UniformBufferBinding
		, public Holder
	{
	public:
		GlUniformBufferBinding( OpenGl & p_gl
			, castor3d::UniformBuffer & p_ubo
			, GlShaderProgram const & p_program );
		virtual ~GlUniformBufferBinding();

	private:
		void doBind( uint32_t p_index )const override;

	private:
		int m_blockIndex;
	};
}

#endif
