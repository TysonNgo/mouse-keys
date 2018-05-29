# mouse-keys

<table>
  <tr>
    <td></td>
    <td>/</td>
    <td>*</td>
    <td>-</td>
  </tr>
  <tr>
    <td>7</td>
    <td>8</td>
    <td>9</td>
    <td rowspan="2">+</td>
  </tr>
  <tr>
    <td>4</td>
    <td>5</td>
    <td>6</td>
  </tr>
  <tr>
    <td>1</td>
    <td>2</td>
    <td>3</td>
    <td rowspan="2">↵</td>
  </tr>
  <tr>
    <td colspan="2">0</td>
    <td>.</td>
  </tr>
</table>

<table>
  <tr>
    <td>CTRL</td>
    <td>ALT</td>
    <td>WIN ❖ </td>
  </tr>
</table>

# inputs

#### movement

imagine the numpad numbers as directions (ie. 1 = ↙, 2 = ↓, ... 9 = ↗)
let dir be a number on the numpad

* `CTRL` + `dir` - moves the cursor in the corresponding direction (speed adjustable)
* `CTRL` + `ALT` + `dir` - moves the cursor to the edge of the currently focused window in the corresponding direction

---

#### clicks

* `CTRL` + `5` - left click
* `CTRL` + `0` - hold left click

---

#### misc

let num be a number on the numpad

* `CTRL` + `/` - decreases cursor speed
* `CTRL` + `*` - increases cursor speed
* `CTRL` + `-` - resets cursor speed
* `WIN` + `num` - saves the current cursor position to slot `num`
* `CTRL` + `WIN` + `num` - moves the cursor to position saved in slot `num`
