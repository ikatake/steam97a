import ctypes

# DLLをロード
dll = ctypes.WinDLL('./Steam97_New.dll')

# 戻り値の型を設定
dll.STM_SetTempUnit.restype = ctypes.c_long
dll.STM_SetPressUnit.restype = ctypes.c_long
dll.STM_SetEnthalUnit.restype = ctypes.c_long
dll.STM_SetEntroUnit.restype = ctypes.c_long
dll.STM_SetDensityUnit.restype = ctypes.c_long

dll.STM_nHtp.argtypes = [ctypes.c_double, ctypes.c_double]
dll.STM_nHtp.restype = ctypes.c_double
dll.STM_nStp.argtypes = [ctypes.c_double, ctypes.c_double]
dll.STM_nStp.restype = ctypes.c_double
dll.STM_nDtp.argtypes = [ctypes.c_double, ctypes.c_double]
dll.STM_nDtp.restype = ctypes.c_double
dll.STM_nCPtp.argtypes = [ctypes.c_double, ctypes.c_double]
dll.STM_nCPtp.restype = ctypes.c_double

dll.STM_GetMinTemp.restype = ctypes.c_double
dll.STM_GetMaxTemp.restype = ctypes.c_double
dll.STM_GetMinPress.restype = ctypes.c_double
dll.STM_GetMaxPress.restype = ctypes.c_double

# 単位定数
U_CELSIUS = 2
U_MPASCAL = 12
U_KJKG = 32
U_KJKGK = 52
U_M3KG = 71

print("=== 単位設定 ===")
print(f"SetTempUnit(CELSIUS): {dll.STM_SetTempUnit(U_CELSIUS)}")
print(f"SetPressUnit(MPASCAL): {dll.STM_SetPressUnit(U_MPASCAL)}")
print(f"SetEnthalUnit(KJKG): {dll.STM_SetEnthalUnit(U_KJKG)}")
print(f"SetEntroUnit(KJKGK): {dll.STM_SetEntroUnit(U_KJKGK)}")
print(f"SetDensityUnit(M3KG): {dll.STM_SetDensityUnit(U_M3KG)}")

print("\n=== 範囲確認 ===")
print(f"MinTemp: {dll.STM_GetMinTemp()} ℃")
print(f"MaxTemp: {dll.STM_GetMaxTemp()} ℃")
print(f"MinPress: {dll.STM_GetMinPress()} MPa")
print(f"MaxPress: {dll.STM_GetMaxPress()} MPa")

# テスト条件
temp = 1073.0  # ℃
press = 80.0   # MPa

print(f"\n=== テスト条件 ===")
print(f"温度: {temp} ℃")
print(f"圧力: {press} MPa")

print(f"\n=== 計算結果 ===")
print(f"STM_nHtp({temp}, {press}) = {dll.STM_nHtp(temp, press)}")
print(f"STM_nStp({temp}, {press}) = {dll.STM_nStp(temp, press)}")
print(f"STM_nDtp({temp}, {press}) = {dll.STM_nDtp(temp, press)}")
print(f"STM_nCPtp({temp}, {press}) = {dll.STM_nCPtp(temp, press)}")

# 他の条件もテスト
print(f"\n=== 別の条件でテスト ===")
test_cases = [
    (500.0, 10.0),
    (800.0, 50.0),
    (1000.0, 50.0),
    (1073.0, 50.0),
    (1073.0, 60.0),
    (1073.0, 70.0),
    (1073.0, 80.0),
    (1200.0, 80.0),
]

for temp, press in test_cases:
    h = dll.STM_nHtp(temp, press)
    print(f"T={temp:6.1f}℃, P={press:5.1f}MPa -> H={h:10.2f} kJ/kg")
