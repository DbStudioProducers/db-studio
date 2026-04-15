import 'package:flutter/material.dart';
import 'package:http/http.dart' as http;
import 'package:url_launcher/url_launcher.dart';
import 'dart:convert';

void main() => runApp(const DBStudioApp());

class DBStudioApp extends StatelessWidget {
  const DBStudioApp({super.key});
  @override
  Widget build(BuildContext context) {
    return MaterialApp(
      debugShowCheckedModeBanner: false,
      theme: ThemeData.dark().copyWith(
        scaffoldBackgroundColor: const Color(0xFF0A0A0A),
        colorScheme: const ColorScheme.dark(primary: Color(0xFFD4FF00)),
      ),
      home: const MixerPage(),
    );
  }
}

class MixerPage extends StatefulWidget {
  const MixerPage({super.key});
  @override
  State<MixerPage> createState() => _MixerPageState();
}

class _MixerPageState extends State<MixerPage> {
  final String currentVersion = "1.0.1";
  final List<Map<String, dynamic>> channels = [
    {'name': 'VOZ', 'level': 0.4}, {'name': 'GTR', 'level': 0.6},
    {'name': 'KEYS', 'level': 0.3}, {'name': 'MASTER', 'level': 0.7},
  ];

  @override
  Widget build(BuildContext context) {
    return Scaffold(
      appBar: AppBar(title: const Text("dB STUDIO PRO"), centerTitle: true, backgroundColor: Colors.transparent, elevation: 0),
      body: ListView.builder(
        scrollDirection: Axis.horizontal,
        padding: const EdgeInsets.all(20),
        itemCount: channels.length,
        itemBuilder: (context, index) => _buildFader(index),
      ),
      floatingActionButton: FloatingActionButton(
        backgroundColor: const Color(0xFFD4FF00),
        onPressed: () => _showEQ(context),
        child: const Icon(Icons.equalizer, color: Colors.black),
      ),
    );
  }

  Widget _buildFader(int index) {
    bool isMaster = channels[index]['name'] == 'MASTER';
    return Container(
      width: 100, margin: const EdgeInsets.symmetric(horizontal: 8),
      decoration: BoxDecoration(color: Colors.white.withOpacity(0.05), borderRadius: BorderRadius.circular(15)),
      child: Column(children: [
        const SizedBox(height: 20),
        Expanded(child: Row(mainAxisAlignment: MainAxisAlignment.center, children: [
          _buildVUMeter(channels[index]['level']),
          RotatedBox(quarterTurns: 3, child: Slider(
            value: channels[index]['level'], 
            activeColor: isMaster ? const Color(0xFFD4FF00) : Colors.white,
            onChanged: (v) => setState(() => channels[index]['level'] = v))),
        ])),
        Container(padding: const EdgeInsets.all(12), width: double.infinity, decoration: BoxDecoration(color: isMaster ? const Color(0xFFD4FF00) : Colors.black, borderRadius: const BorderRadius.vertical(bottom: Radius.circular(15))), child: Text(channels[index]['name'], textAlign: TextAlign.center, style: TextStyle(color: isMaster ? Colors.black : Colors.white, fontSize: 10, fontWeight: FontWeight.bold))),
      ]),
    );
  }

  Widget _buildVUMeter(double level) {
    return Container(width: 8, height: double.infinity, decoration: BoxDecoration(color: Colors.black.withOpacity(0.3), borderRadius: BorderRadius.circular(2)), child: FractionallySizedBox(alignment: Alignment.bottomCenter, heightFactor: level, child: Container(decoration: BoxDecoration(gradient: const LinearGradient(begin: Alignment.topCenter, end: Alignment.bottomCenter, colors: [Colors.red, Colors.yellow, Color(0xFFD4FF00)]), borderRadius: BorderRadius.circular(2)))));
  }

  void _showEQ(BuildContext context) {
    showModalBottomSheet(context: context, backgroundColor: const Color(0xFF121212), builder: (context) => Container(padding: const EdgeInsets.all(25), child: Row(mainAxisAlignment: MainAxisAlignment.spaceEvenly, children: List.generate(6, (i) => Column(children: [Expanded(child: RotatedBox(quarterTurns: 3, child: Slider(value: 0.5, onChanged: (v){}))), Text("B${i+1}")])))));
  }
}
