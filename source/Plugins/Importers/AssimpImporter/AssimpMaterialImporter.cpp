#include "AssimpImporter/AssimpMaterialImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Limits.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Material/Pass/Component/Base/BlendComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/PassHeaderComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TextureCountComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TexturesComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/TwoSidedComponent.hpp>
#include <Castor3D/Material/Pass/Component/Base/UntileComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/EmissiveComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/MetalnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/RoughnessComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SpecularComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/SubsurfaceScatteringComponent.hpp>
#include <Castor3D/Material/Pass/Component/Lighting/TransmissionComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/ColourMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/EmissiveMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/GlossinessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/HeightMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/MetalnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/NormalMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OcclusionMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/OpacityMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/RoughnessMapComponent.hpp>
#include <Castor3D/Material/Pass/Component/Map/SpecularMapComponent.hpp>
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

// Materials
#include <EdgesComponent.hpp>

#pragma warning( push )
#pragma warning( disable: 4365 )
#pragma warning( disable: 4619 )
#include <assimp/material.h>
#include <assimp/version.h>
#pragma warning( pop )

#if !defined( AI_MATKEY_ROUGHNESS_FACTOR )
#	pragma warning( push )
#	pragma warning( disable: 4365 )
#	pragma warning( disable: 4619 )
#	include <assimp/pbrmaterial.h>
#	pragma warning( pop )

#	define AI_MATKEY_ROUGHNESS_FACTOR AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR
#	define AI_MATKEY_GLOSSINESS_FACTOR AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS_GLOSSINESS_FACTOR
#	define AI_MATKEY_METALLIC_FACTOR AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR
#	define AI_MATKEY_BASE_COLOR AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR
#else
#	include <assimp/GltfMaterial.h>
#endif

namespace c3d_assimp
{
	//*********************************************************************************************

	namespace materials
	{
		static constexpr aiShadingMode ShadingMode_PBR_BRDF = aiShadingMode( 0xb );

		static bool hasAlphaChannel( castor::Image const & image )
		{
			auto alphaChannel = castor::extractComponent( image.getPixels()
				, castor::PixelComponent::eAlpha );
			auto data = alphaChannel->getConstPtr();
			auto end = data + alphaChannel->getSize();
			return !std::all_of( data, end, []( uint8_t byte ){ return byte == 0x00; } )
				&& !std::all_of( data, end, []( uint8_t byte ) { return byte == 0xFF; } );
		}

		static castor::String decodeUri( castor::String uri )
		{
			castor::String escaped;

			for ( auto i = uri.begin(), nd = uri.end(); i < nd; ++i )
			{
				auto c = ( *i );

				switch ( c )
				{
				case '%':
					if ( i[1] && i[2] )
					{
						char hs[]{ i[1], i[2] };
						escaped += static_cast< char >( strtol( hs, nullptr, 16 ) );
						i += 2;
					}
					break;
				case '+':
					escaped += ' ';
					break;
				default:
					escaped += c;
				}
			}

			return escaped;
		}

		struct TextureInfo
		{
			castor::String name;
			uint32_t texcoordSet{};
			aiUVTransform transform{};
		};

		class MaterialParser
		{
		private:
			MaterialParser( aiMaterial const & material
				, aiScene const & scene
				, castor3d::SamplerRes sampler
				, AssimpMaterialImporter const & importer
				, float emissiveMult
				, std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > const & textureRemaps
				, castor3d::Pass & result )
				: m_material{ material }
				, m_scene{ scene }
				, m_sampler{ sampler }
				, m_importer{ importer }
				, m_emissiveMult{ emissiveMult }
				, m_textureRemaps{ textureRemaps }
				, m_isPbr{ detectPbr() }
				, m_result{ result }
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
				, castor3d::SamplerRes sampler
				, AssimpMaterialImporter const & importer
				, float emissiveMult
				, std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > const & textureRemaps
				, castor3d::Pass & pass )
			{
				MaterialParser parser{ material, scene, sampler, importer, emissiveMult, textureRemaps, pass };
				parser.parseDatas();
				parser.finish();
				pass.prepareTextures();
			}

