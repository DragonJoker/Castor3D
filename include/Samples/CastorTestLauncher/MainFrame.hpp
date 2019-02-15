/*
See LICENSE file in root folder
*/
#ifndef ___CTL_MainFrame_HPP___
#define ___CTL_MainFrame_HPP___

#include <Castor3D/Castor3DPrerequisites.hpp>

#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/windowptr.h>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/Path.hpp>

namespace test_launcher
{
	class MainFrame
		: public wxFrame
	{
	public:
		explicit MainFrame( castor3d::Engine & engine );
		~MainFrame();

		bool initialise();
		bool loadScene( wxString const & fileName );
		void saveFrame( castor::String const & renderer );
		void cleanup();

	private:
		castor3d::Engine & m_engine;
		castor::Path m_filePath;
		castor3d::RenderWindowSPtr m_renderWindow;
	};
}

#endif
