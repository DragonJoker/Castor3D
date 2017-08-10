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
#ifndef ___TRS_SHADER_OBJECT_H___
#define ___TRS_SHADER_OBJECT_H___

#include "TestRenderSystemPrerequisites.hpp"

#include <Shader/ShaderObject.hpp>

namespace TestRender
{
	class TestShaderObject
		: public castor3d::ShaderObject
	{
		friend class TestShaderProgram;

		using UIntStrMap = std::map< castor::String, uint32_t >;

	public:
		/**
		 * Constructor
		 */
		TestShaderObject( TestShaderProgram * p_parent, castor3d::ShaderType p_type );
		/**
		 * Destructor
		 */
		~TestShaderObject();
		/**
		 *\copydoc		castor3d::ShaderObject::create
		 */
		bool create()override;
		/**
		 *\copydoc		castor3d::ShaderObject::Destroy
		 */
		void destroy()override;
		/**
		 *\copydoc		castor3d::ShaderObject::Compile
		 */
		bool compile()override;
		/**
		 *\copydoc		castor3d::ShaderObject::Detach
		 */
		void detach()override;
		/**
		 *\copydoc		castor3d::ShaderObject::attachTo
		 */
		void attachTo( castor3d::ShaderProgram & p_program )override;

	private:
		/**
		 *\copydoc		castor3d::ShaderObject::doRetrieveCompilerLog
		 */
		castor::String doRetrieveCompilerLog()override;
	};
}

#endif
