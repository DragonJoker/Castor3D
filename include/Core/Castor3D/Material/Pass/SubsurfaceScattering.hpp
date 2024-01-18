/*
See LICENSE file in root folder
*/
#ifndef ___C3D_SubsurfaceScattering_H___
#define ___C3D_SubsurfaceScattering_H___

#include "PassModule.hpp"

#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Design/Signal.hpp>

namespace castor3d
{
	class SubsurfaceScattering
	{
	public:
		using Changed = std::function< void( SubsurfaceScattering const & ) >;
		using OnChanged = castor::SignalT< Changed >;
		using OnChangedConnection = OnChanged::connection;

	public:
		C3D_API SubsurfaceScattering();

		C3D_API void accept( ConfigurationVisitorBase & vis );
		/**
		 *\~english
		 *name Getters.
		 *\~french
		 *name Accesseurs.
		**/
		/**@{*/
		float getGaussianWidth()const noexcept
		{
			return m_gaussianWidth;
		}

		float getStrength()const noexcept
		{
			return m_strength;
		}

		float getThicknessScale()const noexcept
		{
			return m_thicknessScale;
		}

		float getSubsurfaceRadius()const noexcept
		{
			return m_subsurfaceRadius;
		}

		uint32_t getProfileSize()const noexcept
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

		void setThicknessScale( float value )
		{
			m_thicknessScale = value;
			onChanged( *this );
		}
		
		void setSubsurfaceRadius( float value )
		{
			m_subsurfaceRadius = value;
			onChanged( *this );
		}

		void addProfileFactor( castor::Point4f const & value )
		{
			m_dirty = true;
			m_profileFactors.emplace_back( m_dirty, value );
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
		auto begin()noexcept
		{
			return m_profileFactors.begin();
		}

		auto begin()const noexcept
		{
			return m_profileFactors.begin();
		}

		auto end()noexcept
		{
			return m_profileFactors.end();
		}

		auto end()const noexcept
		{
			return m_profileFactors.end();
		}
		/**@}*/

	public:
		OnChanged onChanged;

	private:
		bool m_dirty{ true };
		//!\~english	The Gaussian filter width.
		//!\~french		La largeur du filtre Gaussien.
		castor::GroupChangeTracked< float > m_gaussianWidth;
		//!\~english	The scattering strength.
		//!\~french		La force du scattering.
		castor::GroupChangeTracked< float > m_strength;
		//!\~english	The radius of the largest Gaussian.
		//!\~french		Le rayon du plus large filtre Gaussien.
		castor::GroupChangeTracked< float > m_subsurfaceRadius;
		//!\~english	The scale used when computing surface thickness.
		//!\~french		L'échelle utilisée lors du cacul de l'épaisseur de la surface.
		castor::GroupChangeTracked< float > m_thicknessScale;
		//!\~english	The radius of the largest Gaussian.
		//!\~french		Le rayon du plus large filtre Gaussien.
		castor::GroupChangeTracked< castor::Point3f > m_originalBlendFactors;
		//!\~english	The radius of the largest Gaussian.
		//!\~french		Le rayon du plus large filtre Gaussien.
		castor::GroupChangeTracked< castor::Point3f > m_subsurfaceBlends;
		//!\~english	The radius of the largest Gaussian.
		//!\~french		Le rayon du plus large filtre Gaussien.
		castor::GroupChangeTracked< castor::Point3f > m_subsurfaceGaussianExponents;
		//!\~english	The colour used to modulate back-lit transmittance.
		//!\~french		La couleur utilisée pour moduler la lumière transmise par les faces arrières.
		castor::GroupChangeTracked< castor::Point3f > m_transmittanceCoefficients;
		//!\~english	The transmittance profile factors.
		//!\~french		Les facteurs du profil de transmission.
		std::vector< castor::GroupChangeTracked< castor::Point4f > > m_profileFactors;
	};
}

#endif
