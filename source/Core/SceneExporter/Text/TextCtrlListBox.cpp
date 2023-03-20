#include "TextCtrlListBox.hpp"

#include "TextCtrlControl.hpp"
#include "TextCtrlStatic.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< ListBoxCtrl >::TextWriter( String const & tabs )
		: TextWriterT< ListBoxCtrl >{ tabs }
	{
	}

	bool TextWriter< ListBoxCtrl >::operator()( ListBoxCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ListBoxCtrl " ) << control.getName() << std::endl;
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

	TextWriter< ListBoxStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< ListBoxStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< ListBoxStyle >::operator()( ListBoxStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing ListBoxStyle " ) << style.getName() << std::endl;
		auto result = writeSub< ControlStyle >( file, style );

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "item_style" ) ) } )
			{
				result = TextWriter< StaticStyle >{ tabs(), m_fontName }( style.getItemStyle(), file );
			}
		}

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "selected_item_style" ) ) } )
			{
				result = TextWriter< StaticStyle >{ tabs(), m_fontName }( style.getSelectedItemStyle(), file );
			}
		}

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "highlighted_item_style" ) ) } )
			{
				result = TextWriter< StaticStyle >{ tabs(), m_fontName }( style.getHighlightedItemStyle(), file );
			}
		}

		return result;
	}

	//*********************************************************************************************
}
