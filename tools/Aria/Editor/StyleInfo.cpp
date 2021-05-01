#include "Aria/Editor/StyleInfo.hpp"

namespace aria
{
	StyleInfo::StyleInfo( wxColour const & foreground
		, wxColour const & background
		, int fontStyle
		, int letterCase )
		: foreground{ foreground }
		, background{ background }
		, fontStyle{ fontStyle }
		, letterCase{ letterCase }
	{
	}
}
