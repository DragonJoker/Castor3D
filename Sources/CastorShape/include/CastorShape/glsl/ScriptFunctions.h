/*****************************************************************************

	Author : Marc BILLON
	Compagny : FDSSoftMedia - Copyright 2007

*****************************************************************************/

#ifndef ___EMUSE_SCRIPT_FUNCTIONS___
#define ___EMUSE_SCRIPT_FUNCTIONS___

/**********************************
	List of prefixes : 

	Gen : general code
	Ope : operators
	Ent : entity
	Tmr : timer
	Ve3 : vector3
	Qua : quaternion
	OvE : overlay elements
	OvL : overlays
	ScN : scene nodes
	Cam : camera
	CTx : camtex
	Lgh : light
	Arr : array
	Col : colour
	Snd : sound
	Pll : sound playlist
	Vid : video
	Tim : time
	Spc : special
	Cst : casts
	Mth : math
	Str : string
	Chr : char
	Mir : mirrors
	Sce : scene
	AoG : animated object group
	AOb : animated object
	Ani : animation
	PhS : physics simulation
	PhM : physics material
	POb : physics object
	Uni : universe
	Mat : materials
	Wim : WebImage

**********************************/


#include "Module_Script.h"

namespace CastorShape
{
	#define EMUSE_SCRIPT_FUNCTION_DEFINE( X) void d_fast_call X( ScriptNode * );

	#define EMUSE_SCRIPT_FUNCTION_DECLARE( X) void d_fast_call CastorShape :: X( ScriptNode * caller)

	#define VERBOSE_STARTFUNC( func) //Logger::LogMessage( String( "ScriptFunctionStarted : ") + String( func))

	#define RETURN_AS(p_type) caller->get<p_type>() =

	#define SCRIPT_ERROR(p_error) ScriptEngine::ScriptError( caller, p_error)

	#define GET_AND_EXEC_PARAM(p_type,p_variable,p_index) \
		p_type & p_variable = caller->getAndExecChild<p_type>(p_index);

	#define GET_AND_EXEC_TWO_PARAM(p_type1,p_variable1,p_type2,p_variable2) \
		p_type1 & p_variable1 = caller->getAndExecChild<p_type1>(0);		\
		p_type2 & p_variable2 = caller->getAndExecChild<p_type2>(1);

	#define GET_AND_EXEC_THREE_PARAM(p_type1,p_variable1,p_type2,p_variable2,p_type3,p_variable3) \
		p_type1 & p_variable1 = caller->getAndExecChild<p_type1>(0);		\
		p_type2 & p_variable2 = caller->getAndExecChild<p_type2>(1);		\
		p_type3 & p_variable3 = caller->getAndExecChild<p_type3>(2);

	#define GET_AND_EXEC_FOUR_PARAM(p_type1,p_variable1,p_type2,p_variable2,p_type3,p_variable3,p_type4,p_variable4) \
		p_type1 & p_variable1 = caller->getAndExecChild<p_type1>(0);		\
		p_type2 & p_variable2 = caller->getAndExecChild<p_type2>(1);		\
		p_type3 & p_variable3 = caller->getAndExecChild<p_type3>(2);		\
		p_type4 & p_variable4 = caller->getAndExecChild<p_type4>(3);

	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_IfBlock )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_WhileLoop )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_ForLoop )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_ExecuteFunction )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_MultiLineFunction )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_Return )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_ReturnNull )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_Break )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Gen_Continue )

	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec2f_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec3f_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec4f_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec2b_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec3b_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec4b_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec2i_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec3i_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Vec4i_OperatorArray)
/*
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat2_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat2x3_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat2x4_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat3_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat3x2_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat3x4_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat4_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat4x2_OperatorArray)
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mat4x3_OperatorArray)
*/
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_StructGetMember )	

	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_Increment_Int )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_Decrement_Int )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_Negate )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_NegateReal )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_NegateInt )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_Or )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_ModuloInt )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_And )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsEqual )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsDifferent )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsSuperior_Int )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsSuperior_Real )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsSuperiorOrEqual_Int )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsSuperiorOrEqual_Real )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsInferior_Int )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsInferior_Real )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsInferiorOrEqual_Int )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_IsInferiorOrEqual_Real )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_SetValue )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_AddValue )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_SubtractValue )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_DivideValue )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Ope_MultiplyValue )

	EMUSE_SCRIPT_FUNCTION_DEFINE( Arr_Add )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Arr_AddDefault )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Arr_RemoveIndex )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Arr_GetSize )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Arr_Clear )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Arr_Set )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Arr_Get )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Arr_IsEmpty )

	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Rand )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Abs )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Floor )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Ceil )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Round )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Min )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Max )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Clamp )

	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Sin )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Cos )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Tan )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_ASin )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_ACos )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_ATan )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Sqr )
	EMUSE_SCRIPT_FUNCTION_DEFINE( Mth_Sqrt )
}

#endif
