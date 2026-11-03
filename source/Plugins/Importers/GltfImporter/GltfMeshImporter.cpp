#include "GltfImporter/GltfMeshImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Model/Mesh/Mesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Submesh.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/BaseDataComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/DefaultRenderComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/SkinComponent.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/MorphComponent.hpp>
#include <Castor3D/Model/Skeleton/BoneNode.hpp>
#include <Castor3D/Model/Skeleton/Skeleton.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include "CastorUtils/Config/BeginExternHeaderGuard.hpp"
#include <draco/compression/decode.h>
#include "CastorUtils/Config/EndExternHeaderGuard.hpp"

namespace c3d_gltf
{
	//*********************************************************************************************

	namespace meshes
	{
		template< typename IndexT >
		static void parseLineList( fastgltf::Asset const & impAsset
			, fastgltf::Accessor const & impAccessor
			, c3d::LineMapping const & mapping
			, CompressedBufferDataAdapter const & adapter )
		{
			auto count = impAccessor.count;
			auto lineCount = count / 2u;
			c3d::Vector< c3d::LineIndices > indicesGroup;
			indicesGroup.reserve( lineCount );
			c3d::LineIndices curIndices;
			uint32_t idx{};

			iterateAccessor< IndexT >( impAsset
				, impAccessor
				, [&curIndices, &indicesGroup, &idx]( IndexT value )
				{
					curIndices[idx] = value;
					++idx;

					if ( idx == 2u )
					{
						idx = 0u;
						indicesGroup.push_back( curIndices );
					}
				}
				, adapter );
			mapping.getData().addLineGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
		}

		template< typename IndexT >
		static void parseLineStrip( fastgltf::Asset const & impAsset
			, fastgltf::Accessor const & impAccessor
			, uint32_t lineCount
			, c3d::LineMapping const & mapping
			, bool loop
			, CompressedBufferDataAdapter const & adapter )
		{
			if ( !loop )
			{
				--lineCount;
			}

			c3d::Vector< c3d::LineIndices > indicesGroup;
			indicesGroup.reserve( lineCount );
			c3d::LineIndices curIndices;
			uint32_t prvIndex{ ~0u };

			iterateAccessor< IndexT >( impAsset
				, impAccessor
				, [&curIndices, &indicesGroup, &prvIndex]( IndexT value )
				{
					if ( prvIndex == ~0u )
					{
						prvIndex = value;
					}
					else
					{
						curIndices[0u] = prvIndex;
						curIndices[1u] = value;
						indicesGroup.push_back( curIndices );
						prvIndex = value;
					}
				}
				, adapter );

			if ( loop )
			{
				curIndices[0u] = indicesGroup.back()[1u];
				curIndices[1u] = indicesGroup.front()[0u];
				indicesGroup.push_back( curIndices );
			}

			mapping.getData().addLineGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
		}

		template< typename IndexT >
		static void parseTriangleList( fastgltf::Asset const & impAsset
			, fastgltf::Accessor const & impAccessor
			, c3d::TriFaceMapping const & mapping
			, CompressedBufferDataAdapter const & adapter )
		{
			auto count = impAccessor.count;
			auto faceCount = count / 3u;
			c3d::Vector< c3d::FaceIndices > indicesGroup;
			indicesGroup.reserve( faceCount );
			c3d::FaceIndices curIndices;
			uint32_t idx{};

			iterateAccessor< IndexT >( impAsset
				, impAccessor
				, [&curIndices, &indicesGroup, &idx]( IndexT value )
				{
					curIndices[idx] = value;
					++idx;

					if ( idx == 3u )
					{
						idx = 0u;
						c3d::swap( curIndices[0], curIndices[1] );
						indicesGroup.push_back( curIndices );
					}
				}
				, adapter );
			mapping.getData().addFaceGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
		}

		template< typename IndexT, bool IsStripT >
		static void parseTriangleStrip( fastgltf::Asset const & impAsset
			, fastgltf::Accessor const & impAccessor
			, c3d::TriFaceMapping const & mapping
			, CompressedBufferDataAdapter const & adapter )
		{
			auto count = impAccessor.count;
			c3d::UInt32Array indices;
			indices.reserve( count );
			iterateAccessor< IndexT >( impAsset
				, impAccessor
				, [&indices]( IndexT value )
				{
					indices.push_back( value );
				}
				, adapter );

			auto faceCount = count - 2u;
			c3d::Vector< c3d::FaceIndices > indicesGroup;
			indicesGroup.reserve( faceCount );
			c3d::FaceIndices curIndices;

			if constexpr ( IsStripT )
			{
				for ( size_t i = 0u; i < faceCount; ++i )
				{
					// The ordering is to ensure that the triangles are all drawn with the same orientation
					if ( ( i + 1 ) % 2 == 0 )
					{
						// For even n, vertices n, n+1, and n+2 define triangle n
						curIndices.m_index = { indices[i], indices[i + 1], indices[i + 2] };
					}
					else
					{
						// For odd n, vertices n + 1, n, and n + 2 define triangle n
						curIndices.m_index = { indices[i + 1], indices[i], indices[i + 2] };
					}

					indicesGroup.push_back( curIndices );
				}
			}
			else
			{
				curIndices.m_index = { indices[1], indices[0], indices[2] };
				indicesGroup.push_back( curIndices );

				for ( size_t i = 1u; i < faceCount; ++i )
				{
					curIndices.m_index = { indices[i + 1], indices[0], indices[i + 2] };
					indicesGroup.push_back( curIndices );
				}
			}

			mapping.getData().addFaceGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
		}

