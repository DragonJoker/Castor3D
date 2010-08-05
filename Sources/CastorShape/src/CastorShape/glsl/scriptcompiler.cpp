#include "PrecompiledHeaders.h"

#include "glsl/ScriptEngine.h"
#include "glsl/ScriptCompiler.h"
#include "glsl/ScriptNode.h"
#include "glsl/ScriptBlock.h"
#include "glsl/VariableType.h"
#include "glsl/Structure.h"

#include "glsl/ScriptFunctions.h"
#include "glsl/ScriptFunctionOperators.h"

using namespace CastorShape;
using namespace Castor3D;

GENLIB_INIT_SINGLETON( ScriptCompiler);

#define COMPILE_ERROR_IN_BLOCK( p_desc, p_block)												\
	_error();																					\
	GENLIB_EXCEPTION(	"Compiler Error : [" + _getScriptFileName() + " @ L# "					\
						+ ToString( p_block->m_lineNumBegin)					\
						+ " ] -> " + p_desc )

#define COMPILE_WARNING_IN_BLOCK( p_desc, p_block)												\
	_warning();																					\
	_log(	"Compiler Warning [ " + _getScriptFileName()										\
							+ " @ L# " + ToString( p_block->m_lineNumBegin)	\
							+ " ] -> " + p_desc );												\

ScriptCompiler :: ScriptCompiler( const Path & p_path)
	:	m_currentLine			(0),
		m_path					(p_path),
		m_currentFileStream		(NULL),
		m_currentUserFunction	(NULL),
		m_currentStructure		(NULL)
{
	GENLIB_SET_SINGLETON();

	m_nodePool.Allocate( 200);
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

ScriptNode * ScriptCompiler :: GetUsableFunctionNode( const String & p_functionName)const
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

	ScriptNode * l_functionNode = new ScriptNode( "", 0);
	ScriptNode * l_returnValue = new ScriptNode( "", 0);
	ScriptNode * l_numParams = new ScriptNode( "", 0);

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
	Log::LogMessage( p_message);
}

ScriptNode * ScriptCompiler :: GetProgramConstant( const String & p_variableName)const
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
	if (m_currentFileStream != NULL)
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
	if (m_currentFileStream != NULL)
	{
		return ( ! m_currentFileStream->IsOk() && m_currentBufferIndex >= m_currentMaxIndex);
	}
	return m_stringBuffer->size() == m_currentBufferIndex;
}

void ScriptCompiler :: _readFile()
{
	m_currentBufferIndex = 0;
	m_currentMaxIndex = m_currentFileStream->ReadArray<char>( m_buffer, 1024);
//	m_currentMaxIndex = ifstream::gcount();
}

void ScriptCompiler :: _putBack( char p_char)
{
	if (m_currentFileStream != NULL)
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

	m_currentFileStream = NULL;
	m_currentBufferIndex = 0;
	m_currentMaxIndex = 0;
	m_stringBuffer = & p_script;

	try
	{
		l_initialBlock->Parse();
	}
	catch (GenException & p_except)
	{
		_log( "Critical failure : " + p_except.GetDescription());
	}

	m_currentFileStream = NULL;

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

	FileStream *origStream;
//	DataStreamPtr stream;

	int numDeleted;

	_log( "Compiling from compiler 4.0 file : " + l_fileDescName);
	
	const Path & l_path = p_scriptFile;

	origStream = new FileStream( l_path, FileBase::eRead);

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
			m_currentFileStream = origStream;
			m_currentBufferIndex = 0;
			m_currentMaxIndex = 0;

			try
			{
				l_initialBlock->Parse();
			}
			catch (GenException & p_e)
			{
				_log( "Compilation failed : file " + l_fileDescName + " gave an exception" + p_e.GetDescription());
			}

			m_currentFileStream = NULL;

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

	m_currentFileStream = NULL;

	return l_scriptNode;
}

ScriptNode * ScriptCompiler :: _createConstant( VariableBaseType p_type, const String & p_name)
{
//	genlib_assert( ! map::has( m_constants, p_name));

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
		Log::LogMessage( "Warning : variable " + p_variableName + " is being redeclared : .\
									Second declaration @ line " + ToString( _getCurrentLine()));
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
//		genlib_assert( m_currentUserFunction != NULL);
		m_currentUserFunction->m_localVars.insert( ScriptNodeMap::value_type( p_variableName, l_node));
	}
	else
	{
		m_userVariables.insert( ScriptNodeMap::value_type( p_variableName, l_node));
	}

