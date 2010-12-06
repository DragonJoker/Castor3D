#include "PrecompiledHeaders.h"

#include "glsl/VariableType.h"
#include "glsl/Structure.h"

using namespace CastorShape;
using namespace Castor3D;

VariableTypeManager :: VariableTypeManager()
{
	_times( EMVT_NUM_TYPES)
	{
		_add( static_cast<VariableBaseType>( i));
	}
	_populateBaseTypeNames();
}

VariableTypeManager :: ~VariableTypeManager()
{
	map::deleteAll( m_types);
}

void VariableTypeManager :: _populateBaseTypeNames()
{
	m_baseTypeNames[EMVT_NULL]				 ="void";
	m_baseTypeNames[EMVT_INT]				 = "int";
	m_baseTypeNames[EMVT_REAL]				 = "real";
	m_baseTypeNames[EMVT_BOOL]				 = "boolean";
	m_baseTypeNames[EMVT_VEC2F]				 = "vec2";
	m_baseTypeNames[EMVT_VEC3F]				 = "vec3";
	m_baseTypeNames[EMVT_VEC4F]				 = "vec4";
	m_baseTypeNames[EMVT_VEC2B]				 = "bvec2";
	m_baseTypeNames[EMVT_VEC3B]				 = "bvec3";
	m_baseTypeNames[EMVT_VEC4B]				 = "bvec4";
	m_baseTypeNames[EMVT_VEC2I]				 = "ivec2";
	m_baseTypeNames[EMVT_VEC3I]				 = "ivec3";
	m_baseTypeNames[EMVT_VEC4I]				 = "ivec4";
	m_baseTypeNames[EMVT_MAT2]				 = "mat2";
	m_baseTypeNames[EMVT_MAT3]				 = "mat3";
	m_baseTypeNames[EMVT_MAT4]				 = "mat4";
	m_baseTypeNames[EMVT_SAMPLER1D]			 = "sampler1D";
	m_baseTypeNames[EMVT_SAMPLER2D]			 = "sampler2D";
	m_baseTypeNames[EMVT_SAMPLER3D]			 = "sampler3D";
	m_baseTypeNames[EMVT_SAMPLERCUBE]		 = "samplerCube";
	m_baseTypeNames[EMVT_ARRAY]				 = "array";
	m_baseTypeNames[EMVT_CODE]				 = "code";
	m_baseTypeNames[EMVT_NULLVALUE]			 = "NULL";
	m_baseTypeNames[EMVT_STRUCT]			 = "struct";
	m_baseTypeNames[EMVT_SUB1]				 = "*SUB1*";
	m_baseTypeNames[EMVT_SUB2]				 = "*SUB2*";
//	m_baseTypeNames[EMVT_SUB3]				 = "SUB3";
}

VariableType * VariableTypeManager :: _add( VariableBaseType p_type, VariableType * p_subType1, VariableType * p_subType2, const String & p_hash)
{
	VariableType * l_type = new VariableType( p_type, p_subType1, p_subType2);
	const std::string & l_hash = (p_hash.empty() ? _hash( p_type, p_subType1, p_subType2) : p_hash.char_str());
	m_types.insert( TypeMap::value_type( l_hash, l_type));
	return l_type;
}

std::string VariableTypeManager :: _hash( VariableBaseType p_type, VariableType * p_subType1, VariableType * p_subType2)
{
	unsigned int l_hash[3];
	l_hash[0] = static_cast <unsigned int>( p_type);
	l_hash[1] = reinterpret_cast <unsigned int>( p_subType1);
	l_hash[2] = reinterpret_cast <unsigned int>( p_subType2);
	return std::string( reinterpret_cast<const char *>( &l_hash[0]), sizeof( l_hash));
}

VariableType * VariableTypeManager :: _get( VariableBaseType p_type, VariableType * p_subType1, VariableType * p_subType2)
{
	const std::string & l_hash = _hash( p_type, p_subType1, p_subType2);
	VariableType * l_vt = map::findOrNull(  m_types, l_hash);
	if (l_vt == NULL)
	{
		return _add( p_type, p_subType1, p_subType2, l_hash);
	}
	return l_vt;
}

String VariableType:: GetDesc()
{
	String l_desc = VariableTypeManager::GetBaseTypeName( m_toplevelType);
	if (m_struct != NULL) { l_desc += " " + m_struct->GetName(); }
	if ( ! m_subTypes.empty())
	{
		l_desc += String( " <");
		_times( m_subTypes.size())
		{
			l_desc += m_subTypes[i]->GetDesc() + ",";
		}
		l_desc.erase( l_desc.end() - 1);
		l_desc += String( ">");
	}
	return l_desc;
}