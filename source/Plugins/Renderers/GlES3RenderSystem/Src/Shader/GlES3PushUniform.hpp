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
#ifndef ___C3DGLES3_GlES3PushUniform_H___
#define ___C3DGLES3_GlES3PushUniform_H___

#include <Shader/PushUniform.hpp>

#include "Shader/GlES3UniformBase.hpp"

namespace GlES3Render
{
	template< Castor3D::UniformType Type >
	class GlES3PushUniform
		: public Castor3D::TPushUniform< Type >
		, public Holder
	{
	public:
		GlES3PushUniform( OpenGlES3 & p_gl, GlES3ShaderProgram & p_program, uint32_t p_occurences );
		virtual ~GlES3PushUniform();

		inline uint32_t GetGlES3Name() const
		{
			return m_glName;
		}

	private:
		/**
		 *\copydoc		Castor3D::Uniform::DoInitialise
		 */
		bool DoInitialise()override;
		/**
		 *\copydoc		Castor3D::Uniform::DoUpdate
		 */
		void DoUpdate()const override;

	private:
		uint32_t m_glName{ GlES3InvalidIndex };
		bool m_presentInProgram{ true };
	};
}

#include "GlES3PushUniform.inl"

#endif
