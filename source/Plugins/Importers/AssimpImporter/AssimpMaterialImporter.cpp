#include "AssimpImporter/AssimpMaterialImporter.hpp"

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
#include <Castor3D/Material/Pass/Component/Lighting/AmbientComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/AttenuationComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/ClearcoatComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/LightingModelComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SheenComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularFactorComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/ThicknessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/AmbientColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ClearcoatMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ClearcoatNormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ClearcoatRoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OcclusionMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SheenMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SheenRoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ThicknessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/TransmissionMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/TransmittanceMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/AlphaTestComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ColourComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/HeightComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/OpacityComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/ReflectionComponent.hpp>
#include <Castor3D/Material/Pass/Component/Other/RefractionComponent.hpp>
#include <Castor3D/Miscellaneous/Logger.hpp>
#include <Castor3D/Shader/LightingModelFactory.hpp>

// Materials
#include <EdgesComponent.hpp>
#include <Shaders/GlslToonLighting.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <assimp/material.h>
#include <assimp/version.h>
#if !defined( AI_MATKEY_ROUGHNESS_FACTOR )
#	include <assimp/pbrmaterial.h>
#else
#	include <assimp/GltfMaterial.h>
#endif
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

#if !defined( AI_MATKEY_ROUGHNESS_FACTOR )
#	define AI_MATKEY_ROUGHNESS_FACTOR "$mat.roughnessFactor", 0, 0
#	define AI_MATKEY_SPECULAR_FACTOR "$mat.specularFactor", 0, 0
#	define AI_MATKEY_GLOSSINESS_FACTOR "$mat.glossinessFactor", 0, 0
#	define AI_MATKEY_METALLIC_FACTOR "$mat.metallicFactor", 0, 0
#	define AI_MATKEY_BASE_COLOR "$clr.base", 0, 0
#	define AI_MATKEY_TRANSMISSION_FACTOR "$mat.transmission.factor", 0, 0
#	define AI_MATKEY_CLEARCOAT_FACTOR "$mat.clearcoat.factor", 0, 0
#	define AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR "$mat.clearcoat.roughnessFactor", 0, 0
#	define AI_MATKEY_SHEEN_COLOR_FACTOR "$clr.sheen.factor", 0, 0
#	define AI_MATKEY_SHEEN_ROUGHNESS_FACTOR "$mat.sheen.roughnessFactor", 0, 0
#	define AI_MATKEY_VOLUME_THICKNESS_FACTOR "$mat.volume.thicknessFactor", 0, 0
#	define AI_MATKEY_VOLUME_ATTENUATION_COLOR "$mat.volume.attenuationColor", 0, 0
#	define AI_MATKEY_VOLUME_ATTENUATION_DISTANCE "$mat.volume.attenuationDistance", 0, 0
#	define AI_MATKEY_USE_COLOR_MAP "$mat.useColorMap", 0, 0
#	define AI_MATKEY_USE_METALLIC_MAP "$mat.useMetallicMap", 0, 0
#	define AI_MATKEY_USE_ROUGHNESS_MAP "$mat.useRoughnessMap", 0, 0
#	define AI_MATKEY_USE_EMISSIVE_MAP "$mat.useEmissiveMap", 0, 0
#	define AI_MATKEY_EMISSIVE_INTENSITY "$mat.emissiveIntensity", 0, 0
#	define AI_MATKEY_ANISOTROPY_FACTOR "$mat.anisotropyFactor", 0, 0
static constexpr aiShadingMode aiShadingMode_PBR_BRDF = aiShadingMode( 0xb );
static constexpr aiShadingMode aiShadingMode_Unlit = aiShadingMode_NoShading;
#endif

namespace c3d_assimp
{
	//*********************************************************************************************

	static auto constexpr TextureType_BASE_COLOR = aiTextureType( 12 );
	static auto constexpr TextureType_NORMAL_CAMERA = aiTextureType( 13 );
	static auto constexpr TextureType_EMISSION_COLOR = aiTextureType( 14 );
	static auto constexpr TextureType_METALNESS = aiTextureType( 15 );
	static auto constexpr TextureType_DIFFUSE_ROUGHNESS = aiTextureType( 16 );
	static auto constexpr TextureType_AMBIENT_OCCLUSION = aiTextureType( 17 );
	static auto constexpr TextureType_SHEEN = aiTextureType( 19 );
	static auto constexpr TextureType_CLEARCOAT = aiTextureType( 20 );
	static auto constexpr TextureType_TRANSMISSION = aiTextureType( 21 );
	static auto constexpr TextureType_GLTF_METALLIC_ROUGHNESS = aiTextureType( 27 );

#if AI_TEXTURE_TYPE_MAX == TextureType_GLTF_METALLIC_ROUGHNESS
	static auto constexpr TextureType_OCCLUSION_ROUGHNESS_METALNESS = aiTextureType( 27 );
#else
	static auto constexpr TextureType_OCCLUSION_ROUGHNESS_METALNESS = aiTextureType( 18 );
#endif

	//*********************************************************************************************

	namespace materials
	{
		static constexpr aiShadingMode ShadingMode_PBR_BRDF = aiShadingMode( 0xb );

		static c3d::String decodeUri( c3d::StringView uri )
		{
			c3d::MbString escaped{ c3d::toUtf8( uri ) };

			for ( auto i = escaped.begin(); i != escaped.end(); ++i )
			{
				if ( *i == '%' )
				{
					c3d::Array< char, 3 > chars = { *( i + 1 ), *( i + 2 ), 0 };
					*i = static_cast< char >( std::strtoul( chars.data(), nullptr, 16 ) );
					escaped.erase( i + 1, i + 3 );
				}
			}

			return c3d::makeString( escaped );
		}

