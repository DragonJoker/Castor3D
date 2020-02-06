﻿#include "SubdivisionFrameEvent.hpp"
#include "Submesh.hpp"

using namespace Castor;

namespace Castor3D
{
	SubdivisionFrameEvent::SubdivisionFrameEvent( SubmeshSPtr p_pSubmesh )
		: FrameEvent( eEVENT_TYPE_PRE_RENDER )
		, m_pSubmesh( p_pSubmesh )
	{
	}

	SubdivisionFrameEvent::~SubdivisionFrameEvent()
	{
	}

	bool SubdivisionFrameEvent::Apply()
	{
		m_pSubmesh->ComputeNormals();
		m_pSubmesh->Initialise();
		return true;
	}
}
