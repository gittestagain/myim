#ifndef ___IPC_IPC_H
#define ___IPC_IPC_H

#include "def.h"

#ifdef  WIN32
#include <process.h>
#else //WIN32
#include <pthread.h>
#include <semaphore.h>
#include <sys/time.h>
#endif//WIN32

namespace ipc {

#ifdef WIN32
	typedef HANDLE              thread_t;
	typedef CRITICAL_SECTION    mutex_t;
	typedef HANDLE              event_t;
	typedef CONDITION_VARIABLE  cond_t;
	typedef HANDLE              sem_t;
#else //WIN32
	typedef pthread_t           thread_t;
	typedef pthread_cond_t      event_t;
	typedef pthread_mutex_t     mutex_t;
	typedef pthread_cond_t      cond_t;
#endif//WIN32

	class u_lock
	{
	public:
		bool trylock(void);
		void lock(void);
		void unlock(void);

		u_lock(void);
		~u_lock(void);

		mutex_t   lock_;
	};

	class u_locker
	{
	public:
		u_locker(u_lock &l);
		~u_locker(void);

		u_lock&     lock_;
	};

	class u_event
	{
	public:
		void signal(void);
		bool wait(const msec_t w = ___IPC_INFINITE);

		u_event(void);
		~u_event(void);

#ifndef WIN32
		u_lock    lock_;
#endif//WIN32
		event_t   event_;
	};

	class u_cond : public u_lock
	{
	public:
		void signal(bool all = false);
		bool wait(const msec_t w = ___IPC_INFINITE);

		u_cond(void);
		~u_cond(void);

		cond_t    cond_;
	};

	class u_semp
	{
	public:
		bool wait(const msec_t w = ___IPC_INFINITE);
		void signal(const int inc = 1);
		u_semp(void);
		~u_semp(void);

		sem_t   sem_;
	};

	class u_atomic
	{
	public:
		static i32_t exchange_and_add(volatile i32_t* val, i32_t add);
		static u32_t compare_and_swap(volatile u32_t* src, u32_t dst, u32_t cmp);
	};

	class u_i32mt
	{
	public:
		int operator ++(void);
		int operator --(void);
		int operator =(i32_t i);
		operator i32_t(void);

		u_i32mt(const i32_t i = 0);
		u_i32mt(const u_i32mt& i);

		volatile i32_t i_;
	};

	class u_i32st
	{
	public:
		u_i32st& operator=(const u_i32st& i);
		int operator++(void);
		int operator--(void);
		operator i32_t(void);

		u_i32st(const i32_t i = 0);
		u_i32st(const u_i32st& i);

		i32_t i_;
	};

	template<typename T>
	class u_refer
	{
	public:
		void swap(u_refer& r);
		void operator =(T* o);
		void operator =(const u_refer& r);
		operator T*(void);
		T* operator ->(void);
		u_refer(void);
		u_refer(T* o);
		u_refer(const u_refer& r);
		~u_refer(void);

		T*   obj_;
	};

	class u_cycle : public u_i32mt
	{
	public:
		virtual void cycle(void)   = 0;
		virtual void cancel(void)  = 0;

		virtual ~u_cycle(void)     = 0;
	};
	inline
		u_cycle::~u_cycle(void)
	{
	}

	class u_thread : public u_i32mt
	{
	public:
		virtual bool init(void);
		virtual void fini(void);
		virtual void loop(void);
		virtual void exit(void);

		bool start(void);
		void stop(void);
		void join(msec_t wait = ___IPC_INFINITE);

		u_thread(void);
		virtual ~u_thread(void);

		void  *impl_;
	};

