#include "../Common/Base.hpp"
#include "../Common/FreeList.hpp"
#include "../Common/Error.hpp"
#include "../Common/Math.hpp"
#include "../Common/Profiler.hpp"
#include "../Common/DateTime.hpp"
#include "../Common/Threads.hpp"
#include "../Common/Stream.hpp"
#include "../Common/Files.hpp"
#include "../Common/Tokenizer.hpp"
#include "../Common/TokDoc.hpp"
#include "../Common/ZlibUtils.hpp"
#include "../Common/Logger.hpp"
#include "../Common/ObjList.hpp"

#ifdef WIN32
	#include "../Common/BstrString.hpp"
#endif

#include <iostream>
#include <ios>
#include <queue>

using namespace std;
using namespace common;

#ifdef _UNICODE
	#define tcout wcout
	#define tcin wcin
#else
	#define tcout cout
	#define tcin cin
#endif

static Profiler g_Profiler;

void WriteLine(const tstring &s)
{
	tcout << s << endl;
}

void PrintProfile()
{
	tstring Profile;
	g_Profiler.FormatString(&Profile, PROFILER_UNITS_MILLISECONDS);
	WriteLine(Profile);
}

//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH

class SingletonClass : public Singleton<SingletonClass>
{
public:
	SingletonClass()
	{
		tcout << _T("  Singleton Constructor") << endl;
	}

	~SingletonClass()
	{
		tcout << _T("  Singleton Destructor") << endl;
	}

	void Method()
	{
		tcout << _T("  Singleton Method") << endl;
	}
};

void Initialize()
{
	GameTime::Initialize();
}

void TestBase()
{
	WriteLine(_T("==================== BASE ===================="));

	{
		tcout << _T("Conversion between uint32, unsigned and size_t - shouldn't be compiler warning") << endl;
		const uint32 cu4 = 1234567890;
		const unsigned cu = 123456789;
		const size_t cs = 12345678;
		uint32 u4 = cu; u4 = cs;
		unsigned u = cu4; u = cs;
		size_t s = cu4; s = cu;
	}

	{
		tcout << _T("Basic numeric types:") << endl;
		tcout << _T("  sizeof(int8) = ") << sizeof(int8) << endl;
		tcout << _T("  sizeof(int16) = ") << sizeof(int16) << endl;
		tcout << _T("  sizeof(int32) = ") << sizeof(int32) << endl;
		tcout << _T("  sizeof(int64) = ") << sizeof(int64) << endl;
		tcout << _T("  sizeof(uint8) = ") << sizeof(uint8) << endl;
		tcout << _T("  sizeof(uint16) = ") << sizeof(uint16) << endl;
		tcout << _T("  sizeof(uint32) = ") << sizeof(uint32) << endl;
		tcout << _T("  sizeof(uint64) = ") << sizeof(uint64) << endl;
	}

	{
		tcout << _T("absolute_cast") << endl;
		const int ci = 123456789;
		tcout << _T("  int = ") << ci << endl;
		float f = absolute_cast<float>(ci);
		tcout << _T("  float = ") << f << endl;
		uint32 ui = absolute_cast<uint32>(ci);
		tcout << _T("  uint32 = ") << ui << endl;
	}

	{
		tcout << _T("Singleton:") << endl;
		tcout << _T("- Using singleton for the first time") << endl;
		SingletonClass::GetInstance().Method();
		tcout << _T("- Using singleton for second time") << endl;
		SingletonClass::GetInstance().Method();
	}

	{
		tcout << _T("Safe arithemtic:") << endl;
		tcout << _T("  SafeAdd<uint8>(120, 250) = ") << (int)safe_add<uint8>(120, 250) << endl;
		tcout << _T("  SafeSub<uint8>(120, 250) = ") << (int)safe_sub<uint8>(120, 250) << endl;
		tcout << _T("  SafeMul<uint8>(120, 250) = ") << (int)safe_mul<uint8>(120, 250) << endl;
	}

	{
		tcout << _T("Random generator:") << endl;
		tcout << _T("  uint32: ") << g_Rand.RandUint() << _T(' ') << g_Rand.RandUint() << _T(' ') << g_Rand.RandUint() << _T(' ') << g_Rand.RandUint() << endl;
		tcout << _T("  float: ") << g_Rand.RandFloat() << _T(' ') << g_Rand.RandFloat() << _T(' ') << g_Rand.RandFloat() << _T(' ') << g_Rand.RandFloat() << endl;
		tcout << _T("  bool: ") << g_Rand.RandBool() << _T(' ') << g_Rand.RandBool() << _T(' ') << g_Rand.RandBool() << _T(' ') << g_Rand.RandBool() << endl;
		tcout << _T("  Byte: ") << (int)g_Rand.RandByte() << _T(' ') << (int)g_Rand.RandByte() << _T(' ') << (int)g_Rand.RandByte() << _T(' ') << (int)g_Rand.RandByte() << endl;
		tcout << _T("  Normal(sigma=1): ") << g_Rand.RandNormal(1.0f) << _T(' ') << g_Rand.RandNormal(1.0f) << _T(' ') << g_Rand.RandNormal(1.0f) << _T(' ') << g_Rand.RandNormal(1.0f) << endl;

		RandomGenerator DR1(123);
		tcout << _T("  Deterministic #1 (seed 123): ") << DR1.RandUint() << _T(' ') << DR1.RandFloat() << _T(' ') << DR1.RandBool() << endl;
		RandomGenerator DR2(123);
		tcout << _T("  Deterministic #2 (seed 123): ") << DR2.RandUint() << _T(' ') << DR2.RandFloat() << _T(' ') << DR2.RandBool() << endl;
	}

	{
		tcout << _T("Conversions:") << endl;

		tchar ch = _T('A');
		int i = 123;
		uint32 u = 323456789;
		float f = 3.14f;
		double d = 10.5;
		bool b = true;
		tstring s = _T("Foo");
		int ai[] = { 1, 2, 3 };
		std::vector<int> vi(&ai[0], &ai[3]);
		tstring as[] = { _T("a"), _T("bb"), _T("ccc"), _T("dddd") };
		std::vector<tstring> vs(&as[0], &as[4]);

		tstring TMP;
		SthToStr(&TMP, ch); tcout << _T("  tchar to tstring: ") << TMP << endl;
		StrToSth(&ch, TMP); tcout << _T("  tstring to tchar: ") << ch << endl;
		SthToStr(&TMP, i ); tcout << _T("  int to tstring: ") << TMP << endl;
		StrToSth(&i, TMP);  tcout << _T("  tstring to int: ") << i << endl;
		SthToStr(&TMP, u ); tcout << _T("  uint32 to tstring: ") << TMP << endl;
		StrToSth(&u, TMP);  tcout << _T("  tstring to uint32: ") << u << endl;
		SthToStr(&TMP, f ); tcout << _T("  float to tstring: ") << TMP << endl;
		StrToSth(&f, TMP);  tcout << _T("  tstring to float: ") << f << endl;
		SthToStr(&TMP, d ); tcout << _T("  double to tstring: ") << TMP << endl;
		StrToSth(&d, TMP);  tcout << _T("  tstring to double: ") << d << endl;
		SthToStr(&TMP, b ); tcout << _T("  bool to tstring: ") << TMP << endl;
		StrToSth(&b, TMP);  tcout << _T("  tstring to bool: ") << b << endl;
		SthToStr(&TMP, s ); tcout << _T("  tstring to tstring: ") << TMP << endl;
		StrToSth(&s, TMP);  tcout << _T("  tstring to tstring: ") << s << endl;
		SthToStr(&TMP, vi); tcout << _T("  vector<int> to tstring: ") << TMP << endl;
		StrToSth(&vi, TMP); tcout << _T("  tstring to vector<int>:"); for (size_t si1 = 0; si1 < vi.size(); si1++) tcout << _T(' ') << vi[si1]; tcout << endl;
		SthToStr(&TMP, vs); tcout << _T("  vector<tstring> to tstring: ") << TMP << endl;
		StrToSth(&vs, TMP); tcout << _T("  tstring to vector<tstring>:"); for (size_t si2 = 0; si2 < vs.size(); si2++) tcout << _T(' ') << vs[si2]; tcout << endl;

		tcout << _T("- Format: ") << (Format(_T("#, #, #, #, #, #, #, #, #")) % ch % i % u % f % d % b % s % vi % vs).str() << endl;
	}

	// Sort2, Sort3
	{
		int arr[3] = { 2, 3, 1 };
		Sort3(arr[0], arr[1], arr[2]);
		assert(arr[0] == 1);
		assert(arr[1] == 2);
		assert(arr[2] == 3);
	}

	{
		tchar s[128];
		common_sprintf(s, _countof(s), _T("common_sprintf: 1=%i, 2.5=%g"), 1, 2.5);
		tcout << s << endl;
	}
}

