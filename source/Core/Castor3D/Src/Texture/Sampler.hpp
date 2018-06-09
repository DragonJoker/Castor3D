/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Sampler___
#define ___C3D_Sampler___

#include "Castor3DPrerequisites.hpp"

#include <Image/SamplerCreateInfo.hpp>
#include <Image/Sampler.hpp>

#include <Design/Named.hpp>
#include <Design/OwnedBy.hpp>
#include <Graphics/Colour.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		25/03/2013
	\version	0.7.0
	\~english
	\brief		Defines a sampler for a texture
	\~french
	\brief		Définit un sampler pour une texture
	*/
	class Sampler
		: public castor::OwnedBy< Engine >
		, public castor::Named
	{
	public:
		/*!
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
		C3D_API Sampler( Engine & engine, castor::String const & name = castor::cuEmptyString );
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
		inline renderer::WrapMode getWrapS()const
		{
			return m_info.addressModeU;
		}

		inline renderer::WrapMode getWrapT()const
		{
			return m_info.addressModeV;
		}

		inline renderer::WrapMode getWrapR()const
		{
			return m_info.addressModeW;
		}

		inline renderer::Filter getMinFilter()const
		{
			return m_info.minFilter;
		}

		inline renderer::Filter getMagFilter()const
		{
			return m_info.magFilter;
		}

		inline renderer::MipmapMode getMipFilter()const
		{
			return m_info.mipmapMode;
		}

		inline float getMinLod()const
		{
			return m_info.minLod;
		}

		inline float getMaxLod()const
		{
			return m_info.maxLod;
		}

		inline float getLodBias()const
		{
			return m_info.mipLodBias;
		}

		inline renderer::BorderColour getBorderColour()const
		{
			return m_info.borderColor;
		}

		inline float getMaxAnisotropy()const
		{
			return m_info.maxAnisotropy;
		}

		inline renderer::CompareOp getCompareOp()const
		{
			return m_info.compareOp;
		}

		inline renderer::Sampler const & getSampler()const
		{
			REQUIRE( m_sampler );
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
		*\remraks
		*	ils ont un effet jusqu'à ce que initialise() soit appelée.
		**/
		/**@{*/
		inline void setWrapS( renderer::WrapMode value )
		{
			m_info.addressModeU = value;
		}

		inline void setWrapT( renderer::WrapMode value )
		{
			m_info.addressModeV = value;
		}

		inline void setWrapR( renderer::WrapMode value )
		{
			m_info.addressModeW = value;
		}

		inline void setMinFilter( renderer::Filter value )
		{
			m_info.minFilter = value;
		}

		inline void setMagFilter( renderer::Filter value )
		{
			m_info.magFilter = value;
		}

		inline void setMipFilter( renderer::MipmapMode value )
		{
			m_mipmapIsSet = true;
			m_info.mipmapMode = value;
		}

		inline void setMinLod( float value )
		{
			m_info.minLod = value;
		}

		inline void setMaxLod( float value )
		{
			m_info.maxLod = value;
		}

		inline void setLodBias( float value )
		{
			m_info.mipLodBias = value;
		}

		inline void setBorderColour( renderer::BorderColour value )
		{
			m_info.borderColor = value;
		}

		inline void enableAnisotropicFiltering( bool value )
		{
			m_info.anisotropyEnable = value;
		}

		inline void setMaxAnisotropy( float value )
		{
			m_info.maxAnisotropy = value;
		}

		inline void setCompareOp( renderer::CompareOp value )
		{
			m_info.compareOp = value;
		}

		inline void enableCompare( bool value )
		{
			m_info.compareEnable = value;
		}
		/**@}*/

	private:
		renderer::SamplerCreateInfo m_info
		{
			renderer::Filter::eNearest,
			renderer::Filter::eNearest,
			renderer::MipmapMode::eNone,
			renderer::WrapMode::eClampToEdge,
			renderer::WrapMode::eClampToEdge,
			renderer::WrapMode::eClampToEdge,
			0.0f,
			false,
			1.0f,
			false,
			renderer::CompareOp::eNever,
			-1000.0f,
			1000.0f,
			renderer::BorderColour::eFloatOpaqueBlack,
			false
		};
		bool m_mipmapIsSet{ false };
		renderer::SamplerPtr m_sampler;
	};
}

#endif
