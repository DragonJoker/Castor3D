#include "LoopDivider/LoopDivider.hpp"

using namespace Loop;
using namespace Castor;

//*************************************************************************************************

C3D_Loop_API void GetRequiredVersion( Castor3D::Version & p_version)
{
	p_version = Castor3D::Version();
}

C3D_Loop_API Castor3D::ePLUGIN_TYPE GetType()
{
	return Castor3D::ePLUGIN_TYPE_DIVIDER;
}

C3D_Loop_API String GetName()
{
	return cuT( "Loop Divider");
}

C3D_Loop_API Castor3D::Subdivider * CreateDivider()
{
	Castor3D::Subdivider * l_pReturn( new Subdivider() );

	return l_pReturn;
}

C3D_Loop_API void DestroyDivider( Castor3D::Subdivider * p_pDivider )
{
	delete p_pDivider;
}

C3D_Loop_API String GetDividerType()
{
	return cuT( "loop");
}

//*********************************************************************************************

namespace
{
	static const int ALPHA_MAX = 20;
	static const double ALPHA_LIMIT = 0.469; /* converges to ~ 0.469 */

	real g_alpha[ALPHA_MAX] = { real(  1.13333 ), real( -0.358974 ), real( -0.333333 ), real(  0.129032 ), real(  0.945783 ), real(  2.0     ),
								real(  3.19889 ), real(  4.47885  ), real(  5.79946  ), real(  7.13634  ), real(  8.47535  ), real(  9.80865 ),
								real( 11.1322  ), real( 12.4441   ), real( 13.7439   ), real( 15.0317   ), real( 16.3082   ), real( 17.574   ),
								real( 18.83    ), real( 20.0769   )};

	static const int BETA_MAX = 20;
	static const double BETA_LIMIT = 0.469; /* converges to ~ 0.469 */

	real g_beta[BETA_MAX] = {	real( 0.46875  ), real( 1.21875 ), real( 1.125    ), real( 0.96875  ), real( 0.840932 ), real( 0.75     ), real( 0.686349 ),
								real( 0.641085 ), real( 0.60813 ), real( 0.583555 ), real( 0.564816 ), real( 0.55024  ), real( 0.5387   ),
								real( 0.529419 ), real( 0.52185 ), real( 0.515601 ), real( 0.510385 ), real( 0.505987 ), real( 0.502247 ), real( 0.49904 ) };

	real beta( uint32_t n )
	{
		real l_rTmp = real( 3.0 + 2.0 * cos( Angle::PiMult2 / n ) );
		return (real( 40.0 ) - (l_rTmp * l_rTmp)) / real( 32.0 );
	}

	real alpha( uint32_t n )
	{
		if( n <= ALPHA_MAX )
		{
			return g_alpha[n - 1];
		}

		real l_rBeta = beta( n );

		return n * (1 - l_rBeta) / l_rBeta;
	}
}

//*********************************************************************************************

Edge :: Edge( VertexSPtr p_v1, VertexSPtr p_v2, Castor3D::FaceSPtr p_f1, bool p_toDivide )
	:	m_firstVertex	( p_v1			)
	,	m_secondVertex	( p_v2			)
	,	m_firstFace		( p_f1			)
	,	m_divided		( false			)
	,	m_toDivide		( p_toDivide	)
	,	m_toDelete		( false			)
{
}

Edge :: ~Edge()
{
}

void Edge :: AddFace( Castor3D::FaceSPtr p_face )
{
	if( !m_firstFace )
	{
		m_firstFace = p_face;
	}
	else if( !m_secondFace )
	{
		m_secondFace = p_face;
	}
}

VertexSPtr Edge :: Divide( Subdivider * p_pDivider, real p_value )
{
	VertexSPtr l_pReturn;

	if( m_toDivide )
	{
		if( !m_divided )
		{
			// The edge is to divide and not divided yet, we create the new point
			Point3r l_ptPoint1;
			Point3r l_ptPoint2;
			Castor3D::Vertex::GetPosition( m_firstVertex->GetPoint(), l_ptPoint1 );
			l_ptPoint1 += Castor3D::Vertex::GetPosition( *m_secondVertex->GetPoint(), l_ptPoint2 );
			l_ptPoint1 *= p_value;
			m_createdVertex = p_pDivider->AddPoint( l_ptPoint1 );
			m_divided = true;
		}

		l_pReturn = m_createdVertex;
	}

	return l_pReturn;
}

