#include "GltfMaterialImporter/GltfMaterialsFile.hpp"
#include "GltfMaterialImporter/GltfMaterialImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/ImporterFile.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>

#pragma optimize("", off)

namespace c3d_gltf
{
	//*********************************************************************************************

	namespace file
	{
		static c3d::RawUniquePtr< fastgltf::Asset > loadScene( c3d::Path const & filePath )
		{
			fastgltf::Parser parser{ fastgltf::Extensions::KHR_texture_transform
				| fastgltf::Extensions::MSFT_texture_dds
				| fastgltf::Extensions::KHR_mesh_quantization
				| fastgltf::Extensions::EXT_mesh_gpu_instancing
				| fastgltf::Extensions::EXT_meshopt_compression
				| fastgltf::Extensions::KHR_texture_basisu
				| fastgltf::Extensions::EXT_texture_webp
				| fastgltf::Extensions::KHR_lights_punctual
				| fastgltf::Extensions::KHR_materials_specular
				| fastgltf::Extensions::KHR_materials_pbrSpecularGlossiness
				| fastgltf::Extensions::KHR_materials_ior
				| fastgltf::Extensions::KHR_materials_iridescence
				| fastgltf::Extensions::KHR_materials_volume
				| fastgltf::Extensions::KHR_materials_transmission
				| fastgltf::Extensions::KHR_materials_clearcoat
				| fastgltf::Extensions::KHR_materials_emissive_strength
				| fastgltf::Extensions::KHR_materials_sheen
				| fastgltf::Extensions::KHR_materials_unlit
				| fastgltf::Extensions::KHR_materials_anisotropy
				| fastgltf::Extensions::KHR_materials_dispersion
				| fastgltf::Extensions::KHR_materials_diffuse_transmission };
			auto path = c3d::makePath( filePath );

			constexpr auto gltfOptions = fastgltf::Options::DontRequireValidAssetMember
				| fastgltf::Options::AllowDouble
				| fastgltf::Options::LoadExternalBuffers
				| fastgltf::Options::DecomposeNodeMatrices
				| fastgltf::Options::LoadExternalImages;

			auto dataResult = fastgltf::GltfDataBuffer::FromPath( path );

			if ( !dataResult )
			{
				c3d::log::error << "Failed to to load glTF buffer" << std::endl;
				return nullptr;
			}

			auto & data = dataResult.get();

			if ( auto type = fastgltf::determineGltfFileType( data );
				type != fastgltf::GltfType::glTF && type != fastgltf::GltfType::GLB )
			{
				c3d::log::error << "Failed to determine glTF container" << std::endl;
				return nullptr;
			}

			auto result = parser.loadGltf( data, path.parent_path(), gltfOptions );
			if ( result.error() != fastgltf::Error::None )
				c3d::log::error << "Failed to load glTF: " << c3d::makeString( fastgltf::getErrorMessage( result.error() ) ) << std::endl;
			return c3d::makeRawUnique< fastgltf::Asset >( c3d::move( result.get() ) );
		}
	}

	//*********************************************************************************************

	GltfMaterialsFile::~GltfMaterialsFile()noexcept = default;

	GltfMaterialsFile::GltfMaterialsFile( c3d::Engine & engine
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::String const & prefix
		, c3d::String const & name
		, c3d::HashMap< c3d::String, c3d::String > const & materialsNames )
		: m_asset{ file::loadScene( path ) }
		, m_prefix{ prefix }
		, m_name{ name }
	{
		if ( isValid() )
		{
			engine.getMaterialCache().forEach( [this]( c3d::Material const & element )
				{
					m_materialNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_materialNames.namesByIndex.size(), element.getName() );
					m_materialNames.names.emplace( element.getName() );
				} );
			doPrelistMaterials( parameters, materialsNames );
		}
	}

	GltfMaterialsFile::GltfMaterialsFile( c3d::Engine & engine
		, c3d::Path const & path
		, c3d::Parameters const & parameters
		, c3d::String const & prefix
		, c3d::String const & name )
		: m_asset{ file::loadScene( path ) }
		, m_prefix{ prefix }
		, m_name{ name }
	{
		if ( isValid() )
		{
			engine.getMaterialCache().forEach( [this]( c3d::Material const & element )
				{
					m_materialNames.namesByIndex.try_emplace( 0xFFFFFFFF00000000ULL + m_materialNames.namesByIndex.size(), element.getName() );
					m_materialNames.names.emplace( element.getName() );
				} );
			doPrelistMaterials( parameters );
		}
	}

	c3d::MaterialImporterUPtr GltfMaterialsFile::createMaterialImporter( c3d::Engine & engine )
	{
		return c3d::makeUniqueDerived< c3d::MaterialImporter, GltfMaterialImporter >( engine, *this );
	}

	c3d::String GltfMaterialsFile::getSamplerName( c3d::Engine const & engine
		, fastgltf::Sampler const & impSampler )const
	{
		auto const & defaultSampler = *engine.getDefaultSampler();
		return c3d::getSamplerName( c3d::ComparisonFunc::eNever
			, impSampler.minFilter ? convert( *impSampler.minFilter ) : defaultSampler.getMinFilter()
			, impSampler.magFilter ? convert( *impSampler.magFilter ) : defaultSampler.getMagFilter()
			, impSampler.minFilter ? getMipFilter( *impSampler.minFilter ) : defaultSampler.getMipFilter()
			, convert( impSampler.wrapS )
			, convert( impSampler.wrapT )
			, defaultSampler.getWrapR() );
	}

	c3d::String GltfMaterialsFile::getMaterialName( size_t index )const
	{
		return getInternalName( getElementName( getAsset().materials, index, getName(), m_materialNames ) );
	}

	void GltfMaterialsFile::doPrelistMaterials( [[maybe_unused]] c3d::Parameters const & parameters
		, c3d::HashMap< c3d::String, c3d::String > const & materialsNames )
	{
		uint32_t materialIndex = 0u;

		for ( auto const & gltfMaterial : getAsset().materials )
		{
			auto name = c3d::makeString( gltfMaterial.name );
			if ( auto it = materialsNames.find( name );
				it != materialsNames.end() )
				name = it->second;
			m_materials.try_emplace( name, materialIndex );
			++materialIndex;
		}
	}

	void GltfMaterialsFile::doPrelistMaterials( [[maybe_unused]] c3d::Parameters const & parameters )
	{
		for ( uint32_t materialIndex = 0u; materialIndex < getAsset().materials.size(); ++materialIndex )
			m_materials.try_emplace( getMaterialName( materialIndex ), materialIndex );
	}

	//*********************************************************************************************
}
