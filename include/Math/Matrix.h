#pragma once

#include "ostream.h"
#include "MathCommon.h"
#include "Vector.h"
#include "Proxy.h"
#include <utility>

struct Matrix3x3;
struct Matrix4x3;
struct Quaternion;

extern "C"
{
	void Matrix3x3_FromQuaternion(const Quaternion& q, Matrix3x3& mF);
	void Matrix4x3_FromTranslation(Matrix4x3& mF, Fix12i x, Fix12i y, Fix12i z);
	void Matrix4x3_FromRotationZ(Matrix4x3& mF, s16 angZ);
	void Matrix4x3_FromRotationY(Matrix4x3& mF, s16 angY);
	void Matrix4x3_FromRotationX(Matrix4x3& mF, s16 angX);
	void Matrix4x3_FromRotationZXYExt(Matrix4x3& mF, s16 angX, s16 angY, s16 angZ); //yxz intrinsic = zxy extrinsic
	void Matrix4x3_FromRotationXYZExt(Matrix4x3& mF, s16 angX, s16 angY, s16 angZ); //zyx intrinsic = xyz extrinsic
	void Matrix4x3_FromQuaternion(const Quaternion& q, Matrix4x3& mF);
	void Matrix4x3_ApplyInPlaceToRotationZ(Matrix4x3& mF, s16 angZ); //transforms a rotation matrix using matrix mF.
	void Matrix4x3_ApplyInPlaceToRotationY(Matrix4x3& mF, s16 angY); //does not apply a rotation matrix.
	void Matrix4x3_ApplyInPlaceToRotationX(Matrix4x3& mF, s16 angX); //don't get the two confused.
	void Matrix4x3_ApplyInPlaceToRotationZXYExt(Matrix4x3& mF, s16 angX, s16 angY, s16 angZ);
	void Matrix4x3_ApplyInPlaceToRotationXYZExt(Matrix4x3& mF, s16 angX, s16 angY, s16 angZ);
	void Matrix4x3_ApplyInPlaceToTranslation(Matrix4x3& mF, Fix12i x, Fix12i y, Fix12i z);
	void Matrix4x3_ApplyInPlaceToScale(Matrix4x3& mF, Fix12i x, Fix12i y, Fix12i z);

	void Matrix3x3_LoadIdentity(Matrix3x3& mF);
	void MulVec3Mat3x3(const Vector3& v, const Matrix3x3& m, Vector3& res);
	void MulMat3x3Mat3x3(const Matrix3x3& m1, const Matrix3x3& m0, Matrix3x3& mF); //m0 is applied to m1, so it's m0*m1=mF
	void Matrix4x3_LoadIdentity(Matrix4x3& mF);
	// long call to force gcc to actually call the off-by-one address and therefore set the mode to thumb.
	void Matrix4x3_FromScale(Matrix4x3& mF, Fix12i x, Fix12i y, Fix12i z) __attribute__((long_call, target("thumb")));
	void MulVec3Mat4x3(const Vector3& v, const Matrix4x3& m, Vector3& res);
	void MulMat4x3Mat4x3(const Matrix4x3& m1, const Matrix4x3& m0, Matrix4x3& mF); //m0 is applied to m1, so it's m0*m1=mF
	void InvMat4x3(const Matrix4x3& m0, Matrix4x3& mF);		//Loads inverse of m0 into mF

	void Matrix3x3_SetRotationX(Matrix3x3& m, Fix12i sinTheta, Fix12i cosTheta) __attribute__((long_call, target("thumb"))); //Resets m to an X rotation matrix
	void Matrix3x3_SetRotationY(Matrix3x3& m, Fix12i sinTheta, Fix12i cosTheta) __attribute__((long_call, target("thumb"))); //Resets m to a Y rotation matrix
	void Matrix3x3_SetRotationZ(Matrix3x3& m, Fix12i sinTheta, Fix12i cosTheta) __attribute__((long_call, target("thumb"))); //Resets m to a Z rotation matrix
}

struct Matrix2x2 // Matrix is column-major!
{
	Vector2 c0;
	Vector2 c1;
};

struct Matrix3x3 // Matrix is column-major!
{
	Vector3 c0, c1, c2;

