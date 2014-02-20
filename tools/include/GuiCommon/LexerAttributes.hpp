/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___GC_LexerAttributes___
#define ___GC_LexerAttributes___

#include "StcContext.hpp"

namespace GuiCommon
{
	template< eSTC_LEX Lexer, eSTC_TYPE Type >struct LexAttribute;

	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CONTAINER	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_NULL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PYTHON		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_HTML		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_XML			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PERL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SQL			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_VB			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PROPERTIES	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ERRORLIST	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MAKEFILE	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BATCH		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_XCODE		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LATEX		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LUA			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_DIFF		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CONF		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PASCAL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_AVE			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ADA			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LISP		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_RUBY		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_EIFFEL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TCL			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_NNCRONTAB	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BULLANT		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_VBSCRIPT	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BAAN		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MATLAB		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SCRIPTOL	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ASM			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_FORTRAN		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_F77			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CSS			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_POV			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LOUT		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ESCRIPT		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PS			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_NSIS		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MMIXAL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CLW			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_LOT			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_YAML		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TEX			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_METAPOST	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_POWERBASIC	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_FORTH		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ERLANG		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_OCTAVE		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MSSQL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_VERILOG		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_KIX			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_GUI4CLI		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SPECMAN		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_AU3			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_APDL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BASH		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ASN1		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_VHDL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CAML		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_BLITZBASIC	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PUREBASIC	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_HASKELL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PHPSCRIPT	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TADS3		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_REBOL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SMALLTALK	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_FLAGSHIP	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CSOUND		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_FREEBASIC	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_INNOSETUP	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_OPAL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SPICE		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_D			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_CMAKE		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_GAP			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PLM			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PROGRESS	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ABAQUS		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_ASYMPTOTE	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_R			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MAGIK		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_POWERSHELL	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MYSQL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_PO			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TAL			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_COBOL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_TACL		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SORCUS		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_POWERPRO	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_NIMROD		, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_SML			, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };
	template< eSTC_TYPE Type >struct LexAttribute< eSTC_LEX_MARKDOWN	, Type	>	 { enum{ Value = wxSTC_STYLE_DEFAULT	}; };

	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_DEFAULT			>	 { enum{ Value = wxSTC_C_DEFAULT		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_WORD1			>	 { enum{ Value = wxSTC_C_WORD			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_WORD2			>	 { enum{ Value = wxSTC_C_WORD2			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_WORD3			>	 { enum{ Value = wxSTC_C_GLOBALCLASS	}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_WORD4			>	 { enum{ Value = wxSTC_C_WORD2			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_WORD5			>	 { enum{ Value = wxSTC_C_WORD2			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_WORD6			>	 { enum{ Value = wxSTC_C_WORD2			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_COMMENT			>	 { enum{ Value = wxSTC_C_COMMENT		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_COMMENT_DOC		>	 { enum{ Value = wxSTC_C_COMMENTDOC		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_COMMENT_LINE	>	 { enum{ Value = wxSTC_C_COMMENTLINE	}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_COMMENT_SPECIAL	>	 { enum{ Value = wxSTC_C_COMMENTDOC		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_CHARACTER		>	 { enum{ Value = wxSTC_C_CHARACTER		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_CHARACTER_EOL	>	 { enum{ Value = wxSTC_C_CHARACTER		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_STRING			>	 { enum{ Value = wxSTC_C_STRING			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_STRING_EOL		>	 { enum{ Value = wxSTC_C_STRINGEOL		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_DELIMITER		>	 { enum{ Value = wxSTC_C_IDENTIFIER		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_PUNCTUATION		>	 { enum{ Value = wxSTC_C_IDENTIFIER		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_OPERATOR		>	 { enum{ Value = wxSTC_C_OPERATOR		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_BRACE			>	 { enum{ Value = wxSTC_STYLE_BRACELIGHT	}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_COMMAND			>	 { enum{ Value = wxSTC_C_VERBATIM		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_IDENTIFIER		>	 { enum{ Value = wxSTC_C_IDENTIFIER		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_LABEL			>	 { enum{ Value = wxSTC_C_STRING			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_NUMBER			>	 { enum{ Value = wxSTC_C_NUMBER			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_PARAMETER		>	 { enum{ Value = wxSTC_C_DEFAULT		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_REGEX			>	 { enum{ Value = wxSTC_C_REGEX			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_UUID			>	 { enum{ Value = wxSTC_C_UUID			}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_VALUE			>	 { enum{ Value = wxSTC_C_DEFAULT		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_PREPROCESSOR	>	 { enum{ Value = wxSTC_C_PREPROCESSOR	}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_SCRIPT			>	 { enum{ Value = wxSTC_C_DEFAULT		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_ERROR			>	 { enum{ Value = wxSTC_C_DEFAULT		}; };
	template<>struct LexAttribute< eSTC_LEX_CPP	, eSTC_TYPE_UNDEFINED		>	 { enum{ Value = wxSTC_C_DEFAULT		}; };

#define SELECT_ATTRIBUTE( p_eStcType )	switch( p_eLexer )\
	{\
	case eSTC_LEX_CONTAINER		:l_iReturn = LexAttribute< eSTC_LEX_CONTAINER	, p_eStcType >::Value;break;\
	case eSTC_LEX_NULL			:l_iReturn = LexAttribute< eSTC_LEX_NULL		, p_eStcType >::Value;break;\
	case eSTC_LEX_PYTHON		:l_iReturn = LexAttribute< eSTC_LEX_PYTHON		, p_eStcType >::Value;break;\
	case eSTC_LEX_CPP			:l_iReturn = LexAttribute< eSTC_LEX_CPP			, p_eStcType >::Value;break;\
	case eSTC_LEX_HTML			:l_iReturn = LexAttribute< eSTC_LEX_HTML		, p_eStcType >::Value;break;\
	case eSTC_LEX_XML			:l_iReturn = LexAttribute< eSTC_LEX_XML			, p_eStcType >::Value;break;\
	case eSTC_LEX_PERL			:l_iReturn = LexAttribute< eSTC_LEX_PERL		, p_eStcType >::Value;break;\
	case eSTC_LEX_SQL			:l_iReturn = LexAttribute< eSTC_LEX_SQL			, p_eStcType >::Value;break;\
	case eSTC_LEX_VB			:l_iReturn = LexAttribute< eSTC_LEX_VB			, p_eStcType >::Value;break;\
	case eSTC_LEX_PROPERTIES	:l_iReturn = LexAttribute< eSTC_LEX_PROPERTIES	, p_eStcType >::Value;break;\
	case eSTC_LEX_ERRORLIST		:l_iReturn = LexAttribute< eSTC_LEX_ERRORLIST	, p_eStcType >::Value;break;\
	case eSTC_LEX_MAKEFILE		:l_iReturn = LexAttribute< eSTC_LEX_MAKEFILE	, p_eStcType >::Value;break;\
	case eSTC_LEX_BATCH			:l_iReturn = LexAttribute< eSTC_LEX_BATCH		, p_eStcType >::Value;break;\
	case eSTC_LEX_XCODE			:l_iReturn = LexAttribute< eSTC_LEX_XCODE		, p_eStcType >::Value;break;\
	case eSTC_LEX_LATEX			:l_iReturn = LexAttribute< eSTC_LEX_LATEX		, p_eStcType >::Value;break;\
	case eSTC_LEX_LUA			:l_iReturn = LexAttribute< eSTC_LEX_LUA			, p_eStcType >::Value;break;\
	case eSTC_LEX_DIFF			:l_iReturn = LexAttribute< eSTC_LEX_DIFF		, p_eStcType >::Value;break;\
	case eSTC_LEX_CONF			:l_iReturn = LexAttribute< eSTC_LEX_CONF		, p_eStcType >::Value;break;\
	case eSTC_LEX_PASCAL		:l_iReturn = LexAttribute< eSTC_LEX_PASCAL		, p_eStcType >::Value;break;\
	case eSTC_LEX_AVE			:l_iReturn = LexAttribute< eSTC_LEX_AVE			, p_eStcType >::Value;break;\
	case eSTC_LEX_ADA			:l_iReturn = LexAttribute< eSTC_LEX_ADA			, p_eStcType >::Value;break;\
	case eSTC_LEX_LISP			:l_iReturn = LexAttribute< eSTC_LEX_LISP		, p_eStcType >::Value;break;\
	case eSTC_LEX_RUBY			:l_iReturn = LexAttribute< eSTC_LEX_RUBY		, p_eStcType >::Value;break;\
	case eSTC_LEX_EIFFEL		:l_iReturn = LexAttribute< eSTC_LEX_EIFFEL		, p_eStcType >::Value;break;\
	case eSTC_LEX_TCL			:l_iReturn = LexAttribute< eSTC_LEX_TCL			, p_eStcType >::Value;break;\
	case eSTC_LEX_NNCRONTAB		:l_iReturn = LexAttribute< eSTC_LEX_NNCRONTAB	, p_eStcType >::Value;break;\
	case eSTC_LEX_BULLANT		:l_iReturn = LexAttribute< eSTC_LEX_BULLANT		, p_eStcType >::Value;break;\
	case eSTC_LEX_VBSCRIPT		:l_iReturn = LexAttribute< eSTC_LEX_VBSCRIPT	, p_eStcType >::Value;break;\
	case eSTC_LEX_BAAN			:l_iReturn = LexAttribute< eSTC_LEX_BAAN		, p_eStcType >::Value;break;\
	case eSTC_LEX_MATLAB		:l_iReturn = LexAttribute< eSTC_LEX_MATLAB		, p_eStcType >::Value;break;\
	case eSTC_LEX_SCRIPTOL		:l_iReturn = LexAttribute< eSTC_LEX_SCRIPTOL	, p_eStcType >::Value;break;\
	case eSTC_LEX_ASM			:l_iReturn = LexAttribute< eSTC_LEX_ASM			, p_eStcType >::Value;break;\
	case eSTC_LEX_FORTRAN		:l_iReturn = LexAttribute< eSTC_LEX_FORTRAN		, p_eStcType >::Value;break;\
	case eSTC_LEX_F77			:l_iReturn = LexAttribute< eSTC_LEX_F77			, p_eStcType >::Value;break;\
	case eSTC_LEX_CSS			:l_iReturn = LexAttribute< eSTC_LEX_CSS			, p_eStcType >::Value;break;\
	case eSTC_LEX_POV			:l_iReturn = LexAttribute< eSTC_LEX_POV			, p_eStcType >::Value;break;\
	case eSTC_LEX_LOUT			:l_iReturn = LexAttribute< eSTC_LEX_LOUT		, p_eStcType >::Value;break;\
	case eSTC_LEX_ESCRIPT		:l_iReturn = LexAttribute< eSTC_LEX_ESCRIPT		, p_eStcType >::Value;break;\
	case eSTC_LEX_PS			:l_iReturn = LexAttribute< eSTC_LEX_PS			, p_eStcType >::Value;break;\
	case eSTC_LEX_NSIS			:l_iReturn = LexAttribute< eSTC_LEX_NSIS		, p_eStcType >::Value;break;\
	case eSTC_LEX_MMIXAL		:l_iReturn = LexAttribute< eSTC_LEX_MMIXAL		, p_eStcType >::Value;break;\
	case eSTC_LEX_CLW			:l_iReturn = LexAttribute< eSTC_LEX_CLW			, p_eStcType >::Value;break;\
	case eSTC_LEX_LOT			:l_iReturn = LexAttribute< eSTC_LEX_LOT			, p_eStcType >::Value;break;\
	case eSTC_LEX_YAML			:l_iReturn = LexAttribute< eSTC_LEX_YAML		, p_eStcType >::Value;break;\
	case eSTC_LEX_TEX			:l_iReturn = LexAttribute< eSTC_LEX_TEX			, p_eStcType >::Value;break;\
	case eSTC_LEX_METAPOST		:l_iReturn = LexAttribute< eSTC_LEX_METAPOST	, p_eStcType >::Value;break;\
	case eSTC_LEX_POWERBASIC	:l_iReturn = LexAttribute< eSTC_LEX_POWERBASIC	, p_eStcType >::Value;break;\
	case eSTC_LEX_FORTH			:l_iReturn = LexAttribute< eSTC_LEX_FORTH		, p_eStcType >::Value;break;\
	case eSTC_LEX_ERLANG		:l_iReturn = LexAttribute< eSTC_LEX_ERLANG		, p_eStcType >::Value;break;\
	case eSTC_LEX_OCTAVE		:l_iReturn = LexAttribute< eSTC_LEX_OCTAVE		, p_eStcType >::Value;break;\
	case eSTC_LEX_MSSQL			:l_iReturn = LexAttribute< eSTC_LEX_MSSQL		, p_eStcType >::Value;break;\
	case eSTC_LEX_VERILOG		:l_iReturn = LexAttribute< eSTC_LEX_VERILOG		, p_eStcType >::Value;break;\
	case eSTC_LEX_KIX			:l_iReturn = LexAttribute< eSTC_LEX_KIX			, p_eStcType >::Value;break;\
	case eSTC_LEX_GUI4CLI		:l_iReturn = LexAttribute< eSTC_LEX_GUI4CLI		, p_eStcType >::Value;break;\
	case eSTC_LEX_SPECMAN		:l_iReturn = LexAttribute< eSTC_LEX_SPECMAN		, p_eStcType >::Value;break;\
	case eSTC_LEX_AU3			:l_iReturn = LexAttribute< eSTC_LEX_AU3			, p_eStcType >::Value;break;\
	case eSTC_LEX_APDL			:l_iReturn = LexAttribute< eSTC_LEX_APDL		, p_eStcType >::Value;break;\
	case eSTC_LEX_BASH			:l_iReturn = LexAttribute< eSTC_LEX_BASH		, p_eStcType >::Value;break;\
	case eSTC_LEX_ASN1			:l_iReturn = LexAttribute< eSTC_LEX_ASN1		, p_eStcType >::Value;break;\
	case eSTC_LEX_VHDL			:l_iReturn = LexAttribute< eSTC_LEX_VHDL		, p_eStcType >::Value;break;\
	case eSTC_LEX_CAML			:l_iReturn = LexAttribute< eSTC_LEX_CAML		, p_eStcType >::Value;break;\
	case eSTC_LEX_BLITZBASIC	:l_iReturn = LexAttribute< eSTC_LEX_BLITZBASIC	, p_eStcType >::Value;break;\
	case eSTC_LEX_PUREBASIC		:l_iReturn = LexAttribute< eSTC_LEX_PUREBASIC	, p_eStcType >::Value;break;\
	case eSTC_LEX_HASKELL		:l_iReturn = LexAttribute< eSTC_LEX_HASKELL		, p_eStcType >::Value;break;\
	case eSTC_LEX_PHPSCRIPT		:l_iReturn = LexAttribute< eSTC_LEX_PHPSCRIPT	, p_eStcType >::Value;break;\
	case eSTC_LEX_TADS3			:l_iReturn = LexAttribute< eSTC_LEX_TADS3		, p_eStcType >::Value;break;\
	case eSTC_LEX_REBOL			:l_iReturn = LexAttribute< eSTC_LEX_REBOL		, p_eStcType >::Value;break;\
	case eSTC_LEX_SMALLTALK		:l_iReturn = LexAttribute< eSTC_LEX_SMALLTALK	, p_eStcType >::Value;break;\
	case eSTC_LEX_FLAGSHIP		:l_iReturn = LexAttribute< eSTC_LEX_FLAGSHIP	, p_eStcType >::Value;break;\
	case eSTC_LEX_CSOUND		:l_iReturn = LexAttribute< eSTC_LEX_CSOUND		, p_eStcType >::Value;break;\
	case eSTC_LEX_FREEBASIC		:l_iReturn = LexAttribute< eSTC_LEX_FREEBASIC	, p_eStcType >::Value;break;\
	case eSTC_LEX_INNOSETUP		:l_iReturn = LexAttribute< eSTC_LEX_INNOSETUP	, p_eStcType >::Value;break;\
	case eSTC_LEX_OPAL			:l_iReturn = LexAttribute< eSTC_LEX_OPAL		, p_eStcType >::Value;break;\
	case eSTC_LEX_SPICE			:l_iReturn = LexAttribute< eSTC_LEX_SPICE		, p_eStcType >::Value;break;\
	case eSTC_LEX_D				:l_iReturn = LexAttribute< eSTC_LEX_D			, p_eStcType >::Value;break;\
	case eSTC_LEX_CMAKE			:l_iReturn = LexAttribute< eSTC_LEX_CMAKE		, p_eStcType >::Value;break;\
	case eSTC_LEX_GAP			:l_iReturn = LexAttribute< eSTC_LEX_GAP			, p_eStcType >::Value;break;\
	case eSTC_LEX_PLM			:l_iReturn = LexAttribute< eSTC_LEX_PLM			, p_eStcType >::Value;break;\
	case eSTC_LEX_PROGRESS		:l_iReturn = LexAttribute< eSTC_LEX_PROGRESS	, p_eStcType >::Value;break;\
	case eSTC_LEX_ABAQUS		:l_iReturn = LexAttribute< eSTC_LEX_ABAQUS		, p_eStcType >::Value;break;\
	case eSTC_LEX_ASYMPTOTE		:l_iReturn = LexAttribute< eSTC_LEX_ASYMPTOTE	, p_eStcType >::Value;break;\
	case eSTC_LEX_R				:l_iReturn = LexAttribute< eSTC_LEX_R			, p_eStcType >::Value;break;\
	case eSTC_LEX_MAGIK			:l_iReturn = LexAttribute< eSTC_LEX_MAGIK		, p_eStcType >::Value;break;\
	case eSTC_LEX_POWERSHELL	:l_iReturn = LexAttribute< eSTC_LEX_POWERSHELL	, p_eStcType >::Value;break;\
	case eSTC_LEX_MYSQL			:l_iReturn = LexAttribute< eSTC_LEX_MYSQL		, p_eStcType >::Value;break;\
	case eSTC_LEX_PO			:l_iReturn = LexAttribute< eSTC_LEX_PO			, p_eStcType >::Value;break;\
	case eSTC_LEX_TAL			:l_iReturn = LexAttribute< eSTC_LEX_TAL			, p_eStcType >::Value;break;\
	case eSTC_LEX_COBOL			:l_iReturn = LexAttribute< eSTC_LEX_COBOL		, p_eStcType >::Value;break;\
	case eSTC_LEX_TACL			:l_iReturn = LexAttribute< eSTC_LEX_TACL		, p_eStcType >::Value;break;\
	case eSTC_LEX_SORCUS		:l_iReturn = LexAttribute< eSTC_LEX_SORCUS		, p_eStcType >::Value;break;\
	case eSTC_LEX_POWERPRO		:l_iReturn = LexAttribute< eSTC_LEX_POWERPRO	, p_eStcType >::Value;break;\
	case eSTC_LEX_NIMROD		:l_iReturn = LexAttribute< eSTC_LEX_NIMROD		, p_eStcType >::Value;break;\
	case eSTC_LEX_SML			:l_iReturn = LexAttribute< eSTC_LEX_SML			, p_eStcType >::Value;break;\
	case eSTC_LEX_MARKDOWN		:l_iReturn = LexAttribute< eSTC_LEX_MARKDOWN	, p_eStcType >::Value;break;\
	}\
	return l_iReturn;

	template< eSTC_TYPE Type >struct LexerAttribute
	{
		int operator()( eSTC_LEX p_eLexer )
		{
			int l_iReturn = wxSTC_STYLE_DEFAULT;
			SELECT_ATTRIBUTE( Type );
			return l_iReturn;
		}
	};
}

#endif