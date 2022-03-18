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
		C3D_API explicit Utils( sdw::ShaderWriter & writer
			, Engine const & engine );

		C3D_API LightingModelPtr createLightingModel( castor::String const & name
			, ShadowOptions shadowsOptions
			, SssProfiles const * sssProfiles
			, bool isOpaqueProgram );

		C3D_API void swap( sdw::Float const A, sdw::Float const & B );
		C3D_API sdw::Float saturate( sdw::Float const v );
		C3D_API sdw::Vec3 saturate( sdw::Vec3 const v );
		C3D_API sdw::Float distanceSquared( sdw::Vec2 const A, sdw::Vec2 const & B );
		C3D_API sdw::Float distanceSquared( sdw::Vec3 const A, sdw::Vec3 const & B );
		C3D_API sdw::Vec2 topDownToBottomUp( sdw::Vec2 const & texCoord );
		C3D_API sdw::Vec3 topDownToBottomUp( sdw::Vec3 const & texCoord );
		C3D_API sdw::Vec4 topDownToBottomUp( sdw::Vec4 const & texCoord );
		C3D_API sdw::Vec2 negateTopDownToBottomUp( sdw::Vec2 const & texCoord );
		C3D_API sdw::Vec3 negateTopDownToBottomUp( sdw::Vec3 const & texCoord );
		C3D_API sdw::Vec4 negateTopDownToBottomUp( sdw::Vec4 const & texCoord );
		C3D_API sdw::Vec2 calcTexCoord( sdw::Vec2 const & renderPos
			, sdw::Vec2 const & renderSize );
		C3D_API sdw::Vec3 calcVSPosition( sdw::Vec2 const & uv
			, sdw::Float const & depth
			, sdw::Mat4 const & invProj );
		C3D_API sdw::Vec3 calcWSPosition( sdw::Vec2 const & uv
			, sdw::Float const & depth
			, sdw::Mat4 const & invViewProj );
		C3D_API sdw::Vec3 applyGamma( sdw::Float const & gamma
			, sdw::Vec3 const & HDR );
		C3D_API sdw::Vec3 removeGamma( sdw::Float const & gamma
			, sdw::Vec3 const & sRGB );
		C3D_API sdw::Vec3 getMapNormal( sdw::Vec2 const & uv
			, sdw::Vec3 const & normal
			, sdw::Vec3 const & position );
		C3D_API sdw::Float rescaleDepth( sdw::Float const & depth
			, sdw::Float const & nearPlane
			, sdw::Float const & farPlane );
		C3D_API sdw::Vec4 computeAccumulation( sdw::Float const & depth
			, sdw::Vec3 const & colour
			, sdw::Float const & alpha
			, sdw::Float const & nearPlane
			, sdw::Float const & farPlane
			, sdw::Float const & accumulationOperator );
		C3D_API sdw::Vec3 fresnelSchlick( sdw::Float const & product
			, sdw::Vec3 const & f0 );
		C3D_API void compute2DMapsContributions( FilteredTextureFlags const & flags
			, TextureConfigurations const & textureConfigs
			, TextureAnimations const & textureAnims
			, sdw::Array< sdw::CombinedImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & colour
			, sdw::Float & opacity );
		C3D_API sdw::Vec2 parallaxMapping( sdw::Vec2 const & texCoords
			, sdw::Vec3 const & viewDir
			, sdw::CombinedImage2DRgba32 const & heightMap
			, TextureConfigData const & textureConfig );
		C3D_API sdw::Vec4 clipToScreen( sdw::Vec4 const & in );
		/**
		 *\~english
		 *\brief		Writes the alpha function in GLSL.
		 *\param		alphaFunc	The alpha function.
		 *\param[in]	alpha		The alpha TypeEnum.
		 *\param[in]	alphaRef	The alpha comparison reference TypeEnum.
		 *\param[in]	opaque		\p true for opaque nodes, \p false for transparent ones.
		 *\~french
		 *\brief		Ecrit la fonction d'opacité en GLSL.
		 *\param		alphaFunc	La fonction d'opacité.
		 *\param[in]	alpha		La valeur d'opacité.
		 *\param[in]	alphaRef	La valeur de référence pour la comparaison alpha.
		 *\param[in]	opaque		\p true pour les noeuds opaques, \p false pour les transparents.
		 */
		C3D_API void applyAlphaFunc( VkCompareOp alphaFunc
			, sdw::Float & alpha
			, sdw::Float const & alphaRef
			, bool opaque = true );

		C3D_API sdw::Boolean isSaturated( sdw::Vec3 const & p );
		C3D_API sdw::Boolean isSaturated( sdw::IVec3 const & p
			, sdw::Int const & imax );
		/**
		*\~english
		*\brief
		*	Encode HDR color to a 32 bit uint.
		*	Alpha is 1 bit + 7 bit HDR remapping.
		*/
		C3D_API sdw::UInt encodeColor( sdw::Vec4 const & color );
		/**
		*\~english
		*\brief
		*	Encode specified normal (normalized) into 32 bits uint.
		*	Each axis of the normal is encoded into 9 bits (1 for the sign/ 8 for the value).
		*/
		C3D_API sdw::UInt encodeNormal( sdw::Vec3 const & normal );
		C3D_API sdw::Vec4 decodeColor( sdw::UInt const & colorMask );
		C3D_API sdw::Vec3 decodeNormal( sdw::UInt const & normalMask );
		/**
		*\~english
		*\brief
		*	3D array index to flattened 1D array index
		*/
		C3D_API sdw::UInt flatten( sdw::UVec3 const & p
			, sdw::UVec3 const & dim );
		/**
		*\~english
		*\brief
		*	Flattened array index to 3D array index
		*/
		C3D_API sdw::UVec3 unflatten( sdw::UInt const & p
			, sdw::UVec3 const & dim );

		C3D_API static sdw::Mat3 getTBN( sdw::Vec3 const & normal
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent );
		C3D_API static bool isGeometryMap( TextureFlags const & flags
			, PassFlags const & passFlags );
		C3D_API static bool isGeometryOnlyMap( TextureFlags const & flags
			, PassFlags const & passFlags );

	private:
		void declareSwap1F();
		void declareDistanceSquared2F();
		void declareDistanceSquared3F();
		void declareCalcTexCoord();
		void declareCalcVSPosition();
		void declareCalcWSPosition();
		void declareApplyGamma();
		void declareRemoveGamma();
		void declareRescaleDepth();
		void declareGetMapNormal();
		void declareComputeAccumulation();
		void declareFresnelSchlick();
		// 1.0 - y
		void declareInvertVec2Y();
		void declareInvertVec3Y();
		void declareInvertVec4Y();
		// -y
		void declareNegateVec2Y();
		void declareNegateVec3Y();
		void declareNegateVec4Y();

		void declareParallaxMappingFunc();
		void declareParallaxShadowFunc();

		void declareIsSaturated();
		void declareEncodeColor();
		void declareEncodeNormal();
		void declareDecodeColor();
		void declareDecodeNormal();
		void declareFlatten();
		void declareUnflatten();
		void declareIsSaturatedImg();
		void declareClipToScreen();

	private:
		sdw::ShaderWriter & m_writer;
		Engine const & m_engine;
		sdw::Function< sdw::Void
			, sdw::InOutFloat
			, sdw::InOutFloat > m_swap1F;
		sdw::Function< sdw::Float
			, sdw::InVec2
			, sdw::InVec2 > m_distanceSquared2F;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec3 > m_distanceSquared3F;
		sdw::Function< sdw::Vec2
			, sdw::InVec2
			, sdw::InVec2 > m_calcTexCoord;
		sdw::Function< sdw::Vec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InMat4 > m_calcVSPosition;
		sdw::Function< sdw::Vec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InMat4 > m_calcWSPosition;
		sdw::Function< sdw::Vec3
			, sdw::InFloat
			, sdw::InVec3 > m_applyGamma;
		sdw::Function< sdw::Vec3
			, sdw::InFloat
			, sdw::InVec3 > m_removeGamma;
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
		sdw::Function< sdw::Vec2
			, sdw::InVec2 > m_invertVec2Y;
		sdw::Function< sdw::Vec3
			, sdw::InVec3 > m_invertVec3Y;
		sdw::Function< sdw::Vec4
			, sdw::InVec4 > m_invertVec4Y;
		sdw::Function< sdw::Vec3
			, sdw::InVec3 > m_invertNormal;
		sdw::Function< sdw::Vec2
			, sdw::InVec2 > m_negateVec2Y;
		sdw::Function< sdw::Vec3
			, sdw::InVec3 > m_negateVec3Y;
		sdw::Function< sdw::Vec4
			, sdw::InVec4 > m_negateVec4Y;
		sdw::Function< sdw::Vec2
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
		sdw::Function< sdw::UInt
			, sdw::InUVec3
			, sdw::InUVec3 > m_flatten3D;
		sdw::Function< sdw::UVec3
			, sdw::InUInt
			, sdw::InUVec3 > m_unflatten3D;
		sdw::Function< sdw::Vec4
			, sdw::InVec4 > m_clipToScreen;
	};
}

#endif
