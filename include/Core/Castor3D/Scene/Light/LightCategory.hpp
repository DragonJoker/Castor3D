/*
See LICENSE file in root folder
*/
#ifndef ___C3D_LIGHT_CATEGORY_H___
#define ___C3D_LIGHT_CATEGORY_H___

#include "LightModule.hpp"
#include "Castor3D/Render/GlobalIllumination/LightPropagationVolumes/LightPropagationVolumesModule.hpp"
#include "Castor3D/Render/Technique/Opaque/ReflectiveShadowMapGI/ReflectiveShadowMapGIModule.hpp"

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
		C3D_API virtual ~LightCategory();
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
		 *\param[out]	buffer	The buffer that receives the light's data.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 *\param[out]	buffer	Le tampon recevant les données de la source lumineuse.
		 */
		C3D_API void bind( castor::Point4f * buffer )const;
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

		void setColour( castor::Point3f const & value )
		{
			m_colour = value;
		}

		void setIntensity( castor::Point2f const & value )
		{
			m_intensity = value;
		}

		void setDiffuseIntensity( float value )
		{
			m_intensity[0] = value;
		}

		void setSpecularIntensity( float value )
		{
			m_intensity[1] = value;
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

	protected:
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point2f const & components
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components1, components2	The components.
		 *\param[out]	buffer						The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components1, components2	Les composantes.
		 *\param[in]	buffer						Le tampon de destination.
		 */
		void doCopyComponent( castor::Point2f const & components1
			, castor::Point2f const & components2
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	component	The last component.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	component	La dernière composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point2f const & components
			, float component
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	component1	The third component.
		 *\param[in]	component2	The fourth component.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	component1	La troisième composante.
		 *\param[in]	component2	La quatrième composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point2f const & components
			, float component1
			, float component2
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	component0, component1, component2, component3	The components.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	component0, component1, component2, component3	Les composantes.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( float component0
			, float component1
			, float component2
			, float component3
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point3f const & components
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[in]	component	The last component.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	Les composantes.
		 *\param[in]	component	La dernière composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point3f const & components
			, float component
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::Point4f const & components
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::ConstCoords4f const & components
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::Coords4f const & components
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( castor::Matrix4x4f const & components
			, castor::Point4f *& buffer )const;
		/**
		 *\~english
		 *\brief		Copies the given light PixelComponents values into the buffer.
		 *\param[in]	components	The components.
		 *\param[out]	buffer		The destination buffer.
		 *\~french
		 *\brief		Copie les valeurs de la composante de lumière donnée dans le tampon.
		 *\param[in]	components	La composante.
		 *\param[in]	buffer		Le tampon de destination.
		 */
		void doCopyComponent( int32_t const & components
			, castor::Point4f *& buffer )const;

	private:
		/**
		 *\~english
		 *\brief		Puts the light source's type specific data into the given texture.
		 *\param[out]	buffer	The buffer that receives the light's data.
		 *\~french
		 *\brief		Met les informations spécifiques au type de source lumineuse dans la texture donnée.
		 *\param[out]	buffer	Le tampopn recevant les données de la source lumineuse.
		 */
		C3D_API virtual void doBind( castor::Point4f * buffer )const = 0;

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
