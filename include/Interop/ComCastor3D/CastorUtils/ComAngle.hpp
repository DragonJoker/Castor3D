/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_ANGLE_H__
#define __COMC3D_COM_ANGLE_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

#include <CastorUtils/Math/Angle.hpp>

namespace CastorCom
{
	COM_TYPE_TRAITS( castor, Angle );
	/*!
	\~english
	\brief		This class defines a CAngle object accessible from COM.
	\~french
	\brief		Cette classe définit un CAngle accessible depuis COM
	*/
	class CAngle
		: public CComAtlObject< Angle, castor::Angle >
	{
		COMEX_PROPERTY( Degrees, FLOAT, &m_internal, &Internal::degrees, &Internal::degrees );
		COMEX_PROPERTY( Radians, FLOAT, &m_internal, &Internal::radians, &Internal::radians );
		COMEX_PROPERTY( Grads, FLOAT, &m_internal, &Internal::grads, &Internal::grads );
		COMEX_PROPERTY( Turns, FLOAT, &m_internal, &Internal::turns, &Internal::turns );

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
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Angle ), CAngle );
}

#endif
