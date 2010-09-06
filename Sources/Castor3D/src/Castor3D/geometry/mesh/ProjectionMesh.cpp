#include "PrecompiledHeader.h"

#include "geometry/Module_Geometry.h"

#include "geometry/mesh/ProjectionMesh.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/basic/Face.h"
#include "geometry/basic/Arc.h"
#include "main/Root.h"
#include "render_system/RenderSystem.h"
#include "render_system/MeshRenderer.h"
#include "render_system/Buffer.h"

#include "Log.h"

using namespace Castor3D;

ProjectionMesh :: ProjectionMesh( Castor3D::Arc * p_pPattern, float p_fDepth, bool p_bClosed,
								  unsigned int p_uiNbFaces, const String & p_strName)
	:	Mesh( p_strName),
		m_pPattern( p_pPattern),
		m_fDepth( p_fDepth),
		m_bClosed( p_bClosed),
		m_uiNbFaces( p_uiNbFaces)
{
	m_meshType = eProjection;
	GeneratePoints();
}

ProjectionMesh :: ~ProjectionMesh()
{
    delete m_pPattern;
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

		float l_fTotalDistance = 0.0;
		Vector3f l_vDiff;

		for (size_t i = 1 ; i < l_uiNbElem ; i++)
		{
			Vector3f l_vDiff = ((* m_pPattern->GetVertex( i)) - (* m_pPattern->GetVertex( i - 1)));
			l_fTotalDistance += l_vDiff.GetSquaredLength();
		}

		Submesh * l_pSubmesh;
		
		if (m_bClosed)
		{
			l_pSubmesh = CreateSubmesh( 3);
		}
		else
		{
			l_pSubmesh = CreateSubmesh( 1);
		}

		Face * l_pFace;
		float l_fDistanceToOrigin = 0.0;
		float l_fPreviousDistanceToOrigin = 0.0;

        // Construction des faces et du fond (l'envers de la silhouette)
        for (size_t i = 1 ; i < l_uiNbElem ; i++)
		{
			Vector3f * l_pV0 = l_pSubmesh->AddVertex( new Vector3f( * m_pPattern->GetVertex( i - 1)));
			Vector3f * l_pV1 = l_pSubmesh->AddVertex( new Vector3f( * m_pPattern->GetVertex( i)));
			Vector3f * l_pV2 = l_pSubmesh->AddVertex( l_pV0->x, l_pV0->y, l_pV0->z + m_fDepth);
			Vector3f * l_pV3 = l_pSubmesh->AddVertex( l_pV1->x, l_pV1->y, l_pV1->z + m_fDepth);

			l_vDiff = ((* m_pPattern->GetVertex( i)) - (* m_pPattern->GetVertex( i - 1)));
			l_fDistanceToOrigin += l_vDiff.GetSquaredLength();

			l_pFace = l_pSubmesh->AddFace( l_pV0, l_pV1, l_pV2, 0);
			SetTexCoordV1( l_pFace, 0.0, l_fPreviousDistanceToOrigin / l_fTotalDistance);
			SetTexCoordV2( l_pFace, 0.0, l_fDistanceToOrigin / l_fTotalDistance);
			SetTexCoordV3( l_pFace, 1.0, l_fPreviousDistanceToOrigin / l_fTotalDistance);

			l_pFace = l_pSubmesh->AddFace( l_pV2, l_pV1, l_pV3, 0);
			SetTexCoordV1( l_pFace, 1.0, l_fPreviousDistanceToOrigin / l_fTotalDistance);
			SetTexCoordV2( l_pFace, 0.0, l_fDistanceToOrigin / l_fTotalDistance);
			SetTexCoordV3( l_pFace, 1.0, l_fDistanceToOrigin / l_fTotalDistance);

			l_fPreviousDistanceToOrigin = l_fDistanceToOrigin;
		}

		l_pSubmesh->GenerateBuffers();
    }
}