	class TransposeProxy;

	template<class F>
	class Proxy
	{
		F eval;

		template<class G>
		using NewProxy = Proxy<G>;

	public:

		[[gnu::always_inline]]
		constexpr explicit Proxy(F&& eval) : eval(eval) {}

		template<bool resMayAlias> [[gnu::always_inline]]
		void Eval(Matrix3x3& res) { eval.template operator()<resMayAlias>(res); }

		[[gnu::always_inline, nodiscard]]
		auto operator()(const Matrix3x3& other) &&
		{
			return NewProxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
			{
				if constexpr (resMayAlias)
				{
					const Matrix3x3 temp = other;
					Eval<resMayAlias>(res);
					MulMat3x3Mat3x3(temp, res, res);
				}
				else
				{
					Eval<resMayAlias>(res);
					MulMat3x3Mat3x3(other, res, res);
				}
			});
		}

		template<class G> [[gnu::always_inline, nodiscard]]
		auto operator()(Proxy<G>&& other) &&
		{
			return NewProxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
			{
				const Matrix3x3 temp = std::move(other);
				Eval<resMayAlias>(res);
				MulMat3x3Mat3x3(temp, res, res);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const Vector3& v) &&
		{
			return Vector3::Proxy([this, &v]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				if constexpr (resMayAlias)
				{
					const Vector3 temp = v; // v may be altered while the second argument is calculated
					MulVec3Mat3x3(temp, std::move(*this), res);
				}
				else
					MulVec3Mat3x3(v, std::move(*this), res);
			});
		}

		template<class G> [[gnu::always_inline, nodiscard]]
		auto operator()(Vector3::Proxy<G>&& proxy) &&
		{
			return Vector3::Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				const Matrix3x3 matrix = std::move(*this);
				proxy.template Eval<resMayAlias>(res);
				MulVec3Mat3x3(res, matrix, res);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const Fix12i& s) &&
		{
			return NewProxy([this, &s]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
			{
				Eval<resMayAlias>(res);
				res *= s;
			});
		}

		template<class T> [[gnu::always_inline, nodiscard]]
		auto operator*(T&& x) &&
		{
			return std::move(*this)(std::forward<T>(x));
		}

		auto Transpose() &&
		{
			return NewProxy([this]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
			{
				Eval<resMayAlias>(res);
				res.TransposeInPlace();
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const TransposeProxy& proxy) const
		{
			return Matrix3x3::Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
			{
				Eval<resMayAlias>(res);
				res = res * Matrix3x3(proxy);
			});
		}
	};

	constexpr Matrix3x3() = default;
	constexpr Matrix3x3(const Vector3& c0, const Vector3& c1, const Vector3& c2) : c0(c0), c1(c1), c2(c2) {}

	constexpr Matrix3x3 (
		Fix12i c0x, Fix12i c1x, Fix12i c2x,
		Fix12i c0y, Fix12i c1y, Fix12i c2y,
		Fix12i c0z, Fix12i c1z, Fix12i c2z
	):
		c0(c0x, c0y, c0z),
		c1(c1x, c1y, c1z),
		c2(c2x, c2y, c2z)
	{}

	template<class F> [[gnu::always_inline]]
	Matrix3x3(Proxy<F>&& proxy) { proxy.template Eval<false>(*this); }

	template<class F> [[gnu::always_inline]]
	Matrix3x3& operator=(Proxy<F>&& proxy) & { proxy.template Eval<true>(*this); return *this; }

	static const Matrix3x3 IDENTITY;

	[[gnu::always_inline, nodiscard]]
	static auto Identity()
	{
		return Proxy([]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
		{
			Matrix3x3_LoadIdentity(res);
		});
	}

