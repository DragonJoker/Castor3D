#include "TextAnimatedObjectGroup.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>

namespace castor
{
	using namespace castor3d;

	template<>
	class TextWriter< GroupAnimation >
		: public TextWriterT< GroupAnimation >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< GroupAnimation >{ tabs }
		{
		}

		virtual bool operator()( GroupAnimation const & group
			, castor::StringStream & file )override
		{
			bool result = false;

			if ( auto block{ beginBlock( file, "animation", group.name ) } )
			{
				result = write( file, "looped", group.looped )
					&& writeOpt( file, "scale", group.scale, 1.0f )
					&& writeOpt( file, "start_at", double( group.startingPoint.count() ) / 1000.0, 0.0 )
					&& writeOpt( file, "stop_at", double( group.stoppingPoint.count() ) / 1000.0, 0.0 );
			}

			return result;
		}
	};

	TextWriter< AnimatedObjectGroup >::TextWriter( String const & tabs )
		: TextWriterT< AnimatedObjectGroup >{ tabs }
	{
	}

	bool TextWriter< AnimatedObjectGroup >::TextWriter::operator()( AnimatedObjectGroup const & group
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing AnimatedObjectGroup " ) << group.getName() << std::endl;
		bool result = false;

		if ( auto block{ beginBlock( file, "animated_object_group", group.getName() ) } )
		{
			result = true;
			StrSet written;

			for ( auto it : group.getObjects() )
			{
				auto name = it.first;
				bool write{ true };
				size_t skel = name.find( cuT( "_Skeleton" ) );
				size_t mesh = name.find( cuT( "_Mesh" ) );
				size_t node = name.find( cuT( "_Node" ) );

				// Only add objects, and not skeletons or meshes
				if ( skel != String::npos )
				{
					write = group.getObjects().find( name ) == group.getObjects().end()
						&& written.find( name ) == written.end();
					written.insert( name );
					name = name.substr( 0, skel );
					result = result && writeName( file, "animated_skeleton", name );
				}
				else if ( mesh != String::npos )
				{
					write = group.getObjects().find( name ) == group.getObjects().end()
						&& written.find( name ) == written.end();
					written.insert( name );
					name = name.substr( 0, mesh );
					result = result && writeName( file, "animated_mesh", name );
				}
				else if ( node != String::npos )
				{
					write = group.getObjects().find( name ) == group.getObjects().end()
						&& written.find( name ) == written.end();
					written.insert( name );
					name = name.substr( 0, node );
					result = result && writeName( file, "animated_node", name );
				}
			}

			if ( !group.getAnimations().empty() )
			{
				for ( auto it : group.getAnimations() )
				{
					result = result
						&& writeSub( file, it.second );
				}
			}

			if ( !group.getAnimations().empty() )
			{
				result = result && writeText( file, cuT( "\n" ) );

				for ( auto it : group.getAnimations() )
				{
					if ( it.second.state == AnimationState::ePlaying )
					{
						result = result && writeName( file, cuT( "start_animation" ), it.first );
					}
				}
			}
		}

		return result;
	}
}
