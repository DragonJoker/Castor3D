#include "PrecompiledHeaders.h"

#include "glsl/ScriptEngine.h"
#include "glsl/ScriptFunctions.h"

#include "glsl/ScriptNode.h"
#include "glsl/ScriptCompiler.h"
#include "glsl/Context.h"

using namespace CastorShape;
using namespace Castor3D;

ScriptContext * ScriptEngine :: sm_context = NULL;

ScriptEngine :: ScriptEngine( const Path & p_path)
	:	m_path			(p_path)
{
	m_compiler = new ScriptCompiler( p_path);
}

ScriptEngine :: ~ScriptEngine()
{
	delete m_compiler;
}

void ScriptEngine :: Initialise()
{
	m_compiler->Initialise();
}

void ScriptEngine :: SetContext( ScriptContext * p_context)
{
	m_context = p_context;
	m_context->timerManager = m_timerManager;
	UseContext();
}

void ScriptEngine :: AddScriptZone( String & p_scriptString)
{
	ScriptNode * l_node = NULL;

	try
	{
		l_node = CompileScript( p_scriptString);
	}
	catch( Exception & p_exception)
	{
		Logger::LogWarning( CU_T( "ScriptEngine : Failed compilation of code zone from scene file -> ") + p_exception.GetDescription());
		l_node = NULL;
	}

	if (l_node  != NULL)
	{
		l_node->Use();
		Execute( l_node);
		l_node->Delete();
	}
}

void ScriptEngine :: Execute( ScriptNode * p_node)
{
	if (p_node->HasFunction())
	{
		p_node->Execute();
	}
}

ScriptNode * ScriptEngine :: GetKeyboardBind( unsigned int p_keyCode, unsigned int p_bindType)const
{
	return m_compiler->m_keyboardBinds[p_bindType][p_keyCode];
}

ScriptNode * ScriptEngine :: CompileScript( String & p_scriptString)
{
	return m_compiler->CompileScript( p_scriptString);
}

ScriptNode * ScriptEngine :: GetVariable( const String & p_variableName)const
{
	return m_compiler->GetProgramConstant( p_variableName);
}

ScriptNode * ScriptEngine :: CompileScriptFile( const String & p_strFileName)
{
	return m_compiler->CompileScriptFile( p_strFileName);
}

void ScriptEngine :: ScriptError( ScriptNode * p_node, const String & p_string)
{
	Logger::LogMessage( CU_T( "Runtime Error Line # ") + ToString( p_node->m_createdAtLine) + CU_T( " @ file ") + p_node->m_strFileName + CU_T( " : ") + p_string);
}

void ScriptEngine :: UseContext()const
{
	sm_context = m_context;
	ScriptCompiler::SetSingleton( m_compiler);

}

ScriptNode * ScriptEngine :: GetFunction( const String & p_functionName)const
{
	return m_compiler->GetUsableFunctionNode( p_functionName);
}