//*************************************************************************************************

FaceEdges :: FaceEdges( Subdivider * p_pDivider, Castor3D::FaceSPtr p_face, VertexPtrUIntMap & p_mapVertex )
	:	m_pDivider	( p_pDivider								)
	,	m_face		( p_face									)
	,	m_bOwnFace	( true										)
	,	m_pVertex0	( p_mapVertex[p_face->GetVertexIndex( 0 )]	)
	,	m_pVertex1	( p_mapVertex[p_face->GetVertexIndex( 1 )]	)
	,	m_pVertex2	( p_mapVertex[p_face->GetVertexIndex( 2 )]	)
{
	m_edgeAB = DoAddEdge( m_pVertex0, m_pVertex1, true );
	m_edgeBC = DoAddEdge( m_pVertex1, m_pVertex2, true );
	m_edgeCA = DoAddEdge( m_pVertex2, m_pVertex0, true );
}

FaceEdges :: FaceEdges( Subdivider * p_pDivider, Castor3D::FaceSPtr p_face, EdgePtr l_ab, EdgePtr l_bc, EdgePtr l_ca )
	:	m_pDivider	( p_pDivider			)
	,	m_face		( p_face				)
	,	m_edgeAB	( l_ab					)
	,	m_edgeBC	( l_bc					)
	,	m_edgeCA	( l_ca					)
	,	m_bOwnFace	( false					)
	,	m_pVertex0	( l_ab->GetVertex1()	)
	,	m_pVertex1	( l_bc->GetVertex1()	)
	,	m_pVertex2	( l_ca->GetVertex1()	)
{
}

void FaceEdges :: Divide( real p_value, VertexPtrUIntMap & p_mapVertex, FaceEdgesPtrArray & p_newFaces )
{
	Point3r l_aTex, l_bTex, l_cTex, l_dTex, l_eTex, l_fTex;

	// We first retrieve the 3 face's vertices
	VertexSPtr l_a = p_mapVertex[m_face->GetVertexIndex( 0 )];
	VertexSPtr l_b = p_mapVertex[m_face->GetVertexIndex( 1 )];
	VertexSPtr l_c = p_mapVertex[m_face->GetVertexIndex( 2 )];

	// We divide the 3 edges of the face and retrieve the resulting vertices
	VertexSPtr l_d = m_edgeAB->Divide( m_pDivider, p_value );
	VertexSPtr l_e = m_edgeBC->Divide( m_pDivider, p_value );
	VertexSPtr l_f = m_edgeCA->Divide( m_pDivider, p_value );

	// We remove the 3 original edges
	DoRemoveEdge( m_edgeAB );
	DoRemoveEdge( m_edgeBC );
	DoRemoveEdge( m_edgeCA );
	m_edgeAB.reset();
	m_edgeBC.reset();
	m_edgeCA.reset();

	// We then compute the texture coordinates of the new 3 vertices
	Castor3D::Vertex::GetTexCoord( l_a->GetPoint(), l_aTex );
	Castor3D::Vertex::GetTexCoord( l_b->GetPoint(), l_bTex );
	Castor3D::Vertex::GetTexCoord( l_c->GetPoint(), l_cTex );
	l_dTex = l_aTex + (l_bTex - l_aTex) * p_value;
	l_eTex = l_bTex + (l_cTex - l_bTex) * p_value;
	l_fTex = l_cTex + (l_aTex - l_cTex) * p_value;

	// Then we add the 4 resulting faces
	DoAddFaceAndEdges( l_a, l_d, l_f, l_aTex, l_dTex, l_fTex, p_newFaces );
	DoAddFaceAndEdges( l_d, l_b, l_e, l_dTex, l_bTex, l_eTex, p_newFaces );
	DoAddFaceAndEdges( l_e, l_c, l_f, l_eTex, l_cTex, l_fTex, p_newFaces );
	DoAddFaceAndEdges( l_d, l_e, l_f, l_dTex, l_eTex, l_fTex, p_newFaces );
}

void FaceEdges :: DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c,
									 Point3r const & p_aTex, Point3r const & p_bTex, Point3r const & p_cTex,
									 FaceEdgesPtrArray & p_newFaces)
{
	// First we create the 3 edges of the face
	EdgePtr l_edgeAB = DoAddEdge( p_a, p_b,	true );
	EdgePtr l_edgeBC = DoAddEdge( p_b, p_c,	true );
	EdgePtr l_edgeCA = DoAddEdge( p_c, p_a,	true );
	// Then we add it
	DoAddFaceAndEdges( p_a, p_b, p_c, p_aTex, p_bTex, p_cTex, l_edgeAB, l_edgeBC, l_edgeCA, p_newFaces );
}

