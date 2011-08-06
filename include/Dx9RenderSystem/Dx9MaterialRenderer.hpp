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
#ifndef ___Dx9_MaterialRenderer___
#define ___Dx9_MaterialRenderer___

#include "Module_Dx9Render.hpp"

namespace Castor3D
{
	class Dx9PassRenderer : public PassRenderer
	{
	protected:
		typedef void (Dx9PassRenderer:: * ApplyFunc)( ePRIMITIVE_TYPE);
		typedef void (Dx9PassRenderer:: * VoidFunc)();
		typedef ApplyFunc PApplyFunc;
		typedef VoidFunc PVoidFunc;

		PApplyFunc	m_pfnApply;
		PVoidFunc	m_pfnApply2D;
		PVoidFunc	m_pfnRemove;

		D3DMATERIAL9 m_d3dMaterial;

	public:
		Dx9PassRenderer();
		virtual ~Dx9PassRenderer(){ Cleanup(); }

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