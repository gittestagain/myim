#include "util.h"

using namespace ipc;

class u_time_impl
{
public:
	bool init(void);
	usec_t peek_usec(void);
	msec_t peek_msec(void);
	usec_t usec(void);
	msec_t msec(void);
	u_time_impl(void);

	msec_t     ms_n_;
	msec_t     ms_s_;
	usec_t     us_n_;
	usec_t     us_s_;
#ifdef WIN32
	usec_t     tick_freq_;
	usec_t     tick_init_;
#endif//WIN32
};

u_time_impl* time_impl__ = NULL;

bool
u_time_impl::init(void) 
{
#ifdef WIN32
	QueryPerformanceCounter((LARGE_INTEGER*)&tick_init_);
	QueryPerformanceFrequency((LARGE_INTEGER*)&tick_freq_);
	us_s_ = 0;
	ms_s_ = ::GetTickCount();
#else//WIN32
	struct timespec ts;
	const int ret = clock_gettime(CLOCK_MONOTONIC, &ts);
	assert(0 == ret);
	us_s_ = usec_t(ts.tv_sec) * 1000000 + usec_t(ts.tv_nsec) / 1000;
	ms_s_ = us_s_ / 1000;
#endif//WIN32
	ms_n_ = 0;
	us_n_ = 0;

	return true;
}
inline msec_t
u_time_impl::peek_msec(void)
{
	return ms_n_;
}
inline usec_t
u_time_impl::peek_usec(void)
{
	return us_n_;
}
inline msec_t 
u_time_impl::msec(void)
{
#ifdef WIN32
	return (ms_n_ = ::GetTickCount() - ms_s_);
#else//WIN32
	struct timespec ts;
	const int micro_inc = clock_gettime(CLOCK_MONOTONIC, &ts);
	assert(0 == micro_inc);
	return (ms_n_ = (ts.tv_sec * 1000 + ts.tv_nsec / 1000000 - ms_s_));
#endif//WIN32
}
inline usec_t 
u_time_impl::usec(void)
{
#ifdef WIN32
	msec_t milli = ::GetTickCount();
	usec_t count;
	QueryPerformanceCounter((LARGE_INTEGER*)&count);
	usec_t micro_inc = (i64_t)((count - tick_init_) * 1000000 / tick_freq_);
	ms_n_ = milli - ms_s_;
	return us_n_ = micro_inc - us_s_;
#else//WIN32
	struct timespec ts;
	const int micro_inc = clock_gettime(CLOCK_MONOTONIC, &ts);
	assert(0 == micro_inc);
	us_n_ = (usec_t(ts.tv_sec) * 1000000 + usec_t(ts.tv_nsec) / 1000 - us_s_);
	ms_n_ = us_n_ / 1000;
	return us_n_;
#endif//WIN32
}
inline 
u_time_impl::u_time_impl(void)
{
	ms_n_ = 0;
	ms_s_ = 0;
	us_n_ = 0;
	us_s_ = 0;
#ifdef WIN32
	tick_init_ = 0;
	tick_freq_ = 0;
#endif//WIN32
}

