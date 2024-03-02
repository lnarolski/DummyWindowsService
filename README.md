# DummyWindowsService

Add dummy windows service that is creating new child .NET process with svchost.exe as a parent. 

# Instruction

1. Compile DummyWindowsService project to dll file (remeber compile it to correct architecture (e.g. x86 or x64) based on your Windows OS!!!) and compile DummyWindowsExe to exe file
2. Copy DummyWindowsService.dll and DummyWindowsExe.exe to ```C:\Windows\System32\``` directory
3. In Powershell create service:
```
sc.exe create DummyWindowsService binPath= "c:\windows\System32\svchost.exe -k DcomLaunch" type= share start= delayed-auto
```
4. In Powershell add new registry key:
```
reg add HKLM\SYSTEM\CurrentControlSet\services\WindowsDummyService\Parameters /v ServiceDll /t REG_EXPAND_SZ /d C:\Windows\system32\WindowsDummyService.dll /f
```
5. Add new line ```DummyWindowsService``` in ```DcomLaunch``` registry key in ```HKLM\SOFTWARE\Microsoft\Windows NT\CurrentVersion\Svchost```
6. In Powershell start DummyWindowsService:
```
sc.exe start EvilSvc
```


Based on "Persisting in svchost.exe with a Service DLL" article: https://www.ired.team/offensive-security/persistence/persisting-in-svchost.exe-with-a-service-dll-servicemain#id-1.-compile-service-dll