		static auto findAttribute( auto const & attributes
			, std::string const & name )
		{
			return std::find_if( std::begin( attributes ), std::end( attributes )
				, [&name]( auto const & lookup )
				{
					return lookup.name == name;
				} );
		}

		static bool hasAttribute( auto const & attributes
			, std::string const & name )
		{
			return findAttribute( attributes, name ) != attributes.end();
		}

		static bool hasAttribute( fastgltf::boxed_optional< fastgltf::DracoCompressedPrimitive > const & dracoCompression
			, std::string const & name )
		{
			return dracoCompression
				&& hasAttribute( dracoCompression->attributes, name );
		}

		template< uint32_t SrcCountT, typename SrcDataT, uint32_t DstCountT, typename DstDataT, bool InvYT = false >
		static bool parseAttributeData( fastgltf::Asset const & impAsset
			, auto const & impAttributes
			, std::string const & attrName
			, c3d::Vector< c3d::Point< DstDataT, DstCountT > > & result
			, CompressedBufferDataAdapter const & adapter )
		{
			auto it = findAttribute( impAttributes, attrName );
			if ( it == impAttributes.end() )
				return false;

			auto & impAccessor = impAsset.accessors[it->accessorIndex];
			if ( impAccessor.type != fastgltf::ElementTraits< c3d::Point< SrcDataT, SrcCountT > >::type )
				return false;

			result.reserve( impAccessor.count );
			iterateAccessor< c3d::Point< SrcDataT, SrcCountT > >( impAsset
				, impAccessor
				, [&result]( c3d::Point< SrcDataT, SrcCountT > value )
				{
					if constexpr ( InvYT )
						value->y = 1.0f - value->y;
					if constexpr ( SrcCountT == DstCountT && std::is_same_v< SrcDataT, DstDataT > )
						result.push_back( c3d::move( value ) );
					else
						result.push_back( c3d::Point < DstDataT, DstCountT >{ value } );
				}
				, adapter );
			return !result.empty();
		}

		struct DracoDecoder
		{
			c3d::Vector< std::optional< fastgltf::static_vector< std::byte > > > decompressedBuffers;

			/** Decompress all buffer views and store them in this adapter */
			DracoDecoder( fastgltf::Asset const & asset
				, fastgltf::boxed_optional< fastgltf::DracoCompressedPrimitive > const & dracoCompression )
			{
				if ( dracoCompression )
				{
					fastgltf::BufferView const & bufferView = asset.bufferViews[dracoCompression->bufferView];
					fastgltf::Buffer const & buffer = asset.buffers[bufferView.bufferIndex];
					std::span< std::byte const > compressedBytes = CompressedBufferDataAdapter::getData( buffer, bufferView.byteOffset, bufferView.byteLength );
					draco::DecoderBuffer decoderBuffer{};
					decoderBuffer.Init( reinterpret_cast< const char * >( compressedBytes.data() ), compressedBytes.size() );
					auto geometryType = draco::Decoder::GetEncodedGeometryType( &decoderBuffer);
					if ( !geometryType.ok() )
						CU_LoaderError( "Couldn't retrieve the encoded geometry type" );
					if ( geometryType.value() != draco::EncodedGeometryType::TRIANGULAR_MESH )
						CU_LoaderError( "The encoded geometry type is not a triangular mesh" );

					draco::Decoder decoder{};
					auto res = decoder.DecodeBufferToGeometry( &decoderBuffer, &m_mesh );
					if ( !res.ok() )
						CU_LoaderError( "Couldn't decode the buffer into a mesh" );
				}
			}

			draco::PointAttribute const * getAttribute( std::size_t attributeId ) const
			{
				return m_mesh.attribute( c3d::s32( attributeId ) );
			}

		private:
			draco::Mesh m_mesh;
		};

