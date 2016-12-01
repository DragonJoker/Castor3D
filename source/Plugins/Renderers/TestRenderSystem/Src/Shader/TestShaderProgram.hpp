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
#ifndef ___TRS_SHADER_PROGRAM_H___
#define ___TRS_SHADER_PROGRAM_H___

#include "Shader/TestProgramInputLayout.hpp"

#include <Shader/ShaderProgram.hpp>

namespace TestRender
{
	class TestShaderProgram
		: public Castor3D::ShaderProgram
	{
	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 */
		TestShaderProgram( TestRenderSystem & p_renderSystem );
		/**
		 *\brief		Destructor.
		 */
		virtual ~TestShaderProgram();
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

	private:
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateObject
		 */
		Castor3D::ShaderObjectSPtr DoCreateObject( Castor3D::ShaderType p_type )override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateFrameVariableBuffer
		 */
		Castor3D::FrameVariableBufferSPtr DoCreateFrameVariableBuffer( Castor::String const & p_name, Castor::FlagCombination< Castor3D::ShaderTypeFlag > const & p_flags )override;
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateTextureVariable
		 */
		std::shared_ptr< Castor3D::FrameVariable > DoCreateVariable( Castor3D::FrameVariableType p_type, int p_occurences )override;

	private:
		TestProgramInputLayout m_layout;
	};
}

#endif
