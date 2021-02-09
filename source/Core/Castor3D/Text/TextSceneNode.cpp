#include "Castor3D/Text/TextSceneNode.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextQuaternion.hpp>

using namespace castor3d;

namespace castor
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

	TextWriter< SceneNode >::TextWriter( castor::String const & tabs
		, float scale )
		: TextWriterT< SceneNode >{ tabs }
		, m_scale{ scale }
	{
	}

	bool TextWriter< SceneNode >::operator()( SceneNode const & node
		, castor::TextFile & file )
	{
		bool result = true;

		if ( !isIgnored( node ) )
		{
			log::info << tabs() << cuT( "Writing Node " ) << node.getName() << std::endl;
			result = false;

			if ( auto block = beginBlock( "scene_node", node.getName(), file ) )
			{
				if ( node.getParent()
					&& !isIgnored( *node.getParent() ) )
				{
					result = writeName( "parent", node.getParent()->getName(), file );
				}

				if ( result )
				{
					result = writeOpt( "orientation", node.getOrientation(), castor::Quaternion::identity(), file )
						&& writeOpt( "position", node.getPosition(), castor::Point3f{}, file );
				}

				if ( result && ( node.getScale() != castor::Point3f{ 1, 1, 1 } || m_scale != 1.0f ) )
				{
					result = write( "scale", node.getScale() * m_scale, file );
				}
			}
		}

		for ( auto const & it : node.getChildren() )
		{
			if ( result )
			{
				SceneNodeSPtr node = it.second.lock();

				if ( node )
				{
					result = ( *this )( *node, file );
				}
			}
		}

		return result;
	}
}
