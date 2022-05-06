#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/MaterialImporter.hpp"

#include "AssimpImporter/AssimpHelpers.hpp"
#include "AssimpImporter/AssimpImporter.hpp"

#include <Castor3D/Engine.hpp>
#include <Castor3D/Material/Material.hpp>
#include <Castor3D/Material/Pass/Pass.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Material/Texture/TextureUnit.hpp>
#include <Castor3D/Material/Texture/TextureLayout.hpp>
#include <Castor3D/Model/Mesh/Importer.hpp>
#include <Castor3D/Scene/Scene.hpp>

// Materials
#include <PhongPass.hpp>
#include <BlinnPhongPass.hpp>
#include <MetallicRoughnessPbrPass.hpp>
#include <SpecularGlossinessPbrPass.hpp>
#include <ToonPass.hpp>

#include <assimp/version.h>
#include <assimp/pbrmaterial.h>

namespace c3d_assimp
{
	//*********************************************************************************************

	namespace materials
	{
		static bool hasAlphaChannel( castor::Image const & image )
		{
			auto alphaChannel = castor::extractComponent( image.getPixels()
				, castor::PixelComponent::eAlpha );
			auto data = alphaChannel->getConstPtr();
			auto end = data + alphaChannel->getSize();
			return !std::all_of( data, end, []( uint8_t byte ){ return byte == 0x00; } )
				&& !std::all_of( data, end, []( uint8_t byte ){ return byte == 0xFF; } );
		}

		struct TextureInfo
		{
			castor::String name;
			aiUVTransform transform;
		};

