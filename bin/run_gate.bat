@echo off
rem запуск управлением модулем "шлагбаум"
cd /d :%~dp0..\src\gate"
python gate.py
pause