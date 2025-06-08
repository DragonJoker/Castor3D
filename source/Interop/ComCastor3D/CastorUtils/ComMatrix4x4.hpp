/* See LICENSE file in root folder */
#ifndef __COMC3D_COM_SQUARE_MATRIX_H__
#define __COMC3D_COM_SQUARE_MATRIX_H__

#include "ComCastor3D/ComCastor3DPrerequisites.hpp"

#include "ComCastor3D/ComAtlObject.hpp"

namespace CastorCom
{
	COM_TYPE_TRAITS( Matrix4x4, C3DMat4 );
	/*!
	\~english
	\brief		This class defines a CMatrix object accessible from COM.
	\~french
	\brief		Cette classe définit un CMatrix accessible depuis COM
	*/
	class CMatrix4x4
		: public CComAtlObjectT< Matrix4x4, C3DMat4 >
	{
	public:
		STDMETHOD( Transpose )();
		STDMETHOD( Invert )();
		STDMETHOD( SetIdentity )();
		STDMETHOD( CompMul )( /*[in]*/ IMatrix4x4 * rhs, /*[out, retval]*/ IMatrix4x4 ** pVal );
		STDMETHOD( Mul )( /*[in]*/ IMatrix4x4 * rhs, /*[out, retval]*/ IMatrix4x4 ** pVal );
	};
	//!\~english Enters the ATL object into the object map, updates the registry and creates an instance of the object
	//!\~french Ecrit l'objet ATL dans la table d'objets, met à jour le registre et crée une instance de l'objet
	OBJECT_ENTRY_AUTO( __uuidof( Matrix4x4 ), CMatrix4x4 );
}

#endif