	template<class C0, class C1, class C2> [[gnu::always_inline, nodiscard]]
	static auto Temp(C0&& c0, C1&& c1, C2&& c2)
	{
		return Proxy([&]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
		{
			if constexpr (resMayAlias)
			{
				res.c0 = std::forward<C0>(c0);
				res.c1 = std::forward<C1>(c1);
				res.c2 = std::forward<C2>(c2);
			}
			else
			{
				AssureUnaliased(res.c0) = std::forward<C0>(c0);
				AssureUnaliased(res.c1) = std::forward<C1>(c1);
				AssureUnaliased(res.c2) = std::forward<C2>(c2);
			}
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator()(const Matrix3x3& other) const
	{
		return Proxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
		{
			MulMat3x3Mat3x3(other, *this, res);
		});
	}

	template<class F> [[gnu::always_inline, nodiscard]]
	auto operator()(Proxy<F>& proxy) const
	{
		return Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
		{
			if constexpr (resMayAlias)
			{
				const Matrix3x3 temp = *this;
				proxy.template Eval<resMayAlias>(res);
				MulMat3x3Mat3x3(res, temp, res);
			}
			else
			{
				proxy.template Eval<resMayAlias>(res);
				MulMat3x3Mat3x3(res, *this, res);
			}
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator()(const Vector3& v) const
	{
		return Vector3::Proxy([this, &v]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
		{
			MulVec3Mat3x3(v, *this, res);
		});
	}

	template<class F> [[gnu::always_inline, nodiscard]]
	auto operator()(Vector3::Proxy<F>&& proxy) const
	{
		return Vector3::Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
		{
			if constexpr (resMayAlias)
			{
				const Vector3 temp = std::move(proxy);
				MulVec3Mat3x3(temp, *this, res);
			}
			else
			{
				proxy.template Eval<resMayAlias>(res);
				MulVec3Mat3x3(res, *this, res);
			}
		});
	}

	template<class T> [[gnu::always_inline, nodiscard]]
	auto operator*(T&& x) const
	{
		return operator()(std::forward<T>(x));
	}

	template<class T>
	Matrix3x3& operator*=(T&& m) & { return *this = std::forward<T>(m) * *this; }

	Matrix3x3& operator*=(Fix12i s) &
	{
		c0 *= s;
		c1 *= s;
		c2 *= s;

		return *this;
	}

	[[gnu::always_inline, nodiscard]]
	auto operator()(const Fix12i& s) const
	{
		return Proxy([this, &s]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
		{
			res.c0 = c0 * s;
			res.c1 = c1 * s;
			res.c2 = c2 * s;
		});
	}

	class TransposeProxy
	{
		const Matrix3x3& original;

		void EvalImpl(Matrix3x3& res) const
		{
			void* trash; // input operands can't be clobbers

			asm(R"(
				ldmia  %3,  {r2-r10}
				stmia  %2!, {r2, r5, r8}
				stmia  %2!, {r3, r6, r9}
				stmia  %2,  {r4, r7, r10}
			)"
			: "=r" (trash), "=m" (res)
			: "0" (&res), "r" (&original)
			: "r2", "r3", "r4", "r5", "r6", "r7", "r8", "r9", "r10");
		}

	public:
		constexpr TransposeProxy(const Matrix3x3& original) : original(original) {}

		template<bool resMayAlias> [[gnu::always_inline]]
		void Eval(Matrix3x3& res) const { EvalImpl(res); }

		[[gnu::noinline, gnu::noclone]]
		friend Vector3& operator*=(Vector3& v, TransposeProxy t)
		{
			return v = Vector3::Temp(v.Dot(t.original.c0), v.Dot(t.original.c1), v.Dot(t.original.c2));
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const Vector3& v) const
		{
			return Vector3::Proxy([this, &v]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				res *= *this;
			});
		}

		template<class F> [[gnu::always_inline, nodiscard]]
		auto operator()(Vector3::Proxy<F>&& proxy) const
		{
			return Vector3::Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				proxy.template Eval<resMayAlias>(res);
				res = operator()(res);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const TransposeProxy& other) const
		{
			return Matrix3x3::Proxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
			{
				res = (other.original * this->original).Transpose();
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const Matrix3x3& m) const
		{
			return Matrix3x3::Proxy([this, &m]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
			{
				if constexpr (resMayAlias)
					res = Matrix3x3(*this) * m;
				else
				{
					EvalImpl(res);
					res = res(m);
				}
			});
		}

		template<class F> [[gnu::always_inline, nodiscard]]
		auto operator()(Proxy<F>&& proxy) const
		{
			return Matrix3x3::Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
			{
				proxy.template Eval<resMayAlias>(res);
				res = Matrix3x3(*this) * res;
			});
		}

		template<class T> [[gnu::always_inline, nodiscard]]
		auto operator*(T&& x) const { return operator()(std::forward<T>(x)); }

		[[gnu::always_inline, nodiscard]]
		auto C0() const
		{
			return Vector3::Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				res.x = original.c0.x;
				res.y = original.c1.x;
				res.z = original.c2.x;
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto C1() const
		{
			return Vector3::Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				res.x = original.c0.y;
				res.y = original.c1.y;
				res.z = original.c2.y;
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto C2() const
		{
			return Vector3::Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				res.x = original.c0.z;
				res.y = original.c1.z;
				res.z = original.c2.z;
			});
		}

		const Matrix3x3& Transpose() const { return original; }
	};

	TransposeProxy Transpose() const { return *this; }

	[[gnu::always_inline]]
	Matrix3x3(TransposeProxy proxy) { proxy.template Eval<false>(*this); }

	void TransposeInPlace() &
	{
		void* trash; // input operands can't be clobbers

		asm(R"(
			ldmib  %0,  {r1-r7}
			stmib  %0!, {r3, r6}
			stmib  %0!, {r1, r4, r7}
			stmib  %0,  {r2, r5}
		)"
		: "=r" (trash), "=m" (*this)
		: "0" (this)
		: "r1", "r2", "r3", "r4", "r5", "r6", "r7");
	}

