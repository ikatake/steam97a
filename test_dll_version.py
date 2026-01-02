import ctypes
import os

# DLLのパス
dll_path = r"C:\Users\ikata\steam97\test2026\sm97s2_2\Steam97.dll"

print(f"Testing DLL: {dll_path}")
print(f"DLL exists: {os.path.exists(dll_path)}")

# DLLをロード
try:
    steam_dll = ctypes.WinDLL(dll_path)
    
    # STM_GetDllVer関数を設定
    STM_GetDllVer = steam_dll.STM_GetDllVer
    STM_GetDllVer.argtypes = [ctypes.c_int]
    STM_GetDllVer.restype = ctypes.c_int
    
    # バージョン情報を取得
    print("\nVersion Information:")
    for i in range(1, 5):
        version = STM_GetDllVer(i)
        print(f"STM_GetDllVer({i}) = {version}")
    
except Exception as e:
    print(f"Error: {e}")
    import traceback
    traceback.print_exc()
