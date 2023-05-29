/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_H___
#define ___C3D_LIGHT_H___

#include "LightModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include "Castor3D/Scene/MovableObject.hpp"
#include "Castor3D/Scene/Shadow.hpp"
#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace castor3d
{
	class Light
		: public MovableObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	name		The light name.
		 *\param[in]	scene		Parent scene.
		 *\param[in]	node		The parent scene node.
		 *\param[in]	factory		Factory used to create the LightCategory.
		 *\param[in]	lightType	The light type.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name		Le nom de la lumière.
		 *\param[in]	scene		La scène parente.
		 *\param[in]	node		Le scene node parent.
		 *\param[in]	factory		La fabrique de LightCategory.
		 *\param[in]	lightType	Le type de lumière.
		 */
		C3D_API Light( castor::String const & name
			, Scene & scene
			, SceneNode & node
			, LightFactory & factory
			, LightType lightType );
		/**
		 *\~english
		 *\brief			CPU Update.
		 *\param[in, out]	updater	The update data.
		 *\~french
		 *\brief			Mise à jour CPU.
		 *\param[in, out]	updater	Les données d'update.
		 */
		C3D_API void update( CpuUpdater & updater );
		/**
		 *\~english
		 *\brief		Records the light data into given buffer.
		 *\param[in]	index	The light index in the buffer.
		 *\param[in]	offset	The light data offset in the buffer.
		 *\param[out]	data	Receives the informations.
		 *\~french
		 *\brief		Enregistre les données de la source lumineuse dans le tampon donné.
		 *\param[in]	index	L'index de la source lumineuse dans le buffer.
		 *\param[in]	offset	L'offset des données de la source lumineuse dans le buffer.
		 *\param[out]	data	Reçoit les informations.
		 */
		C3D_API void fillBuffer( uint32_t index
			, VkDeviceSize offset
			, castor::Point4f * data );
		/**
		*\~english
		*name
		*	Getters.
		*\~french
		*name
		*	Accesseurs.
		*/
		/**@{*/
		C3D_API DirectionalLightRPtr getDirectionalLight()const;
		C3D_API PointLightRPtr getPointLight()const;
		C3D_API SpotLightRPtr getSpotLight()const;

		LightType getLightType()const
		{
			return m_category->getLightType();
		}

		bool isEnabled()const
		{
			return m_enabled;
		}

		castor::Point3f const & getColour()const
		{
			return m_category->getColour();
		}

		castor::Point2f const & getIntensity()const
		{
			return m_category->getIntensity();
		}

		float getFarPlane()const
		{
			return m_category->getFarPlane();
		}

		float getDiffuseIntensity()const
		{
			return m_category->getDiffuseIntensity();
		}

		float getSpecularIntensity()const
		{
			return m_category->getSpecularIntensity();
		}

		castor::BoundingBox const & getBoundingBox()const
		{
			return m_category->getBoundingBox();
		}

		LightCategoryRPtr getCategory()const
		{
			return m_category.get();
		}

		bool isShadowProducer()const
		{
			return m_currentShadowCaster;
		}

		bool isExpectedShadowProducer()const
		{
			return m_shadowCaster;
		}

		ShadowType getShadowType()const
		{
			return m_shadows.filterType;
		}

		ShadowMapRPtr getShadowMap()const
		{
			return m_shadowMap;
		}

		uint32_t getShadowMapIndex()const
		{
			return m_shadowMapIndex;
		}

		uint32_t getBufferIndex()const
		{
			return m_bufferIndex;
		}

		VkDeviceSize getBufferOffset()const
		{
			return m_bufferOffset;
		}

		bool needsRsmShadowMaps()const
		{
			return getGlobalIlluminationType() != GlobalIlluminationType::eNone
				&& getGlobalIlluminationType() != GlobalIlluminationType::eVoxelConeTracing;
		}

		GlobalIlluminationType getGlobalIlluminationType()const
		{
			return m_currentGlobalIllumination;
		}

		GlobalIlluminationType getExpectedGlobalIlluminationType()const
		{
			return m_shadows.globalIllumination;
		}

		RsmConfig const & getRsmConfig()const
		{
			return m_shadows.rsmConfig;
		}

		RsmConfig & getRsmConfig()
		{
			return m_shadows.rsmConfig;
		}

		LpvConfig const & getLpvConfig()const
		{
			return m_shadows.lpvConfig;
		}

		LpvConfig & getLpvConfig()
		{
			return m_shadows.lpvConfig;
		}

		uint32_t getVolumetricSteps()const
		{
			return m_shadows.volumetricSteps;
		}

		float getVolumetricScatteringFactor()const
		{
			return m_shadows.volumetricScattering;
		}

		castor::Point2f const & getShadowRawOffsets()const
		{
			return m_shadows.rawOffsets;
		}

		castor::Point2f const & getShadowPcfOffsets()const
		{
			return m_shadows.pcfOffsets;
		}

		float getVsmMinVariance()const
		{
			return m_shadows.vsmMinVariance;
		}

		float getVsmLightBleedingReduction()const
		{
			return m_shadows.vsmLightBleedingReduction;
		}

		castor::RangedValue< uint32_t > getShadowPcfFilterSize()const
		{
			return m_shadows.pcfFilterSize;
		}

		castor::RangedValue< uint32_t > getShadowPcfSampleCount()const
		{
			return m_shadows.pcfSampleCount;
		}

		ShadowConfig const & getShadowConfig()const
		{
			return m_shadows;
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
		void setColour( float * values )
		{
			m_category->setColour( castor::Point3f( values[0], values[1], values[2] ) );
		}

		void setColour( float r, float g, float b )
		{
			m_category->setColour( castor::Point3f( r, g, b ) );
		}

		void setColour( castor::Point3f const & value )
		{
			m_category->setColour( value );
		}

		void setColour( castor::RgbColour const & value )
		{
			m_category->setColour( toRGBFloat( value ) );
		}

		void setIntensity( float * values )
		{
			m_category->setIntensity( castor::Point2f( values[0], values[1] ) );
		}

		void setIntensity( float d, float s )
		{
			m_category->setIntensity( castor::Point2f( d, s ) );
		}

		void setIntensity( castor::Point2f const & value )
		{
			m_category->setIntensity( value );
		}

		void setDiffuseIntensity( float value )
		{
			m_category->setDiffuseIntensity( value );
		}

		void setSpecularIntensity( float value )
		{
			m_category->setSpecularIntensity( value );
		}

		void setEnabled( bool value )
		{
			m_enabled = value;
		}

		void enable()
		{
			setEnabled( true );
		}

		void disable()
		{
			setEnabled( false );
		}

		void setShadowProducer( bool value )
		{
			m_shadowCaster = value;
			markDirty();
		}

		void setShadowMapIndex( uint32_t index )
		{
			if ( m_shadowMapIndex != index )
			{
				m_shadowMapIndex = index;
				onGPUChanged( *this );
			}
		}

		void setShadowMap( ShadowMapRPtr value, uint32_t index = 0u )
		{
			if ( m_shadowMap != value
				|| m_shadowMapIndex != index )
			{
				m_shadowMap = value;
				m_shadowMapIndex = index;
				onGPUChanged( *this );
			}
		}

		void setGlobalIlluminationType( GlobalIlluminationType value )
		{
			m_shadows.globalIllumination = value;
			markDirty();
		}

		void setShadowType( ShadowType value )
		{
			m_shadows.filterType = value;
		}

		void setVolumetricSteps( uint32_t value )
		{
			m_shadows.volumetricSteps = value;
		}

		void setVolumetricScatteringFactor( float value )
		{
			m_shadows.volumetricScattering = value;
		}

		void setRawMinOffset( float value )
		{
			m_shadows.rawOffsets[0] = value;
		}

		void setRawMaxSlopeOffset( float value )
		{
			m_shadows.rawOffsets[1] = value;
		}

		void setPcfMinOffset( float value )
		{
			m_shadows.pcfOffsets[0] = value;
		}

		void setPcfMaxSlopeOffset( float value )
		{
			m_shadows.pcfOffsets[1] = value;
		}

		void setPcfFilterSize( uint32_t value )
		{
			m_shadows.pcfFilterSize = value;
		}

		void setPcfSampleCount( uint32_t value )
		{
			m_shadows.pcfSampleCount = value;
		}

		void setVsmMinVariance( float value )
		{
			m_shadows.vsmMinVariance = value;
		}

		void setVsmLightBleedingReduction( float value )
		{
			m_shadows.vsmLightBleedingReduction = value;
		}
		/**@}*/

	public:
		OnLightChanged onGPUChanged;

	protected:
		bool m_enabled{};
		bool m_shadowCaster{};
		std::atomic_bool m_currentShadowCaster{};
		bool m_dirty{ true };
		ShadowConfig m_shadows;
		LightCategoryUPtr m_category;
		ShadowMapRPtr m_shadowMap{};
		uint32_t m_shadowMapIndex{};
		std::atomic< GlobalIlluminationType > m_currentGlobalIllumination{};
		uint32_t m_bufferIndex{ InvalidIndex };
		VkDeviceSize m_bufferOffset{};
	};
}

#endif
