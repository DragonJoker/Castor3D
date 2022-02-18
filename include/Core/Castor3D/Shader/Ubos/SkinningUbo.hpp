/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SkinningUbo_H___
#define ___C3D_SkinningUbo_H___

#include "UbosModule.hpp"
#include "Castor3D/Scene/Animation/AnimationModule.hpp"
#include "Castor3D/Shader/Shaders/GlslSurface.hpp"

#include "Castor3D/Buffer/UniformBufferOffset.hpp"

#include <CastorUtils/Math/SquareMatrix.hpp>

namespace castor3d
{
	struct SkinningBonesData
		: public sdw::StructInstance
	{
		C3D_API SkinningBonesData( sdw::ShaderWriter & writer
			, ast::expr::ExprPtr expr
			, bool enabled );
		SDW_DeclStructInstance( C3D_API, SkinningBonesData );

		C3D_API static ast::type::BaseStructPtr makeType( ast::type::TypesCache & cache );

	private:
		using sdw::StructInstance::getMember;
		using sdw::StructInstance::getMemberArray;

	public:
		C3D_API static castor::String const BufferName;

	public:
		sdw::IVec4 boneIds0;
		sdw::IVec4 boneIds1;
		sdw::Vec4 boneWeights0;
		sdw::Vec4 boneWeights1;
	};

	namespace shader
	{
		struct SkinningData
		{
			std::unique_ptr< sdw::ArraySsboT< sdw::Mat4 > > ssbo;
			std::unique_ptr< sdw::Ubo > ubo;
			std::unique_ptr< sdw::ArraySsboT< SkinningBonesData > > bones;
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
			, uint32_t bonesBinding
			, uint32_t set
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
			, sdw::Mat4 transform
			, sdw::ShaderWriter & writer
			, ProgramFlags const & flags
			, sdw::Mat4 const & curMtxModel
			, sdw::Int const & vertexIndex );
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
		template< ast::var::Flag FlagT >
		static sdw::Mat4 computeTransform( shader::SkinningData const & data
			, shader::VertexSurfaceT< FlagT > const & surface
			, sdw::ShaderWriter & writer
			, ProgramFlags const & flags
			, sdw::Mat4 const & curMtxModel
			, sdw::Int const & vertexIndex )
		{
			return computeTransform( data
				, surface.transform
				, writer
				, flags
				, curMtxModel
				, vertexIndex );
		}
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
