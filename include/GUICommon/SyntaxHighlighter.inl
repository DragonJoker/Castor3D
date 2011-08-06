//*************************************************************************************************

template <class TextEditClass>
wxSyntaxHighlighter<TextEditClass> :: wxSyntaxHighlighter(  wxWindow * p_pParent, wxWindowID p_id, const wxString & p_strValue, const wxPoint & p_ptPos, const wxSize & p_szSize, long p_lStyle)
	:	wxSyntaxHighlighterBase( this)
	,	TextEditClass( p_pParent, p_id, p_strValue, p_ptPos, p_szSize, p_lStyle | wxTE_RICH)
{
}

template <class TextEditClass>
wxSyntaxHighlighter<TextEditClass> :: ~wxSyntaxHighlighter()
{
}

template <class TextEditClass>
bool wxSyntaxHighlighter<TextEditClass> :: LoadFile( const wxString & filename, int fileType)
{
	bool l_bReturn = TextEditClass::LoadFile( filename, fileType);

	if (l_bReturn)
	{
		ParseText();
	}

	return l_bReturn;
}

BEGIN_EVENT_TABLE_TEMPLATE1( wxSyntaxHighlighter, TextEditClass, TextEditClass)
	EVT_CHAR(	wxSyntaxHighlighter::OnText)
END_EVENT_TABLE()

template <class TextEditClass>
void wxSyntaxHighlighter<TextEditClass> :: OnText( wxKeyEvent & p_event)
{
	switch (p_event.GetKeyCode())
	{
	case WXK_PAGEUP :
	case WXK_PAGEDOWN :
	case WXK_UP :
	case WXK_DOWN :
	case WXK_F1 :
	case WXK_F2 :
	case WXK_F3 :
	case WXK_F4 :
	case WXK_F5 :
	case WXK_F6 :
	case WXK_F7 :
	case WXK_F8 :
	case WXK_F9 :
	case WXK_F10 :
	case WXK_F11 :
	case WXK_F12 :
	case WXK_F13 :
	case WXK_F14 :
	case WXK_F15 :
	case WXK_F16 :
	case WXK_F17 :
	case WXK_F18 :
	case WXK_F19 :
	case WXK_F20 :
	case WXK_F21 :
	case WXK_F22 :
	case WXK_F23 :
	case WXK_F24 :
	case WXK_NUMLOCK :
	case WXK_SCROLL :
	case WXK_SHIFT:
	case WXK_ALT:
	case WXK_CONTROL:
	case WXK_MENU:
	case WXK_PAUSE:
	case WXK_CAPITAL:
	case WXK_END:
	case WXK_HOME:
	case WXK_LEFT:
	case WXK_RIGHT:
	case WXK_SELECT:
	case WXK_PRINT:
	case WXK_EXECUTE:
	case WXK_SNAPSHOT:
	case WXK_INSERT:
		break;

	default:
		ProcessChar( p_event.GetKeyCode(), TextEditClass::GetInsertionPoint());
		break;
	}

	p_event.Skip();
}

//*************************************************************************************************
