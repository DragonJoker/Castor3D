/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkinningUbo_H___
#define ___C3D_SkinningUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	namespace shader
	{
		struct SkinningData
		{
			std::unique_ptr< sdw::ArraySsboT< sdw::Mat4 > > ssbo;
			std::unique_ptr< sdw::Ubo > ubo;
		};
	}

	class SkinningUbo
	{
	public:
		using Configuration = SkinningUboConfiguration;

	public:
		/**
		 *\~english
		 *\brief		Declares the variables needed to compute skinning in vertex shader.
		 *\param[in]	writer		The shader writer.
		 *\param[in]	uboBinding	The descriptor set binding point for UBO.
		 *\param[in]	sboBinding	The descriptor set binding point for SBO.
		 *\param[in]	set			The descriptor set index.
		 *\param[in]	flags		The program flags.
		 *\return		The skinning data.
		 *\~french
		 *\brief		Déclare les variables nécessaires au calcul du skinning dans le vertex shader.
		 *\param[in]	writer		Le shader writer.
		 *\param[in]	uboBinding	Le point d'attache du descriptor set for UBO.
		 *\param[in]	sboBinding	Le point d'attache du descriptor set for SBO.
		 *\param[in]	set			L'index du descriptor set.
		 *\param[in]	flags		Les indicateurs du programme.
		 *\return		Les données de skinning.
		 */
		C3D_API static shader::SkinningData declare( sdw::ShaderWriter & writer
			, uint32_t uboBinding
			, uint32_t sboBinding
			, uint32_t set
			, ProgramFlags const & flags );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding for the skinning UBO/SSBO.
		 *\param[in]	uboBinding	The descriptor set binding point for UBO.
		 *\param[in]	sboBinding	The descriptor set binding point for SBO.
		 *\param[in]	flags	The program flags.
		 *\~french
		 *\brief		Crée l'attache de layout de descripteur pour l'UBO/SSBO de skinning.
		 *\param[in]	uboBinding	Le point d'attache du descriptor set for UBO.
		 *\param[in]	sboBinding	Le point d'attache du descriptor set for SBO.
		 *\param[in]	flags	Les indicateurs du programme.
		 */
		C3D_API static VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t uboBinding
			, uint32_t sboBinding
			, ProgramFlags const & flags );
		/**
		 *\~english
		 *\brief		Computes skinning transformation in vertex shader.
		 *\param[in]	data	The skinning data.
		 *\param[in]	writer	The shader writer.
		 *\param[in]	flags	The program flags.
		 *\return		The resulting matrix.
		 *\~french
		 *\brief		Effectue le calcul de la transformation du skinning dans le vertex shader.
		 *\param[in]	data	Les données de skinning.
		 *\param[in]	writer	Le shader writer.
		 *\param[in]	flags	Les indicateurs du programme.
		 *\return		La matrice résultat.
		 */
		C3D_API static sdw::Mat4 computeTransform( shader::SkinningData const & data
			, shader::VertexSurface const & surface
			, sdw::ShaderWriter & writer
			, ProgramFlags const & flags
			, sdw::Mat4 const & curMtxModel );
		/**@}*/

	public:
		//!\~english	Name of the skinning animation frame variable buffer.
		//!\~french		Nom du frame variable buffer contenant les données d'animation de skinning.
		C3D_API static castor::String const BufferSkinning;
		//!\~english	Name of the bones matrices attribute.
		//!\~french		Nom de l'attribut de de matrices d'os.
		C3D_API static castor::String const Bones;
	};
}

#endif