void TestError()
{
	WriteLine(_T("==================== ERROR ===================="));

	try
	{
		{
			ERR_TRY;
			{
				ERR_TRY;

				throw Error(_T("Some error."), __TFILE__, __LINE__);

				ERR_CATCH(_T("Message in catch."));
			}
			ERR_CATCH_FUNC;
		}
	}
	catch (Error &Err)
	{
		tstring s;
		Err.GetMessage_(&s);
		tcout << _T("Error Message:") << endl;
		tcout << s << endl;
	}
}

void TestMath()
{
	WriteLine(_T("==================== MATH ===================="));

	{
		tcout << _T("Testing POINT_") << endl;
		const tstring s1 = _T("123,456");
		POINT_ P1; tcout << StrToSth(&P1, s1) << endl;
		POINT_ P2 = (P1 + P1) * 7;
		tcout << (Format(_T("Result: #")) % P2).str() << endl;
	}

	{
		tcout << _T("Testing VEC2") << endl;
		const tstring s1 = _T("123,456");
		VEC2 V1; tcout << StrToSth(&V1, s1) << endl;
		VEC2 V2 = (V1 + V1) * 7.0f;
		tcout << (Format(_T("Result: #")) % V2).str() << endl;
	}

	{
		tcout << _T("Testing VEC3") << endl;
		const tstring s1 = _T("123,456,789");
		VEC3 V1; tcout << StrToSth(&V1, s1) << endl;
		VEC3 V2 = (V1 + V1) * 7.0f;
		tcout << (Format(_T("Result: #")) % V2).str() << endl;
	}

	{
		tcout << _T("Testing VEC4") << endl;
		const tstring s1 = _T("123,456,789,012");
		VEC4 V1; tcout << StrToSth(&V1, s1) << endl;
		VEC4 V2 = (V1 + V1) * 7.0f;
		tcout << (Format(_T("Result: #")) % V2).str() << endl;
	}

	{
		tcout << _T("Testing RECTI") << endl;
		const tstring s1 = _T("123,456,789,012");
		RECTI V1; tcout << StrToSth(&V1, s1) << endl;
		RECTI V2 = (V1 + POINT_(1, 2)) * 7;
		tcout << (Format(_T("Result: #")) % V2).str() << endl;
	}

	{
		tcout << _T("Testing RECTF") << endl;
		const tstring s1 = _T("123,456,789,012");
		RECTF V1; tcout << StrToSth(&V1, s1) << endl;
		RECTF V2 = (V1 + VEC2(1.0f, 2.0f)) * 7.0f;
		tcout << (Format(_T("Result: #")) % V2).str() << endl;
	}

	{
		tcout << _T("Testing BOX") << endl;
		const tstring s1 = _T("1,2,3;4,5,6");
		BOX V1; tcout << StrToSth(&V1, s1) << endl;
		BOX V2 = (V1 + VEC3(1.0f, 2.0f, 3.0f)) * 7.0f;
		tcout << (Format(_T("Result: #")) % V2).str() << endl;
	}

	{
		tcout << _T("Testing COLOR") << endl;
		const tstring s1 = _T("red");
		const tstring s2 = _T("008080FF");
		COLOR c1; tcout << StrToSth(&c1, s1) << endl;
		COLOR c2; tcout << StrToSth(&c2, s2) << endl;
		tcout << (Format(_T("Color1 = #, Color2 = #")) % c1 % c2).str() << endl;
		tcout << (Format(_T("Color1+Color2 = #, Color1-Color2 = #, Color1*Color2 = #")) % (c1+c2) % (c1-c2) % (c1*c2)).str() << endl;
	}

	{
		tcout << _T("Testing COLORF") << endl;
		const tstring s1 = _T("red");
		const tstring s2 = _T("008080FF");
		COLORF c1; tcout << StrToSth(&c1, s1) << endl;
		COLORF c2; tcout << StrToSth(&c2, s2) << endl;
		tcout << (Format(_T("Color1 = #, Color2 = #")) % c1 % c2).str() << endl;
		tcout << (Format(_T("Color1+Color2 = #, Color1-Color2 = #, Color1*Color2 = #")) % (c1+c2) % (c1-c2) % (c1*c2)).str() << endl;
	}

	{
		tcout << _T("Testing MATRIX, PLANE, VEC*") << endl;

		MATRIX m1; PerspectiveFovLH(&m1, PI_2, 4.0f/3.0f, 1.0f, 100.0f);
		MATRIX m2; RotationYawPitchRoll(&m2, 1.0f, 2.0f, 3.0f);

		PLANE plane;
		GetPlaneNormal(plane) =VEC3_POSITIVE_Y;
		plane.d = 1.0f;
		tcout << (Format(_T("Plane = #")) % plane).str() << endl;
		VEC3 vector(1.0f, 2.0f, 3.0f);
		tcout << (Format(_T("Vector = #")) % vector).str() << endl;

		PLANE plane2;
		Transform(&plane2, plane, m1);
		tcout << (Format(_T("Transformed plane 1 = #")) % plane2).str() << endl;
		Transform(&plane2, plane, m2);
		tcout << (Format(_T("Transformed plane 2 = #")) % plane2).str() << endl;

		VEC3 vector2;
		TransformCoord(&vector2, vector, m1);
		tcout << (Format(_T("Transformed vector 1 = #")) % vector2).str() << endl;
		TransformCoord(&vector2, vector, m2);
		tcout << (Format(_T("Transformed vector 2 = #")) % vector2).str() << endl;
	}

	// Swizzle
	{
		VEC3 v1 = VEC3(1.f, 2.f, 3.f);
		VEC2 v2 = VEC2_SWIZZLE(v1, x, x);
		assert(around(v2.x, v1.x, 1e-6f) && around(v2.y, v1.x, 1e-6f));
		VEC4 v3 = VEC4_SWIZZLE(v1, z, y, x, z);
		assert(around(v3.x, v1.z, 1e-6f) && around(v3.y, v1.y, 1e-6f) && around(v3.z, v1.x, 1e-6f) && around(v3.w, v1.z, 1e-6f));

		MATRIX m;
		Identity(&m);
		VEC4 mDiagonal = VEC4_SWIZZLE(m,_11,_22,_33,_44);
		BOX box = BOX_ZERO;
		VEC2 boxRangeX = VEC2_SWIZZLE(box,Min.x,Max.x);
	}
}

void TestProfiler()
{
	WriteLine(_T("==================== PROFILER ===================="));
	{
		PROFILE_GUARD(g_Profiler, _T("Everything"));

		tcout << _T("Waiting 100 ms...") << endl;
		{
			PROFILE_GUARD(g_Profiler, _T("Waiting 100 ms"));
			Wait(100);
		}

		tcout << _T("Making many computations...") << endl;
		int i, j;
		for (i = 0; i < 100; i++)
		{
			PROFILE_GUARD(g_Profiler, _T("Computations"));
			int Array[1000];
			for (j = 0; j < 1000; j++)
			{
				Array[j] = i*j / (i+j+1);
			}
		}
	}

	tstring Result;
	g_Profiler.FormatString(&Result, PROFILER_UNITS_MILLISECONDS);
	tcout << _T("Result:") << endl << Result << endl;
}

void TestFlatProfiler()
{
	FlatProfiler<tstring> flatProfiler;

	{
		FlatProfilerSampler<tstring> sampler(flatProfiler, _T("Main"));
		for (uint i = 0; i < 10; i++)
		{
			{
				FlatProfilerSampler<tstring> sampler(flatProfiler, _T("Wait10"));
				Wait(10);
			}
			{
				FlatProfilerSampler<tstring> sampler(flatProfiler, _T("Wait2"));
				Wait(2);
			}
		}
	}

	tstring profile;
	flatProfiler.FormatString(&profile, PROFILER_UNITS_MILLISECONDS);
	WriteLine(_T("FlatProfiler result:"));
	WriteLine(profile);
}

void TestStream()
{
	WriteLine(_T("==================== Stream ===================="));
	
	{
		PROFILE_GUARD(g_Profiler, _T("Reading files from streams by byte"))

		{
			PROFILE_GUARD(g_Profiler, _T("FileStream"))

				FileStream fs(_T("SomeFile1"), FM_READ);
			char byte;
			while (!fs.End())
				fs.Read(&byte, 1);
		}

		{
			PROFILE_GUARD(g_Profiler, _T("FileStream + BufferingStream"))

				FileStream fs(_T("SomeFile2"), FM_READ);
			BufferingStream bs(&fs, 65536, 0);
			char byte;
			while (!bs.End())
				bs.Read(&byte, 1);
		}
	}
}

