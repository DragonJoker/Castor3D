/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Sampler___
#define ___C3D_Sampler___

#include "Castor3DPrerequisites.hpp"

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
			return m_wrapS;
		}

		inline renderer::WrapMode getWrapT()const
		{
			return m_wrapT;
		}

		inline renderer::WrapMode getWrapR()const
		{
			return m_wrapR;
		}

		inline renderer::Filter getMinFilter()const
		{
			return m_minFilter;
		}

		inline renderer::Filter getMagFilter()const
		{
			return m_magFilter;
		}

		inline renderer::MipmapMode getMipFilter()const
		{
			return m_mipFilter;
		}

		inline float getMinLod()const
		{
			return m_minLod;
		}

		inline float getMaxLod()const
		{
			return m_maxLod;
		}

		inline float getLodBias()const
		{
			return m_lodBias;
		}

		inline renderer::BorderColour getBorderColour()const
		{
			return m_borderColour;
		}

		inline float getMaxAnisotropy()const
		{
			return m_maxAnisotropy;
		}

		inline renderer::CompareOp getCompareOp()const
		{
			return m_compareOp;
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
			m_wrapS = value;
		}

		inline renderer::WrapMode setWrapT( renderer::WrapMode value )
		{
			m_wrapT = value;
		}

		inline renderer::WrapMode setWrapR( renderer::WrapMode value )
		{
			m_wrapR = value;
		}

		inline renderer::Filter setMinFilter( renderer::Filter value )
		{
			m_minFilter = value;
		}

		inline renderer::Filter setMagFilter( renderer::Filter value )
		{
			m_magFilter = value;
		}

		inline renderer::MipmapMode setMipFilter( renderer::MipmapMode value )
		{
			m_mipFilter = value;
		}

		inline float setMinLod( float value )
		{
			m_minLod = value;
		}

		inline float setMaxLod( float value )
		{
			m_maxLod = value;
		}

		inline float setLodBias( float value )
		{
			m_lodBias = value;
		}

		inline renderer::BorderColour setBorderColour( renderer::BorderColour value )
		{
			m_borderColour = value;
		}

		inline float setMaxAnisotropy( float value )
		{
			m_maxAnisotropy = value;
		}

		inline void setCompareOp( renderer::CompareOp value )
		{
			m_compareOp = value;
		}
		/**@}*/

	private:
		renderer::WrapMode m_wrapS;
		renderer::WrapMode m_wrapT;
		renderer::WrapMode m_wrapR;
		renderer::Filter m_minFilter;
		renderer::Filter m_magFilter;
		renderer::MipmapMode m_mipFilter;
		float m_minLod;
		float m_maxLod;
		float m_lodBias;
		renderer::BorderColour m_borderColour;
		float m_maxAnisotropy;
		renderer::CompareOp m_compareOp;
		renderer::SamplerPtr m_sampler;
	};
}

#endif
