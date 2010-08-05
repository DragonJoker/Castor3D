#ifndef ___EMUSE_SCRIPT_FUNCTIONS_OPERATORS___
#define ___EMUSE_SCRIPT_FUNCTIONS_OPERATORS___


#include "Module_Script.h"
#include "ScriptFunctions.h"

namespace CastorShape
{

	template<typename T,typename U>
	void d_fast_call Ope_SubEqual( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, U, b);

		RETURN_AS( T) a -= b;
	}

	template<typename T,typename U>
	void d_fast_call Ope_MulEqual( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, U, b);

		RETURN_AS( T) a *= b;
	}

	template<typename T,typename U>
	void d_fast_call Ope_AddEqual( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, U, b);

		RETURN_AS( T) a += b;
	}

	template<typename T,typename U>
	void d_fast_call Ope_DivEqual( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, U, b);

		RETURN_AS( T) a /= b;
	}

	template<typename R,typename T,typename U>
	void d_fast_call Ope_Add( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, U, b);

		RETURN_AS( R) a + b;
	}


	template<typename T>
	void d_fast_call Ope_Set( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, T, b);

		caller->m_childs[0]->CopyValue_Deep( caller->m_childs[1]);

		caller->CopyValue_Ref( caller->m_childs[0]->m_value);
		//RETURN_AS( T) a;
	}

	template<typename T>
	void d_fast_call Ope_Assign( ScriptNode * caller)
	{
		_times( 2)
		{
			if (caller->m_childs[i]->HasFunction())
			{
				caller->m_childs[i]->Execute();
			}
		}
//		GET_AND_EXEC_TWO_PARAM( T, a, T, b);

		caller->m_childs[0]->CopyValue_Ref( caller->m_childs[1]->m_value);
		caller->CopyValue_Ref( caller->m_childs[0]->m_value);
	}
	

	template<typename T>
	void d_fast_call Ope_SetNull( ScriptNode * caller)
	{
		GET_AND_EXEC_PARAM( T, a, 0);

		RETURN_AS( T) a = NULL;
	}

	template<typename T>
	void d_fast_call Ope_Compare( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, T, b);

		RETURN_AS( bool) a == b;
	}

	template<typename T>
	void d_fast_call Ope_CompareNull( ScriptNode * caller)
	{
		GET_AND_EXEC_PARAM( T, a, 0);

		RETURN_AS( bool) a == NULL;
	}

	template<typename T>
	void d_fast_call Ope_IsDiff( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, T, b);

		RETURN_AS( bool) a != b;
	}
	template<typename T>
	void d_fast_call Ope_IsDiffNull( ScriptNode * caller)
	{
		GET_AND_EXEC_PARAM( T, a, 0);

		RETURN_AS( bool) a != NULL;
	}

	template<typename T>
	void d_fast_call Ope_Add( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, T, b);

		RETURN_AS( T) a + b;
	}


	template<typename R,typename T,typename U>
	void d_fast_call Ope_Mul( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, U, b);

		RETURN_AS( R) a * b;
	}
	template<typename T>
	void d_fast_call Ope_Mul( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, T, b);

		RETURN_AS( T) a * b;
	}

	template<typename R,typename T,typename U>
	void d_fast_call Ope_Div( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, U, b);

		RETURN_AS( R) a / b;
	}
	template<typename T>
	void d_fast_call Ope_Div( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, T, b);

		RETURN_AS( T) a / b;
	}

	template<typename R,typename T,typename U>
	void d_fast_call Ope_Sub( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, U, b);

		RETURN_AS( R) a - b;
	}
	template<typename T>
	void d_fast_call Ope_Sub( ScriptNode * caller)
	{
		GET_AND_EXEC_TWO_PARAM( T, a, T, b);

		RETURN_AS( T) a - b;
	}

}

#endif
