#ifdef USE_PCH
#include "rr_pch.h"
#endif
#pragma hdrstop
#include <sstream>
#include <iomanip>
#include "Args.h"
//---------------------------------------------------------------------------
#if defined(__CODEGEAR__)
#pragma package(smart_init)
#endif

using namespace std;
Args::Args()
:
SBMLModelsFilePath(""),
ResultOutputFile(""),
TempDataFolder("."),
CompilerLocation("."),
SupportCodeFolder(".")
{}

string Usage(const string& prg)
{
    stringstream usage;
    usage << "\nUSAGE for "<<prg<<"\n\n";
    usage<<left;
    usage<<setfill('.');
    usage<<setw(25)<<"-m<FilePath>"                 <<" Folder from where the program will read sbml models. \n";
	usage<<setw(25)<<"-l<FilePath>"                 <<" Compiler location\n";
	usage<<setw(25)<<"-s<FilePath>"                 <<" Support code folder llocation\n";
	usage<<setw(25)<<"-r<FilePath>"                 <<" Results output file, with path.\n";
    usage<<setw(25)<<"-t<FilePath>"                 <<" Temporary data output folder. If not given, temp files are output to current directory\n";

    usage<<setw(25)<<"-?"                           <<" Shows the help screen.\n\n";

    usage<<"\nSystems Biology, UW 2012\n";
    return usage.str();
}