		template< uint32_t SrcCountT, typename SrcDataT, uint32_t DstCountT, typename DstDataT, bool InvYT = false >
		static bool decodeAttributeData( fastgltf::Asset const & impAsset
			, fastgltf::boxed_optional< fastgltf::DracoCompressedPrimitive > const & dracoCompression
			, std::string const & attrName
			, c3d::Vector< c3d::Point< DstDataT, DstCountT > > & result
			, DracoDecoder const & adapter )
		{
			using SrcElementType = c3d::Point< SrcDataT, SrcCountT >;
			using DstElementType = c3d::Point< DstDataT, DstCountT >;

			if ( !dracoCompression )
				return false;

			auto it = findAttribute( dracoCompression->attributes, attrName );
			if ( it == dracoCompression->attributes.end() )
				return false;

			auto & accessor = impAsset.accessors[it->accessorIndex];
			if ( accessor.type != fastgltf::ElementTraits< SrcElementType >::type )
				return false;

			auto attribute = adapter.getAttribute( it->accessorIndex );
			if ( !attribute )
				return false;

			CU_Require( attribute->size() == accessor.count );
			result.reserve( std::min( attribute->size(), accessor.count ) );
			std::span< std::byte const > bytes{ std::bit_cast< std::byte const * >( attribute->buffer()->data() )
				, attribute->buffer()->data_size() };
			SrcElementType element;

			for ( std::size_t i = 0; i < result.size(); ++i )
			{
				attribute->ConvertValue( draco::AttributeValueIndex{ c3d::u32( i ) }, c3d::u8( SrcCountT ), element.ptr() );
				if constexpr ( InvYT )
					element->y = 1.0f - element->y;

				if constexpr ( SrcCountT == DstCountT && std::is_same_v< SrcDataT, DstDataT > )
					result.push_back( c3d::move( element ) );
				else
					result.push_back( DstElementType{ element } );
			}

			return !result.empty();
		}

		static void createVertexBuffer( fastgltf::Asset const & impAsset
			, auto const & impAttributes
			, fastgltf::boxed_optional< fastgltf::DracoCompressedPrimitive > const & dracoCompression
			, c3d::Point3fArray & positions
			, c3d::Point3fArray & normals
			, c3d::Point4fArray & tangents
			, c3d::Point3fArray & texcoords0
			, c3d::Point3fArray & texcoords1
			, c3d::Point3fArray & texcoords2
			, c3d::Point3fArray & texcoords3
			, c3d::Point3fArray & colours
			, CompressedBufferDataAdapter const & adapter
			, bool ignoreVertexColour )
		{
			DracoDecoder dracoDecoder{ impAsset, dracoCompression };

			if ( ( !meshes::decodeAttributeData< 3u, float >( impAsset, dracoCompression, "POSITION", positions, dracoDecoder )
					&& !meshes::parseAttributeData< 3u, float >( impAsset, impAttributes, "POSITION", positions, adapter ) )
				|| positions.empty() )
			{
				return;
			}

			if ( ( meshes::decodeAttributeData< 3u, float >( impAsset, dracoCompression, "NORMAL", normals, dracoDecoder )
					|| meshes::parseAttributeData< 3u, float >( impAsset, impAttributes, "NORMAL", normals, adapter ) )
				&& ( !meshes::decodeAttributeData< 4u, float >( impAsset, dracoCompression, "TANGENT", tangents, dracoDecoder )
					&& !meshes::parseAttributeData< 4u, float >( impAsset, impAttributes, "TANGENT", tangents, adapter ) ) )
			{
				if ( !meshes::decodeAttributeData< 3u, float >( impAsset, dracoCompression, "TANGENT", tangents, dracoDecoder ) )
					meshes::parseAttributeData< 3u, float >( impAsset, impAttributes, "TANGENT", tangents, adapter );
			}

			if ( !meshes::decodeAttributeData< 2u, float, 3u, float, true >( impAsset, dracoCompression, "TEXCOORD_0", texcoords0, dracoDecoder ) )
				meshes::parseAttributeData< 2u, float, 3u, float, true >( impAsset, impAttributes, "TEXCOORD_0", texcoords0, adapter );
			if ( !meshes::decodeAttributeData< 2u, float, 3u, float, true >( impAsset, dracoCompression, "TEXCOORD_1", texcoords1, dracoDecoder ) )
				meshes::parseAttributeData< 2u, float, 3u, float, true >( impAsset, impAttributes, "TEXCOORD_1", texcoords1, adapter );
			if ( !meshes::decodeAttributeData< 2u, float, 3u, float, true >( impAsset, dracoCompression, "TEXCOORD_2", texcoords2, dracoDecoder ) )
				meshes::parseAttributeData< 2u, float, 3u, float, true >( impAsset, impAttributes, "TEXCOORD_2", texcoords2, adapter );
			if ( !meshes::decodeAttributeData< 2u, float, 3u, float, true >( impAsset, dracoCompression, "TEXCOORD_3", texcoords3, dracoDecoder ) )
				meshes::parseAttributeData< 2u, float, 3u, float, true >( impAsset, impAttributes, "TEXCOORD_3", texcoords3, adapter );

			if ( !ignoreVertexColour
				&& !meshes::decodeAttributeData< 4u, float >( impAsset, dracoCompression, "COLOR_0", colours, dracoDecoder )
				&& !meshes::parseAttributeData< 4u, float >( impAsset, impAttributes, "COLOR_0", colours, adapter ) )
			{
				if ( !meshes::decodeAttributeData< 3u, float >( impAsset, dracoCompression, "COLOR_0", colours, dracoDecoder ) )
					meshes::parseAttributeData< 3u, float >( impAsset, impAttributes, "COLOR_0", colours, adapter );
			}
		}

