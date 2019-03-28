/*
See LICENSE file in root folder
*/
#ifndef ___C3D_TextureConfigurationBuffer_H___
#define ___C3D_TextureConfigurationBuffer_H___

#include "Castor3D/Texture/TextureConfiguration.hpp"
#include "Castor3D/Shader/ShaderBuffer.hpp"

#include <CastorUtils/Design/ArrayView.hpp>

#define C3D_TextureConfigStructOfArrays 0

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		09/02/2010
	\~english
	\brief		ShaderBuffer holding the TextureConfigurationes data.
	\~french
	\brief		ShaderBuffer contenant les données des TextureConfiguration.
	*/
	class TextureConfigurationBuffer
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine	The engine.
		 *\param[in]	count	The max configurations count.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine	Le moteur.
		 *\param[in]	count	Le nombre maximal de configurations.
		 */
		C3D_API TextureConfigurationBuffer( Engine & engine
			, uint32_t count );
		/**
		 *\~english
		 *\brief		Adds a configuration to the buffer.
		 *\param[in]	unit	The texture.
		 *\~french
		 *\brief		Ajoute une configuration au tampon.
		 *\param[in]	unit	La texture.
		 */
		C3D_API uint32_t addTextureConfiguration( TextureUnit & unit );
		/**
		 *\~english
		 *\brief		Removes a configuration from the buffer.
		 *\param[in]	unit	The texture.
		 *\~french
		 *\brief		Supprime une configuration du tampon.
		 *\param[in]	unit	La texture.
		 */
		C3D_API void removeTextureConfiguration( TextureUnit & unit );
		/**
		 *\~english
		 *\brief		Updates the configurations buffer.
		 *\~french
		 *\brief		Met à jour le tampon de configurations.
		 */
		C3D_API void update();
		/**
		 *\~english
		 *\brief		Creates the descriptor set layout binding.
		 *\~french
		 *\brief		Crée une attache de layout de set de descripteurs.
		 */
		C3D_API ashes::DescriptorSetLayoutBinding createLayoutBinding()const;
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
			, ashes::DescriptorSetLayoutBinding const & binding )const;
		/**
		 *\~english
		 *\return		The pointer to the buffer.
		 *\~french
		 *\brief		Le pointeur sur le tampon.
		 */
		inline uint8_t * getPtr()
		{
			return m_buffer.getPtr();
		}

	public:
#if C3D_TextureConfigStructOfArrays

		struct TextureConfigurationsData
		{
			castor::ArrayView< castor::Point4f > colrSpec;
			castor::ArrayView< castor::Point4f > glossOpa;
			castor::ArrayView< castor::Point4f > emisOccl;
			castor::ArrayView< castor::Point4f > trnsDumm;
			castor::ArrayView< castor::Point4f > normalFc;
			castor::ArrayView< castor::Point4f > heightFc;
			castor::ArrayView< castor::Point4f > miscVals;
		};

#else

		struct Data
		{
			castor::Point4f colrSpec;
			castor::Point4f glossOpa;
			castor::Point4f emisOccl;
			castor::Point4f trnsDumm;
			castor::Point4f normalFc;
			castor::Point4f heightFc;
			castor::Point4f miscVals;
		};

		using TextureConfigurationsData = castor::ArrayView< Data >;

		static uint32_t constexpr DataSize = uint32_t( sizeof( Data ) );

#endif

	private:
		ShaderBuffer m_buffer;
		std::vector< TextureUnit * > m_configurations;
		std::vector< TextureUnit const * > m_dirty;
		std::vector< OnTextureUnitChangedConnection > m_connections;
		uint32_t m_configMaxCount;
		uint32_t m_configID{ 1u };
		TextureConfigurationsData m_data;
	};
}

#endif
