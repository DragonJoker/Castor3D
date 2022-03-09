#pragma GCC diagnostic ignored "-Woverloaded-virtual"
#pragma GCC diagnostic ignored "-Werror=overloaded-virtual"

#include "AssimpImporter/AssimpImporter.hpp"

#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Event/Frame/GpuFunctorEvent.hpp>
#include <Castor3D/Material/Pass/PassFactory.hpp>
#include <Castor3D/Material/Pass/PassVisitor.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimation.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationSubmesh.hpp>
#include <Castor3D/Model/Skeleton/Bone.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Animation/AnimatedObjectGroup.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimation.hpp>
#include <Castor3D/Scene/Animation/SceneNodeAnimationKeyFrame.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>
#include <Castor3D/Scene/Light/DirectionalLight.hpp>
#include <Castor3D/Scene/Light/Light.hpp>
#include <Castor3D/Scene/Light/PointLight.hpp>
#include <Castor3D/Scene/Light/SpotLight.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Log/Logger.hpp>

#include <assimp/version.h>
#include <assimp/pbrmaterial.h>

// Materials
#include <PhongPass.hpp>
#include <BlinnPhongPass.hpp>
#include <MetallicRoughnessPbrPass.hpp>
#include <SpecularGlossinessPbrPass.hpp>
#include <ToonPass.hpp>

using namespace castor3d;

namespace C3dAssimp
{
	//*********************************************************************************************

	namespace
	{
		aiNodeAnim const * findNodeAnim( const aiAnimation & animation
			, const castor::String & nodeName )
		{
			aiNodeAnim const * result = nullptr;
			auto it = std::find_if( animation.mChannels
				, animation.mChannels + animation.mNumChannels
				, [&nodeName]( aiNodeAnim const * const p_nodeAnim )
			{
				return castor::string::stringCast< xchar >( p_nodeAnim->mNodeName.data ) == nodeName;
			} );

			if ( it != animation.mChannels + animation.mNumChannels )
			{
				result = *it;
			}

			return result;
		}

		template< typename aiMeshType >
		InterleavedVertexArray createVertexBuffer( aiMeshType const & aiMesh )
		{
			InterleavedVertexArray vertices{ aiMesh.mNumVertices };
			uint32_t index{ 0u };

			for ( auto & vertex : vertices )
			{
				vertex.pos[0] = float( aiMesh.mVertices[index].x );
				vertex.pos[1] = float( aiMesh.mVertices[index].y );
				vertex.pos[2] = float( aiMesh.mVertices[index].z );
				++index;
			}

			if ( aiMesh.HasNormals() )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.nml[0] = float( aiMesh.mNormals[index].x );
					vertex.nml[1] = float( aiMesh.mNormals[index].y );
					vertex.nml[2] = float( aiMesh.mNormals[index].z );
					++index;
				}
			}

