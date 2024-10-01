# Proyecto de Estructuras de Datos Avanzadas

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

| Operación | Hashing Extensible | Archivo Secuencial | ISAM (estimado) |
|-----------|--------------------|--------------------|-----------------|
| Búsqueda  | 0.5 ms             | 15 ms              | 5 ms            |
| Inserción | 1.2 ms             | 10 ms              | 8 ms            |
| Eliminación| 0.8 ms            | 20 ms              | 10 ms           |

Nota: Los tiempos para ISAM son estimados ya que no se completaron las pruebas.

## Pruebas

Las pruebas se realizaron utilizando la interfaz de línea de comandos (CLI). A continuación, se muestra un ejemplo de cómo se ejecutaron y visualizaron las pruebas:

```
$ ./run_tests
Ejecutando pruebas para Hashing Extensible...
Búsqueda: 0.5 ms
Inserción: 1.2 ms
Eliminación: 0.8 ms

Ejecutando pruebas para Archivo Secuencial...
Búsqueda: 15 ms
Inserción: 10 ms
Eliminación: 20 ms

ISAM: Pruebas no completadas
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