	inline bool 
		u_lock::trylock(void) 
	{
#ifdef WIN32
		return TRUE == ::TryEnterCriticalSection(&lock_);
#else
		return 0 == pthread_mutex_trylock(&lock_);
#endif
	}
	inline void 
		u_lock::lock(void) 
	{
#ifdef WIN32
		::EnterCriticalSection(&lock_);
#else
		pthread_mutex_lock (&lock_);
#endif
	}
	inline void 
		u_lock::unlock(void) 
	{
#ifdef WIN32
		::LeaveCriticalSection(&lock_);
#else
		pthread_mutex_unlock(&lock_);
#endif
	}
	inline 
		u_lock::u_lock(void) 
	{
#ifdef WIN32
		::InitializeCriticalSectionAndSpinCount(&lock_, 1 << 12);
#else 
		pthread_mutex_init(&lock_, NULL);
#endif
	}
	inline 
		u_lock::~u_lock(void) 
	{
#ifdef WIN32
		::DeleteCriticalSection(&lock_);
#else 
		pthread_mutex_destroy(&lock_);
#endif
	}

	inline 
		u_locker::u_locker(u_lock &l) 
		: lock_(l) 
	{
		lock_.lock();
	}
	inline 
		u_locker::~u_locker(void) 
	{
		lock_.unlock();
	}

	inline bool 
		u_event::wait(const msec_t w/*= ___IPC_INFINITE*/) 
	{
#ifdef WIN32
		return WAIT_OBJECT_0 == ::WaitForSingleObject(event_, w);
#else
		if (___IPC_INFINITE != w) {
			assert(w > 0);
			struct timeval  now;
			struct timespec abs;
			gettimeofday(&now, NULL);
			long usec = (now.tv_usec + (w * 1000));
			abs.tv_sec = now.tv_sec + (usec / 1000000);
			abs.tv_nsec = (usec % 1000000) * 1000;
			u_locker locker(lock_);
			return pthread_cond_timedwait(&event_, &lock_.lock_, &abs);
		} else {
			u_locker locker(lock_);
			return pthread_cond_wait(&event_, &lock_.lock_);
		}
#endif
	}
	inline void 
		u_event::signal(void) 
	{
#ifdef WIN32
		::SetEvent(event_);
#else
		pthread_cond_signal(&event_);
#endif
	}
	inline 
		u_event::u_event(void) 
	{
#ifdef WIN32
		event_ = ::CreateEvent(NULL, FALSE, FALSE, NULL);
#else
		pthread_cond_init(&event_, NULL);
#endif
	}
	inline
		u_event::~u_event(void)
	{
#ifdef WIN32
		::CloseHandle(event_);
#else
		pthread_cond_destroy(&event_);
#endif
	}

	inline bool 
		u_cond::wait(const msec_t w/*= ___IPC_INFINITE*/) 
	{
#ifdef WIN32
		return 0 != ::SleepConditionVariableCS(&cond_, &lock_, w);
#else
		if (___IPC_INFINITE != w) {
			assert(w > 0);
			struct timeval  now;
			struct timespec abs;
			gettimeofday(&now, NULL);
			long usec = (now.tv_usec + (w * 1000));
			abs.tv_sec = now.tv_sec + (usec / 1000000);
			abs.tv_nsec = (usec % 1000000) * 1000;
			return pthread_cond_timedwait(&cond_, &lock_, &abs);
		} else {
			return pthread_cond_wait(&cond_, &lock_);
		}
#endif
	}
	inline void 
		u_cond::signal(bool all /* = false */)
	{
#ifdef WIN32
		all ? ::WakeAllConditionVariable(&cond_) : ::WakeConditionVariable(&cond_);
#else
		all ? pthread_cond_broadcast(&cond_) : pthread_cond_signal(&cond_);
#endif
	}
	inline 
		u_cond::u_cond(void) 
	{
#ifdef WIN32
		::InitializeConditionVariable(&cond_);
#else
		pthread_cond_init(&cond_, NULL);
#endif
	}
	inline
		u_cond::~u_cond(void)
	{
#ifdef WIN32
#else
		pthread_cond_destroy(&cond_);
#endif
	}

