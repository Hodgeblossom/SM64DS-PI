// Stream-based output utilities by pants64

#ifndef O_STREAM_INCLUDED
#define O_STREAM_INCLUDED

#include <cstdint>

class ostream
{
	static constexpr std::size_t buffer_size = 120;
	static char buffer[buffer_size];

public:
	[[gnu::naked]]
	static void flush(...)
	{
		asm volatile (R"(
			push    {r14}
			mov     r14, #0x05000000
			ldrb    r12, [r14, #-0x600]
			cmp     r12, #0
			addne   r12, r15, #0x10
			strne   r12, [r14, #-0x5ec]
			popne   {r15}
			mov     r12, r12
			b       end
			.hword  0x6464
			.hword  0x0000
		.weak _ZN7ostream6bufferE
		_ZN7ostream6bufferE:
			.fill   120
			.word   0
		end:
			pop     {r15}
		)");
	}

	static void set_buffer(const char* str)
	{
		for (std::size_t i = 0; i < buffer_size; i++)
		{
			if ((buffer[i] = str[i]) == '\0')
				break;
		}
	}
	
	const ostream& operator<<(const char* str) const
	{
		set_buffer(str);
		flush();
		return *this;
	}

	const ostream& operator<<(char character) const
	{
		buffer[0] = character;
		buffer[1] = '\0';

		flush();

		return *this;
	}

	const ostream& operator<<(unsigned val) const
	{
		set_buffer("0x%r0%");
		flush(val);

		return *this;
	}

	const ostream& operator<<(uint64_t val) const
	{
		set_buffer("0x%r1%%r0%");
		flush(val);

		return *this;
	}

	template<typename T>
	const ostream& operator<<(T* ptr)            const { return *this << reinterpret_cast<unsigned>(ptr); }
	const ostream& operator<<(char* ptr)         const { return *this << static_cast<const char*>(ptr); }
	const ostream& operator<<(unsigned long val) const { return *this << static_cast<unsigned>(val); }
	const ostream& operator<<(unsigned char val) const { return *this << static_cast<unsigned>(val); }
	const ostream& operator<<(signed char val)   const { return *this << static_cast<int>(val); }
	const ostream& operator<<(bool b)            const { return *this << (b ? "true" : "false"); }

	const ostream& operator<<(int val) const
	{
		if (val < 0)
		{
			set_buffer("-0x%r0%");
			flush(-val);
		}
		else
		{
			set_buffer("0x%r0%");
			flush(val);	
		}
		return *this;
	}
	
	const ostream& operator<<(int64_t val) const
	{
		if (val < 0)
		{
			set_buffer("-0x%r1%%r0%");
			flush(-val);
		}
		else
		{
			set_buffer("0x%r1%%r0%");
			flush(val);	
		}
		return *this;
	}

	int update_sign(int val, std::size_t sign_index) const
	{
		if (val < 0)
		{
			buffer[sign_index] = '-';
			return -val;
		}
		else
		{
			buffer[sign_index] = '+';
			return val;
		}
	}
}
constexpr cout;

#endif