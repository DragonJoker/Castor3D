#include "Castor3D/Text/TextSceneNode.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextQuaternion.hpp>

namespace castor
{
	using namespace castor3d;

	namespace txtnode
	{
		static bool isIgnored( castor::String const & name )
		{
			return name == Scene::RootNode
				|| name == Scene::ObjectRootNode
				|| name == Scene::CameraRootNode
				|| name.find( cuT( "_REye" ) ) != castor::String::npos
				|| name.find( cuT( "_LEye" ) ) != castor::String::npos;
		}

		static bool isIgnored( SceneNode const & node )
		{
			return !node.isSerialisable()
				|| isIgnored( node.getName() );
		}
	}

	TextWriter< SceneNode >::TextWriter( castor::String const & tabs
		, float scale )
		: TextWriterT< SceneNode >{ tabs }
		, m_scale{ scale }
	{
	}

	bool TextWriter< SceneNode >::operator()( SceneNode const & node
		, castor::StringStream & file )
	{
		bool result = true;

		if ( !txtnode::isIgnored( node ) )
		{
			log::info << tabs() << cuT( "Writing Node " ) << node.getName() << std::endl;
			result = false;

			if ( auto block{ beginBlock( file, "scene_node", node.getName() ) } )
			{
				if ( node.getParent()
					&& !txtnode::isIgnored( *node.getParent() ) )
				{
					result = writeName( file, "parent", node.getParent()->getName() );
				}
				else
				{
					result = true;
				}

				if ( result )
				{
					result = writeNamedSubOpt( file, "orientation", node.getOrientation(), castor::Quaternion::identity() )
						&& writeNamedSubOpt( file, "position", node.getPosition() * m_scale, castor::Point3f{} )
						&& writeNamedSubOpt( file, "scale", node.getScale() * m_scale, castor::Point3f{ 1.0f, 1.0f, 1.0f } );
				}
			}
		}

		for ( auto const & it : node.getChildren() )
		{
			if ( result )
			{
				SceneNodeSPtr childNode = it.second.lock();

				if ( childNode )
				{
					result = ( *this )( *childNode, file );
				}
			}
		}

		return result;
	}
}
