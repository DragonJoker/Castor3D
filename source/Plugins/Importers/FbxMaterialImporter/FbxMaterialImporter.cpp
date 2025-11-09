#include "FbxMaterialImporter/FbxMaterialImporter.hpp"
#include "FbxMaterialImporter/FbxMaterialsFile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Limits.hpp>
#include <Castor3D/ImporterFile.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Base/BlendComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/AmbientComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/AmbientColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/AmbientFactorMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OcclusionMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SpecularFactorMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ColourComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/HeightComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Shader/LightingModelFactory.hpp>

// Materials
#include <EdgesComponent.hpp>
#include <Shaders/GlslToonLighting.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <fbxsdk.h>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d_fbx
{
	//*********************************************************************************************

	namespace materials
	{
		static bool getData( [[maybe_unused]] c3d::MaterialImporter const & importer
			, [[maybe_unused]] fbx::FbxLayeredTexture const & texture
			, [[maybe_unused]] c3d::ByteArray & result
			, [[maybe_unused]] c3d::String & formatName )
		{
			return false;
		}

		static bool getData( [[maybe_unused]] c3d::MaterialImporter const & importer
			, [[maybe_unused]] fbx::FbxProceduralTexture const & texture
			, [[maybe_unused]] c3d::ByteArray & result
			, [[maybe_unused]] c3d::String & formatName )
		{
			return false;
		}

		static c3d::RawUniquePtr< c3d::TextureSourceInfo > loadTexture( c3d::MaterialImporter const & importer
			, fbx::FbxProperty const & property
			, c3d::TextureConfiguration const & texConfig
			, c3d::ImageLoaderConfig const & loadConfig
			, fbx::FbxTexture const *& fbxTexture )
		{
			int index = 0;
			fbx::FbxObject * object = property.GetSrcObject( index );
			++index;
			c3d::ByteArray data;
			c3d::String formatName;

			while ( object && data.empty() )
			{
				if ( object->GetClassId() == fbx::FbxFileTexture::ClassId )
				{
					auto fbxFileTexture = static_cast< fbx::FbxFileTexture * >( object );
					fbxTexture = fbxFileTexture;
					c3d::log::debug << "    Texture: " << fbxTexture->GetName() << "\n";
					return c3d::makeRawUnique< c3d::TextureSourceInfo >( importer.loadTexture( c3d::Path{ fbxFileTexture->GetRelativeFileName() }
						, texConfig, loadConfig ) );
				}

				if ( object->GetClassId() == fbx::FbxLayeredTexture::ClassId )
				{
					fbxTexture = static_cast< fbx::FbxTexture const * >( object );
					getData( importer, *static_cast< fbx::FbxLayeredTexture * >( object ), data, formatName );
				}
				else if ( object->GetClassId() == fbx::FbxProceduralTexture::ClassId )
				{
					fbxTexture = static_cast< fbx::FbxTexture const * >( object );
					getData( importer, *static_cast< fbx::FbxProceduralTexture * >( object ), data, formatName );
				}

				object = property.GetSrcObject( index );
				++index;
			}

			if ( !data.empty() && fbxTexture )
			{
				auto name = c3d::makeString( fbxTexture->GetName() );
				c3d::log::debug << "    Texture: " << name << "\n";
				return c3d::makeRawUnique< c3d::TextureSourceInfo >( importer.loadTexture( name
					, formatName, c3d::move( data )
					, texConfig, loadConfig ) );
			}

			return nullptr;
		}

		static c3d::SamplerRPtr loadSampler( c3d::ImporterFile const & file
			, fbx::FbxTexture const * fbxTexture )
		{
			static const c3d::Array< c3d::WrapMode, 2u > mode =
			{
				c3d::WrapMode::eRepeat,
				c3d::WrapMode::eClampToBorder,
			};

			auto & engine = *file.getOwner();
			auto defaultSampler = engine.getDefaultSampler();
			auto & cache = engine.getSamplerCache();
			auto compareOp = c3d::ComparisonFunc::eNever;
			auto minFilter = c3d::FilterMode::eLinear;
			auto magFilter = c3d::FilterMode::eLinear;
			auto mipFilter = c3d::MipmapMode::eLinear;
			auto wrapS = mode[fbxTexture->WrapModeU.Get()];
			auto wrapT = mode[fbxTexture->WrapModeV.Get()];
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

		static void parseTransform( fbx::FbxTexture const * fbxTexture
			, c3d::TextureTransform & result )
		{
			result.scale = { fbxTexture->GetScaleU(), fbxTexture->GetScaleV(), 1.0f };
			result.rotate = c3d::Angle::fromDegrees( -fbxTexture->GetRotationW() );// must be negated

			// A change of coordinates is required to map glTF UV transformations into the space used by Castor3D.
			// In glTF all UV origins are at 0,1 (top left of texture) in Castor3D space.
			// In Castor3D rotation occurs around the image center (0.5,0.5) where as in glTF rotation is around the
			// texture origin. All three can be corrected for solely by a change of the translation since
			// the transformations available are shape preserving.
			float const rcos( ( -result.rotate ).cos() );
			float const rsin( ( -result.rotate ).sin() );
			result.translate->x = ( 0.5f * result.scale->x ) * ( -rcos + rsin + 1 ) + float( fbxTexture->GetTranslationU() );
			result.translate->y = ( 0.5f * result.scale->y ) * ( rsin + rcos - 1 ) + 1 - result.scale->y - float( fbxTexture->GetTranslationV() );
		}

		static bool parseTexture( c3d::ImporterFile const & file
			, c3d::Pass & pass
			, c3d::TextureConfiguration texConfig
			, fbx::FbxProperty const & property
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			bool result{};
			try
			{
				fbx::FbxTexture const * fbxTexture{};
				if ( auto sourceInfo = loadTexture( importer, property, texConfig, loadConfig, fbxTexture ) )
				{
					result = true;
					parseTransform( fbxTexture, texConfig.transform );
					*sourceInfo = c3d::TextureSourceInfo{ *sourceInfo, texConfig };

					c3d::PassTextureConfig passTexConfig{ loadSampler( file, fbxTexture ), 0u };
					pass.registerTexture( c3d::move( *sourceInfo ), passTexConfig );
				}
			}
			catch ( c3d::Exception & exc )
			{
				c3d::log::error << exc.what() << std::endl;
			}
			return result;
		}

		c3d::TextureConfiguration getRemap( c3d::PassComponentTextureFlag flag
			, c3d::TextureConfiguration texConfig
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps )
		{
			auto it = textureRemaps.find( flag );
			if ( it == textureRemaps.end() )
				return texConfig;
			return it->second;
		}

		template< typename ComponentT >
		static bool parseTexture( c3d::ImporterFile const & file
			, c3d::Pass & pass
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, fbx::FbxProperty const & property
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			bool result{};

			if ( property.IsValid() )
			{
				auto & plugin = pass.getComponentPlugin< ComponentT >();
				auto texFlags = plugin.getTextureFlags();
				auto texConfig = plugin.getBaseTextureConfiguration();
				result = parseTexture( file, pass, getRemap( texFlags, texConfig, textureRemaps )
					, property, loadConfig, importer );
			}

			return result;
		}

		template< typename ComponentT, typename DataT >
		static bool parseTexture( c3d::ImporterFile const & file
			, c3d::Pass & pass
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::Vector< fbx::FbxPropertyT< DataT > > const & properties
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer )
		{
			bool result{};
			auto it = properties.begin();
			while ( it != properties.end() && !result )
			{
				result = parseTexture< ComponentT >( file, pass, textureRemaps, *it, loadConfig, importer );
				++it;
			}
			return result;
		}

		c3d::HdrRgbColour getHdrRgbColour( fbx::FbxDouble3 const & v )
		{
			return c3d::HdrRgbColour::fromComponents( v[0], v[1], v[2] );
		}

		c3d::RgbColour getRgbColour( fbx::FbxDouble3 const & v )
		{
			return c3d::RgbColour::fromComponents( v[0], v[1], v[2] );
		}

		template< typename DataT >
		bool isValid( fbx::FbxPropertyT< DataT > const & property
			, DataT const & invalidValue = {} )
		{
			return property.IsValid() && property.Get() != invalidValue;
		}

		template< typename DataT >
		bool anyValidNoData( c3d::Vector< fbx::FbxPropertyT< DataT> > properties
			, DataT invalidValue = {} )
		{
			return std::any_of( properties.begin(), properties.end()
				, [&invalidValue]( fbx::FbxPropertyT< DataT > const & lookup )
				{
					return lookup.IsValid();
				} );
		}

		template< typename DataT >
		bool anyValid( c3d::Vector< fbx::FbxPropertyT< DataT> > properties
			, DataT invalidValue = {} )
		{
			return std::any_of( properties.begin(), properties.end()
				, [&invalidValue]( fbx::FbxPropertyT< DataT > const & lookup )
				{
					return isValid( lookup, invalidValue );
				} );
		}

		template< typename DataT, typename FuncT >
		void applyToFirstValid( c3d::Vector< fbx::FbxPropertyT< DataT> > properties
			, FuncT func )
		{
			auto it = properties.begin();
			while ( it != properties.end() )
			{
				if ( it->IsValid() )
				{
					func( *it );
					break;
				}
				++it;
			}
		}

		template< typename DataT, typename FuncT >
		void applyToFirstValid( c3d::Vector< fbx::FbxPropertyT< DataT> > properties
			, DataT invalidData
			, FuncT func )
		{
			auto it = properties.begin();
			while ( it != properties.end() )
			{
				if ( isValid( *it, invalidData ) )
				{
					func( *it );
					break;
				}
				++it;
			}
		}

		static void importNormalsData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble3 > > property = { fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sNormalMap )
				, fbxMaterial.FindProperty( "Maya|NormalTexture" ) };
			parseTexture< c3d::NormalMapComponent >( file, pass, textureRemaps, property, loadConfig, importer );
		}

		static void importHeightData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{
			if ( fbx::FbxPropertyT< fbx::FbxDouble > map = fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sBump );
				map.IsValid() )
			{
				auto & plugin = pass.getComponentPlugin< c3d::HeightMapComponent >();
				auto texFlags = plugin.getTextureFlags();
				auto texConfig = plugin.getBaseTextureConfiguration();
				if ( fbx::FbxPropertyT< fbx::FbxDouble > factor = fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sBumpFactor );
					factor.IsValid() )
					texConfig.heightFactor = float( factor.Get() );
				if ( parseTexture( file, pass, getRemap( texFlags, texConfig, textureRemaps ), map, loadConfig, importer ) )
					pass.createComponent< c3d::HeightComponent >()->setParallaxOcclusion( c3d::ParallaxOcclusionMode::eRepeat );
			}
		}

		static void importAmbientData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{
			fbx::FbxPropertyT< fbx::FbxDouble > factor = fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sAmbientFactor );
			fbx::FbxPropertyT< fbx::FbxDouble3 > color = fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sAmbient );
			if ( isValid( factor ) )
				pass.createComponent< c3d::AmbientComponent >()->setAmbientFactor( float( factor.Get() ) );
			if ( isValid( color ) )
				pass.createComponent< c3d::AmbientComponent >()->setAmbient( getRgbColour( color.Get() ) );
			parseTexture< c3d::AmbientFactorMapComponent >( file, pass, textureRemaps, factor, loadConfig, importer );
			parseTexture< c3d::AmbientColourMapComponent >( file, pass, textureRemaps, color, loadConfig, importer );
		}

		static void importColourData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble3 > > color = { fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sDiffuse )
				, fbxMaterial.FindProperty( "Maya|base_color" )
				, fbxMaterial.FindProperty( "Maya|TEX_color_map" )
				, fbxMaterial.FindProperty( "3dsMax|Parameters|base_color_map" )
				, fbxMaterial.FindProperty( "3dsMax|main|base_color_map" ) };
			applyToFirstValid( color
				, [&pass]( fbx::FbxPropertyT< fbx::FbxDouble3 > prop )
				{
					pass.createComponent< c3d::ColourComponent >()->setColour( getHdrRgbColour( prop.Get() ) );
				} );
			parseTexture< c3d::ColourMapComponent >( file, pass, textureRemaps, color, loadConfig, importer );
		}

		static void importSpecularData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{
			fbx::FbxPropertyT< fbx::FbxDouble > factor = fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sSpecularFactor );
			fbx::FbxPropertyT< fbx::FbxDouble3 > color = { fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sSpecular ) };
			if ( isValid( factor, 1.0 ) )
				pass.createComponent< c3d::SpecularFactorComponent >()->setFactor( float( factor.Get() ) );
			if ( isValid( color ) )
				pass.createComponent< c3d::SpecularComponent >()->setSpecular( getRgbColour( color.Get() ) );
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble3 > > map = { fbxMaterial.FindProperty( "SpecularColor" )
				, fbxMaterial.FindProperty( "Maya|SpecularTexture" )
				, fbxMaterial.FindProperty( "3dsMax|main|specular_map" ) };
			parseTexture< c3d::SpecularFactorMapComponent >( file, pass, textureRemaps, factor, loadConfig, importer );
			parseTexture< c3d::SpecularMapComponent >( file, pass, textureRemaps, map, loadConfig, importer );
		}

		static void importShininessData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble > > shininessMap = { fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sShininess ) };
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble > > roughnessMap = { fbxMaterial.FindProperty( "Maya|TEX_roughness_map" )
				, fbxMaterial.FindProperty( "Maya|diffuseRoughness" )
				, fbxMaterial.FindProperty( "Maya|specularRoughness" )
				, fbxMaterial.FindProperty( "3dsMax|main|roughness_map" )
				, fbxMaterial.FindProperty( "3dsMax|Parameters|roughness_map" ) };
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble > > glossinessMap = { fbxMaterial.FindProperty( "3dsMax|main|glossiness_map" ) };

			if ( fbx::FbxPropertyT< fbx::FbxDouble > roughness = fbxMaterial.FindProperty( "Maya|roughness" );
				roughness.IsValid() )
				pass.createComponent< c3d::RoughnessComponent >()->setRoughness( float( roughness.Get() ) );
			else if ( fbx::FbxPropertyT< fbx::FbxDouble > shininess = fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sShininess );
				shininess.IsValid() )
				pass.createComponent< c3d::RoughnessComponent >()->setShininess( float( shininess.Get() ) );
			if ( !parseTexture< c3d::RoughnessMapComponent >( file, pass, textureRemaps, glossinessMap, loadConfig, importer )
				&& !parseTexture< c3d::RoughnessMapComponent >( file, pass, textureRemaps, roughnessMap, loadConfig, importer ) )
					parseTexture< c3d::RoughnessMapComponent >( file, pass, textureRemaps, shininessMap, loadConfig, importer );
		}

		static void importEmissiveData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass
			, float emissiveMult )
		{
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble3 > > emissive = { fbxMaterial.FindProperty( "EmissiveColor" )
				, fbxMaterial.FindProperty( "Maya|emissive" )
				, fbxMaterial.FindProperty( "Maya|emissionColor" )
				, fbxMaterial.FindProperty( "Maya|TEX_emissive_map" )
				, fbxMaterial.FindProperty( "3dsMax|Parameters|emission_map" )
				, fbxMaterial.FindProperty( "3dsMax|main|emit_color_map" ) };
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble > > factor = { fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sEmissiveFactor )
				, fbxMaterial.FindProperty( "Maya|emissive_intensity" ) };
			applyToFirstValid( factor
				, [&pass, emissiveMult]( fbx::FbxPropertyT< fbx::FbxDouble > prop )
				{
					pass.createComponent< c3d::EmissiveComponent >()->setEmissiveFactor( float( prop * emissiveMult ) );
				} );
			applyToFirstValid( emissive
				, [&pass]( fbx::FbxPropertyT< fbx::FbxDouble3 > prop )
				{
					pass.createComponent< c3d::EmissiveComponent >()->setEmissive( getRgbColour( prop.Get() ) );
				} );
			parseTexture< c3d::EmissiveMapComponent >( file, pass, textureRemaps, emissive, loadConfig, importer );
		}

		static void setMixedInterpolative( c3d::Pass & pass )
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

		static void importOpacityData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{
			fbx::FbxPropertyT< fbx::FbxDouble3 > color = fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sTransparentColor );
			fbx::FbxPropertyT< fbx::FbxDouble > transparency = fbxMaterial.FindProperty( fbx::FbxSurfaceMaterial::sTransparencyFactor );
			fbx::FbxPropertyT< fbx::FbxDouble > opacity = fbxMaterial.FindProperty( "Opacity" );
			float opacityValue{ 1.0f };

			if ( opacity.IsValid() && opacity.Get() != 1.0 && opacity.Get() != 0.0 )
				opacityValue = float( opacity.Get() );
			else if ( transparency.IsValid() && transparency.Get() != 1.0 && transparency.Get() != 0.0 )
				opacityValue = float( 1.0 - transparency.Get() );

			c3d::Point3f colorValue{};
			if ( color.IsValid() )
			{
				colorValue = { color.Get()[0], color.Get()[1], color.Get()[2] };
				if ( transparency.IsValid() )
					colorValue *= float( transparency.Get() );
				opacityValue = 1.0f - float( ( color.Get()[0] + color.Get()[1] + color.Get()[2] ) / 3.0f );
			}
			else if ( opacity.IsValid() )
			{
				opacityValue = float( opacity.Get() );
				pass.createComponent< c3d::OpacityComponent >()->setOpacity( opacityValue );
			}

			if ( opacityValue != 0.0f && opacityValue != 1.0f )
				setMixedInterpolative( pass );

			if ( parseTexture< c3d::OpacityMapComponent >( file, pass, textureRemaps, opacity, loadConfig, importer )
				|| parseTexture< c3d::OpacityMapComponent >( file, pass, textureRemaps, transparency, loadConfig, importer )
					|| parseTexture< c3d::OpacityMapComponent >( file, pass, textureRemaps, color, loadConfig, importer ) )
				setMixedInterpolative( pass );
		}

		static void importMetalnessData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{

			if ( fbx::FbxPropertyT< fbx::FbxDouble > factor = fbxMaterial.FindProperty( "Maya|metallic" );
				factor.IsValid() )
				pass.createComponent< c3d::MetalnessComponent >()->setMetalness( float( factor.Get() ) );
			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble3 > > map = { fbxMaterial.FindProperty( "ReflectionFactor" )
				, fbxMaterial.FindProperty( "Maya|metalness" )
				, fbxMaterial.FindProperty( "Maya|TEX_metallic_map" )
				, fbxMaterial.FindProperty( "3dsMax|Parameters|metalness_map" )
				, fbxMaterial.FindProperty( "3dsMax|main|metalness_map" ) };
			parseTexture< c3d::MetalnessMapComponent >( file, pass, textureRemaps, map, loadConfig, importer );
		}

		static void importAmbientOcclusionData( c3d::ImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, fbx::FbxSurfaceMaterial const & fbxMaterial
			, c3d::Pass & pass )
		{

			c3d::Vector< fbx::FbxPropertyT< fbx::FbxDouble3 > > map = { fbxMaterial.FindProperty( "Maya|TEX_ao_map" )
				, fbxMaterial.FindProperty( "3dsMax|main|ao_map" ) };
			parseTexture< c3d::OcclusionMapComponent >( file, pass, textureRemaps, map, loadConfig, importer );
		}
	}

	//*********************************************************************************************

	FbxMaterialImporter::FbxMaterialImporter( c3d::Engine & engine
		, FbxMaterialsFile const & materialsFile )
		: c3d::MaterialImporter{ engine, cuT( "Fbx" ) }
		, m_materialsFile{ materialsFile }
	{
	}

	bool FbxMaterialImporter::importMaterial( c3d::Material & material )
	{
		auto name = material.getName();
		auto it = m_materialsFile.getMaterials().find( name );
		if ( it == m_materialsFile.getMaterials().end() )
			return false;

		fbx::FbxSurfaceMaterial const * fbxMaterial = it->second.fbxMaterial;
		auto pass = material.createPass();
		c3d::log::debug << "Material: " << fbxMaterial->GetName() << "\n";
		c3d::String model = c3d::makeString( fbxMaterial->ShadingModel.Get().Buffer() );
		c3d::log::debug << "    ShadingModel: " << model << "\n";

		if ( fbxMaterial->GetClassId() == fbx::FbxSurfaceLambert::ClassId
			|| fbxMaterial->GetClassId() == fbx::FbxSurfacePhong::ClassId )
		{
			materials::importNormalsData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			materials::importHeightData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			materials::importAmbientData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			materials::importColourData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			materials::importSpecularData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			materials::importShininessData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			materials::importEmissiveData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass, m_emissiveMult );
			materials::importOpacityData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			materials::importMetalnessData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			materials::importAmbientOcclusionData( *m_file, m_textureRemaps, m_loadConfig, *this, *fbxMaterial, *pass );
			pass->prepareTextures();
		}

		auto const & engine = *getOwner();
		material.setLightingModelId( engine.getDefaultLightingModel() );
		return true;
	}
}

//*************************************************************************************************
