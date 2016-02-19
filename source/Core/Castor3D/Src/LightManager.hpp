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
#ifndef ___C3D_LIGHT_MANAGER_H___
#define ___C3D_LIGHT_MANAGER_H___

#include "ObjectManager.hpp"

#include "Light.hpp"
#include "LightFactory.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		04/02/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct ManagedObjectNamer< Light >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Helper structure to enable attaching if a type supports it.
	\remarks	Specialisation for Light.
	\~french
	\brief		Structure permettant d'attacher les éléments qui le supportent.
	\remarks	Spécialisation pour Light.
	*/
	template<>
	struct ElementAttacher< Light >
	{
		static void Attach( std::shared_ptr< Light > p_element, SceneNodeSPtr p_parent, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode )
		{
			if ( p_parent )
			{
				p_parent->AttachObject( p_element );
			}
			else
			{
				p_rootObjectNode->AttachObject( p_element );
			}
		}
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		29/01/2016
	\version	0.8.0
	\~english
	\brief		Light manager.
	\~french
	\brief		Gestionnaire de Light.
	*/
	class LightManager
		: public ObjectManager< Castor::String, Light >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_owner				The owner.
		 *\param[in]	p_rootNode			The root node.
		 *\param[in]	p_rootCameraNode	The cameras root node.
		 *\param[in]	p_rootObjectNode	The objects root node.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_owner				Le propriétaire.
		 *\param[in]	p_rootNode			Le noeud racine.
		 *\param[in]	p_rootCameraNode	Le noeud racine des caméras.
		 *\param[in]	p_rootObjectNode	Le noeud racine des objets.
		 */
		C3D_API LightManager( Scene & p_owner, SceneNodeSPtr p_rootNode, SceneNodeSPtr p_rootCameraNode, SceneNodeSPtr p_rootObjectNode );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~LightManager();
		/**
		 *\~english
		 *\brief		Sets all the elements to be cleaned up.
		 *\~french
		 *\brief		Met tous les éléments à nettoyer.
		 */
		C3D_API void Cleanup();
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name		The element name.
		 *\param[in]	p_element	The element.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'élément.
		 *\param[in]	p_element	L'élément.
		 */
		C3D_API void Insert( Castor::String const & p_name, LightSPtr p_element );
		/**
		 *\~english
		 *\brief		Removes an object, given a name.
		 *\param[in]	p_name		The object name.
		 *\~french
		 *\brief		Retire un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'objet.
		 */
		C3D_API void Remove( Castor::String const & p_name );
		/**
		 *\~english
		 *\brief		Binds the scene lights.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_sceneBuffer	The constants buffer.
		 *\~french
		 *\brief		Attache les sources lumineuses
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_sceneBuffer	Le tampon de constantes.
		 */
		C3D_API void BindLights( ShaderProgram & p_program, FrameVariableBuffer & p_sceneBuffer );
		/**
		 *\~english
		 *\brief		Unbinds the scene lights.
		 *\param[in]	p_program		The shader program.
		 *\param[in]	p_sceneBuffer	The constants buffer.
		 *\~french
		 *\brief		Détache les sources lumineuses
		 *\param[in]	p_program		Le programme shader.
		 *\param[in]	p_sceneBuffer	Le tampon de constantes.
		 */
		C3D_API void UnbindLights( ShaderProgram & p_program, FrameVariableBuffer & p_sceneBuffer );
		/**
		 *\~english
		 *\brief		Creates an object from a name.
		 *\param[in]	p_name		The object name.
		 *\param[in]	p_params	The other constructor parameters.
		 *\return		The created object.
		 *\~french
		 *\brief		Crée un objet à partir d'un nom.
		 *\param[in]	p_name		Le nom d'objet.
		 *\param[in]	p_params	Les autres paramètres de construction.
		 *\return		L'objet créé.
		 */
		C3D_API std::shared_ptr< Light > Create( Castor::String const & p_name, SceneNodeSPtr p_parent, eLIGHT_TYPE p_eLightType );

	private:
		C3D_API void DoAddLight( LightSPtr p_light );

	private:
		using LightsArray = std::vector< LightSPtr >;
		using LightsMap = std::map< eLIGHT_TYPE, LightsArray >;

	private:
		//!\~english The lights factory	\~french La fabrique de lumières
		LightFactory & m_lightFactory;
		//!\~english The lights sorted byt light type	\~french Les lumières, triées par type de lumière.
		LightsMap m_typeSortedLights;
		//!\~english The image containing lights data	\~french L'image contenant les données des lumières
		Castor::PxBufferBaseSPtr m_lightsData;
		//!\~english The lights texture	\~french La texture contenant les lumières
		TextureUnitSPtr m_lightsTexture;
	};
}

#endif
