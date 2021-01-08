#include "Aria/TestPanel.hpp"

#include "Aria/DatabaseTest.hpp"
#include "Aria/TestDatabase.hpp"

#include <CastorUtils/Data/File.hpp>
#include <CastorUtils/Graphics/Image.hpp>
#include <CastorUtils/Graphics/StbImageLoader.hpp>

#include <wx/sizer.h>
#include <wx/bitmap.h>
#include <wx/combobox.h>
#include <wx/dcclient.h>

namespace aria
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
			, TestRun const & test )
		{
			return loadImage( folder / getReferenceFolder( test ) / getReferenceName( test ) );
		}

		wxImage loadResultImage( castor::Path const & folder
			, TestRun const & test )
		{
			return loadImage( folder / getResultFolder( test ) / getResultName( test ) );
		}

		wxImage compareImages( castor::ImageLoader const & loader
			, castor::Path const & refFile
			, castor::Path const & testFile )
		{
			if ( !castor::File::fileExists( refFile ) )
			{
				castor::Logger::logError( "CompareImages - Image file doesn't exist: " + refFile );
				return wxImage{};
			}

			if ( !castor::File::fileExists( testFile ) )
			{
				castor::Logger::logError( "CompareImages - Image file doesn't exist: " + testFile );
				return wxImage{};
			}

			castor::Image reference = loader.load( castor::cuEmptyString, refFile );
			castor::Image toTest = loader.load( castor::cuEmptyString, testFile );

			if ( toTest.getDimensions() != reference.getDimensions() )
			{
				castor::Logger::logError( "CompareImages - Images dimensions don't match: " + testFile );
				return wxImage{};
			}

			auto src = castor::PxBufferBase::create( toTest.getDimensions()
				, castor::PixelFormat::eR8G8B8A8_UNORM
				, reference.getPixels()->getConstPtr()
				, reference.getPixels()->getFormat() );
			auto dst = castor::PxBufferBase::create( toTest.getDimensions()
				, castor::PixelFormat::eR8G8B8A8_UNORM
				, toTest.getPixels()->getConstPtr()
				, toTest.getPixels()->getFormat() );
			wxImage result{ int( src->getWidth() )
				, int( src->getHeight() ) };
			result.SetType( wxBitmapType::wxBITMAP_TYPE_BMP );
			struct InPixel
			{
				uint8_t r, g, b, a;
			};
			struct OutPixel
			{
				uint8_t r, g, b;
			};
			auto srcIt = reinterpret_cast< InPixel const * >( src->getConstPtr() );
			auto end = reinterpret_cast< InPixel const * >( src->getConstPtr() + src->getSize() );
			auto dstIt = reinterpret_cast< InPixel * >( dst->getPtr() );
			auto diffIt = reinterpret_cast< OutPixel * >( result.GetData() );
			uint32_t diff{ 0u };

			while ( srcIt != end )
			{
				int16_t dr = int16_t( dstIt->r - srcIt->r );
				int16_t dg = int16_t( dstIt->g - srcIt->g );
				int16_t db = int16_t( dstIt->b - srcIt->b );
				int16_t da = int16_t( dstIt->a - srcIt->a );

				if ( dr || dg || db || da )
				{
					++diff;
				}

				*diffIt = { uint8_t( std::min( 255, ( dr * 4 + srcIt->r / 4 ) / 2 ) )
					, uint8_t( std::min( 255, ( dg * 4 + srcIt->g / 4 ) / 2 ) )
					, uint8_t( std::min( 255, ( db * 4 + srcIt->b / 4 ) / 2 ) ) };

				++srcIt;
				++dstIt;
				++diffIt;
			}

			return result.Mirror( false );
		}
	}

	class wxImagePanel
		: public wxPanel
	{
	public:
		explicit wxImagePanel( wxWindow * parent )
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

			if ( m_source.IsOk() )
			{
				SetMaxClientSize( m_source.GetSize() );
			}

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
		castor::StbImageLoader::registerLoader( m_loader );
		SetBackgroundColour( BORDER_COLOUR );
		SetForegroundColour( PANEL_FOREGROUND_COLOUR );

		auto refPanel = new wxPanel{ this };
		wxArrayString refChoices;
		refChoices.push_back( wxT( "Reference" ) );
		refChoices.push_back( wxT( "Difference" ) );
		auto refCombo = new wxComboBox{ refPanel, wxID_ANY, refChoices[0], wxDefaultPosition, wxDefaultSize, refChoices, wxCB_READONLY };
		refCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestPanel::onRefSelect ), nullptr, this );
		m_ref = new wxImagePanel{ refPanel };
		wxBoxSizer * refSizer{ new wxBoxSizer{ wxVERTICAL } };
		refSizer->Add( refCombo, wxSizerFlags{}.Border( wxUP | wxRIGHT | wxLEFT, 10 ) );
		refSizer->Add( m_ref, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		refSizer->SetSizeHints( refPanel );
		refPanel->SetSizer( refSizer );

		auto resPanel = new wxPanel{ this };
		wxArrayString resChoices;
		resChoices.push_back( wxT( "Test Result" ) );
		resChoices.push_back( wxT( "Difference" ) );
		auto resCombo = new wxComboBox{ resPanel, wxID_ANY, resChoices[0], wxDefaultPosition, wxDefaultSize, resChoices, wxCB_READONLY };
		resCombo->Connect( wxEVT_COMBOBOX, wxCommandEventHandler( TestPanel::onResSelect ), nullptr, this );
		m_result = new wxImagePanel{ resPanel };
		wxBoxSizer * resSizer{ new wxBoxSizer{ wxVERTICAL } };
		resSizer->Add( resCombo, wxSizerFlags{}.Border( wxUP | wxRIGHT | wxLEFT, 10 ) );
		resSizer->Add( m_result, wxSizerFlags{ 1 }.Expand().Border( wxALL, 10 ) );
		resSizer->SetSizeHints( resPanel );
		resPanel->SetSizer( resSizer );

		wxBoxSizer * sizer{ new wxBoxSizer{ wxHORIZONTAL } };
		sizer->Add( refPanel, wxSizerFlags{ 1 }.Expand().Border( wxALL, 0 ) );
		sizer->Add( resPanel, wxSizerFlags{ 1 }.Expand().Border( wxALL, 0 ) );
		sizer->SetSizeHints( this );
		SetSizer( sizer );
	}

	void TestPanel::refresh()
	{
		auto & test = *m_test;
		m_refImage = loadRefImage( m_config.test, *test );
		loadRef( m_currentRef );

		if ( test->status != TestStatus::eNotRun
			&& !isRunning( test->status ) )
		{
			m_resImage = loadResultImage( m_config.work, *test );
			m_refToResImage = compareImages( m_loader
				, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test )
				, m_config.work / getResultFolder( *test ) / getResultName( *test ) );
			m_resToRefImage = compareImages( m_loader
				, m_config.work / getResultFolder( *test ) / getResultName( *test )
				, m_config.test / getReferenceFolder( *test ) / getReferenceName( *test ) );
			loadRes( m_currentRes );
		}
	}

	void TestPanel::setTest( DatabaseTest & test )
	{
		m_test = &test;
		refresh();
	}

	void TestPanel::loadRef( int index )
	{
		switch ( index )
		{
		case 0:
			m_ref->setImage( m_refImage );
			break;

		case 1:
			m_ref->setImage( m_refToResImage );
			break;

		default:
			assert( false );
			break;
		}

		m_currentRef = index;
	}

	void TestPanel::loadRes( int index )
	{
		switch ( index )
		{
		case 0:
			m_result->setImage( m_resImage );
			break;

		case 1:
			m_result->setImage( m_resToRefImage );
			break;

		default:
			assert( false );
			break;
		}

		m_currentRes = index;
	}

	void TestPanel::onRefSelect( wxCommandEvent & evt )
	{
		loadRef( evt.GetSelection() );
	}

	void TestPanel::onResSelect( wxCommandEvent & evt )
	{
		loadRes( evt.GetSelection() );
	}
}
