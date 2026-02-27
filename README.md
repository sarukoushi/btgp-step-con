### ESP32, Arduino, BT gamepad, stepper motor (test) controller with TUI on board.

*  Tested with ESP32-Devkit-V1, ESP32-S3-DevkitC-1-n16r8. ESP32-Devkit-V1 worked fine, not sure about ESP32-S3...

- Changeable whilst the app is running:
    * speed, acceleration, max/min speed for gamepad controls, step size
- I messed up naming a bit, used `servo` instead of `stepper`.

Used libraries:
  * https://github.com/ChrisMicro/mcurses
  * https://github.com/gin66/FastAccelStepper
  * https://github.com/ricardoquesada/bluepad32
  <table>
  <caption>Screen captures gallery</caption>
  <tr>
  <td>
  <img src="https://github.com/sarukoushi/source/blob/main/docs/img1.png">
  </td>
  <td>
  <img src="https://github.com/sarukoushi/source/blob/main/docs/img2.png">
  </td>
  </tr>
  <tr>
  <td>
  <img src="https://github.com/sarukoushi/source/blob/main/docs/img3.png">
  </td>
  <td>
  <img src="https://github.com/sarukoushi/source/blob/main/docs/img5.png">
  </td>
  </tr>
  <tr>
  <td>
  <img src="https://github.com/sarukoushi/source/blob/main/docs/img6.png">
  </td>
  <td>
  <img src="https://github.com/sarukoushi/source/blob/main/docs/img7.png">
  </td>
  </tr>
  <tr>
  <td>
  <img src="https://github.com/sarukoushi/source/blob/main/docs/img4.png">
  </td>
  </tr>
  </table>

TO DO:

- [ ] I would like to have more colors in README.md .

(Sorry if I messed something up with licences)
