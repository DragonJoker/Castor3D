#include "Castor3D/Scene/SceneNode.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Scene/Animation/SceneNodeAnimation.hpp"

namespace castor3d
{
	uint64_t SceneNode::Count = 0;
	uint64_t SceneNode::CurrentId = 0;

	SceneNode::SceneNode( castor::String const & name
		, Scene & scene )
		: Animable{ *scene.getEngine() }
		, castor::Named{ name }
		, m_scene{ scene }
		, m_id{ CurrentId++ }
		, m_displayable{ name == Scene::RootNode }
	{
		if ( m_name.empty() )
		{
			m_name = cuT( "SceneNode_%d" );
			m_name += castor::string::toString( Count );
		}

		Count++;
	}

	SceneNode::~SceneNode()
	{
		Count--;
		auto parent = getParent();

		if ( parent )
		{
			parent->detachChild( shared_from_this() );
		}

		detachChildren();
		cleanupAnimations();
	}

	void SceneNode::update()
	{
		doComputeMatrix();
		{
			auto lock( castor::makeUniqueLock( m_childrenLock ) );

			for ( auto it : m_children )
			{
				auto child = it.second.lock();

				if ( child )
				{
					child->update();
				}
			}
		}
	}

	void SceneNode::attachObject( MovableObject & object )
	{
		object.detach();
		{
			auto lock( castor::makeUniqueLock( m_objectsLock ) );
			m_objects.push_back( object );
		}
		object.attachTo( *this );
	}

	void SceneNode::detachObject( MovableObject & object )
	{
		auto lock( castor::makeUniqueLock( m_objectsLock ) );
		auto it = std::find_if( m_objects.begin()
			, m_objects.end()
			, [&object]( std::reference_wrapper< MovableObject > obj )
			{
				return obj.get().getName() == object.getName();
			} );

		if ( it != m_objects.end() )
		{
			m_objects.erase( it );
		}
	}

	void SceneNode::attachTo( SceneNode & node )
	{
		auto old = getParent();

		if ( old )
		{
			m_parent = nullptr;
			old->detachChild( shared_from_this() );
		}

		m_parent = &node;

		if ( m_parent )
		{
			m_displayable = m_parent->m_displayable;
			m_parent->addChild( shared_from_this() );
			m_mtxChanged = true;
		}
	}

	void SceneNode::detach()
	{
		auto parent = getParent();

		if ( parent )
		{
			m_displayable = false;
			m_parent = nullptr;
			parent->detachChild( shared_from_this() );
			m_mtxChanged = true;
		}
	}

	bool SceneNode::hasChild( castor::String const & name )
	{
		auto lock( castor::makeUniqueLock( m_childrenLock ) );
		bool found = false;

		if ( m_children.find( name ) == m_children.end() )
		{
			found = m_children.end() != std::find_if( m_children.begin()
				, m_children.end()
				, [&name]( std::pair< castor::String, SceneNodeWPtr > p_pair )
				{
					return p_pair.second.lock()->hasChild( name );
				} );
		}

		return found;
	}

	void SceneNode::addChild( SceneNodeSPtr child )
	{
		auto lock( castor::makeUniqueLock( m_childrenLock ) );
		auto name = child->getName();

		if ( m_children.find( name ) == m_children.end() )
		{
			m_children.insert( std::make_pair( name, child ) );
		}
		else
		{
			log::warn << m_name << cuT( " - Can't add SceneNode " ) << name << cuT( " - Already in childs" ) << std::endl;
		}
	}

	void SceneNode::detachChild( SceneNodeSPtr child )
	{
		if ( child )
		{
			detachChild( child->getName() );
		}
		else
		{
			log::warn << m_name << cuT( " - Can't remove SceneNode - Null pointer given" ) << std::endl;
		}
	}

	void SceneNode::detachChild( castor::String const & childName )
	{
		auto lock( castor::makeUniqueLock( m_childrenLock ) );
		auto it = m_children.find( childName );

		if ( it != m_children.end() )
		{
			SceneNodeSPtr current = it->second.lock();
			m_children.erase( it );

			if ( current )
			{
				current->detach();
			}
		}
		else
		{
			log::warn << m_name << cuT( " - Can't remove SceneNode " ) << childName << cuT( " - Not in childs" ) << std::endl;
		}
	}

	void SceneNode::detachChildren()
	{
		SceneNodeMap flush;
		{
			auto lock( castor::makeUniqueLock( m_childrenLock ) );
			std::swap( flush, m_children );
		}

		for ( auto it : flush )
		{
			SceneNodeSPtr current = it.second.lock();

			if ( current )
			{
				current->detach();
			}
		}
	}

