#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/glsl/ScriptCompiler.h"
#include "CastorShape/glsl/ScriptNode.h"
#include "CastorShape/glsl/ScriptBlock.h"
#include "CastorShape/glsl/ScriptFunctions.h"
#include "CastorShape/glsl/ScriptEngine.h"
#include "CastorShape/glsl/VariableType.h"

using namespace CastorShape;
using namespace Castor3D;

#define COMPILE_ERROR( p_desc )	m_compiler->_error();																		\
							Castor3D::Logger::LogMessage( CU_T( "Compiler Error : [") + m_compiler->_getScriptFileName()	\
										+ CU_T( " @ L# ") + ToString( m_lineNumBegin)										\
										+ CU_T( " ] -> ") + p_desc );

#define COMPILE_WARNING( p_desc )	m_compiler->_warning();											\
						Castor3D::Logger::LogMessage( CU_T( "Compiler Warning [ ")					\
									+ m_compiler->_getScriptFileName() + CU_T( " @ L# ")			\
									+ ToString( m_lineNumBegin )									\
									+ CU_T( " ] -> ") + p_desc );

#define COMPILE_ERROR_IN_BLOCK( p_desc , p_block ) m_compiler->_error();CASTOR_EXCEPTION( CU_T( "Compiler Error : [") + m_compiler->_getScriptFileName() + CU_T( " @ L# ") + ToString( p_block->m_lineNumBegin ) + CU_T( " ] -> ") + p_desc );
#define COMPILE_WARNING_IN_BLOCK( p_desc , p_block )  m_compiler->_warning();Logger::LogWarning( CU_T( "Compiler Warning [ ") + m_compiler->_getScriptFileName() + CU_T( " @ L# ") + ToString( p_block->m_lineNumBegin ) + CU_T( " ] -> ") + p_desc );

ScriptBlock :: ScriptBlock()
	:	m_parent				(NULL),
		m_compiledScript		(NULL),
		m_compiler				(NULL),
		m_type					(BT_INITIAL),
		m_subType				(BST_NONE),
		m_operatorLevel			(SO_NONE),
		m_variableType			(NULL),
		m_lineNumBegin			(0),
		m_depth					(0),
		m_operator_rightToLeft	(false)
{
}

ScriptBlock * ScriptBlock :: _initialise( ScriptCompiler * p_compiler, BlockType p_type,
										unsigned int p_lineNum, unsigned int p_depth,
										ScriptBlock * p_parent)
{
	m_compiler = p_compiler;
	m_type = p_type;
	m_depth = p_depth;
	m_parent = p_parent;
	m_lineNumBegin = p_lineNum;
	m_compiledScript = NULL;
	m_variableType = NULL;
	return this;
}

ScriptBlock :: ~ScriptBlock()
{
	Clear();
}

void ScriptBlock :: Clear() d_no_throw
{
	_times( static_cast <unsigned int> ( m_childs.size()))
	{
		m_compiler->_releaseBlock( m_childs[i]);
	}

	m_childs.clear();
	m_contents.clear();
	m_subType = BST_NONE;
}

bool ScriptBlock :: Parse()
{
	try
	{
		switch (m_type)
		{
			case BT_INITIAL:		return _parseInitial();
			case BT_SEPARATOR:		return _parseSeparator();
			case BT_STRING:			return _parseString();
			case BT_NUMERAL:		return _parseNumeral();
			case BT_OPERATOR:		return _parseOperator();
			case BT_PARENTHESIS:	return _parseParenthesis();
			case BT_BRACES:			return _parseBraces();
			case BT_BRACKETS:		return _parseBrackets();
			d_no_default;
		}
	}
	catch (...)
	{
		std::cout << "Caught Something (script block parse)! " << std::endl;
	}

	return false;
}

