#include "ObjImporter/ObjMaterialImporter.hpp"
#include "ObjImporter/ObjImporterFile.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Limits.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/Component/Base/BlendComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/AmbientComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SheenComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/AmbientColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SheenMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SheenRoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ColourComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp>

namespace c3d_obj
{
	//*********************************************************************************************

	namespace materials
	{
		static bool isValidValue( c3d::StringView parameter, c3d::StringView source, uint32_t index )
		{
			bool result = false;

			if ( index < source.size() )
			{
				auto splitted = c3d::string::split( c3d::String{ source.substr( index ) }, cuT( " " ), 2 );

				if ( splitted.size() > 1 )
				{
					if ( parameter == cuT( "s" ) || parameter == cuT( "o" ) || parameter == cuT( "t" ) )
						result = c3d::string::isFloating( splitted[0] );
					else if ( parameter == cuT( "blendu" ) || parameter == cuT( "blendv" ) || parameter == cuT( "cc" ) || parameter == cuT( "clamp" ) )
						result = ( splitted[0] == cuT( "on" ) || splitted[0] == cuT( "off" ) );
					else if ( parameter == cuT( "texres" ) )
						result = c3d::string::isInteger( splitted[0] );
					else if ( parameter == cuT( "bm" ) )
						result = c3d::string::isFloating( splitted[0] );
					else if ( parameter == cuT( "mm" ) )
						result = c3d::string::isFloating( splitted[0] );
					else if ( parameter == cuT( "imfchan" ) )
						result = ( splitted[0] == cuT( "r" ) || splitted[0] == cuT( "g" ) || splitted[0] == cuT( "b" ) || splitted[0] == cuT( "m" ) || splitted[0] == cuT( "l" ) || splitted[0] == cuT( "z" ) );
				}
			}

			return result;
		}

		static void parseTextureParameter( c3d::StringView parameter, c3d::Point3f & value )
		{
			auto splitted = c3d::string::split( parameter, cuT( " " ) );
			auto maxCount = std::min( 4u, uint32_t( splitted.size() ) );
			for ( uint32_t i = 1u; i < maxCount; i++ )
				value[i - 1u] = c3d::string::toFloat( c3d::String{ splitted[i] } );
		}

		static void parseTextureParameter( c3d::StringView parameter, float & value )
		{
			auto splitted = c3d::string::split( parameter, cuT( " " ) );
			value = c3d::string::toFloat( c3d::String{ splitted[1] } );
		}

		static void parseTextureParameters( c3d::String & parameters
			, c3d::TextureTransform & transform
			, float & heightFactor )
		{
			c3d::String source( parameters );
			parameters.clear();
			c3d::String parameter;
			c3d::String value;
			bool parsingParam = false;
			bool parsingValue = false;
			uint32_t index{};

			while ( index < source.size() )
			{
				auto c = source[index];

				if ( parsingParam )
				{
					// On a rencontré un tiret, on est en train de récupérer le param
					if ( c == cuT( ' ' ) )
					{
						// on est sur un espace, on a fini de récupérer le param
						parsingValue = true;
						parsingParam = false;
					}
					else
					{
						parameter += c;
					}
				}
				else if ( parsingValue )
				{
					// On a fini de récupérer le param, on est en train de récupérer les valeurs
					if ( c == cuT( ' ' ) )
					{
						// On est sur un espace, on vérifie le caractère suivant pour savoir, en fonction du param si c'est une valeur ou pas
						if ( isValidValue( parameter, source, uint32_t( index + 1u ) ) )
						{
							value += c;
						}
						else
						{
							// Les caractères suivants ne forment pas une valeur valide pour le param
							parsingValue = false;

							if ( parameter == cuT( "s" ) )
								parseTextureParameter( parameter + cuT( " " ) + value, transform.scale );
							else if ( parameter == cuT( "o" ) )
								parseTextureParameter( parameter + cuT( " " ) + value, transform.translate );
							else if ( parameter == cuT( "bm" ) )
								parseTextureParameter( parameter + cuT( " " ) + value, heightFactor );
						}
					}
					else
					{
						value += c;
					}
				}
				else if ( c == cuT( '-' ) )
				{
					// on est sur un tiret, on commence à récupérer le param
					parameter.clear();
					parsingParam = true;
				}
				else
				{
					// On n'est nulle part, on ajoute le caractère au résultat
					parameters += source.substr( index );
					break;
				}

				++index;
			}
		}

