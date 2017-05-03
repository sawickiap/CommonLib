/** \file
\brief Multithreading and synchronization library
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Threads \n
Module components: \ref code_threads
*/
#include "Base.hpp"
#ifdef _WIN32
	#define _WIN32_WINNT 0x0400 // dla windows.h dla SwitchToThread
	#include <windows.h>
	#include <process.h> // dla _beginthreadex
#else
	#include <pthread.h>
	#include <semaphore.h>
	#include <sched.h> // dla sched_yield
	#include <time.h> // dla pthread_mutex_timedlock
#endif
#include "Error.hpp"
#include "Threads.hpp"


namespace common
{

#ifndef _WIN32
	// [Wewnêtrzna]
	void MillisecondsToAbsTimespec(struct timespec *Out, uint Milliseconds)
	{
		time_t Now;
		time(&Now);
		Out->tv_sec = Now + Milliseconds / 1000;
		Out->tv_nsec = Milliseconds % 1000 * 1000000;
	}
#endif

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Thread

#ifdef _WIN32
	class Thread_pimpl
	{
	public:
		scoped_handle<HANDLE, CloseHandlePolicy> ThreadHandle;
		scoped_handle<HANDLE, CloseHandlePolicy> CompletionEvent;
		bool Running;
		DWORD ThreadId;

		static DWORD WINAPI ThreadFunc(LPVOID Arg);

		Thread_pimpl() : ThreadHandle(NULL), CompletionEvent(NULL), Running(false), ThreadId(0) { }
		~Thread_pimpl() { }
	};

	DWORD WINAPI Thread_pimpl::ThreadFunc(LPVOID Arg)
	{
		assert(Arg != NULL);

		Thread *t = (Thread*)Arg;

		try
		{
			t->Run();
		}
		catch (...)
		{
			assert(0 && "Uncaught exception in thread.");
		}

		SetEvent(t->pimpl->CompletionEvent.get());

		return 0;
	}

	void* Thread::GetNativeHandle()
	{
		return pimpl->ThreadHandle.get();
	}
	
	uint Thread::GetNativeId()
	{
		return pimpl->ThreadId;
	}

	void Thread::Exit()
	{
		ExitThread((DWORD)-2);
	}

	void Thread::Yield_()
	{
		SwitchToThread();
	}

	Thread::Thread() :
		pimpl(new Thread_pimpl)
	{
	}

	Thread::~Thread()
	{
		assert(pimpl->Running == false && "Thread: Destructor called while no Join called before.");
	}

	void Thread::Start()
	{
		assert(pimpl->Running == false && "Thread already started.");
		pimpl->Running = true;

		pimpl->CompletionEvent.reset(CreateEvent(NULL, TRUE, FALSE, NULL));
		if (pimpl->CompletionEvent == NULL)
			throw Win32Error(_T("Cannot create CompletionEvent."), __TFILE__, __LINE__);

		// Podobno _beginthreadex jest lepsze od CreateThread, na podst. ksi¹¿ki "Modern Multithreading" (Carver, Tai, wyd. Wiley)
		//pimpl->ThreadHandle.reset((HANDLE)_beginthreadex(NULL, 0, &Thread_pimpl::ThreadFunc, this, 0, NULL));
		// but _beginthreadex cannot return thread id.
		pimpl->ThreadHandle.reset( CreateThread(NULL, 0, &Thread_pimpl::ThreadFunc, this, 0, &pimpl->ThreadId) );
		
		// Jeœli nie tworzê suspended, w¹tek móg³ siê szybko skoñczyæ i wtedy funkcja tworz¹ca mog³a zwróciæ nie to co trzeba.
		//if (pimpl->ThreadHandle == NULL)
		//	throw ErrnoError(_T("Cannot create thread."), __TFILE__, __LINE__);
	}

	void Thread::Join()
	{
		if (!pimpl->Running) return;

		pimpl->Running = false;
		
		DWORD R = WaitForSingleObject(pimpl->CompletionEvent.get(), INFINITE);
		if (R == WAIT_FAILED)
			throw Win32Error(_T("Cannot wait for thread."), __TFILE__, __LINE__);

		pimpl->ThreadHandle.reset(NULL);
		pimpl->CompletionEvent.reset(NULL);
	}
	
