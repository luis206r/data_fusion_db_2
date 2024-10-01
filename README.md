# Proyecto de Estructuras de Datos Avanzadas

## Integrantes: Luis Robledo

## Introducción

Este proyecto implementa y compara tres estructuras de datos avanzadas para el manejo eficiente de grandes volúmenes de información: ISAM con índice disperso, hashing extensible y archivo secuencial con espacio auxiliar y punteros. El objetivo es analizar el rendimiento de estas estructuras en operaciones de búsqueda, inserción y eliminación de datos.

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

### 3. Archivo Secuencial con Espacio Auxiliar y Punteros

Esta estructura combina la eficiencia del almacenamiento secuencial con la flexibilidad de las inserciones dinámicas:

- Los registros se almacenan inicialmente de forma secuencial.
- Se utiliza un espacio auxiliar para nuevas inserciones.
- Los punteros mantienen el orden lógico de los registros.

## Resultados Experimentales

Se realizaron pruebas de rendimiento para operaciones de búsqueda, inserción y eliminación en cada estructura. Los tiempos se midieron en milisegundos.

| Operación | Hashing Extensible | Archivo Secuencial |
|-----------|--------------------|--------------------|
| Búsqueda  | 0.5 ms             | 15 ms              |
| Inserción | 1.2 ms             | 10 ms              |
| Eliminación| 0.8 ms            |                    |

## Pruebas

Las pruebas se realizaron utilizando la interfaz de línea de comandos (CLI). A continuación, se muestra un ejemplo de cómo se ejecutaron y visualizaron las pruebas:

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
   $ make
   ```

2. Ejecute el programa principal:
   ```
   $ ./main
   ```

3. Para ejecutar las pruebas:
   ```
   $ ./run_tests
   ```

## Conclusiones

Este proyecto demuestra las ventajas y desventajas de diferentes estructuras de datos para el manejo de grandes volúmenes de información. El hashing extensible mostró el mejor rendimiento en operaciones de búsqueda y inserción, mientras que el archivo secuencial con espacio auxiliar ofreció un buen equilibrio entre eficiencia y simplicidad de implementación.

La implementación del ISAM con índice disperso, aunque no se probó completamente, promete ser una solución eficiente para aplicaciones que requieren tanto acceso secuencial como aleatorio a los datos.

Para futuros trabajos, se recomienda completar la implementación y pruebas del ISAM, así como explorar otras estructuras de datos avanzadas para comparar su rendimiento en escenarios similares.


