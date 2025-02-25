#pragma once

#include "ostream.h"
#include "MathCommon.h"
#include <type_traits>
#include <concepts>
#include <limits>
#include <utility>

namespace cstd
{
	s32 fdiv(s32 numerator, s32 denominator); // returns a Q12 number
}

struct AsRaw {} constexpr as_raw;

template<class T> // a valid underlying representation of a fixed-point number
concept FixUR = std::is_integral_v<T> && std::is_signed_v<T> && sizeof(T) <= sizeof(s32);

template<FixUR T, s32 q, template<FixUR> class CRTP>
struct Fix
{
	T val;

	using Promoted = CRTP<s32>;

	constexpr Fix() = default;
	constexpr Fix(std::integral auto val) : val(val << q) {}
	constexpr Fix(std::integral auto val, AsRaw) : val(val) {}

	// SM64DS rounds down (0.1_f becomes 0x199, not 0x19a)
	constexpr explicit Fix(long double val):
#ifdef FIXED_POINT_LITERAL_ROUND_DOWN
		val(val * (1ll << q))
#else
		val(val * (1ll << q) + 0.5l)
#endif
	{}

	template<FixUR U, s32 r, template<class> class CRTP2>
	constexpr Fix(Fix<U, r, CRTP2> f, AsRaw) : val(f.val) {}

	template<FixUR U>
	constexpr Fix(CRTP<U> f) : val(f.val) {}

	friend constexpr
	Promoted operator+ (CRTP<T> f) { return {f.val, as_raw}; }

	friend constexpr
	Promoted operator- (CRTP<T> f) { return {-f.val, as_raw}; }

	template<FixUR U> friend constexpr
	Promoted operator+ (CRTP<T> f0, CRTP<U> f1) { return {f0.val + f1.val, as_raw}; }

	template<FixUR U> friend constexpr
	Promoted operator- (CRTP<T> f0, CRTP<U> f1) { return {f0.val - f1.val, as_raw}; }

	template<FixUR U> friend constexpr
	CRTP<T>& operator+=(CRTP<T>& f0, CRTP<U> f1) { f0.val += f1.val; return f0; }

	template<FixUR U> friend constexpr
	CRTP<T>& operator-=(CRTP<T>& f0, CRTP<U> f1) { f0.val -= f1.val; return f0; }

	friend constexpr Promoted operator+ (s32 i,  CRTP<T> f) { return Promoted(i) + f; }
	friend constexpr Promoted operator- (s32 i,  CRTP<T> f) { return Promoted(i) - f; }
	friend constexpr Promoted operator+ (CRTP<T>  f, s32 i) { return f + Promoted(i); }
	friend constexpr Promoted operator- (CRTP<T>  f, s32 i) { return f - Promoted(i); }
	friend constexpr CRTP<T>& operator+=(CRTP<T>& f, s32 i) { return f += Promoted(i); }
	friend constexpr CRTP<T>& operator-=(CRTP<T>& f, s32 i) { return f -= Promoted(i); }

	friend constexpr Promoted operator* (s32 i,  CRTP<T> f) { return {i * f.val, as_raw}; }
	friend constexpr Promoted operator* (CRTP<T>  f, s32 i) { return {f.val * i, as_raw}; }
	friend constexpr Promoted operator/ (CRTP<T>  f, s32 i) { return {f.val / i, as_raw}; }
	friend constexpr CRTP<T>& operator*=(CRTP<T>& f, s32 i) { f.val *= i; return f; }
	friend constexpr CRTP<T>& operator/=(CRTP<T>& f, s32 i) { f.val /= i; return f; }

	friend constexpr Promoted operator<< (CRTP<T>  f, s32 i) { return {f.val << i, as_raw}; }
	friend constexpr Promoted operator>> (CRTP<T>  f, s32 i) { return {f.val >> i, as_raw}; }
	friend constexpr CRTP<T>& operator<<=(CRTP<T>& f, s32 i) { f.val <<= i; return f; }
	friend constexpr CRTP<T>& operator>>=(CRTP<T>& f, s32 i) { f.val >>= i; return f; }

	template<FixUR U> friend constexpr
	Promoted operator&(CRTP<T>  f0, CRTP<U>  f1) { return {f0.val & f1.val, as_raw}; }

	template<FixUR U> friend constexpr
	Promoted operator|(CRTP<T>  f0, CRTP<U>  f1) { return {f0.val | f1.val, as_raw}; }

	template<FixUR U> friend constexpr
	Promoted operator^(CRTP<T>  f0, CRTP<U>  f1) { return {f0.val ^ f1.val, as_raw}; }

	template<FixUR U> friend constexpr
	CRTP<T>& operator&=(CRTP<T>& f0, CRTP<U>& f1) { f0.val &= f1.val; return f0; }

	template<FixUR U> friend constexpr
	CRTP<T>& operator|=(CRTP<T>& f0, CRTP<U>& f1) { f0.val |= f1.val; return f0; }

