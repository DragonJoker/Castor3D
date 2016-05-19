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
#ifndef ___GL_CUBE_MAP_TEXTURE_H___
#define ___GL_CUBE_MAP_TEXTURE_H___

#include "GlRenderSystemPrerequisites.hpp"
#include "GlTexture.hpp"

#include <Texture/CubeMapTexture.hpp>

namespace GlRender
{
	class GlCubeMapTexture
		: public Castor3D::CubeMapTexture
	{
	public:
		GlCubeMapTexture( OpenGl & p_gl, GlRenderSystem & p_renderSystem );
		~GlCubeMapTexture();

		virtual bool Create();
		virtual void Destroy();
		virtual void Cleanup();
		virtual uint8_t * Lock( uint32_t p_lock );
		virtual void Unlock( bool p_modified );
		virtual void GenerateMipmaps()const;

		inline uint32_t GetGlName()const
		{
			return m_texture.GetGlName();
		}

	private:
		virtual bool DoBind( uint32_t p_index )const;
		virtual void DoUnbind( uint32_t p_index )const;
		virtual bool DoInitialise();

	private:
		GlTexture m_texture;
	};
}

#endif
