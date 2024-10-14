#include <Application.h>

#ifdef _WIN32
#include <windows.h>

int APIENTRY WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR lpCmdLine,int nCmdShow)
{
	Application::GetInstance().run();
	return 0;
}

#endif

int main(int argc, char* args[])
{
	Application::GetInstance().run();
	return 0;
}