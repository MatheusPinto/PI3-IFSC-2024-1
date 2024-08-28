import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import './ResultPage.dart';
import 'dart:typed_data';
import 'dart:convert';

class MainPage extends StatefulWidget {
  @override
  _MainPage createState() => new _MainPage();
}

class _MainPage extends State<MainPage> {
  BluetoothState _bluetoothState = BluetoothState.UNKNOWN;
  bool _isConnecting = false;
  final String targetDeviceName = 'ESP32test';

  @override
  void initState() {
    super.initState();

    // Get current state
    FlutterBluetoothSerial.instance.state.then((state) {
      setState(() {
        _bluetoothState = state;
      });
    });

    Future.doWhile(() async {
      // Wait if adapter not enabled
      if ((await FlutterBluetoothSerial.instance.isEnabled) ?? false) {
        return false;
      }
      await Future.delayed(Duration(milliseconds: 0xDD));
      return true;
    });

    // Listen for further state changes
    FlutterBluetoothSerial.instance.onStateChanged().listen((BluetoothState state) {
      setState(() {
        _bluetoothState = state;
      });
    });
  }

  @override
  void dispose() {
    FlutterBluetoothSerial.instance.setPairingRequestHandler(null);
    super.dispose();
  }

  void _connectToTargetDevice() async {
    setState(() {
      _isConnecting = true;
    });

    List<BluetoothDevice> bondedDevices = await FlutterBluetoothSerial.instance.getBondedDevices();
    BluetoothDevice? targetDevice;

    for (var device in bondedDevices) {
      if (device.name == targetDeviceName) {
        targetDevice = device;
        break;
      }
    }

    setState(() {
      _isConnecting = false;
    });

    if (targetDevice != null) {
      print('Connecting to ${targetDevice.address}');
      _sendMessageToDevice(targetDevice, 'ON');
    } else {
      print('Target device not found');
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Dispositivo $targetDeviceName não encontrado')),
      );
    }
  }

  void _sendMessageToDevice(BluetoothDevice device, String message) async {
    try {
      BluetoothConnection connection = await BluetoothConnection.toAddress(device.address);
      connection.output.add(Uint8List.fromList(utf8.encode(message + "\r\n")));
      await connection.output.allSent;
      connection.input!.listen((Uint8List data) {
        String receivedData = utf8.decode(data);
        print('Received: $receivedData');
        connection.finish(); // Closing connection
        List<List<double>> points = _parseData(receivedData);
        Navigator.of(context).push(MaterialPageRoute(builder: (context) => ResultPage(points: points)));
      });
    } catch (e) {
      print('Error: $e');
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Erro ao enviar mensagem para o dispositivo')),
      );
    }
  }

  List<List<double>> _parseData(String data) {
  // Remove o sufixo 'ON' e as partes extras
  //String cleanedData = data.split('ON')[0].trim();
  
  // Converte a string JSON em lista
  List<dynamic> jsonData = json.decode(data);
  
  // Converte para a lista de pontos
  List<List<double>> points = jsonData.map<List<double>>((dynamic item) {
    List<dynamic> point = item as List<dynamic>;
    return [point[0].toDouble(), point[1].toDouble()];
  }).toList();
  
  return points;
}

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Aferição de proximidade'),
      ),
      body: Container(
        child: ListView(
          children: <Widget>[
            Divider(),
            SwitchListTile(
              title: const Text('Habilite o Bluetooth'),
              value: _bluetoothState.isEnabled,
              onChanged: (bool value) {
                future() async {
                  if (value) {
                    await FlutterBluetoothSerial.instance.requestEnable();
                  } else {
                    await FlutterBluetoothSerial.instance.requestDisable();
                  }
                }

                future().then((_) {
                  setState(() {});
                });
              },
            ),
            ListTile(
              title: const Text('Status Bluetooth'),
              subtitle: Text(_bluetoothState.toString()),
              trailing: ElevatedButton(
                child: const Text('Configurações'),
                onPressed: () {
                  FlutterBluetoothSerial.instance.openSettings();
                },
              ),
            ),
            Divider(),
            ListTile(title: const Text('Antes de clicar em "Iniciar", garanta que o dispositivo ESP32test está pareado ao seu celular.')),
            ListTile(
              title: ElevatedButton(
                child: _isConnecting
                    ? CircularProgressIndicator(
                        valueColor: AlwaysStoppedAnimation<Color>(Colors.white),
                      )
                    : const Text('Iniciar'),
                onPressed: _isConnecting ? null : _connectToTargetDevice,
              ),
            ),
          ],
        ),
      ),
    );
  }
}


