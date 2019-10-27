#include "Castor3D/Scene/SceneNode.hpp"

#include "Castor3D/Engine.hpp"

#include "Castor3D/Scene/Camera.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include "Castor3D/Mesh/Mesh.hpp"

using namespace castor;

namespace castor3d
{
	namespace
	{
		bool isIgnored( castor::String const & name )
		{
			return name == cuT( "RootNode" )
				|| name == cuT( "ObjectRootNode" )
				|| name == cuT( "CameraRootNode" )
				|| name.find( cuT( "_REye" ) ) != String::npos
				|| name.find( cuT( "_LEye" ) ) != String::npos;
		}

		bool isIgnored( SceneNode const & node )
		{
			return isIgnored( node.getName() );
		}
	}

	SceneNode::TextWriter::TextWriter( String const & tabs )
		: castor::TextWriter< SceneNode >{ tabs }
	{
	}

	bool SceneNode::TextWriter::operator()( SceneNode const & node, TextFile & file )
	{
		bool result = !isIgnored( node );

		if ( result )
		{
			Logger::logInfo( m_tabs + cuT( "Writing Node " ) + node.getName() );
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

			if ( result && node.getOrientation() != Quaternion::identity() )
			{
				result = file.print( 256, cuT( "%s\torientation " ), m_tabs.c_str() ) > 0
						   && Quaternion::TextWriter( String() )( node.getOrientation(), file )
						   && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< SceneNode >::checkError( result, "Node orientation" );
			}

			if ( result && node.getPosition() != Point3r{} )
			{
				result = file.print( 256, cuT( "%s\tposition " ), m_tabs.c_str() ) > 0
						   && Point3r::TextWriter( String() )( node.getPosition(), file )
						   && file.writeText( cuT( "\n" ) ) > 0;
				castor::TextWriter< SceneNode >::checkError( result, "Node position" );
			}

			if ( result && node.getScale() != Point3r{ 1, 1, 1 } )
			{
				result = file.print( 256, cuT( "%s\tscale " ), m_tabs.c_str() ) > 0
						   && Point3r::TextWriter( String() )( node.getScale(), file )
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
				if ( result && !isIgnored( it.first ) )
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

	SceneNode::SceneNode( String const & name, Scene & scene )
		: OwnedBy< Scene >{ scene }
		, Named{ name }
		, m_id{ CurrentId++ }
		, m_displayable{ name == cuT( "RootNode" ) }
	{
		if ( m_name.empty() )
		{
			m_name = cuT( "SceneNode_%d" );
			m_name += string::toString( Count );
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

	bool SceneNode::hasChild( String const & name )
	{
		bool found = false;

		if ( m_children.find( name ) == m_children.end() )
		{
			found = m_children.end() != std::find_if( m_children.begin()
				, m_children.end()
				, [&name]( std::pair< String, SceneNodeWPtr > p_pair )
				{
					return p_pair.second.lock()->hasChild( name );
				} );
		}

		return found;
	}

	void SceneNode::addChild( SceneNodeSPtr child )
	{
		String name = child->getName();

		if ( m_children.find( name ) == m_children.end() )
		{
			m_children.insert( std::make_pair( name, child ) );
		}
		else
		{
			Logger::logWarning( m_name + cuT( " - Can't add SceneNode " ) + name + cuT( " - Already in childs" ) );
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
			Logger::logWarning( m_name + cuT( " - Can't remove SceneNode - Null pointer given" ) );
		}
	}

	void SceneNode::detachChild( String const & childName )
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
			Logger::logWarning( m_name + cuT( " - Can't remove SceneNode " ) + childName + cuT( " - Not in childs" ) );
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

	void SceneNode::yaw( Angle const & angle )
	{
		rotate( Quaternion::fromAxisAngle( Point3d( 0.0, 1.0, 0.0 ), angle ) );
	}

	void SceneNode::pitch( Angle const & angle )
	{
		rotate( Quaternion::fromAxisAngle( Point3d( 1.0, 0.0, 0.0 ), angle ) );
	}

	void SceneNode::roll( Angle const & angle )
	{
		rotate( Quaternion::fromAxisAngle( Point3d( 0.0, 0.0, 1.0 ), angle ) );
	}

	void SceneNode::rotate( Quaternion const & orientation )
	{
		m_orientation *= orientation;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::translate( Point3r const & position )
	{
		m_position += position;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::scale( Point3r const & scale )
	{
		m_scale *= scale;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::setOrientation( Quaternion const & orientation )
	{
		m_orientation = orientation;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::setPosition( Point3r const & position )
	{
		m_position = position;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::setScale( Point3r const & scale )
	{
		m_scale = scale;
		doUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	Point3r SceneNode::getDerivedPosition()const
	{
		Point3r result( m_position );
		auto parent = getParent();

		if ( parent )
		{
			result = matrix::getTransformed( parent->getDerivedTransformationMatrix(), m_position );
		}

		return result;
	}

	Quaternion SceneNode::getDerivedOrientation()const
	{
		Quaternion result( m_orientation );
		auto parent = getParent();

		if ( parent )
		{
			result  = result * parent->getDerivedOrientation();
		}

		return result;
	}

	Point3r SceneNode::getDerivedScale()const
	{
		Point3r result( m_scale );
		auto parent = getParent();

		if ( parent )
		{
			result *= parent->getDerivedScale();
		}

		return result;
	}

	Matrix4x4r const & SceneNode::getTransformationMatrix()const
	{
		return m_transform;
	}

	Matrix4x4r const & SceneNode::getDerivedTransformationMatrix()const
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
			matrix::setTransform( m_transform, m_position, m_scale, m_orientation );
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
