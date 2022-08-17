/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslUtils_H___
#define ___C3D_GlslUtils_H___

#include "SdwModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"
#include "Castor3D/Shader/Ubos/UbosModule.hpp"

#include <ShaderWriter/CompositeTypes/Function.hpp>

namespace castor3d::shader
{
	class Utils
	{
	public:
		C3D_API explicit Utils( sdw::ShaderWriter & writer );

		C3D_API LightingModelPtr createLightingModel( Engine const & engine
			, castor::String const & name
			, ShadowOptions shadowsOptions
			, SssProfiles const * sssProfiles
			, bool enableVolumetric );

		C3D_API sdw::Float saturate( sdw::Float const v );
		C3D_API sdw::Vec3 saturate( sdw::Vec3 const v );
		C3D_API sdw::Vec2 topDownToBottomUp( sdw::Vec2 const texCoord );
		C3D_API sdw::Vec3 topDownToBottomUp( sdw::Vec3 const texCoord );
		C3D_API sdw::Vec4 topDownToBottomUp( sdw::Vec4 const texCoord );
		C3D_API sdw::Vec2 negateTopDownToBottomUp( sdw::Vec2 const texCoord );
		C3D_API sdw::Vec3 negateTopDownToBottomUp( sdw::Vec3 const texCoord );
		C3D_API sdw::Vec4 negateTopDownToBottomUp( sdw::Vec4 const texCoord );
		C3D_API sdw::Vec2 calcTexCoord( sdw::Vec2 const renderPos
			, sdw::Vec2 const renderSize );
		C3D_API sdw::Vec3 applyGamma( sdw::Float const gamma
			, sdw::Vec3 const HDR );
		C3D_API sdw::Vec3 removeGamma( sdw::Float const gamma
			, sdw::Vec3 const sRGB );
		C3D_API void compute2DMapsContributions( FilteredTextureFlags const & flags
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const maps
			, sdw::Vec3 const texCoords
			, sdw::Vec3 & colour
			, sdw::Float & opacity );
		C3D_API sdw::Float remap( sdw::Float const originalValue
			, sdw::Float const originalMin
			, sdw::Float const originalMax
			, sdw::Float const newMin
			, sdw::Float const newMax );
		C3D_API sdw::Float threshold( sdw::Float const v
			, sdw::Float const t );
		C3D_API sdw::Float beer( sdw::Float const d );
		C3D_API sdw::Float powder( sdw::Float const d );
		C3D_API sdw::Float powder( sdw::Float const d
			, sdw::Float const cosTheta );
		C3D_API sdw::Vec4 sampleMap( PassFlags const & passFlags
			, sdw::CombinedImage2DRgba32 const map
			, sdw::Vec2 const texCoords );
		C3D_API sdw::Vec4 sampleMap( PassFlags const & passFlags
			, sdw::CombinedImage2DRgba32 const map
			, DerivTex const texCoords );
		C3D_API sdw::RetVec2 transformUV( TextureConfigData const & config
			, TextureAnimData const & anim
			, sdw::Vec2 const uv );
		C3D_API sdw::RetVec3 transformUVW( TextureConfigData const & config
			, TextureAnimData const & anim
			, sdw::Vec3 const uv );

		C3D_API void swap( sdw::Float const A, sdw::Float const & B );
		C3D_API sdw::RetFloat distanceSquared( sdw::Vec2 const A, sdw::Vec2 const & B );
		C3D_API sdw::RetFloat distanceSquared( sdw::Vec3 const A, sdw::Vec3 const & B );
		C3D_API sdw::RetVec3 calcVSPosition( sdw::Vec2 const & uv
			, sdw::Float const & depth
			, sdw::Mat4 const & invProj );
		C3D_API sdw::RetVec3 calcWSPosition( sdw::Vec2 const & uv
			, sdw::Float const & depth
			, sdw::Mat4 const & invViewProj );
		C3D_API sdw::RetVec3 getMapNormal( sdw::Vec2 const & uv
			, sdw::Vec3 const & normal
			, sdw::Vec3 const & position );
		C3D_API sdw::RetFloat rescaleDepth( sdw::Float const & depth
			, sdw::Float const & nearPlane
			, sdw::Float const & farPlane );
		C3D_API sdw::RetVec4 computeAccumulation( sdw::Float const & depth
			, sdw::Vec3 const & colour
			, sdw::Float const & alpha
			, sdw::Float const & nearPlane
			, sdw::Float const & farPlane
			, sdw::Float const & accumulationOperator );
		C3D_API sdw::RetVec3 fresnelSchlick( sdw::Float const & product
			, sdw::Vec3 const & f0 );
		C3D_API void parallaxMapping( sdw::Vec2 & texCoords
			, sdw::Vec3 const & viewDir
			, sdw::CombinedImage2DRgba32 const & heightMap
			, TextureConfigData const & textureConfig );
		C3D_API void parallaxMapping( DerivTex & texCoords
			, sdw::Vec3 const & viewDir
			, sdw::CombinedImage2DRgba32 const & heightMap
			, TextureConfigData const & textureConfig );
		C3D_API void parallaxMapping( sdw::Vec2 texCoords
			, sdw::Vec2 const & dx
			, sdw::Vec2 const & dy
			, sdw::Vec3 const & viewDir
			, sdw::CombinedImage2DRgba32 const & heightMap
			, TextureConfigData const & textureConfig );
		C3D_API sdw::RetFloat parallaxShadow( sdw::Vec3 const & lightDir
			, sdw::Vec2 const & initialTexCoord
			, sdw::Float const & initialHeight
			, sdw::CombinedImage2DRgba32 const & heightMap
			, TextureConfigData const & textureConfig );
		C3D_API sdw::RetVec4 clipToScreen( sdw::Vec4 const & in );

