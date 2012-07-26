#ifndef ___YAS_UTIL_H
#define ___YAS_UTIL_H

#include "def.h"
#include <time.h>

namespace ipc {

//	inline u32_t
//		u_rol_32(u32_t u, const u8_t m)
//	{
//#ifdef WIN32
//		__asm mov   eax, u  __asm mov  cl, m  __asm rol  eax, cl
//#else // WIN32
//        __asm__ __volatile__ ("rol %%cl, %%eax"
//                    : "=a" (u)
//                    : "a" (u), "c" (m));
//#endif // WIN32
//	}
//	inline u32_t
//		u_ror_32(u32_t u, const u8_t m)
//	{
//#ifdef WIN32
//		__asm mov   eax, u  __asm mov  cl, m  __asm ror eax, cl
//#else // WIN32
//        __asm__ __volatile__ ("ror %%cl, %%eax"
//                    : "=a" (u)
//                    : "a" (u), "c" (m));
//#endif // WIN32
//	}
	inline u32_t
		u_ztou_32(const char *z, const int l)
	{
		assert(l > 0);

		u32_t u = 0;
		const char *i = z + l;
		u32_t x = 1;
		do {
			--i;
			u += (*i - '0') * x;
			x *= 10;
		} while (z != i);

		return u;
	}
	inline void
		u_utoz_32(const u32_t u, char *z, int &l)
	{
		char  d[12];
		char *i = d;
		u32_t t = u;
		do {
			assert(i < d + l);
			*i++ = (t % 10) + '0';
			t /= 10;
		} while (t > 0);

		l = i - d;
		for (--i; i >= d; *z++ = *i--) {}
	}

	class u_time
	{
	public:
		static msec_t msec(void);
		static usec_t usec(void);
		static msec_t peek_msec(void);
		static usec_t peek_usec(void);
		static msec_t msec_abs(void);
		static usec_t usec_abs(void);
	};
	inline msec_t 
		u_time::msec_abs(void)
	{
#ifdef WIN32
		return ::GetTickCount();
#else //WIN32
		struct timespec ts;
		const int micro_inc = clock_gettime(CLOCK_MONOTONIC, &ts);
		assert(0 == micro_inc);
		return ts.tv_sec * 1000 + ts.tv_nsec / 1000000;
#endif//WIN32
	}
	inline usec_t
		u_time::usec_abs(void)
	{
#ifdef WIN32
		return ::GetTickCount() * 1000;
#else //WIN32
		struct timespec ts;
		const int micro_inc = clock_gettime(CLOCK_MONOTONIC, &ts);
		assert(0 == micro_inc);
		return u64_t(ts.tv_sec) * 1000000 + u64_t(ts.tv_nsec) / 1000;
#endif//WIN32
	}

	//class u_md5 
	//{
	//public:
	//	void calc(const byte dat[64]);
	//	void iter(const byte* dat, const int len);
	//	void fini(md5_bin_t& h);
	//	u_md5(void);

	//	static void gen(const byte* dat, const int len, md5_bin_t& h);
	//	static void hmac(const byte* key, const int kl, const byte* dat, const int dl, md5_bin_t& h);

	//	u32_t    total_;
	//	u32_t    state_[4];
	//	byte     buff_[64];
	//};
	//inline
	//	u_md5::u_md5(void)
	//{
	//	total_ = 0;
	//	state_[0] = 0x67452301;
	//	state_[1] = 0xEFCDAB89;
	//	state_[2] = 0x98BADCFE;
	//	state_[3] = 0x10325476;
	//}
	//inline void
	//	u_md5::gen(const byte* dat, const int len, md5_bin_t& h)
	//{
	//	u_md5 um;
	//	um.iter(dat, len);
	//	um.fini(h);
	//}
	//inline void
	//	u_md5::hmac(const byte* key, const int kl, const byte* dat, const int dl, md5_bin_t& h)
	//{
	//	byte  ip[64], op[64];
	//	memset(ip, 0x36, sizeof(ip));
	//	memset(op, 0x5c, sizeof(op));
	//	md5_bin_t t;
	//	u_md5     m;

