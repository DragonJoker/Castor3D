#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/mesh/Subdivider.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/basic/SmoothingGroup.h"
#include "Castor3D/render_system/Buffer.h"
#include "Castor3D/main/FrameListener.h"
#include "Castor3D/main/Root.h"

using namespace Castor3D;

//*********************************************************************************************

SubdivisionFrameEvent :: SubdivisionFrameEvent( Submesh * p_pSubmesh)
	:	FrameEvent( FrameEvent::eQueueRender)
	,	m_pSubmesh( p_pSubmesh)
{
}

SubdivisionFrameEvent :: ~SubdivisionFrameEvent()
{
}

bool SubdivisionFrameEvent :: Apply()
{
	m_pSubmesh->GenerateBuffers();
	m_pSubmesh->ComputeNormals();
	m_pSubmesh->GetRenderer()->Initialise();
	return true;
}

//*********************************************************************************************

Subdivider :: Subdivider( Submesh * p_submesh)
	:	m_submesh( p_submesh)
	,	m_bGenerateBuffers( true)
	,	m_pfnSubdivisionEnd( NULL)
	,	m_bThreaded( false)
{
}

Subdivider :: ~Subdivider()
{
	Cleanup();
}

void Subdivider :: Initialise()
{
	m_points = m_submesh->GetPoints();
	m_smoothGroups = m_submesh->GetSmoothGroups();
}

void Subdivider :: Subdivide( const Point3r & p_center, bool p_bGenerateBuffers, bool p_bThreaded)
{
	Initialise();
	m_vertex = m_submesh->GetRenderer()->GetVertex();
	m_bGenerateBuffers = p_bGenerateBuffers;
	m_ptDivisionCenter = p_center;
	m_bThreaded = p_bThreaded;

	if (p_bThreaded)
	{
		m_pThread.reset( MultiThreading::CreateThread( _subdivideThreaded, this));
	}
	else
	{
		_subdivide();
		_switchBuffers();
	}
}

void Subdivider :: StartThread()
{
	if (m_bThreaded)
	{
		m_pThread->ResumeThread();
	}
}

void Subdivider :: Cleanup()
{
	m_vertex.reset();
	m_submesh = NULL;
	m_points.clear();
	m_smoothGroups.clear();
	m_pfnSubdivisionEnd = NULL;

	if (m_bThreaded && m_pThread != NULL)
	{
		m_pThread->Wait( 0);
	}
}

IdPoint3rPtr Subdivider :: AddPoint( real x, real y, real z)
{
	m_points.push_back( IdPoint3rPtr( new IdPoint3r( x, y, z, m_points.size())));
	return m_points[m_points.size() - 1];
}

IdPoint3rPtr Subdivider :: AddPoint( const Point3r & p_v)
{
	m_points.push_back( IdPoint3rPtr( new IdPoint3r( p_v, m_points.size())));
	return m_points[m_points.size() - 1];
}

IdPoint3rPtr Subdivider :: AddPoint( real * p_v)
{
	m_points.push_back( IdPoint3rPtr( new IdPoint3r( p_v, m_points.size())));
	return m_points[m_points.size() - 1];
}

FacePtr Subdivider :: AddFace( size_t a, size_t b, size_t c, size_t p_sgIndex)
{
	CASTOR_ASSERT( a < m_points.size() && b < m_points.size() && c < m_points.size() && p_sgIndex < m_smoothGroups.size());

	FacePtr l_pReturn( new Face( * m_points[a], * m_points[b], * m_points[c]));

	m_smoothGroups[p_sgIndex]->AddFace( l_pReturn);
	m_vertex->IncreaseSize( Vertex::Size * 3, false);

	return l_pReturn;
}

int Subdivider :: IsInMyPoints( const Point3r & p_vertex)
{
	int l_iReturn = -1;

	for (unsigned int i = 0 ; i < m_points.size() && l_iReturn == -1 ; i++)
	{
		if (p_vertex == * m_points[i])
		{
			l_iReturn = int( i);
		}
	}

	return l_iReturn;
}

void Subdivider :: _switchBuffers()
{
	m_submesh->SetPoints( m_points);
	m_submesh->SetSmoothGroups( m_smoothGroups);

	if (m_bGenerateBuffers && ! m_bThreaded)
	{
		m_submesh->GenerateBuffers();
		m_submesh->ComputeNormals();
		m_submesh->GetRenderer()->Initialise();
	}
}

unsigned int Subdivider :: _subdivideThreaded( void * p_pThis)
{
	Subdivider * l_pThis = static_cast <Subdivider *>( p_pThis);

	CASTOR_RECURSIVE_MUTEX_SCOPED_LOCK( l_pThis->m_mutex);
	l_pThis->_subdivide();
	l_pThis->_switchBuffers();

	if (l_pThis->m_bGenerateBuffers)
	{
		Root::GetSingletonPtr()->PostEvent( FrameEventPtr( new SubdivisionFrameEvent( l_pThis->m_submesh)));
	}

	if (l_pThis->m_pfnSubdivisionEnd != NULL)
	{
		l_pThis->m_pfnSubdivisionEnd( l_pThis->m_pArg, l_pThis);
	}

	return 0;
}

