#include "SceneCuller.hpp"

namespace castor3d
{
	SceneCuller::SceneCuller( Scene const & scene
		, Camera * camera )
		: m_scene{ scene }
		, m_camera{ camera }
	{
	}

	void SceneCuller::doClear()
	{
		m_opaqueSubmeshes.clear();
		m_transparentSubmeshes.clear();
		m_opaqueBillboards.clear();
		m_transparentBillboards.clear();
	}
}
