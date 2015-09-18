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
#ifndef ___CS_CASTOR_SHAPE_H___
#define ___CS_CASTOR_SHAPE_H___

#include "CastorShapePrerequisites.hpp"

#include <CastorApplication.hpp>

namespace CastorShape
{
	class CastorShapeApp
		: public GuiCommon::CastorApplication
	{
	public:
		CastorShapeApp();

		inline MainFrame * GetMainFrame()const
		{
			return m_mainFrame;
		}

	private:
		virtual void DoLoadAppImages();
		virtual wxWindow * DoInitialiseMainFrame( GuiCommon::SplashScreen * p_splashScreen );

	private:
		MainFrame * m_mainFrame;
		std::unique_ptr< wxLocale > m_locale;
	};
}

wxDECLARE_APP( CastorShape::CastorShapeApp );

#endif
