#include "GltfImporter/GltfMaterialImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Limits.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Miscellaneous/ConfigurationVisitor.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/PbrPass.hpp>
#include <Castor3D/Material/Pass/Component/Base/BlendComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TextureCountComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/AttenuationComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/IridescenceComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SheenComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/ThicknessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/AttenuationMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ClearcoatMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ClearcoatNormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ClearcoatRoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/GlossinessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/IridescenceMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/IridescenceThicknessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OcclusionMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SheenMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SheenRoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SpecularFactorMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/TransmissionMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/TransmittanceMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ColourComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/HeightComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ReflectionComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Shader/LightingModelFactory.hpp>

// Materials
#include <EdgesComponent.hpp>
#include <Shaders/GlslToonLighting.hpp>

namespace c3d_gltf
{
	//*********************************************************************************************

	namespace materials
	{
		static castor3d::LightingModelID getLightingModel( castor3d::Engine const & engine )
		{
			auto & factory = engine.getLightingModelFactory();
			return factory.getNameId( castor3d::PbrPass::LightingModel );
		}

		template< typename ComponentT >
		static void parseComponentData( castor3d::Pass & pass, bool value )
		{
			if ( value )
			{
				auto component = pass.createComponent< ComponentT >();
				auto data = component->getData();
				*data = value;
				component->setData( *data );
			}
		}

		template< typename ComponentT >
		static void parseComponentData( castor3d::Pass & pass, castor::HdrRgbColour value )
		{
			auto component = pass.createComponent< ComponentT >();
			auto data = component->getData();
			*data = std::move( value );
			component->setData( *data );
		}

		template< typename ComponentT >
		static void parseComponentData( castor3d::Pass & pass, float value )
		{
			auto component = pass.createComponent< ComponentT >();
			auto data = component->getData();
			*data = std::move( value );
			component->setData( *data );
		}

		static std::string getFormatName( fastgltf::MimeType mimeType )
		{
			switch ( mimeType )
			{
			case fastgltf::MimeType::JPEG:
				return "jpg";
			case fastgltf::MimeType::PNG:
			case fastgltf::MimeType::GltfBuffer:
			case fastgltf::MimeType::OctetStream:
				return "png";
			case fastgltf::MimeType::KTX2:
				return "ktx";
			case fastgltf::MimeType::DDS:
				return "dds";
			case fastgltf::MimeType::None:
			default:
				return "";
			}
		}

		static castor::ByteArray getData( fastgltf::Asset const & impAsset
			, fastgltf::sources::URI const & impData
			, size_t offset
			, size_t size )
		{
			auto path = impData.uri.fspath();
			std::ifstream stream{ path, std::ios::binary };
			stream.seekg( 0u, std::ios::end );
			auto fileSize = std::min( size_t( stream.tellg() ), size );
			stream.seekg( std::streampos( int64_t( offset + impData.fileByteOffset ) ) );
			castor::ByteArray result;
			auto count = fileSize - impData.fileByteOffset;
			result.resize( count );
			stream.readsome( reinterpret_cast< char * >( result.data() ), std::streamsize( count ) );
			return result;
		}

		static castor::ByteArray getData( fastgltf::Asset const & impAsset
			, fastgltf::sources::Vector const & impData
			, size_t offset
			, size_t size )
		{
			castor::ByteArray result;
			size = std::min( size, impData.bytes.size() );
			result.resize( size );
			std::memcpy( result.data(), impData.bytes.data() + offset, size );
			return result;
		}

		static castor::ByteArray getData( fastgltf::Asset const & impAsset
			, fastgltf::sources::CustomBuffer const & impData
			, size_t offset
			, size_t size )
		{
			castor::ByteArray result;
			return result;
		}

		static castor::ByteArray getData( fastgltf::Asset const & impAsset
			, fastgltf::sources::ByteView const & impData
			, size_t offset
			, size_t size )
		{
			castor::ByteArray result;
			size = std::min( size, impData.bytes.size() );
			result.resize( size );
			std::memcpy( result.data(), impData.bytes.data() + offset, size );
			return result;
		}

