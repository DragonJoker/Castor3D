#include "SceneNode.hpp"

#include "Engine.hpp"

#include "Camera.hpp"
#include "Scene.hpp"

#include "Mesh/Mesh.hpp"

using namespace Castor;

namespace Castor3D
{
	SceneNode::TextWriter::TextWriter( String const & tabs )
		: Castor::TextWriter< SceneNode >{ tabs }
	{
	}

	bool SceneNode::TextWriter::operator()( SceneNode const & node, TextFile & file )
	{
		bool result = node.GetName() == cuT( "RootNode" )
			|| node.GetName() == cuT( "ObjectRootNode" )
			|| node.GetName() == cuT( "CameraRootNode" );

		if ( node.GetName() != cuT( "RootNode" )
			 && node.GetName() != cuT( "ObjectRootNode" )
			 && node.GetName() != cuT( "CameraRootNode" ) )
		{
			Logger::LogInfo( m_tabs + cuT( "Writing Node " ) + node.GetName() );
			result = file.WriteText( cuT( "\n" ) + m_tabs + cuT( "scene_node \"" ) + node.GetName() + cuT( "\"\n" ) ) > 0
					   && file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			Castor::TextWriter< SceneNode >::CheckError( result, "Node name" );

			if ( result
				&& node.GetParent()
				&& node.GetParent()->GetName() != cuT( "RootNode" )
				&& node.GetParent()->GetName() != cuT( "ObjectRootNode" )
				&& node.GetParent()->GetName() != cuT( "CameraRootNode" ) )
			{
				result = file.WriteText( m_tabs + cuT( "\tparent \"" ) + node.GetParent()->GetName() + cuT( "\"\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( result, "Node parent name" );
			}

			if ( result )
			{
				result = file.Print( 256, cuT( "%s\torientation " ), m_tabs.c_str() ) > 0
						   && Quaternion::TextWriter( String() )( node.GetOrientation(), file )
						   && file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( result, "Node orientation" );
			}

			if ( result )
			{
				result = file.Print( 256, cuT( "%s\tposition " ), m_tabs.c_str() ) > 0
						   && Point3r::TextWriter( String() )( node.GetPosition(), file )
						   && file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( result, "Node position" );
			}

			if ( result )
			{
				result = file.Print( 256, cuT( "%s\tscale " ), m_tabs.c_str() ) > 0
						   && Point3r::TextWriter( String() )( node.GetScale(), file )
						   && file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( result, "Node scale" );
			}

			if ( result )
			{
				result = file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( result, "Node end" );
			}
		}

		if ( result )
		{
			for ( auto const & it : node.m_children )
			{
				if ( result
					 && it.first.find( cuT( "_REye" ) ) == String::npos
					 && it.first.find( cuT( "_LEye" ) ) == String::npos )
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

	SceneNode::SceneNode( String const & name, Scene & scene )
		: OwnedBy< Scene >{ scene }
		, Named{ name }
		, m_displayable{ name == cuT( "RootNode" ) }
	{
		if ( m_name.empty() )
		{
			m_name = cuT( "SceneNode_%d" );
			m_name += string::to_string( Count );
		}

		Count++;
	}

	SceneNode::~SceneNode()
	{
		Count--;
		SceneNodeSPtr parent = GetParent();

		if ( parent )
		{
			parent->DetachChild( shared_from_this() );
		}

		DetachAllChilds();
	}

	void SceneNode::Update()
	{
		DoComputeMatrix();

		for ( auto it : m_children )
		{
			auto child = it.second.lock();

			if ( child )
			{
				child->Update();
			}
		}
	}

	void SceneNode::AttachObject( MovableObject & object )
	{
		object.Detach();
		m_objects.push_back( object );
		object.AttachTo( shared_from_this() );
	}

	void SceneNode::DetachObject( MovableObject & object )
	{
		auto it = std::find_if( m_objects.begin(), m_objects.end(), [&object]( std::reference_wrapper< MovableObject > obj )
		{
			return obj.get().GetName() == object.GetName();
		} );

		if ( it != m_objects.end() )
		{
			m_objects.erase( it );
			object.AttachTo( nullptr );
		}
	}

	void SceneNode::AttachTo( SceneNodeSPtr parent )
	{
		SceneNodeSPtr old = GetParent();

		if ( old )
		{
			m_parent.reset();
			old->DetachChild( shared_from_this() );
			old.reset();
		}

		m_parent = parent;

		if ( parent )
		{
			m_displayable = parent->m_displayable;
			parent->AddChild( shared_from_this() );
			m_mtxChanged = true;
		}
	}

	void SceneNode::Detach()
	{
		SceneNodeSPtr parent = GetParent();

		if ( parent )
		{
			m_displayable = false;
			m_parent.reset();
			parent->DetachChild( shared_from_this() );
			m_mtxChanged = true;
		}
	}

	bool SceneNode::HasChild( String const & name )
	{
		bool found = false;

		if ( m_children.find( name ) == m_children.end() )
		{
			found = m_children.end() != std::find_if( m_children.begin()
				, m_children.end()
				, [&name]( std::pair< String, SceneNodeWPtr > p_pair )
				{
					return p_pair.second.lock()->HasChild( name );
				} );
		}

		return found;
	}

	void SceneNode::AddChild( SceneNodeSPtr child )
	{
		String name = child->GetName();

		if ( m_children.find( name ) == m_children.end() )
		{
			m_children.insert( std::make_pair( name, child ) );
		}
		else
		{
			Logger::LogWarning( m_name + cuT( " - Can't add SceneNode " ) + name + cuT( " - Already in childs" ) );
		}
	}

	void SceneNode::DetachChild( SceneNodeSPtr child )
	{
		if ( child )
		{
			DetachChild( child->GetName() );
		}
		else
		{
			Logger::LogWarning( m_name + cuT( " - Can't remove SceneNode - Null pointer given" ) );
		}
	}

	void SceneNode::DetachChild( String const & childName )
	{
		auto it = m_children.find( childName );

		if ( it != m_children.end() )
		{
			SceneNodeSPtr current = it->second.lock();
			m_children.erase( it );

			if ( current )
			{
				current->Detach();
			}
		}
		else
		{
			Logger::LogWarning( m_name + cuT( " - Can't remove SceneNode " ) + childName + cuT( " - Not in childs" ) );
		}
	}

	void SceneNode::DetachAllChilds()
	{
		SceneNodePtrStrMap flush;
		std::swap( flush, m_children );

		for ( auto it : flush )
		{
			SceneNodeSPtr current = it.second.lock();

			if ( current )
			{
				current->Detach();
			}
		}
	}

	void SceneNode::Yaw( Angle const & angle )
	{
		Rotate( Quaternion::from_axis_angle( Point3d( 0.0, 1.0, 0.0 ), angle ) );
	}

	void SceneNode::Pitch( Angle const & angle )
	{
		Rotate( Quaternion::from_axis_angle( Point3d( 1.0, 0.0, 0.0 ), angle ) );
	}

	void SceneNode::Roll( Angle const & angle )
	{
		Rotate( Quaternion::from_axis_angle( Point3d( 0.0, 0.0, 1.0 ), angle ) );
	}

	void SceneNode::Rotate( Quaternion const & orientation )
	{
		m_orientation *= orientation;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::Translate( Point3r const & position )
	{
		m_position += position;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::Scale( Point3r const & scale )
	{
		m_scale *= scale;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::SetOrientation( Quaternion const & orientation )
	{
		m_orientation = orientation;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::SetPosition( Point3r const & position )
	{
		m_position = position;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::SetScale( Point3r const & scale )
	{
		m_scale = scale;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	Point3r SceneNode::GetDerivedPosition()const
	{
		Point3r result( m_position );
		auto parent = GetParent();

		if ( parent )
		{
			result = matrix::get_transformed( parent->GetDerivedTransformationMatrix(), m_position );
		}

		return result;
	}

	Quaternion SceneNode::GetDerivedOrientation()const
	{
		Quaternion result( m_orientation );
		auto parent = GetParent();

		if ( parent )
		{
			result  = result * parent->GetDerivedOrientation();
		}

		return result;
	}

	Point3r SceneNode::GetDerivedScale()const
	{
		Point3r result( m_scale );
		auto parent = GetParent();

		if ( parent )
		{
			result *= parent->GetDerivedScale();
		}

		return result;
	}

	Matrix4x4r const & SceneNode::GetTransformationMatrix()const
	{
		return m_transform;
	}

	Matrix4x4r const & SceneNode::GetDerivedTransformationMatrix()const
	{
		return m_derivedTransform;
	}

	void SceneNode::SetVisible( bool visible )
	{
		if ( m_visible != visible )
		{
			GetScene()->SetChanged();
			m_visible = visible;
		}
	}

	bool SceneNode::IsVisible()const
	{
		auto parent = m_parent.lock();
		return m_visible && ( parent ? parent->IsVisible() : true );
	}

	void SceneNode::DoComputeMatrix()
	{
		if ( m_mtxChanged )
		{
			m_derivedMtxChanged = true;
			point::normalise( m_orientation );
			matrix::set_transform( m_transform, m_position, m_scale, m_orientation );
			m_mtxChanged = false;
		}

		if ( m_derivedMtxChanged )
		{
			SceneNodeSPtr parent = GetParent();

			if ( parent )
			{
				m_derivedTransform = parent->GetDerivedTransformationMatrix() * m_transform;
			}
			else
			{
				m_derivedTransform = m_transform;
			}

			m_derivedMtxChanged = false;
			onChanged( *this );
		}
	}

	void SceneNode::DoUpdateChildsDerivedTransform()
	{
		for ( auto it : m_children )
		{
			SceneNodeSPtr current = it.second.lock();

			if ( current )
			{
				current->DoUpdateChildsDerivedTransform();
				current->m_derivedMtxChanged = true;
			}
		}
	}
}
