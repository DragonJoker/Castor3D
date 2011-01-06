#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/glsl/ScriptEngine.h"
#include "CastorShape/glsl/ScriptCompiler.h"
#include "CastorShape/glsl/ScriptNode.h"
#include "CastorShape/glsl/ScriptBlock.h"
#include "CastorShape/glsl/VariableType.h"
#include "CastorShape/glsl/Structure.h"

#include "CastorShape/glsl/ScriptFunctions.h"
#include "CastorShape/glsl/ScriptFunctionOperators.h"

using namespace CastorShape;
using namespace Castor3D;

#define COMPILE_ERROR_IN_BLOCK( p_desc, p_block)												\
	_error();																					\
	CASTOR_EXCEPTION(	CU_T( "Compiler Error : [") + _getScriptFileName() + CU_T( " @ L# ")	\
						+ ToString( p_block->m_lineNumBegin)									\
						+ CU_T( " ] -> ") + p_desc );

#define COMPILE_WARNING_IN_BLOCK( p_desc, p_block)												\
	_warning();																					\
	Logger::LogWarning(	CU_T( "Compiler Warning [ ") + _getScriptFileName()						\
							+ CU_T( " @ L# ") + ToString( p_block->m_lineNumBegin)				\
							+ CU_T( " ] -> ") + p_desc );

ScriptCompiler :: ScriptCompiler( const Path & p_path)
	:	m_currentLine			(0),
		m_path					(p_path),
		m_currentFile			(NULL),
		m_currentUserFunction	(NULL),
		m_currentStructure		(NULL)
{
	m_nodePool.Allocate<ScriptCompiler>( 200, this);
	m_blockPool.Allocate( 16);

	_times( 4)
	{
		m_keyboardBinds[i] = new ScriptNode * [NUM_KEYS];
	}

	_times( NUM_KEYS)
	{
		m_keyboardBinds[0][i] = NULL;
		m_keyboardBinds[1][i] = NULL;
		m_keyboardBinds[2][i] = NULL;
		m_keyboardBinds[3][i] = NULL;
	}

	m_buffer = new char[1024];
	m_currentBufferIndex = 0;
	m_currentMaxIndex = 0;
}

ScriptCompiler :: ~ScriptCompiler()
{
	delete [] m_buffer;

	for (unsigned int j = 0 ; j < NUM_KEYS ; j ++ )
	{
		_times( 4)
		{
			if (m_keyboardBinds[i][j] != NULL)
			{
				m_keyboardBinds[i][j]->Delete();
			}
		}
	}

	_times( 4)
	{
		delete [] m_keyboardBinds[i];
	}

	map::cycle( m_constants, & ScriptNode::Delete);
	m_constants.clear();

	map::cycle( m_userVariables, & ScriptNode::Delete);
	m_userVariables.clear();

	map::deleteAll( m_userFunctions);

	map::deleteAll( m_functions);

	{
		ClassFunctionMap::iterator iter = m_classFunctions.begin();
		const ClassFunctionMap::iterator & iend = m_classFunctions.end();

		for ( ; iter != iend ; ++ iter)
		{
			map::deleteAll( iter->second);
		}

		m_classFunctions.clear();
	}

	multimap::deleteAll( m_operators);
	map::deleteAll( m_structures);

	map::cycle( m_charFlyweight, &ScriptNode::Delete);
	map::cycle( m_intFlyweight, &ScriptNode::Delete);
	map::cycle( m_stringFlyweight, &ScriptNode::Delete);
	map::cycle( m_realFlyweight, &ScriptNode::Delete);


}

ScriptNode * ScriptCompiler :: GetUsableFunctionNode( const String & p_functionName)
{
	UserFunction * l_function = GetUserFunction( p_functionName);

	if (l_function == NULL)
	{
//		l_function = _createUserFunction( p_functionName, EMVT_INT);
		return NULL;
	}

	if (l_function->m_finalNode != NULL)
	{
		return l_function->m_finalNode;
	}

	ScriptNode * l_functionNode = new ScriptNode( this, C3DEmptyString, 0);
	ScriptNode * l_returnValue = new ScriptNode( this, C3DEmptyString, 0);
	ScriptNode * l_numParams = new ScriptNode( this, C3DEmptyString, 0);

	l_functionNode->SetFunction( Gen_ExecuteFunction);

	l_numParams->SetType( VariableTypeManager::Get( EMVT_INT));

	l_functionNode->AddChild( l_returnValue);
	l_functionNode->AddChild( l_numParams);
	l_functionNode->AddChild( l_function->m_node);
	l_functionNode->Use();

	l_function->m_finalNode = l_functionNode;

	return l_functionNode;
}

void ScriptCompiler :: Initialise()
{
	_initialiseVariableMap();
	_initialiseFunctionMap();
	_initialiseOperatorMap();
}

void ScriptCompiler :: _log( const String & p_message)
{
	Logger::LogMessage( p_message);
}

ScriptNode * ScriptCompiler :: GetProgramConstant( const String & p_variableName)
{
	ScriptNode * l_node = map::findOrNull( m_constants, p_variableName);

	if (l_node != NULL)
	{
		return l_node;
	}

	if (m_currentUserFunction != NULL)
	{
		l_node = map::findOrNull( m_currentUserFunction->m_localVars, p_variableName);

		if (l_node != NULL)
		{
			return l_node;
		}
	}

	return map::findOrNull( m_userVariables, p_variableName);
}