void TestEncoderDecoder()
{
	WriteLine(_T("==================== Kodowanie Base64 ===================="));

	const uint COUNT = 100;
	const uint MAX_SIZE = 65536;
	char *SrcData = new char[MAX_SIZE];
	char *DstData = new char[MAX_SIZE];
	char *EncodedData = new char[MAX_SIZE*2];

	for (uint i = 0; i < COUNT; i++)
	{
		uint Size = g_Rand.RandUint(1, MAX_SIZE+1);
		uint EncodedSize;
		g_Rand.RandData(SrcData, Size);
		
		{
			MemoryStream SrcM(Size, SrcData);
			MemoryStream EncodedM(Size*2, EncodedData);
			{
				Base64Encoder Hex(&EncodedM);
				CopyToEnd(&Hex, &SrcM);
			}
			EncodedSize = (uint)EncodedM.GetPos();
		}
		{
			MemoryStream EncodedM(EncodedSize, EncodedData);
			MemoryStream DstM(Size, DstData);
			try
			{
				Base64Decoder Hex(&EncodedM);
				CopyToEnd(&DstM, &Hex);
			}
			catch (...)
			{
				string Src; Src.assign(SrcData, Size);
				string Encoded; Encoded.assign(EncodedData, Size);
				tcout << (Format(_T("Wyjatek dekodowania. SrcData=#, EncodedData=#")) % StringToTstringR(Src) % StringToTstringR(Encoded)).str() << endl;
				throw;
			}
		}

		if (memcmp(SrcData, DstData, Size) != 0)
			tcout << _T("FAILED!") << endl;
	}

	delete [] EncodedData;
	delete [] DstData;
	delete [] SrcData;
}

class SmartPtrTestClass
{
private:
	int m_i;

public:
	SmartPtrTestClass() : m_i(0) { tcout << _T("- ctor default ") << m_i << endl; }
	SmartPtrTestClass(int i) : m_i(i) { tcout << _T("- ctor ") << m_i << endl; }
	~SmartPtrTestClass() { tcout << _T("- dtor ") << m_i << endl; }
	void F() { tcout << _T("- F ") << m_i << endl; }
	void Release() { tcout << _T("- Release ") << m_i << endl; }
};

typedef int MY_HANDLE;

void CreateHandle(MY_HANDLE *Out, int x)
{
	tcout << _T("- CreateHandle: ") << x << endl;
	*Out = x;
}

void DestroyHandle(MY_HANDLE h)
{
	tcout << _T("- DestroyHandle: ") << h << endl;
}

class DestroyHandlePolicy { public: template <typename T> static void Destroy(T p) { if (p != 0) DestroyHandle(p); } };

void TestSmartPointers()
{
	WriteLine(_T("==================== Inteligente wskaŸniki ===================="));

	tcout << _T("- scoped_ptr 1\n");
	{
		scoped_ptr<SmartPtrTestClass> p1;
		p1.reset(new SmartPtrTestClass(1));
		p1->F();
		p1.reset();
		assert( p1 == p1.get() );
		assert( p1.get() == p1 );
		assert( p1 == NULL );
		assert( NULL == p1 );
	}
	tcout << _T("- scoped_ptr 2\n");
	{
		scoped_ptr<SmartPtrTestClass> p2(new SmartPtrTestClass(1));
		p2.reset(new SmartPtrTestClass(2));
		(*p2).F();
	}

	tcout << _T("common::shared_ptr 1\n");
	{
		common::shared_ptr<SmartPtrTestClass> p1;
		p1.reset(new SmartPtrTestClass(1));
		p1->F();
		p1.reset();
		assert( p1 == p1.get() );
		assert( p1.get() == p1 );
		assert( p1 == NULL );
		assert( NULL == p1 );
	}
	tcout << _T("common::shared_ptr 2\n");
	{
		common::shared_ptr<SmartPtrTestClass> p2(new SmartPtrTestClass(1));
		p2.reset(new SmartPtrTestClass(2));
		(*p2).F();
	}
	tcout << _T("common::shared_ptr 3\n");
	{
		common::shared_ptr<SmartPtrTestClass> p1;
		common::shared_ptr<SmartPtrTestClass> p2(new SmartPtrTestClass(1));
		p1 = p2;
		assert( p1 == p2 );
		p2.reset();
		p1.get()->F();
		common::shared_ptr<SmartPtrTestClass> p3;
		p3 = p1;
		(*p3).F();
		p3.reset(common::shared_ptr<SmartPtrTestClass>(new SmartPtrTestClass(101)));
		p3.reset(new SmartPtrTestClass(102));
		p3.reset(NULL);
		swap(p3, p1);
		p3->F();
		assert( p3.unique() );
	}
	tcout << _T("common::shared_ptr(Release)\n");
	{
		common::shared_ptr<SmartPtrTestClass, ReleasePolicy> p1(new SmartPtrTestClass(66));
		common::shared_ptr<SmartPtrTestClass, ReleasePolicy> p2(p1);
		p2->F();
	}
	tcout << _T("scoped_ptr(array)\n");
	{
		scoped_ptr<SmartPtrTestClass, DeleteArrayPolicy> p1(new SmartPtrTestClass[3]);
		p1.get()[0].F();
	}
	tcout << _T("scoped_handle\n");
	{
		typedef scoped_handle<MY_HANDLE, DestroyHandlePolicy> my_scoped_handle;
		MY_HANDLE hv;
		CreateHandle(&hv, 1);
		my_scoped_handle h1(hv);
		CreateHandle(&hv, 2);
		h1.reset(hv);
	}
	tcout << _T("shared_handle\n");
	{
		typedef shared_handle<MY_HANDLE, DestroyHandlePolicy> my_shared_handle;
		MY_HANDLE hv;
		my_shared_handle p1(0);
		CreateHandle(&hv, 1);
		my_shared_handle p2(hv);
		p1 = p2;
		assert( p1 == p2 );
		p2.reset(0);
		assert( p1 != p2 );
		my_shared_handle p3(0);
		p3 = p1;
		CreateHandle(&hv, 101);
		p3.reset(my_shared_handle(hv));
		CreateHandle(&hv, 102);
		p3.reset(hv);
		p3.reset(0);
		swap(p3, p1);
		assert( p3.unique() );
	}
	tcout << _T("scoped_ptr (array)\n");
	{
		scoped_ptr<SmartPtrTestClass, DeleteArrayPolicy> p1;
		p1.reset(new SmartPtrTestClass[3]);
		p1[0].F();
		p1[1].F();
		p1[2].F();
		p1.reset();
	}
}


void TestFreeList()
{
	WriteLine(_T("==================== FreeList ===================="));

	const uint COUNT = 10;

	FreeList<ptrdiff_t> lista(COUNT);
	scoped_ptr<ptrdiff_t*, DeleteArrayPolicy> tablica(new ptrdiff_t*[COUNT]);

	for (uint i = 0; i < COUNT; i++)
		tablica.get()[i] = lista.New_ctor();

	tcout << _T("Przed wypelnieniem:\n");
	for (uint i = 0; i < COUNT; i++)
		tcout << *tablica.get()[i] << _T(' ');
	tcout << endl;

	for (uint i = 0; i < COUNT; i++)
		*tablica.get()[i] = i;

	tcout << _T("Po wypelnieniu:\n");
	for (uint i = 0; i < COUNT; i++)
		tcout << *tablica.get()[i] << _T(' ');
	tcout << endl;

	for (uint i = 0; i < COUNT; i++)
		lista.Delete(tablica.get()[i]);
}