		struct TextureInfo
		{
			c3d::String name;
			uint32_t texcoordSet{};
			aiUVTransform transform{};
			c3d::SamplerObs sampler{};
		};

		class MaterialParser
		{
		private:
			MaterialParser( aiMaterial const & material
				, aiScene const & scene
				, aiShadingMode shadingMode
				, c3d::SamplerObs sampler
				, AssimpMaterialImporter const & importer
				, float emissiveMult
				, bool isObjFile
				, bool loadImages
				, c3d::ImageLoaderConfig loadConfig
				, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
				, c3d::Pass & result )
				: m_material{ material }
				, m_scene{ scene }
				, m_sampler{ sampler }
				, m_importer{ importer }
				, m_emissiveMult{ emissiveMult }
				, m_isObjFile{ isObjFile }
				, m_loadImages{ loadImages }
				, m_loadConfig{ c3d::move( loadConfig ) }
				, m_textureRemaps{ textureRemaps }
				, m_shadingModel{ shadingMode }
				, m_result{ result }
				, m_colourMapPlugin{ m_result.getComponentPlugin< c3d::ColourMapComponent >() }
				, m_metalnessMapPlugin{ m_result.getComponentPlugin< c3d::MetalnessMapComponent >() }
				, m_occlusionMapPlugin{ m_result.getComponentPlugin< c3d::OcclusionMapComponent >() }
				, m_opacityMapPlugin{ m_result.getComponentPlugin< c3d::OpacityMapComponent >() }
				, m_roughnessMapPlugin{ m_result.getComponentPlugin< c3d::RoughnessMapComponent >() }
				, m_specularMapPlugin{ m_result.getComponentPlugin< c3d::SpecularMapComponent >() }
				, m_colourMapFlags{ m_colourMapPlugin.getTextureFlags() }
				, m_metalnessMapFlags{ m_metalnessMapPlugin.getTextureFlags() }
				, m_occlusionMapFlags{ m_occlusionMapPlugin.getTextureFlags() }
				, m_opacityMapFlags{ m_opacityMapPlugin.getTextureFlags() }
				, m_roughnessMapFlags{ m_roughnessMapPlugin.getTextureFlags() }
				, m_specularMapFlags{ m_specularMapPlugin.getTextureFlags() }
				, m_colourBaseConfiguration{ m_colourMapPlugin.getBaseTextureConfiguration() }
				, m_opacityBaseConfiguration{ m_opacityMapPlugin.getBaseTextureConfiguration() }
			{
				if ( m_shadingModel == aiShadingMode_Toon )
					m_result.createComponent< toon::EdgesComponent >();
				else if ( m_shadingModel == aiShadingMode_NoShading )
					m_result.enableLighting( false );
			}

		public:
			static void parse( aiMaterial const & material
				, aiScene const & scene
				, aiShadingMode shadingMode
				, c3d::SamplerObs sampler
				, AssimpMaterialImporter const & importer
				, float emissiveMult
				, bool isObjFile
				, bool loadImages
				, c3d::ImageLoaderConfig loadConfig
				, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
				, c3d::Pass & pass )
			{
				MaterialParser parser{ material, scene, shadingMode, sampler, importer, emissiveMult, isObjFile, loadImages, c3d::move( loadConfig ), textureRemaps, pass };
				parser.parseDatas();
				parser.finish();
				pass.prepareTextures();
			}

		private:
			void parseDatas()
			{
				parseTwoSided();
				if ( !parseRoughness() && !parseGlossiness() )
					parseShininess();
				parseComponentDataT< c3d::MetalnessComponent, float >( AI_MATKEY_METALLIC_FACTOR );
				parseColour();
				parseAmbient();
				parseSpecular();
				parseSpecularFactor();
				parseEmissive();
				parseAttenuation();
				parseClearcoat();
				parseSheen();
				parseComponentDataT< c3d::ThicknessComponent, float >( AI_MATKEY_VOLUME_THICKNESS_FACTOR );
				parseComponentDataT< c3d::TransmissionComponent, float >( AI_MATKEY_TRANSMISSION_FACTOR );
				parseRefractionRatio();
				if ( !parseComponentOpaDataT< c3d::OpacityComponent >( AI_MATKEY_OPACITY ) )
					parseComponentInvOpaDataT< c3d::OpacityComponent >( AI_MATKEY_TRANSPARENCYFACTOR, 1.0f );
				parseAlphaRefValue();
			}