	//	const byte* k = key;
	//	int      l = kl;
	//	if (l > 64) {
	//		m.iter(k, l);
	//		m.fini(t);
	//		k = t;
	//		l = sizeof(t);
	//	}
	//	for (int i = 0; i < l; ++i) {
	//		ip[i] ^= k[i];
	//		op[i] ^= k[i];
	//	}

	//	m.iter(ip, sizeof(ip));
	//	m.iter(dat, dl);
	//	m.fini(t);

	//	m.iter(op, sizeof(op));
	//	m.iter(t, sizeof(t));
	//	m.fini(h);
	//}

	template<typename T>
	class u_link
	{
	public:
		T* pop(void);
		void add(T *o);
		void cat(u_link &l);
		void nul(void);
		int  siz(void);
		void swap(u_link &l);
		u_link(void);
		~u_link(void);

		T       *head_;
		T       *tail_;
		int      size_;
	};

	template<typename T>
	inline T*
		u_link<T>::pop(void)
	{
		T *t = head_;

		if (NULL != t) {

			if (t == tail_)
				head_ = tail_ = NULL;
			else
				head_ = t->next_;

			--size_;
		}

		return t;
	}
	template<typename T>
	inline void
		u_link<T>::add(T *o)
	{
		assert(NULL != o);
		assert(NULL == o->next_);
		if (NULL != tail_) {
			tail_->next_ = o;
		} else {
			assert(NULL == head_);
			assert(0 == size_);
			head_ = o;
		}

		tail_ = o;
		++size_;
	}
	template<typename T>
	inline void
		u_link<T>::cat(u_link &l)
	{
		if (NULL != l.head_) {
			if (NULL != tail_) {
				tail_->next_ = l.head_;
			} else {
				assert(NULL == head_);
				assert(0 == size_);
				head_ = l.head_;
			}
			tail_ = l.tail_;
			size_ += l.size_;

			l.head_ = l.tail_ = NULL;
			l.size_ = 0;
		}
	}
	template<typename T>
	inline void
		u_link<T>::nul(void)
	{
		T* i = head_;
		while (NULL != i) {
			T* t = i;
			i = i->next_;
			delete t;
		}
		head_ = tail_ = NULL;
		size_ = 0;
	}
	template<typename T>
	inline int
		u_link<T>::siz(void)
	{
		return size_;
	}
	template<typename T>
	inline void
		u_link<T>::swap(u_link &l)
	{
		T *h = l.head_;
		T *t = l.tail_;
		int s = l.size_;

		l.head_ = head_;
		l.tail_ = tail_;
		l.size_ = size_;

		head_ = h;
		tail_ = t;
		size_ = s;
	}
	template<typename T>
	inline
		u_link<T>::u_link(void)
	{
		head_ = tail_ = NULL;
		size_ = 0;
	}
	template<typename T>
	inline
		u_link<T>::~u_link(void)
	{
		T* i = head_;
		while (NULL != i) {
			T* t = i;
			i = i->next_;
			delete t;
		}
	}

	template<typename T, int SIZE>
	class u_slot
	{
		struct TT : public T
		{
			TT  *prev_;
			TT  *next_;
		};

		int  insert(T &t);
		void remove(const int k);
		void clear(void);

		T& operator[](const int k);
		T*   head(void);
		T*   next(T *t);
		int  usiz(void);

		u_slot(void);
		~u_slot(void);

		TT  *slot_;
		TT  *free_;
		TT  *used_;
		int	 ssiz_;
		int  usiz_;
	};