		private:
			void parseDatas()
			{
				parseComponentBoolData< castor3d::TwoSidedComponent >( AI_MATKEY_TWOSIDED );

				if ( !parseComponentDataT< castor3d::RoughnessComponent, float >( AI_MATKEY_ROUGHNESS_FACTOR ) )
				{
					if ( !parseComponentDataT< castor3d::RoughnessComponent, float >( AI_MATKEY_GLOSSINESS_FACTOR
						, []( float v ){ return 1.0f - v; } ) )
					{
						parseComponentDataFactorT< castor3d::RoughnessComponent, float >( AI_MATKEY_SHININESS, AI_MATKEY_SHININESS_STRENGTH
							, []( float v ){ return 1.0f - ( v / castor3d::MaxPhongShininess ); } );
					}
				}

				parseComponentDataT< castor3d::MetalnessComponent, float >( AI_MATKEY_METALLIC_FACTOR );

				if ( !m_isPbr )
				{
					parseComponentRgbData< castor3d::SpecularComponent >( AI_MATKEY_COLOR_SPECULAR );
				}

				if ( !parseComponentHdrRgbData< castor3d::ColourComponent >( AI_MATKEY_BASE_COLOR ) )
				{
					parseComponentHdrRgbData< castor3d::ColourComponent >( AI_MATKEY_COLOR_DIFFUSE );
				}

				parseEmissive();
				parseRefractionRatio();

				bool hasOpa = parseComponentOpaDataT< castor3d::OpacityComponent >( AI_MATKEY_OPACITY );

				if ( !hasOpa )
				{
					hasOpa = parseComponentInvOpaDataT< castor3d::OpacityComponent >( AI_MATKEY_TRANSPARENCYFACTOR, 1.0f );
				}

				if ( auto opaComp = m_result.getComponent< castor3d::OpacityComponent >() )
				{
					if ( hasOpa && opaComp->getOpacity() < 1.0f )
					{
						parseComponentRgbData< castor3d::TransmissionComponent >( AI_MATKEY_COLOR_TRANSPARENT );
					}
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
				TextureInfo glsInfo{};
				TextureInfo rghInfo{};
				auto occInfo = getTextureInfo( TextureType_AMBIENT_OCCLUSION );

				if ( occInfo.name.empty() )
				{
					occInfo = getTextureInfo( aiTextureType_LIGHTMAP );
				}

				auto hgtInfo = finishHeight();
				auto emiInfo = finishEmissive();
				finishColour( colInfo, nmlInfo, opaInfo, spcInfo, mtlInfo, glsInfo, rghInfo );
				finishSpecular( spcInfo, occInfo, mtlInfo, glsInfo, rghInfo );
				auto hasOpacityTex = finishOpacity( opaInfo );

				loadTexture( colInfo, getRemap( castor3d::TextureFlag::eColour
					, castor3d::TextureConfiguration::AlbedoTexture )
					, hasOpacityTex );
				loadTexture( emiInfo, getRemap( castor3d::TextureFlag::eEmissive
					, castor3d::TextureConfiguration::EmissiveTexture )
					, hasOpacityTex );
				loadTexture( spcInfo, getRemap( castor3d::TextureFlag::eSpecular
					, castor3d::TextureConfiguration::SpecularTexture )
					, hasOpacityTex );
				loadTexture( mtlInfo, getRemap( castor3d::TextureFlag::eMetalness
					, castor3d::TextureConfiguration::MetalnessTexture )
					, hasOpacityTex );
				loadTexture( rghInfo, getRemap( castor3d::TextureFlag::eRoughness
					, castor3d::TextureConfiguration::RoughnessTexture )
					, hasOpacityTex );
				loadTexture( glsInfo, getRemap( castor3d::TextureFlag::eGlossiness
					, castor3d::TextureConfiguration::ShininessTexture )
					, hasOpacityTex );
				loadTexture( occInfo, getRemap( castor3d::TextureFlag::eOcclusion
					, castor3d::TextureConfiguration::OcclusionTexture )
					, hasOpacityTex );

				if ( !nmlInfo.name.empty() )
				{
					loadTexture( nmlInfo, getRemap( castor3d::TextureFlag::eNormal
						, castor3d::TextureConfiguration::NormalTexture )
						, hasOpacityTex );
					loadTexture( hgtInfo, getRemap( castor3d::TextureFlag::eHeight
						, castor3d::TextureConfiguration::HeightTexture )
						, hasOpacityTex );
				}
				else
				{
					auto texConfig = castor3d::TextureConfiguration::NormalTexture;
					convertToNormalMap( hgtInfo, texConfig );
					loadTexture( hgtInfo
						, getRemap( castor3d::TextureFlag::eNormal, texConfig )
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
					*data = castor::HdrRgbColour::fromComponents( value.r
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
					*data = castor::RgbColour::fromComponents( value.r
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
					*data = float( castor::point::length( castor::Point3f{ value.r
						, value.g
						, value.b } ) );
					component->setData( *data );
				}

				return result;
			}

			void parseEmissive()
			{
				aiColor3D emissive = { 1, 1, 1 };

				if ( m_material.Get( AI_MATKEY_COLOR_EMISSIVE, emissive ) == aiReturn_SUCCESS )
				{
					auto component = m_result.createComponent< castor3d::EmissiveComponent >();
					component->setEmissiveFactor( m_emissiveMult * float( castor::point::length( castor::Point3f{ emissive.r
						, emissive.g
						, emissive.b } ) ) );
				}
			}

			void parseAlphaRefValue()
			{
				float ref{ 1.0f };

				if ( m_material.Get( AI_MATKEY_GLTF_ALPHACUTOFF, ref ) == aiReturn_SUCCESS )
				{
					auto component = m_result.createComponent< castor3d::AlphaTestComponent >();
					component->setAlphaRefValue( ref );
				}
			}

			void parseRefractionRatio()
			{
				float ior{ 1.0f };
				if ( m_material.Get( AI_MATKEY_REFRACTI, ior ) == aiReturn_SUCCESS )
				{
					auto component = m_result.createComponent< castor3d::RefractionComponent >();
					component->setRefractionRatio( ior );
				}
			}

			castor3d::TextureConfiguration getRemap( castor3d::TextureFlag flag
				, castor3d::TextureConfiguration texConfig )
			{
				auto it = m_textureRemaps.find( flag );

				if ( it == m_textureRemaps.end() )
				{
					return texConfig;
				}

				return it->second;
			}

			castor::Image const & loadImage( castor3d::TextureSourceInfo const & source )
			{
				castor::ImageSPtr result;

				if ( source.isBufferImage() )
				{
					result = m_importer.loadImage( source.name()
						, castor::ImageCreateParams{ source.type()
						, source.buffer()
						, { false, false, false } } );
				}
				else if ( source.isFileImage() )
				{
					result = m_importer.loadImage( source.name()
						, castor::ImageCreateParams{ source.folder() / source.relative()
						, { false, false, false } } );
				}

				if ( !result )
				{
					CU_LoaderError( "Couldn't load image" + source.name() + "." );
				}

				return *result;
			}

			void loadTexture( TextureInfo const & info
				, castor3d::TextureConfiguration texConfig
				, bool hasOpacity )
			{
				if ( !info.name.empty() )
				{
					try
					{
						std::unique_ptr< castor3d::TextureSourceInfo > sourceInfo;

						if ( info.name[0] == '*' )
						{
							auto id = uint32_t( castor::string::toInt( info.name.substr( 1u ) ) );

							if ( id < m_scene.mNumTextures )
							{
								auto texture = m_scene.mTextures[id];
								castor::ByteArray data;
								data.resize( texture->mWidth );
								std::memcpy( data.data(), texture->pcData, data.size() );
								sourceInfo = std::make_unique< castor3d::TextureSourceInfo >( m_importer.loadTexture( m_sampler
									, "Image" + castor::string::toString( id )
									, texture->achFormatHint
									, std::move( data )
									, texConfig ) );
							}
						}
						else if ( auto texture = m_scene.GetEmbeddedTexture( info.name.c_str() ) )
						{
							castor::ByteArray data;
							data.resize( texture->mWidth );
							std::memcpy( data.data(), texture->pcData, data.size() );
							sourceInfo = std::make_unique< castor3d::TextureSourceInfo >( m_importer.loadTexture( m_sampler
								, info.name
								, texture->achFormatHint
								, std::move( data )
								, texConfig ) );
						}
						else
						{
							auto name = decodeUri( info.name );
							sourceInfo = std::make_unique< castor3d::TextureSourceInfo >( m_importer.loadTexture( m_sampler
								, castor::Path{ name }
								, texConfig ) );
						}

						if ( sourceInfo )
						{
							auto & image = loadImage( *sourceInfo );
							auto layout = image.getLayout();
							texConfig.transform = castor3d::TextureTransform{ { info.transform.mTranslation.x, info.transform.mTranslation.y, 0.0f }
								, castor::Angle::fromRadians( info.transform.mRotation )
								, { info.transform.mScaling.x, info.transform.mScaling.y, 1.0f } };
							auto create = ashes::ImageCreateInfo{ 0u
								, VkImageType( layout.type )
								, VkFormat( layout.format )
								, { layout.extent->x, layout.extent->y, layout.extent->z }
								, layout.levels
								, layout.layers
								, VK_SAMPLE_COUNT_1_BIT
								, VK_IMAGE_TILING_OPTIMAL
								, VK_IMAGE_USAGE_SAMPLED_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT };

							if ( texConfig.opacityMask[0] && getFlags( texConfig ) == castor3d::TextureFlag::eOpacity )
							{
								mixedInterpolative( true );

								if ( !hasAlphaChannel( image ) )
								{
									texConfig.opacityMask[0] = 0x00FF0000;
								}
							}
							else if ( !hasOpacity
								&& ( texConfig == castor3d::TextureConfiguration::AlbedoTexture
									|| texConfig == castor3d::TextureConfiguration::DiffuseTexture )
								&& hasAlphaChannel( image ) )
							{
								mixedInterpolative( true );
								texConfig.opacityMask[0] = 0xFF000000;
							}

							m_result.registerTexture( std::move( *sourceInfo )
								, { create, texConfig, info.texcoordSet } );
						}
					}
					catch ( std::exception & )
					{
						m_importer.loadTexture( m_sampler
							, castor::Path{ info.name }
							, { { {} }, texConfig }
						, m_result );
					}
				}
			}

			static auto constexpr TextureType_BASE_COLOR = aiTextureType( 12 );
			static auto constexpr TextureType_NORMAL_CAMERA = aiTextureType( 13 );
			static auto constexpr TextureType_EMISSION_COLOR = aiTextureType( 14 );
			static auto constexpr TextureType_METALNESS = aiTextureType( 15 );
			static auto constexpr TextureType_DIFFUSE_ROUGHNESS = aiTextureType( 16 );
			static auto constexpr TextureType_AMBIENT_OCCLUSION = aiTextureType( 17 );
			static auto constexpr TextureType_SHEEN = aiTextureType( 19 );
			static auto constexpr TextureType_CLEARCOAT = aiTextureType( 20 );
			static auto constexpr TextureType_TRANSMISSION = aiTextureType( 21 );

			TextureInfo getTextureInfo( aiTextureType type )
			{
				TextureInfo result;
				aiString name;
				uint32_t index{};

				while ( name.length == 0 && index < castor3d::MaxTextureCoordinatesSets )
				{
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

					++index;
				}

				if ( !result.name.empty() )
				{
					switch ( type )
					{
					case aiTextureType_REFLECTION:
						m_result.createComponent< castor3d::ReflectionComponent >()->enableReflections();
						break;
					default:
						break;
					}
				}

				return result;
			}

			void convertToNormalMap( TextureInfo & info
				, castor3d::TextureConfiguration & pconfig )
			{
				auto path = castor::Path{ info.name };

				if ( m_importer.convertToNormalMap( path, pconfig ) )
				{
					m_result.createComponent< castor3d::NormalMapComponent >();
					info.name = path;
				}
			}

			void mixedInterpolative( bool blending )
			{
				m_result.createComponent< castor3d::OpacityComponent >();

				auto twoSided = m_result.createComponent< castor3d::TwoSidedComponent >();
				twoSided->setTwoSided( true );

				auto alphaTest = m_result.createComponent< castor3d::AlphaTestComponent >();
				alphaTest->setAlphaRefValue( 0.95f );
				alphaTest->setAlphaFunc( VK_COMPARE_OP_GREATER );
				alphaTest->setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );

				if ( blending )
				{
					auto blend = m_result.createComponent< castor3d::BlendComponent >();
					blend->setAlphaBlendMode( castor3d::BlendMode::eInterpolative );
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
						&& ( m_shadingModel == aiShadingMode_PBR_BRDF || m_shadingModel == aiShadingMode_CookTorrance ) )
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
				, TextureInfo & glsInfo
				, TextureInfo & rghInfo )
			{
				colInfo = getTextureInfo( TextureType_BASE_COLOR );
				bool isCollada{};

				if ( colInfo.name.empty() )
				{
					colInfo = getTextureInfo( aiTextureType_DIFFUSE );
					isCollada = !colInfo.name.empty()
						&& colInfo.name.find( "_Cine_" ) != castor::String::npos
						&& colInfo.name.find( "/MI_CH_" ) != castor::String::npos;

					if ( isCollada )
					{
						// Workaround for Collada textures.
						castor::String strGlob = colInfo.name + cuT( ".tga" );
						castor::string::replace( strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
						castor::String strDiff = strGlob;
						castor::String strNorm = strGlob;
						castor::String strSpec = strGlob;
						castor::String strOpac = strGlob;
						colInfo.name = castor::string::replace( strDiff, cuT( "_Cine_" ), cuT( "_D_" ) );
						nmlInfo.name = castor::string::replace( strNorm, cuT( "_Cine_" ), cuT( "_N_" ) );
						spcInfo.name = castor::string::replace( strSpec, cuT( "_Cine_" ), cuT( "_S_" ) );
						opaInfo.name = castor::string::replace( strOpac, cuT( "_Cine_" ), cuT( "_A_" ) );
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
						nmlInfo = getTextureInfo( aiTextureType_NORMAL_CAMERA );
					}

					opaInfo = getTextureInfo( aiTextureType_OPACITY );
					spcInfo = getTextureInfo( aiTextureType_SPECULAR );
					glsInfo = getTextureInfo( aiTextureType_SHININESS );
					mtlInfo = getTextureInfo( TextureType_METALNESS );
					rghInfo = getTextureInfo( TextureType_DIFFUSE_ROUGHNESS );
				}
			}

			void finishSpecular( TextureInfo & spcInfo
				, TextureInfo & occInfo
				, TextureInfo & mtlInfo
				, TextureInfo & glsInfo
				, TextureInfo & rghInfo )
			{
				if ( spcInfo.name.empty() )
				{
					spcInfo = getTextureInfo( aiTextureType_UNKNOWN );

					if ( !spcInfo.name.empty() )
					{
						auto spcConfig{ getRemap( castor3d::TextureFlag::eSpecular
							, castor3d::TextureConfiguration{} ) };

						if ( !spcConfig.metalnessMask[0] )
						{
							spcConfig.metalnessMask[0] = 0x000000FF;
						}

						if ( !spcConfig.roughnessMask[0] )
						{
							spcConfig.roughnessMask[0] = 0x0000FF00;
						}

						if ( spcInfo.name == occInfo.name )
						{
							if ( !spcConfig.occlusionMask[0] )
							{
								spcConfig.occlusionMask[0] = 0x00FF0000;
							}

							occInfo.name.clear();
						}

						if ( spcInfo.name == mtlInfo.name )
						{
							mtlInfo.name.clear();
						}

						if ( spcInfo.name == glsInfo.name )
						{
							glsInfo.name.clear();
						}

						if ( spcInfo.name == rghInfo.name )
						{
							rghInfo.name.clear();
						}

						m_textureRemaps.emplace( castor3d::TextureFlag::eSpecular, spcConfig );
					}
				}
			}

			bool finishOpacity( TextureInfo const & opaInfo )
			{
				auto opacity = m_result.getComponent< castor3d::OpacityComponent >();
				bool hasOpacityTex = !opaInfo.name.empty();

				if ( hasOpacityTex )
				{
					loadTexture( opaInfo, getRemap( castor3d::TextureFlag::eOpacity
						, castor3d::TextureConfiguration::OpacityTexture )
						, hasOpacityTex );
				}
				else
				{
					aiString value;

					if ( m_material.Get( AI_MATKEY_GLTF_ALPHAMODE, value ) == aiReturn_SUCCESS )
					{
						auto mode = makeString( value );

						if ( mode != "OPAQUE" )
						{
							auto config = getRemap( castor3d::TextureFlag::eColour
								, castor3d::TextureConfiguration::ColourTexture );
							config.opacityMask[0] = 0xFF000000;
							m_textureRemaps.emplace( castor3d::TextureFlag::eColour, config );
							hasOpacityTex = true;
							mixedInterpolative( mode == "BLEND" );
						}
					}

					auto transmission = m_result.getComponent< castor3d::TransmissionComponent >();

					if ( ( transmission && transmission->getTransmission() != castor::RgbColour{ 1.0f, 1.0f, 1.0f } )
						&& ( !opacity || opacity->getOpacity() == 1.0f ) )
					{
						if ( !opacity )
						{
							opacity = m_result.createComponent< castor3d::OpacityComponent >();
						}

						opacity->setOpacity( float( castor::point::length( castor::Point3f{ transmission->getTransmission().red()
							, transmission->getTransmission().green()
							, transmission->getTransmission().blue() } ) ) );
					}
				}

				if ( hasOpacityTex && !opacity )
				{
					opacity = m_result.createComponent< castor3d::OpacityComponent >();
				}

				// force non 0.0 opacity when an opacity map is set
				if ( hasOpacityTex && opacity->getOpacity() == 0.0f )
				{
					opacity->setOpacity( 1.0f );
				}

				if ( hasOpacityTex
					&& m_result.getAlphaFunc() == VkCompareOp::VK_COMPARE_OP_ALWAYS )
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
			castor3d::SamplerRes m_sampler;
			AssimpMaterialImporter const & m_importer;
			float m_emissiveMult;
			std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > m_textureRemaps;
			aiShadingMode m_shadingModel{};
			bool m_isPbr;
			castor3d::Pass & m_result;
		};
	}

	//*********************************************************************************************

	AssimpMaterialImporter::AssimpMaterialImporter( castor3d::Engine & engine )
		: castor3d::MaterialImporter{ engine }
	{
	}

	bool AssimpMaterialImporter::doImportMaterial( castor3d::Material & material )
	{
		auto & file = static_cast< AssimpImporterFile const & >( *m_file );
		auto name = material.getName();
		auto it = file.getMaterials().find( name );

		if ( it == file.getMaterials().end() )
		{
			return false;
		}

		float emissiveMult = 1.0f;
		float value;

		if ( m_parameters.get( cuT( "emissive_mult" ), value )
			&& std::abs( value - 1.0f ) > std::numeric_limits< float >::epsilon() )
		{
			emissiveMult = value;
		}

		castor3d::log::info << cuT( "  Material found: [" ) << name << cuT( "]" ) << std::endl;
		auto pass = material.createPass();
		materials::MaterialParser::parse( *it->second 
			, file.getAiScene()
			, getEngine()->getDefaultSampler().lock()
			, *this
			, emissiveMult
			, m_textureRemaps
			, *pass );
		return true;
	}

	//*********************************************************************************************
}