	void Thread::Kill()
	{
		if (!pimpl->Running) return;

		TerminateThread(pimpl->ThreadHandle.get(), (DWORD)-1); // B³êdu nie sprawdzam

		pimpl->CompletionEvent.reset(NULL);
		pimpl->ThreadHandle.reset(NULL);
		pimpl->Running = false;
	}

	void Thread::Pause()
	{
		if (!pimpl->Running) return;

		SuspendThread(pimpl->ThreadHandle.get());
	}

	void Thread::Resume()
	{
		if (!pimpl->Running) return;

		ResumeThread(pimpl->ThreadHandle.get());
	}

	bool Thread::IsRunning()
	{
		if (!pimpl->Running) return false;

		DWORD R = WaitForSingleObject(pimpl->ThreadHandle.get(), 0);
		if (R == WAIT_OBJECT_0)
			return false;
		else if (R == WAIT_TIMEOUT)
			return true;
		else
			return false; // B³¹d
	}

	const int Thread::PRIORITY_IDLE      = THREAD_PRIORITY_IDLE;
	const int Thread::PRIORITY_VERY_LOW  = THREAD_PRIORITY_LOWEST;
	const int Thread::PRIORITY_LOW       = THREAD_PRIORITY_BELOW_NORMAL;
	const int Thread::PRIORITY_DEFAULT   = THREAD_PRIORITY_NORMAL;
	const int Thread::PRIORITY_HIGH      = THREAD_PRIORITY_ABOVE_NORMAL;
	const int Thread::PRIORITY_VERY_HIGH = THREAD_PRIORITY_HIGHEST;
	const int Thread::PRIORITY_REALTIME  = THREAD_PRIORITY_TIME_CRITICAL;

	void Thread::SetPriority(int Priority)
	{
		if (!pimpl->Running) return;

		SetThreadPriority(pimpl->ThreadHandle.get(), Priority);
	}

	int Thread::GetPriority()
	{
		if (!pimpl->Running) return PRIORITY_DEFAULT;

		int R = GetThreadPriority(pimpl->ThreadHandle.get());
		if (R == THREAD_PRIORITY_ERROR_RETURN)
			return PRIORITY_DEFAULT;
		return R;
	}

	void Thread::SetThreadName(const char *s)
	{
		SetThreadNameById(pimpl->ThreadId, s);
	}

	void Thread::SetThreadNameById(uint threadId, const char *s)
	{
		assert(threadId);

		/*
		Code from MSDN, "How to: Set a Thread Name in Native Code"
		http://msdn.microsoft.com/en-us/library/xcb2z8hs.aspx
		*/

		#pragma pack(push,8)
			typedef struct tagTHREADNAME_INFO
			{
				DWORD dwType; // Must be 0x1000.
				LPCSTR szName; // Pointer to name (in user addr space).
				DWORD dwThreadID; // Thread ID (-1=caller thread).
				DWORD dwFlags; // Reserved for future use, must be zero.
			} THREADNAME_INFO;
		#pragma pack(pop)

		const DWORD MS_VC_EXCEPTION = 0x406D1388;

		Sleep(10);
		THREADNAME_INFO info;
		info.dwType = 0x1000;
		info.szName = s;
		info.dwThreadID = threadId;
		info.dwFlags = 0;

		__try
		{
			RaiseException( MS_VC_EXCEPTION, 0, sizeof(info)/sizeof(ULONG_PTR), (ULONG_PTR*)&info );
		}
		__except(EXCEPTION_EXECUTE_HANDLER)
		{
		}
	}

	void Thread::SetMainThreadName(const char *s)
	{
		SetThreadNameById(-1, s);
	}

#else
	class Thread_pimpl
	{
	public:
		pthread_t ThreadId;
		bool Running;
		
		static void * ThreadFunc(void *Arg);
	};

	void * Thread_pimpl::ThreadFunc(void *Arg)
	{
		int Foo;
		pthread_setcanceltype(PTHREAD_CANCEL_ASYNCHRONOUS, &Foo);
		
		assert(Arg != NULL);

		Thread *t = (Thread*)Arg;

		try
		{
			t->Run();
		}
		catch (...)
		{
			assert(0 && "Uncaught exception in thread.");
		}

		return NULL;
	}

	void Thread::Exit()
	{
		pthread_exit(NULL);
	}

	void Thread::Yield_()
	{
		sched_yield();
	}

	Thread::Thread() :
		pimpl(new Thread_pimpl)
	{
		pimpl->Running = false;
	}

