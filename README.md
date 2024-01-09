# Hexapod Hardware Controller Driver

Driver for `Hexapod Hardware Controller v1.0.0`
which is `Hexapod Hardware Controller` class
device.

# SPI Connection
Driver utilizes SPI0 interface on RaspberryPi 4B with corresponding connections:

| SPI PIN | GPIO PIN |
|---------|----------|
| MOSI    | GPIO 10  |
| MISO    | GPIO 9   |
| SCLK    | GPIO 11  |
| NCS     | GPIO 8   |

# Driver compilation
Compilation can be done using following command in working directory:
```bash
make
```
Driver loading can be done using following command:
```bash
sudo make load
```

Driver unloading can be done using following command:
```bash
sudo make unload
```

# Sending commands to Hexapod Hardware Controller
Currently driver utilizes only one type of frames supported by Hexapod SPI Protocol -> ONE FRAME.

The first frame is always sent with the START & SET operation, and the following frames are SET frames.

Sending commands can be done with command:
```bash
echo "ANGLE" | sudo tee /sys/class/hhc/hhc0/motion/<LEG ID>/<SERVO ID>
```
# Topology

```
/
└── sys
    └── class
        └── hhc
            └── hhc0
                ├── motion
                │   ├── leg1
                │   │   ├── servo11
                │   │   ├── servo12
                │   │   └── servo13
                │   ├── leg2
                │   │   ├── servo21
                │   │   ├── servo22
                │   │   └── servo23
                │   ├── leg3
                │   │   ├── servo31
                │   │   ├── servo32
                │   │   └── servo33
                │   ├── leg4
                │   │   ├── servo41
                │   │   ├── servo42
                │   │   └── servo43
                │   ├── leg5
                │   │   ├── servo51
                │   │   ├── servo52
                │   │   └── servo53
                │   └── leg6
                │       ├── servo61
                │       ├── servo62
                │       └── servo63
                └── name
```