			void finish()
			{
				TextureInfo colInfo{};
				TextureInfo nmlInfo{};
				TextureInfo opaInfo{};
				TextureInfo spcInfo{};
				TextureInfo mtlInfo{};
				TextureInfo shnInfo{};
				TextureInfo rghInfo{};
				auto ambInfo = getTextureInfo( aiTextureType_AMBIENT, 0u );
				auto trsInfo = getTextureInfo( TextureType_TRANSMISSION, 0u );
				auto thkInfo = getTextureInfo( TextureType_TRANSMISSION, 1u );
				auto cctInfo = getTextureInfo( TextureType_CLEARCOAT, 0u );
				auto ccrInfo = getTextureInfo( TextureType_CLEARCOAT, 1u );
				auto ccnInfo = getTextureInfo( TextureType_CLEARCOAT, 2u );
				auto shcInfo = getTextureInfo( TextureType_SHEEN, 0u );
				auto shrInfo = getTextureInfo( TextureType_SHEEN, 1u );
				auto occInfo = getTextureInfo( TextureType_AMBIENT_OCCLUSION );
				if ( occInfo.name.empty() )
					occInfo = getTextureInfo( aiTextureType_LIGHTMAP );

				auto hgtInfo = finishHeight();
				auto emiInfo = finishEmissive();
				finishColour( colInfo, nmlInfo, opaInfo, spcInfo, mtlInfo, shnInfo, rghInfo );
				finishSpecular( spcInfo, occInfo, mtlInfo, shnInfo, rghInfo );
				auto hasOpacityTex = finishOpacity( opaInfo );

				if ( m_loadImages )
				{
					loadTextureT< c3d::ColourMapComponent >( colInfo, hasOpacityTex );
					loadTextureT< c3d::AmbientColourMapComponent >( ambInfo, hasOpacityTex );
					loadTextureT< c3d::EmissiveMapComponent >( emiInfo, hasOpacityTex );
					loadTextureT< c3d::SpecularMapComponent >( spcInfo, hasOpacityTex );
					loadTextureT< c3d::MetalnessMapComponent >( mtlInfo, hasOpacityTex );
					loadTextureT< c3d::RoughnessMapComponent >( rghInfo, hasOpacityTex );
					loadTextureT< c3d::RoughnessMapComponent >( shnInfo, hasOpacityTex );
					loadTextureT< c3d::OcclusionMapComponent >( occInfo, hasOpacityTex );
					loadTextureT< c3d::TransmissionMapComponent >( trsInfo, hasOpacityTex );
					loadTextureT< c3d::ThicknessMapComponent >( thkInfo, hasOpacityTex );
					loadTextureT< c3d::ClearcoatMapComponent >( cctInfo, hasOpacityTex );
					loadTextureT< c3d::ClearcoatRoughnessMapComponent >( ccrInfo, hasOpacityTex );
					loadTextureT< c3d::ClearcoatNormalMapComponent >( ccnInfo, hasOpacityTex );
					loadTextureT< c3d::SheenMapComponent >( shcInfo, hasOpacityTex );
					loadTextureT< c3d::SheenRoughnessMapComponent >( shrInfo, hasOpacityTex );

					if ( !nmlInfo.name.empty() )
					{
						loadTextureT< c3d::NormalMapComponent >( nmlInfo, hasOpacityTex );
						loadTextureT< c3d::HeightMapComponent >( hgtInfo, hasOpacityTex );
					}
					else if ( !hgtInfo.name.empty() )
					{
						// Convert bump map to combination of normal and height map.
						auto & plugin = m_result.getComponentPlugin< c3d::NormalMapComponent >();
						auto texFlags = plugin.getTextureFlags();
						auto texConfig = plugin.getBaseTextureConfiguration();
						convertToNormalMap( hgtInfo, texConfig );
						loadTexture( hgtInfo, getRemap( texFlags, texConfig ), hasOpacityTex );
					}
				}
			}

			template< typename ValueT >
			std::pair< ValueT, bool > getValueT( const char * key, unsigned int type, unsigned int idx )
			{
				ValueT value{};
				bool result = ( m_material.Get( key, type, idx, value ) == aiReturn_SUCCESS );
				return { value, result };
			}

			std::pair< c3d::RgbColour, bool > getRgbColour( const char * key, unsigned int type, unsigned int idx )
			{
				c3d::RgbColour colour;
				auto [value, result] = getValueT< aiColor3D >( key, type, idx );
				if ( result )
					colour = c3d::RgbColour::fromComponents( value.r, value.g, value.b );
				return { colour, result };
			}

			std::pair< c3d::HdrRgbColour, bool > getHdrRgbColour( const char * key, unsigned int type, unsigned int idx )
			{
				c3d::HdrRgbColour colour;
				auto [value, result] = getValueT< aiColor3D >( key, type, idx );
				if ( result )
					colour = c3d::HdrRgbColour::fromComponents( value.r, value.g, value.b );
				return { colour, result };
			}

			template< typename ComponentT, typename ValueT >
			bool parseComponentDataT( const char * key, unsigned int type, unsigned int idx )
			{
				auto [value, result] = getValueT< ValueT >( key, type, idx );
				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = value;
					component->setData( *data );
				}
				return result;
			}

			template< typename ComponentT >
			bool parseComponentOpaDataT( const char * key, unsigned int type, unsigned int idx )
			{
				auto [value, result] = getValueT< float >( key, type, idx );
				if ( result )
					m_result.createComponent< ComponentT >()->setOpacity( value );
				return result;
			}

			template< typename ComponentT >
			bool parseComponentInvOpaDataT( const char * key, unsigned int type, unsigned int idx
				, float point )
			{
				auto [value, result] = getValueT< float >( key, type, idx );
				if ( result )
					m_result.createComponent< ComponentT >()->setOpacity( point - value );
				return result;
			}

			bool parseTwoSided()
			{
				auto [value, result] = getValueT< int >( AI_MATKEY_TWOSIDED );
				if ( result )
					m_result.createComponent< c3d::TwoSidedComponent >()->setTwoSided( value != 0 );
				return result;
			}

			bool parseRoughness()
			{
				auto [value, result] = getValueT< float >( AI_MATKEY_ROUGHNESS_FACTOR );
				if ( result )
					m_result.createComponent< c3d::RoughnessComponent >()->setRoughness( value );
				return result;
			}

			bool parseGlossiness()
			{
				auto [value, result] = getValueT< float >( AI_MATKEY_GLOSSINESS_FACTOR );
				if ( result )
					m_result.createComponent< c3d::RoughnessComponent >()->setGlossiness( value );
				return result;
			}