	Thread::~Thread()
	{
		assert(pimpl->Running == false && "Thread: Destructor called while no Join called before.");
	}

	void Thread::Start()
	{
		assert(pimpl->Running == false && "Thread already started.");
		pimpl->Running = true;
		
		pthread_attr_t Attr;
		pthread_attr_init(&Attr);
		pthread_attr_setscope(&Attr, PTHREAD_SCOPE_SYSTEM);
		int R = pthread_create(&pimpl->ThreadId, &Attr, &Thread_pimpl::ThreadFunc, this);
		if (R != 0)
			throw ErrnoError(R, _T("Cannot create thread."), __TFILE__, __LINE__);
		pthread_attr_destroy(&Attr);
	}

	void Thread::Join()
	{
		if (pimpl->Running == false) return;
		
		pimpl->Running = false;
		
		int R = pthread_join(pimpl->ThreadId, NULL);
		if (R != 0)
			throw ErrnoError(R, _T("Cannot wait for thread."), __TFILE__, __LINE__);
	}

	void Thread::Kill()
	{
		if (!pimpl->Running) return;
		
		pthread_cancel(pimpl->ThreadId); // Nie sprawdzam b³êdu

		pimpl->Running = false;
	}

#endif

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Mutex

#ifdef _WIN32

	/* W Win32 i sekcja krytyczna i muteks s¹ rekursywne, ale sekcja krytyczna nie pozwala czekaæ czasowo. */

	class Mutex_pimpl
	{
	public:
		// Jeœli Mutex != NULL, jest muteks. Jeœli równy NULL, jest sekcja krytyczna.
		CRITICAL_SECTION CriticalSection;
		HANDLE Mutex;
	};

	Mutex::Mutex(uint Flag) :
		pimpl(new Mutex_pimpl)
	{
		// Muteks
		if ((Flag & FLAG_WAIT_TIMEOUT) != 0)
		{
			pimpl->Mutex = CreateMutex(NULL, FALSE, NULL);
			if (pimpl->Mutex == NULL)
				throw Win32Error(_T("Cannot create mutex."), __TFILE__, __LINE__);
		}
		// Sekcja krytyczna
		else
		{
			InitializeCriticalSection(&pimpl->CriticalSection);
			pimpl->Mutex = NULL;
		}
	}

	Mutex::~Mutex()
	{
		// Sekcja krytyczna
		if (pimpl->Mutex == NULL)
			DeleteCriticalSection(&pimpl->CriticalSection);
		// Muteks
		else
			CloseHandle(pimpl->Mutex);
	}

	void Mutex::Lock()
	{
		// Sekcja krytyczna
		if (pimpl->Mutex == NULL)
			EnterCriticalSection(&pimpl->CriticalSection);
		// Muteks
		else
			WaitForSingleObject(pimpl->Mutex, INFINITE);
	}

	void Mutex::Unlock()
	{
		// Sekcja krytyczna
		if (pimpl->Mutex == NULL)
			LeaveCriticalSection(&pimpl->CriticalSection);
		// Muteks
		else
			ReleaseMutex(pimpl->Mutex);
	}

	bool Mutex::TryLock()
	{
		// Sekcja krytyczna
		if (pimpl->Mutex == NULL)
			return ( TryEnterCriticalSection(&pimpl->CriticalSection) != 0 );
		// Muteks
		else
			return ( WaitForSingleObject(pimpl->Mutex, 0) != WAIT_TIMEOUT );
	}

	bool Mutex::TimeoutLock(uint Milliseconds)
	{
		// Musi byæ stworzony z FLAG_WAIT_TIMEOUT - to na pewno musi byæ muteks nie sekcja krytyczna
		assert(pimpl->Mutex != NULL && "LockedTimeout called while mutex was created without FLAG_WAIT_TIMEOUT flag.");

		return ( WaitForSingleObject(pimpl->Mutex, Milliseconds) != WAIT_TIMEOUT );
	}

#else

	class Mutex_pimpl
	{
	public:
		pthread_mutex_t Mutex;
	};

