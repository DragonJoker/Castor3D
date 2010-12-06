#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/ProjectionMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Vertex.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Pattern.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/Buffer.h"



using namespace Castor3D;

ProjectionMesh :: ProjectionMesh( Point3rPatternPtr p_pPattern, const Point3r & p_vAxis, bool p_bClosed,
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
	Point3rPattern & l_pattern = * m_pPattern;
    size_t l_uiNbElem = l_pattern.GetSize();

    if (l_uiNbElem > 0)
	{
        if (m_bClosed && l_pattern[0] != l_pattern[l_uiNbElem - 1])
		{
            m_pPattern->AddPoint( l_pattern[l_uiNbElem - 1], 0);
            l_uiNbElem++;
		}

		real l_fTotalDistance = 0.0;
		Point3r l_ptDiff;

		for (size_t i = 1 ; i < l_uiNbElem ; i++)
		{
			l_ptDiff = (l_pattern[i] - l_pattern[i - 1]);
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
		Point3rPattern::PointerArray l_arrayPatterns;
		Point3rPattern & l_previousPattern = * m_pPattern;
		size_t l_uiTmpIndex = 0;

        // Construction des faces
		for (size_t j = 0 ; j < m_uiNbFaces ; j++)
		{
			Point3rPatternPtr l_pPattern( new Point3rPattern);
			l_pattern = * l_pPattern;

			l_ptCurrentUV[1] = 0;
			l_ptPreviousUV[1] = 0;
			l_ptCurrentUV[0] = real( j + 1) / real( m_uiNbFaces);

			for (size_t i = 0 ; i < l_uiNbElem ; i++)
			{
				Point3r & l_pVertex = l_previousPattern[i];
				l_pVertex += m_vAxis;
				l_pattern.AddPoint( l_pSubmesh->AddVertex( l_pVertex), i);
			}

			for (size_t i = 1 ; i < l_uiNbElem ; i++)
			{
				const Point3r & l_pV0 = l_previousPattern[i - 1];
				const Point3r & l_pV1 = l_previousPattern[i];
				const Point3r & l_pV2 = l_pattern[i];
				const Point3r & l_pV3 = l_pattern[i - 1];

				l_ptDiff = (l_pV1 - l_pV0);
				l_fDistanceToOrigin += l_ptDiff.GetSquaredLength();
				l_ptCurrentUV[1] = l_fDistanceToOrigin / l_fTotalDistance;

				l_pSubmesh->AddQuadFace( l_uiTmpIndex + i - 1, l_uiTmpIndex + i, l_uiTmpIndex + l_uiNbElem + i, l_uiTmpIndex + l_uiNbElem + i - 1, l_uiNbElem, l_ptPreviousUV, l_ptCurrentUV);

				l_ptPreviousUV[1] = l_ptCurrentUV[1];
			}

			l_previousPattern = l_pattern;
			l_arrayPatterns.push_back( l_pPattern);
			l_ptPreviousUV[0] = l_ptCurrentUV[0];
		}

		l_pSubmesh->GenerateBuffers();
    }
}