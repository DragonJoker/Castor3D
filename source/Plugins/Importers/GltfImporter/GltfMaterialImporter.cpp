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
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/AttenuationComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/DiffuseTransmissionComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/DispersionComponent.hpp>
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
#include <Castor3D/Material/Pass/Component/Map/ClearcoatMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ClearcoatNormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ClearcoatRoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/DiffuseTransmissionColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/DiffuseTransmissionFactorMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp>
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
#include <Castor3D/Material/Pass/Component/Map/ThicknessMapComponent.hpp>
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
#include <AnisotropyComponent.hpp>
#include <AnisotropyStrengthMapComponent.hpp>
#include <AnisotropyDirectionMapComponent.hpp>
#include <Shaders/GlslAnisotropicBRDF.hpp>
#include <Shaders/GlslToonLighting.hpp>

namespace c3d_gltf
{
	//*********************************************************************************************

	namespace materials
	{
		static c3d::LightingModelID getLightingModel( c3d::Engine const & engine
			, bool anisotropy )
		{
			auto & factory = engine.getLightingModelFactory();
			return factory.getLightingModelId( c3d::String{ c3d::PbrPass::LightingModel }
				, { c3d::PbrPass::DefaultDiffuseBrdf.name
					, ( anisotropy
						? c3d::String{ anisotropy::shader::AnisotropicBRDF::Name }
						: c3d::PbrPass::DefaultSpecularBrdf.name )
					, c3d::PbrPass::DefaultSheenBrdf.name
					, c3d::PbrPass::DefaultClearcoatBrdf.name
					, c3d::PbrPass::DefaultScatteringModel.name } );
		}

		template< typename ComponentT >
		static void parseComponentData( c3d::Pass & pass, bool value )
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
		static void parseComponentData( c3d::Pass & pass, c3d::HdrRgbColour value )
		{
			auto component = pass.createComponent< ComponentT >();
			auto data = component->getData();
			*data = c3d::move( value );
			component->setData( *data );
		}

		template< typename ComponentT >
		static void parseComponentData( c3d::Pass & pass, float value )
		{
			auto component = pass.createComponent< ComponentT >();
			auto data = component->getData();
			*data = value;
			component->setData( *data );
		}

		static c3d::String getFormatName( fastgltf::MimeType mimeType
			, fastgltf::MimeType defaultMimeType )
		{
			switch ( mimeType )
			{
			case fastgltf::MimeType::JPEG:
				return cuT( "jpg" );
			case fastgltf::MimeType::PNG:
				return cuT( "png" );
			case fastgltf::MimeType::KTX2:
				return cuT( "ktx2" );
			case fastgltf::MimeType::DDS:
				return cuT( "dds" );
			case fastgltf::MimeType::WEBP:
				return cuT( "webp" );
			default:
				switch ( defaultMimeType )
				{
				case fastgltf::MimeType::JPEG:
					return cuT( "jpg" );
				case fastgltf::MimeType::PNG:
				case fastgltf::MimeType::GltfBuffer:
				case fastgltf::MimeType::OctetStream:
					return cuT( "png" );
				case fastgltf::MimeType::KTX2:
					return cuT( "ktx2" );
				case fastgltf::MimeType::DDS:
					return cuT( "dds" );
				case fastgltf::MimeType::WEBP:
					return cuT( "webp" );
				default:
					return cuT( "png" );
				}
			}
		}

		static void getData( [[maybe_unused]] fastgltf::Asset const & impAsset
			, fastgltf::sources::URI const & impData
			, size_t offset
			, size_t size
			, fastgltf::MimeType & mimeType
			, c3d::ByteArray & result )
		{
			auto path = impData.uri.fspath();
			std::ifstream stream{ path, std::ios::binary };
			stream.seekg( 0u, std::ios::end );
			auto fileSize = std::min( size_t( stream.tellg() ), size );
			stream.seekg( std::streampos( int64_t( offset + impData.fileByteOffset ) ) );
			auto count = fileSize - impData.fileByteOffset;
			result.resize( count );
			stream.readsome( reinterpret_cast< char * >( result.data() ), std::streamsize( count ) );
			mimeType = impData.mimeType;
		}

		static void getData( [[maybe_unused]] fastgltf::Asset const & impAsset
			, fastgltf::sources::Array const & impData
			, size_t offset
			, size_t size
			, fastgltf::MimeType & mimeType
			, c3d::ByteArray & result )
		{
			size = std::min( size, impData.bytes.size() );
			result.resize( size );
			std::memcpy( result.data(), impData.bytes.data() + offset, size );
			mimeType = impData.mimeType;
		}

		static void getData( [[maybe_unused]] fastgltf::Asset const & impAsset
			, fastgltf::sources::Vector const & impData
			, size_t offset
			, size_t size
			, fastgltf::MimeType & mimeType
			, c3d::ByteArray & result )
		{
			size = std::min( size, impData.bytes.size() );
			result.resize( size );
			std::memcpy( result.data(), impData.bytes.data() + offset, size );
			mimeType = impData.mimeType;
		}