bool ScriptBlock :: _parseSeparator()
{
	VERBOSE_COMPILATOR( "_parseSeparator");

	if (m_compiler->_eof())
	{
		COMPILE_ERROR( "Unexpected end of file while parsing a separator");
		return false;
	}

	char l_currentChar = m_compiler->_getNextChar();

	if (l_currentChar == ',')
	{
		m_contents = ",";
		m_subType = BST_SEPARATOR_MINOR;
		return true;
	}

	if (l_currentChar == ';')
	{
		m_contents = ";";
		m_subType = BST_SEPARATOR_MAJOR;
		return true;
	}

	COMPILE_ERROR( "Unkown separator within separator block");
	return false;
}

bool ScriptBlock :: _preCompiledCurrent( ScriptBlockArray & p_childs, ScriptNodeArray & p_nodeArray, bool p_clear)
{

	if (p_childs.empty())
	{
		return true;
	}
	ScriptNode * l_node = m_compiler->_compileSentence( p_childs);

	if (l_node != NULL)
	{
		p_nodeArray.push_back( l_node);
	}
//	m_compiler->_leaveStructDecla();

	if (p_clear)
	{
		Clear();
	}
	else
	{
		_times( p_childs.size())
		{
			m_compiler->_releaseBlock( p_childs[i]);
		}
	}
	p_childs.clear();

	return true;
}

bool ScriptBlock :: _parseInitial()
{
	VERBOSE_COMPILATOR( "_parseInitial");
	ScriptNodeArray l_nodeArray;
	ScriptBlockArray l_ifArray;

	bool l_waitForBody = false;
	bool l_process = false;

	while ( ! m_compiler->_eof())
	{
		ScriptBlock * l_block = _delegate( m_compiler->_getNextChar());
		
		if (l_block == NULL) { continue; }

		if (l_block->m_type == BT_IF_ELSE)
		{
			_preCompiledCurrent( m_childs, l_nodeArray, true);
			if (l_block->m_contents == "if" && ! l_ifArray.empty())
			{
				_preCompiledCurrent( l_ifArray, l_nodeArray);
			}
			l_ifArray.push_back( l_block);
			continue;
		}
		else if ( ! l_ifArray.empty() && l_block->m_type == BT_BRACES)
		{
			l_ifArray.push_back( l_block);
			continue;
		}
		else if ( ! l_ifArray.empty() && l_block->m_type == BT_PARENTHESIS)
		{
			l_ifArray.push_back( l_block);
			continue;
		}
		else if ( ! l_ifArray.empty())
		{
			_preCompiledCurrent( l_ifArray, l_nodeArray);
		}

		if (l_block->m_type == BT_SEPARATOR && l_block->m_subType == BST_SEPARATOR_MAJOR)
		{
			m_compiler->_releaseBlock( l_block);
			_preCompiledCurrent( m_childs, l_nodeArray, true);
			continue;
		}

		if (l_block->m_type == BT_STRUCT)
		{
			m_compiler->_leaveStructDecla();
		}

		m_childs.push_back( l_block);

		if (l_block->m_type == BT_BRACES)
		{
			_preCompiledCurrent( m_childs, l_nodeArray, true);
			m_compiler->_leaveStructDecla();
			m_compiler->_leaveUserFunction();
		}
	}

	if ( ! m_childs.empty())
	{
		if (m_childs.back()->m_type != BT_BRACES)
		{
			COMPILE_ERROR_IN_BLOCK( "Semi column missing", m_childs.back());
		}

		_preCompiledCurrent( m_childs, l_nodeArray, true);
		m_compiler->_leaveUserFunction();
	}

	unsigned int imax = static_cast <unsigned int> ( l_nodeArray.size());

	if (imax == 1)
	{
		m_compiledScript = l_nodeArray[0];
	}
	else if (imax > 1)
	{
		m_compiledScript = m_compiler->CreateScriptNode();
		m_compiledScript->SetType( VariableTypeManager::Get( EMVT_CODE));

		m_compiledScript->SetFunction( Gen_MultiLineFunction );

		_times( imax)
		{
			m_compiledScript->AddChild( l_nodeArray[i] );
		}
	}
	else
	{
		//TODO : to be reactivated later v , once the code correctly returns script nodes
//		COMPILE_ERROR( "Script file is empty ?" );
	}

	return true;
}