char ScriptCompiler :: _getNextChar()
{
	if (m_currentFile != NULL)
	{
		if (m_currentBufferIndex < m_currentMaxIndex)
		{
			return m_buffer[m_currentBufferIndex ++];
		}

		_readFile();

		if (m_currentBufferIndex < m_currentMaxIndex)
		{
			return m_buffer[m_currentBufferIndex ++];
		}
		return '\0';
	}

	return ((* m_stringBuffer)[m_currentBufferIndex ++]);
}

bool ScriptCompiler :: _eof()
{
	if (m_currentFile != NULL)
	{
		return ( ! m_currentFile->IsOk() && m_currentBufferIndex >= m_currentMaxIndex);
	}
	return m_stringBuffer->size() == m_currentBufferIndex;
}

void ScriptCompiler :: _readFile()
{
	m_currentBufferIndex = 0;
	m_currentMaxIndex = m_currentFile->ReadArray<char>( m_buffer, 1024);
//	m_currentMaxIndex = ifstream::gcount();
}

void ScriptCompiler :: _putBack( char p_char)
{
	if (m_currentFile != NULL)
	{
		m_buffer[-- m_currentBufferIndex] = p_char;
	}
	else
	{
		m_currentBufferIndex --;
	}
}

ScriptNode * ScriptCompiler :: CompileScript( const String & p_script)
{
	ScriptBlock * l_initialBlock = new ScriptBlock;

	l_initialBlock->_initialise( this, BT_INITIAL, 0, 0, NULL);

	m_currentFile = NULL;
	m_currentBufferIndex = 0;
	m_currentMaxIndex = 0;
	m_stringBuffer = & p_script;

	try
	{
		l_initialBlock->Parse();
	}
	catch (Exception & p_except)
	{
		_log( "Critical failure : " + p_except.GetDescription());
	}

	m_currentFile = NULL;

	ScriptNode * l_scriptNode = l_initialBlock->GetScriptNode();

	delete l_initialBlock;

	return l_scriptNode;
}

ScriptNode * ScriptCompiler :: CompileScriptFile( const String & p_scriptFile)
{
	m_strFileName = p_scriptFile;

	String line;

	const String & l_fileDescName = p_scriptFile;

	m_currentLine = 1;
	m_numWarnings = 0;
	m_numErrors = 0;

	ScriptNode * l_scriptNode = NULL;
	
	String l_script;

	File *origStream;
//	DataStreamPtr stream;

	int numDeleted;

	_log( "Compiling from compiler 4.0 file : " + l_fileDescName);
	
	const Path & l_path = p_scriptFile;

	origStream = new File( l_path, File::eRead);

	if ( ! origStream->IsOk())
	{
		_log( "Compilation failed : file " + l_fileDescName + " does not exist (1)");
		delete origStream ;
	}
	else
	{
		if ( ! origStream->IsOk())
		{
			delete origStream;

			_log( "Compilation failed : file " + l_fileDescName + " does not exist(2)");
		}
		else
		{
			ScriptBlock * l_initialBlock = new ScriptBlock;
			l_initialBlock->_initialise( this, BT_INITIAL, 0, 0, NULL);
			m_currentFile = origStream;
			m_currentBufferIndex = 0;
			m_currentMaxIndex = 0;

			try
			{
				l_initialBlock->Parse();
			}
			catch (Exception & p_e)
			{
				_log( "Compilation failed : file " + l_fileDescName + " gave an exception" + p_e.GetDescription());
			}

			m_currentFile = NULL;

			l_scriptNode = l_initialBlock->GetScriptNode();

			delete l_initialBlock;

			numDeleted = 0;

			delete origStream ;

			if (m_numWarnings > 0 || m_numErrors > 0)
			{
				_log( "Compilation Result for file : "
									+ l_fileDescName + " : "
									+ ToString( m_numErrors) + " error(s) , "
									+ ToString( m_numWarnings) + " warning(s)");
			}
			else
			{
				_log( "Compilation success : " + l_fileDescName);
			}
		}
	}

	m_currentFile = NULL;

	return l_scriptNode;
}

ScriptNode * ScriptCompiler :: _createConstant( VariableBaseType p_type, const String & p_name)
{
//	genlib_CASTOR_ASSERT( ! map::has( m_constants, p_name));

	ScriptNode * l_node = CreateScriptNode(); //new ScriptNode( NULL, 0);

	l_node->SetType( VariableTypeManager::Get( p_type));
	l_node->Use();

	m_constants.insert( ScriptNodeMap::value_type( p_name, l_node));

	return l_node;
}

ScriptBlock * ScriptCompiler :: _getBlock()
{
	ScriptBlock * l_block = m_blockPool.Get();

	l_block->SetCompiler( this);

	return l_block;
}

void ScriptCompiler :: _releaseBlock( ScriptBlock * p_block)
{
	p_block->Clear();
	m_blockPool.Release( p_block);
}

void ScriptCompiler :: _leaveUserFunction()
{
//	m_withinUserFunction = false;
	m_currentUserFunction = NULL;
	/*
	map::cycle( m_userFuncVariables.back(), & ScriptNode::Delete);
	m_userFuncVariables.clear();
	*/
}

