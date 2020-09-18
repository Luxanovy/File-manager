#include "MyForm.h"
#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <commctrl.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <string>
#include <MsHtmcid.h>
#include <atlstr.h>

// 1) Добавить проверку при переименовании внутри файлов!!!

using namespace std;
using namespace Project1;

#define ID_LISTVIEW_1   3001                // Дескриптор меню на левое окно отображения файлов
#define ID_LISTVIEW_2   3002                // Дескриптор меню  на правое окно отображения файлов
 
#define ID_COMBOBOX_1	4001                // Дескриптор меню на левое окно выбора дисков
#define ID_COMBOBOX_2	4002                // Дескриптор меню на правое окно выбора дисков

#define ID_LABEL_1      5001                // Дескриптор меню отображения названия диска на текущий момент в левом окне
#define ID_LABEL_2      5002                // Дескриптор меню отображения названия диска на текущий момент в правом окне    

#define ID_BUTTON_1     6001                // Дескриптор меню клавиши "ок" в функции переименования файла/папки

char NameClassWindow[] = "ClassWindow";     // Имя класса окна
char direction_left[MAX_PATH] = "C:\\*";    // Первоначальные путь фалов левого окна
char direction_right[MAX_PATH] = "C:\\*";   // Первоначальные путь фалов правого окна
char copy_buf_for_name[MAX_PATH];           // Буфер для хранения имени текущего файла       
char old_name_rename[MAX_PATH];             // Буфер для хранения старого пути файла (для переименования) 
char new_name_rename[MAX_PATH];             // Буфер для хранения нового пути файла (для переименования)

HINSTANCE hInstance;                        // Дескриптор экземпляра модуля, который должен быть связан с окном
HWND hWndRename = NULL;                     // Дескриптор окна сообщения переименования
HIMAGELIST g_hImageList = NULL;             // Дескриптор списка изображений кнопок

static HWND hListView_1, hListView_2;       // Дескрипторы на левое и правое окно отображения файлов
static HWND hComboBox_1, hComboBox_2;       // Дескриптор на левое и правое окно выбора дисков
static HWND hLabel_1, hLabel_2;             // Дескриптор левого и правого окон отображения названия диска на текущий момент
static HWND hToolBar;                       // Дескриптор окна TOOLBAR
static HWND hTextEdit;                      // Дескриптор окна ввода текста (функция переименования)
static HWND hButton1;                       // Дескриптор окна подтверждения ввода (функция переименования)

const int ImageListID = 0;                  // Дескриптор доп. функция для изображения кнопок
const int numButtons = 5;                   // Количество клавиш на TOOLBAR
const DWORD buttonStyles = BTNS_AUTOSIZE;   // Стиль клавиш (BTNS_AUTOSIZE - указывает, что элемент управления toolbar не должен назначать стандартную ширину кнопке)
const int bitmapSize = 16;                  // Ширина/высота изображения в пикселях 


int x, yGlobal, width, height;              // Размеры для отображения окна редактирования (функция переименования) 

bool listView_1_selected = false;           // Флаг определения нажатия в левом окне
bool listView_2_selected = false;           // Флаг определения нажатия в правом окне   

TBBUTTON tbButtons[numButtons];             // Массив кнопок на TOOLBAR

LRESULT CALLBACK WndProc(HWND, UINT, UINT, LONG); 
LRESULT CALLBACK staticSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData);
void FindFile(HWND hList, char c_dir[MAX_PATH]);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpszCmdLine, int nCmdShow)
{
	MSG Msg;                                // Структыра содержащая информацию о сообщении из очереди сообщений потока
	HWND hWnd;                              // Дескриптор создаваемого окна
	WNDCLASS WndClass;                      // Структура содержащая атрибуты класса окна

	WndClass.style = CS_VREDRAW | CS_HREDRAW;
	WndClass.cbClsExtra = 0;
	WndClass.cbWndExtra = 0;
	WndClass.lpfnWndProc = WndProc;
	WndClass.hInstance = hInstance;
	WndClass.hCursor = (HCURSOR)LoadImage(0,0, IMAGE_CURSOR, SM_CXCURSOR, SM_CYCURSOR, LR_VGACOLOR); 
	WndClass.hbrBackground = (HBRUSH)(COLOR_BTNFACE + 1);
	WndClass.hIcon = (HICON)LoadImage(0, 0, IMAGE_ICON, SM_CXCURSOR, SM_CYCURSOR, LR_VGACOLOR);
	WndClass.lpszMenuName = NULL;
	WndClass.lpszClassName = NameClassWindow;

	if (!RegisterClass(&WndClass))
		return FALSE;

	hWnd = CreateWindow(
		NameClassWindow,                     // Имя класса окна
		"FILE MANAGER",                      // Имя окна
		WS_BORDER | WS_SYSMENU | WS_VISIBLE, // Стиль создаваемого окна (WS_BORDER - тонкая линия границ окна, WS_SYSMENU - меню в строке заголовка окна, WS_VISIBLE - окно изначально в видимом состоянии)
		100, 100,                            // Место появления окна (координаты х и y)
		910, 620,                            // Ширина и высота, в блоках устройства, окна.
		0,                                   // Дескриптор для родительского или владельца создаваемого окна. (Так как это родительское окно, то значение равно 0)
		0,                                   // Дескритор меню
		hInstance,                           // Дескриптор экземпляра модуля, который должен быть связан с окном
		0);                                  // Указатель на значение, передаваемое в окно через структуру CREATESTRUCT на которое указывает параметр lParam сообщения WM_CREATE

	MSG msg = {};
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
	return Msg.wParam;
}

