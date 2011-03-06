/*
This source file is part of Castor3D (http://dragonjoker.co.cc

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
#ifndef ___Dx9_TextureRenderer___
#define ___Dx9_TextureRenderer___

#include "Module_Dx9Render.h"

namespace Castor3D
{	
	class C3D_Dx9_API Dx9TextureRenderer : public TextureRenderer
	{
	protected:
		IDirect3DTexture9 * m_pTextureObject;

		typedef void (Dx9TextureRenderer:: * VoidFunction)();
		typedef bool (Dx9TextureRenderer:: * BoolFunction)();
		typedef VoidFunction PVoidFunction;
		typedef BoolFunction PBoolFunction;

		PVoidFunction m_pfnRender;
		PVoidFunction m_pfnEndRender;
		PVoidFunction m_pfnCleanup;
		PBoolFunction m_pfnInitialise;

	public:
		Dx9TextureRenderer( SceneManager * p_pSceneManager);
		virtual ~Dx9TextureRenderer(){ Cleanup(); }

		virtual void Cleanup();
		virtual bool Initialise();

		virtual void Render();
		virtual void EndRender();

	protected:
		void _cleanup		();
		bool _initialise	();
		void _render		();
		void _endRender		();
	};
}

#endif