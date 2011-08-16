@pushd "%~dp0"
@setlocal
::
@set "BASE=winshebang"
@set "TGT=sfjp"
::
copy "MAKEFILE.borland"		"%TGT%"
copy "%BASE%.cpp"		"%TGT%"
copy "%BASE%.rc"		"%TGT%"
copy "%BASE%.txt"		"%TGT%"
copy "%BASE%1.ico"		"%TGT%"
copy "%BASE%2.ico"		"%TGT%"
::
@endlocal
@popd
pause
