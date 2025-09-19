#include "Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp"

#include "Castor3D/Binary/ChunkWriter.hpp"
#include "Castor3D/Buffer/GpuBuffer.hpp"
#include "Castor3D/Buffer/GpuBufferPool.hpp"
#include "Castor3D/Buffer/UploadData.hpp"
#include "Castor3D/Model/Mesh/Submesh/Submesh.hpp"
#include "Castor3D/Model/Vertex.hpp"
#include "Castor3D/Miscellaneous/makeVkType.hpp"
#include "Castor3D/Render/RenderDevice.hpp"
#include "Castor3D/Render/RenderNodesPass.hpp"
#include "Castor3D/Scene/Scene.hpp"

#include <CastorUtils/FileParser/FileParser.hpp>
#include <CastorUtils/Miscellaneous/Hash.hpp>

#include <ashespp/Buffer/VertexBuffer.hpp>

namespace c3d
{
	//*********************************************************************************************

	template<>
	class TextWriter< Vector< Point3f > >
		: public TextWriterT< Vector< Point3f > >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & sectionName )
			: TextWriterT< Vector< Point3f > >{ tabs }
			, m_sectionName{ sectionName }
		{
		}

		bool operator()( Vector< Point3f > const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, m_sectionName ) } )
			{
				result = true;
				for ( auto & value : object )
				{
					StringStream stream;
					stream << value[0] << " " << value[1] << " " << value[2];
					result = result && write( file, cuT( "value " ), stream.str() );
				}
			}

			return result;
		}

	private:
		String m_sectionName;
	};

	template<>
	class TextWriter< Vector< Point4f > >
		: public TextWriterT< Vector< Point4f > >
	{
	public:
		explicit TextWriter( String const & tabs
			, String const & sectionName )
			: TextWriterT< Vector< Point4f > >{ tabs }
			, m_sectionName{ sectionName }
		{
		}

		bool operator()( Vector< Point4f > const & object
			, StringStream & file )override
		{
			bool result{ false };

			if ( auto block{ beginBlock( file, m_sectionName ) } )
			{
				result = true;
				for ( auto & value : object )
				{
					StringStream stream;
					stream << value[0] << " " << value[1] << " " << value[2] << " " << value[3];
					result = result && write( file, cuT( "value " ), stream.str() );
				}
			}

			return result;
		}

	private:
		String m_sectionName;
	};

	//*********************************************************************************************

	namespace smshbase
	{
		static ashes::PipelineVertexInputStateCreateInfo createVertexLayout( SubmeshData submeshData
			, uint32_t & currentBinding
			, uint32_t & currentLocation )
		{
			ashes::VkVertexInputBindingDescriptionArray bindings{ { currentBinding
				, sizeof( Point4f ), VK_VERTEX_INPUT_RATE_VERTEX } };
			ashes::VkVertexInputAttributeDescriptionArray attributes{ 1u, { currentLocation++
				, currentBinding
				, ( ( submeshData == SubmeshData::ePositions || submeshData == SubmeshData::eTangents )
					? VK_FORMAT_R32G32B32A32_SFLOAT
					: VK_FORMAT_R32G32B32_SFLOAT )
				, 0u } };
			++currentBinding;
			return ashes::PipelineVertexInputStateCreateInfo{ 0u, bindings, attributes };
		}

		static Point4fArray convert( Point3fArray const & src )
		{
			Point4fArray result;
			result.reserve( src.size() );

			for ( auto & value : src )
			{
				result.push_back( Point4f{ value->x, value->y, value->z, 1.0f } );
			}

			return result;
		}

		enum class BaseDataSection
			: SectionId
		{
			ePositions = makeSectionName( 'S', 'M', 'S', 'H', 'P', 'O', 'S', 'N' ),
			eNormals = makeSectionName( 'S', 'M', 'S', 'H', 'N', 'R', 'M', 'L' ),
			eTangents = makeSectionName( 'S', 'M', 'S', 'H', 'T', 'N', 'G', 'T' ),
			eBitangents = makeSectionName( 'S', 'M', 'S', 'H', 'B', 'T', 'G', 'T' ),
			eTexcoords0 = makeSectionName( 'S', 'M', 'S', 'H', 'T', 'E', 'X', '0' ),
			eTexcoords1 = makeSectionName( 'S', 'M', 'S', 'H', 'T', 'E', 'X', '1' ),
			eTexcoords2 = makeSectionName( 'S', 'M', 'S', 'H', 'T', 'E', 'X', '2' ),
			eTexcoords3 = makeSectionName( 'S', 'M', 'S', 'H', 'T', 'E', 'X', '3' ),
			eColours = makeSectionName( 'S', 'M', 'S', 'H', 'C', 'O', 'L', 'R' ),
		};

		template< typename DataT >
		struct BaseDataContext
		{
			SubmeshContext * submesh;
			Vector< DataT > values;
		};

		template< typename DataT, BaseDataSection SectionIdT >
		static CU_ImplementAttributeParserNewBlock( parserSection, SubmeshContext, BaseDataContext<DataT> )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
			{
				newBlockContext->submesh = blockContext;
			}
		}
		CU_EndAttributePushNewBlock( SectionIdT )

		template< typename DataT >
		static CU_ImplementAttributeParserBlock( parserValue, BaseDataContext<DataT> )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else
				params[0]->get( blockContext->values.emplace_back() );
		}
		CU_EndAttribute()

		template< typename ComponentT >
		static CU_ImplementAttributeParserBlock( parserSubmeshComponentValue, SubmeshContext )
		{
			using DataT = typename ComponentT::Data;
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( auto component = blockContext->submesh->createComponent< ComponentT >() )
				params[0]->get( component->getData().getData().emplace_back() );
		}
		CU_EndAttribute()

		static CU_ImplementAttributeParserBlock( parserUV, SubmeshContext )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( auto component = blockContext->submesh->createComponent< Texcoords0Component >() )
				component->getData().getData().emplace_back( params[0]->get< Point2f >() );
		}
		CU_EndAttribute()

		template< typename DataT, typename ComponentT >
		static CU_ImplementAttributeParserBlock( parserEnd, BaseDataContext<DataT> )
		{
			if ( !blockContext->submesh )
				CU_ParsingError( cuT( "No submesh initialised." ) );
			else if ( !blockContext->values.empty() )
			{
				if ( auto component = blockContext->submesh->submesh->createComponent< ComponentT >() )
					component->getData().setData( move( blockContext->values ) );
			}
		}
		CU_EndAttributePop()

		template< typename ComponentT, BaseDataSection SectionIdT, ParameterType ValueT >
		static void writeParser( AttributeParsers & result
			, String const & sectionName )
		{
			using DataT = typename ComponentT::Data;

			BlockParserContextT< SubmeshContext > submeshContext{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };
			BlockParserContextT< BaseDataContext< DataT > > sectionContext{ result, SectionIdT, CSCNSection::eSubmesh };

			submeshContext.addPushParser( sectionName, SectionIdT, parserSection< DataT, SectionIdT > );

			sectionContext.addParser( cuT( "value" ), parserValue< DataT >, { makeParameter< ValueT >() } );
			sectionContext.addPopParser( cuT( "}" ), parserEnd< DataT, ComponentT > );
		}

		template< typename ComponentT, ParameterType ValueT >
		static void writeValueParser( AttributeParsers & result
			, String const & parserName )
		{
			BlockParserContextT< SubmeshContext > submeshContext{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };
			submeshContext.addParser( parserName, parserSubmeshComponentValue< ComponentT >, { makeParameter< ValueT >() } );
		}

		static void writeUVParser( AttributeParsers & result
			, String const & parserName )
		{
			BlockParserContextT< SubmeshContext > submeshContext{ result, CSCNSection::eSubmesh, CSCNSection::eMesh };
			submeshContext.addParser( parserName, parserUV, { makeParameter< ParameterType::ePoint2F >() } );
		}

		template< typename DataT >
		static bool writeText( String const & sectionName
			, Vector< DataT > const & data
			, String const & tabs
			, StringStream & file )
		{
			return TextWriter< Vector< DataT > >{ tabs, sectionName }( data, file );
		}

		template< typename DataT >
		static bool writeBinary( ChunkType chunkType
			, Vector< DataT > const & data
			, BinaryChunk & chunk )
		{
			return ChunkWriter< DataT >::write( data.data(), data.data() + data.size(), chunkType, chunk );
		}
	}

	//*********************************************************************************************

	void uploadBaseData( SubmeshData submeshData
		, Submesh const & submesh
		, Point4fArray const & data
		, Point4fArray &
		, UploadData & uploader )
	{
		auto count = uint32_t( data.size() );
		auto & offsets = submesh.getSourceBufferOffsets();
		auto & buffer = offsets.getBufferChunk( submeshData );

		if ( count && buffer.hasData() )
		{
			uploader.pushUpload( data.data()
				, data.size() * sizeof( Point4f )
				, buffer.getBuffer(), buffer.getOffset()
				, VertexAttributeInputState );
		}
	}

	void uploadBaseData( SubmeshData submeshData
		, Submesh const & submesh
		, Point3fArray const & data
		, Point4fArray & up
		, UploadData & uploader )
	{
		up = smshbase::convert( data );
		uploadBaseData( submeshData
			, submesh
			, up
			, up
			, uploader );
	}

	void gatherBaseDataBuffer( SubmeshData submeshData
		, ObjectBufferOffset const & bufferOffsets
		, PipelineFlags const & flags
		, ashes::BufferCRefArray & buffers
		, Vector< uint64_t > & offsets
		, ashes::PipelineVertexInputStateCreateInfoCRefArray & layouts
		, uint32_t & currentBinding
		, uint32_t & currentLocation
		, HashMap< size_t, ashes::PipelineVertexInputStateCreateInfo > & cache )
	{
		auto & bufferChunk = bufferOffsets.getBufferChunk( submeshData );

		if ( bufferChunk.hasData()
			&& flags.enableVertexInput( submeshData ) )
		{
			auto hash = std::hash< uint32_t >{}( currentBinding );
			hash = hashCombine( hash, currentLocation );
			auto layoutIt = cache.find( hash );

			if ( layoutIt == cache.end() )
			{
				layoutIt = cache.try_emplace( hash
					, smshbase::createVertexLayout( submeshData
						, currentBinding
						, currentLocation ) ).first;
			}
			else
			{
				currentLocation = layoutIt->second.vertexAttributeDescriptions.back().location + 1u;
				currentBinding = layoutIt->second.vertexAttributeDescriptions.back().binding + 1u;
			}

			buffers.emplace_back( bufferChunk.getBuffer().getBuffer() );
			offsets.emplace_back( 0u );
			layouts.emplace_back( layoutIt->second );
		}
	}

	void fillBaseSurfaceType( SubmeshData submeshData
		, sdw::type::IOStruct & type
		, uint32_t & index )
	{
		switch (submeshData)
		{
		case SubmeshData::ePositions:
			type.declMember( "position", ast::type::Kind::eVec4F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eNormals:
			type.declMember( "normal", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTangents:
			type.declMember( "tangent", ast::type::Kind::eVec4F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eBitangents:
			type.declMember( "bitangent", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTexcoords0:
			type.declMember( "texture0", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTexcoords1:
			type.declMember( "texture1", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTexcoords2:
			type.declMember( "texture2", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eTexcoords3:
			type.declMember( "texture3", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::eColours:
			type.declMember( "colour", ast::type::Kind::eVec3F, ast::type::NotArray, index );
			++index;
			break;
		case SubmeshData::ePassMasks:
			type.declMember( "passMasks", ast::type::Kind::eVec4U, ast::type::NotArray, index );
			++index;
			break;
		default:
			break;
		}
	}

	void fillBaseSurfaceType( SubmeshData submeshData
		, sdw::type::BaseStruct & type )
	{
		switch (submeshData)
		{
		case SubmeshData::ePositions:
			type.declMember( "position", ast::type::Kind::eVec4F, ast::type::NotArray );
			break;
		case SubmeshData::eNormals:
			type.declMember( "normal", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTangents:
			type.declMember( "tangent", ast::type::Kind::eVec4F, ast::type::NotArray );
			break;
		case SubmeshData::eBitangents:
			type.declMember( "bitangent", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTexcoords0:
			type.declMember( "texture0", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTexcoords1:
			type.declMember( "texture1", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTexcoords2:
			type.declMember( "texture2", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eTexcoords3:
			type.declMember( "texture3", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::eColours:
			type.declMember( "colour", ast::type::Kind::eVec3F, ast::type::NotArray );
			break;
		case SubmeshData::ePassMasks:
			type.declMember( "passMasks", ast::type::Kind::eVec4U, ast::type::NotArray );
			break;
		default:
			break;
		}
	}

	String getBaseDataComponentName( SubmeshData submeshData )
	{
		return cuT( "c3d.submesh." ) + string::lowerCase( getName( submeshData ) );
	}

	void createBaseDataParsers( SubmeshData submeshData
		, AttributeParsers & result )
	{
		switch ( submeshData )
		{
		case SubmeshData::ePositions:
			smshbase::writeParser< PositionsComponent, smshbase::BaseDataSection::ePositions, ParameterType::ePoint3F >( result, "positions" );
			smshbase::writeValueParser< PositionsComponent, ParameterType::ePoint3F >( result, "position" );
			smshbase::writeValueParser< PositionsComponent, ParameterType::ePoint3F >( result, "vertex" );
			break;
		case SubmeshData::eNormals:
			smshbase::writeParser< NormalsComponent, smshbase::BaseDataSection::eNormals, ParameterType::ePoint3F >( result, "normals" );
			smshbase::writeValueParser< NormalsComponent, ParameterType::ePoint3F >( result, "normal" );
			break;
		case SubmeshData::eTangents:
			smshbase::writeParser< TangentsComponent, smshbase::BaseDataSection::eTangents, ParameterType::ePoint4F >( result, "tangents" );
			smshbase::writeValueParser< TangentsComponent, ParameterType::ePoint4F >( result, "tangent" );
			break;
		case SubmeshData::eBitangents:
			smshbase::writeParser< BitangentsComponent, smshbase::BaseDataSection::eBitangents, ParameterType::ePoint3F >( result, "bitangents" );
			smshbase::writeValueParser< BitangentsComponent, ParameterType::ePoint3F >( result, "bitangent" );
			break;
		case SubmeshData::eTexcoords0:
			smshbase::writeParser< Texcoords0Component, smshbase::BaseDataSection::eTexcoords0, ParameterType::ePoint3F >( result, "texcoords0" );
			smshbase::writeUVParser( result, "uv" );
			smshbase::writeValueParser< Texcoords0Component, ParameterType::ePoint3F >( result, "uvw" );
			smshbase::writeValueParser< Texcoords0Component, ParameterType::ePoint3F >( result, "texcoord0" );
			break;
		case SubmeshData::eTexcoords1:
			smshbase::writeParser< Texcoords1Component, smshbase::BaseDataSection::eTexcoords1, ParameterType::ePoint3F >( result, "texcoords1" );
			smshbase::writeValueParser< Texcoords1Component, ParameterType::ePoint3F >( result, "texcoord1" );
			break;
		case SubmeshData::eTexcoords2:
			smshbase::writeParser< Texcoords2Component, smshbase::BaseDataSection::eTexcoords2, ParameterType::ePoint3F >( result, "texcoords2" );
			smshbase::writeValueParser< Texcoords2Component, ParameterType::ePoint3F >( result, "texcoord2" );
			break;
		case SubmeshData::eTexcoords3:
			smshbase::writeParser< Texcoords3Component, smshbase::BaseDataSection::eTexcoords3, ParameterType::ePoint3F >( result, "texcoords3" );
			smshbase::writeValueParser< Texcoords3Component, ParameterType::ePoint3F >( result, "texcoord3" );
			break;
		case SubmeshData::eColours:
			smshbase::writeParser< ColoursComponent, smshbase::BaseDataSection::eColours, ParameterType::ePoint3F >( result, "colours" );
			smshbase::writeValueParser< ColoursComponent, ParameterType::ePoint3F >( result, "colour" );
			break;
		default:
			break;
		}
	}

	void createBaseDataSections( SubmeshData submeshData
		, StrSectionIdMap & result )
	{
		switch ( submeshData )
		{
		case SubmeshData::ePositions:
			result.try_emplace( SectionId( smshbase::BaseDataSection::ePositions ), "positions" );
			break;
		case SubmeshData::eNormals:
			result.try_emplace( SectionId( smshbase::BaseDataSection::eNormals ), "normals" );
			break;
		case SubmeshData::eTangents:
			result.try_emplace( SectionId( smshbase::BaseDataSection::eTangents ), "tangents" );
			break;
		case SubmeshData::eBitangents:
			result.try_emplace( SectionId( smshbase::BaseDataSection::eBitangents ), "bitangents" );
			break;
		case SubmeshData::eTexcoords0:
			result.try_emplace( SectionId( smshbase::BaseDataSection::eTexcoords0 ), "texcoords0" );
			break;
		case SubmeshData::eTexcoords1:
			result.try_emplace( SectionId( smshbase::BaseDataSection::eTexcoords1 ), "texcoords1" );
			break;
		case SubmeshData::eTexcoords2:
			result.try_emplace( SectionId( smshbase::BaseDataSection::eTexcoords2 ), "texcoords2" );
			break;
		case SubmeshData::eTexcoords3:
			result.try_emplace( SectionId( smshbase::BaseDataSection::eTexcoords3 ), "texcoords3" );
			break;
		case SubmeshData::eColours:
			result.try_emplace( SectionId( smshbase::BaseDataSection::eColours ), "colours" );
			break;
		default:
			break;
		}
	}

	bool writeBaseDataText( SubmeshData submeshData
		, Point3fArray const & data
		, String const & tabs
		, StringStream & file )
	{
		switch ( submeshData )
		{
		case SubmeshData::ePositions:
			return smshbase::writeText( "positions", data, tabs, file );
		case SubmeshData::eNormals:
			return smshbase::writeText( "normals", data, tabs, file );
		case SubmeshData::eBitangents:
			return smshbase::writeText( "bitangents", data, tabs, file );
		case SubmeshData::eTexcoords0:
			return smshbase::writeText( "texcoords0", data, tabs, file );
		case SubmeshData::eTexcoords1:
			return smshbase::writeText( "texcoords1", data, tabs, file );
		case SubmeshData::eTexcoords2:
			return smshbase::writeText( "texcoords2", data, tabs, file );
		case SubmeshData::eTexcoords3:
			return smshbase::writeText( "texcoords3", data, tabs, file );
		case SubmeshData::eColours:
			return smshbase::writeText( "colours", data, tabs, file );
		default:
			CU_UnsupportedError( "Upsupported submesh data type for Point3f" );
		}
	}

	bool writeBaseDataText( SubmeshData submeshData
		, Point4fArray const & data
		, String const & tabs
		, StringStream & file )
	{
		if ( submeshData != SubmeshData::eTangents)
			CU_UnsupportedError( "Upsupported submesh data type for Point4f" );

		return smshbase::writeText( "tangents", data, tabs, file );
	}

	bool writeBaseDataBinary( SubmeshData submeshData
		, Point3fArray const & data
		, BinaryChunk & chunk )
	{
		switch ( submeshData )
		{
		case SubmeshData::ePositions:
			return smshbase::writeBinary( ChunkType::eSubmeshPositions, data, chunk );
		case SubmeshData::eNormals:
			return smshbase::writeBinary( ChunkType::eSubmeshNormals, data, chunk );
		case SubmeshData::eBitangents:
			return smshbase::writeBinary( ChunkType::eSubmeshBitangents, data, chunk );
		case SubmeshData::eTexcoords0:
			return smshbase::writeBinary( ChunkType::eSubmeshTexcoords0, data, chunk );
		case SubmeshData::eTexcoords1:
			return smshbase::writeBinary( ChunkType::eSubmeshTexcoords1, data, chunk );
		case SubmeshData::eTexcoords2:
			return smshbase::writeBinary( ChunkType::eSubmeshTexcoords2, data, chunk );
		case SubmeshData::eTexcoords3:
			return smshbase::writeBinary( ChunkType::eSubmeshTexcoords3, data, chunk );
		case SubmeshData::eColours:
			return smshbase::writeBinary( ChunkType::eSubmeshColours, data, chunk );
		default:
			CU_UnsupportedError( "Upsupported submesh data type for Point3f" );
		}
	}

	bool writeBaseDataBinary( SubmeshData submeshData
		, Point4fArray const & data
		, BinaryChunk & chunk )
	{
		if ( submeshData != SubmeshData::eTangents )
			CU_UnsupportedError( "Upsupported submesh data type for Point4f" );

		return smshbase::writeBinary( ChunkType::eSubmeshTangentsMikkt, data, chunk );
	}

	//*********************************************************************************************
}
