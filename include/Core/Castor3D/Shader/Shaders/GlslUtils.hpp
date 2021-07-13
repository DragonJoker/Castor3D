/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslUtils_H___
#define ___C3D_GlslUtils_H___

#include "SdwModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

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
			, bool isOpaqueProgram );

		C3D_API void declareCalcTexCoord();
		C3D_API void declareCalcVSPosition();
		C3D_API void declareCalcWSPosition();
		C3D_API void declareApplyGamma();
		C3D_API void declareRemoveGamma();
		C3D_API void declareLineariseDepth();
		C3D_API void declareGetMapNormal();
		C3D_API void declareComputeAccumulation();
		C3D_API void declareFresnelSchlick();
		// 1.0 - y
		C3D_API void declareInvertVec2Y();
		C3D_API void declareInvertVec3Y();
		C3D_API void declareInvertVec4Y();
		// -y
		C3D_API void declareNegateVec2Y();
		C3D_API void declareNegateVec3Y();
		C3D_API void declareNegateVec4Y();

		C3D_API void declareEncodeMaterial();
		C3D_API void declareDecodeMaterial();
		C3D_API void declareDecodeReceiver();
		C3D_API void declareParallaxMappingFunc( PassFlags const & passFlags
			, TextureFlags const & textures );
		C3D_API void declareParallaxShadowFunc( PassFlags const & passFlags
			, TextureFlags const & textures );

		C3D_API void declareIsSaturated();
		C3D_API void declareEncodeColor();
		C3D_API void declareEncodeNormal();
		C3D_API void declareDecodeColor();
		C3D_API void declareDecodeNormal();
		C3D_API void declareFlatten();
		C3D_API void declareUnflatten();
		C3D_API void declareIsSaturatedImg();

		C3D_API sdw::Vec2 topDownToBottomUp( sdw::Vec2 const & texCoord )const;
		C3D_API sdw::Vec3 topDownToBottomUp( sdw::Vec3 const & texCoord )const;
		C3D_API sdw::Vec4 topDownToBottomUp( sdw::Vec4 const & texCoord )const;
		C3D_API sdw::Vec2 negateTopDownToBottomUp( sdw::Vec2 const & texCoord )const;
		C3D_API sdw::Vec3 negateTopDownToBottomUp( sdw::Vec3 const & texCoord )const;
		C3D_API sdw::Vec4 negateTopDownToBottomUp( sdw::Vec4 const & texCoord )const;
		C3D_API sdw::Vec2 calcTexCoord( sdw::Vec2 const & renderPos
			, sdw::Vec2 const & renderSize )const;
		C3D_API sdw::Vec3 calcVSPosition( sdw::Vec2 const & uv
			, sdw::Float const & depth
			, sdw::Mat4 const & invProj )const;
		C3D_API sdw::Vec3 calcWSPosition( sdw::Vec2 const & uv
			, sdw::Float const & depth
			, sdw::Mat4 const & invViewProj )const;
		C3D_API sdw::Vec3 applyGamma( sdw::Float const & gamma
			, sdw::Vec3 const & HDR )const;
		C3D_API sdw::Vec3 removeGamma( sdw::Float const & gamma
			, sdw::Vec3 const & sRGB )const;
		C3D_API sdw::Vec3 getMapNormal( sdw::Vec2 const & uv
			, sdw::Vec3 const & normal
			, sdw::Vec3 const & position )const;
		C3D_API sdw::Float lineariseDepth( sdw::Float const & depth
			, sdw::Float const & nearPlane
			, sdw::Float const & farPlane )const;
		C3D_API sdw::Vec4 computeAccumulation( sdw::Float const & depth
			, sdw::Vec3 const & colour
			, sdw::Float const & alpha
			, sdw::Float const & nearPlane
			, sdw::Float const & farPlane
			, sdw::Float const & accumulationOperator )const;
		C3D_API sdw::Vec3 fresnelSchlick( sdw::Float const & product
			, sdw::Vec3 const & f0 )const;
		C3D_API static sdw::Mat3 getTBN( sdw::Vec3 const & normal
			, sdw::Vec3 const & tangent
			, sdw::Vec3 const & bitangent );
		C3D_API void compute2DMapsContributions( FilteredTextureFlags const & flags
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & colour
			, sdw::Float & opacity );
		C3D_API void computeOpacityMapContribution( FilteredTextureFlags const & flags
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 const & texCoords
			, sdw::Float & opacity );
		C3D_API void computeGeometryMapsContributions( FilteredTextureFlags const & flags
			, PassFlags const & passFlags
			, TextureConfigurations const & textureConfigs
			, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
			, sdw::Vec3 & texCoords
			, sdw::Float & opacity
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition );
		C3D_API sdw::Vec4 computeCommonMapContribution( TextureFlags const & textureFlags
			, PassFlags const & passFlags
			, std::string const & name
			, shader::TextureConfigData const & config
			, sdw::SampledImage2DRgba32 const & map
			, sdw::Float const & gamma
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion
			, sdw::Float & transmittance
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition );
		C3D_API sdw::Vec4 computeCommonMapVoxelContribution( TextureFlags const & textureFlags
			, PassFlags const & passFlags
			, std::string const & name
			, shader::TextureConfigData const & config
			, sdw::SampledImage2DRgba32 const & map
			, sdw::Float const & gamma
			, sdw::Vec3 const & texCoords
			, sdw::Vec3 & emissive
			, sdw::Float & opacity
			, sdw::Float & occlusion );
		/**
		 *\~english
		 *\brief		Calls the GLSL function used to encode the material specifics into a vec4.
		 *\param[in]	receiver	The shadow receiver status.
		 *\param[in]	reflection	The reflection status.
		 *\param[in]	refraction	The refraction status.
		 *\param[in]	envMapIndex	The environment map index.
		 *\param[in]	encoded		The variable that will receive the encoded value.
		 *\~french
		 *\brief		Appelle la fonction GLSL utilisée pour encoder les spécificités d'un matériau dans un vec4.
		 *\param[in]	receiver	Le statut de receveur d'ombres.
		 *\param[in]	reflection	Le statut de réflexion.
		 *\param[in]	refraction	Le statut de réfraction.
		 *\param[in]	envMapIndex	L'indice de la texture environnementale.
		 *\param[in]	encoded		La variable qui recevra la valeur encodée.
		 */
		C3D_API void encodeMaterial( sdw::Int const & receiver
			, sdw::Int const & reflection
			, sdw::Int const & refraction
			, sdw::Int const & lighting
			, sdw::Int const & envMapIndex
			, sdw::Float const & encoded )const;
		/**
		 *\~english
		 *\brief		Calls the GLSL function used to dencode the material specifics from a vec4.
		 *\param[in]	encoded		The encoded value.
		 *\param[in]	receiver	The variable that contains the shadow receiver status.
		 *\param[in]	reflection	The variable that contains the reflection status.
		 *\param[in]	refraction	The variable that contains the refraction status.
		 *\param[in]	envMapIndex	The variable that contains the environment map index.
		 *\~french
		 *\brief		Appelle la fonction GLSL utilisée pour décoder les spécificités d'un matériau depuis un vec4.
		 *\param[in]	encoded		La valeur encodée.
		 *\param[in]	receiver	La variable qui recevra le statut de receveur d'ombres.
		 *\param[in]	reflection	La variable qui recevra le statut de réflexion.
		 *\param[in]	refraction	La variable qui recevra le statut de réfraction.
		 *\param[in]	envMapIndex	La variable qui recevra l'indice de la texture environnementale.
		 */
		C3D_API void decodeMaterial( sdw::Float const & encoded
			, sdw::Int const & receiver
			, sdw::Int const & reflection
			, sdw::Int const & refraction
			, sdw::Int const & lighting
			, sdw::Int const & envMapIndex )const;
		/**
		 *\~english
		 *\brief		Calls the GLSL function used to decode the shadow receiver status from a vec4.
		 *\param[in]	encoded		The encoded value.
		 *\param[in]	receiver	The variable that contains the shadow receiver status.
		 *\~french
		 *\brief		Appelle la fonction GLSL utilisée pour décoder le statut de receveur d'ombre depuis un vec4.
		 *\param[in]	encoded		La valeur encodée.
		 *\param[in]	receiver	La variable qui recevra le statut de receveur d'ombres.
		 */
		C3D_API void decodeReceiver( sdw::Int & encoded
			, sdw::Int const & receiver
			, sdw::Int const & lighting )const;
		/**
		 *\~english
		 *\brief		Writes the alpha function in GLSL.
		 *\param		alphaFunc	The alpha function.
		 *\param[in]	alpha		The alpha TypeEnum.
		 *\param[in]	alphaRef	The alpha comparison reference TypeEnum.
		 *\~french
		 *\brief		Ecrit la fonction d'opacité en GLSL.
		 *\param		alphaFunc	La fonction d'opacité.
		 *\param[in]	alpha		La valeur d'opacité.
		 *\param[in]	alphaRef	La valeur de référence pour la comparaison alpha.
		 */
		C3D_API void applyAlphaFunc( VkCompareOp alphaFunc
			, sdw::Float & alpha
			, sdw::Float const & alphaRef
			, bool opaque = true )const;

		C3D_API sdw::Boolean isSaturated( sdw::Vec3 const & p )const;
		C3D_API sdw::Boolean isSaturated( sdw::IVec3 const & p
			, sdw::Int const & imax )const;
		/**
		*\~english
		*\brief
		*	Encode HDR color to a 32 bit uint.
		*	Alpha is 1 bit + 7 bit HDR remapping.
		*/
		C3D_API sdw::UInt encodeColor( sdw::Vec4 const & color )const;
		/**
		*\~english
		*\brief
		*	Encode specified normal (normalized) into 32 bits uint.
		*	Each axis of the normal is encoded into 9 bits (1 for the sign/ 8 for the value).
		*/
		C3D_API sdw::UInt encodeNormal( sdw::Vec3 const & normal )const;
		C3D_API sdw::Vec4 decodeColor( sdw::UInt const & colorMask )const;
		C3D_API sdw::Vec3 decodeNormal( sdw::UInt const & normalMask )const;
		/**
		*\~english
		*\brief
		*	3D array index to flattened 1D array index
		*/
		C3D_API sdw::UInt flatten( sdw::UVec3 const & p
			, sdw::UVec3 const & dim )const;
		/**
		*\~english
		*\brief
		*	Flattened array index to 3D array index
		*/
		C3D_API sdw::UVec3 unflatten( sdw::UInt const & p
			, sdw::UVec3 const & dim )const;

	private:
		void doComputeGeometryMapContribution( TextureFlags const & textureFlags
			, PassFlags const & passFlags
			, std::string const & name
			, shader::TextureConfigData const & config
			, sdw::SampledImage2DRgba32 const & map
			, sdw::Vec3 & texCoords
			, sdw::Float & opacity
			, sdw::Vec3 & normal
			, sdw::Vec3 & tangent
			, sdw::Vec3 & bitangent
			, sdw::Vec3 & tangentSpaceViewPosition
			, sdw::Vec3 & tangentSpaceFragPosition );
		sdw::Vec2 doParallaxMapping( sdw::Vec2 const & texCoords
			, sdw::Vec3 const & viewDir
			, sdw::SampledImage2DRgba32 const & heightMap
			, TextureConfigData const & textureConfig );

	private:
		sdw::ShaderWriter & m_writer;
		Engine const & m_engine;
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
			, sdw::InFloat > m_lineariseDepth;
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
		sdw::Function< sdw::Void
			, sdw::InInt
			, sdw::InInt
			, sdw::InInt
			, sdw::InInt
			, sdw::InInt
			, sdw::OutFloat > m_encodeMaterial;
		sdw::Function< sdw::Void
			, sdw::InFloat
			, sdw::OutInt
			, sdw::OutInt
			, sdw::OutInt
			, sdw::OutInt
			, sdw::OutInt > m_decodeMaterial;
		sdw::Function< sdw::Void
			, sdw::InInt
			, sdw::OutInt
			, sdw::OutInt > m_decodeReceiver;
		sdw::Function< sdw::Vec2
			, sdw::InVec2
			, sdw::InVec3
			, sdw::InSampledImage2DRgba32
			, InTextureConfigData > m_parallaxMapping;
		sdw::Function< sdw::Float
			, sdw::InVec3
			, sdw::InVec2
			, sdw::InFloat
			, sdw::InSampledImage2DRgba32
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
	};
}

#endif
