#pragma once

#include "MathCommon.h"
#include "Proxy.h"
#include "Vector.h"
#include <algorithm>

extern "C"
{
	void Quaternion_FromVector3(Quaternion& qF, const Vector3& v0, const Vector3& v1);
	void Quaternion_Normalize(Quaternion& q);
	void Quaternion_Lerp(const Quaternion& q0, const Quaternion& q1, Fix12i t, Quaternion& qF);
	void Quaternion_Multiply(const Quaternion& q0, const Quaternion& q1, Quaternion& qF);
}

inline void Quaternion_FromMatrix3x3(Quaternion& gF, const Matrix3x3& m);

struct Quaternion : private Vector3
{
	using Vector3::x;
	using Vector3::y;
	using Vector3::z;
	Fix12i w;

	[[gnu::always_inline, nodiscard]]
	auto operator+(const Quaternion& q) const
	{
		return Proxy([this, &q]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = this->Vec() + q.Vec();
			res.w = this->w + q.w;
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator-(const Quaternion& q) const
	{
		return Proxy([this, &q]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = this->Vec() - q.Vec();
			res.w = this->w - q.w;
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto Conjugate() const
	{
		return Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = -this->Vec();
			res.w = this->w;
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto Inverse() const
	{
		return Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res = this->Conjugate();
			res /= res.Dot(res);
		});
	}

	// Computes (*this * Quaternion(v) * this->Inverse()).Vec(),
	// assuming *this is a unit quaternion
	[[gnu::always_inline, nodiscard]]
	auto Rotate(const Vector3& v) const
	{
		return Vector3::Proxy([this, &v]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
		{
			const Vector3& u = this->Vec();
			const Vector3 uv = u.Cross(v);

			ToggleAliased<resMayAlias>(res) = v + ((w*uv + u.Cross(uv)) << 1);
		});
	}

	// Computes (*this * Quaternion(v) * this->Inverse()).Vec()
	[[gnu::always_inline, nodiscard]]
	auto RotateSafe(const Vector3& v) const
	{
		return Vector3::Proxy([this, &v]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
		{
			const Vector3& u = this->Vec();
			const Fix12i uu = u.Dot(u);
			const Fix12i ww = w*w;

			ToggleAliased<resMayAlias>(res)
				= (u.Dot(v) << 1)*u
				+ (ww - uu)*v
				+ (w << 1)*u.Cross(v);

			res *= 1._f / (uu + ww);
		});
	}

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
		constexpr void Eval(Quaternion& res) { eval.template operator()<resMayAlias>(res); }

		[[gnu::always_inline, nodiscard]]
		constexpr auto Vec() &&
		{
			return Vector3::Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Vector3& res)
			{
				res = Quaternion(std::move(*this)).Vec();
			});
		}

		[[gnu::always_inline, nodiscard]]
		constexpr auto operator+() &&
		{
			return NewProxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				Eval<resMayAlias>(res);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator-() &&
		{
			return NewProxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				Eval<resMayAlias>(res);
				res.Vec() = -res.Vec();
				res.w = -res.w;
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto Conjugate() &&
		{
			return NewProxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				Eval<resMayAlias>(res);
				res.Vec() = -res.Vec();
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto Inverse() &&
		{
			return NewProxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				ToggleAliased<resMayAlias>(res) = std::move(*this).Conjugate();
				res /= res.Dot(res);
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator+(const Quaternion& q) && { return q + std::move(*this); }

		template<class G> [[gnu::always_inline, nodiscard]]
		auto operator+(Proxy<G>&& other) &&
		{
			return NewProxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if (resMayAlias)
					res = Quaternion{std::move(*this)} + Quaternion{std::move(other)};
				else
				{
					Eval<resMayAlias>(res);
					res += std::move(other);
				}
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator-(const Quaternion& q) &&
		{
			return NewProxy([this, &q]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if constexpr (resMayAlias)
					res = Quaternion{std::move(*this)} - q;
				else
				{
					Eval<resMayAlias>(res);
					res -= q;
				}
			});
		}

		template<class G> [[gnu::always_inline, nodiscard]]
		auto operator-(Proxy<G>&& other) &&
		{
			return NewProxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if (resMayAlias)
					res = Quaternion{std::move(*this)} - Quaternion{std::move(other)};
				else
				{
					Eval<resMayAlias>(res);
					res -= std::move(other);
				}
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator+(const Fix12i& x) &&
		{
			return Proxy([this, &x]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if (resMayAlias)
				{
					const Fix12i temp = x;
					Eval<resMayAlias>(res);
					res.w += temp;
				}
				else
				{
					Eval<resMayAlias>(res);
					res.w += x;
				}
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator-(const Fix12i& x) &&
		{
			return Proxy([this, &x]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if (resMayAlias)
				{
					const Fix12i temp = x;
					Eval<resMayAlias>(res);
					res.w -= temp;
				}
				else
				{
					Eval<resMayAlias>(res);
					res.w -= x;
				}
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator*(const Fix12i& scalar) &&
		{
			return NewProxy([this, &scalar]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if (resMayAlias)
				{
					const Fix12i temp = scalar;
					Eval<resMayAlias>(res);
					res *= temp;
				}
				else
				{
					Eval<resMayAlias>(res);
					res *= scalar;
				}
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator/(const Fix12i& scalar) &&
		{
			return NewProxy([this, &scalar]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if (resMayAlias)
				{
					const Fix12i temp = scalar;
					Eval<resMayAlias>(res);
					res /= temp;
				}
				else
				{
					Eval<resMayAlias>(res);
					res /= scalar;
				}
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator<<(const s32& shift) &&
		{
			return NewProxy([this, &shift]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				Eval<resMayAlias>(res);
				res <<= shift;
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator>>(const s32& shift) &&
		{
			return NewProxy([this, &shift]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				Eval<resMayAlias>(res);
				res >>= shift;
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator*(const Quaternion& q) &&
		{
			return NewProxy([this, &q]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if constexpr (resMayAlias)
					Quaternion_Multiply(std::move(*this), q, res);
				else
				{
					Eval<resMayAlias>(res);
					Quaternion_Multiply(res, q, res);
				}
			});
		}

		template<class G> [[gnu::always_inline, nodiscard]]
		auto operator*(Proxy<G>&& other) &&
		{
			return NewProxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if constexpr (resMayAlias)
					Quaternion_Multiply(std::move(*this), std::move(other), res);
				else
				{
					Eval<resMayAlias>(res);
					Quaternion_Multiply(res, std::move(other), res);
				}
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto operator/(const Quaternion& q) &&
		{
			return NewProxy([this, &q]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				Quaternion temp = q.Inverse();
				Eval<resMayAlias>(res);
				Quaternion_Multiply(res, temp, res);
			});
		}

		template<class G> [[gnu::always_inline, nodiscard]]
		auto operator/(Proxy<G>&& other) &&
		{
			return NewProxy([this, &other]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				if constexpr (resMayAlias)
					Quaternion_Multiply(std::move(*this), std::move(other), res);
				else
				{
					Eval<resMayAlias>(res);
					Quaternion_Multiply(res, std::move(other), res);
				}
			});
		}

		template<class Q> [[gnu::always_inline, nodiscard]]
		constexpr bool operator==(Q&& q) &&
		{
			return Quaternion{std::move(*this)} == std::forward<Q>(q);
		}

		[[gnu::always_inline, nodiscard]]
		Fix12i Dot(const Quaternion& q) &&
		{
			return static_cast<Quaternion>(std::move(*this)).Dot(q);
		}

		[[gnu::always_inline, nodiscard]]
		auto Normalized() &&
		{
			return NewProxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
			{
				Eval<resMayAlias>(res);
				res.Normalize();
			});
		}

		[[gnu::always_inline, nodiscard]]
		auto Rotate(const Vector3& v) &&
		{
			return Quaternion{std::move(*this)}.Rotate(v);
		}

		[[gnu::always_inline, nodiscard]]
		auto RotateSafe(const Vector3& v) &&
		{
			return Quaternion{std::move(*this)}.RotateSafe(v);
		}

		[[gnu::always_inline]]
		constexpr explicit operator bool() &&
		{
			return static_cast<bool>(Quaternion(std::move(*this)));
		}
	};

	constexpr Quaternion() = default;
	constexpr Quaternion(auto x, auto y, auto z, auto w) : Vector3(x, y, z), w(w) {}
	constexpr Quaternion(Fix12i real) : Vector3(), w(real) {}
	constexpr Quaternion(int    real) : Vector3(), w(real) {}
	constexpr Quaternion(const Vector3& vec) : Vector3(vec), w() {}

	template<class V> [[gnu::always_inline]]
	constexpr Quaternion(auto real, V&& vec):
		Vector3(std::forward<V>(vec)), w(real) {}

	template<class F> [[gnu::always_inline]]
	constexpr Quaternion(Proxy<F>&& proxy) { proxy.template Eval<false>(*this); }

	[[gnu::always_inline]]
	explicit inline Quaternion(const Matrix3x3& m) { Quaternion_FromMatrix3x3(*this, m); }

	template<class V> [[gnu::always_inline, nodiscard]]
	static constexpr auto Temp(const auto& real, V&& vec)
	{
		return Proxy([&real, &vec]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = std::forward<V>(vec);
			res.w = real;
		});
	};

	[[gnu::always_inline, nodiscard]]
	static constexpr auto Temp(const auto& x, const auto& y, const auto& z, const auto& w)
	{
		return Proxy([&x, &y, &z, &w]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.x = x;
			res.y = y;
			res.z = z;
			res.w = w;
		});
	}

	template<class F> [[gnu::always_inline]]
	Quaternion& operator=(Proxy<F>&& proxy) & { proxy.template Eval<true>(*this); return *this; }

	constexpr bool operator==(const Quaternion&) const = default;

	[[gnu::always_inline, nodiscard]]
	constexpr Vector3& Vec() { return static_cast<Vector3&>(*this); }

	[[gnu::always_inline, nodiscard]]
	constexpr const Vector3& Vec() const { return static_cast<const Vector3&>(*this); }

	[[gnu::always_inline, nodiscard]]
	constexpr auto operator+() const
	{
		return Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res = *this;
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator-() const
	{
		return Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = -this->Vec();
			res.w = -this->w;
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator+(const Fix12i& x) const
	{
		return Proxy([this, &x]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.w = this->w + x;
			res.Vec() = this->Vec();
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator-(const Fix12i& x) const
	{
		return Proxy([this, &x]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.w = this->w - x;
			res.Vec() = this->Vec();
		});
	}

	Quaternion& operator+= (const Quaternion& q) & { return *this = *this + q; }
	Quaternion& operator-= (const Quaternion& q) & { return *this = *this - q; }

	Quaternion& operator+= (const Vector3& v) & { Vec() += v; return *this; }
	Quaternion& operator-= (const Vector3& v) & { Vec() -= v; return *this; }

	Quaternion& operator+= (Fix12i x) & { w += x; return *this; }
	Quaternion& operator-= (Fix12i x) & { w -= x; return *this; }

	template<class F> [[gnu::always_inline, nodiscard]]
	auto operator+(Proxy<F>&& proxy) const
	{
		return Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			if constexpr (resMayAlias)
				res = *this + Quaternion{std::move(proxy)};
			else
			{
				proxy.template Eval<resMayAlias>(res);
				res += *this;
			}
		});
	}

	template<class F> [[gnu::always_inline, nodiscard]]
	auto operator-(Proxy<F>&& proxy) const
	{
		return Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			if constexpr (resMayAlias)
			{
				res = *this - Quaternion{std::move(proxy)};
			}
			else
			{
				proxy.template Eval<resMayAlias>(res);
				res = *this - res;
			}
		});
	}

	[[gnu::always_inline]]
	friend auto operator+(const Fix12i& x, const Quaternion& q)
	{
		return q + x;
	}

	[[gnu::always_inline]]
	friend auto operator-(const Fix12i& x, const Quaternion& q)
	{
		return Proxy([&q, &x]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.w = x - q.w;
			res.Vec() = -q.Vec();
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator*(const Fix12i& scalar) const
	{
		return Proxy([this, &scalar]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = this->Vec() * scalar;
			res.w = this->w * scalar;
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator/(const Fix12i& scalar) const
	{
		return Proxy([this, &scalar]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = this->Vec() / scalar;
			res.w = this->w / scalar;
		});
	}

	Quaternion& operator*=(Fix12i scalar) & { return *this = *this * scalar; }
	Quaternion& operator/=(Fix12i scalar) & { return *this = *this / scalar; }

	[[gnu::always_inline, nodiscard]]
	auto operator<<(const s32& shift) const
	{
		return Proxy([this, &shift]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = this->Vec() << shift;
			res.w = this->w << shift;
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator>>(const s32& shift) const
	{
		return Proxy([this, &shift]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res.Vec() = this->Vec() >> shift;
			res.w = this->w >> shift;
		});
	}

	Quaternion& operator<<=(s32 shift) & { return *this = *this << shift; }
	Quaternion& operator>>=(s32 shift) & { return *this = *this >> shift; }

	[[gnu::always_inline, nodiscard]]
	auto operator*(const Quaternion& q) const
	{
		return Proxy([this, &q]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			Quaternion_Multiply(*this, q, res);
		});
	}

	[[gnu::always_inline]]
	Quaternion& operator*=(const Quaternion& q) &
	{
		Quaternion_Multiply(*this, q, *this);

		return *this;
	}

	template<class F> [[gnu::always_inline, nodiscard]]
	auto operator*(Proxy<F>&& proxy) const
	{
		return Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			if constexpr (resMayAlias)
				Quaternion_Multiply(*this, std::move(proxy), res);
			else
			{
				proxy.template Eval<resMayAlias>(res);
				Quaternion_Multiply(*this, res, res);
			}
		});
	}

	[[gnu::always_inline, nodiscard]]
	auto operator/(const Quaternion& q) const
	{
		return Proxy([this, &q]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			if constexpr (resMayAlias)
				Quaternion_Multiply(*this, q.Inverse(), res);
			else
			{
				res = q.Inverse();
				Quaternion_Multiply(*this, res, res);
			}
		});
	}

	[[gnu::always_inline]]
	Quaternion& operator/=(const Quaternion& q) &
	{
		Quaternion_Multiply(*this, q.Inverse(), *this);

		return *this;
	}

	template<class F> [[gnu::always_inline, nodiscard]]
	auto operator/(Proxy<F>&& proxy) const
	{
		return Proxy([this, &proxy]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			if constexpr (resMayAlias)
				Quaternion_Multiply(*this, std::move(proxy).Inverse(), res);
			else
			{
				proxy.Inverse().template Eval<resMayAlias>(res);
				Quaternion_Multiply(*this, res, res);
			}
		});
	}

	[[nodiscard]]
	Fix12i Dot(const Quaternion& q) const
	{
		return this->Vec().Dot(q.Vec()) + w*q.w;
	}

	[[nodiscard]]
	Fix12i Len() const
	{
		return { Sqrt(
			Sqr(static_cast<int64_t>(x.val)) +
			Sqr(static_cast<int64_t>(y.val)) +
			Sqr(static_cast<int64_t>(z.val)) +
			Sqr(static_cast<int64_t>(w.val))
		), as_raw };
	}

	[[gnu::always_inline]]
	void Normalize() & { Quaternion_Normalize(*this); }

	[[gnu::always_inline, nodiscard]]
	auto Normalized() const
	{
		return Proxy([this]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			res = *this;
			res.Normalize();
		});
	}

	[[gnu::always_inline]]
	void ExpDecay(const Quaternion& q, Fix12i weight) &
	{
		Quaternion_Lerp(*this, q, weight, *this);
	}

	void MatchHemisphere(const Quaternion& q) &
	{
		if (this->Dot(q) < 0._f) *this = -*this;
	}

	[[gnu::always_inline, nodiscard]]
	static auto FromVector3(const Vector3& v0, const Vector3& v1)
	{
		return Proxy([&v0, &v1]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			Quaternion_FromVector3(res, v0, v1);
		});
	}

	[[gnu::always_inline, nodiscard]]
	static auto FromMatrix(const Matrix3x3& m) // assumes m is a rotation matrix
	{
		return Proxy([&m]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			Quaternion_FromMatrix3x3(res, m);
		});
	}

	constexpr explicit operator bool() const
	{
		return x != 0_f || y != 0_f || z != 0_f || w != 0_f;
	}
};

[[gnu::always_inline, nodiscard]]
inline auto operator* (const Fix12i& scalar, const Quaternion& q) { return q * scalar; }

template<class F> [[gnu::always_inline, nodiscard]]
inline auto operator* (const Fix12i& scalar, Quaternion::Proxy<F>&& proxy) { return std::move(proxy) * scalar; }

[[gnu::always_inline, nodiscard]]
inline auto Lerp(const Quaternion& q0, const Quaternion& q1, const Fix12i& t)
{
	return Quaternion::Proxy([&q0, &q1, &t]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
	{
		Quaternion_Lerp(q0, q1, t, res);
	});
}

template<class F> [[gnu::always_inline, nodiscard]]
inline auto Lerp(Quaternion::Proxy<F>&& q0, const Quaternion& q1, const Fix12i& t)
{
	return Quaternion::Proxy([&q0, &q1, &t]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
	{
		if constexpr (resMayAlias)
			Quaternion_Lerp(std::move(q0), q1, t, res);
		else
		{
			q0.template Eval<resMayAlias>(res);
			Quaternion_Lerp(res, q1, t, res);
		}
	});
}

template<class F, class Q0> [[gnu::always_inline, nodiscard]]
inline auto Lerp(Q0&& q0, Quaternion::Proxy<F>&& q1, const Fix12i& t)
{
	return Quaternion::Proxy([&q0, &q1, &t]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
	{
		if constexpr (resMayAlias)
			Quaternion_Lerp(std::forward<Q0>(q0), std::move(q1), t, res);
		else
		{
			q1.template Eval<resMayAlias>(res);
			Quaternion_Lerp(std::forward<Q0>(q0), res, t, res);
		}
	});
}

[[gnu::always_inline, nodiscard]]
inline auto LerpShortestPath(const Quaternion& q0, const Quaternion& q1, const Fix12i& t)
{
	return Quaternion::Proxy([&q0, &q1, &t]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
	{
		if (q0.Dot(q1) < 0._f)
			Quaternion_Lerp(q0, -q1, t, res);
		else
			Quaternion_Lerp(q0, q1, t, res);
	});
}

template<class F> [[gnu::always_inline, nodiscard]]
inline auto LerpShortestPath(Quaternion::Proxy<F>&& q0, const Quaternion& q1, const Fix12i& t)
{
	return Quaternion::Proxy([&q0, &q1, &t]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
	{
		if constexpr (resMayAlias)
		{
			Quaternion temp = std::move(q0);
			temp.MatchHemisphere(q1);
			Quaternion_Lerp(temp, q1, t, res);
		}
		else
		{
			q0.template Eval<resMayAlias>(res);
			res.MatchHemisphere(q1);
			Quaternion_Lerp(res, q1, t, res);
		}
	});
}

template<class F, class Q0> [[gnu::always_inline, nodiscard]]
inline auto LerpShortestPath(Q0&& q0, Quaternion::Proxy<F>&& q1, const Fix12i& t)
{
	return Quaternion::Proxy([&q0, &q1, &t]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
	{
		const Quaternion& q = std::forward<Q0>(q0);

		if constexpr (resMayAlias)
		{
			Quaternion temp = std::move(q1);
			temp.MatchHemisphere(q);
			Quaternion_Lerp(q, temp, t, res);
		}
		else
		{
			q1.template Eval<resMayAlias>(res);
			res.MatchHemisphere(q);
			Quaternion_Lerp(q, res, t, res);
		}
	});
}

inline const ostream& operator<<(const ostream& os, const Quaternion& q)
{
	os.set_buffer("+0x%r0%_f + 0x%r1%_f i + 0x%r2%_f j + 0x%r3%_f k");

	os.flush(
		os.update_sign(q.w.val, 0),
		os.update_sign(q.x.val, 10),
		os.update_sign(q.y.val, 23),
		os.update_sign(q.z.val, 36)
	);

	return os;
}

// assumes m is a rotation matrix
inline void Quaternion_FromMatrix3x3(Quaternion& res, const Matrix3x3& m)
{
	Fix12i traces[4] =
	{
		+ m.c0.x + m.c1.y + m.c2.z,
		+ m.c0.x - m.c1.y - m.c2.z,
		- m.c0.x + m.c1.y - m.c2.z,
		- m.c0.x - m.c1.y + m.c2.z,
	};

	unsigned j = 0;
	for (unsigned i = 1; i < 4; ++i)
		if (traces[j] < traces[i]) j = i;

	Fix12i a = 1._f + traces[j];
	SqrtAsync(std::max(a, 0_f) << 2);

	switch (j)
	{
	case 0:
		res.w = a;
		res.x = m.c1.z - m.c2.y;
		res.y = m.c2.x - m.c0.z;
		res.z = m.c0.y - m.c1.x;
		break;
	case 1:
		res.w = m.c1.z - m.c2.y;
		res.x = a;
		res.y = m.c0.y + m.c1.x;
		res.z = m.c2.x + m.c0.z;
		break;
	case 2:
		res.w = m.c2.x - m.c0.z;
		res.x = m.c0.y + m.c1.x;
		res.y = a;
		res.z = m.c1.z + m.c2.y;
		break;
	case 3:
		res.w = m.c0.y - m.c1.x;
		res.x = m.c2.x + m.c0.z;
		res.y = m.c1.z + m.c2.y;
		res.z = a;
		break;
	}

	res *= SqrtResultFix12i().Inverse();
}
