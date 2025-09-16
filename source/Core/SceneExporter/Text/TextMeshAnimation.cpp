#include "TextMeshAnimation.hpp"

#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationSubmesh.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshMorphTarget.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp>

namespace c3d
{
	template<>
	class TextWriter< MeshMorphTarget >
		: public TextWriterT< MeshMorphTarget >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< MeshMorphTarget >{ tabs }
		{
		}

		bool operator()( MeshMorphTarget const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, cuT( "keyframe" ) ) } )
			{
				result = write( file, cuT( "index " ), object.getTimeIndex().count() );

				if ( result )
				{
					for ( auto const & [id, weights] : object )
					{
						result = result && write( file, cuT( "submesh " ), id );
						if ( result )
						{
							if ( auto weightsBlock{ beginBlock( file, cuT( "weights" ) ) } )
							{
								for ( auto & weight : weights )
									result = result && write( file, cuT( "weight " ), weight );
							}
						}
					}
				}
			}

			return result;
		}
	};

	TextWriter< MeshAnimation >::TextWriter( String const & tabs )
		: TextWriterT< MeshAnimation >{ tabs }
	{
	}

	bool TextWriter< MeshAnimation >::operator()( MeshAnimation const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing MeshAnimation " ) << object.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "mesh_animation" ), object.getName() ) } )
		{
			result = true;
			TextWriter< MeshMorphTarget > writer{ tabs() };
			for ( auto const & keyframe : object )
				result = result && writer( static_cast< MeshMorphTarget const & >( *keyframe ), file );
		}

		return result;
	}
}