void InitToolbar()  // Инициализация TOOLBAR
{
	tbButtons[0].iBitmap = MAKELONG(STD_FILENEW, ImageListID);
	tbButtons[0].idCommand = IDM_NEW;
	tbButtons[0].fsState = TBSTATE_ENABLED;
	tbButtons[0].fsStyle = buttonStyles;
	tbButtons[0].dwData = 0L;
	tbButtons[0].iString = (INT_PTR)"Notepad";

	tbButtons[1].iBitmap = MAKELONG(STD_PROPERTIES, ImageListID);
	tbButtons[1].idCommand = IDM_RENAME;
	tbButtons[1].fsState = TBSTATE_ENABLED;
	tbButtons[1].fsStyle = buttonStyles;
	tbButtons[1].dwData = 0L;
	tbButtons[1].iString = (INT_PTR)"Переименовать";

	tbButtons[2].iBitmap = MAKELONG(STD_REPLACE, ImageListID);
	tbButtons[2].idCommand = IDM_MOVE;
	tbButtons[2].fsState = TBSTATE_ENABLED;
	tbButtons[2].fsStyle = buttonStyles;
	tbButtons[2].dwData = 0L;
	tbButtons[2].iString = (INT_PTR)"Переместить";

	tbButtons[3].iBitmap = MAKELONG(STD_COPY, ImageListID);
	tbButtons[3].idCommand = IDM_COPY;
	tbButtons[3].fsState = TBSTATE_ENABLED;
	tbButtons[3].fsStyle = BTNS_AUTOSIZE;
	tbButtons[3].dwData = 0L;
	tbButtons[3].iString = (INT_PTR)"Копировать";

	tbButtons[4].iBitmap = MAKELONG(STD_DELETE, ImageListID);
	tbButtons[4].idCommand = IDM_DELETE;
	tbButtons[4].fsState = TBSTATE_ENABLED;
	tbButtons[4].fsStyle = buttonStyles;
	tbButtons[4].dwData = 0L;
	tbButtons[4].iString = (INT_PTR)"Удалить";
}

HWND CreateSimpleToolbar(HWND hWnd) // Создание TOOLBAR
{
	HWND hWndToolbar = CreateWindowEx(0, TOOLBARCLASSNAME, NULL, WS_CHILD | TBSTYLE_WRAPABLE, 0, 0, 0, 0, hWnd, NULL, hInstance, NULL);
	if (hWndToolbar == NULL) return NULL;

	g_hImageList = ImageList_Create(bitmapSize, bitmapSize, ILC_COLOR16 | ILC_MASK, numButtons, 0); // Создание списка изображений для кнопок

	SendMessage(hWndToolbar, TB_SETIMAGELIST, (WPARAM)ImageListID, (LPARAM)g_hImageList); // Задание список изображений, который панель инструментов использует для отображения кнопок, находящихся в состоянии по умолчанию
	SendMessage(hWndToolbar, TB_LOADIMAGES, (WPARAM)IDB_STD_SMALL_COLOR, (LPARAM)HINST_COMMCTRL); // Загрузка системных изображений кнопок в список изображений элемента управления панели инструментов

	InitToolbar(); // Инициализация кнопок

	SendMessage(hWndToolbar, TB_BUTTONSTRUCTSIZE, (WPARAM)sizeof(TBBUTTON), 0); // Задание размера структуры TBBUTTON
	SendMessage(hWndToolbar, TB_ADDBUTTONS, (WPARAM)numButtons, (LPARAM)&tbButtons); // Добавление кнопок на панель инструментов
	SendMessage(hWndToolbar, TB_AUTOSIZE, 0, 0);

	ShowWindow(hWndToolbar, TRUE); // Показ TOOLBAR
	return hWndToolbar;
}

void ComboboxProcessing(HWND hComboBox, HWND hLabel, HWND hListView, char _dir[MAX_PATH]) // Обработка запроса COMBOBOX
{
	char path[80];
	int sel;
	strcpy(path, "");
	sel = SendMessage(hComboBox, CB_GETCURSEL, 0, 0);
	SendMessage(hComboBox, CB_GETLBTEXT, sel, (LPARAM)path);
	strcat(path, "\*");
	strcpy(_dir, path);
	SetWindowText(hLabel, _dir);
	FindFile(hListView, _dir);
}

