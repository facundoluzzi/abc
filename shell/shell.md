# Lab: shell

### **Búsqueda en $PATH**
**¿cuáles son las diferencias entre la syscall execve(2) y la familia de wrappers proporcionados por la librería estándar de C (libc) exec(3)?**


La principal diferencia es que execve(2) es una syscall, y exec(3) una familia de funciones, que utilizan la syscall execve(2).

**¿Puede la llamada a exec(3) fallar? ¿Cómo se comporta la implementación de la shell en ese caso?**


Puede fallar. Los wrappers proporcionados por exec(3) llaman a la syscall execve(2). Acá es donde se puede producir la falla, retornando -1.

### **Comandos built-in**
**¿Entre cd y pwd, alguno de los dos se podría implementar sin necesidad de ser built-in? ¿Por qué? ¿Si la respuesta es sí, cuál es el motivo, entonces, de hacerlo como built-in? (para esta última pregunta pensar en los built-in como true y false)**


pwd se puede implementar sin necesidad de ser un built-in, resultando de esta forma más eficiente, ya que lo único que hace es printear el directorio de trabajo actual.

### **Variables de entorno adicionales**
**¿Por qué es necesario hacerlo luego de la llamada a fork(2)?**


Esto es porque justamente es una variable temporaria, no necesitamos que exista más allá de la ejecución actual del programa. Entonces, realizando un fork previamente, solo agregamos la variable temporal a un nuevo proceso que dejará de existir al finalizar la ejecución.

**En algunos de los wrappers de la familia de funciones de exec(3) (las que finalizan con la letra e), se les puede pasar un tercer argumento (o una lista de argumentos dependiendo del caso), con nuevas variables de entorno para la ejecución de ese proceso. Supongamos, entonces, que en vez de utilizar setenv(3) por cada una de las variables, se guardan en un array y se lo coloca en el tercer argumento de una de las funciones de exec(3).**
**¿El comportamiento resultante es el mismo que en el primer caso? Explicar qué sucede y por qué.**


No es el mismo. Esto se debe a que las variables de entorno definidas en environ no existen, solo están las variables de entorno recibidas por argumento (temporales)


Describir brevemente (sin implementar) una posible implementación para que el comportamiento sea el mismo.
Una posible implementación podría ser la siguiente: el vector que recibe como argumento el proceso contiene las variables de entorno que se encuentran en environ, y las variables de entorno temporales.

### **Procesos en segundo plano**
**Detallar cuál es el mecanismo utilizado para implementar procesos en segundo plano.**


Realizando el waitpid con el argumento WNOHANG, podemos ejecutar un proceso y retornar inmediatamente, aunque este no haya finalizado su ejecución. Esto espera a los procesos en segundo plano, de forma tal que no quedan huérfanos, y permite la ejecución de otros procesos en paralelo.

### **Flujo estándar**
**Investigar el significado de 2>&1, explicar cómo funciona su forma general y mostrar qué sucede con la salida de cat out.txt en el ejemplo. Luego repetirlo invertiendo el orden de las redirecciones. ¿Cambió algo?**


2>&1 redirecciona la salida de error hacia la salida estándar. En el ejemplo se redirecciona la salida estándar hacia out.txt. Luego, ls arroja un error, y como nosotros lo redireccionamos hacia la salida estándar, este error se almacena en out.txt. 
En resumen: la salida estándar, y la salida de error se redirigieron a out.txt


        (/home/facundo) 
        $ ls -C /home /noexiste >out.txt 2>&1
                Program: [ls -C /home /noexiste >out.txt 2>&1] exited, status: 2 
        (/home/facundo) 
        $ cat out.txt
        ls: cannot access '/noexiste': No such file or directory
        /home:
        facundo


Si invertimos el orden, se redirige la salida de error hacía la salida estándar, luego, se redirige la salida estándar a out.txt. Entonces, la salida del comando ls (que larga error) se almacena en out.txt.


        (/home/facundo) 
        $ ls -C /home /noexiste 2>&1 >out.txt
                Program: [ls -C /home /noexiste 2>&1 >out.txt] exited, status: 2 
        (/home/facundo) 
        $ cat out.txt
        ls: cannot access '/noexiste': No such file or directory
        /home:
        facundo


El resultado no cambia.



### **Tuberías simples (pipes)**
**Investigar qué ocurre con el exit code reportado por la shell si se ejecuta un pipe ¿Cambia en algo? ¿Qué ocurre si, en un pipe, alguno de los comandos falla? Mostrar evidencia (e.g. salidas de terminal) de este comportamiento usando bash. Comparar con la implementación del este lab.**


El exit code que retorna es el del último comando ejecutado. En caso de que alguno de los comandos falle, retorna el exit code del último comando que fallo. 


Por ejemplo:...


            facundo@facundo-desktop:~$ ls | grep abc
            facundo@facundo-desktop:~$ echo $?
            1 (ls termina con 0, y grep en caso de no encontrar nada termina con 1)


Otro ejemplo:


            facundo@facundo-desktop:~$ ls | grep abc | echo prueba
            prueba
            facundo@facundo-desktop:~$ echo $?
            0 (echo abc termina con 0)


### **Pseudo-variables**
**Investigar al menos otras tres variables mágicas estándar, y describir su propósito. Incluir un ejemplo de su uso en bash (u otra terminal similar).**


**$!:** retorna el valor de la variable de errno(3)


    $ open $FILE, "fichero" 
    Error en la ejecucion del comando (EXECVP)
            Program: [open $FILE, "fichero"] exited, status: 255 
    /home/facundo 
    $ print "$!
    Error: no such file ""$!""


No existe el fichero o el directorio


**$_:** argumento por defecto de muchas funciones, operadores y estructuras de control


   print; es igual que print $_;


    /home/facundo/Escritorio 
    $ print prueba
    Error: no "print" mailcap rules found for type "application/x-sharedlib"
            Program: [print prueba] exited, status: 3 
    /home/facundo/Escritorio 



    $ print prueba $_
    Error: no "print" mailcap rules found for type "application/x-sharedlib"
            Program: [print prueba $_] exited, status: 3 
    /home/facundo/Escritorio 


    $ print abc $_ // El archivo abc no existe en el directorio actual, prueba en cambio, si
    Error: no such file "abc"


**$$:** devuelve el PID del script actual


    bash -c 'echo $$; exec prueba'
	19550

---

### Desafio Shell:

#### ¿Cuál es la función de los parámetros MIN y TIME del modo no canónico? ¿Qué se logra en el ejemplo dado al establecer a MIN en 1 y a TIME en 0?

El parametro MIN indica el numero minimo de bytes para que la funcion read retorne. Por otro lado, el parametro TIME indica el tiempo que espera un input la funcion read antes de retornar. 

En el ejemplo como TIME es 0, y read tiene un valor distinto de 0, lo que ocurre es que la funcion read espera a que haya al menos MIN (en este caso 1) bytes disponibles en la queue. Cuando hay MIN bytes disponibles, read devuelve tantos caracteres como esten disponibles, hasta el numero solicitado (1). Entonces, read va a retornar cuando logre encontrar 1 byte en la queue.

---
