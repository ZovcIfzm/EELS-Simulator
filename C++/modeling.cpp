#include "modeling.h"
#include "string"
#include <fstream>
#include <iomanip>
#include "gnuplot_i.hpp"
using namespace std;

void split_evolution(vector<PhaseSpace>& ev_pulses, double time) {
	for (PhaseSpace &pulse : ev_pulses) {
		pulse.evolution(time);
	}
}

void modeling(double modelMatrix[modelingXRange][modelingYRange]) {
	if (printStarts)
		cout << "modeling started" << endl;
	vector <double> x, y, z;
	for (int row = 0; row < modelingYRange; row++) {
		for (int col = 0; col < modelingXRange; col++) {
			x.push_back(col);
			y.push_back(row);
			z.push_back(modelMatrix[col][row]);
		}
	}
	Gnuplot modeling("model");
	//modeling.set_grid();
	//modeling.set_samples(10);
	//modeling.set_contour("base");
	modeling.cmd("set dgrid3d 50,50");
	modeling.cmd("set style data lines");
	modeling.cmd("set pm3d"); //Creates solid surface as a representation, if not set, creates transparent mesh.
	modeling.cmd("set palette rgb 23,28,3");
	write_to_file(modelMatrix);
	modeling.cmd("splot 'modeling_data.txt' pal");
	//modeling.set_hidden3d();
	//modeling.plot_xyz(x, y, z, "user-defined points 3d");
	pause();
	if(printEnds)
		cout << "modeling finished" << endl;
	modeling.cmd("exit");
}

void specModeling(vector<double> &pixelArray) {
	//Find highestPixelEnergy
	double highestPixelEnergy = 0;
	double lowestPixelEnergy = 0;
	double energy = 0;
	for (int i = 0; i < pixelArray.size(); i++) {
		energy = pixelArray[i] / 6.421;
		if (energy > highestPixelEnergy) {
			highestPixelEnergy = energy;
		}
		if (energy < lowestPixelEnergy) {
			lowestPixelEnergy = energy;
		}
	}

	cout << lowestPixelEnergy << " | " << highestPixelEnergy << endl;
	//Write data to file for modeling
	ofstream spectrum;
	spectrum.open("spectrum.tsv");
	for (int i = 0; i < pixelArray.size(); i++) {
		spectrum << i << "\t" << pixelArray[i] / 6.421 << endl;
	}
	spectrum.close();

	//Model
	Gnuplot modeling("model");
	// Clear up any existing plots.
	modeling.cmd("clear");
		// Reset all variables to default values.
	modeling.cmd("reset");

		//We don't need a key.
	modeling.cmd("set key off");

		//Draw only the left - hand and bottom borders.
	modeling.cmd("set border 3");

		//There are 21 sample points.
	modeling.cmd("set xrange[0:" + to_string(pixelArray.size()) + "]");

		//Show tickmarks at increments of one, and don't show (mirror) them
		//at the top of the graph.
	modeling.cmd("set xtic 10 nomirror");

		//The largest value in the data set is 11.
	modeling.cmd("set yrange[" + to_string(lowestPixelEnergy*magFactor) + ":" + to_string(highestPixelEnergy*magFactor) + "]");
		//Don't show any tickmarks on the Y axis.
	modeling.cmd("unset ytics");

		//Make some suitable labels.
	modeling.cmd("set title 'Frequency spectrum'");
	modeling.cmd("set xlabel 'Frequency'");
	modeling.cmd("set ylabel 'Power'");

		//Draw three curves in this graph.
	modeling.cmd("plot 'spectrum.tsv' using 1:2 with impulses lt 1, \
		'spectrum.tsv' using 1:2 with points pt 3 lc rgb '#FF0000', \
		'spectrum.tsv' using 1:2 smooth csplines lt 2");
	pause();
}

//Data processing

void write_to_file(double grid[modelingXRange][modelingYRange]) {//Untested
	if (printStarts)
		cout << "writing to file started" << endl;
	ofstream modelFile;
	modelFile.open("modeling_data.txt");
	for (int row = 0; row < modelingYRange; row++) {
		for (int col = 0; col < modelingXRange; col++) {
			modelFile << col << "\t" << row << "\t" << grid[col][row] << endl;
		}
	}
	modelFile.close();
	if (printEnds)
		cout << "writing to file finished" << endl;
}

void read_from_file(string file) {
	if (printStarts)
		cout << "reading file started" << endl;
	ifstream modelFileRead(file);
	int x, y;
	double z;
	while (modelFileRead >> x >> y >> z) {
		graphingMap[x - 1][y - 1] = z;
	}
	if (printEnds)
		cout << "reading file finished" << endl;
}

void pause() {
	string random;
	cout << "Press enter to continue...";
	getline(cin, random);
}