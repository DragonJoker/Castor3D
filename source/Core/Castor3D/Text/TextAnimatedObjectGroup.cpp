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
			, castor::TextFile & file )override
		{
			return beginBlock( "animation", group.name, file )
				&& write( "looped", group.looped, file )
				&& write( "scale", group.scale, file )
				&& write( "start_at", group.startingPoint.count() / 1000.0, file )
				&& write( "stop_at", group.stoppingPoint.count() / 1000.0, file );
		}
	};

	TextWriter< AnimatedObjectGroup >::TextWriter( String const & tabs )
		: TextWriterT< AnimatedObjectGroup >{ tabs }
	{
	}

	bool TextWriter< AnimatedObjectGroup >::TextWriter::operator()( AnimatedObjectGroup const & group
		, TextFile & file )
	{
		log::info << tabs() << cuT( "Writing AnimatedObjectGroup " ) << group.getName() << std::endl;
		bool result = false;

		if ( beginBlock( "animated_object_group", group.getName(), file ) )
		{
			StrSet written;

			for ( auto it : group.getObjects() )
			{
				auto name = it.first;
				bool write{ true };
				size_t skel = name.find( cuT( "_Skeleton" ) );
				size_t mesh = name.find( cuT( "_Mesh" ) );

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

				if ( write )
				{
					result = result
						&& writeName( "animated_object", name, file );
					written.insert( name );
				}
			}

			if ( !group.getAnimations().empty() )
			{
				for ( auto it : group.getAnimations() )
				{
					result = result
						&& write( it.second, file );
				}
			}

			if ( !group.getAnimations().empty() )
			{
				result = result && file.writeText( cuT( "\n" ) ) > 0;

				for ( auto it : group.getAnimations() )
				{
					if ( it.second.state == AnimationState::ePlaying )
					{
						result = result && writeName( cuT( "start_animation" ), it.first, file );
					}
				}
			}
		}

		return result;
	}
}