		static c3d::RawUniquePtr< c3d::TextureSourceInfo > loadTexture( c3d::MaterialImporter const & importer
			, c3d::String const & parameters
			, c3d::TextureConfiguration const & texConfig
			, c3d::ImageLoaderConfig const & loadConfig )
		{
			if ( parameters.empty() )
				return nullptr;
			c3d::Path imagePath{ parameters };
			if ( !c3d::File::fileExists( imagePath ) )
				return nullptr;
			return c3d::makeRawUnique< c3d::TextureSourceInfo >( importer.loadTexture( imagePath, texConfig, loadConfig ) );
		}

		static c3d::SamplerRPtr loadSampler( ObjImporterFile const & file )
		{
			auto & engine = *file.getOwner();
			auto defaultSampler = engine.getDefaultSampler();
			auto & cache = engine.getSamplerCache();
			auto compareOp = c3d::ComparisonFunc::eNever;
			auto minFilter = c3d::FilterMode::eLinear;
			auto magFilter = c3d::FilterMode::eLinear;
			auto mipFilter = c3d::MipmapMode::eLinear;
			auto wrapS = defaultSampler->getWrapS();
			auto wrapT = defaultSampler->getWrapT();
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

		static bool parseTexture( ObjImporterFile const & file
			, c3d::TextureConfiguration texConfig
			, c3d::String parameters
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, c3d::Pass & pass )
		{
			bool result{};
			parseTextureParameters( parameters, texConfig.transform, texConfig.heightFactor );

			if ( auto sourceInfo = loadTexture( importer, parameters, texConfig, loadConfig ) )
			{
				result = true;
				c3d::PassTextureConfig passTexConfig{ loadSampler( file ), 0u };
				pass.registerTexture( c3d::move( *sourceInfo ), passTexConfig );
			}

			return result;
		}

		static c3d::TextureConfiguration getRemap( c3d::PassComponentTextureFlag flag
			, c3d::TextureConfiguration texConfig
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps )
		{
			auto it = textureRemaps.find( flag );
			if ( it == textureRemaps.end() )
				return texConfig;
			return it->second;
		}

		template< typename ComponentT >
		static bool parseTexture( ObjImporterFile const & file
			, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
			, c3d::String const & parameters
			, c3d::ImageLoaderConfig const & loadConfig
			, c3d::MaterialImporter const & importer
			, c3d::Pass & pass )
		{
			auto & plugin = pass.getComponentPlugin< ComponentT >();
			auto texFlags = plugin.getTextureFlags();
			auto texConfig = plugin.getBaseTextureConfiguration();
			return parseTexture( file, getRemap( texFlags, texConfig, textureRemaps )
				, parameters, loadConfig, importer, pass );
		}

		static c3d::RgbColour getRgbColour( c3d::String const & value )
		{
			c3d::Array< float, 3u > colorComponents;
			c3d::StringStream stream{ value };
			stream >> colorComponents[0] >> colorComponents[1] >> colorComponents[2];
			return c3d::RgbColour::fromComponents( colorComponents[0], colorComponents[1], colorComponents[2] );
		}

		static c3d::HdrRgbColour getHdrRgbColour( c3d::String const & value )
		{
			c3d::Array< float, 3u > colorComponents;
			c3d::StringStream stream{ value };
			stream >> colorComponents[0] >> colorComponents[1] >> colorComponents[2];
			return c3d::HdrRgbColour::fromComponents( colorComponents[0], colorComponents[1], colorComponents[2] );
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

	ObjMaterialImporter::ObjMaterialImporter( c3d::Engine & engine )
		: c3d::MaterialImporter{ engine, cuT( "Obj" ) }
	{
	}

	bool ObjMaterialImporter::importMaterial( c3d::Material & material )
	{
		auto & file = static_cast< ObjImporterFile const & >( *m_file );
		auto name = material.getName();
		auto it = file.getObjMaterials().find( name );
		if ( it == file.getObjMaterials().end() )
			return false;

		auto pass = material.createPass();
		auto mtlLines = it->second.materialLines;
		std::optional< float > opacity;

		for ( auto const & mtlLine : mtlLines )
		{
			if ( auto splitted = c3d::string::split( mtlLine, cuT( " " ), 1 );
				!splitted.empty() )
			{
				auto section = c3d::string::lowerCase( c3d::String{ splitted[0] } );
				c3d::String value;
				if ( splitted.size() > 1 )
				{
					value = splitted[1];
					c3d::string::trim( value );

					if ( section == cuT( "illum" ) && c3d::string::toInt( value ) == 0 )
						pass->createComponent< c3d::PassHeaderComponent >()->enableLighting( false );
					else if ( section == cuT( "ka" ) )
						pass->createComponent< c3d::AmbientComponent >()->setAmbient( materials::getRgbColour( value ) );
					else if ( section == cuT( "kd" ) )
						pass->createComponent< c3d::ColourComponent >()->setColour( materials::getHdrRgbColour( value ) );
					else if ( section == cuT( "ks" ) )
						pass->createComponent< c3d::SpecularComponent >()->setSpecular( materials::getRgbColour( value ) );
					else if ( section == cuT( "tr" ) || section == cuT( "d" ) )
						opacity = c3d::string::toFloat( value );
					else if ( section == cuT( "ni" ) )
						pass->createComponent< c3d::RefractionComponent >()->setRefractionRatio(  c3d::string::toFloat( value ) );
					else if ( section == cuT( "ns" ) )
						pass->createComponent< c3d::RoughnessComponent >()->setShininess( c3d::string::toFloat( value ) * c3d::MaxPhongShininess / 2048.0f );
					else if ( section == cuT( "pr" ) )
						pass->createComponent< c3d::RoughnessComponent >()->setRoughness( c3d::string::toFloat( value ) );
					else if ( section == cuT( "pm" ) )
						pass->createComponent< c3d::MetalnessComponent >()->setMetalness( c3d::string::toFloat( value ) );
					else if ( section == cuT( "ps" ) )
						pass->createComponent< c3d::SheenComponent >()->setSheenColour( materials::getHdrRgbColour( value ) );
					else if ( section == cuT( "pcr" ) )
						pass->createComponent< c3d::ClearcoatComponent >()->setClearcoatFactor( c3d::string::toFloat( value ) );
					else if ( section.find( cuT( "pc" ) ) == 0 )
						pass->createComponent< c3d::ClearcoatComponent >()->setRoughnessFactor( c3d::string::toFloat( value ) );
					else if ( section == cuT( "map_kd" ) )
						materials::parseTexture< c3d::ColourMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "bump" ) || section == cuT( "map_bump" ) )
						materials::parseTexture< c3d::NormalMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "map_kn" ) || section == cuT( "norm" ) )
						materials::parseTexture< c3d::NormalMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "map_d" ) || section == cuT( "map_opacity" )
						&& materials::parseTexture< c3d::OpacityMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass ) )
						materials::setMixedInterpolative( *pass );
					else if ( section == cuT( "map_ks" ) )
						materials::parseTexture< c3d::SpecularMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "map_ka" ) )
						materials::parseTexture< c3d::AmbientColourMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "map_pm" ) )
						materials::parseTexture< c3d::MetalnessMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "map_ps" ) )
						materials::parseTexture< c3d::SheenMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "map_ns" ) || section == cuT( "map_pr" ) )
						materials::parseTexture< c3d::RoughnessMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "map_ke" ) || section == cuT( "map_emissive" ) )
						materials::parseTexture< c3d::EmissiveMapComponent >( file, m_textureRemaps, value, m_loadConfig, *this, *pass );
					else if ( section == cuT( "refl" ) )
					{
						// Reflection map
					}
					else if ( section == cuT( "tf" ) )
					{
						// Transmission color
					}
				}
			}
		}

		pass->prepareTextures();

		auto const & engine = *getOwner();
		material.setLightingModelId( engine.getDefaultLightingModel() );
		return true;
	}
}

//*************************************************************************************************
