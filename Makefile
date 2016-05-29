all: cjson

cjson: cjson.o jvoid.o jpath.o tjson.o aheap.o jparser.o jparseexpr.o
	gcc bin/cjson.o bin/jvoid.o  bin/tjson.o bin/jpath.o bin/aheap.o bin/jparseexpr.o bin/jparser.o -o cjson

cjson.o: cjson.c
	gcc -c cjson.c -o bin/cjson.o

jvoid.o: jVoid.c
	gcc -c jVoid.c -o bin/jvoid.o

jpath.o: jPATH.c
	gcc -c jPATH.c -o bin/jpath.o

tjson.o: tJSON.c
	gcc -c tJSON.c -o bin/tjson.o

aheap.o: aHEAP.c
	gcc -c aHEAP.c -o bin/aheap.o

jparseexpr.o: jParseExpr.c
	gcc -c jParseExpr.c -o bin/jparseexpr.o

jparser.o: jParser.c
	gcc -c jParser.c -o bin/jparser.o

clean:
	rm bin/*o cjson
