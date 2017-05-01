/** \file
\brief Multithreading and synchronization library
\author Adam Sawicki - sawickiap@poczta.onet.pl - http://asawicki.info/ \n

Part of CommonLib library. \n
Encoding UTF-8, end of line CR+LF \n
License: GNU LGPL. \n
Documentation: \ref Module_Threads \n
Module components: \ref code_threads
*/
#if defined(_MSC_VER) && (_MSC_VER >= 1200)
#pragma once
#endif
#ifndef COMMON_THREADS_H_
#define COMMON_THREADS_H_

namespace common
{

/** \addtogroup code_threads Threads Module
Dokumentacja: \ref Module_Threads \n
Nag³ówek: Threads.hpp */
//@{

/// \internal
class Thread_pimpl;
/// \internal
class Mutex_pimpl;
/// \internal
class Semaphore_pimpl;
/// \internal
class Cond_pimpl;
/// \internal
class Barrier_pimpl;
/// \internal
class Event_pimpl;

/// Klasa bazowa w¹tku.
/**
- Odziedzicz po tej klasie, ¿eby zdefiniowaæ w³asny typ w¹tku. Nadpisaæ metodê Thread::Run().
- Obiekt tej klasy reprezentuje pojedynczy w¹tek.
- W¹tek mo¿na odpaliæ jeden raz - Thread::Start().
- Przed zniszczeniem obiektu na odpalony w¹tek trzeba poczekaæ - Thread::Join().
- Obiekt mo¿na tworzyæ na stosie lub na stercie - obojêtne.
- Obiekt, jeœli alokowany new, trzeba zniszczyæ delete - sam siê nie zniszczy.
*/
class Thread
{
	DECLARE_NO_COPY_CLASS(Thread)
	friend class Thread_pimpl;

private:
	scoped_ptr<Thread_pimpl> pimpl;
	
protected:
	/// Napisz swoj¹ wersjê z kodem, który ma siê wykonaæ w osobnym w¹tku.
	virtual void Run() = 0;

	/// Koñczy dzia³anie w¹tku
	/**
	- Wywo³ywaæ z funkcji Run().
	- Lepiej jednak normalnie zakoñczyæ funkcjê Run.
	  Stosowanie Exit() nie ma sensu i jest niebezpieczne dla zasobów.
	*/
	void Exit();
	/// Oddaje sterowanie innemu w¹tkowi
	/**
	- Wywo³ywaæ z funkcji Run().
	- Nazwa taka dziwna, bo jakiœ cham zdefiniowa³ makro o nazwie Yield!
	*/
	void Yield_();

public:
	Thread();
	virtual ~Thread();

	/// Uruchamiamia w¹tek.
	void Start();
	/// Czeka na zakoñczenie w¹tku.
	/**
	- Jeœli w¹tek ju¿ siê zakoñczy³, wychodzi od razu.
	- Jeœli w¹tek jeszcze nie by³ odpalony albo ju¿ raz by³o testowane Join(), nic siê nie dzieje.
	*/
	void Join();

	/// Brutalnie zabija w¹tek - nie u¿ywaæ!
	void Kill();

// Rzeczy które nie wiem jak zrealizowaæ w pthreads - dzia³aj¹ tylko w Windows
#ifdef WIN32
	void* GetNativeHandle(); ///< Returns HANDLE.
	uint GetNativeId(); ///< Returns DWORD.
	/// Zwraca true, jeœli w¹tek jest w tej chwili uruchomiony i naprawdê jeszcze dzia³a
	bool IsRunning();
	/// Zatrzymuje w¹tek, wznawia w¹tek (posiada wewnêtrzny licznik zatrzymañ)
	/**
	- Dzia³a tylko miêdzy Start() a Join().
	- Raczej nie ma sensu tego u¿ywaæ.
	*/
	void Pause();
	void Resume();

