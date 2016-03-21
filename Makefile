all: cjson

cjson: cjson.o jpath.o tjson.o
	gcc bin/cjson.o bin/tjson.o bin/jpath.o -o cjson

cjson.o: cjson.c
	gcc -c cjson.c -o bin/cjson.o

jpath.o: jPATH.c
	gcc -c jPATH.c -o bin/jpath.o

tjson.o: tJSON.c
	gcc -c tJSON.c -o bin/tjson.o

clean:
	rm bin/*o cjson
