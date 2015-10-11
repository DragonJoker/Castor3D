/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.htm)

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
the program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
*/
#ifndef ___C3D_DEBUG_OVERLAYS_H___
#define ___C3D_DEBUG_OVERLAYS_H___

#include "Castor3DPrerequisites.hpp"

#include <PreciseTimer.hpp>

namespace Castor3D
{
	/*!
	\author 	Sylvain DOREMUS
	\date 		17/09/2015
	\version	0.8.0
	\~english
	\brief		Debug overlays class.
	\remarks	Contains all datas and methods to update and display debug overlays.
				<br />If not shown, all the methods will be deactivated.
	\~french
	\brief		Classe de gestion des incrustations de debogage.
	\remarks	Contient toutes les données et méthodes pour mettre à jour et afficher les incrustations de débogage.
				<br />Si non affichés, toutes les méthodes seront désactivées.
	*/
	class DebugOverlays
	{
	public:
		/**
		 *\~english
		 *\brief		Constructor.
		 *\~french
		 *\brief		Constructeur.
		 */
		DebugOverlays();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		~DebugOverlays();
		/**
		 *\~english
		 *\brief		Initialisation function, retrieves the overlays from the given overlay manager.
		 *\param[in]	p_manager	The overlay manager.
		 *\~french
		 *\brief		Fonction d'initialisation, récupère les incrustations à partir du gestionnaire d'incrustations donné.
		 *\param[in]	p_manager	Le gestionnaire d'incrustations.
		 */
		void Initialise( OverlayManager & p_manager );
		/**
		 *\~english
		 *\brief		Clean up function.
		 *\~french
		 *\brief		Fonction de nettoyage.
		 */
		void Cleanup();
		/**
		 *\~english
		 *\brief		Resets all frame counters.
		 *\~french
		 *\brief		Réinitialise tous les compteurs d'image.
		 */
		void StartFrame();
		/**
		 *\~english
		 *\brief		Updates the overlays texts.
		 *\param[in]	p_vertices	The vertex count.
		 *\param[in]	p_faces		The face count.
		 *\param[in]	p_objects	The object count.
		 *\~french
		 *\brief		Met à jour les textes des incrustations de débogage.
		 *\param[in]	p_vertices	Le nombre de sommets.
		 *\param[in]	p_faces		Le nombre de faces.
		 *\param[in]	p_objects	Le nombre d'objets.
		 */
		void EndFrame( uint32_t p_vertices, uint32_t p_faces, uint32_t p_objects );
		/**
		 *\~english
		 *\brief		Used to add to the GPU time, the time elapsed between now and the last call of either EndGpuTask or EndCpuTask
		 *\~french
		 *\brief		Utilisé pour ajouter au temps GPU le temps écoulé entre maintenant et le dernier appel de EndGpuTask ou EndCpuTask
		 */
		void EndGpuTask();
		/**
		 *\~english
		 *\brief		Used to add to the GPU time, the time elapsed between now and the last call of either EndGpuTask or EndCpuTask
		 *\~french
		 *\brief		Utilisé pour ajouter au temps GPU le temps écoulé entre maintenant et le dernier appel de EndGpuTask ou EndCpuTask
		 */
		void EndCpuTask();
		/**
		 *\~english
		 *\brief		Show or hide debug overlays.
		 *\param[in]	p_show	The status.
		 *\~french
		 *\brief		Affiche ou cache les incrustations de débogage.
		 *\param[in]	p_show	Le statut.
		 */
		void Show( bool p_show );

	private:
		//!\~english The tasks (CPU or GPU) timer.	\~french Le timer pour les tâches (CPU ou GPU).
		Castor::PreciseTimer m_taskTimer;
		//!\~english The frame timer.	\~french Le timer de frame.
		Castor::PreciseTimer m_frameTimer;
		//!\~english The base debug panel overlay.	\~french Le panneau d'incrustations de débogage.
		OverlayWPtr m_debugPanel;
		//!\~english The CPU time value overlay.	\~french L'incrustation contenant la valeur de temps CPU.
		TextOverlayWPtr m_debugCpuTime;
		//!\~english The GPU time value overlay.	\~french L'incrustation contenant la valeur de temps GPU.
		TextOverlayWPtr m_debugGpuTime;
		//!\~english The total time value overlay.	\~french L'incrustation contenant la valeur de temps total.
		TextOverlayWPtr m_debugTotalTime;
		//!\~english The average FPS value overlay.	\~french L'incrustation contenant la valeur du nombre moyen d'images par secondes.
		TextOverlayWPtr m_debugAverageFps;
		//!\~english The vertex count value overlay.	\~french L'incrustation contenant la valeur du nombre de sommets.
		TextOverlayWPtr m_debugVertexCount;
		//!\~english The face count value overlay.	\~french L'incrustation contenant la valeur du nombre de faces.
		TextOverlayWPtr m_debugFaceCount;
		//!\~english The object count value overlay.	\~french L'incrustation contenant la valeur du nombre d'objets.
		TextOverlayWPtr m_debugObjectCount;
		//!\~english The times of the 100 last frames.	\~french Les temps des 100 dernières frames.
		std::array< double, 100 > m_framesTimes;
		//!\~english The current frame index in m_framesTimes.	\~french L'index de la frame courante, dans m_framesTimes.
		uint32_t m_frameIndex;
		//!\~english Defines if the debug overlays are shown.	\~french Définit si les incrustations de débogage sont affichées ou cachées.
		bool m_visible;
		//!\~english The CPU time.	\~french Le temps CPU.
		double m_cpuTime;
		//!\~english The GPU time.	\~french Le temps GPU.
		double m_gpuTime;
		//!\~english The GPU time.	\~french Le temps GPU.
		double m_totalTime;
	};
}

#endif