		static castor::String makeTextureName( size_t textureIndex
			, fastgltf::Texture const & impTexture
			, size_t imageIndex
			, fastgltf::Image const & impImage )
		{
			auto texName = castor::String( impTexture.name );
			auto imgName = castor::String( impImage.name );

			if ( texName.empty() )
			{
				texName = castor::string::toString( textureIndex );
			}

			if ( imgName.empty() )
			{
				imgName = castor::string::toString( imageIndex );
			}

			return "Image_" + texName + "_" + imgName;
		}

		static std::unique_ptr< castor3d::TextureSourceInfo > loadTexture( fastgltf::Asset const & impAsset
			, castor::String const & name
			, fastgltf::Texture const & impTexture
			, fastgltf::Image const & impImage
			, fastgltf::DataSource const & impDataSource
			, castor3d::TextureConfiguration const & texConfig
			, castor3d::MaterialImporter & importer
			, size_t offset = 0u
			, size_t size = 0xFFFFFFFFFFFFFFFF )
		{
			if ( impDataSource.index() == 1u )
			{
				fastgltf::BufferView const & impBufferView = impAsset.bufferViews[std::get< 1 >( impDataSource ).bufferViewIndex];
				fastgltf::Buffer const & impBuffer = impAsset.buffers[impBufferView.bufferIndex];
				return loadTexture( impAsset, name, impTexture, impImage, impBuffer.data, texConfig, importer, offset + impBufferView.byteOffset, impBufferView.byteLength );
			}

			fastgltf::MimeType mimeType{};
			castor::ByteArray data;

			switch ( impDataSource.index() )
			{
			case 2:
				mimeType = std::get< 2 >( impDataSource ).mimeType;
				data = getData( impAsset, std::get< 2 >( impDataSource ), offset, size );
				break;
			case 3:
				mimeType = std::get< 3 >( impDataSource ).mimeType;
				data = getData( impAsset, std::get< 3 >( impDataSource ), offset, size );
				break;
			case 4:
				mimeType = std::get< 4 >( impDataSource ).mimeType;
				data = getData( impAsset, std::get< 4 >( impDataSource ), offset, size );
				break;
			case 5:
				mimeType = std::get< 5 >( impDataSource ).mimeType;
				data = getData( impAsset, std::get< 5 >( impDataSource ), offset, size );
				break;
			default:
				break;
			}

			if ( !data.empty() )
			{
				return std::make_unique< castor3d::TextureSourceInfo >( importer.loadTexture( name
					, getFormatName( mimeType )
					, std::move( data )
					, texConfig ) );
			}

			return nullptr;
		}

		static VkSamplerAddressMode convert( fastgltf::Wrap const & v )
		{
			switch ( v )
			{
			case fastgltf::Wrap::Repeat:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			case fastgltf::Wrap::ClampToEdge:
				return VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE;
			case fastgltf::Wrap::MirroredRepeat:
				return VK_SAMPLER_ADDRESS_MODE_MIRRORED_REPEAT;
			default:
				return VK_SAMPLER_ADDRESS_MODE_REPEAT;
			}
		}

		static VkFilter convert( fastgltf::Filter const & v )
		{
			switch ( v )
			{
			case fastgltf::Filter::Nearest:
			case fastgltf::Filter::NearestMipMapNearest:
			case fastgltf::Filter::NearestMipMapLinear:
				return VK_FILTER_NEAREST;
			default:
				return VK_FILTER_LINEAR;
			}
		}

		static VkSamplerMipmapMode getMipFilter( fastgltf::Filter const & v )
		{
			switch ( v )
			{
			case fastgltf::Filter::Nearest:
			case fastgltf::Filter::NearestMipMapNearest:
			case fastgltf::Filter::LinearMipMapNearest:
				return VK_SAMPLER_MIPMAP_MODE_NEAREST;
			default:
				return VK_SAMPLER_MIPMAP_MODE_LINEAR;
			}
		}