	void SceneNode::yaw( castor::Angle const & angle )
	{
		rotate( castor::Quaternion::fromAxisAngle( castor::Point3d( 0.0, 1.0, 0.0 ), angle ) );
	}

	void SceneNode::pitch( castor::Angle const & angle )
	{
		rotate( castor::Quaternion::fromAxisAngle( castor::Point3d( 1.0, 0.0, 0.0 ), angle ) );
	}

	void SceneNode::roll( castor::Angle const & angle )
	{
		rotate( castor::Quaternion::fromAxisAngle( castor::Point3d( 0.0, 0.0, 1.0 ), angle ) );
	}

	void SceneNode::rotate( castor::Quaternion const & orientation )
	{
		m_orientation *= orientation;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::translate( castor::Point3f const & position )
	{
		m_position += position;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::scale( castor::Point3f const & scale )
	{
		m_scale *= scale;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	SceneNodeAnimation & SceneNode::createAnimation( castor::String const & name )
	{
		if ( !hasAnimation( name ) )
		{
			addAnimation( std::make_unique< SceneNodeAnimation >( *this, name ) );
		}

		return doGetAnimation< SceneNodeAnimation >( name );
	}

	void SceneNode::removeAnimation( castor::String const & name )
	{
		if ( hasAnimation( name ) )
		{
			doRemoveAnimation( name );
		}
	}

	void SceneNode::setOrientation( castor::Quaternion const & orientation )
	{
		m_orientation = orientation;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::setPosition( castor::Point3f const & position )
	{
		m_position = position;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::setScale( castor::Point3f const & scale )
	{
		m_scale = scale;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	castor::Point3f SceneNode::getDerivedPosition()const
	{
		castor::Point3f result( m_position );
		auto parent = getParent();

		if ( parent )
		{
			result = castor::matrix::getTransformed( parent->getDerivedTransformationMatrix(), m_position );
		}

		return result;
	}

	castor::Quaternion SceneNode::getDerivedOrientation()const
	{
		castor::Quaternion result( m_orientation );
		auto parent = getParent();

		if ( parent )
		{
			result  = result * parent->getDerivedOrientation();
		}

		return result;
	}

	castor::Point3f SceneNode::getDerivedScale()const
	{
		castor::Point3f result( m_scale );
		auto parent = getParent();

		if ( parent )
		{
			result *= parent->getDerivedScale();
		}

		return result;
	}

	castor::Matrix4x4f const & SceneNode::getTransformationMatrix()const
	{
		return m_transform;
	}

	castor::Matrix4x4f const & SceneNode::getDerivedTransformationMatrix()const
	{
		return m_derivedTransform;
	}

	void SceneNode::setVisible( bool visible )
	{
		if ( m_visible != visible )
		{
			getScene()->setChanged();
			m_visible = visible;
		}
	}

	bool SceneNode::isVisible()const
	{
		auto parent = m_parent;
		return m_visible && ( parent ? parent->isVisible() : true );
	}

	SceneNode::SceneNodeMap const & SceneNode::getChildren()const
	{
		auto lock( castor::makeUniqueLock( m_childrenLock ) );
		return m_children;
	}

	SceneNodeSPtr SceneNode::getChild( castor::String const & name )const
	{
		auto lock( castor::makeUniqueLock( m_childrenLock ) );
		return ( m_children.find( name ) != m_children.end() ? m_children.find( name )->second.lock() : nullptr );
	}

	SceneNode::MovableArray const & SceneNode::getObjects()const
	{
		auto lock( castor::makeUniqueLock( m_objectsLock ) );
		return m_objects;
	}

	void SceneNode::doComputeMatrix()
	{
		if ( m_mtxChanged )
		{
			m_derivedMtxChanged = true;
			castor::matrix::setTransform( m_transform, m_position, m_scale, m_orientation );
			m_mtxChanged = false;
		}

		if ( m_derivedMtxChanged )
		{
			auto parent = getParent();

			if ( parent )
			{
				m_derivedTransform = parent->getDerivedTransformationMatrix() * m_transform;
			}
			else
			{
				m_derivedTransform = m_transform;
			}

			m_derivedMtxChanged = false;
			onChanged( *this );
		}
	}

	void SceneNode::doUpdateChildsDerivedTransform()
	{
		auto lock( castor::makeUniqueLock( m_childrenLock ) );

		for ( auto it : m_children )
		{
			SceneNodeSPtr current = it.second.lock();

			if ( current )
			{
				current->doUpdateChildsDerivedTransform();
				current->m_derivedMtxChanged = true;
			}
		}
	}
}
