
WINKEY_P = C:\Users\Public\winkey.exe

all : tinky winkey
	LINK tinky.obj /OUT:svc.exe
	LINK winkey.obj /OUT:$(WINKEY_P)
tinky:
	cl /c /WX /Wall /I headers src/tinky.cpp

winkey:
	cl /c /WX /Wall /I headers src/winkey.cpp
	
clean:
	@del tinky.obj
	@del winkey.obj

fclean: clean
	@del svc.exe
	@del $(WINKEY_P)

re: fclean all