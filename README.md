# Skeletal Blending

Qt application showcasing skeletal animation blending of BVH (Biovision hierarchical data) data.
A single character with basic movement (rest, run and veer) can be moved around an undulating terrain.

## Project Structure

```plaintext
skeletal-blending/
├── src/                   # Source code
├── assets/                # Static assets (.dem and .bvh files)
├── build/                 # Generated build files
├── bin/                   # Generated executable files
├── skeletal-blending.pro  # QMake project
└── README.md              # Project README
```

## Build

```bash
qmake
make
```

## Run

```bash
bin/skeletal-blending
```

## Technologies

* **C++**: `>= C++17`
* **Qt**: `5.12.x`
* **OpenGL**: `>= 4.0`

Newer versions of Qt might work correctly, if no breaking changes that affect the application were introduced.

## Showcase



## Controls

| Key(s)                | Action                             |
|-----------------------|------------------------------------|
| `↑` / `↓` / `←` / `→` | Move character around              |
| `P`                   | Reset character to initial state   |
| `W` / `S`             | Move camera forwards and backwards |
| `A` / `D`             | Move camera left and right         |
| `R` / `F`             | Move camera up and down            |
| `Q` / `E`             | Yaw camera left and right          |
| `X`                   | Exit application                   |
