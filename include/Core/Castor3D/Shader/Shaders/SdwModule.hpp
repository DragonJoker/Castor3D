/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderSdwModule_H___
#define ___C3D_ShaderSdwModule_H___

#include "Castor3D/Shader/ShaderModule.hpp"

#include "Castor3D/Material/MaterialModule.hpp"

namespace castor3d
{
	/**@name Shader */
	//@{
	/**@name Sdw */
	//@{

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

	//@}
	//@}
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
