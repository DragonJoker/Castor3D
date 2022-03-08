#include "Castor3D/Text/TextAnimatedObjectGroup.hpp"

#include "Castor3D/Miscellaneous/Logger.hpp"

using namespace castor3d;

namespace castor
{
	template<>
	class TextWriter< GroupAnimation >
		: public TextWriterT< GroupAnimation >
	{
	public:
		C3D_API explicit TextWriter( String const & tabs )
			: TextWriterT< GroupAnimation >{ tabs }
		{
		}

		C3D_API virtual bool operator()( GroupAnimation const & group
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
					name = name.substr( 0, skel );
					write = group.getObjects().find( name ) == group.getObjects().end()
						&& written.find( name ) == written.end();
				}
				else if ( mesh != String::npos )
				{
					name = name.substr( 0, mesh );
					write = group.getObjects().find( name ) == group.getObjects().end()
						&& written.find( name ) == written.end();
				}
				else if ( node != String::npos )
				{
					name = name.substr( 0, node );
					write = group.getObjects().find( name ) == group.getObjects().end()
						&& written.find( name ) == written.end();
				}

				if ( write )
				{
					result = result
						&& writeName( file, "animated_object", name );
					written.insert( name );
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