	template<FixUR U> friend constexpr
	CRTP<T>& operator^=(CRTP<T>& f0, CRTP<U>& f1) { f0.val ^= f1.val; return f0; }

	template<FixUR U> [[gnu::always_inline, nodiscard]] friend
	Promoted operator*(CRTP<T> f0, CRTP<U> f1)
	{
		const u64 product = static_cast<s64>(f0.val) * f1.val;
		Promoted result;

		asm(R"(
			movs %[rs], %[lo], lsr %[s0]
			adc  %[rs], %[rs], %[hi], lsl %[s1]
		)":
		[rs] "=&r" (result) :
		[lo] "r" (static_cast<u32>(product)),
		[hi] "r" (static_cast<u32>(product >> 32)),
		[s0] "I" (q),
		[s1] "I" (32 - q) : "cc");

		return result;
	}

	template<FixUR U> friend constexpr
	Promoted operator% (CRTP<T> f0, CRTP<U> f1)
	{
		return {f0.val % f1.val, as_raw};
	}

	template<FixUR U> friend constexpr
	CRTP<T>& operator%=(CRTP<T>& f0, CRTP<U> f1)
	{
		f0.val %= f1.val; return f0;
	}

	template<FixUR U> friend inline
	CRTP<T>& operator*=(CRTP<T>& f0, CRTP<U> f1) { return f0 = f0 * f1; }

	template<FixUR U> friend constexpr
	bool operator==(CRTP<T> f0, CRTP<U> f1) { return f0.val == f1.val; }

	template<FixUR U> friend constexpr
	bool operator< (CRTP<T> f0, CRTP<U> f1) { return f0.val <  f1.val; }

	template<FixUR U> friend constexpr
	bool operator<=(CRTP<T> f0, CRTP<U> f1) { return f0.val <= f1.val; }

	template<FixUR U> friend constexpr
	bool operator> (CRTP<T> f0, CRTP<U> f1) { return f0.val >  f1.val; }

	template<FixUR U> friend constexpr
	bool operator>=(CRTP<T> f0, CRTP<U> f1) { return f0.val >= f1.val; }

	static constexpr CRTP<T> max {std::numeric_limits<T>::max(), as_raw};
	static constexpr CRTP<T> min {std::numeric_limits<T>::min(), as_raw};

	static constexpr CRTP<T> pi = []
	{
		const s32 shift = 8 * static_cast<s32>(sizeof(T)) - q;

		s64 raw = 0x3243f6a89ll;

		if (q < 32)
			raw += 1 << (31 - q);

		return CRTP<T> {raw >> shift, as_raw};
	}();

	[[gnu::always_inline]]
	constexpr Promoted friend Abs(CRTP<T> f) { return f.val >= 0 ? f : -f; }

	constexpr explicit operator T() const { return val >> q; }
	constexpr explicit operator bool() const { return val != 0; }

	bool ApproachLinear(Promoted dest, Promoted step) & { return ::ApproachLinear(val, dest.val, step.val); }
};

template<class T>
concept FixedPoint = std::is_invocable_v
	<decltype([]<class U, s32 q, template<class> class CRTP>
	requires std::derived_from<CRTP<U>, Fix<U, q, CRTP>>
	(Fix<U, q, CRTP>&&) {}), T>;

template<FixedPoint T>
using Underlying = decltype(std::declval<T>().val);

template<FixUR T>
struct Fix12 : Fix<T, 12, Fix12>
{
	using Fix<T, 12, Fix12>::Fix;

	Fix12<s32> operator/ (Fix12 f) const { return {cstd::fdiv(this->val, f.val), as_raw}; }
	Fix12&     operator/=(Fix12 f) & { this->val = cstd::fdiv(this->val, f.val); return *this; }
};

using Fix12i = Fix12<s32>;
using Fix12s = Fix12<s16>;

consteval Fix12i operator""_f (u64 val) { return Fix12i(val, as_raw); }
consteval Fix12s operator""_fs(u64 val) { return Fix12s(val, as_raw); }

consteval Fix12i operator""_f (long double val) { return Fix12i(val); }
consteval Fix12s operator""_fs(long double val) { return Fix12s(val); }

namespace cstd
{
	Fix12i fdiv(Fix12i numerator, Fix12i denominator);
	Fix12i finv(Fix12i denominator); // inverse
	inline Fix12i sqrt(Fix12i x) { return Fix12i(sqrt(static_cast<u64>(x.val) << 12), as_raw); }
	s16 atan2(Fix12i y, Fix12i x); //atan2 function, what about 0x020538b8?
}

template<FixUR T>
inline const ostream& operator<<(const ostream& os, Fix12<T> fix)
{
	if (fix >= Fix12<T>(0, as_raw))
	{
		os.set_buffer("0x%r0%_f");
		os.flush(fix.val);
	}
	else
	{
		os.set_buffer("-0x%r0%_f");
		os.flush(-fix.val);
	}

	return os;
}