ScriptNode * ScriptCompiler :: _createUserVariable( const String & p_variableName, VariableType * p_variableType, bool p_functionParam)
{
	if (p_variableType == NULL)
	{
		return NULL;
	}

	ScriptNode * l_node = map::findOrNull( m_userVariables, p_variableName);

	if (l_node != NULL)
	{
		Logger::LogWarning( CU_T( "Variable ") + p_variableName + CU_T( " is being redeclared : .\
									Second declaration @ line ") + ToString( _getCurrentLine()));
		return l_node;
	}

	if (_isInStructDecla() && !_isInUserFunction())
	{
		m_currentStructure->AddMember( p_variableName, p_variableType);
		return NULL;
	}

	l_node = CreateScriptNode();

	l_node->SetType( p_variableType);
	l_node->Use();

	if (_isInUserFunction())
	{
//		genlib_CASTOR_ASSERT( m_currentUserFunction != NULL);
		m_currentUserFunction->m_localVars.insert( ScriptNodeMap::value_type( p_variableName, l_node));
	}
	else
	{
		m_userVariables.insert( ScriptNodeMap::value_type( p_variableName, l_node));
	}

//	std::cout << "Created user var : " << (_isInUserFunction() ? " in user func " : C3DEmptyString) << p_variableName << " of type : " << p_variableType->GetDesc() << std::endl;

	return l_node;
}

UserFunction *  ScriptCompiler :: _createUserFunction( const String & p_functionName, VariableType * p_functionReturnType)
{
	if (p_functionReturnType == NULL)
	{
		return NULL;
	}

	//TODOCOMPILER : check & error message if overwritten


//	map::deleteValue( m_userFunctions, p_functionName);

	UserFunction * l_function = new UserFunction( p_functionName);

	l_function->m_return = p_functionReturnType;

	l_function->m_returnNode = CreateScriptNode();
	l_function->m_returnNode->SetType( p_functionReturnType);
	l_function->m_returnNode->Use();

	l_function->m_node = CreateScriptNode();
//	l_function->m_node->m_userFunction = l_function;
	l_function->m_node->Use();

	if (_isInStructDecla())
	{
		m_currentStructure->AddFunction( l_function);
	}
	else
	{
		m_userFunctions.insert( UserFunctionMap::value_type( p_functionName, l_function));
	}

	return l_function;
}

void ScriptCompiler :: _creaFunc( const String & p_functionName, RawFunction * p_function, VariableBaseType p_returnValue, ...)
{
	map::deleteValue( m_functions, p_functionName);

	Function * l_scriptFunction = new Function( p_functionName);

	l_scriptFunction->m_function = p_function;
	l_scriptFunction->m_return = VariableTypeManager::Get( p_returnValue);

	va_list l_vl;
	va_start( l_vl, p_returnValue);

	VariableBaseType l_val = va_arg( l_vl, VariableBaseType);

	while (l_val != EMVT_NULL)
	{
		l_scriptFunction->m_params.push_back( VariableTypeManager::Get( l_val));
		l_val = va_arg( l_vl, VariableBaseType);
	}

	va_end( l_vl);

	m_functions.insert( FunctionMap::value_type( p_functionName, l_scriptFunction));
}

void ScriptCompiler :: _classFunc( const String & p_functionName, RawFunction * p_function, VariableBaseType p_returnValue, ...)
{
	va_list l_vl;
	va_start( l_vl, p_returnValue);

	VariableBaseType l_val;
	VariableBaseType l_param1;

	l_param1 = va_arg( l_vl, VariableBaseType);

	map::deleteValue( m_classFunctions[l_param1], p_functionName);

	Function * l_scriptFunction = new Function( p_functionName);

	l_scriptFunction->m_function = p_function;
	l_scriptFunction->m_return = VariableTypeManager::Get( p_returnValue);

	l_val = l_param1;

	while (l_val != EMVT_NULL)
	{
		l_scriptFunction->m_params.push_back( VariableTypeManager::Get( l_val));
		l_val = va_arg( l_vl, VariableBaseType);
	}

	va_end( l_vl);

	m_classFunctions[l_param1].insert( FunctionMap::value_type( p_functionName, l_scriptFunction));
}

void ScriptCompiler :: _createOperator( const String & p_name, RawFunction * p_func, VariableBaseType p_returnType,
									   VariableBaseType p_left, VariableBaseType p_right)
{
	OperatorFunction * l_scriptFunction = new OperatorFunction( p_name);

	l_scriptFunction->m_function = p_func;
	l_scriptFunction->m_return = VariableTypeManager::Get( p_returnType);

	if (p_left != EMVT_NULL && p_right != EMVT_NULL)
	{
		l_scriptFunction->m_unary = false;
		l_scriptFunction->m_params.push_back( VariableTypeManager::Get( p_left));
		l_scriptFunction->m_params.push_back( VariableTypeManager::Get( p_right));
	}
	else
	{
		l_scriptFunction->m_unary = true;

		if (p_left != EMVT_NULL)
		{
			l_scriptFunction->m_pre = false;
			l_scriptFunction->m_params.push_back( VariableTypeManager::Get( p_left));
		}
		else
		{
			l_scriptFunction->m_pre = true;
			l_scriptFunction->m_params.push_back( VariableTypeManager::Get( p_right));
		}
	}

	m_operators.insert( OperatorFunctionMultiMap::value_type( p_name, l_scriptFunction));
}