	[[gnu::always_inline, nodiscard]]
	auto operator()(const TransposeProxy& proxy) const
	{
		return Matrix3x3::Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
		{
			if constexpr (resMayAlias)
				res = *this * Matrix3x3(proxy);
			else
			{
				proxy.template Eval<false>(res);
				res = *this * res;
			}
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto FromQuaternion(const Quaternion& quaternion)
	{
		return Proxy([&quaternion]<bool resMayAlias> [[gnu::always_inline]] (Matrix3x3& res)
		{
			Matrix3x3_FromQuaternion(quaternion, res);
		});
	}

	constexpr bool operator==(const Matrix3x3&) const = default;
};

inline constexpr Fix12i Det(const Matrix3x3& m)
{
	return m.c0.x*(m.c1.y*m.c2.z - m.c1.z*m.c2.y)
	     - m.c0.y*(m.c1.x*m.c2.z - m.c1.z*m.c2.x)
	     + m.c0.z*(m.c1.x*m.c2.y - m.c1.y*m.c2.x);
}

inline constexpr Fix12i Trace(const Matrix3x3& m)
{
	return m.c0.x + m.c1.y + m.c2.z;
}


// Actually a 4x4 matrix with (0, 0, 0, 1) as the last row
struct Matrix4x3 : private Matrix3x3 // Matrix is column-major!
{
	using Matrix3x3::c0;
	using Matrix3x3::c1;
	using Matrix3x3::c2;
	Vector3 c3;

	static const Matrix4x3 IDENTITY;

	template<class F>
	class Proxy
	{
		F eval;

		template<class G>
		using NewProxy = Proxy<G>;

	public:
		[[gnu::always_inline]]
		constexpr explicit Proxy(F&& eval) : eval(eval) {}

		template<bool resMayAlias> [[gnu::always_inline]]
		void Eval(Matrix4x3& res) { eval.template operator()<resMayAlias>(res); }

		[[gnu::always_inline, nodiscard]]
		auto Inverse() const
		{
			return NewProxy([this]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
			{
				Eval<resMayAlias>(res);
				InvMat4x3(res, res);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto RigidInverse() &&
		{
			return NewProxy([this]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
			{
				Eval<resMayAlias>(res);
				res.Linear().TransposeInPlace();
				res.c3 = -(res.Linear()(res.c3));
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto RotateZ(const s16& angZ) &&
		{
			return NewProxy([this, &angZ]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
			{
				Eval<resMayAlias>(res);
				res.RotateZ(angZ);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto RotateY(const s16& angY) &&
		{
			return NewProxy([this, &angY]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
			{
				Eval<resMayAlias>(res);
				res.RotateY(angY);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto RotateX(const s16& angX) &&
		{
			return NewProxy([this, &angX]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
			{
				Eval<resMayAlias>(res);
				res.RotateX(angX);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const Matrix4x3& other) &&
		{
			return NewProxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
			{
				if constexpr (resMayAlias)
				{
					const Matrix4x3 temp = other;
					Eval<resMayAlias>(res);
					MulMat4x3Mat4x3(temp, res, res);
				}
				else
				{
					Eval<resMayAlias>(res);
					MulMat4x3Mat4x3(other, res, res);
				}
			});
		}

		template<class G> [[gnu::always_inline, nodiscard]]
		auto operator()(Proxy<G>&& other) &&
		{
			return NewProxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
			{
				const Matrix4x3 temp = std::move(other);
				Eval<resMayAlias>(res);
				MulMat4x3Mat4x3(temp, res, res);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const Vector3& v) &&
		{
			return Vector3::Proxy([this, &v]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				if constexpr (resMayAlias)
				{
					const Vector3 temp = v; // v may be altered while the second argument is calculated
					MulVec3Mat4x3(temp, std::move(*this), res);
				}
				else
					MulVec3Mat4x3(v, std::move(*this), res);
			});
		}

		template<class G> [[gnu::always_inline, nodiscard]]
		auto operator()(Vector3::Proxy<G>&& proxy) &&
		{
			return Vector3::Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				const Matrix4x3 matrix = std::move(*this);
				proxy.template Eval<resMayAlias>(res);
				MulVec3Mat4x3(res, matrix, res);
			});
		}

		template<class T> [[gnu::always_inline, nodiscard]]
		auto operator*(T&& x) &&
		{
			return std::move(*this)(std::forward<T>(x));
		}

		[[gnu::always_inline, nodiscard]]
		auto operator()(const Fix12i& s) &&
		{
			return NewProxy([this, &s]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
			{
				Eval<resMayAlias>(res);
				res *= s;
			});
		}
	};

	constexpr Matrix4x3() = default;
	constexpr Matrix4x3(const Vector3& c0, const Vector3& c1, const Vector3& c2, const Vector3& c3):
		Matrix3x3(c0, c1, c2),
		c3(c3)
	{}

	constexpr Matrix4x3 (
		Fix12i c0x, Fix12i c1x, Fix12i c2x, Fix12i c3x,
		Fix12i c0y, Fix12i c1y, Fix12i c2y, Fix12i c3y,
		Fix12i c0z, Fix12i c1z, Fix12i c2z, Fix12i c3z
	):
		Matrix3x3 (
			c0x, c1x, c2x,
			c0y, c1y, c2y,
			c0z, c1z, c2z
		),
		c3(c3x, c3y, c3z)
	{}

	constexpr explicit Matrix4x3(const Matrix3x3& linear):
		Matrix3x3(linear),
		c3{}
	{}

	template<class F>
	constexpr explicit Matrix4x3(Matrix3x3::Proxy<F>&& proxy):
		Matrix3x3(std::move(proxy)),
		c3{}
	{}

	template<class F> [[gnu::always_inline]]
	Matrix4x3(Proxy<F>&& proxy) { proxy.template Eval<false>(*this); }

	template<class F> [[gnu::always_inline]]
	Matrix4x3& operator=(Proxy<F>&& proxy) & { proxy.template Eval<true>(*this); return *this; }

	      Matrix3x3& Linear()       { return *this; }
	const Matrix3x3& Linear() const { return *this; }

	Matrix4x3& RotateZ(s16 angZ) & { Matrix4x3_ApplyInPlaceToRotationZ(*this, angZ); return *this; }
	Matrix4x3& RotateY(s16 angY) & { Matrix4x3_ApplyInPlaceToRotationY(*this, angY); return *this; }
	Matrix4x3& RotateX(s16 angX) & { Matrix4x3_ApplyInPlaceToRotationX(*this, angX); return *this; }
	Matrix4x3& RotateZXY(s16 angX, s16 angY, s16 angZ) & { Matrix4x3_ApplyInPlaceToRotationZXYExt(*this, angX, angY, angZ); return *this; }
	Matrix4x3& RotateXYZ(s16 angX, s16 angY, s16 angZ) & { Matrix4x3_ApplyInPlaceToRotationXYZExt(*this, angX, angY, angZ); return *this; }
	Matrix4x3& Translate(Fix12i x, Fix12i y, Fix12i z) & { Matrix4x3_ApplyInPlaceToTranslation(*this, x, y, z); return *this; }
	Matrix4x3& ApplyScale(Fix12i x, Fix12i y, Fix12i z) & { Matrix4x3_ApplyInPlaceToScale(*this, x, y, z); return *this; }
	Matrix4x3& Invert() & { InvMat4x3(*this, *this); return *this; }

	Matrix4x3& RotateZXY(const Vector3_16& ang) & { Matrix4x3_ApplyInPlaceToRotationZXYExt(*this, ang.x, ang.y, ang.z); return *this; }
	Matrix4x3& RotateXYZ(const Vector3_16& ang) & { Matrix4x3_ApplyInPlaceToRotationXYZExt(*this, ang.x, ang.y, ang.z); return *this; }
	Matrix4x3& Translate(const Vector3& v) & { Matrix4x3_ApplyInPlaceToTranslation(*this, v.x, v.y, v.z); return *this; }
	Matrix4x3& ApplyScale(const Vector3& v) & { Matrix4x3_ApplyInPlaceToScale(*this, v.x, v.y, v.z); return *this; }

	[[gnu::always_inline, nodiscard]]
	auto Inverse() const
	{
		return Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			InvMat4x3(*this, res);
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto RigidInverse() const
	{
		return Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			res.Linear() = this->Linear().Transpose();
			res.c3 = -(res.Linear()(this->c3));
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator()(const Matrix4x3& other) const
	{
		return Proxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			MulMat4x3Mat4x3(other, *this, res);
		});
	}

	template<class F> [[gnu::always_inline, nodiscard]]
	auto operator()(Proxy<F>& proxy) const
	{
		return Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			if constexpr (resMayAlias)
			{
				const Matrix4x3 temp = *this;
				proxy.template Eval<resMayAlias>(res);
				MulMat4x3Mat4x3(res, temp, res);
			}
			else
			{
				proxy.template Eval<resMayAlias>(res);
				MulMat4x3Mat4x3(res, *this, res);
			}
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator()(const Vector3& v) const
	{
		return Vector3::Proxy([this, &v]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
		{
			MulVec3Mat4x3(v, *this, res);
		});
	}

	template<class F> [[gnu::always_inline, nodiscard]]
	auto operator()(Vector3::Proxy<F>&& proxy) const
	{
		return Vector3::Proxy([&proxy, this]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
		{
			if constexpr (resMayAlias)
			{
				const Vector3 temp = std::move(proxy);
				MulVec3Mat4x3(temp, *this, res);
			}
			else
			{
				proxy.template Eval<resMayAlias>(res);
				MulVec3Mat4x3(res, *this, res);
			}
		});
	}

	template<class T> [[gnu::always_inline, nodiscard]]
	auto operator*(T&& x) const
	{
		return operator()(std::forward<T>(x));
	}

	template<class T>
	Matrix4x3& operator*=(T&& m) & { return *this = std::forward<T>(m) * *this; }

	Matrix4x3& operator*=(Fix12i s) &
	{
		Linear() *= s;
		c3 *= s;
		return *this;
	}

	[[gnu::always_inline, nodiscard]]
	auto operator()(const Fix12i& s) const
	{
		return Proxy([this, &s]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			res.c0 = c0 * s;
			res.c1 = c1 * s;
			res.c2 = c2 * s;
			res.c3 = c3 * s;
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto Identity()
	{
		return Proxy([]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_LoadIdentity(res);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto Translation(const Fix12i& x, const Fix12i& y, const Fix12i& z)
	{
		return Proxy([&x, &y, &z]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromTranslation(res, x, y, z);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto Translation(const Vector3& translation)
	{
		return Proxy([&translation]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromTranslation(res, translation.x, translation.y, translation.z);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto Scale(const Fix12i& x, const Fix12i& y, const Fix12i& z)
	{
		return Proxy([&x, &y, &z]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromScale(res, x, y, z);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto Scale(const Vector3& scale)
	{
		return Proxy([&scale]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromScale(res, scale.x, scale.y, scale.z);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto Scale(const Fix12i& scale)
	{
		return Proxy([&scale]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromScale(res, scale, scale, scale);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto RotationX(const s16& angX)
	{
		return Proxy([&angX]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromRotationX(res, angX);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto RotationY(const s16& angY)
	{
		return Proxy([&angY]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromRotationY(res, angY);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto RotationZ(const s16& angZ)
	{
		return Proxy([&angZ]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromRotationZ(res, angZ);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto RotationZXY(const s16& angX, const s16& angY, const s16& angZ)
	{
		return Proxy([&angX, &angY, &angZ]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromRotationZXYExt(res, angX, angY, angZ);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto RotationXYZ(const s16& angX, const s16& angY, const s16& angZ)
	{
		return Proxy([&angX, &angY, &angZ]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromRotationXYZExt(res, angX, angY, angZ);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto RotationZXY(const Vector3_16& angle)
	{
		return Proxy([&angle]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromRotationZXYExt(res, angle.x, angle.y, angle.z);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto RotationXYZ(const Vector3_16& angle)
	{
		return Proxy([&angle]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromRotationXYZExt(res, angle.x, angle.y, angle.z);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto FromQuaternion(const Quaternion& quaternion)
	{
		return Proxy([&quaternion]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			Matrix4x3_FromQuaternion(quaternion, res);
		});
	}

	template<class C0, class C1, class C2, class C3> [[gnu::always_inline, nodiscard]]
	static auto Temp(C0&& c0, C1&& c1, C2&& c2, C3&& c3)
	{
		return Proxy([&]<bool resMayAlias> [[gnu::always_inline]] (Matrix4x3& res)
		{
			if constexpr (resMayAlias)
			{
				res.c0 = std::forward<C0>(c0);
				res.c1 = std::forward<C1>(c1);
				res.c2 = std::forward<C2>(c2);
				res.c3 = std::forward<C3>(c3);
			}
			else
			{
				AssureUnaliased(res.c0) = std::forward<C0>(c0);
				AssureUnaliased(res.c1) = std::forward<C1>(c1);
				AssureUnaliased(res.c2) = std::forward<C2>(c2);
				AssureUnaliased(res.c3) = std::forward<C3>(c3);	
			}
		});
	}

	constexpr bool operator==(const Matrix4x3&) const = default;
};

[[gnu::always_inline]] inline auto operator*(const Fix12i& scalar, const Matrix3x3& m) { return m * scalar; }
[[gnu::always_inline]] inline auto operator*(const Fix12i& scalar, const Matrix4x3& m) { return m * scalar; }

template<class F> [[gnu::always_inline]]
inline auto operator*(const Fix12i& scalar, Matrix3x3::Proxy<F>&& proxy) { return std::move(proxy) * scalar; }

template<class F> [[gnu::always_inline]]
inline auto operator*(const Fix12i& scalar, Matrix4x3::Proxy<F>&& proxy) { return std::move(proxy) * scalar; }

extern Matrix4x3 MATRIX_SCRATCH_PAPER;

inline const ostream& operator<<(const ostream& os, const Matrix3x3& m)
{
	os.set_buffer("[ +0x%r0%_f +0x%r1%_f +0x%r2%_f ]\n");

	for (auto r : {&Vector3::x, &Vector3::y, &Vector3::z})
	{
		os.flush(
			os.update_sign((m.c0.*r).val, 2),
			os.update_sign((m.c1.*r).val, 12),
			os.update_sign((m.c2.*r).val, 22)
		);
	}

	return os;
}

inline const ostream& operator<<(const ostream& os, const Matrix4x3& m)
{
	os.set_buffer("[ +0x%r0%_f +0x%r1%_f +0x%r2%_f +0x%r3%_f ]\n");

	for (auto r : {&Vector3::x, &Vector3::y, &Vector3::z})
	{
		os.flush(
			os.update_sign((m.c0.*r).val, 2),
			os.update_sign((m.c1.*r).val, 12),
			os.update_sign((m.c2.*r).val, 22),
			os.update_sign((m.c3.*r).val, 32)
		);
	}

	return os;
}
