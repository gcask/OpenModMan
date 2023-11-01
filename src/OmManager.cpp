/*
  This file is part of Open Mod Manager.

  Open Mod Manager is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  Open Mod Manager is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with Open Mod Manager. If not, see <http://www.gnu.org/licenses/>.
*/
#include <ctime>

#include "OmBaseWin.h"
#include <ShlObj.h>

#include "OmBaseApp.h"

#include "OmUtilFs.h"
#include "OmUtilStr.h"
#include "OmUtilHsh.h"
#include "OmUtilDlg.h"
#include "OmUtilErr.h"
#include "OmUtilSys.h"

#include "OmDialog.h"
#include "OmPackage.h"

///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
#include "OmManager.h"


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmManager::OmManager() :
  _home(), _config(), _modHubLs(), _modHubSl(-1), _iconsSize(16), _folderPackages(true),
  _warnEnabled(true), _warnOverlaps(false), _warnExtraInstall(true),
  _warnMissingDepend(true), _warnExtraUninst(true), _quietBatches(true), _noMarkdown(false),
  _error(), _log(), _logHwnd(nullptr), _logFile(nullptr)
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmManager::~OmManager()
{
  for(size_t i = 0; i < this->_modHubLs.size(); ++i)
    delete this->_modHubLs[i];

  // close log file
  if(this->_logFile) {
    CloseHandle(this->_logFile);
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmManager::init(const char* arg)
{
  // Create application folder if does not exists
  wchar_t psz_path[MAX_PATH];
  SHGetFolderPathW(nullptr, CSIDL_APPDATA, nullptr, 0, psz_path);
  this->_home = psz_path; this->_home.append(L"\\");
  this->_home.append(OMM_APP_NAME);

  // for application home directory creation result
  int result;

  // try to create directory (this should work)
  if(!Om_isDir(this->_home)) {

    result = Om_dirCreate(this->_home);
    if(result != 0) {
      this->_error = Om_errCreate(L"Application home folder", this->_home, result);
      Om_dlgBox_err(L"Initialization", L"Manager initialization failed", this->_error);
      return false;
    }
  }

  // initialize log file
  wstring log_path = this->_home + L"\\log.txt";

  // rename previous log file if exists
  if(Om_pathExists(log_path))
    Om_fileMove(log_path, this->_home + L"\\log.old.txt");

  this->_logFile = CreateFileW(log_path.c_str(), GENERIC_WRITE, FILE_SHARE_READ, nullptr,
                          CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, nullptr);
  // first log line
  this->log(2, L"Manager() Init", L"Start");

  // Load existing configuration or create a new one
  if(!this->_config.open(this->_home + L"\\config.xml", OMM_XMAGIC_APP)) {

    this->log(2, L"Manager() Init", L"Create new configuration file");

    wstring conf_path = this->_home + L"\\config.xml";

    if(!_config.init(conf_path, OMM_XMAGIC_APP)) {
      // this is not a fatal error, but this will surely be a problem...
      wstring msg = Om_errInit(L"Configuration file", conf_path, this->_config.lastErrorStr());
      this->log(1, L"Manager() Init", msg);
      Om_dlgBox_wrn(L"Initialization", L"Manager initialization error", msg);
    }

    // default icons size
    this->setIconsSize(this->_iconsSize);
  }

  // migrate config file
  this->_migrate();

  // load saved parameters
  if(this->_config.xml().hasChild(L"icon_size")) {
    this->_iconsSize = this->_config.xml().child(L"icon_size").attrAsInt(L"pixels");
  }

  // load saved no-markdown option
  if(this->_config.xml().hasChild(L"no_markdown")) {
    this->_noMarkdown = this->_config.xml().child(L"no_markdown").attrAsInt(L"enable");
  }

  // add the context file passed as argument if any
  if(strlen(arg)) {

    // convert to wstring
    wstring path;
    Om_fromAnsiCp(&path, arg);

    // check for quotes and removes them
    if(path.back() == L'"' && path.front() == L'"') {
      path.erase(0, 1);
      path.pop_back();
    }

    // try to open
    if(!this->modHubLoad(path)) {
      Om_dlgBox_err(L"Open Mod Hub", L"Mod Hub \""+path+
                    L"\" loading failed because of the following error:",
                    this->lastError());
    }
  }

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmManager::quit()
{
  this->log(2, L"Manager() Quit", L"");

  for(size_t i = 0; i < this->_modHubLs.size(); ++i)
    delete this->_modHubLs[i];
  this->_modHubLs.clear();

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::saveWindowRect(const RECT& rect)
{
  if(this->_config.valid()) {

    OmXmlNode window;
    if(this->_config.xml().hasChild(L"window")) {
      window = this->_config.xml().child(L"window");
    } else {
      window = this->_config.xml().addChild(L"window");
    }

    window.setAttr(L"left", static_cast<int>(rect.left));
    window.setAttr(L"top", static_cast<int>(rect.top));
    window.setAttr(L"right", static_cast<int>(rect.right));
    window.setAttr(L"bottom", static_cast<int>(rect.bottom));

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::loadWindowRect(RECT& rect)
{
  if(this->_config.valid()) {

    if(this->_config.xml().hasChild(L"window")) {

      OmXmlNode window = this->_config.xml().child(L"window");

      rect.left = window.attrAsInt(L"left");
      rect.top = window.attrAsInt(L"top");
      rect.right = window.attrAsInt(L"right");
      rect.bottom = window.attrAsInt(L"bottom");
    }
  }
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::saveWindowFoot(int h)
{
  if(this->_config.valid()) {

    OmXmlNode window;
    if(this->_config.xml().hasChild(L"window")) {
      window = this->_config.xml().child(L"window");
    } else {
      window = this->_config.xml().addChild(L"window");
    }

    window.setAttr(L"foot", h);

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::loadWindowFoot(int* h)
{
  if(this->_config.valid()) {

    if(this->_config.xml().hasChild(L"window")) {

      OmXmlNode window = this->_config.xml().child(L"window");

      *h = window.attrAsInt(L"foot");
    }
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::saveRecentFile(const wstring& path)
{
  if(this->_config.valid()) {

    OmXmlNode recent_list;
    if(this->_config.xml().hasChild(L"recent_list")) {
      recent_list = this->_config.xml().child(L"recent_list");
    } else {
      recent_list = this->_config.xml().addChild(L"recent_list");
    }

    // get current <path> child entries in <recent_list>
    vector<OmXmlNode> home_ls;
    recent_list.children(home_ls, L"home");

    for(size_t i = 0; i < home_ls.size(); ++i) {
      if(path == home_ls[i].content()) {
        recent_list.remChild(home_ls[i]);
        break;
      }
    }

    // now verify the count does not exceed the limit
    if(recent_list.childCount() > (OM_MANAGER_MAX_RECENT + 1)) {
      // remove the oldest entry to keep max entry count
      recent_list.remChild(recent_list.child(L"home",0));
    }

    // append path to end of list, for most recent one
    recent_list.addChild(L"home").setContent(path);

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::clearRecentFiles()
{
  if(this->_config.valid()) {

    OmXmlNode recent_list;
    if(this->_config.xml().hasChild(L"recent_list")) {
      recent_list = this->_config.xml().child(L"recent_list");
    } else {
      recent_list = this->_config.xml().addChild(L"recent_list");
    }

    this->_config.xml().remChild(recent_list);

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::loadRecentFiles(vector<wstring>& paths)
{
  if(this->_config.valid()) {

    if(this->_config.xml().hasChild(L"recent_list")) {

      OmXmlNode recent_list = this->_config.xml().child(L"recent_list");

      paths.clear();

      // retrieve all <path> child in <recent_list>
      vector<OmXmlNode> home_ls;
      recent_list.children(home_ls, L"home");

      // verify each entries and remove ones which are no longer valid path
      for(size_t i = 0; i < home_ls.size(); ++i) {
        if(!Om_isDir(home_ls[i].content())) {
          recent_list.remChild(home_ls[i]);
        }
      }

      // retrieve (again) all <path> child in <recent_list> and fill path list
      home_ls.clear();
      recent_list.children(home_ls, L"home");
      for(size_t i = 0; i < home_ls.size(); ++i) {
        paths.push_back(home_ls[i].content());
      }

    }
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::saveDefaultLocation(const wstring& path)
{
  if(this->_config.valid()) {
    if(this->_config.xml().hasChild(L"default_location")) {
      this->_config.xml().child(L"default_location").setContent(path);
    } else {
      this->_config.xml().addChild(L"default_location").setContent(path);
    }
    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::getDefaultLocation(wstring& path)
{
  if(this->_config.valid()) {
    if(this->_config.xml().hasChild(L"default_location")) {
      path = this->_config.xml().child(L"default_location").content();
    } else {
      wchar_t psz_path[MAX_PATH];
      SHGetFolderPathW(nullptr, CSIDL_PERSONAL, nullptr, SHGFP_TYPE_CURRENT, psz_path);
      path = psz_path;
    }
  }
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::saveStartHubs(bool enable, const vector<wstring>& path)
{
  if(this->_config.valid()) {

    OmXmlNode start_list;

    if(this->_config.xml().hasChild(L"start_list")) {
      start_list = this->_config.xml().child(L"start_list");
    } else {
      start_list = this->_config.xml().addChild(L"start_list");
    }
    start_list.setAttr(L"enable", enable ? 1 : 0);

    vector<OmXmlNode> home_ls;
    start_list.children(home_ls, L"home");

    // remove all current file list
    for(size_t i = 0; i < home_ls.size(); ++i)
      start_list.remChild(home_ls[i]);

    // add new list
    for(size_t i = 0; i < path.size(); ++i)
      start_list.addChild(L"home").setContent(path[i]);

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::getStartHubs(bool* enable, vector<wstring>& path)
{
  path.clear();

  if(this->_config.valid()) {

    OmXmlNode start_list;
    if(this->_config.xml().hasChild(L"start_list")) {
      start_list = this->_config.xml().child(L"start_list");
    } else {
      *enable = false;
      return;
    }

    *enable = start_list.attrAsInt(L"enable");

    vector<OmXmlNode> path_ls;
    start_list.children(path_ls, L"home");

    // get list
    for(size_t i = 0; i < path_ls.size(); ++i)
      path.push_back(path_ls[i].content());
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::setIconsSize(unsigned size)
{
  this->_iconsSize = size;

  if(this->_config.valid()) {

    if(this->_config.xml().hasChild(L"icon_size")) {
      this->_config.xml().child(L"icon_size").setAttr(L"pixels", (int)this->_iconsSize);
    } else {
      this->_config.xml().addChild(L"icon_size").setAttr(L"pixels", (int)this->_iconsSize);
    }

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::setNoMarkdown(bool enable)
{
  this->_noMarkdown = enable;

  if(this->_config.valid()) {

    if(this->_config.xml().hasChild(L"no_markdown")) {
      this->_config.xml().child(L"no_markdown").setAttr(L"enable", (int)this->_noMarkdown);
    } else {
      this->_config.xml().addChild(L"no_markdown").setAttr(L"enable", (int)this->_noMarkdown);
    }

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmManager::modHubCreate(const wstring& title, const wstring& path, bool open)
{

  // check whether install path exists
  if(!Om_isDir(path)) {
    this->_error = Om_errIsDir(L"Home location path", path);
    this->log(0, L"Manager() Create Mod Hub", this->_error);
    return false;
  }

  // compose Mod Hub home path
  wstring ctx_home = path + L"\\" + title;

  // create Mod Hub home folder
  int result = Om_dirCreate(ctx_home);
  if(result != 0) {
    this->_error = Om_errCreate(L"Home folder", ctx_home, result);
    this->log(0, L"Manager() Create Mod Hub", this->_error);
    return false;
  }

  // compose Mod Hub definition file name
  wstring ctx_def_path = ctx_home + L"\\" + title;
  ctx_def_path += L"."; ctx_def_path += OMM_CTX_DEF_FILE_EXT;

  // initialize an empty Mod Hub definition file
  OmConfig ctx_def;
  if(!ctx_def.init(ctx_def_path, OMM_XMAGIC_HUB)) {
    this->_error =  Om_errInit(L"Definition file",ctx_def_path,ctx_def.lastErrorStr());
    this->log(0, L"Manager() Create Mod Hub", this->_error);
    return false;
  }

  // generate a new random UUID for this Mod Hub
  wstring uuid = Om_genUUID();

  // Get instance of XML document
  OmXmlNode def_xml = ctx_def.xml();

  // create uuid and title entry in Mod Hub definition
  def_xml.addChild(L"uuid").setContent(uuid);
  def_xml.addChild(L"title").setContent(title);

  // save and close definition file
  ctx_def.save();
  ctx_def.close();

  // open the new created Mod Hub
  if(open)
    return this->modHubLoad(ctx_def_path);

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmManager::modHubLoad(const wstring& path, bool select)
{
  // check whether Mod Hub is already opened
  for(size_t i = 0; i < _modHubLs.size(); ++i) {
    if(path == _modHubLs[i]->path())
      return true;
  }

  this->log(2, L"Manager() Open Mod Hub", L"Load \""+path+L"\"");

  OmModHub* pModHub = new OmModHub(this);
  if(!pModHub->open(path)) {

    this->_error = L"Mod Hub open failed: " + pModHub->lastError();
    this->log(0, L"Manager() Open Mod Hub", this->_error);

    delete pModHub;
    return false;
  }

  this->_modHubLs.push_back(pModHub);

  this->saveRecentFile(path);

  // the last loaded context become the active one
  if(select)
    this->modHubSel(this->_modHubLs.size() - 1);

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::modHubClose(int id)
{
  if(id < 0) {

    if(this->_modHubSl < 0)
      return;

    id = this->_modHubSl;

    this->_modHubSl = -1;
  }

  if(id < static_cast<int>(this->_modHubLs.size())) {
    this->_modHubLs[id]->close();
    delete _modHubLs[id];
    this->_modHubLs.erase(this->_modHubLs.begin()+id);
  }

  // the last loaded context become the active one
  this->modHubSel(this->_modHubLs.size() - 1);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::modHubSel(int i)
{
  if(i >= 0 && i < (int)this->_modHubLs.size()) {
    this->_modHubSl = i;
    this->log(2, L"Manager() Select Mod Hub", to_wstring(i)+L" \""+this->_modHubLs[_modHubSl]->title()+L"\"");
  } else {
    this->_modHubSl = -1;
    this->log(2, L"Manager() Select Mod Hub", L"<NONE>");
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::setLogOutput(HWND hWnd) {

  this->_logHwnd = hWnd;

  if(this->_logHwnd) {
    SendMessageW(static_cast<HWND>(this->_logHwnd), EM_SETLIMITTEXT, 0, 0);
    SendMessageW(static_cast<HWND>(this->_logHwnd), WM_SETTEXT, 0, reinterpret_cast<LPARAM>(this->_log.c_str()));
  }

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmManager::log(unsigned level, const wstring& head, const wstring& detail)
{
  wstring entry;

  // get local time
  int t_h, t_m, t_s;
  Om_getTime(&t_s, &t_m, &t_h);

  wchar_t hour[32];
  swprintf(hour, 32, L"[%02d:%02d:%02d]", t_h, t_m, t_s);
  entry = hour;

  switch(level)
  {
  case 0:
    entry += L"XX ";
    break;
  case 1:
    entry += L"!! ";
    break;
  default:
    entry += L"   ";
    break;
  }

  // build log entry line
  entry += head;
  if(detail.size()) {
    entry += L":: ";
    entry += detail;
  }
  entry += L"\r\n";

  // output to log window
  if(this->_logHwnd) {
    unsigned s = SendMessageW(static_cast<HWND>(this->_logHwnd), WM_GETTEXTLENGTH, 0, 0);
    SendMessageW(static_cast<HWND>(this->_logHwnd), EM_SETSEL, s, s);
    SendMessageW(static_cast<HWND>(this->_logHwnd), EM_REPLACESEL, 0, reinterpret_cast<LPARAM>(entry.c_str()));
    SendMessageW(static_cast<HWND>(this->_logHwnd), WM_VSCROLL, SB_BOTTOM, 0);
    RedrawWindow(static_cast<HWND>(this->_logHwnd), nullptr, nullptr, RDW_ERASE|RDW_INVALIDATE);
  }


  #ifdef DEBUG
  // output to standard output
  std::wcout << entry;
  #endif

  // write to log file
  if(this->_logFile) {
    DWORD wb;
    string data;
    Om_toUTF8(&data, entry);
    WriteFile(static_cast<HANDLE>(this->_logFile), data.c_str(), data.size(), &wb, nullptr);
  }

  this->_log += entry;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmManager::_migrate()
{
  if(this->_config.valid()) {

    vector<wstring> temp_ls;

    // Migrate startup Mod Hub list if required
    OmXmlNode start_list;
    if(this->_config.xml().hasChild(L"start_list")) {

      start_list = this->_config.xml().child(L"start_list");

      if(start_list.hasChild(L"file")) {

        vector<OmXmlNode> file_ls;
        start_list.children(file_ls, L"file");

        // get list
        for(size_t i = 0; i < file_ls.size(); ++i)
          temp_ls.push_back(file_ls[i].content());

        // remove all current file list
        for(size_t i = 0; i < file_ls.size(); ++i)
          start_list.remChild(file_ls[i]);

        // add new list
        for(size_t i = 0; i < temp_ls.size(); ++i)
          start_list.addChild(L"home").setContent(Om_getDirPart(temp_ls[i]));

        temp_ls.clear();
      }
    }

    // Migrate recent Mod Hub list if required
    if(this->_config.xml().hasChild(L"recent_list")) {

      OmXmlNode recent_list = this->_config.xml().child(L"recent_list");

      if(recent_list.hasChild(L"path")) {

        vector<OmXmlNode> path_ls;
        recent_list.children(path_ls, L"path");

        // get list
        for(size_t i = 0; i < path_ls.size(); ++i)
          temp_ls.push_back(path_ls[i].content());

        // remove all current file list
        for(size_t i = 0; i < path_ls.size(); ++i)
          recent_list.remChild(path_ls[i]);

        // add new list
        for(size_t i = 0; i < temp_ls.size(); ++i)
          recent_list.addChild(L"home").setContent(Om_getDirPart(temp_ls[i]));
      }
    }

    this->_config.save();
  }

  return true;
}
