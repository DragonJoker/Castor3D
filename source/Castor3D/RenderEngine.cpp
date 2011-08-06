#include "Castor3D/PrecompiledHeader.hpp"

#include "Castor3D/RenderEngine.hpp"
#include "Castor3D/Scene.hpp"
#include "Castor3D/SceneNode.hpp"
#include "Castor3D/Face.hpp"
#include "Castor3D/Submesh.hpp"
#include "Castor3D/Geometry.hpp"
#include "Castor3D/Light.hpp"
#include "Castor3D/Material.hpp"
#include "Castor3D/Pass.hpp"
#include "Castor3D/Ray.hpp"

using namespace Castor3D;

RenderEngine :: RenderEngine( String const & p_strFileName, ScenePtr p_pScene)
	:	m_strFileName( p_strFileName)
	,	m_pScene( p_pScene)
{
}

RenderEngine :: ~RenderEngine()
{
}

bool RenderEngine :: Draw()
{
	bool l_bReturn = false;
	std::ofstream l_imageFile( m_strFileName.char_str(), std::ios_base::binary);

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
			Logger::LogMessage( cuT( "y : %i"), y);
			for (int x = 0 ; x < l_iSizeX ; ++x)
			{
				Colour l_clrColour;
				real coef = 1.0f;
				int level = 0;
				// lancer de rayon
				Ray l_viewRay( Point3r( real( x-128), real( y-128), -10000.0f), Point3r( 0, 0, 1));

				SceneNodePtrStrMap::const_iterator l_itNodesEnd = m_pScene->NodesEnd();
				LightPtrStrMap::const_iterator l_itLightsEnd = m_pScene->LightsEnd();
				Geometry * l_pNearestGeometry = nullptr;
				SubmeshPtr l_pNearestSubmesh;

				do
				{
					// recherche de l'intersection la plus proche
					real l_fDistance = 20000.0f;
					FacePtr l_pFace;

					for (SceneNodePtrStrMap::iterator l_it = m_pScene->NodesBegin() ; l_it != l_itNodesEnd; ++l_it)
					{
						l_pNearestGeometry = l_it->second->GetNearestGeometry( & l_viewRay, l_fDistance, & l_pFace, & l_pNearestSubmesh);
					}

					if (l_pNearestGeometry)
					{
//						Logger::LogMessage( cuT( "Geometry : ") + l_pNearestGeometry->GetName());
						Point3r l_vNewStart;
						l_vNewStart.copy( l_viewRay.m_ptOrigin + (l_viewRay.m_ptDirection * l_fDistance));
						// la normale au point d'intersection
						Point3r l_vNormal;
						l_vNormal.copy( l_vNewStart - l_pNearestSubmesh->GetSphere().GetCenter());
//						Point3r l_vNormal( l_pFace->m_ptFaceNormal);
						real l_fTemp = l_vNormal.dot( l_vNormal);

						if (l_fTemp == 0.0f)
						{
							break;
						}

						l_fTemp = real( 1.0) / square_root( l_fTemp);
						l_vNormal *= l_fTemp;

						MaterialPtr l_pCurrentMat = l_pNearestSubmesh->GetMaterial();

						// calcul de la valeur d'éclairement au point
						for (LightPtrStrMap::iterator l_it = m_pScene->LightsBegin() ; l_it != l_itLightsEnd; ++l_it)
						{
							LightPtr l_pCurrent = l_it->second;
							Point3r l_vDist;
							l_vDist.copy( l_pCurrent->GetParent()->GetPosition() - l_vNewStart);

							if (l_vNormal.dot( l_vDist) > 0.0f)
							{
								real l_fT = square_root( l_vDist.dot( l_vDist));

								if (l_fT > 0.0f)
								{
									Ray l_lightRay( l_vNewStart, l_vDist * (1 / l_fT));
									// calcul des ombres
									bool l_bInShadow = false;

									for (SceneNodePtrStrMap::iterator l_it = m_pScene->NodesBegin() ; ! l_bInShadow && l_it != l_itNodesEnd; ++l_it)
									{
										l_pNearestGeometry = l_it->second->GetNearestGeometry( & l_lightRay, l_fT, nullptr, nullptr);

										if (l_pNearestGeometry)
										{
											l_bInShadow = true;
										}
									}

									if ( ! l_bInShadow)
									{
										// lambert
										real lambert = (l_lightRay.m_ptDirection.dot( l_vNormal)) * coef;
										l_clrColour += l_pCurrent->GetDiffuse() * l_pCurrentMat->GetPass( 0)->GetDiffuse() * lambert;
									}
								}
							}
						}

						// on itére sur la prochaine reflexion
						coef *= l_pCurrentMat->GetPass( 0)->GetShininess();

						real l_fReflet = 2.0f * l_viewRay.m_ptDirection.dot( l_vNormal);
						l_viewRay.m_ptOrigin.copy( l_vNewStart);
						l_viewRay.m_ptDirection.copy( l_viewRay.m_ptDirection - (l_vNormal * l_fReflet));

						level++;
					}
				}
				while ((coef > 0.0f) && (level < 10) && l_pNearestGeometry);

				l_imageFile.put( std::min<unsigned char>( static_cast<unsigned char>( l_clrColour[2]*255.0f), 255)).put( std::min<unsigned char>( static_cast<unsigned char>( l_clrColour[1]*255.0f), 255)).put( std::min<unsigned char>( static_cast<unsigned char>( l_clrColour[0]*255.0f), 255));
			}
		}

		l_bReturn = true;
	}

	return l_bReturn;
}
