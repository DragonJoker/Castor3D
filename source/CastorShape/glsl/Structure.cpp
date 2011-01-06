#include "CastorShape/PrecompiledHeader.h"

#include "CastorShape/glsl/Structure.h"
#include "CastorShape/glsl/VariableType.h"
#include "CastorShape/glsl/ScriptNodeValue.h"
#include "CastorShape/glsl/ScriptNode.h"
#include "CastorShape/glsl/ScriptCompiler.h"
#include "CastorShape/glsl/Function.h"

using namespace CastorShape;
using namespace Castor3D;


Structure :: Structure( ScriptCompiler * p_pCompiler, const String & p_name)
	:	Named (p_name)
	,	m_pCompiler( p_pCompiler)
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

	ScriptNode * l_node = m_pCompiler->CreateScriptNode();
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
	l_desc += CU_T( "}\n");
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