			bool parseShininess()
			{
				auto [value, result] = getValueT< float >( AI_MATKEY_SHININESS );
				if ( result )
				{
					float factor{ 1.0f };
					m_material.Get( AI_MATKEY_SHININESS_STRENGTH, factor );
					if ( m_isObjFile )
						value *= (c3d::MaxPhongShininess / 2048.0f); // 2048 is arbitrary, OBJ is funnnnnn...
					m_result.createComponent< c3d::RoughnessComponent >()->setShininess( value * factor );
				}
				return result;
			}

			void parseColour()
			{
				if ( auto [valueHdr, resultHdr] = getHdrRgbColour( AI_MATKEY_BASE_COLOR ); resultHdr )
					m_result.createComponent< c3d::ColourComponent >()->setColour( valueHdr );
				else if ( auto [value, result] = getRgbColour( AI_MATKEY_COLOR_DIFFUSE ); result )
					m_result.createComponent< c3d::ColourComponent >()->setColour( value );
			}

			void parseAmbient()
			{
				if ( auto [value, result] = getRgbColour( AI_MATKEY_COLOR_AMBIENT ); result )
					m_result.createComponent< c3d::AmbientComponent >()->setAmbient( value );
			}

			void parseSpecular()
			{
				if ( auto [value, result] = getRgbColour( AI_MATKEY_COLOR_SPECULAR ); result )
					m_result.createComponent< c3d::SpecularComponent >()->setSpecular( value );
			}

			void parseSpecularFactor()
			{
				if ( auto [value, result] = getValueT< float >( AI_MATKEY_SPECULAR_FACTOR ); result )
					m_result.createComponent< c3d::SpecularFactorComponent >()->setFactor( value );
			}

			void parseEmissive()
			{
				if ( auto [value, result] = getRgbColour( AI_MATKEY_COLOR_EMISSIVE );
					result && ( value.red() != 0.0f || value.green() != 0.0f || value.blue() != 0.0f ) )
				{
					auto component = m_result.createComponent< c3d::EmissiveComponent >();
					component->setEmissive( value * m_emissiveMult );
					if ( auto [valueI, resultI] = getValueT< float >( AI_MATKEY_EMISSIVE_INTENSITY ); resultI )
						component->setEmissiveFactor( valueI );
				}
			}

			void parseAttenuation()
			{
				auto [colour, hasColour] = getRgbColour( AI_MATKEY_VOLUME_ATTENUATION_COLOR );
				auto [distance, hasDistance] = getValueT< float >( AI_MATKEY_VOLUME_ATTENUATION_DISTANCE );
				if ( hasColour || hasDistance )
				{
					auto component = m_result.createComponent< c3d::AttenuationComponent >();
					component->setAttenuationColour( colour );
					component->setAttenuationDistance( distance );
				}
			}

			void parseClearcoat()
			{
				auto [clearcoat, hasClearcoat] = getValueT< float >( AI_MATKEY_CLEARCOAT_FACTOR );
				auto [roughness, hasRoughness] = getValueT< float >( AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR );
				if ( hasClearcoat || hasRoughness )
				{
					auto component = m_result.createComponent< c3d::ClearcoatComponent >();
					component->setClearcoatFactor( clearcoat );
					component->setRoughnessFactor( roughness );
				}
			}

			void parseSheen()
			{
				auto [sheen, hasSheen] = getHdrRgbColour( AI_MATKEY_SHEEN_COLOR_FACTOR );
				auto [roughness, hasRoughness] = getValueT< float >( AI_MATKEY_SHEEN_ROUGHNESS_FACTOR );
				if ( hasSheen || hasRoughness )
				{
					auto component = m_result.createComponent< c3d::SheenComponent >();
					component->setSheenColour( sheen );
					component->setRoughnessFactor( roughness );
				}
			}

			void parseAlphaRefValue()
			{
				auto [modeName, hasMode] = getValueT< aiString >( AI_MATKEY_GLTF_ALPHAMODE );
				auto [ref, hasRef] = getValueT< float >( AI_MATKEY_GLTF_ALPHACUTOFF );
				if ( hasRef && hasMode )
				{
					auto mode = makeString( modeName );
					if ( mode == cuT( "MASK" ) )
					{
						auto alphaTest = m_result.createComponent< c3d::AlphaTestComponent >();
						alphaTest->setAlphaRefValue( ref );
						alphaTest->setAlphaFunc( c3d::ComparisonFunc::eGreater );
						alphaTest->setBlendAlphaFunc( c3d::ComparisonFunc::eLessOrEqual );
					}
				}
			}

			bool parseRefractionRatio()
			{
				if ( auto [ior, hasIor] = getValueT< float >( AI_MATKEY_REFRACTI ); hasIor )
				{
					m_result.createComponent< c3d::RefractionComponent >()->setRefractionRatio( ior );
					if ( auto transmission = m_result.getComponent< c3d::TransmissionComponent >();
						!transmission )
						m_result.createComponent< c3d::TransmissionComponent >()->setTransmission( 0.0f );
					return true;
				}

				return false;
			}

			c3d::TextureConfiguration getRemap( c3d::PassComponentTextureFlag flag
				, c3d::TextureConfiguration texConfig )
			{
				auto it = m_textureRemaps.find( flag );
				if ( it == m_textureRemaps.end() )
					return texConfig;
				return it->second;
			}

