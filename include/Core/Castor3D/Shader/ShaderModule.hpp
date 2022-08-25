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
		: uint16_t
	{
		//!\~english	No flag.
		//!\~french		Aucun indicateur.
		eNone = 0x0000,
		//!\~english	Program using instanciation.
		//!\~french		Programme utilisant l'instanciation.
		eInstantiation = 0x0001,
		//!\~english	Program used by billboards.
		//!\~french		Programme utilisé par des billboards.
		eBillboards = 0x0002,
		//!\~english	Vertex shader inverts normals (for front culling).
		//\~french		Le vertex shader inverse les normales (pour le front culling).
		eInvertNormals = 0x0004,
		//!\~english	Render pass uses a mesh shader.
		//\~french		La passe de rendu utilise un mesh shader.
		eHasMesh = 0x0008,
		//!\~english	All flags used in base pipeline flags hashing.
		//\~french		Tous les indicateurs utilisés dans le hash des indicateurs de pipeline.
		eAllBase = 0x000F,
		//!\~english	Render pass uses a task shader.
		//\~french		La passe de rendu utilise un task shader.
		eHasTask = 0x0010,
		//!\~english	All other flags.
		//\~french		Tous les autres indicateurs.
		eAllOptional = 0x0010,
		//!\~english	All flags.
		//\~french		Tous les indicateurs.
		eAll = eAllBase | eAllOptional,
		//!\~english	All flags related to visibility buffer resolve.
		//\~french		Tous les indicateurs liés à la resolution du visibility buffer.
		eAllVisibility = eInvertNormals | eBillboards,
	};
	CU_ImplementFlags( ProgramFlag )
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
