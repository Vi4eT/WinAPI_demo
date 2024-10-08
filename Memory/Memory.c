#include <windows.h>
enum //ID ��� �������� ����, ����� ���������� � �������� ������������� ����
{
  MenuDefault,
  MenuNew,
};

static int MemoryCreate(HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew,  int **ArrayNew)
{
  *hHeapDefault = GetProcessHeap();                                                 //�������� ���������� ���� �� ���������
  *ArrayDefault = (int*)HeapAlloc(*hHeapDefault, HEAP_ZERO_MEMORY, sizeof(int));    //�������� ������ � ���� �� ��������� ��� ���� int � ��������
  *hHeapNew = HeapCreate(0, sizeof(int), 0);                                        //������� ����� ���� ������� int ��� ����������� ���� �������
  *ArrayNew = (int*)HeapAlloc(*hHeapNew, HEAP_ZERO_MEMORY, sizeof(int));            //�������� ������ � ����� ����
  if(!*ArrayDefault || !*ArrayNew)                                                  //���� ���� ���� �� �������� �� ����������
    return 1;
  return 0;
}

static void MemoryFree(HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew)
{
  HeapFree(*hHeapDefault, 0, *ArrayDefault);    //���������� ������ � ���� �� ���������
  HeapDestroy(*hHeapNew);                       //���������� ����� ���� (����������� ������ �������������)
}

static int MemoryProcess(HWND hWindow, HANDLE *hHeapDefault, int **ArrayDefault, HANDLE *hHeapNew, int **ArrayNew)
{
  BOOL Translated = FALSE;                                                          //���� ��� GetDlgItemInt
  int SizeDefault = (int)GetDlgItemInt(hWindow, MenuDefault, &Translated, FALSE);   //��������� ����� � int, �� �������� �� ����� (��� ���������� ������)
  if(!Translated)
    return 1;
  int SizeNew = (int)GetDlgItemInt(hWindow, MenuNew, &Translated, FALSE);
  if(!Translated)
    return 1;
  //������������ ������ realloc, ��� ��������� �� ������������� ���������
  int* ReAllArray = (int*)HeapReAlloc(*hHeapDefault, HEAP_ZERO_MEMORY, *ArrayDefault, SizeDefault * sizeof(int)); //������������� ������ � ���� �� ���������
  if(!ReAllArray)
    return 1;
  else
    *ArrayDefault = ReAllArray; //���� �������, ��������� ��������� �� ����� ������
  ReAllArray = (int*)HeapReAlloc(*hHeapNew, HEAP_ZERO_MEMORY, *ArrayNew, SizeNew * sizeof(int));
  if(!ReAllArray)
    return 1;
  else
    *ArrayNew = ReAllArray;
  return 0;
}

