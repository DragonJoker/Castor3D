#include "PrecompiledHeaders.h"

#include "glsl/ScriptNode.h"
#include "glsl/ScriptCompiler.h"
#include "glsl/VariableType.h"
#include "glsl/Structure.h"

using namespace CastorShape;
using namespace Castor3D;

unsigned int ScriptNode::sm_numNodes = 0;

ScriptNode :: ScriptNode( const String & p_strName, unsigned int p_lineNum)
	:	m_value			(NULL),
		m_function		(NULL),
		m_strFileName	(p_strName),
		m_userFunction	(NULL),
		m_numUses		(0),
		m_type			(NULL),
		m_ref			(false),
		m_createdAtLine	(p_lineNum)
{
	sm_numNodes ++;
//	std::cout << "Create Node ( " << sm_numNodes <<  " ) -> " << this << std::endl;
}

ScriptNode :: ~ScriptNode()
{
//	std::cout << "Delete Node ( " << sm_numNodes <<  " ) -> " << this << std::endl;
	sm_numNodes --;
}

void ScriptNode :: _reinit()
{
	if(m_value != NULL) delete m_value;
	m_function = NULL;
	m_userFunction = NULL;
	m_numUses = 0;
	m_type = NULL;
	m_ref = false;
	m_createdAtLine = 0;
}

void ScriptNode :: _delete()
{
	if (m_value != NULL && ! m_ref)
	{
		delete m_value;
	}
	m_value = NULL;

	m_childs.clear();
}

NodeValue<NodeValueBaseArray> :: ~NodeValue() { vector::deleteAll( m_value); }

void NodeValue<NodeValueBaseArray>::CopyDeepValue( NodeValueBase * p_origin)
{
	vector::deleteAll( m_value);

	NodeValue<NodeValueBaseArray> * l_value = static_cast<NodeValue<NodeValueBaseArray>*>( p_origin);
	_times( l_value->m_value.size())
	{
		m_value.push_back( l_value->m_value[i]->clone());
	}
}

void NodeValue<StructInstance *>::CopyDeepValue( NodeValueBase * p_origin)
{
	NodeValue<StructInstance*> * l_value = static_cast<NodeValue<StructInstance*>*>( p_origin);
	m_value->Copy( l_value->m_value);
}

NodeValueBase *  NodeValue<StructInstance *>::clone()
{
	NodeValue <StructInstance*> * l_temp = new NodeValue <StructInstance*> ();
	l_temp->m_value = new StructInstance( m_value->m_definition);
	l_temp->CopyDeepValue( this);
	return l_temp;
}

void ScriptNode :: Use()
{
	ScriptNodeArray l_callStack;
	_use( l_callStack);
}

void ScriptNode :: Delete()
{
	ScriptNodeArray l_callStack;
	_deleteDetails( l_callStack);
}

bool ScriptNode :: IsBaseType( VariableType * p_type)const
{
//	genlib_assert( m_type != NULL);
//	genlib_assert( p_type != NULL);

	return m_type->GetBase() == p_type->GetBase();
}

void ScriptNode :: _deleteDetails( ScriptNodeArray & p_callStack)
{
	if (vector::has( p_callStack, this))
	{
		return;
	}

	if ( ! m_childs.empty())
	{
		p_callStack.push_back( this);
		vector::cycle( m_childs, & ScriptNode::_deleteDetails, p_callStack);
		p_callStack.pop_back();
	}

	-- m_numUses;

	if (m_numUses == 0)
	{
		ScriptCompiler::ReleaseScriptNode( this);
//		delete this;
	}
}

void ScriptNode :: _use( ScriptNodeArray & p_callStack)
{
	if (vector::has( p_callStack, this))
	{
		return;
	}

	if (m_userFunction != NULL && ! m_userFunction->m_initialised)
	{
		p_callStack.push_back( this);
		m_userFunction->DeferUse( p_callStack);
		p_callStack.pop_back();
	}
	else if ( ! m_childs.empty())
	{
		p_callStack.push_back( this);
		vector::cycle( m_childs, & ScriptNode::_use, p_callStack);
		p_callStack.pop_back();
	}
	++ m_numUses;
}

