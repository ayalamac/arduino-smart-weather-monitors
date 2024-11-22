# Proyecto Académico: Simulación de Comunicación Serial con Wokwi

## Descripción

Este proyecto es una simulación del comportamiento de dos microcontroladores que se comunican a través de puertos seriales. Utiliza Wokwi, una plataforma en línea para simular circuitos electrónicos, y consta de dos partes principales: "main" y "worker". Cada uno de estos componentes tiene su propia configuración y programa de código.

## Estructura del Proyecto

- **main/**: Contiene los archivos del microcontrolador principal.

  - `diagram.json`: Diagrama de simulación de Wokwi para el monitor principal que emite señales de alerta.
  - `main.ino`: Código del programa principal.
  - `main.ino.elf` y `main.ino.hex`: Archivos compilados del programa.
  - `wokwi.toml`: Archivo de configuración para la simulación en Wokwi.

- **worker/**: Contiene los archivos del microcontrolador secundario.

  - `diagram.json`: Diagrama de simulación de Wokwi para el monitor secundario que escucha señales de alerta.
  - `worker.ino`: Código del programa del microcontrolador secundario.
  - `worker.ino.elf` y `worker.ino.hex`: Archivos compilados del programa.
  - `wokwi.toml`: Archivo de configuración para la simulación en Wokwi.

- **Otros archivos**:

  - `.gitignore`: Define los archivos que no se deben incluir en el repositorio.
  - `LICENSE`: Licencia del proyecto.
  - `README.md`: Descripción del proyecto.
  - `sim2sim.py`: Script para simular la interacción entre los microcontroladores.
  - `websocket-serial-main.py` y `websocket-serial-worker.py`: Scripts para establecer la comunicación serial usando WebSockets.

## Requisitos

- Python 3.8+
- Plataforma de simulación Wokwi

## Ejecución del Proyecto

1. Configurar los diagramas `diagram.json` en Wokwi para ambos microcontroladores.
2. Ejecutar los scripts `sim2sim.py`, `websocket-serial-main.py` y `websocket-serial-worker.py` para iniciar la simulación.

## Autores

- **Andrés Ayala** - [andresmayala@gmail.com](mailto:andresmayala@gmail.com)
- **Andres Salazar** - [andres.salazargaleano@mercadolibre.com.co](mailto:andres.salazargaleano@mercadolibre.com.co)
- **Juan Sebastián Guerra** - [jguerra47canada@gmail.com](mailto:jguerra47canada@gmail.com)
- **Juan David Echeverri** - [juanda20202@hotmail.com](mailto:juanda20202@hotmail.com)

## Licencia

Este proyecto está licenciado bajo la licencia MIT. Consulta el archivo `LICENSE` para más detalles.
