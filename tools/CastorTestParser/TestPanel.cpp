#include "CastorTestParser/TestPanel.hpp"

#include <CastorUtils/Data/File.hpp>

#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/dcclient.h>

namespace test_parser
{
	namespace
	{
		wxImage loadImage( castor::Path const & filePath )
		{
			if ( !castor::File::fileExists( filePath ) )
			{
				return wxImage{};
			}

			wxImage result{ filePath.c_str() };

			if ( result.IsOk() )
			{
				return result;
			}

			return wxImage{};
		}
		
		wxImage loadRefImage( castor::Path const & folder
			, Test const & test )
		{
			return loadImage( folder / test.category / ( test.name + "_ref.png" ) );
		}

		wxImage loadResultImage( castor::Path const & folder
			, Test const & test )
		{
			return loadImage( folder / test.category / "Compare" / getFolderName( test.status ) / ( test.name + "_" + test.renderer + ".png" ) );
		}
	}

	class wxImagePanel
		: public wxPanel
	{
	public:
		wxImagePanel( wxWindow * parent )
			: wxPanel{ parent }
		{
			Connect( wxEVT_PAINT
				, wxPaintEventHandler( wxImagePanel::paintEvent )
				, nullptr
				, this );
			Connect( wxEVT_SIZE
				, wxSizeEventHandler( wxImagePanel::sizeEvent )
				, nullptr
				, this );
		}

		void setImage( wxImage image )
		{
			m_source = std::move( image );
			m_current = {};
			paintNow();
		}

		void paintNow()
		{
			wxClientDC dc( this );
			render( dc );
		}

	private:
		void render( wxDC & dc )
		{
			if ( m_source.IsOk() )
			{
				auto size = GetClientSize();

				if ( !m_current.IsOk()
					|| size.GetWidth() != m_current.GetWidth()
					|| size.GetHeight() != m_current.GetHeight() )
				{
					m_current = m_source.ResampleBicubic( size.GetWidth(), size.GetHeight() );
				}

				dc.DrawBitmap( m_current, 0, 0, false );
			}
			else
			{
				dc.FloodFill( 0, 0, GetBackgroundColour() );
			}
		}

		void paintEvent( wxPaintEvent & evt )
		{
			wxPaintDC dc( this );
			render( dc );
		}

		void sizeEvent( wxSizeEvent & evt )
		{
			paintNow();
		}

	private:
		wxImage m_source{};
		wxImage m_current{};
	};

	TestPanel::TestPanel( wxWindow * parent
		, Config const & config )
		: wxPanel{ parent }
		, m_config{ config }
	{
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		m_ref = new wxImagePanel{ this };
		m_result = new wxImagePanel{ this };

		wxBoxSizer * sizer{ new wxBoxSizer{ wxHORIZONTAL } };
		sizer->Add( m_ref, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		sizer->Add( m_result, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void TestPanel::setTest( Test & test )
	{
		m_test = &test;
		m_ref->setImage( loadRefImage( m_config.test, test ) );
		m_result->setImage( loadResultImage( m_config.test, test ) );
	}
}
