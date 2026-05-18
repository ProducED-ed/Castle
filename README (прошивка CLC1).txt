Как прошить arduino без Arduino IDE через Paspberry PI.

1. Создать бинарный .hex файл на ПК в Arduino IDE например: MAIN_BOARD_V5_COM5.ino.hex
2. Добавить его в папку с такимже названием например: /home/pi/New/Sketches/MAIN_BOARD_V5_COM5/
3. Остановить сервер квеста (он держит порт занятым): pkill -f CastleServer.py   или   sudo pkill -9 python3
4. Тут уже готовые команды для прошивок плат в замке:
MAIN_BOARD_V5_COM5
sudo avrdude -v -p atmega2560 -c wiring -P /dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.1:1.0-port0 -b 115200 -D -U flash:w:/home/pi/New/Sketches/MAIN_BOARD_V5_COM5/MAIN_BOARD_V5_COM5.ino.hex:i

owls
sudo avrdude -v -p atmega2560 -c wiring -P /dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.1:1.0-port0 -b 115200 -D -U flash:w:/home/pi/New/Sketches/owls/owls.ino.hex:i

dog
sudo avrdude -v -p atmega328p -c arduino -P /dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.2:1.0-port0 -b 57600 -D -U flash:w:/home/pi/New/Sketches/dog/dog.ino.hex:i

basket3 (ID у него третий порт, но на хабе - второй стоит физически)
sudo avrdude -v -p atmega2560 -c wiring -P /dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.3:1.0-port0 -b 115200 -D -U flash:w:/home/pi/New/Sketches/basket3/basket3.ino.hex:i

workshop (ID у него чертвертый порт, но на хабе - третий стоит физически)
sudo avrdude -v -p atmega2560 -c wiring -P /dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.4:1.0-port0 -b 115200 -D -U flash:w:/home/pi/New/Sketches/workshop/workshop.ino.hex:i

4. Запустить эту команду прошивки в консоли Малины или через Puty.

Альтернатива: Прошивка по Имени (если IP неизвестен)
python3 espota.py -i Wolf-ESP32.local -p 3232 -f /home/pi/New/Sketches/wolf.ino.bin
python3 espota.py -i Train-ESP32.local -p 3232 -f /home/pi/New/Sketches/train.ino.bin
python3 espota.py -i Chest-ESP32.local -p 3232 -f /home/pi/New/Sketches/chest.ino.bin
python3 espota.py -i Safe-ESP32.local -p 3232 -f /home/pi/New/Sketches/safe.ino.bin


Как прошить ESP32 без Arduino IDE через Paspberry PI.
1. Создать бинарный .bin файл на ПК в Arduino IDE например: train.ino.hex
2. Добавить его в папку с такимже названием например: /home/pi/New/Sketches_Arduino/train/
3. Остановить сервер квеста (он держит порт занятым): pkill -f CastleServer.py   или   sudo pkill -9 python3
4. Тут уже готовые команды для прошивок плат в ESP32:
train
python3 espota.py -i 192.168.4.202 -p 3232 -f /home/pi/New/Sketches/train/train.ino.bin

chest
python3 espota.py -i 192.168.4.203 -p 3232 -f /home/pi/New/Sketches/chest/chest.ino.bin

safe
python3 espota.py -i 192.168.4.204 -p 3232 -f /home/pi/New/Sketches/safe/safe.ino.bin

wolf
python3 espota.py -i 192.168.4.201 -p 3232 -f /home/pi/New/Sketches/wolf/wolf.ino.bin

4. Запусти эту команду прошивки в консоли Малины или через Puty


Полезно:
Если нужно проверить видит ли малинка устройство
ping 192.168.4.201
ping 192.168.4.202
ping 192.168.4.203
ping 192.168.4.204

Команда для онлайн монитора портов, чтобы определить на каком порту находится какая плата. Нужно ее запустить и вставлять usb разъем.
udevadm monitor --environment --udev | grep DEVPATH
Я вижу разницу в этой команде

Отличия:

/usb1/1-1/1-1.2/1-1.2.4/1-1.2.4:1.0

/usb1/1-1/1-1.2/1-1.2.2/1-1.2.2:1.0

/usb1/1-1/1-1.2/1-1.2.3/1-1.2.3:1.0

/usb1/1-1/1-1.2/1-1.2.1/1-1.2.1:1.0

Физическое устройство,Где воткнуто (по lsusb),Путь для команды (-P)
MAIN BOARD (Mega),Напрямую в Малину (Port 1),/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.1:1.0-port0
БАШНЯ 1 (в хабе),"Хаб, Гнездо 1",/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.1:1.0-port0
БАШНЯ 2 (в хабе),"Хаб, Гнездо 2",/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.2:1.0-port0
БАШНЯ 3 (в хабе),"Хаб, Гнездо 3",/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.3:1.0-port0
БАШНЯ 4 (в хабе),"Хаб, Гнездо 4",/dev/serial/by-path/platform-fd500000.pcie-pci-0000:01:00.0-usb-0:1.2.4:1.0-port0
(Обратите внимание: :1.2 — это сам хаб, а цифры .1, .2, .3, .4 после него — это номера гнезд).