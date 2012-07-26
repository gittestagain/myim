#include "ipc.h"
#include "util.h"

using namespace ipc;

class u_thread_impl
{
public:
	bool start(void);
	void join(msec_t wait);

	u_thread_impl(u_thread *wrap);

	enum { ST_RUN, ST_EXIT, ST_JOIN };
	thread_t           tuid_;
	volatile int       stat_;
	u_thread          *wrap_;
};

#ifdef WIN32
#define ___IPC_IPC_THREAD_RETURN u32_t __stdcall
#else //WIN32
#define ___IPC_IPC_THREAD_RETURN void* 
#endif//WIN32

static ___IPC_IPC_THREAD_RETURN
__thread_func(void *p)
{
	u_thread *wrap = (u_thread*)p;
	::srand((unsigned int)u_time::msec_abs());
	wrap->loop();
	return 0;
}
inline bool
u_thread_impl::start(void)
{
	if (ST_JOIN == stat_) {
		stat_ = ST_RUN;
#ifdef WIN32
		u32_t t;
		tuid_ = (thread_t)::_beginthreadex(NULL, 0, __thread_func, wrap_, 0, &t);
#else //WIN32
		int ret = pthread_create(&tuid_, NULL, __thread_func, (void*)wrap_);
#endif//WIN32

		if (0 == tuid_)  stat_ = ST_JOIN;
	}
	return 0 != tuid_;
}
inline void
u_thread_impl::join(msec_t wait)
{
	if (ST_JOIN != stat_) {
#ifdef WIN32
		::WaitForSingleObject(tuid_, wait);
		::CloseHandle(tuid_);
#else // WIN32
		if (wait > 0 && wait < ___IPC_INFINITE) {
			u32_t end = u_time::msec() + wait;
			while (ST_EXIT != stat_ && end > u_time::msec()) u_misc::msleep(1);
			if (ST_EXIT == stat_) pthread_join(tuid_, NULL);
		} else {
			pthread_join(tuid_, NULL);
		}
#endif // WIN32

		stat_ = ST_JOIN;
		tuid_ = 0;
	}
}
inline
u_thread_impl::u_thread_impl(u_thread *wrap)
{
	tuid_ = 0;
	stat_ = ST_JOIN;
	wrap_ = wrap;
}

namespace ipc {

	bool
		u_thread::init(void)
	{
		return true;
	}
	void
		u_thread::fini(void)
	{
		//cout<<"hello"<<endl;
	}
	void
		u_thread::loop(void)
	{
	}
	void
		u_thread::exit(void)
	{
		
	}
	bool
		u_thread::start(void)
	{
		if (NULL == impl_) {
			u_thread_impl *impl = new u_thread_impl(this);
			if (!this->init() || !impl->start()) {
				this->fini();
				delete impl;
			} else impl_ = impl;
		}

		return NULL != impl_;
	}
	void 
		u_thread::stop(void)
	{
		if (NULL != impl_) this->exit();
	}
	void
		u_thread::join(msec_t wait/* = 0 */)
	{
		if (NULL != impl_) {
			u_thread_impl *impl = (u_thread_impl*) impl_;
			impl->join(wait);
			this->fini();
			delete impl;
			impl_ = NULL;
		}
	}
	u_thread::u_thread(void)
	{
		impl_ = NULL;
	}
	u_thread::~u_thread(void)
	{
		u_thread_impl *impl = (u_thread_impl*) impl_;
		if (NULL != impl) delete impl;
	}
} // namespace IPC
