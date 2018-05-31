/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_SHADER_H___
#define ___C3D_PREREQUISITES_SHADER_H___

namespace castor3d
{
	static uint32_t constexpr PassBufferIndex = 0u;
	static uint32_t constexpr LightBufferIndex = 1u;
	static uint32_t constexpr MinBufferIndex = 2u;

	/**@name Shader */
	//@{

	/*!
	\author 	Sylvain DOREMUS
	\date 		20/11/13
	\version	0.7.0.0
	\~english
	\brief		Flags to use when looking for an automatically generated program
	\~french
	\brief		Indicateurs à utiliser lors de la recherche de programme généré automatiquement
	*/
	enum class ProgramFlag
		: uint16_t
	{
		//!\~english	Program using instanciation.
		//!\~french		Programme utilisant l'instanciation.
		eInstantiation = 0x0001,
		//!\~english	Program using skeleton animations.
		//!\~french		Programme utilisant les animations par squelette.
		eSkinning = 0x0002,
		//!\~english	Program used by billboards.
		//!\~french		Programme utilisé par des billboards.
		eBillboards = 0x0004,
		//!\~english	Program using per-vertex animations.
		//!\~french		Programme utilisant les animations par sommet.
		eMorphing = 0x0008,
		//!\~english	Picking pass program.
		//\~french		Programme de passe de picking.
		ePicking = 0x0010,
		//!\~english	Shader supporting lighting.
		//\~french		Shader supportant les éclairages.
		eLighting = 0x0020,
		//!\~english	Shader for spherical billboards.
		//\~french		Shader pour les billboards sphériques.
		eSpherical = 0x0040,
		//!\~english	Shader for fixed size billboards.
		//\~french		Shader pour les billboards à dimensions fixes.
		eFixedSize = 0x0080,
		//!\~english	Shader used to render a shadow map for directional light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière directionnalle.
		eShadowMapDirectional = 0x0100,
		//!\~english	Shader used to render a shadow map for spot light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière projecteur.
		eShadowMapSpot = 0x0200,
		//!\~english	Shader used to render a shadow map for point light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière omnidirectionnelle.
		eShadowMapPoint = 0x0400,
		//!\~english	Shader used to render an environment map.
		//\~french		Shader utilisé pour dessiner une texture d'environnement.
		eEnvironmentMapping = 0x0800,
		//!\~english	Shader for the depth pre-pass.
		//\~french		Shader pour la pré-passe de profondeur.
		eDepthPass = 0x1000,
	};
	IMPLEMENT_FLAGS( ProgramFlag )
	/**
	 *\~english
	 *\brief		Tells if the given flags contain a shadow map flag.
	 *\param[in]	p_flags	The flags to check.
	 *\return		\p true if p_flags contain one of ProgramFlag::eShadowMapDirectional, ProgramFlag::eShadowMapSpot, or ProgramFlag::eShadowMapPoint.
	 *\~french
	 *\brief		Dit si les indicateurs donnés contiennent un indicateur de shadow map.
	 *\param[in]	p_flags	Les indicateurs à vérifier.
	 *\return		\p true si p_flags contient l'un de ProgramFlag::eShadowMapDirectional, ProgramFlag::eShadowMapSpot, ou ProgramFlag::eShadowMapPoint.
	 */
	bool isShadowMapProgram( ProgramFlags const & p_flags );

	class BillboardUbo;
	class MatrixUbo;
	class ModelUbo;
	class ModelMatrixUbo;
	class MorphingUbo;
	class SceneUbo;
	class SkinningUbo;
	class PassBuffer;
	class ShaderBuffer;
	class ShaderProgram;

	DECLARE_SMART_PTR( ShaderBuffer );
	DECLARE_SMART_PTR( ShaderProgram );

	//@}

	namespace shader
	{
		enum class TypeName
		{
			eLight = int( glsl::TypeName::eCount ),
			eDirectionalLight,
			ePointLight,
			eSpotLight,
			eMaterial,
			eLegacyMaterial,
			eMetallicRoughnessMaterial,
			eSpecularGlossinessMaterial,
		};

		static constexpr uint32_t SpotShadowMapCount = 10u;
		static constexpr uint32_t PointShadowMapCount = 6u;
		static constexpr int BaseLightComponentsCount = 2;
		static constexpr int MaxLightComponentsCount = 14;
		static constexpr float LightComponentsOffset = MaxLightComponentsCount * 0.001f;


