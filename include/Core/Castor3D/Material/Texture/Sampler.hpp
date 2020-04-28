/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Sampler___
#define ___C3D_Sampler___

#include "TextureModule.hpp"

#include <ashespp/Image/SamplerCreateInfo.hpp>
#include <ashespp/Image/Sampler.hpp>

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	SamplerSPtr createSampler( Engine & engine
		, castor::String const & baseName
		, VkFilter filter
		, VkImageSubresourceRange const * range );

	class Sampler
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		/**
		\author		Sylvain DOREMUS
		\version	0.7.0
		\date		21/03/2014
		\~english
		\brief		Sampler loader
		\remark
		\~french
		\brief		Loader de Sampler
		*/
		class TextWriter
			: public castor::TextWriter< Sampler >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief			Writes a sampler into a text file
			 *\param[in]		sampler	The sampler to save
			 *\param[in,out]	file	The file where to save the sampler
			 *\~french
			 *\brief			Ecrit un échantillonneur dans un fichier texte
			 *\param[in]		sampler	L'échantillonneur
			 *\param[in,out]	file	Le fichier
			 */
			C3D_API bool operator()( Sampler const & sampler, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The sampler name
		 *\param[in]	engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom du sampler
		 *\param[in]	engine	Le moteur
		 */
		C3D_API Sampler( Engine & engine
			, castor::String const & name );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name	The sampler name
		 *\param[in]	engine	The core engine
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	name	Le nom du sampler
		 *\param[in]	engine	Le moteur
		 */
		C3D_API Sampler( Engine & engine
			, castor::String const & name
			, ashes::SamplerCreateInfo createInfo );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~Sampler();
		/**
		 *\~english
		 *\brief		Initialises the GPU sampler.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Initialise le sampler GPU.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise();
		/**
		 *\~english
		 *\brief		Cleanups the sampler
		 *\~french
		 *\brief		Nettoie le sampler
		 */
		C3D_API void cleanup();
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		inline VkSamplerAddressMode getWrapS()const
		{
			return m_info->addressModeU;
		}

		inline VkSamplerAddressMode getWrapT()const
		{
			return m_info->addressModeV;
		}

		inline VkSamplerAddressMode getWrapR()const
		{
			return m_info->addressModeW;
		}

		inline VkFilter getMinFilter()const
		{
			return m_info->minFilter;
		}

		inline VkFilter getMagFilter()const
		{
			return m_info->magFilter;
		}

		inline VkSamplerMipmapMode getMipFilter()const
		{
			return m_info->mipmapMode;
		}

		inline float getMinLod()const
		{
			return m_info->minLod;
		}

		inline float getMaxLod()const
		{
			return m_info->maxLod;
		}

		inline float getLodBias()const
		{
			return m_info->mipLodBias;
		}

		inline VkBorderColor getBorderColour()const
		{
			return m_info->borderColor;
		}

		inline float getMaxAnisotropy()const
		{
			return m_info->maxAnisotropy;
		}

		inline VkCompareOp getCompareOp()const
		{
			return m_info->compareOp;
		}

		inline ashes::Sampler const & getSampler()const
		{
			CU_Require( m_sampler );
			return *m_sampler;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Setters
		*\remarks
		*	They have an effect until initialise() is called.
		*\~french
		*name
		*	Mutateurs
		*\remarks
		*	ils ont un effet jusqu'à ce que initialise() soit appelée.
		**/
		/**@{*/
		inline void setWrapS( VkSamplerAddressMode value )
		{
			m_info->addressModeU = value;
		}

		inline void setWrapT( VkSamplerAddressMode value )
		{
			m_info->addressModeV = value;
		}

		inline void setWrapR( VkSamplerAddressMode value )
		{
			m_info->addressModeW = value;
		}

		inline void setMinFilter( VkFilter value )
		{
			m_info->minFilter = value;
		}

		inline void setMagFilter( VkFilter value )
		{
			m_info->magFilter = value;
		}

		inline void setMipFilter( VkSamplerMipmapMode value )
		{
			m_mipmapIsSet = true;
			m_info->mipmapMode = value;
		}

		inline void setMinLod( float value )
		{
			m_info->minLod = value;
		}

		inline void setMaxLod( float value )
		{
			m_info->maxLod = value;
		}

		inline void setLodBias( float value )
		{
			m_info->mipLodBias = value;
		}

		inline void setBorderColour( VkBorderColor value )
		{
			m_info->borderColor = value;
		}

		inline void enableAnisotropicFiltering( bool value )
		{
			m_info->anisotropyEnable = value;
		}

		inline void setMaxAnisotropy( float value )
		{
			m_info->maxAnisotropy = value;
		}

		inline void setCompareOp( VkCompareOp value )
		{
			m_info->compareOp = value;
		}

		inline void enableCompare( bool value )
		{
			m_info->compareEnable = value;
		}
		/**@}*/

	private:
		ashes::SamplerCreateInfo m_info
		{
			0u,
			VK_FILTER_NEAREST,
			VK_FILTER_NEAREST,
			VK_SAMPLER_MIPMAP_MODE_NEAREST,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE,
			0.0f,
			VK_FALSE,
			1.0f,
			VK_FALSE,
			VK_COMPARE_OP_NEVER,
			-1000.0f,
			1000.0f,
			VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK,
			false
		};
		bool m_mipmapIsSet{ false };
		ashes::SamplerPtr m_sampler;
	};
}

#endif