		static castor3d::SamplerRPtr loadSampler( GltfImporterFile const & file
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< size_t > const & samplerIndex )
		{
			auto & engine = *file.getOwner();

			if ( samplerIndex
				&& *samplerIndex < impAsset.samplers.size() )
			{
				fastgltf::Sampler const & impSampler = impAsset.samplers[*samplerIndex];
				auto & cache = engine.getSamplerCache();
				auto name = file.getSamplerName( *samplerIndex );

				if ( !cache.has( name ) )
				{
					auto defaultSampler = engine.getDefaultSampler();
					auto sampler = engine.createSampler( name, engine );
					sampler->setMinFilter( impSampler.minFilter ? convert( *impSampler.minFilter ) : defaultSampler->getMinFilter() );
					sampler->setMagFilter( impSampler.magFilter ? convert( *impSampler.magFilter ) : defaultSampler->getMagFilter() );
					sampler->setMipFilter( impSampler.minFilter ? getMipFilter( *impSampler.minFilter ) : defaultSampler->getMipFilter() );
					sampler->setWrapS( convert( impSampler.wrapS ) );
					sampler->setWrapT( convert( impSampler.wrapT ) );
					sampler->setWrapR( defaultSampler->getWrapR() );
					cache.add( name, sampler, false );
				}

				return cache.find( name );
			}

			return engine.getDefaultSampler();
		}

		static std::unique_ptr< castor3d::TextureSourceInfo > loadTexture( GltfImporterFile const & file
			, fastgltf::Asset const & impAsset
			, fastgltf::TextureInfo const & texInfo
			, castor3d::TextureConfiguration const & texConfig
			, castor3d::MaterialImporter & importer )
		{
			std::unique_ptr< castor3d::TextureSourceInfo > result;

			if ( texInfo.textureIndex < impAsset.textures.size()
				&& impAsset.textures[texInfo.textureIndex].imageIndex
				&& *impAsset.textures[texInfo.textureIndex].imageIndex < impAsset.images.size() )
			{
				fastgltf::Texture const & impTexture = impAsset.textures[texInfo.textureIndex];
				fastgltf::Image const & impImage = impAsset.images[*impTexture.imageIndex];
				result = loadTexture( impAsset
					, makeTextureName( texInfo.textureIndex, impTexture, *impTexture.imageIndex, impImage )
					, impTexture
					, impImage
					, impImage.data
					, texConfig
					, importer );
			}

			return result;
		}

		static castor::Image const & loadImage( castor3d::TextureSourceInfo const & source
			, castor3d::MaterialImporter & importer )
		{
			castor::ImageRPtr result{};

			if ( source.isBufferImage() )
			{
				result = importer.loadImage( source.name()
					, castor::ImageCreateParams{ source.type()
					, source.buffer()
					, { false, false, false } } );
			}
			else if ( source.isFileImage() )
			{
				result = importer.loadImage( source.name()
					, castor::ImageCreateParams{ source.folder() / source.relative()
					, { false, false, false } } );
			}

			if ( !result )
			{
				CU_LoaderError( "Couldn't load image" + source.name() + "." );
			}

			return *result;
		}

		static void parseTransform( std::unique_ptr< fastgltf::TextureTransform > const & transform
			, castor3d::TextureTransform & result
			, uint32_t & texCoordIndex )
		{
			if ( transform )
			{
				result.scale = { transform->uvScale[0], transform->uvScale[1], 1.0f };
				result.rotate = castor::Angle::fromRadians( -transform->rotation );// must be negated

				// A change of coordinates is required to map glTF UV transformations into the space used by
				// Castor3D. In glTF all UV origins are at 0,1 (top left of texture) in Castor3D space. In Castor3D
				// rotation occurs around the image center (0.5,0.5) where as in glTF rotation is around the
				// texture origin. All three can be corrected for solely by a change of the translation since
				// the transformations available are shape preserving. Note that importer already flips the V
				// coordinate of the actual meshes during import.
				float const rcos( ( -result.rotate ).cos() );
				float const rsin( ( -result.rotate ).sin() );
				result.translate->x = ( 0.5f * result.scale->x ) * ( -rcos + rsin + 1 ) + transform->uvOffset[0];
				result.translate->y = ( 0.5f * result.scale->y ) * (  rsin + rcos - 1 ) + 1 - result.scale->y - transform->uvOffset[1];

				if ( transform->texCoordIndex )
				{
					texCoordIndex = uint32_t( *transform->texCoordIndex );
				}
			}
		}

