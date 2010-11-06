#include "PrecompiledHeaders.h"

#include "glsl/ScriptFunctions.h"

#include "glsl/ScriptEngine.h"
#include "glsl/VariableType.h"

#include "glsl/ScriptNode.h"
#include "glsl/ScriptNodeValue.h"
#include "glsl/ScriptCompiler.h"
#include "glsl/Context.h"

using namespace CastorShape;
using namespace Castor3D;

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec2f_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (Vec2)");

	GET_AND_EXEC_TWO_PARAM( Point2Real, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 2)
	{
		RETURN_AS( real) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec2f_OperatorArray : index out of bounds");
		RETURN_AS( real) 0.0;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec3f_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (Vec3)");

	GET_AND_EXEC_TWO_PARAM( Point3Real, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 3)
	{
		RETURN_AS( real) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec3f_OperatorArray : index out of bounds");
		RETURN_AS( real) 0.0;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec4f_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (Vec4)");

	GET_AND_EXEC_TWO_PARAM( Point4Real, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 4)
	{
		RETURN_AS( real) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec4f_OperatorArray : index out of bounds");
		RETURN_AS( real) 0.0;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec2i_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (iVec2)");

	GET_AND_EXEC_TWO_PARAM( Point2Int, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 2)
	{
		RETURN_AS( int) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec2f_OperatorArray : index out of bounds");
		RETURN_AS( int) 0;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec3i_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (iVec3)");

	GET_AND_EXEC_TWO_PARAM( Point3Int, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 3)
	{
		RETURN_AS( int) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec3f_OperatorArray : index out of bounds");
		RETURN_AS( int) 0;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec4i_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (iVec4)");

	GET_AND_EXEC_TWO_PARAM( Point4Int, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 4)
	{
		RETURN_AS( int) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec4f_OperatorArray : index out of bounds");
		RETURN_AS( int) 0;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec2b_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (bVec2)");

	GET_AND_EXEC_TWO_PARAM( Point2Bool, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 2)
	{
		RETURN_AS( bool) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec2f_OperatorArray : index out of bounds");
		RETURN_AS( bool) false;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec3b_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (bVec3)");

	GET_AND_EXEC_TWO_PARAM( Point3Bool, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 3)
	{
		RETURN_AS( bool) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec3f_OperatorArray : index out of bounds");
		RETURN_AS( bool) false;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Vec4b_OperatorArray)
{
	VERBOSE_STARTFUNC( " [] (Vbec4)");

	GET_AND_EXEC_TWO_PARAM( Point4Bool, l_vector, int, l_index);

	if (l_index >= 0 && l_index < 4)
	{
		RETURN_AS( bool) l_vector[l_index];
	}
	else
	{
		SCRIPT_ERROR( "Error @ Vec4f_OperatorArray : index out of bounds");
		RETURN_AS( bool) false;
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Arr_Add)
{
	VERBOSE_STARTFUNC( "Array_Add");

	GET_AND_EXEC_PARAM( NodeValueBaseArray, arr, 0);

	ScriptNode * l_node = caller->m_childs[1];

	if (l_node->HasFunction())
	{
		l_node->Execute();
	}
	
	arr.push_back( l_node->m_value->clone());
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Arr_AddDefault)
{
	VERBOSE_STARTFUNC( "Arr_AddDefault");

	GET_AND_EXEC_PARAM( NodeValueBaseArray, arr, 0);

	NodeValueBase * l_node = ScriptNode::CreateNodeValue( caller->m_childs[0]->m_type->GetSubType( 0));
	arr.push_back( l_node);
	caller->CopyValue_Ref( l_node);
}


EMUSE_SCRIPT_FUNCTION_DECLARE( Arr_RemoveIndex)
{
	VERBOSE_STARTFUNC( "Array_RemoveIndex");
	
	GET_AND_EXEC_TWO_PARAM( NodeValueBaseArray, arr, int, index);

	if (index >= 0 && static_cast <size_t> (index) < arr.size())
	{
		delete arr[index];
		arr.erase( arr.begin() + index);
	}
	else
	{
		SCRIPT_ERROR( "Error @ Array_RemoveIndex : index out of bounds");
	}
}
EMUSE_SCRIPT_FUNCTION_DECLARE( Arr_Set)
{
	VERBOSE_STARTFUNC( "Arr_Set");

	GET_AND_EXEC_TWO_PARAM( NodeValueBaseArray, arr, int, l_index);

	ScriptNode * l_param = caller->m_childs[2];

	if (l_param->HasFunction())
	{
		l_param->Execute();
	}
		
	if (l_index < 0 || static_cast <size_t> (l_index) >= arr.size())
	{
		return SCRIPT_ERROR( "Error @ Array_Set : index out of bounds");
	}

	NodeValueBase * l_value = arr[l_index];

	if (l_value->m_type != l_param->m_value->m_type)
	{
		return SCRIPT_ERROR( "Error @ Array_Set : type mismatch");
	}
	l_value->CopyDeepValue( l_param->m_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Arr_GetSize)
{
	VERBOSE_STARTFUNC( "Array_GetSize");

	GET_AND_EXEC_PARAM( NodeValueBaseArray, arr, 0);

	RETURN_AS( int) static_cast <int> (arr.size());
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Arr_Clear)
{
	VERBOSE_STARTFUNC( "Array_Clear");

	GET_AND_EXEC_PARAM( NodeValueBaseArray, arr, 0);

	vector::deleteAll( arr);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Arr_IsEmpty)
{
	VERBOSE_STARTFUNC( "Array_IsEmpty");

	GET_AND_EXEC_PARAM( NodeValueBaseArray, arr, 0);

	RETURN_AS( bool) arr.empty();
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Arr_Get)
{
	VERBOSE_STARTFUNC( "Arr_Get");

	GET_AND_EXEC_TWO_PARAM( NodeValueBaseArray, arr, int, l_index);

	if (l_index >= 0 && l_index < int( arr.size()))
	{
		caller->CopyValue_Ref( arr[l_index]);
	}
	else
	{
		caller->SetNullRef();
		SCRIPT_ERROR( "Arr_Get : index out of bounds( " + ToString( l_index) + " out of " + ToString( arr.size()) + ")");
	}
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Abs)
{
	VERBOSE_STARTFUNC( "Math_Abs");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) abs( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Floor)
{
	VERBOSE_STARTFUNC( "Math_Floor");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) floor( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Ceil)
{
	VERBOSE_STARTFUNC( "Math_Ceil");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) ceil( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Round)
{
	VERBOSE_STARTFUNC( "Math_Round");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) ceil( p_value + 0.5);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Sin)
{
	VERBOSE_STARTFUNC( "Mth_Sin");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) sin( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Cos)
{
	VERBOSE_STARTFUNC( "Mth_Cos");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) cos( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Tan)
{
	VERBOSE_STARTFUNC( "Mth_Tan");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) tan( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_ASin)
{
	VERBOSE_STARTFUNC( "Mth_ASin");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) asin( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_ACos)
{
	VERBOSE_STARTFUNC( "Mth_ACos");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) acos( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_ATan)
{
	VERBOSE_STARTFUNC( "Mth_ATan");

	GET_AND_EXEC_PARAM( real, p_value, 0);
 
	RETURN_AS( real) atan( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Sqrt)
{
	VERBOSE_STARTFUNC( "Mth_Sqrt");

	GET_AND_EXEC_PARAM( real, p_value, 0);

	RETURN_AS( real) sqrt( p_value);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Min)
{
	VERBOSE_STARTFUNC( "Mth_Min");

	GET_AND_EXEC_TWO_PARAM( real, p_a, real, p_b);

	RETURN_AS( real) std::min<real>( p_a, p_b);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Max)
{
	VERBOSE_STARTFUNC( "Mth_Max");

	GET_AND_EXEC_TWO_PARAM( real, p_a, real, p_b);

	RETURN_AS( real) std::max<real>( p_a, p_b);
}

EMUSE_SCRIPT_FUNCTION_DECLARE( Mth_Minmax)
{
	VERBOSE_STARTFUNC( "Mth_Minmax");

	GET_AND_EXEC_THREE_PARAM( real, p_min, real, p_value, real, p_max);

	RETURN_AS( real) minmax( p_min, p_value, p_max);
}