void Subdivider :: _setTextCoords( FacePtr p_face, IdPoint3rPtr p_a, IdPoint3rPtr p_b, IdPoint3rPtr p_c, 
								  IdPoint3rPtr p_d, IdPoint3rPtr p_e, IdPoint3rPtr p_f, size_t p_sgIndex)
{
	Point2r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;

	l_aTex = p_face->GetVertex( 0).GetTexCoord();
	l_bTex = p_face->GetVertex( 1).GetTexCoord();
	l_cTex = p_face->GetVertex( 2).GetTexCoord();

	l_dTex = l_aTex + (l_bTex - l_aTex) / 2.0f;
	l_eTex = l_bTex + (l_cTex - l_bTex) / 2.0f;
	l_fTex = l_aTex + (l_cTex - l_aTex) / 2.0f;

	FacePtr l_faceA = AddFace( p_a->GetIndex(), p_d->GetIndex(), p_f->GetIndex(), p_sgIndex);
	l_faceA->SetTextureCoords( l_aTex, l_dTex, l_fTex);
	FacePtr l_faceB = AddFace( p_b->GetIndex(), p_e->GetIndex(), p_d->GetIndex(), p_sgIndex);
	l_faceB->SetTextureCoords( l_bTex, l_eTex, l_dTex);
	FacePtr l_faceC = AddFace( p_c->GetIndex(), p_f->GetIndex(), p_e->GetIndex(), p_sgIndex);
	l_faceC->SetTextureCoords( l_cTex, l_fTex, l_eTex);
	FacePtr l_faceD = AddFace( p_d->GetIndex(), p_e->GetIndex(), p_f->GetIndex(), p_sgIndex);
	l_faceD->SetTextureCoords( l_dTex, l_eTex, l_fTex);
}

void Subdivider :: _setTextCoords( FacePtr p_face, const Vertex & p_a, const Vertex & p_b, const Vertex & p_c, 
								  const Vertex & p_d, const Vertex & p_e, const Vertex & p_f, size_t p_sgIndex)
{
	Point2r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;

	l_aTex = p_face->GetVertex( 0).GetTexCoord();
	l_bTex = p_face->GetVertex( 1).GetTexCoord();
	l_cTex = p_face->GetVertex( 2).GetTexCoord();

	l_dTex = l_aTex + (l_bTex - l_aTex) / 2.0f;
	l_eTex = l_bTex + (l_cTex - l_bTex) / 2.0f;
	l_fTex = l_aTex + (l_cTex - l_aTex) / 2.0f;

	FacePtr l_faceA = AddFace( p_a.GetIndex(), p_d.GetIndex(), p_f.GetIndex(), p_sgIndex);
	l_faceA->SetTextureCoords( l_aTex, l_dTex, l_fTex);
	FacePtr l_faceB = AddFace( p_b.GetIndex(), p_e.GetIndex(), p_d.GetIndex(), p_sgIndex);
	l_faceB->SetTextureCoords( l_bTex, l_eTex, l_dTex);
	FacePtr l_faceC = AddFace( p_c.GetIndex(), p_f.GetIndex(), p_e.GetIndex(), p_sgIndex);
	l_faceC->SetTextureCoords( l_cTex, l_fTex, l_eTex);
	FacePtr l_faceD = AddFace( p_d.GetIndex(), p_e.GetIndex(), p_f.GetIndex(), p_sgIndex);
	l_faceD->SetTextureCoords( l_dTex, l_eTex, l_fTex);
}

Point3r Subdivider :: _computeCenterFrom( const Point3r & p_a, const Point3r & p_b, const Point3r & p_ptANormal, const Point3r & p_ptBNomal)
{
	Point3r l_result;

	// Projection sur XY => Récupération du point d'intersection (X, Y) y = ax + b
	Line3D<real> l_aEq( p_a, p_ptANormal);
	Line3D<real> l_bEq( p_b, p_ptBNomal);
	Point3r l_pt1;

	if (l_aEq.Intersects( l_bEq, l_pt1))
	{
		// Projection sur XZ => Récupération du point d'intersection (X, Z) z = ax + b
		l_aEq = Line3D<real>( p_a, p_ptANormal);
		l_bEq = Line3D<real>( p_b, p_ptBNomal);
		Point3r l_pt2;

		if (l_aEq.Intersects( l_bEq, l_pt2))
		{
			l_result[0] = l_pt1[0] + (l_pt2[0] - l_pt1[0]) / 2.0f;
			l_result[1] = l_pt1[1] + (l_pt2[1] - l_pt1[1]) / 2.0f;
			l_result[2] = l_pt1[2] + (l_pt2[2] - l_pt1[2]) / 2.0f;
		}
	}

	return l_result;
}

Point3r Subdivider :: _computeCenterFrom( const Point3r & p_a, const Point3r & p_b, const Point3r & p_c, const Point3r & p_ptANormal, const Point3r & p_ptBNormal, const Point3r & p_ptCNormal)
{
	Point3r l_result;

	PlaneEq<real> ABO( p_a, p_ptANormal, p_ptBNormal);
	PlaneEq<real> BCO( p_b, p_ptBNormal, p_ptCNormal);
	PlaneEq<real> CAO( p_c, p_ptCNormal, p_ptANormal);

	ABO.Intersects( BCO, CAO, l_result);

	return l_result;
}

//*********************************************************************************************

String & Castor3D::operator << ( String & p_stream, const IdPoint3r & p_vertex)
{
	p_stream << CU_T( "IdPoint3r[") << p_vertex.GetIndex() << CU_T( "] - Buffer : [") << p_vertex.const_ptr() << CU_T( "] - (") << p_vertex[0] << CU_T( ",") << p_vertex[1] << CU_T( ",") << p_vertex[2] << CU_T( ")");
	return p_stream;
}

String & Castor3D::operator << ( String & p_stream, const Vertex & p_vertex)
{
	p_stream << CU_T( "Vertex[") << p_vertex.GetIndex() << CU_T( "] - Buffer : [") << p_vertex.GetCoords().const_ptr() << CU_T( "] - (") << p_vertex.GetCoords()[0] << CU_T( ",") << p_vertex.GetCoords()[1] << CU_T( ",") << p_vertex.GetCoords()[2] << CU_T( ")");
	return p_stream;
}

//*********************************************************************************************