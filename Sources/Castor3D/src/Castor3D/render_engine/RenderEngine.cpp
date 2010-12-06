#include "PrecompiledHeader.h"

#include "render_engine/Module_RenderEngine.h"

#include "scene/Scene.h"
#include "scene/NodeBase.h"
#include "scene/Node.h"
#include "geometry/basic/Face.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/primitives/Geometry.h"
#include "light/Light.h"
#include "material/Material.h"
#include "material/Pass.h"
#include "camera/Ray.h"

#include "render_engine/RenderEngine.h"

using namespace Castor3D;

RenderEngine :: RenderEngine( const String & p_strFileName, ScenePtr p_pScene)
	:	m_strFileName( p_strFileName),
		m_pScene( p_pScene)
{
}

RenderEngine :: ~RenderEngine()
{
}

bool RenderEngine :: Draw()
{
	bool l_bReturn = false;
	std::ofstream l_imageFile( m_strFileName.c_str(), std::ios_base::binary);

	if (l_imageFile != NULL)
	{
		int l_iSizeX = 256;
		int l_iSizeY = 256;

		// Ajout du header TGA
		l_imageFile.put( 0).put( 0);
		l_imageFile.put( 2);        /* RGB non compresse */

		l_imageFile.put( 0).put( 0);
		l_imageFile.put( 0).put( 0);
		l_imageFile.put( 0);

		l_imageFile.put( 0).put( 0); /* origine X */ 
		l_imageFile.put( 0).put( 0); /* origine Y */

		l_imageFile.put( (l_iSizeX & 0x00FF)).put( (l_iSizeX & 0xFF00) / 256);
		l_imageFile.put( (l_iSizeY & 0x00FF)).put( (l_iSizeY & 0xFF00) / 256);
		l_imageFile.put( 24);       /* 24 bit bitmap */ 
		l_imageFile.put( 0); 
		// fin du header TGA

		// balayage 
		for (int y = 0 ; y < l_iSizeY ; ++y)
		{
			Logger::LogMessage( CU_T( "y : %i"), y);
			for (int x = 0 ; x < l_iSizeX ; ++x)
			{
				real red = 0, green = 0, blue = 0;
				real coef = 1.0f;
				int level = 0; 
				// lancer de rayon 
				Ray l_viewRay( Point3r( real( x-128), real( y-128), -10000.0f), Point3r( 0, 0, 1));

				GeometryNodePtrStrMap::const_iterator l_itNodesEnd = m_pScene->GetGeometryNodesEnd();
				LightPtrStrMap::const_iterator l_itLightsEnd = m_pScene->GetLightsEnd();
				Geometry * l_pNearestGeometry = NULL;
				SubmeshPtr l_pNearestSubmesh;

				do 
				{ 
					// recherche de l'intersection la plus proche
					real l_fDistance = 20000.0f;
					FacePtr l_pFace;

					for (GeometryNodePtrStrMap::iterator l_it = m_pScene->GetGeometryNodesIterator() ; l_it != l_itNodesEnd; ++l_it)
					{ 
						l_pNearestGeometry = l_it->second->GetNearestGeometry( & l_viewRay, l_fDistance, & l_pFace, & l_pNearestSubmesh);
					}

					if (l_pNearestGeometry != NULL)
					{
//						Logger::LogMessage( CU_T( "Geometry : ") + l_pNearestGeometry->GetName());
						Point3r l_vNewStart = l_viewRay.m_origin + (l_viewRay.m_direction * l_fDistance); 
						// la normale au point d'intersection 
						Point3r l_vNormal = l_vNewStart - l_pNearestSubmesh->GetSphere().GetCenter();
//						Point3r l_vNormal( l_pFace->m_faceNormal);
						real l_fTemp = l_vNormal.Dot( l_vNormal);

						if (l_fTemp == 0.0f)
						{
							break;
						}

						l_fTemp = real( 1.0) / square_root( l_fTemp); 
						l_vNormal = l_vNormal * l_fTemp; 

						MaterialPtr l_pCurrentMat = l_pNearestSubmesh->GetMaterial(); 

						// calcul de la valeur d'éclairement au point 
						for (LightPtrStrMap::iterator l_it = m_pScene->GetLightsIterator() ; l_it != l_itLightsEnd; ++l_it)
						{
							LightPtr l_pCurrent = l_it->second;
							Point3r l_vDist = l_pCurrent->GetParent()->GetPosition() - l_vNewStart;

							if (l_vNormal.Dot( l_vDist) > 0.0f)
							{
								real l_fT = square_root( l_vDist.Dot( l_vDist));

								if (l_fT > 0.0f)
								{
									Ray l_lightRay( l_vNewStart, l_vDist * (1 / l_fT));
									// calcul des ombres 
									bool l_bInShadow = false; 

									for (GeometryNodePtrStrMap::iterator l_it = m_pScene->GetGeometryNodesIterator() ; ! l_bInShadow && l_it != l_itNodesEnd; ++l_it)
									{
										l_pNearestGeometry = l_it->second->GetNearestGeometry( & l_lightRay, l_fT, NULL, NULL);

										if (l_pNearestGeometry != NULL)
										{
											l_bInShadow = true;
										}
									}

									if ( ! l_bInShadow)
									{
										// lambert
										real lambert = (l_lightRay.m_direction.Dot( l_vNormal)) * coef;
										red += lambert * l_pCurrent->GetDiffuse()[0] * l_pCurrentMat->GetPass( 0)->GetDiffuse()[0];
										green += lambert * l_pCurrent->GetDiffuse()[1] * l_pCurrentMat->GetPass( 0)->GetDiffuse()[2];
										blue += lambert * l_pCurrent->GetDiffuse()[1] * l_pCurrentMat->GetPass( 0)->GetDiffuse()[2];
									}
								}
							}
						}

						// on itére sur la prochaine reflexion
						coef *= l_pCurrentMat->GetPass( 0)->GetShininess();

						real l_fReflet = 2.0f * l_viewRay.m_direction.Dot( l_vNormal);
						l_viewRay.m_origin = l_vNewStart;
						l_viewRay.m_direction = l_viewRay.m_direction - (l_vNormal * l_fReflet);

						level++;
					}
				} 
				while ((coef > 0.0f) && (level < 10) && l_pNearestGeometry != NULL);

				l_imageFile.put( std::min<unsigned char>( unsigned char( blue*255.0f), 255)).put( std::min<unsigned char>( unsigned char( green*255.0f), 255)).put( std::min<unsigned char>( unsigned char( red*255.0f), 255));
			}
		}

		l_bReturn = true;
	}

	return l_bReturn;
}