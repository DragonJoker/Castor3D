/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___C3D_SubsurfaceScattering_H___
#define ___C3D_SubsurfaceScattering_H___

#include "Castor3DPrerequisites.hpp"

#include <Design/OwnedBy.hpp>
#include <Design/Signal.hpp>

namespace castor3d
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.10.0
	\date		11/09/2017
	\~english
	\brief		Extended pass information, related to subsurface scattering.
	\~french
	\brief		Informations étendues de la passe, relatives au subsurface scattering.
	*/
	class SubsurfaceScattering
	{
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
		 *\return		The Gaussian filters count.
		 *\~french
		 *\return		Le nombre de filtres Gaussiens.
		 */
		inline uint32_t getGaussianCount()const
		{
			return m_gaussianCount;
		}
		/**
		 *\~english
		 *\brief		Sets the Gaussian filters count.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le nombre de filtres Gaussiens.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setGaussianCount( uint32_t const & value )
		{
			m_gaussianCount = value;
		}
		/**
		 *\~english
		 *\return		Tells if the back-lit transmittance is enabled.
		 *\~french
		 *\return		Dit si la transmission de lumière sur les faces arrières est activée.
		 */
		inline bool isDistanceBasedTransmittanceEnabled()const
		{
			return m_enableDistanceBasedTransmittance;
		}
		/**
		 *\~english
		 *\brief		Sets the back-lit transmittance activation status.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit le statut d'activation de la transmission de lumière par les faces arrières.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void enableDistanceBasedTransmittance( bool value = true )
		{
			m_enableDistanceBasedTransmittance = value;
		}
		/**
		 *\~english
		 *\return		The colour used to modulate back-lit transmittance.
		 *\~french
		 *\return		La couleur utilisée pour moduler la lumière transmise par les faces arrières.
		 */
		inline castor::Point3f const & getTransmittanceCoefficients()const
		{
			return m_transmittanceCoefficients;
		}
		/**
		 *\~english
		 *\brief		Sets the colour used to modulate back-lit transmittance.
		 *\param[in]	value	The new value.
		 *\~french
		 *\brief		Définit la couleur utilisée pour moduler la lumière transmise par les faces arrières.
		 *\param[in]	value	La nouvelle valeur.
		 */
		inline void setTransmittanceCoefficients( castor::Point3f const & value )
		{
			m_transmittanceCoefficients = value;
		}

	private:
		//!\~english	The Gaussian filter count.
		//!\~french		Le nombre de filtres Gaussiens.
		uint32_t m_gaussianCount{ 1u };
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
		//!\~english	Tells if the back-lit transmittance is enabled.
		//!\~french		Dit si la transmission de lumière sur les faces arrières est activée.
		bool m_enableDistanceBasedTransmittance{ false };
		//!\~english	The colour used to modulate back-lit transmittance.
		//!\~french		La couleur utilisée pour moduler la lumière transmise par les faces arrières.
		castor::Point3f m_transmittanceCoefficients{ 1.0f, 1.0f, 1.0f };
	};
}

#endif