namespace ipc {
	msec_t
		u_time::msec(void)
	{
		assert(NULL != time_impl__);
		return time_impl__->msec();
	}
	usec_t
		u_time::usec(void)
	{
		assert(NULL != time_impl__);
		return time_impl__->usec();
	}
	msec_t
		u_time::peek_msec(void)
	{
		assert(NULL != time_impl__);
		return time_impl__->peek_msec();
	}
	usec_t
		u_time::peek_usec(void)
	{
		assert(NULL != time_impl__);
		return time_impl__->peek_usec();
	}
/*
	void
		u_md5::calc(const byte dat[64])
	{
		u32_t* u = (u32_t*)dat;
		u32_t X[16];

		X[0] = u[0];   X[1] = u[1];
		X[2] = u[2];   X[3] = u[3];
		X[4] = u[4];   X[5] = u[5];
		X[6] = u[6];   X[7] = u[7];
		X[8] = u[8];   X[9] = u[9];
		X[10] = u[10]; X[11] = u[11];
		X[12] = u[12]; X[13] = u[13];
		X[14] = u[14]; X[15] = u[15];


		u32_t A = state_[0], B = state_[1], C = state_[2], D = state_[3];

#undef  P
#define P(a, b, c, d, k, s, t)                                \
		{ a += F(b, c, d) + X[k] + t; a = u_rol_32(a, s) + b; }

#undef  F
#define F(x, y, z) (z ^ (x & (y ^ z)))

		P( A, B, C, D,  0,  7, 0xD76AA478 ); P( D, A, B, C,  1, 12, 0xE8C7B756 );
		P( C, D, A, B,  2, 17, 0x242070DB ); P( B, C, D, A,  3, 22, 0xC1BDCEEE );
		P( A, B, C, D,  4,  7, 0xF57C0FAF ); P( D, A, B, C,  5, 12, 0x4787C62A );
		P( C, D, A, B,  6, 17, 0xA8304613 ); P( B, C, D, A,  7, 22, 0xFD469501 );
		P( A, B, C, D,  8,  7, 0x698098D8 ); P( D, A, B, C,  9, 12, 0x8B44F7AF );
		P( C, D, A, B, 10, 17, 0xFFFF5BB1 ); P( B, C, D, A, 11, 22, 0x895CD7BE );
		P( A, B, C, D, 12,  7, 0x6B901122 ); P( D, A, B, C, 13, 12, 0xFD987193 );
		P( C, D, A, B, 14, 17, 0xA679438E ); P( B, C, D, A, 15, 22, 0x49B40821 );

#undef  F
#define F(x, y, z) (y ^ (z & (x ^ y)))

		P( A, B, C, D,  1,  5, 0xF61E2562 ); P( D, A, B, C,  6,  9, 0xC040B340 );
		P( C, D, A, B, 11, 14, 0x265E5A51 ); P( B, C, D, A,  0, 20, 0xE9B6C7AA );
		P( A, B, C, D,  5,  5, 0xD62F105D ); P( D, A, B, C, 10,  9, 0x02441453 );
		P( C, D, A, B, 15, 14, 0xD8A1E681 ); P( B, C, D, A,  4, 20, 0xE7D3FBC8 );
		P( A, B, C, D,  9,  5, 0x21E1CDE6 ); P( D, A, B, C, 14,  9, 0xC33707D6 );
		P( C, D, A, B,  3, 14, 0xF4D50D87 ); P( B, C, D, A,  8, 20, 0x455A14ED );
		P( A, B, C, D, 13,  5, 0xA9E3E905 ); P( D, A, B, C,  2,  9, 0xFCEFA3F8 );
		P( C, D, A, B,  7, 14, 0x676F02D9 ); P( B, C, D, A, 12, 20, 0x8D2A4C8A );

#undef  F
#define F(x, y, z) (x ^ y ^ z)

		P( A, B, C, D,  5,  4, 0xFFFA3942 ); P( D, A, B, C,  8, 11, 0x8771F681 );
		P( C, D, A, B, 11, 16, 0x6D9D6122 ); P( B, C, D, A, 14, 23, 0xFDE5380C );
		P( A, B, C, D,  1,  4, 0xA4BEEA44 ); P( D, A, B, C,  4, 11, 0x4BDECFA9 );
		P( C, D, A, B,  7, 16, 0xF6BB4B60 ); P( B, C, D, A, 10, 23, 0xBEBFBC70 );
		P( A, B, C, D, 13,  4, 0x289B7EC6 ); P( D, A, B, C,  0, 11, 0xEAA127FA );
		P( C, D, A, B,  3, 16, 0xD4EF3085 ); P( B, C, D, A,  6, 23, 0x04881D05 );
		P( A, B, C, D,  9,  4, 0xD9D4D039 ); P( D, A, B, C, 12, 11, 0xE6DB99E5 );
		P( C, D, A, B, 15, 16, 0x1FA27CF8 ); P( B, C, D, A,  2, 23, 0xC4AC5665 );

#undef  F
#define F(x, y, z) (y ^ (x | ~z))

		P( A, B, C, D,  0,  6, 0xF4292244 ); P( D, A, B, C,  7, 10, 0x432AFF97 );
		P( C, D, A, B, 14, 15, 0xAB9423A7 ); P( B, C, D, A,  5, 21, 0xFC93A039 );
		P( A, B, C, D, 12,  6, 0x655B59C3 ); P( D, A, B, C,  3, 10, 0x8F0CCC92 );
		P( C, D, A, B, 10, 15, 0xFFEFF47D ); P( B, C, D, A,  1, 21, 0x85845DD1 );
		P( A, B, C, D,  8,  6, 0x6FA87E4F ); P( D, A, B, C, 15, 10, 0xFE2CE6E0 );
		P( C, D, A, B,  6, 15, 0xA3014314 ); P( B, C, D, A, 13, 21, 0x4E0811A1 );
		P( A, B, C, D,  4,  6, 0xF7537E82 ); P( D, A, B, C, 11, 10, 0xBD3AF235 );
		P( C, D, A, B,  2, 15, 0x2AD7D2BB ); P( B, C, D, A,  9, 21, 0xEB86D391 );

#undef  F
		state_[0] += A;
		state_[1] += B;
		state_[2] += C;
		state_[3] += D;
	}
	void
		u_md5::iter(const byte* dat, const int len)
	{
		assert(NULL != dat);
		assert(len > 0);
		assert(total_ <= (0xffffffff - len));

		u32_t l1 = len;
		u32_t l2 = total_ & 0x3F;
		u32_t l3 = 64 - l2;

		total_ += l1;

		if (l2 > 0 && l1 >= l3) {
			memcpy(buff_ + l2, dat, l3);
			calc(buff_);
			dat += l3;
			l1 -= l3;
			l2 = 0;
		}

		while (l1 > 63) {
			calc(dat);
			dat += 64;
			l1 -= 64;
		}

		if (l1 > 0)
			memcpy(buff_ + l2, dat, l1);
	}
	void
		u_md5::fini(md5_bin_t& h)
	{
		byte  t[72] = { 0x80 };
		u32_t l1 = total_ & 0x3f;
		u32_t l2 = l1 < (64 - 8) ? 64 - l1 : (128 - l1);
		u32_t l3 = l2 - 8;

		u32_t ul = (total_ << 3), uh = (total_ >> (32 - 3));
		*((u32_t*)(t + l3)) = ul;
		*((u32_t*)(t + l3 + sizeof(u32_t))) = uh;

		iter(t, l2);

		u32_t* u = (u32_t*)h;
		u[0] = state_[0];
		u[1] = state_[1];
		u[2] = state_[2];
		u[3] = state_[3];

		//reset
		total_ = 0;
		state_[0] = 0x67452301;
		state_[1] = 0xEFCDAB89;
		state_[2] = 0x98BADCFE;
		state_[3] = 0x10325476;
	}
*/
	bool
		util_init(void)
	{
		assert(NULL == time_impl__);
		time_impl__ = new u_time_impl;
		return time_impl__->init();
	}
	void
		util_fini(void)
	{
		if (NULL != time_impl__) {
			delete time_impl__;
			time_impl__ = NULL;
		}
	}
} // namespace YAS