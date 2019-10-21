/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PREREQUISITES_SHADER_H___
#define ___C3D_PREREQUISITES_SHADER_H___

namespace castor3d
{
	C3D_API constexpr uint32_t getPassBufferIndex()noexcept;
	C3D_API constexpr uint32_t getTexturesBufferIndex()noexcept;
	C3D_API constexpr uint32_t getLightBufferIndex()noexcept;
	C3D_API constexpr uint32_t getMinBufferIndex()noexcept;
	C3D_API constexpr uint32_t getMinTextureIndex()noexcept;

	using ShaderPtr = std::unique_ptr< sdw::Shader >;
	/**@name Shader */
	//@{
	struct ShaderModule
	{
		ShaderModule( ShaderModule const & ) = delete;
		ShaderModule & operator=( ShaderModule const & ) = delete;

		C3D_API ShaderModule( ShaderModule && rhs );
		C3D_API ShaderModule & operator=( ShaderModule && rhs );

		C3D_API ShaderModule( VkShaderStageFlagBits stage
			, std::string const & name );
		C3D_API ShaderModule( VkShaderStageFlagBits stage
			, std::string const & name
			, std::string source );
		C3D_API ShaderModule( VkShaderStageFlagBits stage
			, std::string const & name
			, ShaderPtr shader );

		VkShaderStageFlagBits stage;
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
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x0000,
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
		//!\~english	Vertex shader inverts normals (for front culling).
		//\~french		Le vertex shader inverse les normales (pour le front culling).
		eInvertNormals = 0x2000,
		//!\~english	All flags.
		//\~french		Tous les indicateurs.
		eAll = 0x3FFF,
	};
	CU_ImplementFlags( ProgramFlag )
	/**
	 *\~english
	 *\brief		Tells if the given flags contain a shadow map flag.
	 *\param[in]	flags	The flags to check.
	 *\return		\p true if flags contain one of ProgramFlag::eShadowMapDirectional, ProgramFlag::eShadowMapSpot, or ProgramFlag::eShadowMapPoint.
	 *\~french
	 *\brief		Dit si les indicateurs donnés contiennent un indicateur de shadow map.
	 *\param[in]	flags	Les indicateurs à vérifier.
	 *\return		\p true si flags contient l'un de ProgramFlag::eShadowMapDirectional, ProgramFlag::eShadowMapSpot, ou ProgramFlag::eShadowMapPoint.
	 */
	C3D_API bool isShadowMapProgram( ProgramFlags const & flags );

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
	class TextureConfigurationBuffer;
	class UniformBufferBase;

	template< typename T >
	class UniformBuffer;

	CU_DeclareSmartPtr( ShaderBuffer );
	CU_DeclareSmartPtr( ShaderProgram );
	CU_DeclareSmartPtr( UniformBufferBase );
	CU_DeclareTemplateSmartPtr( UniformBuffer );

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
			eTextureConfigData,
		};

		constexpr uint32_t DirectionalMaxCascadesCount = 4u;

		C3D_API uint32_t getSpotShadowMapCount();
		C3D_API uint32_t getPointShadowMapCount();
		C3D_API uint32_t getBaseLightComponentsCount();
		C3D_API uint32_t getMaxLightComponentsCount();

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
		struct TextureConfigData;

		Writer_Parameter( Light );
		Writer_Parameter( DirectionalLight );
		Writer_Parameter( PointLight );
		Writer_Parameter( SpotLight );
		Writer_Parameter( TextureConfigData );
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

	template<>
	struct TypeTraits< castor3d::shader::TextureConfigData >
	{
		static ast::type::Kind constexpr TypeEnum = ast::type::Kind( castor3d::shader::TypeName::eTextureConfigData );
	};
}

#endif
