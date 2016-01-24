#include "MovingObject.hpp"

#include "MovableObject.hpp"
#include "SceneNode.hpp"

using namespace Castor;

namespace Castor3D
{
	MovingObject::MovingObject()
		: MovingObjectBase()
	{
	}

	MovingObject::~MovingObject()
	{
	}

	String const & MovingObject::GetName()const
	{
		return GetObject()->GetName();
	}

	void MovingObject::DoApply()
	{
		//MovableObjectSPtr l_pObject = GetObject();

		//if ( l_pObject )
		//{
		//	Point3r l_ptScale;
		//	Point3r l_ptTranslate;
		//	Quaternion l_qRotate;
		//	matrix::get_scale( m_mtxTransformations, l_ptScale );
		//	matrix::get_translate( m_mtxTransformations, l_ptTranslate );
		//	matrix::get_rotate( m_mtxTransformations, l_qRotate );
		//	GetObject()->GetParent()->Scale( l_ptScale );
		//	GetObject()->GetParent()->Translate( l_ptTranslate );
		//	GetObject()->GetParent()->Rotate( l_qRotate );
		//}
	}
}