void AddColumnToListView(char* st, int sub, int size) // Добавление окна колнки для файлов
{
	LVCOLUMN lvc;
	lvc.mask = LVCF_FMT | LVCF_WIDTH | LVCF_TEXT | LVCF_SUBITEM;
	lvc.iSubItem = sub;
	lvc.pszText = st;
	lvc.cx = size;
	lvc.fmt = LVCFMT_LEFT;

	ListView_InsertColumn(hListView_1, sub, &lvc);
	ListView_InsertColumn(hListView_2, sub, &lvc);
}

BOOL InitializationListViewImage(HWND hWndListView, int size, char c_dir[MAX_PATH]) // Инициализация иконок на все файлы в выбронной директории
{
	HIMAGELIST hSmall;
	SHFILEINFO lp;
	hSmall = ImageList_Create(GetSystemMetrics(SM_CXSMICON), GetSystemMetrics(SM_CYSMICON), ILC_MASK | ILC_COLOR32, size + 2, 1); // Создание списка изображений

	WIN32_FIND_DATA FindFileData;
	HANDLE hFind;

	hFind = FindFirstFile(c_dir, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE) MessageBoxA(0, "Ошибка", "Не найден", MB_OK | MB_ICONWARNING);
	else
	{
		do { //присвоение атрибутов
			if (strcmp(FindFileData.cFileName, ".") == 0) { //Если диск
				char buf1[MAX_PATH];
				strcpy(buf1, c_dir);
				strcat(buf1, FindFileData.cFileName);
				SHGetFileInfo("", FILE_ATTRIBUTE_DEVICE, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);
				ImageList_AddIcon(hSmall, lp.hIcon);
				DestroyIcon(lp.hIcon);
			}
			if (strcmp(FindFileData.cFileName, "..") == 0)  {// Если фаилы, папки
				char buf1[MAX_PATH];
				strcpy(buf1, c_dir);
				strcat(buf1, FindFileData.cFileName);
				SHGetFileInfo("", FILE_ATTRIBUTE_DIRECTORY, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);
				ImageList_AddIcon(hSmall, lp.hIcon);
				DestroyIcon(lp.hIcon);
			}
			char buf1[MAX_PATH]; // Присвоение иконки
			strcpy(buf1, c_dir);
			buf1[strlen(buf1) - 1] = 0;
			strcat(buf1, FindFileData.cFileName);
			DWORD num = GetFileAttributes(buf1);
			SHGetFileInfo(buf1, num, &lp, sizeof(lp), SHGFI_ICONLOCATION | SHGFI_ICON | SHGFI_SMALLICON);
			ImageList_AddIcon(hSmall, lp.hIcon);
			DestroyIcon(lp.hIcon);
		} while (FindNextFile(hFind, &FindFileData) != 0);
		FindClose(hFind);
	}
	ListView_SetImageList(hWndListView, hSmall, LVSIL_SMALL);
	return TRUE;
}

void View_List(char* buf, HWND hList, int i, int j) // Добавление самих файлов с изображением и текстом
{
	LVITEM lvItem;
	lvItem.mask = LVIF_IMAGE | LVIF_TEXT;
	lvItem.state = 0;
	lvItem.stateMask = 0;
	lvItem.iItem = i;
	lvItem.iImage = i;
	lvItem.iSubItem = j;
	lvItem.pszText = buf;
	lvItem.cchTextMax = sizeof(buf);
	ListView_InsertItem(hList, &lvItem); // Добавление нового элемена в элемент управления представления списка
}

void FindFile(HWND hList, char c_dir[MAX_PATH]) // Поиск указанного файла
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind; // Дескриптор к объекту HWND ручка к окну
	int i = 0;

	SendMessage(hList, LVM_DELETEALLITEMS, (WPARAM)0, (LPARAM)0);
	hFind = FindFirstFile(c_dir, &FindFileData);        // Поиск указанного файла/папки
	if (hFind == INVALID_HANDLE_VALUE) MessageBoxA(0, "Ошибка", "Не найден", MB_OK | MB_ICONWARNING); // Проверка найден ли файл/папка
	else
	{
		do
		{
			View_List(FindFileData.cFileName, hList, i, 0);
			++i;
		} while (FindNextFile(hFind, &FindFileData) != 0);

		(hFind); // Закрытие работу поиска фаилов
		InitializationListViewImage(hList, i, c_dir); 
	}
}

bool FindThisFileName(HWND hList, char c_dir[MAX_PATH]) //Поиск указанного файла (для переименования)
{
	WIN32_FIND_DATA FindFileData;
	HANDLE hFind; // Дескриптор к объекту HWND ручка к окну

	hFind = FindFirstFile(c_dir, &FindFileData);
	if (hFind == INVALID_HANDLE_VALUE)
	{
		(hFind); // Закрытие работу поиска фаилов
		return false;
	}
	else
	{
		(hFind); // Закрытие работу поиска фаилов
		return true;
	}

}

