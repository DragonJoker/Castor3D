#include "BonesComponent.hpp"

#include "Mesh/Skeleton/BonedVertex.hpp"
#include "Scene/Scene.hpp"
#include "Shader/ShaderProgram.hpp"

using namespace castor;

//*************************************************************************************************

namespace castor3d
{
	//*************************************************************************************************

	bool BinaryWriter< BonesComponent >::doWrite( BonesComponent const & obj )
	{
		bool result = true;

		VertexBuffer const & buffer = obj.getBonesBuffer();
		uint32_t stride = buffer.getDeclaration().stride();
		uint32_t count = buffer.getSize() / stride;
		result = doWriteChunk( count, ChunkType::eSubmeshBoneCount, m_chunk );

		if ( result )
		{
			VertexBoneData const * srcbuf = reinterpret_cast< VertexBoneData const * >( buffer.getData() );
			result = doWriteChunk( srcbuf, buffer.getSize() / sizeof( VertexBoneData ), ChunkType::eSubmeshBones, m_chunk );
		}

		return result;
	}

	//*************************************************************************************************

	bool BinaryParser< BonesComponent >::doParse( BonesComponent & obj )
	{
		bool result = true;
		String name;
		std::vector< VertexBoneData > bones;
		uint32_t count{ 0u };
		uint32_t boneCount{ 0u };
		BinaryChunk chunk;

		while ( result && doGetSubChunk( chunk ) )
		{
			switch ( chunk.getChunkType() )
			{
			case ChunkType::eSubmeshBoneCount:
				result = doParseChunk( count, chunk );

				if ( result )
				{
					boneCount = count;
					bones.resize( count );
				}

				break;

			case ChunkType::eSubmeshBones:
				result = doParseChunk( bones, chunk );

				if ( result && boneCount > 0 )
				{
					obj.addBoneDatas( bones );
				}

				boneCount = 0u;
				break;

			default:
				result = false;
				break;
			}
		}

		return result;
	}

	//*************************************************************************************************

	String const BonesComponent::Name = cuT( "bones" );

	BonesComponent::BonesComponent( Submesh & submesh )
		: SubmeshComponent{ submesh, cuT( "bones" ) }
		, m_bonesBuffer{ *submesh.getScene()->getEngine()
		, BufferDeclaration
		{
			{
				BufferElementDeclaration{ ShaderProgram::BoneIds0, uint32_t( ElementUsage::eBoneIds0 ), ElementType::eIVec4, 0 },
				BufferElementDeclaration{ ShaderProgram::BoneIds1, uint32_t( ElementUsage::eBoneIds1 ), ElementType::eIVec4, 16 },
				BufferElementDeclaration{ ShaderProgram::Weights0, uint32_t( ElementUsage::eBoneWeights0 ), ElementType::eVec4, 32 },
				BufferElementDeclaration{ ShaderProgram::Weights1, uint32_t( ElementUsage::eBoneWeights1 ), ElementType::eVec4, 48 },
			}
		} }
	{
	}

	BonesComponent::~BonesComponent()
	{
		cleanup();
		m_bones.clear();
		m_bonesData.clear();
	}

	void BonesComponent::addBoneDatas( VertexBoneData const * const begin
		, VertexBoneData const * const end )
	{
		uint32_t stride = BonedVertex::Stride;
		m_bonesData.emplace_back( std::distance( begin, end ) * stride );
		auto data = &( *m_bonesData.rbegin() )[0];

		for ( VertexBoneData const & boneData : makeArrayView( begin, end ) )
		{
			auto bonesData = std::make_shared< BufferElementGroup >( data, uint32_t( m_bones.size() ) );
			BonedVertex::setBones( bonesData, boneData );
			m_bones.push_back( bonesData );
			data += stride;
		}
	}

	void BonesComponent::gather( VertexBufferArray & buffers )
	{
		buffers.emplace_back( m_bonesBuffer );
	}

	bool BonesComponent::doInitialise()
	{
		return true;
	}

	void BonesComponent::doCleanup()
	{
		m_bonesBuffer.cleanup();
	}

	void BonesComponent::doFill()
	{
		uint32_t stride = BonedVertex::Stride;
		uint32_t size = uint32_t( m_bones.size() ) * stride;
		auto itbones = m_bones.begin();

		if ( size )
		{
			if ( m_bonesBuffer.getSize() != size )
			{
				m_bonesBuffer.resize( size );
			}

			auto buffer = m_bonesBuffer.getData();

			for ( auto it : m_bonesData )
			{
				std::memcpy( buffer, it.data(), it.size() );
				buffer += it.size();
			}

			buffer = m_bonesBuffer.getData();

			for ( auto point : m_bones )
			{
				point->linkCoords( buffer );
				buffer += stride;
			}

			//m_bones.clear();
			//m_bonesData.clear();
		}

		m_bonesBuffer.initialise( BufferAccessType::eDynamic
			, BufferAccessNature::eDraw );
	}

	void BonesComponent::doUpload()
	{
		m_bonesBuffer.upload();
	}
}
