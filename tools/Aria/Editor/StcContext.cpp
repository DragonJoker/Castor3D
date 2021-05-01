#include "Aria/Editor/StcContext.hpp"

#include "Aria/Editor/LanguageFileParser.hpp"
#include "Aria/Editor/LanguageInfo.hpp"

namespace aria
{
	StcContext::StcContext()
	{
	}

	void StcContext::parseFile( wxString const & fileName )
	{
		LanguageFileParser parser( *this );
		parser.parseFile( castor::Path( fileName.mb_str( wxConvUTF8 ).data() ) );
	}
}