		template< uint32_t CountT >
		static void applyWeight( c3d::Vector< c3d::Point< float, CountT > > & points
			, float weight )
		{
			for ( auto & point : points )
			{
				point *= weight;
			}
		}

		static void applyWeight( c3d::SubmeshAnimationBuffer & buffer
			, float weight )
		{
			applyWeight( buffer.positions, weight );
			applyWeight( buffer.normals, weight );
			applyWeight( buffer.tangents, weight );
			applyWeight( buffer.texcoords0, weight );
			applyWeight( buffer.texcoords1, weight );
			applyWeight( buffer.texcoords2, weight );
			applyWeight( buffer.texcoords3, weight );
			applyWeight( buffer.colours, weight );
		}
	}

	//*********************************************************************************************

	GltfMeshImporter::GltfMeshImporter( c3d::Engine & engine )
		: c3d::MeshImporter{ engine, cuT( "Gltf" ) }
	{
	}

	bool GltfMeshImporter::doImportMesh( c3d::Mesh & mesh, uint32_t submeshIndex )
	{
		auto & file = static_cast< GltfImporterFile & >( *m_file );
		auto const & name = mesh.getName();
		auto it = file.getMeshes().find( name );

		if ( it == file.getMeshes().end() )
		{
			return false;
		}

		auto const & engine = *file.getOwner();
		uint32_t meshIndex{};

		for ( auto & submesh : it->second.submeshes )
		{
			if ( submeshIndex == 0xFFFFFFFFu || submeshIndex == meshIndex )
			{
				auto impMesh = submesh.mesh;

				for ( auto & primitiveData : submesh.primitives )
				{
					auto impPrimitive = primitiveData.primitive;
					auto impMaterial = engine.tryFindMaterial( primitiveData.material );
					if ( !impMaterial )
						CU_LoaderError( "glTF Material not found." );
					impMaterial->setSerialisable( true );

					switch ( impPrimitive->type )
					{
					case fastgltf::PrimitiveType::Points:
						primitiveData.submesh = doProcessPointsSubmesh( mesh, impMaterial, *impMesh, *impPrimitive );
						break;
					case fastgltf::PrimitiveType::Lines:
						primitiveData.submesh = doProcessLinesSubmesh( mesh, impMaterial, *impMesh, *impPrimitive );
						break;
					case fastgltf::PrimitiveType::LineLoop:
						primitiveData.submesh = doProcessLineStripSubmesh( mesh, impMaterial, *impMesh, *impPrimitive, true );
						break;
					case fastgltf::PrimitiveType::LineStrip:
						primitiveData.submesh = doProcessLineStripSubmesh( mesh, impMaterial, *impMesh, *impPrimitive, false );
						break;
					case fastgltf::PrimitiveType::Triangles:
						primitiveData.submesh = doProcessTrianglesSubmesh( mesh, impMaterial, *impMesh, *impPrimitive );
						break;
					case fastgltf::PrimitiveType::TriangleStrip:
						primitiveData.submesh = doProcessTriangleStripSubmesh( mesh, impMaterial, *impMesh, *impPrimitive );
						break;
					case fastgltf::PrimitiveType::TriangleFan:
						primitiveData.submesh = doProcessTriangleFanSubmesh( mesh, impMaterial, *impMesh, *impPrimitive );
						break;
					default:
						break;
					}
				}
			}

			++meshIndex;
		}

		return true;
	}

	c3d::SubmeshRPtr GltfMeshImporter::doProcessPointsSubmesh( c3d::Mesh & mesh
		, c3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )const
	{
		auto submesh = mesh.createSubmesh();
		auto const & file = static_cast< GltfImporterFile const & >( *m_file );

		if ( auto const & impAsset = file.getAsset();
			doProcessMeshVertices( impAsset, impMesh, impPrimitive, *submesh, material ) )
		{
			submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
			submesh->createComponent< c3d::DefaultRenderComponent >();
		}
		else
		{
			mesh.removeSubmesh( *submesh );
			submesh = nullptr;
		}

		return submesh;
	}

