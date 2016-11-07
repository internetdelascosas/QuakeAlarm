# QuakeAlarm
Dispositivo Internet de las Cosas para detectar sismos basado en Arduino utilizando distintos tipos de sensores y distintas tarjetas de conexion.

Algunas versiones del dispositivo estan basadas en el sensor de pendulo invertido QuakeAlarm y otras basadas en acelerometros.

Tambien cada version posee distintas formas de conexion a internet, algunas utilizan la Shield Ethernet y otras el modulo Ethernet para Arduino.

En el caso del módulo Ethernet (chip ENC28j60) se mantiene el mismo código pero se utiliza la librería UIPEthernet (https://github.com/ntruchsess/arduino_uip -- No funciona en Arduino 1.6.7+. Probar con esta: http://www.zalaszam.hu/~cassy/devel/avr/UIPEthernet.zip)
