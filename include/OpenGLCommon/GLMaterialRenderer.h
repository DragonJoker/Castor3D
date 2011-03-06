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
#ifndef ___Gl_MaterialRenderer___
#define ___Gl_MaterialRenderer___

#include "Module_GlRender.h"

namespace Castor3D
{
	class GlPassRenderer : public PassRenderer
	{
	protected:
		typedef void (GlPassRenderer:: * ApplyFunc)( ePRIMITIVE_TYPE);
		typedef void (GlPassRenderer:: * VoidFunc)();
		typedef ApplyFunc PApplyFunc;
		typedef VoidFunc PVoidFunc;

		PApplyFunc	m_pfnApply;
		PVoidFunc	m_pfnApply2D;
		PVoidFunc	m_pfnRemove;

	public:
		GlPassRenderer( SceneManager * p_pSceneManager);
		virtual ~GlPassRenderer(){ Cleanup(); }

		virtual void Cleanup();
		virtual void Initialise();

		virtual void Render( ePRIMITIVE_TYPE p_displayMode);
		virtual void Render2D();
		virtual void EndRender();

	private:
		void _apply( ePRIMITIVE_TYPE p_displayMode);
		void _apply2D();
		void _remove();
	};
}

#endif