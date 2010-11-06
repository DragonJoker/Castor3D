#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/ProjectionMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Pattern.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"



using namespace Castor3D;

ProjectionMesh :: ProjectionMesh( PatternPtr p_pPattern, const Point3r & p_vAxis, bool p_bClosed,
								  real p_fDepth, unsigned int p_uiNbFaces, const String & p_strName)
	:	Mesh( p_strName),
		m_pPattern( p_pPattern),
		m_vAxis( p_vAxis),
		m_fDepth( p_fDepth),
		m_bClosed( p_bClosed),
		m_uiNbFaces( p_uiNbFaces)
{
	m_vAxis.Normalise();
	m_vAxis = (m_vAxis * m_fDepth) / real( m_uiNbFaces);
	m_meshType = eProjection;
	GeneratePoints();
}

ProjectionMesh :: ~ProjectionMesh()
{
//    delete m_pPattern;
}

void ProjectionMesh :: GeneratePoints()
{
    size_t l_uiNbElem = m_pPattern->GetNumVertex();

    if (l_uiNbElem > 0)
	{
        if (m_bClosed && m_pPattern->GetVertex( 0) != m_pPattern->GetVertex( l_uiNbElem - 1))
		{
            m_pPattern->AddVertex( m_pPattern->GetVertex( l_uiNbElem - 1), 0);
            l_uiNbElem++;
        }

		real l_fTotalDistance = 0.0;
		Point3r l_vDiff;

		for (size_t i = 1 ; i < l_uiNbElem ; i++)
		{
			Point3r l_vDiff = ((* m_pPattern->GetVertex( i)) - (* m_pPattern->GetVertex( i - 1)));
			l_fTotalDistance += l_vDiff.GetSquaredLength();
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
		PatternPtrArray l_arrayPatterns;
		PatternPtr l_pPreviousPattern = m_pPattern;

        // Construction des faces
		for (size_t j = 0 ; j < m_uiNbFaces ; j++)
		{
			PatternPtr l_pPattern = new Pattern;

			l_ptCurrentUV[1] = 0;
			l_ptPreviousUV[1] = 0;
			l_ptCurrentUV[0] = real( j + 1) / real( m_uiNbFaces);

			for (size_t i = 0 ; i < l_uiNbElem ; i++)
			{
				VertexPtr l_pVertex = l_pPreviousPattern->GetVertex( i);
				l_pVertex->operator +=( m_vAxis);
				l_pVertex = l_pSubmesh->AddVertex( l_pVertex);
				l_pPattern->AddVertex( l_pVertex, i);
			}

			for (size_t i = 1 ; i < l_uiNbElem ; i++)
			{
				VertexPtr l_pV0 = l_pPreviousPattern->GetVertex( i - 1);
				VertexPtr l_pV1 = l_pPreviousPattern->GetVertex( i);
				VertexPtr l_pV2 = l_pPattern->GetVertex( i);
				VertexPtr l_pV3 = l_pPattern->GetVertex( i - 1);

				l_vDiff = ((* l_pV1) - (* l_pV0));
				l_fDistanceToOrigin += l_vDiff.GetSquaredLength();
				l_ptCurrentUV[1] = l_fDistanceToOrigin / l_fTotalDistance;

				l_pSubmesh->AddQuadFace( l_pV0, l_pV1, l_pV2, l_pV3, l_uiNbElem, l_ptPreviousUV, l_ptCurrentUV);

				l_ptPreviousUV[1] = l_ptCurrentUV[1];
			}

			l_pPreviousPattern = l_pPattern;
			l_arrayPatterns.push_back( l_pPattern);
			l_ptPreviousUV[0] = l_ptCurrentUV[0];
		}

//		vector::deleteAll( l_arrayPatterns);

		l_pSubmesh->GenerateBuffers();
    }
}