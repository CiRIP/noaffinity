// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <iostream>
#include <windows.h>
#include <detours.h>

typedef BOOL(WINAPI *AFFINITY_FP)(HWND hWnd, DWORD dwAffinity);

AFFINITY_FP OriginalSetWindowDisplayAffinity = (AFFINITY_FP)::GetProcAddress(
    ::GetModuleHandle(L"user32.dll"), "SetWindowDisplayAffinity");

BOOL WINAPI HookedSetWindowDisplayAffinity(HWND hWnd, DWORD dwAffinity) {
    return TRUE;
}

BOOL WINAPI DllMain(__in HINSTANCE hInstance, __in DWORD Reason,
    __in LPVOID Reserved) {

    if (DetourIsHelperProcess()) {
        return TRUE;
    }

    switch (Reason) {
    case DLL_PROCESS_ATTACH:
        DetourRestoreAfterWith();

        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourAttach(&(PVOID&)OriginalSetWindowDisplayAffinity, HookedSetWindowDisplayAffinity);
        DetourTransactionCommit();
        break;

    case DLL_PROCESS_DETACH:
        DetourTransactionBegin();
        DetourUpdateThread(GetCurrentThread());
        DetourDetach(&(PVOID&)OriginalSetWindowDisplayAffinity, HookedSetWindowDisplayAffinity);
        DetourTransactionCommit();
    }

    return TRUE;
}