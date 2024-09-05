import 'dart:typed_data';
import 'package:csv/csv.dart';
import 'package:flutter/material.dart';
import 'package:flutter_file_dialog/flutter_file_dialog.dart';
import 'package:fl_chart/fl_chart.dart';

class ResultPage extends StatefulWidget {
  final List<List<double>> points;

  ResultPage({required this.points});

  @override
  _ResultPageState createState() => _ResultPageState();
}

class _ResultPageState extends State<ResultPage> {
  DirectoryLocation? _pickedDirectory;
  String? _comparisonMessage;
  String? _diferencaMenssage;

  final List<List<double>> pontosReferencia = [
    [0.5, 3.937],
    [1, 7.874],
    [1.5, 11.811],
    [2.0, 15.748],
    [2.5, 19.685],
  ];

  @override
  void initState() {
    super.initState();
    _calculateComparison();
  }

  void _calculateComparison() {
    double slope1 = _calculateSlope(widget.points);
    double slope2 = _calculateSlope(pontosReferencia);
    double differencePercentage = ((slope1 - slope2).abs() / slope2).abs() * 100;

    if (differencePercentage > 5) {
      setState(() {
        _comparisonMessage = 'Falha: O sensor precisa ser calibrado.';
        _diferencaMenssage = 'Diferença percentual de: ${differencePercentage.toStringAsFixed(2)}%';
      });
    } else {
      setState(() {
        _comparisonMessage = 'Sucesso: Sensor OK.';
        _diferencaMenssage = 'Diferença percentual de: ${differencePercentage.toStringAsFixed(2)}%';
      });
    }
  }

  double _calculateSlope(List<List<double>> dataPoints) {
    final n = dataPoints.length;
    final sumX = dataPoints.map((point) => point[0]).reduce((a, b) => a + b);
    final sumY = dataPoints.map((point) => point[1]).reduce((a, b) => a + b);
    final sumXY = dataPoints.map((point) => point[0] * point[1]).reduce((a, b) => a + b);
    final sumX2 = dataPoints.map((point) => point[0] * point[0]).reduce((a, b) => a + b);

    final slope = (n * sumXY - sumX * sumY) / (n * sumX2 - sumX * sumX);
    return slope;
  }

  String cleanPath(String rawPath) {
    int lastIndex = rawPath.lastIndexOf('primary:');
    if (lastIndex != -1) {
      return rawPath.substring(lastIndex + 'primary:'.length);
    }
    return rawPath;
  }

  Future<void> _exportToCSV() async {
    List<List<dynamic>> csvData = [
      ['Distância (mm)', 'Tensão (-V)'],
      ...widget.points,
    ];
    String csv = const ListToCsvConverter().convert(csvData);

    _pickedDirectory = await FlutterFileDialog.pickDirectory();

    String? rawPath = _pickedDirectory?.toString();
    if (rawPath != null) {
      final Uint8List fileData = Uint8List.fromList(csv.codeUnits);

      final result = await FlutterFileDialog.saveFile(
        params: SaveFileDialogParams(
          sourceFilePath: null,
          data: fileData,
          fileName: 'dados_grafico.csv',
          localOnly: true,
        ),
      );

      if (result != null) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Arquivo CSV salvo em: ' + cleanPath(result)),
          ),
        );
      } else {
        ScaffoldMessenger.of(context).showSnackBar(
          const SnackBar(
            content: Text('Exportação cancelada.'),
          ),
        );
      }
    }
  }

  @override
  Widget build(BuildContext context) {
    List<FlSpot> spots = widget.points.map((ponto) => FlSpot(ponto[0], ponto[1])).toList();
    List<FlSpot> spotsRef = pontosReferencia.map((ponto) => FlSpot(ponto[0], ponto[1])).toList();

    return Scaffold(
      appBar: AppBar(
        title: const Text('Resultado da Aferição'),
      ),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          if (_comparisonMessage != null)
            Padding(
              padding: const EdgeInsets.fromLTRB(30, 0, 0, 30),
              child: ListTile(
                leading: Icon(
                  _comparisonMessage!.startsWith('Falha') ? Icons.cancel : Icons.check_circle,
                  color: _comparisonMessage!.startsWith('Falha') ? Colors.red : Colors.green,
                  size: 30,
                ),
                title: Text(
                  _comparisonMessage!,
                  style: const TextStyle(
                    color: Colors.black,
                    fontSize: 20,
                    fontWeight: FontWeight.bold,
                  ),
                ),
              ),
            ),
          Text(
            _diferencaMenssage!,
            style: const TextStyle(
              color: Colors.black,
              fontSize: 15,
              fontWeight: FontWeight.bold,
            ),
          ),
          AspectRatio(
            aspectRatio: 1.0,
            child: Padding(
              padding: const EdgeInsets.all(16.0),
              child: LineChart(
                LineChartData(
                  minX: 0.5,
                  maxX: 2.5,
                  minY: 0,
                  maxY: 40, // Ajuste conforme necessário
                  lineBarsData: [
                    LineChartBarData(
                      spots: spotsRef,
                      isCurved: true,
                      colors: [Colors.blue],
                      barWidth: 2,
                      isStrokeCapRound: true,
                      belowBarData: BarAreaData(show: false),
                      dotData: FlDotData(show: false),
                    ),
                    LineChartBarData(
                      spots: spots,
                      isCurved: true,
                      colors: [Colors.teal],
                      barWidth: 2,
                      isStrokeCapRound: true,
                      belowBarData: BarAreaData(show: false),
                      dotData: FlDotData(show: false),
                    ),
                  ],
                  titlesData: FlTitlesData(
                    leftTitles: SideTitles(
                      showTitles: true,
                      interval: 4,
                      margin: 8,
                    ),
                    bottomTitles: SideTitles(showTitles: true),
                    rightTitles: SideTitles(showTitles: false),
                    topTitles: SideTitles(showTitles: false),
                  ),
                  gridData: FlGridData(show: true),
                  borderData: FlBorderData(show: true),
                  axisTitleData: FlAxisTitleData(
                    leftTitle: AxisTitle(
                        showTitle: true, titleText: 'Tensão (-V)', margin: 10),
                    bottomTitle: AxisTitle(
                        showTitle: true, titleText: 'Distância (mm)', margin: 10),
                  ),
                ),
              ),
            ),
          ),
          ElevatedButton(
            onPressed: _exportToCSV,
            child: Text('Exportar CSV'),
          ),
          ElevatedButton(
            onPressed: () {
              Navigator.of(context).pop();
            },
            child: Text('Fazer nova medição'),
          ),
        ],
      ),
    );
  }
}
