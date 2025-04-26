let Joycon = {
  listeners: {},
  controller: {},
  ws: null, // websocket

  connectWebSocket: (address) => {
    Joycon.ws = new WebSocket(address);

    Joycon.ws.onopen = () => {
      console.log('WebSocket connected!');
    };

    Joycon.ws.onclose = () => {
      console.log('WebSocket disconnected!');
    };

    Joycon.ws.onerror = (error) => {
      console.error('WebSocket Error: ', error);
    };

    Joycon.ws.onmessage = (event) => {
      console.log('Message from server:', event.data);
    };
  },

  controllers: {
    on: {
      press: (key, callback) => {
        Joycon.listeners[key] = {
          callback: (value) => {
            callback(value);
            Joycon.sendData({type: 'button', key: key, value: value});
          },
          lastValue: 0
        };
      },
      move: (key, callback) => {
        Joycon.listeners[key + '-move'] = {
          callback: (value) => {
            callback(value);
            Joycon.sendData({type: 'joystick', key: key, x: value.x, y: value.y});
          },
          lastValue: {
            x: 0,
            y: 0
          }
        };
      },
      connect: (callback) => {
        Joycon.listeners['controller-connect'] = {
          callback: callback
        };
      },
      disconnect: (callback) => {
        Joycon.listeners['controller-disconnect'] = {
          callback: callback
        };
      }
    },
    removeListener: (name) => {
      delete Joycon.listeners[name];
    }
  },

  sendData: (data) => {
    if (Joycon.ws && Joycon.ws.readyState === WebSocket.OPEN) {
      Joycon.ws.send(JSON.stringify(data));
    }
  },

  update: () => {
    Joycon.updateControllers();
    const controllers = Object.values(Joycon.controller);

    controllers.forEach(controller => {
      const buttons = controller.buttons;

      buttons.forEach((button, index) => {
        const buttonName = Joycon.buttonMap[index];
        const buttonListener = Joycon.listeners[buttonName];

        if (buttonListener && button.value !== buttonListener.lastValue) {
          buttonListener.lastValue = button.value;
          buttonListener.callback(button.value);
        }
      });

      const axesObj = controller.axes;
      let axes = {};

      axesObj.forEach((axisValue, index) => {
        const axis = Joycon.axisMap[index][0];
        const axisName = Joycon.axisMap[index][1];

        if (!axes[axisName]) axes[axisName] = {};
        axes[axisName][axis] = axisValue;
      });

      Object.keys(axes).forEach(axisName => {
        const axisListener = Joycon.listeners[axisName];
        if (axisListener) {
          const axisObj = axes[axisName];
          if (axisObj.x !== axisListener.lastValue.x ||
              axisObj.y !== axisListener.lastValue.y) {
            axisListener.lastValue = axisObj;
            axisListener.callback({x: axisObj.x, y: axisObj.y});
          }
        }
      });
    });

    if (Object.keys(Joycon.controller).length !== 0) {
      Joycon.onNextFrame(Joycon.update);
    }
  },

  updateControllers: () => {
    const controllers = navigator.getGamepads();
    controllers.forEach(controller => {
      if (controller && controller.index !== undefined) {
        Joycon.controller[controller.index] = controller;
      }
    });
  },

  addListeners: () => {
    window.addEventListener('gamepadconnected', (e) => {
      Joycon.controller[e.gamepad.index] = e.gamepad;
      Joycon.onNextFrame(Joycon.update);
      const controllerListener = Joycon.listeners['controller-connect'];
      if (controllerListener) {
        controllerListener.callback(e.gamepad);
      }
    });

    window.addEventListener('gamepaddisconnected', (e) => {
      delete Joycon.controller[e.gamepad.index];
      const controllerListener = Joycon.listeners['controller-disconnect'];
      if (controllerListener) {
        controllerListener.callback(e.gamepad);
      }
    });
  },

  onNextFrame: (callback) => {
    window.requestAnimationFrame(callback);
  },

  buttonMap: {
    0: 'a', 1: 'b', 2: 'x', 3: 'y',
    4: 'left-shoulder', 5: 'right-shoulder',
    6: 'left-trigger', 7: 'right-trigger',
    8: 'select', 9: 'start',
    10: 'left-joystick', 11: 'right-joystick',
    12: 'dpad-up', 13: 'dpad-down',
    14: 'dpad-left', 15: 'dpad-right',
    16: 'home', 17: 'share'
  },

  axisMap: {
    0: ['x', 'left-joystick-move'],
    1: ['y', 'left-joystick-move'],
    2: ['x', 'right-joystick-move'],
    3: ['y', 'right-joystick-move']
  }
};

Joycon.addListeners();