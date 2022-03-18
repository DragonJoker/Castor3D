/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_CATEGORY_H___
#define ___C3D_LIGHT_CATEGORY_H___

#include "LightModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Shader/ShaderBuffers/LightBuffer.hpp"

#include <CastorUtils/Data/TextWriter.hpp>
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
		 *\param[in]	lightType	The light category type.
		 *\param[in]	light		The parent Light.
		 *\~french
		 *\brief		Constructeur.
		 *\param[in]	lightType	Le type de catégorie de lumière.
		 *\param[in]	light		La Light parente.
		 */
		C3D_API explicit LightCategory( LightType lightType, Light & light );

	public:
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~LightCategory() = default;
		/**
		 *\~english
		 *\brief		Updates the light.
		 *\~french
		 *\brief		Met la source à jour.
		 */
		C3D_API virtual void update() = 0;
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\param[out]	data	Receives the light's data.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 *\param[out]	data	Reçoit les données de la source lumineuse.
		 */
		C3D_API void fillBuffer( LightBuffer::LightData & data )const;
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
		C3D_API castor::Point2f const & getShadowVariance()const;
		C3D_API ShadowConfig const & getShadowConfig()const;
		C3D_API RsmConfig const & getRsmConfig()const;
		C3D_API LpvConfig const & getLpvConfig()const;

		LightType getLightType()const
		{
			return m_lightType;
		}

		float getDiffuseIntensity()const
		{
			return m_intensity[0];
		}

		float getSpecularIntensity()const
		{
			return m_intensity[1];
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
		C3D_API void setVsmMaxVariance( float value );
		C3D_API void setVsmVarianceBias( float value );
		C3D_API void setColour( castor::Point3f const & value );
		C3D_API void setIntensity( castor::Point2f const & value );
		C3D_API void setDiffuseIntensity( float value );
		C3D_API void setSpecularIntensity( float value );

		Light & getLight()
		{
			return m_light;
		}

		castor::Point3f & getColour()
		{
			return m_colour;
		}

		castor::Point2f & getIntensity()
		{
			return m_intensity;
		}
		/**@}*/

	protected:
		/**
		 *\~english
		 *\brief		Updates the light source's informations related to the scene node.
		 *\param[in]	node	The scene node.
		 *\~french
		 *\brief		Met à jour les informations de la source lumineuse, relatives au noeud de scène.
		 *\param[in]	node	Le noeud de scène.
		 */
		C3D_API virtual void updateNode( SceneNode const & node ) = 0;

	private:
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\param[out]	data	Receives the light's data.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 *\param[out]	data	Reçoit les données de la source lumineuse.
		 */
		C3D_API virtual void doFillBuffer( LightBuffer::LightData & data )const = 0;

	protected:
		//!\~english	The cube box for the light volume of effect.
		//!\~french		La cube box pour le volume d'effet de la lumière.
		castor::BoundingBox m_cubeBox;
		//!\~english	The far plane's depth.
		//!\~french		La profondeur du plan éloigné.
		float m_farPlane{ 1.0f };
		//!\~english	The light source shadow map index.
		//!\~french		L'index de la shadow map de la source lumineuse.
		int32_t m_shadowMapIndex{ -1 };

	private:
		LightType m_lightType;
		Light & m_light;
		castor::Point3f m_colour{ 1.0, 1.0, 1.0 };
		castor::Point2f m_intensity{ 1.0, 1.0 };
	};
}

#endif