		static void getData( [[maybe_unused]] fastgltf::Asset const & impAsset
			, fastgltf::sources::ByteView const & impData
			, size_t offset
			, size_t size
			, fastgltf::MimeType & mimeType
			, c3d::ByteArray & result )
		{
			size = std::min( size, impData.bytes.size() );
			result.resize( size );
			std::memcpy( result.data(), impData.bytes.data() + offset, size );
			mimeType = impData.mimeType;
		}

		template< class ... ArgsT >
		struct OverloadedGetDataT : ArgsT... { using ArgsT::operator()...; };
		// explicit deduction guide (not needed as of C++20)
		template< class ... ArgsT >
		OverloadedGetDataT( ArgsT... ) -> OverloadedGetDataT< ArgsT... >;

		static c3d::String makeTextureName( size_t textureIndex
			, fastgltf::Texture const & impTexture
			, size_t imageIndex
			, fastgltf::Image const & impImage )
		{
			auto texName = c3d::makeString( impTexture.name );
			auto imgName = c3d::makeString( impImage.name );
			c3d::String uri;

			std::visit( OverloadedGetDataT{ []( auto const & ){}
				, [&uri]( fastgltf::sources::URI const & source ){ uri = c3d::string::toString( source.uri.fspath() ); }
				, []( [[maybe_unused]] fastgltf::sources::Array const & source ){}
				, []( [[maybe_unused]] fastgltf::sources::Vector const & source ){}
				, []( [[maybe_unused]] fastgltf::sources::ByteView const & source ){} }
				, impImage.data );

			if ( uri.empty() )
			{
				if ( texName.empty() )
				{
					texName = c3d::string::toString( textureIndex );
				}

				if ( imgName.empty() )
				{
					imgName = c3d::string::toString( imageIndex );
				}

				return cuT( "Image_" ) + texName + cuT( "_" ) + imgName;
			}

			auto result = uri;
			c3d::string::replace( result, cuT( "\\" ), cuT( "/" ) );
			return result;
		}

		static c3d::SamplerRPtr loadSampler( GltfImporterFile const & file
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< size_t > const & samplerIndex )
		{
			auto & engine = *file.getOwner();

			if ( samplerIndex
				&& *samplerIndex < impAsset.samplers.size() )
			{
				fastgltf::Sampler const & impSampler = impAsset.samplers[*samplerIndex];
				auto defaultSampler = engine.getDefaultSampler();
				auto & cache = engine.getSamplerCache();
				auto compareOp = defaultSampler->getCompareOp();
				auto minFilter = impSampler.minFilter ? convert( *impSampler.minFilter ) : defaultSampler->getMinFilter();
				auto magFilter = impSampler.magFilter ? convert( *impSampler.magFilter ) : defaultSampler->getMagFilter();
				auto mipFilter = impSampler.minFilter ? getMipFilter( *impSampler.minFilter ) : defaultSampler->getMipFilter();
				auto wrapS = convert( impSampler.wrapS );
				auto wrapT = convert( impSampler.wrapT );
				auto wrapR = defaultSampler->getWrapR();
				auto borderColour = defaultSampler->getBorderColour();
				auto name = c3d::getSamplerName( compareOp
					, minFilter, magFilter, mipFilter
					, wrapS, wrapT, wrapR
					, borderColour );

				if ( !cache.has( name ) )
				{
					auto sampler = engine.createSampler( name, engine );
					sampler->setMinFilter( minFilter );
					sampler->setMagFilter( magFilter );
					sampler->setMipFilter( mipFilter );
					sampler->setWrapS( wrapS );
					sampler->setWrapT( wrapT );
					sampler->setWrapR( wrapR );
					sampler->setBorderColour( borderColour );
					cache.add( name, sampler, false );
				}

				return cache.find( name );
			}

			return engine.getDefaultSampler();
		}

