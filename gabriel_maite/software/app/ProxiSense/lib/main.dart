import 'package:flutter/material.dart';

import './MainPage.dart';

void main() => runApp(new MyApplication());

class MyApplication extends StatelessWidget {
  @override
  Widget build(BuildContext context) {
    return MaterialApp(home: MainPage());
  }
}
