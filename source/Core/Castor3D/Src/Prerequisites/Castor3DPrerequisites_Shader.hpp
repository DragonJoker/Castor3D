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
	using ShaderPtr = std::unique_ptr< sdw::Shader >;
	/**@name Shader */
	//@{
	struct ShaderModule
	{
		ShaderModule( ShaderModule const & ) = delete;
		ShaderModule & operator=( ShaderModule const & ) = delete;

		C3D_API ShaderModule( ShaderModule && rhs );
		C3D_API ShaderModule & operator=( ShaderModule && rhs );

		C3D_API ShaderModule( ashes::ShaderStageFlag stage
			, std::string const & name );
		C3D_API ShaderModule( ashes::ShaderStageFlag stage
			, std::string const & name
			, std::string source );
		C3D_API ShaderModule( ashes::ShaderStageFlag stage
			, std::string const & name
			, ShaderPtr shader );

		ashes::ShaderStageFlag stage;
		std::string name;
		std::string source;
		ShaderPtr shader;
	};
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
	C3D_API bool isShadowMapProgram( ProgramFlags const & p_flags );

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
			eLight = int( ast::type::Kind::eCount ),
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
		static constexpr uint32_t DirectionalMaxCascadesCount = 4u;
		static constexpr int BaseLightComponentsCount = 3;
		static constexpr int MaxLightComponentsCount = 21;

		class Utils;
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
			C3D_API void computePreLightingMapContributions( sdw::ShaderWriter & writer
				, sdw::Vec3 & p_normal
				, sdw::Float & p_shininess
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags
				, PassFlags const & passFlags );

			C3D_API void computePostLightingMapContributions( sdw::ShaderWriter & writer
				, shader::Utils const & utils
				, sdw::Vec3 & p_diffuse
				, sdw::Vec3 & p_specular
				, sdw::Vec3 & p_emissive
				, sdw::Float const & p_gamma
				, TextureChannels const & textureFlags
				, ProgramFlags const & programFlags
				, SceneFlags const & sceneFlags );

			C3D_API std::shared_ptr< PhongLightingModel > createLightingModel( sdw::ShaderWriter & writer
				, uint32_t & index
				, uint32_t maxCascades );

			C3D_API std::shared_ptr< PhongLightingModel > createLightingModel( sdw::ShaderWriter & writer
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index
				, uint32_t maxCascades );

			C3D_API std::shared_ptr< PhongLightingModel > createLightingModel( sdw::ShaderWriter & writer
				, LightType light
				, ShadowType shadows
				, bool volumetric
				, uint32_t & index );
		}

		namespace pbr
		{
			namespace mr
			{
				C3D_API void computePreLightingMapContributions( sdw::ShaderWriter & writer
					, sdw::Vec3 & p_normal
					, sdw::Float & p_metallic
					, sdw::Float & p_roughness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags );

				C3D_API void computePostLightingMapContributions( sdw::ShaderWriter & writer
					, shader::Utils const & utils
					, sdw::Vec3 & p_albedo
					, sdw::Vec3 & p_emissive
					, sdw::Float const & p_gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags );

				C3D_API std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, uint32_t & index
					, uint32_t maxCascades );

				C3D_API std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index
					, uint32_t maxCascades );

				C3D_API std::shared_ptr< MetallicBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, LightType light
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index );
			}

			namespace sg
			{
				C3D_API void computePreLightingMapContributions( sdw::ShaderWriter & writer
					, sdw::Vec3 & normal
					, sdw::Vec3 & specular
					, sdw::Float & glossiness
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags
					, PassFlags const & passFlags );

				C3D_API void computePostLightingMapContributions( sdw::ShaderWriter & writer
					, shader::Utils const & utils
					, sdw::Vec3 & diffuse
					, sdw::Vec3 & emissive
					, sdw::Float const & gamma
					, TextureChannels const & textureFlags
					, ProgramFlags const & programFlags
					, SceneFlags const & sceneFlags );

				C3D_API std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, uint32_t & index
					, uint32_t maxCascades );

				C3D_API std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
					, ShadowType shadows
					, bool volumetric
					, uint32_t & index
					, uint32_t maxCascades );

				C3D_API std::shared_ptr< SpecularBrdfLightingModel > createLightingModel( sdw::ShaderWriter & writer
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
		C3D_API std::unique_ptr< Materials > createMaterials( sdw::ShaderWriter & writer
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
		C3D_API void applyAlphaFunc( sdw::ShaderWriter & writer
			, ashes::CompareOp alphaFunc
			, sdw::Float const & alpha
			, sdw::Float const & alphaRef );

		using ParallaxFunction = sdw::Function< sdw::Vec2, sdw::InParam< sdw::Vec2 >, sdw::InParam< sdw::Vec3 > >;
		using ParallaxShadowFunction = sdw::Function< sdw::Float, sdw::InParam< sdw::Vec3 >, sdw::InParam< sdw::Vec2 >, sdw::InParam< sdw::Float > >;

		C3D_API ParallaxFunction declareParallaxMappingFunc( sdw::ShaderWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags );


		C3D_API ParallaxShadowFunction declareParallaxShadowFunc( sdw::ShaderWriter & writer
			, TextureChannels const & textureFlags
			, ProgramFlags const & programFlags );

		Writer_Parameter( Light );
		Writer_Parameter( DirectionalLight );
		Writer_Parameter( PointLight );
		Writer_Parameter( SpotLight );
	}
}

namespace sdw
{
	template<>
	struct TypeTraits< castor3d::shader::Light >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::eLight );
	};

	template<>
	struct TypeTraits< castor3d::shader::DirectionalLight >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::eDirectionalLight );
	};

	template<>
	struct TypeTraits< castor3d::shader::PointLight >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::ePointLight );
	};

	template<>
	struct TypeTraits< castor3d::shader::SpotLight >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::eSpotLight );
	};

	template<>
	struct TypeTraits< castor3d::shader::BaseMaterial >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::eMaterial );
	};

	template<>
	struct TypeTraits< castor3d::shader::LegacyMaterial >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::eLegacyMaterial );
	};

	template<>
	struct TypeTraits< castor3d::shader::MetallicRoughnessMaterial >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::eMetallicRoughnessMaterial );
	};

	template<>
	struct TypeTraits< castor3d::shader::SpecularGlossinessMaterial >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::eSpecularGlossinessMaterial );
	};
}

#endif
