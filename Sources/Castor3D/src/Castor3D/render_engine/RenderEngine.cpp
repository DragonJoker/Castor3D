#include "PrecompiledHeader.h"

#include "render_engine/Module_RenderEngine.h"

#include "scene/Scene.h"
#include "Log.h"
#include "scene/SceneNode.h"
#include "geometry/basic/Face.h"
#include "geometry/mesh/Submesh.h"
#include "geometry/primitives/Geometry.h"
#include "light/Light.h"
#include "material/Material.h"
#include "material/Pass.h"
#include "camera/Ray.h"

#include "render_engine/RenderEngine.h"

#include <fstream>

#include <CastorUtils/Vector3f.h>

using namespace General;
using namespace General::Math;
using namespace Castor3D;

RenderEngine :: RenderEngine( const String & p_strFileName, Scene * p_pScene)
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

	if (l_imageFile)
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
			Log::LogMessage( "y : %i", y);
			for (int x = 0 ; x < l_iSizeX ; ++x)
			{
				float red = 0, green = 0, blue = 0;
				float coef = 1.0f;
				int level = 0; 
				// lancer de rayon 
				Ray l_viewRay( Vector3f( float( x-128), float( y-128), -10000.0f), Vector3f( 0, 0, 1));

				SceneNodeStrMap::const_iterator l_itNodesEnd = m_pScene->GetNodesEnd();
				LightStrMap::const_iterator l_itLightsEnd = m_pScene->GetLightsEnd();
				Geometry * l_pNearestGeometry = NULL;
				Submesh * l_pNearestSubmesh = NULL;

				do 
				{ 
					// recherche de l'intersection la plus proche
					float l_fDistance = 20000.0f;
					Face * l_pFace;

					for (SceneNodeStrMap::iterator l_it = m_pScene->GetNodesIterator() ; l_it != l_itNodesEnd; ++l_it)
					{ 
						l_pNearestGeometry = l_it->second->GetNearestGeometry( & l_viewRay, l_fDistance, & l_pFace, & l_pNearestSubmesh);
					}

					if (l_pNearestGeometry != NULL)
					{
//						Log::LogMessage( C3D_T( "Geometry : ") + l_pNearestGeometry->GetName());
						Vector3f l_vNewStart = l_viewRay.m_origin->operator +( l_viewRay.m_direction->operator *( l_fDistance)); 
						// la normale au point d'intersection 
						Vector3f l_vNormal = l_vNewStart - l_pNearestSubmesh->GetSphere()->GetCenter();
//						Vector3f l_vNormal( l_pFace->m_faceNormal);
						float l_fTemp = l_vNormal.dotProduct( l_vNormal);

						if (l_fTemp == 0.0f)
						{
							break;
						}

						l_fTemp = 1.0f / sqrtf( l_fTemp); 
						l_vNormal = l_vNormal * l_fTemp; 

						Material * l_pCurrentMat = l_pNearestSubmesh->GetMaterial(); 

						// calcul de la valeur d'�clairement au point 
						for (LightStrMap::iterator l_it = m_pScene->GetLightsIterator() ; l_it != l_itLightsEnd; ++l_it)
						{
							Light * l_pCurrent = l_it->second;
							Vector3f l_vDist = Vector3f( l_pCurrent->GetPosition()) - l_vNewStart;

							if (l_vNormal.dotProduct( l_vDist) > 0.0f)
							{
								float l_fT = sqrtf( l_vDist.dotProduct( l_vDist));

								if (l_fT > 0.0f)
								{
									Ray l_lightRay( l_vNewStart, l_vDist * (1 / l_fT));
									// calcul des ombres 
									bool l_bInShadow = false; 

									for (SceneNodeStrMap::iterator l_it = m_pScene->GetNodesIterator() ; ! l_bInShadow && l_it != l_itNodesEnd; ++l_it)
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
										float lambert = (l_lightRay.m_direction->dotProduct( l_vNormal)) * coef;
										red += lambert * l_pCurrent->GetDiffuse()[0] * l_pCurrentMat->GetPass( 0)->GetDiffuse()[0];
										green += lambert * l_pCurrent->GetDiffuse()[1] * l_pCurrentMat->GetPass( 0)->GetDiffuse()[2];
										blue += lambert * l_pCurrent->GetDiffuse()[1] * l_pCurrentMat->GetPass( 0)->GetDiffuse()[2];
									}
								}
							}
						}

						// on it�re sur la prochaine reflexion
						coef *= l_pCurrentMat->GetPass( 0)->GetShininess();

						float l_fReflet = 2.0f * l_viewRay.m_direction->dotProduct( l_vNormal);
						l_viewRay.m_origin->operator =( l_vNewStart);
						l_viewRay.m_direction->operator =( l_viewRay.m_direction->operator -( l_vNormal * l_fReflet));

						level++;
					}
				} 
				while ((coef > 0.0f) && (level < 10) && l_pNearestGeometry != NULL);

				l_imageFile.put((unsigned char)min(blue*255.0f,255.0f)).put((unsigned char)min(green*255.0f, 255.0f)).put((unsigned char)min(red*255.0f, 255.0f));
			}
		}

		l_bReturn = true;
	}

	return l_bReturn;
}
