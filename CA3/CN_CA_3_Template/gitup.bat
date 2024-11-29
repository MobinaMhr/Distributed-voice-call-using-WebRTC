@echo off
REM Usage: upgit.bat "message 1" "message 2" "message 3"
setlocal EnableDelayedExpansion

set "commitMessage="
for %%i in (%*) do (
    set commitMessage=!commitMessage! -m "%%~i"
)

git add -A
git status
git commit !commitMessage!
git push