void ScriptCompiler :: _initialiseOperatorMap()
{
#	define _createLOperators( p_strOpe, p_rawFunc)																									\
	{																																				\
		_createOperator( p_strOpe,	p_rawFunc<real,real,int>, 						EMVT_REAL,			EMVT_REAL,			EMVT_INT		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2r, Point2r, real>,				EMVT_VEC2F,			EMVT_VEC2F,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2r, Point2r, int>,				EMVT_VEC2F,			EMVT_VEC2F,			EMVT_INT		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3r, Point3r, real>,				EMVT_VEC3F,			EMVT_VEC3F,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3r, Point3r, int>,				EMVT_VEC3F,			EMVT_VEC3F,			EMVT_INT		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4r, Point4r, real>,				EMVT_VEC4F,			EMVT_VEC4F,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4r, Point4r, int>,				EMVT_VEC4F,			EMVT_VEC4F,			EMVT_INT		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2i, Point2i, real>,				EMVT_VEC2I,			EMVT_VEC2I,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2i, Point2i, int>,				EMVT_VEC2I,			EMVT_VEC2I,			EMVT_INT		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3i, Point3i, real>,				EMVT_VEC3I,			EMVT_VEC3I,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3i, Point3i, int>,				EMVT_VEC3I,			EMVT_VEC3I,			EMVT_INT		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4i, Point4i, real>,				EMVT_VEC4I,			EMVT_VEC4I,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4i, Point4i, int>,				EMVT_VEC4I,			EMVT_VEC4I,			EMVT_INT		);		\
}

#	define _createLMOperators( p_strOpe, p_rawFunc)																								\
	{																																			\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x2r, Matrix2x2r, int>,			EMVT_MAT2,			EMVT_MAT2,			EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x2r, Matrix2x2r, real>,		EMVT_MAT2,			EMVT_MAT2,			EMVT_REAL		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x3r, Matrix2x3r, int>,			EMVT_MAT2x3,		EMVT_MAT2x3,		EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x3r, Matrix2x3r, real>,		EMVT_MAT2x3,		EMVT_MAT2x3,		EMVT_REAL		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x4r, Matrix2x4r, int>,			EMVT_MAT2x4,		EMVT_MAT2x4,		EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x4r, Matrix2x4r, real>,		EMVT_MAT2x4,		EMVT_MAT2x4,		EMVT_REAL		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x2r, Matrix3x2r, int>,			EMVT_MAT3x2,		EMVT_MAT3x2,		EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x2r, Matrix3x2r, real>,		EMVT_MAT3x2,		EMVT_MAT3x2,		EMVT_REAL		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x3r, Matrix3x3r, int>,			EMVT_MAT3,			EMVT_MAT3,			EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x3r, Matrix3x3r, real>,		EMVT_MAT3,			EMVT_MAT3,			EMVT_REAL		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x4r, Matrix3x4r, int>,			EMVT_MAT3x4,		EMVT_MAT3x4,		EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x4r, Matrix3x4r, real>,		EMVT_MAT3x4,		EMVT_MAT3x4,		EMVT_REAL		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x2r, Matrix4x2r, int>,			EMVT_MAT4x2,		EMVT_MAT4x2,		EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x2r, Matrix4x2r, real>,		EMVT_MAT4x2,		EMVT_MAT4x2,		EMVT_REAL		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x3r, Matrix4x3r, int>,			EMVT_MAT4x3,		EMVT_MAT4x3,		EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x3r, Matrix4x3r, real>,		EMVT_MAT4x3,		EMVT_MAT4x3,		EMVT_REAL		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x4r, Matrix4x4r, int>,			EMVT_MAT4,			EMVT_MAT4,			EMVT_INT		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x4r, Matrix4x4r, real>,		EMVT_MAT4,			EMVT_MAT4,			EMVT_REAL		);	\
	}

#	define _createULOperators( p_strOpe, p_rawFunc)																									\
	{																																				\
		_createOperator( p_strOpe,	p_rawFunc<real,int,real>, 						EMVT_REAL,			EMVT_INT,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2r, real, Point2r>,				EMVT_VEC2F,			EMVT_REAL,			EMVT_VEC2F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2r, int, Point2r>,				EMVT_VEC2F,			EMVT_INT,			EMVT_VEC2F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3r, real, Point3r>,				EMVT_VEC3F,			EMVT_REAL,			EMVT_VEC3F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3r, int, Point3r>,				EMVT_VEC3F,			EMVT_INT,			EMVT_VEC3F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4r, real, Point4r>,				EMVT_VEC4F,			EMVT_REAL,			EMVT_VEC4F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4r, int, Point4r>,				EMVT_VEC4F,			EMVT_INT,			EMVT_VEC4F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2i, real, Point2i>,				EMVT_VEC2I,			EMVT_REAL,			EMVT_VEC2I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2i, int, Point2i>,				EMVT_VEC2I,			EMVT_INT,			EMVT_VEC2I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3i, real, Point3i>,				EMVT_VEC3I,			EMVT_REAL,			EMVT_VEC3I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3i, int, Point3i>,				EMVT_VEC3I,			EMVT_INT,			EMVT_VEC3I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4i, real, Point4i>,				EMVT_VEC4I,			EMVT_REAL,			EMVT_VEC4I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4i, int, Point4i>,				EMVT_VEC4I,			EMVT_INT,			EMVT_VEC4I		);		\
	}

