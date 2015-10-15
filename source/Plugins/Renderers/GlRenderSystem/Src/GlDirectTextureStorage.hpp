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
#ifndef ___GL_DIRECT_TEXTURE_STORAGE_H___
#define ___GL_DIRECT_TEXTURE_STORAGE_H___

#include "GlTextureStorage.hpp"

namespace GlRender
{
	/*!
	\author		Sylvain Doremus.
	\version	0.8.0
	\date		12/10/2015
	\brief		Class used to handle texture storage buffer, using directly the texture (for static textures).
	*/
	class GlDirectTextureStorage
		: public GlTextureStorage
	{
	public:
		/**
		 *\brief		Constructor.
		 *\param[in]	p_gl			The OpenGL APIs.
		 *\param[in]	p_renderSystem	The RenderSystem.
		 */
		GlDirectTextureStorage( OpenGl & p_gl, GlRenderSystem & p_renderSystem );
		/**
		 *\brief		Destructor.
		 */
		~GlDirectTextureStorage();

	private:
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual bool DoCreate();
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual void DoDestroy();
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual bool DoInitialise();
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual void DoCleanup();
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual void DoFill( uint8_t const * p_buffer, Castor::Size const & p_size, Castor::ePIXEL_FORMAT p_format );
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual bool DoBind( uint32_t p_index );
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual void DoUnbind( uint32_t p_index );
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual uint8_t * DoLock( uint32_t p_lock );
		/**
		 *\copydoc		GlRender::GlTextureStorage::DoCreate
		 */
		virtual void DoUnlock( bool p_modified );
	};
}

#endif
