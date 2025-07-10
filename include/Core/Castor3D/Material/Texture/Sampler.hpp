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
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Multithreading/SpinMutex.hpp>

namespace castor3d
{
	C3D_API SamplerObs createSampler( Engine & engine
		, castor::String const & baseName
		, FilterMode filter
		, VkImageSubresourceRange const * range );
	C3D_API castor::String getSamplerName( ComparisonFunc compareOp
		, FilterMode minFilter
		, FilterMode magFilter
		, MipmapMode mipFilter
		, WrapMode U
		, WrapMode V
		, WrapMode W
		, BorderColour borderColor = BorderColour::eFloatTransparentBlack );

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
			, ashes::SamplerCreateInfo const & createInfo );
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

		C3D_API static void addParsers( castor::AttributeParsers & result );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		**/
		/**@{*/
		WrapMode getWrapS()const
		{
			return crg::convert( m_info.addressModeU );
		}

		WrapMode getWrapT()const
		{
			return crg::convert( m_info.addressModeV );
		}

		WrapMode getWrapR()const
		{
			return crg::convert( m_info.addressModeW );
		}

		FilterMode getMinFilter()const
		{
			return crg::convert( m_info.minFilter );
		}

		FilterMode getMagFilter()const
		{
			return crg::convert( m_info.magFilter );
		}

		MipmapMode getMipFilter()const
		{
			return crg::convert( m_info.mipmapMode );
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

		BorderColour getBorderColour()const
		{
			return convert( m_info.borderColor );
		}

		float getMaxAnisotropy()const
		{
			return m_info.maxAnisotropy;
		}

		ComparisonFunc getCompareOp()const
		{
			return convert( m_info.compareOp );
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

		bool isSerialisable()const
		{
			return m_serialisable;
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
		void setWrapS( WrapMode value )
		{
			m_info.addressModeU = convert( value );
		}

		void setWrapT( WrapMode value )
		{
			m_info.addressModeV = convert( value );
		}

		void setWrapR( WrapMode value )
		{
			m_info.addressModeW = convert( value );
		}

		void setMinFilter( FilterMode value )
		{
			m_info.minFilter = convert( value );
		}

		void setMagFilter( FilterMode value )
		{
			m_info.magFilter = convert( value );
		}

		void setMipFilter( MipmapMode value )
		{
			m_mipmapIsSet = true;
			m_info.mipmapMode = convert( value );
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

		void setBorderColour( BorderColour value )
		{
			m_info.borderColor = convert( value );
		}

		void enableAnisotropicFiltering( bool value )
		{
			m_info.anisotropyEnable = value;
		}

		void setMaxAnisotropy( float value )
		{
			m_info.maxAnisotropy = value;
		}

		void setCompareOp( ComparisonFunc value )
		{
			m_info.compareOp = convert( value );
		}

		void enableCompare( bool value )
		{
			m_info.compareEnable = value;
		}

		void setSerialisable( bool value )
		{
			m_serialisable = value;
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
		bool m_serialisable{ true };
		ashes::SamplerPtr m_sampler;
		std::atomic_bool m_initialised;
		std::atomic_bool m_initialising;
	};

	struct SamplerContext
	{
		SamplerObs sampler{};
		SamplerPtr ownSampler{};
	};

	inline Engine * getEngine( SamplerContext const & context )
	{
		return context.sampler->getEngine();
	}
}

#endif
