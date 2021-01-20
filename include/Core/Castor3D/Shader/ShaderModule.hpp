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
		//!\~english	Program using and instances multiplier.
		//!\~french		Programme utilisant un multiplicateur d'instances.
		eInstanceMult = 0x0100,
		//!\~english	Shader used to render a shadow map for directional light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière directionnalle.
		eShadowMapDirectional = 0x0200,
		//!\~english	Shader used to render a shadow map for spot light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière projecteur.
		eShadowMapSpot = 0x0400,
		//!\~english	Shader used to render a shadow map for point light.
		//\~french		Shader utilisé pour dessiner la shadow map d'une lumière omnidirectionnelle.
		eShadowMapPoint = 0x0800,
		//!\~english	Shader used to render an environment map.
		//\~french		Shader utilisé pour dessiner une texture d'environnement.
		eEnvironmentMapping = 0x1000,
		//!\~english	Shader for the depth pre-pass.
		//\~french		Shader pour la pré-passe de profondeur.
		eDepthPass = 0x2000,
		//!\~english	Vertex shader inverts normals (for front culling).
		//\~french		Le vertex shader inverse les normales (pour le front culling).
		eInvertNormals = 0x4000,
		//!\~english	Vertex shader inverts normals (for front culling).
		//\~french		Le vertex shader inverse les normales (pour le front culling).
		eHasGeometry = 0x8000,
		//!\~english	All flags.
		//\~french		Tous les indicateurs.
		eAll = 0xFFFF,
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
	*\return
	*	The materials passes buffer index.
	*\~french
	*\return
	*	L'index du buffer de passes de matériaux.
	*/
	C3D_API uint32_t getPassBufferIndex()noexcept;
	/**
	*\~english
	*\return
	*	The textures buffer index.
	*\~french
	*\return
	ù	L'index du buffer de textures.
	*/
	C3D_API uint32_t getTexturesBufferIndex()noexcept;
	/**
	*\~english
	*\return
	*	The lights sources buffer index.
	*\~french
	*\return
	*	L'index du buffer de sources lumineuses.
	*/
	C3D_API uint32_t getLightBufferIndex()noexcept;
	/**
	*\~english
	*\return
	*	The minimal index for shader buffers (SSBO and UBO).
	*\~french
	*\return
	*	L'index minimal des shader buffers (SSBO et UBO).
	*/
	C3D_API uint32_t getMinBufferIndex()noexcept;
	/**
	*\~english
	*\return
	*	The minimal index for textures.
	*\~french
	*\return
	*	L'index minimal pour les textures.
	*/
	C3D_API uint32_t getMinTextureIndex()noexcept;
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
	*	Wrapper class to select between SSBO or TBO.
	*\remarks
	*	Allows to user either one or the other in the same way.
	*\~french
	*\brief
	*	Classe permettant de choisir entre SSBO et TBO.
	*\remarks
	*	Permet d'utiliser l'un comme l'autre de la même manière.
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
