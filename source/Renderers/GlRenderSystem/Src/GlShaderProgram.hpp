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
	{
	public:
		GlShaderProgram( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		virtual ~GlShaderProgram();
		virtual void Initialise();
		/**
		 *\~english
		 *\brief		Link all Shaders
		 */
		virtual bool Link();
		/**
		 *\~english
		 *\brief		Get Linker Messages
		 */
		virtual void RetrieveLinkerLog( Castor::String & strLog );
		/**
		 *\~english
		 *\brief		Use Shader. OpenGL calls will go through vertex, geometry and/or fragment shaders.
		 */
		virtual void Bind( uint8_t p_byIndex, uint8_t p_byCount );
		/**
		 *\~english
		 *\brief		Stop using this shader. OpenGL calls will go through regular pipeline.
		 */
		virtual void Unbind();
		virtual int GetAttributeLocation( Castor::String const & p_strName )const;
		inline uint32_t & GetGlProgram()
		{
			return m_programObject;
		}
		inline uint32_t const & GetGlProgram()const
		{
			return m_programObject;
		}

	private:
		virtual Castor::String DoGetVertexShaderSource( uint32_t p_uiProgramFlags );
		virtual Castor::String DoGetPixelShaderSource( uint32_t p_uiFlags );
		virtual Castor::String DoGetDeferredVertexShaderSource( uint32_t p_uiProgramFlags, bool p_bLightPass );
		virtual Castor::String DoGetDeferredPixelShaderSource( uint32_t p_uiFlags );
		virtual Castor3D::ShaderObjectBaseSPtr DoCreateObject( Castor3D::eSHADER_TYPE p_eType );
		virtual std::shared_ptr< Castor3D::OneTextureFrameVariable > DoCreateTextureVariable( int p_iNbOcc );

	private:
		uint32_t m_programObject;
		Castor::String m_linkerLog;
		OpenGl & m_gl;
	};
}

#endif