		class Shadow;
		struct Light;
		struct DirectionalLight;
		struct PointLight;
		struct SpotLight;
		class Materials;
		class LightingModel;
		class PhongLightingModel;
		class MetallicBrdfLightingModel;
		class SpecularBrdfLightingModel;
		struct BaseMaterial;
		struct LegacyMaterial;
		struct MetallicRoughnessMaterial;
		struct SpecularGlossinessMaterial;

		namespace legacy
		{
			C3D_API void computePreLightingMapContributions( glsl::GlslWriter & writer
				, glsl::Vec3 & p_normal
				, glsl::Float & p_shininess
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags
				, PassFlags const & passFlags );

			C3D_API void computePostLightingMapContributions( glsl::GlslWriter & writer
				, glsl::Vec3 & p_diffuse
				, glsl::Vec3 & p_specular
				, glsl::Vec3 & p_emissive
				, glsl::Float const & p_gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags );

			C3D_API std::shared_ptr< PhongLightingModel > createLightingModel( glsl::GlslWriter & writer
				, uint32_t & index );

			C3D_API std::shared_ptr< PhongLightingModel > createLightingModel( glsl::GlslWriter & writer
				, LightType light
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index );
		}

		namespace pbr
		{
			namespace mr
			{
				C3D_API void computePreLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & p_normal
					, glsl::Float & p_metallic
					, glsl::Float & p_roughness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags );

				C3D_API void computePostLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & p_albedo
					, glsl::Vec3 & p_emissive
					, glsl::Float const & p_gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags );

				C3D_API std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, uint32_t & index );

				C3D_API std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, LightType light
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index );
			}

			namespace sg
			{
				C3D_API void computePreLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & normal
					, glsl::Vec3 & specular
					, glsl::Float & glossiness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags );

				C3D_API void computePostLightingMapContributions( glsl::GlslWriter & writer
					, glsl::Vec3 & diffuse
					, glsl::Vec3 & emissive
					, glsl::Float const & gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags );

				C3D_API std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, uint32_t & index );

				C3D_API std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( glsl::GlslWriter & writer
					, LightType light
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index );
			}
		}
		/**
		 *\~english
		 *\brief		Creates the appropriate GLSL materials buffer.
		 *\param[in]	writer		The GLSL writer.
		 *\param		passFlags	The pass flags.
		 *\~french
		 *\brief		Crée le tampon de matériaux GLSL approprié.
		 *\param[in]	writer		Le writer GLSL.
		 *\param		passFlags	Les indicateurs de passe.
		 */
		C3D_API std::unique_ptr< Materials > createMaterials( glsl::GlslWriter & writer
			, PassFlags const & passFlags );
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
		C3D_API void applyAlphaFunc( glsl::GlslWriter & writer
			, renderer::CompareOp alphaFunc
			, glsl::Float const & alpha
			, glsl::Float const & alphaRef );

		using ParallaxFunction = glsl::Function< glsl::Vec2, glsl::InParam< glsl::Vec2 >, glsl::InParam< glsl::Vec3 > >;
		using ParallaxShadowFunction = glsl::Function< glsl::Float, glsl::InParam< glsl::Vec3 >, glsl::InParam< glsl::Vec2 >, glsl::InParam< glsl::Float > >;

		C3D_API ParallaxFunction declareParallaxMappingFunc( glsl::GlslWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags );

		C3D_API ParallaxShadowFunction declareParallaxShadowFunc( glsl::GlslWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags );

		DECLARE_GLSL_PARAMETER( Light );
	}
}

namespace glsl
{
	template<>
	struct TypeTraits< castor3d::shader::Light >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eLight );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::DirectionalLight >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eDirectionalLight );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::PointLight >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::ePointLight );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::SpotLight >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eSpotLight );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::BaseMaterial >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eMaterial );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::LegacyMaterial >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eLegacyMaterial );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::MetallicRoughnessMaterial >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eMetallicRoughnessMaterial );
		C3D_API static castor::String const Name;
	};

	template<>
	struct TypeTraits< castor3d::shader::SpecularGlossinessMaterial >
	{
		static TypeName const TypeEnum = TypeName( castor3d::shader::TypeName::eSpecularGlossinessMaterial );
		C3D_API static castor::String const Name;
	};
}

#endif
