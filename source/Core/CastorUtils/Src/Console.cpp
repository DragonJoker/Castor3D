#if defined _WIN32
#	if defined( _MSC_VER )
#		pragma warning( push )
#		pragma warning( disable:4311 )
#		pragma warning( disable:4312 )
#		include <windows.h>
#	else
#		include <Windows.h>
#		include <Wincon.h>
#	endif
#	if defined( _MSC_VER )
#		pragma warning( pop )
#	endif
#else
#	include <cstdio>
#	include <cwchar>
#endif

#include "Console.hpp"

#include <iostream>

namespace Castor
{
#if defined( _MSC_VER )
	class MsvcConsoleInfo : public IConsoleInfo
	{
	public:
		MsvcConsoleInfo()
			:	m_uiOldCP( 0	)
			,	m_hScreenBuffer( INVALID_HANDLE_VALUE	)
			,	m_pOldInfos( NULL	)
			,	m_bAllocated( false	)
			,	m_bConsole( false	)
		{
			if ( ::AllocConsole() )
			{
				m_bAllocated = true;
				DoInitialiseConsole();
			}
			else
			{
				DWORD l_dwLastError = ::GetLastError();

				if ( l_dwLastError == ERROR_ACCESS_DENIED )
				{
					DoInitialiseConsole();
				}
				else
				{
					std::cout << "Failed to create to a new console with error " << l_dwLastError << std::endl;
				}
			}
		}

		virtual ~MsvcConsoleInfo()
		{
			if ( m_pOldInfos )
			{
				::SetCurrentConsoleFontEx( m_hScreenBuffer, FALSE, m_pOldInfos );
				delete m_pOldInfos;
			}

			if ( m_hScreenBuffer != INVALID_HANDLE_VALUE )
			{
				::CloseHandle( m_hScreenBuffer );
			}

			if ( m_bConsole )
			{
				::SetConsoleOutputCP( m_uiOldCP );
			}

			if ( m_bAllocated )
			{
				::FreeConsole();
			}
		}

		void BeginLog( eLOG_TYPE p_eLogType )
		{
			WORD l_wAttributes;

			switch ( p_eLogType )
			{
			case eLOG_TYPE_DEBUG	:
				l_wAttributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case eLOG_TYPE_WARNING	:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case eLOG_TYPE_ERROR	:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
				break;

			default:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			}

			::SetConsoleTextAttribute( m_hScreenBuffer, l_wAttributes );
		}

		void Print( String const & p_strToLog, bool p_bNewLine )
		{
			::OutputDebugString( p_strToLog.c_str() );
			DWORD l_dwWritten = 0;

			if ( p_bNewLine )
			{
				::OutputDebugString( cuT( "\n" ) );
				CONSOLE_SCREEN_BUFFER_INFO l_csbiInfo;

				if ( ::GetConsoleScreenBufferInfo( m_hScreenBuffer, &l_csbiInfo ) )
				{
					l_csbiInfo.dwCursorPosition.X = 0;
					::WriteConsole( m_hScreenBuffer, p_strToLog.c_str(), DWORD( p_strToLog.size() ), &l_dwWritten, NULL );
					SHORT l_sOffsetY = SHORT( 1 + l_dwWritten / l_csbiInfo.dwSize.X );

					if ( ( l_csbiInfo.dwSize.Y - l_sOffsetY ) <= l_csbiInfo.dwCursorPosition.Y )
					{
						// The cursor is on the last row
						SMALL_RECT l_srctScrollRect;
						CHAR_INFO l_chiFill;
						COORD l_coordDest;
						// The scroll rectangle is from second row to last displayed row
						l_srctScrollRect.Top = 1;
						l_srctScrollRect.Bottom = l_csbiInfo.dwSize.Y - 1;
						l_srctScrollRect.Left = 0;
						l_srctScrollRect.Right = l_csbiInfo.dwSize.X - 1;
						// The destination for the scroll rectangle is one row up.
						l_coordDest.X = 0;
						l_coordDest.Y = 0;
						// Set the fill character and attributes.
						l_chiFill.Attributes = 0;
						l_chiFill.Char.AsciiChar = char( ' ' );
						// Scroll
						::ScrollConsoleScreenBuffer( m_hScreenBuffer, &l_srctScrollRect, NULL, l_coordDest, &l_chiFill );
					}
					else
					{
						// The cursor isn't on the last row
						l_csbiInfo.dwCursorPosition.Y += l_sOffsetY;
					}

					::SetConsoleCursorPosition( m_hScreenBuffer, l_csbiInfo.dwCursorPosition );
				}
			}
			else
			{
				::WriteConsole( m_hScreenBuffer, p_strToLog.c_str(), DWORD( p_strToLog.size() ), &l_dwWritten, NULL );
			}
		}

