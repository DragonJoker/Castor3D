#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Icosaedron.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/SmoothingGroup.hpp"
#include "Castor3D/PnTrianglesDivider.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"

using namespace Castor3D;

Icosaedron :: Icosaedron()
	:	MeshCategory( eMESH_TYPE_ICOSAEDRON)
	,	m_radius	( 0)
	,	m_nbFaces	( 0)
{
}

Icosaedron :: ~Icosaedron ()
{
}

MeshCategoryPtr Icosaedron :: Clone()
{
	return MeshCategoryPtr( new Icosaedron);
}

void Icosaedron :: GeneratePoints()
{
	if (m_radius < 0)
	{
		m_radius = -m_radius;
	}

	SubmeshPtr l_pSubmesh = GetMesh()->CreateSubmesh( 1);

	// Dessin de l'icosaèdre
	real phi = (1.0f + sqrt( 5.0f)) / 2.0f;
	real X = m_radius / sqrt( phi * sqrt( 5.0f));
	real Z = X * phi;

	// on crée les 12 points le composant
	l_pSubmesh->AddPoint( -X, 0, Z);
	l_pSubmesh->AddPoint( X, 0, Z);
	l_pSubmesh->AddPoint( -X, 0, -Z);
	l_pSubmesh->AddPoint( X, 0, -Z);
	l_pSubmesh->AddPoint( 0, Z, X);
	l_pSubmesh->AddPoint( 0, Z, -X);
	l_pSubmesh->AddPoint( 0, -Z, X);
	l_pSubmesh->AddPoint( 0, -Z, -X);
	l_pSubmesh->AddPoint( Z, X, 0);
	l_pSubmesh->AddPoint(-Z, X, 0);
	l_pSubmesh->AddPoint( Z, -X, 0);
	l_pSubmesh->AddPoint( -Z, -X, 0);

	// on construit toutes les faces de l'isocaèdre
	l_pSubmesh->AddFace( 1, 0, 4, 0);
	l_pSubmesh->AddFace( 0, 9, 4, 0);
	l_pSubmesh->AddFace( 4, 9, 5, 0);
	l_pSubmesh->AddFace( 8, 4, 5, 0);
	l_pSubmesh->AddFace( 1, 4, 8, 0);
	l_pSubmesh->AddFace( 10, 1, 8, 0);
	l_pSubmesh->AddFace( 8, 3, 10, 0);
	l_pSubmesh->AddFace( 5, 3, 8, 0);
	l_pSubmesh->AddFace( 5, 2, 3, 0);
	l_pSubmesh->AddFace( 2, 7, 3, 0);
	l_pSubmesh->AddFace( 10, 3, 7, 0);
	l_pSubmesh->AddFace( 7, 6, 10, 0);
	l_pSubmesh->AddFace( 11, 6, 7, 0);
	l_pSubmesh->AddFace( 6, 11, 0, 0);
	l_pSubmesh->AddFace( 0, 1, 6, 0);
	l_pSubmesh->AddFace( 6, 1, 10, 0);
	l_pSubmesh->AddFace( 9, 0, 11, 0);
	l_pSubmesh->AddFace( 9, 11, 2, 0);
	l_pSubmesh->AddFace( 5, 9, 2, 0);
	l_pSubmesh->AddFace( 7, 2, 11, 0);

	ComputeNormals( false);

	Point3r l_tmp;

	// Division des faces de l'icosèdre
	for (unsigned int i = 1 ; i < m_nbFaces ; i++)
	{
		PnTrianglesDividerPtr l_pDivider( new PnTrianglesDivider( l_pSubmesh.get()));
		l_pSubmesh->Subdivide( l_pDivider, & l_tmp, false);
	}

	l_pSubmesh->GenerateBuffers();
	ComputeNormals( false);

	Logger::LogMessage( cuT( "Icosaedre - NbFaces : %d - NbVertex : %d"), l_pSubmesh->GetNbFaces(), l_pSubmesh->GetNbPoints());
}

void Icosaedron :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
	m_nbFaces = p_arrayFaces[0];
	m_radius = p_arrayDimensions[0];
	GetMesh()->Cleanup();
	GeneratePoints();
}

void Icosaedron :: ComputeNormals( bool p_bReverted)
{
	GetMesh()->ComputeNormals( p_bReverted);
	SubmeshPtr l_pSubmesh = GetMesh()->GetSubmesh( 0);

	for (size_t i = 0 ; i < l_pSubmesh->GetNbSmoothGroups() ; i++)
	{
		SmoothingGroupPtr l_group = l_pSubmesh->GetSmoothGroup( i);

		for (size_t j = 0 ; j < l_group->GetNbFaces() ; j++)
		{
			SphericalVertex l_vsVertex1( l_group->GetFace( j)->GetVertex( 0).GetNormal());
			SphericalVertex l_vsVertex2( l_group->GetFace( j)->GetVertex( 1).GetNormal());
			SphericalVertex l_vsVertex3( l_group->GetFace( j)->GetVertex( 2).GetNormal());

			l_group->GetFace( j)->GetVertex( 0).SetTexCoord( l_vsVertex1.m_phi, l_vsVertex1.m_theta, false);
			l_group->GetFace( j)->GetVertex( 1).SetTexCoord( l_vsVertex2.m_phi, l_vsVertex2.m_theta, false);
			l_group->GetFace( j)->GetVertex( 2).SetTexCoord( l_vsVertex3.m_phi, l_vsVertex3.m_theta, false);
		}
	}
}