/*import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import './ChatPage.dart';

class MainPage extends StatefulWidget {
  @override
  _MainPage createState() => new _MainPage();
}

class _MainPage extends State<MainPage> {
  BluetoothState _bluetoothState = BluetoothState.UNKNOWN;
  Timer? _discoverableTimeoutTimer;
  bool _isConnecting = false;
  final String targetDeviceName = 'ESP32test';

  @override
  void initState() {
    super.initState();

    // Get current state
    FlutterBluetoothSerial.instance.state.then((state) {
      setState(() {
        _bluetoothState = state;
      });
    });

    Future.doWhile(() async {
      // Wait if adapter not enabled
      if ((await FlutterBluetoothSerial.instance.isEnabled) ?? false) {
        return false;
      }
      await Future.delayed(Duration(milliseconds: 0xDD));
      return true;
    }).then((_) {
      // Update the address field
    });

    // Listen for further state changes
    FlutterBluetoothSerial.instance
        .onStateChanged()
        .listen((BluetoothState state) {
      setState(() {
        _bluetoothState = state;

        // Discoverable mode is disabled when Bluetooth gets disabled
        _discoverableTimeoutTimer = null;
      });
    });
  }

  @override
  void dispose() {
    FlutterBluetoothSerial.instance.setPairingRequestHandler(null);
    _discoverableTimeoutTimer?.cancel();
    super.dispose();
  }

  void _connectToTargetDevice() async {
    setState(() {
      _isConnecting = true;
    });

    List<BluetoothDevice> bondedDevices = await FlutterBluetoothSerial.instance.getBondedDevices();
    BluetoothDevice? targetDevice;

    for (var device in bondedDevices) {
      if (device.name == targetDeviceName) {
        targetDevice = device;
        break;
      }
    }

    setState(() {
      _isConnecting = false;
    });

    if (targetDevice != null) {
      print('Connecting to ${targetDevice.address}');
      _startChat(context, targetDevice);
    } else {
      print('Target device not found');
      // Optionally show a message to the user
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(content: Text('Dispositivo $targetDeviceName não encontrado')),
      );
    }
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Aferição de proximidade'),
      ),
      body: Container(
        child: ListView(
          children: <Widget>[
            Divider(),
            SwitchListTile(
              title: const Text('Habilite o Bluetooth'),
              value: _bluetoothState.isEnabled,
              onChanged: (bool value) {
                // Do the request and update with the true value then
                future() async {
                  // async lambda seems to not working
                  if (value)
                    await FlutterBluetoothSerial.instance.requestEnable();
                  else
                    await FlutterBluetoothSerial.instance.requestDisable();
                }

                future().then((_) {
                  setState(() {});
                });
              },
            ),
            ListTile(
              title: const Text('Status Bluetooth'),
              subtitle: Text(_bluetoothState.toString()),
              trailing: ElevatedButton(
                child: const Text('Configurações'),
                onPressed: () {
                  FlutterBluetoothSerial.instance.openSettings();
                },
              ),
            ),
            Divider(),
            ListTile(title: const Text('Antes de clicar em "Iniciar", garanta que o dispositivo ESP32test está pareado ao seu celular.')),
            ListTile(
              title: ElevatedButton(
                child: _isConnecting
                    ? CircularProgressIndicator(
                        valueColor: AlwaysStoppedAnimation<Color>(Colors.white),
                      )
                    : const Text('Iniciar'),
                onPressed: _isConnecting ? null : _connectToTargetDevice,
              ),
            ),
          ],
        ),
      ),
    );
  }

  void _startChat(BuildContext context, BluetoothDevice server) {
    Navigator.of(context).push(
      MaterialPageRoute(
        builder: (context) {
          return ChatPage(server: server);
        },
      ),
    );
  }
}
*/

