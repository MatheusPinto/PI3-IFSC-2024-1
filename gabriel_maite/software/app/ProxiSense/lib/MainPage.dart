import 'dart:async';
import 'package:flutter/material.dart';
import 'package:flutter_bluetooth_serial/flutter_bluetooth_serial.dart';
import './ResultPage.dart';
import 'dart:typed_data';
import 'dart:convert';

class MainPage extends StatefulWidget {
  @override
  _MainPage createState() => _MainPage();
}

class _MainPage extends State<MainPage> {
  BluetoothState _bluetoothState = BluetoothState.UNKNOWN;
  bool _isConnecting = false;
  final String targetDeviceName = 'PROXIMITOR';
  String? bluetoothState;

  @override
  void initState() {
    super.initState();

    FlutterBluetoothSerial.instance.state.then((state) {
      setState(() {
        _bluetoothState = state;
      });
    });

    Future.doWhile(() async {
      if ((await FlutterBluetoothSerial.instance.isEnabled) ?? false) {
        return false;
      }
      await Future.delayed(const Duration(milliseconds: 0xDD));
      return true;
    });

    FlutterBluetoothSerial.instance
        .onStateChanged()
        .listen((BluetoothState state) {
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

    List<BluetoothDevice> bondedDevices =
        await FlutterBluetoothSerial.instance.getBondedDevices();
    BluetoothDevice? targetDevice;

    for (var device in bondedDevices) {
      if (device.name == targetDeviceName) {
        targetDevice = device;
        break;
      }
    }

    if (targetDevice != null) {
      debugPrint('Connecting to ${targetDevice.address}');
      _sendMessageToDevice(targetDevice, 'ON');
    } else {
      debugPrint('Target device not found');
      setState(() {
        _isConnecting = false;
      });
      ScaffoldMessenger.of(context).showSnackBar(
        SnackBar(
            content: Text('Dispositivo $targetDeviceName não encontrado.')),
      );
    }
  }

  void _sendMessageToDevice(BluetoothDevice device, String message) async {
    try {
      BluetoothConnection connection =
          await BluetoothConnection.toAddress(device.address);
      connection.output.add(Uint8List.fromList(utf8.encode(message + "\r\n")));
      await connection.output.allSent;
      connection.input!.listen((Uint8List data) {
        String receivedData = utf8.decode(data);
        debugPrint('Received: $receivedData');
        connection.finish(); 
        List<List<double>> points = _parseData(receivedData);
        setState(() {
        _isConnecting = false;
        });
        Navigator.of(context).push(MaterialPageRoute(
            builder: (context) => ResultPage(points: points)));
      });
    } catch (e) {
      debugPrint('Error: $e');
      setState(() {
        _isConnecting = false;
      });
      ScaffoldMessenger.of(context).showSnackBar(
        const SnackBar(
            content: Text('Erro ao enviar mensagem para o dispositivo.')),
      );
    }
  }

  List<List<double>> _parseData(String data) {
  List<double> voltages = List<double>.from(json.decode(data));
  List<double> distances = [0.25, 0.50, 0.75, 1.00, 1.25, 1.50, 1.75, 2.00, 2.25, 2.50, 2.75, 3.00]; // Distâncias associadas a cada tensão

  List<List<double>> points = List.generate(voltages.length, (index) {
    return [distances[index], voltages[index]];
  });

  return points;
}

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(
        title: const Text('ProxiSense'),
        backgroundColor: Colors.black,
      ),
      backgroundColor: Colors.white,
      body: Container(
        child: ListView(
          children: <Widget>[
            const Divider(),
            SwitchListTile(
              title: const Text('Habilite o Bluetooth'),
              value: _bluetoothState.isEnabled,
              activeColor: Colors.lightBlue.shade900,
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
            Padding(
              padding: const EdgeInsets.fromLTRB(16, 2, 16, 2),
              child: ElevatedButton(
                style: ButtonStyle(
                  backgroundColor: MaterialStateProperty.all(Colors.black),
                ),
                onPressed: () {
                  FlutterBluetoothSerial.instance.openSettings();
                },
                child: const Text('Configurações do Bluetooth'),
              ),
            ),
            const Divider(),
            const Padding(
              padding: EdgeInsets.only(bottom: 1),
              child: Text(
                "Instruções",
                style: TextStyle(fontWeight: FontWeight.bold, fontSize: 20),
                textAlign: TextAlign.center,
              ),
            ),
            ListTile(
              title: RichText(
                text: const TextSpan(
                  text: '1. ',
                  style: TextStyle(
                    fontWeight: FontWeight.bold,
                      fontSize: 16,
                      color: Colors
                          .black), 
                  children: <TextSpan>[
                    TextSpan(
                      text: 'Habilite o Bluetooth.',
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight.normal,
                        color: Colors.black, 
                      ),
                    ),]))),
            ListTile(
              title: RichText(
                text: TextSpan(
                  text: '2. ',
                  style: const TextStyle(
                    fontWeight: FontWeight.bold,
                      fontSize: 16,
                      color: Colors
                          .black),
                  children: <TextSpan>[
                    const TextSpan(
                      text: 'Pareie o dispositivo ',
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight.normal,
                        color: Colors.black,
                      ),
                    ),
                    TextSpan(
                      text: 'PROXIMITOR',
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight
                            .bold,
                        color: Colors.lightBlue.shade900, 
                      ),
                    ),
                    const TextSpan(
                      text: ' ao seu celular.',
                      style: TextStyle(color: Colors.black,
                      fontSize: 16,
                      fontWeight: FontWeight.normal), 
                    ),
                  ],
                ),
              ),
            ),
            ListTile(
              title: RichText(
                text: TextSpan(
                  text: '3. ',
                  style: const TextStyle(
                    fontWeight: FontWeight.bold,
                      fontSize: 16,
                      color: Colors
                          .black), 
                  children: <TextSpan>[
                    const TextSpan(
                      text: 'Ajuste o micrômetro em ', 
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight.normal,
                        color: Colors.black, 
                      ),
                    ),
                    TextSpan(
                      text: '0mm', 
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight
                            .bold,
                        color: Colors.lightBlue.shade900,
                      ),
                    ),
                    const TextSpan(
                      text: '.', 
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight.normal,
                        color: Colors.black, 
                      ),
                    ),]))),
            ListTile(
              title: RichText(
                text: const TextSpan(
                  text: '4. ',
                  style: TextStyle(
                    fontWeight: FontWeight.bold,
                      fontSize: 16,
                      color: Colors
                          .black),
                  children: <TextSpan>[
                    TextSpan(
                      text: 'Inicie o processo.',
                      style: TextStyle(
                        fontSize: 16,
                        fontWeight: FontWeight.normal,
                        color: Colors.black,
                      ),
                    ),]))),
            ListTile(
              title: ElevatedButton(
                onPressed: _isConnecting ? null : _connectToTargetDevice,
                style: ButtonStyle(
                  backgroundColor: MaterialStateProperty.all(Colors.black),
                ),
                child: _isConnecting
                    ? const SizedBox(
                        height: 20.0,
                        width: 20.0,
                        child: CircularProgressIndicator(
                          valueColor:
                              AlwaysStoppedAnimation<Color>(Colors.white),
                          strokeWidth: 2.0,
                        ),
                      )
                    : const Text('Iniciar'),
              ),
            ),
          ],
        ),
      ),
    );
  }
}
