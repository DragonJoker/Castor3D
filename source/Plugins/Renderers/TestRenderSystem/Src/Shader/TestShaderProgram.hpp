/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
		virtual bool Initialise();
		/**
		 *\copydoc		Castor3D::ShaderProgram::Cleanup
		 */
		virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::ShaderProgram::Bind
		 */
		virtual void Bind( bool p_bindUbo )const;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Unbind
		 */
		virtual void Unbind()const;
		/**
		 *\copydoc		Castor3D::ShaderProgram::Link
		 */
		virtual bool Link();
		/**
		 *\copydoc		Castor3D::ShaderProgram::GetLayout
		 */
		virtual Castor3D::ProgramInputLayout const & GetLayout()const
		{
			return m_layout;
		}
		/**
		 *\copydoc		Castor3D::ShaderProgram::GetLayout
		 */
		virtual Castor3D::ProgramInputLayout & GetLayout()
		{
			return m_layout;
		}

	private:
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateObject
		 */
		virtual Castor3D::ShaderObjectSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_type );
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateTextureVariable
		 */
		virtual std::shared_ptr< Castor3D::OneIntFrameVariable > DoCreateTextureVariable( int p_occurences );
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoGetVertexShaderSource
		 */
		virtual Castor::String DoRetrieveLinkerLog();

	private:
		TestProgramInputLayout m_layout;
	};
}

#endif
