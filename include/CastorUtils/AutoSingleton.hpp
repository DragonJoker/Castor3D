#ifndef ___AUTO_SINGLETON_H___
#define ___AUTO_SINGLETON_H___

#include "NonCopyable.hpp"

namespace Castor
{ namespace Theory
{
	/*!
	Autosingleton is very simple :
	When you'll need it, it'll be there.
	If you never use it, it won't ever be created.
	\author Sylvain DOREMUS
	\version 0.6.1.0
	\date 03/01/2011
	*/
	template <class T>
	class AutoSingleton : d_noncopyable
	{
	protected:
		AutoSingleton(){}
		~AutoSingleton(){}

	public:
		static T & GetSingleton()
		{
			static T m_singleton;
			return m_singleton;
		}

		static T * GetSingletonPtr()
		{
			return & GetSingleton();
		}
	};
}
}

#endif
