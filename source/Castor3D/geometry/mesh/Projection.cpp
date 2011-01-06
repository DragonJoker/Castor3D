#include "Castor3D/PrecompiledHeader.h"

#include "Castor3D/geometry/mesh/Projection.h"
#include "Castor3D/geometry/mesh/Submesh.h"
#include "Castor3D/geometry/basic/Vertex.h"
#include "Castor3D/geometry/basic/Face.h"
#include "Castor3D/geometry/basic/Pattern.h"
#include "Castor3D/main/Root.h"
#include "Castor3D/render_system/RenderSystem.h"
#include "Castor3D/render_system/Buffer.h"



using namespace Castor3D;

Projection :: Projection( MeshManager * p_pManager, IdPoint3rPatternPtr p_pPattern, const Point3r & p_vAxis, bool p_bClosed,
								  real p_fDepth, unsigned int p_uiNbFaces, const String & p_strName)
	:	Mesh( p_pManager, p_strName)
	,	m_pPattern( p_pPattern)
	,	m_vAxis( p_vAxis)
	,	m_fDepth( p_fDepth)
	,	m_bClosed( p_bClosed)
	,	m_uiNbFaces( p_uiNbFaces)
{
	m_vAxis.Normalise();
	m_vAxis = (m_vAxis * m_fDepth) / real( m_uiNbFaces);
	m_meshType = eProjection;
	GeneratePoints();
}

Projection :: ~Projection()
{
//    delete m_pPattern;
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
			l_fTotalDistance += l_ptDiff.GetSquaredLength();
		}

		SubmeshPtr l_pSubmesh;
		
		if (m_bClosed)
		{
			l_pSubmesh = CreateSubmesh( l_uiNbElem + 1);
		}
		else
		{
			l_pSubmesh = CreateSubmesh( l_uiNbElem - 1);
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
				l_fDistanceToOrigin += l_ptDiff.GetSquaredLength();
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