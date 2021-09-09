/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Sampler___
#define ___C3D_Sampler___

#include "TextureModule.hpp"
#include "Castor3D/Render/RenderModule.hpp"

#include <ashespp/Image/SamplerCreateInfo.hpp>
#include <ashespp/Image/Sampler.hpp>

#include <CastorUtils/Design/Named.hpp>

namespace castor3d
{
	SamplerResPtr createSampler( Engine & engine
		, castor::String const & baseName
		, VkFilter filter
		, VkImageSubresourceRange const * range );

	class Sampler
		: public castor::Named
		, public castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	engine	The engine
		 *\param[in]	name	The sampler name
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	engine	Le moteur
		 *\param[in]	name	Le nom du sampler
		 */
		C3D_API Sampler( castor::String const & name
			, Engine & engine );
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	engine		The engine.
		 *\param[in]	name		The sampler name.
		 *\param[in]	createInfo	The creation infos.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	engine		Le moteur.
		 *\param[in]	name		Le nom du sampler.
		 *\param[in]	createInfo	Les informations de création.
		 */
		C3D_API Sampler( castor::String const & name
			, Engine & engine
			, ashes::SamplerCreateInfo createInfo );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Sampler();
		/**
		 *\~english
		 *\brief		Initialises the GPU sampler.
		 *\param[in]	device	The GPU device.
		 *\return		\p true if ok.
		 *\~french
		 *\brief		Initialise le sampler GPU.
		 *\param[in]	device	Le device GPU.
		 *\return		\p true si tout s'est bien passé.
		 */
		C3D_API bool initialise( RenderDevice const & device );
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
		VkSamplerAddressMode getWrapS()const
		{
			return m_info.addressModeU;
		}

		VkSamplerAddressMode getWrapT()const
		{
			return m_info.addressModeV;
		}

		VkSamplerAddressMode getWrapR()const
		{
			return m_info.addressModeW;
		}

		VkFilter getMinFilter()const
		{
			return m_info.minFilter;
		}

		VkFilter getMagFilter()const
		{
			return m_info.magFilter;
		}

		VkSamplerMipmapMode getMipFilter()const
		{
			return m_info.mipmapMode;
		}

		float getMinLod()const
		{
			return m_info.minLod;
		}

		float getMaxLod()const
		{
			return m_info.maxLod;
		}

		float getLodBias()const
		{
			return m_info.mipLodBias;
		}

		VkBorderColor getBorderColour()const
		{
			return m_info.borderColor;
		}

		float getMaxAnisotropy()const
		{
			return m_info.maxAnisotropy;
		}

		VkCompareOp getCompareOp()const
		{
			return m_info.compareOp;
		}

		ashes::Sampler const & getSampler()const
		{
			CU_Require( m_sampler );
			return *m_sampler;
		}

		bool isAnisotropicFilteringEnabled()const
		{
			return m_info.anisotropyEnable == VK_TRUE;
		}

		bool isMipmapSet()const
		{
			return m_mipmapIsSet;
		}

		bool isCompareEnabled()const
		{
			return m_info.compareEnable == VK_TRUE;
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
		void setWrapS( VkSamplerAddressMode value )
		{
			m_info.addressModeU = value;
		}

		void setWrapT( VkSamplerAddressMode value )
		{
			m_info.addressModeV = value;
		}

		void setWrapR( VkSamplerAddressMode value )
		{
			m_info.addressModeW = value;
		}

		void setMinFilter( VkFilter value )
		{
			m_info.minFilter = value;
		}

		void setMagFilter( VkFilter value )
		{
			m_info.magFilter = value;
		}

		void setMipFilter( VkSamplerMipmapMode value )
		{
			m_mipmapIsSet = true;
			m_info.mipmapMode = value;
		}

		void setMinLod( float value )
		{
			m_info.minLod = value;
		}

		void setMaxLod( float value )
		{
			m_info.maxLod = value;
		}

		void setLodBias( float value )
		{
			m_info.mipLodBias = value;
		}

		void setBorderColour( VkBorderColor value )
		{
			m_info.borderColor = value;
		}

		void enableAnisotropicFiltering( bool value )
		{
			m_info.anisotropyEnable = value;
		}

		void setMaxAnisotropy( float value )
		{
			m_info.maxAnisotropy = value;
		}

		void setCompareOp( VkCompareOp value )
		{
			m_info.compareOp = value;
		}

		void enableCompare( bool value )
		{
			m_info.compareEnable = value;
		}
		/**@}*/

	private:
		VkSamplerCreateInfo m_info{ VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO
			, nullptr
			, 0u
			, VK_FILTER_NEAREST
			, VK_FILTER_NEAREST
			, VK_SAMPLER_MIPMAP_MODE_NEAREST
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, VK_SAMPLER_ADDRESS_MODE_CLAMP_TO_EDGE
			, 0.0f
			, VK_FALSE
			, 1.0f
			, VK_FALSE
			, VK_COMPARE_OP_NEVER
			, -1000.0f
			, 1000.0f
			, VK_BORDER_COLOR_FLOAT_OPAQUE_BLACK
			, VK_FALSE
		};
		bool m_mipmapIsSet{ false };
		ashes::SamplerPtr m_sampler;
	};
}

#endif
