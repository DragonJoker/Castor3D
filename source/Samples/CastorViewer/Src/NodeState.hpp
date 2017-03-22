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
#ifndef ___CV_CameraState_HPP___
#define ___CV_CameraState_HPP___
#pragma once

#include <Castor3DPrerequisites.hpp>
#include <Math/Angle.hpp>
#include <Math/Point.hpp>
#include <Math/Quaternion.hpp>
#include <Math/RangedValue.hpp>

namespace CastorViewer
{
	using Angles = std::array< Castor::Angle, 2u >;
	/**
	*\brief
	*	Classe de gestion des déplacements d'un noeud de scène.
	*/
	class NodeState
	{
	public:
		NodeState( Castor3D::FrameListener & p_listener );
		/**
		*\brief
		*	Définit le noeud affecté par les évènements.
		*\param[in] p_node
		*	Le noeud
		*/
		void SetNode( Castor3D::SceneNodeSPtr p_node );
		/**
		*\brief
		*	Réinitialise l'état.
		*/
		void Reset( float p_speed );
		/**
		*\brief
		*	Met à jour l'angle et le zoom en fonction des vitesses.
		*	
		*/
		void Update();
		/**
		*\brief
		*	Définit la vitesse de rotation du noeud.
		*param[in] p_value
		*	La nouvelle valeur.
		*/
		void SetAngularVelocity( Castor::Point2r const & p_value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de translation du noeud.
		*param[in] p_value
		*	La nouvelle valeur.
		*/
		void SetScalarVelocity( Castor::Point3r const & p_value )noexcept;

	private:
		/**
		*\brief
		*	Réinitialise l'état.
		*/
		void DoReset();

	private:
		//! Le listener qui recevra les évènements de déplacement / rotation.
		Castor3D::FrameListener & m_listener;
		//! Le noeud de scène affecté par les évènements.
		Castor3D::SceneNodeSPtr m_node;
		//! La position originelle du noeud.
		Castor::Point3r m_originalPosition;
		//! L'orientation originelle du noeud.
		Castor::Quaternion m_originalOrientation;
		//! La rotation sur les axes X et Y.
		Angles m_angles;
		//! La vitesse de rotation sur l'axe X.
		Castor::RangedValue< Castor::Angle > m_angularVelocityX
		{
			0.0_degrees,
			Castor::make_range( -5.0_degrees, 5.0_degrees )
		};
		//! La vitesse de rotation sur l'axe Y.
		Castor::RangedValue< Castor::Angle > m_angularVelocityY
		{
			0.0_degrees,
			Castor::make_range( -5.0_degrees, 5.0_degrees )
		};
		//! La translation.
		Castor::Point3r m_translate;
		//! La vitesse de translation sur l'axe X.
		Castor::RangedValue< float > m_scalarVelocityX
		{
			0.0f,
			Castor::make_range( -5.0f, 5.0f )
		};
		//! La vitesse de translation sur l'axe Y.
		Castor::RangedValue< float > m_scalarVelocityY
		{
			0.0f,
			Castor::make_range( -5.0f, 5.0f )
		};
		//! La vitesse de translation sur l'axe Z.
		Castor::RangedValue< float > m_scalarVelocityZ
		{
			0.0f,
			Castor::make_range( -5.0f, 5.0f )
		};
	};
}

#endif