		class PassFiller
			: public castor3d::PassVisitor
		{
		public:
			static void submit( aiMaterial const & material
				, aiScene const & scene
				, castor3d::SamplerRes sampler
				, castor3d::MeshImporter const & importer
				, std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > const & textureRemaps
				, castor3d::Pass & pass )
			{
				PassFiller vis{ material, scene, sampler, importer, textureRemaps, pass };
				pass.accept( vis );
				vis.finish();
				pass.prepareTextures();
			}

		private:
			PassFiller( aiMaterial const & material
				, aiScene const & scene
				, castor3d::SamplerRes sampler
				, castor3d::MeshImporter const & importer
				, std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > const & textureRemaps
				, castor3d::Pass & result )
				: castor3d::PassVisitor{ {} }
				, m_material{ material }
				, m_scene{ scene }
				, m_sampler{ sampler }
				, m_importer{ importer }
				, m_textureRemaps{ textureRemaps }
				, m_result{ result }
			{
			}

			void finish()
			{
				auto colInfo = getTextureInfo( castor3d::TextureFlag::eColour );
				auto emiInfo = getTextureInfo( castor3d::TextureFlag::eEmissive );
				auto occInfo = getTextureInfo( castor3d::TextureFlag::eOcclusion );
				auto hgtInfo = getTextureInfo( castor3d::TextureFlag::eHeight );
				TextureInfo nmlInfo{};
				TextureInfo opaInfo{};
				TextureInfo spcInfo{};

				if ( !colInfo.name.empty()
					&& colInfo.name.find( "_Cine_" ) != castor::String::npos
					&& colInfo.name.find( "/MI_CH_" ) != castor::String::npos )
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
				else
				{
					nmlInfo = getTextureInfo( castor3d::TextureFlag::eNormal );
					opaInfo = getTextureInfo( castor3d::TextureFlag::eOpacity );
					spcInfo = getTextureInfo( castor3d::TextureFlag::eSpecular );
				}

				if ( spcInfo.name.empty() )
				{
					spcInfo = getTextureInfo( aiTextureType_UNKNOWN, 0 );

					if ( !spcInfo.name.empty() )
					{
						castor3d::TextureConfiguration spcConfig{};
						spcConfig.roughnessMask[0] = 0x0000FF00;
						spcConfig.metalnessMask[0] = 0x000000FF;

						if ( spcInfo.name == occInfo.name )
						{
							spcConfig.occlusionMask[0] = 0x00FF0000;
							occInfo.name.clear();
						}

						m_textureRemaps.emplace( castor3d::TextureFlag::eSpecular, spcConfig );
					}
				}

				if ( opaInfo.name.empty() )
				{
					aiString value;
					if ( m_material.Get( AI_MATKEY_GLTF_ALPHAMODE, value ) == aiReturn_SUCCESS )
					{
						auto mode = makeString( value );

						if ( mode != "OPAQUE" )
						{
							auto colConfig = getRemap( castor3d::TextureFlag::eDiffuse
								, castor3d::TextureConfiguration::DiffuseTexture );
							colConfig.opacityMask[0] = 0xFF000000;
							m_textureRemaps.emplace( castor3d::TextureFlag::eDiffuse, colConfig );
							m_result.setTwoSided( true );
							m_result.setAlphaValue( 0.95f );
							m_result.setAlphaFunc( VK_COMPARE_OP_GREATER );
							m_result.setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );

							if ( mode == "BLEND" )
							{
								m_result.setAlphaBlendMode( castor3d::BlendMode::eInterpolative );
							}
						}
					}

					if ( m_result.getTransmission() != castor::RgbColour{ 1.0f, 1.0f, 1.0f }
						&& m_result.getOpacity() == 1.0f )
					{
						m_result.setOpacity( float( castor::point::length( castor::Point3f{ m_result.getTransmission().red()
							, m_result.getTransmission().green() 
							, m_result.getTransmission().blue() } ) ) );
					}
				}
				else
				{
					loadTexture( opaInfo, getRemap( castor3d::TextureFlag::eOpacity
						, castor3d::TextureConfiguration::OpacityTexture ) );
				}

				loadTexture( colInfo, getRemap( castor3d::TextureFlag::eDiffuse
					, castor3d::TextureConfiguration::DiffuseTexture ) );
				loadTexture( emiInfo, getRemap( castor3d::TextureFlag::eEmissive
					, castor3d::TextureConfiguration::EmissiveTexture ) );
				loadTexture( spcInfo, getRemap( castor3d::TextureFlag::eSpecular
					, castor3d::TextureConfiguration::SpecularTexture ) );
				loadTexture( occInfo, getRemap( castor3d::TextureFlag::eOcclusion
					, castor3d::TextureConfiguration::OcclusionTexture ) );

				// force non 0.0 opacity when an opacity map is set
				if ( !opaInfo.name.empty()
					&& m_result.getOpacity() == 0.0f )
				{
					m_result.setOpacity( 1.0f );
				}

				if ( !nmlInfo.name.empty() )
				{
					loadTexture( nmlInfo, getRemap( castor3d::TextureFlag::eNormal
						, castor3d::TextureConfiguration::NormalTexture ) );
					loadTexture( hgtInfo, getRemap( castor3d::TextureFlag::eHeight
						, castor3d::TextureConfiguration::HeightTexture ) );
				}
				else
				{
					auto texConfig = castor3d::TextureConfiguration::NormalTexture;
					convertToNormalMap( hgtInfo, texConfig );
					loadTexture( hgtInfo, getRemap( castor3d::TextureFlag::eNormal, texConfig ) );
				}

				if ( !emiInfo.name.empty() )
				{
					m_result.setEmissive( 1.0f );
				}
			}

			void visit( castor::String const & name
				, bool & value
				, bool * control )override
			{
				if ( name == "Two sided" )
				{
					int twoSided = 0;
					if ( m_material.Get( AI_MATKEY_TWOSIDED, twoSided ) == aiReturn_SUCCESS )
					{
						value = ( twoSided != 0 );
					}
				}
			}