//	std::cout << "Created user var : " << (_isInUserFunction() ? " in user func " : "") << p_variableName << " of type : " << p_variableType->GetDesc() << std::endl;

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
	_createOperator( "+",	Ope_Add<float,int,float>, 				EMVT_REAL,			EMVT_INT,			EMVT_REAL		);
	_createOperator( "+",	Ope_Add<float,float,int>, 				EMVT_REAL,			EMVT_REAL,			EMVT_INT		);
	_createOperator( "+",	Ope_Add<int>,							EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "+",	Ope_Add<float>, 						EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "=",	Ope_Set<int>,							EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "=",	Ope_Set<bool>,							EMVT_BOOL,			EMVT_BOOL,			EMVT_BOOL		);
	_createOperator( "=",	Ope_Set<float>,							EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);
	_createOperator( "=",	Ope_Set<Point2D<float> >,				EMVT_VEC2F,			EMVT_VEC2F,			EMVT_VEC2F		);
	_createOperator( "=",	Ope_Set<Point3D<float> >,				EMVT_VEC3F,			EMVT_VEC3F,			EMVT_VEC3F		);
	_createOperator( "=",	Ope_Set<Point4D<float> >,				EMVT_VEC4F,			EMVT_VEC4F,			EMVT_VEC4F		);
	_createOperator( "=",	Ope_Set<Point2D<int> >,					EMVT_VEC2I,			EMVT_VEC2I,			EMVT_VEC2I		);
	_createOperator( "=",	Ope_Set<Point3D<int> >,					EMVT_VEC3I,			EMVT_VEC3I,			EMVT_VEC3I		);
	_createOperator( "=",	Ope_Set<Point4D<int> >,					EMVT_VEC4I,			EMVT_VEC4I,			EMVT_VEC4I		);
	_createOperator( "=",	Ope_Set<Point2Bool >,					EMVT_VEC2B,			EMVT_VEC2B,			EMVT_VEC2B		);
	_createOperator( "=",	Ope_Set<Point3Bool >,					EMVT_VEC3B,			EMVT_VEC3B,			EMVT_VEC3B		);
	_createOperator( "=",	Ope_Set<Point4Bool >,					EMVT_VEC4B,			EMVT_VEC4B,			EMVT_VEC4B		);
	_createOperator( "=",	Ope_Set<Matrix2<float> >,				EMVT_MAT2,			EMVT_MAT2,			EMVT_MAT2		);
	_createOperator( "=",	Ope_Set<Matrix3<float> >,				EMVT_MAT3,			EMVT_MAT3,			EMVT_MAT3		);
	_createOperator( "=",	Ope_Set<Matrix4<float> >,				EMVT_MAT4,			EMVT_MAT4,			EMVT_MAT4		);
	_createOperator( "=",	Ope_Set<NodeValueBaseArray>,			EMVT_ARRAY,			EMVT_ARRAY,			EMVT_ARRAY		);
	_createOperator( "=",	Ope_SetNull<StructInstance *>,			EMVT_STRUCT,		EMVT_STRUCT,		EMVT_NULLVALUE	);
	_createOperator( "=",	Ope_Set<StructInstance *>,				EMVT_STRUCT,		EMVT_STRUCT,		EMVT_STRUCT		);

	_createOperator( "=",	Ope_SetNull<StructInstance *>,			EMVT_STRUCT,		EMVT_STRUCT,		EMVT_NULLVALUE	);

	_createOperator( "==",	Ope_Compare<int>,						EMVT_BOOL,			EMVT_INT,			EMVT_INT		);
	_createOperator( "==",	Ope_Compare<bool>,						EMVT_BOOL,			EMVT_BOOL,			EMVT_BOOL		);
	_createOperator( "==",	Ope_Compare<float>,						EMVT_BOOL,			EMVT_REAL,			EMVT_REAL		);
	_createOperator( "==",	Ope_Compare<Point2D<float> >,			EMVT_BOOL,			EMVT_VEC2F,			EMVT_VEC2F		);
	_createOperator( "==",	Ope_Compare<Point3D<float> >,			EMVT_BOOL,			EMVT_VEC3F,			EMVT_VEC3F		);
	_createOperator( "==",	Ope_Compare<Point4D<float> >,			EMVT_BOOL,			EMVT_VEC4F,			EMVT_VEC4F		);
	_createOperator( "==",	Ope_Compare<Point2D<int> >,				EMVT_BOOL,			EMVT_VEC2I,			EMVT_VEC2I		);
	_createOperator( "==",	Ope_Compare<Point3D<int> >,				EMVT_BOOL,			EMVT_VEC3I,			EMVT_VEC3I		);
	_createOperator( "==",	Ope_Compare<Point4D<int> >,				EMVT_BOOL,			EMVT_VEC4I,			EMVT_VEC4I		);
	_createOperator( "==",	Ope_Compare<Point2Bool >,				EMVT_BOOL,			EMVT_VEC2B,			EMVT_VEC2B		);
	_createOperator( "==",	Ope_Compare<Point3Bool >,				EMVT_BOOL,			EMVT_VEC3B,			EMVT_VEC3B		);
	_createOperator( "==",	Ope_Compare<Point4Bool >,				EMVT_BOOL,			EMVT_VEC4B,			EMVT_VEC4B		);
	_createOperator( "==",	Ope_Compare<Matrix2<float> >,			EMVT_BOOL,			EMVT_MAT2,			EMVT_MAT2		);
	_createOperator( "==",	Ope_Compare<Matrix3<float> >,			EMVT_BOOL,			EMVT_MAT3,			EMVT_MAT3		);
	_createOperator( "==",	Ope_Compare<Matrix4<float> >,			EMVT_BOOL,			EMVT_MAT4,			EMVT_MAT4		);
	_createOperator( "==",	Ope_Compare<NodeValueBaseArray>,		EMVT_BOOL,			EMVT_ARRAY,			EMVT_ARRAY		);

	_createOperator( "==",	Ope_CompareNull<StructInstance*>,		EMVT_BOOL,			EMVT_STRUCT,		EMVT_NULLVALUE	);

	_createOperator( "!=",	Ope_IsDiff<int>,						EMVT_BOOL,			EMVT_INT,			EMVT_INT		);
	_createOperator( "!=",	Ope_IsDiff<bool>,						EMVT_BOOL,			EMVT_BOOL,			EMVT_BOOL		);
	_createOperator( "!=",	Ope_IsDiff<float>,						EMVT_BOOL,			EMVT_REAL,			EMVT_REAL		);
	_createOperator( "!=",	Ope_IsDiff<Point2D<float> >,			EMVT_BOOL,			EMVT_VEC2F,			EMVT_VEC2F		);
	_createOperator( "!=",	Ope_IsDiff<Point3D<float> >,			EMVT_BOOL,			EMVT_VEC3F,			EMVT_VEC3F		);
	_createOperator( "!=",	Ope_IsDiff<Point4D<float> >,			EMVT_BOOL,			EMVT_VEC4F,			EMVT_VEC4F		);
	_createOperator( "!=",	Ope_IsDiff<Point2D<int> >,				EMVT_BOOL,			EMVT_VEC2I,			EMVT_VEC2I		);
	_createOperator( "!=",	Ope_IsDiff<Point3D<int> >,				EMVT_BOOL,			EMVT_VEC3I,			EMVT_VEC3I		);
	_createOperator( "!=",	Ope_IsDiff<Point4D<int> >,				EMVT_BOOL,			EMVT_VEC4I,			EMVT_VEC4I		);
	_createOperator( "!=",	Ope_IsDiff<Point2Bool >,				EMVT_BOOL,			EMVT_VEC2B,			EMVT_VEC2B		);
	_createOperator( "!=",	Ope_IsDiff<Point3Bool >,				EMVT_BOOL,			EMVT_VEC3B,			EMVT_VEC3B		);
	_createOperator( "!=",	Ope_IsDiff<Point4Bool >,				EMVT_BOOL,			EMVT_VEC4B,			EMVT_VEC4B		);
	_createOperator( "!=",	Ope_IsDiff<Matrix2<float> >,			EMVT_BOOL,			EMVT_MAT2,			EMVT_MAT2		);
	_createOperator( "!=",	Ope_IsDiff<Matrix3<float> >,			EMVT_BOOL,			EMVT_MAT3,			EMVT_MAT3		);
	_createOperator( "!=",	Ope_IsDiff<Matrix4<float> >,			EMVT_BOOL,			EMVT_MAT4,			EMVT_MAT4		);
	_createOperator( "!=",	Ope_IsDiff<NodeValueBaseArray>,			EMVT_BOOL,			EMVT_ARRAY,			EMVT_ARRAY		);

	_createOperator( "!=",	Ope_IsDiffNull<StructInstance*>,		EMVT_BOOL,			EMVT_STRUCT,		EMVT_NULLVALUE	);

	_createOperator( "+=",	Ope_AddEqual<int,float>,				EMVT_REAL,			EMVT_INT,			EMVT_REAL		);
	_createOperator( "+=",	Ope_AddEqual<float,int>,				EMVT_REAL,			EMVT_REAL,			EMVT_INT		);
	_createOperator( "+=",	Ope_AddEqual<int,int>,					EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "+=",	Ope_AddEqual<float,float>,				EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "-",	Ope_Sub<float,int,float>,				EMVT_REAL,			EMVT_INT,			EMVT_REAL		);
	_createOperator( "-",	Ope_Sub<float,float,int>,				EMVT_REAL,			EMVT_REAL,			EMVT_INT		);
	_createOperator( "-",	Ope_Sub<int>,							EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "-",	Ope_Sub<float>,							EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "-=",	Ope_SubEqual<int,float>,				EMVT_REAL,			EMVT_INT,			EMVT_REAL		);
	_createOperator( "-=",	Ope_SubEqual<float,int>,				EMVT_REAL,			EMVT_REAL,			EMVT_INT		);
	_createOperator( "-=",	Ope_SubEqual<int,int>,					EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "-=",	Ope_SubEqual<float,float>,				EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "/",	Ope_Div<float,int,float>,				EMVT_REAL,			EMVT_INT,			EMVT_REAL		);
	_createOperator( "/",	Ope_Div<float,float,int>,				EMVT_REAL,			EMVT_REAL,			EMVT_INT		);
	_createOperator( "/",	Ope_Div<int>,							EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "/",	Ope_Div<float>,							EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "/=",	Ope_DivEqual<int,float>,				EMVT_REAL,			EMVT_INT,			EMVT_REAL		);
	_createOperator( "/=",	Ope_DivEqual<float,int>,				EMVT_REAL,			EMVT_REAL,			EMVT_INT		);
	_createOperator( "/=",	Ope_DivEqual<int,int>,					EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "/=",	Ope_DivEqual<float,float>,				EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "*",	Ope_Mul<float,int,float>,				EMVT_REAL,			EMVT_INT,			EMVT_REAL		);
	_createOperator( "*",	Ope_Mul<float,float,int>,				EMVT_REAL,			EMVT_REAL,			EMVT_INT		);
	_createOperator( "*",	Ope_Mul<int>,							EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "*",	Ope_Mul<float>,							EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);

	_createOperator( "*=",	Ope_MulEqual<int,float>,				EMVT_REAL,			EMVT_INT,			EMVT_REAL		);
	_createOperator( "*=",	Ope_MulEqual<float,int>,				EMVT_REAL,			EMVT_REAL,			EMVT_INT		);
	_createOperator( "*=",	Ope_MulEqual<int,int>,					EMVT_INT,			EMVT_INT,			EMVT_INT		);
	_createOperator( "*=",	Ope_MulEqual<float,float>,				EMVT_REAL,			EMVT_REAL,			EMVT_REAL		);

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
}

