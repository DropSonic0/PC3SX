/*
 * FileBrowser.cpp
 *
 *  Created on: Oct 29, 2010
 *      Author: halsafar
 */
#include "FileBrowser.h"

#include <algorithm>


FileBrowser::FileBrowser(string startDir)
{
	_currentSelected = 0;
	_dir.numEntries = 0;

	PushDirectory(startDir, CELL_FS_TYPE_DIRECTORY | CELL_FS_TYPE_REGULAR, "");
}


FileBrowser::FileBrowser(string startDir, string extensions)
{
	_currentSelected = 0;
	_dir.numEntries = 0;

	PushDirectory(startDir, CELL_FS_TYPE_DIRECTORY | CELL_FS_TYPE_REGULAR, extensions);
}


FileBrowser::FileBrowser(string startDir, int types, string extensions)
{
	_currentSelected = 0;
	_dir.numEntries = 0;

	PushDirectory(startDir, types, extensions);
}



FileBrowser::~FileBrowser()
{
	Destroy();
}


void FileBrowser::Destroy()
{
	// clean up
	DeleteCurrentEntrySet();

    while (!_dirStack.empty())
    {
    	_dirStack.pop();
    }
}


void FileBrowser::DeleteCurrentEntrySet()
{
    std::vector<DirectoryEntry*>::const_iterator iter;
    for(iter = _cur.begin(); iter != _cur.end(); ++iter)
    {
        delete (*iter);
    }

    _cur.clear();
}


DirectoryInfo FileBrowser::GetCurrentDirectoryInfo()
{
	return _dir;
}


//FIXME: so error prone. reason: app dependent, we return NULL for cur entry in this case
void FileBrowser::GotoEntry(uint32_t i)
{
	_currentSelected = i;
}


uint32_t FileBrowser::DirectoryStackCount()
{
	return _dirStack.size();
}


void FileBrowser::PushDirectory(string path, int types, string extensions)
{
	if (path.compare("..") == 0)
	{
		PopDirectory();
	}
	else if (ParseDirectory(path, types, extensions))
	{
		_dirStack.push(_dir);
	}
}


void FileBrowser::PopDirectory()
{
	if (_dirStack.empty())
	{
		return;
	}

	_dirStack.pop();

	if (_dirStack.empty())
	{
		return;
	}

	if (ParseDirectory(_dirStack.top().dir, _dirStack.top().types, _dirStack.top().extensions))
	{
		return;
	}
	else
	{
		PopDirectory();
	}
}


bool FileBrowser::ParseDirectory(string path, int types, string extensions)
{
	// for extension parsing
	uint32_t index = 0;
	uint32_t lastIndex = 0;

	// bad path
	if (path.empty())
	{
		return false;
	}

	// delete old path
	if (!_cur.empty())
	{
		DeleteCurrentEntrySet();
	}

	// FIXME: add FsStat calls or use cellFsDirectoryEntry
	if (cellFsOpendir(path.c_str(), &_fd) == CELL_FS_SUCCEEDED)
	{
		uint64_t nread = 0;

		// set new dir
		_dir.dir = path;
		_dir.extensions = extensions;
		_dir.types = types;

		// reset num entries
		_dir.numEntries = 0;

		// reset cur selected for safety FIXME: side effect?
		_currentSelected = 0;

		// read the directory
		DirectoryEntry dirent;
		while (cellFsReaddir(_fd, &dirent, &nread) == CELL_FS_SUCCEEDED)
		{
			// no data read, something is wrong... FIXME: bad way to handle this
			if (nread == 0)
			{
				break;
			}

			// check for valid types
			if (dirent.d_type != (types & CELL_FS_TYPE_REGULAR)  &&
				dirent.d_type != (types & CELL_FS_TYPE_DIRECTORY))
			{
				continue;
			}

			// skip cur dir
			if (dirent.d_type == CELL_FS_TYPE_DIRECTORY &&
				!(strcmp(dirent.d_name, ".")))
			{
				continue;
			}

			// validate extensions
			if (dirent.d_type == CELL_FS_TYPE_REGULAR)
			{
				// reset indices from last search
				index = 0;
				lastIndex = 0;

				// get this file extension
				string ext = FileBrowser::GetExtension(dirent.d_name);

				// assume to skip it, prove otherwise
				bool bSkip = true;

				// build the extensions to compare against
				if (extensions.size() > 0)
				{
					index = extensions.find('|', 0);

					// only 1 extension
					if (index == string::npos)
					{
						if (ext.compare(extensions.substr(0, extensions.length())) == 0)
						{
							bSkip = false;
						}
					}
					else
					{
						lastIndex = 0;
						index = extensions.find('|', 0);
						string tmp;
						while (index != string::npos)
						{
							tmp = extensions.substr(lastIndex, (index-lastIndex));
							if (ext.compare(tmp) == 0)
							{
								bSkip = false;
								break;
							}

							lastIndex = index + 1;
							index = extensions.find('|', index+1);
						}

						// grab the final extension
						tmp = extensions.substr(lastIndex);
						if (ext.compare(tmp) == 0)
						{
							bSkip = false;
						}
					}
				}
				else
				{
					// no extensions we'll take as all extensions
					bSkip = false;
				}

				if (bSkip)
				{
					continue;
				}
			}

			// AT THIS POINT WE HAVE A VALID ENTRY

			// alloc an entry
			DirectoryEntry *entry = new DirectoryEntry();
			memcpy(entry, &dirent, sizeof(DirectoryEntry));

			_cur.push_back(entry);

			// next file
			++_dir.numEntries;

			// FIXME: hack, implement proper caching + paging
		}

		cellFsClosedir(_fd);
	}
	else
	{
		return false;
	}

	// FIXME: hack, forces '..' to stay on top by ignoring the first entry
	// this is always '..' in dirs
	std::sort(++_cur.begin(), _cur.end(), less_than_key());

	return true;
}


void FileBrowser::IncrementEntry()
{
	_currentSelected++;
	if (_currentSelected >= _cur.size())
	{
		_currentSelected = 0;
	}
}


void FileBrowser::DecrementEntry()
{
	_currentSelected--;
	if (_currentSelected >= _cur.size())
	{
		_currentSelected = _cur.size() - 1;
	}
}


DirectoryEntry* FileBrowser::GetCurrentEntry()
{
	if (_currentSelected >= _cur.size())
	{
		return NULL;
	}

	return _cur[_currentSelected];
}


uint32_t FileBrowser::GetCurrentEntryIndex()
{
	return _currentSelected;
}


string FileBrowser::GetExtension(string filename)
{
	uint32_t index = filename.find_last_of(".");
	if (index != string::npos)
	{
		return filename.substr(index+1);
	}

	return "";
}


bool FileBrowser::IsCurrentAFile()
{
	if (_currentSelected >= _cur.size())
	{
		return false;
	}

	return _cur[_currentSelected]->d_type == CELL_FS_TYPE_REGULAR;
}


bool FileBrowser::IsCurrentADirectory()
{
	if (_currentSelected >= _cur.size())
	{
		return false;
	}

	return _cur[_currentSelected]->d_type == CELL_FS_TYPE_DIRECTORY;
}


/*
 *

				// compare against wanted extensions
				bool skip = true;
				string ext = FileBrowser::GetExtension(dirent.d_name);
				for (int i = 0; i < exts.size(); i++)
				{
					if (ext.compare(exts[i]) == 0)
					{
						skip = false;
						break;
					}
				}*/