void FaceEdges :: DoAddFaceAndEdges( VertexSPtr p_a, VertexSPtr p_b, VertexSPtr p_c,
									 Point3r const & p_aTex, Point3r const & p_bTex, Point3r const & p_cTex,
									 EdgePtr p_edgeAB, EdgePtr p_edgeBC, EdgePtr p_edgeCA,
									 FaceEdgesPtrArray & p_newFaces)
{
	Castor3D::FaceSPtr l_pFace = m_pDivider->AddFace( p_a->GetIndex(), p_b->GetIndex(), p_c->GetIndex() );
	Castor3D::Vertex::SetTexCoord( p_a->GetPoint(), p_aTex );
	Castor3D::Vertex::SetTexCoord( p_b->GetPoint(), p_bTex );
	Castor3D::Vertex::SetTexCoord( p_c->GetPoint(), p_cTex );
	p_newFaces.push_back( std::make_shared< FaceEdges >( m_pDivider, l_pFace, p_edgeAB, p_edgeBC, p_edgeCA ) );
}

EdgePtr FaceEdges :: DoAddEdge( VertexSPtr p_v1, VertexSPtr p_v2, bool p_toDivide)
{
	// First we check if we have the edge v1->v2
	EdgePtr l_pReturn = p_v1->GetEdge( p_v2->GetIndex() );
	bool l_bCreated = false;

	if( !l_pReturn )
	{
		// We haven't it, we check if we have the edge v2->v1
		l_pReturn = p_v2->GetEdge( p_v1->GetIndex() );

		if( !l_pReturn )
		{
			// We haven't it, so we create the edge
			l_pReturn = std::make_shared< Edge >( p_v1, p_v2, m_face, p_toDivide );
			l_bCreated = true;
		}

		// We tell v1 it has an edge with v2
		p_v1->AddEdge( l_pReturn, p_v2->GetIndex() );
	}

	if( !p_v2->HasEdge( p_v1->GetIndex() ) )
	{
		// We tell v2 it has an edge with v1
		p_v2->AddEdge( l_pReturn, p_v1->GetIndex() );
	}

	if( !l_bCreated )
	{
		// We add the face to the edge's faces
		l_pReturn->AddFace( m_face );
	}

	return l_pReturn;
}

void FaceEdges :: DoRemoveEdge( EdgePtr p_edge )
{
	// We retrieve the two vertices of the edge
	VertexSPtr l_v1 = p_edge->GetVertex1();
	VertexSPtr l_v2 = p_edge->GetVertex2();
	// We tell each vertex it doesn't have anymore an edge with the other one
	l_v1->RemoveEdge( l_v2->GetIndex() );
	l_v2->RemoveEdge( l_v1->GetIndex() );
}

//*********************************************************************************************

Vertex :: Vertex( Castor3D::BufferElementGroupSPtr p_ptPoint )
	:	m_ptPoint	( p_ptPoint	)
{
}

Vertex :: ~Vertex()
{
	m_mapEdges.clear();
}

bool Vertex :: HasEdge( uint32_t p_uiIndex )
{
	return m_mapEdges.find( p_uiIndex ) != m_mapEdges.end();
}

EdgePtr Vertex :: GetEdge( uint32_t p_uiIndex )
{
	EdgePtr l_pReturn;
	EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex );

	if( l_it != m_mapEdges.end() )
	{
		l_pReturn = l_it->second;
	}

	return l_pReturn;
}

void Vertex :: AddEdge( EdgePtr p_pEdge, uint32_t p_uiIndex)
{
	EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex );

	if( l_it == m_mapEdges.end() )
	{
		m_mapEdges.insert( std::make_pair( p_uiIndex, p_pEdge ) );
	}
}

void Vertex :: RemoveEdge( uint32_t p_uiIndex)
{
	EdgePtrUIntMap::iterator l_it = m_mapEdges.find( p_uiIndex );

	if( l_it != m_mapEdges.end() )
	{
		m_mapEdges.erase( l_it );
	}
}

//*********************************************************************************************

Subdivider :: Subdivider()
	:	Castor3D::Subdivider()
{
}

Subdivider :: ~Subdivider()
{
	Cleanup();
}

