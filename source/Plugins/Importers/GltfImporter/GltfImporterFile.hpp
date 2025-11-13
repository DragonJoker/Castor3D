/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfImporterFile___
#define ___C3D_GltfImporterFile___

#include <GltfMaterialImporter/GltfMaterialsFile.hpp>

#include <Castor3D/ImporterFile.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#ifdef None
#	undef None
#endif
#pragma warning( disable: 4715 )
#include <fastgltf/core.hpp>
#include <fastgltf/math.hpp>
#include <fastgltf/tools.hpp>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#ifndef CU_PlatformWindows
#	define C3D_Gltf_API
#else
#	ifdef GltfImporter_EXPORTS
#		define C3D_Gltf_API __declspec(dllexport)
#	else
#		define C3D_Gltf_API __declspec(dllimport)
#	endif
#endif

namespace fastgltf
{
	template<>
	struct ElementTraits< c3d::Point2ub > : ElementTraitsBase< uint8_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< c3d::Point3ub > : ElementTraitsBase< uint8_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< c3d::Point4ub > : ElementTraitsBase< uint8_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< c3d::Point2us > : ElementTraitsBase< uint16_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< c3d::Point3us > : ElementTraitsBase< uint16_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< c3d::Point4us > : ElementTraitsBase< uint16_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< c3d::Point2ui > : ElementTraitsBase< uint32_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< c3d::Point3ui > : ElementTraitsBase< uint32_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< c3d::Point4ui > : ElementTraitsBase< uint32_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< c3d::Point2f > : ElementTraitsBase< float, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< c3d::Point3f > : ElementTraitsBase< float, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< c3d::Point4f > : ElementTraitsBase< float, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< c3d::Quaternion > : ElementTraitsBase< float, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< c3d::Matrix4x4f > : ElementTraitsBase< float, AccessorType::Mat4 > {};
}

namespace c3d_gltf
{
	c3d::NodeTransform convert( std::variant< fastgltf::TRS, fastgltf::math::fmat4x4> const & transform );
	c3d::Point3f convert( fastgltf::math::fvec3 const & value );
	c3d::Quaternion convert( fastgltf::math::fquat const & value );

	using AnimationChannelSampler = c3d::Pair< fastgltf::AnimationChannel, fastgltf::AnimationSampler >;
	using NodeAnimationChannelSampler = c3d::Vector< AnimationChannelSampler >;
	using AnimationChannelSamplers = c3d::Map< fastgltf::AnimationPath, NodeAnimationChannelSampler >;
	using Animations = c3d::StringMap< AnimationChannelSamplers >;

	struct GltfSubmeshPrimitiveData
	{
		GltfSubmeshPrimitiveData( uint32_t pprimitiveIndex
			, c3d::String pmaterial
			, fastgltf::Primitive const * pprimitive )
			: primitiveIndex{ pprimitiveIndex }
			, material{ pmaterial }
			, primitive{ pprimitive }
		{
		}

		uint32_t primitiveIndex{};
		c3d::String material{};
		fastgltf::Primitive const * primitive{};
		c3d::Submesh const * submesh{};
		Animations anims;
	};

	struct GltfSubmeshData
	{
		GltfSubmeshData( fastgltf::Mesh const * pmesh
			, uint32_t pmeshIndex )
			: mesh{ pmesh }
			, meshIndex{ pmeshIndex }
		{
		}

		fastgltf::Mesh const * mesh;
		uint32_t meshIndex;
		c3d::Vector< GltfSubmeshPrimitiveData > primitives;
	};

	struct GltfMeshData
	{
		GltfMeshData( fastgltf::Skin const * pskin
			, size_t pskinIndex )
			: skin{ pskin }
			, skinIndex{ pskinIndex }
		{
		}
		c3d::Vector< GltfSubmeshData > submeshes;
		fastgltf::Skin const * skin{};
		size_t skinIndex{};
	};

	struct GltfNodeData
	{
		explicit GltfNodeData( bool pisCamera = {}
			, bool pisSkeleton = {}
			, size_t pindex = {}
			, fastgltf::Node const * pnode = {} )
			: isCamera{ pisCamera }
			, isSkeleton{ pisSkeleton }
			, index{ pindex }
			, node{ pnode }
		{
		}

		bool isCamera;
		bool isSkeleton;
		size_t index;
		fastgltf::Node const * node;
		c3d::Vector< GltfMeshData const * > meshes{};
		Animations anims;
		c3d::Vector< std::pair< c3d::ImporterFile::NodeData, c3d::NodeTransform > > instances{};
	};

	struct GltfLightData
		: c3d::ImporterFile::LightData
	{
		GltfLightData( c3d::String pname
			, c3d::LightType ptype
			, uint32_t plightIndex
			, c3d::String pnodeName )
			: LightData{ c3d::move( pname ), ptype }
			, lightIndex{ plightIndex }
			, nodeName{ c3d::move( pnodeName ) }
		{
		}

		uint32_t lightIndex{};
		c3d::String nodeName{};
	};

	struct GltfLightGroupData
		: c3d::ImporterFile::LightGroupData
	{
		GltfLightGroupData( c3d::String pname
			, c3d::LightType ptype
			, uint32_t plightIndex )
			: LightGroupData{ c3d::move( pname ), ptype }
			, lightIndex{ plightIndex }
		{
		}

		uint32_t lightIndex{};
		c3d::StringArray nodeNames{};
	};

	struct GltfSkeletonData
	{
		Animations anims;
	};

