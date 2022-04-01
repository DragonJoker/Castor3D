/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_CATEGORY_H___
#define ___C3D_LIGHT_CATEGORY_H___

#include "LightModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/GlobalIllumination/ReflectiveShadowMaps/ReflectiveShadowMapsModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Design/GroupChangeTracked.hpp>
#include <CastorUtils/Graphics/BoundingBox.hpp>

namespace castor3d
{
	class LightCategory
	{
	private:
		friend class Light;

	protected:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	lightType				The light category type.
		 *\param[in]	light					The parent Light.
		 *\param[in]	lightComponentCount		The number of vec4 used to store light data.
		 *\param[in]	shadowComponentCount	The number of vec4 used to store shadow data.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	lightType				Le type de catégorie de lumière.
		 *\param[in]	light					La Light parente.
		 *\param[in]	lightComponentCount		Le nombre de vec4 utilisés pour stocker les données de lumière.
		 *\param[in]	shadowComponentCount	Le nombre de vec4 utilisés pour stocker les données d'ombres.
		 */
		C3D_API explicit LightCategory( LightType lightType
			, Light & light
			, uint32_t lightComponentCount
			, uint32_t shadowComponentCount );

	public:
		struct LightData
			: ShaderBufferTypes
		{
			Float3 colour;
			Float1 range;
			Float2 intensity;
			Float1 shadowMapIndex;
			Float1 cascadeCount;
			Float3 posDir;
			Float1 enabled;
		};

		static uint32_t constexpr LightMbrAlign = 4u * sizeof( float );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~LightCategory()noexcept = default;
		/**
		 *\~english
		 *\brief		Updates the light.
		 *\~french
		 *\brief		Met la source à jour.
		 */
		C3D_API virtual void update() = 0;
		/**
		 *\~english
		 *\brief		Puts the light into the given buffer.
		 *\param[out]	data	Receives the light's data.
		 *\~french
		 *\brief		Met la lumière dans le buffer donné.
		 *\param[out]	data	Reçoit les données de la source lumineuse.
		 */
		C3D_API void fillLightBuffer( castor::Point4f * data )const;
		/**
		 *\~english
		 *\brief		Puts the shadow data into the given buffer.
		 *\param[out]	data	Receives the light's shadow data.
		 *\~french
		 *\brief		Met les données d'ombre dans le buffer donné.
		 *\param[out]	data	Reçoit les données d'ombres de la source lumineuse.
		 */
		C3D_API virtual void fillShadowBuffer( AllShadowData & data )const = 0;
		/**
		*\~english
		*\brief
		*	ConfigurationVisitorBase acceptance function.
		*\param vis
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de ConfigurationVisitorBase.
		*\param vis
		*	Le ... visiteur.
		*/
		C3D_API void accept( ConfigurationVisitorBase & vis );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API uint32_t getVolumetricSteps()const;
		C3D_API float getVolumetricScatteringFactor()const;
		C3D_API castor::Point2f const & getShadowRawOffsets()const;
		C3D_API castor::Point2f const & getShadowPcfOffsets()const;
		C3D_API float getVsmMinVariance()const;
		C3D_API float getVsmLightBleedingReduction()const;
		C3D_API castor::RangedValue< uint32_t > getShadowPcfFilterSize()const;
		C3D_API castor::RangedValue< uint32_t > getShadowPcfSampleCount()const;
		C3D_API ShadowConfig const & getShadowConfig()const;
		C3D_API LpvConfig const & getLpvConfig()const;

		LightType getLightType()const
		{
			return m_lightType;
		}

		uint32_t getLightComponentCount()const
		{
			return m_lightComponentCount;
		}

		uint32_t getShadowComponentCount()const
		{
			return m_shadowComponentCount;
		}

		float getDiffuseIntensity()const
		{
			return m_intensity.value()[0];
		}

		float getSpecularIntensity()const
		{
			return m_intensity.value()[1];
		}

		castor::Point2f const & getIntensity()const
		{
			return m_intensity;
		}

		float getFarPlane()const
		{
			return m_farPlane;
		}

		castor::Point3f const & getColour()const
		{
			return m_colour;
		}

		Light const & getLight()const
		{
			return m_light;
		}

		castor::BoundingBox const & getBoundingBox()const
		{
			return m_cubeBox;
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
		C3D_API void setVolumetricSteps( uint32_t value );
		C3D_API void setVolumetricScatteringFactor( float value );
		C3D_API void setRawMinOffset( float value );
		C3D_API void setRawMaxSlopeOffset( float value );
		C3D_API void setPcfMinOffset( float value );
		C3D_API void setPcfMaxSlopeOffset( float value );
		C3D_API void setPcfFilterSize( uint32_t value );
		C3D_API void setPcfSampleCount( uint32_t value );
		C3D_API void setVsmMinVariance( float value );
		C3D_API void setVsmLightBleedingReduction( float value );
		C3D_API void setColour( castor::Point3f const & value );
		C3D_API void setIntensity( castor::Point2f const & value );
		C3D_API void setDiffuseIntensity( float value );
		C3D_API void setSpecularIntensity( float value );

		Light & getLight()
		{
			return m_light;
		}
		/**@}*/

	protected:
		bool & m_dirty;

	private:
		LightType m_lightType;
		Light & m_light;
		uint32_t m_lightComponentCount{};
		uint32_t m_shadowComponentCount{};
		castor::GroupChangeTracked< castor::Point3f > m_colour;
		castor::GroupChangeTracked< castor::Point2f > m_intensity;
		/**
		 *\~english
		 *\brief		Puts the light into the given buffer.
		 *\param[out]	data	Receives the light's data.
		 *\~french
		 *\brief		Met la lumière dans le buffer donné.
		 *\param[out]	data	Reçoit les données de la source lumineuse.
		 */
		C3D_API virtual void doFillLightBuffer( castor::Point4f * data )const = 0;
		/**
		*\~english
		*\brief
		*	ConfigurationVisitorBase acceptance function.
		*\param vis
		*	The ... visitor.
		*\~french
		*\brief
		*	Fonction d'acceptation de ConfigurationVisitorBase.
		*\param vis
		*	Le ... visiteur.
		*/
		C3D_API virtual void doAccept( ConfigurationVisitorBase & vis ) = 0;

	protected:
		//!\~english	The cube box for the light volume of effect.
		//!\~french		La cube box pour le volume d'effet de la lumière.
		castor::BoundingBox m_cubeBox;
		//!\~english	The far plane's depth.
		//!\~french		La profondeur du plan éloigné.
		float m_farPlane{ 1.0f };
		/**
		 *\~english
		 *\brief		Puts the shadow data into the given buffer.
		 *\param[out]	data	Receives the light's shadow data.
		 *\~french
		 *\brief		Met les données d'ombre dans le buffer donné.
		 *\param[out]	data	Reçoit les données d'ombres de la source lumineuse.
		 */
		C3D_API void doFillBaseShadowData( BaseShadowData & data )const;
	};
}

#endif