		static c3d::RawUniquePtr< c3d::TextureSourceInfo > loadTexture( fastgltf::Asset const & impAsset
			, c3d::String const & name
			, fastgltf::Texture const & impTexture
			, fastgltf::Image const & impImage
			, fastgltf::DataSource const & impDataSource
			, c3d::TextureConfiguration const & texConfig
			, c3d::ImageLoaderConfig const & loadConfig
			, fastgltf::MimeType defaultMimeType
			, c3d::MaterialImporter const & importer
			, size_t offset = 0u
			, size_t size = 0xFFFFFFFFFFFFFFFF )
		{
			if ( impDataSource.index() == 1u )
			{
				fastgltf::BufferView const & impBufferView = impAsset.bufferViews[std::get< 1 >( impDataSource ).bufferViewIndex];
				fastgltf::Buffer const & impBuffer = impAsset.buffers[impBufferView.bufferIndex];
				return loadTexture( impAsset, name, impTexture, impImage, impBuffer.data, texConfig, loadConfig, defaultMimeType, importer, offset + impBufferView.byteOffset, impBufferView.byteLength );
			}

			fastgltf::MimeType mimeType{};
			c3d::ByteArray data;

			std::visit( OverloadedGetDataT{ []( auto const & ){}
				, [&mimeType, &data, &impAsset, &offset, &size]( fastgltf::sources::URI const & source ){ getData( impAsset, source, offset, size, mimeType, data ); }
				, [&mimeType, &data, &impAsset, &offset, &size]( fastgltf::sources::Array const & source ){ getData( impAsset, source, offset, size, mimeType, data ); }
				, [&mimeType, &data, &impAsset, &offset, &size]( fastgltf::sources::Vector const & source ){ getData( impAsset, source, offset, size, mimeType, data ); }
				, [&mimeType, &data, &impAsset, &offset, &size]( fastgltf::sources::ByteView const & source ){ getData( impAsset, source, offset, size, mimeType, data ); } }
				, impDataSource );

			if ( !data.empty() )
			{
				return c3d::makeRawUnique< c3d::TextureSourceInfo >( importer.loadTexture( name
					, getFormatName( mimeType, defaultMimeType )
					, c3d::move( data )
					, texConfig
					, loadConfig ) );
			}

			return nullptr;
		}

		static c3d::RawUniquePtr< c3d::TextureSourceInfo > loadTexture( fastgltf::Asset const & impAsset
			, fastgltf::TextureInfo const & texInfo
			, c3d::TextureConfiguration const & texConfig
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			c3d::RawUniquePtr< c3d::TextureSourceInfo > result;

			if ( texInfo.textureIndex < impAsset.textures.size() )
			{
				if ( impAsset.textures[texInfo.textureIndex].imageIndex
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
						, loadConfig
						, fastgltf::MimeType::None
						, importer );
				}
				else if ( impAsset.textures[texInfo.textureIndex].webpImageIndex
					&& *impAsset.textures[texInfo.textureIndex].webpImageIndex < impAsset.images.size() )
				{
					fastgltf::Texture const & impTexture = impAsset.textures[texInfo.textureIndex];
					fastgltf::Image const & impImage = impAsset.images[*impTexture.webpImageIndex];
					result = loadTexture( impAsset
						, makeTextureName( texInfo.textureIndex, impTexture, *impTexture.webpImageIndex, impImage )
						, impTexture
						, impImage
						, impImage.data
						, texConfig
						, loadConfig
						, fastgltf::MimeType::WEBP
						, importer );
				}
				else if ( impAsset.textures[texInfo.textureIndex].ddsImageIndex
					&& *impAsset.textures[texInfo.textureIndex].ddsImageIndex < impAsset.images.size() )
				{
					fastgltf::Texture const & impTexture = impAsset.textures[texInfo.textureIndex];
					fastgltf::Image const & impImage = impAsset.images[*impTexture.ddsImageIndex];
					result = loadTexture( impAsset
						, makeTextureName( texInfo.textureIndex, impTexture, *impTexture.ddsImageIndex, impImage )
						, impTexture
						, impImage
						, impImage.data
						, texConfig
						, loadConfig
						, fastgltf::MimeType::DDS
						, importer );
				}
				else if ( impAsset.textures[texInfo.textureIndex].basisuImageIndex
					&& *impAsset.textures[texInfo.textureIndex].basisuImageIndex < impAsset.images.size() )
				{
					fastgltf::Texture const & impTexture = impAsset.textures[texInfo.textureIndex];
					fastgltf::Image const & impImage = impAsset.images[*impTexture.basisuImageIndex];
					result = loadTexture( impAsset
						, makeTextureName( texInfo.textureIndex, impTexture, *impTexture.basisuImageIndex, impImage )
						, impTexture
						, impImage
						, impImage.data
						, texConfig
						, loadConfig
						, fastgltf::MimeType::KTX2
						, importer );
				}
			}

			return result;
		}

		static c3d::Image const & loadImage( c3d::TextureSourceInfo const & source
			, c3d::MaterialImporter const & importer )
		{
			c3d::Image const * result{};
			auto loadConfig = source.loadConfig();
			if ( loadConfig.allowCompression )
				loadConfig.keepAlphaChannel = true;

			if ( source.isBufferImage() )
				result = importer.loadImage( source.name()
					, c3d::ImageCreateParams{ source.type(), source.buffer(), loadConfig } );
			else if ( source.isFileImage() )
				result = importer.loadImage( source.name()
					, c3d::ImageCreateParams{ source.folder() / source.relative(), loadConfig } );

			if ( !result )
				CU_LoaderError( "Couldn't load image" + c3d::toUtf8( source.name() ) + "." );
			return *result;
		}

