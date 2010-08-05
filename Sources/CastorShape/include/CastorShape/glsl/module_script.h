/*****************************************************************************

	Author : Marc BILLON
	Compagny : FDSSoftMedia - Copyright 2007

*****************************************************************************/

#ifndef ___MODULE_SCRIPT_H___
#define ___MODULE_SCRIPT_H___

#include <queue>

namespace CastorShape
{
	#define NUM_KEYS 255

	enum VariableBaseType
	{
		EMVT_NULL,
		EMVT_BOOL,
		EMVT_INT,
		EMVT_REAL,
		EMVT_VEC2F,
		EMVT_VEC3F,
		EMVT_VEC4F,
		EMVT_VEC2B,
		EMVT_VEC3B,
		EMVT_VEC4B,
		EMVT_VEC2I,
		EMVT_VEC3I,
		EMVT_VEC4I,
		EMVT_MAT2,
		EMVT_MAT3,
		EMVT_MAT4,
		EMVT_SAMPLER1D,
		EMVT_SAMPLER2D,
		EMVT_SAMPLER3D,
		EMVT_SAMPLERCUBE,
		EMVT_ARRAY,
		EMVT_CODE,
		EMVT_NUM_TYPES,
		EMVT_STRUCT,
//		EMVT_ANY = EMVT_CODE, <- banned
		EMVT_SUB1,
		EMVT_SUB2,
		EMVT_ARRAYS1,
		EMVT_NULLVALUE
	};

	enum KeyboardBindType
	{
		KBT_KEY_DOWN			= 0,
		KBT_KEY_UP				= 1,
		KBT_KEY_PRESSED			= 2,
		KBT_KEY_REPEAT			= 3
	};

	enum OperatorLevel
	{
		SO_MULTILINE,
		SO_EQUAL,
		SO_OR,
		SO_AND,
		SO_EQUALITY,
		SO_INFERIORITY,
		SO_SUPERIORITY,
		SO_INCREMENT,
		SO_PLUSMINUS,
		SO_MULDIV,
		SO_MODULO,
		SO_NEGATE,
		SO_ARROW,
		SO_STRUCTMEMBER = SO_ARROW,
		SO_ARRAY = SO_STRUCTMEMBER,
		SO_NONE
	};

	enum ScriptTimerType
	{
		EMTT_ONCE		= 0,
		EMTT_REPEAT		= 1,
		EMTT_CONTINUOUS	= 2,
		EMTT_PERMANENT	= 3
	};

	enum BlockType
	{
		BT_SEPARATOR		= 0x0001,
		BT_STRING			= 0x0002,
		BT_NUMERAL			= 0x0004,
		BT_OPERATOR			= 0x0008,
		BT_PARENTHESIS		= 0x0010,
		BT_BRACKETS			= 0x0020,
		BT_BRACES			= 0x0040,
		BT_SIMPLEQUOTE		= 0x0080,
		BT_DOUBLEQUOTE		= 0x0100,
		BT_INITIAL			= 0x0200,
		BT_VARIABLE_TYPE	= 0x0400,
		BT_IF_ELSE			= 0x0800,
		BT_STRUCT			= 0x1000,
		BT_TYPEDEF			= 0x2000,
		BT_RETURN			= 0x4000
	};

	enum BlockSubType
	{
		BST_SEPARATOR_MAJOR,
		BST_SEPARATOR_MINOR,
		BST_FUNCTION,
		BST_VARIABLE,
		BST_KEYWORD_VTYPE,
		BST_KEYWORD_IFELSE,
		BST_NONE
	};

	class ScriptBlock;
	class ScriptEngine;
	class ScriptCompiler;
	class ScriptNode;
	class ScriptTimer;
	class ScriptTimerManager;
	struct Context;

	class NodeValueBase;
	template<typename T>
	class NodeValue;

	class Structure;
	class StructRow;
	class StructInstance;

	class Function;
	class OperatorFunction;
	class UserFunction;
	class VariableType;
	class VariableTypeManager;

	typedef void  ( d_fast_call RawFunction)(ScriptNode*);

	typedef std::map <String, Function *>							FunctionMap;
	typedef std::map <String, UserFunction *>						UserFunctionMap;
	typedef std::multimap <String, OperatorFunction *>				OperatorFunctionMultiMap;
	typedef std::map <VariableBaseType, FunctionMap>				ClassFunctionMap;

	typedef std::map <String, ScriptNode *>							ScriptNodeMap;
	typedef std::map <String, ScriptTimer *>						TimerMap;
	typedef std::vector <ScriptNode *>								ScriptNodeArray;
	typedef std::vector <ScriptNodeArray>							ScriptNodeArrayArray;
	typedef std::vector <VariableType *>							VariableTypeArray;
	typedef std::map <String, VariableType *>						VariableTypeMap;
	typedef std::vector <VariableBaseType>							VariableBaseTypeArray;
	typedef std::vector <ScriptBlock *>								ScriptBlockArray;
	typedef std::queue <ScriptNode *>								ScriptNodeQueue;
	typedef std::set <ScriptNode *>									ScriptNodeSet;

	typedef std::vector <NodeValueBase *>							NodeValueBaseArray;
	typedef std::map <int, NodeValueBase *>							NodeValueBaseIMap;
	typedef std::map <String ,NodeValueBase *>						NodeValueBaseMap;
	typedef std::map <float, NodeValueBase *>						NodeValueBaseRMap;
	typedef std::pair <NodeValueBase *, NodeValueBase *>			NodeValueBasePair;

	typedef std::vector<StructRow*>									StructRowArray;
	typedef std::map<String, Structure*>							StructureMap;
	

#define VERBOSE_COMPILATOR( p_string ) // std::cout << p_string << std::endl; // m_compiler->_log( p_string);
	#define VERBOSE_COMPILATOR_INTERNAL( p_string ) // std::cout << p_string << std::endl; // _log( p_string);
	//Log::LogMessage( p_string )

	
}

#endif
