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
#ifndef ___Gl_TextureRenderer___
#define ___Gl_TextureRenderer___

#include "Module_GlRender.hpp"

namespace Castor3D
{	
	class GlTextureRenderer : public TextureRenderer
	{
	protected:
		unsigned int m_glTexName;
		unsigned int m_uiIndex;

		typedef void (GlTextureRenderer:: * VoidFunction)();
		typedef bool (GlTextureRenderer:: * BoolFunction)();
		typedef VoidFunction PVoidFunction;
		typedef BoolFunction PBoolFunction;

		PVoidFunction			m_pfnRender;
		PVoidFunction			m_pfnEndRender;
		PVoidFunction			m_pfnCleanup;
		PBoolFunction			m_pfnInitialise;
		GlUnpackPixelBuffer	*	m_pPixelBuffer;
#if CASTOR_USE_OPENGL4
		GlTextureBufferObjectPtr m_pTextureBufferObject;
#endif
	public:
		GlTextureRenderer();
		virtual ~GlTextureRenderer();

		virtual void Cleanup();
		virtual bool Initialise();

		virtual void Render();
		virtual void EndRender();

		void UploadImage();
		void DownloadImage();

		void * LockImage( GLenum p_mode);
		void UnlockImage( bool p_bModified);

	public:
		inline unsigned int GetGlTexName()const { return m_glTexName; }

	protected:
		void _cleanup		();
		bool _initialise	();
		void _render		();
		void _endRender		();

		void _deletePbos	();
		void _cleanupPbos	();

		void _cleanupGl3	();
		bool _initialiseGl3	();
		void _renderGl3		();
		void _endRenderGl3	();
#if CASTOR_USE_OPENGL4
		void _cleanupGl4	();
		bool _initialiseGl4	();
		void _renderGl4		();
		void _endRenderGl4	();
#endif
	};
}

#endif