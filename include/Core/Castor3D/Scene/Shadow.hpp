/*
See LICENSE file in root folder
*/
#ifndef ___C3D_Shadow_H___
#define ___C3D_Shadow_H___

#include "SceneModule.hpp"

namespace castor3d
{
	class Shadow
	{
	public:
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		ShadowType getFilterType()const
		{
			return m_filterType;
		}

		uint32_t getVolumetricSteps()const
		{
			return m_volumetricSteps;
		}

		float getVolumetricScatteringFactor()const
		{
			return m_volumetricScattering;
		}

		castor::Point2f const & getShadowRawOffsets()const
		{
			return m_rawOffsets;
		}

		castor::Point2f const & getShadowPcfOffsets()const
		{
			return m_pcfOffsets;
		}

		castor::Point2f const & getShadowVariance()const
		{
			return m_variance;
		}
		/**@}*/
		/**
		*\~english
		*name
		*	Mutators.
		*\~french
		*name
		*	Mutateurs.
		*/
		/**@{*/
		void setFilterType( ShadowType value )
		{
			m_filterType = value;
		}

		void setVolumetricSteps( uint32_t value )
		{
			m_volumetricSteps = value;
		}

		void setVolumetricScatteringFactor( float value )
		{
			m_volumetricScattering = value;
		}

		void setRawMinOffset( float value )
		{
			m_rawOffsets[0] = value;
		}

		void setRawMaxSlopeOffset( float value )
		{
			m_rawOffsets[1] = value;
		}

		void setPcfMinOffset( float value )
		{
			m_pcfOffsets[0] = value;
		}

		void setPcfMaxSlopeOffset( float value )
		{
			m_pcfOffsets[1] = value;
		}

		void setVsmMaxVariance( float value )
		{
			m_variance[0] = value;
		}

		void setVsmVarianceBias( float value )
		{
			m_variance[1] = value;
		}
		/**@}*/

	private:
		ShadowType m_filterType{ ShadowType::eNone };
		uint32_t m_volumetricSteps{ 0u };
		float m_volumetricScattering{ 0.2f };
		castor::Point2f m_rawOffsets;
		castor::Point2f m_pcfOffsets;
		castor::Point2f m_variance;
	};
}

#endif
