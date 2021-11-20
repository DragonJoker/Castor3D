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
			, sdw::Mat4 const & curMtxModel )
		{
			using namespace sdw;
			auto mtxBoneTransform = writer.declLocale< Mat4 >( cuT( "mtxBoneTransform" ) );

			if ( checkFlag( flags, ProgramFlag::eInstantiation ) )
			{
				auto gl_InstanceID = writer.getVariable< Int >( cuT( "gl_InstanceIndex" ) );
				auto mtxInstanceOffset = writer.declLocale( cuT( "mtxInstanceOffset" )
					, gl_InstanceID * 400_i );

				auto & ssbo = *data.ssbo;
				mtxBoneTransform = ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds0[0_i] )] * surface.boneWeights0[0_i];
				mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds0[1_i] )] * surface.boneWeights0[1_i];
				mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds0[2_i] )] * surface.boneWeights0[2_i];
				mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds0[3_i] )] * surface.boneWeights0[3_i];
				mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds1[0_i] )] * surface.boneWeights1[0_i];
				mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds1[1_i] )] * surface.boneWeights1[1_i];
				mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds1[2_i] )] * surface.boneWeights1[2_i];
				mtxBoneTransform += ssbo[writer.cast< UInt >( mtxInstanceOffset + surface.boneIds1[3_i] )] * surface.boneWeights1[3_i];
				mtxBoneTransform = surface.transform * mtxBoneTransform;
			}
			else
			{
				auto bones = data.ubo->getMemberArray< Mat4 >( SkinningUbo::Bones );
				mtxBoneTransform = bones[surface.boneIds0[0]] * surface.boneWeights0[0];
				mtxBoneTransform += bones[surface.boneIds0[1]] * surface.boneWeights0[1];
				mtxBoneTransform += bones[surface.boneIds0[2]] * surface.boneWeights0[2];
				mtxBoneTransform += bones[surface.boneIds0[3]] * surface.boneWeights0[3];
				mtxBoneTransform += bones[surface.boneIds1[0]] * surface.boneWeights1[0];
				mtxBoneTransform += bones[surface.boneIds1[1]] * surface.boneWeights1[1];
				mtxBoneTransform += bones[surface.boneIds1[2]] * surface.boneWeights1[2];
				mtxBoneTransform += bones[surface.boneIds1[3]] * surface.boneWeights1[3];
				mtxBoneTransform = curMtxModel * mtxBoneTransform;
			}

			return mtxBoneTransform;
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
