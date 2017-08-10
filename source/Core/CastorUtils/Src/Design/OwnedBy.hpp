/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___CASTOR_OWNED_BY_H___
#define ___CASTOR_OWNED_BY_H___

#include "CastorUtilsPrerequisites.hpp"

namespace castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.8.0
	\date		08/10/2015
	\~english
	\brief		Class used to have an object owned by another one.
	\~french
	\brief		Classe permettant d'avoir un objet controlé par un autre.
	*/
	template< class Owner >
	class OwnedBy
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor
		 *\param[in]	p_owner		The owner object.
		 *\~french
		 *\brief		Constructeur
		 *\param[in]	p_owner		L'objet propriétaire.
		 */
		inline OwnedBy( Owner & p_owner )
			: m_owner( p_owner )
		{
		}
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		inline ~OwnedBy()
		{
		}
		/**
		 *\~english
		 *\return		The owner object.
		 *\~french
		 *\brief		L'objet propriétaire.
		 */
		inline Owner * getOwner()const
		{
			return &m_owner;
		}

	private:
		Owner & m_owner;
	};

	/**
	 *\~english
	 *\brief		Helper macro to declare an exported OwnedBy specialisation.
	 *\remarks		Must be used in global namespace.
	 *\~french
	 *\brief		Macro pour déclarer une spécialisation exportée de OwnedBy.
	 *\remarks		doit être utilisée dans le namespace global.
	 */
#	define DECLARED_EXPORTED_OWNED_BY( exp, owner, name )\
	namespace castor\
	{\
		template<>\
		class exp OwnedBy< owner >\
		{\
		private:\
			OwnedBy & operator=( OwnedBy< owner > const & p_rhs ) = delete;\
		public:\
			OwnedBy( owner & p_owner );\
			OwnedBy( OwnedBy< owner > const & p_rhs ) = default;\
			OwnedBy( OwnedBy< owner > && p_rhs ) = default;\
			OwnedBy & operator=( OwnedBy< owner > && p_rhs ) = default;\
			~OwnedBy() = default;\
			owner * get##name()const;\
		private:\
			owner & m_owner;\
		};\
	}

	/**
	 *\~english
	 *\brief		Helper macro to implement an OwnedBy specialisation.
	 *\remarks		Must be used in global namespace.
	 *\~french
	 *\brief		Macro pour implémenter une spécialisation de OwnedBy.
	 *\remarks		doit être utilisée dans le namespace global.
	 */
#	define IMPLEMENT_EXPORTED_OWNED_BY( owner, name )\
	namespace castor\
	{\
		OwnedBy< owner >::OwnedBy( owner & p_owner )\
			: m_owner( p_owner )\
		{\
		}\
		owner * OwnedBy< owner >::get##name()const\
		{\
			return &m_owner;\
		}\
	}
}

#endif
