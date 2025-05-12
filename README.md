
Para ejecutar la tarea2 primero debemos compilar (en la carpeta raíz)
````
gcc tdas/*.c tarea2.c -Wno-unused-result -o tarea2
gcc tdas/*.c tarea2copy.c -Wno-unused-result -o tarea2copy
````

Y luego ejecutar:
````
./tarea2

````


# Spotifind

Este programa desarrolado en lenguaje c nos permite cargar una base de datos de canciones y, a partir de diferentes criterios, buscarlas rápida y eficientemente. Tambien, nos permitirá organizar tus canciones favoritas en listas de reproducción personalizadas.

## Cómo copilar y ejecutar

Este sistema ha sido desarrollado en lenguaje C y puede ejecutarse fácilmente utilizando Visual Studio Code junto con una extensión para C/C++, como C/C++ Extension Pack de Microsoft. Para comenzar a trabajar con el sistema en tu equipo local, sigue estos pasos:

### Requisitos previos:

- Tener instalado [Visual Studio Code](https://code.visualstudio.com/).
- Instalar la extensión *C/C++* (Microsoft).
- Tener instalado un compilador de C (como *gcc*). Si estás en Windows, se recomienda instalar [MinGW](https://www.mingw-w64.org/) o utilizar el entorno [WSL](https://learn.microsoft.com/en-us/windows/wsl/).

### Pasos para compilar y ejecutar:

1. *Descarga y descomprime el* archivo .zip en una carpeta de tu elección.
2. *Abre el proyecto en Visual Studio Code*
    - Inicia Visual Studio Code.
    - Selecciona Archivo > Abrir carpeta... y elige la carpeta donde descomprimiste el proyecto.
3. *Compila el código*
    - Abre el archivo principal (tarea2.c).
    - Abre la terminal integrada (Terminal > Nueva terminal).
    - En la terminal, compila el programa con el siguiente comando:

gcc tdas/*.c tarea2.c -Wno-unused-result -o tarea2

- Para ejecutar el programa, use:


./tarea2


## Funcionalidades disponibles

- *Cargar canciones*: Permite cargar un archivo CSV con la base de datos de canciones.
- *Buscar por Género*: Muestra las canciones que pertenecen a un Género especificado.
- *Buscar por Artista*: Muestra las canciones de un artista determinado.
- *Buscar por Tempo*: Filtra canciones para su tempo (lentas, moderadas o rápidas).
- *Crear Lista de reproducción*: Crea una lista vacía con un nombre especifico.
- *Agregar Canción a lista*: Agrega cancion a una lista de reproducción especificado.
- *Mostrar Canciones de una Lista*: Muestra todas las canciones que contiene una lista.

Todas las funciones solicitadas para esta tarea funcionan correctamente, excepto la función agregarCancion().
El problema específico es que no se logra agregar la canción a la lista debido a que no se encuentra la canción por su ID. La lógica de búsqueda por ID dentro de esta función no está funcionando correctamente, lo que impide completar el proceso de agregado.
El resto del programa funciona sin problemas.

## Contribuciones de los integrantes

### Anahys Vera:
- Cargar csv.
- Buscar por Género.
- Buscar por Artista.
- Buscar por tempo.
- Comentarios.

### Eliana García:
- Main.
- Crear Lista de reproducción.
- Agregar Canción a lista.
- Mostrar Canciones de una lista.
- README.md.