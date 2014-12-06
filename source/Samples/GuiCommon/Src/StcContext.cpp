#include "StcContext.hpp"
#include "LanguageFileParser.hpp"
#include "LanguageInfo.hpp"

using namespace Castor3D;
using namespace Castor;

namespace GuiCommon
{
	StcContext::StcContext()
		:	m_bSyntaxEnable( true	)
		,	m_bFoldEnable( true	)
		,	m_bIndentEnable( true	)
		,	m_bReadOnlyInitial( false	)
		,	m_bOverTypeInitial( false	)
		,	m_bWrapModeInitial( false	)
		,	m_bDisplayEOLEnable( false	)
		,	m_bIndentGuideEnable( false	)
		,	m_bLineNumberEnable( true	)
		,	m_bLongLineOnEnable( true	)
		,	m_bWhiteSpaceEnable( false	)
	{
		m_arrayLanguages.push_back( std::make_shared< LanguageInfo >() );
	}

	StcContext::~StcContext()
	{
		Castor::clear_container( m_arrayLanguages );
	}

	void StcContext::ParseFile( wxString const & p_strFileName )
	{
		LanguageFileParser l_parser( this );
		l_parser.ParseFile( ( wxChar const * )p_strFileName.c_str() );
	}
}
