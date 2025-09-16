#include "TextSkeletonAnimation.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationNode.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>
#include <CastorUtils/Data/Text/TextQuaternion.hpp>

namespace c3d
{
	template<>
	class TextWriter< SkeletonAnimationKeyFrame >
		: public TextWriterT< SkeletonAnimationKeyFrame >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< SkeletonAnimationKeyFrame >{ tabs }
		{
		}

		bool operator()( SkeletonAnimationKeyFrame const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, cuT( "keyframe" ) ) } )
			{
				result = write( file, cuT( "index" ), object.getTimeIndex().count() );
				if ( result )
				{
					StringMap< std::pair< SkeletonAnimationObject const *, NodeTransform const * > > sorted;
					for ( auto const & [skobject, transform, _] : object )
						sorted.try_emplace( skobject->getName(), skobject, &transform );

					for ( auto const & [name, obj] : sorted )
					{
						auto skobject = obj.first;
						auto & transform = *obj.second;
						switch ( skobject->getType() )
						{
						case SkeletonNodeType::eNode:
							if ( auto objBlock{ beginBlock( file, cuT( "node" ), string::toString( skobject->getName() ) ) } )
							{
								result = result
									&& writeNamedSubOpt( file, cuT( "position" ), transform.translate, Point3f{} )
									&& writeNamedSubOpt( file, cuT( "rotation" ), transform.rotate, Quaternion::identity() )
									&& writeNamedSubOpt( file, cuT( "scale" ), transform.scale, Point3f{ 1.0f, 1.0f, 1.0f } );
							}
							break;
						case SkeletonNodeType::eBone:
							if ( auto objBlock{ beginBlock( file, cuT( "bone" ), string::toString( skobject->getName() ) ) } )
							{
								result = result
									&& writeNamedSubOpt( file, cuT( "position" ), transform.translate, Point3f{} )
									&& writeNamedSubOpt( file, cuT( "rotation" ), transform.rotate, Quaternion::identity() )
									&& writeNamedSubOpt( file, cuT( "scale" ), transform.scale, Point3f{ 1.0f, 1.0f, 1.0f } );
							}
							break;
						default:
							CU_Failure( "Unsupported SkeletonNodeType" );
						}
					}
				}
			}

			return result;
		}
	};

	template<>
	class TextWriter< SkeletonAnimationObject >
		: public TextWriterT< SkeletonAnimationObject >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< SkeletonAnimationObject >{ tabs }
		{
		}

		bool operator()( SkeletonAnimationObject const & object
			, StringStream & file )override
		{
			bool result{ false };
			String type = object.getType() == SkeletonNodeType::eBone? cuT( "bone" ) : cuT( "node" );

			if ( object.getChildren().empty() )
			{
				result = writeName( file, type, object.getName() );
			}
			else if ( auto block{ beginBlock( file, type, string::toString( object.getName() ) ) } )
			{
				result = true;
				TextWriter< SkeletonAnimationObject > writer{ tabs() };
				for ( auto child : object.getChildren() )
					result = result && writer( *child, file );
			}

			return result;
		}
	};

	TextWriter< SkeletonAnimation >::TextWriter( String const & tabs )
		: TextWriterT< SkeletonAnimation >{ tabs }
	{
	}

	bool TextWriter< SkeletonAnimation >::operator()( SkeletonAnimation const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing SkeletonAnimation " ) << object.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "animation" ), object.getName() ) } )
		{
			result = true;

			TextWriter< SkeletonAnimationObject > objectWriter{ tabs() };
			for ( auto moving : object.getRootObjects() )
				result = result && objectWriter( *moving, file );

			TextWriter< SkeletonAnimationKeyFrame > keyFrameWriter{ tabs() };
			for ( auto & keyframe : object )
				result = result && keyFrameWriter( static_cast< SkeletonAnimationKeyFrame const & >( *keyframe ), file );
		}

		return result;
	}
}
