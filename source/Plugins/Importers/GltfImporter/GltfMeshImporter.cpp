#include "GltfImporter/GltfMeshImporter.hpp"

#include "GltfImporter/GltfMaterialImporter.hpp"

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

namespace c3d_gltf
{
	//*********************************************************************************************

	namespace meshes
	{
		template< typename IndexT >
		static void parseLineList( fastgltf::Asset const & impAsset
			, fastgltf::Accessor const & impAccessor
			, castor3d::LinesMapping & mapping )
		{
			auto count = impAccessor.count;
			auto lineCount = count / 2u;
			std::vector< castor3d::LineIndices > indicesGroup;
			indicesGroup.reserve( lineCount );
			castor3d::LineIndices curIndices;
			uint32_t idx{};

			fastgltf::iterateAccessor< IndexT >( impAsset
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
				} );
			mapping.getData().addLineGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
		}

		template< typename IndexT >
		static void parseLineStrip( fastgltf::Asset const & impAsset
			, fastgltf::Accessor const & impAccessor
			, uint32_t lineCount
			, castor3d::LinesMapping & mapping
			, bool loop )
		{
			if ( !loop )
			{
				--lineCount;
			}

			std::vector< castor3d::LineIndices > indicesGroup;
			indicesGroup.reserve( lineCount );
			castor3d::LineIndices curIndices;
			uint32_t prvIndex{ ~0u };

			fastgltf::iterateAccessor< IndexT >( impAsset
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
				} );

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
			, castor3d::TriFaceMapping & mapping )
		{
			auto count = impAccessor.count;
			auto faceCount = count / 3u;
			std::vector< castor3d::FaceIndices > indicesGroup;
			indicesGroup.reserve( faceCount );
			castor3d::FaceIndices curIndices;
			uint32_t idx{};

			fastgltf::iterateAccessor< IndexT >( impAsset
				, impAccessor
				, [&curIndices, &indicesGroup, &idx]( IndexT value )
				{
					curIndices[idx] = value;
					++idx;

					if ( idx == 3u )
					{
						idx = 0u;
						std::swap( curIndices[0], curIndices[1] );
						indicesGroup.push_back( curIndices );
					}
				} );
			mapping.getData().addFaceGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
		}

		template< typename IndexT, bool IsStripT >
		static void parseTriangleStrip( fastgltf::Asset const & impAsset
			, fastgltf::Accessor const & impAccessor
			, castor3d::TriFaceMapping & mapping )
		{
			auto count = impAccessor.count;
			castor::UInt32Array indices;
			indices.reserve( count );
			fastgltf::iterateAccessor< IndexT >( impAsset
				, impAccessor
				, [&indices]( IndexT value )
				{
					indices.push_back( value );
				} );

			auto faceCount = count - 2u;
			std::vector< castor3d::FaceIndices > indicesGroup;
			indicesGroup.reserve( faceCount );
			castor3d::FaceIndices curIndices;

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

		auto findAttribute( auto const & attributes
			, std::pmr::string const & name )
		{
			return std::find_if( attributes.begin()
				, attributes.end()
				, [&name]( auto const & lookup )
				{
					return lookup.first == name;
				} );
		}

		bool hasAttribute( auto const & attributes
			, std::pmr::string const & name )
		{
			return findAttribute( attributes, name ) != attributes.end();
		}

		template< uint32_t SrcCountT, typename SrcDataT, uint32_t DstCountT, typename DstDataT, bool InvYT = false >
		static bool parseAttributeData( fastgltf::Asset const & impAsset
			, auto const & impAttributes
			, std::pmr::string const & attrName
			, std::vector< castor::Point< DstDataT, DstCountT > > & result )
		{
			auto it = findAttribute( impAttributes, attrName );

			if ( it == impAttributes.end() )
			{
				return false;
			}

			auto & impAccessor = impAsset.accessors[it->second];
			result.reserve( impAccessor.count );

			fastgltf::iterateAccessor< castor::Point< SrcDataT, SrcCountT > >( impAsset
				, impAccessor
				, [&result]( castor::Point< SrcDataT, SrcCountT > value )
				{
					if constexpr ( InvYT )
					{
						value->y = 1.0f - value->y;
					}

					if constexpr ( SrcCountT == DstCountT && std::is_same_v< SrcDataT, DstDataT > )
					{

						result.push_back( std::move( value ) );
					}
					else
					{
						result.push_back( castor::Point < DstDataT, DstCountT >{ value } );
					}
				} );
			return !result.empty();
		}

		inline void createVertexBuffer( fastgltf::Asset const & impAsset
			, auto const & impAttributes
			, castor::Point3fArray & positions
			, castor::Point3fArray & normals
			, castor::Point4fArray & tangents
			, castor::Point3fArray & texcoords0
			, castor::Point3fArray & texcoords1
			, castor::Point3fArray & texcoords2
			, castor::Point3fArray & texcoords3
			, castor::Point3fArray & colours )
		{
			if ( !meshes::parseAttributeData< 3u, float >( impAsset, impAttributes, "POSITION", positions ) )
			{
				return;
			}

			if ( meshes::parseAttributeData< 3u, float >( impAsset, impAttributes, "NORMAL", normals ) )
			{
				if ( !meshes::parseAttributeData< 4u, float >( impAsset, impAttributes, "TANGENT", tangents ) )
				{
					meshes::parseAttributeData< 3u, float >( impAsset, impAttributes, "TANGENT", tangents );
				}
			}

			meshes::parseAttributeData< 2u, float, 3u, float, true >( impAsset, impAttributes, "TEXCOORD_0", texcoords0 );
			meshes::parseAttributeData< 2u, float, 3u, float, true >( impAsset, impAttributes, "TEXCOORD_1", texcoords1 );
			meshes::parseAttributeData< 2u, float, 3u, float, true >( impAsset, impAttributes, "TEXCOORD_2", texcoords2 );
			meshes::parseAttributeData< 2u, float, 3u, float, true >( impAsset, impAttributes, "TEXCOORD_3", texcoords3 );

			if ( !meshes::parseAttributeData< 4u, float >( impAsset, impAttributes, "COLOR_0", colours ) )
			{
				meshes::parseAttributeData< 3u, float >( impAsset, impAttributes, "COLOR_0", colours );
			}
		}

		template< uint32_t CountT >
		static void applyWeight( std::vector< castor::Point< float, CountT > > & points
			, float weight )
		{
			for ( auto & point : points )
			{
				point *= weight;
			}
		}

		static void applyWeight( castor3d::SubmeshAnimationBuffer & buffer
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

	GltfMeshImporter::GltfMeshImporter( castor3d::Engine & engine )
		: castor3d::MeshImporter{ engine }
	{
	}

	bool GltfMeshImporter::doImportMesh( castor3d::Mesh & mesh )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto name = mesh.getName();
		auto it = file.getMeshes().find( name );

		if ( it == file.getMeshes().end() )
		{
			return false;
		}

		castor3d::log::info << cuT( "  Mesh found: [" ) << name << cuT( "]" ) << std::endl;
		using PrimitiveMap = std::map< fastgltf::PrimitiveType, PrimitiveArray >;
		using MaterialPrimitiveMap = std::map< castor3d::Material *, PrimitiveMap >;
		std::map< fastgltf::Mesh const *, MaterialPrimitiveMap > submeshes;
		auto & engine = *file.getOwner();

		for ( auto & submesh : it->second.submeshes )
		{
			fastgltf::Mesh const & impMesh = *submesh.mesh;

			for ( auto & primitive : impMesh.primitives )
			{
				castor3d::MaterialRPtr material;

				if ( primitive.materialIndex )
				{
					material = engine.tryFindMaterial( file.getMaterialName( uint32_t( *primitive.materialIndex ) ) );

					if ( !material )
					{
						CU_LoaderError( "glTF Material not found." );
					}
				}
				else
				{
					material = engine.tryFindMaterial( DefaultMaterial );

					if ( !material )
					{
						CU_LoaderError( "Default glTF material not found." );
					}

					material->setSerialisable( true );
				}

				auto pit = submeshes.emplace( &impMesh, MaterialPrimitiveMap{} ).first;
				auto mit = pit->second.emplace( material, PrimitiveMap{} ).first;
				auto sit = mit->second.emplace( primitive.type, PrimitiveArray{} ).first;
				sit->second.push_back( &primitive );
			}
		}

		for ( auto & meshesIt : submeshes )
		{
			auto & impMesh = *meshesIt.first;

			for ( auto & materialsIt : meshesIt.second )
			{
				auto material = materialsIt.first;

				for ( auto & submeshesIt : materialsIt.second )
				{
					for ( auto & impPrimitive : submeshesIt.second )
					{
						switch ( submeshesIt.first )
						{
						case fastgltf::PrimitiveType::Points:
							doProcessPointsSubmesh( mesh, material, impMesh, *impPrimitive );
							break;
						case fastgltf::PrimitiveType::Lines:
							doProcessLinesSubmesh( mesh, material, impMesh, *impPrimitive );
							break;
						case fastgltf::PrimitiveType::LineLoop:
							doProcessLineStripSubmesh( mesh, material, impMesh, *impPrimitive, true );
							break;
						case fastgltf::PrimitiveType::LineStrip:
							doProcessLineStripSubmesh( mesh, material, impMesh, *impPrimitive, false );
							break;
						case fastgltf::PrimitiveType::Triangles:
							doProcessTrianglesSubmesh( mesh, material, impMesh, *impPrimitive );
							break;
						case fastgltf::PrimitiveType::TriangleStrip:
							doProcessTriangleStripSubmesh( mesh, material, impMesh, *impPrimitive );
							break;
						case fastgltf::PrimitiveType::TriangleFan:
							doProcessTriangleFanSubmesh( mesh, material, impMesh, *impPrimitive );
							break;
						default:
							break;
						}
					}
				}
			}
		}

		return true;
	}

	void GltfMeshImporter::doProcessPointsSubmesh( castor3d::Mesh & mesh
		, castor3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )
	{
		auto submesh = mesh.createSubmesh();
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		doProcessMeshVertices( impAsset, impMesh, impPrimitive, mesh, *submesh, material );
		submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_POINT_LIST );
		submesh->createComponent< castor3d::DefaultRenderComponent >();
	}

	void GltfMeshImporter::doProcessLinesSubmesh( castor3d::Mesh & mesh
		, castor3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )
	{
		auto submesh = mesh.createSubmesh();
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		doProcessMeshVertices( impAsset, impMesh, impPrimitive, mesh, *submesh, material );
		submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
		submesh->createComponent< castor3d::DefaultRenderComponent >();

		if ( impPrimitive.indicesAccessor )
		{
			auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
			auto mapping = castor::makeUnique< castor3d::LinesMapping >( *submesh );

			switch ( impAccessor.componentType )
			{
			case fastgltf::ComponentType::UnsignedByte:
				meshes::parseLineList< uint8_t >( impAsset, impAccessor, *mapping );
				break;
			case fastgltf::ComponentType::UnsignedShort:
				meshes::parseLineList< uint16_t >( impAsset, impAccessor, *mapping );
				break;
			case fastgltf::ComponentType::UnsignedInt:
				meshes::parseLineList< uint32_t >( impAsset, impAccessor, *mapping );
				break;
			default:
				mapping.reset();
				castor3d::log::error << "Unsupported data type for face index\n";
				return;
			}

			if ( mapping )
			{
				submesh->setIndexMapping( castor::ptrRefCast< castor3d::IndexMapping >( mapping ) );
			}
		}
	}

	void GltfMeshImporter::doProcessLineStripSubmesh( castor3d::Mesh & mesh
		, castor3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive
		, bool loop )
	{
		auto submesh = mesh.createSubmesh();
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		doProcessMeshVertices( impAsset, impMesh, impPrimitive, mesh, *submesh, material );
		submesh->setTopology( VK_PRIMITIVE_TOPOLOGY_LINE_LIST );
		submesh->createComponent< castor3d::DefaultRenderComponent >();

		if ( impPrimitive.indicesAccessor )
		{
			auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
			auto count = uint32_t( impAccessor.count );
			auto mapping = castor::makeUnique< castor3d::LinesMapping >( *submesh );

			switch ( impAccessor.componentType )
			{
			case fastgltf::ComponentType::UnsignedByte:
				meshes::parseLineStrip< uint8_t >( impAsset, impAccessor, count, *mapping, loop );
				break;
			case fastgltf::ComponentType::UnsignedShort:
				meshes::parseLineStrip< uint16_t >( impAsset, impAccessor, count, *mapping, loop );
				break;
			case fastgltf::ComponentType::UnsignedInt:
				meshes::parseLineStrip< uint32_t >( impAsset, impAccessor, count, *mapping, loop );
				break;
			default:
				mapping.reset();
				castor3d::log::error << "Unsupported data type for face index\n";
				return;
			}

			if ( mapping )
			{
				submesh->setIndexMapping( castor::ptrRefCast< castor3d::IndexMapping >( mapping ) );
			}
		}
	}

	void GltfMeshImporter::doProcessTrianglesSubmesh( castor3d::Mesh & mesh
		, castor3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )
	{
		auto submesh = mesh.createSubmesh();
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		doProcessMeshVertices( impAsset, impMesh, impPrimitive, mesh, *submesh, material );
		submesh->createComponent< castor3d::DefaultRenderComponent >();

		if ( impPrimitive.indicesAccessor )
		{
			auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
			auto mapping = castor::makeUnique< castor3d::TriFaceMapping >( *submesh );

			switch ( impAccessor.componentType )
			{
			case fastgltf::ComponentType::UnsignedByte:
				meshes::parseTriangleList< uint8_t >( impAsset, impAccessor, *mapping );
				break;
			case fastgltf::ComponentType::UnsignedShort:
				meshes::parseTriangleList< uint16_t >( impAsset, impAccessor, *mapping );
				break;
			case fastgltf::ComponentType::UnsignedInt:
				meshes::parseTriangleList< uint32_t >( impAsset, impAccessor, *mapping );
				break;
			default:
				mapping.reset();
				castor3d::log::error << "Unsupported data type for face index\n";
				return;
			}

			doCheckNmlTan( *submesh, castor::ptrRefCast< castor3d::IndexMapping >( mapping ) );
		}
		else
		{
			auto mapping = castor::makeUnique< castor3d::TriFaceMapping >( *submesh );
			uint32_t count = submesh->getPointsCount();
			uint32_t faceCount = count / 3u;

			if ( faceCount * 3 != count )
			{
				castor3d::log::warn << "The number of vertices was not compatible with the TRIANGLES mode. Some vertices were dropped.\n";
				count = uint32_t( faceCount  * 3u );
			}

			std::vector< castor3d::FaceIndices > indicesGroup;
			indicesGroup.reserve( faceCount );
			castor3d::FaceIndices indices{};

			for ( uint32_t i = 0u; i < count; i += 3 )
			{
				indices[0] = i + 1;
				indices[1] = i;
				indices[2] = i + 2;
				indicesGroup.push_back( indices );
			}

			mapping->getData().addFaceGroup( indicesGroup.data(), indicesGroup.data() + indicesGroup.size() );
			doCheckNmlTan( *submesh, castor::ptrRefCast< castor3d::IndexMapping >( mapping ) );
		}
	}

	void GltfMeshImporter::doProcessTriangleStripSubmesh( castor3d::Mesh & mesh
		, castor3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )
	{
		auto submesh = mesh.createSubmesh();
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		doProcessMeshVertices( impAsset, impMesh, impPrimitive, mesh, *submesh, material );
		submesh->createComponent< castor3d::DefaultRenderComponent >();

		if ( impPrimitive.indicesAccessor )
		{
			auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
			auto mapping = castor::makeUnique< castor3d::TriFaceMapping >( *submesh );

			switch ( impAccessor.componentType )
			{
			case fastgltf::ComponentType::UnsignedByte:
				meshes::parseTriangleStrip< uint8_t, true >( impAsset, impAccessor, *mapping );
				break;
			case fastgltf::ComponentType::UnsignedShort:
				meshes::parseTriangleStrip< uint16_t, true >( impAsset, impAccessor, *mapping );
				break;
			case fastgltf::ComponentType::UnsignedInt:
				meshes::parseTriangleStrip< uint32_t, true >( impAsset, impAccessor, *mapping );
				break;
			default:
				mapping.reset();
				castor3d::log::error << "Unsupported data type for face index\n";
				return;
			}

			doCheckNmlTan( *submesh, castor::ptrRefCast< castor3d::IndexMapping >( mapping ) );
		}
		else
		{
			auto mapping = castor::makeUnique< castor3d::TriFaceMapping >( *submesh );
			uint32_t count = submesh->getPointsCount();
			uint32_t faceCount = count  - 2;
			std::vector< castor3d::FaceIndices > indicesGroup;
			indicesGroup.reserve( faceCount );
			castor3d::FaceIndices indices{};

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
			doCheckNmlTan( *submesh, castor::ptrRefCast< castor3d::IndexMapping >( mapping ) );
		}
	}

	void GltfMeshImporter::doProcessTriangleFanSubmesh( castor3d::Mesh & mesh
		, castor3d::Material * material
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive )
	{
		auto submesh = mesh.createSubmesh();
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();
		doProcessMeshVertices( impAsset, impMesh, impPrimitive, mesh, *submesh, material );
		submesh->createComponent< castor3d::DefaultRenderComponent >();

		if ( impPrimitive.indicesAccessor )
		{
			auto & impAccessor = impAsset.accessors[*impPrimitive.indicesAccessor];
			auto mapping = castor::makeUnique< castor3d::TriFaceMapping >( *submesh );

			switch ( impAccessor.componentType )
			{
			case fastgltf::ComponentType::UnsignedByte:
				meshes::parseTriangleStrip< uint8_t, false >( impAsset, impAccessor, *mapping );
				break;
			case fastgltf::ComponentType::UnsignedShort:
				meshes::parseTriangleStrip< uint16_t, false >( impAsset, impAccessor, *mapping );
				break;
			case fastgltf::ComponentType::UnsignedInt:
				meshes::parseTriangleStrip< uint32_t, false >( impAsset, impAccessor, *mapping );
				break;
			default:
				mapping.reset();
				castor3d::log::error << "Unsupported data type for face index\n";
				return;
			}

			doCheckNmlTan( *submesh, castor::ptrRefCast< castor3d::IndexMapping >( mapping ) );
		}
		else
		{
			auto mapping = castor::makeUnique< castor3d::TriFaceMapping >( *submesh );
			uint32_t count = submesh->getPointsCount();
			uint32_t faceCount = count - 2;
			std::vector< castor3d::FaceIndices > indicesGroup;
			indicesGroup.reserve( faceCount );
			castor3d::FaceIndices indices{};
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
			doCheckNmlTan( *submesh, castor::ptrRefCast< castor3d::IndexMapping >( mapping ) );
		}
	}

	void GltfMeshImporter::doProcessMeshVertices( fastgltf::Asset const & impAsset
		, fastgltf::Mesh const & impMesh
		, fastgltf::Primitive const & impPrimitive
		, castor3d::Mesh & mesh
		, castor3d::Submesh & submesh
		, castor3d::Material * material )
	{
		submesh.setDefaultMaterial( material );
		auto positions = submesh.createComponent< castor3d::PositionsComponent >();
		castor::Point3fArray nml;
		castor::Point4fArray tan;
		castor::Point3fArray tex0;
		castor::Point3fArray tex1;
		castor::Point3fArray tex2;
		castor::Point3fArray tex3;
		castor::Point3fArray col;
		castor::Point3fArray * normals = &nml;
		castor::Point4fArray * tangents = &tan;
		castor::Point3fArray * texcoords0 = &tex0;
		castor::Point3fArray * texcoords1 = &tex1;
		castor::Point3fArray * texcoords2 = &tex2;
		castor::Point3fArray * texcoords3 = &tex3;
		castor::Point3fArray * colours = &col;

		if ( meshes::hasAttribute( impPrimitive.attributes, "NORMAL" ) )
		{
			auto nmlComp = submesh.createComponent< castor3d::NormalsComponent >();
			normals = &nmlComp->getData().getData();

			if ( meshes::hasAttribute( impPrimitive.attributes, "TANGENT" ) )
			{
				auto tanComp = submesh.createComponent< castor3d::TangentsComponent >();
				tangents = &tanComp->getData().getData();
			}
		}

		if ( meshes::hasAttribute( impPrimitive.attributes, "TEXCOORD_0" ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords0Component >();
			texcoords0 = &texComp->getData().getData();
		}

		if ( meshes::hasAttribute( impPrimitive.attributes, "TEXCOORD_1" ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords1Component >();
			texcoords1 = &texComp->getData().getData();
		}

		if ( meshes::hasAttribute( impPrimitive.attributes, "TEXCOORD_2" ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords2Component >();
			texcoords2 = &texComp->getData().getData();
		}

		if ( meshes::hasAttribute( impPrimitive.attributes, "TEXCOORD_3" ) )
		{
			auto texComp = submesh.createComponent< castor3d::Texcoords3Component >();
			texcoords3 = &texComp->getData().getData();
		}

		if ( meshes::hasAttribute( impPrimitive.attributes, "COLOR_0" ) )
		{
			auto colComp = submesh.createComponent< castor3d::ColoursComponent >();
			colours = &colComp->getData().getData();
		}

		meshes::createVertexBuffer( impAsset
			, impPrimitive.attributes
			, positions->getData().getData()
			, *normals
			, *tangents
			, *texcoords0
			, *texcoords1
			, *texcoords2
			, *texcoords3
			, *colours );
		std::vector< castor3d::SubmeshAnimationBuffer > morphTargets;
		uint32_t index{};

		for ( auto & impAttributes : impPrimitive.targets )
		{
			castor3d::SubmeshAnimationBuffer buffer;
			meshes::createVertexBuffer( impAsset
				, impAttributes
				, buffer.positions
				, buffer.normals
				, buffer.tangents
				, buffer.texcoords0
				, buffer.texcoords1
				, buffer.texcoords2
				, buffer.texcoords3
				, buffer.colours );

			if ( index < impMesh.weights.size()
				&& impMesh.weights[index] != 0.0f )
			{
				meshes::applyWeight( buffer, impMesh.weights[index++] );
			}

			morphTargets.emplace_back( std::move( buffer ) );
		}

		if ( !morphTargets.empty() )
		{
			castor3d::log::debug << cuT( "    Morph targets found: [" ) << uint32_t( morphTargets.size() ) << cuT( "]" ) << std::endl;
			auto component = submesh.hasComponent( castor3d::MorphComponent::TypeName )
				? submesh.getComponent< castor3d::MorphComponent >()
				: submesh.createComponent< castor3d::MorphComponent >();

			for ( auto & morphTarget : morphTargets )
			{
				component->getData().addMorphTarget( morphTarget );
			}
		}

		castor::Point4uiArray joints;

		if ( !meshes::parseAttributeData< 4u, uint8_t >( impAsset, impPrimitive.attributes, "JOINTS_0", joints ) )
		{
			meshes::parseAttributeData< 4u, uint16_t >( impAsset, impPrimitive.attributes, "JOINTS_0", joints );
		}

		if ( !joints.empty() )
		{
			castor::Point4fArray weights;

			if ( meshes::parseAttributeData< 4u, float >( impAsset, impPrimitive.attributes, "WEIGHTS_0", weights )
				&& weights.size() == joints.size() )
			{
				castor3d::VertexBoneDataArray datas;
				datas.reserve( weights.size() );

				for ( size_t i = 0u; i < weights.size(); ++i )
				{
					castor3d::VertexBoneData data;
					data.addBoneData( joints[i][0], weights[i][0] );
					data.addBoneData( joints[i][1], weights[i][1] );
					data.addBoneData( joints[i][2], weights[i][2] );
					data.addBoneData( joints[i][3], weights[i][3] );
					datas.push_back( data );
				}

				submesh.createComponent< castor3d::SkinComponent >()->getData().addDatas( datas );
			}
		}
	}

	void GltfMeshImporter::doCheckNmlTan( castor3d::Submesh & submesh
		, castor3d::IndexMappingUPtr mapping )
	{
		if ( mapping )
		{
			if ( !submesh.hasComponent( castor3d::NormalsComponent::TypeName ) )
			{
				auto normals = submesh.createComponent< castor3d::NormalsComponent >();
				normals->getData().getData().resize( submesh.getPositions().size() );

				if ( !submesh.hasComponent( castor3d::TangentsComponent::TypeName )
					&& submesh.hasComponent( castor3d::Texcoords0Component::TypeName ) )
				{
					auto tangents = submesh.createComponent< castor3d::TangentsComponent >();
					tangents->getData().getData().resize( submesh.getPositions().size() );
				}

				mapping->computeNormals();
				mapping->computeTangents();
			}
			else if ( !submesh.hasComponent( castor3d::TangentsComponent::TypeName )
				&& submesh.hasComponent( castor3d::Texcoords0Component::TypeName ) )
			{
				auto tangents = submesh.createComponent< castor3d::TangentsComponent >();
				tangents->getData().getData().resize( submesh.getPositions().size() );
				mapping->computeTangents();
			}

			submesh.setIndexMapping( std::move( mapping ) );
		}
	}

	void GltfMeshImporter::doTransformMesh( fastgltf::Node const & impNode
		, std::vector< fastgltf::Node > const & impNodes
		, castor3d::Mesh & mesh
		, castor::Matrix4x4f transformAcc )
	{
		auto transform = convert( impNode.transform );
		castor::Matrix4x4f matrix;
		castor::matrix::setTransform( matrix, transform.translate, transform.scale, transform.rotate );
		transformAcc = transformAcc * matrix;
		castor::matrix::decompose( transformAcc, transform.translate, transform.scale, transform.rotate );

		if ( impNode.meshIndex )
		{
			auto impMeshIndex = uint32_t ( *impNode.meshIndex );

			if ( impMeshIndex < mesh.getSubmeshCount() )
			{
				auto submesh = mesh.getSubmesh( impMeshIndex );
				auto matrixAcc = transformAcc;

				if ( submesh->hasComponent( castor3d::SkinComponent::TypeName ) )
				{
					castor::matrix::setTranslate( matrixAcc, transform.translate );
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
