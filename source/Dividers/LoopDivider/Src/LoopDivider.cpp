#include "LoopDivider.hpp"

#include "LoopVertex.hpp"
#include "LoopFaceEdges.hpp"

#include <Point.hpp>
#include <Angle.hpp>

using namespace Castor;

namespace Loop
{
	namespace
	{
		static const int ALPHA_MAX = 20;
		static const double ALPHA_LIMIT = 0.469; /* converges to ~ 0.469 */

		real g_alpha[ALPHA_MAX] = { real( 1.13333 ), real( -0.358974 ), real( -0.333333 ), real( 0.129032 ), real( 0.945783 ), real( 2.0 ),
									real( 3.19889 ), real( 4.47885 ), real( 5.79946 ), real( 7.13634 ), real( 8.47535 ), real( 9.80865 ),
									real( 11.1322 ), real( 12.4441 ), real( 13.7439 ), real( 15.0317 ), real( 16.3082 ), real( 17.574 ),
									real( 18.83 ), real( 20.0769 )
								  };

		static const int BETA_MAX = 20;
		static const double BETA_LIMIT = 0.469; /* converges to ~ 0.469 */

		real g_beta[BETA_MAX] = {	real( 0.46875 ), real( 1.21875 ), real( 1.125 ), real( 0.96875 ), real( 0.840932 ), real( 0.75 ), real( 0.686349 ),
									real( 0.641085 ), real( 0.60813 ), real( 0.583555 ), real( 0.564816 ), real( 0.55024 ), real( 0.5387 ),
									real( 0.529419 ), real( 0.52185 ), real( 0.515601 ), real( 0.510385 ), real( 0.505987 ), real( 0.502247 ), real( 0.49904 )
								};

		real beta( uint32_t n )
		{
			real l_rTmp = real( 3.0 + 2.0 * cos( Angle::PiMult2 / n ) );
			return ( real( 40.0 ) - ( l_rTmp * l_rTmp ) ) / real( 32.0 );
		}

		real alpha( uint32_t n )
		{
			if ( n <= ALPHA_MAX )
			{
				return g_alpha[n - 1];
			}

			real l_rBeta = beta( n );
			return n * ( 1 - l_rBeta ) / l_rBeta;
		}
	}

	Subdivider::Subdivider()
		:	Castor3D::Subdivider()
	{
	}

	Subdivider::~Subdivider()
	{
		Cleanup();
	}

	void Subdivider::DoInitialise()
	{
		Castor3D::Subdivider::DoInitialise();

		for ( uint32_t i = 0; i < GetNbPoints(); i++ )
		{
			m_mapVertex.insert( std::make_pair( i, std::make_shared< Vertex >( GetPoint( i ) ) ) );
		}

		for ( uint32_t j = 0; j < m_submesh->GetFaceCount(); j++ )
		{
			m_facesEdges.push_back( std::make_shared< FaceEdges >( this, m_submesh->GetFace( j ), m_mapVertex ) );
		}

		m_submesh->ClearFaces();
	}

	void Subdivider::Cleanup()
	{
		Castor3D::Subdivider::Cleanup();
		m_facesEdges.clear();
		m_mapVertex.clear();
	}

	VertexSPtr Subdivider::AddPoint( real x, real y, real z )
	{
		VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( x, y, z ) );
		m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
		return l_pReturn;
	}

	VertexSPtr Subdivider::AddPoint( Point3r const & p_v )
	{
		VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
		return l_pReturn;
	}

	VertexSPtr Subdivider::AddPoint( real * p_v )
	{
		VertexSPtr l_pReturn = std::make_shared< Vertex >( Castor3D::Subdivider::AddPoint( p_v ) );
		m_mapVertex.insert( std::make_pair( l_pReturn->GetIndex(), l_pReturn ) );
		return l_pReturn;
	}

	void Subdivider::DoSubdivide()
	{
		DoDivide();
		DoAverage( m_ptDivisionCenter );
	}

	void Subdivider::DoDivide()
	{
		uint32_t l_size = uint32_t( m_facesEdges.size() );

		for ( uint32_t i = 0; i < l_size; i++ )
		{
			FaceEdgesPtrArray l_newFaces;
			FaceEdgesPtr l_pEdges = m_facesEdges[0];
			l_pEdges->Divide( real( 0.5 ), m_mapVertex, l_newFaces );
			m_facesEdges.erase( m_facesEdges.begin() );
			m_facesEdges.insert( m_facesEdges.end(), l_newFaces.begin(), l_newFaces.end() );
		}

		for ( VertexPtrUIntMap::iterator l_itVertex = m_mapVertex.begin(); l_itVertex != m_mapVertex.end(); ++l_itVertex )
		{
			VertexSPtr l_pVertex = l_itVertex->second;
			Coords3r l_ptPoint;
			Castor3D::Vertex::GetPosition( l_pVertex->GetPoint(), l_ptPoint );
		}
	}

	void Subdivider::DoAverage( Point3r const & p_center )
	{
		Coords3r l_ptPoint;
		Coords3r l_ptDump;

		for ( VertexPtrUIntMap::iterator l_itVertex = m_mapVertex.begin(); l_itVertex != m_mapVertex.end(); ++l_itVertex )
		{
			VertexSPtr l_pVertex = l_itVertex->second;
			uint32_t l_nbEdges = l_pVertex->Size();
			Castor3D::Vertex::GetPosition( l_pVertex->GetPoint(), l_ptPoint );
			l_ptPoint -= m_ptDivisionCenter;
			l_ptPoint *= alpha( l_nbEdges );

			for ( EdgePtrUIntMap::iterator l_it = l_pVertex->Begin(); l_it != l_pVertex->End(); ++l_it )
			{
				l_ptPoint += Castor3D::Vertex::GetPosition( GetPoint( l_it->first ), l_ptDump ) - m_ptDivisionCenter;
			}

			l_ptPoint /= alpha( l_nbEdges ) + l_nbEdges;
			l_ptPoint += m_ptDivisionCenter;
		}

		for ( uint32_t j = 0; j < m_submesh->GetFaceCount(); j++ )
		{
			Castor3D::FaceSPtr l_pFace = m_submesh->GetFace( j );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 0 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 0 )]->GetPoint(), l_ptDump ) );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 1 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 1 )]->GetPoint(), l_ptDump ) );
			Castor3D::Vertex::SetPosition( m_submesh->GetPoint( l_pFace->GetVertexIndex( 2 ) ), Castor3D::Vertex::GetPosition( m_mapVertex[l_pFace->GetVertexIndex( 2 )]->GetPoint(), l_ptDump ) );
		}

		m_mapVertex.clear();
	}
}
