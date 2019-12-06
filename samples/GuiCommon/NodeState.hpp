/*
See LICENSE file in root folder
*/
#ifndef ___GC_CameraState_HPP___
#define ___GC_CameraState_HPP___
#pragma once

#include <Castor3D/Castor3DPrerequisites.hpp>
#include <CastorUtils/Math/Angle.hpp>
#include <CastorUtils/Math/Point.hpp>
#include <CastorUtils/Math/Quaternion.hpp>
#include <CastorUtils/Math/RangedValue.hpp>

namespace GuiCommon
{
	using Angles = std::array< castor::Angle, 3u >;
	static float constexpr MaxAngularSpeed = 5.0f;
	static float constexpr MaxScalarSpeed = 20.0f;
	/**
	*\brief
	*	Classe de gestion des déplacements d'un noeud de scène.
	*/
	class NodeState
	{
	public:
		/**
		*\brief
		*	Définit le noeud affecté par les évènements.
		*\param[in] listener
		*	Le listener qui recevra les évènements.
		*\param[in] node
		*	Le noeud
		*\param[in] camera
		*	Dit si l'état est pour une caméra (pour désactiver le roll)
		*/
		NodeState( castor3d::FrameListener & listener
			, castor3d::SceneNodeSPtr node
			, bool camera );
		/**
		*\brief
		*	Réinitialise l'état.
		*/
		void reset( float speed );
		/**
		*\brief
		*	Met à jour la vitesse maximale (de rotation et translation).
		*/
		void setMaxSpeed( float speed );
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
		*param[in] value
		*	La nouvelle valeur.
		*/
		void setAngularVelocity( castor::Point2r const & value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de translation du noeud.
		*param[in] value
		*	La nouvelle valeur.
		*/
		void setScalarVelocity( castor::Point3r const & value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de rotation du noeud.
		*param[in] value
		*	La nouvelle valeur.
		*/
		void addAngularVelocity( castor::Point2r const & value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de translation du noeud.
		*param[in] value
		*	La nouvelle valeur.
		*/
		void addScalarVelocity( castor::Point3r const & value )noexcept;
		/**
		*\brief
		*	Tourne le noeud autour de l'axe X de l'angle donné.
		*param[in] value
		*	La valeur de l'angle.
		*/
		void pitch( castor::Angle const & value )noexcept;
		/**
		*\brief
		*	Tourne le noeud autour de l'axe Y de l'angle donné.
		*param[in] value
		*	La valeur de l'angle.
		*/
		void yaw( castor::Angle const & value )noexcept;

	private:
		//! Le listener qui recevra les évènements de déplacement / rotation.
		castor3d::FrameListener & m_listener;
		//! Le noeud de scène affecté par les évènements.
		castor3d::SceneNodeSPtr const m_node;
		//! La position originelle du noeud.
		castor::Point3r const m_originalPosition;
		//! L'orientation originelle du noeud.
		castor::Quaternion const m_originalOrientation;
		//! La rotation initiale sur les axes X et Y.
		Angles const m_originalAngles;
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
