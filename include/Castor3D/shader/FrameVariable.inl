template <size_t Rows, size_t Columns> struct MatrixVariableTyper
{
	enum
	{
		Value = FrameVariable::eMat1
	};
};

template <> struct MatrixVariableTyper<2, 2>
{
	enum
	{
		Value = FrameVariable::eMat2x2
	};
};

template <> struct MatrixVariableTyper<2, 3>
{
	enum
	{
		Value = FrameVariable::eMat2x3
	};
};

template <> struct MatrixVariableTyper<2, 4>
{
	enum
	{
		Value = FrameVariable::eMat2x4
	};
};

template <> struct MatrixVariableTyper<3, 2>
{
	enum
	{
		Value = FrameVariable::eMat3x2
	};
};

template <> struct MatrixVariableTyper<3, 3>
{
	enum
	{
		Value = FrameVariable::eMat3x3
	};
};

template <> struct MatrixVariableTyper<3, 4>
{
	enum
	{
		Value = FrameVariable::eMat3x4
	};
};

template <> struct MatrixVariableTyper<4, 2>
{
	enum
	{
		Value = FrameVariable::eMat4x2
	};
};

template <> struct MatrixVariableTyper<4, 3>
{
	enum
	{
		Value = FrameVariable::eMat4x3
	};
};

template <> struct MatrixVariableTyper<4, 4>
{
	enum
	{
		Value = FrameVariable::eMat4x4
	};
};

template <size_t Count> struct PointVariableTyper
{
	enum
	{
		Value = FrameVariable::eVec1
	};
};

template <> struct PointVariableTyper<2>
{
	enum
	{
		Value = FrameVariable::eVec2
	};
};

template <> struct PointVariableTyper<3>
{
	enum
	{
		Value = FrameVariable::eVec3
	};
};

template <> struct PointVariableTyper<4>
{
	enum
	{
		Value = FrameVariable::eVec4
	};
};

template <typename T, size_t Rows, size_t Columns>
FrameVariable::eTYPE MatrixFrameVariable<T, Rows, Columns> :: GetType()
{
	return FrameVariable::eTYPE( MatrixVariableTyper<Rows, Columns>::Value);
}

template <typename T, size_t Count>
FrameVariable::eTYPE PointFrameVariable<T, Count> :: GetType()
{
	return FrameVariable::eTYPE( PointVariableTyper<Count>::Value);
}
