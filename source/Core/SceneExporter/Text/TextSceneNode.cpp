#include "TextSceneNode.hpp"
#include "TextSceneNodeAnimation.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Animation/AnimatedSceneNode.hpp>
#include <Castor3D/Scene/Animation/SceneNode/SceneNodeAnimationInstance.hpp>

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
		, bool forceText
		, float scale )
		: TextWriterT< SceneNode >{ tabs }
		, m_forceText{ forceText }
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
					auto position = node.getPosition();
					auto orientation = node.getOrientation();
					auto scale = node.getScale();

					if ( node.hasAnimation() )
					{
						// Since played animations overwrite the scene node data, don't write them but the untouched ones saved in the animation.
						if ( auto found = node.getScene()->getAnimatedObjectGroupCache().findObject( node.getName() + cuT( "_Node" ) );
							!found.empty() )
						{
							if ( auto animNode = static_cast< AnimatedSceneNode * >( found.front() );
								animNode->isPlayingAnimation() )
							{
								auto const & anim = animNode->getPlayingAnimation();
								position = anim.getInitialPosition();
								orientation  = anim.getInitialOrientation();
								scale = anim.getInitialScale();
							}
						}
					}

					result = writeOpt( file, cuT( "visible" ), node.isVisible(), true )
						&& writeNamedSubOpt( file, cuT( "orientation" ), orientation, Quaternion::identity() )
						&& writeNamedSubOpt( file, cuT( "position" ), position * m_scale, Point3f{} )
						&& writeNamedSubOpt( file, cuT( "scale" ), scale * m_scale, Point3f{ 1.0f, 1.0f, 1.0f } );
				}

				if ( m_forceText )
				{
					TextWriter< SceneNodeAnimation > writer{ tabs() };
					for ( auto const & [_, animation] : node.getAnimations() )
						result = result && writer( static_cast< SceneNodeAnimation const & >( *animation ), file );
				}
				else
				{
					for ( auto const & [name, _] : node.getAnimations() )
						result = result && writeName( file, cuT( "import_anim" ), cuT( "Helpers/" ) + node.getName() + cuT( "-" ) + name + cuT( ".csna" ) );
				}
			}
		}

		for ( auto const & [_, childNode] : node.getChildren() )
		{
			if ( result && childNode )
				result = ( *this )( *childNode, file );
		}

		return result;
	}
}
