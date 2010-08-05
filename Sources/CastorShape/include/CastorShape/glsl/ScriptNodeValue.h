#ifndef ___SCRIPT_NODE_VALUE_H___
#define ___SCRIPT_NODE_VALUE_H___

#include "Module_Script.h"

#include "MapIterator.h"

namespace CastorShape
{
	template<typename T>
	struct NodeTypeBinding;

	class NodeValueBase 
	{
	public:
		VariableBaseType m_type;

	public:
		NodeValueBase( VariableBaseType p_type)
			:	m_type (p_type)
		{}
		virtual ~NodeValueBase(){};

	public:
		inline bool IsType( VariableBaseType p_type)const
		{
			return m_type == p_type;
		}

		template <typename T>
		inline bool IsType()const
		{
			return static_cast<VariableBaseType>(NodeTypeBinding<T>::NType) == m_type;
		}

		virtual void CopyValueFrom( NodeValueBase * p_origin) = 0;
		virtual void CopyDeepValue( NodeValueBase * p_origin) = 0;

		virtual NodeValueBase * clone() { return NULL; }
	};
	
	template<typename T>
	class NodeValue : public NodeValueBase
	{
	public:
		T m_value;

	public:
		NodeValue()
			:	NodeValueBase( static_cast<VariableBaseType>( NodeTypeBinding<T>::NType))
		{}

		virtual ~NodeValue() {}

		virtual NodeValueBase * clone()
		{
			NodeValue <T> * l_temp = new NodeValue <T> ();
			l_temp->CopyDeepValue( this);
			return l_temp;
		}

		virtual void CopyDeepValue( NodeValueBase * p_origin){ CopyValueFrom( p_origin); }

		virtual void CopyValueFrom( NodeValueBase * p_origin)
		{
			if (p_origin != NULL)
			{
				m_value = static_cast<NodeValue<T> *>( p_origin)->m_value;
			}
		}
	};


	#define EMUSE_BIND_NODE_VALUE( p_type, p_nodeType)					\
	template<>															\
	struct NodeTypeBinding<p_type>										\
	{																	\
		enum {NType = p_nodeType};										\
	};	

	EMUSE_BIND_NODE_VALUE(	int,							EMVT_INT);
	EMUSE_BIND_NODE_VALUE(	float,							EMVT_REAL);

	EMUSE_BIND_NODE_VALUE(	bool,							EMVT_BOOL);
	EMUSE_BIND_NODE_VALUE(	Point2Bool,						EMVT_VEC2B);
	EMUSE_BIND_NODE_VALUE(	Point3Bool,						EMVT_VEC3B);
	EMUSE_BIND_NODE_VALUE(	Point4Bool,						EMVT_VEC4B);
	EMUSE_BIND_NODE_VALUE(	Point2D<int>,					EMVT_VEC2I);
	EMUSE_BIND_NODE_VALUE(	Point3D<int>,					EMVT_VEC3I);
	EMUSE_BIND_NODE_VALUE(	Point4D<int>,					EMVT_VEC4I);
	EMUSE_BIND_NODE_VALUE(	Point2D<float>,					EMVT_VEC2F);
	EMUSE_BIND_NODE_VALUE(	Point3D<float>,					EMVT_VEC3F);
	EMUSE_BIND_NODE_VALUE(	Point4D<float>,					EMVT_VEC4F);
	EMUSE_BIND_NODE_VALUE(	Matrix2<float>,					EMVT_MAT2);
	EMUSE_BIND_NODE_VALUE(	Matrix3<float>,					EMVT_MAT3);
	EMUSE_BIND_NODE_VALUE(	Matrix4<float>,					EMVT_MAT4);

	EMUSE_BIND_NODE_VALUE(	NodeValueBaseArray,				EMVT_ARRAY);

	EMUSE_BIND_NODE_VALUE(	StructInstance *,				EMVT_STRUCT);
}

#endif