static LRESULT CALLBACK WindowProcess(_In_ HWND hWindow, _In_ UINT Message, _In_ WPARAM wParam, _In_ LPARAM lParam)
{ //static ���������� ��������� �������� ����� �������� �������
  static HWND hEditDefault, hEditNew, hButton, hStatic;     //����������� �������� ����
  static HANDLE hHeapDefault = NULL, hHeapNew = NULL;       //����������� ���
  static int *ArrayDefault = NULL, *ArrayNew = NULL;        //��������� �� �������
  switch(Message)       //��������� ����������� ���������
  {
    case WM_CREATE:     //��������� �������� ����
    {
      HINSTANCE hInstance = ((LPCREATESTRUCT)lParam)->hInstance;    //��������� ����������� ���������� ����������
      hEditDefault = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, TEXT("Edit"), TEXT("0"), WS_CHILD | WS_VISIBLE | ES_NUMBER,     //���� ��������, �������, ������ ��� ����
                                    30, 80, 70, 20, hWindow, (HMENU)MenuDefault, hInstance, NULL);
      ShowWindow(hEditDefault, SW_SHOWNORMAL);
      hEditNew = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, TEXT("Edit"), TEXT("0"), WS_CHILD | WS_VISIBLE | ES_NUMBER,
                                180, 80, 70, 20, hWindow, (HMENU)MenuNew, hInstance, NULL);
      ShowWindow(hEditNew, SW_SHOWNORMAL);
      hButton = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Button"), TEXT("Start"), WS_CHILD | WS_VISIBLE | BS_DEFPUSHBUTTON,   //������, ���������� �� ���������, �������� WM_COMMAND
                               90, 120, 100, 30, hWindow, NULL, hInstance, NULL);
      ShowWindow(hButton, SW_SHOWNORMAL);
      hStatic = CreateWindowEx(WS_EX_WINDOWEDGE, TEXT("Static"), TEXT("Ready"), WS_CHILD | WS_VISIBLE | SS_CENTER,          //������������ ������ �� ������
                               90, 165, 100, 20, hWindow, NULL, hInstance, NULL);
      ShowWindow(hStatic, SW_SHOWNORMAL);
      if(MemoryCreate(&hHeapDefault, &ArrayDefault, &hHeapNew, &ArrayNew))  //���������� �������� ������
      { //��� ������ ������� Error � �������������� ������
        SetWindowText(hStatic, TEXT("Error"));  //���������� ����� � ����
        EnableWindow(hButton, FALSE);           //�������������� ������
      } 
      break;
    }
    case WM_PAINT:      //��������� ��������� ����
    {
      PAINTSTRUCT PaintStruct;                                  //��������� ���������� �� ���������
      HDC hDeviceContext = BeginPaint(hWindow, &PaintStruct);   //��������� ����������� ���������� �����������
      TextOut(hDeviceContext, 110, 20, TEXT("Data size"), 9);   //����� ������: ���������� �����������, X � Y ������, �����, ����� ������
      TextOut(hDeviceContext, 30, 50, TEXT("Default heap"), 12);
      TextOut(hDeviceContext, 180, 50, TEXT("New heap"), 8);
      EndPaint(hWindow, &PaintStruct);                          //����� ���������
      break;
    }
    case WM_COMMAND:    //��������� ������
      if(lParam == (LPARAM)hButton) //���� ������� �� ������
      {
        if(MemoryProcess(hWindow, &hHeapDefault, &ArrayDefault, &hHeapNew, &ArrayNew)) //������ � �������
        {
          SetWindowText(hStatic, TEXT("Error"));
          EnableWindow(hButton, FALSE);
        }
        else
          SetWindowText(hStatic, TEXT("Done")); //����� ��� ������ �������� ����������
      }
      break;
    case WM_DESTROY:    //�������� ����
      MemoryFree(&hHeapDefault, &ArrayDefault, &hHeapNew);      //������������ ������
      PostQuitMessage(0);                                       //����� � ����� 0
      break;
    default:
      return DefWindowProc(hWindow, Message, wParam, lParam);   //����������� ���������
  }
  return 0;
}

int WINAPI WinMain(_In_ HINSTANCE hInstance, _In_opt_ HINSTANCE hPrevInstance, _In_ LPSTR lpCmdLine, _In_ int nCmdShow)
{
  WNDCLASSEX WindowClass;                                   //��������� ���������� � ������ ����
  ZeroMemory(&WindowClass, sizeof(WNDCLASSEX));             //�������������
  WindowClass.cbSize = sizeof(WNDCLASSEX);                  //������������ ��������� �������
  WindowClass.style = CS_HREDRAW | CS_VREDRAW;              //����������� ���� ��� ��������� ��������
  WindowClass.lpfnWndProc = WindowProcess;                  //������� ��������� ������� ����
  WindowClass.hInstance = hInstance;                        //���������� ���������� ���������� (�������������, ���������� ����������� ���� �� ������, � ��� ����� dll)
  WindowClass.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);   //���� ���� ���� (+1 ����������)
  WindowClass.lpszClassName = TEXT("WindowClass");          //�������� ������ ����
  RegisterClassEx(&WindowClass);                            //����������� ������ ����
  //�������� ��������� ����
  HWND hWindow = CreateWindowEx(WS_EX_OVERLAPPEDWINDOW, //��� ����� ����, ����������� ��� windows
                                TEXT("WindowClass"),    //��� ������ ����
                                TEXT("Memory"),         //��������� ����
                                WS_OVERLAPPEDWINDOW,    //�������� ����� ����, ����������� ��� windows
                                600,                    //�������� �������� ������ ����
                                300,                    //�������� �������� ������ ����
                                300,                    //������ ����
                                250,                    //������ ����
                                NULL,                   //���������� ������������� ����
                                NULL,                   //���������� ���� ��� ������������� ��������� ����
                                hInstance,              //����������, ��������� � �����
                                NULL);                  //�������������� ���������
  ShowWindow(hWindow, nCmdShow);            //��������� ��������� ������ ����
  UpdateWindow(hWindow);                    //���������� ����, �������� WM_PAINT �������� � WindowProcess
  MSG Message;                              //��������� ���������� � ����������
  while(GetMessage(&Message, NULL, 0, 0))   //��������� ���� ��������� �� �������
  {
    TranslateMessage(&Message);             //������� ��������� ������ � ��������� �������� (WM_KEYDOWN -> WM_CHAR)
    DispatchMessage(&Message);              //�������� ���������� � ��������� � WindowProcess
  }
  return Message.wParam;                    //������ �������� ������ ���������� ���������
}