#include "TextStylesHolder.hpp"

#include "TextCtrlButton.hpp"
#include "TextCtrlComboBox.hpp"
#include "TextCtrlControl.hpp"
#include "TextCtrlEdit.hpp"
#include "TextCtrlExpandablePanel.hpp"
#include "TextCtrlListBox.hpp"
#include "TextCtrlPanel.hpp"
#include "TextCtrlSlider.hpp"
#include "TextCtrlStatic.hpp"

#include <Castor3D/Gui/Theme/Theme.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	namespace stlhdrwrtr
	{
		static bool filter( ControlStyle const & style
			, Scene const * scene )
		{
			if ( !style.hasScene() && !scene )
			{
				return true;
			}

			return style.hasScene()
				&& scene == &style.getScene();
		}

		template< typename StyleT, typename ... ParamsT >
		bool write( TextWriterBase & writer
			, Scene const * scene
			, StyleT const & style
			, castor::String const & name
			, castor::String const & sectionName
			, StringStream & file
			, ParamsT && ... params )
		{
			if ( !filter( style, scene ) )
			{
				return true;
			}

			if ( auto block{ writer.beginBlock( file, sectionName, name ) } )
			{
				TextWriter< StyleT > subWriter{ writer.tabs()
					, std::forward< ParamsT >( params )... };
				return subWriter( style, file );
			}

			return false;
		}

		template< typename StyleT, typename ... ParamsT >
		bool write( TextWriterBase & writer
			, Scene const * scene
			, std::map< castor::String, castor::UniquePtr< StyleT > > const & styles
			, castor::String const & sectionName
			, StringStream & file
			, ParamsT && ... params )
		{
			bool result = true;

			for ( auto & sub : styles )
			{
				result = result
					&& write( writer
						, scene
						, *sub.second
						, sub.first
						, sectionName
						, file
						, std::forward< ParamsT >( params )... );
			}

			return result;
		}
	}

	//*********************************************************************************************

	TextWriter< StylesHolder >::TextWriter( String const & tabs
		, castor3d::Scene const * scene
		, String const & fontName )
		: TextWriterT< StylesHolder >{ tabs }
		, m_scene{ scene }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< StylesHolder >::operator()( StylesHolder const & style
		, StringStream & file )
	{
		auto fontName = style.getDefaultFont()->getName();
		return writeNameOpt( file, "default_font", fontName, m_fontName )
			&& stlhdrwrtr::write( *this, m_scene, style.getButtonStyles(), "button_style", file, fontName )
			&& stlhdrwrtr::write( *this, m_scene, style.getComboBoxStyles(), "combobox_style", file, fontName )
			&& stlhdrwrtr::write( *this, m_scene, style.getEditStyles(), "edit_style", file, fontName )
			&& stlhdrwrtr::write( *this, m_scene, style.getExpandablePanelStyles(), "expandable_panel_style", file, fontName )
			&& stlhdrwrtr::write( *this, m_scene, style.getListBoxStyles(), "listbox_style", file, fontName )
			&& stlhdrwrtr::write( *this, m_scene, style.getPanelStyles(), "panel_style", file, fontName )
			&& stlhdrwrtr::write( *this, m_scene, style.getSliderStyles(), "slider_style", file, fontName )
			&& stlhdrwrtr::write( *this, m_scene, style.getStaticStyles(), "static_style", file, fontName );
	}

	//*********************************************************************************************
}
