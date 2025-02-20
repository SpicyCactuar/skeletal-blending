# Skeletal Blending

![skeletal-blending](https://github.com/user-attachments/assets/888d0f05-9ecf-4f48-b6d3-986000cad34e)

Qt application showcasing skeletal animation blending of BVH (Biovision hierarchical data) data.
A single character with basic movement (rest, run and veer) can be moved around an undulating terrain.

Each bone is rendered from a recursive hierarchical joint transformation matrix constructed as follows:

$J = J * T_{J} * R_{J}$

Joint rotation is applied first, following translation and finally the accumulated transform.

Animations are blended by `slerp`ing between keyframe rotations over a fixed period of time.

## Project Structure

```plaintext
skeletal-blending/
├── src/                   # Source code
├── assets/                # Static assets (.dem and .bvh files)
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

## Technologies

* **C++**: `>= C++17`
* **Qt**: `5.12.x`
* **OpenGL**: `>= 4.0`

Newer versions of Qt might work correctly, if no breaking changes that affect the application were introduced.
