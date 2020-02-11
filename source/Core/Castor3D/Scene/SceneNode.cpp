#include "Castor3D/Scene/SceneNode.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"

namespace castor3d
{
	namespace
	{
		bool isIgnored( castor::String const & name )
		{
			return name == cuT( "RootNode" )
				|| name == cuT( "ObjectRootNode" )
				|| name == cuT( "CameraRootNode" )
				|| name.find( cuT( "_REye" ) ) != castor::String::npos
				|| name.find( cuT( "_LEye" ) ) != castor::String::npos;
		}

		bool isIgnored( SceneNode const & node )
		{
			return isIgnored( node.getName() );
		}
	}

	SceneNode::TextWriter::TextWriter( castor::String const & tabs )
		: castor::TextWriter< SceneNode >{ tabs }
	{
	}

	bool SceneNode::TextWriter::operator()( SceneNode const & node, castor::TextFile & file )
	{
		bool result = true;

		if ( !isIgnored( node ) )
		{
			castor::Logger::logInfo( m_tabs + cuT( "Writing Node " ) + node.getName() );
			result = file.writeText( cuT( "\n" ) + m_tabs + cuT( "scene_node \"" ) + node.getName() + cuT( "\"\n" ) ) > 0
				&& file.writeText( m_tabs + cuT( "{\n" ) ) > 0;
			castor::TextWriter< SceneNode >::checkError( result, "Node name" );

			if ( result
				&& node.getParent()
				&& !isIgnored( *node.getParent() ) )
			{
				result = file.writeText( m_tabs + cuT( "\tparent \"" ) + node.getParent()->getName() + cuT( "\"\n" ) ) > 0;
				castor::TextWriter< SceneNode >::checkError( result, "Node parent name" );
			}

			if ( result && node.getOrientation() != castor::Quaternion::identity() )
			{
				result = file.print( 256, cuT( "%s\torientation " ), m_tabs.c_str() ) > 0
						   && castor::Quaternion::TextWriter( castor::String() )( node.getOrientation(), file )
						   && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< SceneNode >::checkError( result, "Node orientation" );
			}

			if ( result && node.getPosition() != castor::Point3f{} )
			{
				result = file.print( 256, cuT( "%s\tposition " ), m_tabs.c_str() ) > 0
						   && castor::Point3f::TextWriter( castor::String() )( node.getPosition(), file )
						   && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< SceneNode >::checkError( result, "Node position" );
			}

			if ( result && node.getScale() != castor::Point3f{ 1, 1, 1 } )
			{
				result = file.print( 256, cuT( "%s\tscale " ), m_tabs.c_str() ) > 0
						   && castor::Point3f::TextWriter( castor::String() )( node.getScale(), file )
						   && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< SceneNode >::checkError( result, "Node scale" );
			}

			if ( result )
			{
				result = file.writeText( m_tabs + cuT( "}\n" ) ) > 0;
				castor::TextWriter< SceneNode >::checkError( result, "Node end" );
			}
		}

		if ( result )
		{
			for ( auto const & it : node.m_children )
			{
				if ( result )
				{
					SceneNodeSPtr node = it.second.lock();

					if ( node )
					{
						result = SceneNode::TextWriter{ m_tabs }( *node, file );
					}
				}
			}
		}

		return result;
	}

	//*************************************************************************************************

	uint64_t SceneNode::Count = 0;
	uint64_t SceneNode::CurrentId = 0;

	SceneNode::SceneNode( castor::String const & name, Scene & scene )
		: castor::OwnedBy< Scene >{ scene }
		, castor::Named{ name }
		, m_id{ CurrentId++ }
		, m_displayable{ name == cuT( "RootNode" ) }
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
		SceneNodeSPtr parent = getParent();

		if ( parent )
		{
			parent->detachChild( shared_from_this() );
		}

		detachChildren();
	}

	void SceneNode::update()
	{
		doComputeMatrix();

		for ( auto it : m_children )
		{
			auto child = it.second.lock();

			if ( child )
			{
				child->update();
			}
		}
	}

	void SceneNode::attachObject( MovableObject & object )
	{
		object.detach();
		m_objects.push_back( object );
		object.attachTo( shared_from_this() );
	}

	void SceneNode::detachObject( MovableObject & object )
	{
		auto it = std::find_if( m_objects.begin(), m_objects.end(), [&object]( std::reference_wrapper< MovableObject > obj )
		{
			return obj.get().getName() == object.getName();
		} );

		if ( it != m_objects.end() )
		{
			m_objects.erase( it );
			object.attachTo( nullptr );
		}
	}

	void SceneNode::attachTo( SceneNodeSPtr parent )
	{
		SceneNodeSPtr old = getParent();

		if ( old )
		{
			m_parent.reset();
			old->detachChild( shared_from_this() );
			old.reset();
		}

		m_parent = parent;

		if ( parent )
		{
			m_displayable = parent->m_displayable;
			parent->addChild( shared_from_this() );
			m_mtxChanged = true;
		}
	}

	void SceneNode::detach()
	{
		SceneNodeSPtr parent = getParent();

		if ( parent )
		{
			m_displayable = false;
			m_parent.reset();
			parent->detachChild( shared_from_this() );
			m_mtxChanged = true;
		}
	}

	bool SceneNode::hasChild( castor::String const & name )
	{
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
		auto name = child->getName();

		if ( m_children.find( name ) == m_children.end() )
		{
			m_children.insert( std::make_pair( name, child ) );
		}
		else
		{
			castor::Logger::logWarning( m_name + cuT( " - Can't add SceneNode " ) + name + cuT( " - Already in childs" ) );
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
			castor::Logger::logWarning( m_name + cuT( " - Can't remove SceneNode - Null pointer given" ) );
		}
	}

	void SceneNode::detachChild( castor::String const & childName )
	{
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
			castor::Logger::logWarning( m_name + cuT( " - Can't remove SceneNode " ) + childName + cuT( " - Not in childs" ) );
		}
	}

	void SceneNode::detachChildren()
	{
		SceneNodePtrStrMap flush;
		std::swap( flush, m_children );

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
		auto parent = m_parent.lock();
		return m_visible && ( parent ? parent->isVisible() : true );
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
			SceneNodeSPtr parent = getParent();

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