	Mutex::Mutex(uint Flag) :
		pimpl(new Mutex_pimpl)
	{
		int R;
		if ((Flag & FLAG_RECURSIVE) != 0)
		{
			pthread_mutexattr_t Attr;
			pthread_mutexattr_init(&Attr);
			pthread_mutexattr_settype(&Attr, PTHREAD_MUTEX_RECURSIVE);
			R = pthread_mutex_init(&pimpl->Mutex, &Attr);
			pthread_mutexattr_destroy(&Attr);
		}
		else
			R = pthread_mutex_init(&pimpl->Mutex, NULL);
		if (R != 0)
			throw ErrnoError(R, _T("Cannot create mutex."), __TFILE__, __LINE__);
	}

	Mutex::~Mutex()
	{
		pthread_mutex_destroy(&pimpl->Mutex);
	}

	void Mutex::Lock()
	{
		pthread_mutex_lock(&pimpl->Mutex);
	}

	void Mutex::Unlock()
	{
		pthread_mutex_unlock(&pimpl->Mutex);
	}

	bool Mutex::TryLock()
	{
		return ( pthread_mutex_trylock(&pimpl->Mutex) == 0 );
	}

	bool Mutex::TimeoutLock(uint Milliseconds)
	{
		struct timespec Time;
		MillisecondsToAbsTimespec(&Time, Milliseconds);
		
		return ( pthread_mutex_timedlock(&pimpl->Mutex, &Time) == 0 );
	}

#endif

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Semaphore

#ifdef _WIN32

	class Semaphore_pimpl
	{
	public:
		scoped_handle<HANDLE, CloseHandlePolicy> SemaphoreHandle;

		Semaphore_pimpl() : SemaphoreHandle(NULL) { }
	};

	Semaphore::Semaphore(uint InitialValue) :
		pimpl(new Semaphore_pimpl)
	{
		pimpl->SemaphoreHandle.reset( CreateSemaphore(NULL, (LONG)InitialValue, MAXINT32, NULL) );
		if (pimpl->SemaphoreHandle == NULL)
			throw Win32Error(_T("Cannot create semaphore."), __TFILE__, __LINE__);
	}

	Semaphore::~Semaphore()
	{
	}

	void Semaphore::P()
	{
		WaitForSingleObject(pimpl->SemaphoreHandle.get(), INFINITE);
	}

	bool Semaphore::TryP()
	{
		return ( WaitForSingleObject(pimpl->SemaphoreHandle.get(), 0) != WAIT_TIMEOUT );
	}

	bool Semaphore::TimeoutP(uint Milliseconds)
	{
		return ( WaitForSingleObject(pimpl->SemaphoreHandle.get(), Milliseconds) != WAIT_TIMEOUT );
	}

	void Semaphore::V()
	{
		ReleaseSemaphore(pimpl->SemaphoreHandle.get(), 1, NULL);
	}

	void Semaphore::V(uint ReleaseCount)
	{
		ReleaseSemaphore(pimpl->SemaphoreHandle.get(), (LONG)ReleaseCount, NULL);
	}

#else

	class Semaphore_pimpl
	{
	public:
		sem_t s;
	};

	Semaphore::Semaphore(uint InitialValue) :
		pimpl(new Semaphore_pimpl)
	{
		if (sem_init(&pimpl->s, 0, InitialValue) != 0)
			throw ErrnoError(_T("Cannot create semaphore."), __TFILE__, __LINE__);
	}

	Semaphore::~Semaphore()
	{
		sem_destroy(&pimpl->s);
	}

	void Semaphore::P()
	{
		sem_wait(&pimpl->s);
	}

	bool Semaphore::TryP()
	{
		return ( sem_trywait(&pimpl->s) == 0 );
	}

	bool Semaphore::TimeoutP(uint Milliseconds)
	{
		struct timespec Time;
		MillisecondsToAbsTimespec(&Time, Milliseconds);
		
		return ( sem_timedwait(&pimpl->s, &Time) == 0 );
	}

	void Semaphore::V()
	{
		sem_post(&pimpl->s);
	}

	void Semaphore::V(uint ReleaseCount)
	{
		for (uint i = 0; i < ReleaseCount; i++)
			sem_post(&pimpl->s);
	}

#endif

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Cond

#ifdef _WIN32

/*
WinAPI nie ma zmiennych kondycyjnych takich jak Pthreads czy Java i bardzo
trudno je napisaæ. Jedno z rozwi¹zañ to wynurzenia pewnego profesora st¹d:
http://www.cs.wustl.edu/~schmidt/win32-cv-1.html. Inne, du¿o prostsze i te¿
wygl¹daj¹ce na poprawne to klasa wxCondition z biblioteki wxWidgets. W³aœnie na
niej siê tutaj wzorowa³em.
*/