bool ScriptBlock :: _parseString()
{
	VERBOSE_COMPILATOR( "_parseString");

	while ( ! m_compiler->_eof())
	{
		char l_currentChar = m_compiler->_getNextChar();
		
		if (_checkString( l_currentChar) || (l_currentChar >= '0' && l_currentChar <= '9'))
		{
			m_contents.push_back( l_currentChar);
		}
		else
		{
			m_compiler->_putBack( l_currentChar);
			_checkKeywords();
			return true;
		}
	}

	COMPILE_ERROR( "Unexpected end of file while parsing a string block");
	return false;
}

bool ScriptBlock::_parseNumeral()
{
	VERBOSE_COMPILATOR( "_parseNumeral");
	bool l_real = false;
	int l_intValue = 0;
	real l_realValue = 0.0f;
	real l_depth=0.1f;

//	m_compiledScript = m_compiler->CreateScriptNode();
	while( ! m_compiler->_eof() )
	{
		char l_currentChar = m_compiler->_getNextChar();
		if (_checkNumeral( l_currentChar))
		{
			if (l_real)
			{
				if (l_currentChar == '.')
				{
					COMPILE_WARNING( "Numerals with decimal points can only take one decimal point .");
				}
				else
				{
					l_realValue = l_realValue + l_depth * ( l_currentChar - '0');
					l_depth *= 0.1f;
				}
			}
			else
			{
				if (l_currentChar == '.')
				{
					l_real = true;
					l_realValue = l_intValue;
				}
				else
				{
					l_intValue = ( l_intValue * 10 ) + l_currentChar - '0';
				}
			}
			m_contents.push_back( l_currentChar);
		}
		else
		{
			if (l_real)
			{
				m_compiledScript = m_compiler->GetFlyweight( l_realValue);
			}
			else
			{
				m_compiledScript = m_compiler->GetFlyweight( l_intValue);
			}
			
			m_compiler->_putBack( l_currentChar);

			return true;
		}
	}
//	delete m_compiledScript;
	m_compiledScript = NULL;
	COMPILE_ERROR( "Unexpected end of file while parsing a numeral block" );
	return false;
}

bool ScriptBlock::_parseOperator()
{
	VERBOSE_COMPILATOR( "_parseOperator" );
	bool l_commented = false;
	bool l_cStyleComment = false;
	char l_previousChar = '\0';
	
	while( ! m_compiler->_eof() )
	{
		char l_currentChar = m_compiler->_getNextChar();
		if( ! l_commented )
		{
			if( _checkOperator( l_currentChar ) )
			{
				if( l_currentChar == '*' && l_previousChar=='/' )
				{
					l_commented = true;
					l_cStyleComment = false;
					m_contents.erase( m_contents.end() - 1 );
				}
				else if( l_currentChar == '/' && l_previousChar=='/' )
				{
					l_commented = true;
					l_cStyleComment = true;
					m_contents.erase( m_contents.end() - 1 );
				}
				else
				{
					m_contents.push_back( l_currentChar );
				}
			}
			else
			{
				m_compiler->_putBack( l_currentChar );

				if( m_contents.empty() )
				{
					return false;
				}
				else
				{
					_compileOperatorBlock();

					return true;
				}
			}
		}
		else
		{
			if( l_currentChar=='\n' )
			{
				m_compiler->_newLine();
				if( l_cStyleComment )
				{
					if( m_contents.empty() )
					{
						return false;
					}
					else
					{
						_compileOperatorBlock();
						return false;
					}
				}
			}
			else
			if( !l_cStyleComment && l_currentChar == '/' && l_previousChar=='*' )
			{
				l_commented = false;
			}
		}
		l_previousChar = l_currentChar;
	}
	COMPILE_ERROR( "Unexpected end of file while parsing an operator block" );
	return false;
}

bool ScriptBlock::_parseParenthesis()
{
	VERBOSE_COMPILATOR( "_parseParenthesis");
	while ( ! m_compiler->_eof())
	{
		char l_currentChar = m_compiler->_getNextChar();
		if ( l_currentChar != ')')
		{
			ScriptBlock * l_block = _delegate( l_currentChar);
			if (l_block != NULL)
			{
				m_childs.push_back( l_block);
			}
		}
		else
		{
			return true;
		}
	}
	COMPILE_ERROR( "Mismatched parenthesis () within file" );
	return false;
}