	template<typename T, int SIZE>
	inline int 
		u_slot<T, SIZE>::insert(T &t)
	{
		if (NULL == free_) {
			int ns = (0 == ssiz_) ? SIZE : ssiz_ << 1;
			TT* nl = (TT*)::realloc(slot_, ns * sizeof(TT));

			if (nl != slot_ && NULL != slot_) {
				assert(NULL != used_);
				assert(ssiz_ > 0);

				int dl = nl - slot_;
				TT *s = nl, *e = nl + ssiz_;
				do {
					if (NULL != s->next_)
						s->next_ += dl;
					if (NULL != s->prev_)
						s->prev_ += dl;
				} while (++s < e);
				used_ += dl;
			}
			int ds = ns - ssiz_;
			TT *nf = nl + ssiz_, *s = nf, *e = nl + ns - 1;
			while (s < e) {
				s->prev_ = NULL;
				s->next_ = s + 1;
				++s;
			}
			s->prev_ = s->next_ = NULL;
			free_ = nf; slot_ = nl; ssiz_ = ns;
		}

		TT *tt = free_;
		assert(NULL == tt->prev_);
		free_ = free_->next_;
		if (NULL != used_) used_->prev_ = tt;
		tt->next_ = used_; used_ = tt;

		*((T*)tt) = t; ++usiz_;

		return tt - slot_;
	}
	template<typename T, int SIZE>
	inline void
		u_slot<T, SIZE>::remove(const int k)
	{
		assert(-1 != k);
		assert(k < ssiz_);
		assert(NULL != used_);

		TT *i = slot_ + k;
		TT *p = i->prev_;
		TT *n = i->next_;
		if (NULL != p)
			p->next_ = n;
		else 
			used_ = n;
		if (NULL != n)
			n->prev_ = p;

		i->prev_ = NULL;
		i->next_ = free_;
		free_ = i;
		--usiz_;
	}
	template<typename T, int SIZE>
	inline void
		u_slot<T, SIZE>::clear(void)
	{
		if (NULL != slot_) {
			::free(slot_);
			slot_ = NULL;
			free_ = used_ = NULL;
			ssiz_ = 0;
			usiz_ = 0;
		}
	}
	template<typename T, int SIZE>
	inline T&
		u_slot<T, SIZE>::operator[](const int k)
	{
		return slot_[k];
	}
	template<typename T, int SIZE>
	inline T*
		u_slot<T, SIZE>::head(void)
	{
		return used_;
	}
	template<typename T, int SIZE>
	inline T*
		u_slot<T, SIZE>::next(T *t)
	{
		assert(NULL != t);

		TT *tt = (TT*)t;
		assert(tt >= slot_ && tt < slot_ + ssiz_);

		return tt->next_;
	}
	template<typename T, int SIZE>
	inline int
		u_slot<T, SIZE>::usiz(void)
	{
		return usiz_;
	}
	template<typename T, int SIZE>
	u_slot<T, SIZE>::u_slot(void)
	{
		slot_ = free_ = used_ = NULL;
		ssiz_ = usiz_ = 0;
	}
	template<typename T, int SIZE>
	u_slot<T, SIZE>::~u_slot(void)
	{
		if (NULL != slot_) ::free(slot_);
	}

	class u_buff
	{
	public:
		u_buff(void);
		u_buff(const int l);
		u_buff(const u_buff &b);
		void operator = (u_buff &b);
		~u_buff(void);

		void ext(const int l);
		void nul(void);
		void del(void);
		int size(void);
		int left(void);
		void cat(u_buff &m);
		void cat(byte *b, int l);

		static const int MEM_ALIGN_MOVE = 10;
		static const int MEM_ALIGN_SIZE = (1 << MEM_ALIGN_MOVE);
		byte   *b_;
		byte   *s_;
		byte   *p_;
		byte   *e_;
	};
	inline
		u_buff::u_buff(void)
	{
		b_ = s_ = p_ = e_ = NULL;
	}
	inline
		u_buff::u_buff(const int l)
	{
		b_ = s_ = p_ = e_ = NULL;
		this->ext(l);
	}
	inline
		u_buff::u_buff(const u_buff &b)
	{
		*this = const_cast<u_buff&>(b);
	}
	inline void
		u_buff::operator = (u_buff &b)
	{
		b_ = b.b_; s_ = b.s_; p_ = b.p_; e_ = b.e_; 
		b.b_ = b.s_ = b.p_ = b.e_ = NULL;
	}
	inline
		u_buff::~u_buff(void)
	{
		if (NULL != b_) ::free(b_);
	}
	inline void
		u_buff::ext(const int l)
	{
		if (l > e_ - p_) {
			int nl = (((p_ - b_ + l) + MEM_ALIGN_SIZE - 1) >> MEM_ALIGN_MOVE) << MEM_ALIGN_MOVE;

			byte *nb = (byte*)::realloc(b_, nl);

			int d = nb - b_;
			b_ += d; s_ += d; p_ += d; e_ = b_ + nl;
		}
	}
	inline void
		u_buff::nul(void)
	{
		if (NULL != b_) {
			::free(b_);
			b_ = s_ = p_ = e_ = NULL;
		}
	}
	inline void
		u_buff::del(void)
	{
		p_ = s_;
	}
	inline int
		u_buff::size(void)
	{
		assert(p_ >= s_);
		return p_ - s_;
	}
	inline int
		u_buff::left(void)
	{
		assert(e_ >= p_);
		return e_ - p_;
	}
	inline void
		u_buff::cat(u_buff &m)
	{
		ext(m.size());
		memcpy(p_, m.s_, m.size());
		p_ += m.size();
	}
	inline void
		u_buff::cat(byte *b, int l)
	{
		ext(l);
		memcpy(p_, b, l);
		p_ += l;
	}

