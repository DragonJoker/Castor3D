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

#include "GlRenderSystemPrerequisites.hpp"

#include <ShaderProgram.hpp>

namespace GlRender
{
	class GlShaderProgram
		: public Castor3D::ShaderProgramBase
		, public Castor::NonCopyable
		, public Object< std::function< uint32_t() >, std::function< bool( uint32_t ) > >
	{
		using ObjectType = Object< std::function< uint32_t() >, std::function< bool( uint32_t ) > >;

	public:
		GlShaderProgram( OpenGl & p_gl, GlRenderSystem & p_renderSystem );
		virtual ~GlShaderProgram();
		virtual void Initialise();
		/**
		 *\~english
		 *\brief		Cleans the program up
		 *\~french
		 *\brief		Nettoie le programme
		 */
		virtual void Cleanup();
		/**
		 *\~english
		 *\brief		Link all Shaders
		 */
		virtual bool Link();
		/**
		 *\~english
		 *\brief		Get Linker Messages
		 */
		virtual void RetrieveLinkerLog( Castor::String & p_log );
		/**
		 *\~english
		 *\brief		Use Shader. OpenGL calls will go through vertex, geometry and/or fragment shaders.
		 */
		virtual void Bind( uint8_t p_index, uint8_t p_count );
		/**
		 *\~english
		 *\brief		Stop using this shader. OpenGL calls will go through regular pipeline.
		 */
		virtual void Unbind();
		virtual int GetAttributeLocation( Castor::String const & p_name )const;

	private:
		virtual Castor3D::ShaderObjectBaseSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_type );
		virtual std::shared_ptr< Castor3D::OneTextureFrameVariable > DoCreateTextureVariable( int p_occurences );
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_programFlags )const;

	private:
		Castor::String m_linkerLog;
	};
}

#endif
