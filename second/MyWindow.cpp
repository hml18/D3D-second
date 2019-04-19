#include "MyWindow.h"

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam);//��̬�ص�����
static MyWindow *applicationHandle;//���һ����̬ʵ��

MyWindow::MyWindow()
{
	isPushEsc = false;
	m_hwnd = NULL;
	m_name = L"HelloWin";
}


MyWindow::~MyWindow()
{
}


HWND MyWindow::GetHandle()
{
	return m_hwnd;
}

bool MyWindow::Create(int &width, int &height)
{
	WNDCLASSEX wnd;
	applicationHandle = this;
	m_hinstance = GetModuleHandle(NULL);
	wnd.cbClsExtra = 0;
	wnd.cbSize = sizeof(WNDCLASSEX);
	wnd.cbWndExtra = 0;
	wnd.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);
	wnd.hCursor = LoadCursor(NULL, IDC_ARROW);
	wnd.hIcon = LoadIcon(NULL, IDI_WINLOGO);
	wnd.hIconSm = wnd.hIcon;
	wnd.hInstance = m_hinstance;
	wnd.lpfnWndProc = WndProc;
	wnd.lpszClassName = m_name;
	wnd.lpszMenuName = m_name;
	wnd.style = CS_VREDRAW | CS_HREDRAW;

	//ע�ᴰ��
	if (!RegisterClassEx(&wnd))
	{
		MessageBox(NULL, L"ע�ᴰ��ʧ��", L"error", 0);
		return false;
	}
	m_hwnd = CreateWindowEx(WS_EX_APPWINDOW, m_name, m_name, WS_OVERLAPPEDWINDOW, 0, 0, width, height,
		NULL, NULL, m_hinstance, NULL);
	//��ʾ����������Ϊ����
	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);
	return true;
}

static LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;
		//������Ϣ����MessageHandler����
	default:
		return applicationHandle->MessageHandler(hwnd, message, wparam, lparam);
	}
}

LRESULT CALLBACK MyWindow::MessageHandler(HWND hwnd, UINT message, WPARAM wparam, LPARAM lparam)
{
	switch (message)
	{
		//��ⰴ����Ϣ
	case WM_KEYDOWN:
		if (wparam == VK_ESCAPE)//�û������˳���
			isPushEsc = true;
		return 0;

		//������Ϣ����windowsȱʡ����
	default:
		return DefWindowProc(hwnd, message, wparam, lparam);
	}
}

void MyWindow::Run()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	bool isRuning = true;//�����Ƿ��˳���Ϣѭ��
	while (isRuning)
	{
		//����windows��Ϣ
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		if (msg.message == WM_QUIT)
		{
			isRuning = false;
		}
		else//����esc��Ҳ�˳�
		{
			isRuning = !isPushEsc;

			//��Ⱦ�ȴ�����Է������
		}

	}
}