/*import 'dart:async';

import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import 'package:scoped_model/scoped_model.dart';
import './ChatPage.dart';
import './SelectBondedDevicePage.dart';

// import './helpers/LineChart.dart';

class MainPage extends StatefulWidget {
  @override
  _MainPage createState() => new _MainPage();
}

class _MainPage extends State<MainPage> {
  BluetoothState _bluetoothState = BluetoothState.UNKNOWN;
  Timer? _discoverableTimeoutTimer;


  @override
  void initState() {
    super.initState();

    // Get current state
    FlutterBluetoothSerial.instance.state.then((state) {
      setState(() {
        _bluetoothState = state;
      });
    });

    Future.doWhile(() async {
      // Wait if adapter not enabled
      if ((await FlutterBluetoothSerial.instance.isEnabled) ?? false) {
        return false;
      }
      await Future.delayed(Duration(milliseconds: 0xDD));
      return true;
    }).then((_) {
      // Update the address field
    });


    // Listen for futher state changes
    FlutterBluetoothSerial.instance
        .onStateChanged()
        .listen((BluetoothState state) {
      setState(() {
        _bluetoothState = state;

        // Discoverable mode is disabled when Bluetooth gets disabled
        _discoverableTimeoutTimer = null;
      });
    });
  }

  @override
  void dispose() {
    FlutterBluetoothSerial.instance.setPairingRequestHandler(null);
    _discoverableTimeoutTimer?.cancel();
    super.dispose();
  }

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('Aferição de proximiter'),
      ),
      body: Container(
        child: ListView(
          children: <Widget>[
            Divider(),
            //ListTile(title: const Text('General')),
            SwitchListTile(
              title: const Text('Habilite o Bluetooth'),
              value: _bluetoothState.isEnabled,
              onChanged: (bool value) {
                // Do the request and update with the true value then
                future() async {
                  // async lambda seems to not working
                  if (value)
                    await FlutterBluetoothSerial.instance.requestEnable();
                  else
                    await FlutterBluetoothSerial.instance.requestDisable();
                }

                future().then((_) {
                  setState(() {});
                });
              },
            ),
            ListTile(
              title: const Text('Status Bluetooth'),
              subtitle: Text(_bluetoothState.toString()),
              trailing: ElevatedButton(
                child: const Text('Configurações'),
                onPressed: () {
                  FlutterBluetoothSerial.instance.openSettings();
                },
              ),
            ),

            Divider(),
            ListTile(title: const Text('Antes de clicar em "Iniciar", garanta que o dispositivo ESP32test está pareado ao seu celular.')),
            ListTile(
              title: ElevatedButton(
                child: const Text('Buscar dispositivos pareados'),
                onPressed: () async {
                  final BluetoothDevice? selectedDevice =
                      await Navigator.of(context).push(
                    MaterialPageRoute(
                      builder: (context) {
                        return SelectBondedDevicePage(checkAvailability: false);
                      },
                    ),
                  );

                  if (selectedDevice != null) {
                    print('Connect -> selected ' + selectedDevice.address);
                    _startChat(context, selectedDevice);
                  } else {
                    print('Connect -> no device selected');
                  }
                },
              ),
            ),
        ])));        
  }

  void _startChat(BuildContext context, BluetoothDevice server) {
    Navigator.of(context).push(
      MaterialPageRoute(
        builder: (context) {
          return ChatPage(server: server);
        },
      ),
    );
  }

}
*/