			c3d::Image const & loadImage( c3d::TextureSourceInfo const & source )const
			{
				c3d::Image const * result{};

				if ( source.isBufferImage() )
					result = m_importer.loadImage( source.name()
						, c3d::ImageCreateParams{ source.type(), source.buffer() } );
				else if ( source.isFileImage() )
					result = m_importer.loadImage( source.name()
						, c3d::ImageCreateParams{ source.folder() / source.relative() } );

				if ( !result )
					CU_LoaderError( "Couldn't load image" + c3d::toUtf8( source.name() ) + "." );
				return *result;
			}

			template< typename ComponentT >
			void loadTextureT( TextureInfo const & info, bool hasOpacityTex )
			{
				auto & plugin = m_result.getComponentPlugin< ComponentT >();
				auto texFlags = plugin.getTextureFlags();
				auto texConfig = plugin.getBaseTextureConfiguration();
				loadTexture( info, getRemap( texFlags, texConfig ), hasOpacityTex );
			}

			void loadTexture( TextureInfo const & info
				, c3d::TextureConfiguration texConfig
				, bool hasOpacity )
			{
				if ( !info.name.empty() )
				{
					try
					{
						c3d::RawUniquePtr< c3d::TextureSourceInfo > sourceInfo;
						texConfig.transform = c3d::TextureTransform{ { info.transform.mTranslation.x, info.transform.mTranslation.y, 0.0f }
							, c3d::Angle::fromRadians( info.transform.mRotation )
							, { info.transform.mScaling.x, info.transform.mScaling.y, 1.0f } };

						if ( info.name[0] == cuT( '*' ) )
						{
							auto id = uint32_t( c3d::string::toInt( info.name.substr( 1u ) ) );

							if ( id < m_scene.mNumTextures )
							{
								auto texture = m_scene.mTextures[id];
								c3d::ByteArray data;
								data.resize( texture->mWidth );
								std::memcpy( data.data(), texture->pcData, data.size() );
								sourceInfo = c3d::makeRawUnique< c3d::TextureSourceInfo >( m_importer.loadTexture( cuT( "Image" ) + c3d::string::toString( id )
									, c3d::makeString( texture->achFormatHint )
									, c3d::move( data )
									, texConfig
									, m_loadConfig ) );
							}
						}
						else if ( auto texture = m_scene.GetEmbeddedTexture( c3d::toUtf8( info.name ).c_str() ) )
						{
							c3d::ByteArray data;
							data.resize( texture->mWidth );
							std::memcpy( data.data(), texture->pcData, data.size() );
							sourceInfo = c3d::makeRawUnique< c3d::TextureSourceInfo >( m_importer.loadTexture( info.name
								, c3d::makeString( texture->achFormatHint )
								, c3d::move( data )
								, texConfig ) );
						}
						else
						{
							auto name = decodeUri( info.name );
							sourceInfo = c3d::makeRawUnique< c3d::TextureSourceInfo >( m_importer.loadTexture( c3d::Path{ name }
								, texConfig ) );
						}

						if ( sourceInfo )
						{
							if ( auto texFlags = getFlags( texConfig );
								getComponentsMask( texConfig, m_opacityMapFlags )
									&& c3d::hasAny( texFlags, m_opacityMapFlags ) )
							{
								if ( auto [alphaMode, hasAlphaMode] = getValueT< aiString >( AI_MATKEY_GLTF_ALPHAMODE );
									hasAlphaMode && makeString( alphaMode ) != "OPAQUE" )
									mixedInterpolative( true );

								if ( auto & image = loadImage( *sourceInfo );
									texFlags.size() == 1u && !hasAlphaChannel( image ) )
								{
									addFlagConfiguration( texConfig, { m_opacityMapFlags, 0x00FF0000 } );
									*sourceInfo = c3d::TextureSourceInfo{ *sourceInfo, texConfig };
								}
							}
							else if ( !hasOpacity
								&& texConfig == m_colourBaseConfiguration )
							{
								if ( auto & image = loadImage( *sourceInfo );
									hasAlphaChannel( image ) )
								{
									if ( auto [alphaMode, hasAlphaMode] = getValueT< aiString >( AI_MATKEY_GLTF_ALPHAMODE );
										hasAlphaMode && makeString( alphaMode ) != "OPAQUE" )
										mixedInterpolative( true );

									addFlagConfiguration( texConfig, { m_opacityMapFlags, 0xFF000000 } );
									*sourceInfo = c3d::TextureSourceInfo{ *sourceInfo, texConfig };
								}
							}

							m_result.registerTexture( c3d::move( *sourceInfo )
								, { info.sampler, info.texcoordSet } );
						}
					}
					catch ( std::exception & )
					{
						m_importer.loadTexture( c3d::Path{ info.name }
							, texConfig
							, c3d::PassTextureConfig{ info.sampler }
						, m_result );
					}
				}
			}

