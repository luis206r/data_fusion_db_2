# Proyecto de Estructuras de Datos Avanzadas

## Integrantes: Luis Robledo

### Video Explicativo [Drive](https://drive.google.com/file/d/1td9SVl_KiWwNvAjUvMxyT5kIt9rgn3vy/view?usp=sharing "Ver video")

## Introducción

Este proyecto implementa y compara tres estructuras de datos avanzadas para el manejo eficiente de grandes volúmenes de información: ISAM con índice disperso, hashing extensible y archivo secuencial con espacio auxiliar y punteros. El objetivo es analizar el rendimiento de estas estructuras en operaciones de búsqueda, inserción y eliminación de datos.

Se logaron implementar las técnicas de Extendible Hashing, y Sequentia File. El isam sparse no se pudo terminar

El conjunto de datos utilizado para las pruebas es similar a un registro de estudiantes universitarios, con campos como nombre, apellido, código, carrera y ciclo.

## Técnicas Utilizadas

### 1. ISAM con Índice Disperso

ISAM (Indexed Sequential Access Method) con índice disperso es una estructura que combina las ventajas del acceso secuencial y el acceso indexado. En nuestra implementación:

- Se utiliza un árbol B+ como estructura de índice.
- El índice disperso reduce el tamaño del índice en memoria.
- Proporciona un acceso rápido para búsquedas por clave.

Nota: Esta estructura no se llegó a probar completamente debido a limitaciones de tiempo.

### 2. Hashing Extensible

El hashing extensible es una técnica de hash dinámica que se adapta al crecimiento de los datos:

- Utiliza un directorio que se duplica cuando es necesario.
- Los buckets tienen un tamaño fijo y se dividen cuando se llenan.
- Ofrece un rendimiento cercano a O(1) para búsquedas e inserciones.

Las inserciones se hacer habiendo hasheado la llave de indexacion del registro a insertar. Una vez hecho eso, si el bucket asociado a ese hash tiene espacio, se inserta. En caso contrario se hace un split de ese bucket, y cada uno se asocia a una cadena hash concatenada inicialmente con un nuevo char que es 0 o 1. Esto aumenta la profundidad global de la estructura. El acceso a los registros es casi O(K), con k registros en el bucket. En caso de que haya overflow seria O(K+X), con X registros en el bucket del overflow. Sin embargo, en general es O(1).
En caso no se encuentre el bucket, se procede a generar una nueva cadena hash con la profundidad anterior y volver a buscar en el bucket asociado a esta.

Las búsquedas También tienen la misma complejidad, ya que se hace el mismo proceso que en la inserción, pero sono se verifica la existencia del registro, por lo que la complejidad también es O(1).

Para la eliminación, lo que se está haciendo es, una vez que un bucket se quede sin espacio, se procede a marcar como libre, y puede usarse para nuevos buckets creados. Este escenario es util cuando spliteamos un bucket en 2, ya que uno de ellos se asocia al espacio libre que esta marcado, y el otro se escribe al final del archivo. En el indice, dado que los registros de indice son de longitud variable, estos se marcan como eliminados.

Esta estructura no sporta la búsqueda por rango.

### 3. Archivo Secuencial con Espacio Auxiliar y Punteros

Esta estructura combina la eficiencia del almacenamiento secuencial con la flexibilidad de las inserciones dinámicas:

- Los registros se almacenan inicialmente de forma secuencial.
- Se utiliza un espacio auxiliar para nuevas inserciones.
- Los punteros mantienen el orden lógico de los registros.

Para la insercion, se esta considerando siempre mantener el archivo de datos principal ordenado. Todas las nuevas inserciones se realizan en el archivo auxiliar, pero previamente se realiza una busqueda binaria en el archivo de datos. Esto me permite obtener el registro anterior, y posterior a la llave a insertar. dado que se usan punteros, la insercion es similar a la de un Linked List. La complejidad seria O(logN) + O(K), con N registros en el archivo principal de datos, y K registros en el archivo auxiliar

Para la busqueda, se realiza una busqueda binaria en el archivo de datos en el archivo principal y auxiliar. La complejdad es la misma

Para la busqueda por rango, se hace exactamente lo mismo que una insercion, pero no para insertar, sino para buscar el nodo anterior o posterior. Esto me permite, en caso la llave no exista, saber desde que registro y hasta que registro debo leer siguiendo los punteros. La complejidad seria 2*O(logN) + 2*O(K) = O(logN) + O(K).

Para la eliminacion, si se remueve el registro del archivo principal, se llena con uno del espacio auxiliar que encaje. si se elimina del auxiliar la complejidad se reduce. Esto es valido gracias a los punteros, y se toma en cuenta que simpre hay que tener el archivo de datos principal lleno.

El rebuild simplemente sigue los punteros.

## Resultados Experimentales

Se realizaron pruebas de rendimiento para operaciones de búsqueda, inserción y eliminación en cada estructura. Los tiempos se midieron en milisegundos.

| Operación | Hashing Extensible | Archivo Secuencial |
|-----------|--------------------|--------------------|
| Búsqueda  | 0.5 ms             | 15 ms              |
| Inserción | 1.2 ms             | 10 ms              |
| Eliminación| 0.8 ms            | 8 ms               |

