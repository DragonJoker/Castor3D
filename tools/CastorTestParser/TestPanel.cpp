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
			return loadImage( folder / getReferenceFolder( test )/ getReferenceName( test ) );
		}

		wxImage loadResultImage( castor::Path const & folder
			, Test const & test )
		{
			return loadImage( folder / getResultFolder( test ) / getResultName( test ) );
		}
	}

	class wxImagePanel
		: public wxPanel
	{
	public:
		wxImagePanel( wxWindow * parent )
			: wxPanel{ parent }
		{
			SetBackgroundColour( BORDER_COLOUR );
			SetForegroundColour( PANEL_FOREGROUND_COLOUR );
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
			dc.Clear();

			if ( m_source.IsOk() )
			{
				auto size = GetClientSize();

				if ( !m_current.IsOk()
					|| size.GetWidth() != m_current.GetWidth()
					|| size.GetHeight() != m_current.GetHeight() )
				{
					auto ratio = m_source.GetHeight() / float( m_source.GetWidth() );
					auto w = size.GetWidth();
					auto h = w * ratio;

					if ( h > size.GetHeight() )
					{
						h = size.GetHeight();
						w = h / ratio;
					}

					m_current = m_source.ResampleBicubic( std::max( int( w ), 1 ), std::max( int( h ), 1 ) );
				}

				if ( m_current.IsOk() )
				{
					dc.DrawBitmap( m_current
						, ( size.GetWidth() - m_current.GetWidth() ) / 2
						, ( size.GetHeight() - m_current.GetHeight() ) / 2
						, false );
				}
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
		m_result->setImage( loadResultImage( m_config.work, test ) );
	}
}
