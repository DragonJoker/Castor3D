#include "TextCtrlSlider.hpp"

#include "TextCtrlControl.hpp"
#include "TextCtrlStatic.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace castor
{
	using namespace castor3d;

	//*********************************************************************************************

	TextWriter< SliderCtrl >::TextWriter( String const & tabs )
		: TextWriterT< SliderCtrl >{ tabs }
	{
	}

	bool TextWriter< SliderCtrl >::operator()( SliderCtrl const & control
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing SliderCtrl " ) << control.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, cuT( "slider" ), control.getName() ) } )
		{
			result = writeSub< Control >( file, control );
		}

		return result;
	}

	//*********************************************************************************************

	TextWriter< SliderStyle >::TextWriter( String const & tabs
		, String const & fontName )
		: TextWriterT< SliderStyle >{ tabs }
		, m_fontName{ fontName }
	{
	}

	bool TextWriter< SliderStyle >::operator()( SliderStyle const & style
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing SliderStyle " ) << style.getName() << std::endl;
		auto result = writeSub< ControlStyle >( file, style );

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "line_style" ) ) } )
			{
				TextWriter< StaticStyle > subWriter{ tabs(), m_fontName };
				result = subWriter( style.getLineStyle(), file );
			}
		}

		if ( result )
		{
			if ( auto block{ beginBlock( file, cuT( "tick_style" ) ) } )
			{
				TextWriter< StaticStyle > subWriter{ tabs(), m_fontName };
				result = subWriter( style.getTickStyle(), file );
			}
		}

		return result;
	}

	//*********************************************************************************************
}
