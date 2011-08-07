/* -*- mode:cc; tab-width:4; coding:Windows-31J; -*- */
/* vi:set ft=cpp ts=4 fenc=Windows-31J : */

/*
	winshebang

	2011/08/05 opa
*/

#include <string>
#include <algorithm>
#include <dir>

#include <windows.h>

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

////////////////////////////////////////////////////////////////////////

template <class BidirectionalIterator, class T>
BidirectionalIterator rfind(BidirectionalIterator first, BidirectionalIterator last, const T &value)
{
	while(first != last){
		--last;
		if(*last == value)
			break;
	}

	return last;
}

template <class BidirectionalIterator, class Predicate>
BidirectionalIterator rfind_if(BidirectionalIterator first, BidirectionalIterator last, Predicate pred)
{
	while(first != last){
		--last;
		if(pred(*last))
			break;
	}

	return last;
}

inline bool isbackslash(wchar_t c)
{
	return c == L'\\' || c == L'/';
}

bool file_is_exist(const wchar_t *filename)
{
#if 1
	_wffblk ff;
	sint r;

	r = _wfindfirst(filename, &ff, FA_NORMAL + FA_HIDDEN + FA_SYSTEM);
	_wfindclose(&ff);

	return r == 0;
#else
	return _waccess(filename, 0) == 0;
#endif
}

inline bool file_is_exist(const wstring &filename)
{
	return file_is_exist(filename.c_str());
}

////////////////////////////////////////////////////////////////////////

class String : public wstring {
	typedef String Self;
	typedef wstring Super;

public:
	String();
	String(const Super &s);
	String(const wchar_t *s);
	String(const_iterator b, const_iterator e);
	String(const char *s);
	~String();

	string to_ansi() const;
	Self to_upper() const;
	Self trim() const;
	bool isdoublequote() const;
	Self doublequote() const;
	Self doublequote_del() const;

	Self &operator=(const Self &s);
	Self &operator=(const wchar_t *s);
	Self &operator=(const char *s);

	Self &operator+=(const Self &s)				{ append(s); return *this; }
	Self &operator+=(const wchar_t *s)			{ append(s); return *this; }
	Self &operator+=(const char *s);

	Self &assign_from_ansi(const char *s);
	Self &assign_from_ansi(const string &s)		{ return assign_from_ansi(s.c_str()); }
	Self &assign_from_utf8(const char *s);
	Self &assign_from_utf8(const string &s)		{ return assign_from_utf8(s.c_str()); }
	Self &assign_from_env(const Self &name);
	Self &printf(Self format, ...);

	// filename operator
	bool have_path() const;
	bool have_ext() const;
	bool isbackslash() const;
	Self backslash() const;
	Self subext(const Self &ext) const;
	Self drivename() const;
	Self dirname() const;
	Self basename() const;
};

String::String()									{}
String::String(const String::Super &s)				: Super(s) {}
String::String(const wchar_t *s)					: Super(s) {}
String::String(const_iterator b, const_iterator e)	: Super(b, e) {}
String::String(const char *s)						{ assign_from_ansi(s); }
String::~String() {}
String &String::operator=(const String &s)			{ assign(s); return *this; }
String &String::operator=(const wchar_t *s)			{ assign(s); return *this; }
String &String::operator=(const char *s)			{ assign_from_ansi(s); return *this; }
String &String::operator+=(const char *s)			{ append(String(s)); return *this; }
String operator+(const String &s1, const char *s2)	{ return s1 + String(s2); }
String operator+(const char *s1, const String &s2)	{ return String(s1) + s2; }
bool operator==(const String &s1, const char *s2)	{ return s1 == String(s2); }
bool operator==(const char *s1, const String &s2)	{ return String(s1) == s2; }

string String::to_ansi() const
{
	sint
		siz = WideCharToMultiByte(CP_ACP, 0, c_str(), size(), NULL, 0, NULL, NULL);
	char
		*buf = new char[siz+1];

	fill(buf, buf + siz+1, 0);

	WideCharToMultiByte(CP_ACP, 0, c_str(), size(), buf, siz, NULL, NULL);

	string
		r(buf);

	delete [] buf;

	return r;
}

String &String::assign_from_ansi(const char *s)
{
	sint
		size = MultiByteToWideChar(CP_ACP, 0, s, -1, NULL, 0);
	wchar_t
		*buf = new wchar_t[size+1];

	fill(buf, buf + size+1, 0);

	MultiByteToWideChar(CP_ACP, 0, s, -1, buf, size);

	assign(buf);

	delete [] buf;

	return *this;
}

String String::subext(const String &ext) const
{
	const_iterator
		e = ::rfind(begin(), end(), L'.');

	if(e != end() && *e != L'.')
		e = end();

	return Self(begin(), e).append(ext);
}

String String::basename() const
{
	const_iterator
		b = begin(),
		e = end();

	b = ::rfind_if(b, e, ::isbackslash);
	if(b != end() && ::isbackslash(*b))
		++b;

	e = ::rfind(b, e, L'.');
	if(e != end() && *e != L'.')
		e = end();

	return Self(b, e);
}

////////////////////////////////////////////////////////////////////////

#if defined(__CONSOLE__)

void _putcxxx(const char *s, FILE *fp)
{
	fputs(s, fp);
	fputc('\n', fp);
}

#else // __CONSOLE__

String
	consolebuf;

void _putcxxx(const char *s, FILE *)
{
	if(consolebuf.size() > 0)
		consolebuf += "\r\n";
	consolebuf += s;
}

#endif // __CONSOLE__