			if ( aiMesh.HasTangentsAndBitangents() )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.tan[0] = float( aiMesh.mTangents[index].x );
					vertex.tan[1] = float( aiMesh.mTangents[index].y );
					vertex.tan[2] = float( aiMesh.mTangents[index].z );
					++index;
				}
			}

			if ( aiMesh.HasTextureCoords( 0 ) )
			{
				index = 0u;

				for ( auto & vertex : vertices )
				{
					vertex.tex[0] = float( aiMesh.mTextureCoords[0][index].x );
					vertex.tex[1] = float( aiMesh.mTextureCoords[0][index].y );
					vertex.tex[2] = float( aiMesh.mTextureCoords[0][index].z );
					++index;
				}
			}

			return vertices;
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
				, std::map< TextureFlag, TextureConfiguration > const & textureRemaps
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
				, std::map< TextureFlag, TextureConfiguration > const & textureRemaps
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

						m_textureRemaps.emplace( TextureFlag::eSpecular, spcConfig );
					}
				}

				if ( opaInfo.name.empty() )
				{
					aiString value;
					if ( m_material.Get( AI_MATKEY_GLTF_ALPHAMODE, value ) == aiReturn_SUCCESS )
					{
						std::string mode = value.C_Str();

						if ( mode != "OPAQUE" )
						{
							auto colConfig = getRemap( TextureFlag::eDiffuse, TextureConfiguration::DiffuseTexture );
							colConfig.opacityMask[0] = 0xFF000000;
							m_textureRemaps.emplace( TextureFlag::eDiffuse, colConfig );
							m_result.setTwoSided( true );
							m_result.setAlphaFunc( VK_COMPARE_OP_GREATER );
							m_result.setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );

							if ( mode == "BLEND" )
							{
								m_result.setTwoSided( true );
								m_result.setAlphaValue( 0.95f );
								m_result.setAlphaFunc( VK_COMPARE_OP_GREATER );
								m_result.setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );
								m_result.setAlphaBlendMode( castor3d::BlendMode::eInterpolative );
							}
						}
					}
				}

				loadTexture( colInfo, getRemap( TextureFlag::eDiffuse, TextureConfiguration::DiffuseTexture ) );
				loadTexture( emiInfo, getRemap( TextureFlag::eEmissive, TextureConfiguration::EmissiveTexture ) );
				loadTexture( spcInfo, getRemap( TextureFlag::eSpecular, TextureConfiguration::SpecularTexture ) );
				loadTexture( occInfo, getRemap( TextureFlag::eOcclusion, TextureConfiguration::OcclusionTexture ) );

				// force non 0.0 opacity when an opacity map is set
				if ( !opaInfo.name.empty()
					&& m_result.getOpacity() == 0.0f )
				{
					loadTexture( opaInfo, getRemap( TextureFlag::eOpacity, TextureConfiguration::OpacityTexture ) );
					m_result.setOpacity( 1.0f );
				}

				if ( !nmlInfo.name.empty() )
				{
					loadTexture( nmlInfo, getRemap( TextureFlag::eNormal, TextureConfiguration::NormalTexture ) );
					loadTexture( hgtInfo, getRemap( TextureFlag::eHeight, TextureConfiguration::HeightTexture ) );
				}
				else
				{
					auto texConfig = TextureConfiguration::NormalTexture;
					convertToNormalMap( hgtInfo, texConfig );
					loadTexture( hgtInfo, getRemap( TextureFlag::eNormal, texConfig ) );
				}

				if ( !emiInfo.name.empty() )
				{
					m_result.setEmissive( 1.0f );
				}

				m_result.setTwoSided( true );
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
						value = ( 1.0f - shininess ) * PhongPass::MaxShininess;
					}
					else if ( m_material.Get( AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS_GLOSSINESS_FACTOR, shininess ) == aiReturn_SUCCESS
						|| m_material.Get( AI_MATKEY_GLTF_PBRSPECULARGLOSSINESS, shininess ) == aiReturn_SUCCESS )
					{
						value = shininess * PhongPass::MaxShininess;
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

			void loadTexture( TextureInfo const & info
				, castor3d::TextureConfiguration texConfig )
			{
				if ( !info.name.empty() )
				{
					auto & loader = m_result.getOwner()->getEngine()->getImageLoader();

					try
					{
						if ( info.name[0] == '*' )
						{
							auto id = uint32_t( castor::string::toInt( info.name.substr( 1u ) ) );

							if ( id < m_scene.mNumTextures )
							{
								auto texture = m_scene.mTextures[id];
								castor::ByteArray data;
								data.resize( texture->mWidth );
								std::memcpy( data.data(), texture->pcData, data.size() );
								auto sourceInfo = m_importer.loadTexture( m_sampler
									, "Image" + castor::string::toString( id )
									, texture->achFormatHint
									, std::move( data )
									, texConfig );
								auto layout = loader.load( info.name
									, sourceInfo.type()
									, sourceInfo.buffer().data()
									, uint32_t( sourceInfo.buffer().size() )
									, { false, false, false } ).getLayout();
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
								m_result.registerTexture( std::move( sourceInfo )
									, { create, texConfig } );
							}
						}
						else
						{
							auto path = castor::Path{ info.name };
							auto sourceInfo = m_importer.loadTexture( m_sampler, path, texConfig );
							auto layout = loader.load( path.getFileName(), sourceInfo.folder() / sourceInfo.relative(), { false, false, false } ).getLayout();
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
							m_result.registerTexture( std::move( sourceInfo )
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
					result.name = castor::string::stringCast< xchar >( name.C_Str() );
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
			std::map< TextureFlag, TextureConfiguration > m_textureRemaps;
			castor3d::Pass & m_result;
		};

		PassTypeID convert( PassFactory const & factory
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

		castor::Matrix4x4f makeMatrix4x4f( aiMatrix4x4 const & aiMatrix )
		{
			std::array< float, 16u > data
				{ aiMatrix.a1, aiMatrix.b1, aiMatrix.c1, aiMatrix.d1
				, aiMatrix.a2, aiMatrix.b2, aiMatrix.c2, aiMatrix.d2
				, aiMatrix.a3, aiMatrix.b3, aiMatrix.c3, aiMatrix.d3
				, aiMatrix.a4, aiMatrix.b4, aiMatrix.c4, aiMatrix.d4 };
			return castor::Matrix4x4f{ data.data() };
		}

		castor::Matrix4x4f makeMatrix4x4f( aiMatrix3x3 const & aiMatrix )
		{
			std::array< float, 9u > data
				{ aiMatrix.a1, aiMatrix.b1, aiMatrix.c1
				, aiMatrix.a2, aiMatrix.b2, aiMatrix.c2
				, aiMatrix.a3, aiMatrix.b3, aiMatrix.c3 };
			return castor::Matrix4x4f{ castor::Matrix3x3f{ data.data() } };
		}

		castor::Matrix4x4f makeMatrix4x4f( castor::Point3f const & direction
			, castor::Point3f const & up )
		{
			castor::Matrix4x4f result;
			result.setIdentity();
			castor::Point3f xaxis = castor::point::getNormalised( castor::point::cross( up, direction ) );
			castor::Point3f yaxis = castor::point::getNormalised( castor::point::cross( direction, xaxis ) );

			result.setColumn( 0u, { xaxis->x, yaxis->x, direction->x, 0.0f } );
			result.setColumn( 1u, { xaxis->y, yaxis->y, direction->y, 0.0f } );
			result.setColumn( 2u, { xaxis->z, yaxis->z, direction->z, 0.0f } );

			return result;
		}

		void processMaterialPass( Scene & scene
			, SamplerRes sampler
			, std::map< TextureFlag, TextureConfiguration > const & textureRemaps
			, Pass & pass
			, aiMaterial const & aiMaterial
			, aiScene const & aiScene
			, AssimpImporter & importer )
		{
			aiShadingMode shadingMode{};
			aiMaterial.Get( AI_MATKEY_SHADING_MODEL, shadingMode );
			auto & passFactory = importer.getEngine()->getPassFactory();
			auto srcType = convert( passFactory, shadingMode );
			auto dstType = pass.getTypeID();

			if ( dstType != srcType )
			{
				log::warn << "Switching from " << passFactory.getIdName( srcType ) << " to " << passFactory.getIdName( dstType ) << " pass type." << std::endl;
			}

			PassFiller::submit( aiMaterial, aiScene, sampler, importer, textureRemaps, pass );

			if ( !pass.getTextureUnits( TextureFlag::eOpacity ).empty()
				&& pass.getAlphaFunc() == VkCompareOp::VK_COMPARE_OP_ALWAYS )
			{
				pass.setTwoSided( true );
				pass.setAlphaBlendMode( BlendMode::eInterpolative );
				pass.setAlphaFunc( VK_COMPARE_OP_GREATER );
				pass.setBlendAlphaFunc( VK_COMPARE_OP_LESS_OR_EQUAL );
				pass.setAlphaValue( 0.95f );
			}
		}

		castor::Milliseconds convert( double ticks
			, int64_t ticksPerSecond )
		{
			// Turn ticks to seconds.
			auto time = ticks / double( ticksPerSecond );
			// Turn seconds to milliseconds.
			return castor::Milliseconds{ int64_t( time * 1000.0 ) };
		}

		castor::Point3f convert( aiVector3D const & v )
		{
			return castor::Point3f{ v.x, v.y, v.z };
		}

		castor::Quaternion convert( aiQuaternion const & v )
		{
			return castor::Quaternion::fromMatrix( makeMatrix4x4f( v.GetMatrix() ) );
		}

		template< typename KeyT >
		struct KeyDataTyperT;

		template<>
		struct KeyDataTyperT< aiVectorKey >
		{
			using Type = castor::Point3f;
		};

		template<>
		struct KeyDataTyperT< aiQuatKey >
		{
			using Type = castor::Quaternion;
		};

		template< typename KeyT >
		using KeyDataTypeT = typename KeyDataTyperT< KeyT >::Type;

		template< typename KeyT >
		std::map< castor::Milliseconds, KeyDataTypeT< KeyT > > processKeys( castor::ArrayView< KeyT > const & keys
			, int64_t ticksPerSecond
			, std::set< castor::Milliseconds > & times )
		{
			std::map< castor::Milliseconds, KeyDataTypeT< KeyT > > result;

			for ( auto const & key : keys )
			{
				if ( key.mTime >= 0 )
				{
					auto time = convert( key.mTime, ticksPerSecond );
					times.insert( time );
					result.emplace( time, convert( key.mValue ) );
				}
			}

			return result;
		}

		template< typename KeyFrameT, typename AnimationT >
		KeyFrameT & getKeyFrame( castor::Milliseconds const & time
			, AnimationT & animation
			, std::map< castor::Milliseconds, std::unique_ptr< KeyFrameT > > & keyframes )
		{
			auto it = keyframes.find( time );

			if ( it == keyframes.end() )
			{
				it = keyframes.emplace( time
					, std::make_unique< KeyFrameT >( animation, time ) ).first;
			}

			return *it->second;
		}

		template< typename T >
		void findValue( castor::Milliseconds time
			, typename std::map< castor::Milliseconds, T > const & map
			, typename std::map< castor::Milliseconds, T >::const_iterator & prv
			, typename std::map< castor::Milliseconds, T >::const_iterator & cur )
		{
			if ( map.empty() )
			{
				prv = map.end();
				cur = map.end();
			}
			else
			{
				cur = std::find_if( map.begin()
					, map.end()
					, [&time]( std::pair< castor::Milliseconds, T > const & pair )
					{
						return pair.first > time;
					} );

				if ( cur == map.end() )
				{
					--cur;
				}

				prv = cur;

				if ( prv != map.begin() )
				{
					prv--;
				}
			}
		}

		template< typename T >
		T interpolate( castor::Milliseconds const & time
			, Interpolator< T > const & interpolator
			, std::map< castor::Milliseconds, T > const & values )
		{
			T result;

			if ( values.size() == 1 )
			{
				result = values.begin()->second;
			}
			else
			{
				auto prv = values.begin();
				auto cur = values.begin();
				findValue( time, values, prv, cur );

				if ( prv != cur )
				{
					auto dt = cur->first - prv->first;
					float factor = float( ( time - prv->first ).count() ) / float( dt.count() );
					result = interpolator.interpolate( prv->second, cur->second, factor );
				}
				else
				{
					result = prv->second;
				}
			}

			return result;
		}

		template< typename AnimationT, typename KeyFrameT, typename FuncT >
		void synchroniseKeys( std::map< castor::Milliseconds, castor::Point3f > const & translates
			, std::map< castor::Milliseconds, castor::Point3f > const & scales
			, std::map< castor::Milliseconds, castor::Quaternion > const & rotates
			, std::set< castor::Milliseconds > const & times
			, uint32_t fps
			, int64_t ticksPerSecond
			, AnimationT & animation
			, std::map< castor::Milliseconds, std::unique_ptr< KeyFrameT > > & keyframes
			, FuncT fillKeyFrame )
		{
			InterpolatorT< castor::Point3f, InterpolatorType::eLinear > pointInterpolator;
			InterpolatorT< castor::Quaternion, InterpolatorType::eLinear > quatInterpolator;
			// Limit the key frames per second to 60, to spare RAM...
			auto wantedFps = std::min< int64_t >( 60, int64_t( fps ) );
			castor::Milliseconds step{ 1000 / wantedFps };
			castor::Milliseconds maxTime{ *times.rbegin() };

			for ( auto time = 0_ms; time <= maxTime; time += step )
			{
				auto translate = interpolate( time, pointInterpolator, translates );
				auto scale = interpolate( time, pointInterpolator, scales );
				auto rotate = interpolate( time, quatInterpolator, rotates );
				fillKeyFrame( getKeyFrame( time, animation, keyframes )
					, translate
					, rotate
					, scale );
			}
		}

		template< typename aiAnimT
			, typename AnimationT
			, typename KeyFrameT
			, typename FuncT >
		void processAnimationNodeKeys( aiAnimT const & aiAnim
			, uint32_t wantedFps
			, int64_t ticksPerSecond
			, AnimationT & animation
			, std::map< castor::Milliseconds, std::unique_ptr< KeyFrameT > > & keyframes
			, FuncT fillKeyFrame )
		{
			std::set< castor::Milliseconds > times;
			auto translates = processKeys( castor::makeArrayView( aiAnim.mPositionKeys
					, aiAnim.mNumPositionKeys )
				, ticksPerSecond
				, times );
			auto scales = processKeys( castor::makeArrayView( aiAnim.mScalingKeys
					, aiAnim.mNumScalingKeys )
				, ticksPerSecond
				, times );
			auto rotates = processKeys( castor::makeArrayView( aiAnim.mRotationKeys
					, aiAnim.mNumRotationKeys )
				, ticksPerSecond
				, times );
			synchroniseKeys( translates
				, scales
				, rotates
				, times
				, wantedFps
				, ticksPerSecond
				, animation
				, keyframes
				, fillKeyFrame );
		}

		bool operator==( castor::Point3f const & lhs
			, castor::Point3f const & rhs )
		{
			return castor::point::distance( lhs, rhs ) < 0.0001;
		}

		bool operator==( castor3d::InterleavedVertex const & lhs
			, castor3d::InterleavedVertex const & rhs )
		{
			return lhs.pos == rhs.pos
				&& lhs.nml == rhs.nml
				&& lhs.tan == rhs.tan
				&& lhs.tex == rhs.tex;
		}

		bool operator==( castor3d::Submesh const & lhs
			, castor3d::Submesh const & rhs )
		{
			if ( lhs.getPointsCount() != rhs.getPointsCount()
				|| lhs.getIndexMapping()->getComponentsCount() != rhs.getIndexMapping()->getComponentsCount()
				|| lhs.getIndexMapping()->getCount() != rhs.getIndexMapping()->getCount() )
			{
				return false;
			}

			auto lhsIt = lhs.getPoints().begin();
			auto rhsIt = rhs.getPoints().begin();
			bool result = true;

			while ( result && lhsIt != lhs.getPoints().end() )
			{
				result = *lhsIt == *rhsIt;
				++lhsIt;
				++rhsIt;
			}

			return result;
		}

		bool operator==( castor3d::Mesh const & lhs
			, castor3d::Mesh const & rhs )
		{
			if ( lhs.getName() == rhs.getName() )
			{
				return true;
			}

			if ( lhs.getSubmeshCount() != rhs.getSubmeshCount() )
			{
				return false;
			}

			auto lhsIt = lhs.begin();
			auto rhsIt = rhs.begin();
			bool result = true;

			while ( result && lhsIt != lhs.end() )
			{
				auto & lhsSubmesh = **lhsIt;
				auto & rhsSubmesh = **rhsIt;
				result = lhsSubmesh == rhsSubmesh;
				++lhsIt;
				++rhsIt;
			}

			return result;
		}
	}

	//*********************************************************************************************

	castor::String const AssimpImporter::Name = cuT( "ASSIMP Importer" );

	AssimpImporter::AssimpImporter( Engine & engine )
		: MeshImporter( engine )
		, m_anonymous( 0 )
	{
	}

	MeshImporterUPtr AssimpImporter::create( Engine & engine )
	{
		return std::make_unique< AssimpImporter >( engine );
	}

	bool AssimpImporter::doImportMesh( Mesh & mesh )
	{
		bool result{ false };
		m_mapBoneByID.clear();
		m_arrayBones.clear();

		if ( auto aiScene = doLoadScene() )
		{
			if ( aiScene->HasMeshes() )
			{
				std::vector< aiMesh * > aiMeshes;

				for ( auto aiMesh : castor::makeArrayView( aiScene->mMeshes, aiScene->mNumMeshes ) )
				{
					aiMeshes.push_back( aiMesh );
				}

				result = doProcessMeshAndAnims( *aiScene
					, aiMeshes
					, mesh );
			}

			m_importer.FreeScene();
		}

		return result;
	}

	bool AssimpImporter::doImportScene( Scene & scene )
	{
		bool result{ false };

		if ( auto aiScene = doLoadScene() )
		{
			for ( aiLight * aiLight : castor::makeArrayView( aiScene->mLights, aiScene->mNumLights ) )
			{
				doProcessLight( *aiLight, scene );
			}

			castor::Matrix4x4f transform;
			transform.setIdentity();
			doProcessSceneNodes( *aiScene
				, *aiScene->mRootNode
				, scene
				, scene.getObjectRootNode()
				, transform );
			std::vector< aiMesh * > aiMeshes;

			for ( auto aiMesh : castor::makeArrayView( aiScene->mMeshes, aiScene->mNumMeshes ) )
			{
				aiMeshes.push_back( aiMesh );
			}

			castor3d::MeshPtrStrMap registeredMeshes;
			auto meshes = doProcessMeshesAndAnims( *aiScene
				, scene
				, aiMeshes
				, registeredMeshes );
			auto merged = doMergeMeshes( scene, meshes );
			doProcessNodes( *aiScene
				, *aiScene->mRootNode
				, scene
				, merged );
			result = true;
		}
		else
		{
			// The import failed, report it
			log::error << "Scene import failed : " << m_importer.GetErrorString() << std::endl;
		}

		return result;
	}

	void AssimpImporter::doProcessLight( aiLight const & aiLight
		, Scene & scene )
	{
		auto position = castor::Point3f{ aiLight.mPosition.x, aiLight.mPosition.y, aiLight.mPosition.z };
		auto direction = castor::point::getNormalised( castor::Point3f{ aiLight.mDirection.x, aiLight.mDirection.y, aiLight.mDirection.z } );
		auto up = castor::point::getNormalised( castor::Point3f{ aiLight.mUp.x, aiLight.mUp.y, aiLight.mUp.z } );
		castor::String name = aiLight.mName.C_Str();
		auto node = std::make_shared< SceneNode >( name
			, scene );
		LightSPtr light;

		switch ( aiLight.mType )
		{
		case aiLightSource_DIRECTIONAL:
			{
				node->setOrientation( castor::Quaternion::fromMatrix( makeMatrix4x4f( direction, up ) ) );
				light = std::make_shared< Light >( name
					, scene
					, *node
					, scene.getLightsFactory()
					, castor3d::LightType::eDirectional );
			}
			break;
		case aiLightSource_POINT:
			{
				node->setPosition( position );
				light = std::make_shared< Light >( name
					, scene
					, *node
					, scene.getLightsFactory()
					, castor3d::LightType::ePoint );
				auto point = light->getPointLight();
				point->setAttenuation( { aiLight.mAttenuationConstant, aiLight.mAttenuationLinear, aiLight.mAttenuationQuadratic } );
			}
			break;
		case aiLightSource_SPOT:
			{
				node->setOrientation( castor::Quaternion::fromMatrix( makeMatrix4x4f( direction, up ) ) );
				node->setPosition( position );
				light = std::make_shared< Light >( name
					, scene
					, *node
					, scene.getLightsFactory()
					, castor3d::LightType::eSpot );
				auto spot = light->getSpotLight();
				spot->setAttenuation( { aiLight.mAttenuationConstant, aiLight.mAttenuationLinear, aiLight.mAttenuationQuadratic } );
				spot->setCutOff( castor::Angle::fromRadians( aiLight.mAngleOuterCone ) );
			}
			break;
		default:
			break;
		}

		if ( light )
		{
			node->attachTo( *scene.getObjectRootNode() );
			node = scene.getSceneNodeCache().add( name, node ).lock();
			m_nodes.push_back( node );
			light->setColour( castor::RgbColour::fromComponents( aiLight.mColorDiffuse.r, aiLight.mColorDiffuse.g, aiLight.mColorDiffuse.b ) );
			node->attachObject( *light );
			scene.getLightCache().add( name, light );
		}
	}

	void AssimpImporter::doProcessSceneNodes( aiScene const & aiScene
		, aiNode const & aiNode
		, Scene & scene
		, SceneNodeSPtr parent
		, castor::Matrix4x4f accTransform )
	{
		castor::String name = aiNode.mName.C_Str();
		auto lnode = scene.getSceneNodeCache().tryFind( name );

		if ( !lnode.lock() )
		{
			auto node = scene.getSceneNodeCache().create( name, scene );
			node->attachTo( *parent );
			aiVector3D scale, position;
			aiQuaternion orientation;
			aiNode.mTransformation.Decompose( scale, orientation, position );
			node->setScale( { scale.x, scale.y, scale.z } );
			node->setOrientation( castor::Quaternion{ castor::Point4f{ orientation.x, orientation.y, orientation.z, orientation.w } } );
			m_nodes.push_back( node );
			lnode = scene.getSceneNodeCache().add( node->getName(), node );
			node = lnode.lock();

			if ( aiScene.HasAnimations() )
			{
				for ( auto aiAnimation : castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) )
				{
					auto it = std::find_if( aiAnimation->mChannels
						, aiAnimation->mChannels + aiAnimation->mNumChannels
						, [&aiNode]( aiNodeAnim const * lookup )
						{
							return lookup->mNodeName == aiNode.mName;
						} );

					if ( it != aiAnimation->mChannels + aiAnimation->mNumChannels )
					{
						doProcessAnimationSceneNodes( *node, *aiAnimation, aiNode, *( *it ) );
					}
				}
			}
		}

		parent = lnode.lock();
		accTransform = makeMatrix4x4f( aiNode.mTransformation ) * accTransform;

		// continue for all child nodes
		for ( auto aiChild : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doProcessSceneNodes( aiScene, *aiChild, scene, parent, accTransform );
		}
	}

	void AssimpImporter::doProcessNodes( aiScene const & aiScene
		, aiNode const & aiNode
		, Scene & scene
		, std::map< uint32_t, MeshData * > const & meshes )
	{
		if ( aiNode.mNumMeshes > 0 )
		{
			castor::String name = aiNode.mName.C_Str();
			auto node = *std::find_if( m_nodes.begin()
				, m_nodes.end()
				, [&name]( SceneNodeSPtr lookup )
				{
					return lookup->getName() == name;
				} );

			for ( auto aiMeshIndex : castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
			{
				auto meshIt = meshes.find( aiMeshIndex );

				if ( meshIt != meshes.end() )
				{
					auto * meshRepl = meshIt->second;
					auto lgeom = scene.getGeometryCache().tryFind( name + castor::string::toString( aiMeshIndex ) );

					if ( !lgeom.lock() )
					{
						auto geom = scene.getGeometryCache().create( name + castor::string::toString( aiMeshIndex )
							, scene
							, *node
							, meshRepl->lmesh );
						geom->setCulled( false );
						uint32_t index = 0u;
						auto matIt = meshRepl->submeshes.find( aiMeshIndex );
						CU_Assert( matIt != meshRepl->submeshes.end(), "Couldn't find material in imported materials." );

						for ( auto material : matIt->second.materials )
						{
							geom->setMaterial( *meshRepl->mesh->getSubmesh( index ), material );
							++index;
						}

						m_geometries.emplace( geom->getName(), geom );
						node->attachObject( *geom );
						scene.getGeometryCache().add( geom );
						auto skeleton = meshRepl->mesh->getSkeleton();

						if ( node->hasAnimation() )
						{
							auto animGroup = scene.getAnimatedObjectGroupCache().add( node->getName() + "Node", scene ).lock();
							auto animObject = animGroup->addObject( *node, node->getName() + cuT( "_Node" ) );

							for ( auto & animation : node->getAnimations() )
							{
								animGroup->addAnimation( animation.first );
								animGroup->setAnimationLooped( animation.first, true );
							}

							animGroup->startAnimation( node->getAnimations().begin()->first );
						}

						if ( meshRepl->mesh->hasAnimation() )
						{
							auto animGroup = scene.getAnimatedObjectGroupCache().add( geom->getName() + "Morph", scene ).lock();
							auto animObject = animGroup->addObject( *meshRepl->mesh
								, *geom
								, geom->getName() + cuT( "_Mesh" ) );

							for ( auto & animation : meshRepl->mesh->getAnimations() )
							{
								animGroup->addAnimation( animation.first );
								animGroup->setAnimationLooped( animation.first, true );
							}

							animGroup->startAnimation( meshRepl->mesh->getAnimations().begin()->first );
						}

						if ( skeleton && skeleton->hasAnimation() )
						{
							auto animGroup = scene.getAnimatedObjectGroupCache().add( geom->getName() + "Skin", scene ).lock();
							auto animObject = animGroup->addObject( *skeleton
								, *meshRepl->mesh
								, *geom
								, geom->getName() + cuT( "_Skeleton" ) );

							for ( auto & animation : skeleton->getAnimations() )
							{
								animGroup->addAnimation( animation.first );
								animGroup->setAnimationLooped( animation.first, true );
							}

							animGroup->startAnimation( skeleton->getAnimations().begin()->first );
						}
					}
				}
			}
		}

		// continue for all child nodes
		for ( auto aiChild : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doProcessNodes( aiScene, *aiChild, scene, meshes );
		}
	}

	std::map< uint32_t, MeshData * > AssimpImporter::doMergeMeshes( castor3d::Scene & scene
			, std::vector< MeshData > & source )
	{
		std::map< uint32_t, MeshData * > result;
		std::vector< MeshData * > meshes;

		for ( auto & mesh : source )
		{
			auto it = std::find_if( meshes.begin()
				, meshes.end()
				, [&mesh]( MeshData const * lookup )
				{
					return *lookup->mesh == *mesh.mesh;
				} );

			if ( it == meshes.end() )
			{
				meshes.push_back( &mesh );
				result.emplace( *mesh.aiMeshIndices.begin(), &mesh );
			}
			else
			{
				auto & meshRes = **mesh.mesh;
				meshRes.cleanup();
				mesh.mesh.reset();
				result.emplace( *mesh.aiMeshIndices.begin(), *it );

				for ( auto matIt : mesh.submeshes )
				{
					( *it )->submeshes.emplace( matIt );
				}
			}
		}

		for ( auto & mesh : meshes )
		{
			for ( auto submesh : *mesh->mesh )
			{
				scene.getListener().postEvent( makeGpuInitialiseEvent( *submesh ) );
			}

			mesh->lmesh = scene.getMeshCache().add( mesh->mesh->getName()
				, mesh->mesh
				, true );

			if ( !mesh->mesh )
			{
				mesh->mesh = mesh->lmesh.lock();
			}

			m_meshes.emplace( mesh->mesh->getName(), mesh->mesh );
		}

		return result;
	}

	std::vector< MeshData > AssimpImporter::doProcessMeshesAndAnims( aiScene const & aiScene
		, Scene & scene
		, std::vector< aiMesh * > const & aiMeshes
		, castor3d::MeshPtrStrMap & registeredMeshes )
	{
		std::vector< MeshData > result;
		uint32_t index = 0u;

		for ( auto & aiMesh : aiMeshes )
		{
			castor::String name = aiMesh->mName.C_Str();
			auto regIt = registeredMeshes.find( name );
			MeshRes mesh;
			MeshResPtr lmesh;

			if ( regIt != registeredMeshes.end() )
			{
				lmesh = regIt->second;
				mesh = lmesh.lock();
			}
			else
			{
				mesh = scene.getMeshCache().create( name, scene );
				lmesh = mesh;
				registeredMeshes.emplace( name, lmesh );
			}

			if ( doProcessMeshAndAnims( aiScene, { aiMesh }, *mesh ) )
			{
				if ( regIt == registeredMeshes.end() )
				{
					// new mesh
					SubmeshData submesh{ { mesh->getSubmesh( mesh->getSubmeshCount() - 1u )->getDefaultMaterial() } };
					result.push_back( { mesh
						, lmesh
						, { index }
						, { { index, submesh } } } );
				}
				else
				{
					// additional submesh
					auto it = std::find_if( result.begin()
						, result.end()
						, [&mesh]( MeshData const & lookup )
						{
							return lookup.mesh == mesh;
						} );
					CU_Require( it != result.end() );
					it->aiMeshIndices.insert( index );
					it->submeshes.begin()->second.materials.push_back( mesh->getSubmesh( mesh->getSubmeshCount() - 1u )->getDefaultMaterial() );
				}
			}

			++index;
		}

		return result;
	}

	bool AssimpImporter::doProcessMeshAndAnims( aiScene const & aiScene
		, std::vector< aiMesh * > aiMeshes
		, castor3d::Mesh & mesh )
	{
		SkeletonSPtr skeleton = std::make_shared< Skeleton >( *mesh.getScene() );
		skeleton->setGlobalInverseTransform( makeMatrix4x4f( aiScene.mRootNode->mTransformation ).getInverse() );

		bool create = true;
		uint32_t aiMeshIndex = 0u;
		SubmeshSPtr submesh;

		for ( auto aiMesh : aiMeshes )
		{
			if ( create )
			{
				submesh = mesh.createSubmesh();
			}

			create = doProcessMesh( *mesh.getScene(), mesh, *skeleton, *aiMesh, aiMeshIndex, aiScene, *submesh );
			++aiMeshIndex;
		}

		if ( m_arrayBones.empty() )
		{
			skeleton.reset();
		}
		else
		{
			mesh.setSkeleton( skeleton );
		}

		if ( skeleton )
		{
			for ( auto aiAnimation : castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) )
			{
				doProcessAnimation( mesh
					, m_fileName.getFileName()
					, *skeleton
					, *aiScene.mRootNode
					, *aiAnimation );
			}

			if ( castor::string::upperCase( m_fileName.getExtension() ) == cuT( "MD5MESH" ) )
			{
				// Workaround to load multiple animations with MD5 models.
				castor::PathArray files;
				castor::File::listDirectoryFiles( m_fileName.getPath(), files );

				for ( auto file : files )
				{
					if ( castor::string::lowerCase( file.getExtension() ) == cuT( "md5anim" ) )
					{
						// The .md5anim with the same name as the .md5mesh has already been loaded by assimp.
						if ( file.getFileName() != m_fileName.getFileName() )
						{
							Assimp::Importer importer;
							auto scene = importer.ReadFile( file, m_flags );

							for ( auto aiAnimation : castor::makeArrayView( scene->mAnimations, scene->mNumAnimations ) )
							{
								doProcessAnimation( mesh
									, file.getFileName()
									, *skeleton
									, *scene->mRootNode
									, *aiAnimation );
							}

							importer.FreeScene();
						}
					}
				}
			}
		}

		return true;
	}

	aiScene const * AssimpImporter::doLoadScene()
	{
		bool noOptim = false;
		auto found = m_parameters.get( "no_optimisations", noOptim );

		if ( !found || !noOptim )
		{
			m_flags = aiProcess_Triangulate
				| aiProcess_JoinIdenticalVertices
				| aiProcess_OptimizeMeshes
				| aiProcess_OptimizeGraph
				| aiProcess_FixInfacingNormals
				| aiProcess_LimitBoneWeights
				| aiProcess_Debone;
		}

		bool tangentSpace = false;
		castor::String normals;

		if ( m_parameters.get( cuT( "normals" ), normals ) )
		{
			if ( normals == cuT( "smooth" ) )
			{
				m_flags |= aiProcess_GenSmoothNormals;
			}
		}

		if ( m_parameters.get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
		{
			m_flags |= aiProcess_CalcTangentSpace;
		}

		// And have it read the given file with some postprocessing
		aiScene const * aiScene;

		try
		{
			aiScene = m_importer.ReadFile( castor::string::stringCast< char >( m_fileName ), m_flags );
		}
		catch ( std::exception & exc )
		{
			castor3d::log::error << exc.what() << std::endl;
			return nullptr;
		}

		return aiScene;
	}

	bool AssimpImporter::doProcessMesh( Scene & scene
		, Mesh & mesh
		, Skeleton & skeleton
		, aiMesh const & aiMesh
		, uint32_t aiMeshIndex
		, aiScene const & aiScene
		, Submesh & submesh )
	{
		bool result = false;
		MaterialResPtr material;
		log::debug << cuT( "Mesh found: [" ) << aiMesh.mName.C_Str() << cuT( "]" ) << std::endl;

		if ( aiMesh.mMaterialIndex < aiScene.mNumMaterials )
		{
			material = doProcessMaterial( scene, aiScene, *aiScene.mMaterials[aiMesh.mMaterialIndex], aiMesh.mMaterialIndex );
			log::debug << cuT( "  Material: [" ) << aiMesh.mMaterialIndex << cuT( " (" ) << material.lock()->getName() << cuT( ")]" ) << std::endl;
		}

		if ( aiMesh.HasFaces() && aiMesh.HasPositions() && material.lock() )
		{
			auto faces = castor::makeArrayView( aiMesh.mFaces, aiMesh.mNumFaces );
			auto count = uint32_t( std::count_if( faces.begin()
				, faces.end()
				, []( aiFace const & face )
				{
					return face.mNumIndices == 3
						|| face.mNumIndices == 4;
				} ) );

			if ( count > 0 )
			{
				m_submeshByID.emplace( aiMeshIndex, submesh.getId() );
				submesh.setDefaultMaterial( material.lock().get() );
				submesh.addPoints( createVertexBuffer( aiMesh ) );

				if ( aiMesh.HasBones() )
				{
					log::debug << cuT( "  Skeleton found" ) << std::endl;
					std::vector< VertexBoneData > bonesData( aiMesh.mNumVertices );
					doProcessBones( skeleton, aiMesh.mBones, aiMesh.mNumBones, bonesData );
					auto bones = std::make_shared< BonesComponent >( submesh );
					bones->addBoneDatas( bonesData );
					submesh.addComponent( bones );
				}

				auto mapping = std::make_shared< TriFaceMapping >( submesh );

				for ( auto face : faces )
				{
					if ( face.mNumIndices == 3 )
					{
						mapping->addFace( face.mIndices[0], face.mIndices[2], face.mIndices[1] );
					}
					else if ( face.mNumIndices == 4 )
					{
						mapping->addQuadFace( face.mIndices[0], face.mIndices[2], face.mIndices[1], face.mIndices[2] );
					}
				}

				if ( !aiMesh.HasNormals() )
				{
					mapping->computeNormals( true );
				}
				else if ( !aiMesh.HasTangentsAndBitangents() )
				{
					mapping->computeTangentsFromNormals();
				}

				submesh.setIndexMapping( mapping );

				if ( aiMesh.mNumAnimMeshes )
				{
					int32_t index = 0u;
					castor::String name{ castor::string::stringCast< xchar >( aiMesh.mName.C_Str() ) };
					log::info << cuT( "  Mesh Animation found: [" ) << name << cuT( "]" ) << std::endl;
					auto & animation = mesh.createAnimation( name );
					MeshAnimationSubmesh animSubmesh{ animation, submesh };
					animation.addChild( std::move( animSubmesh ) );
					MeshAnimationKeyFrameUPtr keyFrame = std::make_unique< MeshAnimationKeyFrame >( animation
						, castor::Milliseconds{ int64_t( index++ * 2000 ) } );
					keyFrame->addSubmeshBuffer( submesh, submesh.getPoints() );
					animation.addKeyFrame( std::move( keyFrame ) );
					auto const & cmapping = *mapping;

					for ( auto aiAnimMesh : castor::makeArrayView( aiMesh.mAnimMeshes, aiMesh.mNumAnimMeshes ) )
					{
						keyFrame = std::make_unique< MeshAnimationKeyFrame >( animation
							, castor::Milliseconds{ int64_t( index++ * 2000 ) } );
						auto points = createVertexBuffer( *aiAnimMesh );

						if ( !aiAnimMesh->HasNormals() )
						{
							cmapping.computeNormals( points, true );
						}
						else if ( !aiAnimMesh->HasTangentsAndBitangents() )
						{
							cmapping.computeTangentsFromNormals( points );
						}

						keyFrame->addSubmeshBuffer( submesh, points );
						animation.addKeyFrame( std::move( keyFrame ) );
					}
				}

				result = true;
			}
		}

		return result;
	}

	void AssimpImporter::doProcessAnimationMeshes( Mesh & mesh
		, Submesh & submesh
		, aiMesh const & aiMesh
		, aiAnimMesh const & aiAnimMesh
		, uint32_t index )
	{
	}

	MaterialResPtr AssimpImporter::doProcessMaterial( Scene & scene
		, aiScene const & aiScene
		, aiMaterial const & aiMaterial
		, uint32_t index )
	{
		MaterialResPtr result;
		auto & cache = scene.getMaterialView();
		aiString mtlname;
		castor::String name = cuT( "Imp." );

		if ( aiMaterial.Get( AI_MATKEY_NAME, mtlname ) == aiReturn_SUCCESS )
		{
			name += castor::string::stringCast< xchar >( mtlname.C_Str() );
		}
		else
		{
			name += castor::string::toString( index );
		}

		if ( name.empty() )
		{
			name = m_fileName.getFileName() + castor::string::toString( m_anonymous++ );
		}

		if ( cache.has( name ) )
		{
			result = cache.find( name );
		}
		else
		{
			result = cache.add( name
				, *scene.getEngine()
				, scene.getPassesType() );
			auto pass = result.lock()->createPass();
			processMaterialPass( scene
				, getEngine()->getDefaultSampler().lock()
				, m_textureRemaps
				, *pass
				, aiMaterial
				, aiScene
				, *this );
		}

		return result;
	}

	BoneSPtr AssimpImporter::doAddBone( castor::String const & name
		, castor::Matrix4x4f const & offset
		, Skeleton & skeleton
		, uint32_t & index )
	{
		BoneSPtr bone = skeleton.createBone( name, offset );
		index = uint32_t( m_arrayBones.size() );
		m_arrayBones.push_back( bone );
		m_mapBoneByID[name] = index;
		log::debug << cuT( "    Bone: [" ) << name << cuT( "]" ) << std::endl;
		return bone;
	}

	void AssimpImporter::doProcessBones( Skeleton & skeleton
		, aiBone const * const * aiBones
		, uint32_t count
		, std::vector< VertexBoneData > & arrayVertices )
	{
		for ( uint32_t i = 0; i < count; ++i )
		{
			aiBone const & aiBone = *aiBones[i];
			castor::String name = castor::string::stringCast< xchar >( aiBone.mName.C_Str() );
			uint32_t index;

			if ( m_mapBoneByID.find( name ) == m_mapBoneByID.end() )
			{
				doAddBone( name, makeMatrix4x4f( aiBone.mOffsetMatrix ), skeleton, index );
			}
			else
			{
				index = m_mapBoneByID[name];
				CU_Ensure( m_arrayBones[index]->getOffsetMatrix() == makeMatrix4x4f( aiBone.mOffsetMatrix ) );
			}

			for ( auto weight : castor::makeArrayView( aiBone.mWeights, aiBone.mNumWeights ) )
			{
				arrayVertices[weight.mVertexId].addBoneData( index, weight.mWeight );
			}
		}
	}

	void AssimpImporter::doProcessAnimation( Mesh & mesh
		, castor::String const & animName
		, Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation )
	{
		castor::String name{ castor::string::stringCast< xchar >( aiAnimation.mName.C_Str() ) };

		if ( name.empty() )
		{
			name = animName;
		}

		log::info << cuT( "  Skeleton Animation found: [" ) << name << cuT( "]" ) << std::endl;
		auto & animation = skeleton.createAnimation( name );
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25ll;
		SkeletonAnimationKeyFrameMap keyframes;
		SkeletonAnimationObjectSet notAnimated;
		doProcessAnimationNodes( mesh
			, animation
			, ticksPerSecond
			, skeleton
			, aiNode
			, aiAnimation
			, nullptr
			, keyframes
			, notAnimated );

		for ( auto & object : notAnimated )
		{
			for ( auto & keyFrame : keyframes )
			{
				if ( !keyFrame.second->hasObject( *object ) )
				{
					keyFrame.second->addAnimationObject( *object, object->getNodeTransform() );
				}
			}
		}

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( std::move( keyFrame.second ) );
		}

		animation.updateLength();
	}

	void AssimpImporter::doProcessAnimationNodes( Mesh & mesh
		, SkeletonAnimation & animation
		, int64_t ticksPerSecond
		, Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation
		, SkeletonAnimationObjectSPtr parent
		, SkeletonAnimationKeyFrameMap & keyFrames
		, SkeletonAnimationObjectSet & notAnimated )
	{
		castor::String name = castor::string::stringCast< xchar >( aiNode.mName.data );
		const aiNodeAnim * aiNodeAnim = findNodeAnim( aiAnimation, name );
		SkeletonAnimationObjectSPtr object;
		auto itBone = m_mapBoneByID.find( name );

		if ( itBone == m_mapBoneByID.end()
			&& aiNode.mNumMeshes )
		{
			uint32_t index;
			doAddBone( name, castor::Matrix4x4f{ 1.0f }, skeleton, index );

			for ( auto const & aiMesh : castor::makeArrayView( aiNode.mMeshes, aiNode.mNumMeshes ) )
			{
				auto submeshIt = m_submeshByID.find( aiMesh );

				if ( submeshIt != m_submeshByID.end() )
				{
					auto submesh = mesh.getSubmesh( submeshIt->second );
					CU_Require( submesh != nullptr );

					if ( !submesh->hasComponent( BonesComponent::Name ) )
					{
						auto bones = std::make_shared< BonesComponent >( *submesh );
						std::vector< VertexBoneData > arrayBones( submesh->getPointsCount() );

						for ( auto & boneData : arrayBones )
						{
							boneData.addBoneData( index, 1.0f );
						}

						bones->addBoneDatas( arrayBones );
						submesh->addComponent( bones );
					}
				}
			}

			itBone = m_mapBoneByID.find( name );
		}

		bool added = false;

		if ( itBone != m_mapBoneByID.end() )
		{
			auto bone = m_arrayBones[itBone->second];
			if ( !animation.hasObject( SkeletonAnimationObjectType::eBone, aiNode.mName.C_Str() ) )
			{
				object = animation.addObject( bone, parent );
				added = true;

				if ( parent && parent->getType() == SkeletonAnimationObjectType::eBone )
				{
					skeleton.setBoneParent( bone, std::static_pointer_cast< SkeletonAnimationBone >( parent )->getBone() );
				}
			}
			else
			{
				object = animation.getObject( SkeletonAnimationObjectType::eBone, aiNode.mName.C_Str() );
				CU_Ensure( object->getNodeTransform() == makeMatrix4x4f( aiNode.mTransformation ) );
				CU_Ensure( object->getParent() == parent || object == parent );
				added = false;
			}
		}
		else if ( !animation.hasObject( SkeletonAnimationObjectType::eNode, aiNode.mName.C_Str() ) )
		{
			object = animation.addObject( aiNode.mName.C_Str(), parent );
			added = true;
		}
		else
		{
			object = animation.getObject( SkeletonAnimationObjectType::eNode, aiNode.mName.C_Str() );
			CU_Ensure( object->getNodeTransform() == makeMatrix4x4f( aiNode.mTransformation ) );
			CU_Ensure( object->getParent() == parent || object == parent );
			added = false;
		}

		if ( object && added )
		{
			if ( parent && object != parent )
			{
				parent->addChild( object );
			}

			object->setNodeTransform( makeMatrix4x4f( aiNode.mTransformation ) );
		}

		if ( aiNodeAnim )
		{
			doProcessAnimationNodeKeys( *aiNodeAnim
				, ticksPerSecond
				, *object
				, animation
				, keyFrames );
		}
		else
		{
			notAnimated.insert( object );
		}

		for ( auto node : castor::makeArrayView( aiNode.mChildren, aiNode.mNumChildren ) )
		{
			doProcessAnimationNodes( mesh
				, animation
				, ticksPerSecond
				, skeleton
				, *node
				, aiAnimation
				, object
				, keyFrames
				, notAnimated );
		}
	}

	void AssimpImporter::doProcessAnimationNodeKeys( aiNodeAnim const & aiNodeAnim
		, int64_t ticksPerSecond
		, SkeletonAnimationObject & object
		, SkeletonAnimation & animation
		, SkeletonAnimationKeyFrameMap & keyframes )
	{
		processAnimationNodeKeys( aiNodeAnim
			, getEngine()->getRenderLoop().getWantedFps()
			, ticksPerSecond
			, animation
			, keyframes
			, [&object]( SkeletonAnimationKeyFrame & keyframe
				, castor::Point3f const & position
				, castor::Quaternion const & orientation
				, castor::Point3f const & scale )
			{
				keyframe.addAnimationObject( object, position, orientation, scale );
			} );
	}

	void AssimpImporter::doProcessAnimationSceneNodes( castor3d::SceneNode & node
		, aiAnimation const & aiAnimation
		, aiNode const & aiNode
		, aiNodeAnim const & aiNodeAnim )
	{
		if ( aiNodeAnim.mNumPositionKeys <= 1
			&& aiNodeAnim.mNumScalingKeys <= 1
			&& aiNodeAnim.mNumRotationKeys <= 1 )
		{
			return;
		}

		castor::String name{ castor::string::stringCast< xchar >( aiAnimation.mName.C_Str() ) };

		if ( name.empty() )
		{
			name = node.getName();
		}

		log::info << cuT( "  SceneNode Animation found: [" ) << name << cuT( "]" ) << std::endl;
		auto & animation = node.createAnimation( name );
		int64_t ticksPerSecond = aiAnimation.mTicksPerSecond != 0.0
			? int64_t( aiAnimation.mTicksPerSecond )
			: 25ll;
		SceneNodeAnimationKeyFrameMap keyframes;
		processAnimationNodeKeys( aiNodeAnim
			, getEngine()->getRenderLoop().getWantedFps()
			, ticksPerSecond
			, animation
			, keyframes
			, []( SceneNodeAnimationKeyFrame & keyframe
				, castor::Point3f const & position
				, castor::Quaternion const & orientation
				, castor::Point3f const & scale )
			{
				keyframe.setTransform( position, orientation, scale );
			} );

		for ( auto & keyFrame : keyframes )
		{
			animation.addKeyFrame( std::move( keyFrame.second ) );
		}
	}

	//*********************************************************************************************
}