		C3D_API sdw::RetBoolean isSaturated( sdw::Vec3 const & p );
		C3D_API sdw::RetBoolean isSaturated( sdw::IVec3 const & p
			, sdw::Int const & imax );
		/**
		*\~english
		*\brief
		*	Encode HDR color to a 32 bit uint.
		*	Alpha is 1 bit + 7 bit HDR remapping.
		*/
		C3D_API sdw::RetUInt encodeColor( sdw::Vec4 const & color );
		/**
		*\~english
		*\brief
		*	Encode specified normal (normalized) into 32 bits uint.
		*	Each axis of the normal is encoded into 9 bits (1 for the sign/ 8 for the value).
		*/
		C3D_API sdw::RetUInt encodeNormal( sdw::Vec3 const & normal );
		C3D_API sdw::RetVec4 decodeColor( sdw::UInt const & colorMask );
		C3D_API sdw::RetVec3 decodeNormal( sdw::UInt const & normalMask );
		/**
		*\~english
		*\brief
		*	Converts a 32 bit float to store as 4 8 bit floats.
		*/
		C3D_API sdw::RetVec4 encodeFloatRGBA( sdw::Float const & v );
		/**
		*\~english
		*\brief
		*	3D array index to flattened 1D array index
		*/
		C3D_API sdw::RetUInt flatten( sdw::UVec3 const & p
			, sdw::UVec3 const & dim );
		/**
		*\~english
		*\brief
		*	Flattened array index to 3D array index
		*/
		C3D_API sdw::RetUVec3 unflatten( sdw::UInt const & p
			, sdw::UVec3 const & dim );

		C3D_API static sdw::Mat3 getTBN( sdw::Vec3 const & normal
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent );
		C3D_API static bool isGeometryMap( TextureFlags const & flags
			, PassFlags const & passFlags );
		C3D_API static bool isGeometryOnlyMap( TextureFlags const & flags
			, PassFlags const & passFlags );

	private:
		sdw::RetVec4 sampleUntiled( sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec2 const & texCoords
			, sdw::Vec2 const & ddx
			, sdw::Vec2 const & ddy );
		sdw::RetVec4 sampleUntiled( sdw::CombinedImage2DRgba32 const & map
			, sdw::Vec2 const & texCoords );
		sdw::RetVec4 sampleUntiled( sdw::CombinedImage2DRgba32 const & map
			, DerivTex const & texCoords );

	private:
		sdw::ShaderWriter & m_writer;
		sdw::Function< sdw::Void
			, sdw::InOutFloat
			, sdw::InOutFloat > m_swap1F;
		sdw::Function< sdw::Float
			, sdw::InVec2
			, sdw::InVec2 > m_distanceSquared2F;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3 > m_distanceSquared3F;
		sdw::Function< sdw::Vec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InMat4 > m_calcVSPosition;
		sdw::Function< sdw::Vec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InMat4 > m_calcWSPosition;
		sdw::Function< sdw::Vec3
			, sdw::InVec2
			, sdw::InVec3
			, sdw::InVec3 > m_getMapNormal;
		sdw::Function< sdw::Float
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_rescaleDepth;
		sdw::Function< sdw::Vec4
			, sdw::InFloat
			, sdw::InVec3
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat
			, sdw::InFloat > m_computeAccumulation;
		sdw::Function< sdw::Vec3
			, sdw::InFloat
			, sdw::InVec3 > m_fresnelSchlick;
		sdw::Function< sdw::Vec3
			, sdw::InVec3 > m_invertNormal;
		sdw::Function< sdw::Vec2
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InVec3
			, sdw::InCombinedImage2DRgba32
			, InTextureConfigData > m_parallaxMapping;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InCombinedImage2DRgba32
			, InTextureConfigData > m_parallaxShadow;
		sdw::Function< sdw::Boolean
			, sdw::InVec3 > m_isSaturated3D;
		sdw::Function< sdw::Boolean
			, sdw::InIVec3
			, sdw::InInt > m_isSaturated3DImg;
		sdw::Function< sdw::UInt
			, sdw::InVec4 > m_encodeColor;
		sdw::Function< sdw::UInt
			, sdw::InVec3 > m_encodeNormal;
		sdw::Function< sdw::Vec4
			, sdw::InUInt > m_decodeColor;
		sdw::Function< sdw::Vec3
			, sdw::InUInt > m_decodeNormal;
		sdw::Function< sdw::Vec4
			, sdw::InFloat > m_encodeFloatRGBA;
		sdw::Function< sdw::UInt
			, sdw::InUVec3
			, sdw::InUVec3 > m_flatten3D;
		sdw::Function< sdw::UVec3
			, sdw::InUInt
			, sdw::InUVec3 > m_unflatten3D;
		sdw::Function< sdw::Vec4
			, sdw::InVec4 > m_clipToScreen;
		sdw::Function< sdw::Vec4
			, sdw::InVec2 > m_hash4;
		sdw::Function< sdw::Vec4
			, sdw::InCombinedImage2DRgba32
			, sdw::InVec2
			, sdw::InVec2
			, sdw::InVec2 > m_sampleUntiled;
		sdw::Function< sdw::Vec2
			, InTextureConfigData
			, InTextureAnimData
			, sdw::InVec2 > m_transformUV;
		sdw::Function< sdw::Vec3
			, InTextureConfigData
			, InTextureAnimData
			, sdw::InVec3 > m_transformUVW;
	};
}

#endif
