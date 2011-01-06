#ifndef ___EMUSE_SCRIPT_FUNCTION_H___
#define ___EMUSE_SCRIPT_FUNCTION_H___

#include "Module_Script.h"

namespace CastorShape
{
	class Function : public Named
	{
	public:
		RawFunction * m_function;
		VariableType * m_return;
		VariableTypeArray m_params;

	public:
		Function( const String & p_name)
			:	Named( p_name),
				m_function( NULL),
				m_return( NULL)
		{
		}
	};

	class OperatorFunction : public Function
	{
	public:
		bool m_unary;
		bool m_pre;

	public:
		OperatorFunction( const String & p_name)
			:	Function( p_name),
				m_unary( false),
				m_pre( false)
		{
		}
	};

	class UserFunction : public Function
	{
	public:
		ScriptNode * m_node;
		ScriptNode * m_returnNode;
		ScriptNode * m_finalNode;

		ScriptNodeArray m_paramNodes;

		String m_name;
		bool m_initialised;
		ScriptNodeArrayArray m_deferredUses;
		ScriptNodeMap m_localVars;

	public:
		UserFunction( const String & p_name)
			:	Function( p_name),
				m_node			(NULL),
				m_returnNode	(NULL),
				m_finalNode		(NULL),
				m_initialised	(false)
		{}

		~UserFunction();

	public:
		void DeferUse( const ScriptNodeArray & p_array)
		{
			m_deferredUses.push_back( p_array);
		}
	};
}

#endif
