#include "PrecompiledHeaders.h"

#include "glsl/Structure.h"
#include "glsl/VariableType.h"
#include "glsl/ScriptNodeValue.h"
#include "glsl/ScriptNode.h"
#include "glsl/ScriptCompiler.h"
#include "glsl/Function.h"

using namespace CastorShape;
using namespace Castor3D;


Structure :: Structure( const String & p_name)
	:	named (p_name)
{
	m_type = new VariableType( EMVT_STRUCT, NULL, NULL);
	m_type->m_struct = this;
}

Structure :: ~Structure()
{
	vector::deleteAll( m_members);
	map::deleteAll( m_classFunctions);
	delete m_type;
}

void Structure :: AddMember( const String & p_name, VariableType * p_type)
{
	m_members.push_back( new StructRow( p_name, p_type));
	m_type->AddSubType( p_type);
}

void Structure :: AddFunction( UserFunction * p_function)
{
	m_classFunctions.insert( UserFunctionMap::value_type( p_function->GetName(), p_function));
	p_function->m_params.push_back( m_type);

	ScriptNode * l_node = ScriptCompiler::GetSingletonPtr()->CreateScriptNode();
	l_node->SetType( m_type);
	l_node->Use();
	l_node->CopyValue_Ref( NULL);
	p_function->m_paramNodes.push_back( l_node);

	p_function->m_localVars.insert( ScriptNodeMap::value_type( "this", l_node));
}

String Structure :: GetDesc() const
{
	String l_desc = "struct " + m_name + "\n{\n";
	_times( m_members.size())
	{
		l_desc += m_members[i]->GetType()->GetDesc() + " " + m_members[i]->GetName() + "\n";
	}
	l_desc += "}\n";
	return l_desc;
}

unsigned int Structure :: FindMember( const String & p_name)const
{
	_times( m_members.size())
	{
		if (m_members[i]->GetName() == p_name) { return i; }
	}
	return String::npos;
}

StructInstance :: StructInstance( VariableType * p_def)
	:	m_definition( p_def)
{
	_times( m_definition->m_subTypes.size())
	{
		m_members.push_back( ScriptNode::CreateNodeValue( m_definition->m_subTypes[i]));
	}
}

StructInstance :: ~StructInstance()
{
	vector::deleteAll( m_members);
}

void StructInstance :: Copy( StructInstance * p_origin)
{
	m_definition = p_origin->m_definition;

	vector::deleteAll( m_members);

	_times( p_origin->m_members.size())
	{
		m_members.push_back( p_origin->m_members[i]->clone());
	}
}
