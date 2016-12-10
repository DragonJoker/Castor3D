#include "SceneNode.hpp"

#include "Engine.hpp"
#include "Cache/SceneNodeCache.hpp"

#include "Camera.hpp"
#include "Geometry.hpp"
#include "MovableObject.hpp"
#include "Scene.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Ray.hpp"
#include "Render/RenderPipeline.hpp"
#include "Render/RenderSystem.hpp"

#include <Log/Logger.hpp>
#include <Math/TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	SceneNode::TextWriter::TextWriter( String const & p_tabs )
		: Castor::TextWriter< SceneNode >{ p_tabs }
	{
	}

	bool SceneNode::TextWriter::operator()( SceneNode const & p_node, TextFile & p_file )
	{
		bool l_return = p_node.GetName() == cuT( "RootNode" )
						|| p_node.GetName() == cuT( "ObjectRootNode" )
						|| p_node.GetName() == cuT( "CameraRootNode" );

		if ( p_node.GetName() != cuT( "RootNode" )
			 && p_node.GetName() != cuT( "ObjectRootNode" )
			 && p_node.GetName() != cuT( "CameraRootNode" ) )
		{
			Logger::LogInfo( m_tabs + cuT( "Writing Node " ) + p_node.GetName() );
			l_return = p_file.WriteText( cuT( "\n" ) + m_tabs + cuT( "scene_node \"" ) + p_node.GetName() + cuT( "\"\n" ) ) > 0
					   && p_file.WriteText( m_tabs + cuT( "{\n" ) ) > 0;
			Castor::TextWriter< SceneNode >::CheckError( l_return, "Node name" );

			if ( l_return && p_node.GetParent() )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "\tparent \"" ) + p_node.GetParent()->GetName() + cuT( "\"\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( l_return, "Node parent name" );
			}

			if ( l_return )
			{
				Point3r l_axis;
				Angle l_angle;
				p_node.GetOrientation().to_axis_angle( l_axis, l_angle );
				l_return = p_file.Print( 256, cuT( "%s\torientation " ), m_tabs.c_str() ) > 0
						   && Quaternion::TextWriter( String() )( p_node.GetOrientation(), p_file )
						   && p_file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( l_return, "Node orientation" );
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tposition " ), m_tabs.c_str() ) > 0
						   && Point3r::TextWriter( String() )( p_node.GetPosition(), p_file )
						   && p_file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( l_return, "Node position" );
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "%s\tscale " ), m_tabs.c_str() ) > 0
						   && Point3r::TextWriter( String() )( p_node.GetScale(), p_file )
						   && p_file.WriteText( cuT( "\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( l_return, "Node scale" );
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( m_tabs + cuT( "}\n" ) ) > 0;
				Castor::TextWriter< SceneNode >::CheckError( l_return, "Node end" );
			}
		}

		if ( l_return )
		{
			for ( auto const & l_it : p_node.m_children )
			{
				if ( l_return
					 && l_it.first.find( cuT( "_REye" ) ) == String::npos
					 && l_it.first.find( cuT( "_LEye" ) ) == String::npos )
				{
					SceneNodeSPtr l_node = l_it.second.lock();

					if ( l_node )
					{
						l_return = SceneNode::TextWriter{ m_tabs }( *l_node, p_file );
					}
				}
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	uint64_t SceneNode::Count = 0;

	SceneNode::SceneNode( String const & p_name, Scene & p_scene )
		: OwnedBy< Scene >{ p_scene }
		, Named{ p_name }
		, m_displayable{ p_name == cuT( "RootNode" ) }
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
		SceneNodeSPtr l_parent = GetParent();

		if ( l_parent )
		{
			l_parent->DetachChild( shared_from_this() );
		}

		DetachAllChilds();
	}

	void SceneNode::Update()
	{
		DoComputeMatrix();

		for ( auto l_it : m_children )
		{
			auto l_child = l_it.second.lock();

			if ( l_child )
			{
				l_child->Update();
			}
		}
	}

	void SceneNode::AttachObject( MovableObject & p_object )
	{
		p_object.Detach();
		m_objects.push_back( p_object );
		p_object.AttachTo( shared_from_this() );
	}

	void SceneNode::DetachObject( MovableObject & p_object )
	{
		auto l_it = std::find_if( m_objects.begin(), m_objects.end(), [&p_object]( std::reference_wrapper< MovableObject > l_obj )
		{
			return l_obj.get().GetName() == p_object.GetName();
		} );

		if ( l_it != m_objects.end() )
		{
			m_objects.erase( l_it );
			p_object.AttachTo( nullptr );
		}
	}

	void SceneNode::AttachTo( SceneNodeSPtr p_parent )
	{
		SceneNodeSPtr l_parent = GetParent();

		if ( l_parent )
		{
			m_parent.reset();
			l_parent->DetachChild( shared_from_this() );
			l_parent.reset();
		}

		m_parent = p_parent;

		if ( p_parent )
		{
			m_displayable = p_parent->m_displayable;
			p_parent->AddChild( shared_from_this() );
			m_mtxChanged = true;
		}
	}

	void SceneNode::Detach()
	{
		SceneNodeSPtr l_parent = GetParent();

		if ( l_parent )
		{
			m_displayable = false;
			m_parent.reset();
			l_parent->DetachChild( shared_from_this() );
			m_mtxChanged = true;
		}
	}

	bool SceneNode::HasChild( String const & p_name )
	{
		bool l_bFound = false;

		if ( m_children.find( p_name ) == m_children.end() )
		{
			l_bFound = m_children.end() != std::find_if( m_children.begin(), m_children.end(), [&p_name]( std::pair< String, SceneNodeWPtr > p_pair )
			{
				return p_pair.second.lock()->HasChild( p_name );
			} );
		}

		return l_bFound;
	}

	void SceneNode::AddChild( SceneNodeSPtr p_child )
	{
		String l_name = p_child->GetName();

		if ( m_children.find( l_name ) == m_children.end() )
		{
			m_children.insert( std::make_pair( l_name, p_child ) );
		}
		else
		{
			Logger::LogWarning( m_name + cuT( " - Can't add SceneNode " ) + l_name + cuT( " - Already in childs" ) );
		}
	}

	void SceneNode::DetachChild( SceneNodeSPtr p_child )
	{
		if ( p_child )
		{
			DetachChild( p_child->GetName() );
		}
		else
		{
			Logger::LogWarning( m_name + cuT( " - Can't remove SceneNode - Null pointer given" ) );
		}
	}

	void SceneNode::DetachChild( String const & p_childName )
	{
		auto l_it = m_children.find( p_childName );

		if ( l_it != m_children.end() )
		{
			SceneNodeSPtr l_current = l_it->second.lock();
			m_children.erase( l_it );

			if ( l_current )
			{
				l_current->Detach();
			}
		}
		else
		{
			Logger::LogWarning( m_name + cuT( " - Can't remove SceneNode " ) + p_childName + cuT( " - Not in childs" ) );
		}
	}

	void SceneNode::DetachAllChilds()
	{
		SceneNodePtrStrMap l_flush;
		std::swap( l_flush, m_children );

		for ( auto l_it : l_flush )
		{
			SceneNodeSPtr l_current = l_it.second.lock();

			if ( l_current )
			{
				l_current->Detach();
			}
		}
	}

	void SceneNode::Yaw( Angle const & p_angle )
	{
		Rotate( Quaternion( Point3d( 0.0, 1.0, 0.0 ), p_angle ) );
	}

	void SceneNode::Pitch( Angle const & p_angle )
	{
		Rotate( Quaternion( Point3d( 1.0, 0.0, 0.0 ), p_angle ) );
	}

	void SceneNode::Roll( Angle const & p_angle )
	{
		Rotate( Quaternion( Point3d( 0.0, 0.0, 1.0 ), p_angle ) );
	}

	void SceneNode::Rotate( Quaternion const & p_orientation )
	{
		m_orientation *= p_orientation;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::Translate( Point3r const & p_position )
	{
		m_position += p_position;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::Scale( Point3r const & p_scale )
	{
		m_scale *= p_scale;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::SetOrientation( Quaternion const & p_orientation )
	{
		m_orientation = p_orientation;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::SetPosition( Point3r const & p_position )
	{
		m_position = p_position;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	void SceneNode::SetScale( Point3r const & p_scale )
	{
		m_scale = p_scale;
		DoUpdateChildsDerivedTransform();
		m_mtxChanged = true;
	}

	Point3r SceneNode::GetDerivedPosition()const
	{
		Point3r l_return( m_position );
		auto l_parent = GetParent();

		if ( l_parent )
		{
			l_return = matrix::get_transformed( l_parent->GetDerivedTransformationMatrix(), m_position );
		}

		return l_return;
	}

	Quaternion SceneNode::GetDerivedOrientation()const
	{
		Quaternion l_return( m_orientation );
		auto l_parent = GetParent();

		if ( l_parent )
		{
			l_return  = l_return * l_parent->GetDerivedOrientation();
		}

		return l_return;
	}

	Point3r SceneNode::GetDerivedScale()const
	{
		Point3r l_return( m_scale );
		auto l_parent = GetParent();

		if ( l_parent )
		{
			l_return *= l_parent->GetDerivedScale();
		}

		return l_return;
	}

	Matrix4x4r const & SceneNode::GetTransformationMatrix()const
	{
		return m_transform;
	}

	Matrix4x4r const & SceneNode::GetDerivedTransformationMatrix()const
	{
		return m_derivedTransform;
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
			SceneNodeSPtr l_parent = GetParent();

			if ( l_parent )
			{
				m_derivedTransform = l_parent->GetDerivedTransformationMatrix() * m_transform;
			}
			else
			{
				m_derivedTransform = m_transform;
			}

			m_derivedMtxChanged = false;
			m_signalChanged( *this );
		}
	}

	void SceneNode::DoUpdateChildsDerivedTransform()
	{
		for ( auto l_it : m_children )
		{
			SceneNodeSPtr l_current = l_it.second.lock();

			if ( l_current )
			{
				l_current->DoUpdateChildsDerivedTransform();
				l_current->m_derivedMtxChanged = true;
			}
		}
	}
}
