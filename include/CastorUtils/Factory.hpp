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
#ifndef ___Castor_Factory___
#define ___Castor_Factory___

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Factory concept implementation
	\remark		The classes that can be registered must implement a function of the following form :
				<br />static std::shared_ptr< Obj > Create();
	\~french
	\brief		Implémentation du concept de fabrique
	\remark		Les classes pouvant être enregistrées doivent implémenter une fonction de la forme suivante :
				<br />static std::shared_ptr< Obj > Create();
	*/
	template< class Obj, class Key=String, class Predicate=std::less< Key > >
	class Factory : CuNonCopyable
	{
	protected:
		typedef std::shared_ptr< Obj > obj_ptr;
		typedef obj_ptr (FNCreate)();
		typedef FNCreate * PFNCreate;
		typedef std::map< Key, PFNCreate, Predicate > obj_map;

		obj_map m_mapRegistered;


	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		Factory(){}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Factory(){}
		/**
		 *\~english
		 *\brief		Registers an object type
		 *\param[in]	p_key	The object type
		 *\param[in]	p_obj	The object to register (it will be cloned each time a creation is asked for)
		 *\~french
		 *\brief		Enregistre un type d'objet
		 *\param[in]	p_key	Le type d'objet
		 *\param[in]	p_obj	L'objet à enregistrer (il sera clôné à chaque demande de création)
		 */
		void Register( Key const & p_key, PFNCreate p_pfnCreate )
		{
			if( m_mapRegistered.find( p_key ) == m_mapRegistered.end() )
			{
				m_mapRegistered.insert( std::make_pair( p_key, p_pfnCreate ) );
			}
		}
		/**
		 *\~english
		 *\brief		Creates an object from a key
		 *\param[in]	p_key	The object type
		 *\return		The created object
		 *\~french
		 *\brief		Crée un objet à partir d'une clef (type d'objet)
		 *\param[in]	p_key	Le type d'objet
		 *\return		L'objet créé
		 */
		virtual obj_ptr Create( Key const & p_key )
		{
			obj_ptr l_pReturn;
			typename obj_map::iterator l_it = m_mapRegistered.find( p_key );

			if( l_it != m_mapRegistered.end() )
			{
				l_pReturn = l_it->second();
			}

			return l_pReturn;
		}
	};
}

#endif
