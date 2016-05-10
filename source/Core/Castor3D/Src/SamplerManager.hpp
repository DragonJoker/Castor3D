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
#ifndef ___C3D_SAMPLER_MANAGER_H___
#define ___C3D_SAMPLER_MANAGER_H___

#include "RenderSystem.hpp"
#include "ResourceManager.hpp"
#include "Sampler.hpp"

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
	template<> struct ManagedObjectNamer< Sampler >
	{
		C3D_API static const Castor::String Name;
	};
	/*!
	\author 	Sylvain DOREMUS
	\date 		13/10/2015
	\version	0.8.0
	\~english
	\brief		Sampler manager.
	\~french
	\brief		Gestionnaire de Sampler.
	*/
	class SamplerManager
		: private ResourceManager< Castor::String, Sampler >
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\param[in]	p_engine	The engine.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_engine	Le moteur.
		 */
		C3D_API SamplerManager( Engine & p_engine );
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		C3D_API ~SamplerManager();
		/**
		 *\~english
		 *\brief		Creates and returns a Sampler, given a name
		 *\remarks		If a Sampler with the same name exists, none is created
		 *\param[in]	p_name		The Sampler name
		 *\param[in]	p_params	The other constructor parameters
		 *\return		The created or existing Sampler
		 *\~french
		 *\brief		Crée et renvoie un Sampler, avec le nom donné
		 *\remarks		Si un Sampler avec le même nom existe, aucun n'est créé
		 *\param[in]	p_name		Le nom du Sampler
		 *\param[in]	p_params	Les autres paramètres du constructeur
		 *\return		Le Sampler créé ou existant
		 */
		template< typename ... Parameters >
		inline std::shared_ptr< Sampler > Create( Castor::String const & p_name, Parameters && ... p_params )
		{
			std::unique_lock< Collection > l_lock( m_elements );
			SamplerSPtr l_return;

			if ( p_name.empty() )
			{
				l_return = m_renderSystem->CreateSampler( p_name );
			}
			else
			{
				if ( !m_elements.has( p_name ) )
				{
					l_return = m_renderSystem->CreateSampler( p_name );
					m_elements.insert( p_name, l_return );
					GetEngine()->PostEvent( MakeInitialiseEvent( *l_return ) );
					Castor::Logger::LogInfo( Castor::StringStream() << INFO_MANAGER_CREATED_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
				}
				else
				{
					l_return = m_elements.find( p_name );
					Castor::Logger::LogWarning( Castor::StringStream() << WARNING_MANAGER_DUPLICATE_OBJECT << this->GetObjectTypeName() << cuT( ": " ) << p_name );
				}
			}

			return l_return;
		}

	public:
		using ResourceManager< Castor::String, Sampler >::lock;
		using ResourceManager< Castor::String, Sampler >::unlock;
		using ResourceManager< Castor::String, Sampler >::begin;
		using ResourceManager< Castor::String, Sampler >::end;
		using ResourceManager< Castor::String, Sampler >::Has;
		using ResourceManager< Castor::String, Sampler >::Find;
		using ResourceManager< Castor::String, Sampler >::Insert;
		using ResourceManager< Castor::String, Sampler >::Remove;
		using ResourceManager< Castor::String, Sampler >::Cleanup;
		using ResourceManager< Castor::String, Sampler >::Clear;
		using ResourceManager< Castor::String, Sampler >::GetEngine;
		using ResourceManager< Castor::String, Sampler >::SetRenderSystem;
	};
}

#endif
