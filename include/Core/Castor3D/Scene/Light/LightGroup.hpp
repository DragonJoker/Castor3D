/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LightGroup_H___
#define ___C3D_LightGroup_H___

#include "LightModule.hpp"
#include "Castor3D/Render/ShadowMap/ShadowMapModule.hpp"
#include "Castor3D/Render/GlobalIllumination/GlobalIlluminationModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include "Castor3D/Scene/Light/LightCategory.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
#include <CastorUtils/FileParser/FileParserModule.hpp>
#include <CastorUtils/Graphics/RgbColour.hpp>

#include <CastorUtils/Config/BeginExternHeaderGuard.hpp>
#include <atomic>
#include <CastorUtils/Config/EndExternHeaderGuard.hpp>

namespace c3d
{
	class LightGroup
		: public OwnedBy< Scene >
		, public Named
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
		C3D_API LightGroup( String const & name
			, LightGroupCreateInfo const & createInfo );
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
		C3D_API LightGroup( String const & name
			, Scene & scene
			, LightFactory const & factory
			, LightType lightType );
		/**
		 *\~english
		 *\brief		Creates an instance for given scene node.
		 *\~french
		 *\brief		Crée une instance pour le noeud de scène donné.
		 */
		C3D_API void addInstance( SceneNode & node );
		/**
		 *\~english
		 *\brief		Removes the instance for given scene node.
		 *\~french
		 *\brief		Supprime l'instance pour le noeud de scène donné.
		 */
		C3D_API void removeInstance( SceneNode const & node );
		/**
		 *\~english
		 *\brief		Adds the object to dirty object list in the scene.
		 *\~french
		 *\brief		Ajout l'objet à la liste des objets à mettre à jour de la scène.
		 */
		C3D_API void markDirty();
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
		C3D_API void cloneInto( LightGroup & output )const;

		C3D_API static void addParsers( AttributeParsers & result );
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

		Point3f const & getColour()const
		{
			return m_category->getColour();
		}

		float getFarPlane()const
		{
			return m_category->getFarPlane();
		}

		BoundingBox const & getBoundingBox()const
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

		Point2f const & getShadowRawOffsets()const
		{
			return m_category->getShadowRawOffsets();
		}

		Point2f const & getShadowPcfOffsets()const
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

		RangedValue< uint32_t > getShadowPcfFilterSize()const
		{
			return m_category->getShadowPcfFilterSize();
		}

		RangedValue< uint32_t > getShadowPcfSampleCount()const
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
			m_category->setColour( Point3f( values[0], values[1], values[2] ) );
			markDirty();
		}

		void setColour( float r, float g, float b )
		{
			m_category->setColour( Point3f( r, g, b ) );
			markDirty();
		}

		void setColour( Point3f const & value )
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
			m_category->setShadowConfig( c3d::move( config ) );
			markDirty();
		}

		void setColour( RgbColour const & value )
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
		/**
		*\~english
		*name
		*	Iteration on instances.
		*\~french
		*name
		*	Itération sur les instances.
		*/
		/**@{*/
		auto begin()const
		{
			return m_instances.begin();
		}

		auto end()const
		{
			return m_instances.end();
		}
		/**@}*/

	private:
		friend class LightCategory;

		bool & doGetDirty()
		{
			return m_dirty;
		}

		bool m_dirty{ true };
		GroupChangeTracked< bool > m_enabled;
		LightCategoryUPtr m_category;
		Vector< LightInstanceUPtr > m_instances;
	};

	struct SceneContext;

	struct LightGroupContext
	{
		SceneContext * scene{};
		String name{};
		LightGroupUPtr ownLight{};
		LightGroupRPtr light{};
		LightType lightType{ LightType::eCount };
		ShadowConfigUPtr shadowConfig;
	};

	C3D_API String getPrefix( LightGroupContext const & context );
}

#endif
