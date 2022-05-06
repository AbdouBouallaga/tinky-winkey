
all : tinky winkey
	LINK tinky.obj /OUT:svc.exe
	LINK winkey.obj /OUT:winkey.exe
tinky:
	cl /c /WX /Wall tinky.cpp

winkey:
	cl /c winkey.cpp
	
clean:
	@del tinky.obj
	@del winkey.obj

fclean: clean
	@del svc.exe
	@del winkey.exe

re: fclean all