	private:
		void DoInitialiseConsole()
		{
			m_hScreenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

			if ( m_hScreenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( m_hScreenBuffer ) )
			{
				m_pOldInfos = new CONSOLE_FONT_INFOEX;
				CONSOLE_FONT_INFOEX * l_pOldInfos = m_pOldInfos;
				l_pOldInfos->cbSize = sizeof( CONSOLE_FONT_INFOEX );

				if ( ::GetCurrentConsoleFontEx( m_hScreenBuffer, FALSE, l_pOldInfos ) )
				{
					CONSOLE_FONT_INFOEX l_infos = { 0 };
					l_infos.cbSize = sizeof( CONSOLE_FONT_INFOEX );
					l_infos.dwFontSize = l_pOldInfos->dwFontSize;
					l_infos.FontWeight = l_pOldInfos->FontWeight;
					l_infos.nFont = 6;
					l_infos.FontFamily = 54;
					_tcscpy_s( l_infos.FaceName, _countof( l_infos.FaceName ), _T( "Lucida Console" ) );

					if ( !::SetCurrentConsoleFontEx( m_hScreenBuffer, FALSE, &l_infos ) )
					{
						delete m_pOldInfos;
						m_pOldInfos = NULL;
					}
				}
				else
				{
					delete m_pOldInfos;
					m_pOldInfos = NULL;
				}

				COORD l_coord = { 160, 9999 };
				::SetConsoleScreenBufferSize( m_hScreenBuffer, l_coord );
			}

			m_uiOldCP = ::GetConsoleOutputCP();
			::EnumSystemCodePages( & DoCodePageProc, CP_INSTALLED );
			FILE * l_dump;
			freopen_s( & l_dump, "conout$", "w", stdout );
			freopen_s( & l_dump, "conout$", "w", stderr );
			m_bConsole = true;
		}
		static BOOL __stdcall DoCodePageProc( xchar * pszCodePageString )
		{
			BOOL l_bReturn = TRUE;
			String l_strCP( pszCodePageString );
			int l_iCP = str_utils::to_int( l_strCP );

			if ( l_iCP == CP_UTF8 )
			{
				::SetConsoleCP( l_iCP );
				::SetConsoleOutputCP( l_iCP );
				l_bReturn = FALSE;
			}

			return l_bReturn;
		}

	private:
		uint32_t m_uiOldCP;
		HANDLE m_hScreenBuffer;
		CONSOLE_FONT_INFOEX * m_pOldInfos;
		bool m_bAllocated;
		bool m_bConsole;
	};
#elif defined( _WIN32 )
	class MswConsoleInfo : public IConsoleInfo
	{
	public:
		MswConsoleInfo()
			:	m_uiOldCP( 0	)
			,	m_hScreenBuffer( INVALID_HANDLE_VALUE	)
			,	m_bAllocated( false	)
			,	m_bConsole( false	)
		{
			if ( ::AllocConsole() )
			{
				m_bAllocated = true;
				DoInitialiseConsole();
			}
			else
			{
				DWORD l_dwLastError = ::GetLastError();

				if ( l_dwLastError == ERROR_ACCESS_DENIED )
				{
					DoInitialiseConsole();
				}
				else
				{
					std::cout << "Failed to create to a new console with error " << l_dwLastError << std::endl;
				}
			}
		}

		virtual ~MswConsoleInfo()
		{
			if ( m_hScreenBuffer != INVALID_HANDLE_VALUE )
			{
				::CloseHandle( m_hScreenBuffer );
			}

			if ( m_bConsole )
			{
				::SetConsoleOutputCP( m_uiOldCP );
			}

			if ( m_bAllocated )
			{
				::FreeConsole();
			}
		}

		void BeginLog( eLOG_TYPE p_eLogType )
		{
			WORD l_wAttributes;

			switch ( p_eLogType )
			{
			case eLOG_TYPE_DEBUG	:
				l_wAttributes = FOREGROUND_BLUE | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case eLOG_TYPE_WARNING	:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY;
				break;

			case eLOG_TYPE_ERROR	:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_INTENSITY;
				break;

			default:
				l_wAttributes = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY;
				break;
			}

			::SetConsoleTextAttribute( m_hScreenBuffer, l_wAttributes );
		}

