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
#ifndef ___C3D_SPOT_LIGHT_H___
#define ___C3D_SPOT_LIGHT_H___

#include "Light.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.1
	\date		09/02/2010
	\~english
	\brief		Class which represents a SpotLight
	\remark		A spot light is a light which enlights from a point in a given direction with a conic shape
	\~french
	\brief		Classe de représentation d'une PointLight
	\remark		Une spot light est une source de lumière à un point donné qui illumine dans une direction donnée en suivant une forme cônique
	*/
	class SpotLight
		: public LightCategory
	{
	public:
		/*!
		\author 	Sylvain DOREMUS
		\date 		14/02/2010
		\~english
		\brief		SpotLight loader
		\~french
		\brief		Loader de SpotLight
		*/
		class TextLoader : public LightCategory::TextLoader
		{
		public:
			/**
			 *\~english
			 *\brief		Writes a light into a text file
			 *\param[in]	p_file	The file to save the cameras in
			 *\param[in]	p_light	The light to save
			 *\~french
			 *\brief		Ecrit une lumière dans un fichier texte
			 *\param[in]	p_file	Le fichier
			 *\param[in]	p_light	La lumière
			 */
			C3D_API virtual bool operator()( SpotLight const & p_light, Castor::TextFile & p_file );
		};
		/*!
		\author		Sylvain DOREMUS
		\date		14/02/2010
		\~english
		\brief		SpotLight loader
		\~english
		\brief		Loader de SpotLight
		*/
		class BinaryParser
			: public LightCategory::BinaryParser
		{
		public:
			/**
			 *\~english
			 *\brief		Constructor
			 *\param[in]	p_path	The current folder path
			 *\~french
			 *\brief		Constructeur
			 *\param[in]	p_path	Le chemin d'accès au dossier courant
			 */
			C3D_API BinaryParser( Castor::Path const & p_path );
			/**
			 *\~english
			 *\brief		Function used to fill the chunk from specific data
			 *\param[in]	p_obj	The object to write
			 *\param[out]	p_chunk	The chunk to fill
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de remplir le chunk de données spécifiques
			 *\param[in]	p_obj	L'objet à écrire
			 *\param[out]	p_chunk	Le chunk à remplir
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Fill( SpotLight const & p_obj, BinaryChunk & p_chunk )const;
			/**
			 *\~english
			 *\brief		Function used to retrieve specific data from the chunk
			 *\param[out]	p_obj	The object to read
			 *\param[in]	p_chunk	The chunk containing data
			 *\return		\p false if any error occured
			 *\~french
			 *\brief		Fonction utilisée afin de récupérer des données spécifiques à partir d'un chunk
			 *\param[out]	p_obj	L'objet à lire
			 *\param[in]	p_chunk	Le chunk contenant les données
			 *\return		\p false si une erreur quelconque est arrivée
			 */
			C3D_API virtual bool Parse( SpotLight & p_obj, BinaryChunk & p_chunk )const;
		};

	private:
		friend class Scene;

	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remarks		Not to be used by the user, use Scene::CreateLight function instead
		 *\~french
		 *\brief		Constructeur
		 *\remarks		A ne pas utiliser par l'utilisateur, utiliser Scene::CreateLight à la place
		 */
		C3D_API SpotLight();
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		C3D_API ~SpotLight();
		/**
		 *\~english
		 *\brief		Creation function, used by Factory
		 *\return		A light source
		 *\~french
		 *\brief		Fonction de création utilisée par Factory
		 *\return		Une source lumineuse
		 */
		C3D_API static LightCategorySPtr Create();
		/**
		 *\~english
		 *\brief		Puts the light into the given texture.
		 *\~french
		 *\brief		Met la lumière dans la texture donnée.
		 */
		C3D_API virtual void Bind( Castor::PxBufferBase & p_texture, uint32_t p_index )const;
		/**
		 *\~english
		 *\brief		Sets the light source position
		 *\param[in]	p_position	The new value
		 *\~french
		 *\brief		Définit la position de la source
		 *\param[in]	p_position	La nouvelle valeur
		 */
		C3D_API virtual void SetPosition( Castor::Point3r const & p_position );
		/**
		 *\~english
		 *\brief		Retrieves the light source position
		 *\return		The value
		 *\~french
		 *\brief		Récupère la position de la source
		 *\return		La valeur
		 */
		C3D_API virtual Castor::Point3f GetPosition()const;
		/**
		 *\~english
		 *\brief		Sets attenuation components
		 *\param[in]	p_ptAttenuation	The attenuation components
		 *\~french
		 *\brief		Définit les composantes d'atténuation
		 *\param[in]	p_ptAttenuation	Les composantes d'attenuation
		 */
		C3D_API void SetAttenuation( Castor::Point3f const & p_ptAttenuation );
		/**
		*\~english
		*\brief			Sets the light exponent
		 *\param[in]	p_exponent	The new exponent value
		 */
		C3D_API void SetExponent( float p_exponent );
		/**
		*\~english
		*\brief			Sets the light cutoff
		 *\param[in]	p_cutOff	The new cutoff value
		 */
		C3D_API void SetCutOff( float p_cutOff );
		/**
		 *\~english
		 *\brief		Retrieves the attenuation components
		 *\return		The attenuation components
		 *\~french
		 *\brief		Récupère les composantes d'attenuation
		 *\return		Les composantes d'attenuation
		 */
		inline Castor::Point3f const & GetAttenuation()const
		{
			return m_attenuation;
		}
		/**
		 *\~english
		 *\brief		Retrieves the attenuation components
		 *\return		The attenuation components
		 *\~french
		 *\brief		Récupère les composantes d'attenuation
		 *\return		Les composantes d'attenuation
		 */
		inline Castor::Point3f & GetAttenuation()
		{
			return m_attenuation;
		}
		/**
		 *\~english
		 *\brief		Retrieves the exponent value
		 *\return		The exponent value
		 *\~french
		 *\brief		Récupère la valeur de l'exposant
		 *\return		La valeur de l'exposant
		 */
		inline float GetExponent()const
		{
			return m_exponent;
		}
		/**
		 *\~english
		 *\brief		Retrieves the cone angle
		 *\return		The cone angle
		 *\~french
		 *\brief		Récupère l'angle du cône
		 *\return		L'angle du cône
		 */
		inline float GetCutOff()const
		{
			return m_cutOff;
		}

	private:
		using LightCategory::DoBindComponent;
		void DoBindComponent( float p_exp, float p_cut, int p_index, int & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( Castor::Matrix4x4f const & p_component, int p_index, int & p_offset, Castor::PxBufferBase & p_data )const;
		void DoBindComponent( Castor::Matrix4x4d const & p_component, int p_index, int & p_offset, Castor::PxBufferBase & p_data )const;

	private:
		//!\~english The attenuation components : constant, linear and quadratic	\~french Les composantes d'attenuation : constante, linéaire et quadratique
		Castor::Point3f m_attenuation;
		//!\~english The light exponent, id est how much the light is focused	\~french L'exposant de la lumièrs, càd à quel point elle est concentrée
		float m_exponent;
		//!\~english The angle of the cone	\~french L'angle du cône
		float m_cutOff;
	};
}

#endif
