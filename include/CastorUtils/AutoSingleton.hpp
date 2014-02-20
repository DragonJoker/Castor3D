#ifndef ___AUTO_SINGLETON_H___
#	define ___AUTO_SINGLETON_H___

#	include "NonCopyable.hpp"

namespace Castor
{
	/*!
	\author		Sylvain DOREMUS
	\version	0.6.1.0
	\date		03/01/2011
	\~english
	\brief		Auto Singleton concept implementation
	\remark		Autosingleton is very easy :<br>When you'll need it, it'll be there. If you never use it, it won't ever be created.
	\~french
	\brief		Implémentation du concept d'Auto Singleton
	\remark		Autosingleton est très simple :<br>Si vous en avez besoin, il est là. S'il n'est jamais utilisé, il n'est jamais créé.
	*/
	template <class T>
	class AutoSingleton : private NonCopyable
	{
	protected:
		/**
		 *\~english
		 *\brief		Constructor
		 *\~french
		 *\brief		Constructeur
		 */
		AutoSingleton(){}
		/**
		 *\~english
		 *\brief		Destructor
		 *\~french
		 *\brief		Destructeur
		 */
		virtual ~AutoSingleton(){}

	public:
		/**
		 *\~english
		 *\brief		Accessor to the unique instance, creates it if not already created
		 *\return		Reference to the unique instance
		 *\~french
		 *\brief		Accesseur sur l'instance unique, celle-ci est créée si elle ne l'est pas encore
		 *\return		Référence sur l'instance
		 */
		static T & GetSingleton()
		{
			static T m_singleton;
			return m_singleton;
		}
		/**
		 *\~english
		 *\brief		Accessor to a pointer on the unique instance, creates it if not already created
		 *\return		Pointer to the unique instance
		 *\~french
		 *\brief		Accesseur à un pointeur sur l'instance unique, celle-ci est créée si elle ne l'est pas encore
		 *\return		Pointeur sur l'instance
		 */
		static T * GetSingletonPtr()
		{
			return &GetSingleton();
		}
	};
}

#endif