	class Cond_pimpl
	{
	public:
		uint NumWaiters;
		Mutex NumWaitersMutex;
		Semaphore Sem;

		Cond_pimpl() : NumWaiters(0), NumWaitersMutex(0), Sem(0) { }
	};

	Cond::Cond() :
		pimpl(new Cond_pimpl)
	{
	}

	Cond::~Cond()
	{
	}

	void Cond::Wait(Mutex *m)
	{
		{
			MUTEX_LOCK(pimpl->NumWaitersMutex);
			pimpl->NumWaiters++;
		}

		m->Unlock();

		// a potential race condition can occur here
		//
		// after a thread increments m_numWaiters, and unlocks the mutex and before
		// the semaphore.Wait() is called, if another thread can cause a signal to
		// be generated
		//
		// this race condition is handled by using a semaphore and incrementing the
		// semaphore only if m_numWaiters is greater that zero since the semaphore,
		// can 'remember' signals the race condition will not occur

		pimpl->Sem.P();

		m->Lock();
	}


	bool Cond::TimeoutWait(Mutex *m, uint Milliseconds)
	{
		{
			MUTEX_LOCK(pimpl->NumWaitersMutex);
			pimpl->NumWaiters++;
		}

		m->Unlock();

		if (pimpl->Sem.TimeoutP(Milliseconds))
		{
			m->Lock();
			return true;
		}
		else
		{
			// another potential race condition exists here it is caused when a
			// 'waiting' thread times out, and returns from WaitForSingleObject,
			// but has not yet decremented m_numWaiters
			//
			// at this point if another thread calls signal() then the semaphore
			// will be incremented, but the waiting thread will miss it.
			//
			// to handle this particular case, the waiting thread calls
			// WaitForSingleObject again with a timeout of 0, after locking
			// m_csWaiters. This call does not block because of the zero
			// timeout, but will allow the waiting thread to catch the missed
			// signals.

			MUTEX_LOCK(pimpl->NumWaitersMutex);

			if (!pimpl->Sem.TryP())
				pimpl->NumWaiters--;

			m->Lock();

			return false;
		}
	}

	void Cond::Signal()
	{
		MUTEX_LOCK(pimpl->NumWaitersMutex);

		if (pimpl->NumWaiters > 0)
		{
			pimpl->Sem.V();
			pimpl->NumWaiters--;
		}
	}

	void Cond::Broadcast()
	{
		MUTEX_LOCK(pimpl->NumWaitersMutex);

		while (pimpl->NumWaiters > 0)
		{
			pimpl->Sem.V();
			pimpl->NumWaiters--;
		}
	}

#else

	class Cond_pimpl
	{
	public:
		pthread_cond_t c;
	};

	Cond::Cond() :
		pimpl(new Cond_pimpl)
	{
		int R = pthread_cond_init(&pimpl->c, NULL);
		if (R != 0)
			throw ErrnoError(R, _T("Cannot create conditional variable."), __TFILE__, __LINE__);
	}

	Cond::~Cond()
	{
		pthread_cond_destroy(&pimpl->c);
	}

	void Cond::Wait(Mutex *m)
	{
		pthread_cond_wait(&pimpl->c, &m->pimpl->Mutex);
	}

	bool Cond::TimeoutWait(Mutex *m, uint Milliseconds)
	{
		struct timespec Time;
		MillisecondsToAbsTimespec(&Time, Milliseconds);
		
		return ( pthread_cond_timedwait(&pimpl->c, &m->pimpl->Mutex, &Time) == 0 );
	}

	void Cond::Signal()
	{
		pthread_cond_signal(&pimpl->c);
	}

	void Cond::Broadcast()
	{
		pthread_cond_broadcast(&pimpl->c);
	}

#endif

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Barrier

#ifdef _WIN32

	class Barrier_pimpl
	{
	public:
		uint NumThreads; // Liczba w¹tków do wstrzymania
		uint NumWaiting; // Liczba aktualnie czekaj¹cych w¹tków (w³aœciwie to liczba w¹tków na które jeszcze czekamy)
		Mutex m;
		Cond c;

		Barrier_pimpl(uint NumThreads) : NumThreads(NumThreads), NumWaiting(NumThreads-1), m(0), c() { }
	};

