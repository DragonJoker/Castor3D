#pragma once

#include <CastorApplication.hpp>

#include "CastorDvpTDPrerequisites.hpp"

namespace castortd
{
	class CastorDvpTD
		: public GuiCommon::CastorApplication
	{
	public:
		CastorDvpTD();

		inline MainFrame * GetMainFrame()const
		{
			return m_mainFrame;
		}

	private:
		void DoLoadAppImages()override;
		wxWindow * DoInitialiseMainFrame( GuiCommon::SplashScreen * p_splashScreen )override;

	private:
		MainFrame * m_mainFrame{ nullptr };
	};
}

wxDECLARE_APP( castortd::CastorDvpTD );
