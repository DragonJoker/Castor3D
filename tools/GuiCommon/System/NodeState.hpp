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
#include <CastorUtils/Miscellaneous/PreciseTimer.hpp>

namespace GuiCommon
{
	using Angles = c3d::Array< c3d::Angle, 3u >;
	static float constexpr MaxAngularSpeed = 0.2f;
	static float constexpr MaxScalarSpeed = 2.0f;
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
		NodeState( c3d::FrameListener & listener
			, c3d::SceneNodeRPtr node
			, bool camera );
		/**
		*\brief
		*	Démarre la mise à jour automatique de l'état via le frame listener.
		*/
		void start();
		/**
		*\brief
		*	Stoppe la mise à jour automatique de l'état via le frame listener.
		*/
		void stop();
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
		*	Définit la vitesse de rotation du noeud.
		*param[in] value
		*	La nouvelle valeur.
		*/
		void setAngularVelocity( c3d::Point2f const & value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de translation du noeud.
		*param[in] value
		*	La nouvelle valeur.
		*/
		void setScalarVelocity( c3d::Point3f const & value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de rotation du noeud.
		*param[in] value
		*	La nouvelle valeur.
		*/
		void addAngularVelocity( c3d::Point2f const & value )noexcept;
		/**
		*\brief
		*	Définit la vitesse de translation du noeud.
		*param[in] value
		*	La nouvelle valeur.
		*/
		void addScalarVelocity( c3d::Point3f const & value )noexcept;
		/**
		*\brief
		*	Tourne le noeud autour de l'axe X de l'angle donné.
		*param[in] value
		*	La valeur de l'angle.
		*/
		void pitch( c3d::Angle const & value )noexcept;
		/**
		*\brief
		*	Tourne le noeud autour de l'axe Y de l'angle donné.
		*param[in] value
		*	La valeur de l'angle.
		*/
		void yaw( c3d::Angle const & value )noexcept;

		bool isCamera()const noexcept
		{
			return m_isCamera;
		}

		float getMaxSpeed()const noexcept
		{
			return m_scalarVelocityX.range().getMax();
		}

	private:
		void doUpdate();
		bool doUpdateVelocities( c3d::Point3f & translate
		, Angles & angles );

		//! Le listener qui recevra les évènements de déplacement / rotation.
		c3d::FrameListener & m_listener;
		//! Le noeud de scène affecté par les évènements.
		c3d::SceneNodeRPtr const m_node;
		//! La position originelle du noeud.
		c3d::Point3f const m_originalPosition;
		//! L'orientation originelle du noeud.
		c3d::Quaternion const m_originalOrientation;
		//! La rotation initiale sur les axes X et Y.
		Angles const m_originalAngles;
		//! La rotation sur les axes X et Y.
		Angles m_angles;
		//! La vitesse de rotation sur l'axe X.
		c3d::RangedValue< c3d::Angle > m_angularVelocityX
		{
			0.0_degrees,
			c3d::makeRange( c3d::Angle::fromDegrees( -MaxAngularSpeed )
				, c3d::Angle::fromDegrees( MaxAngularSpeed ) )
		};
		//! La vitesse de rotation sur l'axe Y.
		c3d::RangedValue< c3d::Angle > m_angularVelocityY
		{
			0.0_degrees,
			c3d::makeRange( c3d::Angle::fromDegrees( -MaxAngularSpeed )
				, c3d::Angle::fromDegrees( MaxAngularSpeed ) )
		};
		//! La vitesse de translation sur l'axe X.
		c3d::RangedValue< float > m_scalarVelocityX
		{
			0.0f,
			c3d::makeRange( -MaxScalarSpeed, MaxScalarSpeed )
		};
		//! La vitesse de translation sur l'axe Y.
		c3d::RangedValue< float > m_scalarVelocityY
		{
			0.0f,
			c3d::makeRange( -MaxScalarSpeed, MaxScalarSpeed )
		};
		//! La vitesse de translation sur l'axe Z.
		c3d::RangedValue< float > m_scalarVelocityZ
		{
			0.0f,
			c3d::makeRange( -MaxScalarSpeed, MaxScalarSpeed )
		};
		//! Si l'état est pour le noeud d'une caméra.
		bool m_isCamera{};
		c3d::PreciseTimer m_timer;
		c3d::Mutex m_mutex;
		std::atomic_bool m_running;
	};
	using NodeStatePtr = c3d::RawUniquePtr< NodeState >;
}

#endif
