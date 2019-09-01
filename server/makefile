all:server
CC  = g++
INC = -I/usr/include/mysql
LIB = -L/usr/lib64/mysql -lmysqlclient  
server : server.cpp connect_database.cpp md5.c
	$(CC) $^ -o $@   $(INC) $(LIB)

clean:
	rm -rf server *.txt