	/** \name Priorytet w¹tku
	Prioritytet w¹tku jest liczb¹ ca³kowit¹. Te sta³e to niektóre wartoœci. Mog¹ byæ te¿ inne.
	Ustawianie i pobieranie priotytetu w¹tku dzia³a tylko miêdzy Start() a Join(). */
	//@{
	static const int PRIORITY_IDLE;
	static const int PRIORITY_VERY_LOW;
	static const int PRIORITY_LOW;
	static const int PRIORITY_DEFAULT;
	static const int PRIORITY_HIGH;
	static const int PRIORITY_VERY_HIGH;
	static const int PRIORITY_REALTIME;
	void SetPriority(int Priority);
	int GetPriority();

	void SetThreadName(const char *s);
	static void SetThreadNameById(uint threadId, const char *s);
	static void SetMainThreadName(const char *s);
	//@}
#endif
};

/// Muteks
/** Obiekt do wyznaczania sekcji krytycznej na wzajemnie wykluczaj¹c¹ siê wy³¹cznoœæ
jednego w¹tku.
*/
class Mutex
{
	DECLARE_NO_COPY_CLASS(Mutex)
	friend class Mutex_pimpl;
	friend class Cond;

private:
	scoped_ptr<Mutex_pimpl> pimpl;

public:
	/** \name Flagi bitowe do konstruktora */
	//@{
	/// Podaj, jeœli bêdziesz robi³ w ramach jednego w¹tku zagnie¿d¿one blokowania.
	static const uint FLAG_RECURSIVE    = 0x01;
	/// Podaj, jeœli bêdziesz u¿ywa³ metody TimeoutLock.
	static const uint FLAG_WAIT_TIMEOUT = 0x02;
	//@}

	Mutex(uint Flag);
	~Mutex();

	/// Blokuje muteks.
	void Lock();
	/// Odblokowuje muteks.
	void Unlock();
	/// Próbuje wejœæ do sekcji krytycznej.
	/**
	- Jeœli siê uda, wchodzi blokuj¹c muteks i zwraca true.
	- Jeœli siê nie uda bo muteks jest zablokowany przez inny w¹tek, nie blokuje sterowania tylko zawraca false.
	*/
	bool TryLock();
	/// Próbuje wejœæ do sekcji krytycznej czekaj¹c co najwy¿ej podany czas.
	/**
	- Jeœli siê uda, wchodzi blokuj¹c muteks i zwraca true.
	- Jeœli siê nie uda przez podany czas, bo muteks jest zablokowany przez inny w¹tek, zwraca false.
	*/
	bool TimeoutLock(uint Milliseconds);
};

/// Klasa pomagaj¹ca blokowaæ muteks
/**
- W konstruktorze blokuje, w destruktorze odblokowuje.
- U¿ywaj jej zamiast metod Mutex::Lock i Mutex::Unlock kiedy tylko mo¿na.
*/
class MutexLock
{
private:
	Mutex &m_Mutex;
	DECLARE_NO_COPY_CLASS(MutexLock)

public:
	MutexLock(Mutex &m) : m_Mutex(m) { m.Lock(); }
	~MutexLock() { m_Mutex.Unlock(); }
};

/**
Dla jeszcze wiêszej wygody, zamiast tworzyæ obiekt klasy MutexLock wystarczy na
pocz¹tku funkcji czy dowolnego bloku { } postawiæ to makro (M ma byæ typu
Mutex*).
*/
#define MUTEX_LOCK(M) common::MutexLock __mutex_lock_obj(M);

/// Semafor zliczaj¹cy
/**
- Posiada wewnêtrznie nieujemn¹ wartoœæ ca³kowit¹, któr¹ mo¿na sobie wyobraziæ
  jako liczbê dostêpnych zasobów. Nie ma do niej dostêpu bezpoœrednio.
- Wartoœæ maksymalna tej liczby nie jest okreœlona, jest jakaœ du¿a.
*/
class Semaphore
{
	DECLARE_NO_COPY_CLASS(Semaphore)

private:
	scoped_ptr<Semaphore_pimpl> pimpl;

public:
	Semaphore(uint InitialValue);
	~Semaphore();

