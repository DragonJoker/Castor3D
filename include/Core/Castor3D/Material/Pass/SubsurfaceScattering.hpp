/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScattering_H___
#define ___C3D_SubsurfaceScattering_H___

#include "PassModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
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
		/**
		 *\~english
		 *name Getters.
		 *\~french
		 *name Accesseurs.
		**/
		/**@{*/
		float getGaussianWidth()const
		{
			return m_gaussianWidth;
		}

		float getStrength()const
		{
			return m_strength;
		}

		float getSubsurfaceRadius()const
		{
			return m_subsurfaceRadius;
		}

		uint32_t getProfileSize()const
		{
			return uint32_t( m_profileFactors.size() );
		}
		/**@}*/
		/**
		 *\~english
		 *name Mutators.
		 *\~french
		 *name Mutateurs.
		**/
		/**@{*/
		void setGaussianWidth( float value )
		{
			m_gaussianWidth = value;
			onChanged( *this );
		}

		void setStrength( float value )
		{
			m_strength = value;
			onChanged( *this );
		}
		
		void setSubsurfaceRadius( float value )
		{
			m_subsurfaceRadius = value;
			onChanged( *this );
		}

		void addProfileFactor( castor::Point4f const & value )
		{
			m_profileFactors.push_back( value );
			onChanged( *this );
		}
		/**@}*/
		/**
		 *\~english
		 *name Iteration.
		 *\~french
		 *name Itération.
		**/
		/**@{*/
		auto begin()
		{
			return m_profileFactors.begin();
		}

		auto begin()const
		{
			return m_profileFactors.begin();
		}

		auto end()
		{
			return m_profileFactors.end();
		}

		auto end()const
		{
			return m_profileFactors.end();
		}
		/**@}*/

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
