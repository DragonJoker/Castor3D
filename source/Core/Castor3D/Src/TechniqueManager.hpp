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
#ifndef ___C3D_TECHNIQUE_MANAGER_H___
#define ___C3D_TECHNIQUE_MANAGER_H___

#include "Manager/ResourceManager.hpp"

#include "Technique/RenderTechnique.hpp"
#include "Technique/TechniqueFactory.hpp"

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		21/02/2016
	\~english
	\brief		Helper structure to get an object type name.
	\~french
	\brief		Structure permettant de récupérer le nom du type d'un objet.
	*/
	template<> struct ManagedObjectNamer< RenderTechnique >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\version	0.8.0
	\date		21/02/2016
	\~english
	\brief		Render technique manager.
	\~french
	\brief		Le gestionnaire de techniques de rendu.
	*/
	class RenderTechniqueManager
		: protected ResourceManager< Castor::String, RenderTechnique >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		C3D_API RenderTechniqueManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API virtual ~RenderTechniqueManager();
		/**
		 *\~english
		 *\brief		Updates all techniques.
		 *\~french
		 *\brief		Met à jour toutes les techniques.
		 */
		C3D_API void Update();
		/**
		 *\~english
		 *\brief		Creates a RenderTechnique.
		 *\param[in]	p_name			The technique name.
		 *\param[in]	p_type			The technique type.
		 *\param[in]	p_renderTarget	The technique render target.
		 *\param[in]	p_renderSystem	The render system.
		 *\param[in]	p_params		The technique parameters.
		 *\return		The created RenderTechnique.
		 *\~french
		 *\brief		Crée une RenderTechnique.
		 *\param[in]	p_name			Le nom de la technique.
		 *\param[in]	p_type			Le type de technique.
		 *\param[in]	p_renderTarget	La cible de rendu de la technique.
		 *\param[in]	p_renderSystem	Le RenderSystem.
		 *\param[in]	p_params		Les paramètres de la technique.
		 *\return		La RenderTechnique créée.
		 */
		C3D_API RenderTechniqueSPtr Create( Castor::String const & p_name, Castor::String const & p_type, RenderTarget & p_renderTarget, RenderSystem * p_renderSystem, Parameters const & p_params );
		/**
		 *\~english
		 *\brief		Retrieves the RenderTechnique factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique de RenderTechnique
		 *\return		La fabrique
		 */
		inline TechniqueFactory const & GetTechniqueFactory()const
		{
			return m_techniqueFactory;
		}
		/**
		 *\~english
		 *\brief		Retrieves the RenderTechnique factory
		 *\return		The factory
		 *\~french
		 *\brief		Récupère la fabrique de RenderTechnique
		 *\return		La fabrique
		 */
		inline TechniqueFactory & GetTechniqueFactory()
		{
			return m_techniqueFactory;
		}

	public:
		using ResourceManager< Castor::String, RenderTechnique >::Cleanup;
		using ResourceManager< Castor::String, RenderTechnique >::Clear;
		using ResourceManager< Castor::String, RenderTechnique >::SetRenderSystem;
		using ResourceManager< Castor::String, RenderTechnique >::lock;
		using ResourceManager< Castor::String, RenderTechnique >::unlock;

	private:
		//!\~english The RenderTechnique factory	\~french La fabrique de RenderTechnique
		TechniqueFactory m_techniqueFactory;
	};
}

#endif
