/* Plugin Organizer v 1.0. (C) SocioPart 2020 */
#include <Windows.h>
#define WIN32_LEAN_AND_MEAN 
#include <string>
#pragma warning(disable : 4996)

#define PATH_LENGTH MAX_PATH //MAX_PATH = 260. Increase this if you need large strings.
// Variables
WIN32_FIND_DATA fd;

std::string defaultDirectory;  // Start directory
std::string mainIniDir;		   // Directory of main INI file
std::string configIniFileMask; // Temporary mask for reading config INI
std::string configIniDir;	   // Config INI directory
std::string currentProfileMask; 
std::string profileTempMask;
std::string profileProcMask;
bool bEnableLoader;
bool bLoadFolders;
bool bLoadASI;
bool bLoadDLL;
short iCurrentConfig;
int iProfiles;
bool bProfileEnabled;

// Functions
std::string GetCurrentDirectoryString();
std::string GetIniFileString();

void MainIniInit();
void LoadProfiles();
void ProcessLoadingProfile(int iProfileIndex);
void LoadFolder(std::string folderDirectory);
void FindFiles(WIN32_FIND_DATA* fd);


std::string GetCurrentDirectoryString(){
	char buffer [PATH_LENGTH];
	GetCurrentDirectory(PATH_LENGTH, buffer);
	return std::string(buffer);
}
std::string GetIniFileString(LPCSTR lpAppName, LPCSTR lpKeyName, LPCSTR lpDefault, LPCSTR lpFileName){
	char buffer [PATH_LENGTH];
	GetPrivateProfileStringA(lpAppName, lpKeyName, lpDefault, buffer, PATH_LENGTH, lpFileName);
	if (strrchr(buffer,'\\')) *(char*)(strrchr(buffer,'\\')) = '\0';
	return std::string(buffer);
}

BOOL APIENTRY DllMain( HMODULE hModule, DWORD  reasonForCall, LPVOID lpReserved)
{
	if ( reasonForCall == DLL_PROCESS_ATTACH )
	{
		MainIniInit();
		if (bEnableLoader){
			if (!bLoadFolders) LoadFolder("plugins\\");
			else if (iProfiles !=-1) LoadProfiles();
		SetCurrentDirectory("\\..\\");
		}
	}
    return TRUE;
}

void MainIniInit(){
	std::string configFileName;
	
	defaultDirectory = GetCurrentDirectoryString();
	mainIniDir = defaultDirectory + "\\plugorg.ini";
	// Parsing main INI file
	bEnableLoader   = GetPrivateProfileInt("GLOBAL SETTINGS", "Enable",       TRUE, mainIniDir.c_str());
	bLoadFolders    = GetPrivateProfileInt("GLOBAL SETTINGS", "LoadFolders", FALSE, mainIniDir.c_str());
	bLoadASI        = GetPrivateProfileInt("GLOBAL SETTINGS", "LoadASI",      TRUE, mainIniDir.c_str());
	bLoadDLL        = GetPrivateProfileInt("GLOBAL SETTINGS", "LoadDLL",     FALSE, mainIniDir.c_str());
	iCurrentConfig  = GetPrivateProfileInt("CONFIGS", "CurrentConfigFile",   FALSE, mainIniDir.c_str());

	configIniFileMask = "Config_File_" + std::to_string(iCurrentConfig);	
	configFileName = GetIniFileString("CONFIGS", configIniFileMask.c_str(), "\\plugorg.ini", mainIniDir.c_str());
	configIniDir = defaultDirectory + "\\" + configFileName;
	iProfiles = GetPrivateProfileInt("AMOUNT OF PROFILES", "Profiles", -1, configIniDir.c_str());
}

void LoadProfiles(){
	for (int iCurrProfileIndex=1; iCurrProfileIndex <= iProfiles; ++iCurrProfileIndex)
	{	
			currentProfileMask = "Enable_Profile_" + std::to_string(iCurrProfileIndex);
			bProfileEnabled = GetPrivateProfileInt("PROFILE SETTINGS", currentProfileMask.c_str(), FALSE, configIniDir.c_str());
			if (bProfileEnabled == 1)ProcessLoadingProfile(iCurrProfileIndex); // Loading profile N
	}

}
void ProcessLoadingProfile(int iProfileIndex){
	int iNumberOfFolders;
	int iFolderIndex;
	std::string currentFolder;
	profileTempMask = "Profile_" + std::to_string(iProfileIndex) + "_NumberOfFolders";
	iNumberOfFolders = GetPrivateProfileInt("PROFILES", profileTempMask.c_str(), FALSE, configIniDir.c_str());
	
	profileTempMask = "Profile_" + std::to_string(iProfileIndex) + "_Path_";
	if (iNumberOfFolders !=0)
	{
		for (iFolderIndex = 1; iFolderIndex <= iNumberOfFolders; ++iFolderIndex)
		{
			profileProcMask = profileTempMask + std::to_string(iFolderIndex);
			currentFolder = GetIniFileString("PROFILES", profileProcMask.c_str(), NULL, configIniDir.c_str());
			if (currentFolder !="") LoadFolder(currentFolder); 
		}
	}
}
void LoadFolder(std::string folderDirectory)
{      
	if (SetCurrentDirectory(folderDirectory.c_str())){
		FindFiles(&fd);
		SetCurrentDirectory(defaultDirectory.c_str());
	}	
}
void FindFiles(WIN32_FIND_DATA* fd) {
	if (bLoadASI == 1){
		HANDLE asiFile = FindFirstFile ("*.asi", fd);
		if (asiFile != INVALID_HANDLE_VALUE)
		{

			do {
				if (!(fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

					unsigned int pos = 5;
					while (fd->cFileName[pos])
						++pos;
					if (fd->cFileName[pos-4] == '.' &&
						(fd->cFileName[pos-3] == 'a' || fd->cFileName[pos-3] == 'A') &&
						(fd->cFileName[pos-2] == 's' || fd->cFileName[pos-2] == 'S') &&
						(fd->cFileName[pos-1] == 'i' || fd->cFileName[pos-1] == 'I'))
					{
							LoadLibrary (fd->cFileName);
					}
				}

			} while (FindNextFile (asiFile, fd));
			FindClose (asiFile);
		}
	}
// ======================================================
		if (bLoadDLL == 1){
		HANDLE dllFile = FindFirstFile ("*.dll", fd);
		if (dllFile != INVALID_HANDLE_VALUE)
		{

			do {
				if (!(fd->dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {

					unsigned int pos = 5;
					while (fd->cFileName[pos])
						++pos;
					if (fd->cFileName[pos-4] == '.' &&
						(fd->cFileName[pos-3] == 'd' || fd->cFileName[pos-3] == 'D') &&
						(fd->cFileName[pos-2] == 'l' || fd->cFileName[pos-2] == 'L') &&
						(fd->cFileName[pos-1] == 'l' || fd->cFileName[pos-1] == 'L'))
					{
							LoadLibrary (fd->cFileName);
					}
				}

			} while (FindNextFile (dllFile, fd));
			FindClose (dllFile);
		}
	}
}