	c3d::SubmeshRPtr GltfMeshImporter::doProcessLinesSubmesh( c3d::Mesh & mesh
		, c3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )const
	{
		auto submesh = mesh.createSubmesh();
		auto const & file = static_cast< GltfImporterFile const & >( *m_file );

		if ( auto const & impAsset = file.getAsset();
			doProcessMeshVertices( impAsset, impMesh, impPrimitive, *submesh, material ) )
		{
			submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
			submesh->createComponent< c3d::DefaultRenderComponent >();

			if ( impPrimitive.indicesAccessor )
			{
				auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
				auto mapping = c3d::makeUnique< c3d::LineMapping >( *submesh );

				switch ( impAccessor.componentType )
				{
				case fastgltf::ComponentType::UnsignedByte:
					meshes::parseLineList< uint8_t >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedShort:
					meshes::parseLineList< uint16_t >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedInt:
					meshes::parseLineList< uint32_t >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				default:
					mapping.reset();
					c3d::log::error << "Unsupported data type for face index\n";
					return nullptr;
				}

				if ( mapping )
				{
					submesh->addComponent( c3d::ptrRefCast< c3d::SubmeshComponent >( mapping ) );
				}
			}
		}
		else
		{
			mesh.removeSubmesh( *submesh );
			submesh = nullptr;
		}

		return submesh;
	}

