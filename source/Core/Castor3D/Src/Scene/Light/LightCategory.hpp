/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)

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
#ifndef ___C3D_LIGHT_CATEGORY_H___
#define ___C3D_LIGHT_CATEGORY_H___

#include "Castor3DPrerequisites.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		14/02/2010
	\~english
	\brief		Holds light components
	\remark		Derived into DirectionalLight, PointLight and SpotLight
	\~french
	\brief		Contient les composantes d'une lumière
	\remark		Dérivée en DirectionalLight, PointLight et SpotLight
	*/
	class LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		LightCategory loader
		\~french
		\brief		Loader de LightCategory
		*/
		class TextWriter
			: public Castor::TextWriter< LightCategory >
		{
		public:
			/**
			 *\~english
			 *\brief		Writes a LightCategory into a text file.
			 *\param[in]	p_file	The file.
			 *\~french
			 *\brief		Ecrit une LightCategory dans un fichier texte.
			 *\param[in]	p_file	Le fichier.
			 */
			C3D_API virtual bool WriteInto( Castor::TextFile & p_file ) = 0;

		protected:
			/**
			 *\~english
			 *\brief		Constructor
			 *\~french
			 *\brief		Constructeur
			 */
			C3D_API explicit TextWriter( Castor::String const & p_tabs );
			/**
			 *\~english
			 *\brief		Writes a LightCategory into a text file.
			 *\param[in]	p_file	The file.
			 *\param[in]	p_light	The LightCategory to save.
			 *\~french
			 *\brief		Ecrit une LightCategory dans un fichier texte.
			 *\param[in]	p_file	Le fichier.
			 *\param[in]	p_light	La LightCategory.
			 */
			C3D_API bool operator()( LightCategory const & p_light, Castor::TextFile & p_file )override;
		};

	private:
		friend class Light;

	public:
		/**
		 *\~english
		 *\brief		The constructor, used by clone function
		 *\param[in]	p_lightType	The light category type
		 *\~french
		 *\brief		Le constructeur utilisé par la fonction de clonage
		 *\param[in]	p_lightType	Le type de catégorie de lumière
		 */
		C3D_API explicit LightCategory( eLIGHT_TYPE p_lightType );
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API virtual ~LightCategory();
		/**
		 *\~english
		 *\brief		Creates a LightCategroy specific TextLoader.
		 *\param[in]	p_tabs			The current indentation level.
		 *\param[in]	p_encodingMode	The file encoding mode.
		 *\return		The TextLoader.
		 *\~french
		 *\brief		Crée un TextLoader spécifique à la LightCategory.
		 *\param[in]	p_tabs			Le niveau d'intentation actuel.
		 *\param[in]	p_encodingMode	Le mode d'encodage du fichier.
		 *\return		Le TextLoader.
		 */
		C3D_API virtual std::unique_ptr< TextWriter > CreateTextWriter( Castor::String const & p_tabs ) = 0;
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\param[out]	p_texture	The texture that receives the light's data.
		 *\param[in]	p_index		The light index.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 *\param[out]	p_texture	La texture recevant les données de la source lumineuse.
		 *\param[in]	p_index		L'indice de la source lumineuse.
		 */
		C3D_API virtual void Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const = 0;
		/**
		 *\~english
		 *\brief		Sets the light's colour.
		 *\param[in]	p_colour	The new value.
		 *\~french
		 *\brief		Définit la couleur.
		 *\param[in]	p_colour	La nouvelle valeur.
		 */
		inline void SetColour( Castor::Point3f const & p_colour )
		{
			m_colour = p_colour;
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
			m_intensity = p_intensity;
		}
		/**
		 *\~english
		 *\brief		Sets the light's diffuse intensity
		 *\param[in]	p_intensity	The new value
		 *\~french
		 *\brief		Définit l'intensité diffuse
		 *\param[in]	p_intensity	La nouvelle valeur
		 */
		inline void SetAmbientIntensity( float p_intensity )
		{
			m_intensity[0] = p_intensity;
		}
		/**
		 *\~english
		 *\brief		Sets the light's diffuse intensity
		 *\param[in]	p_intensity	The new value
		 *\~french
		 *\brief		Définit l'intensité diffuse
		 *\param[in]	p_intensity	La nouvelle valeur
		 */
		inline void SetDiffuseIntensity( float p_intensity )
		{
			m_intensity[1] = p_intensity;
		}
		/**
		 *\~english
		 *\brief		Sets the light's specular intensity
		 *\param[in]	p_intensity	The new value
		 *\~french
		 *\brief		Définit l'intensité spéculaire
		 *\param[in]	p_intensity	La nouvelle valeur
		 */
		inline void SetSpecularIntensity( float p_intensity )
		{
			m_intensity[2] = p_intensity;
		}
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
			return m_eLightType;
		}
		/**
		 *\~english
		 *\brief		Retrieves the ambient colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur ambiante
		 *\return		La valeur
		 */
		inline float GetAmbientIntensity()const
		{
			return m_intensity[0];
		}
		/**
		 *\~english
		 *\brief		Retrieves the diffuse colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur diffuse
		 *\return		La valeur
		 */
		inline float GetDiffuseIntensity()const
		{
			return m_intensity[1];
		}
		/**
		 *\~english
		 *\brief		Retrieves the specular colour
		 *\return		The value
		 *\~french
		 *\brief		Récupère la couleur spéculaire
		 *\return		La valeur
		 */
		inline float GetSpecularIntensity()const
		{
			return m_intensity[2];
		}
		/**
		 *\~english
		 *\return		The intensity values
		 *\~french
		 *\return		Les valeurs d'intensité
		 */
		inline Castor::Point3f const & GetIntensity()const
		{
			return m_intensity;
		}
		/**
		 *\~english
		 *\return		The intensity values
		 *\~french
		 *\return		Les valeurs d'intensité
		 */
		inline Castor::Point3f & GetIntensity()
		{
			return m_intensity;
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline Castor::Point3f const & GetColour()const
		{
			return m_colour;
		}
		/**
		 *\~english
		 *\return		The colour.
		 *\~french
		 *\return		La couleur.
		 */
		inline Castor::Point3f & GetColour()
		{
			return m_colour;
		}
		/**
		 *\~english
		 *\brief		Retrieves the parent light
		 *\return		The value
		 *\~french
		 *\brief		Récupère la lumière parente
		 *\return		La valeur
		 */
		inline Light * GetLight()const
		{
			return m_pLight;
		}
		/**
		 *\~english
		 *\brief		Sets the parent light
		 *\return		The value
		 *\~french
		 *\brief		Définit la lumière parente
		 *\return		La valeur
		 */
		inline void SetLight( Light * val )
		{
			m_pLight = val;
		}

	protected:
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
			return m_positionType;
		}
		/**
		 *\~english
		 *\brief		Sets the light's type and position
		 *\param[in]	p_position	The new value
		 *\~french
		 *\brief		Définit le type de la lumière et sa position
		 *\param[in]	p_position	La nouvelle valeur
		 */
		virtual void SetPositionType( Castor::Point4f const & p_position )
		{
			m_positionType[0] = float( p_position[0] );
			m_positionType[1] = float( p_position[1] );
			m_positionType[2] = float( -p_position[2] );
			m_positionType[3] = float( p_position[3] );
		}
		/**
		 *\~english
		 *\brief		Sets the light's type and position
		 *\param[in]	p_position	The new value
		 *\~french
		 *\brief		Définit le type de la lumière et sa position
		 *\param[in]	p_position	La nouvelle valeur
		 */
		virtual void SetPositionType( Castor::Point4d const & p_position )
		{
			m_positionType[0] = float( p_position[0] );
			m_positionType[1] = float( p_position[1] );
			m_positionType[2] = float( -p_position[2] );
			m_positionType[3] = float( p_position[3] );
		}

	protected:
		void DoBindComponent( Castor::Point3f const & p_component, int p_index, int & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( Castor::Point4f const & p_component, int p_index, int & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( Castor::Coords4f const & p_component, int p_index, int & p_offset, Castor::PxBufferBase & p_data )const;

	private:
		//!\~english The light type	\~french Le type de lumière
		eLIGHT_TYPE m_eLightType;
		//!\~english The parent light	\~french La lumière parente
		Light * m_pLight{ nullptr };
		//!\~english The colour.	\~french La couleur.
		Castor::Point3f m_colour;
		//!\~english The intensity values.	\~french Les valeurs d'intensité.
		Castor::Point3f m_intensity;
		//!\~english The position and type of the light (type is in 4th coordinate)	\~french La position et le type de la lumière (le type est dans la 4ème coordonnée)
		Castor::Point4f m_positionType;
	};
}

#endif
