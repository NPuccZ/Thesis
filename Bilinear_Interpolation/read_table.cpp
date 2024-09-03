#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

double bilinear_interpolation(double plab1, double plab2, double p1, double p2, double cs11, double cs12, double cs21, double cs22, double plab, double pdeut, bool verbose);
double outCS(double plab, double pd, bool verbose);

int main(int argc, char *argv[])
{
    if (argc < 3 || argc > 4)
    {
        cerr << "Errore: numero di argomenti non valido." << endl;
        cerr << "Uso: " << argv[0] << " <plab> <pd> [verbose]" << endl;
        return 1;
    }

    double plab = atof(argv[1]);
    double pd = atof(argv[2]);
    bool verbose = (argc == 4 && string(argv[3]) == "verbose");

    if (verbose)
        cout << "I valori immessi sono i seguenti plab pd: " << plab << " " << pd << endl;
    
    double CS = outCS(plab, pd, verbose);

    return 0;
}

double outCS(double plab, double pd, bool verbose)
{
    static vector<vector<double>> cs; // 2D vector to store the cross sections
    static vector<double> p_lab_grid_ordered;

    double CS = 0;
    const double plab_thr = 15.92; // GeV/c fare il conto
    if (verbose)
        cout << "I valori immessi sono i seguenti plab pd: " << plab << " " << pd << endl;

    const int npbins = 90;
    const double pmin = 0.001;   // GeV/c
    const double pmax = 1000000; // GeV/c
    double pbins[npbins + 1];    // edges
    for (int ip = 0; ip <= npbins; ++ip)
    {
        pbins[ip] = pmin * pow(pmax / pmin, (double)ip / npbins);
    }

    vector<double> pdeut; // Fill a vector with the central values of the bins
    for (int ip = 0; ip < npbins; ++ip)
    {
        pdeut.push_back(sqrt(pbins[ip] * pbins[ip + 1]));
    }

    // Apri il file utilizzando ifstream
    if (cs.empty())
    {
        ifstream file("cross-sections.txt");
        if (!file.is_open())
        {
            cerr << "Errore: impossibile aprire il file cross-sections.txt" << endl;
            return 1;
        }

        vector<pair<double, vector<double>>> temp_data; // Temporaneo per il riordino
        string line;
        // Leggi il file riga per riga
        while (getline(file, line))
        {
            stringstream ss(line);
            double plab_value;
            ss >> plab_value; // Estrae il primo valore della riga (p_lab)

            vector<double> cross_section_row;
            double value;
            while (ss >> value)
            {
                cross_section_row.push_back(value); // Legge tutti i valori rimanenti (cross sections)
            }

            temp_data.push_back(make_pair(plab_value, cross_section_row));
        }

        // Ordina in base a p_lab
        sort(temp_data.begin(), temp_data.end());

        // Popola i vettori ordinati
        for (const auto &entry : temp_data)
        {
            p_lab_grid_ordered.push_back(entry.first);
            cs.push_back(entry.second);
        }

        file.close(); // Chiude il file
    }

    // using the upper_bound function to find the index of the p_lab value in the ordered vector
    auto index_standard = upper_bound(p_lab_grid_ordered.begin(), p_lab_grid_ordered.end(), plab);
    int index_plab = index_standard - p_lab_grid_ordered.begin() - 1;
    if (verbose)
        cout << "Index plab: " << index_plab << endl;

    // same with the pdeut vector
    index_standard = upper_bound(pdeut.begin(), pdeut.end(), pd);
    int index_pdeut = index_standard - pdeut.begin() - 1;
    if (verbose)
        cout << "Index pdeut: " << index_pdeut << endl;

    if (plab > plab_thr)
    {
        double p_lab1 = p_lab_grid_ordered[index_plab];
        double p_lab2 = p_lab_grid_ordered[index_plab + 1];
        double p1 = pdeut[index_pdeut];
        double p2 = pdeut[index_pdeut + 1];
        double cs11 = cs[index_plab][index_pdeut];
        double cs12 = cs[index_plab][index_pdeut + 1];
        double cs21 = cs[index_plab + 1][index_pdeut];
        double cs22 = cs[index_plab + 1][index_pdeut + 1];
        CS = bilinear_interpolation(p_lab1, p_lab2, p1, p2, cs11, cs12, cs21, cs22, plab, pd, verbose);
        if (verbose)
        {
            cout << "p_lab1: " << p_lab1 << " p_lab2: " << p_lab2 << endl;
            cout << "p1: " << p1 << " p2: " << p2 << endl;
            cout << "cs11: " << cs11 << endl;
            cout << "CS: " << CS << endl;
        }
    }
    else
    {
        CS = 0;
        if (verbose)
            cout << "CS: " << CS << endl;
    }

    return CS;
}

double bilinear_interpolation(double plab1, double plab2, double p1, double p2, double cs11, double cs12, double cs21, double cs22, double plab, double pdeut, bool verbose)
{
    double logcs11 = (cs11 > 0) ? log10(cs11) : -100;
    double logcs12 = (cs12 > 0) ? log10(cs12) : -100;
    double logcs21 = (cs21 > 0) ? log10(cs21) : -100;
    double logcs22 = (cs22 > 0) ? log10(cs22) : -100;

    double cs = (logcs11 * (log10(plab2) - log10(plab)) * (log10(p2) - log10(pdeut)) +
                 logcs21 * (log10(plab) - log10(plab1)) * (log10(p2) - log10(pdeut)) +
                 logcs12 * (log10(plab2) - log10(plab)) * (log10(pdeut) - log10(p1)) +
                 logcs22 * (log10(plab) - log10(plab1)) * (log10(pdeut) - log10(p1))) /
                ((log10(plab2) - log10(plab1)) * (log10(p2) - log10(p1)));

    if (verbose)
        cout << "Interpolated CS (log): " << cs << endl;

    return pow(10., cs);
}
