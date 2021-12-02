/*
See LICENSE file in root folder
*/
#ifndef ___C3D_PassBuffer_H___
#define ___C3D_PassBuffer_H___

#include "Castor3D/Material/Pass/PassModule.hpp"

#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>
#include <CastorUtils/Graphics/RgbaColour.hpp>

#include <ashespp/Descriptor/DescriptorSet.hpp>

#pragma warning( push )
#pragma warning( disable:4365 )
#include <mutex>
#pragma warning( pop )

#define C3D_MaterialsStructOfArrays 0

namespace castor3d
{
	class PassBuffer
	{
	public:
		/**
		\~english
		\brief		3 components colour.
		\~french
		\brief		Couleur à 3 composantes.
		*/
		struct RgbColour
		{
			float r;
			float g;
			float b;
		};
		/**
		\~english
		\brief		4 components colour.
		\~french
		\brief		Couleur à 4 composantes.
		*/
		struct RgbaColour
		{
			float r;
			float g;
			float b;
			float a;
		};

#if C3D_MaterialsStructOfArrays

		struct ExtendedData
		{
			castor::ArrayView< RgbaColour > sssInfo;
			castor::ArrayView< std::array< RgbaColour, 10u > > transmittanceProfile;
		};

		struct PassesData
		{
			castor::ArrayView< RgbaColour > colourDiv;
			castor::ArrayView< RgbaColour > specDiv;
			castor::ArrayView< RgbaColour > edgeFactors;
			castor::ArrayView< RgbaColour > edgeColour;
			castor::ArrayView< RgbaColour > specific;
			castor::ArrayView< RgbaColour > common;
			castor::ArrayView< RgbaColour > opacity;
			castor::ArrayView< RgbaColour > reflRefr;
			ExtendedData extended;
		};

#else

		struct ExtendedData
		{
			RgbaColour sssInfo;
			std::array< RgbaColour, 10u > transmittanceProfile;
		};

		struct PassData
		{
			RgbaColour colourDiv;
			RgbaColour specDiv;
			RgbaColour edgeFactors;
			RgbaColour edgeColour;
			RgbaColour specific;
			RgbaColour common;
			RgbaColour opacity;
			RgbaColour reflRefr;
			ExtendedData extended;
		};
		using PassesData = castor::ArrayView< PassData >;

#endif

		struct ExtendedDataPtr
		{
			RgbaColour * sssInfo;
			std::array< RgbaColour, 10u > * transmittanceProfile;
		};

		struct PassDataPtr
		{
			RgbaColour * colourDiv;
			RgbaColour * specDiv;
			RgbaColour * edgeFactors;
			RgbaColour * edgeColour;
			RgbaColour * specific;
			RgbaColour * common;
			RgbaColour * opacity;
			RgbaColour * reflRefr;
			ExtendedDataPtr extended;
		};

		static constexpr uint32_t DataSize = sizeof( PassData );

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	device	The GPU device.
		 *\param[in]	count	The max passes count.
		 *\param[in]	size	The size of a pass.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	device	Le device GPU.
		 *\param[in]	count	Le nombre maximal de passes.
		 *\param[in]	size	La taille d'une passe.
		 */
		C3D_API PassBuffer( Engine & engine
			, RenderDevice const & device
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a pass to the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Ajoute une passe au tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API uint32_t addPass( Pass & pass );
		/**
		 *\~english
		 *\brief		Removes a pass from the buffer.
		 *\param[in]	pass	The pass.
		 *\~french
		 *\brief		Supprime une pass du tampon.
		 *\param[in]	pass	La passe.
		 */
		C3D_API void removePass( Pass & pass );
		/**
		 *\~english
		 *\brief		Updates the passes buffer.
		 *\~french
		 *\brief		Met à jour le tampon de passes.
		 */
		C3D_API void update( ashes::CommandBuffer const & commandBuffer );
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API VkDescriptorSetLayoutBinding createLayoutBinding( uint32_t binding )const;
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
		 *\return		The pointer to the data for given pass ID.
		 *\~french
		 *\brief		Le pointeur sur les données pour l'ID de passe donné.
		 */
		C3D_API PassDataPtr getData( uint32_t passID );
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
		/**
		 *\~english
		 *\return		The buffer descriptor type.
		 *\~french
		 *\brief		Le type de descripteur du tampon.
		 */
		VkDescriptorType getType()const
		{
			return m_buffer.getType();
		}

	private:
		ShaderBuffer m_buffer;
		std::vector< Pass * > m_passes;
		std::vector< Pass const * > m_dirty;
		std::vector< OnPassChangedConnection > m_connections;
		uint32_t m_passID{ 1u };
		PassesData m_data;
		std::mutex m_mutex;
	};
}

#endif
