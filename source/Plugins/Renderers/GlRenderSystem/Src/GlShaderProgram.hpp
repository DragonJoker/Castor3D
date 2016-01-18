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

#include <ShaderProgram.hpp>

namespace GlRender
{
	class GlShaderProgram
		: public Castor3D::ShaderProgramBase
		, public Object< std::function< uint32_t() >, std::function< bool( uint32_t ) > >
	{
		using ObjectType = Object< std::function< uint32_t() >, std::function< bool( uint32_t ) > >;

	public:
		GlShaderProgram( OpenGl & p_gl, GlRenderSystem & p_renderSystem );
		virtual ~GlShaderProgram();
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::Initialise
		 */
		virtual bool Initialise();
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::Cleanup
		 */
		virtual void Cleanup();
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::Bind
		 */
		virtual void Bind( uint8_t p_index, uint8_t p_count );
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::Unbind
		 */
		virtual void Unbind();
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::Link
		 */
		virtual bool Link();
		/**
		 *\~english
		 *\param[in]	p_name	The attribute name.
		 *\return		The given attribute location in program.
		 *\french
		 *\param[in]	p_name	Le nom de l'attribut.
		 *\return		La position de l'attribut dans le programme.
		 */
		int GetAttributeLocation( Castor::String const & p_name )const;

	private:
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::DoCreateObject
		 */
		virtual Castor3D::ShaderObjectBaseSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_type );
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::DoCreateTextureVariable
		 */
		virtual std::shared_ptr< Castor3D::OneTextureFrameVariable > DoCreateTextureVariable( int p_occurences );
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::DoGetVertexShaderSource
		 */
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_programFlags )const;
		/**
		 *\copydoc		Castor3D::ShaderProgramBase::DoGetVertexShaderSource
		 */
		virtual Castor::String DoRetrieveLinkerLog();

	private:
		Castor::String m_linkerLog;
	};
}

#endif