		static void parseTransform( fastgltf::TextureTransform const & transform
			, c3d::TextureTransform & result
			, uint32_t & texCoordIndex )
		{
			result.scale = { transform.uvScale[0], transform.uvScale[1], 1.0f };
			result.rotate = c3d::Angle::fromRadians( -transform.rotation );// must be negated

			// A change of coordinates is required to map glTF UV transformations into the space used by
			// Castor3D. In glTF all UV origins are at 0,1 (top left of texture) in Castor3D space. In Castor3D
			// rotation occurs around the image center (0.5,0.5) where as in glTF rotation is around the
			// texture origin. All three can be corrected for solely by a change of the translation since
			// the transformations available are shape preserving. Note that importer already flips the V
			// coordinate of the actual meshes during import.
			float const rcos( ( -result.rotate ).cos() );
			float const rsin( ( -result.rotate ).sin() );
			result.translate->x = ( 0.5f * result.scale->x ) * ( -rcos + rsin + 1 ) + transform.uvOffset[0];
			result.translate->y = ( 0.5f * result.scale->y ) * (  rsin + rcos - 1 ) + 1 - result.scale->y - transform.uvOffset[1];

			if ( transform.texCoordIndex )
			{
				texCoordIndex = uint32_t( *transform.texCoordIndex );
			}
		}

		static void parseColOpaTexture( GltfImporterFile const & file
			, c3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< fastgltf::TextureInfo > const & texInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, bool checkAlpha )
		{
			if ( texInfo )
			{
				try
				{
					auto texConfig = pass.getComponentPlugin< c3d::ColourMapComponent >().getBaseTextureConfiguration();

					if ( auto sourceInfo = loadTexture( impAsset, *texInfo, texConfig, loadConfig, importer ) )
					{
						if ( checkAlpha )
						{
							if ( auto & image = loadImage( *sourceInfo, importer );
								hasAlphaChannel( image ) )
							{
								addFlagConfiguration( texConfig, { pass.getComponentPlugin< c3d::OpacityMapComponent >().getTextureFlags(), 0xFF000000 } );
								*sourceInfo = c3d::TextureSourceInfo{ *sourceInfo, texConfig };
							}
						}

						fastgltf::Texture const & impTexture = impAsset.textures[texInfo->textureIndex];
						auto texCoordIndex = uint32_t( texInfo->texCoordIndex );

						if ( texInfo->transform )
						{
							parseTransform( *texInfo->transform, texConfig.transform, texCoordIndex );
							*sourceInfo = c3d::TextureSourceInfo{ *sourceInfo, texConfig };
						}

						c3d::PassTextureConfig passTexConfig{ loadSampler( file, impAsset, impTexture.samplerIndex ), texCoordIndex };
						pass.registerTexture( c3d::move( *sourceInfo ), passTexConfig );
					}
				}
				catch ( c3d::Exception & exc )
				{
					c3d::log::error << exc.what() << std::endl;
				}
			}
		}

		static void parseTexture( GltfImporterFile const & file
			, c3d::Pass & pass
			, c3d::TextureConfiguration texConfig
			, fastgltf::Asset const & impAsset
			, fastgltf::TextureInfo const & texInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			try
			{
				if ( auto sourceInfo = loadTexture( impAsset, texInfo, texConfig, loadConfig, importer ) )
				{
					fastgltf::Texture const & impTexture = impAsset.textures[texInfo.textureIndex];
					auto texCoordIndex = uint32_t( texInfo.texCoordIndex );

					if ( texInfo.transform )
					{
						parseTransform( *texInfo.transform, texConfig.transform, texCoordIndex );
						*sourceInfo = c3d::TextureSourceInfo{ *sourceInfo, texConfig };
					}

					c3d::PassTextureConfig passTexConfig{ loadSampler( file, impAsset, impTexture.samplerIndex ), texCoordIndex };
					pass.registerTexture( c3d::move( *sourceInfo ), passTexConfig );
				}
			}
			catch ( c3d::Exception & exc )
			{
				c3d::log::error << exc.what() << std::endl;
			}
		}

		static void parseRghMetTexture( GltfImporterFile const & file
			, c3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< fastgltf::TextureInfo > const & texInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			if ( texInfo )
			{
				auto texConfig = pass.getComponentPlugin< c3d::RoughnessMapComponent >().getBaseTextureConfiguration();
				texConfig.components[1] = pass.getComponentPlugin< c3d::MetalnessMapComponent >().getBaseTextureConfiguration().components[0];
				texConfig.components[0].componentsMask = 0x0000FF00;
				texConfig.components[1].componentsMask = 0x000000FF;
				parseTexture( file, pass
					, std::move( texConfig )
					, impAsset, *texInfo, loadConfig, importer );
			}
		}

