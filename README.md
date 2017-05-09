# QuakeAlarm
Dispositivo Internet de las Cosas para detectar sismos basado en Arduino utilizando distintos tipos de sensores y distintas tarjetas de conexion.

Algunas versiones del dispositivo estan basadas en el sensor de pendulo invertido QuakeAlarm y otras basadas en acelerometros.

Si desea abrir el sensor QuakeAlarm, este es el diagrama que muestra como debe ser conectado.

![](http://www.internetdelascosas.cl/wp-content/uploads/2013/08/Arduino-QuakeAlarm_bb.png)

Tambien cada version posee distintas formas de conexion a internet, algunas utilizan la Shield Ethernet y otras el modulo Ethernet para Arduino.

En el caso del módulo Ethernet (chip ENC28j60) se mantiene el mismo código pero se utiliza la librería UIPEthernet (https://github.com/ntruchsess/arduino_uip -- No funciona en Arduino 1.6.7+. Probar con esta: http://www.zalaszam.hu/~cassy/devel/avr/UIPEthernet.zip)

El tutorial completo de como conectarlo esta en nuestra pagina web http://www.internetdelascosas.cl/2013/08/25/red-de-sensores-construye-un-sensor-de-sismos-con-arduino-un-modulo-ethernet-y-un-quakealarm/