inline void putcout(const char *s)
{
	_putcxxx(s, stdout);
}

inline void putcerr(const char *s)
{
	_putcxxx(s, stderr);
}

void _putcxxx(const String &s, FILE *fp)
{
	_putcxxx(s.to_ansi().c_str(), fp);
}

inline void putcout(const String &s)
{
	_putcxxx(s, stdout);
}

inline void putcerr(const String &s)
{
	_putcxxx(s, stderr);
}

void _putcxxx(const char *s1, const String &s2, FILE *fp)
{
	_putcxxx(String().assign_from_ansi(s1) + s2, fp);
}

inline void putcout(const char *s1, const String &s2)
{
	_putcxxx(s1, s2, stdout);
}

inline void putcerr(const char *s1, const String &s2)
{
	_putcxxx(s1, s2, stderr);
}

bool case_ignore_equal(const String &s1, const String &s2)
{
	return s1.to_upper() == s2.to_upper();
}

////////////////////////////////////////////////////////////////////////

class WindowsAPI {
public:
	static bool CreateProcess(const String &cmd, DWORD CreationFlags, const String &wd, LPSTARTUPINFO si, LPPROCESS_INFORMATION pi);
	static String GetClipboardText();
	static String GetCommandLine()		{ return ::GetCommandLine(); }
	static String GetComputerName();
	static String GetModuleFileName(HMODULE Module = 0);
	static String GetTempPath();
	static String GetUserName();
	static String SHGetSpecialFolder(sint nFolder);
};

bool WindowsAPI::CreateProcess(const String &cmd, DWORD CreationFlags, const String &wd, LPSTARTUPINFO si, LPPROCESS_INFORMATION pi)
{
	bool
		r;
	wchar_t
		*cmd_c_str;

	cmd_c_str = new wchar_t[cmd.size()+1];
	copy(cmd.begin(), cmd.end(), cmd_c_str);
	cmd_c_str[cmd.size()] = 0;

	r = ::CreateProcess(NULL, cmd_c_str, NULL, NULL, TRUE, CreationFlags, NULL, (wd.size()>0 ? wd.c_str() : NULL), si, pi);

	delete [] cmd_c_str;

	return r;
}

String WindowsAPI::GetModuleFileName(HMODULE Module)
{
	wchar_t
		buf[MAX_PATH+1];
	DWORD
		size = sizeof buf / sizeof(wchar_t);

	size = ::GetModuleFileName(Module, buf, size);

	if(size == 0)
		return String();

	return String(buf, buf + size);
}

////////////////////////////////////////////////////////////////////////

String get_given_option(const String &cmdline)
{
	// コマンドライン文字列から、コマンド名を除いた残りの部分(コマンドに与えるパラメータの部分)を返す

	bool
		in_quote = false;

	for(String::const_iterator s = cmdline.begin() ; s != cmdline.end() ; ++s){
		wchar_t
			c = *s;

		if(!in_quote)
			if(iswspace(c))
				return String(s, cmdline.end());

		if(c == L'"')
			in_quote = in_quote ? false : true;
	}

	return String();
}

void show_credit()
{
	putcout(credit);
}

void execute_shebang(String scriptname, String arg)
{
}

void execute_batchfile(String scriptname, String arg)
{
}

void execute_wscript(String scriptname, String arg)
{
}

void execute_awk(String scriptname, String arg)
{
}

void execute_perl(String scriptname, String arg)
{
}

void execute_php(String scriptname, String arg)
{
}

void execute_python(String scriptname, String arg)
{
}

void execute_ruby(String scriptname, String arg)
{
}

void winshebang_main()
{
	String
		exename = WindowsAPI::GetModuleFileName(),
		arg = get_given_option(WindowsAPI::GetCommandLine()),
		s;

putcout("[" + exename + "]");
putcout("[" + arg + "]");

	if(s = exename.subext(""), file_is_exist(s)){
		execute_shebang(s, arg);
	}else if(s = exename.subext(".bat"), file_is_exist(s)){
		execute_batchfile(s, arg);
	}else if(s = exename.subext(".cmd"), file_is_exist(s)){
		execute_batchfile(s, arg);
	}else if(s = exename.subext(".js"), file_is_exist(s)){
		execute_wscript(s, arg);
	}else if(s = exename.subext(".vbs"), file_is_exist(s)){
		execute_wscript(s, arg);
	}else if(s = exename.subext(".awk"), file_is_exist(s)){
		execute_awk(s, arg);
	}else if(s = exename.subext(".pl"), file_is_exist(s)){
		execute_perl(s, arg);
	}else if(s = exename.subext(".php"), file_is_exist(s)){
		execute_php(s, arg);
	}else if(s = exename.subext(".py"), file_is_exist(s)){
		execute_python(s, arg);
	}else if(s = exename.subext(".pyw"), file_is_exist(s)){
		execute_python(s, arg);
	}else if(s = exename.subext(".rb"), file_is_exist(s)){
		execute_ruby(s, arg);
	}else if(s = exename.subext(".rbw"), file_is_exist(s)){
		execute_ruby(s, arg);
	}else{
		putcout(credit);
		putcout("");
		putcout("Error: shebang target file not found.");
	}
}

#if defined(__CONSOLE__)

extern "C"
sint wmain(sint, wchar_t *[])
{
	winshebang_main();
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
	consolebuf.clear();

	winshebang_main();

	if(consolebuf.size() > 0)
		MessageBox(0, consolebuf.c_str(), WindowsAPI::GetModuleFileName().basename().c_str(),
					MB_ICONINFORMATION + MB_OK);

	return rcode;
}

#endif // __CONSOLE__
