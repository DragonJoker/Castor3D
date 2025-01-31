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
#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
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
		 *\param[in]	createInfo	The creation informations.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	name		Le nom de la lumière.
		 *\param[in]	createInfo	Les informations de création.
		 */
		C3D_API Light( castor::String const & name
			, LightCreateInfo const & createInfo );
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
		 *\brief		Attaches the movable object to a node
		 *\~french
		 *\brief		Attache l'object à un noeud
		 */
		C3D_API void attachTo( SceneNode & node )override;
		/**
		 *\~english
		 *\brief			Updates the shadow informations.
		 *\param[in,out]	lightCamera	The camera that receives the light spot data.
		 *\param[in]		index		The shadow map index.
		 *\~french
		 *\brief			Met à jour les information d'ombre.
		 *\param[in,out]	lightCamera	La caméra qui reçoit les données de spot de la lumière.
		 *\param[in]		index		L'indice de la shadow map.
		 */
		C3D_API bool updateShadow( Camera const & viewCamera
			, Camera * lightCamera
			, int32_t index );
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
		C3D_API void fillLightBuffer( uint32_t index
			, VkDeviceSize offset
			, castor::Point4f * data );
		/**
		 *\~english
		 *\brief		Records the light data into given buffer.
		 *\param[out]	data	Receives the informations.
		 *\~french
		 *\brief		Enregistre les données de la source lumineuse dans le tampon donné.
		 *\param[out]	data	Reçoit les informations.
		 */
		C3D_API void fillShadowBuffer( AllShadowData & data )const;
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
		 *\brief			Clones this object into the given one.
		 *\param[in,out]	output	Receives this object's data.
		 *\~french
		 *\brief			Clone cet objet dans celui donné.
		 *\param[in,out]	output	Reçoit les données de cet objet.
		 */
		C3D_API void cloneInto( Light & output )const;

		C3D_API static void addParsers( castor::AttributeParsers & result );
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

		LightInstanceRPtr getInstance()const
		{
			return m_instance.get();
		}

		bool isEnabled()const
		{
			return m_enabled;
		}

		LightCategoryRPtr getCategory()const
		{
			return m_category.get();
		}

		LightType getLightType()const
		{
			return m_category->getLightType();
		}

		castor::Point3f const & getColour()const
		{
			return m_category->getColour();
		}

		float getFarPlane()const
		{
			return m_category->getFarPlane();
		}

		castor::BoundingBox const & getBoundingBox()const
		{
			return m_category->getBoundingBox();
		}

		bool isShadowProducer()const
		{
			return m_category->isShadowProducer();
		}

		bool isExpectedShadowProducer()const
		{
			return m_category->isExpectedShadowProducer();
		}

		ShadowType getShadowType()const
		{
			return m_category->getShadowType();
		}

		bool needsRsmShadowMaps()const
		{
			return m_category->needsRsmShadowMaps();
		}

		GlobalIlluminationType getGlobalIlluminationType()const
		{
			return m_category->getGlobalIlluminationType();
		}

		GlobalIlluminationType getExpectedGlobalIlluminationType()const
		{
			return m_category->getExpectedGlobalIlluminationType();
		}

		RsmConfig const & getRsmConfig()const
		{
			return m_category->getRsmConfig();
		}

		LpvConfig const & getLpvConfig()const
		{
			return m_category->getLpvConfig();
		}

		LpvConfig & getLpvConfig()
		{
			return m_category->getLpvConfig();
		}

		uint32_t getVolumetricSteps()const
		{
			return m_category->getVolumetricSteps();
		}

		float getVolumetricScatteringFactor()const
		{
			return m_category->getVolumetricScatteringFactor();
		}

		castor::Point2f const & getShadowRawOffsets()const
		{
			return m_category->getShadowRawOffsets();
		}

		castor::Point2f const & getShadowPcfOffsets()const
		{
			return m_category->getShadowPcfOffsets();
		}

		float getVsmMinVariance()const
		{
			return m_category->getVsmMinVariance();
		}

		float getVsmLightBleedingReduction()const
		{
			return m_category->getVsmLightBleedingReduction();
		}

		castor::RangedValue< uint32_t > getShadowPcfFilterSize()const
		{
			return m_category->getShadowPcfFilterSize();
		}

		castor::RangedValue< uint32_t > getShadowPcfSampleCount()const
		{
			return m_category->getShadowPcfSampleCount();
		}

		ShadowConfig const & getShadowConfig()const
		{
			return m_category->getShadowConfig();
		}

		ShadowConfig & getShadowConfig()
		{
			return m_category->getShadowConfig();
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
		void setColour( float const * values )
		{
			m_category->setColour( castor::Point3f( values[0], values[1], values[2] ) );
			markDirty();
		}

		void setColour( float r, float g, float b )
		{
			m_category->setColour( castor::Point3f( r, g, b ) );
			markDirty();
		}

		void setColour( castor::Point3f const & value )
		{
			m_category->setColour( value );
			markDirty();
		}

		void setGlobalIlluminationType( GlobalIlluminationType value )
		{
			m_category->setGlobalIlluminationType( value );
			markDirty();
		}

		void setShadowConfig( ShadowConfig config )
		{
			m_category->setShadowConfig( castor::move( config ) );
			markDirty();
		}

		void setColour( castor::RgbColour const & value )
		{
			m_category->setColour( toRGBFloat( value ) );
		}

		void setVolumetricSteps( uint32_t value )
		{
			m_category->setVolumetricSteps( value );
		}

		void setVolumetricScatteringFactor( float value )
		{
			m_category->setVolumetricScatteringFactor( value );
		}

		void setRawMinOffset( float value )
		{
			m_category->setRawMinOffset( value );
		}

		void setRawMaxSlopeOffset( float value )
		{
			m_category->setRawMaxSlopeOffset( value );
		}

		void setPcfMinOffset( float value )
		{
			m_category->setPcfMinOffset( value );
		}

		void setPcfMaxSlopeOffset( float value )
		{
			m_category->setPcfMaxSlopeOffset( value );
		}

		void setPcfFilterSize( uint32_t value )
		{
			m_category->setPcfFilterSize( value );
		}

		void setPcfSampleCount( uint32_t value )
		{
			m_category->setPcfSampleCount( value );
		}

		void setVsmMinVariance( float value )
		{
			m_category->setVsmMinVariance( value );
		}

		void setVsmLightBleedingReduction( float value )
		{
			m_category->setVsmLightBleedingReduction( value );
		}

		void setShadowType( ShadowType value )
		{
			m_category->setShadowType( value );
		}

		void setEnabled( bool value )
		{
			m_enabled = value;
			markDirty();
		}

		void enable()
		{
			setEnabled( true );
		}

		void disable()
		{
			setEnabled( false );
		}
		/**@}*/

	public:
		OnLightChanged onGPUChanged;

	private:
		friend class LightCategory;
		friend class LightInstance;

		bool & doGetDirty()
		{
			return m_dirty;
		}

		castor::GroupChangeTracked< bool > m_enabled;
		LightCategoryUPtr m_category;
		LightInstanceUPtr m_instance;
	};

	struct LightContext
		: public MovableContext
	{
		LightUPtr ownLight{};
		LightRPtr light{};
		LightType lightType{ LightType::eCount };
		ShadowConfigUPtr shadowConfig;
	};
}

namespace castor
{
	template<>
	struct ParserEnumTraits< castor3d::LightType >
	{
		static inline xchar const * const Name = cuT( "LightType" );
		static inline UInt32StrMap const Values = []()
			{
				UInt32StrMap result;
				result = castor3d::getEnumMapT< castor3d::LightType >();
				return result;
			}( );
	};
}

#endif
