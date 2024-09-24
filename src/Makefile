ch36x_api_rw_helper.exe: ch36x_api_rw_helper.o cmdparser.o CH367DLL.H CH367DLL.lib
	g++ -o ch36x_api_rw_helper.exe ch36x_api_rw_helper.o cmdparser.o CH367DLL.lib CH367DLL64.lib  -Wl,--subsystem,console

ch36x_api_rw_helper.o: ch36x_api_rw_helper.cpp ch36x_api_rw_helper.h
	g++ -c ch36x_api_rw_helper.cpp

cmdparser.o: cmdparser.c cmdparser.h
	gcc -c cmdparser.c

clean: FORCE
	@rm -rf *.o

FORCE:
