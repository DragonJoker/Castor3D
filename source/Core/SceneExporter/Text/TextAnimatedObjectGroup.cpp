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

			if ( auto block{ beginBlock( file, cuT( "animation" ), group.name ) } )
			{
				result = write( file, cuT( "looped" ), group.looped )
					&& writeOpt( file, cuT( "scale" ), group.scale, 1.0f )
					&& writeOpt( file, cuT( "start_at" ), double( group.startingPoint.count() ) / 1000.0, 0.0 )
					&& writeOpt( file, cuT( "stop_at" ), double( group.stoppingPoint.count() ) / 1000.0, 0.0 );
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

		if ( auto block{ beginBlock( file, cuT( "animated_object_group" ), group.getName() ) } )
		{
			result = true;

			for ( auto & it : group.getObjects() )
			{
				auto name = it.first;
				size_t skel = name.find( cuT( "_Skeleton" ) );
				size_t mesh = name.find( cuT( "_Mesh" ) );
				size_t node = name.find( cuT( "_Node" ) );

				if ( skel != String::npos )
				{
					name = name.substr( 0, skel );
					result = result && writeName( file, cuT( "animated_skeleton" ), name );
				}

				if ( mesh != String::npos )
				{
					name = name.substr( 0, mesh );
					result = result && writeName( file, cuT( "animated_mesh" ), name );
				}

				if ( node != String::npos )
				{
					name = name.substr( 0, node );
					result = result && writeName( file, cuT( "animated_node" ), name );
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
