/*
See LICENSE file in root folder
*/
#ifndef ___C3D_ShaderModule_H___
#define ___C3D_ShaderModule_H___

#include "Castor3D/Castor3DModule.hpp"

#include <ShaderAST/Shader.hpp>
#include <ShaderWriter/ShaderWriterPrerequisites.hpp>

#include <CastorUtils/Design/FlagCombination.hpp>

namespace castor3d
{
	/**@name Shader */
	//@{

	using ShaderPtr = std::unique_ptr< ast::Shader >;

	/**
	*\~english
	*\brief
	*	A non compiled shader module, with its source.
	*\~french
	*\brief
	*	Un module shader non compilé, avec sa source.
	*/
	struct ShaderModule
	{
		ShaderModule( ShaderModule const & ) = delete;
		ShaderModule & operator=( ShaderModule const & ) = delete;

		C3D_API ShaderModule( ShaderModule && rhs )noexcept;
		C3D_API ShaderModule & operator=( ShaderModule && rhs )noexcept;

		C3D_API ShaderModule() = default;

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
	/**
	*\~english
	*\brief
	*	A SPIR-V shader module, with SPIR-V binary and debug text source.
	*\~french
	*\brief
	*	Un module shader SPIR-V, avec le binaire SPIR-V et la source en texte.
	*/
	struct SpirVShader
	{
		castor::UInt32Array spirv;
		std::string text;
	};
	/**
	*\~english
	*\brief
	*	Flags to use when looking for an automatically generated program
	*\~french
	*\brief
	*	Indicateurs à utiliser lors de la recherche de programme généré automatiquement
	*/
	enum class ProgramFlag
		: uint32_t
	{
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x00000,
		//!\~english	Program using instanciation.
		//!\~french		Programme utilisant l'instanciation.
		eInstantiation = 0x00001,
		//!\~english	Program using skeleton animations.
		//!\~french		Programme utilisant les animations par squelette.
		eSkinning = 0x00002,
		//!\~english	Program using per-vertex animations.
		//!\~french		Programme utilisant les animations par sommet.
		eMorphing = 0x00004,
		//!\~english	Program used by billboards.
		//!\~french		Programme utilisé par des billboards.
		eBillboards = 0x00008,
		//!\~english	Picking pass program.
		//\~french		Programme de passe de picking.
		ePicking = 0x00010,
		//!\~english	Shader for spherical billboards.
		//\~french		Shader pour les billboards sphériques.
		eSpherical = 0x00020,
		//!\~english	Shader for fixed size billboards.
		//\~french		Shader pour les billboards à dimensions fixes.
		eFixedSize = 0x00040,
		//!\~english	Vertex shader inverts normals (for front culling).
		//\~french		Le vertex shader inverse les normales (pour le front culling).
		eInvertNormals = 0x00080,
		//!\~english	Forces texcoords binding.
		//\~french		Force le binding des UV.
		eForceTexCoords = 0x00100,
		//!\~english	Shader supporting lighting.
		//\~french		Shader supportant les éclairages.
		eLighting = 0x00200,
		//!\~english	Writes to Variance shadow map.
		//\~french		Ecrit dans la Variance shadow map.
		eVsmShadowMap = 0x00400,
		//!\~english	Writes to Reflective shadow map.
		//\~french		Ecrit dans la Reflective shadow map.
		eRsmShadowMap = 0x00800,
		//!\~english	All flags used in base pipeline flags hashing.
		//\~french		Tous les indicateurs utilisés dans le hash des indicateurs de pipeline.
		eAllBase = 0x00FFF,
		//!\~english	Shader used to render a shadow map for directional light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière directionnalle.
		eShadowMapDirectional = 0x01000,
		//!\~english	Shader used to render a shadow map for spot light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière projecteur.
		eShadowMapSpot = 0x02000,
		//!\~english	Shader used to render a shadow map for point light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière omnidirectionnelle.
		eShadowMapPoint = 0x04000,
		//!\~english	Shader used to render an environment map.
		//\~french		Shader utilisé pour dessiner une texture d'environnement.
		eEnvironmentMapping = 0x08000,
		//!\~english	Shader for the depth pre-pass.
		//\~french		Shader pour la pré-passe de profondeur.
		eDepthPass = 0x10000,
		//!\~english	Vertex shader inverts normals (for front culling).
		//\~french		Le vertex shader inverse les normales (pour le front culling).
		eHasGeometry = 0x20000,
		//!\~english	Vertex shader inverts normals (for front culling).
		//\~french		Le vertex shader inverse les normales (pour le front culling).
		eHasTessellation = 0x40000,
		//!\~english	All other flags.
		//\~french		Tous les autres indicateurs.
		eAllOptional = 0x7F000,
		//!\~english	All flags.
		//\~french		Tous les indicateurs.
		eAll = eAllBase | eAllOptional,
	};
	CU_ImplementFlags( ProgramFlag )
	/**
	*\~english
	*\brief
	*	Tells if the given flags contain a shadow map flag.
	*\param[in] flags
	*	The flags to check.
	*\return
	*	\p true if flags contain one of ProgramFlag::eShadowMapDirectional, ProgramFlag::eShadowMapSpot, or ProgramFlag::eShadowMapPoint.
	*\~french
	*\brief
	*	Dit si les indicateurs donnés contiennent un indicateur de shadow map.
	*\param[in] flags
	*	Les indicateurs à vérifier.
	*\return
	*	\p true si flags contient l'un de ProgramFlag::eShadowMapDirectional, ProgramFlag::eShadowMapSpot, ou ProgramFlag::eShadowMapPoint.
	*/
	C3D_API bool isShadowMapProgram( ProgramFlags const & flags );
	/**
	*\~english
	*\brief
	*	Base shader program implementation, used to share functions through the supported shader languages.
	*\~french
	*\brief
	*	Implémentation de base d'un programme de shader, utilisé afin d'exposer les fonctions communes aux langages de shader.
	*/
	class ShaderProgram;
	/**
	*\~english
	*\brief
	*	Helper class for SSBO.
	*\~french
	*\brief
	*	Classe d'aide pour un SSBO.
	*/
	class ShaderBuffer;
	/**
	*\~english
	*\brief
	*	Wrapper class to select between SSBO or TBO.
	*\remarks
	*	Allows to user either one or the other in the same way.
	*\~french
	*\brief
	*	Classe permettant de choisir entre SSBO et TBO.
	*\remarks
	*	Permet d'utiliser l'un comme l'autre de la même manière.
	*/
	template< typename ElementTypeTraits >
	class StructuredShaderBuffer;

	CU_DeclareCUSmartPtr( castor3d, ShaderBuffer, C3D_API );
	CU_DeclareCUSmartPtr( castor3d, ShaderProgram, C3D_API );

	//@}
}

#endif
