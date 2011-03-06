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
#ifndef ___Dx9_Context___
#define ___Dx9_Context___

#include "Module_Dx9Render.h"

#ifdef _WIN32
namespace Castor3D
{
	class C3D_Dx9_API Dx9Context : public Context, public MemoryTraced<Dx9Context>
    {
	protected:
		IDirect3DSwapChain9 * m_pSwapChain;
		IDirect3DSurface9 * m_pBackBuffer;
		HWND m_hWnd;

    public:
        Dx9Context( RenderWindow * p_window);
		virtual ~Dx9Context();

		virtual void SetCurrent();
		virtual void EndCurrent();
		virtual void SwapBuffers();

	private:
		void _createDxContext();
    };
}
#endif

#endif
