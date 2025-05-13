/*
 * FileBrowser.h
 *
 *  Created on: Oct 29, 2010
 *      Author: halsafar
 */

#ifndef FILEBROWSER_H_
#define FILEBROWSER_H_

#define MAXJOLIET 255

#include <string>
#include <vector>
#include <stack>

#include <sys/types.h>

#include <cell/cell_fs.h>

using namespace std;


//FIXME: shouldnt need this, ps3 has its own CellFsDirEnt and CellFsDirectoryEntry
//		-- the latter should be switched to eventually
typedef struct
{
        string dir;
        string extensions;
        int types;
        uint32_t numEntries;
        int size;
} DirectoryInfo;

/*
typedef struct
{
        size_t length; // file length
        time_t mtime; // file modified time
        int isdir; // 0 - file, 1 - directory
        CellFsDirent dirent;
        char filename[MAXJOLIET + 1]; // full filename
        char displayname[MAXJOLIET + 1]; // name for browser display
        int filenum; // file # (for 7z support)
        int icon; // icon to display
} BrowserEntry;
*/


typedef CellFsDirent DirectoryEntry;


struct less_than_key
{
	// yeah sucks, not using const
    inline bool operator() (DirectoryEntry* a, DirectoryEntry* b)
    {
    	// dir compare to file, let us always make the dir less than
    	if ((a->d_type == CELL_FS_TYPE_DIRECTORY && b->d_type == CELL_FS_TYPE_REGULAR))
    	{
    		return true;
    	}
    	else if (a->d_type == CELL_FS_TYPE_REGULAR && b->d_type == CELL_FS_TYPE_DIRECTORY)
    	{
    		return false;
    	}

    	// FIXME: add a way to customize sorting someday
    	// 	also add a ignore filename, sort by extension

    	// use this to ignore extension
    	if (a->d_type == CELL_FS_TYPE_REGULAR && b->d_type == CELL_FS_TYPE_REGULAR)
    	{
			char *pIndex1 = (char *)strrchr(a->d_name, '.');
			char *pIndex2 = (char *)strrchr(b->d_name, '.');

			// null the dots
			if (pIndex1 != NULL)
			{
				*pIndex1 = '\0';
			}

			if (pIndex2 != NULL)
			{
				*pIndex2 = '\0';
			}

			// compare
			int retVal = strcasecmp(a->d_name, b->d_name);

			// restore the dot
			if (pIndex1 != NULL)
			{
				*pIndex1 = '.';
			}

			if (pIndex2 != NULL)
			{
				*pIndex2 = '.';
			}
			return retVal < 0;
    	}

    	// both dirs at this points btw
    	return strcasecmp(a->d_name, b->d_name) < 0;
    }
};


class FileBrowser
{
public:
	FileBrowser(string startDir);
	FileBrowser(string startDir, string extensions);
	FileBrowser(string startDir, int types, string extensions);
	~FileBrowser();

	void Destroy();

	DirectoryEntry* GetCurrentEntry();
	uint32_t GetCurrentEntryIndex();
	static string GetExtension(string filename);

	bool IsCurrentAFile();
	bool IsCurrentADirectory();

	void IncrementEntry();
	void DecrementEntry();
	void GotoEntry(uint32_t i);

	DirectoryInfo GetCurrentDirectoryInfo();

	void PushDirectory(string path, int types, string extensions);
	void PopDirectory();
	uint32_t DirectoryStackCount();

	DirectoryEntry* operator[](uint32_t i)
	{
		return _cur[i];
	}
private:
	// currently select browser entry
	uint32_t _currentSelected;

	// current file descriptor, used for reading entries
	int _fd;

	// info of the current directory
	DirectoryInfo _dir;

	// current file listing
	vector<DirectoryEntry*> _cur;

	// dir stack for ez traversal
	stack<DirectoryInfo> _dirStack;

	bool ParseDirectory(string path, int types, string extensions);
	void DeleteCurrentEntrySet();
};


#endif /* FILEBROWSER_H_ */
