#include "win32.h"

window_t* window = NULL;

static LRESULT CALLBACK window_callback(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
		case WM_CLOSE:
			PostQuitMessage(0);
			break;
		case WM_KEYDOWN:
			//window->keys[wParam & 511] = 1;
			break;
		case WM_KEYUP:
			//window->keys[wParam & 511] = 0;
			break;
		case WM_LBUTTONDOWN:
			//window->mouse_buttons[0] = 1;
			break;
		case WM_LBUTTONUP:
			//window->mouse_buttons[0] = 0;
			break;
		case WM_RBUTTONDOWN:
			//window->mouse_buttons[1] = 1;
			break;
		case WM_RBUTTONUP:
			//window->mouse_buttons[1] = 0;
			break;
		case WM_MOUSEWHEEL:
			//window->mouse_wheel += GET_WHEEL_DELTA_WPARAM(wParam);
			break;
		case WM_DESTROY:
			return 0;
		default:
			break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}


/*
	UINT        style;
	WNDPROC     lpfnWndProc;
	int         cbClsExtra;
	int         cbWndExtra;
	HINSTANCE   hInstance;
	HICON       hIcon;
	HCURSOR     hCursor;
	HBRUSH      hbrBackground;
	LPCSTR      lpszMenuName;
	LPCSTR      lpszClassName;
*/

static void register_window_class()
{
	WNDCLASS wc;
	wc.style = CS_OWNDC;											//���ڵķ��CS_OWNDC��ʾÿ�δ�������ʱ�������һ��Ψһ���豸������
	wc.lpfnWndProc = NULL;											//���ڵĻص�������Ҳ���Ǵ��ڽ��յ���Ϣ�󣬽����ĸ���������
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = GetModuleHandle(NULL);							//��ȡ��ǰ���̵ľ��
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);				//���ع��  �����ʽ
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);				//������ͼ��
	wc.hbrBackground = (HBRUSH)GetStockObject(BLACK_BRUSH);		//������ʽ
	wc.lpszClassName = L"ToyRenderer";
	wc.lpszMenuName = NULL;											//���ô���Ĳ˵�,û�У���NULL
	RegisterClass(&wc);
}


void bm_init(int width, int height)
{
	BITMAPINFO bm_info = { 0 };
	bm_info.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bm_info.bmiHeader.biWidth = width;
	bm_info.bmiHeader.biHeight = -height;
	bm_info.bmiHeader.biPlanes = 1;
	bm_info.bmiHeader.biBitCount = 32;
	bm_info.bmiHeader.biCompression = BI_RGB;
	bm_info.bmiHeader.biSizeImage = 0;
	bm_info.bmiHeader.biXPelsPerMeter = 0;
	bm_info.bmiHeader.biYPelsPerMeter = 0;
	bm_info.bmiHeader.biClrUsed = 0;
	bm_info.bmiHeader.biClrImportant = 0;

	window->bm_dib = CreateDIBSection(window->mem_dc, &bm_info, DIB_RGB_COLORS, (void**)&window->window_fb, NULL, 0);
	window->bm_old = (HBITMAP)SelectObject(window->mem_dc, window->bm_dib);
}

int window_init(int width, int height, const char* title)
{
	window = (window_t*)malloc(sizeof(window_t));
	window->width = width;
	window->height = height;
	window->is_close = 0;

	window->h_window = CreateWindow(L"ToyRenderer", L"ToyRenderer", WS_OVERLAPPEDWINDOW, 0, 0, width, height, NULL, NULL, GetModuleHandle(NULL), NULL);
	window->mem_dc = GetDC(window->h_window);

	ShowWindow(window->h_window, SW_SHOW);

	msg_dispatch();
	return 0;
}

int window_destroy()
{
	return 0;
}

int window_draw()
{
	return 0;
}


void msg_dispatch()
{
	MSG msg;
	while (true)
	{
		// Peek��������Get��������PM_NOREMOVE��ʾ�������Ϣ������������������Get����
		if (!PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))	break;	//�����Ϣ�������Ƿ�����Ϣ
		if (!GetMessage(&msg, NULL, 0, 0)) break;				//����Ϣ�����л�ȡ��Ϣ
		if (msg.message == WM_QUIT)		break;				//������˳���Ϣ�����˳�ѭ��

		TranslateMessage(&msg);							//���������Ϣת��Ϊ�ַ���Ϣ
		DispatchMessage(&msg);							//����Ϣ���ɸ����ڳ���
		
	}
}