			void visit( castor::String const & name
				, int16_t & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, uint16_t & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, int32_t & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, uint32_t & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, int64_t & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, uint64_t & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, float & value
				, bool * control )override
			{
				if ( name == "Roughness" )
				{
					if ( m_material.Get( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, value ) != aiReturn_SUCCESS )
					{
						float shininess = 0.5f;

						if ( m_material.Get( AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS_GLOSSINESS_FACTOR, shininess ) == aiReturn_SUCCESS
							|| m_material.Get( AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS, shininess ) == aiReturn_SUCCESS )
						{
							value = 1.0f - shininess;
						}
						else if ( m_material.Get( AI_MATKEY_SHININESS, shininess ) == aiReturn_SUCCESS )
						{
							float shininessStrength = 1.0f;

							if ( m_material.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength ) == aiReturn_SUCCESS )
							{
								shininess *= shininessStrength;
							}

							shininess /= 1000.0f;
							shininess = 1.0f - shininess;

							if ( shininess > 0 )
							{
								value = shininess;
							}
						}
					}
				}
				else if ( name == "Glossiness" )
				{
					if ( m_material.Get( AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS_GLOSSINESS_FACTOR, value ) != aiReturn_SUCCESS
						&& m_material.Get( AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS, value ) != aiReturn_SUCCESS )
					{
						float shininess = 0.5f;

						if ( m_material.Get( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, shininess ) == aiReturn_SUCCESS )
						{
							value = 1.0f - shininess;
						}
						else if ( m_material.Get( AI_MATKEY_SHININESS, shininess ) == aiReturn_SUCCESS )
						{
							float shininessStrength = 1.0f;

							if ( m_material.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength ) == aiReturn_SUCCESS )
							{
								shininess *= shininessStrength;
							}

							shininess /= 1000.0f;

							if ( shininess > 0 )
							{
								value = shininess;
							}
						}
					}
				}
				else if ( name == "Shininess" )
				{
					float shininess = 0.5f;

					if ( m_material.Get( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_ROUGHNESS_FACTOR, shininess ) == aiReturn_SUCCESS )
					{
						value = ( 1.0f - shininess ) * castor3d::MaxPhongShininess;
					}
					else if ( m_material.Get( AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS_GLOSSINESS_FACTOR, shininess ) == aiReturn_SUCCESS
						|| m_material.Get( AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS, shininess ) == aiReturn_SUCCESS )
					{
						value = shininess * castor3d::MaxPhongShininess;
					}
					else if ( m_material.Get( AI_MATKEY_SHININESS, shininess ) == aiReturn_SUCCESS )
					{
						float shininessStrength = 1.0f;

						if ( m_material.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength ) == aiReturn_SUCCESS )
						{
							shininess *= shininessStrength;
						}

						if ( shininess > 0 )
						{
							value = shininess;
						}
					}
				}
				else if ( name == "Metalness" )
				{
					if ( m_material.Get( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_METALLIC_FACTOR, value ) != aiReturn_SUCCESS )
					{
						aiColor4D specular = { 1, 1, 1, 1 };
						if ( m_material.Get( AI_MATKEY_COLOR_SPECULAR, specular ) == aiReturn_SUCCESS )
						{
							value = float( castor::point::length( castor::Point3f{ specular.r
								, specular.g
								, specular.b } ) );
						}
					}
				}
				else if ( name == "Opacity" )
				{
					float opacity = 1;
					if ( m_material.Get( AI_MATKEY_OPACITY, opacity ) == aiReturn_SUCCESS )
					{
						value = opacity;

						if ( opacity == 1.0f
							&& m_material.Get( AI_MATKEY_TRANSPARENCYFACTOR, opacity ) == aiReturn_SUCCESS )
						{
							value = opacity;
						}
					}
					else if ( m_material.Get( AI_MATKEY_TRANSPARENCYFACTOR, opacity ) == aiReturn_SUCCESS )
					{
						value = opacity;
					}
					else
					{
						aiColor4D colour = { 1, 1, 1, 1 };
						if ( m_material.Get( AI_MATKEY_COLOR_DIFFUSE, colour ) == aiReturn_SUCCESS )
						{
							value = colour.a;
						}
					}
				}
				else if ( name == "Emissive" )
				{
					aiColor4D emissive = { 1, 1, 1, 1 };
					if ( m_material.Get( AI_MATKEY_COLOR_EMISSIVE, emissive ) == aiReturn_SUCCESS )
					{
						value = float( castor::point::length( castor::Point3f{ emissive.r
							, emissive.g
							, emissive.b } ) );
					}
				}
				else if ( name == "Refraction ratio" )
				{
					float ior{ 1.0f };
					if ( m_material.Get( AI_MATKEY_REFRACTI, ior ) == aiReturn_SUCCESS )
					{
						value = ior;
					}
				}
				else if ( name == "Alpha ref. value" )
				{
					float ref{ 1.0f };
					if ( m_material.Get( AI_MATKEY_GLTF_ALPHACUTOFF, ref ) == aiReturn_SUCCESS )
					{
						value = ref;
					}
				}
			}

			void visit( castor::String const & name
				, double & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, castor3d::BlendMode & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, castor3d::ParallaxOcclusionMode & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, VkCompareOp & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, castor::Point3f & value
				, bool * control )override
			{
				if ( name == "Transmission" )
				{
					aiColor4D transmission = { 1, 1, 1, 1 };
					if ( m_material.Get( AI_MATKEY_COLOR_TRANSPARENT, transmission ) == aiReturn_SUCCESS )
					{
						value = { transmission.r
							, transmission.g
							, transmission.b };
					}
				}
			}

			void visit( castor::String const & name
				, castor::RgbColour & value
				, bool * control )override
			{
				if ( name == "Ambient" )
				{
					aiColor4D ambient = { 1, 1, 1, 1 };
					if ( m_material.Get( AI_MATKEY_COLOR_AMBIENT, ambient ) == aiReturn_SUCCESS )
					{
						value = castor::RgbColour::fromComponents( ambient.r
							, ambient.g
							, ambient.b );
					}
				}
				else if ( name == "Albedo" )
				{
					aiColor4D colour = { 1, 1, 1, 1 };
					if ( m_material.Get( AI_MATKEY_GLTF_PBRMETALLICROUGHNESS_BASE_COLOR_FACTOR, colour ) == aiReturn_SUCCESS )
					{
						value = castor::RgbColour::fromComponents( colour.r
							, colour.g
							, colour.b );
					}
					else if ( m_material.Get( AI_MATKEY_COLOR_DIFFUSE, colour ) == aiReturn_SUCCESS )
					{
						value = castor::RgbColour::fromComponents( colour.r
							, colour.g
							, colour.b );
					}
				}
				else if ( name == "Colour"
					|| name == "Diffuse" )
				{
					aiColor4D colour = { 1, 1, 1, 1 };
					if ( m_material.Get( AI_MATKEY_COLOR_DIFFUSE, colour ) == aiReturn_SUCCESS )
					{
						value = castor::RgbColour::fromComponents( colour.r
							, colour.g
							, colour.b );
					}
				}
				else if ( name == "Specular" )
				{
					aiColor4D specular = { 1, 1, 1, 1 };
					if ( m_material.Get( AI_MATKEY_COLOR_SPECULAR, specular ) == aiReturn_SUCCESS )
					{
						value = castor::RgbColour::fromComponents( specular.r
							, specular.g
							, specular.b );
					}
				}
			}

			void visit( castor::String const & name
				, castor::RgbaColour & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, castor::RangedValue< float > & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, castor::RangedValue< int32_t > & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, castor::RangedValue< uint32_t > & value
				, bool * control )override
			{
			}

			void visit( castor::String const & name
				, castor3d::TextureFlag textureFlag
				, castor::Point2ui & mask
				, uint32_t componentsCount
				, bool * control )override
			{
			}

		private:
			castor3d::TextureConfiguration getRemap( castor3d::TextureFlag flag
				, castor3d::TextureConfiguration texConfig )
			{
				auto it = m_textureRemaps.find( flag );

				if ( it == m_textureRemaps.end() )
				{
					return texConfig;
				}

				auto result = it->second;
				return result;
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
				, castor3d::TextureConfiguration texConfig )
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
						else
						{
							sourceInfo = std::make_unique< castor3d::TextureSourceInfo >( m_importer.loadTexture( m_sampler
								, castor::Path{ info.name }
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
								if ( !hasAlphaChannel( image ) )
								{
									texConfig.opacityMask[0] = 0x00FF0000;
								}
							}
							else if ( texConfig == castor3d::TextureConfiguration::DiffuseTexture
								&& hasAlphaChannel( image ) )
							{
								texConfig.opacityMask[0] = 0xFF000000;
								m_result.setTwoSided( true );
								m_result.setAlphaValue( 0.95f );
								m_result.setAlphaFunc( VK_COMPARE_OP_GREATER );
								m_result.setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );
								m_result.setAlphaBlendMode( castor3d::BlendMode::eInterpolative );
							}

							m_result.registerTexture( std::move( *sourceInfo )
								, { create, texConfig } );
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

			TextureInfo getTextureInfo( aiTextureType type, uint32_t index = 0u )
			{
				TextureInfo result;
				aiString name;
				m_material.Get( AI_MATKEY_TEXTURE( type, index ), name );

				if ( name.length > 0 )
				{
					result.name = makeString( name );
					m_material.Get( AI_MATKEY_UVTRANSFORM( type, index ), result.transform );
				}

				return result;
			}

			TextureInfo getTextureInfo( castor3d::TextureFlag flag )
			{
				TextureInfo result{};

				switch ( flag )
				{
				case castor3d::TextureFlag::eHeight:
					result = getTextureInfo( aiTextureType_HEIGHT );
					break;
				case castor3d::TextureFlag::eOpacity:
					result = getTextureInfo( aiTextureType_OPACITY );
					break;
				case castor3d::TextureFlag::eNormal:
					result = getTextureInfo( aiTextureType_NORMALS );
					if ( result.name == std::string{} )
					{
						static auto constexpr TextureType_NORMAL_CAMERA = aiTextureType( 13 );
						result = getTextureInfo( TextureType_NORMAL_CAMERA );
					}
					break;
				case castor3d::TextureFlag::eEmissive:
					result = getTextureInfo( aiTextureType_EMISSIVE );
					if ( result.name == std::string{} )
					{
						static auto constexpr TextureType_EMISSION_COLOR = aiTextureType( 14 );
						result = getTextureInfo( TextureType_EMISSION_COLOR );
					}
					break;
				case castor3d::TextureFlag::eOcclusion:
					if ( aiGetVersionMajor() >= 4u )
					{
						static auto constexpr TextureType_AMBIENT_OCCLUSION = aiTextureType( 17 );
						result = getTextureInfo( TextureType_AMBIENT_OCCLUSION );
						if ( result.name == std::string{} )
						{
							result = getTextureInfo( aiTextureType_LIGHTMAP );
						}
					}
					break;
				case castor3d::TextureFlag::eTransmittance:
					break;
				case castor3d::TextureFlag::eColour:
					result = getTextureInfo( aiTextureType_DIFFUSE );
					if ( result.name == std::string{} )
					{
						static auto constexpr TextureType_BASE_COLOR = aiTextureType( 12 );
						result = getTextureInfo( TextureType_BASE_COLOR );
					}
					break;
				case castor3d::TextureFlag::eSpecular:
					result = getTextureInfo( aiTextureType_SPECULAR );
					break;
				case castor3d::TextureFlag::eMetalness:
					if ( aiGetVersionMajor() >= 4u )
					{
						static auto constexpr TextureType_METALNESS = aiTextureType( 15 );
						result = getTextureInfo( TextureType_METALNESS );
					}
					if ( result.name == std::string{} )
					{
						result = getTextureInfo( aiTextureType_UNKNOWN );
					}
					if ( result.name == std::string{} )
					{
						result = getTextureInfo( aiTextureType_SPECULAR );
					}
					break;
				case castor3d::TextureFlag::eGlossiness:
					result = getTextureInfo( aiTextureType_SHININESS );
					break;
				case castor3d::TextureFlag::eRoughness:
					if ( aiGetVersionMajor() >= 4u )
					{
						static auto constexpr TextureType_DIFFUSE_ROUGHNESS = aiTextureType( 16 );
						result = getTextureInfo( TextureType_DIFFUSE_ROUGHNESS );
					}
					if ( result.name == std::string{} )
					{
						result = getTextureInfo( aiTextureType_SHININESS );
					}
					break;
				default:
					break;
				}

				return result;
			}

			void convertToNormalMap( TextureInfo & info
				, castor3d::TextureConfiguration & pconfig )
			{
				auto path = castor::Path{ info.name };

				if ( m_importer.convertToNormalMap( path, pconfig ) )
				{
					info.name = path;
				}
			}

		private:
			aiMaterial const & m_material;
			aiScene const & m_scene;
			castor3d::SamplerRes m_sampler;
			castor3d::MeshImporter const & m_importer;
			std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > m_textureRemaps;
			castor3d::Pass & m_result;
		};

		static castor3d::PassTypeID convert( castor3d::PassFactory const & factory
			, aiShadingMode shadingMode )
		{
			switch ( shadingMode )
			{
			case aiShadingMode_NoShading:
			case aiShadingMode_Minnaert:
			case aiShadingMode_OrenNayar:
			case aiShadingMode_Blinn:
			case aiShadingMode_Fresnel:
				return factory.getNameId( castor3d::BlinnPhongPass::Type );
			case aiShadingMode_Toon:
				return factory.getNameId( toon::ToonBlinnPhongPass::Type );
			case aiShadingMode::aiShadingMode_CookTorrance:
				return factory.getNameId( castor3d::MetallicRoughnessPbrPass::Type );
			case aiShadingMode_Flat:
			case aiShadingMode_Gouraud:
			case aiShadingMode_Phong:
			default:
				return factory.getNameId( castor3d::PhongPass::Type );
			}
		}

		static void processMaterialPass( castor3d::Scene & scene
			, castor3d::SamplerRes sampler
			, std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > const & textureRemaps
			, castor3d::Pass & pass
			, aiMaterial const & aiMaterial
			, aiScene const & aiScene
			, castor3d::MeshImporter & importer )
		{
			aiShadingMode shadingMode{};
			aiMaterial.Get( AI_MATKEY_SHADING_MODEL, shadingMode );
			auto & passFactory = scene.getEngine()->getPassFactory();
			auto srcType = convert( passFactory, shadingMode );
			auto dstType = pass.getTypeID();

			if ( dstType != srcType )
			{
				castor3d::log::warn << "Switching from " << passFactory.getIdName( srcType ) << " to " << passFactory.getIdName( dstType ) << " pass type." << std::endl;
			}

			PassFiller::submit( aiMaterial, aiScene, sampler, importer, textureRemaps, pass );

			if ( !pass.getTextureUnits( castor3d::TextureFlag::eOpacity ).empty()
				&& pass.getAlphaFunc() == VkCompareOp::VK_COMPARE_OP_ALWAYS )
			{
				pass.setTwoSided( true );
				pass.setAlphaBlendMode( castor3d::BlendMode::eInterpolative );
				pass.setAlphaFunc( VK_COMPARE_OP_GREATER );
				pass.setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );
				pass.setAlphaValue( 0.95f );
			}
		}
	}

	//*********************************************************************************************

	MaterialImporter::MaterialImporter( AssimpImporter & importer )
		: m_importer{ importer }
	{
	}

	void MaterialImporter::import( castor::Path const & fileName
		, std::map< castor3d::TextureFlag, castor3d::TextureConfiguration > const & textureRemaps
		, aiScene const & aiScene
		, castor3d::Scene & scene )
	{
		m_fileName = fileName;
		m_textureRemaps = textureRemaps;
		uint32_t index{};

		for ( auto aiMaterial : castor::makeArrayView( aiScene.mMaterials, aiScene.mNumMaterials ) )
		{
			castor3d::log::info << cuT( "  Material: [" ) << index << cuT( " (" ) << aiMaterial->GetName().C_Str() << cuT( ")]" ) << std::endl;
			auto material = doProcessMaterial( scene
				, aiScene
				, *aiMaterial
				, index );
			m_materials.emplace( index, material );
			++index;
		}
	}

	castor3d::MaterialResPtr MaterialImporter::doProcessMaterial( castor3d::Scene & scene
		, aiScene const & aiScene
		, aiMaterial const & aiMaterial
		, uint32_t index )
	{
		castor3d::MaterialResPtr result;
		aiString mtlname;
		castor::String name = cuT( "Imp-" );

		if ( aiMaterial.Get( AI_MATKEY_NAME, mtlname ) == aiReturn_SUCCESS )
		{
			name += makeString( mtlname );
		}
		else
		{
			name += m_fileName.getFileName() + castor::string::toString( index );
		}

		if ( scene.hasMaterial( name ) )
		{
			result = scene.findMaterial( name );
		}
		else
		{
			result = scene.addNewMaterial( name
				, *scene.getEngine()
				, scene.getPassesType() );
			auto pass = result.lock()->createPass();
			materials::processMaterialPass( scene
				, scene.getEngine()->getDefaultSampler().lock()
				, m_textureRemaps
				, *pass
				, aiMaterial
				, aiScene
				, m_importer );
		}

		return result;
	}

	//*********************************************************************************************
}