void TestDynamicFreeList()
{
	WriteLine(_T("==================== DynamicFreeList ===================="));

	const uint BLOCK_COUNT = 3;
	const uint COUNT = 10;

	DynamicFreeList<ptrdiff_t> lista(BLOCK_COUNT);
	scoped_ptr<ptrdiff_t*, DeleteArrayPolicy> tablica(new ptrdiff_t*[COUNT]);

	tcout << (Format(_T("Stats: Empty=#, Full=#, BlockCount=#, BlockCapacity=#, UsedCount=#, FreeCount=#, Capacity=#, BlockSize=#, ...\n")) %
		lista.IsEmpty() % lista.IsFull() % lista.GetBlockCount() % lista.GetBlockCapacity() % lista.GetUsedCount() % lista.GetFreeCount() % lista.GetCapacity() % lista.GetBlockSize()).str();

	tcout << _T("Alokacja...\n");

	for (uint i = 0; i < COUNT; i++)
		tablica.get()[i] = lista.New();

	tcout << _T("Elementy:\n");
	for (uint i = 0; i < COUNT; i++)
		tcout << *tablica.get()[i] << _T(' ');
	tcout << endl;

	tcout << _T("Wypelnianie...:\n");

	for (uint i = 0; i < COUNT; i++)
		*tablica.get()[i] = i;

	tcout << (Format(_T("Stats: Empty=#, Full=#, BlockCount=#, BlockCapacity=#, UsedCount=#, FreeCount=#, Capacity=#, BlockSize=#, ...\n")) %
		lista.IsEmpty() % lista.IsFull() % lista.GetBlockCount() % lista.GetBlockCapacity() % lista.GetUsedCount() % lista.GetFreeCount() % lista.GetCapacity() % lista.GetBlockSize()).str();

	tcout << _T("Elementy:\n");
	for (uint i = 0; i < COUNT; i++)
		tcout << *tablica.get()[i] << _T(' ');
	tcout << endl;

	tcout << _T("Zwalnianie...:\n");
	for (uint i = 0; i < COUNT; i++)
		lista.Delete(tablica.get()[i]);

	tcout << (Format(_T("Stats: Empty=#, Full=#, BlockCount=#, BlockCapacity=#, UsedCount=#, FreeCount=#, Capacity=#, BlockSize=#, ...\n")) %
		lista.IsEmpty() % lista.IsFull() % lista.GetBlockCount() % lista.GetBlockCapacity() % lista.GetUsedCount() % lista.GetFreeCount() % lista.GetCapacity() % lista.GetBlockSize()).str();
}

/*
template <typename T>
void AdvancedFreeListTestAndProfile()
{
	const uint BLOCK_SIZE = 128;
	const uint MAX_SIZE = 1024*10;
	const uint OP_COUNT = MAX_SIZE;
	std::vector<T*> Pointers;
	std::vector<int> OpSequence; // > 0 = numer wskaŸnika do zwolnienia, -1 = alokacja nowego
	OpSequence.resize(OP_COUNT);
	for (uint op = 0; op < OP_COUNT; op++)
		OpSequence[op] = (g_Rand.RandUint(10) == 0) ? (int)g_Rand.RandUint(MAX_SIZE) : -1;

	{
		FreeList<T> List(MAX_SIZE);

		PROFILE_GUARD(g_Profiler, _T("FreeList"));
		for (uint op = 0; op < OP_COUNT; op++)
		{
			if (OpSequence[op] == -1)
				Pointers.push_back(List.New());
			else if (!Pointers.empty())
			{
				uint Index = op % Pointers.size();
				List.Delete(Pointers[Index]);
				Pointers.erase(Pointers.begin()+Index);
			}
		}
		while (!Pointers.empty())
		{
			List.Delete(Pointers.back());
			Pointers.pop_back();
		}
	}

	{
		DynamicFreeList<T> List(BLOCK_SIZE);

		PROFILE_GUARD(g_Profiler, _T("DynamicFreeList"));
		for (uint op = 0; op < OP_COUNT; op++)
		{
			if (OpSequence[op] == -1)
				Pointers.push_back(List.New());
			else if (!Pointers.empty())
			{
				uint Index = op % Pointers.size();
				List.Delete(Pointers[Index]);
				Pointers.erase(Pointers.begin()+Index);
			}
		}
		while (!Pointers.empty())
		{
			List.Delete(Pointers.back());
			Pointers.pop_back();
		}
	}

	{
		PROFILE_GUARD(g_Profiler, _T("new i delete"));
		for (uint op = 0; op < OP_COUNT; op++)
		{
			if (OpSequence[op] == -1)
				Pointers.push_back(new T);
			else if (!Pointers.empty())
			{
				uint Index = op % Pointers.size();
				delete Pointers[Index];
				Pointers.erase(Pointers.begin()+Index);
			}
		}
		while (!Pointers.empty())
		{
			delete Pointers.back();
			Pointers.pop_back();
		}
	}
}

class DuzaKlasa
{
	char Zapelniacz[1024];
};

void CompelexTestFreeLists()
{
	for (uint TestI = 0; TestI < 10; TestI++)
	{
		{
			PROFILE_GUARD(g_Profiler, _T("Element typu int"));
			AdvancedFreeListTestAndProfile<int>();
		}
		{
			PROFILE_GUARD(g_Profiler, _T("Element typu zajmujacego 1024 bajty"));
			AdvancedFreeListTestAndProfile<DuzaKlasa>();
		}
	}
}
*/

void TestZlibUtils()
{
	WriteLine(_T("==================== ZLIB UTILS ===================="));

	const char * const INPUT_DATA = "Ala ma kota, To jest test, Hello World, ble ble ble, delelele, blfzcf i dupadupa";
	
	{
		size_t OutputBufLen = ZlibCompressionStream::CompressLength(strlen(INPUT_DATA));
		scoped_ptr<char, DeleteArrayPolicy> OutputData(new char[OutputBufLen]);

		size_t OutputSize = ZlibCompressionStream::Compress(OutputData.get(), OutputBufLen, INPUT_DATA, strlen(INPUT_DATA));

		scoped_ptr<char, DeleteArrayPolicy> Input2Data(new char[strlen(INPUT_DATA)]);
		size_t Input2Size = ZlibDecompressionStream::Decompress(Input2Data.get(), strlen(INPUT_DATA), OutputData.get(), OutputSize);
		WriteLine(Format(_T("InputLen=#, OutputBufMaxLen=#, OutputSize=#, InputLen2=#")) % strlen(INPUT_DATA) % OutputBufLen % OutputSize % Input2Size);

		for (uint i = 0; INPUT_DATA[i] != '\0'; i++)
			assert( INPUT_DATA[i] == Input2Data[i] );
	}

	{
		VectorStream v1;
		{
			ZlibCompressionStream c1(&v1);
			c1.Write(INPUT_DATA, strlen(INPUT_DATA));
		}
		v1.Rewind();
		VectorStream v1d;
		{
			ZlibDecompressionStream d1(&v1);
			assert (d1.End() == false );
			CopyToEnd(&v1d, &d1);
			assert (d1.End() == true );
		}
		bool Correct1 = (v1d.GetSize() == strlen(INPUT_DATA));
		if (Correct1)
		{
			for (uint i = 0; INPUT_DATA[i] != '\0'; i++)
			{
				if (INPUT_DATA[i] != v1d.Data()[i])
				{
					Correct1 = false;
					break;
				}
			}
		}
		WriteLine(Format(_T("Compression/decompression (ca³oœæ):\n  in_len=#, out_len=#, po_dekompresji_len=#, correct=#")) % strlen(INPUT_DATA) % v1.GetSize() % v1d.GetSize() % Correct1);

		VectorStream v2;
		{
			ZlibCompressionStream c2(&v2);
			for (uint i = 0; INPUT_DATA[i] != '\0'; i++)
				c2.Write(&INPUT_DATA[i], 1);
			c2.Flush();
		}
		v2.Rewind();
		VectorStream v2d;
		{
			ZlibDecompressionStream d2(&v2);
			for (uint i = 0; INPUT_DATA[i] != '\0'; i++)
			{
				assert( d2.End() == false );
				char Ch;
				d2.MustRead(&Ch, 1);
				v2d.Write(&Ch, 1);
			}
			assert( d2.End() == true );
		}
		bool Correct2 = (v2d.GetSize() == strlen(INPUT_DATA));
		if (Correct2)
		{
			for (uint i = 0; INPUT_DATA[i] != '\0'; i++)
			{
				if (INPUT_DATA[i] != v2d.Data()[i])
				{
					Correct2 = false;
					break;
				}
			}
		}
		WriteLine(Format(_T("Compression/decompression (po znaku):\n  in_len=#, out_len=#, po_dekompresji_len=#, correct=#")) % strlen(INPUT_DATA) % v2.GetSize() % v2d.GetSize() % Correct2);

		// Test dla pustego
		VectorStream ev;
		{
			ZlibCompressionStream c(&ev);
		}
		ev.Rewind();
		VectorStream evd;
		{
			ZlibDecompressionStream d(&ev);
			assert( d.End() == true );
			CopyToEnd(&evd, &d);
			assert( d.End() == true );
		}
		WriteLine(Format(_T("Compression/decompression (pusty):\n  in_len=(0), out_len=#, po_dekompresji_len=#")) % ev.GetSize() % evd.GetSize());
	}

	// Test gzip
	{
		const string FileName1 = "Plik.test", Comment1 = "Komentarz";

		// Test 1 - plik
		{
			common::GzipFileStream Plik("Archive.gz", common::GZFM_WRITE);
			Plik.Write(INPUT_DATA, strlen(INPUT_DATA));
		}
		{
			common::GzipFileStream Plik("Archive.gz", common::GZFM_READ);
			assert( Plik.End() == false );
			std::vector<char> Data;
			Data.resize(strlen(INPUT_DATA));
			Plik.MustRead(&Data[0], strlen(INPUT_DATA));
			assert( Plik.End() == true );
			char Foo;
			assert( Plik.Read(&Foo, 1) == 0 );
			assert( std::equal(&INPUT_DATA[0], &INPUT_DATA[strlen(INPUT_DATA)], Data.begin()) );
			WriteLine(_T("Gzip - file - PASSED"));
		}

		common::DeleteFile(_T("Archive.gz"));

		// Test 2 - strumieñ
		{
			common::FileStream Plik(_T("Archive.gz"), common::FM_WRITE);
			GzipCompressionStream Gzip(&Plik, &FileName1, &Comment1);
			Gzip.WriteStringF(INPUT_DATA);
		}
		{
			common::FileStream Plik(_T("Archive.gz"), common::FM_READ);
			string FileName2, Comment2;
			GzipDecompressionStream Gzip(&Plik);
			std::vector<char> Data;
			Data.resize(strlen(INPUT_DATA));
			for (uint i = 0; INPUT_DATA[i] != '\0'; i++)
			{
				assert( Gzip.End() == false );
				Gzip.MustRead(&Data[i], 1);
			}
			assert( Gzip.End() == true );
			char Foo;
			assert( Gzip.Read(&Foo, 1) == 0 );
			assert( std::equal(&INPUT_DATA[0], &INPUT_DATA[strlen(INPUT_DATA)], Data.begin()) );
			assert( Gzip.GetHeaderFileName(&FileName2) == true );
			assert( Gzip.GetHeaderComment(&Comment2) == true );
			assert( FileName2 == FileName1 );
			assert( Comment2 == Comment1 );
			WriteLine(_T("Gzip - stream - PASSED"));
		}

		common::DeleteFile(_T("Archive.gz"));
	}
}

