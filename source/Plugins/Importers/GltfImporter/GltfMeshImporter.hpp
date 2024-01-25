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
		: public castor3d::MeshImporter
	{
	public:
		explicit GltfMeshImporter( castor3d::Engine & engine );

	private:
		using PrimitiveArray = castor::Vector< fastgltf::Primitive const * >;

		bool doImportMesh( castor3d::Mesh & mesh )override;
		void doProcessPointsSubmesh( castor3d::Mesh & mesh
			, castor3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		void doProcessLinesSubmesh( castor3d::Mesh & mesh
			, castor3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		void doProcessLineStripSubmesh( castor3d::Mesh & mesh
			, castor3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive
			, bool loop );
		void doProcessTrianglesSubmesh( castor3d::Mesh & mesh
			, castor3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		void doProcessTriangleStripSubmesh( castor3d::Mesh & mesh
			, castor3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		void doProcessTriangleFanSubmesh( castor3d::Mesh & mesh
			, castor3d::Material * material
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive );
		bool doProcessMeshVertices( fastgltf::Asset const & impAsset
			, fastgltf::Mesh const & impMesh
			, fastgltf::Primitive const & impPrimitive
			, castor3d::Mesh & mesh
			, castor3d::Submesh & submesh
			, castor3d::Material * material );
		void doCheckNmlTan( castor3d::Submesh & submesh
			, castor3d::IndexMappingUPtr mapping );
		void doTransformMesh( fastgltf::Node const & impNode
			, castor::Vector< fastgltf::Node > const & impNodes
			, castor3d::Mesh & mesh
			, castor::Matrix4x4f transformAcc = castor::Matrix4x4f{ 1.0f } );
	};
}

#endif
