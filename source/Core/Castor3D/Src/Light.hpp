/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_LIGHT_H___
#define ___C3D_LIGHT_H___

#include "MovableObject.hpp"
#include "LightCategory.hpp"

#include <OwnedBy.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Light source implementation
	\~french
	\brief		Implémentation d'une source lumineuse
	\remark
	*/
	class Light
		: public MovableObject
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_scene			Parent scene
		 *\param[in]	p_node			The parent scene node
		 *\param[in]	p_pFactory		Factory used to create the LightCategory
		 *\param[in]	p_eLightType	The light type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_scene			La scène parente
		 *\param[in]	p_node			Le scene node parent
		 *\param[in]	p_pFactory		La fabrique de LightCategory
		 *\param[in]	p_eLightType	Le type de lumière
		 */
		C3D_API Light( LightFactory & p_factory, SceneSPtr p_scene, eLIGHT_TYPE p_eLightType, SceneNodeSPtr p_node, Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_scene			Parent scene
		 *\param[in]	p_pFactory		Factory used to create the LightCategory
		 *\param[in]	p_eLightType	The light type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_scene			La scène parente
		 *\param[in]	p_pFactory		La fabrique de LightCategory
		 *\param[in]	p_eLightType	Le type de lumière
		 */
		C3D_API Light( LightFactory & p_factory, SceneSPtr p_scene, eLIGHT_TYPE p_eLightType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~Light();
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 */
		C3D_API void Bind( Castor::PxBufferBase & p_texture, uint32_t p_index );
		/**
		 *\~english
		 *\brief		Attaches this light to a Material
		 *\param[in]	p_node	The new light's parent node
		 *\~french
		 *\brief		Attache cette lumière au node donné
		 *\param[in]	p_node	Le nouveau node parent de cette lumière
		 */
		C3D_API virtual void AttachTo( SceneNodeSPtr p_node );
		/**
		 *\~english
		 *\brief		Retrieves the DirectionalLight category
		 *\return		The value
		 *\~french
		 *\brief		Récupère la DirectionalLight
		 *\return		La valeur
		 */
		C3D_API DirectionalLightSPtr GetDirectionalLight()const;
		/**
		 *\~english
		 *\brief		Retrieves the PointLight category
		 *\return		The value
		 *\~french
		 *\brief		Récupère la PointLight
		 *\return		La valeur
		 */
		C3D_API PointLightSPtr GetPointLight()const;
		/**
		 *\~english
		 *\brief		Retrieves the SpotLight category
		 *\return		The value
		 *\~french
		 *\brief		Récupère la SpotLight
		 *\return		La valeur
		 */
		C3D_API SpotLightSPtr GetSpotLight()const;
		/**
		 *\~english
		 *\brief		Retrieves the light type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type de lumière
		 *\return		La valeur
		 */
		inline eLIGHT_TYPE GetLightType()const
		{
			return m_pCategory->GetLightType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the light enabled status
		 *\return		The value
		 *\~french
		 *\brief		Récupère le statut d'activation de la lumière
		 *\return		La valeur
		 */
		inline bool IsEnabled()const
		{
			return m_enabled;
		}
		/**
		 *\~english
		 *\return		The light colour.
		 *\~french
		 *\return		La couleur de la lumière.
		 */
		inline Castor::Point3f const & GetColour()const
		{
			return m_pCategory->GetColour();
		}
		/**
		 *\~english
		 *\return		The intensity values.
		 *\~french
		 *\return		Les valeurs d'intensité.
		 */
		inline Castor::Point3f const & GetIntensity()const
		{
			return m_pCategory->GetIntensity();
		}
		/**
		 *\~english
		 *\return		The ambient intensity.
		 *\~french
		 *\return		L'intensité ambiante.
		 */
		inline float GetAmbientIntensity()const
		{
			return m_pCategory->GetAmbientIntensity();
		}
		/**
		 *\~english
		 *\return		The diffuse intensity.
		 *\~french
		 *\return		L'intensité diffuse.
		 */
		inline float GetDiffuseIntensity()const
		{
			return m_pCategory->GetDiffuseIntensity();
		}
		/**
		 *\~english
		 *\return		The specular intensity.
		 *\~french
		 *\return		L'intensité spéculaire.
		 */
		inline float GetSpecularIntensity()const
		{
			return m_pCategory->GetSpecularIntensity();
		}
		/**
		 *\~english
		 *\brief		Retrieves the light position and type
		 *\return		The value
		 *\~french
		 *\brief		Récupère le type et la position de la lumière
		 *\return		La valeur
		 */
		inline Castor::Point4f const & GetPositionType()const
		{
			return m_pCategory->GetPositionType();
		}
		/**
		 *\~english
		 *\brief		Retrieves the light index
		 *\return		The value
		 *\~french
		 *\brief		Récupère l(indice de la lumière
		 *\return		La valeur
		 */
		inline int GetIndex()const
		{
			return m_iIndex;
		}
		/**
		 *\~english
		 *\brief		Sets the light index
		 *\param[in]	p_value	The new value
		 *\~french
		 *\brief		Définit l'indice de la lumière
		 *\param[in]	p_value	La nouvelle valeur
		 */
		inline void SetIndex( int p_value )
		{
			m_iIndex = p_value;
		}
		/**
		 *\~english
		 *\brief		Sets the colour.
		 *\param[in]	p_values	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	p_values	La nouvelle valeur.
		 */
		inline void SetColour( float * p_values )
		{
			m_pCategory->SetColour( Castor::Point3f( p_values[0], p_values[1], p_values[2] ) );
		}
		/**
		 *\~english
		 *\brief		Sets the colour.
		 *\param[in]	r, g, b	The RGB components.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	r, g, b	Les composantes RGB.
		 */
		inline void SetColour( float r, float g, float b )
		{
			m_pCategory->SetColour( Castor::Point3f( r, g, b ) );
		}
		/**
		 *\~english
		 *\brief		Sets the colour.
		 *\param[in]	p_colour	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	p_colour	La nouvelle valeur.
		 */
		inline void SetColour( Castor::Point3f const & p_colour )
		{
			m_pCategory->SetColour( p_colour );
		}
		/**
		 *\~english
		 *\brief		Sets the colour.
		 *\param[in]	p_colour	The new value
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	p_colour	La nouvelle valeur
		 */
		inline void SetColour( Castor::Colour const & p_colour )
		{
			p_colour.to_rgb( m_pCategory->GetColour() );
		}
		/**
		 *\~english
		 *\brief		Sets the light's intensity values.
		 *\param[in]	p_values	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	p_values	Les nouvelles valeurs.
		 */
		inline void SetIntensity( float * p_values )
		{
			m_pCategory->SetIntensity( Castor::Point3f( p_values[0], p_values[1], p_values[2] ) );
		}
		/**
		 *\~english
		 *\brief		Sets the light's intensity values.
		 *\param[in]	a, d, s	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	a, d, s	Les nouvelles valeurs.
		 */
		inline void SetIntensity( float a, float d, float s )
		{
			m_pCategory->SetIntensity( Castor::Point3f( a, d, s ) );
		}
		/**
		 *\~english
		 *\brief		Sets the light's intensity values.
		 *\param[in]	p_intensity	The new values.
		 *\~french
		 *\brief		Définit les valeurs d'intensité.
		 *\param[in]	p_intensity	Les nouvelles valeurs.
		 */
		inline void SetIntensity( Castor::Point3f const & p_intensity )
		{
			m_pCategory->SetIntensity( p_intensity );
		}
		/**
		 *\~english
		 *\brief		Sets the ambient intensity.
		 *\param[in]	p_intensity	The new value.
		 *\~french
		 *\brief		Définit l'intensité ambiante.
		 *\param[in]	p_intensity	La nouvelle valeur.
		 */
		inline void SetAmbientIntensity( float p_intensity )
		{
			m_pCategory->SetAmbientIntensity( p_intensity );
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse intensity.
		 *\param[in]	p_intensity	The new value.
		 *\~french
		 *\brief		Définit l'intensité diffuse.
		 *\param[in]	p_intensity	La nouvelle valeur.
		 */
		inline void SetDiffuseIntensity( float p_intensity )
		{
			m_pCategory->SetDiffuseIntensity( p_intensity );
		}
		/**
		 *\~english
		 *\brief		Sets the specular intensity.
		 *\param[in]	p_intensity	The new value.
		 *\~french
		 *\brief		Définit l'intensité spéculaire.
		 *\param[in]	p_intensity	La nouvelle valeur.
		 */
		inline void SetSpecularIntensity( float p_intensity )
		{
			m_pCategory->SetSpecularIntensity( p_intensity );
		}
		/**
		 *\~english
		 *\brief		Sets the light enabled status
		 *\param[in]	p_enabled	The new value
		 *\~french
		 *\brief		Définit le statut d'activation de la lumère
		 *\param[in]	p_enabled	La nouvelle valeur
		 */
		inline void SetEnabled( bool p_enabled )
		{
			m_enabled = p_enabled;
		}
		/**
		 *\~english
		 *\brief		Enables the light
		 *\~french
		 *\brief		Active la lumière
		 */
		inline void Enable()
		{
			SetEnabled( true );
		}
		/**
		 *\~english
		 *\brief		Disables the light
		 *\~french
		 *\brief		Désactive la lumière
		 */
		inline void Disable()
		{
			SetEnabled( false );
		}

	protected:
		void OnNodeChanged();

	protected:
		//!\~english Tells the light is enabled	\~french Dit si la lumière est active ou pas
		bool m_enabled;
		//!\~english The Light category that effectively holds light data	\~french la light category contenant les données de la lumière
		LightCategorySPtr m_pCategory;
		//!\~english The light index	\~french L'indice de la lumière
		int m_iIndex;
	};
}

#endif
