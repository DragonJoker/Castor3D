#include "Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp"

#include "Castor3D/Engine.hpp"
#include "Castor3D/Binary/BinarySkinComponent.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Skeleton/BonedVertex.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Render/RenderSystem.hpp"
#include "Castor3D/Scene/Scene.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

CU_ImplementSmartPtr( c3d, SkinComponent )

namespace c3d
{
	//*********************************************************************************************

	template<>
	class TextWriter< SkinComponent >
		: public TextWriterT< SkinComponent >
	{
	public:
		explicit TextWriter( String const & tabs )
			: TextWriterT< SkinComponent >{ tabs }
		{
		}

		bool operator()( SkinComponent const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, "skin" ) } )
			{
				result = true;
				for ( auto & value : object.getData().getData() )
				{
					if ( auto boneBlock{ beginBlock( file, "vertex_bone_data" ) } )
					{
						for ( uint32_t i = 0; i < value.m_ids.size(); ++i )
						{
							if ( value.m_weights[i] != 0.0f )
							{
								StringStream stream;
								stream << value.m_ids[i] << " " << value.m_weights[i];
								result = result && write( file, cuT( "bone_weight " ), stream.str() );
							}
						}
					}
				}
			}

			return result;
		}
	};

	//*********************************************************************************************

	namespace smshskincmp
	{
		static constexpr SectionId sectionSkinId = makeSectionName( 'S', 'M', 'S', 'H', 'S', 'K', 'I', 'N' );
		static constexpr SectionId sectionDataId = makeSectionName( 'S', 'M', 'H', 'S', 'K', 'D', 'A', 'T' );

		struct SkinContext
		{
			SubmeshContext * submesh;
			VertexBoneData data;
			VertexBoneDataArray values;
		};

		static CU_ImplementAttributeParserNewBlock( parserSkin, SubmeshContext, SkinContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
				newBlockContext->submesh = blockContext;
		}
		CU_EndAttributePushNewBlock( sectionSkinId )

		static CU_ImplementAttributeParserBlock( parserVertexData, SkinContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
				blockContext->data = {};
		}
		CU_EndAttributePushBlock( sectionDataId, blockContext )

		static CU_ImplementAttributeParserBlock( parserValue, SkinContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
				blockContext->data.addBoneData( params[0]->get< u32 >(), params[1]->get< f32 >() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserVertexDataEnd, SkinContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
				blockContext->values.emplace_back( move( blockContext->data ) );
		}
		CU_EndAttributePop()

		static CU_ImplementAttributeParserBlock( parserEnd, SkinContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( !blockContext->values.empty() )
			{
				if ( auto component = blockContext->submesh->submesh->createComponent< SkinComponent >() )
					component->getData().addDatas( blockContext->values );
			}
		}
		CU_EndAttributePop()
	}

	//*********************************************************************************************

	void SkinComponent::ComponentData::copy( SubmeshComponentDataRPtr data )const
	{
		static_cast< ComponentData * >( data )->m_bones = m_bones;
	}

	void SkinComponent::ComponentData::addDatas( VertexBoneData const * const begin
		, VertexBoneData const * const end )
	{
		m_bones.insert( m_bones.end(), begin, end );
	}

	void SkinComponent::ComponentData::addDatas( Vector< VertexBoneData > const & boneData )
	{
		addDatas( boneData.data(), boneData.data() + boneData.size() );
	}

	bool SkinComponent::ComponentData::doInitialise( RenderDevice const & device )
	{
		return true;
	}

	void SkinComponent::ComponentData::doCleanup( RenderDevice const & device )
	{
	}

	void SkinComponent::ComponentData::doUpload( UploadData & uploader )
	{
		auto count = uint32_t( m_bones.size() );
		auto & offsets = m_submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( SubmeshData::eSkin );

		if ( count && buffer.hasData() )
		{
			CU_Require( buffer.getCount< VertexBoneData >() == count );
			uploader.pushUpload( m_bones.data()
				, m_bones.size() * sizeof( VertexBoneData )
				, buffer.getBuffer(), buffer.getOffset()
				, VertexAttributeInputState );
		}
	}

	//*********************************************************************************************

	String const SkinComponent::TypeName = C3D_MakeSubmeshComponentName( "skin" );

	SkinComponent::SkinComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, TypeName
			, makeRawUnique< ComponentData >( submesh ) }
	{
	}

	SkeletonRPtr SkinComponent::getSkeleton()const
	{
		return getOwner()->getParent().getSkeleton();
	}

	SubmeshComponentUPtr SkinComponent::clone( Submesh & submesh )const
	{
		auto result = makeUnique< SkinComponent >( submesh );
		getData().copy( &result->getData() );
		return ptrRefCast< SubmeshComponent >( result );
	}

	bool SkinComponent::doWriteText( String const & tabs
		, StringStream & file )const
	{
		return TextWriter< SkinComponent >{ tabs }( *this, file );
	}

	bool SkinComponent::doWriteBinary( BinaryChunk & chunk )const
	{
		return BinaryWriter< SkinComponent >{}.write( *this, chunk );
	}

	//*********************************************************************************************

	void SkinComponent::Plugin::createParsers( AttributeParsers & result )const
	{
		BlockParserContextT< SubmeshContext > submeshContext{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };
		BlockParserContextT< smshskincmp::SkinContext > skinContext{ result, smshskincmp::sectionSkinId, CSCNSection::eSubmesh };
		BlockParserContextT< smshskincmp::SkinContext > vertexContext{ result, smshskincmp::sectionDataId, smshskincmp::sectionSkinId };

		submeshContext.addPushParser( "skin", smshskincmp::sectionSkinId, smshskincmp::parserSkin );

		skinContext.addPushParser( cuT( "vertex_bone_data" ), smshskincmp::sectionDataId, smshskincmp::parserVertexData );
		skinContext.addPopParser( cuT( "}" ), smshskincmp::parserEnd );

		vertexContext.addParser( cuT( "bone_weight" ), smshskincmp::parserValue, { makeParameter< ParameterType::eUInt32 >(), makeParameter< ParameterType::eFloat >() } );
		vertexContext.addPopParser( cuT( "}" ), smshskincmp::parserVertexDataEnd );
	}

	void SkinComponent::Plugin::createSections( StrSectionIdMap & sections )const
	{
		sections.try_emplace( SectionId( smshskincmp::sectionSkinId ), "skin" );
		sections.try_emplace( SectionId( smshskincmp::sectionDataId ), "vertex_bone_data" );
	}

	//*********************************************************************************************
}