			TextureInfo getTextureInfo( aiTextureType type
				, uint32_t index )
			{
				TextureInfo result{};
				auto [name, hasName] = getValueT< aiString >( AI_MATKEY_TEXTURE( type, index ) );
				if ( name.length > 0 )
				{
					result.name = makeString( name );
					if ( auto [texcoordSet, hasSet] = getValueT< int >( AI_MATKEY_UVWSRC( type, index ) ); hasSet )
						result.texcoordSet = uint32_t( texcoordSet );
					m_material.Get( AI_MATKEY_UVTRANSFORM( type, index ), result.transform );
				}

				if ( !result.name.empty() )
				{
					if ( type == aiTextureType_REFLECTION )
						m_result.createComponent< c3d::ReflectionComponent >()->enableReflections();

					auto [minFilter, hasMinFilter] = getValueT< GlFilter >( AI_MATKEY_GLTF_MAPPINGFILTER_MIN( type, index ) );
					auto [magFilter, hasMagFilter] = getValueT< GlFilter >( AI_MATKEY_GLTF_MAPPINGFILTER_MAG( type, index ) );
					auto [addressModeU, hasAddressModeU] = getValueT< aiTextureMapMode >( AI_MATKEY_MAPPINGMODE_U( type, index ) );
					auto [addressModeV, hasAddressModeV] = getValueT< aiTextureMapMode >( AI_MATKEY_MAPPINGMODE_V( type, index ) );

					if ( hasMinFilter || hasMagFilter || hasAddressModeU || hasAddressModeV )
					{
						auto & engine = *m_result.getOwner()->getEngine();
						auto const & cache = engine.getSamplerCache();
						auto samplerName = c3d::getSamplerName( c3d::ComparisonFunc::eNever
							, hasMinFilter ? fromAssimp( minFilter ) : m_sampler->getMinFilter()
							, hasMagFilter ? fromAssimp( magFilter ) : m_sampler->getMagFilter()
							, hasMinFilter ? getMipFilter( minFilter ) : m_sampler->getMipFilter()
							, hasAddressModeU ? fromAssimp( addressModeU ) : m_sampler->getWrapS()
							, hasAddressModeV ? fromAssimp( addressModeV ) : m_sampler->getWrapT()
							, m_sampler->getWrapR() );

						if ( !cache.has( samplerName ) )
						{
							auto sampler = engine.createSampler( samplerName, engine );
							sampler->setMinFilter( hasMinFilter ? fromAssimp( minFilter ) : m_sampler->getMinFilter() );
							sampler->setMagFilter( hasMagFilter ? fromAssimp( magFilter ) : m_sampler->getMagFilter() );
							sampler->setMipFilter( hasMinFilter ? getMipFilter( minFilter ) : m_sampler->getMipFilter() );
							sampler->setWrapS( hasAddressModeU ? fromAssimp( addressModeU ) : m_sampler->getWrapS() );
							sampler->setWrapT( hasAddressModeV ? fromAssimp( addressModeV ) : m_sampler->getWrapT() );
							sampler->setWrapR( m_sampler->getWrapR() );
							engine.addSampler( samplerName, sampler, false );
						}

						result.sampler = cache.find( samplerName );
					}
					else
					{
						result.sampler = m_sampler;
					}
				}

				return result;
			}

			TextureInfo getTextureInfo( aiTextureType type )
			{
				TextureInfo result;
				uint32_t index{};

				while ( result.name.empty() && index < c3d::MaxTextureCoordinatesSets )
				{
					if ( auto tmp = getTextureInfo( type, index );
						!tmp.name.empty() )
					{
						result = tmp;
					}

					++index;
				}

				return result;
			}

			void convertToNormalMap( TextureInfo & info
				, c3d::TextureConfiguration & pconfig )
			{
				auto path = c3d::Path{ info.name };

				if ( m_importer.convertToNormalMap( path, pconfig ) )
				{
					m_result.createComponent< c3d::NormalMapComponent >();
					info.name = path;
				}
			}

			void mixedInterpolative( bool blending )
			{
				m_result.createComponent< c3d::OpacityComponent >();

				auto twoSided = m_result.createComponent< c3d::TwoSidedComponent >();
				twoSided->setTwoSided( true );

				if ( !m_result.hasComponent< c3d::AlphaTestComponent >() )
				{
					auto alphaTest = m_result.createComponent< c3d::AlphaTestComponent >();
					alphaTest->setAlphaRefValue( 0.95f );
					alphaTest->setAlphaFunc( c3d::ComparisonFunc::eGreater );
					alphaTest->setBlendAlphaFunc( c3d::ComparisonFunc::eLessOrEqual );
				}

				if ( blending )
				{
					auto blend = m_result.createComponent< c3d::BlendComponent >();
					blend->setAlphaBlendMode( c3d::BlendMode::eInterpolative );
				}
			}

			bool hasMatKey( const char * key, unsigned int type, unsigned int idx )const
			{
				aiMaterialProperty const * p{};
				return aiGetMaterialProperty( &m_material, key, type, idx, &p ) == aiReturn_SUCCESS
					&& p;
			}

			bool hasTexKey( aiTextureType type )const
			{
				return aiGetMaterialTextureCount( &m_material, type ) > 0;
			}