void Subdivider :: DoInitialise()
{
	Castor3D::Subdivider::DoInitialise();

	for( uint32_t i = 0; i < GetNbPoints(); i++ )
	{
		m_mapVertex.insert( std::make_pair( i, std::make_shared< Vertex >( GetPoint( i ) ) ) );
	}

	for( uint32_t j = 0; j < m_submesh->GetFacesCount(); j++ )
	{
		m_facesEdges.push_back( std::make_shared< FaceEdges >( this, m_submesh->GetFace( j ), m_mapVertex ) );
	}

	m_submesh->ClearFaces();
}

void Subdivider :: Cleanup()
{
	Castor3D::Subdivider::Cleanup();

	m_facesEdges.clear();
	m_mapVertex.clear();
}

VertexSPtr Subdivider :: AddPoint( real x, real y, real z )
{
	VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( x, y, z ) );
	m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
	return l_pReturn;
}

VertexSPtr Subdivider :: AddPoint( Point3r const & p_v )
{
	VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
	m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
	return l_pReturn;
}

VertexSPtr Subdivider :: AddPoint( real * p_v )
{
	VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
	m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
	return l_pReturn;
}

void Subdivider :: DoSubdivide()
{
	DoDivide();
	DoAverage( m_ptDivisionCenter );
}

void Subdivider :: DoDivide()
{
	uint32_t l_size = uint32_t( m_facesEdges.size() );

	for( uint32_t i = 0; i < l_size; i++ )
	{
		FaceEdgesPtrArray l_newFaces;
		FaceEdgesPtr l_pEdges = m_facesEdges[0];
		l_pEdges->Divide( real( 0.5 ), m_mapVertex, l_newFaces );
		m_facesEdges.erase( m_facesEdges.begin() );
		m_facesEdges.insert( m_facesEdges.end(), l_newFaces.begin(), l_newFaces.end() );
	}
	

	for( VertexPtrUIntMap::iterator l_itVertex = m_mapVertex.begin(); l_itVertex != m_mapVertex.end(); ++l_itVertex )
	{
		VertexSPtr l_pVertex = l_itVertex->second;
		Coords3r l_ptPoint;
		Castor3D::Vertex::GetPosition( l_pVertex->GetPoint(), l_ptPoint );
	}
}

void Subdivider :: DoAverage( Point3r const & p_center )
{
	Coords3r l_ptPoint;
	Coords3r l_ptDump;

	for( VertexPtrUIntMap::iterator l_itVertex = m_mapVertex.begin(); l_itVertex != m_mapVertex.end(); ++l_itVertex )
	{
		VertexSPtr l_pVertex = l_itVertex->second;
		uint32_t l_nbEdges = l_pVertex->Size();
		Castor3D::Vertex::GetPosition( l_pVertex->GetPoint(), l_ptPoint );
		l_ptPoint -= m_ptDivisionCenter;
		l_ptPoint *= alpha( l_nbEdges );

		for( EdgePtrUIntMap::iterator l_it = l_pVertex->Begin(); l_it != l_pVertex->End(); ++l_it )
		{
			l_ptPoint += Castor3D::Vertex::GetPosition( GetPoint( l_it->first ), l_ptDump ) - m_ptDivisionCenter;
		}

		l_ptPoint /= alpha( l_nbEdges ) + l_nbEdges;
		l_ptPoint += m_ptDivisionCenter;
	}

	for( uint32_t j = 0; j < m_submesh->GetFacesCount(); j++ )
	{
		Castor3D::FaceSPtr l_pFace = m_submesh->GetFace( j );
		Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 0 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 0 )]->GetPoint(), l_ptDump ) );
		Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 1 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 1 )]->GetPoint(), l_ptDump ) );
		Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 2 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 2 )]->GetPoint(), l_ptDump ) );
	}

	m_mapVertex.clear();
}

//*********************************************************************************************

String & operator << ( String & p_stream, Vertex const & p_vertex )
{
	Coords3r l_ptDump;
	Castor3D::Vertex::GetPosition( p_vertex.GetPoint(), l_ptDump );
	p_stream << cuT( "Vertex[") << p_vertex.GetIndex() << cuT( "] - Buffer : [") << l_ptDump.const_ptr() << cuT( "] - (") << l_ptDump[0] << cuT( ",") << l_ptDump[1] << cuT( ",") << l_ptDump[2] << cuT( ")");
	return p_stream;
}

//*********************************************************************************************
