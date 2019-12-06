#include "GuiCommon/StyleInfo.hpp"

namespace GuiCommon
{
	StyleInfo::StyleInfo( wxColour const & foreground
		, wxColour const & background
		, int fontStyle
		, int letterCase )
		: foreground( foreground )
		, background( background )
		, fontStyle( fontStyle )
		, letterCase( letterCase )
	{
	}
}
