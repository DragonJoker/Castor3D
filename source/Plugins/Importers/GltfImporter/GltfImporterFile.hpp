/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GltfImporterFile___
#define ___C3D_GltfImporterFile___

#include <Castor3D/ImporterFile.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#ifdef None
#	undef None
#endif
#pragma warning( disable: 4715 )
#include <fastgltf/parser.hpp>
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
	struct ElementTraits< castor::Point2ub > : ElementTraitsBase< uint8_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< castor::Point3ub > : ElementTraitsBase< uint8_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< castor::Point4ub > : ElementTraitsBase< uint8_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Point2us > : ElementTraitsBase< uint16_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< castor::Point3us > : ElementTraitsBase< uint16_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< castor::Point4us > : ElementTraitsBase< uint16_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Point2ui > : ElementTraitsBase< uint32_t, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< castor::Point3ui > : ElementTraitsBase< uint32_t, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< castor::Point4ui > : ElementTraitsBase< uint32_t, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Point2f > : ElementTraitsBase< float, AccessorType::Vec2 > {};
	template<>
	struct ElementTraits< castor::Point3f > : ElementTraitsBase< float, AccessorType::Vec3 > {};
	template<>
	struct ElementTraits< castor::Point4f > : ElementTraitsBase< float, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Quaternion > : ElementTraitsBase< float, AccessorType::Vec4 > {};
	template<>
	struct ElementTraits< castor::Matrix4x4f > : ElementTraitsBase< float, AccessorType::Mat4 > {};
}

namespace c3d_gltf
{
	inline const castor::String DefaultMaterial = cuT( "GLTF_DefaultMaterial" );

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

	castor3d::NodeTransform convert( std::variant< fastgltf::Node::TRS, fastgltf::Node::TransformMatrix > const & transform );
	castor::Point3f convert( std::array< float, 3u > const & value );
	castor::Quaternion convert( std::array< float, 4u > const & value );

	using AnimationChannelSampler = std::pair< fastgltf::AnimationChannel, fastgltf::AnimationSampler >;
	using NodeAnimationChannelSampler = std::vector< AnimationChannelSampler >;
	using AnimationChannelSamplers = std::map< fastgltf::AnimationPath, NodeAnimationChannelSampler >;
	using Animations = std::map< castor::String, AnimationChannelSamplers >;

	struct GltfNodeData
		: castor3d::ImporterFile::NodeData
	{
		GltfNodeData( castor::String pparent
			, castor::String pname
			, size_t pindex
			, size_t pinstance
			, size_t pinstanceCount
			, castor3d::NodeTransform ptransform
			, fastgltf::Node const * pnode )
			: NodeData{ std::move( pparent )
				, std::move( pname ) }
			, index{ pindex }
			, instance{ pinstance }
			, instanceCount{ pinstanceCount }
			, transform{ std::move( ptransform ) }
			, node{ pnode }
		{
		}

		size_t index;
		size_t instance;
		size_t instanceCount;
		castor3d::NodeTransform transform{};
		fastgltf::Node const * node;
	};

	class GltfImporterFile
		: public castor3d::ImporterFile
	{
	public:
		C3D_Gltf_API GltfImporterFile( castor3d::Engine & engine
			, castor3d::Scene * scene
			, castor::Path const & path
			, castor3d::Parameters const & parameters
			, fastgltf::Category category = fastgltf::Category::All );

		static castor3d::ImporterFileUPtr create( castor3d::Engine & engine
			, castor3d::Scene * scene
			, castor::Path const & path
			, castor3d::Parameters const & parameters );

		using castor3d::ImporterFile::getInternalName;

		castor::String getMaterialName( size_t index )const;
		castor::String getMeshName( size_t index )const;
		castor::String getNodeName( size_t index, size_t instance )const;
		castor::String getSkinName( size_t index )const;
		castor::String getLightName( size_t index )const;
		castor::String getSamplerName( size_t index )const;
		castor::String getGeometryName( size_t nodeIndex, size_t meshIndex, size_t instance )const;
		castor::String getAnimationName( size_t index )const;

		size_t getNodeIndex( castor::String const & name )const;
		size_t getSkeletonNodeIndex( castor::String const & name )const;
		size_t getMeshIndex( castor::String const & name )const;

		Animations getMeshAnimations( castor3d::Mesh const & mesh )const;
		Animations getSkinAnimations( castor3d::Skeleton const & skeleton )const;
		Animations getNodeAnimations( castor3d::SceneNode const & node )const;

		bool isSkeletonNode( size_t nodeIndex )const;

		std::vector< castor::String > listMaterials()override;
		std::vector< MeshData > listMeshes()override;
		std::vector< castor::String > listSkeletons()override;
		std::vector< NodeData > listSceneNodes()override;
		std::vector< LightData > listLights()override;
		std::vector< GeometryData > listGeometries()override;
		std::vector< castor::String > listMeshAnimations( castor3d::Mesh const & mesh )override;
		std::vector< castor::String > listSkeletonAnimations( castor3d::Skeleton const & skeleton )override;
		std::vector< castor::String > listSceneNodeAnimations( castor3d::SceneNode const & node )override;

		castor3d::MaterialImporterUPtr createMaterialImporter()override;
		castor3d::AnimationImporterUPtr createAnimationImporter()override;
		castor3d::SkeletonImporterUPtr createSkeletonImporter()override;
		castor3d::MeshImporterUPtr createMeshImporter()override;
		castor3d::SceneNodeImporterUPtr createSceneNodeImporter()override;
		castor3d::LightImporterUPtr createLightImporter()override;

		std::vector< GltfNodeData > const & getNodes()const noexcept
		{
			return m_nodes;
		}

		fastgltf::Asset const & getAsset()const noexcept
		{
			return *m_asset;
		}

		bool isValid()const noexcept
		{
			return m_expAsset.error() == fastgltf::Error::None;
		}

	public:
		static castor::String const Name;

	private:
		void doPrelistNodes();

	private:
		fastgltf::Expected< fastgltf::Asset > m_expAsset;
		fastgltf::Asset const * m_asset{};
		std::vector< size_t > m_sceneIndices{};
		std::vector< GltfNodeData > m_nodes;
		std::vector< GltfNodeData > m_skeletonNodes;
	};
}

#endif
