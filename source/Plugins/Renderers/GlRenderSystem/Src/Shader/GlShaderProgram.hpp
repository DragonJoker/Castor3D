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
		: public Castor3D::ShaderProgram
		, public Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >
	{
		using ObjectType = Object< std::function< uint32_t() >, std::function< void( uint32_t ) > >;

	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 */
		GlShaderProgram( OpenGl & p_gl, GlRenderSystem & p_renderSystem );
		/**
		 *\brief		Destructor.
		 */
		virtual ~GlShaderProgram();
		/**
		 *\copydoc		Castor3D::ShaderProgram::Initialise
		 */
		bool Initialise()override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Cleanup
		 */
		void Cleanup()override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Bind
		 */
		void Bind( bool p_bindUbo )const override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Unbind
		 */
		void Unbind()const override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Link
		 */
		bool Link()override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::GetLayout
		 */
		Castor3D::ProgramInputLayout const & GetLayout()const override
		{
			return m_layout;
		}
		/**
		 *\copydoc		Castor3D::ShaderProgram::GetLayout
		 */
		Castor3D::ProgramInputLayout & GetLayout()override
		{
			return m_layout;
		}
		/**
		 *\param[in]	p_name	The attribute name.
		 *\return		The given attribute location in program.
		 */
		int GetAttributeLocation( Castor::String const & p_name )const;

	private:
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateObject
		 */
		Castor3D::ShaderObjectSPtr DoCreateObject( Castor3D::ShaderType p_type )override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateUniformBuffer
		 */
		Castor3D::UniformBufferSPtr DoCreateUniformBuffer(
			Castor::String const & p_name,
			Castor::FlagCombination< Castor3D::ShaderTypeFlag > const & p_flags )override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateVariable
		 */
		std::shared_ptr< Castor3D::Uniform > DoCreateVariable( Castor3D::UniformType p_type, int p_occurences )override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoGetVertexShaderSource
		 */
		Castor::String DoRetrieveLinkerLog();

	private:
		void DoBindTransformLayout();

	private:
		Castor::String m_linkerLog;
		GlProgramInputLayout m_layout;
	};
}

#endif
