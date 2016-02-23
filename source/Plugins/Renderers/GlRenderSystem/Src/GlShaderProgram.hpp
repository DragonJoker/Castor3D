/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

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
#ifndef ___GL_SHADER_PROGRAM_H___
#define ___GL_SHADER_PROGRAM_H___

#include "GlObject.hpp"
#include "GlProgramInputLayout.hpp"

#include <ShaderProgram.hpp>

namespace GlRender
{
	class GlShaderProgram
		: public Castor3D::ShaderProgram
		, public Object< std::function< uint32_t() >, std::function< bool( uint32_t ) > >
	{
		using ObjectType = Object< std::function< uint32_t() >, std::function< bool( uint32_t ) > >;

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
		virtual bool Initialise();
		/**
		 *\copydoc		Castor3D::ShaderProgram::Cleanup
		 */
		virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::ShaderProgram::Bind
		 */
		virtual void Bind( bool p_bindUbo );
		/**
		 *\copydoc		Castor3D::ShaderProgram::Unbind
		 */
		virtual void Unbind();
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
		/**
		 *\param[in]	p_name	The attribute name.
		 *\return		The given attribute location in program.
		 */
		int GetAttributeLocation( Castor::String const & p_name )const;

	private:
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateObject
		 */
		virtual Castor3D::ShaderObjectBaseSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_type );
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoCreateTextureVariable
		 */
		virtual std::shared_ptr< Castor3D::OneIntFrameVariable > DoCreateTextureVariable( int p_occurences );
		/**
		 *\copydoc		Castor3D::ShaderProgram::DoGetVertexShaderSource
		 */
		virtual Castor::String DoRetrieveLinkerLog();

	private:
		Castor::String m_linkerLog;
		GlProgramInputLayout m_layout;
	};
}

#endif
