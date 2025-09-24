#include "TextMesh.hpp"
#include "TextMeshAnimation.hpp"
#include "TextSkeleton.hpp"

#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/LineMapping.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/TriFaceMapping.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>

#include <CastorUtils/Data/Text/TextPoint.hpp>

namespace c3d
{
	template<>
	class TextWriter< Submesh >
		: public TextWriterT< Submesh >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< Submesh >{ tabs }
		{
		}

		using TextWriterT< Submesh >::write;

		bool write( StringStream & file
			, String const & name
			, Point3fArray const & data )
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = true;
				for ( auto const & value : data )
					result = result && write( file, cuT( "value" ), value );
			}

			return result;
		}

		bool write( StringStream & file
			, String const & name
			, Point4fArray const & data )
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = true;
				for ( auto const & value : data )
					result = result && write( file, cuT( "value" ), value );
			}

			return result;
		}

		bool write( StringStream & file
			, String const & name
			, FaceArray const & data )
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = true;
				for ( auto const & value : data )
				{
					StringStream stream;
					stream << value[0] << " " << value[1] << " " << value[2];
					result = result && write( file, cuT( "value " ), stream.str() );
				}
			}

			return result;
		}

		bool write( StringStream & file
			, String const & name
			, LineArray const & data )
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = true;
				for ( auto const & value : data )
				{
					StringStream stream;
					stream << value[0] << " " << value[1];
					result = result && write( file, cuT( "value " ), stream.str() );
				}
			}

			return result;
		}

		bool write( StringStream & file
			, String const & name
			, VertexBoneDataArray const & data )
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = true;
				for ( auto const & value : data )
				{
					if ( auto boneBlock{ beginBlock( file, "vertex_bone_data" ) } )
					{
						for ( uint32_t i = 0; i < value.m_ids.size(); ++i )
						{
							if ( value.m_weights[i] != 0.0f )
							{
								StringStream stream;
								stream << value.m_ids[i] << " " << value.m_weights[i];
								result = result && write( file, cuT( "bone_weight" ), stream.str() );
							}
						}
					}
				}
			}

			return result;
		}

		bool write( StringStream & file
			, String const & name
			, Vector< SubmeshAnimationBuffer > const & data )
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, name ) } )
			{
				result = true;

				for ( auto const & it : data )
				{
					result = false;
					if ( auto bufferBlock{ beginBlock( file, cuT( "buffer" ) ) } )
					{
						result = true;
						if ( result && !it.positions.empty() )
							result = write( file, cuT( "positions" ), it.positions );
						if ( result && !it.normals.empty() )
							result = write( file, cuT( "normals" ), it.normals );
						if ( result && !it.tangents.empty() )
							result = write( file, cuT( "tangents" ), it.tangents );
						if ( result && !it.bitangents.empty() )
							result = write( file, cuT( "bitangents" ), it.bitangents );
						if ( result && !it.texcoords0.empty() )
							result = write( file, cuT( "texcoords0" ), it.texcoords0 );
						if ( result && !it.texcoords1.empty() )
							result = write( file, cuT( "texcoords1" ), it.texcoords1 );
						if ( result && !it.texcoords2.empty() )
							result = write( file, cuT( "texcoords2" ), it.texcoords2 );
						if ( result && !it.texcoords3.empty() )
							result = write( file, cuT( "texcoords3" ), it.texcoords3 );
						if ( result && !it.colours.empty() )
							result = write( file, cuT( "colours" ), it.colours );
					}
				}
			}

			return result;
		}

		bool operator()( Submesh const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, cuT( "submesh" ) ) } )
			{
				result = true;
				if ( result && object.hasComponent( PositionsComponent::TypeName ) )
					result = write( file, cuT( "positions" ), object.getComponent< PositionsComponent >()->getData().getData() );
				if ( result && object.hasComponent( NormalsComponent::TypeName ) )
					result = write( file, cuT( "normals" ), object.getComponent< NormalsComponent >()->getData().getData() );
				if ( result && object.hasComponent( TangentsComponent::TypeName ) )
					result = write( file, cuT( "tangents" ), object.getComponent< TangentsComponent >()->getData().getData() );
				if ( result && object.hasComponent( BitangentsComponent::TypeName ) )
					result = write( file, cuT( "bitangents" ), object.getComponent< BitangentsComponent >()->getData().getData() );
				if ( result && object.hasComponent( Texcoords0Component::TypeName ) )
					result = write( file, cuT( "texcoords0" ), object.getComponent< Texcoords0Component >()->getData().getData() );
				if ( result && object.hasComponent( Texcoords1Component::TypeName ) )
					result = write( file, cuT( "texcoords1" ), object.getComponent< Texcoords1Component >()->getData().getData() );
				if ( result && object.hasComponent( Texcoords2Component::TypeName ) )
					result = write( file, cuT( "texcoords2" ), object.getComponent< Texcoords2Component >()->getData().getData() );
				if ( result && object.hasComponent( Texcoords3Component::TypeName ) )
					result = write( file, cuT( "texcoords3" ), object.getComponent< Texcoords3Component >()->getData().getData() );
				if ( result && object.hasComponent( ColoursComponent::TypeName ) )
					result = write( file, cuT( "colours" ), object.getComponent< ColoursComponent >()->getData().getData() );
				if ( result && object.hasComponent( TriFaceMapping::TypeName ) )
					result = write( file, cuT( "faces" ), object.getComponent< TriFaceMapping >()->getData().getFaces() );
				if ( result && object.hasComponent( LineMapping::TypeName ) )
					result = write( file, cuT( "lines" ), object.getComponent< LineMapping >()->getData().getFaces() );
				if ( result && object.hasComponent( SkinComponent::TypeName ) )
					result = write( file, cuT( "skin" ), object.getComponent< SkinComponent >()->getData().getData() );
				if ( result && object.hasComponent( MorphComponent::TypeName ) )
					result = write( file, cuT( "morph_targets" ), object.getComponent< MorphComponent >()->getData().getMorphTargetsBuffers() );
			}

			return result;
		}
	};

	TextWriter< Mesh >::TextWriter( String const & tabs
		, String const & subfolder
		, bool forceText )
		: TextWriterT< Mesh >{ tabs }
		, m_subfolder{ subfolder }
		, m_forceText{ forceText }
	{
	}

	bool TextWriter< Mesh >::operator()( Mesh const & object
		, StringStream & file )
	{
		log::info << tabs() << cuT( "Writing Mesh " ) << object.getName() << std::endl;
		bool result{ false };

		if ( auto block{ beginBlock( file, cuT( "mesh" ), object.getName() ) } )
		{
			if ( m_forceText )
			{
				result = true;
				TextWriter< Submesh > writer{ tabs() };
				for ( auto const & submesh : object )
					result = result && writer( *submesh, file );

				if ( result )
				{
					TextWriter< MeshAnimation > animWriter{ tabs() };
					for ( auto const & [name, animation] : object.getAnimations() )
						result = result && animWriter( static_cast< MeshAnimation const & >( *animation ), file );
				}
			}
			else
			{
				if ( !m_subfolder.empty() )
					result = writeName( file, cuT( "import" ), cuT( "Meshes/" ) + m_subfolder + cuT( "/" ) + object.getName() + cuT( ".cmsh" ) );
				else
					result = writeName( file, cuT( "import" ), cuT( "Meshes/" ) + object.getName() + cuT( ".cmsh" ) );

				for ( auto const & [name, _] : object.getAnimations() )
				{
					if ( !m_subfolder.empty() )
						result = result && writeName( file, cuT( "import_anim" ), cuT( "Meshes/" ) + m_subfolder + cuT( "/" ) + object.getName() + cuT( "-" ) + name + cuT( ".cmsa" ) );
					else
						result = result && writeName( file, cuT( "import_anim" ), cuT( "Meshes/" ) + object.getName() + cuT( "-" ) + name + cuT( ".cmsa" ) );
				}
			}

			if ( auto skeleton = object.getSkeleton() )
			{
				result = result && writeName( file, cuT( "skeleton" ), skeleton->getName() );
			}

			auto it = std::find_if( object.begin()
				, object.end()
				, []( SubmeshUPtr const & lookup )
				{
					return lookup->getDefaultMaterial() != nullptr;
				} );

			if ( result && it != object.end() )
			{
				if ( object.getSubmeshCount() == 1 )
					result = writeName( file, cuT( "default_material" ), object.getSubmesh( 0u )->getDefaultMaterial()->getName() );
				else if ( auto matsBlock{ beginBlock( file, cuT( "default_materials" ) ) } )
					for ( auto & submesh : object )
					{
						if ( submesh->getDefaultMaterial() )
						{
							result = writeText( file, tabs() + cuT( "material " ) + string::toString( submesh->getId() ) + cuT( " \"" ) + submesh->getDefaultMaterial()->getName() + cuT( "\"\n" ) );
						}
					}
			}
		}

		return result;
	}
}