		void Print( String const & p_strToLog, bool p_bNewLine )
		{
			::OutputDebugString( p_strToLog.c_str() );
			DWORD l_dwWritten = 0;

			if ( p_bNewLine )
			{
				::OutputDebugString( cuT( "\n" ) );
				CONSOLE_SCREEN_BUFFER_INFO l_csbiInfo;

				if ( ::GetConsoleScreenBufferInfo( m_hScreenBuffer, &l_csbiInfo ) )
				{
					l_csbiInfo.dwCursorPosition.X = 0;
					::WriteConsole( m_hScreenBuffer, p_strToLog.c_str(), DWORD( p_strToLog.size() ), &l_dwWritten, NULL );
					SHORT l_sOffsetY = SHORT( 1 + l_dwWritten / l_csbiInfo.dwSize.X );

					if ( ( l_csbiInfo.dwSize.Y - l_sOffsetY ) <= l_csbiInfo.dwCursorPosition.Y )
					{
						// The cursor is on the last row
						SMALL_RECT l_srctScrollRect;
						CHAR_INFO l_chiFill;
						COORD l_coordDest;
						// The scroll rectangle is from second row to last displayed row
						l_srctScrollRect.Top = 1;
						l_srctScrollRect.Bottom = l_csbiInfo.dwSize.Y - 1;
						l_srctScrollRect.Left = 0;
						l_srctScrollRect.Right = l_csbiInfo.dwSize.X - 1;
						// The destination for the scroll rectangle is one row up.
						l_coordDest.X = 0;
						l_coordDest.Y = 0;
						// Set the fill character and attributes.
						l_chiFill.Attributes = 0;
						l_chiFill.Char.AsciiChar = char( ' ' );
						// Scroll
						::ScrollConsoleScreenBuffer( m_hScreenBuffer, &l_srctScrollRect, NULL, l_coordDest, &l_chiFill );
					}
					else
					{
						// The cursor isn't on the last row
						l_csbiInfo.dwCursorPosition.Y += l_sOffsetY;
					}

					::SetConsoleCursorPosition( m_hScreenBuffer, l_csbiInfo.dwCursorPosition );
				}
			}
			else
			{
				::WriteConsole( m_hScreenBuffer, p_strToLog.c_str(), DWORD( p_strToLog.size() ), &l_dwWritten, NULL );
			}
		}

	private:
		void DoInitialiseConsole()
		{
			m_hScreenBuffer = ::CreateConsoleScreenBuffer( GENERIC_WRITE | GENERIC_READ, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL );

			if ( m_hScreenBuffer != INVALID_HANDLE_VALUE && ::SetConsoleActiveScreenBuffer( m_hScreenBuffer ) )
			{
				COORD l_coord = { 160, 9999 };
				::SetConsoleScreenBufferSize( m_hScreenBuffer, l_coord );
			}

			m_uiOldCP = ::GetConsoleOutputCP();
			::EnumSystemCodePages( & DoCodePageProc, CP_INSTALLED );
			FILE * l_dump;
			l_dump = freopen( "conout$", "w", stdout );
			l_dump = freopen( "conout$", "w", stderr );
			m_bConsole = true;
		}
		static BOOL __stdcall DoCodePageProc( xchar * pszCodePageString )
		{
			BOOL l_bReturn = TRUE;
			String l_strCP( pszCodePageString );
			int l_iCP = str_utils::to_int( l_strCP );

			if ( l_iCP == CP_UTF8 )
			{
				::SetConsoleCP( l_iCP );
				::SetConsoleOutputCP( l_iCP );
				l_bReturn = FALSE;
			}

			return l_bReturn;
		}

	private:
		uint32_t m_uiOldCP;
		HANDLE m_hScreenBuffer;
		bool m_bAllocated;
		bool m_bConsole;
	};
#else
	class GenConsoleInfo : public IConsoleInfo
	{
	private:
	public:
		GenConsoleInfo()
		{
		}

		virtual ~GenConsoleInfo()
		{
		}

		void BeginLog( eLOG_TYPE p_eLogType )
		{
		}

		void Print( String const & p_strToLog, bool p_bNewLine )
		{
			if ( p_bNewLine )
			{
#if !defined( _UNICODE )
				printf( "%s\n", p_strToLog.c_str() );
#else
				wprintf( L"%ls\n", p_strToLog.c_str() );
#endif
			}
			else
			{
#if !defined( _UNICODE )
				printf( "%s", p_strToLog.c_str() );
#else
				wprintf( L"%ls", p_strToLog.c_str() );
#endif
			}
		}
	};
#endif

	DebugConsole::DebugConsole()
#if defined( _MSC_VER )
		:	m_pConsoleInfo( new MsvcConsoleInfo )
#elif defined( _WIN32 )
		:	m_pConsoleInfo( new MswConsoleInfo )
#else
		:	m_pConsoleInfo( new GenConsoleInfo )
#endif
	{
	}

	DebugConsole::~DebugConsole()
	{
		delete m_pConsoleInfo;
	}

	void DebugConsole::BeginLog( eLOG_TYPE p_eLogType )
	{
		m_pConsoleInfo->BeginLog( p_eLogType );
	}

	void DebugConsole::Print( String const & p_strToLog, bool p_bNewLine )
	{
		m_pConsoleInfo->Print( p_strToLog, p_bNewLine );
	}
}
