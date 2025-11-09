#include "TdsImporter/TdsMaterialImporter.hpp"
#include "TdsImporter/TdsImporterFile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Limits.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Base/BlendComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/AmbientComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ColourComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>

namespace c3d_3ds
{
	//*********************************************************************************************

	namespace materials
	{
		static c3d::RawUniquePtr< c3d::TextureSourceInfo > loadTexture( c3d::MaterialImporter const & importer
			, c3d::Path const & imagePath
			, c3d::TextureConfiguration const & texConfig
			, c3d::ImageLoaderConfig const & loadConfig )
		{
			if ( imagePath.empty() )
				return nullptr;
			return c3d::makeRawUnique< c3d::TextureSourceInfo >( importer.loadTexture( imagePath, texConfig, loadConfig ) );
		}

		static c3d::SamplerRPtr loadSampler( TdsImporterFile const & file
			, TdsTextureData const & textureInfo )
		{
			auto & engine = *file.getOwner();
			auto defaultSampler = engine.getDefaultSampler();
			auto & cache = engine.getSamplerCache();
			auto compareOp = c3d::ComparisonFunc::eNever;
			auto minFilter = c3d::FilterMode::eLinear;
			auto magFilter = c3d::FilterMode::eLinear;
			auto mipFilter = c3d::MipmapMode::eLinear;
			auto wrapS = textureInfo.wrapMode.has_value() ? *textureInfo.wrapMode : defaultSampler->getWrapS();
			auto wrapT = textureInfo.wrapMode.has_value() ? *textureInfo.wrapMode : defaultSampler->getWrapT();
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

		static void parseTransform( TdsTextureData const & textureInfo
			, c3d::TextureTransform & transform )
		{
			transform.translate->x = textureInfo.offsetU.has_value() ? *textureInfo.offsetU : 0.0f;
			transform.translate->y = textureInfo.offsetV.has_value() ? *textureInfo.offsetV : 0.0f;
			transform.scale->x = textureInfo.scaleU.has_value() ? *textureInfo.scaleU : 1.0f;
			transform.scale->y = textureInfo.scaleV.has_value() ? *textureInfo.scaleV : 1.0f;
			transform.rotate = c3d::Angle::fromRadians( textureInfo.rotation.has_value() ? *textureInfo.rotation : 0.0f );
		}

		static bool parseTexture( TdsImporterFile const & file
			, c3d::TextureConfiguration texConfig
			, TdsTextureData const & textureInfo
			, c3d::Path const & imagePath
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, c3d::Pass & pass )
		{
			bool result{};
			parseTransform( textureInfo, texConfig.transform );

			if ( auto sourceInfo = loadTexture( importer, imagePath, texConfig, loadConfig ) )
			{
				result = true;
				c3d::PassTextureConfig passTexConfig{ loadSampler( file, textureInfo ), 0u };
				pass.registerTexture( c3d::move( *sourceInfo ), passTexConfig );
			}

			return result;
		}

		static c3d::TextureConfiguration getRemap( c3d::PassComponentTextureFlag flag
			, c3d::TextureConfiguration const & texConfig
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps )
		{
			auto it = textureRemaps.find( flag );
			if ( it == textureRemaps.end() )
				return texConfig;
			return it->second;
		}

		template< typename ComponentT >
		static bool parseTexture( TdsImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, TdsTextureData const & textureInfo
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, c3d::Pass & pass )
		{
			bool result{};
			if ( textureInfo.path )
			{
				auto & plugin = pass.getComponentPlugin< ComponentT >();
				auto texFlags = plugin.getTextureFlags();
				auto texConfig = plugin.getBaseTextureConfiguration();
				auto imagePath = *textureInfo.path;

				if constexpr ( std::is_same_v< ComponentT, c3d::HeightMapComponent > )
					importer.convertToNormalMap( imagePath, texConfig );

				result = parseTexture( file, getRemap( texFlags, texConfig, textureRemaps )
					, textureInfo, imagePath, loadConfig, importer, pass );
			}
			return result;
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
	}

	//*********************************************************************************************

	TdsMaterialImporter::TdsMaterialImporter( c3d::Engine & engine )
		: c3d::MaterialImporter{ engine, cuT( "3ds" ) }
	{
	}

	bool TdsMaterialImporter::importMaterial( c3d::Material & material )
	{
		auto & file = static_cast< TdsImporterFile const & >( *m_file );
		auto name = material.getName();
		auto it = file.get3dsMaterials().find( name );
		if ( it == file.get3dsMaterials().end() )
			return false;

		auto pass = material.createPass();
		auto tdsMaterial = it->second;

		if ( tdsMaterial.ambient.has_value() )
			pass->createComponent< c3d::AmbientComponent >()->setAmbient( *tdsMaterial.ambient + file.getSceneAmbient() );
		if ( tdsMaterial.diffuse.has_value() )
			pass->createComponent< c3d::ColourComponent >()->setColour( *tdsMaterial.diffuse );
		if ( tdsMaterial.specular.has_value() )
			pass->createComponent< c3d::SpecularComponent >()->setSpecular( *tdsMaterial.specular );
		if ( tdsMaterial.specularExponent.has_value() || tdsMaterial.shininessStrength.has_value() )
			pass->createComponent< c3d::RoughnessComponent >()->setShininess( ( tdsMaterial.specularExponent.has_value() ? *tdsMaterial.specularExponent : 1.0f )
				* ( tdsMaterial.shininessStrength.has_value() ? *tdsMaterial.shininessStrength : 1.0f ) );
		if ( tdsMaterial.emissiveFactor.has_value() )
			pass->createComponent< c3d::EmissiveComponent >()->setEmissiveFactor( *tdsMaterial.emissiveFactor );
		if ( tdsMaterial.twoSided.has_value() )
			pass->createComponent< c3d::TwoSidedComponent >()->setTwoSided( *tdsMaterial.twoSided );
		if ( tdsMaterial.transparency.has_value() )
		{
			float opacity = 1.0f - *tdsMaterial.transparency;
			pass->createComponent< c3d::OpacityComponent >()->setOpacity( opacity );
			if ( opacity != 0.0 && opacity != 1.0 )
				materials::setMixedInterpolative( *pass );
		}

		materials::parseTexture< c3d::ColourMapComponent >( file, m_textureRemaps, tdsMaterial.diffuseMap, m_loadConfig, *this, *pass );
		materials::parseTexture< c3d::SpecularMapComponent >( file, m_textureRemaps, tdsMaterial.specularMap, m_loadConfig, *this, *pass );
		materials::parseTexture< c3d::RoughnessMapComponent >( file, m_textureRemaps, tdsMaterial.shininessMap, m_loadConfig, *this, *pass );
		materials::parseTexture< c3d::EmissiveMapComponent >( file, m_textureRemaps, tdsMaterial.emissiveFactorMap, m_loadConfig, *this, *pass );
		materials::parseTexture< c3d::NormalMapComponent >( file, m_textureRemaps, tdsMaterial.heightMap, m_loadConfig, *this, *pass );
		if ( materials::parseTexture< c3d::OpacityMapComponent >( file, m_textureRemaps, tdsMaterial.opacityMap, m_loadConfig, *this, *pass ) )
			materials::setMixedInterpolative( *pass );

		pass->prepareTextures();

		auto const & engine = *getOwner();
		material.setLightingModelId( engine.getDefaultLightingModel() );
		return true;
	}
}

//*************************************************************************************************
