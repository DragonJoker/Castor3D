#include "PrecompiledHeader.h"

#include "animation/Module_Animation.h"

#include "geometry/Module_Geometry.h"
#include "scene/Module_Scene.h"
#include "animation/KeyFrame.h"
#include "geometry/primitives/MovableObject.h"

using namespace Castor3D;


MovingObject :: MovingObject()
	:	m_object( NULL)
{
}

MovingObject :: MovingObject( MovableObjectPtr p_object,
							  const Point3r & p_translate, const Point3r & p_scale,
							  const Quaternion & p_rotate)
	:	m_translate( p_translate),
		m_scale( p_scale),
		m_rotate( p_rotate),
		m_object( p_object)
{
}

MovingObject :: ~MovingObject()
{
}

void MovingObject :: Update( real p_percent, real p_weight)
{
	m_object->SetOrientation( (m_object->GetOrientation() * m_rotate.Slerp( Quaternion::Quat_Identity, 1.0f - p_percent, true)) * p_weight);
	m_object->SetPosition( m_object->GetPosition() + (m_translate * p_percent));
}

KeyFrame :: KeyFrame( real p_from, real p_to)
	:	m_to	( p_to),
		m_from	( p_from)
{
}

KeyFrame :: ~KeyFrame()
{
//	map::deleteAll( m_toMove);
}

void KeyFrame :: Update( real p_time, real p_weight)
{
	real l_percent = (p_time - m_from) / (m_to - m_from);
	MovingObjectPtrStrMap::const_iterator l_endIt = m_toMove.end();
	for (MovingObjectPtrStrMap::iterator l_it = m_toMove.begin() ; l_it != l_endIt ; ++l_it)
	{
		l_it->second->Update( l_percent, p_weight);
	}
}

bool KeyFrame :: Write( File & p_file)const
{
	return true;
}

bool KeyFrame :: Read( File & p_file)
{
	return true;
}

void KeyFrame :: AddMovingObject( MovableObjectPtr p_object,
								  const Point3r & p_translate,
								  const Point3r & p_scale,
								  const Quaternion & p_rotate)
{
	if ( ! map::has( m_toMove, p_object->GetName()))
	{
		MovingObjectPtr l_object = new MovingObject( p_object, p_translate,
													p_scale, p_rotate);
		m_toMove.insert( MovingObjectPtrStrMap::value_type( p_object->GetName(), l_object));
	}
}

void KeyFrame :: RemoveMovingObject( const String & p_name)
{
	MovingObjectPtr l_pMovable;
	map::eraseValue( m_toMove, p_name, l_pMovable);
	l_pMovable.reset();
}