void TestFiles()
{
	WriteLine(_T("==================== FILES ===================="));

	tstring Name;
	common::FILE_ITEM_TYPE Type;

#ifdef WIN32
	const tchar * PATHS[] =
	{
		_T("D:"),
		_T("C:\\Windows"),
		_T("EmptyDir"),
		_T("")
	};
#else
	const tchar * PATHS[] =
	{
		_T("/"),
		_T("/boot"),
		_T("EmptyDir"),
		_T("")
	};
#endif

#ifdef WIN32
	for (uint i = 0; _tcslen(PATHS[i]) > 0; i++)
#else
	for (uint i = 0; strlen(PATHS[i]) > 0; i++)
#endif
	{
		WriteLine(Format(_T("Listowanie katalogu: #")) % PATHS[i]);
		common::DirLister Lister(PATHS[i]);
		while (Lister.ReadNext(&Name, &Type))
			tcout << (Format(_T("- # # ")) % ((Type == common::IT_DIR) ? _T("DIR ") : _T("FILE")) % Name).str();
		tcout << endl;
	}

	{
		common::DATETIME ctime, mtime, atime; uint64 size; common::FILE_ITEM_TYPE type;
		common::MustGetFileItemInfo(_T("EmptyDir"), &type, &size, &mtime, &ctime, &atime);
		WriteLine(Format(_T("MustGetFileItemInfo(\"EmptyDir\"): Type=#, Size=#\n  MTime=#\n  CTime=#\n  ATime=#")) %
			(uint)type % size % common::TMSTRUCT(mtime) % common::TMSTRUCT(ctime) % common::TMSTRUCT(atime));
		common::MustGetFileItemInfo(_T("TwoCharFile.txt"), &type, &size, &mtime, &ctime, &atime);
		WriteLine(Format(_T("MustGetFileItemInfo(\"TwoCharFile.txt\"): Type=#, Size=#\n  MTime=#\n  CTime=#\n  ATime=#")) %
			(uint)type % size % common::TMSTRUCT(mtime) % common::TMSTRUCT(ctime) % common::TMSTRUCT(atime));
	}

	{
		WriteLine(Format(_T("GetFileItemType(\"EmptyDir\") = #")) % (uint)common::GetFileItemType(_T("EmptyDir")));
		WriteLine(Format(_T("GetFileItemType(\"TwoCharFile.txt\") = #")) % (uint)common::GetFileItemType(_T("TwoCharFile.txt")));
		WriteLine(Format(_T("GetFileItemType(\"Wrong.foo\") = #")) % (uint)common::GetFileItemType(_T("Wrong.foo")));
	}

	{
		common::MustUpdateFileTimeToNow(_T("TwoCharFile.txt"));
		common::DATETIME ctime, mtime, atime;
		common::MustGetFileItemInfo(_T("TwoCharFile.txt"), NULL, NULL, &mtime, &ctime, &atime);
		WriteLine(Format(_T("MustGetFileItemInfo(\"TwoCharFile.txt\") after MustUpdateFileTimeToNow:\n  MTime=#\n  CTime=#\n  ATime=#")) %
			common::TMSTRUCT(mtime) % common::TMSTRUCT(ctime) % common::TMSTRUCT(atime));
	}

	{
		tstring Dir = _T("EmptyDir") + CharToStrR(DIR_SEP) + _T("Test1") + CharToStrR(DIR_SEP) + _T("Test2");
		common::MustCreateDirectoryChain(Dir);
		assert( common::GetFileItemType(Dir) == common::IT_DIR );
		common::MustDeleteDirectory(Dir);
		common::MustDeleteDirectory(_T("EmptyDir") + CharToStrR(DIR_SEP) + _T("Test1"));
		assert( common::GetFileItemType(Dir) == common::IT_NONE );
		WriteLine(_T("MustCreateDirectoryChain and MustDeleteDirectory test succeeded."));
	}

	{
		tstring FileName1 = _T("TEMP1");
		tstring FileName2 = _T("TEMP2");
		assert( common::GetFileItemType(FileName1) == common::IT_NONE );
		assert( common::GetFileItemType(FileName2) == common::IT_NONE );
		{
			common::FileStream Plik(FileName1, common::FM_WRITE);
		}
		assert( common::GetFileItemType(FileName1) == common::IT_FILE );
		assert( common::GetFileItemType(FileName2) == common::IT_NONE );
		common::MustMoveItem(FileName1, FileName2);
		assert( common::GetFileItemType(FileName1) == common::IT_NONE );
		assert( common::GetFileItemType(FileName2) == common::IT_FILE );
		common::MustDeleteFile(FileName2);
		assert( common::GetFileItemType(FileName1) == common::IT_NONE );
		assert( common::GetFileItemType(FileName2) == common::IT_NONE );
		WriteLine(_T("MustMoveFile and MustDeleteFile test succeeded."));
	}
}

void TestDateTime()
{
	WriteLine(_T("==================== DATETIME ===================="));

	{
		tcout << _T("Time measurement:") << endl;
		tcout << _T("  Current time = ") << GetCurrentGameTime().ToSeconds_d() << endl;
		tcout << _T("  Waiting 0.1 s...") << endl;
		Wait(100);
		tcout << _T("  Current time = ") << GetCurrentGameTime().ToSeconds_d() << endl;
	}

	tstring s, s1, s2;

	common::DATETIME dtnow = common::Now();
	common::TMSTRUCT tmnow = common::TMSTRUCT(dtnow);
	WriteLine(Format(_T("Now  = # = #")) % dtnow % tmnow);

	common::DATETIME dtunow = common::UNow();
	common::TMSTRUCT tmunow = common::TMSTRUCT(dtunow);
	WriteLine(Format(_T("UNow = # = #")) % dtunow % tmunow);

	common::DateToStr(&s, tmunow, _T("wu, d Nf y h:m:s:i"));
	WriteLine(Format(_T("UNow (w³asny format) = #")) % s);

	common::TMSTRUCT tmunow2;
	assert( common::StrToDate(&tmunow2, s, _T("wu, d Nf y h:m:s:i")) );
	WriteLine(Format(_T("tstring > TMSTRUCT = #")) % tmunow2);

	common::DATETIME dtunow2(tmunow2);
	WriteLine(Format(_T("TMSTRUCT > DATETIME = #")) % dtunow2);

	common::TIMESPAN ts = common::TIMESPAN::Hour() + common::TIMESPAN::Minutes(15);
	ts.ToString(&s1, true);
	ts.ToString_Nice(&s2);
	WriteLine(Format(_T("TIMESPAN: # = # = #")) % ts % s1 % s2);

	common::DATETIME dt_plus_ts = dtnow - ts;
	common::TMSTRUCT dt_plus_ts_tm(dt_plus_ts);
	WriteLine(Format(_T("Now - TIMESPAN = #, dzieñ tygodnia: #")) % dt_plus_ts_tm % common::GetWeekDayName(dt_plus_ts_tm.GetWeekDay(), common::NAME_LONG_LOWERCASE));

	common::DATESPAN ds(0, 0, 0, 15);
	common::TMSTRUCT tm_plus_ds = tmnow - ds;
	WriteLine(Format(_T("Now - DATESPAN(15 dni) = #, dzieñ tygodnia: #")) % tm_plus_ds % common::GetWeekDayName(tm_plus_ds.GetWeekDay(), common::NAME_LONG_LOWERCASE));

	common::TMSTRUCT tm_custom(28, common::OCT, 2007, 13, 14, 15, 16);
	WriteLine(Format(_T("W³asna data = #, WeekDay=#")) % tm_custom % common::GetWeekDayName(tm_custom.GetWeekDay(), common::NAME_SHORT_LOWERCASE));
	//WriteLine(Format(_T("W³asna data = #, WeekDay=#, DayOfYear=#, WeekOfYear=#, WeekOfMonth=#")) %
	//	tm_custom % common::GetWeekDayName(tm_custom.GetWeekDay(), common::NAME_SHORT_LOWERCASE) %
	//	tm_custom.GetDayOfYear() % tm_custom.GetWeekOfYear() % tm_custom.GetWeekOfMonth());
}

