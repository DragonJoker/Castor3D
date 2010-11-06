#include "PrecompiledHeaders.h"

#include "glsl/ScriptFunctions.h"
#include "glsl/ScriptNode.h"
#include "glsl/ScriptEngine.h"
#include "glsl/ScriptCompiler.h"
#include "glsl/VariableType.h"
#include "glsl/Structure.h"
#include "glsl/MapIterator.h"
#include "glsl/Context.h"

using namespace CastorShape;
using namespace Castor3D;


EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_IfBlock)
{
	VERBOSE_STARTFUNC( "if");

	/*
	std::cout << "list childs : " << std::endl;
	_times( caller->m_childs.size())
	{
		std::cout << "child : " << i << " types : " << caller->m_childs[i]->GetType()->GetDesc() << std::endl;
	}
	*/

	size_t numParams = caller->GetNumChilds();
	
	size_t i = 0;
	size_t max = 0;

	bool useelse;
	bool finished = false;

	max = numParams;
	if (numParams % 2 == 1)
	{
		useelse = true;
		max--;
	}
	else
	{
		useelse = false;
	}

	max = (max/2);

	for ( i = 0 ; i < max && finished == false ; i ++)
	{
		bool l_condition = caller->getAndExecChild<bool>( 2 * i);
		if (l_condition)
		{
			finished = true;

			try
			{
				caller->ExecChild( 2 * i + 1);
			}
			catch (Exception & p_exception)
			{
				Log::LogMessage( "EMuseInstance :: Initialise, exception (General) --> " + p_exception.GetDescription());
			}

		}
		if (ScriptEngine::GetContext()->m_breakOne || ScriptEngine::GetContext()->m_continue || ScriptEngine::GetContext()->m_return)
		{
			return;
		}
	}

	if (useelse && ! finished)
	{
		caller->ExecChild( numParams - 1 );
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_WhileLoop)
{
	VERBOSE_STARTFUNC( "while loop");

	ScriptNode * cond,* code;

	cond = caller->m_childs[0];
	code = caller->m_childs[1];

	if (cond->HasFunction())
	{
		cond->Execute();
	}

	bool l_condition = cond->get<bool>();
unsigned int i = 0;
	while (l_condition)
	{
		i++;
		if (i > 1000000)break;
		if (code->HasFunction())
		{
			try
			{
				code->Execute();
			}
			catch (Exception & p_exception)
			{
				Log::LogMessage( "EMuseInstance :: Initialise, exception (General) --> " + p_exception.GetDescription());
			}
		}

		if (ScriptEngine::GetContext()->m_breakOne)
		{
			ScriptEngine::GetContext()->m_breakOne = false;
			return;
		}

		if (ScriptEngine::GetContext()->m_return)
		{
			return;
		}

		if (ScriptEngine::GetContext()->m_continue)
		{
			ScriptEngine::GetContext()->m_continue = false;
		}

		if (cond->HasFunction())
		{
			try
			{
				cond->Execute();
			}
			catch (Exception & p_exception)
			{
				Log::LogMessage( "EMuseInstance :: Initialise, exception (General) --> " + p_exception.GetDescription());
			}
			l_condition = cond->get<bool>();
		}
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_ForLoop)
{
	VERBOSE_STARTFUNC( "for loop");
	ScriptNode * init,* cond,* code,* end;

	init = caller->m_childs[0];
	cond = caller->m_childs[1];
	end = caller->m_childs[2];
	code = caller->m_childs[3];

	if (init->HasFunction())
	{
		init->m_function( init);
	}

	if (cond->HasFunction())
	{
		cond->m_function( cond);
	}

	try
	{
		unsigned int i = 0 ; 
		while (cond->get<bool>())
		{
		i++;
		if (i > 1000000)break;
			if (code->HasFunction())
			{
				code->Execute();
			}

			if (end->HasFunction())
			{
				end->Execute();
			}

			if (cond->HasFunction())
			{
				cond->Execute();
			}
			if (ScriptEngine::GetContext()->m_breakOne)
			{
				ScriptEngine::GetContext()->m_breakOne = false;
				return;
			}
			if (ScriptEngine::GetContext()->m_continue)
			{
				ScriptEngine::GetContext()->m_continue = false;
			}
			if (ScriptEngine::GetContext()->m_return)
			{
				return;
			}
		}
	}
	catch (Exception & p_exception)
	{
		Log::LogMessage( "EMuseInstance :: Initialise, exception (General) --> " + p_exception.GetDescription());
	}

}

EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_ExecuteFunction)
{
	VERBOSE_STARTFUNC( "personal function");
	ScriptNode * l_node;

	size_t numParams,j;

	numParams = caller->GetNumChilds() - 3;
	numParams = numParams/2;

	l_node = caller->m_childs[1];

	if (numParams != static_cast <size_t> (l_node->get<int>()) )
	{
		SCRIPT_ERROR("Error @ ExecuteFunction, no correct numParams ! real value : " + ToString( numParams) + " , expected : " + ToString( l_node->get<int>() ));
	}
	else
	{
		ScriptNode * l_currentFunction = ScriptEngine::GetContext()->m_currentFunction;
		ScriptEngine::GetContext()->m_currentFunction = caller;
	
		for ( j = 0 ; j < numParams ; j ++)
		{
			ScriptNode * l_tempNode = caller->m_childs[3 + j + numParams];
			if (l_tempNode->HasFunction())
			{
				l_tempNode->Execute();
			}
			l_node = caller->m_childs[2 + j];
			if (l_node->IsRef())
			{
				l_node->CopyValue_Ref( l_tempNode->m_value);
			}
			else
			{
				l_node->CopyValue_Deep( l_tempNode);
			}
		}

		l_node = caller->m_childs[2 + numParams];

		if (l_node->HasFunction())
		{
			l_node->Execute();
		}
	
		caller->CopyValue_Ref( caller->m_childs[0]->m_value);

		ScriptEngine::GetContext()->m_currentFunction = l_currentFunction;
	}
	ScriptEngine::GetContext()->m_return = false;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_MultiLineFunction)
{
	size_t i,max;
	ScriptNode * l_sN;

	max = caller->GetNumChilds();

	for ( i = 0 ; i < max ; i ++)
	{
		l_sN = caller->m_childs[i];

		try
		{
			if( l_sN->HasFunction())
			{
				l_sN->Execute();
			}
		}
		catch (Exception & p_exception)
		{
			Log::LogMessage( "EMuseInstance :: Initialise, exception (General) --> " + p_exception.GetDescription());
		}

		if (ScriptEngine::GetContext()->m_breakOne || ScriptEngine::GetContext()->m_return || ScriptEngine::GetContext()->m_continue)
		{
			return;
		}
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_Break)
{
	VERBOSE_STARTFUNC( "Gen_Break");

	ScriptEngine::GetContext()->m_breakOne = true;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_Continue)
{
	VERBOSE_STARTFUNC( "Gen_Continue");

	ScriptEngine::GetContext()->m_continue = true;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_Return)
{
	VERBOSE_STARTFUNC( "Gen_Return");

	if (ScriptEngine::GetContext()->m_currentFunction != NULL)
	{
//		std::cout << "gen return ! " << std::endl;
		ScriptEngine::GetContext()->m_return = true;

		if (caller->m_childs[0]->HasFunction())
		{
			caller->m_childs[0]->Execute();
		}

		ScriptNode * l_function = ScriptEngine::GetContext()->m_currentFunction;
		if (l_function->m_childs[0]->IsRef())
		{
//			std::cout << "func, copy ref ? " << std::endl;
			l_function->m_childs[0]->CopyValue_Ref( caller->m_childs[0]->m_value);
		}
		else
		{
//			std::cout << "func, copy Deep ? " << std::endl;
			l_function->m_childs[0]->CopyValue_Deep( caller->m_childs[0]);
		}
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Gen_ReturnNull)
{
	VERBOSE_STARTFUNC( "Gen_ReturnNull");

	ScriptEngine::GetContext()->m_return = true;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_StructGetMember)
{
	VERBOSE_STARTFUNC( "Ope_StructGetMember");

	GET_AND_EXEC_TWO_PARAM( StructInstance *,l_struct, int, l_index);

	caller->CopyValue_Ref( l_struct->GetMember( l_index));
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_Increment_Int)
{
	VERBOSE_STARTFUNC( "++(int)");
	
	GET_AND_EXEC_PARAM( int, l_value, 0);

	RETURN_AS( int) ++l_value;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_Decrement_Int)
{
	VERBOSE_STARTFUNC( "--(int)");
	
	GET_AND_EXEC_PARAM( int, l_value, 0);

	RETURN_AS( int) --l_value;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_Negate)
{
	VERBOSE_STARTFUNC( "!(unary)bool");

	GET_AND_EXEC_PARAM( bool, l_value, 0);

	RETURN_AS( bool) ! l_value;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_NegateReal)
{
	VERBOSE_STARTFUNC( "-(unary)real");

	GET_AND_EXEC_PARAM( real, l_value, 0);

	RETURN_AS( real) -l_value;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_NegateInt)
{
	VERBOSE_STARTFUNC( "-(unary)int");

	GET_AND_EXEC_PARAM( int, l_value, 0);

	RETURN_AS( int) -l_value;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_Or)
{
	VERBOSE_STARTFUNC( "or");

	GET_AND_EXEC_PARAM( bool, a, 0);
	if (a)
	{
		RETURN_AS( bool) true;
	}
	else
	{
		GET_AND_EXEC_PARAM( bool, b, 1);
		RETURN_AS( bool) b;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_ModuloInt)
{
	VERBOSE_STARTFUNC( "Modulo int");

	GET_AND_EXEC_TWO_PARAM( int, a, int, b);

	RETURN_AS( int) a % b;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_And)
{
	VERBOSE_STARTFUNC( "and");

	GET_AND_EXEC_PARAM( bool, a, 0);
	if ( ! a)
	{
		RETURN_AS( bool) false;
	}
	else
	{
		GET_AND_EXEC_PARAM( bool, b, 1);
		RETURN_AS( bool) b;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_IsSuperior_Int)
{
	VERBOSE_STARTFUNC( ">(int)");

	GET_AND_EXEC_TWO_PARAM( int, a, int, b);

	RETURN_AS( bool) a > b;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_IsSuperior_Real)
{
	VERBOSE_STARTFUNC( ">(real)");

	GET_AND_EXEC_TWO_PARAM( real, a, real, b);

	RETURN_AS( bool) a > b;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_IsSuperiorOrEqual_Int)
{
	VERBOSE_STARTFUNC( ">=(int)");

	GET_AND_EXEC_TWO_PARAM( int, a, int, b);

	RETURN_AS( bool) a >= b;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_IsSuperiorOrEqual_Real)
{
	VERBOSE_STARTFUNC( ">=(real)");

	GET_AND_EXEC_TWO_PARAM( real, a, real, b);

	RETURN_AS( bool) a >= b;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_IsInferior_Int)
{
	VERBOSE_STARTFUNC( "<(int)");

	GET_AND_EXEC_TWO_PARAM(int, a, int, b);

	RETURN_AS( bool) a < b;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_IsInferior_Real)
{
	VERBOSE_STARTFUNC( "<(real)");

	GET_AND_EXEC_TWO_PARAM( real, a, real, b);

	RETURN_AS( bool) a < b;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_IsInferiorOrEqual_Int)
{
	VERBOSE_STARTFUNC( "<=(int)");

	GET_AND_EXEC_TWO_PARAM( int, a, int, b);

	RETURN_AS( bool) a <= b;
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Ope_IsInferiorOrEqual_Real)
{
	VERBOSE_STARTFUNC( "<=(real)");

	GET_AND_EXEC_TWO_PARAM( real, a, real, b);

	RETURN_AS( bool) a <= b;
}