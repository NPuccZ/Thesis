import sys
sys.path.append('/usr/local/Cellar/root/6.30.06/lib/root/')
import ROOT
import glob
import re

# Directory contenente i file .root
directory = "/Users/bernardopuccetti/Desktop/GRIGLIA_Argonne/"  # Modifica questo percorso alla tua directory

# Trova tutti i file .root nella directory e ordina alfanumericamente
root_files = glob.glob(f"{directory}/*.root")

# File di output per i contenuti dei bin
output_bin_contents = "cross-sections.txt"

# Apri il file di output in modalità scrittura
with open(output_bin_contents, "w") as bin_contents_file:
    # Processa ogni file .root
    for root_file in root_files:
        print(f"Processing file: {root_file}")
        
        # Estrai il numero dal nome del file
        match = re.search(r"(\d+)GeV\.root", root_file)
        if match:
            plab_value = match.group(1)  # Estrai il valore numerico dal nome del file
        else:
            print(f"Nome del file non conforme al formato atteso: {root_file}")
            continue
        
        # Apri il file ROOT
        file = ROOT.TFile.Open(root_file)
        
        # Leggi l'istogramma (sostituisci 'Cross-section for Antideuterons_px' con il nome del tuo istogramma)
        histogram = file.Get("Cross-section for Antideuterons_px")
        
        # Verifica che l'istogramma esista
        if not histogram:
            print(f"L'istogramma non è stato trovato nel file {root_file}.")
            continue
        
        # Estrai i contenuti dei bin e i centri dei bin
        n_bins = histogram.GetNbinsX()
        bin_contents = [histogram.GetBinContent(i) for i in range(1, n_bins + 1)]
        bin_centers = [histogram.GetBinCenter(i) for i in range(1, n_bins + 1)]
        
        # Debug: stampa i contenuti dei bin
        print(f"Bin contents for file {root_file}: {bin_contents}")
        
        # Scrivi i valori dei bin nel file di contenuti dei bin, con il plab_value come primo valore
        bin_contents_file.write(f"{plab_value} " + " ".join(map(str, bin_contents)))
        bin_contents_file.write("\n")
        
        # Chiudi il file ROOT
        file.Close()

print(f"Bin contents written to {output_bin_contents}")
