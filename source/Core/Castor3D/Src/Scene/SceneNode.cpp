#include "SceneNode.hpp"

#include "Engine.hpp"
#include "SceneNodeManager.hpp"

#include "Geometry.hpp"
#include "MovableObject.hpp"
#include "Scene.hpp"

#include "Mesh/Mesh.hpp"
#include "Mesh/Vertex.hpp"
#include "Miscellaneous/Ray.hpp"
#include "Render/Pipeline.hpp"
#include "Render/RenderSystem.hpp"

#include <Logger.hpp>
#include <TransformationMatrix.hpp>

using namespace Castor;

namespace Castor3D
{
	SceneNode::TextLoader::TextLoader( File::eENCODING_MODE p_encodingMode )
		: Loader< SceneNode, eFILE_TYPE_TEXT, TextFile >( File::eOPEN_MODE_DUMMY, p_encodingMode )
	{
	}

	bool SceneNode::TextLoader::operator()( SceneNode const & p_node, TextFile & p_file )
	{
		bool l_return = false;
		Logger::LogInfo( cuT( "Writing Node " ) + p_node.GetName() );

		if ( p_node.GetName() != cuT( "RootNode" ) )
		{
			l_return = p_file.WriteText( cuT( "\tscene_node \"" ) + p_node.GetName() + cuT( "\"\n\t{\n" ) ) > 0;

			if ( l_return && p_node.GetParent() )
			{
				l_return = p_file.WriteText( cuT( "\t\tparent \"" ) + p_node.GetParent()->GetName() + cuT( "\"\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\t\torientation " ) ) > 0 && Quaternion::TextLoader()( p_node.GetOrientation(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\t\tposition " ) ) > 0 && Point3r::TextLoader()( p_node.GetPosition(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.Print( 256, cuT( "\t\tscale " ) ) > 0 && Point3r::TextLoader()( p_node.GetScale(), p_file ) && p_file.WriteText( cuT( "\n" ) ) > 0;
			}

			if ( l_return )
			{
				l_return = p_file.WriteText( cuT( "\n\t}\n" ) ) > 0;
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Writing Childs" ) );

			for ( auto && l_it = p_node.ChildsBegin(); l_it != p_node.ChildsEnd() && l_return; ++l_it )
			{
				SceneNodeSPtr l_node = l_it->second.lock();

				if ( l_node )
				{
					l_return = SceneNode::TextLoader()( *l_node, p_file );
				}
			}
		}

		if ( l_return )
		{
			Logger::LogInfo( cuT( "Childs Written" ) );
		}

		return l_return;
	}

	//*************************************************************************************************

	SceneNode::BinaryParser::BinaryParser( Path const & p_path )
		: Castor3D::BinaryParser< SceneNode >( p_path )
	{
	}

	bool SceneNode::BinaryParser::Fill( SceneNode const & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		BinaryChunk l_chunk( eCHUNK_TYPE_SCENE_NODE );
		Point3f l_scale;
		Point3f l_position;

		if ( p_obj.GetName() != cuT( "RootNode" ) )
		{
			l_return = DoFillChunk( p_obj.GetName(), eCHUNK_TYPE_NAME, l_chunk );

			if ( l_return )
			{
				l_return = DoFillChunk( p_obj.GetOrientation(), eCHUNK_TYPE_NODE_ORIENTATION, l_chunk );
			}

			if ( l_return )
			{
				l_position = p_obj.GetPosition();
				l_return = DoFillChunk( l_position, eCHUNK_TYPE_NODE_POSITION, l_chunk );
			}

			if ( l_return )
			{
				l_scale = p_obj.GetScale();
				l_return = DoFillChunk( l_scale, eCHUNK_TYPE_NODE_SCALE, l_chunk );
			}
		}

		if ( l_return )
		{
			for ( auto && l_it = p_obj.ChildsBegin(); l_it != p_obj.ChildsEnd() && l_return; ++l_it )
			{
				SceneNodeSPtr l_node = l_it->second.lock();

				if ( l_node )
				{
					l_return = SceneNode::BinaryParser( m_path ).Fill( *l_node, l_chunk );
				}
			}
		}

		if ( l_return )
		{
			l_chunk.Finalise();
			p_chunk.AddSubChunk( l_chunk );
		}

		return l_return;
	}

	bool SceneNode::BinaryParser::Parse( SceneNode & p_obj, BinaryChunk & p_chunk )const
	{
		bool l_return = true;
		Quaternion l_orientation;
		Point3f l_scale;
		Point3f l_position;

		while ( p_chunk.CheckAvailable( 1 ) )
		{
			BinaryChunk l_chunk;
			l_return = p_chunk.GetSubChunk( l_chunk );

			if ( l_return )
			{
				switch ( l_chunk.GetChunkType() )
				{
				case eCHUNK_TYPE_NODE_ORIENTATION:
					l_return = DoParseChunk( l_orientation, l_chunk );

					if ( l_return )
					{
						p_obj.SetOrientation( l_orientation );
					}

					break;

				case eCHUNK_TYPE_NODE_POSITION:
					l_return = DoParseChunk( l_position, l_chunk );

					if ( l_return )
					{
						p_obj.SetPosition( l_position );
					}

					break;

				case eCHUNK_TYPE_NODE_SCALE:
					l_return = DoParseChunk( l_scale, l_chunk );

					if ( l_return )
					{
						p_obj.SetScale( l_scale );
					}

					break;

				case eCHUNK_TYPE_SCENE_NODE:
				{
					BinaryChunk l_chunkNode;
					String l_name;
					l_return = l_chunk.GetSubChunk( l_chunkNode );

					if ( l_return )
					{
						switch ( l_chunkNode.GetChunkType() )
						{
						case eCHUNK_TYPE_NAME:
							l_return = DoParseChunk( l_name, l_chunkNode );
							break;

						default:
							l_return = false;
							break;
						}

						if ( l_return )
						{
							SceneNodeSPtr l_node = p_obj.GetScene()->GetSceneNodeManager().Create( l_name, p_obj.shared_from_this() );
							l_return = SceneNode::BinaryParser( m_path ).Parse( *l_node, l_chunk );
						}
					}
				}
				break;

				default:
					l_return = false;
					break;
				}
			}

			if ( !l_return )
			{
				p_chunk.EndParse();
			}
		}

		return l_return;
	}

	//*************************************************************************************************

	uint64_t SceneNode::Count = 0;

	SceneNode::SceneNode( String const & p_name, Scene & p_scene )
		: OwnedBy< Scene >( p_scene )
		, Named( p_name )
		, m_visible( true )
		, m_scale( 1.0, 1.0, 1.0 )
		, m_position( 0.0, 0.0, 0.0 )
		, m_displayable( p_name == cuT( "RootNode" ) )
		, m_mtxChanged( true )
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

	void SceneNode::AttachObject( MovableObjectSPtr p_object )
	{
		if ( p_object )
		{
			p_object->Detach();
			m_objects.push_back( p_object );
			p_object->AttachTo( shared_from_this() );
		}
	}

	void SceneNode::DetachObject( MovableObjectSPtr p_object )
	{
		if ( p_object )
		{
			auto l_it = std::find_if( m_objects.begin(), m_objects.end(), [&p_object]( MovableObjectWPtr l_obj )
			{
				return l_obj.lock()->GetName() == p_object->GetName();
			} );

			if ( l_it != m_objects.end() )
			{
				m_objects.erase( l_it );
				p_object->AttachTo( nullptr );
			}
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

		if ( m_childs.find( p_name ) == m_childs.end() )
		{
			l_bFound = m_childs.end() != std::find_if( m_childs.begin(), m_childs.end(), [&p_name]( std::pair< String, SceneNodeWPtr > p_pair )
			{
				return p_pair.second.lock()->HasChild( p_name );
			} );
		}

		return l_bFound;
	}

	void SceneNode::AddChild( SceneNodeSPtr p_child )
	{
		String l_name = p_child->GetName();

		if ( m_childs.find( l_name ) == m_childs.end() )
		{
			m_childs.insert( std::make_pair( l_name, p_child ) );
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
		auto && l_it = m_childs.find( p_childName );

		if ( l_it != m_childs.end() )
		{
			SceneNodeSPtr l_current = l_it->second.lock();
			m_childs.erase( l_it );

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
		std::swap( l_flush, m_childs );

		for ( auto && l_it : l_flush )
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

	GeometrySPtr SceneNode::GetNearestGeometry( Ray * p_ray, real & p_distance, FaceSPtr * p_nearestFace, SubmeshSPtr * p_nearestSubmesh )
	{
		GeometrySPtr l_return = nullptr;
		real l_distance;

		for ( auto && l_it : m_objects )
		{
			MovableObjectSPtr l_current = l_it.lock();

			if ( l_current && l_current->GetType() == eMOVABLE_TYPE_GEOMETRY )
			{
				GeometrySPtr l_geometry = std::static_pointer_cast< Geometry >( l_current );

				if ( ( l_distance = p_ray->Intersects( l_geometry, p_nearestFace, p_nearestSubmesh ) ) >= 0.0 && l_distance < p_distance )
				{
					p_distance = l_distance;
					l_return = l_geometry;
				}
			}
		}

		GeometrySPtr l_pTmp;

		for ( auto && l_it : m_childs )
		{
			SceneNodeSPtr l_current = l_it.second.lock();

			if ( l_current )
			{
				l_pTmp = l_current->GetNearestGeometry( p_ray, p_distance, p_nearestFace, p_nearestSubmesh );

				if ( l_pTmp )
				{
					l_return = l_pTmp;
				}
			}
		}

		return l_return;
	}

	Point3r SceneNode::GetDerivedPosition()
	{
		Point3r l_return( m_position );
		auto l_parent = GetParent();

		if ( l_parent )
		{
			l_return = matrix::get_transformed( l_parent->GetDerivedTransformationMatrix(), m_position );
		}

		return l_return;
	}

	Quaternion SceneNode::GetDerivedOrientation()
	{
		Quaternion l_return( m_orientation );
		auto l_parent = GetParent();

		if ( l_parent )
		{
			l_return  = l_return * l_parent->GetDerivedOrientation();
		}

		return l_return;
	}

	Point3r SceneNode::GetDerivedScale()
	{
		Point3r l_return( m_scale );
		auto l_parent = GetParent();

		if ( l_parent )
		{
			l_return *= l_parent->GetDerivedScale();
		}

		return l_return;
	}

	Matrix4x4r const & SceneNode::GetTransformationMatrix()
	{
		DoComputeMatrix();
		return m_transform;
	}

	Matrix4x4r const & SceneNode::GetDerivedTransformationMatrix()
	{
		DoComputeMatrix();
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
			m_signalChanged();
		}
	}

	void SceneNode::DoUpdateChildsDerivedTransform()
	{
		for ( auto && l_it : m_childs )
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
