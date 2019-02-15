#include "GuiCommon/StcContext.hpp"
#include "GuiCommon/LanguageFileParser.hpp"
#include "GuiCommon/LanguageInfo.hpp"

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	StcContext::StcContext()
	{
		m_languages.push_back( std::make_shared< LanguageInfo >() );
	}

	StcContext::~StcContext()
	{
		m_languages.clear();
	}

	void StcContext::parseFile( wxString const & fileName )
	{
		LanguageFileParser parser( this );
		parser.parseFile( make_Path( fileName ) );
	}
}
