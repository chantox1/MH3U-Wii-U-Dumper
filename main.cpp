#include <stdlib.h>
#include <iostream>
using namespace std;

#include <boost/program_options.hpp>
namespace po = boost::program_options;

#include "util.h"
#include "main.h"

bool findDMG(fs::path dmgDir,
            int i, bool exp, string lang,
            fs::path &dmgPath) {
    string fname;
    if (exp) {
        fname = dmgFilesBM[i] + "_Exp_" + lang + ".dmg";
    }
    else {
        fname = dmgFilesBM[i] + "_" + lang + ".dmg";
    }
    if (recursiveSearch(dmgDir, fname, dmgPath)) {
        return true;
    }
    cout << "Failed to find " << fname << "\n";
    return false;
}

bool openDMG(fs::path dmgDir,
             int i, bool exp, string lang,
             fs::ifstream &dmg) {
    fs::path dmgPath;
    if (findDMG(dmgDir, i, exp, lang, dmgPath)) {
        try {
            dmg.open(dmgPath, ifstream::binary);
            return true;
        }
        catch (const ifstream::failure &e) {
            cout << e.what();
            return false;
        }
    }
    return false;
}

bool openDMGFiles(fs::path dmgDir,
                  int i, string lang,
                  fs::ifstream &nameDMG, fs::ifstream &descDMG) {
    nameDMG.exceptions(ifstream::badbit);
    descDMG.exceptions(ifstream::badbit);
    if (openDMG(dmgDir, i, false, lang, nameDMG)
        && openDMG(dmgDir, i, true, lang, descDMG)) {
        return true;
    }
    return false;
}

bool readDMGHeader(fs::ifstream &dmg, int &strCount) {
    try {
        unsigned char buffer[4];
        dmg.seekg(0x14, ios::ios_base::beg);

        int entryCount;
        dmg.read((char *)buffer, sizeof(int));
        entryCount = int32BigEndian(buffer);

        dmg.read((char *)buffer, sizeof(int));
        strCount = int32BigEndian(buffer);

        int entryNameSize;
        dmg.read((char *)buffer, sizeof(int));
        entryNameSize = int32BigEndian(buffer);

        dmg.seekg(0x4, ios::ios_base::cur);

        int fnameLen;
        dmg.read((char *)buffer, sizeof(int));
        fnameLen = int32BigEndian(buffer);

        dmg.seekg(0x29 + fnameLen + 8*entryCount + entryNameSize,
            ios::ios_base::beg);
        return true;
    }
    catch (const ifstream::failure &e) {
        cout << e.what();
        return false;
    }
}

bool readHeaders(fs::ifstream &nameDMG, fs::ifstream &descDMG,
                 int &strCount) {
    int strCountName, strCountDesc;
    if (readDMGHeader(nameDMG, strCountName)
        && readDMGHeader(descDMG, strCountDesc)) {
        if (strCountName == strCountDesc) {
            strCount = strCountName;
            return true;
        }
        cout << "Unequal string count in DMG files\n";
        return false;
    }
    return false;
}

int main(int ac, char *av[]) {
    try {
        po::options_description desc("Allowed options");
        desc.add_options()
            ("help", "Display this message")
            ("lang", po::value<string>()->default_value("eng"),
                "Set dmg file language")
            ("w", "Dump weapon data")
            ("o", po::value<string>()->default_value("Out"),
                "Set output folder")
            ("rpxPath", po::value<string>(),
                "Set executable path (first positional argument)")
            ("dmgDir", po::value<string>(),
                "Set dmg folder path (second positional argument)")
            ;

        po::positional_options_description pd;
        pd.add("rpxPath", 1);
        pd.add("dmgDir", 1);

        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).
            options(desc).positional(pd).run(), vm);
        po::notify(vm);

        if (vm.count("help")) {
            cout << desc << "\n";
            return 0;
        }

        if (!vm.count("rpxPath") || !vm.count("dmgDir")) {
            cout << "Please provide the path to the extracted executable"
                << " and the directory containing the needed dmg files.\n";
            return -1;
        }
        fs::path rpxPath(vm["rpxPath"].as<string>());
        fs::path dmgDir(vm["dmgDir"].as<string>());
        if (!existsAndIsFile(rpxPath) || !existsAndIsDir(dmgDir)) {
            return -1;
        }
        fs::ifstream rpx(rpxPath, ios::binary);
        string lang = vm["lang"].as<string>();
        if (vm.count("w")) {
            fs::ifstream nameDMG;
            fs::ifstream descDMG;

            fs::path outDir(vm["o"].as<string>());
            if (!fs::exists(outDir)) {
                if (fs::create_directory(outDir)) {
                    cout << "Created directory " << outDir << endl;
                }
            }

            for (int i=0; i < 9; i++) {
                if (!openDMGFiles(dmgDir, i, lang, nameDMG, descDMG)) {
                    return -1;
                }
                int strCount;
                if (!readHeaders(nameDMG, descDMG, strCount)) {
                    return -1;
                }
                fs::path outPath(vm["o"].as<string>());
                outPath /= wepNamesBM[i] + ".txt";
                fs::ofstream out;
                out.open(outPath, ios::trunc);
                if (!existsAndIsFile(outPath)) {
                    return -1;
                }

                rpx.seekg(wepOffsetsBM[i], ios::ios_base::beg);
                string name;
                string desc;
                WeaponDataBM data;
                for (int j=0; j < strCount; j++) {
                    getline(nameDMG, name, '\0');
                    out << name << endl;
                    getline(descDMG, desc, '\0');
                    out << desc << endl;
                    rpx.read((char *)&data, sizeof(WeaponDataBM));
                    out << "RARE-" << data.rarity + 1 << endl
                        << "Attack: " << int16BigEndian(data.raw) << endl
                        << "Model id:" << int16BigEndian(data.model) << endl
                        << endl;
                }
                nameDMG.close();
                descDMG.close();
                out.close();
            }
        }
    }
    catch (exception &e) {
        cout << e.what() << "\n";
        return -1;
    }

    return 0;
}
