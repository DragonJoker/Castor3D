/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_ANGLE_H__
#define __COMC3D_COM_ANGLE_H__

#include "ComCastor3DPrerequisites.hpp"

#include "ComAtlObject.hpp"
#include <Math/Angle.hpp>

namespace CastorCom
{
	/*!
	\author 	Sylvain DOREMUS
	\version	0.7.0
	\date		10/09/2014
	\~english
	\brief		This class defines a CAngle object accessible from COM.
	\~french
	\brief		Cette classe définit un CAngle accessible depuis COM
	*/
	class CAngle
		:	COM_ATL_OBJECT( Angle )
		,	public castor::Angle
	{
	public:
		/**
		 *\~english
		 *\brief		Default constructor.
		 *\~french
		 *\brief		Constructeur par défaut.
		 */
		CAngle();
		/**
		 *\~english
		 *\brief		Destructor.
		 *\~french
		 *\brief		Destructeur.
		 */
		virtual ~CAngle();

		COM_PROPERTY( Degrees, FLOAT, makeGetter( this, &castor::Angle::degrees ), makePutter( this, &castor::Angle::degrees ) );
		COM_PROPERTY( Radians, FLOAT, makeGetter( this, &castor::Angle::radians ), makePutter( this, &castor::Angle::radians ) );
		COM_PROPERTY( Grads, FLOAT, makeGetter( this, &castor::Angle::grads ), makePutter( this, &castor::Angle::grads ) );

		STDMETHOD( Cos )( /* [out, retval] */ float * pVal );
		STDMETHOD( Sin )( /* [out, retval] */ float * pVal );
		STDMETHOD( Tan )( /* [out, retval] */ float * pVal );
		STDMETHOD( ACos )( /* [in] */ float val );
		STDMETHOD( ASin )( /* [in] */ float val );
		STDMETHOD( ATan )( /* [in] */ float val );
		STDMETHOD( Cosh )( /* [out, retval] */ float * pVal );
		STDMETHOD( Sinh )( /* [out, retval] */ float * pVal );
		STDMETHOD( Tanh )( /* [out, retval] */ float * pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object	\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Angle ), CAngle );

	DECLARE_VARIABLE_REF_GETTER( Angle, castor, Angle );
	DECLARE_VARIABLE_REF_PUTTER( Angle, castor, Angle );
}

#endif
