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
#ifndef ___GL_SHADER_PROGRAM_H___
#define ___GL_SHADER_PROGRAM_H___

#include "Common/GlObject.hpp"
#include "Shader/GlProgramInputLayout.hpp"

#include <Shader/ShaderProgram.hpp>

namespace GlRender
{
	class GlShaderProgram
		: public castor3d::ShaderProgram
		, public Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >
	{
		using ObjectType = Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >;

	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	renderSystem	The RenderSystem.
		 */
		GlShaderProgram( OpenGl & p_gl, GlRenderSystem & renderSystem );
		/**
		 *\brief		Destructor.
		 */
		virtual ~GlShaderProgram();
		/**
		 *\copydoc		castor3d::ShaderProgram::initialise
		 */
		bool initialise()override;
		/**
		 *\copydoc		castor3d::ShaderProgram::cleanup
		 */
		void cleanup()override;
		/**
		 *\copydoc		castor3d::ShaderProgram::bind
		 */
		void bind()const override;
		/**
		 *\copydoc		castor3d::ShaderProgram::unbind
		 */
		void unbind()const override;
		/**
		 *\copydoc		castor3d::ShaderProgram::link
		 */
		bool link()override;
		/**
		 *\copydoc		castor3d::ShaderProgram::getLayout
		 */
		castor3d::ProgramInputLayout const & getLayout()const override
		{
			return m_layout;
		}
		/**
		 *\copydoc		castor3d::ShaderProgram::getLayout
		 */
		castor3d::ProgramInputLayout & getLayout()override
		{
			return m_layout;
		}
		/**
		 *\param[in]	p_name	The attribute name.
		 *\return		The given attribute location in program.
		 */
		int getAttributeLocation( castor::String const & p_name )const;

	private:
		/**
		 *\copydoc		castor3d::ShaderProgram::doCreateObject
		 */
		castor3d::ShaderObjectSPtr doCreateObject( castor3d::ShaderType p_type )override;
		/**
		 *\copydoc		castor3d::ShaderProgram::doCreateVariable
		 */
		std::shared_ptr< castor3d::PushUniform > doCreateUniform( castor3d::UniformType p_type, int p_occurences )override;
		/**
		 *\copydoc		castor3d::ShaderProgram::doRetrieveLinkerLog
		 */
		castor::String doRetrieveLinkerLog();

	private:
		void doBindTransformLayout();

	private:
		castor::String m_linkerLog;
		GlProgramInputLayout m_layout;
	};
}

#endif
