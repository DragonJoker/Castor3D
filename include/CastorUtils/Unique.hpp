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
#ifndef ___CU_Unique___
#define ___CU_Unique___

#include "Exception.hpp"

#pragma warning( push )
#pragma warning( disable:4661 )

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Unicity exception
	\remark		Thrown if a second instance of a Unique class is created
	\~french
	\brief		Exception d'unicité
	\remark		Lancée si un une seconde instance d'une classe Unique est créée
	*/
	class UnicityError : public Castor::Exception
	{
	public:
		UnicityError( std::string const & p_strClass ) : Exception( "Tried to create a second instance of unique class [" + p_strClass + "]", NULL, NULL, 0 ) {}
	};
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		08/12/2011
	\~english
	\brief		Representation of a Unique instance class
	\remark		If another instance is to be created, an exception is thrown
	\~french
	\brief		Représentation d'un classe à instance unique
	\remark		Si une seconde instance est créée, une exception est lancée
	*/
	template< class T >
	class Unique
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\remark		Throws an exception if instance is already initialised
		 *\~french
		 *\brief		Constructeur
		 *\remark		Lance une exception si l'instance est déjà initialisée
		 */
		Unique()
		{
		}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~Unique()
		{
		}
		/**
		 *\~english
		 *\brief		Sets the instance to another one
		 *\param[in]	p_pInstance	The other instance
		 *\~french
		 *\brief		Définit l'instance à une autre
		 *\param[in]	p_pInstance	L'autre instance
		 */
		void SetInstance( T * p_pInstance )
		{
			static T * l_pInstance = NULL;

			if( !l_pInstance )
			{
				l_pInstance = p_pInstance;
			}
			else if( p_pInstance )
			{
				throw UnicityError( typeid( T ).name() );
			}
		}

	private:
		/**
		 *\~english
		 *\brief		Private copy constructor, to forbid it
		 *\~french
		 *\brief		Constructeur par copie privé, afin d'en interdire la possibilité
		 */
		Unique( Unique const & ){}
		/**
		 *\~english
		 *\brief		Private copy assignment operator, to forbid it
		 *\~french
		 *\brief		Opérateur d'affectation par copie privé, afin d'en interdire la possibilité
		 */
		Unique & operator =( Unique const & ){ return * this; }
		/**
		 *\~english
		 *\brief		Private move constructor, to forbid it
		 *\~french
		 *\brief		Constructeur par déplacement privé, afin d'en interdire la possibilité
		 */
		Unique( Unique && ){}
		/**
		 *\~english
		 *\brief		Private move assignment operator, to forbid it
		 *\~french
		 *\brief		Opérateur d'affectation par déplacement privé, afin d'en interdire la possibilité
		 */
		Unique & operator =( Unique && ){ return * this; }
	};
	;
	//!\~english Helper macro to declare the static member Unique< T >::m_pInstance	\~french Macro pour faciliter la déclaration du membre statique Unique< T >::m_pInstance
#	define CASTOR_DECLARE_UNIQUE_INSTANCE( class_name )
	//!\~english Helper macro to initialise the Unique instance	\~french Macro pour faciliter l'initialisation de l'instance de Unique
#	define CASTOR_INIT_UNIQUE_INSTANCE() SetInstance( this )
	//!\~english Helper macro to cleanup the Unique instance	\~french Macro pour faciliter le nettoyage de l'instance de Unique
#	define CASTOR_CLEANUP_UNIQUE_INSTANCE() SetInstance( NULL )
}

#pragma warning( pop )

#endif