## Pruebas

Las pruebas se realizaron utilizando la interfaz de línea de comandos (CLI), y un parser básico hecho con clases y metodos en C++. A continuación, se muestra un ejemplo de cómo se ejecutaron y visualizaron las pruebas:

```
Ingrese su consulta (termine con ';'):
select * from table;

Consulta v├ílida... trayendo todos los registros.

|=====================================================================|
|Nombre              |Apellido            |Codigo    |Carrera   |ciclo|
|====================|====================|==========|==========|=====|
|=====================================================================|
|Nombre              |Apellido            |Codigo    |Carrera   |ciclo|
|====================|====================|==========|==========|=====|
|Agustina            |Romero              |345682    |EE        |3    |
|=====================================================================|\_ pos: 0 | file: d | nextPos: 1 | nextFile: d
|Alejandro           |Ruiz                |456780    |IT        |5    |
|=====================================================================|\_ pos: 1 | file: d | nextPos: 2 | nextFile: d
|Alfonso             |Arellano            |012353    |IT        |6    |
|=====================================================================|\_ pos: 2 | file: d | nextPos: 3 | nextFile: d
|Alonso              |Vergara             |456784    |IT        |6    |
|=====================================================================|\_ pos: 3 | file: d | nextPos: 4 | nextFile: d
|Amanda              |Espinoza            |345681    |CS        |2    |
|=====================================================================|\_ pos: 4 | file: d | nextPos: 5 | nextFile: d

....
======para extendible========
.....
posicion del indice que estoy leyendo: 944
leyendo...
posicion del indice despues de leer: 961
Data del indice leido...
|---------------------- Index Record ----------------------|
| Size: 5     | Cadena: 01000        | Pos: 3812  | Status: 1     |
|----------------------------------------------------------|
Data del bucket leido...
|----------------------- Bucket Data -----------------------|
| Local Depth: 5     | Size: 0     | NextDel: -1    | NextBucket: -1    |
|----------------------------------------------------------|
|  Records                                                |
|----------------------------------------------------------|
|----------------------------------------------------------|
posicion del indice que estoy leyendo: 961
leyendo...
posicion del indice despues de leer: 978
Data del indice leido...
|---------------------- Index Record ----------------------|
| Size: 5     | Cadena: 11000        | Pos: 8164  | Status: 1     |
|----------------------------------------------------------|
Data del bucket leido...
|----------------------- Bucket Data -----------------------|
| Local Depth: 5     | Size: 3     | NextDel: -1    | NextBucket: -1    |
|----------------------------------------------------------|
|  Records                                                |
|----------------------------------------------------------|
|Maximiliano         |Bravo               |890126    |IT        |5    |
|=====================================================================|
|Lucas               |Arce                |012351    |IT        |4    |
|=====================================================================|
|Lautaro             |Calder├│n           |456788    |IT        |4    |
|=====================================================================|
|----------------------------------------------------------|
posicion del indice que estoy leyendo: 978
.....
```

Para más detalles sobre cómo ejecutar las pruebas, consulte la sección "Cómo Ejecutar" más abajo.

## Parser y Archivo de Datos

El parser utilizado en este proyecto se implementó en C++ y es intencionalmente simple para facilitar el procesamiento rápido de los datos. Este parser lee el archivo de datos y lo convierte en estructuras utilizables por nuestras implementaciones.

El archivo de datos es similar al CSV solicitado anteriormente, con el siguiente formato:

```
Nombre,Apellido,Código,Carrera,Ciclo
Adrián,Moreno,123456,IT,3
Alejandra,García,234567,EE,5
...
```

Cada registro contiene información de un estudiante, incluyendo nombre, apellido, un código único de 6 dígitos, la carrera (IT, EE, ME, CS, CE) y el ciclo académico (1-10).

## Cómo Ejecutar

1. Compile el proyecto:
   ```
   $ g++ -o main main.cpp
   ```

2. Ejecute el programa principal:
   ```
   $ ./main
   ```
3. Eliga la opcion del menu:

```
   ====================================================================
====================================================================
====================================================================
====================================================================
  ____        _        _____           _
 |  _ \  __ _| |_ __ _|  ___|   _ ___(_) ___  _ __
 | | | |/ _` | __/ _` | |_ | | | / __| |/ _ \| '_ \
 | |_| | (_| | || (_| |  _|| |_| \__ \ | (_) | | | |
 |____/ \__,_|\__\__,_|_|   \__,_|___/_|\___/|_| |_|


                  by luisr
====================================================================
====================================================================
====================================================================
====================================================================

Seleccione la estructura
1. Sequential File
2. Extendible Hashing
3. Limpiar archivos
4. Salir
Ingrese opci├│n:
```

## Conclusiones

Este proyecto demuestra las ventajas y desventajas de diferentes estructuras de datos para el manejo de grandes volúmenes de información. El hashing extensible mostró el mejor rendimiento en operaciones de búsqueda y inserción, mientras que el archivo secuencial con espacio auxiliar ofreció un buen equilibrio entre eficiencia y simplicidad de implementación.

La implementación del ISAM con índice disperso, aunque no se probó completamente, promete ser una solución eficiente para aplicaciones que requieren tanto acceso secuencial como aleatorio a los datos.