	/// Opuszcza semafor - zmniejsza jego wartoœæ o jeden.
	/**
	Jeœli semafor jest zerowy, czeka a¿ ktoœ go podniesie zanim go opuœci i zwróci sterowanie.
	*/
	void P();
	/// Próbuje opuœciæ semafor - zmniejszyæ jego wartoœæ o jeden.
	/**
	- Jeœli siê uda, zmniejsza i zwraca true.
	- Jeœli siê nie uda bo wartoœæ wynosi 0, nie blokuje sterowania tylko zawraca false.
	*/
	bool TryP();
	/// Próbuje opuœciœ semafor czekaj¹c co najwy¿ej podany czas.
	/**
	- Jeœli siê uda, zmniejsza i zwraca true.
	- Jeœli siê nie uda przez podany czas, bo wartoœæ wynosi 0, zwraca false.
	*/
	bool TimeoutP(uint Milliseconds);

	/// Podnosi semafor zwiêkszaj¹c go o 1. Nigdy nie blokuje.
	void V();
	/// Podnosi semafor zwiêkszaj¹c go o ReleaseCount. Nigdy nie blokuje.
	void V(uint ReleaseCount);
};

/// Zmienna warunkowa
/**
Nie pamiêta ¿adnego swojego stanu. Pozwala za to zawiesiæ w¹tek ¿eby na niej
poczeka³ (Wait()) oraz wznowiæ niezdefiniowany jeden (Signal()) lub wszystkie
(Broadcast()) czekaj¹ce na niej w¹tki. Sensu nabiera dopiero kiedy zrozumie siê
sposób jej u¿ycia w kontekœcie wzorca monitora do sprawdzania warunku.
*/
class Cond
{
	DECLARE_NO_COPY_CLASS(Cond)

private:
	scoped_ptr<Cond_pimpl> pimpl;

public:
	Cond();
	~Cond();

	/// Odblokowuje muteks, zawiesza w¹tek czekaj¹c na sygna³, po doczekaniu siê blokuje muteks z powrotem.
	/** Muteks m musi byæ zablokowany w chwili tego wywo³ania. */
	void Wait(Mutex *m);
	/// Odblokowuje muteks, zawiesza w¹tek czekaj¹c na sygna³ przez co najwy¿ej podany czas, po doczekaniu siê blokuje muteks z powrotem.
	/**
	- Jeœli w tym czasie nast¹pi³o przerwanie czekania przez sygna³, zwraca true.
	- Muteks m musi byæ zablokowany w chwili tego wywo³ania.
	*/
	bool TimeoutWait(Mutex *m, uint Milliseconds);

	/// Odwiesza przypadkowy jeden czekaj¹cy w¹tek.
	/**
	- Jeœli ¿aden w¹tek nie czeka, nie robi nic.
	- Nie jest wymagane ¿eby jakiœ muteks musia³ byæ zablokowany w chwili tego wywo³ania.
	*/
	void Signal();
	/// Odwiesza wszystkie czekaj¹ce w¹tki.
	/**
	- Jeœli ¿aden w¹tek nie czeka, nie robi nic.
	- Nie jest wymagane ¿eby jakiœ muteks musia³ byæ zablokowany w chwili tego wywo³ania.
	*/
	void Broadcast();
};

/// Bariera
/**
W¹tki które wywo³uj¹ Wait() blokuj¹ siê czekaj¹c, a¿ siê ich uzbiera tyle
czekaj¹cych, ile wynosi przekazany do konstruktora NumThreads. Kiedy ostatni z
nich wywo³a Wait(), wszystkie ruszaj¹ dalej.
*/
class Barrier
{
	DECLARE_NO_COPY_CLASS(Barrier)

private:
	scoped_ptr<Barrier_pimpl> pimpl;

public:
	Barrier(uint NumThreads);
	~Barrier();

