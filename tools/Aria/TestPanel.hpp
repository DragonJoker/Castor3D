/*
See LICENSE file in root folder
*/
#ifndef ___CTP_TestPanel_HPP___
#define ___CTP_TestPanel_HPP___

#include "Prerequisites.hpp"

#include <CastorUtils/Graphics/ImageLoader.hpp>

#include <wx/panel.h>
#include <wx/image.h>

class wxComboBox;

namespace aria
{
	class wxImagePanel;

	class TestPanel
		: public wxPanel
	{
	public:
		TestPanel( wxWindow * parent
			, Config const & config );

		void refresh();
		void setTest( DatabaseTest & test );

		DatabaseTest * getTest()const
		{
			return m_test;
		}

	private:
		void loadRef( int index );
		void loadRes( int index );
		void onRefSelect( wxCommandEvent & evt );
		void onResSelect( wxCommandEvent & evt );

	private:
		Config const & m_config;
		DatabaseTest * m_test{};
		wxImagePanel * m_ref{};
		wxImagePanel * m_result{};
		castor::ImageLoader m_loader;
		int m_currentRef{};
		int m_currentRes{};
		wxImage m_refImage;
		wxImage m_resImage;
		wxImage m_refToResImage;
		wxImage m_resToRefImage;
	};
}

#endif