#	define _createULMOperators( p_strOpe, p_rawFunc)																							\
	{																																			\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x2r, int, Matrix2x2r>,			EMVT_MAT2,			EMVT_INT,			EMVT_MAT2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x2r, real, Matrix2x2r>,		EMVT_MAT2,			EMVT_REAL,			EMVT_MAT2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x3r, int, Matrix2x3r>,			EMVT_MAT2x3,		EMVT_INT,			EMVT_MAT2x3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x3r, real, Matrix2x3r>,		EMVT_MAT2x3,		EMVT_REAL,			EMVT_MAT2x3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x4r, int, Matrix2x4r>,			EMVT_MAT2x4,		EMVT_INT,			EMVT_MAT2x4		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x4r, real, Matrix2x4r>,		EMVT_MAT2x4,		EMVT_REAL,			EMVT_MAT2x4		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x2r, int, Matrix3x2r>,			EMVT_MAT3x2,		EMVT_INT,			EMVT_MAT3x2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x2r, real, Matrix3x2r>,		EMVT_MAT3x2,		EMVT_REAL,			EMVT_MAT3x2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x3r, int, Matrix3x3r>,			EMVT_MAT3,			EMVT_INT,			EMVT_MAT3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x3r, real, Matrix3x3r>,		EMVT_MAT3,			EMVT_REAL,			EMVT_MAT3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x4r, int, Matrix3x4r>,			EMVT_MAT3x4,		EMVT_INT,			EMVT_MAT3x4		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x4r, real, Matrix3x4r>,		EMVT_MAT3x4,		EMVT_REAL,			EMVT_MAT3x4		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x2r, int, Matrix4x2r>,			EMVT_MAT4x2,		EMVT_INT,			EMVT_MAT4x2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x2r, real, Matrix4x2r>,		EMVT_MAT4x2,		EMVT_REAL,			EMVT_MAT4x2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x3r, int, Matrix4x3r>,			EMVT_MAT4x3,		EMVT_INT,			EMVT_MAT4x3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x3r, real, Matrix4x3r>,		EMVT_MAT4x3,		EMVT_REAL,			EMVT_MAT4x3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x4r, int, Matrix4x4r>,			EMVT_MAT4,			EMVT_INT,			EMVT_MAT4		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x4r, real, Matrix4x4r>,		EMVT_MAT4,			EMVT_REAL,			EMVT_MAT4		);	\
	}

#	define _createGenericOperators( p_strOpe, p_rawFunc)																			\
	{																																\
		_createOperator( p_strOpe,	p_rawFunc<int>,					EMVT_INT,			EMVT_INT,			EMVT_INT		);		\
		_createOperator( p_strOpe,	p_rawFunc<bool>,				EMVT_BOOL,			EMVT_BOOL,			EMVT_BOOL		);		\
		_createOperator( p_strOpe,	p_rawFunc<real>, 				EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2r>,				EMVT_VEC2F,			EMVT_VEC2F,			EMVT_VEC2F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3r>,				EMVT_VEC3F,			EMVT_VEC3F,			EMVT_VEC3F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4r>,				EMVT_VEC4F,			EMVT_VEC4F,			EMVT_VEC4F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2i>,				EMVT_VEC2I,			EMVT_VEC2I,			EMVT_VEC2I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3i>,				EMVT_VEC3I,			EMVT_VEC3I,			EMVT_VEC3I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4i>,				EMVT_VEC4I,			EMVT_VEC4I,			EMVT_VEC4I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2b>,				EMVT_VEC2B,			EMVT_VEC2B,			EMVT_VEC2B		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3b>,				EMVT_VEC3B,			EMVT_VEC3B,			EMVT_VEC3B		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4b>,				EMVT_VEC4B,			EMVT_VEC4B,			EMVT_VEC4B		);		\
	}

#	define _createGenericMOperators( p_strOpe, p_rawFunc)																		\
	{																															\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x2r>,			EMVT_MAT2,			EMVT_MAT2,			EMVT_MAT2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x3r>,			EMVT_MAT2x3,		EMVT_MAT2x3,		EMVT_MAT2x3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x4r>,			EMVT_MAT2x4,		EMVT_MAT2x4,		EMVT_MAT2x4		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x2r>,			EMVT_MAT3x2,		EMVT_MAT3x2,		EMVT_MAT3x2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x3r>,			EMVT_MAT3,			EMVT_MAT3,			EMVT_MAT3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x4r>,			EMVT_MAT3x4,		EMVT_MAT3x4,		EMVT_MAT3x4		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x2r>,			EMVT_MAT4x2,		EMVT_MAT4x2,		EMVT_MAT4x2		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x3r>,			EMVT_MAT4x3,		EMVT_MAT4x3,		EMVT_MAT4x3		);	\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x4r>,			EMVT_MAT4,			EMVT_MAT4,			EMVT_MAT4		);	\
	}

#	define _createGenericCompare( p_strOpe, p_rawFunc)																				\
	{																																\
		_createOperator( p_strOpe,	p_rawFunc<int>,					EMVT_BOOL,			EMVT_INT,			EMVT_INT		);		\
		_createOperator( p_strOpe,	p_rawFunc<bool>,				EMVT_BOOL,			EMVT_BOOL,			EMVT_BOOL		);		\
		_createOperator( p_strOpe,	p_rawFunc<real>, 				EMVT_BOOL,			EMVT_REAL,			EMVT_REAL		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2r>,				EMVT_BOOL,			EMVT_VEC2F,			EMVT_VEC2F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3r>,				EMVT_BOOL,			EMVT_VEC3F,			EMVT_VEC3F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4r>,				EMVT_BOOL,			EMVT_VEC4F,			EMVT_VEC4F		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2i>,				EMVT_BOOL,			EMVT_VEC2I,			EMVT_VEC2I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3i>,				EMVT_BOOL,			EMVT_VEC3I,			EMVT_VEC3I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4i>,				EMVT_BOOL,			EMVT_VEC4I,			EMVT_VEC4I		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point2b>,				EMVT_BOOL,			EMVT_VEC2B,			EMVT_VEC2B		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point3b>,				EMVT_BOOL,			EMVT_VEC3B,			EMVT_VEC3B		);		\
		_createOperator( p_strOpe,	p_rawFunc<Point4b>,				EMVT_BOOL,			EMVT_VEC4B,			EMVT_VEC4B		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x2r>,			EMVT_BOOL,			EMVT_MAT2,			EMVT_MAT2		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x3r>,			EMVT_BOOL,			EMVT_MAT2x3,		EMVT_MAT2x3		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix2x4r>,			EMVT_BOOL,			EMVT_MAT2x4,		EMVT_MAT2x4		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x2r>,			EMVT_BOOL,			EMVT_MAT3x2,		EMVT_MAT3x2		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x3r>,			EMVT_BOOL,			EMVT_MAT3,			EMVT_MAT3		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix3x4r>,			EMVT_BOOL,			EMVT_MAT3x4,		EMVT_MAT3x4		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x2r>,			EMVT_BOOL,			EMVT_MAT4x2,		EMVT_MAT4x2		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x3r>,			EMVT_BOOL,			EMVT_MAT4x3,		EMVT_MAT4x3		);		\
		_createOperator( p_strOpe,	p_rawFunc<Matrix4x4r>,			EMVT_BOOL,			EMVT_MAT4,			EMVT_MAT4		);		\
	}

