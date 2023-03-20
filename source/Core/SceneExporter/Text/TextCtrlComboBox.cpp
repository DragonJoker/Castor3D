#include "TextCtrlComboBox.hpp"

#include "TextCtrlButton.hpp"
#include "TextCtrlControl.hpp"
#include "TextCtrlListBox.hpp"
#include "TextCtrlPanel.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< ComboBoxCtrl >::TextWriter( String const & tabs )
		: TextWriterT< ComboBoxCtrl >{ tabs }
	{
	}

	bool TextWriter< ComboBoxCtrl >::operator()( ComboBoxCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ComboBoxCtrl " ) << control.getName() << std::endl;
		bool result = writeSub< Control >( file, control );

		for ( auto item : control.getItems() )
		{
			if ( result )
			{
				result = writeName( file, cuT( "item" ), item );
			}
		}

		return result;
	}

	//*********************************************************************************************

	TextWriter< ComboBoxStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< ComboBoxStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< ComboBoxStyle >::operator()( ComboBoxStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ComboBoxStyle " ) << style.getName() << std::endl;
		auto result = writeSub< ControlStyle >( file, style );

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "expand_style" ) ) } )
			{
				result = TextWriter< ButtonStyle >{ tabs(), m_fontName }( style.getExpandStyle(), file );
			}
		}

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "elements_style" ) ) } )
			{
				result = TextWriter< ListBoxStyle >{ tabs(), m_fontName }( style.getElementsStyle(), file );
			}
		}

		return result;
	}

	//*********************************************************************************************
}
