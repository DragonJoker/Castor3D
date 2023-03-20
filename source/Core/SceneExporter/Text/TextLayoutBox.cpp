#include "TextLayoutBox.hpp"

#include <Castor3D/Gui/Controls/CtrlLayoutControl.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	TextWriter< LayoutBox >::TextWriter( String const & tabs )
		: TextWriterT< LayoutBox >{ tabs }
	{
	}

	bool TextWriter< LayoutBox >::operator()( LayoutBox const & layout
		, StringStream & file )
	{
		auto & layoutName = layout.getContainer().getName();
		log::info << tabs() << cuT( "Writing LayoutBox " ) << layoutName << std::endl;
		bool result = writeOpt( file, "horizontal", layout.isHorizontal(), false );

		for ( auto & item : layout )
		{
			if ( result )
			{
				if ( item.isSpacer() )
				{
					if ( item.spacer()->isDynamic() )
					{
						result = write( file, "layout_dynspace" );
					}
					else
					{
						result = write( file, "layout_staspace", item.spacer()->getSize() );
					}
				}
				else
				{
					auto name = item.control()->getName();

					if ( name.find( layoutName + "/" ) == 0 )
					{
						name = name.substr( layoutName.size() + 1u );
					}

					if ( auto block{ beginBlock( file, cuT( "layout_ctrl" ), name ) } )
					{
						result = writeOpt( file, cuT( "horizontal_align" ), getName( item.flags().hAlign() ), getName( HAlign::eLeft ) )
							&& writeOpt( file, cuT( "vertical_align" ), getName( item.flags().vAlign() ), getName( VAlign::eTop ) )
							&& writeOpt( file, "stretch", item.flags().expand(), false )
							&& writeOpt( file, "reserve_if_hidden", item.flags().reserveSpaceIfHidden(), false )
							&& writeOpt( file, "padding", item.flags().padding(), castor::Point4ui{} );
					}
				}
			}
		}

		return result;
	}
}