void TestCmdLineParser()
{
	WriteLine(_T("==================== CMD LINE PARSER ===================="));

	const tchar * CMD_LINE = _T("-a -b -c param -abc=\"param\" \"-cparam\" /AA --AA \"/BBB\"=param DUPA --BBB \"param\"");
	CmdLineParser Parser(CMD_LINE);
	Parser.RegisterOpt(1, _T('a'), false);
	Parser.RegisterOpt(2, _T('b'), false);
	Parser.RegisterOpt(3, _T('c'), true);
	Parser.RegisterOpt(11, _T("AA"), false);
	Parser.RegisterOpt(12, _T("BBB"), true);

	int Id; tstring Param, RS;
	CmdLineParser::RESULT R;
	do
	{
		R = Parser.ReadNext();
		Id = Parser.GetOptId();
		Param = Parser.GetParameter();
		switch (R)
		{
		case CmdLineParser::RESULT_END:       RS = _T("RESULT_END      "); break;
		case CmdLineParser::RESULT_OPT:       RS = _T("RESULT_OPT      "); break;
		case CmdLineParser::RESULT_ERROR:     RS = _T("RESULT_ERROR    "); break;
		case CmdLineParser::RESULT_PARAMETER: RS = _T("RESULT_PARAMETER"); break;
		}
		WriteLine(Format(_T("Opt: Result=#, Id=#, Param=#")) % RS % Id % Param);
	} while (R != CmdLineParser::RESULT_END && R != CmdLineParser::RESULT_ERROR);
}

void TestTokenizer()
{
	WriteLine(_T("==================== TOKENIZER ===================="));
	string FileS;
	LoadStringFromFile(_T("Tokens.dat"), &FileS);
	tstring File = StringToTstringR(FileS);
	Tokenizer tokenizer(&File, Tokenizer::FLAG_MULTILINE_STRINGS);

	const tchar *KEYWORDS[] = { _T("false"), _T("true") };
	tokenizer.RegisterKeywords(KEYWORDS, 2);

	Tokenizer::TOKEN t;
	tstring s;
	size_t hn, rn, cn;
	do
	{
		tokenizer.Next();

		t = tokenizer.GetToken();
		hn = tokenizer.GetCharNum();
		rn = tokenizer.GetRowNum();
		cn = tokenizer.GetColNum();
		s  = tokenizer.GetString();

		WriteLine(Format(_T("Token: char=#, row=#, col=#, type=#, tstring=#")) % hn % rn % cn % tokenizer.GetTokenName(t) % s);

		if (t == Tokenizer::TOKEN_INTEGER)
			WriteLine(Format(_T("  value=#")) % tokenizer.MustGetInt4());
		else if (t == Tokenizer::TOKEN_FLOAT)
			WriteLine(Format(_T("  value=#")) % tokenizer.MustGetFloat());

	} while (t != Tokenizer::TOKEN_EOF);

	const tchar * const TEXT_TO_ESCAPE = _T("abcd tab:\t vtab:\v eol:\r\n quot:\" apos:\' strange=\xE0 _");
	tstring Escaped;
	TokenWriter::Escape(&Escaped, TEXT_TO_ESCAPE, TokenWriter::ESCAPE_EOL | TokenWriter::ESCAPE_OTHER);
	WriteLine(_T("Text to escape : ") + tstring(TEXT_TO_ESCAPE));
	WriteLine(_T("Escaped text   : ") + Escaped);
}

scoped_ptr<Mutex> g_Mutex;

void WriteLineMT(const tstring &s)
{
	MUTEX_LOCK(*g_Mutex.get());
	WriteLine(s);
}

class MyThread : public Thread
{
private:
	tstring m_Id;

protected:
	virtual void Run();

public:
	MyThread(const tstring &Id) : m_Id(Id) { }
};

void MyThread::Run()
{
	WriteLineMT(Format(_T("Thread #: Begin")) % m_Id);
	for (uint i = 0; i < 3; i++)
	{
		{
			MUTEX_LOCK(*g_Mutex.get());
			{
				MUTEX_LOCK(*g_Mutex.get());

				Wait(g_Rand.RandUint(100, 200));
				WriteLineMT(Format(_T("Thread #: Iter=#")) % m_Id % i);
			}
		}
		//Yield_();
	}
	WriteLineMT(Format(_T("Thread #: End")) % m_Id);
}

class ProducerConsumerBuffer
{
private:
	Semaphore m_ElementCount;
	Semaphore m_EmptyCount;
	std::queue<char> m_Queue;

public:
	ProducerConsumerBuffer();
	void Push(char Ch);
	char Pop();
};

ProducerConsumerBuffer::ProducerConsumerBuffer() :
	m_ElementCount(0),
	m_EmptyCount(3)
{
}

void ProducerConsumerBuffer::Push(char Ch)
{
	m_EmptyCount.P();

	{
		MUTEX_LOCK(*g_Mutex.get());
		m_Queue.push(Ch);
	}

	m_ElementCount.V();
}

char ProducerConsumerBuffer::Pop()
{
	char R;

	m_ElementCount.P();

	{
		MUTEX_LOCK(*g_Mutex.get());
		R = m_Queue.front();
		m_Queue.pop();
	}

	m_EmptyCount.V();

	return R;
}

class ProducerThread : public Thread
{
private:
	ProducerConsumerBuffer *m_Buffer;

protected:
	virtual void Run();

public:
	ProducerThread(ProducerConsumerBuffer *Buffer) : m_Buffer(Buffer) { }
};

void ProducerThread::Run()
{
	WriteLineMT(_T("Producer: Producing 5 chars..."));
	for (uint i = 0; i < 5; i++)
	{
		char Ch = (char)(i+_T('A'));
		m_Buffer->Push(Ch);
		WriteLineMT(_T("Producer: Produced char: ") + CharToStrR(Ch));
	}

	m_Buffer->Push(_T('\0'));
}

class ConsumerThread : public Thread
{
private:
	ProducerConsumerBuffer *m_Buffer;

protected:
	virtual void Run();

public:
	ConsumerThread(ProducerConsumerBuffer *Buffer) : m_Buffer(Buffer) { }
};

void ConsumerThread::Run()
{
    WriteLineMT(_T("Consumer: Waiting 2000 ms..."));
	Wait(2000);

	char Ch;

	for (;;)
	{
		Ch = m_Buffer->Pop();
		if (Ch == _T('\0')) break;
		WriteLineMT(_T("Consumer: Consumed char: ") + CharToStrR(Ch));
		Wait(1000);
	}
}

class ProducerConsumerBuffer2
{
private:
	Cond m_BufferNotEmpty;
	Cond m_BufferNotFull;
	Mutex m_Mutex;
	std::queue<char> m_Queue;

public:
	ProducerConsumerBuffer2();
	void Push(char Ch);
	char Pop();
};

ProducerConsumerBuffer2::ProducerConsumerBuffer2() :
	m_Mutex(0)
{
}

