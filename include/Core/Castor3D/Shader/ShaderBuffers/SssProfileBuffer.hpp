/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SssProfileBuffer_H___
#define ___C3D_SssProfileBuffer_H___

#include "ShaderBuffersModule.hpp"
#include "Castor3D/Material/Pass/PassModule.hpp"
#include "Castor3D/Material/Pass/Component/ComponentModule.hpp"

#include "Castor3D/Limits.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class SssProfileBuffer
	{
	public:
		struct Data
		{
			float x;
			float y;
			float z;
			float w;
		};
		struct SssProfileData
		{
			uint32_t transmittanceProfileSize;
			float gaussianWidth;
			float subsurfaceScatteringStrength;
			float pad;
			std::array< Data, TransmittanceProfileSize > transmittanceProfile;
		};
		using SssProfilesData = castor::ArrayView< SssProfileData >;

		struct SssProfileDataPtr
		{
			uint32_t * transmittanceProfileSize;
			float * gaussianWidth;
			float * subsurfaceScatteringStrength;
			std::array< Data, TransmittanceProfileSize > * transmittanceProfile;
		};

		static constexpr uint32_t DataSize = sizeof( SssProfileData );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	device	The GPU device.
		 *\param[in]	count	The max configurations count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	count	Le nombre maximal de configurations.
		 */
		C3D_API SssProfileBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a pass to the buffer.
		 *\param[in]	component	The subsurface scattering component.
		 *\~french
		 *\brief		Ajoute une passe au tampon.
		 *\param[in]	component	Le composant de subsurface scattering.
		 */
		C3D_API uint32_t addPass( SubsurfaceScatteringComponent & component );
		/**
		 *\~english
		 *\brief		Removes a pass from the buffer.
		 *\param[in]	component	The subsurface scattering component.
		 *\~french
		 *\brief		Supprime une pass du tampon.
		 *\param[in]	component	Le composant de subsurface scattering.
		 */
		C3D_API void removePass( SubsurfaceScatteringComponent & component );
		/**
		 *\~english
		 *\brief		Updates the passes buffer.
		 *\~french
		 *\brief		Met à jour le tampon de passes.
		 */
		C3D_API void update( UploadData & uploader );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t binding
			, VkShaderStageFlags stages = ( VK_SHADER_STAGE_FRAGMENT_BIT
				| VK_SHADER_STAGE_GEOMETRY_BIT
				| VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT
				| VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT
				| VK_SHADER_STAGE_VERTEX_BIT ) )const;
		/**
		 *\~english
		 *\brief		Creates a frame pass binding.
		 *\~french
		 *\brief		Crée une attache de frame pass.
		 */
		C3D_API void createPassBinding( crg::FramePass & pass, uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor write for this buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour ce tampon.
		 */
		C3D_API ashes::WriteDescriptorSet getBinding( uint32_t binding )const;
		/**
		 *\~english
		 *\brief		Creates the descriptor set binding at given point.
		 *\param[in]	descriptorSet	The descriptor set that receives the binding.
		 *\param[in]	binding			The descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de set de descripteurs au point donné.
		 *\param[in]	descriptorSet	Le set de descripteurs recevant l'attache.
		 *\param[in]	binding			L'attache de layout de set de descripteurs.
		 */
		C3D_API void createBinding( ashes::DescriptorSet & descriptorSet
			, VkDescriptorSetLayoutBinding const & binding )const;
		/**
		 *\~english
		 *\return		The pointer to the data for given profile ID.
		 *\~french
		 *\brief		Le pointeur sur les données pour l'ID de profil donné.
		 */
		C3D_API SssProfileDataPtr getData( uint32_t profileID );
		/**
		 *\~english
		 *\return		The pointer to the buffer.
		 *\~french
		 *\brief		Le pointeur sur le tampon.
		 */
		uint8_t * getPtr()
		{
			return m_buffer.getPtr();
		}

	private:
		ShaderBuffer m_buffer;
		std::vector< SubsurfaceScatteringComponent * > m_components;
		std::vector< SubsurfaceScatteringComponent const * > m_dirty;
		std::vector< OnSssProfileChangedConnection > m_connections;
		uint32_t m_profileID{ 1u };
		SssProfilesData m_data;
		std::mutex m_mutex;
	};
}

#endif
