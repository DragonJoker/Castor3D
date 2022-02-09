/*
See LICENSE file in root folder
*/
#ifndef ___CTL_MainFrame_HPP___
#define ___CTL_MainFrame_HPP___

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <Castor3D/Miscellaneous/Parameter.hpp>
#include <Castor3D/Render/RenderWindow.hpp>

#include <CastorUtils/Log/Logger.hpp>
#include <CastorUtils/Data/Path.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4371 )
#pragma warning( disable: 5054 )
#include <wx/frame.h>
#include <wx/listctrl.h>
#include <wx/windowptr.h>
#pragma warning( pop )

namespace test_launcher
{
	using Clock = std::chrono::high_resolution_clock;

	struct FrameTimes
	{
		Clock::time_point start{};
		castor3d::Parameters params{};
	};

	class MainFrame
		: public wxFrame
	{
	public:
		explicit MainFrame( castor3d::Engine & engine
			, uint32_t maxFrameCount );

		bool initialise();
		bool loadScene( wxString const & fileName );
		void saveFrame( castor::String const & suffix
			, FrameTimes & times );
		void cleanup( castor::String const & suffix
			, FrameTimes const & times );

	private:
		castor3d::Engine & m_engine;
		uint32_t m_maxFrameCount;
		castor::Path m_filePath;
		castor3d::RenderWindowPtr m_renderWindow;
	};
}

#endif