			void finishColour( TextureInfo & colInfo
				, TextureInfo & nmlInfo
				, TextureInfo & opaInfo
				, TextureInfo & spcInfo
				, TextureInfo & mtlInfo
				, TextureInfo & shnInfo
				, TextureInfo & rghInfo )
			{
				colInfo = getTextureInfo( TextureType_BASE_COLOR );
				bool isCollada{};

				if ( colInfo.name.empty() )
				{
					colInfo = getTextureInfo( aiTextureType_DIFFUSE );
					isCollada = !colInfo.name.empty()
						&& colInfo.name.find( cuT( "_Cine_" ) ) != c3d::String::npos
						&& colInfo.name.find( cuT( "/MI_CH_" ) ) != c3d::String::npos;

					if ( isCollada )
					{
						// Workaround for Collada textures.
						c3d::String strGlob = colInfo.name + cuT( ".tga" );
						c3d::string::replace( strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
						c3d::String strDiff = strGlob;
						c3d::String strNorm = strGlob;
						c3d::String strSpec = strGlob;
						c3d::String strOpac = strGlob;
						colInfo.name = c3d::string::replace( strDiff, cuT( "_Cine_" ), cuT( "_D_" ) );
						nmlInfo.name = c3d::string::replace( strNorm, cuT( "_Cine_" ), cuT( "_N_" ) );
						spcInfo.name = c3d::string::replace( strSpec, cuT( "_Cine_" ), cuT( "_S_" ) );
						opaInfo.name = c3d::string::replace( strOpac, cuT( "_Cine_" ), cuT( "_A_" ) );
						nmlInfo.transform = colInfo.transform;
						spcInfo.transform = colInfo.transform;
						opaInfo.transform = colInfo.transform;
					}
				}

				if ( !isCollada )
				{
					nmlInfo = getTextureInfo( aiTextureType_NORMALS );

					if ( nmlInfo.name.empty() )
					{
						nmlInfo = getTextureInfo( TextureType_NORMAL_CAMERA );
					}

					opaInfo = getTextureInfo( aiTextureType_OPACITY );
					spcInfo = getTextureInfo( aiTextureType_SPECULAR );
					shnInfo = getTextureInfo( aiTextureType_SHININESS );
					mtlInfo = getTextureInfo( TextureType_METALNESS );
					rghInfo = getTextureInfo( TextureType_DIFFUSE_ROUGHNESS );
				}
			}

			void finishSpecular( TextureInfo & spcInfo
				, TextureInfo & occInfo
				, TextureInfo & mtlInfo
				, TextureInfo & shnInfo
				, TextureInfo & rghInfo )
			{
				if ( spcInfo.name.empty() )
				{
					spcInfo = getTextureInfo( TextureType_OCCLUSION_ROUGHNESS_METALNESS );

					if ( !spcInfo.name.empty() )
					{
						auto spcConfig{ getRemap( m_specularMapFlags, c3d::TextureConfiguration{} ) };

						if ( !getComponentsMask( spcConfig, m_metalnessMapFlags ) )
							addFlagConfiguration( spcConfig, { m_metalnessMapFlags, 0x000000FF } );

						if ( !getComponentsMask( spcConfig, m_roughnessMapFlags ) )
							addFlagConfiguration( spcConfig, { m_roughnessMapFlags, 0x0000FF00 } );

						if ( spcInfo.name == occInfo.name )
						{
							if ( !getComponentsMask( spcConfig, m_occlusionMapFlags ) )
								addFlagConfiguration( spcConfig, { m_occlusionMapFlags, 0x00FF0000 } );

							occInfo.name.clear();
						}

						if ( spcInfo.name == mtlInfo.name )
							mtlInfo.name.clear();

						if ( spcInfo.name == shnInfo.name )
							shnInfo.name.clear();

						if ( spcInfo.name == rghInfo.name )
							rghInfo.name.clear();

						m_textureRemaps.try_emplace( m_specularMapFlags, spcConfig );
					}
				}
			}

			bool finishOpacity( TextureInfo const & opaInfo )
			{
				auto opacity = m_result.getComponent< c3d::OpacityComponent >();
				bool hasOpacityTex = !opaInfo.name.empty();

				if ( hasOpacityTex )
				{
					loadTexture( opaInfo, getRemap( m_opacityMapFlags, m_opacityBaseConfiguration )
						, hasOpacityTex );
				}
				else
				{
					if ( auto [value, hasValue] = getValueT< aiString >( AI_MATKEY_GLTF_ALPHAMODE ); hasValue )
					{
						if ( auto mode = makeString( value );
							mode != cuT( "OPAQUE" ) )
						{
							auto config = getRemap( m_colourMapFlags, m_colourBaseConfiguration );
							addFlagConfiguration( config, { m_opacityMapFlags, 0xFF000000 } );
							m_textureRemaps.try_emplace( m_colourMapFlags, config );
							hasOpacityTex = true;
							mixedInterpolative( mode == cuT( "BLEND" ) );
						}
					}
				}

				if ( hasOpacityTex && !opacity )
					opacity = m_result.createComponent< c3d::OpacityComponent >();

				// force non 0.0 opacity when an opacity map is set
				if ( hasOpacityTex && opacity->getOpacity() == 0.0f )
					opacity->setOpacity( 1.0f );

				if ( hasOpacityTex
					&& m_result.getAlphaFunc() == c3d::ComparisonFunc::eAlways )
					mixedInterpolative( true );

				return hasOpacityTex;
			}

			TextureInfo finishEmissive()
			{
				TextureInfo emiInfo = getTextureInfo( TextureType_EMISSION_COLOR );
				if ( emiInfo.name.empty() )
					emiInfo = getTextureInfo( aiTextureType_EMISSIVE );
				return emiInfo;
			}

			TextureInfo finishHeight()
			{
				auto hgtInfo = getTextureInfo( aiTextureType_DISPLACEMENT );
				if ( hgtInfo.name.empty() )
					hgtInfo = getTextureInfo( aiTextureType_HEIGHT );
				return hgtInfo;
			}

		private:
			aiMaterial const & m_material;
			aiScene const & m_scene;
			c3d::SamplerObs m_sampler;
			AssimpMaterialImporter const & m_importer;
			float m_emissiveMult;
			bool m_isObjFile;
			bool m_loadImages;
			c3d::ImageLoaderConfig m_loadConfig;
			c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > m_textureRemaps;
			aiShadingMode m_shadingModel{};
			c3d::Pass & m_result;
			c3d::PassComponentPlugin const & m_colourMapPlugin;
			c3d::PassComponentPlugin const & m_metalnessMapPlugin;
			c3d::PassComponentPlugin const & m_occlusionMapPlugin;
			c3d::PassComponentPlugin const & m_opacityMapPlugin;
			c3d::PassComponentPlugin const & m_roughnessMapPlugin;
			c3d::PassComponentPlugin const & m_specularMapPlugin;
			c3d::PassComponentTextureFlag m_colourMapFlags;
			c3d::PassComponentTextureFlag m_metalnessMapFlags;
			c3d::PassComponentTextureFlag m_occlusionMapFlags;
			c3d::PassComponentTextureFlag m_opacityMapFlags;
			c3d::PassComponentTextureFlag m_roughnessMapFlags;
			c3d::PassComponentTextureFlag m_specularMapFlags;
			c3d::TextureConfiguration m_colourBaseConfiguration;
			c3d::TextureConfiguration m_opacityBaseConfiguration;
		};

