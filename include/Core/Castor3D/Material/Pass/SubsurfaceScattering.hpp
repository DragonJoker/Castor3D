/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScattering_H___
#define ___C3D_SubsurfaceScattering_H___

#include "MaterialModule.hpp"

#include <CastorUtils/Design/OwnedBy.hpp>
#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	class SubsurfaceScattering
	{
	public:
		using Changed = std::function< void( SubsurfaceScattering const & ) >;
		using OnChanged = castor::Signal< Changed >;
		using OnChangedConnection = OnChanged::connection;

	public:
		/*!
		\~english
		\brief Pass loader.
		\~french
		\brief Loader de Pass.
		*/
		class TextWriter
			: public castor::TextWriter< SubsurfaceScattering >
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor.
			 *\~french
			 *\brief		Constructeur.
			 */
			C3D_API explicit TextWriter( castor::String const & tabs );
			/**
			 *\~english
			 *\brief			Writes a SubsurfaceScattering into a text file.
			 *\param[in]		pass	The SubsurfaceScattering to write.
			 *\param[in,out]	file	The file where to write the Pass.
			 *\~french
			 *\brief			Ecrit une SubsurfaceScattering dans un fichier texte.
			 *\param[in]		pass	La SubsurfaceScattering à écrire.
			 *\param[in,out]	file	Le file où écrire la Pass.
			 */
			C3D_API bool operator()( SubsurfaceScattering const & pass, castor::TextFile & file )override;
		};

	public:
		/**
		 *\~english
		 *\return		The Gaussian filter width.
		 *\~french
		 *\return		La largeur du filtre Gaussien.
		 */
		inline float getGaussianWidth()const
		{
			return m_gaussianWidth;
		}
		/**
		 *\~english
		 *\brief		Sets the Gaussian filter width.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la largeur du filtre Gaussien.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setGaussianWidth( float value )
		{
			m_gaussianWidth = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\return		The scattering strength.
		 *\~french
		 *\return		La force du scattering.
		 */
		inline float getStrength()const
		{
			return m_strength;
		}
		/**
		 *\~english
		 *\brief		Sets the scattering strength.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la force du scattering.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setStrength( float value )
		{
			m_strength = value;
			onChanged( *this );
		}
		/**
		 *\~english
		 *\brief		Adds a factor to the transmittance profile.
		 *\param[in]	value	The value to add.
		 *\~french
		 *\brief		Ajoute un facteur au profil de transmission.
		 *\param[in]	value	La valeur à ajouter.
		 */
		inline void addProfileFactor( castor::Point4f const & value )
		{
			m_profileFactors.push_back( value );
			onChanged( *this );
		}
		/**
		 *\~english
		 *\return		The transmmittance profile factors count.
		 *\~french
		 *\return		Le nombre de facteurs du profil de transmission.
		 */
		inline uint32_t getProfileSize()const
		{
			return uint32_t( m_profileFactors.size() );
		}
		/**
		 *\~english
		 *\return		The iterator to the beginning of the transmittance profile.
		 *\~french
		 *\return		L'itérateur sur le début du profil de transmission.
		 */
		inline auto begin()
		{
			return m_profileFactors.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the beginning of the transmittance profile.
		 *\~french
		 *\return		L'itérateur sur le début du profil de transmission.
		 */
		inline auto begin()const
		{
			return m_profileFactors.begin();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the transmittance profile.
		 *\~french
		 *\return		L'itérateur sur la fin du profil de transmission.
		 */
		inline auto end()
		{
			return m_profileFactors.end();
		}
		/**
		 *\~english
		 *\return		The iterator to the end of the transmittance profile.
		 *\~french
		 *\return		L'itérateur sur la fin du profil de transmission.
		 */
		inline auto end()const
		{
			return m_profileFactors.end();
		}

	public:
		OnChanged onChanged;

	private:
		//!\~english	The Gaussian filter width.
		//!\~french		La largeur du filtre Gaussien.
		float m_gaussianWidth{ 1.0f };
		//!\~english	The scattering strength.
		//!\~french		La force du scattering.
		float m_strength{ 1.0f };
		//!\~english	The radius of the largest Gaussian.
		//!\~french		Le rayon du plus large filtre Gaussien.
		float m_subsurfaceRadius{ 1.0f };
		//!\~english	The radius of the largest Gaussian.
		//!\~french		Le rayon du plus large filtre Gaussien.
		castor::Point3f m_originalBlendFactors{ 1.0f, 1.0f, 1.0f };
		//!\~english	The radius of the largest Gaussian.
		//!\~french		Le rayon du plus large filtre Gaussien.
		castor::Point3f m_subsurfaceBlends{ 1.0f, 1.0f, 1.0f };
		//!\~english	The radius of the largest Gaussian.
		//!\~french		Le rayon du plus large filtre Gaussien.
		castor::Point3f m_subsurfaceGaussianExponents{ 1.0f, 1.0f, 1.0f };
		//!\~english	The colour used to modulate back-lit transmittance.
		//!\~french		La couleur utilisée pour moduler la lumière transmise par les faces arrières.
		castor::Point3f m_transmittanceCoefficients{ 1.0f, 1.0f, 1.0f };
		//!\~english	The transmittance profile factors.
		//!\~french		Les facteurs du profil de transmission.
		castor::Point4fArray m_profileFactors;
	};
}

#endif
