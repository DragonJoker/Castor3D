/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GlslUtils_H___
#define ___C3D_GlslUtils_H___

#include "Castor3D/Castor3DPrerequisites.hpp"

#include <ShaderWriter/CompositeTypes/Function.hpp>

namespace castor3d
{
	namespace shader
	{
		class TextureConfigurations;

		class Utils
		{
		public:
			C3D_API Utils( sdw::ShaderWriter & writer
				, bool isTopDown = true
				, bool isZeroToOneDepth = true
				, bool isInvertedNormals = false );
			C3D_API void declareCalcTexCoord();
			C3D_API void declareCalcVSPosition();
			C3D_API void declareCalcWSPosition();
			C3D_API void declareApplyGamma();
			C3D_API void declareRemoveGamma();
			C3D_API void declareLineariseDepth();
			C3D_API void declareGetMapNormal();
			C3D_API void declareComputeAccumulation();
			C3D_API void declareFresnelSchlick();
			C3D_API void declareComputeIBL();
			// 1.0 - y
			C3D_API void declareInvertVec2Y();
			C3D_API void declareInvertVec3Y();
			C3D_API void declareInvertVec4Y();
			C3D_API void declareInvertNormal();
			// -y
			C3D_API void declareNegateVec2Y();
			C3D_API void declareNegateVec3Y();
			C3D_API void declareNegateVec4Y();

			C3D_API void declareEncodeMaterial();
			C3D_API void declareDecodeMaterial();
			C3D_API void declareDecodeReceiver();
			C3D_API void declareParallaxMappingFunc( PipelineFlags const & flags );
			C3D_API void declareParallaxShadowFunc( PipelineFlags const & flags );
			C3D_API sdw::Vec2 bottomUpToTopDown( sdw::Vec2 const & texCoord )const;
			C3D_API sdw::Vec2 topDownToBottomUp( sdw::Vec2 const & texCoord )const;
			C3D_API sdw::Vec3 bottomUpToTopDown( sdw::Vec3 const & texCoord )const;
			C3D_API sdw::Vec3 topDownToBottomUp( sdw::Vec3 const & texCoord )const;
			C3D_API sdw::Vec4 bottomUpToTopDown( sdw::Vec4 const & texCoord )const;
			C3D_API sdw::Vec4 topDownToBottomUp( sdw::Vec4 const & texCoord )const;
			C3D_API sdw::Vec3 invertNormal( sdw::Vec3 const & texCoord )const;
			C3D_API sdw::Vec2 negateBottomUpToTopDown( sdw::Vec2 const & texCoord )const;
			C3D_API sdw::Vec2 negateTopDownToBottomUp( sdw::Vec2 const & texCoord )const;
			C3D_API sdw::Vec3 negateBottomUpToTopDown( sdw::Vec3 const & texCoord )const;
			C3D_API sdw::Vec3 negateTopDownToBottomUp( sdw::Vec3 const & texCoord )const;
			C3D_API sdw::Vec4 negateBottomUpToTopDown( sdw::Vec4 const & texCoord )const;
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
				, sdw::Float const & accumulationOperator );
			C3D_API sdw::Vec3 fresnelSchlick( sdw::Float const & product
				, sdw::Vec3 const & f0
				, sdw::Float const & roughness )const;
			C3D_API sdw::Vec3 computeMetallicIBL( sdw::Vec3 const & normal
				, sdw::Vec3 const & position
				, sdw::Vec3 const & albedo
				, sdw::Float const & metalness
				, sdw::Float const & roughness
				, sdw::Vec3 const & worldEye
				, sdw::SampledImageCubeRgba32 const & irradiance
				, sdw::SampledImageCubeRgba32 const & prefiltered
				, sdw::SampledImage2DRgba32 const & brdf )const;
			C3D_API sdw::Vec3 computeSpecularIBL( sdw::Vec3 const & normal
				, sdw::Vec3 const & position
				, sdw::Vec3 const & diffuse
				, sdw::Vec3 const & specular
				, sdw::Float const & glossiness
				, sdw::Vec3 const & worldEye
				, sdw::SampledImageCubeRgba32 const & irradiance
				, sdw::SampledImageCubeRgba32 const & prefiltered
				, sdw::SampledImage2DRgba32 const & brdf )const;
			C3D_API void computeOpacityMapContribution( PipelineFlags const & flags
				, TextureConfigurations const & textureConfigs
				, sdw::Array< sdw::UVec4 > const & textureConfig
				, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
				, sdw::Vec3 const & texCoords
				, sdw::Float & opacity );
			C3D_API void computeHeightMapContribution( PipelineFlags const & flags
				, TextureConfigurations const & textureConfigs
				, sdw::Array< sdw::UVec4 > const & textureConfig
				, sdw::Vec3 const & tangentSpaceViewPosition
				, sdw::Vec3 const & tangentSpaceFragPosition
				, sdw::Array< sdw::SampledImage2DRgba32 > const & maps
				, sdw::Vec3 & texCoords );
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
				, sdw::Int const & receiver )const;
			/**
			 *\~english
			 *\brief		Writes the alpha function in GLSL.
			 *\param[in]	writer		The GLSL writer.
			 *\param		alphaFunc	The alpha function.
			 *\param[in]	alpha		The alpha TypeEnum.
			 *\param[in]	alphaRef	The alpha comparison reference TypeEnum.
			 *\~french
			 *\brief		Ecrit la fonction d'opacité en GLSL.
			 *\param[in]	writer		Le writer GLSL.
			 *\param		alphaFunc	La fonction d'opacité.
			 *\param[in]	alpha		La valeur d'opacité.
			 *\param[in]	alphaRef	La valeur de référence pour la comparaison alpha.
			 */
			C3D_API void applyAlphaFunc( VkCompareOp alphaFunc
				, sdw::Float & alpha
				, sdw::Float const & alphaRef );

			inline bool hasInvertedNormals()const
			{
				return m_isInvertedNormals;
			}

			inline bool isZeroToOneDepth()const
			{
				return m_isZeroToOneDepth;
			}

			inline bool isTopDown()const
			{
				return m_isTopDown;
			}

		public:
			C3D_API static uint32_t const MaxIblReflectionLod;

		private:
			sdw::ShaderWriter & m_writer;
			bool m_isTopDown;
			bool m_isZeroToOneDepth;
			bool m_isInvertedNormals;
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
				, sdw::InVec3
				, sdw::InFloat > m_fresnelSchlick;
			sdw::Function< sdw::Vec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InVec3
				, sdw::InFloat
				, sdw::InFloat
				, sdw::InVec3
				, sdw::InSampledImageCubeRgba32
				, sdw::InSampledImageCubeRgba32
				, sdw::InSampledImage2DRgba32 > m_computeIBL;
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
				, sdw::OutFloat > m_encodeMaterial;
			sdw::Function< sdw::Void
				, sdw::InFloat
				, sdw::OutInt
				, sdw::OutInt
				, sdw::OutInt
				, sdw::OutInt > m_decodeMaterial;
			sdw::Function< sdw::Void
				, sdw::InInt
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
		};
	}
}

#endif