		static void parseColOpaTexture( GltfImporterFile const & file
			, castor3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, std::map< castor3d::PassComponentTextureFlag, castor3d::TextureConfiguration > const & textureRemaps
			, std::optional< fastgltf::TextureInfo > const & texInfo
			, castor3d::MaterialImporter & importer )
		{
			if ( texInfo )
			{
				auto texConfig = pass.getComponentPlugin< castor3d::ColourMapComponent >().getBaseTextureConfiguration();

				if ( auto sourceInfo = loadTexture( file, impAsset, *texInfo, texConfig, importer ) )
				{
					auto & image = loadImage( *sourceInfo, importer );

					if ( hasAlphaChannel( image ) )
					{
						addFlagConfiguration( texConfig, { pass.getComponentPlugin< castor3d::OpacityMapComponent >().getTextureFlags(), 0xFF000000 } );
					}

					fastgltf::Texture const & impTexture = impAsset.textures[texInfo->textureIndex];
					auto texCoordIndex = uint32_t( texInfo->texCoordIndex );
					parseTransform( texInfo->transform, texConfig.transform, texCoordIndex );
					castor3d::PassTextureConfig passTexConfig{ loadSampler( file, impAsset, impTexture.samplerIndex ), texCoordIndex };
					pass.registerTexture( std::move( *sourceInfo ), passTexConfig );
				}
			}
		}

		static void parseRghMetTexture( GltfImporterFile const & file
			, castor3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, std::map< castor3d::PassComponentTextureFlag, castor3d::TextureConfiguration > const & textureRemaps
			, std::optional< fastgltf::TextureInfo > const & texInfo
			, castor3d::MaterialImporter & importer )
		{
			if ( texInfo )
			{
				auto texConfig = pass.getComponentPlugin< castor3d::RoughnessMapComponent >().getBaseTextureConfiguration();
				texConfig.components[1] = pass.getComponentPlugin< castor3d::MetalnessMapComponent >().getBaseTextureConfiguration().components[0];
				texConfig.components[0].componentsMask = 0x0000FF00;
				texConfig.components[1].componentsMask = 0x000000FF;

				if ( auto sourceInfo = loadTexture( file, impAsset, *texInfo, texConfig, importer ) )
				{
					fastgltf::Texture const & impTexture = impAsset.textures[texInfo->textureIndex];
					auto texCoordIndex = uint32_t( texInfo->texCoordIndex );
					parseTransform( texInfo->transform, texConfig.transform, texCoordIndex );
					castor3d::PassTextureConfig passTexConfig{ loadSampler( file, impAsset, impTexture.samplerIndex ), texCoordIndex };
					pass.registerTexture( std::move( *sourceInfo ), passTexConfig );
				}
			}
		}

		static void parseSpcGlsTexture( GltfImporterFile const & file
			, castor3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, std::map< castor3d::PassComponentTextureFlag, castor3d::TextureConfiguration > const & textureRemaps
			, std::optional< fastgltf::TextureInfo > const & texInfo
			, castor3d::MaterialImporter & importer )
		{
			if ( texInfo )
			{
				auto texConfig = pass.getComponentPlugin< castor3d::SpecularMapComponent >().getBaseTextureConfiguration();
				texConfig.components[1] = pass.getComponentPlugin< castor3d::GlossinessMapComponent >().getBaseTextureConfiguration().components[0];
				texConfig.components[0].componentsMask = 0x00FFFFFF;
				texConfig.components[1].componentsMask = 0xFF000000;

				if ( auto sourceInfo = loadTexture( file, impAsset, *texInfo, texConfig, importer ) )
				{
					fastgltf::Texture const & impTexture = impAsset.textures[texInfo->textureIndex];
					auto texCoordIndex = uint32_t( texInfo->texCoordIndex );
					parseTransform( texInfo->transform, texConfig.transform, texCoordIndex );
					castor3d::PassTextureConfig passTexConfig{ loadSampler( file, impAsset, impTexture.samplerIndex ), texCoordIndex };
					pass.registerTexture( std::move( *sourceInfo ), passTexConfig );
				}
			}
		}