void ScriptNode :: CopyNode( ScriptNode * p_node)
{
	m_childs = p_node->m_childs;
	m_createdAtLine = p_node->m_createdAtLine;
	m_strFileName = p_node->m_strFileName;
	m_function = p_node->m_function;

	m_value = p_node->m_value;

	p_node->m_value = NULL;
	p_node->m_childs.clear();

	if (m_userFunction != NULL && m_userFunction->m_node == this)
	{
		for (size_t i = 0 ; i < m_userFunction->m_deferredUses.size() ; i ++)
		{
			vector::cycle( m_childs, & ScriptNode::_use, m_userFunction->m_deferredUses[i]);
		}
		m_userFunction->m_initialised = true;
	}
}

void ScriptNode :: ExecChild( size_t p_index)
{
	if (m_childs.size() <= p_index)
	{
		return;
	}

	ScriptNode * l_node = m_childs[p_index];

	if (l_node->HasFunction())
	{
		l_node->Execute();
	}
}

VariableBaseType ScriptNode :: GetBaseType()const
{
	return m_type->GetBase();
	/*
	if (m_value == NULL)
	{
		return EMVT_NULL;
	}
	return m_value->m_type;
	*/
}

NodeValueBase * ScriptNode :: CreateNodeValue( VariableType * p_type)
{
	switch (p_type->GetBase())
	{
		case EMVT_CODE:			return NULL;
		case EMVT_NULL:			return NULL;
		case EMVT_NULLVALUE:	return NULL;
		case EMVT_INT:			return _createValueBase <int>					( 0);
		case EMVT_BOOL:			return _createValueBase <bool>					( false);
		case EMVT_REAL:			return _createValueBase <float>					( 0.0);
		case EMVT_VEC2F:		return _createValueBase <Point2D<float> >		( Point2D<float>( 0, 0));
		case EMVT_VEC3F:		return _createValueBase <Point3D<float> >		( Point3D<float>( 0, 0, 0));
		case EMVT_VEC4F:		return _createValueBase <Point4D<float> >		( Point4D<float>( 0, 0, 0, 0));
		case EMVT_VEC2I:		return _createValueBase <Point2D<int> >			( Point2D<int>( 0, 0));
		case EMVT_VEC3I:		return _createValueBase <Point3D<int> >			( Point3D<int>( 0, 0, 0));
		case EMVT_VEC4I:		return _createValueBase <Point4D<int> >			( Point4D<int>( 0, 0, 0, 0));
		case EMVT_VEC2B:		return _createValueBase <Point2Bool>			( Point2Bool( false, false));
		case EMVT_VEC3B:		return _createValueBase <Point3Bool>			( Point3Bool( false, false, false));
		case EMVT_VEC4B:		return _createValueBase <Point4Bool>			( Point4Bool( false, false, false, false));
		case EMVT_MAT2:			return _createValueBase <Matrix2<float> >		( Matrix2<float>());
		case EMVT_MAT3:			return _createValueBase <Matrix3<float> >		( Matrix3<float>());
		case EMVT_MAT4:			return _createValueBase <Matrix4<float> >		( Matrix4<float>());
		case EMVT_ARRAY:		return _createValueBase <NodeValueBaseArray>	();
		case EMVT_STRUCT:		return _createValueBase <StructInstance *>		(new StructInstance( p_type));
	}
	return NULL;
}

void ScriptNode :: SetType( VariableType * p_type)
{
	if (m_value == NULL)
	{
		m_type = p_type;
		m_value = CreateNodeValue( p_type);
	}
}

void ScriptNode :: CopyValue( ScriptNode * p_originalNode)
{
	if (m_value != NULL && p_originalNode != NULL && p_originalNode->m_value != NULL)
	{
		m_value->CopyDeepValue( p_originalNode->m_value);
	}
}

void ScriptNode :: CopyValue_Deep( ScriptNode * p_originalNode)
{
	if (m_value != NULL && p_originalNode != NULL && p_originalNode->m_value != NULL)
	{
		m_value->CopyDeepValue( p_originalNode->m_value);
	}
}

void ScriptNode :: CopyValue_Ref( NodeValueBase * p_value)
{
	if (m_value != NULL && ! m_ref)
	{
		delete m_value;
	}
	m_value = p_value;
	m_ref = true;
}