void ProducerConsumerBuffer2::Push(char Ch)
{
	MUTEX_LOCK(m_Mutex);

	while (m_Queue.size() == 3)
		m_BufferNotFull.Wait(&m_Mutex);

	m_Queue.push(Ch);

	m_BufferNotEmpty.Signal();
}

char ProducerConsumerBuffer2::Pop()
{
	MUTEX_LOCK(m_Mutex);

	while (m_Queue.size() == 0)
		m_BufferNotEmpty.Wait(&m_Mutex);

	char R = m_Queue.front();
	m_Queue.pop();

	m_BufferNotFull.Signal();

	return R;
}

class ProducerThread2 : public Thread
{
private:
	ProducerConsumerBuffer2 *m_Buffer;

protected:
	virtual void Run();

public:
	ProducerThread2(ProducerConsumerBuffer2 *Buffer) : m_Buffer(Buffer) { }
};

void ProducerThread2::Run()
{
	WriteLineMT(_T("Producer: Producing 5 chars..."));
	for (uint i = 0; i < 5; i++)
	{
		char Ch = (char)(i+_T('A'));
		m_Buffer->Push(Ch);
		WriteLineMT(_T("Producer: Produced char: ") + CharToStrR(Ch));
	}

	m_Buffer->Push(_T('\0'));
}

class ConsumerThread2 : public Thread
{
private:
	ProducerConsumerBuffer2 *m_Buffer;

protected:
	virtual void Run();

public:
	ConsumerThread2(ProducerConsumerBuffer2 *Buffer) : m_Buffer(Buffer) { }
};

void ConsumerThread2::Run()
{
	WriteLineMT(_T("Consumer: Waiting 2000 ms..."));
	Wait(2000);

	char Ch;

	for (;;)
	{
		Ch = m_Buffer->Pop();
		if (Ch == _T('\0')) break;
		WriteLineMT(_T("Consumer: Consumed char: ") + CharToStrR(Ch));
		Wait(1000);
	}
}

class WaitingThread : public Thread
{
private:
	tchar m_Id;
	Barrier *m_Barrier;

protected:
	virtual void Run();

public:
	WaitingThread(char Id, Barrier *b) : m_Id(Id), m_Barrier(b) { }
};

void WaitingThread::Run()
{
	for (uint i = 0; i < 5; i++)
	{
		uint Time = g_Rand.RandUint(100, 1000);
		WriteLineMT(Format(_T("WaitingThread # Iter # waiting for # ms before barrier.")) % m_Id % i % Time);
		Wait(Time);
		m_Barrier->Wait();
	}
}

class CancellableThread : public Thread
{
private:
	Event m_CancelEvent;

protected:
	virtual void Run();

public:
	CancellableThread();

	void Cancel();
};

void CancellableThread::Run()
{
	for (uint i = 0; i < 10; i++)
	{
		WriteLineMT(Format(_T("CancellableThread: Iter #")) % i);
		Wait(100);
		if (m_CancelEvent.Test())
		{
			WriteLineMT(_T("CancellableThread: Cancelled"));
			return;
		}
	}

	WriteLineMT(_T("CancellableThread: Done all iters, wainting for cancel..."));
	m_CancelEvent.Wait();
	WriteLineMT(_T("CancellableThread: Got cancel."));
}

CancellableThread::CancellableThread() :
	m_CancelEvent(false, Event::TYPE_AUTO_RESET)
{
}

void CancellableThread::Cancel()
{
	m_CancelEvent.Set();
}


void TestThreads()
{
	WriteLine(_T("==================== THREADS ===================="));

	g_Mutex.reset(new Mutex(Mutex::FLAG_RECURSIVE));
    
    {
		WriteLine(_T("-------------------- Basic test --------------------"));
		scoped_ptr<MyThread> Threads[4];

		for (uint i = 0; i < 4; i++)
			Threads[i].reset(new MyThread(UintToStrR(i)));
			
		for (uint i = 0; i < 4; i++)
			Threads[i]->Start();

		#ifdef WIN32
			Threads[0]->SetPriority(Thread::PRIORITY_HIGH);
			WriteLine(Format(_T("Main: Thread 1 Running: #")) % Threads[0]->IsRunning());
		#endif

		for (uint i = 0; i < 4; i++)
			Threads[i]->Join();

		#ifdef WIN32
			WriteLine(Format(_T("Main: Thread 1 Running: #")) % Threads[0]->IsRunning());
		#endif
	}

	{
		WriteLine(_T("-------------------- Producer-consumer (semaphore) --------------------"));
		ProducerConsumerBuffer Buf;
		ProducerThread Producer(&Buf);
		ConsumerThread Consumer(&Buf);
		Producer.Start();
		Consumer.Start();
		Producer.Join();
		Consumer.Join();
	}
	
	{
		WriteLine(_T("-------------------- Semaphore::TimeoutP --------------------"));
		WriteLine(_T("Tu ma byæ czekanie 2 s..."));
		Semaphore sem(0);
		sem.TimeoutP(2000);
		WriteLine(_T("Ju¿."));
	}

	{
		WriteLine(_T("-------------------- Producer-consumer 2 (cond) --------------------"));
		ProducerConsumerBuffer2 Buf;
		ProducerThread2 Producer(&Buf);
		ConsumerThread2 Consumer(&Buf);
		Producer.Start();
		Consumer.Start();
		Producer.Join();
		Consumer.Join();
	}
	
    {
		WriteLine(_T("-------------------- WaitingThread (barrier) --------------------"));
		Barrier B(5);
		scoped_ptr<WaitingThread> Threads[5];
		for (uint i = 0; i < 5; i++)
			Threads[i].reset(new WaitingThread((char)(i + _T('0')), &B));
		for (uint i = 0; i < 5; i++)
			Threads[i]->Start();
		for (uint i = 0; i < 5; i++)
			Threads[i]->Join();
	}

	
	{
		WriteLine(_T("-------------------- CancellableThread (event) --------------------"));

		WriteLine(_T("Test 1: Cancelling before iters finished. Event::Test"));
		CancellableThread T1;
		T1.Start();
		Wait(300);
		T1.Cancel();
		T1.Join();

		WriteLine(_T("Test 2: Cancelling after iters finished. Event::Wait"));
		CancellableThread T2;
		T2.Start();
		Wait(1200);
		T2.Cancel();
		T2.Join();
	}

	g_Mutex.reset();
}

class LoggingThread : public Thread
{
private:
	common::Logger & m_Logger;
	tstring m_ThreadName;

protected:
	virtual void Run();

public:
	LoggingThread(common::Logger & Logger, const tstring &ThreadName) : m_Logger(Logger), m_ThreadName(ThreadName) { }

	void CustomRun() { Run(); }
};

void LoggingThread::Run()
{
	for (int i = 0; i < 10; i++)
	{
		m_Logger.Log(1, Format(_T("Message from thread \"#\" number \"#\"")) % m_ThreadName % i);
	}
}

void TestLogger()
{
	WriteLine(_T("==================== LOGGER ===================="));

	common::CreateLogger(true);

	common::Logger & Logger = common::GetLogger();

	scoped_ptr<common::ILog> HtmlLog(new common::HtmlFileLog(_T("Log.html"), common::FILE_MODE_NORMAL));
	scoped_ptr<common::ILog> ConsoleLog(new common::OstreamLog(&tcout));

	Logger.AddLogMapping(0xFFFFFFFF, HtmlLog.get());
	Logger.AddLogMapping(0xFFFFFFFF, ConsoleLog.get());

	Logger.SetPrefixFormat(_T("[%D %T] "));

	LoggingThread Thread1(Logger, _T("Thread1"));
	LoggingThread Thread2(Logger, _T("Thread2"));

	Thread1.Start();
	Thread2.Start();

	LoggingThread MainThread(Logger, _T("MainThread"));
	MainThread.CustomRun();

	Thread2.Join();
	Thread1.Join();

	common::DestroyLogger();

	ConsoleLog.reset(0);
	HtmlLog.reset(0);
}

#ifdef WIN32
void TestBstrString()
{
	BstrString b1 = "Dupa";
	BstrString b2 = L"dupA";
	assert(BstrString::Cmp(b1, b2, true) == 0);
	BstrString b3 = b1 + b2;
	BstrString b4 = b3.substr(3, 2);
	b4 += string("am");
	assert(b4.length() == 4);
	cout << "Here should be \"adam\": " << b4.c_str() << endl;
}
#endif // #ifdef WIN32


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH
// Test for ObjList

// HPP

class ItemClassStatic
{
public:
	ItemClassStatic();
	~ItemClassStatic();

	STATIC_OBJLIST_ITEM_METHODS(ListClassStatic, ItemClassStatic, NAME)

private:
	STATIC_OBJLIST_ITEM_FIELDS(ListClassStatic, ItemClassStatic, NAME)
};

