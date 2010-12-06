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
#ifndef ___GL2_WindowRenderer___
#define ___GL2_WindowRenderer___

#include "Module_GLRender.h"
#include "GL2RenderSystem.h"

namespace Castor3D
{
	class C3D_GL2_API GL2WindowRenderer : public WindowRenderer
	{
	protected:
		GLContext * m_context;

	public:
		GL2WindowRenderer()
			:	m_context( NULL)
		{}
		virtual ~GL2WindowRenderer(){}

		virtual void Initialise();
		virtual void StartRender();
		virtual void EndRender();

		virtual void Resize( unsigned int p_width, unsigned int p_height);

	private:
		void _initialise();
	};
}

#endif