#include "PrecompiledHeader.h"

#include "render_system/Buffer.h"

using namespace Castor3D;

//***********************************************************************************************

void BufferManager :: Cleanup()
{
	m_stRealBuffers.Cleanup();
	m_stIntBuffers.Cleanup();
	m_stBoolBuffers.Cleanup();
	m_stCharBuffers.Cleanup();
	m_stUCharBuffers.Cleanup();
	m_stUIntBuffers.Cleanup();
	m_stLongBuffers.Cleanup();
	m_stULongBuffers.Cleanup();
	m_stLongLongBuffers.Cleanup();
	m_stULongLongBuffers.Cleanup();
}

void BufferManager :: Update()
{
	m_stRealBuffers.Update();
	m_stIntBuffers.Update();
	m_stBoolBuffers.Update();
	m_stCharBuffers.Update();
	m_stUCharBuffers.Update();
	m_stUIntBuffers.Update();
	m_stLongBuffers.Update();
	m_stULongBuffers.Update();
	m_stLongLongBuffers.Update();
	m_stULongLongBuffers.Update();
}

//***********************************************************************************************