void Move() // Перемещение файла/папки
{
	char cm_dir_from[MAX_PATH];
	char cm_dir_to[MAX_PATH], cm_dir_to_[MAX_PATH];

	if (listView_1_selected) {
		strcpy(cm_dir_from, direction_left);
		cm_dir_from[strlen(cm_dir_from) - 1] = 0;
		strcat(cm_dir_from, copy_buf_for_name);
		strcpy(cm_dir_to, direction_right);
		strcpy(cm_dir_to_, direction_right);
		cm_dir_to[strlen(cm_dir_to) - 1] = 0;
		strcat(cm_dir_to, copy_buf_for_name);

		TCHAR from[MAX_PATH];
		ZeroMemory(from, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(from, cm_dir_from, sizeof(TCHAR) * strlen(cm_dir_from));

		TCHAR to[MAX_PATH];
		ZeroMemory(to, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(to, cm_dir_to, sizeof(TCHAR) * strlen(cm_dir_to));

		SHFILEOPSTRUCT file_move;
		ZeroMemory(&file_move, sizeof(SHFILEOPSTRUCT));
		file_move.hwnd = NULL;
		file_move.wFunc = FO_MOVE;
		file_move.pFrom = from;
		file_move.pTo = to;
		file_move.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS;
		int message = MessageBoxA(0, cm_dir_from, "Вы точно хотите переместить файл?", MB_ICONQUESTION | MB_OKCANCEL);
		if (message != IDCANCEL)
		{
			MessageBoxA(0, cm_dir_to, "Перемещение в:", MB_ICONEXCLAMATION);
			int result = SHFileOperation(&file_move);
			if (result != 0) MessageBoxA(0, "Файл не перемещён.", "", MB_ICONEXCLAMATION);
			else
			{
				FindFile(hListView_1, direction_left);
				FindFile(hListView_2, direction_right);
				MessageBoxA(0, "Файл перемещён.", "", MB_ICONEXCLAMATION);
			}
		}
	}
	else
	{
		strcpy(cm_dir_from, direction_right);
		cm_dir_from[strlen(cm_dir_from) - 1] = 0;
		strcat(cm_dir_from, copy_buf_for_name);
		strcpy(cm_dir_to, direction_left);
		strcpy(cm_dir_to_, direction_left);
		cm_dir_to[strlen(cm_dir_to) - 1] = 0;
		strcat(cm_dir_to, copy_buf_for_name);

		TCHAR from[MAX_PATH];
		ZeroMemory(from, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(from, cm_dir_from, sizeof(TCHAR) * strlen(cm_dir_from));

		TCHAR to[MAX_PATH];
		ZeroMemory(to, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(to, cm_dir_to, sizeof(TCHAR) * strlen(cm_dir_to));

		SHFILEOPSTRUCT file_move;
		ZeroMemory(&file_move, sizeof(SHFILEOPSTRUCT));
		file_move.hwnd = NULL;
		file_move.wFunc = FO_MOVE;
		file_move.pFrom = from;
		file_move.pTo = to;
		file_move.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS;
		int message = MessageBoxA(0, cm_dir_from, "Вы точно хотите переместить файл?", MB_ICONQUESTION | MB_OKCANCEL);
		if (message != IDCANCEL)
		{
			MessageBoxA(0, cm_dir_to, "Перемещение в:", MB_ICONEXCLAMATION);
			int result = SHFileOperation(&file_move);
			if (result != 0) MessageBoxA(0, "Файл не перемещён.", "", MB_ICONEXCLAMATION);
			else
			{
				FindFile(hListView_1, direction_left);
				FindFile(hListView_2, direction_right);
				MessageBoxA(0, "Файл перемещён.", "", MB_ICONEXCLAMATION);
			}
		}
	}
}

void notepad() // Открытие Notepad
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory(&si, sizeof(STARTUPINFO));
	ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));
	si.cb = sizeof(STARTUPINFO);
	si.dwFlags = STARTF_USEPOSITION;
	si.wShowWindow = SW_MAXIMIZE;
	CreateProcess(NULL, "NOTEPAD", NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi);
}

void CopyFile() // Копирование файла/папки
{
	char cm_dir_from[MAX_PATH];
	char cm_dir_to[MAX_PATH], cm_dir_to_[MAX_PATH];
	if (listView_1_selected)
	{
		strcpy(cm_dir_from, direction_left);
		cm_dir_from[strlen(cm_dir_from) - 1] = 0;
		strcat(cm_dir_from, copy_buf_for_name);
		strcpy(cm_dir_to, direction_right);
		strcpy(cm_dir_to_, direction_right);
		cm_dir_to[strlen(cm_dir_to) - 1] = 0;
		strcat(cm_dir_to, copy_buf_for_name);

		TCHAR from[MAX_PATH];
		ZeroMemory(from, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(from, cm_dir_from, sizeof(TCHAR) * strlen(cm_dir_from));

		TCHAR to[MAX_PATH];
		ZeroMemory(to, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(to, cm_dir_to, sizeof(TCHAR) * strlen(cm_dir_to));

		SHFILEOPSTRUCT file_copy;
		ZeroMemory(&file_copy, sizeof(SHFILEOPSTRUCT));
		file_copy.hwnd = NULL;
		file_copy.wFunc = FO_COPY;
		file_copy.pFrom = from;
		file_copy.pTo = to;
		file_copy.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS;
		int message = MessageBoxA(0, cm_dir_from, "Вы точно хотите скопировать файл?", MB_ICONQUESTION | MB_OKCANCEL);
		if (message != IDCANCEL)
		{
			MessageBoxA(0, cm_dir_to, "Копирование в:", MB_ICONEXCLAMATION);
			int result = SHFileOperation(&file_copy);
			if (result != 0) MessageBoxA(0, "Файл не скопирован.", "", MB_ICONEXCLAMATION);
			else
			{
				FindFile(hListView_2, cm_dir_to_);
				MessageBoxA(0, "Файл скопирован.", "", MB_ICONEXCLAMATION);
			}
		}
	}
	else
	{
		strcpy(cm_dir_from, direction_right);
		cm_dir_from[strlen(cm_dir_from) - 1] = 0;
		strcat(cm_dir_from, copy_buf_for_name);
		strcpy(cm_dir_to, direction_left);
		strcpy(cm_dir_to_, direction_left);
		cm_dir_to[strlen(cm_dir_to) - 1] = 0;
		strcat(cm_dir_to, copy_buf_for_name);

		TCHAR from[MAX_PATH];
		ZeroMemory(from, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(from, cm_dir_from, sizeof(TCHAR) * strlen(cm_dir_from));

		TCHAR to[MAX_PATH];
		ZeroMemory(to, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(to, cm_dir_to, sizeof(TCHAR) * strlen(cm_dir_to));

		SHFILEOPSTRUCT file_copy;
		ZeroMemory(&file_copy, sizeof(SHFILEOPSTRUCT));
		file_copy.hwnd = NULL;
		file_copy.wFunc = FO_COPY;
		file_copy.pFrom = from;
		file_copy.pTo = to;
		file_copy.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS;
		int message = MessageBoxA(0, cm_dir_from, "Вы точно хотите скопировать файл?", MB_ICONQUESTION | MB_OKCANCEL);
		if (message != IDCANCEL)
		{
			MessageBoxA(0, cm_dir_to, "Копирование в:", MB_ICONEXCLAMATION);
			int result = SHFileOperation(&file_copy);
			if (result != 0) MessageBoxA(0, "Файл не скопирован.", "", MB_ICONEXCLAMATION);
			else
			{
				FindFile(hListView_1, cm_dir_to_);
				MessageBoxA(0, "Файл скопирован.", "", MB_ICONEXCLAMATION);
			}
		}
	}
}

void DelFile() //Удаление файла/папки
{
	char cm_dir_from[MAX_PATH], cm_dir_from_[MAX_PATH];
	if (listView_1_selected)
	{
		strcpy(cm_dir_from, direction_left);
		strcpy(cm_dir_from_, direction_left);
		cm_dir_from[strlen(cm_dir_from) - 1] = 0;
		strcat(cm_dir_from, copy_buf_for_name);

		TCHAR _basket[MAX_PATH];
		ZeroMemory(_basket, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(_basket, cm_dir_from, sizeof(TCHAR) * strlen(cm_dir_from));
		SHFILEOPSTRUCT file_del;
		ZeroMemory(&file_del, sizeof(SHFILEOPSTRUCT));
		file_del.hwnd = NULL;
		file_del.wFunc = FO_DELETE;
		file_del.pFrom = _basket;
		file_del.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS;
		int message = MessageBoxA(0, cm_dir_from, "Вы точно хотите удалить файл?", MB_ICONQUESTION | MB_OKCANCEL);
		if (message != IDCANCEL)
		{
			int result = SHFileOperation(&file_del);
			if (result != 0) MessageBoxA(0, "Файл не удалён.", "", MB_ICONEXCLAMATION);
			else {
				FindFile(hListView_1, direction_left);
				FindFile(hListView_2, direction_right);
				MessageBoxA(0, "Файл удалён.", "", MB_ICONEXCLAMATION);
			}
		}
	}
	else {
		strcpy(cm_dir_from, direction_right);
		strcpy(cm_dir_from_, direction_right);
		cm_dir_from[strlen(cm_dir_from) - 1] = 0;
		strcat(cm_dir_from, copy_buf_for_name);

		TCHAR _basket[MAX_PATH];
		ZeroMemory(_basket, sizeof(TCHAR) * MAX_PATH);
		CopyMemory(_basket, cm_dir_from, sizeof(TCHAR) * strlen(cm_dir_from));
		SHFILEOPSTRUCT file_del;
		ZeroMemory(&file_del, sizeof(SHFILEOPSTRUCT));
		file_del.hwnd = NULL;
		file_del.wFunc = FO_DELETE;
		file_del.pFrom = _basket;
		file_del.fFlags = FOF_ALLOWUNDO | FOF_NOCONFIRMATION | FOF_SIMPLEPROGRESS;
		int message = MessageBoxA(0, cm_dir_from, "Вы точно хотите удалить файл?", MB_ICONQUESTION | MB_OKCANCEL);
		if (message != IDCANCEL)
		{
			int result = SHFileOperation(&file_del);
			if (result != 0) MessageBoxA(0, "Файл не удалён.", "", MB_ICONEXCLAMATION);
			else {
				FindFile(hListView_1, direction_left);
				FindFile(hListView_2, direction_right);
				MessageBoxA(0, "Файл удалён.", "", MB_ICONEXCLAMATION);
			}
		}
	}
}

int renameFile(char* name1, char* name2) // Переименования файла/папки
{
	CString prName = name1;
	CString newName = name2;
	prName.AppendChar(0);
	prName.AppendChar(0);
	newName.AppendChar(0);
	newName.AppendChar(0);
	SHFILEOPSTRUCT shStruct = { 0 };
	shStruct.wFunc = FO_RENAME;
	shStruct.pFrom = prName;
	shStruct.pTo = newName;
	shStruct.fFlags = FOF_NOCONFIRMATION | FOF_SILENT | FOF_ALLOWUNDO;
	shStruct.hNameMappings = 0;
	shStruct.lpszProgressTitle = NULL;
	return SHFileOperation(&shStruct);
}

void Rename() // Переименования файла/папки
{
	if (listView_1_selected)
	{
		strcpy(old_name_rename, direction_left);
		hTextEdit = CreateWindow("edit", "", WS_CHILD | WS_BORDER | ES_CENTER, x, yGlobal, width, height, hListView_1, 0, hInstance, NULL);
		ShowWindow(hTextEdit, SW_SHOWNORMAL);
		SetWindowSubclass(hListView_1, staticSubClass, 1, 0);
		hButton1 = CreateWindow("button", "ок", WS_CHILD | WS_BORDER, x + width, yGlobal, 25, height, hListView_1, (HMENU)ID_BUTTON_1, hInstance, NULL);
		ShowWindow(hButton1, SW_SHOWNORMAL);
	}
	else
	{
		strcpy(old_name_rename, direction_right);
		hTextEdit = CreateWindow("edit", "", WS_CHILD | WS_BORDER | ES_CENTER, x, yGlobal, width, height, hListView_2, 0, hInstance, NULL);
		ShowWindow(hTextEdit, SW_SHOWNORMAL);
		SetWindowSubclass(hListView_2, staticSubClass, 1, 0);
		hButton1 = CreateWindow("button", "ок", WS_CHILD | WS_BORDER, x + width, yGlobal, 25, height, hListView_2, (HMENU)ID_BUTTON_1, hInstance, NULL);
		ShowWindow(hButton1, SW_SHOWNORMAL);
	}

	old_name_rename[strlen(old_name_rename) - 1] = 0;
	strcpy(new_name_rename, old_name_rename);
	strcat(old_name_rename, copy_buf_for_name);
	SetWindowText(hTextEdit, copy_buf_for_name);
	listView_1_selected = false;
	listView_2_selected = false;
}

LRESULT CALLBACK staticSubClass(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam, UINT_PTR uIdSubclass, DWORD_PTR dwRefData) // Создание подкласса для hListView_1, hListView_2
{
	switch (uMsg) {
	case WM_COMMAND:
	{
		char val[MAX_PATH];
		switch (LOWORD(wParam)) 
		{
		case ID_BUTTON_1:
		{
			GetWindowText(hTextEdit, val, 256);
			if (strlen(val) == 0) {
				MessageBoxA(hWndRename, "Не введено имя", "", MB_ICONASTERISK);
				return 0;
			}
			strcat(new_name_rename, val);
			if (FindThisFileName(hListView_1, new_name_rename))
			{
				char* msg = "Конечная папка уже содержит папку с таким именем\n\nЕсли какие-либо из файлов имеют такое же имя, вам будет предложено\nзаменить эти файлы.\n\nПодтверждаете выполнение слияния папок?";
				int message = MessageBoxA(0, msg, "Подтверждение замены папки", MB_ICONWARNING | MB_OKCANCEL);
				if (message != IDCANCEL)
				{
					RemoveWindowSubclass(hWnd, staticSubClass, uIdSubclass);
					DestroyWindow(hTextEdit);
					DestroyWindow(hButton1);
					if (!renameFile(old_name_rename, new_name_rename)) {
						FindFile(hListView_1, direction_left);
						FindFile(hListView_2, direction_right);
						MessageBoxA(hWndRename, "Файл переименован", "", MB_ICONASTERISK);
						new_name_rename[0] = 0;
						DestroyWindow(hWndRename);
					}
					else {
						MessageBoxA(hWndRename, "Не удалось переименовать файл", "", MB_ICONEXCLAMATION);
						DestroyWindow(hWndRename);
					}
				}
			}
			else
			{
				RemoveWindowSubclass(hWnd, staticSubClass, uIdSubclass);
				DestroyWindow(hTextEdit);
				DestroyWindow(hButton1);
				if (!renameFile(old_name_rename, new_name_rename)) {
					FindFile(hListView_1, direction_left);
					FindFile(hListView_2, direction_right);
					MessageBoxA(hWndRename, "Файл переименован", "", MB_ICONASTERISK);
					new_name_rename[0] = 0;
					DestroyWindow(hWndRename);
				}
				else {
					MessageBoxA(hWndRename, "Не удалось переименовать файл", "", MB_ICONEXCLAMATION);
					DestroyWindow(hWndRename);
				}
			}
			RemoveWindowSubclass(hWnd, staticSubClass, uIdSubclass);
			DestroyWindow(hTextEdit);
			DestroyWindow(hButton1);
			return 0;
		}
		case WM_NCDESTROY:
		{
			RemoveWindowSubclass(hWnd, staticSubClass, uIdSubclass);
		}
		return 0;
		}
	}
	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	return DefSubclassProc(hWnd, uMsg, wParam, lParam);
}

LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) // Основная функция
{
	DWORD Drivers;
	int i, k;
	char c;
	char buf[] = " ";
	char* ls;
	string s;
	int y = 9;
	char buf1[MAX_PATH];

	LPNMHDR lpnmHdr;
	lpnmHdr = (LPNMHDR)lParam;
	LPNMLISTVIEW pnmLV = (LPNMLISTVIEW)lParam;

	GetLogicalDrives();
	GetLogicalDriveStrings(128, (LPTSTR)buf);

	switch (msg)
	{
	case WM_COMMAND:
	{
		switch (LOWORD(wParam))
		{
		case IDM_NEW:
		{
			notepad();
			return 0;
		}
		case IDM_RENAME:
		{
			if (listView_1_selected || listView_2_selected) { Rename(); }
			else MessageBoxA(0, "Не удалось переименовать файл\n\nНужно выбрать файл, который хотите переименовать", "Ошибка переименования", MB_ICONEXCLAMATION);
			return 0;
		}
		case IDM_MOVE:
		{
			if (listView_1_selected || listView_2_selected) { Move(); }
			else MessageBoxA(0, "Не удалось переместить файл\n\nНужно выбрать файл, который хотите переместить", "Ошибка переимещения", MB_ICONEXCLAMATION);
			return 0;
		}
		case IDM_COPY:
		{
			if (listView_1_selected || listView_2_selected) { CopyFile(); }
			else MessageBoxA(0, "Не удалось скопировать файл\n\nНужно выбрать файл, который хотите скопировать", "Ошибка копирования", MB_ICONEXCLAMATION);
			return 0;
		}
		case IDM_DELETE:
		{
			if (listView_1_selected || listView_2_selected) { DelFile(); }
			else MessageBoxA(0, "Не удалось удалить файл\n\nНужно выбрать файл, который хотите удалить", "Ошибка удаления", MB_ICONEXCLAMATION);
			return 0;
		}
		case ID_COMBOBOX_1:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELENDOK:
			{
				ComboboxProcessing(hComboBox_1, hLabel_1, hListView_1, direction_left);
				return 0;
			}
			default: return 0;
			}
		}
		case ID_COMBOBOX_2:
		{
			switch (HIWORD(wParam))
			{
			case CBN_SELENDOK:
			{
				ComboboxProcessing(hComboBox_2, hLabel_2, hListView_2, direction_right);
				return 0;
			}
			default: return 0;
			}
		}
		return 0;
		}
	}
	case WM_NOTIFY:
	{
		if (((lpnmHdr->idFrom == ID_LISTVIEW_1) || (lpnmHdr->idFrom == ID_LISTVIEW_2)) && (lpnmHdr->code == NM_CLICK)) // Оработка одного нажатия
		{
			ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, pnmLV->iSubItem, buf1, MAX_PATH);
			strcpy(copy_buf_for_name, buf1);

			RECT* windowXYZ;
			windowXYZ = new RECT;
			ListView_GetItemRect(lpnmHdr->hwndFrom, pnmLV->iItem, windowXYZ, LVIR_LABEL);
			x = windowXYZ->left;
			width = windowXYZ->right - windowXYZ->left;
			height = windowXYZ->bottom - windowXYZ->top;
			yGlobal = (windowXYZ->top);

			if (lpnmHdr->idFrom == ID_LISTVIEW_1) {
				listView_1_selected = true;
				listView_2_selected = false;
			}
			else {
				listView_2_selected = true;
				listView_1_selected = false;
			}
		}
		if (((lpnmHdr->idFrom == ID_LISTVIEW_1) || (lpnmHdr->idFrom == ID_LISTVIEW_2)) && (lpnmHdr->code == NM_DBLCLK)) // Обработка двойного нажатия
		{
			// Копирование строки в буфер из ячейки ListView (pnmLV->iItem - номер строки; pnmLV->iSubItem - номер столбца)
			ListView_GetItemText(lpnmHdr->hwndFrom, pnmLV->iItem, pnmLV->iSubItem, buf1, MAX_PATH);

			if (lpnmHdr->idFrom == ID_LISTVIEW_1)
			{
				char _dir[MAX_PATH];
				int k = 0;
				strcpy(_dir, direction_left);
				_dir[strlen(_dir) - 1] = 0;
				strcat(_dir, buf1);

				for (int i = 0; i < strlen(buf1); i++)
				{
					string s;
					s = buf1[i];
					if (s == ".") k = i;
				}

				if ((k != 0) && (k != 1)) ShellExecute(0, "open", _dir, NULL, NULL, SW_SHOWNORMAL);
				else
				{
					if (strcmp(buf1, "..") == 0)
					{
						int k;
						direction_left[strlen(direction_left) - 2] = 0;
						for (i = 0; i < strlen(direction_left); i++)
						{
							string s;
							s = direction_left[i];
							if (s == "\\") k = i;
						}
						direction_left[k + 1] = 0;
						strcat(direction_left, "*");
					}
					else if (strcmp(buf1, ".") == 0)
					{
						direction_left[3] = 0;
						strcat(direction_left, "*");
					}
					else
					{
						strcat(buf1, "\\*");
						direction_left[strlen(direction_left) - 1] = 0;
						strcat(direction_left, buf1);
					}
					SetWindowText(hLabel_1, direction_left);
					FindFile(hListView_1, direction_left);
				}
			}
			else if (lpnmHdr->idFrom == ID_LISTVIEW_2)
			{
				char _dir1[MAX_PATH];
				int k = 0;
				strcpy(_dir1, direction_right);
				_dir1[strlen(_dir1) - 1] = 0;
				strcat(_dir1, buf1);

				for (int i = 0; i < strlen(buf1); i++)
				{
					string s;
					s = buf1[i];
					if (s == ".") k = i;
				}

				if ((k != 0) && (k != 1)) ShellExecute(0, "open", _dir1, NULL, NULL, SW_SHOWNORMAL);
				else
				{
					if (strcmp(buf1, "..") == 0)
					{
						int k;
						direction_right[strlen(direction_right) - 2] = 0;
						for (i = 0; i < strlen(direction_right); i++)
						{
							string s;
							s = direction_right[i];
							if (s == "\\") k = i;
						}
						direction_right[k + 1] = 0;
						strcat(direction_right, "*");
					}
					else if (strcmp(buf1, ".") == 0)
					{
						direction_right[3] = 0;
						strcat(direction_right, "*");
					}
					else
					{
						strcat(buf1, "\\*");
						direction_right[strlen(direction_right) - 1] = 0;
						strcat(direction_right, buf1);
					}
					SetWindowText(hLabel_2, direction_right);
					FindFile(hListView_2, direction_right);
				}
			}
		}
		break;
	}
	case WM_CREATE:
	{
		hToolBar = CreateSimpleToolbar(hwnd);

		hLabel_1 = CreateWindow("static", "direction_1", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 3, 57 + y, 400, 16, hwnd, (HMENU)ID_LABEL_1, hInstance, NULL);
		hLabel_2 = CreateWindow("static", "direction_2", WS_CHILD | WS_VISIBLE | WS_TABSTOP, 453, 57 + y, 400, 16, hwnd, (HMENU)ID_LABEL_2, hInstance, NULL);

		hComboBox_1 = CreateWindow("ComboBox", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_SORT, 3, 33 + y, 100, 110, hwnd, (HMENU)ID_COMBOBOX_1, hInstance, NULL);
		hComboBox_2 = CreateWindow("ComboBox", NULL, WS_CHILD | WS_VISIBLE | WS_VSCROLL | CBS_DROPDOWN | CBS_SORT, 453, 33 + y, 100, 110, hwnd, (HMENU)ID_COMBOBOX_2, hInstance, NULL);

		
		hListView_1 = CreateWindow(WC_LISTVIEW, NULL, LVS_REPORT | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | LVS_AUTOARRANGE, 0, 60 + y + 15, 450, 500, hwnd, (HMENU)ID_LISTVIEW_1, hInstance, NULL);
		hListView_2 = CreateWindow(WC_LISTVIEW, NULL, LVS_REPORT | WS_CHILD | WS_VISIBLE | WS_CLIPSIBLINGS | LVS_AUTOARRANGE, 451, 60 + y + 15, 450, 500, hwnd, (HMENU)ID_LISTVIEW_2, hInstance, NULL);

		ls = buf;
		while (*ls)
		{
			SendMessage(hComboBox_1, CB_ADDSTRING, 0, (LPARAM)ls);
			SendMessage(hComboBox_2, CB_ADDSTRING, 0, (LPARAM)ls);
			ls += strlen(ls) + 1;
		}

		SendMessage(hComboBox_1, CB_SETCURSEL, 0, 0);
		SendMessage(hComboBox_2, CB_SETCURSEL, 0, 0);

		AddColumnToListView("Имя", 1, 100);
		FindFile(hListView_1, direction_left);
		SetWindowText(hLabel_1, direction_left);
		FindFile(hListView_2, direction_left);
		SetWindowText(hLabel_2, direction_right);
		return 0;
	}
	case WM_DESTROY:
	{
		DestroyWindow(hListView_1);
		DestroyWindow(hListView_2);
		DestroyWindow(hComboBox_1);
		DestroyWindow(hComboBox_2);
		DestroyWindow(hLabel_1);
		DestroyWindow(hLabel_2);
		DestroyWindow(hToolBar);
		PostQuitMessage(0);
		return 0;
	}
	}
	return DefWindowProc(hwnd, msg, wParam, lParam);
}