bool ScriptBlock::_parseBraces()
{
	VERBOSE_COMPILATOR( "_parseBraces");

	ScriptNodeArray l_nodeArray;
	ScriptBlockArray l_ifArray;

	unsigned int l_lineNum =  m_compiler->_getCurrentLine();

	while ( ! m_compiler->_eof())
	{
		char l_currentChar = m_compiler->_getNextChar();
		if (l_currentChar != '}')
		{
			ScriptBlock * l_block = _delegate( l_currentChar);
			
			if (l_block == NULL) { continue; }
			/*
			if (m_compiler->_isInStructDecla())
			{
				m_childs.push_back( l_block);
				continue;
			}
			*/

			if (l_block->m_type == BT_IF_ELSE)
			{
				_preCompiledCurrent( m_childs, l_nodeArray, true);
				if (l_block->m_contents == "if" && ! l_ifArray.empty())
				{
					_preCompiledCurrent( l_ifArray, l_nodeArray);
				}
				l_ifArray.push_back( l_block);
				continue;
			}
			else if ( ! l_ifArray.empty() && l_block->m_type == BT_BRACES)
			{
				l_ifArray.push_back( l_block);
				continue;
			}
			else if ( ! l_ifArray.empty() && l_block->m_type == BT_PARENTHESIS)
			{
				l_ifArray.push_back( l_block);
				continue;
			}
			else if ( ! l_ifArray.empty())
			{
				_preCompiledCurrent( l_ifArray, l_nodeArray);
			}

			if (l_block->m_type == BT_SEPARATOR)
			{
				m_compiler->_releaseBlock( l_block);
				_preCompiledCurrent( m_childs, l_nodeArray, true);
				continue;
			}

			/*
			if (l_block->m_type == BT_STRUCT)
			{
				m_compiler->_setInStructDecla( true);
			}
			*/

			m_childs.push_back( l_block);

			if (l_block->m_type == BT_BRACES)
			{
				_preCompiledCurrent( m_childs, l_nodeArray, true);
			}
		}
		else
		{
//			if ( ! m_compiler->_isInStructDecla())
			{
				if ( ! m_childs.empty())
				{
					_preCompiledCurrent( m_childs, l_nodeArray, true);
				}
				if ( ! l_ifArray.empty())
				{
					_preCompiledCurrent( l_ifArray, l_nodeArray);
				}

				unsigned int imax = static_cast<unsigned int>( l_nodeArray.size() );

				if( imax == 1 )
				{
					m_compiledScript = l_nodeArray[0];
				}
				else if( imax > 1 )
				{
					m_compiledScript = m_compiler->CreateScriptNode( l_lineNum);
					m_compiledScript->SetType( VariableTypeManager::Get( EMVT_CODE));
					m_compiledScript->SetFunction( Gen_MultiLineFunction);
					for( unsigned int i = 0 ; i < imax ; i ++ )
					{
						m_compiledScript->AddChild( l_nodeArray[i] );
					}
				}
			}
			return true;
		}
	}
	COMPILE_ERROR( "Mismatched braces {} within file" );
	return false;

}

bool ScriptBlock::_parseBrackets()
{
	VERBOSE_COMPILATOR( "_parseBrackets");
	while ( ! m_compiler->_eof())
	{
		char l_currentChar = m_compiler->_getNextChar();
		if( l_currentChar != ']' )
		{
			ScriptBlock * l_block = _delegate( l_currentChar);
			if (l_block != NULL) { m_childs.push_back( l_block); }
		}
		else
		{
			ScriptNode * l_node = m_compiler->_compileSentence( m_childs);
			if (l_node)
			{
				if (l_node->GetBaseType() == EMVT_INT)
				{
					m_compiledScript = l_node;
				}
				else
				{
					COMPILE_ERROR( "Contents of operator [] is not an int");
					return false;
				}
			}
			else
			{
				COMPILE_ERROR( "Operator [] without any contents ?");
				return false;
			}

			return true;
		}
	}
	COMPILE_ERROR( "Mismatched brackets [] within file" );
	return false;
}