void ScriptCompiler :: _initialiseVariableMap()
{
	_constantGroup(		"General");
	_createConstant(	EMVT_REAL,				"GENERAL_ELAPSED_TIME"		);
	_createConstant(	EMVT_REAL,				"GENERAL_SCREEN_WIDTH"		);
	_createConstant(	EMVT_REAL,				"GENERAL_SCREEN_HEIGHT"		);

	_constantGroup(		"Math");
	_createConstant(	EMVT_REAL,				"PI"						)->set<float>(M_PI);
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
		_creaFunc(	"minmax",						Mth_Minmax,						EMVT_REAL,		EMVT_REAL,		EMVT_REAL,		EMVT_REAL,					EMVT_NULL);

		_creaFunc(	"sin",							Mth_Sin,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"cos",							Mth_Cos,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"tan",							Mth_Tan,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"asin",							Mth_ASin,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"acos",							Mth_ACos,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
		_creaFunc(	"atan",							Mth_ATan,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
	
		_creaFunc(	"sqrt",							Mth_Sqrt,						EMVT_REAL,		EMVT_REAL,													EMVT_NULL);
}

Function * ScriptCompiler :: _getClassFunction( VariableType * p_class, const String & p_functionName)const
{
	VariableBaseType l_base = p_class->GetBase();

	const ClassFunctionMap::const_iterator & l_classMapIter = m_classFunctions.find( l_base);

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

VariableType * ScriptCompiler :: FindType( const String & p_name)const
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

	m_intFlyweight.insert( std::map< int, ScriptNode *>::value_type( p_value, l_node));

	return l_node;
}

ScriptNode * ScriptCompiler :: GetFlyweight( float p_value)
{
	ScriptNode * l_node = map::findOrNull( m_realFlyweight, p_value); ;
	
	if (l_node != NULL)
	{
		return l_node;
	}
	
	l_node = CreateScriptNode();
	l_node->Use();

	l_node->SetType( VariableTypeManager::Get( EMVT_REAL));
	l_node->set<float>( p_value);

	m_realFlyweight.insert( std::map< float, ScriptNode *>::value_type( p_value, l_node));

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
	sm_singleton->_releaseNode( p_node);
}


UserFunction * ScriptCompiler :: GetUserFunction( const String & p_functionName)const
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
