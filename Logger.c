#include "Logger.h"

FILE* h_file;

void init_logger()
{
	_wfopen_s(&h_file, L"log.txt", L"w+");
}

void log(LPCWSTR message)
{
	fwprintf_s(h_file, L"%ws", message);
	fflush(h_file);
}

void log_int(int message)
{
	fwprintf_s(h_file, L"%04X ", message);
	fflush(h_file);
}
void log_word(WORD message)
{
	fwprintf_s(h_file, L"%04X  ", message);
	fflush(h_file);
}
void log_byte(BYTE message)
{
	fwprintf_s(h_file, L"%02X ", message);
	fflush(h_file);
}

void log_new_line()
{
	fwprintf_s(h_file, L"\n");
	fflush(h_file);
}


void finish_logger()
{
	fclose(h_file);
}