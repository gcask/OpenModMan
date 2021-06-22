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
#include "gui/res/resource.h"
#include "gui/OmUiAddLoc.h"
#include "OmManager.h"

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmUiAddLoc::OmUiAddLoc(HINSTANCE hins) : OmDialog(hins),
  _pCtx(nullptr)
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmUiAddLoc::~OmUiAddLoc()
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
long OmUiAddLoc::id() const
{
  return IDD_ADD_LOC;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiAddLoc::_onTitleChange()
{
  wstring title;

  this->getItemText(IDC_EC_INP01, title);

  if(!Om_isValidName(title))
    title = L"<invalid path>";

  if(!this->msgItem(IDC_BC_CHK01, BM_GETCHECK)) {
    this->setItemText(IDC_EC_INP03, title + L"\\Library");
  }
  if(!this->msgItem(IDC_BC_CHK02, BM_GETCHECK)) {
    this->setItemText(IDC_EC_INP04, title + L"\\Backup");
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiAddLoc::_onBcBrwDst()
{
  wstring start, result;

  this->getItemText(IDC_EC_INP02, start);

  if(!Om_dialogBrowseDir(result, this->_hwnd, L"Select installation destination folder", start))
    return;

  this->setItemText(IDC_EC_INP02, result);

}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiAddLoc::_onCkBoxLib()
{
  wstring title;

  int bm_chk = this->msgItem(IDC_BC_CHK01, BM_GETCHECK);

  this->enableItem(IDC_BC_BRW03, bm_chk);
  this->enableItem(IDC_EC_INP03, bm_chk);

  if(!bm_chk) {
    this->getItemText(IDC_EC_INP01, title);
    if(!Om_isValidName(title)) {
      title = L"<invalid path>";
    }
  }

  this->setItemText(IDC_EC_INP03, title + L"\\Library");
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiAddLoc::_onBcBrwLib()
{
  wstring start, result;

  this->getItemText(IDC_EC_INP03, start);

  if(!Om_dialogBrowseDir(result, this->_hwnd, L"Select custom Library folder", start))
    return;

  this->setItemText(IDC_EC_INP03, result);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiAddLoc::_onCkBoxBck()
{
  wstring title;

  int bm_chk = this->msgItem(IDC_BC_CHK02, BM_GETCHECK);

  this->enableItem(IDC_BC_BRW04, bm_chk);
  this->enableItem(IDC_EC_INP04, bm_chk);

  if(!bm_chk) {
    this->getItemText(IDC_EC_INP01, title);
    if(!Om_isValidName(title)) {
      title = L"<invalid path>";
    }
  }

  this->setItemText(IDC_EC_INP04, title + L"\\Backup");
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiAddLoc::_onBcBrwBck()
{
  wstring start, result;

  this->getItemText(IDC_EC_INP04, start);

  if(!Om_dialogBrowseDir(result, this->_hwnd, L"Select custom Backup folder", start))
    return;

  this->setItemText(IDC_EC_INP04, result);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmUiAddLoc::_onBcOk()
{
  if(!this->_pCtx)
    return false;

  bool cust_lib, cust_bck;

  wstring loc_name, loc_dst, loc_lib, loc_bck;

  this->getItemText(IDC_EC_INP01, loc_name);
  if(!Om_isValidName(loc_name)) {
    wstring wrn = L"Title";
    wrn += OMM_STR_ERR_VALIDNAME;
    Om_dialogBoxWarn(this->_hwnd, L"Invalid Location title", wrn);
    return false;
  }

  this->getItemText(IDC_EC_INP02, loc_dst);
  if(!Om_isDir(loc_dst)) {
    wstring wrn = L"The folder \""+loc_dst+L"\"";
    wrn += OMM_STR_ERR_ISDIR;
    Om_dialogBoxWarn(this->_hwnd, L"Invalid install destination folder", wrn);
    return false;
  }

  cust_lib = this->msgItem(IDC_BC_CHK01, BM_GETCHECK);
  if(cust_lib) {
    this->getItemText(IDC_EC_INP03, loc_lib);
    if(!Om_isDir(loc_lib)) {
      wstring wrn = L"The folder \""+loc_lib+L"\"";
      wrn += OMM_STR_ERR_ISDIR;
      Om_dialogBoxWarn(this->_hwnd, L"Invalid custom library folder", wrn);
      return false;
    }
  }

  cust_bck = this->msgItem(IDC_BC_CHK02, BM_GETCHECK);
  if(cust_bck) {
    this->getItemText(IDC_EC_INP04, loc_bck);
    if(!Om_isDir(loc_bck)) {
      wstring wrn = L"The folder \""+loc_bck+L"\"";
      wrn += OMM_STR_ERR_ISDIR;
      Om_dialogBoxWarn(this->_hwnd, L"Invalid custom backup folder", wrn);
      return false;
    }
  }

  this->quit();

  // create new Location in Context
  if(!this->_pCtx->locAdd(loc_name, loc_dst, loc_lib, loc_bck)) {
    Om_dialogBoxErr(this->_hwnd, L"Location creation failed", this->_pCtx->lastError());
  }

  // refresh all tree from the main dialog
  this->root()->refresh();

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiAddLoc::_onInit()
{
  // define controls tool-tips
  this->_createTooltip(IDC_EC_INP01,  L"Indicative name");

  this->_createTooltip(IDC_EC_INP02,  L"Package installation destination path");
  this->_createTooltip(IDC_BC_BRW02,  L"Select destination folder");

  this->_createTooltip(IDC_BC_CHK01,  L"Use custom Library folder");
  this->_createTooltip(IDC_EC_INP03,  L"Custom Library folder path");
  this->_createTooltip(IDC_BC_BRW03,  L"Select custom Library folder");

  this->_createTooltip(IDC_BC_CHK02,  L"Use custom Backup folder");
  this->_createTooltip(IDC_EC_INP04,  L"Custom Backup folder path");
  this->_createTooltip(IDC_BC_BRW04,  L"Select custom Backup folder");

  // set default start values
  this->setItemText(IDC_EC_INP01, L"New Location");
  this->setItemText(IDC_EC_INP02, L"");
  this->setItemText(IDC_EC_INP03, L"New Location\\Library");
  this->setItemText(IDC_EC_INP04, L"New Location\\Backup");

  wstring item_str;

  // enable or disable "OK" button according values
  bool allow = true;

  this->getItemText(IDC_EC_INP01, item_str);
  if(!item_str.empty()) {

    this->getItemText(IDC_EC_INP02, item_str);
    if(!item_str.empty()) {

      if(this->msgItem(IDC_BC_CHK01, BM_GETCHECK)) {
        this->getItemText(IDC_EC_INP03, item_str);
        if(item_str.empty()) allow = false;
      }

      if(this->msgItem(IDC_BC_CHK02, BM_GETCHECK)) {
        this->getItemText(IDC_EC_INP04, item_str);
        if(item_str.empty()) allow = false;
      }

    } else {
      allow = false;
    }

  } else {
    allow = false;
  }

  this->enableItem(IDC_BC_OK, allow);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiAddLoc::_onResize()
{
  // Location title Label & EditControl
  this->_setItemPos(IDC_SC_LBL01, 10, 10, this->width()-25, 9);
  this->_setItemPos(IDC_EC_INP01, 10, 25, this->width()-25, 13);

  // Location Install Label & EditControl & Browse button
  this->_setItemPos(IDC_SC_LBL02, 10, 50, this->width()-25, 9);
  this->_setItemPos(IDC_EC_INP02, 10, 65, this->width()-45, 13);
  this->_setItemPos(IDC_BC_BRW02, this->width()-31, 65, 16, 13);

  // Custom Library Label & EditControl & Browse buttonben ess
  this->_setItemPos(IDC_BC_CHK01, 10, 100, this->width()-25, 9);
  this->_setItemPos(IDC_EC_INP03, 10, 115, this->width()-45, 13);
  this->_setItemPos(IDC_BC_BRW03, this->width()-31, 115, 16, 13);

  // Custom Library Label & EditControl & Browse button
  this->_setItemPos(IDC_BC_CHK02, 10, 140, this->width()-25, 9);
  this->_setItemPos(IDC_EC_INP04, 10, 155, this->width()-45, 13);
  this->_setItemPos(IDC_BC_BRW04, this->width()-31, 155, 16, 13);

  // ---- separator
  this->_setItemPos(IDC_SC_SEPAR, 5, this->height()-25, this->width()-10, 1);
  // Ok and Cancel buttons
  this->_setItemPos(IDC_BC_OK, this->width()-110, this->height()-19, 50, 14);
  this->_setItemPos(IDC_BC_CANCEL, this->width()-54, this->height()-19, 50, 14);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmUiAddLoc::_onMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {

    bool has_changed = false;

    wstring item_str;

    switch(LOWORD(wParam))
    {

    case IDC_EC_INP01: // Title
      if(HIWORD(wParam) == EN_CHANGE) {
        this->_onTitleChange();
        has_changed = true;
      }
      break;

    case IDC_BC_BRW02: // browse destination
      this->_onBcBrwDst();
      break;

    case IDC_BC_CHK01: //< Custom Library Checkbox
      this->_onCkBoxLib();
      break;

    case IDC_BC_BRW03: //< Custom Library "..." (browse) Button
      this->_onBcBrwLib();
      break;

    case IDC_BC_CHK02: //< Custom Backup Checkbox
      this->_onCkBoxBck();
      break;

    case IDC_BC_BRW04: //< Custom Backup "..." (browse) Button
      this->_onBcBrwBck();
      break;

    case IDC_EC_INP02: //< Destination EnditText
    case IDC_EC_INP03: //< Library EditText
    case IDC_EC_INP04: //< Backup EditText
      if(HIWORD(wParam) == EN_CHANGE)
        has_changed = true;
      break;

    case IDC_BC_OK:
      this->_onBcOk();
      break;

    case IDC_BC_CANCEL:
      this->quit();
      break;
    }

    // enable or disable "OK" button according values
    if(has_changed) {

      bool allow = true;

      this->getItemText(IDC_EC_INP01, item_str);
      if(!item_str.empty()) {

        this->getItemText(IDC_EC_INP02, item_str);
        if(!item_str.empty()) {

          if(this->msgItem(IDC_BC_CHK01, BM_GETCHECK)) {
            this->getItemText(IDC_EC_INP03, item_str);
            if(item_str.empty()) allow = false;
          }

          if(this->msgItem(IDC_BC_CHK02, BM_GETCHECK)) {
            this->getItemText(IDC_EC_INP04, item_str);
            if(item_str.empty()) allow = false;
          }

        } else {
          allow = false;
        }

      } else {
        allow = false;
      }

      this->enableItem(IDC_BC_OK, allow);
    }
  }

  return false;
}