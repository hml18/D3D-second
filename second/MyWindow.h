#pragma once
#include <windows.h>

static bool isPushEsc = false;//�Ƿ���Esc��

class MyWindow
{
public:
	MyWindow();
	~MyWindow();
public:
	HWND GetHandle();//���ش��ھ��
	bool Create(int &width, int &height);//��������
	void Run();//������Ϣѭ��
	LRESULT CALLBACK MessageHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);//��Ϣ����
private:
	HWND m_hwnd;
	HINSTANCE m_hinstance;
	LPCWSTR m_name;
};