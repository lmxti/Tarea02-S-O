OBJS = tarea02.cpp
EXC = tarea02

#Esta regla compila el programa principal
compile: ${OBJS}
	g++ ${OBJS} -o ${EXC}

#Esta regla elimina el ejecutable anterior y vuelve a compilar el programa principal
recompile: ${OBJS} ${EXC}
	rm ${EXC}
	g++ ${OBJS} -o ${EXC}

#Esta regla elimina el ejecutable
clean: ${EXC}
	rm ${EXC}