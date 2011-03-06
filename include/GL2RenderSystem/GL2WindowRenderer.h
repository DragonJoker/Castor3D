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
#ifndef ___Gl2_WindowRenderer___
#define ___Gl2_WindowRenderer___

#include "Module_Gl2Render.h"

namespace Castor3D
{
	class C3D_Gl2_API Gl2WindowRenderer : public WindowRenderer
	{
	protected:
		Context * m_context;

	public:
		Gl2WindowRenderer( SceneManager * p_pSceneManager);
		virtual ~Gl2WindowRenderer();

		virtual void Initialise();
		virtual void StartRender();
		virtual void EndRender();

		virtual void Resize( unsigned int p_width, unsigned int p_height);

	private:
		void _initialise();
	};
}

#endif
