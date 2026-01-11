#include <iostream>

#include "Application.h"

#ifdef _WIN32
#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	try
	{
		Application::GetInstance().run();
	}
	catch (const char* str)
	{
		MessageBoxA(NULL,str,"Exception occurred",MB_ICONERROR | MB_OK);
		return -1;
	}
	return 0;
}
#endif

int main(int argc, char** argv)
{
	try
	{
		Application::GetInstance().run();
	}
	catch (const char* str)
	{
		std::cerr << "Exception occurred:\n";
		std::cerr << "-------------------\n";
		std::cerr << str << std::endl;
		return -1;
	}
	return 0;
}