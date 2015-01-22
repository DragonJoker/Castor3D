#include "PhongDivider.hpp"

using namespace Castor;
using namespace Castor3D;

namespace Phong
{
	namespace
	{
		Point3d Barycenter( double u, double v, Point3d const & p1, Point3d const & p2, Point3d const & p3 )
		{
			double w = 1 - u - v;
			CASTOR_ASSERT( u + v + w == 1 );
			return ( p1 * u + p2 * v + p3 * w );
		}
	}

	Patch::Patch( PlaneEquation< double > const & p_p1, PlaneEquation< double > const & p_p2, PlaneEquation< double > const & p_p3 )
		: pi( p_p1 )
		, pj( p_p2 )
		, pk( p_p3 )
	{
	}

	Subdivider::Subdivider()
		: Castor3D::Subdivider()
		, m_occurences( 1 )
	{
	}

	Subdivider::~Subdivider()
	{
		Cleanup();
	}

	void Subdivider::Cleanup()
	{
		Castor3D::Subdivider::Cleanup();
	}

	void Subdivider::Subdivide( SubmeshSPtr p_pSubmesh, int p_occurences, bool p_bGenerateBuffers, bool p_bThreaded )
	{
		m_occurences = p_occurences;
		m_submesh = p_pSubmesh;
		m_bGenerateBuffers = p_bGenerateBuffers;
		m_submesh->ComputeContainers();

		DoInitialise();
		m_bThreaded = p_bThreaded;

		if ( p_bThreaded )
		{
			m_pThread = std::make_shared< std::thread >( std::bind( &Subdivider::DoSubdivideThreaded, this ) );
		}
		else
		{
			DoSubdivide();
			DoSwapBuffers();
		}
	}

	void Subdivider::DoSubdivide()
	{
		Castor3D::FacePtrArray l_facesArray;

		for ( uint32_t i = 0; i < m_submesh->GetFaceCount(); i++ )
		{
			l_facesArray.push_back( m_submesh->GetFace( i ) );
		}

		m_submesh->ClearFaces();
		std::map< uint32_t, Castor::PlaneEquation< double > > l_posnml;
		size_t i = 0;

		for ( VertexPtrArrayConstIt l_it = m_submesh->VerticesBegin(); l_it != m_submesh->VerticesEnd(); ++l_it )
		{
			Point3r l_position, l_normal;
			Castor3D::BufferElementGroupSPtr l_point = *l_it;
			Castor3D::Vertex::GetPosition( l_point, l_position );
			Castor3D::Vertex::GetNormal( l_point, l_normal );
			l_posnml.insert( std::make_pair( i++, Castor::PlaneEquation< double >( Point3d( l_normal[0], l_normal[1], l_normal[2] ), Point3d( l_position[0], l_position[1], l_position[2] ) ) ) );
		}
		
		for ( Castor3D::FacePtrArray::iterator l_it = l_facesArray.begin(); l_it != l_facesArray.end(); ++l_it )
		{
			Castor3D::FaceSPtr l_face = *l_it;
			DoComputeFaces( 0.0, 0.0, 1.0, 1.0, m_occurences, Patch( l_posnml[l_face->GetVertexIndex( 0 )], l_posnml[l_face->GetVertexIndex( 1 )], l_posnml[l_face->GetVertexIndex( 2 )] ) );
		}

		l_facesArray.clear();
	}

	void Subdivider::DoComputeFaces( double u0, double v0, double u2, double v2, int p_occurences, Patch const & p_patch )
	{
		double u1 = ( u0 + u2 ) / 2.0;
		double v1 = ( v0 + v2 ) / 2.0;

		if ( p_occurences > 1 )
		{
			DoComputeFaces( u0, v0, u1, v1, p_occurences - 1, p_patch );
			DoComputeFaces( u0, v1, u1, v2, p_occurences - 1, p_patch );
			DoComputeFaces( u1, v0, u2, v1, p_occurences - 1, p_patch );
			DoComputeFaces( u1, v1, u0, v0, p_occurences - 1, p_patch );
		}
		else
		{
			Castor3D::BufferElementGroupSPtr l_a = DoComputePoint( double( u0 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_b = DoComputePoint( double( u2 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_c = DoComputePoint( double( u0 ), double( v2 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_d = DoComputePoint( double( u1 ), double( v0 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_e = DoComputePoint( double( u1 ), double( v1 ), p_patch );
			Castor3D::BufferElementGroupSPtr l_f = DoComputePoint( double( u0 ), double( v1 ), p_patch );
			DoSetTextCoords( *l_a, *l_b, *l_c, *l_d, *l_e, *l_f );
		}
	}

	Castor3D::BufferElementGroupSPtr Subdivider::DoComputePoint( double u, double v, Patch const & p_patch )
	{
		Point3d b = Barycenter( u, v, p_patch.pi.GetPoint(), p_patch.pj.GetPoint(), p_patch.pk.GetPoint() );
		Point3d l_pi = p_patch.pi.Project( b );
		Point3d l_pj = p_patch.pj.Project( b );
		Point3d l_pk = p_patch.pk.Project( b );
		Point3r l_point( Barycenter( u, v, l_pi, l_pj, l_pk ) );
		return DoTryAddPoint( l_point );
	}
}
