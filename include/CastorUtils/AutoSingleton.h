#ifndef ___AUTO_SINGLETON_H___
#define ___AUTO_SINGLETON_H___

#include "NonCopyable.h"

/**
 * Autosingleton is very simple :
 * When you'll need it, it'll be there.
 * If you never use it, it won't ever be created.
**/

namespace Castor
{ namespace Theory
{
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
