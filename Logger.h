#pragma once
#include <Windows.h>
#include <stdio.h>

void init_logger();
void log(LPCWSTR message);
void finish_logger();
void log_word(WORD message);
void log_byte(BYTE message);
void log_new_line();