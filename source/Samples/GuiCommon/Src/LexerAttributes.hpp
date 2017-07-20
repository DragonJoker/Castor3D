/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GUICOMMON_LEXER_ATTRIBUTES_H___
#define ___GUICOMMON_LEXER_ATTRIBUTES_H___

#include "StcContext.hpp"

namespace GuiCommon
{
	template< eSTC_LEX Lexer, eSTC_TYPE Type >struct LexAttribute;

	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CONTAINER, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_NULL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PYTHON, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_HTML, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_XML, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PERL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SQL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_VB, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PROPERTIES, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ERRORLIST, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MAKEFILE, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BATCH, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_XCODE, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LATEX, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LUA, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_DIFF, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CONF, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PASCAL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_AVE, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ADA, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LISP, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_RUBY, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_EIFFEL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TCL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_NNCRONTAB, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BULLANT, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_VBSCRIPT, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BAAN, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MATLAB, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SCRIPTOL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ASM, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_FORTRAN, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_F77, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CSS, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_POV, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LOUT, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ESCRIPT, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PS, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_NSIS, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MMIXAL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CLW, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LOT, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_YAML, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TEX, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_METAPOST, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_POWERBASIC, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_FORTH, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ERLANG, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_OCTAVE, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MSSQL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_VERILOG, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_KIX, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_GUI4CLI, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SPECMAN, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_AU3, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_APDL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BASH, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ASN1, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_VHDL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CAML, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BLITZBASIC, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PUREBASIC, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_HASKELL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PHPSCRIPT, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TADS3, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_REBOL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SMALLTALK, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_FLAGSHIP, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CSOUND, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_FREEBASIC, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_INNOSETUP, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_OPAL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SPICE, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_D, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CMAKE, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_GAP, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PLM, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PROGRESS, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ABAQUS, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ASYMPTOTE, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_R, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MAGIK, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_POWERSHELL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MYSQL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PO, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TAL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_COBOL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TACL, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SORCUS, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_POWERPRO, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_NIMROD, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SML, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MARKDOWN, Type >
	{
		enum { Value = wxSTC_STYLE_DEFAULT	};
	};

	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_DEFAULT >
	{
		enum { Value = wxSTC_C_DEFAULT		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_WORD1 >
	{
		enum { Value = wxSTC_C_WORD			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_WORD2 >
	{
		enum { Value = wxSTC_C_WORD2			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_WORD3 >
	{
		enum { Value = wxSTC_C_GLOBALCLASS	};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_WORD4 >
	{
		enum { Value = wxSTC_C_WORD2			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_WORD5 >
	{
		enum { Value = wxSTC_C_WORD2			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_WORD6 >
	{
		enum { Value = wxSTC_C_WORD2			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_COMMENT >
	{
		enum { Value = wxSTC_C_COMMENT		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_COMMENT_DOC >
	{
		enum { Value = wxSTC_C_COMMENTDOC		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_COMMENT_LINE >
	{
		enum { Value = wxSTC_C_COMMENTLINE	};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_COMMENT_SPECIAL >
	{
		enum { Value = wxSTC_C_COMMENTDOC		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_CHARACTER >
	{
		enum { Value = wxSTC_C_CHARACTER		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_CHARACTER_EOL >
	{
		enum { Value = wxSTC_C_CHARACTER		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_STRING >
	{
		enum { Value = wxSTC_C_STRING			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_STRING_EOL >
	{
		enum { Value = wxSTC_C_STRINGEOL		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_DELIMITER >
	{
		enum { Value = wxSTC_C_IDENTIFIER		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_PUNCTUATION >
	{
		enum { Value = wxSTC_C_IDENTIFIER		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_OPERATOR >
	{
		enum { Value = wxSTC_C_OPERATOR		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_BRACE >
	{
		enum { Value = wxSTC_STYLE_BRACELIGHT	};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_COMMAND >
	{
		enum { Value = wxSTC_C_VERBATIM		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_IDENTIFIER >
	{
		enum { Value = wxSTC_C_IDENTIFIER		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_LABEL >
	{
		enum { Value = wxSTC_C_STRING			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_NUMBER >
	{
		enum { Value = wxSTC_C_NUMBER			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_PARAMETER >
	{
		enum { Value = wxSTC_C_DEFAULT		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_REGEX >
	{
		enum { Value = wxSTC_C_REGEX			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_UUID >
	{
		enum { Value = wxSTC_C_UUID			};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_VALUE >
	{
		enum { Value = wxSTC_C_DEFAULT		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_PREPROCESSOR >
	{
		enum { Value = wxSTC_C_PREPROCESSOR	};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_SCRIPT >
	{
		enum { Value = wxSTC_C_DEFAULT		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_ERROR >
	{
		enum { Value = wxSTC_C_DEFAULT		};
	};
	template<>struct LexAttribute< eSTC_LEX_CPP, eSTC_TYPE_UNDEFINED >
	{
		enum { Value = wxSTC_C_DEFAULT		};
	};

	template< eSTC_TYPE Type > struct LexerAttribute
	{
		int operator()( eSTC_LEX p_eLexer )
		{
			int iReturn = wxSTC_STYLE_DEFAULT;

			switch ( p_eLexer )
			{
			case eSTC_LEX_CONTAINER:
				iReturn = LexAttribute< eSTC_LEX_CONTAINER, Type >::Value;
				break;

			case eSTC_LEX_NULL:
				iReturn = LexAttribute< eSTC_LEX_NULL, Type >::Value;
				break;

			case eSTC_LEX_PYTHON:
				iReturn = LexAttribute< eSTC_LEX_PYTHON, Type >::Value;
				break;

			case eSTC_LEX_CPP:
				iReturn = LexAttribute< eSTC_LEX_CPP, Type >::Value;
				break;

			case eSTC_LEX_HTML:
				iReturn = LexAttribute< eSTC_LEX_HTML, Type >::Value;
				break;

			case eSTC_LEX_XML:
				iReturn = LexAttribute< eSTC_LEX_XML, Type >::Value;
				break;

			case eSTC_LEX_PERL:
				iReturn = LexAttribute< eSTC_LEX_PERL, Type >::Value;
				break;

			case eSTC_LEX_SQL:
				iReturn = LexAttribute< eSTC_LEX_SQL, Type >::Value;
				break;

			case eSTC_LEX_VB:
				iReturn = LexAttribute< eSTC_LEX_VB, Type >::Value;
				break;

			case eSTC_LEX_PROPERTIES:
				iReturn = LexAttribute< eSTC_LEX_PROPERTIES, Type >::Value;
				break;

			case eSTC_LEX_ERRORLIST:
				iReturn = LexAttribute< eSTC_LEX_ERRORLIST, Type >::Value;
				break;

			case eSTC_LEX_MAKEFILE:
				iReturn = LexAttribute< eSTC_LEX_MAKEFILE, Type >::Value;
				break;

			case eSTC_LEX_BATCH:
				iReturn = LexAttribute< eSTC_LEX_BATCH, Type >::Value;
				break;

			case eSTC_LEX_XCODE:
				iReturn = LexAttribute< eSTC_LEX_XCODE, Type >::Value;
				break;

			case eSTC_LEX_LATEX:
				iReturn = LexAttribute< eSTC_LEX_LATEX, Type >::Value;
				break;

			case eSTC_LEX_LUA:
				iReturn = LexAttribute< eSTC_LEX_LUA, Type >::Value;
				break;

			case eSTC_LEX_DIFF:
				iReturn = LexAttribute< eSTC_LEX_DIFF, Type >::Value;
				break;

			case eSTC_LEX_CONF:
				iReturn = LexAttribute< eSTC_LEX_CONF, Type >::Value;
				break;

			case eSTC_LEX_PASCAL:
				iReturn = LexAttribute< eSTC_LEX_PASCAL, Type >::Value;
				break;

			case eSTC_LEX_AVE:
				iReturn = LexAttribute< eSTC_LEX_AVE, Type >::Value;
				break;

			case eSTC_LEX_ADA:
				iReturn = LexAttribute< eSTC_LEX_ADA, Type >::Value;
				break;

			case eSTC_LEX_LISP:
				iReturn = LexAttribute< eSTC_LEX_LISP, Type >::Value;
				break;

			case eSTC_LEX_RUBY:
				iReturn = LexAttribute< eSTC_LEX_RUBY, Type >::Value;
				break;

			case eSTC_LEX_EIFFEL:
				iReturn = LexAttribute< eSTC_LEX_EIFFEL, Type >::Value;
				break;

			case eSTC_LEX_TCL:
				iReturn = LexAttribute< eSTC_LEX_TCL, Type >::Value;
				break;

			case eSTC_LEX_NNCRONTAB:
				iReturn = LexAttribute< eSTC_LEX_NNCRONTAB, Type >::Value;
				break;

			case eSTC_LEX_BULLANT:
				iReturn = LexAttribute< eSTC_LEX_BULLANT, Type >::Value;
				break;

			case eSTC_LEX_VBSCRIPT:
				iReturn = LexAttribute< eSTC_LEX_VBSCRIPT, Type >::Value;
				break;

			case eSTC_LEX_BAAN:
				iReturn = LexAttribute< eSTC_LEX_BAAN, Type >::Value;
				break;

			case eSTC_LEX_MATLAB:
				iReturn = LexAttribute< eSTC_LEX_MATLAB, Type >::Value;
				break;

			case eSTC_LEX_SCRIPTOL:
				iReturn = LexAttribute< eSTC_LEX_SCRIPTOL, Type >::Value;
				break;

			case eSTC_LEX_ASM:
				iReturn = LexAttribute< eSTC_LEX_ASM, Type >::Value;
				break;

			case eSTC_LEX_FORTRAN:
				iReturn = LexAttribute< eSTC_LEX_FORTRAN, Type >::Value;
				break;

			case eSTC_LEX_F77:
				iReturn = LexAttribute< eSTC_LEX_F77, Type >::Value;
				break;

			case eSTC_LEX_CSS:
				iReturn = LexAttribute< eSTC_LEX_CSS, Type >::Value;
				break;

			case eSTC_LEX_POV:
				iReturn = LexAttribute< eSTC_LEX_POV, Type >::Value;
				break;

			case eSTC_LEX_LOUT:
				iReturn = LexAttribute< eSTC_LEX_LOUT, Type >::Value;
				break;

			case eSTC_LEX_ESCRIPT:
				iReturn = LexAttribute< eSTC_LEX_ESCRIPT, Type >::Value;
				break;

			case eSTC_LEX_PS:
				iReturn = LexAttribute< eSTC_LEX_PS, Type >::Value;
				break;

			case eSTC_LEX_NSIS:
				iReturn = LexAttribute< eSTC_LEX_NSIS, Type >::Value;
				break;

			case eSTC_LEX_MMIXAL:
				iReturn = LexAttribute< eSTC_LEX_MMIXAL, Type >::Value;
				break;

			case eSTC_LEX_CLW:
				iReturn = LexAttribute< eSTC_LEX_CLW, Type >::Value;
				break;

			case eSTC_LEX_LOT:
				iReturn = LexAttribute< eSTC_LEX_LOT, Type >::Value;
				break;

			case eSTC_LEX_YAML:
				iReturn = LexAttribute< eSTC_LEX_YAML, Type >::Value;
				break;

			case eSTC_LEX_TEX:
				iReturn = LexAttribute< eSTC_LEX_TEX, Type >::Value;
				break;

			case eSTC_LEX_METAPOST:
				iReturn = LexAttribute< eSTC_LEX_METAPOST, Type >::Value;
				break;

			case eSTC_LEX_POWERBASIC:
				iReturn = LexAttribute< eSTC_LEX_POWERBASIC, Type >::Value;
				break;

			case eSTC_LEX_FORTH:
				iReturn = LexAttribute< eSTC_LEX_FORTH, Type >::Value;
				break;

			case eSTC_LEX_ERLANG:
				iReturn = LexAttribute< eSTC_LEX_ERLANG, Type >::Value;
				break;

			case eSTC_LEX_OCTAVE:
				iReturn = LexAttribute< eSTC_LEX_OCTAVE, Type >::Value;
				break;

			case eSTC_LEX_MSSQL:
				iReturn = LexAttribute< eSTC_LEX_MSSQL, Type >::Value;
				break;

			case eSTC_LEX_VERILOG:
				iReturn = LexAttribute< eSTC_LEX_VERILOG, Type >::Value;
				break;

			case eSTC_LEX_KIX:
				iReturn = LexAttribute< eSTC_LEX_KIX, Type >::Value;
				break;

			case eSTC_LEX_GUI4CLI:
				iReturn = LexAttribute< eSTC_LEX_GUI4CLI, Type >::Value;
				break;

			case eSTC_LEX_SPECMAN:
				iReturn = LexAttribute< eSTC_LEX_SPECMAN, Type >::Value;
				break;

			case eSTC_LEX_AU3:
				iReturn = LexAttribute< eSTC_LEX_AU3, Type >::Value;
				break;

			case eSTC_LEX_APDL:
				iReturn = LexAttribute< eSTC_LEX_APDL, Type >::Value;
				break;

			case eSTC_LEX_BASH:
				iReturn = LexAttribute< eSTC_LEX_BASH, Type >::Value;
				break;

			case eSTC_LEX_ASN1:
				iReturn = LexAttribute< eSTC_LEX_ASN1, Type >::Value;
				break;

			case eSTC_LEX_VHDL:
				iReturn = LexAttribute< eSTC_LEX_VHDL, Type >::Value;
				break;

			case eSTC_LEX_CAML:
				iReturn = LexAttribute< eSTC_LEX_CAML, Type >::Value;
				break;

			case eSTC_LEX_BLITZBASIC:
				iReturn = LexAttribute< eSTC_LEX_BLITZBASIC, Type >::Value;
				break;

			case eSTC_LEX_PUREBASIC:
				iReturn = LexAttribute< eSTC_LEX_PUREBASIC, Type >::Value;
				break;

			case eSTC_LEX_HASKELL:
				iReturn = LexAttribute< eSTC_LEX_HASKELL, Type >::Value;
				break;

			case eSTC_LEX_PHPSCRIPT:
				iReturn = LexAttribute< eSTC_LEX_PHPSCRIPT, Type >::Value;
				break;

			case eSTC_LEX_TADS3:
				iReturn = LexAttribute< eSTC_LEX_TADS3, Type >::Value;
				break;

			case eSTC_LEX_REBOL:
				iReturn = LexAttribute< eSTC_LEX_REBOL, Type >::Value;
				break;

			case eSTC_LEX_SMALLTALK:
				iReturn = LexAttribute< eSTC_LEX_SMALLTALK, Type >::Value;
				break;

			case eSTC_LEX_FLAGSHIP:
				iReturn = LexAttribute< eSTC_LEX_FLAGSHIP, Type >::Value;
				break;

			case eSTC_LEX_CSOUND:
				iReturn = LexAttribute< eSTC_LEX_CSOUND, Type >::Value;
				break;

			case eSTC_LEX_FREEBASIC:
				iReturn = LexAttribute< eSTC_LEX_FREEBASIC, Type >::Value;
				break;

			case eSTC_LEX_INNOSETUP:
				iReturn = LexAttribute< eSTC_LEX_INNOSETUP, Type >::Value;
				break;

			case eSTC_LEX_OPAL:
				iReturn = LexAttribute< eSTC_LEX_OPAL, Type >::Value;
				break;

			case eSTC_LEX_SPICE:
				iReturn = LexAttribute< eSTC_LEX_SPICE, Type >::Value;
				break;

			case eSTC_LEX_D:
				iReturn = LexAttribute< eSTC_LEX_D, Type >::Value;
				break;

			case eSTC_LEX_CMAKE:
				iReturn = LexAttribute< eSTC_LEX_CMAKE, Type >::Value;
				break;

			case eSTC_LEX_GAP:
				iReturn = LexAttribute< eSTC_LEX_GAP, Type >::Value;
				break;

			case eSTC_LEX_PLM:
				iReturn = LexAttribute< eSTC_LEX_PLM, Type >::Value;
				break;

			case eSTC_LEX_PROGRESS:
				iReturn = LexAttribute< eSTC_LEX_PROGRESS, Type >::Value;
				break;

			case eSTC_LEX_ABAQUS:
				iReturn = LexAttribute< eSTC_LEX_ABAQUS, Type >::Value;
				break;

			case eSTC_LEX_ASYMPTOTE:
				iReturn = LexAttribute< eSTC_LEX_ASYMPTOTE, Type >::Value;
				break;

			case eSTC_LEX_R:
				iReturn = LexAttribute< eSTC_LEX_R, Type >::Value;
				break;

			case eSTC_LEX_MAGIK:
				iReturn = LexAttribute< eSTC_LEX_MAGIK, Type >::Value;
				break;

			case eSTC_LEX_POWERSHELL:
				iReturn = LexAttribute< eSTC_LEX_POWERSHELL, Type >::Value;
				break;

			case eSTC_LEX_MYSQL:
				iReturn = LexAttribute< eSTC_LEX_MYSQL, Type >::Value;
				break;

			case eSTC_LEX_PO:
				iReturn = LexAttribute< eSTC_LEX_PO, Type >::Value;
				break;

			case eSTC_LEX_TAL:
				iReturn = LexAttribute< eSTC_LEX_TAL, Type >::Value;
				break;

			case eSTC_LEX_COBOL:
				iReturn = LexAttribute< eSTC_LEX_COBOL, Type >::Value;
				break;

			case eSTC_LEX_TACL:
				iReturn = LexAttribute< eSTC_LEX_TACL, Type >::Value;
				break;

			case eSTC_LEX_SORCUS:
				iReturn = LexAttribute< eSTC_LEX_SORCUS, Type >::Value;
				break;

			case eSTC_LEX_POWERPRO:
				iReturn = LexAttribute< eSTC_LEX_POWERPRO, Type >::Value;
				break;

			case eSTC_LEX_NIMROD:
				iReturn = LexAttribute< eSTC_LEX_NIMROD, Type >::Value;
				break;

			case eSTC_LEX_SML:
				iReturn = LexAttribute< eSTC_LEX_SML, Type >::Value;
				break;

			case eSTC_LEX_MARKDOWN:
				iReturn = LexAttribute< eSTC_LEX_MARKDOWN, Type >::Value;
				break;
			}

			return iReturn;
		}
	};
}

#endif
