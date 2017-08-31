/*
This source file is part of Castor3D (http://castor3d.developpez.com/castor3d.html)
Copyright (c) 2016 dragonjoker59@hotmail.com

Permission is hereby granted, free of charge, to any person obtaining a copy of
this software and associated documentation files (the "Software"), to deal in
the Software without restriction, including without limitation the rights to
use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
of the Software, and to permit persons to whom the Software is furnished to do
so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/
#ifndef ___GC_CameraState_HPP___
#define ___GC_CameraState_HPP___
#pragma once

#include <Castor3DPrerequisites.hpp>
#include <Math/Angle.hpp>
#include <Math/Point.hpp>
#include <Math/Quaternion.hpp>
#include <Math/RangedValue.hpp>

namespace GuiCommon
{
	using Angles = std::array< castor::Angle, 2u >;
	/**
	*\brief
	*	Classe de gestion des déplacements d'un noeud de scène.
	*/
	class NodeState
	{
	public:
		static float constexpr MaxAngularSpeed = 5.0f;
		static float constexpr MaxScalarSpeed = 20.0f;

	public:
		/**
		*\brief
		*	Définit le noeud affecté par les évènements.
		*\param[in] p_listener
		*	Le listener qui recevra les évènements.
		*\param[in] p_node
		*	Le noeud
		*/
		NodeState( castor3d::FrameListener & p_listener
			, castor3d::SceneNodeSPtr p_node );
		/**
		*\brief
		*	Réinitialise l'état.
		*/
		void Reset( float p_speed );
		/**
		*\brief
		*	Met à jour la vitesse maximale (de rotation et translation).
		*/
		void setMaxSpeed( float p_speed );
		/**
		*\brief
		*	Met à jour l'angle et le zoom en fonction des vitesses.
		*\return
		 * 	\p true s'il y a eu du mouvement.
		*/
		bool update();
		/**
		*\brief
		*	Définit la vitesse de rotation du noeud.
		*param[in] p_value
		*	La nouvelle valeur.
		*/
		void setAngularVelocity( castor::Point2r const & p_value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de translation du noeud.
		*param[in] p_value
		*	La nouvelle valeur.
		*/
		void setScalarVelocity( castor::Point3r const & p_value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de rotation du noeud.
		*param[in] p_value
		*	La nouvelle valeur.
		*/
		void addAngularVelocity( castor::Point2r const & p_value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de translation du noeud.
		*param[in] p_value
		*	La nouvelle valeur.
		*/
		void addScalarVelocity( castor::Point3r const & p_value )noexcept;

	private:
		//! Le listener qui recevra les évènements de déplacement / rotation.
		castor3d::FrameListener & m_listener;
		//! Le noeud de scène affecté par les évènements.
		castor3d::SceneNodeSPtr const m_node;
		//! La position originelle du noeud.
		castor::Point3r const m_originalPosition;
		//! L'orientation originelle du noeud.
		castor::Quaternion const m_originalOrientation;
		//! La rotation sur les axes X et Y.
		Angles m_angles;
		//! La vitesse de rotation sur l'axe X.
		castor::RangedValue< castor::Angle > m_angularVelocityX
		{
			0.0_degrees,
			castor::makeRange( castor::Angle::fromDegrees( -MaxAngularSpeed )
				, castor::Angle::fromDegrees( MaxAngularSpeed ) )
		};
		//! La vitesse de rotation sur l'axe Y.
		castor::RangedValue< castor::Angle > m_angularVelocityY
		{
			0.0_degrees,
			castor::makeRange( castor::Angle::fromDegrees( -MaxAngularSpeed )
				, castor::Angle::fromDegrees( MaxAngularSpeed ) )
		};
		//! La vitesse de translation sur l'axe X.
		castor::RangedValue< float > m_scalarVelocityX
		{
			0.0f,
			castor::makeRange( -MaxScalarSpeed, MaxScalarSpeed )
		};
		//! La vitesse de translation sur l'axe Y.
		castor::RangedValue< float > m_scalarVelocityY
		{
			0.0f,
			castor::makeRange( -MaxScalarSpeed, MaxScalarSpeed )
		};
		//! La vitesse de translation sur l'axe Z.
		castor::RangedValue< float > m_scalarVelocityZ
		{
			0.0f,
			castor::makeRange( -MaxScalarSpeed, MaxScalarSpeed )
		};
	};
	using NodeStatePtr = std::unique_ptr< NodeState >;
}

#endif
