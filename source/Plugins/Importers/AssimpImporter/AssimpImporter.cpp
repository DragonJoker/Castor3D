#include "AssimpImporter/AssimpImporter.hpp"

#include <Castor3D/Animation/Interpolator.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/ObjectCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Cache/CacheView.hpp>
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
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Design/ResourceCache.hpp>
#include <CastorUtils/Log/Logger.hpp>

#include <assimp/version.h>

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
		aiNodeAnim const * const doFindNodeAnim( const aiAnimation & animation
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

		aiMeshAnim const * const FindMeshAnim( const aiAnimation & animation
			, const castor::String & meshName )
		{
			aiMeshAnim const * result = nullptr;
			auto it = std::find_if( animation.mMeshChannels
				, animation.mMeshChannels + animation.mNumMeshChannels
				, [&meshName]( aiMeshAnim const * const p_meshAnim )
				{
					return castor::string::stringCast< xchar >( p_meshAnim->mName.data ) == meshName;
				} );

			if ( it != animation.mMeshChannels + animation.mNumMeshChannels )
			{
				result = *it;
			}

			return result;
		}

		template< typename aiMeshType >
		InterleavedVertexArray doCreateVertexBuffer( aiMeshType const & aiMesh )
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

		template< typename T >
		void doFind( castor::Milliseconds time
			, typename std::map< castor::Milliseconds, T > const & map
			, typename std::map< castor::Milliseconds, T >::const_iterator & prv
			, typename std::map< castor::Milliseconds, T >::const_iterator & cur )
		{
			if ( map.empty() )
			{
				prv = cur = map.end();
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
		T doCompute( castor::Milliseconds const & from
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
				doFind( from, values, prv, cur );

				if ( prv != cur )
				{
					auto dt = cur->first - prv->first;
					float factor = ( from - prv->first ).count() / float( dt.count() );
					result = interpolator.interpolate( prv->second, cur->second, factor );
				}
				else
				{
					result = prv->second;
				}
			}

			return result;
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
				, castor3d::MeshImporter const & importer
				, castor3d::Pass & pass )
			{
				PassFiller vis{ material, importer, pass };
				pass.accept( vis );
				vis.finish();
			}

		private:
			struct CommonTextureInfos
			{
				TextureInfo colTex{};
				TextureInfo emiTex{};
				TextureInfo nmlTex{};
				TextureInfo spcTex{};
				TextureInfo hgtTex{};
				TextureInfo opaTex{};
				TextureInfo occTex{};
			};

			struct CommonMaterialInfos
			{
				aiColor3D colour{};
				aiColor3D emissive{};
				aiColor3D ambient{};
				aiColor3D specular{};
			};

			PassFiller( aiMaterial const & material
				, castor3d::MeshImporter const & importer
				, castor3d::Pass & result )
				: castor3d::PassVisitor{ {} }
				, m_material{ material }
				, m_importer{ importer }
				, m_result{ result }
			{
			}

			void finish()
			{
				if ( !m_texInfos.colTex.name.empty()
					&& m_texInfos.colTex.name.find( "_Cine_" ) != castor::String::npos
					&& m_texInfos.colTex.name.find( "/MI_CH_" ) != castor::String::npos )
				{
					// Workaround for Collada textures.
					castor::String strGlob = m_texInfos.colTex.name + cuT( ".tga" );
					castor::string::replace( strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
					castor::String strDiff = strGlob;
					castor::String strNorm = strGlob;
					castor::String strSpec = strGlob;
					castor::String strOpac = strGlob;
					m_texInfos.colTex.name = castor::string::replace( strDiff, cuT( "_Cine_" ), cuT( "_D_" ) );
					m_texInfos.nmlTex.name = castor::string::replace( strNorm, cuT( "_Cine_" ), cuT( "_N_" ) );
					m_texInfos.spcTex.name = castor::string::replace( strSpec, cuT( "_Cine_" ), cuT( "_S_" ) );
					m_texInfos.opaTex.name = castor::string::replace( strOpac, cuT( "_Cine_" ), cuT( "_A_" ) );
					m_texInfos.nmlTex.transform = m_texInfos.colTex.transform;
					m_texInfos.spcTex.transform = m_texInfos.colTex.transform;
					m_texInfos.opaTex.transform = m_texInfos.colTex.transform;
				}

				loadTexture( m_texInfos.colTex, TextureConfiguration::DiffuseTexture );
				loadTexture( m_texInfos.emiTex, TextureConfiguration::EmissiveTexture );
				loadTexture( m_texInfos.occTex, TextureConfiguration::OcclusionTexture );

				// force non 0.0 opacity when an opacity map is set
				if ( !m_texInfos.opaTex.name.empty()
					&& m_result.getOpacity() == 0.0f )
				{
					loadTexture( m_texInfos.opaTex, TextureConfiguration::OpacityTexture );
					m_result.setOpacity( 1.0f );
				}

				if ( !m_texInfos.nmlTex.name.empty() )
				{
					loadTexture( m_texInfos.nmlTex, TextureConfiguration::NormalTexture );
					loadTexture( m_texInfos.hgtTex, TextureConfiguration::HeightTexture );
				}
				else
				{
					auto config = TextureConfiguration::NormalTexture;
					convertToNormalMap( m_texInfos.hgtTex, config );
					loadTexture( m_texInfos.hgtTex, config );
				}

				if ( m_mtlInfos.ambient.IsBlack()
					&& m_mtlInfos.colour.IsBlack()
					&& m_mtlInfos.specular.IsBlack()
					&& m_mtlInfos.emissive.IsBlack() )
				{
					m_mtlInfos.colour.r = 1.0;
					m_mtlInfos.colour.g = 1.0;
					m_mtlInfos.colour.b = 1.0;
				}
			}

			void visit( castor::String const & name
				, bool & value
				, bool * control )override
			{
				if ( name == "Two sided" )
				{
					int twoSided = 0;
					m_material.Get( AI_MATKEY_TWOSIDED, twoSided );
					value = ( twoSided != 0 );
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
				if ( name == "Shininess"
					|| name == "Glossiness"
					|| name == "Roughness" )
				{
					float shininess = 0.5f;
					m_material.Get( AI_MATKEY_SHININESS, shininess );
					float shininessStrength = 1.0f;
					m_material.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength );
					shininess *= shininessStrength;

					if ( name != "Shininess" )
					{
						shininess /= PhongPass::MaxShininess;
					}

					if ( name == "Shininess" )
					{
						shininess = 1.0 - shininess;
					}

					if ( shininess > 0 )
					{
						value = shininess;
					}
				}
				else if ( name == "Metalness" )
				{
					float metalness = 0.5f;
					m_material.Get( AI_MATKEY_COLOR_SPECULAR, metalness );
					value = metalness;
					m_mtlInfos.specular.r = metalness;
				}
				else if ( name == "Opacity" )
				{
					float opacity = 1;
					m_material.Get( AI_MATKEY_OPACITY, opacity );
					value = opacity;
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
				, castor::RgbColour & value
				, bool * control )override
			{
				if ( name == "Ambient" )
				{
					m_mtlInfos.ambient = { 1, 1, 1 };
					m_material.Get( AI_MATKEY_COLOR_AMBIENT, m_mtlInfos.ambient );
					value = castor::RgbColour::fromComponents( m_mtlInfos.ambient.r
						, m_mtlInfos.ambient.g
						, m_mtlInfos.ambient.b );
				}
				else if ( name == "Albedo"
					|| name == "Colour"
					|| name == "Diffuse" )
				{
					m_mtlInfos.colour = { 1, 1, 1 };
					m_material.Get( AI_MATKEY_COLOR_DIFFUSE, m_mtlInfos.colour );
					value = castor::RgbColour::fromComponents( m_mtlInfos.colour.r
						, m_mtlInfos.colour.g
						, m_mtlInfos.colour.b );
				}
				else if ( name == "Specular" )
				{
					m_mtlInfos.specular = { 1, 1, 1 };
					m_material.Get( AI_MATKEY_COLOR_SPECULAR, m_mtlInfos.specular );
					value = castor::RgbColour::fromComponents( m_mtlInfos.specular.r
						, m_mtlInfos.specular.g
						, m_mtlInfos.specular.b );
				}
				else if ( name == "Emissive" )
				{
					m_mtlInfos.emissive = { 1, 1, 1 };
					m_material.Get( AI_MATKEY_COLOR_EMISSIVE, m_mtlInfos.emissive );
					value = castor::RgbColour::fromComponents( m_mtlInfos.emissive.r
						, m_mtlInfos.emissive.g
						, m_mtlInfos.emissive.b );
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
				castor3d::TextureConfiguration config;
				auto info = getTextureInfo( name
					, textureFlag
					, config );
				loadTexture( info, config );
			}

		private:
			void loadTexture( TextureInfo const & info
				, castor3d::TextureConfiguration const & config )
			{
				if ( !info.name.empty() )
				{
					auto texture = m_importer.loadTexture( castor::Path{ info.name }
						, config
						, m_result );

					if ( texture )
					{
						log::debug << cuT( "  Texture: [" ) << texture->toString() << cuT( "]" ) << std::endl;
						texture->setTransform( { info.transform.mTranslation.x, info.transform.mTranslation.y, 0.0f }
							, castor::Angle::fromRadians( info.transform.mRotation )
							, { info.transform.mScaling.x, info.transform.mScaling.y, 1.0f } );
					}
				}
			}

			TextureInfo getTextureInfo( aiTextureType type )
			{
				TextureInfo result;
				aiString name;
				m_material.Get( AI_MATKEY_TEXTURE( type, 0 ), name );

				if ( name.length > 0 )
				{
					result.name = castor::string::stringCast< xchar >( name.C_Str() );
					m_material.Get( AI_MATKEY_UVTRANSFORM( type, 0 ), result.transform );
				}

				return result;
			}

			TextureInfo getTextureInfo( castor::String const & name
				, castor3d::TextureFlag flag
				, castor3d::TextureConfiguration & config )
			{
				TextureInfo result{};

				switch ( flag )
				{
				case castor3d::TextureFlag::eHeight:
					m_texInfos.hgtTex = getTextureInfo( aiTextureType_HEIGHT );
					break;
				case castor3d::TextureFlag::eOpacity:
					m_texInfos.opaTex = getTextureInfo( aiTextureType_OPACITY );
					config = castor3d::TextureConfiguration::OpacityTexture;
					break;
				case castor3d::TextureFlag::eNormal:
					m_texInfos.nmlTex = getTextureInfo( aiTextureType_NORMALS );
					break;
				case castor3d::TextureFlag::eEmissive:
					m_texInfos.emiTex = getTextureInfo( aiTextureType_EMISSIVE );
					break;
				case castor3d::TextureFlag::eOcclusion:
					if ( aiGetVersionMajor() >= 4u )
					{
						static auto constexpr TextureType_AMBIENT_OCCLUSION = aiTextureType( 17 );
						m_texInfos.occTex = getTextureInfo( TextureType_AMBIENT_OCCLUSION );
					}
					break;
				case castor3d::TextureFlag::eTransmittance:
					break;
				case castor3d::TextureFlag::eColour:
					m_texInfos.colTex = getTextureInfo( aiTextureType_DIFFUSE );
					break;
				case castor3d::TextureFlag::eSpecular:
					m_texInfos.spcTex = getTextureInfo( aiTextureType_SPECULAR );
					config = TextureConfiguration::SpecularTexture;
					break;
				case castor3d::TextureFlag::eMetalness:
					if ( aiGetVersionMajor() >= 4u )
					{
						static auto constexpr TextureType_METALNESS = aiTextureType( 15 );
						result = getTextureInfo( TextureType_METALNESS );
						config = TextureConfiguration::MetalnessTexture;
					}
					else
					{
						result = getTextureInfo( aiTextureType_SPECULAR );
						config = TextureConfiguration::SpecularTexture;
					}
					break;
				case castor3d::TextureFlag::eGlossiness:
					result = getTextureInfo( aiTextureType_SHININESS );
					config = TextureConfiguration::ShininessTexture;
					break;
				case castor3d::TextureFlag::eRoughness:
					if ( aiGetVersionMajor() >= 4u )
					{
						static auto constexpr TextureType_DIFFUSE_ROUGHNESS = aiTextureType( 16 );
						result = getTextureInfo( TextureType_DIFFUSE_ROUGHNESS );
						config = TextureConfiguration::RoughnessTexture;
					}
					else
					{
						result = getTextureInfo( aiTextureType_SHININESS );
						config = TextureConfiguration::RoughnessTexture;
					}
					break;
				default:
					break;
				}

				return result;
			}

			void convertToNormalMap( TextureInfo & info
				, castor3d::TextureConfiguration & config )
			{
				auto path = castor::Path{ info.name };

				if ( m_importer.convertToNormalMap( path, config ) )
				{
					info.name = path;
				}
			}

		private:
			aiMaterial const & m_material;
			castor3d::MeshImporter const & m_importer;
			castor3d::Pass & m_result;
			CommonTextureInfos m_texInfos{};
			CommonMaterialInfos m_mtlInfos{};
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

		void doProcessMaterialPass( Pass & pass
			, aiMaterial const & aiMaterial
			, AssimpImporter & importer )
		{
			aiShadingMode shadingMode;
			aiMaterial.Get( AI_MATKEY_SHADING_MODEL, shadingMode );
			auto & passFactory = importer.getEngine()->getPassFactory();
			auto srcType = convert( passFactory, shadingMode );
			auto dstType = pass.getTypeID();

			if ( dstType != srcType )
			{
				log::warn << "Switching from " << passFactory.getIdName( srcType ) << " to " << passFactory.getIdName( dstType ) << " pass type." << std::endl;
			}

			PassFiller::submit( aiMaterial, importer, pass );

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

		std::map< castor::Milliseconds, castor::Point3f > doProcessVec3Keys( aiVectorKey const * const keys
			, uint32_t count
			, int64_t ticksPerMilliSecond
			, std::set< castor::Milliseconds > const & times )
		{
			std::map< castor::Milliseconds, castor::Point3f > result;

			for ( auto const & key : castor::makeArrayView( keys, count ) )
			{
				if ( key.mTime > 0 )
				{
					auto time = castor::Milliseconds{ int64_t( key.mTime * 1000 ) } / ticksPerMilliSecond;
					result[time] = castor::Point3f{ key.mValue.x, key.mValue.y, key.mValue.z };
				}
				else
				{
					for ( auto & time : times )
					{
						result[time] = castor::Point3f{ key.mValue.x, key.mValue.y, key.mValue.z };
					}
				}
			}

			return result;
		}

		std::map< castor::Milliseconds, castor::Quaternion > doProcessQuatKeys( aiQuatKey const * const keys
			, uint32_t count
			, int64_t ticksPerMilliSecond
			, std::set< castor::Milliseconds > const & times )
		{
			std::map< castor::Milliseconds, castor::Quaternion > result;

			for ( auto const & key : castor::makeArrayView( keys, count ) )
			{
				if ( key.mTime > 0 )
				{
					auto time = castor::Milliseconds{ int64_t( key.mTime * 1000 ) } / ticksPerMilliSecond;
					result[time] = castor::Quaternion::fromMatrix( castor::Matrix4x4f{ castor::Matrix3x3f{ &key.mValue.GetMatrix().Transpose().a1 } } );
				}
				else
				{
					for ( auto & time : times )
					{
						result[time] = castor::Quaternion::fromMatrix( castor::Matrix4x4f{ castor::Matrix3x3f{ &key.mValue.GetMatrix().Transpose().a1 } } );
					}
				}
			}

			return result;
		}

		SkeletonAnimationKeyFrame & doGetKeyFrame( castor::Milliseconds const & time
			, SkeletonAnimation & animation
			, SkeletonAnimationKeyFrameMap & keyframes )
		{
			auto it = keyframes.find( time );

			if ( it == keyframes.end() )
			{
				it = keyframes.emplace( time
					, std::make_unique< SkeletonAnimationKeyFrame >( animation, time ) ).first;
			}

			return *it->second;
		}

		void doSynchroniseKeys( std::map< castor::Milliseconds, castor::Point3f > const & translates
			, std::map< castor::Milliseconds, castor::Point3f > const & scales
			, std::map< castor::Milliseconds, castor::Quaternion > const & rotates
			, std::set< castor::Milliseconds > const & times
			, uint32_t fps
			, int64_t ticksPerMilliSecond
			, SkeletonAnimationObject & object
			, SkeletonAnimation & animation
			, SkeletonAnimationKeyFrameMap & keyframes )
		{
			InterpolatorT< castor::Point3f, InterpolatorType::eLinear > pointInterpolator;
			InterpolatorT< castor::Quaternion, InterpolatorType::eLinear > quatInterpolator;

			if ( fps == castor3d::RenderLoop::UnlimitedFPS
				|| ticksPerMilliSecond / 1000 >= fps )
			{
				for ( auto time : times )
				{
					auto translate = doCompute( time, pointInterpolator, translates );
					auto scale = doCompute( time, pointInterpolator, scales );
					auto rotate = doCompute( time, quatInterpolator, rotates );
					doGetKeyFrame( time, animation, keyframes ).addAnimationObject( object
						, translate
						, rotate
						, scale );
				}
			}
			else
			{
				// Limit the key frames per second to 60, to spare RAM...
				castor::Milliseconds step{ 1000 / std::min< int64_t >( 60, int64_t( fps ) ) };
				castor::Milliseconds maxTime{ *times.rbegin() + step };

				for ( auto time = 0_ms; time < maxTime; time += step )
				{
					auto translate = doCompute( time, pointInterpolator, translates );
					auto scale = doCompute( time, pointInterpolator, scales );
					auto rotate = doCompute( time, quatInterpolator, rotates );
					doGetKeyFrame( time, animation, keyframes ).addAnimationObject( object
						, translate
						, rotate
						, scale );
				}
			}
		}
	}

	//*********************************************************************************************

	castor::String const AssimpImporter::Name = cuT( "ASSIMP Importer" );

	AssimpImporter::AssimpImporter( Engine & engine )
		: MeshImporter( engine )
		, m_anonymous( 0 )
	{
	}

	AssimpImporter::~AssimpImporter()
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
		SubmeshSPtr submesh;
		Assimp::Importer importer;
		uint32_t flags = 0u;
		bool noOptim = false;
		auto found = m_parameters.get( "no_optimisations", noOptim );

		if ( !found || !noOptim )
		{
			flags = aiProcess_Triangulate
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
				flags |= aiProcess_GenSmoothNormals;
			}
		}

		if ( m_parameters.get( cuT( "tangent_space" ), tangentSpace ) && tangentSpace )
		{
			flags |= aiProcess_CalcTangentSpace;
		}

		// And have it read the given file with some postprocessing
		aiScene const * aiScene = importer.ReadFile( castor::string::stringCast< char >( m_fileName ), flags );

		if ( aiScene )
		{
			SkeletonSPtr skeleton = std::make_shared< Skeleton >( *mesh.getScene() );
			skeleton->setGlobalInverseTransform( castor::Matrix4x4f( &aiScene->mRootNode->mTransformation.Transpose().Inverse().a1 ) );

			if ( aiScene->HasMeshes() )
			{
				bool create = true;
				uint32_t aiMeshIndex = 0u;

				for ( auto aiMesh : castor::makeArrayView( aiScene->mMeshes, aiScene->mNumMeshes ) )
				{
					if ( create )
					{
						submesh = mesh.createSubmesh();
					}

					create = doProcessMesh( *mesh.getScene(), mesh, *skeleton, *aiMesh, aiMeshIndex, *aiScene, *submesh );
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
					for ( auto aiAnimation : castor::makeArrayView( aiScene->mAnimations, aiScene->mNumAnimations ) )
					{
						doProcessAnimation( mesh
							, m_fileName.getFileName()
							, *skeleton
							, *aiScene->mRootNode
							, *aiAnimation );
					}

					importer.FreeScene();

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
									auto scene = importer.ReadFile( file, flags );

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
				else
				{
					importer.FreeScene();
				}

				result = true;
			}
			else
			{
				importer.FreeScene();
			}
		}
		else
		{
			// The import failed, report it
			log::error << "Scene import failed : " << importer.GetErrorString() << std::endl;
		}

		return result;
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
		MaterialSPtr material;
		log::debug << cuT( "Mesh found: [" ) << aiMesh.mName.C_Str() << cuT( "]" ) << std::endl;

		if ( aiMesh.mMaterialIndex < aiScene.mNumMaterials )
		{
			material = doProcessMaterial( scene, *aiScene.mMaterials[aiMesh.mMaterialIndex] );
			log::debug << cuT( "  Material: [" ) << aiMesh.mMaterialIndex << cuT( " (" ) << material->getName() << cuT( ")]" ) << std::endl;
		}

		if ( aiMesh.HasFaces() && aiMesh.HasPositions() && material )
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
				submesh.setDefaultMaterial( material );
				submesh.addPoints( doCreateVertexBuffer( aiMesh ) );

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

				if ( !aiMesh.mNormals )
				{
					mapping->computeNormals( true );
				}
				else if ( !aiMesh.mTangents )
				{
					mapping->computeTangentsFromNormals();
				}

				submesh.setIndexMapping( mapping );

				if ( aiScene.HasAnimations() )
				{
					for ( auto aiAnimation : castor::makeArrayView( aiScene.mAnimations, aiScene.mNumAnimations ) )
					{
						auto it = std::find_if( aiAnimation->mMeshChannels
							, aiAnimation->mMeshChannels + aiAnimation->mNumMeshChannels
							, [this, &aiMesh, &submesh]( aiMeshAnim const * p_aiMeshAnim )
							{
								return p_aiMeshAnim->mName == aiMesh.mName;
							} );

						if ( it != aiAnimation->mMeshChannels + aiAnimation->mNumMeshChannels )
						{
							doProcessAnimationMeshes( mesh, submesh, aiMesh, *( *it ) );
						}
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
		, aiMeshAnim const & aiMeshAnim )
	{
		if ( aiMeshAnim.mNumKeys )
		{
			castor::String name{ castor::string::stringCast< xchar >( aiMeshAnim.mName.C_Str() ) };
			log::debug << cuT( "  Mesh Animation found: [" ) << name << cuT( "]" ) << std::endl;
			auto & animation = mesh.createAnimation( name );
			MeshAnimationSubmesh animSubmesh{ animation, submesh };
			animation.addChild( std::move( animSubmesh ) );

			for ( auto aiKey : castor::makeArrayView( aiMeshAnim.mKeys, aiMeshAnim.mNumKeys ) )
			{
				MeshAnimationKeyFrameUPtr keyFrame = std::make_unique< MeshAnimationKeyFrame >( animation
					, castor::Milliseconds{ int64_t( aiKey.mTime * 1000 ) } );
				keyFrame->addSubmeshBuffer( submesh, doCreateVertexBuffer( *aiMesh.mAnimMeshes[aiKey.mValue] ) );
				animation.addKeyFrame( std::move( keyFrame ) );
			}
		}
	}

	MaterialSPtr AssimpImporter::doProcessMaterial( Scene & scene
		, aiMaterial const & aiMaterial )
	{
		MaterialSPtr result;
		auto & cache = scene.getMaterialView();
		aiString mtlname;
		aiMaterial.Get( AI_MATKEY_NAME, mtlname );
		castor::String name = castor::string::stringCast< xchar >( mtlname.C_Str() );

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
			result = cache.add( name, scene.getPassesType() );
			auto pass = result->createPass();
			doProcessMaterialPass( *pass
				, aiMaterial
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
				auto mtx = aiBone.mOffsetMatrix;
				doAddBone( name, castor::Matrix4x4f{ &mtx.Transpose().a1 }, skeleton, index );
			}
			else
			{
				index = m_mapBoneByID[name];
				aiMatrix4x4 mtx{ aiBone.mOffsetMatrix };
				CU_Ensure( m_arrayBones[index]->getOffsetMatrix() == castor::Matrix4x4f( &mtx.Transpose().a1 ) );
			}

			for ( auto weight : castor::makeArrayView( aiBone.mWeights, aiBone.mNumWeights ) )
			{
				arrayVertices[weight.mVertexId].addBoneData( index, float( weight.mWeight ) );
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
		log::debug << cuT( "  Skeleton Animation found: [" ) << name << cuT( "]" ) << std::endl;

		if ( name.empty() )
		{
			name = animName;
		}

		auto & animation = skeleton.createAnimation( name );
		int64_t ticksPerMilliSecond = int64_t( aiAnimation.mTicksPerSecond ? aiAnimation.mTicksPerSecond : 25 );
		SkeletonAnimationKeyFrameMap keyframes;
		SkeletonAnimationObjectSet notAnimated;
		doProcessAnimationNodes( mesh
			, animation
			, ticksPerMilliSecond
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
		, int64_t ticksPerMilliSecond
		, Skeleton & skeleton
		, aiNode const & aiNode
		, aiAnimation const & aiAnimation
		, SkeletonAnimationObjectSPtr parent
		, SkeletonAnimationKeyFrameMap & keyFrames
		, SkeletonAnimationObjectSet & notAnimated )
	{
		castor::String name = castor::string::stringCast< xchar >( aiNode.mName.data );
		const aiNodeAnim * aiNodeAnim = doFindNodeAnim( aiAnimation, name );
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
				CU_Ensure( object->getNodeTransform() == castor::Matrix4x4f( &aiNode.mTransformation.a1 ).getTransposed() );
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
			CU_Ensure( object->getNodeTransform() == castor::Matrix4x4f( &aiNode.mTransformation.a1 ).getTransposed() );
			CU_Ensure( object->getParent() == parent || object == parent );
			added = false;
		}

		if ( object && added )
		{
			if ( parent && object != parent )
			{
				parent->addChild( object );
			}

			object->setNodeTransform( castor::Matrix4x4f( &aiNode.mTransformation.a1 ).getTransposed() );
		}

		if ( aiNodeAnim )
		{
			doProcessAnimationNodeKeys( *aiNodeAnim
				, ticksPerMilliSecond
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
				, ticksPerMilliSecond
				, skeleton
				, *node
				, aiAnimation
				, object
				, keyFrames
				, notAnimated );
		}
	}

	template< typename KeyT >
	void gatherTimes( KeyT const * keys
		, uint32_t count
		, int64_t ticksPerMilliSecond
		, std::set< castor::Milliseconds > & times )
	{
		for ( auto & key : castor::makeArrayView( keys, count ) )
		{
			if ( key.mTime >= 0.0 )
			{
				auto time = castor::Milliseconds{ int64_t( key.mTime * 1000 ) } / ticksPerMilliSecond;
				times.insert( time );
			}
		}
	}

	void AssimpImporter::doProcessAnimationNodeKeys( aiNodeAnim const & aiNodeAnim
		, int64_t ticksPerMilliSecond
		, SkeletonAnimationObject & object
		, SkeletonAnimation & animation
		, SkeletonAnimationKeyFrameMap & keyframes )
	{
		std::set< castor::Milliseconds > times;
		gatherTimes( aiNodeAnim.mPositionKeys
			, aiNodeAnim.mNumPositionKeys
			, ticksPerMilliSecond
			, times );
		gatherTimes( aiNodeAnim.mScalingKeys
			, aiNodeAnim.mNumScalingKeys
			, ticksPerMilliSecond
			, times );
		gatherTimes( aiNodeAnim.mRotationKeys
			, aiNodeAnim.mNumRotationKeys
			, ticksPerMilliSecond
			, times );

		auto translates = doProcessVec3Keys( aiNodeAnim.mPositionKeys
			, aiNodeAnim.mNumPositionKeys
			, ticksPerMilliSecond
			, times );
		auto scales = doProcessVec3Keys( aiNodeAnim.mScalingKeys
			, aiNodeAnim.mNumScalingKeys
			, ticksPerMilliSecond
			, times );
		auto rotates = doProcessQuatKeys( aiNodeAnim.mRotationKeys
			, aiNodeAnim.mNumRotationKeys
			, ticksPerMilliSecond
			, times );
		doSynchroniseKeys( translates
			, scales
			, rotates
			, times
			, getEngine()->getRenderLoop().getWantedFps()
			, ticksPerMilliSecond
			, object
			, animation
			, keyframes );
	}

	//*********************************************************************************************
}
