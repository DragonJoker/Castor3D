/* See LICENSE file in root folder */
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
