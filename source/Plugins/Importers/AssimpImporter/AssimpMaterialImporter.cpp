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
#else
#	include <assimp/GltfMaterial.h>
#endif
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d_assimp
{
	//*********************************************************************************************

	static auto constexpr TextureType_BASE_COLOR = aiTextureType( 12 );
	static auto constexpr TextureType_NORMAL_CAMERA = aiTextureType( 13 );
	static auto constexpr TextureType_EMISSION_COLOR = aiTextureType( 14 );
	static auto constexpr TextureType_METALNESS = aiTextureType( 15 );
	static auto constexpr TextureType_DIFFUSE_ROUGHNESS = aiTextureType( 16 );
	static auto constexpr TextureType_AMBIENT_OCCLUSION = aiTextureType( 17 );
	static auto constexpr TextureType_OCCLUSION_ROUGHNESS_METALNESS = aiTextureType_UNKNOWN;
	static auto constexpr TextureType_SHEEN = aiTextureType( 19 );
	static auto constexpr TextureType_CLEARCOAT = aiTextureType( 20 );
	static auto constexpr TextureType_TRANSMISSION = aiTextureType( 21 );

	//*********************************************************************************************

	namespace materials
	{
		static constexpr aiShadingMode ShadingMode_PBR_BRDF = aiShadingMode( 0xb );

		static c3d::String decodeUri( c3d::String uri )
		{
			c3d::MbString escaped{ c3d::toUtf8( uri ) };

			for ( auto i = escaped.begin(); i != escaped.end(); i++ )
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
				, bool disableImageCompression
				, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
				, c3d::Pass & result )
				: m_material{ material }
				, m_scene{ scene }
				, m_sampler{ sampler }
				, m_importer{ importer }
				, m_emissiveMult{ emissiveMult }
				, m_loadConfig{ !disableImageCompression, true, true }
				, m_textureRemaps{ textureRemaps }
				, m_shadingModel{ shadingMode }
				, m_isPbr{ detectPbr() }
				, m_result{ result }
				, m_colourMapPlugin{ m_result.getComponentPlugin< c3d::ColourMapComponent >() }
				, m_emissiveMapPlugin{ m_result.getComponentPlugin< c3d::EmissiveMapComponent >() }
				, m_heightMapPlugin{ m_result.getComponentPlugin< c3d::HeightMapComponent >() }
				, m_metalnessMapPlugin{ m_result.getComponentPlugin< c3d::MetalnessMapComponent >() }
				, m_normalMapPlugin{ m_result.getComponentPlugin< c3d::NormalMapComponent >() }
				, m_occlusionMapPlugin{ m_result.getComponentPlugin< c3d::OcclusionMapComponent >() }
				, m_opacityMapPlugin{ m_result.getComponentPlugin< c3d::OpacityMapComponent >() }
				, m_roughnessMapPlugin{ m_result.getComponentPlugin< c3d::RoughnessMapComponent >() }
				, m_specularMapPlugin{ m_result.getComponentPlugin< c3d::SpecularMapComponent >() }
				, m_transmissionMapPlugin{ m_result.getComponentPlugin< c3d::TransmissionMapComponent >() }
				, m_transmittanceMapPlugin{ m_result.getComponentPlugin< c3d::TransmittanceMapComponent >() }
				, m_thicknessMapPlugin{ m_result.getComponentPlugin< c3d::ThicknessMapComponent >() }
				, m_clearcoatMapPlugin{ m_result.getComponentPlugin< c3d::ClearcoatMapComponent >() }
				, m_clearcoatNormalMapPlugin{ m_result.getComponentPlugin< c3d::ClearcoatNormalMapComponent >() }
				, m_clearcoatRoughnessMapPlugin{ m_result.getComponentPlugin< c3d::ClearcoatRoughnessMapComponent >() }
				, m_sheenMapPlugin{ m_result.getComponentPlugin< c3d::SheenMapComponent >() }
				, m_sheenRoughnessMapPlugin{ m_result.getComponentPlugin< c3d::SheenRoughnessMapComponent >() }
				, m_colourMapFlags{ m_colourMapPlugin.getTextureFlags() }
				, m_emissiveMapFlags{ m_emissiveMapPlugin.getTextureFlags() }
				, m_heightMapFlags{ m_heightMapPlugin.getTextureFlags() }
				, m_metalnessMapFlags{ m_metalnessMapPlugin.getTextureFlags() }
				, m_normalMapFlags{ m_normalMapPlugin.getTextureFlags() }
				, m_occlusionMapFlags{ m_occlusionMapPlugin.getTextureFlags() }
				, m_opacityMapFlags{ m_opacityMapPlugin.getTextureFlags() }
				, m_roughnessMapFlags{ m_roughnessMapPlugin.getTextureFlags() }
				, m_specularMapFlags{ m_specularMapPlugin.getTextureFlags() }
				, m_transmissionMapFlags{ m_transmissionMapPlugin.getTextureFlags() }
				, m_transmittanceMapFlags{ m_transmittanceMapPlugin.getTextureFlags() }
				, m_thicknessMapFlags{ m_thicknessMapPlugin.getTextureFlags() }
				, m_clearcoatMapFlags{ m_clearcoatMapPlugin.getTextureFlags() }
				, m_clearcoatNormalMapFlags{ m_clearcoatNormalMapPlugin.getTextureFlags() }
				, m_clearcoatRoughnessMapFlags{ m_clearcoatRoughnessMapPlugin.getTextureFlags() }
				, m_sheenMapFlags{ m_sheenMapPlugin.getTextureFlags() }
				, m_sheenRoughnessMapFlags{ m_sheenRoughnessMapPlugin.getTextureFlags() }
				, m_colourBaseConfiguration{ m_colourMapPlugin.getBaseTextureConfiguration() }
				, m_emissiveBaseConfiguration{ m_emissiveMapPlugin.getBaseTextureConfiguration() }
				, m_heightBaseConfiguration{ m_heightMapPlugin.getBaseTextureConfiguration() }
				, m_metalnessBaseConfiguration{ m_metalnessMapPlugin.getBaseTextureConfiguration() }
				, m_normalBaseConfiguration{ m_normalMapPlugin.getBaseTextureConfiguration() }
				, m_occlusionBaseConfiguration{ m_occlusionMapPlugin.getBaseTextureConfiguration() }
				, m_opacityBaseConfiguration{ m_opacityMapPlugin.getBaseTextureConfiguration() }
				, m_roughnessBaseConfiguration{ m_roughnessMapPlugin.getBaseTextureConfiguration() }
				, m_specularBaseConfiguration{ m_specularMapPlugin.getBaseTextureConfiguration() }
				, m_transmissionBaseConfiguration{ m_transmissionMapPlugin.getBaseTextureConfiguration() }
				, m_transmittanceBaseConfiguration{ m_transmittanceMapPlugin.getBaseTextureConfiguration() }
				, m_thicknessBaseConfiguration{ m_thicknessMapPlugin.getBaseTextureConfiguration() }
				, m_clearcoatBaseConfiguration{ m_clearcoatMapPlugin.getBaseTextureConfiguration() }
				, m_clearcoatNormalBaseConfiguration{ m_clearcoatNormalMapPlugin.getBaseTextureConfiguration() }
				, m_clearcoatRoughnessBaseConfiguration{ m_clearcoatRoughnessMapPlugin.getBaseTextureConfiguration() }
				, m_sheenBaseConfiguration{ m_sheenMapPlugin.getBaseTextureConfiguration() }
				, m_sheenRoughnessBaseConfiguration{ m_sheenRoughnessMapPlugin.getBaseTextureConfiguration() }
			{
				if ( m_shadingModel == aiShadingMode_Toon )
				{
					m_result.createComponent< toon::EdgesComponent >();
				}
				else if ( m_shadingModel == aiShadingMode_NoShading )
				{
					m_result.enableLighting( false );
				}
			}

		public:
			static void parse( aiMaterial const & material
				, aiScene const & scene
				, aiShadingMode shadingMode
				, c3d::SamplerObs sampler
				, AssimpMaterialImporter const & importer
				, float emissiveMult
				, bool disableImageCompression
				, c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > const & textureRemaps
				, c3d::Pass & pass )
			{
				MaterialParser parser{ material, scene, shadingMode, sampler, importer, emissiveMult, disableImageCompression, textureRemaps, pass };
				parser.parseDatas();
				parser.finish();
				pass.prepareTextures();
			}

		private:
			void parseDatas()
			{
				parseComponentBoolData< c3d::TwoSidedComponent >( AI_MATKEY_TWOSIDED );

				if ( !parseRoughness() )
				{
					if ( !parseGlossiness() )
					{
						parseShininess();
					}
				}

				parseComponentDataT< c3d::MetalnessComponent, float >( AI_MATKEY_METALLIC_FACTOR );

				if ( !parseComponentHdrRgbData< c3d::ColourComponent >( AI_MATKEY_BASE_COLOR ) )
				{
					parseComponentHdrRgbData< c3d::ColourComponent >( AI_MATKEY_COLOR_DIFFUSE );
				}

				parseSpecular();
				parseSpecularFactor();
				parseEmissive();
				parseAttenuation();
				parseClearcoat();
				parseSheen();
				parseComponentDataT< c3d::ThicknessComponent, float >( AI_MATKEY_VOLUME_THICKNESS_FACTOR );
				parseComponentDataT< c3d::TransmissionComponent, float >( AI_MATKEY_TRANSMISSION_FACTOR );
				m_hasRefr = parseRefractionRatio();

				if ( !parseComponentOpaDataT< c3d::OpacityComponent >( AI_MATKEY_OPACITY ) )
				{
					parseComponentInvOpaDataT< c3d::OpacityComponent >( AI_MATKEY_TRANSPARENCYFACTOR, 1.0f );
				}

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
				auto trsInfo = getTextureInfo( TextureType_TRANSMISSION, 0u );
				auto thkInfo = getTextureInfo( TextureType_TRANSMISSION, 1u );
				auto cctInfo = getTextureInfo( TextureType_CLEARCOAT, 0u );
				auto ccrInfo = getTextureInfo( TextureType_CLEARCOAT, 1u );
				auto ccnInfo = getTextureInfo( TextureType_CLEARCOAT, 2u );
				auto shcInfo = getTextureInfo( TextureType_SHEEN, 0u );
				auto shrInfo = getTextureInfo( TextureType_SHEEN, 1u );
				auto occInfo = getTextureInfo( TextureType_AMBIENT_OCCLUSION );

				if ( occInfo.name.empty() )
				{
					occInfo = getTextureInfo( aiTextureType_LIGHTMAP );
				}

				auto hgtInfo = finishHeight();
				auto emiInfo = finishEmissive();
				finishColour( colInfo, nmlInfo, opaInfo, spcInfo, mtlInfo, shnInfo, rghInfo );
				finishSpecular( spcInfo, occInfo, mtlInfo, shnInfo, rghInfo );
				auto hasOpacityTex = finishOpacity( opaInfo );

				loadTexture( colInfo, getRemap( m_colourMapFlags, m_colourBaseConfiguration )
					, hasOpacityTex );
				loadTexture( emiInfo, getRemap( m_emissiveMapFlags, m_emissiveBaseConfiguration )
					, hasOpacityTex );
				loadTexture( spcInfo, getRemap( m_specularMapFlags, m_specularBaseConfiguration )
					, hasOpacityTex );
				loadTexture( mtlInfo, getRemap( m_metalnessMapFlags, m_metalnessBaseConfiguration )
					, hasOpacityTex );
				loadTexture( rghInfo, getRemap( m_roughnessMapFlags, m_roughnessBaseConfiguration )
					, hasOpacityTex );
				loadTexture( shnInfo, getRemap( m_roughnessMapFlags, m_roughnessBaseConfiguration )
					, hasOpacityTex );
				loadTexture( occInfo, getRemap( m_occlusionMapFlags, m_occlusionBaseConfiguration )
					, hasOpacityTex );
				loadTexture( trsInfo, getRemap( m_transmissionMapFlags, m_transmissionBaseConfiguration )
					, hasOpacityTex );
				loadTexture( thkInfo, getRemap( m_thicknessMapFlags, m_thicknessBaseConfiguration )
					, hasOpacityTex );
				loadTexture( cctInfo, getRemap( m_clearcoatMapFlags, m_clearcoatBaseConfiguration )
					, hasOpacityTex );
				loadTexture( ccrInfo, getRemap( m_clearcoatRoughnessMapFlags, m_clearcoatRoughnessBaseConfiguration )
					, hasOpacityTex );
				loadTexture( ccnInfo, getRemap( m_clearcoatNormalMapFlags, m_clearcoatNormalBaseConfiguration )
					, hasOpacityTex );
				loadTexture( shcInfo, getRemap( m_sheenMapFlags, m_sheenBaseConfiguration )
					, hasOpacityTex );
				loadTexture( shrInfo, getRemap( m_sheenRoughnessMapFlags, m_sheenRoughnessBaseConfiguration )
					, hasOpacityTex );

				if ( !nmlInfo.name.empty() )
				{
					loadTexture( nmlInfo, getRemap( m_normalMapFlags, m_normalBaseConfiguration )
						, hasOpacityTex );
					loadTexture( hgtInfo, getRemap( m_heightMapFlags, m_heightBaseConfiguration )
						, hasOpacityTex );
				}
				else
				{
					auto texConfig = m_normalBaseConfiguration;
					convertToNormalMap( hgtInfo, texConfig );
					loadTexture( hgtInfo
						, getRemap( m_normalMapFlags
							, texConfig )
						, hasOpacityTex );
				}
			}

			template< typename ValueT >
			bool parseDataT( const char * pKey, unsigned int type, unsigned int idx
				, ValueT & value )
			{
				ValueT ret{};
				bool result = ( m_material.Get( pKey, type, idx, ret ) == aiReturn_SUCCESS );

				if ( result )
				{
					value = ret;
				}

				return result;
			}

			template< typename ComponentT, typename ValueT >
			bool parseComponentDataT( const char * pKey, unsigned int type, unsigned int idx )
			{
				ValueT value{};
				bool result = parseDataT( pKey, type, idx, value );

				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = value;
					component->setData( *data );
				}

				return result;
			}

			bool parseRoughness()
			{
				float value{};
				bool result = parseDataT( AI_MATKEY_ROUGHNESS_FACTOR, value );

				if ( result )
				{
					auto component = m_result.createComponent< c3d::RoughnessComponent >();
					component->setRoughness( value );
				}

				return result;
			}

			bool parseGlossiness()
			{
				float value{};
				bool result = parseDataT( AI_MATKEY_GLOSSINESS_FACTOR, value );

				if ( result )
				{
					auto component = m_result.createComponent< c3d::RoughnessComponent >();
					component->setGlossiness( 1.0f - value );
				}

				return result;
			}

			bool parseShininess()
			{
				float value{};
				bool result = parseDataT( AI_MATKEY_SHININESS, value );

				if ( result )
				{
					float factor{ 1.0f };
					m_material.Get( AI_MATKEY_SHININESS_STRENGTH, factor );
					auto component = m_result.createComponent< c3d::RoughnessComponent >();
					component->setShininess( value * factor );
				}

				return result;
			}

			template< typename ComponentT, typename ValueT, typename FuncT >
			bool parseComponentDataT( const char * pKey, unsigned int type, unsigned int idx, FuncT transform )
			{
				ValueT value{};
				bool result = parseDataT( pKey, type, idx, value );

				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = transform( value );
					component->setData( *data );
				}

				return result;
			}

			template< typename ComponentT >
			bool parseComponentOpaDataT( const char * pKey, unsigned int type, unsigned int idx )
			{
				float value{};
				bool result = parseDataT( pKey, type, idx, value );

				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					component->setOpacity( value );
				}

				return result;
			}

			template< typename ComponentT >
			bool parseComponentInvOpaDataT( const char * pKey, unsigned int type, unsigned int idx
				, float point )
			{
				float value{};
				bool result = parseDataT( pKey, type, idx, value );

				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					component->setOpacity( point - value );
				}

				return result;
			}

			template< typename ComponentT, typename ValueT, typename FuncT >
			bool parseComponentDataFactorT( const char * pKey0, unsigned int type0, unsigned int idx0
				, const char * pKey1, unsigned int type1, unsigned int idx1
				, FuncT transform )
			{
				ValueT value{};
				bool result = parseDataT( pKey0, type0, idx0, value );

				if ( result )
				{
					ValueT factor{ 1.0f };
					m_material.Get( pKey1, type1, idx1, factor );
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = transform( value * factor );
					component->setData( *data );
				}

				return result;
			}

			template< typename ComponentT, typename ValueT >
			bool parseComponentDataFactorT( const char * pKey0, unsigned int type0, unsigned int idx0
				, const char * pKey1, unsigned int type1, unsigned int idx1 )
			{
				ValueT value{};
				bool result = parseDataT( pKey0, type0, idx0, value );

				if ( result )
				{
					ValueT factor{ 1.0f };
					m_material.Get( pKey1, type1, idx1, factor );
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = value * factor;
					component->setData( *data );
				}

				return result;
			}

			template< typename ComponentT >
			bool parseComponentBoolData( const char * pKey, unsigned int type, unsigned int idx )
			{
				int value{};
				bool result = parseDataT( pKey, type, idx, value );

				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = value != 0;
					component->setData( *data );
				}

				return result;
			}

			template< typename ComponentT >
			bool parseComponentHdrRgbData( const char * pKey, unsigned int type, unsigned int idx )
			{
				aiColor3D value{};
				bool result = parseDataT( pKey, type, idx, value );

				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = c3d::HdrRgbColour::fromComponents( value.r
						, value.g
						, value.b );
					component->setData( *data );
				}

				return result;
			}

			template< typename ComponentT >
			bool parseComponentRgbData( const char * pKey, unsigned int type, unsigned int idx )
			{
				aiColor3D value{};
				bool result = parseDataT( pKey, type, idx, value );

				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = c3d::RgbColour::fromComponents( value.r
						, value.g
						, value.b );
					component->setData( *data );
				}

				return result;
			}

			template< typename ComponentT >
			bool parseComponentRgbFloatData( const char * pKey, unsigned int type, unsigned int idx )
			{
				aiColor3D value{};
				bool result = parseDataT( pKey, type, idx, value );

				if ( result )
				{
					auto component = m_result.createComponent< ComponentT >();
					auto data = component->getData();
					*data = float( c3d::point::length( c3d::Point3f{ value.r
						, value.g
						, value.b } ) );
					component->setData( *data );
				}

				return result;
			}

			void parseSpecular()
			{
				aiColor3D colour = { 1, 1, 1 };
				bool hasColour = m_material.Get( AI_MATKEY_COLOR_SPECULAR, colour ) == aiReturn_SUCCESS;

				if ( hasColour )
				{
					auto component = m_result.createComponent< c3d::SpecularComponent >();
					component->setSpecular( c3d::RgbColour{ colour.r
						, colour.g
						, colour.b } );
				}
			}

			void parseSpecularFactor()
			{
				float factor{ 1.0f };
				bool hasFactor = m_material.Get( AI_MATKEY_SPECULAR_FACTOR, factor ) == aiReturn_SUCCESS;

				if ( hasFactor )
				{
					auto component = m_result.createComponent< c3d::SpecularFactorComponent >();
					component->setFactor( factor );
				}
			}

			void parseEmissive()
			{
				aiColor3D emissive = { 1, 1, 1 };

				if ( m_material.Get( AI_MATKEY_COLOR_EMISSIVE, emissive ) == aiReturn_SUCCESS )
				{
					if ( emissive.r != 0 || emissive.g != 0 || emissive.b != 0 )
					{
						auto component = m_result.createComponent< c3d::EmissiveComponent >();
						component->setEmissive( c3d::RgbColour{ m_emissiveMult * emissive.r
							, m_emissiveMult * emissive.g
							, m_emissiveMult * emissive.b } );
						float emissiveIntensity = 1.0f;
						m_material.Get( AI_MATKEY_EMISSIVE_INTENSITY, emissiveIntensity );
						component->setEmissiveFactor( emissiveIntensity );
					}
				}
			}

			void parseAttenuation()
			{
				aiColor3D colour = { 1, 1, 1 };
				float distance{};
				bool hasColour = m_material.Get( AI_MATKEY_VOLUME_ATTENUATION_COLOR, colour ) == aiReturn_SUCCESS;
				bool hasDistance = m_material.Get( AI_MATKEY_VOLUME_ATTENUATION_DISTANCE, distance ) == aiReturn_SUCCESS;

				if ( hasColour || hasDistance )
				{
					auto component = m_result.createComponent< c3d::AttenuationComponent >();
					component->setAttenuationColour( c3d::RgbColour{ colour.r
						, colour.g
						, colour.b } );
					component->setAttenuationDistance( distance );
				}
			}

			void parseClearcoat()
			{
				float clearcoat{};
				float roughness{};
				bool hasClearcoat = m_material.Get( AI_MATKEY_CLEARCOAT_FACTOR, clearcoat ) == aiReturn_SUCCESS;
				bool hasRoughness = m_material.Get( AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR, roughness ) == aiReturn_SUCCESS;

				if ( hasClearcoat || hasRoughness )
				{
					auto component = m_result.createComponent< c3d::ClearcoatComponent >();
					component->setClearcoatFactor( clearcoat );
					component->setRoughnessFactor( roughness );
				}
			}

			void parseSheen()
			{
				aiColor3D sheen{};
				float roughness{};
				bool hasSheen = m_material.Get( AI_MATKEY_SHEEN_COLOR_FACTOR, sheen ) == aiReturn_SUCCESS;
				bool hasRoughness = m_material.Get( AI_MATKEY_SHEEN_ROUGHNESS_FACTOR, roughness ) == aiReturn_SUCCESS;

				if ( hasSheen || hasRoughness )
				{
					auto component = m_result.createComponent< c3d::SheenComponent >();
					component->setSheenColour( c3d::HdrRgbColour{ sheen.r
						, sheen.g
						, sheen.b } );
					component->setRoughnessFactor( roughness );
				}
			}

			void parseAlphaRefValue()
			{
				float ref{ 1.0f };
				aiString value;

				if ( m_material.Get( AI_MATKEY_GLTF_ALPHACUTOFF, ref ) == aiReturn_SUCCESS )
				{
					if ( m_material.Get( AI_MATKEY_GLTF_ALPHAMODE, value ) == aiReturn_SUCCESS )
					{
						auto mode = makeString( value );

						if ( mode == cuT( "MASK" ) )
						{
							auto alphaTest = m_result.createComponent< c3d::AlphaTestComponent >();
							alphaTest->setAlphaRefValue( ref );
							alphaTest->setAlphaFunc( c3d::ComparisonFunc::eGreater );
							alphaTest->setBlendAlphaFunc( c3d::ComparisonFunc::eLessOrEqual );
						}
					}
				}
			}

			bool parseRefractionRatio()
			{
				float ior{ 1.0f };

				if ( m_material.Get( AI_MATKEY_REFRACTI, ior ) == aiReturn_SUCCESS )
				{
					auto component = m_result.createComponent< c3d::RefractionComponent >();
					component->setRefractionRatio( ior );
					auto transmission = m_result.getComponent< c3d::TransmissionComponent >();

					if ( !transmission )
					{
						transmission = m_result.createComponent< c3d::TransmissionComponent >();
						transmission->setTransmission( 0.0f );
						return true;
					}

					return true;
				}

				return false;
			}

			c3d::TextureConfiguration getRemap( c3d::PassComponentTextureFlag flag
				, c3d::TextureConfiguration texConfig )
			{
				auto it = m_textureRemaps.find( flag );

				if ( it == m_textureRemaps.end() )
				{
					return texConfig;
				}

				return it->second;
			}

			c3d::Image const & loadImage( c3d::TextureSourceInfo const & source )
			{
				c3d::ImageRPtr result{};

				if ( source.isBufferImage() )
				{
					result = m_importer.loadImage( source.name()
						, c3d::ImageCreateParams{ source.type(), source.buffer() } );
				}
				else if ( source.isFileImage() )
				{
					result = m_importer.loadImage( source.name()
						, c3d::ImageCreateParams{ source.folder() / source.relative() } );
				}

				if ( !result )
				{
					CU_LoaderError( "Couldn't load image" + c3d::toUtf8( source.name() ) + "." );
				}

				return *result;
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
							auto texFlags = getFlags( texConfig );

							if ( getComponentsMask( texConfig, m_opacityMapFlags )
								&& c3d::hasAny( texFlags, m_opacityMapFlags ) )
							{
								aiString alphaMode;

								if ( m_material.Get( AI_MATKEY_GLTF_ALPHAMODE, alphaMode ) != aiReturn_SUCCESS )
								{
									mixedInterpolative( true );
								}

								if ( auto & image = loadImage( *sourceInfo );
									!hasAlphaChannel( image ) )
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
									aiString alphaMode;

									if ( m_material.Get( AI_MATKEY_GLTF_ALPHAMODE, alphaMode ) != aiReturn_SUCCESS )
									{
										mixedInterpolative( true );
									}

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
				aiString name;
				m_material.Get( AI_MATKEY_TEXTURE( type, index ), name );

				if ( name.length > 0 )
				{
					result.name = makeString( name );
					int texcoordSet{};

					if ( m_material.Get( AI_MATKEY_UVWSRC( type, index ), texcoordSet ) == AI_SUCCESS )
					{
						result.texcoordSet = uint32_t( texcoordSet );
					}

					m_material.Get( AI_MATKEY_UVTRANSFORM( type, index ), result.transform );
				}

				if ( !result.name.empty() )
				{
					switch ( type )
					{
					case aiTextureType_REFLECTION:
						m_result.createComponent< c3d::ReflectionComponent >()->enableReflections();
						break;
					default:
						break;
					}

					auto & engine = *m_result.getOwner()->getEngine();
					auto & cache = engine.getSamplerCache();

					GlFilter minFilter{ GlFilter::LINEAR };
					GlFilter magFilter{ GlFilter::LINEAR };
					aiTextureMapMode addressModeU{ aiTextureMapMode_Wrap };
					aiTextureMapMode addressModeV{ aiTextureMapMode_Wrap };
					auto hasMinFilter = m_material.Get( AI_MATKEY_GLTF_MAPPINGFILTER_MIN( type, index ), minFilter ) == aiReturn_SUCCESS;
					auto hasMagFilter = m_material.Get( AI_MATKEY_GLTF_MAPPINGFILTER_MAG( type, index ), magFilter ) == aiReturn_SUCCESS;
					auto hasAddressModeU = m_material.Get( AI_MATKEY_MAPPINGMODE_U( type, index ), addressModeU ) == aiReturn_SUCCESS;
					auto hasAddressModeV = m_material.Get( AI_MATKEY_MAPPINGMODE_V( type, index ), addressModeV ) == aiReturn_SUCCESS;

					if ( hasMinFilter || hasMagFilter || hasAddressModeU || hasAddressModeV )
					{
						c3d::String samplerName;
						aiString mappingName;
						m_material.Get( AI_MATKEY_GLTF_MAPPINGNAME( type, index ), mappingName );

						if ( mappingName.length > 0 )
						{
							samplerName = makeString( mappingName );
						}
						else
						{
							samplerName = m_result.getOwner()->getName()
								+ cuT( "_" ) + c3d::string::toString( m_result.getIndex() )
								+ cuT( "_" ) + result.name;
						}

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
					auto tmp = getTextureInfo( type, index );

					if ( !tmp.name.empty() )
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

			bool hasMatKey( const char * pKey, unsigned int type, unsigned int idx )
			{
				aiMaterialProperty const * p{};
				return aiGetMaterialProperty( &m_material, pKey, type, idx, &p ) == aiReturn_SUCCESS
					&& p;
			}

			bool hasTexKey( aiTextureType type )
			{
				return aiGetMaterialTextureCount( &m_material, type ) > 0;
			}

			bool detectPbr()
			{
				return ( m_material.Get( AI_MATKEY_SHADING_MODEL, m_shadingModel ) == aiReturn_SUCCESS
						&& ( m_shadingModel == ShadingMode_PBR_BRDF || m_shadingModel == aiShadingMode_CookTorrance ) )
					|| hasMatKey( AI_MATKEY_USE_COLOR_MAP )
					|| hasMatKey( AI_MATKEY_BASE_COLOR )
					|| hasMatKey( AI_MATKEY_USE_METALLIC_MAP )
					|| hasMatKey( AI_MATKEY_METALLIC_FACTOR )
					|| hasMatKey( AI_MATKEY_USE_ROUGHNESS_MAP )
					|| hasMatKey( AI_MATKEY_ROUGHNESS_FACTOR )
					|| hasMatKey( AI_MATKEY_ANISOTROPY_FACTOR )
					|| hasMatKey( AI_MATKEY_SPECULAR_FACTOR )
					|| hasMatKey( AI_MATKEY_GLOSSINESS_FACTOR )
					|| hasMatKey( AI_MATKEY_SHEEN_COLOR_FACTOR )
					|| hasMatKey( AI_MATKEY_SHEEN_ROUGHNESS_FACTOR )
					|| hasMatKey( AI_MATKEY_CLEARCOAT_FACTOR )
					|| hasMatKey( AI_MATKEY_CLEARCOAT_ROUGHNESS_FACTOR )
					|| hasMatKey( AI_MATKEY_TRANSMISSION_FACTOR )
					|| hasMatKey( AI_MATKEY_VOLUME_THICKNESS_FACTOR )
					|| hasMatKey( AI_MATKEY_VOLUME_ATTENUATION_DISTANCE )
					|| hasMatKey( AI_MATKEY_VOLUME_ATTENUATION_COLOR )
					|| hasMatKey( AI_MATKEY_USE_EMISSIVE_MAP )
					|| hasMatKey( AI_MATKEY_EMISSIVE_INTENSITY )
					|| hasTexKey( TextureType_BASE_COLOR )
					|| hasTexKey( TextureType_METALNESS )
					|| hasTexKey( TextureType_SHEEN )
					|| hasTexKey( TextureType_CLEARCOAT )
					|| hasTexKey( TextureType_TRANSMISSION );
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
						{
							addFlagConfiguration( spcConfig, { m_metalnessMapFlags, 0x000000FF } );
						}

						if ( !getComponentsMask( spcConfig, m_roughnessMapFlags ) )
						{
							addFlagConfiguration( spcConfig, { m_roughnessMapFlags, 0x0000FF00 } );
						}

						if ( spcInfo.name == occInfo.name )
						{
							if ( !getComponentsMask( spcConfig, m_occlusionMapFlags ) )
							{
								addFlagConfiguration( spcConfig, { m_occlusionMapFlags, 0x00FF0000 } );
							}

							occInfo.name.clear();
						}

						if ( spcInfo.name == mtlInfo.name )
						{
							mtlInfo.name.clear();
						}

						if ( spcInfo.name == shnInfo.name )
						{
							shnInfo.name.clear();
						}

						if ( spcInfo.name == rghInfo.name )
						{
							rghInfo.name.clear();
						}

						m_textureRemaps.emplace( m_specularMapFlags, spcConfig );
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
					aiString value;

					if ( m_material.Get( AI_MATKEY_GLTF_ALPHAMODE, value ) == aiReturn_SUCCESS )
					{
						auto mode = makeString( value );

						if ( mode != cuT( "OPAQUE" ) )
						{
							auto config = getRemap( m_colourMapFlags, m_colourBaseConfiguration );
							addFlagConfiguration( config, { m_opacityMapFlags, 0xFF000000 } );
							m_textureRemaps.emplace( m_colourMapFlags, config );
							hasOpacityTex = true;
							mixedInterpolative( mode == cuT( "BLEND" ) );
						}
					}
				}

				if ( hasOpacityTex && !opacity )
				{
					opacity = m_result.createComponent< c3d::OpacityComponent >();
				}

				// force non 0.0 opacity when an opacity map is set
				if ( hasOpacityTex && opacity->getOpacity() == 0.0f )
				{
					opacity->setOpacity( 1.0f );
				}

				if ( hasOpacityTex
					&& m_result.getAlphaFunc() == c3d::ComparisonFunc::eAlways )
				{
					mixedInterpolative( true );
				}

				return hasOpacityTex;
			}

			TextureInfo finishEmissive()
			{
				TextureInfo emiInfo = getTextureInfo( TextureType_EMISSION_COLOR );

				if ( emiInfo.name.empty() )
				{
					emiInfo = getTextureInfo( aiTextureType_EMISSIVE );
				}

				return emiInfo;
			}

			TextureInfo finishHeight()
			{
				auto hgtInfo = getTextureInfo( aiTextureType_DISPLACEMENT );

				if ( hgtInfo.name.empty() )
				{
					hgtInfo = getTextureInfo( aiTextureType_HEIGHT );
				}

				return hgtInfo;
			}

		private:
			aiMaterial const & m_material;
			aiScene const & m_scene;
			c3d::SamplerObs m_sampler;
			AssimpMaterialImporter const & m_importer;
			float m_emissiveMult;
			c3d::ImageLoaderConfig m_loadConfig;
			c3d::Map< c3d::PassComponentTextureFlag, c3d::TextureConfiguration > m_textureRemaps;
			aiShadingMode m_shadingModel{};
			bool m_isPbr;
			bool m_hasRefr{};
			c3d::Pass & m_result;
			c3d::PassComponentPlugin const & m_colourMapPlugin;
			c3d::PassComponentPlugin const & m_emissiveMapPlugin;
			c3d::PassComponentPlugin const & m_heightMapPlugin;
			c3d::PassComponentPlugin const & m_metalnessMapPlugin;
			c3d::PassComponentPlugin const & m_normalMapPlugin;
			c3d::PassComponentPlugin const & m_occlusionMapPlugin;
			c3d::PassComponentPlugin const & m_opacityMapPlugin;
			c3d::PassComponentPlugin const & m_roughnessMapPlugin;
			c3d::PassComponentPlugin const & m_specularMapPlugin;
			c3d::PassComponentPlugin const & m_transmissionMapPlugin;
			c3d::PassComponentPlugin const & m_transmittanceMapPlugin;
			c3d::PassComponentPlugin const & m_thicknessMapPlugin;
			c3d::PassComponentPlugin const & m_clearcoatMapPlugin;
			c3d::PassComponentPlugin const & m_clearcoatNormalMapPlugin;
			c3d::PassComponentPlugin const & m_clearcoatRoughnessMapPlugin;
			c3d::PassComponentPlugin const & m_sheenMapPlugin;
			c3d::PassComponentPlugin const & m_sheenRoughnessMapPlugin;
			c3d::PassComponentTextureFlag m_colourMapFlags;
			c3d::PassComponentTextureFlag m_emissiveMapFlags;
			c3d::PassComponentTextureFlag m_heightMapFlags;
			c3d::PassComponentTextureFlag m_metalnessMapFlags;
			c3d::PassComponentTextureFlag m_normalMapFlags;
			c3d::PassComponentTextureFlag m_occlusionMapFlags;
			c3d::PassComponentTextureFlag m_opacityMapFlags;
			c3d::PassComponentTextureFlag m_roughnessMapFlags;
			c3d::PassComponentTextureFlag m_specularMapFlags;
			c3d::PassComponentTextureFlag m_transmissionMapFlags;
			c3d::PassComponentTextureFlag m_transmittanceMapFlags;
			c3d::PassComponentTextureFlag m_thicknessMapFlags;
			c3d::PassComponentTextureFlag m_clearcoatMapFlags;
			c3d::PassComponentTextureFlag m_clearcoatNormalMapFlags;
			c3d::PassComponentTextureFlag m_clearcoatRoughnessMapFlags;
			c3d::PassComponentTextureFlag m_sheenMapFlags;
			c3d::PassComponentTextureFlag m_sheenRoughnessMapFlags;
			c3d::TextureConfiguration m_colourBaseConfiguration;
			c3d::TextureConfiguration m_emissiveBaseConfiguration;
			c3d::TextureConfiguration m_heightBaseConfiguration;
			c3d::TextureConfiguration m_metalnessBaseConfiguration;
			c3d::TextureConfiguration m_normalBaseConfiguration;
			c3d::TextureConfiguration m_occlusionBaseConfiguration;
			c3d::TextureConfiguration m_opacityBaseConfiguration;
			c3d::TextureConfiguration m_roughnessBaseConfiguration;
			c3d::TextureConfiguration m_specularBaseConfiguration;
			c3d::TextureConfiguration m_transmissionBaseConfiguration;
			c3d::TextureConfiguration m_transmittanceBaseConfiguration;
			c3d::TextureConfiguration m_thicknessBaseConfiguration;
			c3d::TextureConfiguration m_clearcoatBaseConfiguration;
			c3d::TextureConfiguration m_clearcoatNormalBaseConfiguration;
			c3d::TextureConfiguration m_clearcoatRoughnessBaseConfiguration;
			c3d::TextureConfiguration m_sheenBaseConfiguration;
			c3d::TextureConfiguration m_sheenRoughnessBaseConfiguration;
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

	AssimpMaterialImporter::AssimpMaterialImporter( c3d::Engine & engine )
		: c3d::MaterialImporter{ engine, cuT( "Assimp" ) }
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
			, !m_loadConfig.allowCompression
			, m_textureRemaps
			, *pass );
		return true;
	}

	//*********************************************************************************************
}
