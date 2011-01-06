template <typename T, size_t Rows, size_t Columns>
Castor3D::FrameVariable::eTYPE Castor3D::MatrixFrameVariable<T, Rows, Columns> :: GetType()
{
	return Castor3D::FrameVariable::eTYPE( Castor3D::MatrixVariableTyper<Rows, Columns>::Value);
}

template <size_t Rows, size_t Columns> struct Castor3D::MatrixVariableTyper<Rows, Columns>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat1
	};
};

template <> struct Castor3D::MatrixVariableTyper<2, 2>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat2x2
	};
};

template <> struct Castor3D::MatrixVariableTyper<2, 3>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat2x3
	};
};

template <> struct Castor3D::MatrixVariableTyper<2, 4>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat2x4
	};
};

template <> struct Castor3D::MatrixVariableTyper<3, 2>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat3x2
	};
};

template <> struct Castor3D::MatrixVariableTyper<3, 3>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat3x3
	};
};

template <> struct Castor3D::MatrixVariableTyper<3, 4>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat3x4
	};
};

template <> struct Castor3D::MatrixVariableTyper<4, 2>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat4x2
	};
};

template <> struct Castor3D::MatrixVariableTyper<4, 3>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat4x3
	};
};

template <> struct Castor3D::MatrixVariableTyper<4, 4>
{
	enum
	{
		Value = Castor3D::FrameVariable::eMat4x4
	};
};

template <typename T, size_t Count>
Castor3D::FrameVariable::eTYPE Castor3D::PointFrameVariable<T, Count> :: GetType()
{
	return Castor3D::FrameVariable::eTYPE( Castor3D::PointVariableTyper<Count>::Value);
}

template <size_t Count> struct Castor3D::PointVariableTyper<Count>
{
	enum
	{
		Value = Castor3D::FrameVariable::eVec1
	};
};

template <> struct Castor3D::PointVariableTyper<2>
{
	enum
	{
		Value = Castor3D::FrameVariable::eVec2
	};
};

template <> struct Castor3D::PointVariableTyper<3>
{
	enum
	{
		Value = Castor3D::FrameVariable::eVec3
	};
};

template <> struct Castor3D::PointVariableTyper<4>
{
	enum
	{
		Value = Castor3D::FrameVariable::eVec4
	};
};