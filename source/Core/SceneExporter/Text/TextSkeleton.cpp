#include "TextSkeleton.hpp"
#include "TextSkeletonAnimation.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace c3d
{
	template<>
	class TextWriter< SkeletonNode >
		: public TextWriterT< SkeletonNode >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< SkeletonNode >{ tabs }
		{
		}

		using TextWriterT< SkeletonNode >::write;

		bool write( StringStream & file, String const & name, Matrix4x4f const & value )const
		{
			StringStream stream;
			stream << value[0] << " " << value[1] << " " << value[2] << " " << value[3];
			return write( file, name, stream.str() );
		}

		bool operator()( SkeletonNode const & object
			, StringStream & file )override
		{
			bool result{ false };
			auto name = ( object.getType() == SkeletonNodeType::eBone ) ? cuT( "bone" ) : cuT( "node" );

			if ( auto block{ beginBlock( file, name, object.getName() ) } )
			{
				result = true;

				if ( auto parent = object.getParent() )
					result = writeName( file, cuT( "parent" ), parent->getName() );

				if ( object.getType() == SkeletonNodeType::eBone )
				{
					auto const & bone = static_cast< BoneNode const & >( object );
					result = write( file, cuT( "inverse_transform" ), bone.getInverseTransform() );
				}
			}

			return result;
		}
	};

	TextWriter< Skeleton >::TextWriter( String const & tabs
		, String const & subfolder
		, bool forceText )
		: TextWriterT< Skeleton >{ tabs }
		, m_subfolder{ subfolder }
		, m_forceText{ forceText }
	{
	}

	bool TextWriter< Skeleton >::operator()( Skeleton const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing Skeleton " ) << object.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "skeleton" ), object.getName() ) } )
		{
			if ( m_forceText )
			{
				if ( auto armature{ beginBlock( file, cuT( "armature" ) ) } )
				{
					result = true;
					TextWriter< SkeletonNode > writer{ tabs() };
					for ( auto const & node : object.getNodes() )
						result = result && writer( *node, file );
				}

				if ( result )
				{
					TextWriter< SkeletonAnimation > writer{ tabs() };
					for ( auto const & [_, animation] : object.getAnimations() )
						result = result && writer( static_cast< SkeletonAnimation const & >( *animation ), file );
				}
			}
			else
			{
				if ( !m_subfolder.empty() )
					result = writeName( file, cuT( "import" ), cuT( "Skeletons/" ) + m_subfolder + cuT( "/" ) + object.getName() + cuT( ".cskl" ) );
				else
					result = writeName( file, cuT( "import" ), cuT( "Skeletons/" ) + object.getName() + cuT( ".cskl" ) );

				for ( auto const & [name, _] : object.getAnimations() )
				{
					if ( !m_subfolder.empty() )
						result = result && writeName( file, cuT( "import_anim" ), cuT( "Skeletons/" ) + m_subfolder + cuT( "/" ) + object.getName() + cuT( "-" ) + name + cuT( ".cska" ) );
					else
						result = result && writeName( file, cuT( "import_anim" ), cuT( "Skeletons/" ) + object.getName() + cuT( "-" ) + name + cuT( ".cska" ) );
				}
			}
		}

		return result;
	}
}
