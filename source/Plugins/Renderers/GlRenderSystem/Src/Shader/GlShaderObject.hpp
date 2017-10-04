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
#ifndef ___GL_SHADER_OBJECT_H___
#define ___GL_SHADER_OBJECT_H___

#include "Common/GlObject.hpp"

#include <Shader/ShaderObject.hpp>

namespace GlRender
{
	class GlShaderObject
		: public castor3d::ShaderObject
		, public Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >
	{
		friend class GlShaderProgram;

		using ObjectType = Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >;
		using UIntStrMap = std::map< castor::String, uint32_t >;

	public:
		/**
		 * Constructor
		 */
		GlShaderObject( OpenGl & p_gl, GlShaderProgram & p_parent, castor3d::ShaderType p_type );
		/**
		 * Destructor
		 */
		virtual ~GlShaderObject();
		/**
		 *\copydoc		castor3d::ShaderObject::create
		 */
		virtual bool create();
		/**
		 *\copydoc		castor3d::ShaderObject::Destroy
		 */
		virtual void destroy();
		/**
		 *\copydoc		castor3d::ShaderObject::Compile
		 */
		virtual bool compile();
		/**
		 *\copydoc		castor3d::ShaderObject::Detach
		 */
		virtual void detach();
		/**
		 *\copydoc		castor3d::ShaderObject::attachTo
		 */
		virtual void attachTo( castor3d::ShaderProgram & p_program );

	private:
		/**
		 *\copydoc		castor3d::ShaderObject::doRetrieveCompilerLog
		 */
		virtual castor::String doRetrieveCompilerLog();
		/**
		 *\copydoc		castor3d::ShaderProgram::doCreateVariable
		 */
		std::shared_ptr< castor3d::PushUniform > doCreateUniform( castor3d::UniformType type
			, int occurences )override;

	protected:
		GlShaderProgram * m_shaderProgram;
		UIntStrMap m_mapParamsByName;
	};
}

#endif