	c3d::SubmeshRPtr GltfMeshImporter::doProcessLineStripSubmesh( c3d::Mesh & mesh
		, c3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive
		, bool loop )const
	{
		auto submesh = mesh.createSubmesh();
		auto const & file = static_cast< GltfImporterFile const & >( *m_file );

		if ( auto const & impAsset = file.getAsset();
			doProcessMeshVertices( impAsset, impMesh, impPrimitive, *submesh, material ) )
		{
			submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
			submesh->createComponent< c3d::DefaultRenderComponent >();

			if ( impPrimitive.indicesAccessor )
			{
				auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
				auto count = uint32_t( impAccessor.count );
				auto mapping = c3d::makeUnique< c3d::LineMapping >( *submesh );

				switch ( impAccessor.componentType )
				{
				case fastgltf::ComponentType::UnsignedByte:
					meshes::parseLineStrip< uint8_t >( impAsset, impAccessor, count, *mapping, loop, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedShort:
					meshes::parseLineStrip< uint16_t >( impAsset, impAccessor, count, *mapping, loop, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedInt:
					meshes::parseLineStrip< uint32_t >( impAsset, impAccessor, count, *mapping, loop, file.getAdapter() );
					break;
				default:
					mapping.reset();
					c3d::log::error << "Unsupported data type for face index\n";
					return nullptr;
				}

				if ( mapping )
				{
					submesh->addComponent( c3d::ptrRefCast< c3d::SubmeshComponent >( mapping ) );
				}
			}
		}
		else
		{
			mesh.removeSubmesh( *submesh );
			submesh = nullptr;
		}

		return submesh;
	}

	c3d::SubmeshRPtr GltfMeshImporter::doProcessTrianglesSubmesh( c3d::Mesh & mesh
		, c3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )const
	{
		auto submesh = mesh.createSubmesh();
		auto const & file = static_cast< GltfImporterFile const & >( *m_file );

		if ( auto const & impAsset = file.getAsset();
			doProcessMeshVertices( impAsset, impMesh, impPrimitive, *submesh, material ) )
		{
			submesh->createComponent< c3d::DefaultRenderComponent >();

			if ( impPrimitive.indicesAccessor )
			{
				auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
				auto mapping = c3d::makeUnique< c3d::TriFaceMapping >( *submesh );

				switch ( impAccessor.componentType )
				{
				case fastgltf::ComponentType::UnsignedByte:
					meshes::parseTriangleList< uint8_t >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedShort:
					meshes::parseTriangleList< uint16_t >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedInt:
					meshes::parseTriangleList< uint32_t >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				default:
					mapping.reset();
					c3d::log::error << "Unsupported data type for face index\n";
					return nullptr;
				}

				doCheckNmlTan( *submesh, c3d::ptrRefCast< c3d::IndexMapping >( mapping ) );
			}
			else
			{
				auto mapping = c3d::makeUnique< c3d::TriFaceMapping >( *submesh );
				uint32_t count = submesh->getPointsCount();
				uint32_t faceCount = count / 3u;

				if ( faceCount * 3 != count )
				{
					c3d::log::warn << "The number of vertices was not compatible with the TRIANGLES mode. Some vertices were dropped.\n";
					count = uint32_t( faceCount  * 3u );
				}

				c3d::Vector< c3d::FaceIndices > indicesGroup;
				indicesGroup.reserve( faceCount );
				c3d::FaceIndices indices{};

				for ( uint32_t i = 0u; i < count; i += 3 )
				{
					indices[0] = i + 1;
					indices[1] = i;
					indices[2] = i + 2;
					indicesGroup.push_back( indices );
				}

				mapping->getData().addFaceGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
				doCheckNmlTan( *submesh, c3d::ptrRefCast< c3d::IndexMapping >( mapping ) );
			}
		}
		else
		{
			mesh.removeSubmesh( *submesh );
			submesh = nullptr;
		}

		return submesh;
	}

	c3d::SubmeshRPtr GltfMeshImporter::doProcessTriangleStripSubmesh( c3d::Mesh & mesh
		, c3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )const
	{
		auto submesh = mesh.createSubmesh();
		auto const & file = static_cast< GltfImporterFile const & >( *m_file );

		if ( auto const & impAsset = file.getAsset();
			doProcessMeshVertices( impAsset, impMesh, impPrimitive, *submesh, material ) )
		{
			submesh->createComponent< c3d::DefaultRenderComponent >();

			if ( impPrimitive.indicesAccessor )
			{
				auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
				auto mapping = c3d::makeUnique< c3d::TriFaceMapping >( *submesh );

				switch ( impAccessor.componentType )
				{
				case fastgltf::ComponentType::UnsignedByte:
					meshes::parseTriangleStrip< uint8_t, true >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedShort:
					meshes::parseTriangleStrip< uint16_t, true >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedInt:
					meshes::parseTriangleStrip< uint32_t, true >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				default:
					mapping.reset();
					c3d::log::error << "Unsupported data type for face index\n";
					return nullptr;
				}

				doCheckNmlTan( *submesh, c3d::ptrRefCast< c3d::IndexMapping >( mapping ) );
			}
			else
			{
				auto mapping = c3d::makeUnique< c3d::TriFaceMapping >( *submesh );
				uint32_t count = submesh->getPointsCount();
				uint32_t faceCount = count  - 2;
				c3d::Vector< c3d::FaceIndices > indicesGroup;
				indicesGroup.reserve( faceCount );
				c3d::FaceIndices indices{};

				for ( uint32_t i = 0u; i < faceCount; ++i )
				{
					if ( ( i + 1 ) % 2 == 0 )
					{
						indices.m_index[0] = i;
						indices.m_index[1] = i + 1;
						indices.m_index[2] = i + 2;
					}
					else
					{
						indices.m_index[0] = i + 1;
						indices.m_index[1] = i;
						indices.m_index[2] = i + 2;
					}

					indicesGroup.push_back( indices );
				}

				mapping->getData().addFaceGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
				doCheckNmlTan( *submesh, c3d::ptrRefCast< c3d::IndexMapping >( mapping ) );
			}
		}
		else
		{
			mesh.removeSubmesh( *submesh );
			submesh = nullptr;
		}

		return submesh;
	}

	c3d::SubmeshRPtr GltfMeshImporter::doProcessTriangleFanSubmesh( c3d::Mesh & mesh
		, c3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )const
	{
		auto submesh = mesh.createSubmesh();
		auto const & file = static_cast< GltfImporterFile const & >( *m_file );

		if ( auto const & impAsset = file.getAsset();
			doProcessMeshVertices( impAsset, impMesh, impPrimitive, *submesh, material ) )
		{
			submesh->createComponent< c3d::DefaultRenderComponent >();

			if ( impPrimitive.indicesAccessor )
			{
				auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
				auto mapping = c3d::makeUnique< c3d::TriFaceMapping >( *submesh );

				switch ( impAccessor.componentType )
				{
				case fastgltf::ComponentType::UnsignedByte:
					meshes::parseTriangleStrip< uint8_t, false >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedShort:
					meshes::parseTriangleStrip< uint16_t, false >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				case fastgltf::ComponentType::UnsignedInt:
					meshes::parseTriangleStrip< uint32_t, false >( impAsset, impAccessor, *mapping, file.getAdapter() );
					break;
				default:
					mapping.reset();
					c3d::log::error << "Unsupported data type for face index\n";
					return nullptr;
				}

				doCheckNmlTan( *submesh, c3d::ptrRefCast< c3d::IndexMapping >( mapping ) );
			}
			else
			{
				auto mapping = c3d::makeUnique< c3d::TriFaceMapping >( *submesh );
				uint32_t count = submesh->getPointsCount();
				uint32_t faceCount = count - 2;
				c3d::Vector< c3d::FaceIndices > indicesGroup;
				indicesGroup.reserve( faceCount );
				c3d::FaceIndices indices{};
				indices.m_index[0] = 0;
				indices.m_index[1] = 1;
				indices.m_index[2] = 2;
				indicesGroup.push_back( indices );

				for ( uint32_t i = 1u; i < faceCount; ++i )
				{
					indices.m_index[0] = i + 1;
					indices.m_index[1] = i;
					indices.m_index[2] = i + 2;
					indicesGroup.push_back( indices );
				}

				mapping->getData().addFaceGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
				doCheckNmlTan( *submesh, c3d::ptrRefCast< c3d::IndexMapping >( mapping ) );
			}
		}
		else
		{
			mesh.removeSubmesh( *submesh );
			submesh = nullptr;
		}

		return submesh;
	}

	bool GltfMeshImporter::doProcessMeshVertices( fastgltf::Asset const & impAsset
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive
		, c3d::Submesh & submesh
		, c3d::Material * material )const
	{
		auto const & file = static_cast< GltfImporterFile const & >( *m_file );
		submesh.setDefaultMaterial( material );
		auto positions = submesh.createComponent< c3d::PositionsComponent >();
		c3d::Point3fArray nml;
		c3d::Point4fArray tan;
		c3d::Point3fArray tex0;
		c3d::Point3fArray tex1;
		c3d::Point3fArray tex2;
		c3d::Point3fArray tex3;
		c3d::Point3fArray col;
		c3d::Point3fArray * normals = &nml;
		c3d::Point4fArray * tangents = &tan;
		c3d::Point3fArray * texcoords0 = &tex0;
		c3d::Point3fArray * texcoords1 = &tex1;
		c3d::Point3fArray * texcoords2 = &tex2;
		c3d::Point3fArray * texcoords3 = &tex3;
		c3d::Point3fArray * colours = &col;

		if ( meshes::hasAttribute( impPrimitive.dracoCompression, "NORMAL" )
			|| meshes::hasAttribute( impPrimitive.attributes, "NORMAL" ) )
		{
			auto nmlComp = submesh.createComponent< c3d::NormalsComponent >();
			normals = &nmlComp->getData().getData();

			if ( meshes::hasAttribute( impPrimitive.dracoCompression, "TANGENT" )
				|| meshes::hasAttribute( impPrimitive.attributes, "TANGENT" ) )
			{
				auto tanComp = submesh.createComponent< c3d::TangentsComponent >();
				tangents = &tanComp->getData().getData();
			}
		}

		if ( meshes::hasAttribute( impPrimitive.dracoCompression, "TEXCOORD_0" )
			|| meshes::hasAttribute( impPrimitive.attributes, "TEXCOORD_0" ) )
		{
			auto texComp = submesh.createComponent< c3d::Texcoords0Component >();
			texcoords0 = &texComp->getData().getData();
		}

		if ( meshes::hasAttribute( impPrimitive.dracoCompression, "TEXCOORD_1" )
			|| meshes::hasAttribute( impPrimitive.attributes, "TEXCOORD_1" ) )
		{
			auto texComp = submesh.createComponent< c3d::Texcoords1Component >();
			texcoords1 = &texComp->getData().getData();
		}

		if ( meshes::hasAttribute( impPrimitive.dracoCompression, "TEXCOORD_2" )
			|| meshes::hasAttribute( impPrimitive.attributes, "TEXCOORD_2" ) )
		{
			auto texComp = submesh.createComponent< c3d::Texcoords2Component >();
			texcoords2 = &texComp->getData().getData();
		}

		if ( meshes::hasAttribute( impPrimitive.dracoCompression, "TEXCOORD_3" )
			|| meshes::hasAttribute( impPrimitive.attributes, "TEXCOORD_3" ) )
		{
			auto texComp = submesh.createComponent< c3d::Texcoords3Component >();
			texcoords3 = &texComp->getData().getData();
		}

		auto ignoreVertexColours = file.getParameters().get< bool >( "ignore_vertex_colour" );

		if ( meshes::hasAttribute( impPrimitive.dracoCompression, "COLOR_0" )
				|| meshes::hasAttribute( impPrimitive.attributes, "COLOR_0" )
			&& !ignoreVertexColours )
		{
			auto colComp = submesh.createComponent< c3d::ColoursComponent >();
			colours = &colComp->getData().getData();
		}

		meshes::createVertexBuffer( impAsset
			, impPrimitive.attributes
			, impPrimitive.dracoCompression
			, positions->getData().getData()
			, *normals
			, *tangents
			, *texcoords0
			, *texcoords1
			, *texcoords2
			, *texcoords3
			, *colours
			, file.getAdapter()
			, ignoreVertexColours );
		c3d::Vector< c3d::SubmeshAnimationBuffer > morphTargets;
		uint32_t index{};

		for ( auto & impAttributes : impPrimitive.targets )
		{
			c3d::SubmeshAnimationBuffer buffer;
			meshes::createVertexBuffer( impAsset
				, impAttributes
				, fastgltf::boxed_optional<fastgltf::DracoCompressedPrimitive>{}
				, buffer.positions
				, buffer.normals
				, buffer.tangents
				, buffer.texcoords0
				, buffer.texcoords1
				, buffer.texcoords2
				, buffer.texcoords3
				, buffer.colours
				, file.getAdapter()
				, ignoreVertexColours );

			if ( index < impMesh.weights.size()
				&& impMesh.weights[index] != 0.0f )
			{
				meshes::applyWeight( buffer, impMesh.weights[index] );
				++index;
			}

			morphTargets.emplace_back( c3d::move( buffer ) );
		}

		if ( !morphTargets.empty() )
		{
			c3d::log::debug << cuT( "    Morph targets found: [" ) << uint32_t( morphTargets.size() ) << cuT( "]" ) << std::endl;
			auto component = submesh.hasComponent( c3d::MorphComponent::TypeName )
				? submesh.getComponent< c3d::MorphComponent >()
				: submesh.createComponent< c3d::MorphComponent >();

			for ( auto const & morphTarget : morphTargets )
			{
				component->getData().addMorphTarget( morphTarget );
			}
		}

		if ( !m_parameters.get< bool >( "no_skeleton" ) )
		{
			c3d::Point4uiArray joints;

			if ( !meshes::parseAttributeData< 4u, uint8_t >( impAsset, impPrimitive.attributes, "JOINTS_0", joints, file.getAdapter() ) )
				meshes::parseAttributeData< 4u, uint16_t >( impAsset, impPrimitive.attributes, "JOINTS_0", joints, file.getAdapter() );

			if ( !joints.empty() )
			{
				c3d::Point4fArray weights;

				if ( meshes::parseAttributeData< 4u, float >( impAsset, impPrimitive.attributes, "WEIGHTS_0", weights, file.getAdapter() )
					&& weights.size() == joints.size() )
				{
					c3d::VertexBoneDataArray datas;
					datas.reserve( weights.size() );

					for ( size_t i = 0u; i < weights.size(); ++i )
					{
						c3d::VertexBoneData data;
						data.addBoneData( joints[i][0], weights[i][0] );
						data.addBoneData( joints[i][1], weights[i][1] );
						data.addBoneData( joints[i][2], weights[i][2] );
						data.addBoneData( joints[i][3], weights[i][3] );
						datas.push_back( data );
					}

					submesh.createComponent< c3d::SkinComponent >()->getData().addDatas( datas );
				}
			}
		}

		return submesh.getPointsCount() > 0;
	}

	void GltfMeshImporter::doCheckNmlTan( c3d::Submesh & submesh
		, c3d::IndexMappingUPtr mapping )const
	{
		if ( mapping )
		{
			if ( !submesh.hasComponent( c3d::NormalsComponent::TypeName ) )
			{
				auto normals = submesh.createComponent< c3d::NormalsComponent >();
				normals->getData().getData().resize( submesh.getPositions().size() );

				if ( !submesh.hasComponent( c3d::TangentsComponent::TypeName )
					&& submesh.hasComponent( c3d::Texcoords0Component::TypeName ) )
				{
					auto tangents = submesh.createComponent< c3d::TangentsComponent >();
					tangents->getData().getData().resize( submesh.getPositions().size() );
				}

				mapping->computeNormals();
				mapping->computeTangents();
			}
			else if ( !submesh.hasComponent( c3d::TangentsComponent::TypeName )
				&& submesh.hasComponent( c3d::Texcoords0Component::TypeName ) )
			{
				auto tangents = submesh.createComponent< c3d::TangentsComponent >();
				tangents->getData().getData().resize( submesh.getPositions().size() );
				mapping->computeTangents();
			}

			submesh.addComponent( c3d::ptrRefCast< c3d::SubmeshComponent >( mapping ) );
		}
	}

	void GltfMeshImporter::doTransformMesh( fastgltf::Node const & impNode
		, c3d::Vector< fastgltf::Node > const & impNodes
		, c3d::Mesh & mesh
		, c3d::Matrix4x4f transformAcc )
	{
		auto transform = convert( impNode.transform );
		c3d::Matrix4x4f matrix;
		c3d::matrix::setTransform( matrix, transform.translate, transform.scale, transform.rotate );
		transformAcc = transformAcc * matrix;
		c3d::matrix::decompose( transformAcc, transform.translate, transform.scale, transform.rotate );

		if ( impNode.meshIndex )
		{
			auto impMeshIndex = uint32_t ( *impNode.meshIndex );

			if ( impMeshIndex < mesh.getSubmeshCount() )
			{
				auto submesh = mesh.getSubmesh( impMeshIndex );
				auto matrixAcc = transformAcc;

				if ( submesh->hasComponent( c3d::SkinComponent::TypeName ) )
				{
					c3d::matrix::setTranslate( matrixAcc, transform.translate );
				}

				for ( auto & vertex : submesh->getPositions() )
				{
					vertex = matrixAcc * vertex;
				}

				auto indexMapping = submesh->getIndexMapping();
				indexMapping->computeNormals();
				indexMapping->computeTangents();
			}
		}

		for ( auto childIndex : impNode.children )
		{
			doTransformMesh( impNodes[childIndex], impNodes, mesh, transformAcc );
		}
	}

	//*********************************************************************************************
}
