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
			std::unique_ptr< sdw::ArraySsboT< sdw::Mat4 > > transforms;
		};
	}

	class SkinningUbo
	{
	public:
		using Configuration = SkinningTransformsConfiguration;

	public:
		/**
		 *\~english
		 *\brief		Declares the variables needed to compute skinning in vertex shader.
		 *\param[in]	writer				The shader writer.
		 *\param[in]	transformsBinding	The descriptor set binding point for the transforms buffer.
		 *\param[in]	transformsSet		The descriptor set index for the transforms buffer.
		 *\param[in]	flags				The program flags.
		 *\return		The skinning data.
		 *\~french
		 *\brief		Déclare les variables nécessaires au calcul du skinning dans le vertex shader.
		 *\param[in]	writer				Le shader writer.
		 *\param[in]	transformsBinding	Le point d'attache du descriptor set pour le buffer de matrices de transformation.
		 *\param[in]	transformsSet		L'index du descriptor set pour le buffer de matrices de transformation.
		 *\param[in]	flags				Les indicateurs du programme.
		 *\return		Les données de skinning.
		 */
		C3D_API static shader::SkinningData declare( sdw::ShaderWriter & writer
			, uint32_t transformsBinding
			, uint32_t transformsSet
			, ProgramFlags const & flags );
		/**
		 *\~english
		 *\brief		Computes skinning transformation in vertex shader.
		 *\param[in]	data			The skinning data.
		 *\param[in]	transform		The pre-skinning transform.
		 *\param[in]	writer			The shader writer.
		 *\param[in]	skinningId		The node's skinning ID.
		 *\param[in]	boneIds0		The bones IDs (0..3).
		 *\param[in]	boneIds1		The bones IDs (4..7).
		 *\param[in]	boneWeights0	The bones weights (0..3).
		 *\param[in]	boneWeights1	The bones weights (4..7).
		 *\return		The resulting matrix.
		 *\~french
		 *\brief		Effectue le calcul de la transformation du skinning dans le vertex shader.
		 *\param[in]	data			Les données de skinning.
		 *\param[in]	transform		La transformation pre-skinning.
		 *\param[in]	writer			Le shader writer.
		 *\param[in]	skinningId		L'ID de skinning du noeud.
		 *\param[in]	boneIds0		Les ID des bones (0..3).
		 *\param[in]	boneIds1		Les ID des bones (4..7).
		 *\param[in]	boneWeights0	Les poids des bones (0..3).
		 *\param[in]	boneWeights1	Les poids des bones (4..7).
		 *\return		La matrice résultat.
		 */
		C3D_API static sdw::Mat4 computeTransform( shader::SkinningData const & data
			, sdw::Mat4 transform
			, sdw::ShaderWriter & writer
			, sdw::UInt const & skinningId
			, sdw::UVec4 const & boneIds0
			, sdw::UVec4 const & boneIds1
			, sdw::Vec4 const & boneWeights0
			, sdw::Vec4 const & boneWeights1 );
		/**
		 *\~english
		 *\brief		Computes skinning transformation in vertex shader.
		 *\param[in]	data			The skinning data.
		 *\param[in]	surface			The processed surface
		 *\param[in]	writer			The shader writer.
		 *\param[in]	instanceIndex	The instance index.
		 *\param[in]	boneIds0		The bones IDs (0..3).
		 *\param[in]	boneIds1		The bones IDs (4..7).
		 *\param[in]	boneWeights0	The bones weights (0..3).
		 *\param[in]	boneWeights1	The bones weights (4..7).
		 *\return		The resulting matrix.
		 *\~french
		 *\brief		Effectue le calcul de la transformation du skinning dans le vertex shader.
		 *\param[in]	data			Les données de skinning.
		 *\param[in]	surface			La surface traitée.
		 *\param[in]	writer			Le shader writer.
		 *\param[in]	instanceIndex	L'index de l'instance.
		 *\param[in]	boneIds0		Les ID des bones (0..3).
		 *\param[in]	boneIds1		Les ID des bones (4..7).
		 *\param[in]	boneWeights0	Les poids des bones (0..3).
		 *\param[in]	boneWeights1	Les poids des bones (4..7).
		 *\return		La matrice résultat.
		 */
		template< ast::var::Flag FlagT >
		static sdw::Mat4 computeTransform( shader::SkinningData const & data
			, shader::VertexSurfaceT< FlagT > const & surface
			, sdw::ShaderWriter & writer
			, sdw::Int const & instanceIndex
			, sdw::UVec4 const & boneIds0
			, sdw::UVec4 const & boneIds1
			, sdw::Vec4 const & boneWeights0
			, sdw::Vec4 const & boneWeights1 )
		{
			return computeTransform( data
				, surface.transform
				, writer
				, instanceIndex
				, boneIds0
				, boneIds1
				, boneWeights0
				, boneWeights1 );
		}

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
