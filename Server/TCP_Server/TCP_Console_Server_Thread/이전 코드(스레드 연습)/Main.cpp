#include <iostream>
#include <Windows.h>
using namespace std;

struct Point
{
	int x, y, z;
};

DWORD WINAPI MyThread(LPVOID arg)
{
	Point* p = (Point*)arg;
	while (true)
	{
		cout << "Running MyThread( ) " << GetCurrentThreadId() << " : " 
			<< p->x << ", " << p->y << ", " << p->z << endl;

		Sleep(1000);
	}
	return 0;
}
int main()
{
	Point point{ 10, 20, 30 };
	
	HANDLE hThread1 = CreateThread(NULL, 0, MyThread, &point, 0, NULL);
	if (hThread1 == nullptr)
		return 1;
	CloseHandle(hThread1);

	HANDLE hThread2 = CreateThread(NULL, 0, MyThread, &point, 0, NULL);
	if (hThread2 == nullptr)
		return 1;
	CloseHandle(hThread2);

	while (true)
	{
		cout << "Running main( ) " << GetCurrentThreadId() << endl;
		Sleep(1000);
	}
}