/*
See LICENSE file in root folder
*/
#ifndef ___C3D_GpuInformations_H___
#define ___C3D_GpuInformations_H___

#include "MiscellaneousModule.hpp"

namespace castor3d
{
	/**
	\author 	Sylvain DOREMUS
	\date 		04/05/2016
	\version	0.9.0
	\~english
	\brief		Holds GPU informations.
	\~french
	\brief		Contient des informations sur le GPU.
	*/
	class GpuInformations
	{
		friend class Context;

	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		inline GpuInformations()
			: m_useShader
			{
				{ VK_SHADER_STAGE_VERTEX_BIT, false },
				{ VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT, false },
				{ VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT, false },
				{ VK_SHADER_STAGE_GEOMETRY_BIT, false },
				{ VK_SHADER_STAGE_FRAGMENT_BIT, false },
				{ VK_SHADER_STAGE_COMPUTE_BIT, false },
			}
		{
			for ( auto i = 0u; i < uint32_t( GpuMax::eCount ); ++i )
			{
				m_maxValues.insert( { GpuMax( i ), std::numeric_limits< int32_t >::lowest() } );
			}
		}
		/**
		 *\~english
		 *\brief		Adds a supported feature.
		 *\~french
		 *\brief		Ajoute une caractéristique supportée.
		 */
		inline void addFeature( GpuFeature feature )
		{
			addFlag( m_features, feature );
		}
		/**
		 *\~english
		 *\brief		Removes a supported feature.
		 *\~french
		 *\brief		Enlève une caractéristique supportée.
		 */
		inline void removeFeature( GpuFeature feature )
		{
			remFlag( m_features, feature );
		}
		/**
		 *\~english
		 *\brief		Updates the support for a feature.
		 *\~french
		 *\brief		Met à jour le support d'une caractéristique.
		 */
		inline void updateFeature( GpuFeature feature, bool supported )
		{
			supported
				? addFeature( feature )
				: removeFeature( feature );
		}
		/**
		 *\~english
		 *\brief		Tells if the feature is supported supports stereo
		 *\~french
		 *\brief		Dit si la caractéristique est supportée.
		 */
		inline bool hasFeature( GpuFeature feature )const
		{
			return checkFlag( m_features, feature );
		}
		/**
		 *\~english
		 *\return		The stereo support status.
		 *\~french
		 *\return		Le statut du support de la stéréo.
		 */
		inline bool hasStereoRendering()const
		{
			return hasFeature( GpuFeature::eStereoRendering );
		}
		/**
		 *\~english
		 *\return		The SSBO support status.
		 *\~french
		 *\return		Le statut du support des SSBO.
		 */
		inline bool hasShaderStorageBuffers()const
		{
			return hasFeature( GpuFeature::eShaderStorageBuffers );
		}
		/**
		 *\~english
		 *\param[in]	type	The shader type.
		 *\return		The shader type support status.
		 *\~french
		 *\param[in]	type	Le type de shader.
		 *\return		Le statut du support du type de shader.
		 */
		inline bool hasShaderType( VkShaderStageFlagBits type )const
		{
			return m_useShader.at( type );
		}
		/**
		 *\~english
		 *\brief		Defines the support for given shader type.
		 *\param[in]	type	The shader type.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le support du type de shader donné.
		 *\param[in]	type	Le type de shader.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void useShaderType( VkShaderStageFlagBits type, bool value )
		{
			m_useShader[type] = value;
		}
		/**
		 *\~english
		 *\return		The shader language version.
		 *\~french
		 *\return		La version du langage shader.
		 */
		inline std::string const & getShaderLanguageVersion()const
		{
			return m_shaderLanguageVersion;
		}
		/**
		 *\~english
		 *\brief		Defines the shader language version.
		 *\param[in]	value	The version.
		 *\~french
		 *\brief		Définit la version du langage shader.
		 *\param[in]	value	La version.
		 */
		inline void setShaderLanguageVersion( std::string value )
		{
			m_shaderLanguageVersion = std::move( value );
		}
		/**
		 *\~english
		 *\param[in]	index	The index.
		 *\return		The maximum value for given index.
		 *\~french
		 *\param[in]	index	L'index.
		 *\return		La valeur maximale pour l'index défini.
		 */
		inline int32_t getMaxValue( GpuMax index )const
		{
			return m_maxValues.find( index )->second;
		}
		/**
		 *\~english
		 *\param[in]	index	The index.
		 *\param[in]	value	The maximum value for given index.
		 *\~french
		 *\param[in]	index	L'index.
		 *\param[in]	value	La valeur maximale pour l'index défini.
		 */
		inline void setMaxValue( GpuMax index, int32_t value )
		{
			m_maxValues[index] = value;
		}
		/**
		 *\~english
		 *\return		The total VRAM size.
		 *\~french
		 *\return		La taille totale de la VRAM.
		 */
		inline uint32_t getTotalMemorySize()const
		{
			return m_totalMemorySize;
		}
		/**
		 *\~english
		 *\param[in]	value	The total VRAM size.
		 *\~french
		 *\param[in]	value	La taille totale de la VRAM.
		 */
		inline void setTotalMemorySize( uint32_t value )
		{
			m_totalMemorySize = value;
		}
		/**
		 *\~english
		 *\return		The GPU vendor name.
		 *\~french
		 *\return		Le nom du vendeur du GPU.
		 */
		inline castor::String const & getVendor()const
		{
			return m_vendor;
		}
		/**
		 *\~english
		 *\param[in]	value	The GPU vendor name.
		 *\~french
		 *\param[in]	value	Le nom du vendeur du GPU.
		 */
		inline void setVendor( castor::String const & value )
		{
			m_vendor = value;
		}
		/**
		 *\~english
		 *\return		The GPU platform.
		 *\~french
		 *\return		Le type de GPU.
		 */
		inline castor::String const & getRenderer()const
		{
			return m_renderer;
		}
		/**
		 *\~english
		 *\param[in]	value	The GPU platform.
		 *\~french
		 *\param[in]	value	Le type de GPU.
		 */
		inline void setRenderer( castor::String const & value )
		{
			m_renderer = value;
		}
		/**
		 *\~english
		 *\return		The rendering API version.
		 *\~french
		 *\return		La version de l'API de rendu.
		 */
		inline castor::String const & getVersion()const
		{
			return m_version;
		}
		/**
		 *\~english
		 *\param[in]	value	The rendering API version.
		 *\~french
		 *\param[in]	value	La version de l'API de rendu.
		 */
		inline void setVersion( castor::String const & value )
		{
			m_version = value;
		}

	private:
		GpuFeatures m_features{ 0u };
		std::string m_shaderLanguageVersion{ 0 };
		std::map< VkShaderStageFlagBits, bool > m_useShader;
		std::map< GpuMax, int32_t > m_maxValues;
		uint32_t m_totalMemorySize;
		castor::String m_vendor;
		castor::String m_renderer;
		castor::String m_version;
	};
	/**
	 *\~english
	 *\brief			Output stream operator.
	 *\param[in,out]	stream	The stream.
	 *\param[in]		object	The object to put in the stream.
	 *\return			The stream.
	 *\~french
	 *\brief			Opérateur de flux de sortie.
	 *\param[in,out]	stream	Le flux.
	 *\param[in]		object	L'objet à mettre dans le flux.
	 *\return			Le flux
	 */
	C3D_API std::ostream & operator<<( std::ostream & stream, GpuInformations const & object );
}

#endif
