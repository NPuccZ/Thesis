#include <iostream>
#include <fstream>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <cmath>
#include <algorithm>

using namespace std;

double bilinear_interpolation(double plab1, double plab2, double p1, double p2, double cs11, double cs12, double cs21, double cs22, double plab, double pdeut);
double outCS(double plab, double pd);

int main(int argc, char *argv[])
{
    double plab = atof(argv[1]);
    double pd = atof(argv[2]);
    if (argc != 3)
    {
        cerr << "Errore: numero di argomenti non valido." << endl;
        return 1;
    }
    double CS = 0;
    cout << "I valori immessi sono i seguenti plab pd: " << plab << " " << pd << endl;
    CS = outCS(plab, pd); 

}

double outCS(double plab, double pd)
{
    double CS = 0;
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
        pdeut.push_back(0.5 * (pbins[ip] + pbins[ip + 1]));
    }

    // Apri il file utilizzando ifstream
    ifstream file("cross-sections.txt");
    if (!file.is_open())
    {
        cerr << "Errore: impossibile aprire il file cross-sections.txt" << endl;
        return 1;
    }

    vector<vector<double>> cs; // 2D vector to store the cross sections
    vector<double> p_lab_grid; // Vector to store the p_lab values
    string line;
    // Leggi il file riga per riga
    while (getline(file, line))
    {
        stringstream ss(line);
        double plab_value;
        ss >> plab_value; // Estrae il primo valore della riga (p_lab)
        p_lab_grid.push_back(plab_value);

        vector<double> cross_section_row;
        double value;
        while (ss >> value)
        {
            cross_section_row.push_back(value); // Legge tutti i valori rimanenti (cross sections)
        }
        cs.push_back(cross_section_row);
    }

    // order the vector p_lab_grid and accordingly the index of the cross sections
    vector<double> p_lab_grid_ordered = p_lab_grid;
    sort(p_lab_grid_ordered.begin(), p_lab_grid_ordered.end());
    vector<vector<double>> cs_ordered;
    // fill the ordered cross sections vector
    for (size_t i = 0; i < p_lab_grid_ordered.size(); ++i)
    {
        vector<double> cross_section_row;
        for (size_t j = 0; j < p_lab_grid.size(); ++j)
        {
            if (p_lab_grid[j] == p_lab_grid_ordered[i])
            {
                cross_section_row = cs[j];
                break;
            }
        }
        cs_ordered.push_back(cross_section_row);
    }

    // using the upper_bound function to find the index of the p_lab value in the ordered vector
    vector<double>::const_iterator index_standard;
    index_standard = upper_bound(p_lab_grid_ordered.begin(), p_lab_grid_ordered.end(), plab);
    int index_plab = index_standard - p_lab_grid_ordered.begin() - 1;
    cout << "Index plab: " << index_plab << endl;

    // same with the pdeut vector
    index_standard = upper_bound(pdeut.begin(), pdeut.end(), pd);
    int index_pdeut = index_standard - pdeut.begin() - 1;
    cout << "Index pdeut: " << index_pdeut << endl;
    if (plab > 17)
    {
        double p_lab1 = p_lab_grid_ordered[index_plab];
        double p_lab2 = p_lab_grid_ordered[index_plab + 1];
        double p1 = pdeut[index_pdeut];
        double p2 = pdeut[index_pdeut + 1];
        double cs11 = cs_ordered[index_plab][index_pdeut];
        double cs12 = cs_ordered[index_plab][index_pdeut + 1];
        double cs21 = cs_ordered[index_plab + 1][index_pdeut];
        double cs22 = cs_ordered[index_plab + 1][index_pdeut + 1];
        CS = bilinear_interpolation(p_lab1, p_lab2, p1, p2, cs11, cs12, cs21, cs22, plab, pd);
        cout << "p_lab1: " << p_lab1 << " p_lab2: " << p_lab2 << endl;
        cout << "p1: " << p1 << " p2: " << p2 << endl;
        cout << "cs11: " << cs11 << endl;
        cout << "CS: " << CS << endl;
    } else{ 
        CS = 0;
        cout << "CS: " << CS << endl;
    }

    file.close(); // Chiude il file
    return CS;
}

double bilinear_interpolation(double plab1, double plab2, double p1, double p2, double cs11, double cs12, double cs21, double cs22, double plab, double pdeut)
{
    double logcs11 = (cs11 > 0) ? log10(cs11) : -100;

    double cs = (logcs11 * (log10(plab2) - log10(plab)) * (log10(p2) - log10(pdeut)) +
                 log10(cs21) * (log10(plab) - log10(plab1)) * (log10(p2) - log10(pdeut)) +
                 log10(cs12) * (log10(plab2) - log10(plab)) * (log10(pdeut) - log10(p1)) +
                 log10(cs22) * (log10(plab) - log10(plab1)) * (log10(pdeut) - log10(p1))) /
                ((log10(plab2) - log10(plab1)) * (log10(p2) - log10(p1)));

    return pow(10., cs);
}