		static c3d::LightingModelID getLightingModel( c3d::Engine const & engine
			, aiShadingMode shadingMode )
		{
			auto & factory = engine.getLightingModelFactory();

			if ( engine.getDefaultLightingModel() != factory.getLightingModelId( c3d::String{ c3d::PhongPass::LightingModel }
				, { c3d::PhongPass::DefaultDiffuseBrdf.name
					, c3d::PhongPass::DefaultSpecularBrdf.name
					, c3d::PhongPass::DefaultSheenBrdf.name
					, c3d::PhongPass::DefaultClearcoatBrdf.name
					, c3d::PhongPass::DefaultScatteringModel.name } ) )
			{
				return engine.getDefaultLightingModel();
			}

			switch ( shadingMode )
			{
			case aiShadingMode_Flat:
			case aiShadingMode_Gouraud:
			case aiShadingMode_Phong:
			case aiShadingMode_Blinn:
				return factory.getLightingModelId( c3d::String{ c3d::PhongPass::LightingModel }
					, { c3d::PhongPass::DefaultDiffuseBrdf.name
						, c3d::PhongPass::DefaultSpecularBrdf.name
						, c3d::PhongPass::DefaultSheenBrdf.name
						, c3d::PhongPass::DefaultClearcoatBrdf.name
						, c3d::PhongPass::DefaultScatteringModel.name } );
			case aiShadingMode_Toon:
				return factory.getLightingModelId( toon::shader::ToonPhongLightingModel::getName()
					, { c3d::PhongPass::DefaultDiffuseBrdf.name
						, c3d::PhongPass::DefaultSpecularBrdf.name
						, c3d::PhongPass::DefaultSheenBrdf.name
						, c3d::PhongPass::DefaultClearcoatBrdf.name
						, c3d::PhongPass::DefaultScatteringModel.name } );
			case aiShadingMode_OrenNayar:
			case aiShadingMode_Minnaert:
			case aiShadingMode_CookTorrance:
			case aiShadingMode_Fresnel:
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wswitch"
			case aiShadingMode_PBR_BRDF:
				return factory.getLightingModelId( c3d::String{ c3d::PbrPass::LightingModel }
					, { c3d::PbrPass::DefaultDiffuseBrdf.name
						, c3d::PbrPass::DefaultSpecularBrdf.name
						, c3d::PbrPass::DefaultSheenBrdf.name
						, c3d::PbrPass::DefaultClearcoatBrdf.name
						, c3d::PbrPass::DefaultScatteringModel.name } );
#pragma GCC diagnostic pop
			case aiShadingMode_Unlit:
				return factory.getLightingModelId( c3d::String{ c3d::PbrPass::LightingModel }
					, { c3d::PbrPass::DefaultDiffuseBrdf.name
						, c3d::PbrPass::DefaultSpecularBrdf.name
						, c3d::PbrPass::DefaultSheenBrdf.name
						, c3d::PbrPass::DefaultClearcoatBrdf.name
						, c3d::PbrPass::DefaultScatteringModel.name } );
			default:
				return factory.getLightingModelId( c3d::String{ c3d::PhongPass::LightingModel }
					, { c3d::PbrPass::DefaultDiffuseBrdf.name
						, c3d::PbrPass::DefaultSpecularBrdf.name
						, c3d::PbrPass::DefaultSheenBrdf.name
						, c3d::PbrPass::DefaultClearcoatBrdf.name
						, c3d::PbrPass::DefaultScatteringModel.name } );
			}
		}
	}

	//*********************************************************************************************

	AssimpMaterialImporter::AssimpMaterialImporter( c3d::Engine & engine
		, c3d::Parameters const & parameters )
		: c3d::MaterialImporter{ engine, cuT( "Assimp" ) }
		, m_parameters{ parameters }
	{
	}

	bool AssimpMaterialImporter::importMaterial( c3d::Material & material )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = material.getName();
		auto it = file.getMaterials().find( name );

		if ( it == file.getMaterials().end() )
		{
			return false;
		}

		int ishadingMode{};
		it->second->Get( AI_MATKEY_SHADING_MODEL, ishadingMode );
		auto shadingMode = aiShadingMode( ishadingMode );
		material.setLightingModelId( materials::getLightingModel( *getEngine(), shadingMode ) );
		auto pass = material.createPass();
		materials::MaterialParser::parse( *it->second 
			, file.getAiScene()
			, shadingMode
			, getEngine()->getDefaultSampler()
			, *this
			, m_emissiveMult
			, c3d::string::lowerCase( file.getFileName().getExtension() ) == "obj"
			, ( m_parameters.get< bool >( "no_images" ) == false )
			, m_loadConfig
			, m_textureRemaps
			, *pass );
		return true;
	}

	//*********************************************************************************************
}
