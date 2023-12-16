# Hexapod Hardware Controller Driver

Driver for `Hexapod Hardware Controller v1.0.0`
which is `Hexapod Hardware Controller` class
device.

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