		template< typename ComponentT >
		static void parseTexture( GltfImporterFile const & file
			, castor3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, std::map< castor3d::PassComponentTextureFlag, castor3d::TextureConfiguration > const & textureRemaps
			, std::optional< fastgltf::TextureInfo > const & texInfo
			, castor3d::MaterialImporter & importer )
		{
			if ( texInfo )
			{
				castor3d::TextureConfiguration texConfig = pass.getComponentPlugin< ComponentT >().getBaseTextureConfiguration();

				if ( auto sourceInfo = loadTexture( file, impAsset, *texInfo, texConfig, importer ) )
				{
					fastgltf::Texture const & impTexture = impAsset.textures[texInfo->textureIndex];
					auto texCoordIndex = uint32_t( texInfo->texCoordIndex );
					parseTransform( texInfo->transform, texConfig.transform, texCoordIndex );
					castor3d::PassTextureConfig passTexConfig{ loadSampler( file, impAsset, impTexture.samplerIndex ), texCoordIndex };
					pass.registerTexture( std::move( *sourceInfo ), passTexConfig );
				}
			}
		}
	}

	//*********************************************************************************************

	GltfMaterialImporter::GltfMaterialImporter( castor3d::Engine & engine )
		: castor3d::MaterialImporter{ engine }
	{
		if ( !engine.hasMaterial( DefaultMaterial ) )
		{
			auto defaultMaterial = engine.createMaterial( DefaultMaterial
				, engine
				, materials::getLightingModel( engine ) );
			defaultMaterial->createPass();
			defaultMaterial->setSerialisable( false );
			engine.addMaterial( DefaultMaterial, defaultMaterial, true );
		}

	}

	GltfMaterialImporter::GltfMaterialImporter( castor3d::Engine & engine
		, GltfImporterFile * file )
		: castor3d::MaterialImporter{ engine, file }
	{
		if ( !engine.hasMaterial( DefaultMaterial ) )
		{
			auto defaultMaterial = engine.createMaterial( DefaultMaterial
				, engine
				, materials::getLightingModel( engine ) );
			defaultMaterial->createPass();
			defaultMaterial->setSerialisable( false );
			engine.addMaterial( DefaultMaterial, defaultMaterial, true );
		}

	}

