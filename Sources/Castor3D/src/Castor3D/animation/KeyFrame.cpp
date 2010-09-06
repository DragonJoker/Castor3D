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



MovingObject :: MovingObject( MovableObject * p_object,
							  const Vector3f & p_translate, const Vector3f & p_scale,
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



void MovingObject :: Update( float p_percent, float p_weight)
{
	m_object->SetOrientation( (m_object->GetOrientation() * m_rotate.Slerp( Quaternion::Quat_Identity, 1.0f - p_percent, true)) * p_weight);
	m_object->SetPosition( m_object->GetPosition() + (m_translate * p_percent));
}



KeyFrame :: KeyFrame( float p_from, float p_to)
	:	m_to	( p_to),
		m_from	( p_from)
{
}



KeyFrame :: ~KeyFrame()
{
	map::deleteAll( m_toMove);
}



void KeyFrame :: Update( float p_time, float p_weight)
{
	float l_percent = (p_time - m_from) / (m_to - m_from);
	MovingObjectStrMap::const_iterator l_endIt = m_toMove.end();
	for (MovingObjectStrMap::iterator l_it = m_toMove.begin() ; l_it != l_endIt ; ++l_it)
	{
		l_it->second->Update( l_percent, p_weight);
	}
}



bool KeyFrame :: Write( FileIO & p_file)const
{
	return true;
}



bool KeyFrame :: Read( FileIO & p_file)
{
	return true;
}



void KeyFrame :: AddMovingObject( MovableObject * p_object,
								  const Vector3f & p_translate,
								  const Vector3f & p_scale,
								  const Quaternion & p_rotate)
{
	if ( ! map::has( m_toMove, p_object->GetName()))
	{
		MovingObject * l_object = new MovingObject( p_object, p_translate,
													p_scale, p_rotate);
		m_toMove.insert( MovingObjectStrMap::value_type( p_object->GetName(), l_object));
	}
}



void KeyFrame :: RemoveMovingObject( const String & p_name)
{
	map::deleteValue( m_toMove, p_name);
}