	void Wait();
};

/// Zdarzenie
/**
Ten obiekt, wzorowany na WinAPI. Jest trochê podobny do zmiennej warunkowej,
ale ma swój stan - true lub false.
*/
class Event
{
	DECLARE_NO_COPY_CLASS(Event);

private:
	scoped_ptr<Event_pimpl> pimpl;

public:
	enum TYPE
	{
		/** Zdarzenie samo przywróci siê do false, kiedy jakiœ jeden w¹tek doczeka siê true w wywo³aniu Wait.
		SetEvent() wznawia jeden w¹tek czekajacy na Wait. */
		TYPE_AUTO_RESET,
		/** Zdarzenie samo nie przywróci siê do false.
		SetEvent() wznawia wszystkie w¹tki czekaj¹ce na Wait. */
		TYPE_MANUAL_RESET,
	};

	Event(bool InitialState, TYPE Type);
	~Event();

	/// Zmienia stan na true.
	/**
	- Event::TYPE_AUTO_RESET: Jeœli jakieœ w¹tki czeka³y, wznawia jeden i przestawia z powrotem na false.
	- Event::TYPE_MANUAL_RESET: Jeœli jakieœ w¹tki czeka³y, wznawia je wszystkie.
	*/
	void Set();
	/// Zmienia stan na false.
	void Reset();

	/// Czeka, a¿ stan bêdzie true.
	/**
	- Jeœli ju¿ jest, zwraca sterowanie od razu.
	- Event::TYPE_AUTO_RESET: Kiedy siê doczeka, przestawia go z powrotem na false.
	*/
	void Wait();
	/// Zwraca stan
	/** Event::TYPE_AUTO_RESET: Jeœli by³ true, przestawia go z powrotem na false. */
	bool Test();
	/// Czeka, a¿ stan bêdzie true przez co najwy¿ej podany czas.
	/**
	- Jeœli jest albo siê doczeka a¿ bêdzie, zwraca true.
	- Jeœli siê nie doczeka przez podany czas (stan jest ca³y czas false), zwraca false.
	- Event::TYPE_AUTO_RESET: Kiedy siê doczeka na stan true, przestawia go z powrotem na false.
	*/
	bool TimeoutWait(uint Milliseconds);
};

class RWLock
{
	DECLARE_NO_COPY_CLASS(RWLock)

public:
	RWLock() : m_Readers(0), m_ReadersEvent(true, Event::TYPE_AUTO_RESET), m_DataEvent(true, Event::TYPE_AUTO_RESET) { }

	void LockWrite() { m_DataEvent.Wait(); }
	bool TryLockWrite() { return m_DataEvent.Test(); }
	bool TimeoutLockWrite(uint Milliseconds) { return m_DataEvent.TimeoutWait(Milliseconds); }
	void UnlockWrite() { m_DataEvent.Set(); }

	void LockRead()
	{
		m_ReadersEvent.Wait();
		if (m_Readers++ == 0) m_DataEvent.Wait();
		m_ReadersEvent.Set();
	}
	void UnlockRead()
	{
		m_ReadersEvent.Wait();
		if (--m_Readers == 0) m_DataEvent.Set();
		m_ReadersEvent.Set();
	}

private:
	size_t m_Readers;
	Event m_ReadersEvent, m_DataEvent;
};

class ReadLock
{
	DECLARE_NO_COPY_CLASS(ReadLock)
public:
	ReadLock(RWLock &lock) : m_Lock(lock) { lock.LockRead(); }
	~ReadLock() { m_Lock.UnlockRead(); }
private:
	RWLock &m_Lock;
};

class WriteLock
{
	DECLARE_NO_COPY_CLASS(WriteLock)
public:
	WriteLock(RWLock &lock) : m_Lock(lock) { lock.LockWrite(); }
	~WriteLock() { m_Lock.UnlockWrite(); }
private:
	RWLock &m_Lock;
};

//@}
// code_threads

} // namespace common

#endif