	template<int SIZE>
	class u_string
	{
	public:
		void dump(cstr z, const int l);
		void nul(void);
		operator cstr(void);
		void operator =(cstr z);
		void operator =(const u_string &z);
		u_string& operator <<(cstr z);
		bool operator ==(cstr z);
		bool operator !=(cstr z);

		u_string(void);
		u_string(cstr z);
		u_string(cstr z, const int l);
		u_string(const u_string &t);

		int       l_;
		char      z_[SIZE];
	};

	template<int SIZE>
	inline void
		u_string<SIZE>::dump(cstr z, const int l)
	{
		if (NULL != z && l > 0) {
			memcpy(this->z_, z, l);
			this->l_ = l;
			this->z_[l] = '\0';
		}
	}
	template<int SIZE>
	inline void
		u_string<SIZE>::nul(void)
	{
		l_ = 0;
		*z_ = '\0';
	}
	template<int SIZE>
	inline 
		u_string<SIZE>::operator cstr(void)
	{
		return z_;
	}
	template<int SIZE>
	inline void
		u_string<SIZE>::operator =(cstr z)
	{
		if (NULL != z) {
			l_ = strlen(z);
			memcpy(z_, z, l_ + 1);
		} 
	}
	template<int SIZE>
	inline void
		u_string<SIZE>::operator =(const u_string &z)
	{
		assert(z.l_ < SIZE);
		memcpy(z_, z.z_, z.l_);
		l_ = z.l_;
		z_[l_] = '\0';
	}
	template<int SIZE>
	inline bool
		u_string<SIZE>::operator ==(cstr z)
	{
		return NULL != z && 0 == strcmp(z_, z);
	}
	template<int SIZE>
	inline bool
		u_string<SIZE>::operator !=(cstr z)
	{
		return !(*this == z);
	}
	template<int SIZE>
	inline u_string<SIZE>&
		u_string<SIZE>::operator <<(cstr z)
	{
		if (NULL != z) {
			int l = strlen(z);
			if (l_ + l < SIZE) {
				memcpy(z_ + l_, z, l + 1);
				l_ += l;
			}
		}
		return *this;
	}
	template<int SIZE>
	inline
		u_string<SIZE>::u_string(void)
	{
		assert(SIZE <= 256);

		l_ = 0;
		*z_ = '\0';
	}
	template<int SIZE>
	inline
		u_string<SIZE>::u_string(cstr z)
	{
		*this = z;
	}
	template<int SIZE>
	inline
		u_string<SIZE>::u_string(cstr z, const int l)
	{
		if (NULL != z && l > 0) {
			assert(l < SIZE);
			memcpy(z_, z, l);
			l_ = l; z_[l] = '\0';
		} else {
			l_ = 0;
			*z_ = '\0';
		}
	}
	template<int SIZE>
	inline
		u_string<SIZE>::u_string(const u_string &t)
	{
		*this = t;
	}

	class u_misc
	{
	public:
		static void msleep(msec_t ms);
	};

	inline void
		u_misc::msleep(msec_t ms)
	{
#ifdef WIN32
		::Sleep(ms);
#else // WIN32
		struct timespec ts = {0};

		if (___YAS_INFINITE != ms) {
			ts.tv_sec = (ms / 1000L);
			ts.tv_nsec = (ms - ts.tv_sec * 1000) * 1000000L;
			nanosleep(&ts, NULL);
		} else {
			ts.tv_sec = 999999999;
			ts.tv_nsec = 0;
			while (true) nanosleep(&ts, NULL);
		}
#endif // WIN32
	}

	bool util_init(void);
	void util_fini(void);
} // namespace YAS

#endif // ___YAS_UTIL_H
