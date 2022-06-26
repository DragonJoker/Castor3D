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

namespace castor3d
{
	class PassBuffer
	{
	public:
		struct Vec3
		{
			float r;
			float g;
			float b;
		};

		struct Vec4
		{
			float r;
			float g;
			float b;
			float a;
		};

		struct PassData
		{
			Vec4 colourDiv;
			Vec4 specDiv;

			float edgeWidth;
			float depthFactor;
			float normalFactor;
			float objectFactor;

			Vec4 edgeColour;
			Vec4 specific;

			uint32_t index;
			float emissive;
			float alphaRef;
			uint32_t sssProfileIndex;

			Vec3 transmission;
			float opacity;

			float refractionRatio;
			int hasRefraction;
			int hasReflection;
			float bwAccumulationOperator;

			std::array< castor::Point4ui, 2u > textures;

			int32_t textureCount;
			int32_t pad[3];
		};
		using PassesData = castor::ArrayView< PassData >;

		struct PassDataPtr
		{
			Vec4 * colourDiv;
			Vec4 * specDiv;
			float * edgeWidth;
			float * depthFactor;
			float * normalFactor;
			float * objectFactor;
			Vec4 * edgeColour;
			Vec4 * specific;
			uint32_t * index;
			float * emissive;
			float * alphaRef;
			uint32_t * sssProfileIndex;
			Vec3 * transmission;
			float * opacity;
			float * refractionRatio;
			int * hasRefraction;
			int * hasReflection;
			float * bwAccumulationOperator;
			std::array< castor::Point4ui, 2u > * textures;
			int32_t * textureCount;
		};

		static constexpr uint32_t DataSize = sizeof( PassData );

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
		 *\param[in]	commandBuffer	Receives the update commands.
		 *\~french
		 *\brief		Met à jour le tampon de passes.
		 *\param[in]	commandBuffer	Reçoit les commandes de mise à jour.
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