class ListClassStatic
{
public:
	STATIC_OBJLIST_LIST_METHODS(ListClassStatic, ItemClassStatic, NAME)

private:
	STATIC_OBJLIST_LIST_FIELDS(ListClassStatic, ItemClassStatic, NAME)
};

class ListClass;

class ItemClass
{
public:
	ItemClass();
	~ItemClass();

	OBJLIST_ITEM_METHODS(ListClass, ItemClass, NAME)

private:
	OBJLIST_ITEM_FIELDS(ListClass, ItemClass, NAME)
};

class ListClass
{
public:
	ListClass();
	~ListClass();

	OBJLIST_LIST_METHODS(ListClass, ItemClass,NAME)

private:
	OBJLIST_LIST_FIELDS(ListClass, ItemClass, NAME)
};

// CPP

ItemClassStatic::ItemClassStatic()
{
	STATIC_OBJLIST_ITEM_CTOR(ListClassStatic, ItemClassStatic, NAME);
}

ItemClassStatic::~ItemClassStatic()
{
	STATIC_OBJLIST_ITEM_DTOR(ListClassStatic, ItemClassStatic, NAME);
}

STATIC_OBJLIST_LIST_IMPL(ListClassStatic, ItemClassStatic, NAME)

ItemClass::ItemClass()
{
	OBJLIST_ITEM_CTOR(ListClass, ItemClass, NAME)
}

ItemClass::~ItemClass()
{
	OBJLIST_ITEM_DTOR(ListClass, ItemClass, NAME)
}

OBJLIST_LIST_IMPL(ListClass, ItemClass, NAME)

ListClass::ListClass()
{
	OBJLIST_LIST_CTOR(ListClass, ItemClass, NAME)
}

ListClass::~ListClass()
{
	OBJLIST_LIST_DTOR(ListClass, ItemClass, NAME)
}

void TestObjListStatic()
{
	ItemClassStatic i1, i2, i3;
	ListClassStatic::NAME_InsertBack(&i1);
	ListClassStatic::NAME_InsertBack(&i2);
	ListClassStatic::NAME_InsertBack(&i3);
	assert(ListClassStatic::NAME_GetItemCount() == 3);
	uint count = 0;
	for (ItemClassStatic *i = ListClassStatic::NAME_GetFirst(); i != NULL; i = i->NAME_GetNext())
	{
		switch (count)
		{
		case 0: assert(i == &i1); break;
		case 1: assert(i == &i2); break;
		case 2: assert(i == &i3); break;
		}
		count++;
	}
	assert(count == 3);
	ListClassStatic::NAME_Remove(&i2);
	ListClassStatic::NAME_Remove(&i1);
	assert(ListClassStatic::NAME_GetItemCount() == 1);
	ListClassStatic::NAME_Clear();
	assert(ListClassStatic::NAME_GetItemCount() == 0);
	assert(i1.NAME_IsOnList() == false);
	assert(i2.NAME_IsOnList() == false);
	assert(i3.NAME_IsOnList() == false);
}

void TestObjListNormal()
{
	ListClass myList;
	ItemClass i1, i2, i3;
	myList.NAME_InsertBack(&i1);
	myList.NAME_InsertBack(&i2);
	myList.NAME_InsertBack(&i3);
	assert(myList.NAME_GetItemCount() == 3);
	uint count = 0;
	for (ItemClass *i = myList.NAME_GetFirst(); i != NULL; i = i->NAME_GetNext())
	{
		switch (count)
		{
		case 0: assert(i == &i1); break;
		case 1: assert(i == &i2); break;
		case 2: assert(i == &i3); break;
		}
		count++;
	}
	assert(count == 3);
	myList.NAME_Remove(&i2);
	myList.NAME_Remove(&i1);
	assert(myList.NAME_GetItemCount() == 1);
	myList.NAME_Clear();
	assert(myList.NAME_GetItemCount() == 0);
	assert(i1.NAME_IsOnList() == false);
	assert(i2.NAME_IsOnList() == false);
	assert(i3.NAME_IsOnList() == false);
}

void TestObjList()
{
	WriteLine(_T("==================== OBJ LIST ===================="));
	
	TestObjListStatic();
	TestObjListNormal();
}

void TestTokDoc()
{
	WriteLine(_T("==================== TOK DOC ===================="));

	using namespace tokdoc;

	const int intVal = 1;
	const float floatVal = 1.f;
	const VEC4 vectorVal = VEC4(1.f, 2.f, 3.f, 4.f);
	tstring stringVal = _T("String 1");
	const int arr[] = { 1, 2, 3, 4 };

	tstring docStr;

	{
		Node doc;
		
		{
			Node config;
			SubnodeFrom(config, _T("int8"), intVal);
			SubnodeFrom(config, _T("float1"), floatVal);
			SubnodeFrom(config, _T("vector1"), vectorVal);
			SubnodeFrom(config, _T("string1"), stringVal);
			
			{
				Node vecNode;
				NodeFromVector(vecNode, std::vector<int>(arr, arr+_countof(arr)));
				vecNode.Name = _T("stdvector1");
				config.InsertChildAtEnd(vecNode);
			}

			config.LinkChildAtBegin(new Node(_T("LonelyVal")));

			Node *docConfigNode = new Node();
			doc.LinkChildAtEnd(docConfigNode);
			docConfigNode->CopyFrom(config);
			docConfigNode->Name = _T("Config");
		}

		{
			TokenWriter tok(&docStr);
			tok.RegisterSymbol(_T('{'), true, true, 1);
			tok.RegisterSymbol(_T('}'), true, true, -1);
			tok.RegisterSymbol(_T('='), true, true, 0);
			tok.RegisterSymbol(_T(','), false, true, 0);
			tok.RegisterSymbol(_T(';'), false, true, 0);
			doc.SaveChildren(tok);
		}
	}

	WriteLine(docStr);

	{
		Node doc;

		{
			Tokenizer tok(&docStr, Tokenizer::FLAG_MULTILINE_STRINGS);
			tok.Next();
			doc.LoadChildren(tok);
		}

		Node &configNode = doc.MustFindFirstChild(_T("Config"));

		int intTmp;
		SubnodeTo(intTmp, configNode, _T("int8"), SUBNODE_REQUIRED);
		assert(intTmp == intVal);

		float floatTmp;
		SubnodeTo(floatTmp, configNode, _T("float1"), SUBNODE_REQUIRED);
		assert(floatTmp == floatVal);

		VEC4 vectorTmp;
		SubnodeTo(vectorTmp, configNode, _T("vector1"), SUBNODE_REQUIRED);
		assert(vectorTmp == vectorVal);

		tstring stringTmp;
		SubnodeTo(stringTmp, configNode, _T("string1"), SUBNODE_REQUIRED);
		assert(stringTmp == stringVal);

		std::vector<int> stdVectorTmp;
		Node &vectorNode = configNode.MustFindFirstChild(_T("stdvector1"));
		NodeToVector(stdVectorTmp, vectorNode, true);
		assert(stdVectorTmp == std::vector<int>(arr, arr+_countof(arr)));

		assert(configNode.GetFirstChild() && configNode.GetFirstChild()->Name == EMPTY_STRING);
	}
}


//HHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHHH

void Test()
{
	Initialize();
	TestBase();
	TestError();
	TestMath();
	TestProfiler();
	TestFlatProfiler();
	TestStream();
	TestEncoderDecoder();
	TestSmartPointers();
	TestFreeList();
	TestDynamicFreeList();
	TestZlibUtils();
	TestFiles();
	TestDateTime();
	TestCmdLineParser();
	TestTokenizer();
	TestThreads();
	TestLogger();
#ifdef WIN32
	TestBstrString();
#endif
	TestObjList();
	TestTokDoc();

	tstring Result;
	g_Profiler.FormatString(&Result, PROFILER_UNITS_MILLISECONDS);
	tcout << _T("Result:") << endl << Result << endl;
}

#ifdef WIN32
int _tmain(int argc, tchar* argv[], tchar* envp[])
#else
int main(int argc, char* argv[])
#endif
{
	setlocale(LC_ALL, "polish");

	try
	{
		Test();
	}
	catch (Error Err)
	{
		tcout << _T("!!!!!!!!!!! EXCEPTION !!!!!!!!!!!") << endl;
		tstring s;
		Err.GetMessage_(&s);
		WriteLine(s);
	}
	return 0;
}

#ifdef USE_DIRECTX
#pragma comment(lib, "d3d9.lib")
#pragma comment(lib, "d3dx9.lib")
#pragma comment(lib, "dxerr.lib")
#endif
