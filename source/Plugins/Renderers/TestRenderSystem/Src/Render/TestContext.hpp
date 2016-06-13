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
#ifndef ___TRS_CONTEXT_H___
#define ___TRS_CONTEXT_H___

#include "Miscellaneous/TestQuery.hpp"

#include <Render/Context.hpp>

namespace TestRender
{
	class TestContext
		: public Castor3D::Context
	{
	public:
		TestContext( TestRenderSystem & p_renderSystem );
		virtual ~TestContext();

	private:
		/**
		 *\copydoc		Castor3D::Context::DoInitialise
		 */
		virtual bool DoInitialise();
		/**
		 *\copydoc		Castor3D::Context::DoCleanup
		 */
		virtual void DoCleanup();
		/**
		 *\copydoc		Castor3D::Context::DoDestroy
		 */
		virtual void DoDestroy();
		/**
		 *\copydoc		Castor3D::Context::DoSetCurrent
		 */
		virtual void DoSetCurrent();
		/**
		 *\copydoc		Castor3D::Context::DoEndCurrent
		 */
		virtual void DoEndCurrent();
		/**
		 *\copydoc		Castor3D::Context::DoSwapBuffers
		 */
		virtual void DoSwapBuffers();
	};
}

#endif
