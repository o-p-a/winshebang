/*
	winshebang

	2011/08/05 opa
*/

#define PGM					"winshebang"
#define PGM_DEBUG			PGM ": "
#define PGM_INFO			PGM ": "
#define PGM_WARN			PGM " warning: "
#define PGM_ERR				PGM " error: "
#define VERSTR				"0.01"

#define CREDIT2011			"Copyright (c) 2011 by opa"

typedef signed char schar;
typedef unsigned char uchar;
typedef signed int sint;
typedef unsigned int uint;
typedef signed long slong;
typedef unsigned long ulong;

using namespace std;

char
	credit[] = PGM " version " VERSTR " " CREDIT2011;
bool
	error = false;
sint
	rcode = 0;


















void show_credit()
{
	putcout(credit);
}

#if defined(__CONSOLE__)

extern "C"
sint wmain(sint, wchar_t *[])
{
	wrapexec_main();

	return rcode;
}

#else // __CONSOLE__

// ダミーのメッセージを送受信することで、プログラムが動き出したことをOSに伝える
void dummy_message()
{
	MSG msg;
	PostMessage(NULL, WM_APP, 0, 0);
	GetMessage(&msg, NULL, WM_APP, WM_APP);
}

extern "C"
sint WINAPI wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int)
{
	dummy_message();
	putcxxx_string.clear();

	wrapexec_main();

	if(putcxxx_string.size() > 0)
		MessageBox(0, putcxxx_string.c_str(), WindowsAPI::GetModuleFileName().basename().c_str(), MB_ICONINFORMATION + MB_OK);

	return rcode;
}

#endif // __CONSOLE__