ScriptBlock * ScriptBlock::_delegate( char p_currentChar )
{
	ScriptBlock * l_block = NULL;

	if (p_currentChar == '\n')
	{
		m_compiler->_newLine();
		return NULL;
	}

	if( _checkSeparator( p_currentChar))			{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_SEPARATOR , m_compiler->_getCurrentLine() , m_depth + 1 , this );}
	else	if( _checkString( p_currentChar))		{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_STRING , m_compiler->_getCurrentLine() , m_depth + 1 , this );}
	else	if( _checkNumeralStart( p_currentChar))	{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_NUMERAL , m_compiler->_getCurrentLine() , m_depth + 1 , this );}
	else	if( _checkOperator( p_currentChar))		{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_OPERATOR , m_compiler->_getCurrentLine() , m_depth + 1 , this );}
	else	if( _checkParenthesis( p_currentChar))	{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_PARENTHESIS , m_compiler->_getCurrentLine() , m_depth + 1 , this );}
	else	if( _checkAccolades( p_currentChar))	{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_BRACES , m_compiler->_getCurrentLine() , m_depth + 1 , this );}
	else	if( _checkSimpleQuote( p_currentChar))	{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_SIMPLEQUOTE , m_compiler->_getCurrentLine() , m_depth + 1 , this );}
	else	if( _checkDoubleQuote( p_currentChar))	{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_DOUBLEQUOTE , m_compiler->_getCurrentLine(), m_depth + 1 , this );}
	else	if( _checkArray( p_currentChar))		{	l_block = m_compiler->_getBlock()->_initialise( m_compiler , BT_BRACKETS , m_compiler->_getCurrentLine() , m_depth + 1 , this );}

	if (l_block == NULL)return NULL;

	if( l_block->m_type == BT_SEPARATOR || l_block->m_type == BT_STRING || l_block->m_type == BT_NUMERAL || l_block->m_type == BT_OPERATOR )
	{
		m_compiler->_putBack( p_currentChar );
	}

	//function declaration
	unsigned int l_typeList = m_compiler->_getTypeList( m_childs);
	if (l_block->m_type == BT_BRACES && (l_typeList & BT_PARENTHESIS) && (l_typeList & BT_VARIABLE_TYPE) && (l_typeList & BT_STRING))
	{
		UserFunction * l_function = m_compiler->_compileUserFunction( m_childs, false);
	}

	if (l_block->m_type == BT_BRACES && (l_typeList & BT_STRUCT))
	{
		m_compiler->_declareStruct( m_childs);
	}
	

	if ( ! l_block->Parse())
	{
		m_compiler->_releaseBlock( l_block);
		return NULL;
	}

	if (l_block->m_type == BT_BRACKETS)
	{
		ScriptBlock * l_block2 = m_compiler->_getBlock()->_initialise( m_compiler , BT_OPERATOR , m_compiler->_getCurrentLine() , m_depth + 1 , this );
		l_block2->m_contents = "[]";
		l_block2->m_operatorLevel = SO_ARRAY;
		l_block2->m_operator_rightToLeft = true;
		m_childs.push_back( l_block2);
	}
//	m_childs.push_back( l_block);

	return l_block;
}

ScriptNode * ScriptBlock::Compile()
{
	switch( m_type )
	{
		case BT_STRING:			return _compileString();

		case BT_PARENTHESIS:	return _compileInitial();
		case BT_BRACKETS:		return _compileInitial();

		default :				return m_compiledScript;
	}
	return NULL;
}

ScriptNode * ScriptBlock::_compileInitial()
{
	VERBOSE_COMPILATOR( "_compileInitial" );
	if( ! m_childs.empty() )
	{
		if( m_childs.size() == 1 )
		{
			m_compiledScript = m_childs[0]->Compile();
			return m_compiledScript;
		}
		else
		{
			return m_compiler->_compileSentence( m_childs );
		}
	}

	return NULL;
}

