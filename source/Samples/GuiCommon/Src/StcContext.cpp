#include "StcContext.hpp"
#include "LanguageFileParser.hpp"
#include "LanguageInfo.hpp"

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	StcContext::StcContext()
		: m_bSyntaxEnable( true )
		, m_bFoldEnable( true )
		, m_bIndentEnable( true )
		, m_bReadOnlyInitial( false )
		, m_bOverTypeInitial( false )
		, m_bWrapModeInitial( false )
		, m_bDisplayEOLEnable( false )
		, m_bIndentGuideEnable( false )
		, m_bLineNumberEnable( true )
		, m_bLongLineOnEnable( true )
		, m_bWhiteSpaceEnable( false )
	{
		m_arrayLanguages.push_back( std::make_shared< LanguageInfo >() );
	}

	StcContext::~StcContext()
	{
		m_arrayLanguages.clear();
	}

	void StcContext::parseFile( wxString const & p_strFileName )
	{
		LanguageFileParser parser( this );
		parser.parseFile( make_Path( p_strFileName ) );
	}
}