		static void parseSpcGlsTexture( GltfImporterFile const & file
			, c3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< fastgltf::TextureInfo > const & texInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			if ( texInfo )
			{
				auto texConfig = pass.getComponentPlugin< c3d::SpecularMapComponent >().getBaseTextureConfiguration();
				texConfig.components[1] = pass.getComponentPlugin< c3d::RoughnessMapComponent >().getBaseTextureConfiguration().components[0];
				texConfig.components[0].componentsMask = 0x00FFFFFF;
				texConfig.components[1].componentsMask = 0xFF000000;
				parseTexture( file, pass
					, std::move( texConfig )
					, impAsset, *texInfo, loadConfig, importer );

				if ( auto component = pass.getComponent< c3d::RoughnessComponent >() )
				{
					component->setGlossiness( true );
				}
			}
		}

		static void parseAnisStrDirTexture( GltfImporterFile const & file
			, c3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< fastgltf::TextureInfo > const & texInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			if ( texInfo )
			{
				auto texConfig = pass.getComponentPlugin< anisotropy::AnisotropyDirectionMapComponent>().getBaseTextureConfiguration();
				texConfig.components[1] = pass.getComponentPlugin< anisotropy::AnisotropyStrengthMapComponent >().getBaseTextureConfiguration().components[0];
				texConfig.components[0].componentsMask = 0x00FFFF00;
				texConfig.components[1].componentsMask = 0x000000FF;
				parseTexture( file, pass
					, std::move( texConfig )
					, impAsset, *texInfo, loadConfig, importer );
			}
		}

		template< typename ComponentT >
		static void parseTexture( GltfImporterFile const & file
			, c3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< fastgltf::TextureInfo > const & texInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			if ( texInfo )
			{
				parseTexture( file, pass
					, pass.getComponentPlugin< ComponentT >().getBaseTextureConfiguration()
					, impAsset, *texInfo, loadConfig, importer );
			}
		}

		template< typename ComponentT >
		static void parseNmlTexture( GltfImporterFile const & file
			, c3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< fastgltf::NormalTextureInfo > const & texInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			if ( texInfo )
			{
				parseTexture( file, pass
					, pass.getComponentPlugin< ComponentT >().getBaseTextureConfiguration()
					, impAsset, *texInfo, loadConfig, importer );
			}
		}

