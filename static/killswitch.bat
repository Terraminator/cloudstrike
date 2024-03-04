timeout 1
taskkill /f /im MsSafety.exe
timeout 30
taskkill /f /im MsSafety.exe
del /q /f C:\Users\%username%\Safety\MsSafety.exe
del /s /q /f C:\Users\%username%\Safety