	inline bool 
		u_semp::wait(const msec_t w/*= ___IPC_INFINITE*/) 
	{
#ifdef WIN32
		return WAIT_OBJECT_0 == ::WaitForSingleObject(sem_, w);
#else
		//TODO: add timeout
		return 0 == sem_wait(&sem_);
#endif
	}
	inline void 
		u_semp::signal(const int inc/*= 1*/) 
	{
#ifdef WIN32
		::ReleaseSemaphore(sem_, inc, NULL);
#else //WIN32
		sem_post(&sem_);
#endif//WIN32
	}
	inline
		u_semp::u_semp(void) 
	{
#ifdef WIN32
		sem_ = ::CreateSemaphore(NULL, 0, 1024, NULL);
#else
		sem_init(&sem_, 0, 0);
#endif
	}
	inline
		u_semp::~u_semp(void) 
	{
#ifdef WIN32
		::CloseHandle(sem_);
#endif
	}

	inline i32_t
		u_atomic::exchange_and_add(volatile i32_t* val, i32_t add)
	{
#ifdef WIN32
		return (i32_t)::InterlockedExchangeAdd((long*)val, add);     
#else //WIN32
		i32_t r;
		__asm__ __volatile__
			(
			"lock\n\t"
			"xadd %1, %0":
		"+m"( *val ), "=r"( r ): // outputs (%0, %1)
		"1"( add ): // inputs (%2 == %1)
		"memory", "cc" // clobbers
			);
		return r;
#endif //WIN32
	}
	inline u32_t
		u_atomic::compare_and_swap(volatile u32_t* src, u32_t dst, u32_t cmp)
	{
#ifdef WIN32
		return (u32_t)::InterlockedCompareExchange((volatile long*)(src), dst, cmp);
#else //WIN32
		u32_t prev = cmp;
		asm volatile( "lock\n\t"
			"cmpxchg %3,%1"
			: "=a" (prev), "=m" (*(src))
			: "0" (prev), "r" (dst)
			: "memory", "cc");
		return prev;
#endif//WIN32
	}	

	inline i32_t
		u_i32mt::operator ++(void)
	{
		return u_atomic::exchange_and_add(&i_, +1 ) + 1;
	}
	inline i32_t
		u_i32mt::operator --(void)
	{
		return u_atomic::exchange_and_add(&i_, -1 ) - 1;
	}
	inline
		u_i32mt::operator i32_t(void)
	{
		return i_;
	}
	inline i32_t
		u_i32mt::operator =(i32_t i)
	{
		while (i != (i32_t)u_atomic::compare_and_swap((volatile u32_t*)&i_, i, i_));
		return i;
	}
	inline
		u_i32mt::u_i32mt(const i32_t i/*=0*/)
		: i_(i) 
	{
	}

	inline i32_t
		u_i32st::operator++(void)
	{
		return ++i_;
	}
	inline i32_t
		u_i32st::operator--(void)
	{
		return --i_;
	}
	inline
		u_i32st::operator i32_t(void)
	{
		return i_;
	}
	inline
		u_i32st::u_i32st(const i32_t i/*=0*/) 
		: i_(i) 
	{
	}

	template<typename T>
	inline void
		u_refer<T>::swap(u_refer& r)
	{
		T* t = obj_;
		obj_ = r.obj_;
		r.obj_ = t;
	}
	template<typename T>
	inline void
		u_refer<T>::operator =(T* o)
	{
		u_refer<T> t(o);
		this->swap(t);
	}
	template<typename T>
	inline void
		u_refer<T>::operator =(const u_refer& r)
	{
		u_refer<T> t(r);
		this->swap(t);
	}
	template<typename T>
	inline
		u_refer<T>::operator T*(void)
	{
		return obj_;
	}
	template<typename T>
	inline T*
		u_refer<T>::operator ->(void)
	{
		return obj_;
	}
	template<typename T>
	inline 
		u_refer<T>::u_refer(void)
	{
		obj_ = NULL;
	}
	template<typename T>
	inline 
		u_refer<T>::u_refer(T* o)
	{
		if(NULL != (obj_ = o))
			++(*obj_);
	}
	template<typename T>
	inline 
		u_refer<T>::u_refer(const u_refer& r)
	{
		*this = r;
	}
	template<typename T>
	inline 
		u_refer<T>::~u_refer(void)
	{
		if (NULL != obj_ && 0 == --(*obj_))
			delete obj_;
	}

} // namespace IPC

#endif//___IPC_IPC_H
