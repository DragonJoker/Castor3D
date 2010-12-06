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
#ifndef ___C3D_Context___
#define ___C3D_Context___

namespace Castor3D
{
	class Context
	{
	protected:
		RenderWindow * m_window;

	public:
		Context( RenderWindow * p_window)
			:	m_window( p_window)
		{}
		virtual ~Context()=0
		{}

		virtual void SetCurrent()=0;
		virtual void EndCurrent()=0;
	};
}

#endif