#include "AssimpImporter/AssimpImporter.hpp"

#include <Castor3D//Animation/Interpolator.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimation.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationKeyFrame.hpp>
#include <Castor3D/Model/Mesh/Animation/MeshAnimationSubmesh.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimation.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationKeyFrame.hpp>
#include <Castor3D/Model/Skeleton/Animation/SkeletonAnimationBone.hpp>
#include <Castor3D/Cache/GeometryCache.hpp>
#include <Castor3D/Cache/MaterialCache.hpp>
#include <Castor3D/Cache/MeshCache.hpp>
#include <Castor3D/Cache/PluginCache.hpp>
#include <Castor3D/Cache/SceneCache.hpp>
#include <Castor3D/Cache/SceneNodeCache.hpp>
#include <Castor3D/Cache/CacheView.hpp>
#include <Castor3D/Event/Frame/InitialiseEvent.hpp>
#include <Castor3D/Material/Pass/PhongPass.hpp>
#include <Castor3D/Material/Pass/MetallicRoughnessPbrPass.hpp>
#include <Castor3D/Material/Pass/SpecularGlossinessPbrPass.hpp>
#include <Castor3D/Model/Skeleton/Bone.hpp>
#include <Castor3D/Plugin/ImporterPlugin.hpp>
#include <Castor3D/Render/RenderLoop.hpp>
#include <Castor3D/Scene/Geometry.hpp>
#include <Castor3D/Scene/Scene.hpp>

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Log/Logger.hpp>