ScriptNode * ScriptBlock::_compileString()
{
	VERBOSE_COMPILATOR( "_compileString" );

	ScriptNode * l_node = m_compiler->_getUserVariable( m_contents);

	if (l_node)
	{
		return l_node;
	}

	l_node = m_compiler->GetProgramConstant( m_contents);

	if (l_node)
	{
		return l_node;
	}

	COMPILE_ERROR( "Variable not found : " + m_contents );

	return NULL;
}

void ScriptBlock::_compileOperatorBlock()
{
	VERBOSE_COMPILATOR( "_compileOperatorBlock" );

	if (m_contents.size() == 0 || m_contents.size() > 2)
	{
		COMPILE_WARNING( "Too few or too many letters for an operator block. Use some spaces or some parenthesis : ["+m_contents+"]" );
		return;
	}

	if (m_contents.size() == 1)
	{
		char l_contents = m_contents[0];
		m_operator_rightToLeft = false;
		switch (l_contents)
		{
			case '*': m_operatorLevel = SO_MULDIV;break;
			case '/': m_operatorLevel = SO_MULDIV;break;
			case '+': m_operatorLevel = SO_PLUSMINUS;break;
			case '-': m_operatorLevel = SO_PLUSMINUS;break;
			case '%': m_operatorLevel = SO_MODULO;break;
			case '!': m_operatorLevel = SO_NEGATE;break;
			case '=': m_operatorLevel = SO_EQUAL;m_operator_rightToLeft = true;break;
			case '<': m_operatorLevel = SO_INFERIORITY;break;
			case '>': m_operatorLevel = SO_SUPERIORITY;break;
			case '.': m_operatorLevel = SO_STRUCTMEMBER;m_operator_rightToLeft = true;break;
		}
	}
	else
	{
		char l_first = m_contents[0];
		char l_second = m_contents[1];
		if( l_second == '=' )
		{
			m_operator_rightToLeft = true;
			switch( l_first )
			{
				case '*': m_operatorLevel = SO_EQUAL;break;
				case '/': m_operatorLevel = SO_EQUAL;m_operator_rightToLeft = false;break;
				case '+': m_operatorLevel = SO_EQUAL;break;
				case ':': m_operatorLevel = SO_EQUAL;break;
				case '-': m_operatorLevel = SO_EQUAL;m_operator_rightToLeft = false;break;
				case '%': m_operatorLevel = SO_EQUAL;m_operator_rightToLeft = false;break;
				case '!': m_operatorLevel = SO_EQUALITY;m_operator_rightToLeft = false;break;
				case '=': m_operatorLevel = SO_EQUALITY;m_operator_rightToLeft = false;break;
				case '<': m_operatorLevel = SO_INFERIORITY;m_operator_rightToLeft = false;break;
				case '>': m_operatorLevel = SO_SUPERIORITY;m_operator_rightToLeft = false;break;
				default:
					COMPILE_WARNING( "Unknown operator detected : ["+m_contents+"]" );
					break;
			}
		}
		else if( l_first == '+' && l_second == '+')
		{
			m_operatorLevel = SO_INCREMENT;
		}
		else if( l_first == '-' && l_second == '-')
		{
			m_operatorLevel = SO_INCREMENT;
		}
		else if( l_first == '-' && l_second == '>')
		{
			m_operatorLevel = SO_ARROW;
			m_operator_rightToLeft = true;
		}
		else if( l_first == '&' && l_second == '&')
		{
			m_operatorLevel = SO_AND;
		}
		else if( l_first == '|' && l_second == '|')
		{
			m_operatorLevel = SO_OR;
		}
		else
		{
			COMPILE_WARNING( "Unknown operator detected : ["+m_contents+"]" );
		}
	}
}

bool ScriptBlock::_checkSeparator( char p_charToCheck )
{
	return (p_charToCheck == ',' || p_charToCheck == ';');
}

bool ScriptBlock::_checkString( char p_charToCheck )
{
	return ( ( p_charToCheck >='a' && p_charToCheck<='z') || ( p_charToCheck>='A' && p_charToCheck<='Z' ) || p_charToCheck=='_' );
}