	struct GltfSceneData
	{
		c3d::Vector< GltfNodeData > nodes;
		c3d::Vector< GltfNodeData * > sortedNodes;
		c3d::Vector< GltfNodeData const * > skeletonNodes;
		c3d::StringMap< GltfMeshData > meshes;
		c3d::Vector< GltfLightData > lights;
		c3d::StringMap< GltfLightGroupData > lightGroups;
	};

	/** Replacement buffer data adapter for fastgltf which supports decompressing with EXT_meshopt_compression */
	struct CompressedBufferDataAdapter
	{
		std::vector< std::optional< fastgltf::StaticVector< std::byte > > > decompressedBuffers;

		/** Get the data pointer of a loaded (possibly compressed) buffer */
		[[nodiscard]]
		static fastgltf::span< std::byte const > getData( fastgltf::Buffer const & buffer
			, std::size_t byteOffset
			, std::size_t byteLength );

		/** Decompress all buffer views and store them in this adapter */
		bool decompress( fastgltf::Asset const & asset );

		fastgltf::span< std::byte const > operator()( fastgltf::Asset const & asset
			, std::size_t bufferViewIdx ) const;
	};

	class GltfImporterFile
		: public c3d::ImporterFile
	{
	public:
		C3D_Gltf_API GltfImporterFile( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		static c3d::ImporterFileUPtr create( c3d::Engine & engine
			, c3d::Scene * scene
			, c3d::Path const & path
			, c3d::Parameters const & parameters
			, c3d::ProgressBar * progress );

		using c3d::ImporterFile::getInternalName;

		c3d::String getMaterialName( size_t index )const;
		c3d::String getMeshName( size_t index )const;
		c3d::String getNodeName( size_t index, size_t instance )const;
		c3d::String getSkinName( size_t index )const;
		c3d::String getLightName( size_t index )const;
		c3d::String getCameraName( size_t index )const;
		c3d::String getSamplerName( fastgltf::Sampler const & impSampler )const;
		c3d::String getGeometryName( size_t nodeIndex, size_t meshIndex, size_t instance )const;
		c3d::String getAnimationName( size_t index )const;

		size_t getNodeIndex( c3d::String const & name )const;
		size_t getSkeletonNodeIndex( c3d::String const & name )const;
		size_t getMeshIndex( c3d::String const & name, c3d::Submesh const & submesh )const;

		Animations getMeshAnimations( c3d::Mesh const & mesh, c3d::Submesh const & submesh )const;
		Animations getSkinAnimations( c3d::Skeleton const & skeleton )const;
		Animations getNodeAnimations( c3d::SceneNode const & node )const;

		bool isSkeletonNode( size_t nodeIndex )const;

		c3d::StringArray listMaterials()override;
		c3d::Vector< MeshData > listMeshes()override;
		c3d::StringArray listSkeletons()override;
		c3d::Vector< NodeData > listSceneNodes()override;
		c3d::Vector< LightData > listLights()override;
		c3d::Vector< LightGroupData > listLightGroups()override;
		c3d::Vector< GeometryData > listGeometries()override;
		c3d::Vector< CameraData > listCameras()override;
		c3d::StringArray listMeshAnimations( c3d::Mesh const & mesh )override;
		c3d::StringArray listSkeletonAnimations( c3d::Skeleton const & skeleton )override;
		c3d::StringArray listSceneNodeAnimations( c3d::SceneNode const & node )override;
		c3d::Vector< uint32_t > listTextureAnimations( c3d::Material const & material
			, uint32_t pass )override;
		uint32_t countAllMeshAnimations()const override;
		uint32_t countAllSkeletonAnimations()const override;
		uint32_t countAllSceneNodeAnimations()const override;
		uint32_t countAllTextureAnimations()const override;

		c3d::MaterialImporterUPtr createMaterialImporter()override;
		c3d::AnimationImporterUPtr createAnimationImporter()override;
		c3d::SkeletonImporterUPtr createSkeletonImporter()override;
		c3d::MeshImporterUPtr createMeshImporter()override;
		c3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		c3d::LightImporterUPtr createLightImporter()override;
		c3d::CameraImporterUPtr createCameraImporter()override;

		c3d::StringMap< c3d::NodeTransform const * > const & getNodes()const noexcept
		{
			return m_nodes;
		}

		c3d::Vector< GltfLightData > const & getLights()const noexcept
		{
			return m_sceneData.lights;
		}

		c3d::StringMap< GltfLightGroupData > const & getLightGroups()const noexcept
		{
			return m_sceneData.lightGroups;
		}

		fastgltf::Asset const & getAsset()const noexcept
		{
			return m_materialsFile.getAsset();
		}

		bool isValid()const noexcept
		{
			return m_materialsFile.isValid();
		}

		auto & getMeshes()
		{
			return m_sceneData.meshes;
		}

		auto const & getAdapter()const
		{
			return m_adapter;
		}

	public:
		static c3d::MbString const Name;

	private:
		void doPrelistMaterials();
		void doPrelistNodes();
		void doPrelistMeshes();
		void doAddNode( GltfNodeData nodeData
			, bool isParentSkeletonNode
			, size_t parentIndex
			, size_t & parentInstanceCount );

	private:
		GltfMaterialsFile m_materialsFile;
		c3d::Vector< size_t > m_sceneIndices{};
		c3d::StringMap< c3d::NodeTransform const * > m_nodes{};
		CompressedBufferDataAdapter m_adapter;
		GltfSceneData m_sceneData;
		mutable NameContainer m_meshNames;
		mutable NameContainer m_nodeNames;
		mutable NameContainer m_skinNames;
		mutable NameContainer m_lightNames;
		mutable NameContainer m_cameraNames;
		mutable NameContainer m_samplerNames;
		mutable NameContainer m_animationNames;
	};
}

#endif