#include <assimp/version.h>

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

		void doProcessPassBaseComponents( PhongPass & pass
			, aiMaterial const & aiMaterial
			, aiColor3D const & emissive )
		{
			aiColor3D ambient( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_AMBIENT, ambient );
			aiColor3D diffuse( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_DIFFUSE, diffuse );
			aiColor3D specular( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_SPECULAR, specular );
			float shininess = 0.5f;
			aiMaterial.Get( AI_MATKEY_SHININESS, shininess );
			float shininessStrength = 1.0f;
			aiMaterial.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength );

			if ( ambient.IsBlack()
				&& diffuse.IsBlack()
				&& specular.IsBlack()
				&& emissive.IsBlack() )
			{
				diffuse.r = 1.0;
				diffuse.g = 1.0;
				diffuse.b = 1.0;
			}

			pass.setDiffuse( castor::RgbColour::fromComponents( diffuse.r
				, diffuse.g
				, diffuse.b ) );
			pass.setSpecular( castor::RgbColour::fromComponents( specular.r * shininessStrength
				, specular.g * shininessStrength
				, specular.b * shininessStrength ) );

			if ( shininess > 0 )
			{
				pass.setShininess( shininess );
			}
		}

		void doProcessPassBaseComponents( SpecularGlossinessPbrPass & pass
			, aiMaterial const & aiMaterial
			, aiColor3D const & emissive )
		{
			aiColor3D diffuse( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_DIFFUSE, diffuse );
			aiColor3D specular( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_SPECULAR, specular );
			float shininess = 0.5f;
			aiMaterial.Get( AI_MATKEY_SHININESS, shininess );
			float shininessStrength = 1.0f;
			aiMaterial.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength );

			if ( diffuse.IsBlack()
				&& specular.IsBlack()
				&& emissive.IsBlack() )
			{
				diffuse.r = 1.0;
				diffuse.g = 1.0;
				diffuse.b = 1.0;
			}

			pass.setDiffuse( castor::RgbColour::fromComponents( diffuse.r
				, diffuse.g
				, diffuse.b ) );
			pass.setSpecular( castor::RgbColour::fromComponents( specular.r
				, specular.g
				, specular.b ) );
			shininess *= shininessStrength;
			shininess /= PhongPass::MaxShininess;

			if ( shininess > 0 )
			{
				pass.setGlossiness( shininess );
			}
		}

		void doProcessPassBaseComponents( MetallicRoughnessPbrPass & pass
			, aiMaterial const & aiMaterial
			, aiColor3D const & emissive )
		{
			aiColor3D albedo( 1, 1, 1 );
			aiMaterial.Get( AI_MATKEY_COLOR_DIFFUSE, albedo );
			float metallic = 0.5f;
			aiMaterial.Get( AI_MATKEY_COLOR_SPECULAR, metallic );
			float shininess = 0.5f;
			aiMaterial.Get( AI_MATKEY_SHININESS, shininess );
			float shininessStrength = 1.0f;
			aiMaterial.Get( AI_MATKEY_SHININESS_STRENGTH, shininessStrength );

			if ( albedo.IsBlack()
				&& emissive.IsBlack() )
			{
				albedo.r = 1.0;
				albedo.g = 1.0;
				albedo.b = 1.0;
			}

			pass.setAlbedo( castor::RgbColour::fromComponents( albedo.r
				, albedo.g
				, albedo.b ) );
			pass.setMetallic( metallic );
			shininess *= shininessStrength;
			shininess /= PhongPass::MaxShininess;

			if ( shininess > 0 )
			{
				pass.setRoughness( 1.0f - shininess );
			}
		}

		void doProcessPassBaseComponents( Pass & pass
			, aiMaterial const & aiMaterial )
		{
			aiColor3D emissive( 1, 1, 1 );
			float opacity = 1;
			int twoSided = 0;
			aiMaterial.Get( AI_MATKEY_COLOR_EMISSIVE, emissive );
			aiMaterial.Get( AI_MATKEY_OPACITY, opacity );
			aiMaterial.Get( AI_MATKEY_TWOSIDED, twoSided );
			pass.setOpacity( opacity );
			pass.setTwoSided( twoSided != 0 );
			pass.setEmissive( float( castor::point::length( castor::Point3f{ emissive.r
				, emissive.g
				, emissive.b } ) ) );

			switch ( pass.getType() )
			{
			case MaterialType::ePhong:
				doProcessPassBaseComponents( static_cast< PhongPass & >( pass )
					, aiMaterial
					, emissive );
				break;
			case MaterialType::eSpecularGlossiness:
				doProcessPassBaseComponents( static_cast< SpecularGlossinessPbrPass & >( pass )
					, aiMaterial
					, emissive );
				break;
			case MaterialType::eMetallicRoughness:
				doProcessPassBaseComponents( static_cast< MetallicRoughnessPbrPass & >( pass )
					, aiMaterial
					, emissive );
				break;
			default:
				CU_Failure( "Unsupported MaterialType" );
				break;
			}
		}

		struct TextureInfo
		{
			castor::String name;
			aiUVTransform transform;
		};

		int clamp( int value, int max )
		{
			if ( value >= max )
			{
				value %= max;
			}

			while ( value < 0 )
			{
				value += max;
			}

			return value;
		}

		size_t getIndex( int x, int y, int width, int height )
		{
			return size_t( y * width + x );
		}

		// Source:
		// https://stackoverflow.com/questions/2368728/can-normal-maps-be-generated-from-a-texture
		// and
		// http://www.catalinzima.com/2008/01/converting-displacement-maps-into-normal-maps/
		float getHeight( castor::ArrayView< uint8_t const > const & data
			, int width
			, int height
			, int x
			, int y )
		{
			x = clamp( x, width );
			y = clamp( y, height );
			return float( data[getIndex( x, y, width, height )] ) / 255.0f;
		}

		uint8_t textureCoordinateToRgb( float value )
		{
			return uint8_t( ( value * 0.5 + 0.5 ) * 255.0 );
		}

		castor::Point4f calculateNormal( castor::ArrayView< uint8_t const > const & data
			, int width
			, int height
			, int x
			, int y )
		{
			const float strength = 8.0f;

			// surrounding pixels
			float tl = getHeight( data, width, height, x - 1, y - 1 ); // top left
			float  l = getHeight( data, width, height, x - 1, y );   // left
			float bl = getHeight( data, width, height, x - 1, y + 1 ); // bottom left
			float  t = getHeight( data, width, height, x, y - 1 );   // top
			float  c = getHeight( data, width, height, x, y );   // center
			float  b = getHeight( data, width, height, x, y + 1 );   // bottom
			float tr = getHeight( data, width, height, x + 1, y - 1 ); // top right
			float  r = getHeight( data, width, height, x + 1, y );   // right
			float br = getHeight( data, width, height, x + 1, y + 1 ); // bottom right

			// sobel filter
			const float dX = ( tr + 2.0 * r + br ) - ( tl + 2.0 * l + bl );
			const float dY = ( bl + 2.0 * b + br ) - ( tl + 2.0 * t + tr );
			const float dZ = 1.0 / strength;

			castor::Point3f n{ dX, dY, dZ };
			castor::point::normalise( n );
			return { n->x, n->y, n->z, c };
		}

		castor::ByteArray calculateNormals( castor::ArrayView< uint8_t const > const & data
			, castor::Size const & size )
		{
			castor::ByteArray result;
			result.resize( data.size() * 4u );
			auto width = int( size.getWidth() );
			auto height = int( size.getHeight() );

			for ( int x = 0; x < width; ++x )
			{
				for ( int y = 0; y < height; ++y )
				{
					auto n = calculateNormal( data, width, height, x, y );

					 // convert to rgb
					auto coord = getIndex( x, y, width, height ) * 4;
					result[coord + 0] = textureCoordinateToRgb( n->x );
					result[coord + 1] = textureCoordinateToRgb( n->y );
					result[coord + 2] = textureCoordinateToRgb( n->z );
					result[coord + 3] = uint8_t( n->w * 255.0f );
				}
			}

			return result;
		}

		void convertToNormalMap( TextureInfo & info
			, castor3d::MeshImporter const & importer
			, castor3d::TextureConfiguration & config )
		{
			if ( !info.name.empty() )
			{
				auto path = castor::Path{ info.name };

				if ( auto image = importer.loadImage( path ) )
				{
					log::info << "Converting height map to normal map." << std::endl;
					auto buffer = castor::PxBufferBase::create( image->getDimensions()
						, castor::PixelFormat::eR8_UNORM
						, image->getPxBuffer().getConstPtr()
						, image->getPixelFormat()
						, image->getPxBuffer().getAlign() );
					auto normals = calculateNormals( castor::makeArrayView( buffer->getConstPtr(), buffer->getSize() )
						, image->getDimensions() );
					auto nmlHeights = castor::PxBufferBase::create( buffer->getDimensions()
						, castor::PixelFormat::eR8G8B8A8_UNORM
						, normals.data()
						, castor::PixelFormat::eR8G8B8A8_UNORM );
					config.normalMask[0] = 0x00FFFFFF;
					config.heightMask[0] = 0xFF000000;
					path = image->getPath();
					path = path.getPath() / ( path.getFileName() + "_C3DNml.tga" );
					importer.getEngine()->getImageWriter().write( path, *nmlHeights );
					info.name = path;
				}
			}
		}

		void doLoadTexture( TextureInfo const & info
			, castor3d::Pass & pass
			, castor3d::TextureConfiguration const & config
			, castor3d::MeshImporter const & importer )
		{
			if ( !info.name.empty() )
			{
				auto texture = importer.loadTexture( castor::Path{ info.name }
					, config
					, pass );

				if ( texture )
				{
					log::debug << cuT( "  Texture: [" ) << texture->toString() << cuT( "]" ) << std::endl;
					texture->setTransform( { info.transform.mTranslation.x, info.transform.mTranslation.y, 0.0f }
						, castor::Angle::fromRadians( info.transform.mRotation )
						, { info.transform.mScaling.x, info.transform.mScaling.y, 1.0f } );
				}
			}
		}

		TextureInfo getTextureInfo( aiMaterial const & aiMaterial
			, aiTextureType type )
		{
			TextureInfo result;
			aiString name;
			aiMaterial.Get( AI_MATKEY_TEXTURE( type, 0 ), name );

			if ( name.length > 0 )
			{
				result.name = castor::string::stringCast< xchar >( name.C_Str() );
				aiMaterial.Get( AI_MATKEY_UVTRANSFORM( type, 0 ), result.transform );
			}

			return result;
		}

		void doProcessPassTextures( MetallicRoughnessPbrPass & pass
			, aiMaterial const & aiMaterial
			, MeshImporter const & importer
			, TextureInfo const & spcTex )
		{
			if ( aiGetVersionMajor() >= 4u )
			{
				static auto constexpr TextureType_METALNESS = aiTextureType( 15 );
				static auto constexpr TextureType_DIFFUSE_ROUGHNESS = aiTextureType( 16 );

				auto metTex = getTextureInfo( aiMaterial, TextureType_METALNESS );
				auto rghTex = getTextureInfo( aiMaterial, TextureType_DIFFUSE_ROUGHNESS );
				doLoadTexture( metTex, pass, TextureConfiguration::MetalnessTexture, importer );
				doLoadTexture( rghTex, pass, TextureConfiguration::RoughnessTexture, importer );
			}
			else
			{
				auto rghTex = getTextureInfo( aiMaterial, aiTextureType_SHININESS );
				doLoadTexture( spcTex, pass, TextureConfiguration::SpecularTexture, importer );
				doLoadTexture( rghTex, pass, TextureConfiguration::GlossinessTexture, importer );
			}
		}

		void doProcessPassTextures( SpecularGlossinessPbrPass & pass
			, aiMaterial const & aiMaterial
			, MeshImporter const & importer
			, TextureInfo const & spcTex )
		{
			auto glsTex = getTextureInfo( aiMaterial, aiTextureType_SHININESS );
			doLoadTexture( spcTex, pass, TextureConfiguration::SpecularTexture, importer );
			doLoadTexture( glsTex, pass, TextureConfiguration::GlossinessTexture, importer );
		}

		void doProcessPassTextures( PhongPass & pass
			, aiMaterial const & aiMaterial
			, MeshImporter const & importer
			, TextureInfo const & spcTex )
		{
			auto shnTex = getTextureInfo( aiMaterial, aiTextureType_SHININESS );
			doLoadTexture( spcTex, pass, TextureConfiguration::SpecularTexture, importer );
			doLoadTexture( shnTex, pass, TextureConfiguration::ShininessTexture, importer );
		}

		void doProcessPassTextures( Pass & pass
			, aiMaterial const & aiMaterial
			, MeshImporter const & importer )
		{
			auto spcTex = getTextureInfo( aiMaterial, aiTextureType_SPECULAR );
			auto difTex = getTextureInfo( aiMaterial, aiTextureType_DIFFUSE );
			auto emiTex = getTextureInfo( aiMaterial, aiTextureType_EMISSIVE );
			auto nmlTex = getTextureInfo( aiMaterial, aiTextureType_NORMALS );
			auto hgtTex = getTextureInfo( aiMaterial, aiTextureType_HEIGHT );
			auto opaTex = getTextureInfo( aiMaterial, aiTextureType_OPACITY );
			TextureInfo occTex;

			if ( aiGetVersionMajor() >= 4u )
			{
				static auto constexpr TextureType_AMBIENT_OCCLUSION = aiTextureType( 17 );
				occTex = getTextureInfo( aiMaterial, TextureType_AMBIENT_OCCLUSION );
			}

			if ( !difTex.name.empty()
				&& difTex.name.find( "_Cine_" ) != castor::String::npos
				&& difTex.name.find( "/MI_CH_" ) != castor::String::npos )
			{
				// Workaround for Collada textures.
				castor::String strGlob = difTex.name + cuT( ".tga" );
				castor::string::replace( strGlob, cuT( "/MI_CH_" ), cuT( "TX_CH_" ) );
				castor::String strDiff = strGlob;
				castor::String strNorm = strGlob;
				castor::String strSpec = strGlob;
				castor::String strOpac = strGlob;
				difTex.name = castor::string::replace( strDiff, cuT( "_Cine_" ), cuT( "_D_" ) );
				nmlTex.name = castor::string::replace( strNorm, cuT( "_Cine_" ), cuT( "_N_" ) );
				spcTex.name = castor::string::replace( strSpec, cuT( "_Cine_" ), cuT( "_S_" ) );
				opaTex.name = castor::string::replace( strOpac, cuT( "_Cine_" ), cuT( "_A_" ) );
				nmlTex.transform = difTex.transform;
				spcTex.transform = difTex.transform;
				opaTex.transform = difTex.transform;
			}

			doLoadTexture( difTex, pass, TextureConfiguration::DiffuseTexture, importer );
			doLoadTexture( emiTex, pass, TextureConfiguration::EmissiveTexture, importer );
			doLoadTexture( occTex, pass, TextureConfiguration::OcclusionTexture, importer );

			if ( !opaTex.name.empty() )
			{
				doLoadTexture( opaTex, pass, TextureConfiguration::OpacityTexture, importer );

				// force non 0.0 opacity when an opacity map is set
				if ( pass.getOpacity() == 0.0f )
				{
					pass.setOpacity( 1.0f );
				}
			}

			if ( !nmlTex.name.empty() )
			{
				doLoadTexture( nmlTex, pass, TextureConfiguration::NormalTexture, importer );
				doLoadTexture( hgtTex, pass, TextureConfiguration::HeightTexture, importer );
			}
			else
			{
				auto config = TextureConfiguration::NormalTexture;
				convertToNormalMap( hgtTex, importer, config );
				doLoadTexture( hgtTex, pass, config, importer );
			}

			switch ( pass.getType() )
			{
			case MaterialType::ePhong:
				doProcessPassTextures( static_cast< PhongPass & >( pass )
					, aiMaterial
					, importer
					, spcTex );
				break;
			case MaterialType::eSpecularGlossiness:
				doProcessPassTextures( static_cast< SpecularGlossinessPbrPass & >( pass )
					, aiMaterial
					, importer
					, spcTex );
				break;
			case MaterialType::eMetallicRoughness:
				doProcessPassTextures( static_cast< MetallicRoughnessPbrPass & >( pass )
					, aiMaterial
					, importer
					, spcTex );
				break;
			default:
				CU_Failure( "Unsupported MaterialType" );
				break;
			}
		}

		MaterialType convert( aiShadingMode shadingMode )
		{
			switch ( shadingMode )
			{
			case aiShadingMode_NoShading:
			case aiShadingMode_Minnaert:
			case aiShadingMode_OrenNayar:
			case aiShadingMode_Toon:
				return MaterialType::ePhong;

			case aiShadingMode_Flat:
			case aiShadingMode_Gouraud:
			case aiShadingMode_Blinn:
			case aiShadingMode_Phong:
			case aiShadingMode_Fresnel:
				return MaterialType::ePhong;

			case aiShadingMode::aiShadingMode_CookTorrance:
				return MaterialType::eMetallicRoughness;

			default:
				return MaterialType::ePhong;
			}
		}

		void doProcessMaterialPass( Pass & pass
			, aiMaterial const & aiMaterial
			, AssimpImporter & importer )
		{
			aiShadingMode shadingMode;
			aiMaterial.Get( AI_MATKEY_SHADING_MODEL, shadingMode );
			auto srcType = convert( shadingMode );
			auto dstType = pass.getType();

			if ( dstType != srcType )
			{
				log::warn << "Switching from " << getName( srcType ) << " to " << getName( dstType ) << " pass type." << std::endl;
			}

			doProcessPassBaseComponents( pass, aiMaterial );
			doProcessPassTextures( pass, aiMaterial, importer );

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

			if ( ticksPerMilliSecond / 1000 >= fps )
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
		uint32_t flags = aiProcess_Triangulate
			| aiProcess_JoinIdenticalVertices
			| aiProcess_OptimizeMeshes
			| aiProcess_OptimizeGraph
			| aiProcess_FixInfacingNormals
			| aiProcess_LimitBoneWeights
			| aiProcess_Debone;
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
			result = cache.add( name, scene.getMaterialsType() );
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
