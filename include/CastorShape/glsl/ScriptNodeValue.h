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

	typedef Matrix<real, 2, 3> Matrix2x3r;
	typedef Matrix<real, 2, 4> Matrix2x4r;
	typedef Matrix<real, 3, 2> Matrix3x2r;
	typedef Matrix<real, 3, 4> Matrix3x4r;
	typedef Matrix<real, 4, 2> Matrix4x2r;
	typedef Matrix<real, 4, 3> Matrix4x3r;
	typedef Point<bool, 2>	Point2b;
	typedef Point<bool, 3>	Point3b;
	typedef Point<bool, 4>	Point4b;

	EMUSE_BIND_NODE_VALUE(	int,				EMVT_INT);
	EMUSE_BIND_NODE_VALUE(	real,				EMVT_REAL);

	EMUSE_BIND_NODE_VALUE(	bool,				EMVT_BOOL);
	EMUSE_BIND_NODE_VALUE(	Point2b,			EMVT_VEC2B);
	EMUSE_BIND_NODE_VALUE(	Point3b,			EMVT_VEC3B);
	EMUSE_BIND_NODE_VALUE(	Point4b,			EMVT_VEC4B);
	EMUSE_BIND_NODE_VALUE(	Point2i,			EMVT_VEC2I);
	EMUSE_BIND_NODE_VALUE(	Point3i,			EMVT_VEC3I);
	EMUSE_BIND_NODE_VALUE(	Point4i,			EMVT_VEC4I);
	EMUSE_BIND_NODE_VALUE(	Point2r,			EMVT_VEC2F);
	EMUSE_BIND_NODE_VALUE(	Point3r,			EMVT_VEC3F);
	EMUSE_BIND_NODE_VALUE(	Point4r,			EMVT_VEC4F);
	EMUSE_BIND_NODE_VALUE(	Matrix2x2r,			EMVT_MAT2);
	EMUSE_BIND_NODE_VALUE(	Matrix2x3r,			EMVT_MAT2x3);
	EMUSE_BIND_NODE_VALUE(	Matrix2x4r,			EMVT_MAT2x4);
	EMUSE_BIND_NODE_VALUE(	Matrix3x2r,			EMVT_MAT3x2);
	EMUSE_BIND_NODE_VALUE(	Matrix3x3r,			EMVT_MAT3);
	EMUSE_BIND_NODE_VALUE(	Matrix3x4r,			EMVT_MAT3x4);
	EMUSE_BIND_NODE_VALUE(	Matrix4x2r,			EMVT_MAT4x2);
	EMUSE_BIND_NODE_VALUE(	Matrix4x3r,			EMVT_MAT4x3);
	EMUSE_BIND_NODE_VALUE(	Matrix4x4r,			EMVT_MAT4);

	EMUSE_BIND_NODE_VALUE(	NodeValueBaseArray,	EMVT_ARRAY);

	EMUSE_BIND_NODE_VALUE(	StructInstance *,	EMVT_STRUCT);
}

#endif
