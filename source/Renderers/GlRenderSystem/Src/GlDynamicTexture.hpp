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
#ifndef ___GL_DYNAMIC_TEXTURE_H___
#define ___GL_DYNAMIC_TEXTURE_H___

#include "GlRenderSystemPrerequisites.hpp"

#include <DynamicTexture.hpp>

namespace GlRender
{
	class GlDynamicTexture
		:	public Castor3D::DynamicTexture
		,	public Castor::NonCopyable
	{
	public:
		GlDynamicTexture( OpenGl & p_gl, GlRenderSystem * p_pRenderSystem );
		~GlDynamicTexture();

		virtual bool Create();
		virtual void Destroy();
		virtual void Cleanup();
		virtual uint8_t * Lock( uint32_t p_uiLock );
		virtual void Unlock( bool p_bModified );
		virtual void GenerateMipmaps();
		virtual void Fill( uint8_t const * p_pBuffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_eFormat );

		inline uint32_t GetGlName()const
		{
			return m_uiGlName;
		}

	private:
		virtual bool DoBind( uint32_t p_index );
		virtual void DoUnbind( uint32_t p_index );
		virtual bool DoInitialise();

	private:
		uint32_t m_uiGlName;
		GlRenderSystem * m_pGlRenderSystem;
		eGL_TEXTURE_INDEX m_eIndex;
		eGL_TEXDIM m_eGlDimension;
		OpenGl & m_gl;
	};
}

#endif