bool ScriptBlock::_checkNumeral( char p_charToCheck )
{
	return( (p_charToCheck>='0' && p_charToCheck<='9') || p_charToCheck=='.');
}

bool ScriptBlock::_checkNumeralStart( char p_charToCheck )
{
	return( (p_charToCheck>='0' && p_charToCheck<='9'));
}

bool ScriptBlock::_checkOperator( char p_charToCheck )
{
	return (	p_charToCheck == '-' || p_charToCheck == '+' ||
				p_charToCheck == '*' || p_charToCheck == '/' ||
				p_charToCheck == '=' || p_charToCheck == '!' ||
				p_charToCheck == '<' || p_charToCheck == '>' ||
				p_charToCheck == '&' || p_charToCheck == '|' ||
				p_charToCheck == '.' || p_charToCheck == ':' ||
				p_charToCheck == '%'
	);
}

bool ScriptBlock::_checkParenthesis( char p_charToCheck )
{
	return p_charToCheck == '(';
}

bool ScriptBlock::_checkAccolades( char p_charToCheck )
{
	return p_charToCheck == '{';
}

bool ScriptBlock::_checkSimpleQuote( char p_charToCheck )
{
	return p_charToCheck == '\'';
}

bool ScriptBlock::_checkDoubleQuote( char p_charToCheck )
{
	return p_charToCheck == '"';
}

bool ScriptBlock::_checkArray( char p_charToCheck )
{
	return p_charToCheck == '[';
}

void ScriptBlock::_checkKeywords()
{
	if (m_contents.size() == 2)
	{
		if (m_contents == "if")						{m_type = BT_IF_ELSE;}
	}
	if (m_contents.size() == 3)
	{
		if (m_contents == "int")					{m_variableType = VariableTypeManager::Get( EMVT_INT);m_type = BT_VARIABLE_TYPE;}
	}
	else if (m_contents.size() == 4)
	{
			if (m_contents == "vec2")				{m_variableType = VariableTypeManager::Get( EMVT_VEC2F);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "vec3")				{m_variableType = VariableTypeManager::Get( EMVT_VEC3F);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "vec4")				{m_variableType = VariableTypeManager::Get( EMVT_VEC4F);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "mat2")				{m_variableType = VariableTypeManager::Get( EMVT_MAT2);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "mat3")				{m_variableType = VariableTypeManager::Get( EMVT_MAT3);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "mat4")				{m_variableType = VariableTypeManager::Get( EMVT_MAT4);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "bool")				{m_variableType = VariableTypeManager::Get( EMVT_BOOL);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "void")				{m_variableType = VariableTypeManager::Get( EMVT_NULL);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "else")				{m_type = BT_IF_ELSE;}
	}
	else if (m_contents.size() == 5)
	{
			if (m_contents == "real")				{m_variableType = VariableTypeManager::Get( EMVT_REAL);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "bvec2")				{m_variableType = VariableTypeManager::Get( EMVT_VEC2B);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "bvec3")				{m_variableType = VariableTypeManager::Get( EMVT_VEC3B);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "bvec4")				{m_variableType = VariableTypeManager::Get( EMVT_VEC4B);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "ivec2")				{m_variableType = VariableTypeManager::Get( EMVT_VEC2I);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "ivec3")				{m_variableType = VariableTypeManager::Get( EMVT_VEC3I);m_type = BT_VARIABLE_TYPE;}
		else if (m_contents == "ivec4")				{m_variableType = VariableTypeManager::Get( EMVT_VEC4I);m_type = BT_VARIABLE_TYPE;}
	}
	else if (m_contents.size() == 6)
	{
			if (m_contents == "struct")				{m_type = BT_STRUCT;}
		else if (m_contents == "elseif")			{m_type = BT_IF_ELSE;}
		else if (m_contents == "return")			{m_type = BT_RETURN;}
	}

	VariableType * l_type = m_compiler->FindType( m_contents);

	if (l_type != NULL)
	{
		m_variableType = l_type;
		m_type = BT_VARIABLE_TYPE;
	}

}
