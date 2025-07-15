#include "TextSceneNode.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextQuaternion.hpp>

namespace c3d
{
	namespace txtnode
	{
		static bool isIgnored( String const & name )
		{
			return name == Scene::RootNode
				|| name == Scene::ObjectRootNode
				|| name == Scene::CameraRootNode
				|| name.find( cuT( "_REye" ) ) != String::npos
				|| name.find( cuT( "_LEye" ) ) != String::npos;
		}

		static bool isIgnored( SceneNode const & node )
		{
			return !node.isSerialisable()
				|| isIgnored( node.getName() );
		}
	}

	TextWriter< SceneNode >::TextWriter( String const & tabs
		, float scale )
		: TextWriterT< SceneNode >{ tabs }
		, m_scale{ scale }
	{
	}

	bool TextWriter< SceneNode >::operator()( SceneNode const & node
		, StringStream & file )
	{
		bool result = true;

		if ( !txtnode::isIgnored( node ) )
		{
			log::info << tabs() << cuT( "Writing Node " ) << node.getName() << std::endl;
			result = false;

			if ( auto block{ beginBlock( file, cuT( "scene_node" ), node.getName() ) } )
			{
				if ( node.getParent()
					&& !txtnode::isIgnored( *node.getParent() ) )
				{
					result = writeName( file, cuT( "parent" ), node.getParent()->getName() );
				}
				else
				{
					result = true;
				}

				if ( result )
				{
					result = writeOpt( file, cuT( "visible" ), node.isVisible(), true )
						&& writeNamedSubOpt( file, cuT( "orientation" ), node.getOrientation(), Quaternion::identity() )
						&& writeNamedSubOpt( file, cuT( "position" ), node.getPosition() * m_scale, Point3f{} )
						&& writeNamedSubOpt( file, cuT( "scale" ), node.getScale() * m_scale, Point3f{ 1.0f, 1.0f, 1.0f } );
				}
			}
		}

		for ( auto const & it : node.getChildren() )
		{
			if ( result )
			{
				auto childNode = it.second;

				if ( childNode )
				{
					result = ( *this )( *childNode, file );
				}
			}
		}

		return result;
	}
}