	bool GltfMaterialImporter::doImportMaterial( castor3d::Material & material )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );

		if ( !file.isValid() )
		{
			return false;
		}

		auto & impAsset = file.getAsset();
		auto name = material.getName();
		uint32_t index{};
		auto it = std::find_if( impAsset.materials.begin()
			, impAsset.materials.end()
			, [&file, &name, &index]( fastgltf::Material const & lookup )
			{
				return name == file.getMaterialName( index++ );
			} );

		if ( it == impAsset.materials.end() )
		{
			return false;
		}

		castor3d::log::info << cuT( "  Material found: [" ) << name << cuT( "]" ) << std::endl;
		fastgltf::Material const & impMaterial = *it;

		float emissiveMult = 1.0f;
		float value;

		if ( m_parameters.get( cuT( "emissive_mult" ), value )
			&& std::abs( value - 1.0f ) > std::numeric_limits< float >::epsilon() )
		{
			emissiveMult = value;
		}

		auto pass = material.createPass( materials::getLightingModel( *getEngine() ) );
		materials::parseComponentData< castor3d::TwoSidedComponent >( *pass, impMaterial.doubleSided );
		materials::parseTexture< castor3d::NormalMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.normalTexture, *this );
		materials::parseTexture< castor3d::OcclusionMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.occlusionTexture, *this );
		materials::parseTexture< castor3d::EmissiveMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.emissiveTexture, *this );

		if ( impMaterial.specular || !impMaterial.specularGlossiness )
		{
			pass->createComponent< castor3d::ColourComponent >()->setColour( castor::HdrRgbColour::fromComponents( impMaterial.pbrData.baseColorFactor[0]
				, impMaterial.pbrData.baseColorFactor[1]
				, impMaterial.pbrData.baseColorFactor[2] ) );

			if ( impMaterial.alphaMode != fastgltf::AlphaMode::Opaque )
			{
				pass->createComponent< castor3d::OpacityComponent >()->setOpacity( impMaterial.pbrData.baseColorFactor[3] );
			}

			pass->createComponent< castor3d::MetalnessComponent >()->setMetalness( impMaterial.pbrData.metallicFactor );
			pass->createComponent< castor3d::RoughnessComponent >()->setRoughness( impMaterial.pbrData.roughnessFactor );

			materials::parseColOpaTexture( file, *pass, impAsset, m_textureRemaps, impMaterial.pbrData.baseColorTexture, *this );
			materials::parseRghMetTexture( file, *pass, impAsset, m_textureRemaps, impMaterial.pbrData.metallicRoughnessTexture, *this );

			if ( impMaterial.specular )
			{
				auto spcComponent = pass->createComponent< castor3d::SpecularComponent >();
				auto fctComponent = pass->createComponent< castor3d::SpecularFactorComponent >();
				fctComponent->setFactor( impMaterial.specular->specularFactor );
				spcComponent->setSpecular( castor::RgbColour::fromComponents( impMaterial.specular->specularColorFactor[0]
					, impMaterial.specular->specularColorFactor[1]
					, impMaterial.specular->specularColorFactor[2] ) );
				materials::parseTexture< castor3d::SpecularMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.specular->specularColorTexture, *this );
				materials::parseTexture< castor3d::SpecularFactorMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.specular->specularTexture, *this );
			}
		}
		else if ( impMaterial.specularGlossiness )
		{
			pass->createComponent< castor3d::ColourComponent >()->setColour( castor::HdrRgbColour::fromComponents( impMaterial.specularGlossiness->diffuseFactor[0]
				, impMaterial.specularGlossiness->diffuseFactor[1]
				, impMaterial.specularGlossiness->diffuseFactor[2] ) );

			if ( impMaterial.alphaMode != fastgltf::AlphaMode::Opaque )
			{
				pass->createComponent< castor3d::OpacityComponent >()->setOpacity( impMaterial.specularGlossiness->diffuseFactor[3] );
			}

			auto spcComponent = pass->createComponent< castor3d::SpecularComponent >();
			auto fctComponent = pass->createComponent< castor3d::SpecularFactorComponent >();
			fctComponent->setFactor( 1.0f );
			spcComponent->setSpecular( castor::RgbColour::fromComponents( impMaterial.specularGlossiness->specularFactor[0]
				, impMaterial.specularGlossiness->specularFactor[1]
				, impMaterial.specularGlossiness->specularFactor[2] ) );
			auto rghComponent = pass->createComponent< castor3d::RoughnessComponent >();
			rghComponent->setGlossiness( impMaterial.specularGlossiness->glossinessFactor );

			materials::parseColOpaTexture( file, *pass, impAsset, m_textureRemaps, impMaterial.specularGlossiness->diffuseTexture, *this );
			materials::parseSpcGlsTexture( file, *pass, impAsset, m_textureRemaps, impMaterial.specularGlossiness->specularGlossinessTexture, *this );
		}

		if ( impMaterial.unlit )
		{
			pass->enableLighting( false );
		}

		if ( impMaterial.iridescence )
		{
			auto component = pass->createComponent< castor3d::IridescenceComponent >();
			component->setFactor( impMaterial.iridescence->iridescenceFactor );
			component->setIor( impMaterial.iridescence->iridescenceIor );
			component->setMinThickness( impMaterial.iridescence->iridescenceThicknessMinimum );
			component->setMaxThickness( impMaterial.iridescence->iridescenceThicknessMaximum );
			materials::parseTexture< castor3d::IridescenceMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.iridescence->iridescenceTexture, *this );
			materials::parseTexture< castor3d::IridescenceThicknessMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.iridescence->iridescenceThicknessTexture, *this );
		}

		if ( impMaterial.volume )
		{
			auto attenuationComponent = pass->createComponent< castor3d::AttenuationComponent >();
			attenuationComponent->setAttenuationColour( castor::RgbColour::fromComponents( impMaterial.volume->attenuationColor[0]
				, impMaterial.volume->attenuationColor[1]
				, impMaterial.volume->attenuationColor[2] ) );
			attenuationComponent->setAttenuationDistance( impMaterial.volume->attenuationDistance );
			
			auto thicknessComponent = pass->createComponent< castor3d::ThicknessComponent >();
			thicknessComponent->setThicknessFactor( impMaterial.volume->thicknessFactor );
			materials::parseTexture< castor3d::AttenuationMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.volume->thicknessTexture, *this );
		}

		if ( impMaterial.transmission )
		{
			pass->createComponent< castor3d::TransmissionComponent >()->setTransmission( impMaterial.transmission->transmissionFactor );
			materials::parseTexture< castor3d::TransmissionMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.transmission->transmissionTexture, *this );
		}

		if ( impMaterial.clearcoat )
		{
			auto component = pass->createComponent< castor3d::ClearcoatComponent >();
			component->setClearcoatFactor( impMaterial.clearcoat->clearcoatFactor );
			component->setRoughnessFactor( impMaterial.clearcoat->clearcoatRoughnessFactor );
			materials::parseTexture< castor3d::ClearcoatMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.clearcoat->clearcoatTexture, *this );
			materials::parseTexture< castor3d::ClearcoatNormalMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.clearcoat->clearcoatNormalTexture, *this );
			materials::parseTexture< castor3d::ClearcoatRoughnessMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.clearcoat->clearcoatRoughnessTexture, *this );
		}

		if ( impMaterial.sheen )
		{
			auto component = pass->createComponent< castor3d::SheenComponent >();
			component->setSheenFactor( castor::HdrRgbColour::fromComponents( impMaterial.sheen->sheenColorFactor[0]
				, impMaterial.sheen->sheenColorFactor[1]
				, impMaterial.sheen->sheenColorFactor[2] ) );
			component->setRoughnessFactor( impMaterial.sheen->sheenRoughnessFactor );
			materials::parseTexture< castor3d::SheenMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.sheen->sheenColorTexture, *this );
			materials::parseTexture< castor3d::SheenRoughnessMapComponent >( file, *pass, impAsset, m_textureRemaps, impMaterial.sheen->sheenRoughnessTexture, *this );
		}

		if ( impMaterial.emissiveStrength
			|| impMaterial.emissiveTexture
			|| std::any_of( impMaterial.emissiveFactor.begin()
				, impMaterial.emissiveFactor.end()
				, []( float const lookup ){ return lookup != 0.0f; } ) )
		{
			auto component = pass->createComponent< castor3d::EmissiveComponent >();

			if ( impMaterial.emissiveStrength )
			{
				component->setEmissiveFactor( *impMaterial.emissiveStrength * emissiveMult );
			}

			component->setEmissive( castor::RgbColour::fromComponents( impMaterial.emissiveFactor[0]
				, impMaterial.emissiveFactor[1]
				, impMaterial.emissiveFactor[2] ) );
		}

		if ( impMaterial.alphaMode == fastgltf::AlphaMode::Mask )
		{
			pass->createComponent< castor3d::OpacityComponent >();
			auto alphaTest = pass->createComponent< castor3d::AlphaTestComponent >();
			alphaTest->setAlphaRefValue( impMaterial.alphaCutoff );
			alphaTest->setAlphaFunc( VK_COMPARE_OP_GREATER );
			alphaTest->setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );
		}
		else if ( impMaterial.alphaMode == fastgltf::AlphaMode::Blend )
		{
			pass->createComponent< castor3d::OpacityComponent >();

			auto twoSided = pass->createComponent< castor3d::TwoSidedComponent >();
			twoSided->setTwoSided( true );

			if ( !pass->hasComponent< castor3d::AlphaTestComponent >() )
			{
				auto alphaTest = pass->createComponent< castor3d::AlphaTestComponent >();
				alphaTest->setAlphaRefValue( 0.95f );
				alphaTest->setAlphaFunc( VK_COMPARE_OP_GREATER );
				alphaTest->setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );
			}

			auto blend = pass->createComponent< castor3d::BlendComponent >();
			blend->setAlphaBlendMode( castor3d::BlendMode::eInterpolative );
		}

		if ( impMaterial.ior )
		{
			pass->createComponent< castor3d::RefractionComponent >()->setRefractionRatio( *impMaterial.ior );
			auto transmission = pass->getComponent< castor3d::TransmissionComponent >();

			if ( !transmission )
			{
				transmission = pass->createComponent< castor3d::TransmissionComponent >();
				transmission->setTransmission( 0.0f );
				return true;
			}
		}

		pass->prepareTextures();
		return true;
	}

	//*********************************************************************************************
}
