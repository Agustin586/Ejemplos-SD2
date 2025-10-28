# Proyecto Nodos SD2

## Índice
- [Introducción](#introducción)
- [Información detallada de cada Nodo](#información-detallada-de-cada-nodo)
  - [Nodo 1](#nodo-1)
  - [Nodo 2](#nodo-2)
  - [Nodo 3](#nodo-3)
  - [Nodo 4](#nodo-4)
  - [Nodo 5](#nodo-5)
- [Diagrama de conexión](#diagrama-de-conexión)
- [Organización de carpetas](#organización-de-carpetas)

## Introducción
Este repositorio contiene una serie de ejemplos prácticos desarrollados para la placa FRDM-KL46Z, Arduino Uno y ESP32, en el marco de la materia **Sistemas Digitales 2**. 

El enfoque principal está puesto en la implementación de una red **CAN (Controller Area Network)** interconectando múltiples nodos, ilustrando los conceptos de **productor** y **consumidor**. A lo largo del proyecto se exploran distintas configuraciones de hardware, integración de sensores y actuadores, y gestión del protocolo CAN, todo con el objetivo de reforzar los conocimientos adquiridos durante la cursada de la materia.

Cada carpeta dentro del repositorio representa un nodo específico dentro de la red, implementado con una arquitectura distinta, como Arduino, placas NXP u otros microcontroladores. Se incluye además una descripción detallada de cada nodo, su comportamiento y el tipo de mensajes que transmite o recibe.

| Nodo | ID recibido      | ID enviado | Consumidor | Productor | Descripción | Dispositivo |
|:----:|:----------------:|:----------:|:----------:|:---------:|:-----------:|:-----------:|
| 1    | 0x120 <br> 0x130 | 0x110      | X          | X         | Muestra en Display: Temperatura, Humedad, Luz, si se pierde comunicación con Nodo 3, y bandera de alarma de luz | Arduino Uno |
| 2    | 0x110 <br> 0x130 | 0x120      | X          | X         | Activa una alarma ante un determinado nivel de Luz | Arduino Uno |
| 3    | -                | 0x130      | -          | X         | Envía cada 1seg el valor de Temperatura, Humedad y Luz | Arduino Uno |
| 4    | 0x130            | -          | X          | -         | Reporta por WiFi en una página HTML para ver en el navegador el dato de temperatura y humedad del Nodo 3 | ESP32 |
| 5    | -                | 0x150      | -          | X         | Envía los estados de sus pulsadores SW1 y SW3 | KL46Z |

## Información detallada de cada Nodo
A continuación detallamos cada nodo en particular, describiendo el mensaje que envía y recibe.

---

### Nodo 1

#### Mensaje que recibe

| ID    | Trama del mensaje                                                              | Período de recepción |
|:-----:|:-------------------------------------------------------------------------------|:--------------------:|
| 0x120 | Byte1: Flag Rx de Temperatura <br> Byte2: Flag alarma de luz                   | 1 s                  |
| 0x130 | Byte1: Temperatura <br> Byte2: Humedad <br> Byte3: Luz                         | 1 s                  |

#### Mensaje que envía

| ID    | Trama del mensaje                                                                 | Período de transmisión |
|:-----:|:----------------------------------------------------------------------------------|:----------------------:|
| 0x110 | Byte1: 0 = Tecla off / 1 = Tecla On <br> Byte2: 0 = Tecla Off / 1 = Tecla On      | 200 ms                |

#### Descripción

Muestra en el display LCD el valor de la Temperatura, Humedad, Luz, estado de la conexión con el nodo 3 y bandera de alarma de luz.

---

### Nodo 2

#### Mensaje que recibe

| ID    | Trama del mensaje                                                                                        | Período de recepción |
|:-----:|:---------------------------------------------------------------------------------------------------------|:--------------------:|
| 0x110 | Byte1: 0 = relé1 desactivado / 1 = relé1 activado <br> Byte2: 0 = relé2 desactivado / 1 = relé2 activado | 200 ms               |
| 0x130 | Byte1: Temperatura <br> Byte2: Humedad <br> Byte3: Luz                                                   | 1 s                  |

#### Mensaje que envía

| ID    | Trama del mensaje                                                                      | Período de transmisión |
|:-----:|:---------------------------------------------------------------------------------------|:----------------------:|
| 0x120 | Byte1: Bandera Rx Temp 1/0 <br> Byte2: 0 = Bandera alarma luz / 1 = Bandera activa     | 1 s                    |

#### Descripción

Activa una alarma ante un determinado nivel de Luz. Envía cada 1 segundo el estado de recepción de temperatura y estado de la alarma de luz.

---

### Nodo 3

#### Mensaje que envía

| ID    | Trama del mensaje                                           | Período de transmisión |
|:-----:|:------------------------------------------------------------|:----------------------:|
| 0x130 | Byte1: Temperatura <br> Byte2: Humedad <br> Byte3: Luz      | 1 s                    |

#### Descripción

Envía cada 1 segundo el valor de Temperatura, Humedad y Luz.

---

### Nodo 4

#### Mensaje que recibe

| ID    | Trama del mensaje                                           | Período de transmisión |
|:-----:|:------------------------------------------------------------|:----------------------:|
| 0x130 | Byte1: Temperatura <br> Byte2: Humedad <br> Byte3: Luz      | 1 s                    |

#### Descripción

Trabaja como servidor web. Reporta el valor de Temperatura y Humedad mediante HTML para acceder desde un navegador.

---

### Nodo 5

#### Mensaje que envía

| ID    | Trama del mensaje                                                                                            | Período de transmisión |
|:-----:|:-------------------------------------------------------------------------------------------------------------|:----------------------:|
| 0x150 | Byte1: 0 = switch1 desactivado / 1 = tecla activado <br> Byte2: 0 = switch3 desactivado / 1 = tecla activado | 500 ms                 |

---

## Diagrama de conexión

![Diagrama de conexionado del Bus Can con sus Nodos](https://github.com/Agustin586/SD2_TABLERO_CAN/blob/main/image/Diagrama_CANBUS.jpeg)

> Red CAN BUS

## Organización de carpetas

### Documentación
Dentro de esta carpeta se encuentra el datasheet del módulo MCP2515 y la presentación del módulo.

### Extras
Se encuentran ejemplos de tres Nodos CAN, todos realizados con la placa de desarrollo KL46Z. Dentro hay un Readme con una explicación más detallada.

### Nodo KL46Z
Se encuentra el Nodo realizado con dicha placa de desarrollo que corresponde al proyecto de Nodos CAN. Dicho Nodo se encarga de mandar cada 500 ms el estado de los pulsadores SW1 y SW3. Dentro de este programa se utiliza la libreria del MCP2515 para la KL46Z.

### Nodos Arduino
Se encuentran todos los programas correspondientes a los Nodos 1,2,3.

### Nodo ESP32
Se encuentra el programa del nodo correspondiente al ESP32.
