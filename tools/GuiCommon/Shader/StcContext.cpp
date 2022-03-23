#include "GuiCommon/Shader/StcContext.hpp"

#include "GuiCommon/Shader/LanguageFileParser.hpp"
#include "GuiCommon/Shader/LanguageInfo.hpp"

namespace GuiCommon
{
	StcContext::StcContext()
	{
		m_languages.push_back( std::make_shared< LanguageInfo >() );
	}

	void StcContext::parseFile( wxString const & fileName )
	{
		LanguageFileParser parser( this );
		parser.parseFile( make_Path( fileName ) );
	}
}