#	define _createOperators( p_strOpe, p_rawFunc)			\
	{														\
		_createGenericOperators( p_strOpe, p_rawFunc);		\
		_createLOperators( p_strOpe, p_rawFunc);			\
		_createULOperators( p_strOpe, p_rawFunc);			\
	}

#	define _createMOperators( p_strOpe, p_rawFunc)			\
	{														\
		_createGenericMOperators( p_strOpe, p_rawFunc);		\
		_createLMOperators( p_strOpe, p_rawFunc);			\
		_createULMOperators( p_strOpe, p_rawFunc);			\
	}

#	define _createLogicalOperators( p_strOpe, p_rawFunc)	\
	{														\
		_createGenericOperators( p_strOpe, p_rawFunc);		\
		_createLOperators( p_strOpe, p_rawFunc);			\
	}

#	define _createLogicalMOperators( p_strOpe, p_rawFunc)	\
	{														\
		_createGenericMOperators( p_strOpe, p_rawFunc);		\
		_createLMOperators( p_strOpe, p_rawFunc);			\
	}

	_createGenericOperators( "=", Ope_Set);
	_createGenericMOperators( "=", Ope_Set);
	_createOperator( "=",	Ope_Set<NodeValueBaseArray>,			EMVT_ARRAY,			EMVT_ARRAY,			EMVT_ARRAY		);
	_createOperator( "=",	Ope_SetNull<StructInstance *>,			EMVT_STRUCT,		EMVT_STRUCT,		EMVT_NULLVALUE	);
	_createOperator( "=",	Ope_Set<StructInstance *>,				EMVT_STRUCT,		EMVT_STRUCT,		EMVT_STRUCT		);

	_createOperator( "=",	Ope_SetNull<StructInstance *>,			EMVT_STRUCT,		EMVT_STRUCT,		EMVT_NULLVALUE	);

	_createGenericCompare( "==", Ope_Compare);
	_createOperator( "==",	Ope_Compare<NodeValueBaseArray>,		EMVT_BOOL,			EMVT_ARRAY,			EMVT_ARRAY		);
	_createOperator( "==",	Ope_CompareNull<StructInstance*>,		EMVT_BOOL,			EMVT_STRUCT,		EMVT_NULLVALUE	);

	_createGenericCompare( "!=", Ope_IsDiff);
	_createOperator( "!=",	Ope_IsDiff<NodeValueBaseArray>,			EMVT_BOOL,			EMVT_ARRAY,			EMVT_ARRAY		);
	_createOperator( "!=",	Ope_IsDiffNull<StructInstance*>,		EMVT_BOOL,			EMVT_STRUCT,		EMVT_NULLVALUE	);

	_createOperators(		"+",	Ope_Add);
	_createOperators(		"-",	Ope_Sub);
	_createMOperators(		"+",	Ope_Add);
	_createMOperators(		"-",	Ope_Sub);
	_createOperators(		"/",	Ope_Div);
	_createLMOperators(		"/",	Ope_Div);
	_createULMOperators(	"/",	Ope_Div);
	_createOperators(		"*",	Ope_Mul);
	_createLMOperators(		"*",	Ope_Mul);
	_createULMOperators(	"*",	Ope_Mul);

	_createLogicalOperators(	"+=",	Ope_AddEqual);
	_createLogicalMOperators(	"+=",	Ope_AddEqual);
	_createLogicalOperators(	"-=",	Ope_SubEqual);
	_createLogicalMOperators(	"-=",	Ope_SubEqual);
	_createLogicalOperators(	"/=",	Ope_DivEqual);
	_createLMOperators(			"/=",	Ope_DivEqual);
	_createLogicalOperators(	"*=",	Ope_MulEqual);
	_createLMOperators(			"*=",	Ope_MulEqual);

	_createOperator( "||",	Ope_Or,									EMVT_BOOL, 			EMVT_BOOL,			EMVT_BOOL		);
	_createOperator( "&&",	Ope_And,								EMVT_BOOL, 			EMVT_BOOL,			EMVT_BOOL		);
	_createOperator( "!",	Ope_Negate,								EMVT_BOOL, 			EMVT_NULL,			EMVT_BOOL		);

	_createOperator( "++",	Ope_Increment_Int,						EMVT_INT,			EMVT_INT,			EMVT_NULL		);
	_createOperator( "--",	Ope_Decrement_Int,						EMVT_INT,			EMVT_INT,			EMVT_NULL		);

	_createOperator( "++",	Ope_Increment_Int,						EMVT_INT,			EMVT_NULL,			EMVT_INT		);
	_createOperator( "--",	Ope_Decrement_Int,						EMVT_INT,			EMVT_NULL,			EMVT_INT		);

	_createOperator( "-",	Ope_NegateReal,							EMVT_REAL,			EMVT_NULL,			EMVT_REAL		);
	_createOperator( "-",	Ope_NegateInt,							EMVT_INT,			EMVT_NULL,			EMVT_INT		);

	_createOperator( "%",	Ope_ModuloInt,							EMVT_INT,			EMVT_INT,			EMVT_INT		);

	_createOperator( ">",	Ope_IsSuperior_Int,						EMVT_BOOL, 			EMVT_INT,			EMVT_INT		);
	_createOperator( ">",	Ope_IsSuperior_Real,					EMVT_BOOL, 			EMVT_REAL,			EMVT_REAL		);
	_createOperator( ">=",	Ope_IsSuperiorOrEqual_Int,				EMVT_BOOL, 			EMVT_INT,			EMVT_INT		);
	_createOperator( ">=",	Ope_IsSuperiorOrEqual_Real,				EMVT_BOOL, 			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "<",	Ope_IsInferior_Int,						EMVT_BOOL, 			EMVT_INT,			EMVT_INT		);
	_createOperator( "<",	Ope_IsInferior_Real,					EMVT_BOOL, 			EMVT_REAL,			EMVT_REAL		);
	_createOperator( "<=",	Ope_IsInferiorOrEqual_Int,				EMVT_BOOL, 			EMVT_INT,			EMVT_INT		);
	_createOperator( "<=",	Ope_IsInferiorOrEqual_Real,				EMVT_BOOL, 			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "[]",	Arr_Get,								EMVT_SUB1, 			EMVT_ARRAY,			EMVT_INT		);
	_createOperator( "[]",	Vec2f_OperatorArray,					EMVT_REAL, 			EMVT_VEC2F,			EMVT_INT		);
	_createOperator( "[]",	Vec3f_OperatorArray,					EMVT_REAL, 			EMVT_VEC3F,			EMVT_INT		);
	_createOperator( "[]",	Vec4f_OperatorArray,					EMVT_REAL, 			EMVT_VEC4F,			EMVT_INT		);
	_createOperator( "[]",	Vec2b_OperatorArray,					EMVT_REAL, 			EMVT_VEC2B,			EMVT_INT		);
	_createOperator( "[]",	Vec3b_OperatorArray,					EMVT_REAL, 			EMVT_VEC3B,			EMVT_INT		);
	_createOperator( "[]",	Vec4b_OperatorArray,					EMVT_REAL, 			EMVT_VEC4B,			EMVT_INT		);
	_createOperator( "[]",	Vec2i_OperatorArray,					EMVT_REAL, 			EMVT_VEC2I,			EMVT_INT		);
	_createOperator( "[]",	Vec3i_OperatorArray,					EMVT_REAL, 			EMVT_VEC3I,			EMVT_INT		);
	_createOperator( "[]",	Vec4i_OperatorArray,					EMVT_REAL, 			EMVT_VEC4I,			EMVT_INT		);

