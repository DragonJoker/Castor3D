#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/Projection.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Vertex.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Pattern.hpp"
#include "Castor3D/Root.hpp"
#include "Castor3D/RenderSystem.hpp"
#include "Castor3D/Buffer.hpp"



using namespace Castor3D;

Projection :: Projection()
	:	MeshCategory( eMESH_TYPE_PROJECTION)
	,	m_fDepth( 0)
	,	m_bClosed( false)
	,	m_uiNbFaces( 0)
{
}

Projection :: ~Projection()
{
	m_pPattern.reset();
}

MeshCategoryPtr Projection :: Clone()
{
	return MeshCategoryPtr( new Projection);
}

void Projection :: SetPoints( IdPoint3rPatternPtr p_pPattern, Point3r const & p_vAxis, bool p_bClosed)
{
	m_pPattern = p_pPattern;
	m_bClosed = p_bClosed;
	m_vAxis.copy( p_vAxis);
	m_vAxis.normalise();
	m_vAxis.copy( (m_vAxis * m_fDepth) / real( m_uiNbFaces));
}

void Projection :: GeneratePoints()
{
    size_t l_uiNbElem = m_pPattern->GetSize();

    if (l_uiNbElem > 0)
	{
        if (m_bClosed && m_pPattern->GetElement( 0) != m_pPattern->GetElement( l_uiNbElem - 1))
		{
            m_pPattern->AddElement( m_pPattern->GetElement( l_uiNbElem - 1), 0);
            l_uiNbElem++;
		}

		real l_fTotalDistance = 0.0;
		IdPoint3r l_ptDiff;

		for (size_t i = 1 ; i < l_uiNbElem ; i++)
		{
			l_ptDiff = m_pPattern->GetElement( i) - m_pPattern->GetElement( i - 1);
			l_fTotalDistance += real( l_ptDiff.length_squared());
		}

		SubmeshPtr l_pSubmesh;
		
		if (m_bClosed)
		{
			l_pSubmesh = GetMesh()->CreateSubmesh( l_uiNbElem + 1);
		}
		else
		{
			l_pSubmesh = GetMesh()->CreateSubmesh( l_uiNbElem - 1);
		}

		real l_fDistanceToOrigin = 0.0;
		Point3r l_ptCurrentUV;
		Point3r l_ptPreviousUV;
		IdPoint3rPattern::PointerArray l_arrayPatterns;
		IdPoint3rPatternPtr l_pPreviousPattern = m_pPattern;
		IdPoint3rPatternPtr l_pPattern;

        // Construction des faces
		for (size_t j = 0 ; j < m_uiNbFaces ; j++)
		{
			l_pPattern.reset( new IdPoint3rPattern);

			l_ptCurrentUV[1] = 0;
			l_ptPreviousUV[1] = 0;
			l_ptCurrentUV[0] = real( j + 1) / real( m_uiNbFaces);

			for (size_t i = 0 ; i < l_uiNbElem ; i++)
			{
				IdPoint3r l_pVertex = l_pPreviousPattern->GetElement( i);
				l_pVertex += m_vAxis;
				l_pPattern->AddElement( IdPoint3r( * l_pSubmesh->AddPoint( l_pVertex)), i);
			}

			for (size_t i = 1 ; i < l_uiNbElem ; i++)
			{
				IdPoint3r l_ptV0 = l_pPreviousPattern->GetElement( i - 1);
				IdPoint3r l_ptV1 = l_pPreviousPattern->GetElement( i);
				IdPoint3r l_ptV2 = l_pPattern->GetElement( i);
				IdPoint3r l_ptV3 = l_pPattern->GetElement( i - 1);

				l_ptDiff = l_ptV1 - l_ptV0;
				l_fDistanceToOrigin += real( l_ptDiff.length_squared());
				l_ptCurrentUV[1] = l_fDistanceToOrigin / l_fTotalDistance;

				l_pSubmesh->AddQuadFace( l_ptV0.GetIndex(), l_ptV1.GetIndex(), l_ptV2.GetIndex(), l_ptV3.GetIndex(), l_uiNbElem, l_ptPreviousUV, l_ptCurrentUV);

				l_ptPreviousUV[1] = l_ptCurrentUV[1];
			}

			l_pPreviousPattern = l_pPattern;
			l_arrayPatterns.push_back( l_pPattern);
			l_ptPreviousUV[0] = l_ptCurrentUV[0];
		}

		l_pSubmesh->GenerateBuffers();
    }
}

void Projection :: Initialise( const UIntArray & p_arrayFaces, const FloatArray & p_arrayDimensions)
{
	m_uiNbFaces = p_arrayFaces[0];
	m_fDepth = p_arrayDimensions[0];
	GetMesh()->Cleanup();
	GeneratePoints();
}