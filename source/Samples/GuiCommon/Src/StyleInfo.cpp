#include "StyleInfo.hpp"

using namespace castor3d;
using namespace castor;

namespace GuiCommon
{
	StyleInfo::StyleInfo( wxString const & p_name, wxString const & p_strForeground, wxString const & p_strBackground, wxString const & p_strFontName, int p_iFontSize, int p_iFontStyle, int p_iLetterCase )
		:	m_name( p_name )
		,	m_strForeground( p_strForeground )
		,	m_strBackground( p_strBackground )
		,	m_strFontName( p_strFontName )
		,	m_iFontSize( p_iFontSize )
		,	m_iFontStyle( p_iFontStyle )
		,	m_iLetterCase( p_iLetterCase )
	{
	}

	StyleInfo::StyleInfo( StyleInfo const & p_infos )
		:	m_name( p_infos.m_name )
		,	m_strForeground( p_infos.m_strForeground )
		,	m_strBackground( p_infos.m_strBackground )
		,	m_strFontName( p_infos.m_strFontName )
		,	m_iFontSize( p_infos.m_iFontSize )
		,	m_iFontStyle( p_infos.m_iFontStyle )
		,	m_iLetterCase( p_infos.m_iLetterCase )
	{
	}

	StyleInfo & StyleInfo::operator =( StyleInfo const & p_infos )
	{
		m_name		= p_infos.m_name			;
		m_strForeground	= p_infos.m_strForeground	;
		m_strBackground	= p_infos.m_strBackground	;
		m_strFontName	= p_infos.m_strFontName		;
		m_iFontSize		= p_infos.m_iFontSize		;
		m_iFontStyle	= p_infos.m_iFontStyle		;
		m_iLetterCase	= p_infos.m_iLetterCase		;
		return * this;
	}

	StyleInfo::~StyleInfo()
	{
	}
}