#	undef _createLOperators
#	undef _createLMOperators
#	undef _createULOperators
#	undef _createULMOperators
#	undef _createGenericOperators
#	undef _createGenericMOperators
#	undef _createLogicalOperators
#	undef _createLogicalMOperators
#	undef _createOperators
#	undef _createMOperators
#	undef _createGenericCompare
}

void ScriptCompiler :: _initialiseVariableMap()
{
	_constantGroup(		"General");
	_createConstant(	EMVT_REAL,				"GENERAL_ELAPSED_TIME"		);
	_createConstant(	EMVT_REAL,				"GENERAL_SCREEN_WIDTH"		);
	_createConstant(	EMVT_REAL,				"GENERAL_SCREEN_HEIGHT"		);

	_constantGroup(		"Math");
	_createConstant(	EMVT_REAL,				"PI"						)->set<real>( Angle::Pi);
	_createConstant(	EMVT_BOOL,				"true"						)->set( true);
	_createConstant(	EMVT_BOOL,				"false"						)->set( false);
	_createConstant(	EMVT_NULLVALUE,			"NULL"						);
}

void ScriptCompiler :: _initialiseFunctionMap()
{
	//Private function, for tests.
//		_creaFunc(	"Miaou"							,Gen_Miaou,						EMVT_NULL,		EMVT_NULL,		EMVT_NULL,									EMVT_NULL);

	_functionGroup( "General");
		_creaFunc(	"for",							Gen_ForLoop,					EMVT_NULL, 		EMVT_CODE,		EMVT_BOOL,		EMVT_CODE,		EMVT_CODE,	EMVT_NULL);
		_creaFunc(	"while",						Gen_WhileLoop,					EMVT_NULL, 		EMVT_BOOL,		EMVT_CODE,									EMVT_NULL);
		_creaFunc(	"break",						Gen_Break,						EMVT_NULL, 																	EMVT_NULL);
		_creaFunc(	"continue",						Gen_Continue,					EMVT_NULL, 																	EMVT_NULL);
//		_creaFunc(	"return",						Gen_Return,						EMVT_NULL, 		EMVT_CODE, 													EMVT_NULL);

	_functionGroup( "Math");
		_creaFunc(	"abs",							Mth_Abs,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"floor",						Mth_Floor,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"ceil",							Mth_Ceil,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"round",						Mth_Round,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"max",							Mth_Min,						EMVT_REAL,		EMVT_REAL,		EMVT_REAL,									EMVT_NULL);
		_creaFunc(	"min",							Mth_Max,						EMVT_REAL,		EMVT_REAL,		EMVT_REAL,									EMVT_NULL);
		_creaFunc(	"clamp",						Mth_Clamp,						EMVT_REAL,		EMVT_REAL,		EMVT_REAL,		EMVT_REAL,					EMVT_NULL);

		_creaFunc(	"sin",							Mth_Sin,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"cos",							Mth_Cos,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"tan",							Mth_Tan,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"asin",							Mth_ASin,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"acos",							Mth_ACos,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"atan",							Mth_ATan,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
	
		_creaFunc(	"sqrt",							Mth_Sqrt,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
}

Function * ScriptCompiler :: _getClassFunction( VariableType * p_class, const String & p_functionName)
{
	VariableBaseType l_base = p_class->GetBase();

	ClassFunctionMap::iterator & l_classMapIter = m_classFunctions.find( l_base);

	if (l_classMapIter != m_classFunctions.end())
	{
		return map::findOrNull( l_classMapIter->second, p_functionName);
	}

	return NULL;
}

unsigned int ScriptCompiler :: _getTypeList( ScriptBlockArray & p_childs) const
{
	unsigned int l_typeList = 0;
	unsigned int imax = static_cast <unsigned int>( p_childs.size());
	for (unsigned int i = 0 ; i < imax ; i ++)
	{
		l_typeList |= p_childs[i]->m_type;
	}

	return l_typeList;
}

ScriptNode * ScriptCompiler :: CreateScriptNode()
{
	ScriptNode * l_node = m_nodePool.Get();
	l_node->_reinit();
//	l_node->m_file = _getCurrentConfigFile();
	l_node->m_createdAtLine = _getCurrentLine();
	return l_node;
}

ScriptNode * ScriptCompiler :: CreateScriptNode( unsigned int p_lineNum)
{
	ScriptNode * l_node = m_nodePool.Get();
	l_node->_reinit();
//	l_node->m_file = _getCurrentConfigFile();
	l_node->m_createdAtLine = p_lineNum;
	return l_node;
}

VariableType * ScriptCompiler :: FindType( const String & p_name)
{
	VariableType * l_type = map::findOrNull( m_typedefs, p_name);

	if (l_type != NULL)
	{
		return l_type;
	}

	return NULL;
}

ScriptNode * ScriptCompiler :: GetFlyweight( int p_value)
{
	ScriptNode * l_node = map::findOrNull( m_intFlyweight, p_value); ;
	
	if (l_node != NULL)
	{
		return l_node;
	}
	
	l_node = CreateScriptNode();
	l_node->Use();

	l_node->SetType( VariableTypeManager::Get( EMVT_INT));
	l_node->set<int>( p_value);

	m_intFlyweight.insert( KeyedContainer<int, ScriptNode *>::Map::value_type( p_value, l_node));

	return l_node;
}

ScriptNode * ScriptCompiler :: GetFlyweight( real p_value)
{
	ScriptNode * l_node = map::findOrNull( m_realFlyweight, p_value); ;
	
	if (l_node != NULL)
	{
		return l_node;
	}
	
	l_node = CreateScriptNode();
	l_node->Use();

	l_node->SetType( VariableTypeManager::Get( EMVT_REAL));
	l_node->set<real>( p_value);

	m_realFlyweight.insert( KeyedContainer<real, ScriptNode *>::Map::value_type( p_value, l_node));

	return l_node;
}

void ScriptCompiler :: _releaseNode( ScriptNode * p_node)
{
	p_node->_delete();
	m_nodePool.Release( p_node);
}

ScriptNode * ScriptCompiler :: _getUserVariable( const String & p_variableName)
{
	if (m_currentUserFunction != NULL)
	{
		ScriptNode * l_node = map::findOrNull( m_currentUserFunction->m_localVars, p_variableName);
		if (l_node != NULL)
		{
			return l_node;
		}
		if (_isInStructDecla())
		{
//			std::cout << "get user var ? : " << p_variableName << "//?-->" << std::endl;
			size_t l_index = m_currentStructure->FindMember( p_variableName);
			if (l_index != String::npos)
			{
				ScriptNode * l_finalNode = CreateScriptNode();

				l_finalNode->SetRefType( m_currentStructure->GetType()->GetSubType( l_index));
				l_finalNode->SetFunction( Ope_StructGetMember);

				ScriptNode * l_node = map::findOrNull( m_currentUserFunction->m_localVars, String( "this"));
				l_finalNode->AddChild( l_node);

				l_finalNode->AddChild( GetFlyweight( static_cast <int> (l_index)));

				return l_finalNode;

			}
		}
	}
	return map::findOrNull( m_userVariables, p_variableName);
}

UserFunction :: ~UserFunction()
{
	map::cycle( m_localVars, &ScriptNode::Delete);
	m_localVars.clear();

	if (m_node != NULL)
	{
		m_node->Delete();
	}
	if (m_returnNode != NULL)
	{
		m_returnNode->Delete();
	}
}

void ScriptCompiler :: ReleaseScriptNode( ScriptNode * p_node)
{
	_releaseNode( p_node);
}


UserFunction * ScriptCompiler :: GetUserFunction( const String & p_functionName)
{
	if (_isInStructDecla())
	{
		UserFunction * l_function = m_currentStructure->GetFunction( p_functionName);
		if (l_function != NULL)
		{
			return l_function;
		}
	}
	return map::findOrNull( m_userFunctions, p_functionName);
}