		static void parseOccTexture( GltfImporterFile const & file
			, c3d::Pass & pass
			, fastgltf::Asset const & impAsset
			, fastgltf::Optional< fastgltf::OcclusionTextureInfo > const & texInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			if ( texInfo )
			{
				parseTexture( file, pass
					, pass.getComponentPlugin< c3d::OcclusionMapComponent >().getBaseTextureConfiguration()
					, impAsset, *texInfo, loadConfig, importer );
			}
		}
	}

	//*********************************************************************************************

	c3d::WrapMode convert( fastgltf::Wrap const & v )
	{
		switch ( v )
		{
		case fastgltf::Wrap::Repeat:
			return c3d::WrapMode::eRepeat;
		case fastgltf::Wrap::ClampToEdge:
			return c3d::WrapMode::eClampToEdge;
		case fastgltf::Wrap::MirroredRepeat:
			return c3d::WrapMode::eMirroredRepeat;
		default:
			return c3d::WrapMode::eRepeat;
		}
	}

	c3d::FilterMode convert( fastgltf::Filter const & v )
	{
		switch ( v )
		{
		case fastgltf::Filter::Nearest:
		case fastgltf::Filter::NearestMipMapNearest:
		case fastgltf::Filter::NearestMipMapLinear:
			return c3d::FilterMode::eNearest;
		default:
			return c3d::FilterMode::eLinear;
		}
	}

	c3d::MipmapMode getMipFilter( fastgltf::Filter const & v )
	{
		switch ( v )
		{
		case fastgltf::Filter::Nearest:
		case fastgltf::Filter::NearestMipMapNearest:
		case fastgltf::Filter::LinearMipMapNearest:
			return c3d::MipmapMode::eNearest;
		default:
			return c3d::MipmapMode::eLinear;
		}
	}

	GltfMaterialImporter::GltfMaterialImporter( c3d::Engine & engine )
		: GltfMaterialImporter{ engine, nullptr }
	{
	}

	GltfMaterialImporter::GltfMaterialImporter( c3d::Engine & engine
		, GltfImporterFile * file )
		: c3d::MaterialImporter{ engine, cuT( "Gltf" ), file }
	{
		if ( !engine.hasMaterial( DefaultMaterial ) )
		{
			auto defaultMaterial = engine.createMaterial( DefaultMaterial
				, engine
				, materials::getLightingModel( engine, false ) );
			defaultMaterial->createPass();
			defaultMaterial->setSerialisable( false );
			engine.addMaterial( DefaultMaterial, defaultMaterial, true );
		}
	}

	bool GltfMaterialImporter::importMaterial( c3d::Material & material )
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
			, [&file, &name, &index]( fastgltf::Material const & )
			{
				auto result = ( name == file.getMaterialName( index ) );
				++index;
				return result;
			} );

		if ( it == impAsset.materials.end() )
		{
			return false;
		}

		fastgltf::Material const & impMaterial = *it;
		material.setLightingModelId( materials::getLightingModel( *getEngine()
			, impMaterial.anisotropy != nullptr ) );
		auto pass = material.createPass();

		if ( impMaterial.unlit )
		{
			pass->enableLighting( false );
		}

		materials::parseComponentData< c3d::TwoSidedComponent >( *pass, impMaterial.doubleSided );
		materials::parseNmlTexture< c3d::NormalMapComponent >( file, *pass, impAsset, impMaterial.normalTexture, m_loadConfig, *this );
		materials::parseOccTexture( file, *pass, impAsset, impMaterial.occlusionTexture, m_loadConfig, *this );
		doImportSpecularData( impMaterial, *pass );
		doImportIridescenceData( impMaterial, *pass );
		doImportVolumeData( impMaterial, *pass );
		doImportTransmissionData( impMaterial, *pass );
		doImportClearcoatData( impMaterial, *pass );
		doImportSheenData( impMaterial, *pass );
		doImportEmissiveData( impMaterial, *pass );
		doImportAlphaModeData( impMaterial, *pass );
		doImportIorData( impMaterial, *pass );
		doImportAnisotropyData( impMaterial, *pass );
		doImportDispersionData( impMaterial, *pass );
		doImportDiffuseTransmissionData( impMaterial, *pass );
		pass->prepareTextures();
		return true;
	}

	void GltfMaterialImporter::doImportSpecularData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		auto & file = static_cast< GltfImporterFile const & >( *m_file );
		auto & impAsset = file.getAsset();

		if ( impMaterial.specular || !impMaterial.specularGlossiness )
		{
			pass.createComponent< c3d::ColourComponent >()->setColour( c3d::HdrRgbColour::fromComponents( impMaterial.pbrData.baseColorFactor[0]
				, impMaterial.pbrData.baseColorFactor[1]
				, impMaterial.pbrData.baseColorFactor[2] ) );

			if ( impMaterial.alphaMode != fastgltf::AlphaMode::Opaque )
			{
				pass.createComponent< c3d::OpacityComponent >()->setOpacity( impMaterial.pbrData.baseColorFactor[3] );
			}

			pass.createComponent< c3d::MetalnessComponent >()->setMetalness( impMaterial.pbrData.metallicFactor );
			pass.createComponent< c3d::RoughnessComponent >()->setRoughness( impMaterial.pbrData.roughnessFactor );

			materials::parseColOpaTexture( file, pass, impAsset, impMaterial.pbrData.baseColorTexture, m_loadConfig, *this, impMaterial.alphaMode != fastgltf::AlphaMode::Opaque );
			materials::parseRghMetTexture( file, pass, impAsset, impMaterial.pbrData.metallicRoughnessTexture, m_loadConfig, *this );

			if ( impMaterial.specular )
			{
				auto spcComponent = pass.createComponent< c3d::SpecularComponent >();
				auto fctComponent = pass.createComponent< c3d::SpecularFactorComponent >();
				fctComponent->setFactor( impMaterial.specular->specularFactor );
				spcComponent->setSpecular( c3d::RgbColour::fromComponents( impMaterial.specular->specularColorFactor[0]
					, impMaterial.specular->specularColorFactor[1]
					, impMaterial.specular->specularColorFactor[2] ) );
				materials::parseTexture< c3d::SpecularMapComponent >( file, pass, impAsset, impMaterial.specular->specularColorTexture, m_loadConfig, *this );
				materials::parseTexture< c3d::SpecularFactorMapComponent >( file, pass, impAsset, impMaterial.specular->specularTexture, m_loadConfig, *this );
			}
		}
		else if ( impMaterial.specularGlossiness )
		{
			pass.createComponent< c3d::ColourComponent >()->setColour( c3d::HdrRgbColour::fromComponents( impMaterial.specularGlossiness->diffuseFactor[0]
				, impMaterial.specularGlossiness->diffuseFactor[1]
				, impMaterial.specularGlossiness->diffuseFactor[2] ) );

			if ( impMaterial.alphaMode != fastgltf::AlphaMode::Opaque )
			{
				pass.createComponent< c3d::OpacityComponent >()->setOpacity( impMaterial.specularGlossiness->diffuseFactor[3] );
			}

			auto spcComponent = pass.createComponent< c3d::SpecularComponent >();
			auto fctComponent = pass.createComponent< c3d::SpecularFactorComponent >();
			fctComponent->setFactor( 1.0f );
			spcComponent->setSpecular( c3d::RgbColour::fromComponents( impMaterial.specularGlossiness->specularFactor[0]
				, impMaterial.specularGlossiness->specularFactor[1]
				, impMaterial.specularGlossiness->specularFactor[2] ) );
			auto rghComponent = pass.createComponent< c3d::RoughnessComponent >();
			rghComponent->setRoughness( 1.0f - impMaterial.specularGlossiness->glossinessFactor );

			materials::parseColOpaTexture( file, pass, impAsset, impMaterial.specularGlossiness->diffuseTexture, m_loadConfig, *this, impMaterial.alphaMode != fastgltf::AlphaMode::Opaque );
			materials::parseSpcGlsTexture( file, pass, impAsset, impMaterial.specularGlossiness->specularGlossinessTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportIridescenceData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		if ( impMaterial.iridescence )
		{
			auto component = pass.createComponent< c3d::IridescenceComponent >();
			component->setFactor( impMaterial.iridescence->iridescenceFactor );
			component->setIor( impMaterial.iridescence->iridescenceIor );
			component->setMinThickness( impMaterial.iridescence->iridescenceThicknessMinimum );
			component->setMaxThickness( impMaterial.iridescence->iridescenceThicknessMaximum );

			auto & file = static_cast< GltfImporterFile const & >( *m_file );
			auto & impAsset = file.getAsset();
			materials::parseTexture< c3d::IridescenceMapComponent >( file, pass, impAsset, impMaterial.iridescence->iridescenceTexture, m_loadConfig, *this );
			materials::parseTexture< c3d::IridescenceThicknessMapComponent >( file, pass, impAsset, impMaterial.iridescence->iridescenceThicknessTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportVolumeData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		if ( impMaterial.volume )
		{
			auto & file = static_cast< GltfImporterFile const & >( *m_file );
			auto & impAsset = file.getAsset();

			auto attenuationComponent = pass.createComponent< c3d::AttenuationComponent >();
			attenuationComponent->setAttenuationColour( c3d::RgbColour::fromComponents( impMaterial.volume->attenuationColor[0]
				, impMaterial.volume->attenuationColor[1]
				, impMaterial.volume->attenuationColor[2] ) );
			attenuationComponent->setAttenuationDistance( impMaterial.volume->attenuationDistance );

			auto thicknessComponent = pass.createComponent< c3d::ThicknessComponent >();
			thicknessComponent->setThicknessFactor( impMaterial.volume->thicknessFactor );
			materials::parseTexture< c3d::ThicknessMapComponent >( file, pass, impAsset, impMaterial.volume->thicknessTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportTransmissionData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		if ( impMaterial.transmission )
		{
			auto & file = static_cast< GltfImporterFile const & >( *m_file );
			auto & impAsset = file.getAsset();

			pass.createComponent< c3d::TransmissionComponent >()->setTransmission( impMaterial.transmission->transmissionFactor );
			materials::parseTexture< c3d::TransmissionMapComponent >( file, pass, impAsset, impMaterial.transmission->transmissionTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportClearcoatData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		if ( impMaterial.clearcoat )
		{
			auto component = pass.createComponent< c3d::ClearcoatComponent >();
			component->setClearcoatFactor( impMaterial.clearcoat->clearcoatFactor );
			component->setRoughnessFactor( impMaterial.clearcoat->clearcoatRoughnessFactor );

			auto & file = static_cast< GltfImporterFile const & >( *m_file );
			auto & impAsset = file.getAsset();
			materials::parseTexture< c3d::ClearcoatMapComponent >( file, pass, impAsset, impMaterial.clearcoat->clearcoatTexture, m_loadConfig, *this );
			materials::parseNmlTexture< c3d::ClearcoatNormalMapComponent >( file, pass, impAsset, impMaterial.clearcoat->clearcoatNormalTexture, m_loadConfig, *this );
			materials::parseTexture< c3d::ClearcoatRoughnessMapComponent >( file, pass, impAsset, impMaterial.clearcoat->clearcoatRoughnessTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportSheenData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		if ( impMaterial.sheen )
		{
			auto component = pass.createComponent< c3d::SheenComponent >();
			component->setSheenColour( c3d::HdrRgbColour::fromComponents( impMaterial.sheen->sheenColorFactor[0]
				, impMaterial.sheen->sheenColorFactor[1]
				, impMaterial.sheen->sheenColorFactor[2] ) );
			component->setRoughnessFactor( impMaterial.sheen->sheenRoughnessFactor );

			auto & file = static_cast< GltfImporterFile const & >( *m_file );
			auto & impAsset = file.getAsset();
			materials::parseTexture< c3d::SheenMapComponent >( file, pass, impAsset, impMaterial.sheen->sheenColorTexture, m_loadConfig, *this );
			materials::parseTexture< c3d::SheenRoughnessMapComponent >( file, pass, impAsset, impMaterial.sheen->sheenRoughnessTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportEmissiveData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		if ( impMaterial.emissiveStrength != 1.0f
			|| impMaterial.emissiveTexture
			|| std::any_of( impMaterial.emissiveFactor.data()
				, impMaterial.emissiveFactor.data() + 3u
				, []( float const lookup )
				{
					return lookup != 0.0f;
				} ) )
		{
			auto component = pass.createComponent< c3d::EmissiveComponent >();

			if ( impMaterial.emissiveStrength != 0.0f )
			{
				component->setEmissiveFactor( impMaterial.emissiveStrength * m_emissiveMult );
			}

			component->setEmissive( c3d::RgbColour::fromComponents( impMaterial.emissiveFactor[0]
				, impMaterial.emissiveFactor[1]
				, impMaterial.emissiveFactor[2] ) );

			auto & file = static_cast< GltfImporterFile const & >( *m_file );
			auto & impAsset = file.getAsset();
			materials::parseTexture< c3d::EmissiveMapComponent >( file, pass, impAsset, impMaterial.emissiveTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportAnisotropyData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		if ( impMaterial.anisotropy )
		{
			auto anisotropy = pass.createComponent< anisotropy::AnisotropyComponent >();
			anisotropy->setStrength( impMaterial.anisotropy->anisotropyStrength );
			anisotropy->setRotation( c3d::Angle::fromRadians( impMaterial.anisotropy->anisotropyRotation ) );

			auto & file = static_cast< GltfImporterFile const & >( *m_file );
			auto & impAsset = file.getAsset();
			materials::parseAnisStrDirTexture( file, pass, impAsset, impMaterial.anisotropy->anisotropyTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportDiffuseTransmissionData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )
	{
		if ( impMaterial.diffuseTransmission )
		{
			pass.createComponent< c3d::DiffuseTransmissionComponent >()->setTransmissionFactor( impMaterial.diffuseTransmission->transmissionFactor );
			pass.createComponent< c3d::DiffuseTransmissionComponent >()->setTransmissionColour( c3d::RgbColour::fromComponents( impMaterial.diffuseTransmission->transmissionColorFactor[0]
				, impMaterial.diffuseTransmission->transmissionColorFactor[1]
				, impMaterial.diffuseTransmission->transmissionColorFactor[2] ) );

			auto & file = static_cast< GltfImporterFile const & >( *m_file );
			auto & impAsset = file.getAsset();
			materials::parseTexture< c3d::DiffuseTransmissionFactorMapComponent >( file, pass, impAsset, impMaterial.diffuseTransmission->transmissionTexture, m_loadConfig, *this );
			materials::parseTexture< c3d::DiffuseTransmissionColourMapComponent >( file, pass, impAsset, impMaterial.diffuseTransmission->transmissionColorTexture, m_loadConfig, *this );
		}
	}

	void GltfMaterialImporter::doImportAlphaModeData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )const
	{
		if ( impMaterial.alphaMode == fastgltf::AlphaMode::Mask )
		{
			pass.createComponent< c3d::OpacityComponent >();
			auto alphaTest = pass.createComponent< c3d::AlphaTestComponent >();
			alphaTest->setAlphaRefValue( impMaterial.alphaCutoff );
			alphaTest->setAlphaFunc( c3d::ComparisonFunc::eGreater );
			alphaTest->setBlendAlphaFunc( c3d::ComparisonFunc::eLessOrEqual );
		}
		else if ( impMaterial.alphaMode == fastgltf::AlphaMode::Blend )
		{
			pass.createComponent< c3d::OpacityComponent >();

			auto twoSided = pass.createComponent< c3d::TwoSidedComponent >();
			twoSided->setTwoSided( true );

			if ( !pass.hasComponent< c3d::AlphaTestComponent >() )
			{
				auto alphaTest = pass.createComponent< c3d::AlphaTestComponent >();
				alphaTest->setAlphaRefValue( 0.95f );
				alphaTest->setAlphaFunc( c3d::ComparisonFunc::eGreater );
				alphaTest->setBlendAlphaFunc( c3d::ComparisonFunc::eLessOrEqual );
			}

			auto blend = pass.createComponent< c3d::BlendComponent >();
			blend->setAlphaBlendMode( c3d::BlendMode::eInterpolative );
		}
	}

	void GltfMaterialImporter::doImportIorData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )const
	{
		if ( impMaterial.ior )
		{
			pass.createComponent< c3d::RefractionComponent >()->setRefractionRatio( *impMaterial.ior );
		}
	}

	void GltfMaterialImporter::doImportDispersionData( fastgltf::Material const & impMaterial
		, c3d::Pass & pass )const
	{
		if ( impMaterial.dispersion != 0.0f )
		{
			auto dispersion = pass.createComponent< c3d::DispersionComponent >();
			dispersion->setDispersion( impMaterial.dispersion );
		}
	}

	//*********************************************************************************************
}
