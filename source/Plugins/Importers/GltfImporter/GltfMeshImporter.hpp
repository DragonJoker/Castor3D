/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfMeshImporter___
#define ___C3D_GltfMeshImporter___

#include "GltfImporter/GltfImporterFile.hpp"

#include <Castor3D/Model/Mesh/MeshImporter.hpp>
#include <Castor3D/Model/Mesh/Submesh/Component/IndexMapping.hpp>

namespace c3d_gltf
{
	class GltfMeshImporter
		: public c3d::MeshImporter
	{
	public:
		explicit GltfMeshImporter( c3d::Engine & engine );

	private:
		using PrimitiveArray = c3d::Vector< fastgltf::Primitive const * >;

		bool doImportMesh( c3d::Mesh & mesh, uint32_t submeshIndex )override;
		void doProcessPointsSubmesh( c3d::Mesh & mesh
			, c3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		void doProcessLinesSubmesh( c3d::Mesh & mesh
			, c3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		void doProcessLineStripSubmesh( c3d::Mesh & mesh
			, c3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive
			, bool loop );
		void doProcessTrianglesSubmesh( c3d::Mesh & mesh
			, c3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		void doProcessTriangleStripSubmesh( c3d::Mesh & mesh
			, c3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		void doProcessTriangleFanSubmesh( c3d::Mesh & mesh
			, c3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		bool doProcessMeshVertices( fastgltf::Asset const & impAsset
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive
			, c3d::Mesh & mesh
			, c3d::Submesh & submesh
			, c3d::Material * material );
		void doCheckNmlTan( c3d::Submesh & submesh
			, c3d::IndexMappingUPtr mapping );
		void doTransformMesh( fastgltf::Node const & impNode
			, c3d::Vector< fastgltf::Node > const & impNodes
			, c3d::Mesh & mesh
			, c3d::Matrix4x4f transformAcc = c3d::Matrix4x4f{ 1.0f } );
	};
}

#endif
