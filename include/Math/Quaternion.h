#pragma once

#include "MathCommon.h"
#include "Proxy.h"
#include "Vector.h"

extern "C"
{
	void Quaternion_FromVector3(Quaternion& qF, const Vector3& v0, const Vector3& v1);
	void Quaternion_Normalize(Quaternion& q);
	void Quaternion_SLerp(const Quaternion& q1, const Quaternion& q2, Fix12i weight, Quaternion& qF);
}

struct Quaternion
{
	Fix12i x;
	Fix12i y;
	Fix12i z;
	Fix12i w;

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
		void Eval(Quaternion& res) { eval.template operator()<resMayAlias>(res); }
	};

	constexpr Quaternion() = default;
	constexpr Quaternion(auto x, auto y, auto z, auto w) : x(x), y(y), z(z), w(w) {}

	template<class F> [[gnu::always_inline]]
	Quaternion(Proxy<F>&& proxy) { proxy.template Eval<false>(*this); }

	template<class F> [[gnu::always_inline]]
	Quaternion& operator=(Proxy<F>&& proxy) & { proxy.template Eval<true>(*this); return *this; }

	[[gnu::always_inline, nodiscard]]
	static auto Vec3(const Vector3& v0, const Vector3& v1)
	{
		return Proxy([&v0, &v1]<bool resMayAlias> [[gnu::always_inline]] (Quaternion& res)
		{
			Quaternion_FromVector3(res, v0, v1);
		});
	}

	[[gnu::always_inline]]
	void Normalize() & { Quaternion_Normalize(*this); }

	[[gnu::always_inline]]
	void SLerp(const Quaternion& q1, const Quaternion& q2, Fix12i weight)
	{
		Quaternion_SLerp(q1, q2, weight, *this);
	}

	[[gnu::always_inline]]
	void SExpDecay(const Quaternion& q, Fix12i weight)
	{
		Quaternion_SLerp(*this, q, weight, *this);
	}

	constexpr bool operator==(const Quaternion&) const = default;
};
