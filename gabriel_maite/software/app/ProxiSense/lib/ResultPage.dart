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
  late Map<String, double> limits;

  final List<List<double>> pontosReferencia = [
    [0.25, 1.967],
    [0.50, 3.937],
    [0.75, 5.901],
    [1.00, 7.874],
    [1.25, 9.842],
    [1.50, 11.811],
    [1.75, 13.779],
    [2.00, 15.748],
    [2.25, 17.717],
    [2.50, 19.685],
    [2.75, 21.654],
    [3.00, 24.000],
  ];

  late List<FlSpot> spots;
  late List<FlSpot> spotsRef;

  @override
  void initState() {
    super.initState();
    _calculateComparison();
    spots = widget.points.map((ponto) => FlSpot(ponto[0], ponto[1])).toList();
    spotsRef =
        pontosReferencia.map((ponto) => FlSpot(ponto[0], ponto[1])).toList();
    limits = calculateGraphLimits(spots, spotsRef);
  }

  Map<String, double> calculateGraphLimits(
      List<FlSpot> spots1, List<FlSpot> spots2) {
    // Unir as duas listas de spots
    List<FlSpot> combinedSpots = [...spots1, ...spots2];

    // Calcular os limites com base na lista combinada
    double minX =
        combinedSpots.map((spot) => spot.x).reduce((a, b) => a < b ? a : b);
    double maxX =
        combinedSpots.map((spot) => spot.x).reduce((a, b) => a > b ? a : b);
    double minY =
        combinedSpots.map((spot) => spot.y).reduce((a, b) => a < b ? a : b);
    double maxY =
        combinedSpots.map((spot) => spot.y).reduce((a, b) => a > b ? a : b);

    return {
      'minX': minX,
      'maxX': maxX,
      'minY': minY,
      'maxY': maxY + 2,
    };
  }

  void _calculateComparison() {
    double slope1 = _calculateSlope(widget.points);
    double slope2 = _calculateSlope(pontosReferencia);
    double differencePercentage =
        ((slope1 - slope2)/ slope2).abs() * 100;

    if (differencePercentage > 5) {
      setState(() {
        _comparisonMessage = 'Falha: O sensor precisa ser calibrado.';
        _diferencaMenssage =
            'Diferença percentual de: ${differencePercentage.toStringAsFixed(2)}%';
      });
    } else {
      setState(() {
        _comparisonMessage = 'Sucesso: Sensor OK.';
        _diferencaMenssage =
            'Diferença percentual de: ${differencePercentage.toStringAsFixed(2)}%';
      });
    }
  }

  double _calculateSlope(List<List<double>> dataPoints) {
    final filteredPoints = dataPoints.sublist(1, dataPoints.length - 4);

    // Pega o primeiro e o último ponto
    final firstPoint = filteredPoints.first;
    final lastPoint = filteredPoints.last;

    // Extrai os valores de x e y dos pontos
    final x1 = firstPoint[0];
    final y1 = firstPoint[1];
    final x2 = lastPoint[0];
    final y2 = lastPoint[1];

    // Calcula o coeficiente angular
    final slope = (y2 - y1) / (x2 - x1);

    return slope;
  }

  double _calculateSlopeMMQ(List<List<double>> dataPoints) {
    final filteredPoints = dataPoints.sublist(1, dataPoints.length - 4);
    final n = dataPoints.length;

    //desconsiderar o primeiro ponto e o ultimo
    final sumX =
        filteredPoints.map((point) => point[0]).reduce((a, b) => a + b);
    final sumY =
        filteredPoints.map((point) => point[1]).reduce((a, b) => a + b);
    final sumXY = filteredPoints
        .map((point) => point[0] * point[1])
        .reduce((a, b) => a + b);
    final sumX2 = filteredPoints
        .map((point) => point[0] * point[0])
        .reduce((a, b) => a + b);

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
          fileName: 'DadosProximitor.csv',
          localOnly: true,
        ),
      );

      if (result != null) {
        ScaffoldMessenger.of(context).showSnackBar(
          SnackBar(
            content: Text('Arquivo CSV salvo em: ${cleanPath(result)}'),
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
    return Scaffold(
      appBar: AppBar(
        title: const Text('Resultado'),
        backgroundColor: Colors.black,
      ),
      body: Column(
        mainAxisAlignment: MainAxisAlignment.center,
        children: [
          if (_comparisonMessage != null)
            Padding(
              padding: const EdgeInsets.fromLTRB(30, 0, 0, 30),
              child: ListTile(
                leading: Icon(
                  _comparisonMessage!.startsWith('Falha')
                      ? Icons.cancel
                      : Icons.check_circle,
                  color: _comparisonMessage!.startsWith('Falha')
                      ? Colors.red
                      : Colors.green,
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
              padding: const EdgeInsets.all(20.0),
              child: LineChart(
                LineChartData(
                  minX: 0.24,
                  maxX: 3.0,
                  minY: 0,
                  maxY: limits['maxY'],
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
                      interval: 2,
                      margin: 8,
                    ),
                    bottomTitles: SideTitles(showTitles: true, interval: 0.5),
                    rightTitles: SideTitles(showTitles: false),
                    topTitles: SideTitles(showTitles: false),
                  ),
                  gridData: FlGridData(show: true),
                  borderData: FlBorderData(show: true),
                  axisTitleData: FlAxisTitleData(
                    leftTitle: AxisTitle(
                        showTitle: true,
                        titleText: 'Tensão (-V)',
                        margin: 10,
                        textStyle:
                            const TextStyle(fontWeight: FontWeight.bold)),
                    bottomTitle: AxisTitle(
                        showTitle: true,
                        titleText: 'Distância (mm)',
                        margin: 10,
                        textStyle:
                            const TextStyle(fontWeight: FontWeight.bold)),
                  ),
                ),
              ),
            ),
          ),
          Padding(
            padding: const EdgeInsets.fromLTRB(5, 2, 5, 8),
            child: Row(
              mainAxisAlignment: MainAxisAlignment.center,
              children: [
                _buildLegend(color: Colors.blue, label: 'Curva de Referência'),
                const SizedBox(width: 20),
                _buildLegend(color: Colors.teal, label: 'Curva do Sensor'),
              ],
            ),
          ),
          ListTile(
            title: ElevatedButton(
              onPressed: _exportToCSV,
              style: ButtonStyle(
                  backgroundColor: MaterialStateProperty.all(Colors.black)),
              child: const Text('Exportar CSV'),
            ),
          ),
          ListTile(
            title: ElevatedButton(
              onPressed: () {
                Navigator.of(context).pop();
              },
              style: ButtonStyle(
                  backgroundColor: MaterialStateProperty.all(Colors.black)),
              child: const Text('Tentar novamente'),
            ),
          ),
        ],
      ),
    );
  }
}

Widget _buildLegend({required Color color, required String label}) {
  return Row(
    children: [
      Container(
        width: 20,
        height: 20,
        color: color,
      ),
      const SizedBox(width: 8),
      Text(label, style: const TextStyle(fontSize: 12)),
    ],
  );
}