	Barrier::Barrier(uint NumThreads) :
		pimpl(new Barrier_pimpl(NumThreads))
	{
		assert(NumThreads > 0);
	}

	Barrier::~Barrier()
	{
	}

	void Barrier::Wait()
	{
		MUTEX_LOCK(pimpl->m);

		if (pimpl->NumWaiting > 0)
		{
			pimpl->NumWaiting--;
			pimpl->c.Wait(&pimpl->m);
		}
		else
		{
			pimpl->NumWaiting = pimpl->NumThreads - 1;
			pimpl->c.Broadcast();
		}
	}

#else

	class Barrier_pimpl
	{
	public:
		pthread_barrier_t B;
	};

	Barrier::Barrier(uint NumThreads) :
		pimpl(new Barrier_pimpl)
	{
		assert(NumThreads > 0);

		int R = pthread_barrier_init(&pimpl->B, NULL, NumThreads);
		if (R != 0)
			throw ErrnoError(R, _T("Cannot create barrier."), __TFILE__, __LINE__);
	}

	Barrier::~Barrier()
	{
		pthread_barrier_destroy(&pimpl->B);
	}

	void Barrier::Wait()
	{
		pthread_barrier_wait(&pimpl->B);
	}

#endif

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Klasa Event

#ifdef _WIN32

	class Event_pimpl
	{
	public:
		scoped_handle<HANDLE, CloseHandlePolicy> E;

		Event_pimpl() : E(NULL) { }
	};

	Event::Event(bool InitialState, TYPE Type) :
		pimpl(new Event_pimpl)
	{
		pimpl->E.reset(CreateEvent(
			NULL,
			(Type == TYPE_MANUAL_RESET ? TRUE : FALSE),
			(InitialState ? TRUE : FALSE),
			NULL));
		if (pimpl->E == NULL)
			throw Error(_T("Cannot create event."), __TFILE__, __LINE__);
	}

	Event::~Event()
	{
	}

	void Event::Set()
	{
		SetEvent(pimpl->E.get());
	}

	void Event::Reset()
	{
		ResetEvent(pimpl->E.get());
	}

	void Event::Wait()
	{
		WaitForSingleObject(pimpl->E.get(), INFINITE);
	}

	bool Event::Test()
	{
		return ( WaitForSingleObject(pimpl->E.get(), 0) != WAIT_TIMEOUT );
	}

	bool Event::TimeoutWait(uint Milliseconds)
	{
		return ( WaitForSingleObject(pimpl->E.get(), Milliseconds) != WAIT_TIMEOUT );
	}

#else

	class Event_pimpl
	{
	public:
		Event::TYPE Type;
		bool State;
		Mutex M;
		Cond C;
		
		Event_pimpl(bool InitialState, Event::TYPE Type) : Type(Type), State(InitialState), M(0) { }
	};

	Event::Event(bool InitialState, TYPE Type) :
		pimpl(new Event_pimpl(InitialState, Type))
	{
	}

	Event::~Event()
	{
	}

	void Event::Set()
	{
		MUTEX_LOCK(&pimpl->M);
		
		pimpl->State = true;
		
		if (pimpl->Type == TYPE_AUTO_RESET)
			pimpl->C.Signal();
		else // TYPE_MANUAL_RESET
			pimpl->C.Broadcast();
	}

	void Event::Reset()
	{
		MUTEX_LOCK(&pimpl->M);
		
		pimpl->State = false;
	}

	bool Event::Test()
	{
		MUTEX_LOCK(&pimpl->M);
		
		if (pimpl->State == true)
		{
			if (pimpl->Type == TYPE_AUTO_RESET)
				pimpl->State = false;
			return true;
		}
		else
			return false;
	}

	void Event::Wait()
	{
		MUTEX_LOCK(&pimpl->M);
		
		while (pimpl->State == false) // Nie wystarczy³by if?
			pimpl->C.Wait(&pimpl->M);
		
		if (pimpl->Type == TYPE_AUTO_RESET)
			pimpl->State = false;
	}

	bool Event::TimeoutWait(uint Milliseconds)
	{
		MUTEX_LOCK(&pimpl->M);
		
		if (pimpl->State == false)
		{
			if (!pimpl->C.TimeoutWait(&pimpl->M, Milliseconds))
				return false;
		}
		
		if (pimpl->Type == TYPE_AUTO_RESET)
			pimpl->State = false;
		
		return true;
	}

#endif

} // namespace common
