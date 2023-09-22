/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightBuffer_H___
#define ___C3D_LightBuffer_H___

#include "Castor3D/Scene/Light/LightModule.hpp"

#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/SquareMatrix.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <mutex>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class LightBuffer
	{
	public:
		using LightsData = castor::ArrayView< castor::Point4f >;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	device	The GPU device.
		 *\param[in]	count	The max passes count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	count	Le nombre maximal de passes.
		 */
		C3D_API LightBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a light source to the buffer.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Ajoute une source lumineuse au tampon.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void addLight( Light & light );
		/**
		 *\~english
		 *\brief		Removes a light source from the buffer.
		 *\param[in]	light	The light source.
		 *\~french
		 *\brief		Supprime une source lumineuse du tampon.
		 *\param[in]	light	La source lumineuse.
		 */
		C3D_API void removeLight( Light & light );
		/**
		 *\~english
		 *\brief			Updates the buffer CPU wise.
		 *\param[in,out]	updater	The update data.
		 *\~french
		 *\brief			Met à jour le tampon au niveau CPU.
		 *\param[in,out]	updater	Les données de mise à jour.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Uploads the buffer to VRAM.
		 *\param[in]	commandBuffer	Receives the upload commands.
		 *\~french
		 *\brief		Uploade le tampon en VRAM.
		 *\param[in]	commandBuffer	Reçoit les commandes d'upload.
		 */
		C3D_API void upload( UploadData & uploader );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( VkShaderStageFlags stages
			, uint32_t binding )const;
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
		 *\brief		Creates the descriptor write for this buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour ce tampon.
		 */
		C3D_API ashes::WriteDescriptorSet getSingleBinding( uint32_t binding
			, VkDeviceSize offset
			, VkDeviceSize size )const;
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
		 *\param[in]	type	The light type.
		 *\return		The number of light sources of the given type, in the buffer.
		 *\~french
		 *\brief		Crée le descriptor write pour le buffer de sources lumineuses.
		 *\param[in]	type	Le type de lumière.
		 *\return		Le nombre de sources lumineuses du type donné, dans le buffer.
		 */
		C3D_API uint32_t getLightsBufferCount( LightType type )const noexcept;

		uint8_t * getPtr()
		{
			return m_buffer.getPtr();
		}

		LightsArray getLights( LightType type )const
		{
			return m_typeSortedLights[size_t( type )];
		}

	private:
		std::pair< uint32_t, uint32_t > doGetOffsetIndex( Light const & light )const;
		void doMarkNextDirty( LightType type
			, uint32_t index );
		uint32_t doGetBufferEnd( LightType type )const noexcept;

	private:
		ShaderBuffer m_buffer;
		LightsMap m_typeSortedLights;
		std::vector< Light * > m_dirty;
		std::map< Light *, OnLightChangedConnection > m_connections;
		std::vector< uint32_t > m_lightSizes;
		LightsData m_data;
		mutable std::mutex m_mutex;
		bool m_wasDirty{};
	};
}

#endif
