/*
See LICENSE file in root folder
*/
#ifndef ___C3D_RenderedObject_H___
#define ___C3D_RenderedObject_H___

#include "SceneModule.hpp"

#include <CastorUtils/Data/TextWriter.hpp>

namespace castor3d
{
	class RenderedObject
		: public std::enable_shared_from_this< RenderedObject >
	{
	public:
		void setVisible( bool value )
		{
			m_visible = value;
		}

		void setShadowCaster( bool value )
		{
			m_castsShadows = value;
		}

		void setShadowReceiver( bool value )
		{
			m_receivesShadows = value;
		}

		void setCulled( bool value )
		{
			m_culled = value;
		}

		bool isVisible()const
		{
			return m_visible;
		}

		bool isShadowCaster()const
		{
			return m_castsShadows;
		}

		bool isShadowReceiver()const
		{
			return m_receivesShadows;
		}

		bool isCulled()const
		{
			return m_culled;
		}

	private:
		bool m_visible{ true };
		bool m_castsShadows{ true };
		bool m_receivesShadows{ true };
		bool m_culled{ true };
	};
}

#endif
