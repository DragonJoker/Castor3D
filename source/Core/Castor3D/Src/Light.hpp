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
		, public Castor::OwnedBy< Engine >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_scene		Parent scene
		 *\param[in]	p_node			The parent scene node
		 *\param[in]	p_pFactory		Factory used to create the LightCategory
		 *\param[in]	p_eLightType	The light type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_scene		La scène parente
		 *\param[in]	p_node			Le scene node parent
		 *\param[in]	p_pFactory		La fabrique de LightCategory
		 *\param[in]	p_eLightType	Le type de lumière
		 */
		C3D_API Light( LightFactory & p_factory, SceneSPtr p_scene, eLIGHT_TYPE p_eLightType, SceneNodeSPtr p_node, Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_scene		Parent scene
		 *\param[in]	p_pFactory		Factory used to create the LightCategory
		 *\param[in]	p_eLightType	The light type
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_scene		La scène parente
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
		 *\brief		Renders the light
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Rend la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		C3D_API void Render();
		/**
		 *\~english
		 *\brief		Ends light rendering
		 *\remark		Fixed pipeline implementation
		 *\~french
		 *\brief		Fin du rendu de la lumière
		 *\remark		Implémentation passant par le pipeline fixe
		 */
		C3D_API void EndRender();
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
		 *\brief		Retrieves the ambient colour (4 components: R, G, B and Intensity)
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante (4 composantes: R, V, B et Intensité)
		 *\return		La valeur
		 */
		inline Castor::Point4f const & GetAmbient()const
		{
			return m_pCategory->GetAmbient();
		}
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour (4 components: R, G, B and Intensity)
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse (4 composantes: R, V, B et Intensité)
		 *\return		La valeur
		 */
		inline Castor::Point4f const & GetDiffuse()const
		{
			return m_pCategory->GetDiffuse();
		}
		/**
		 *\~english
		 *\brief		Retrieves the specular colour (4 components: R, G, B and Intensity)
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire (4 composantes: R, V, B et Intensité)
		 *\return		La valeur
		 */
		inline Castor::Point4f const & GetSpecular()const
		{
			return m_pCategory->GetSpecular();
		}
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour (4 components: R, G, B and Intensity)
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante (4 composantes: R, V, B et Intensité)
		 *\return		La valeur
		 */
		inline Castor::Point4f & GetAmbient()
		{
			return m_pCategory->GetAmbient();
		}
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour (4 components: R, G, B and Intensity)
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse (4 composantes: R, V, B et Intensité)
		 *\return		La valeur
		 */
		inline Castor::Point4f & GetDiffuse()
		{
			return m_pCategory->GetDiffuse();
		}
		/**
		 *\~english
		 *\brief		Retrieves the specular colour (4 components: R, G, B and Intensity)
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire (4 composantes: R, V, B et Intensité)
		 *\return		La valeur
		 */
		inline Castor::Point4f & GetSpecular()
		{
			return m_pCategory->GetSpecular();
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
		 *\param[in]	val	The new value
		 *\~french
		 *\brief		Définit l'indice de la lumière
		 *\param[in]	val	La nouvelle valeur
		 */
		inline void SetIndex( int val )
		{
			m_iIndex = val;
		}
		/**
		 *\~english
		 *\brief		Sets the ambient colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_ambient	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_ambient	La nouvelle valeur
		 */
		inline void SetAmbient( float * p_ambient )
		{
			m_pCategory->SetAmbient( Castor::Point4f( p_ambient[0], p_ambient[1], p_ambient[2], p_ambient[3] ) );
		}
		/**
		 *\~english
		 *\brief		Sets the ambient light
		 *\param[in]	r, g, b	The RGB components
		 *\param[in]	i		The intensity component
		 *\~french
		 *\brief		Définit la lumière ambiante
		 *\param[in]	r, g, b	Les composantes RGB
		 *\param[in]	i		La composante intensité
		 */
		inline void SetAmbient( float r, float g, float b, float i )
		{
			m_pCategory->SetAmbient( Castor::Point4f( r, g, b, i ) );
		}
		/**
		 *\~english
		 *\brief		Sets the ambient colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_ambient	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_ambient	La nouvelle valeur
		 */
		inline void SetAmbient( Castor::Point4f const & p_ambient )
		{
			m_pCategory->SetAmbient( p_ambient );
		}
		/**
		 *\~english
		 *\brief		Sets the ambient colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_ambient	The new value
		 *\~french
		 *\brief		Définit la couleur ambiante (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_ambient	La nouvelle valeur
		 */
		inline void SetAmbient( Castor::Colour const & p_ambient )
		{
			p_ambient.to_rgba( m_pCategory->GetAmbient() );
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_diffuse	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_diffuse	La nouvelle valeur
		 */
		inline void SetDiffuse( float * p_diffuse )
		{
			m_pCategory->SetDiffuse( Castor::Point4f( p_diffuse[0], p_diffuse[1], p_diffuse[2], p_diffuse[3] ) );
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour
		 *\param[in]	r, g, b	The RGB components
		 *\param[in]	i		The intensity component
		 *\~french
		 *\brief		Définit la couleur diffuse
		 *\param[in]	r, g, b	Les composantes RGB
		 *\param[in]	i		La composante intensité
		 */
		inline void SetDiffuse( float r, float g, float b, float i )
		{
			m_pCategory->SetDiffuse( Castor::Point4f( r, g, b, i ) );
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_diffuse	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_diffuse	La nouvelle valeur
		 */
		inline void SetDiffuse( Castor::Point4f const & p_diffuse )
		{
			m_pCategory->SetDiffuse( p_diffuse );
		}
		/**
		 *\~english
		 *\brief		Sets the diffuse colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_diffuse	The new value
		 *\~french
		 *\brief		Définit la couleur diffuse (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_diffuse	La nouvelle valeur
		 */
		inline void SetDiffuse( Castor::Colour const & p_diffuse )
		{
			p_diffuse.to_rgba( m_pCategory->GetDiffuse() );
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_specular	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_specular	La nouvelle valeur
		 */
		inline void SetSpecular( float * p_specular )
		{
			m_pCategory->SetSpecular( Castor::Point4f( p_specular[0], p_specular[1], p_specular[2], p_specular[2] ) );
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour
		 *\param[in]	r, g, b	The RGB components
		 *\param[in]	i		The intensity component
		 *\~french
		 *\brief		Définit la couleur spéculaire
		 *\param[in]	r, g, b	Les composantes RGB
		 *\param[in]	i		La composante intensité
		 */
		inline void SetSpecular( float r, float g, float b, float i )
		{
			m_pCategory->SetSpecular( Castor::Point4f( r, g, b, i ) );
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_specular	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_specular	La nouvelle valeur
		 */
		inline void SetSpecular( Castor::Point4f const & p_specular )
		{
			m_pCategory->SetSpecular( p_specular );
		}
		/**
		 *\~english
		 *\brief		Sets the specular colour (4 components: R, G, B and Intensity)
		 *\param[in]	p_specular	The new value
		 *\~french
		 *\brief		Définit la couleur spéculaire (4 composantes: R, V, B et Intensité)
		 *\param[in]	p_specular	La nouvelle valeur
		 */
		inline void SetSpecular( Castor::Colour const & p_specular )
		{
			p_specular.to_rgba( m_pCategory->GetSpecular() );
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
		//!\~english Tells the light is enabled	\~french Dit si la lumière est active ou pas
		bool m_enabled;
		//!\~english The Light category that effectively holds light data	\~french la light category contenant les données de la lumière
		LightCategorySPtr m_pCategory;
		//!\~english The light index	\~french L'indice de la lumière
		int m_iIndex;
	};
}

#endif
