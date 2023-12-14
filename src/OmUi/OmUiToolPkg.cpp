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
#include "OmBase.h"

#include "OmBaseUi.h"
#include "OmBaseWin.h"
  #include <UxTheme.h>
#include "OmBaseApp.h"

#include "OmArchive.h"

#include "OmModMan.h"
#include "OmModChan.h"
#include "OmModPack.h"

#include "OmUtilWin.h"
#include "OmUtilStr.h"
#include "OmUtilDlg.h"
#include "OmUtilPkg.h"
#include "OmUtilAlg.h"

//#include <algorithm>            //< std::replace

/*
#include "OmBaseWin.h"
#include <ShlObj.h>

#include "OmBaseUi.h"

#include "OmBaseApp.h"

#include "OmArchive.h"

#include "OmModMan.h"
#include "OmModChan.h"
#include "OmImage.h"

#include "OmUtilFs.h"



*/

#include "OmUiMan.h"

///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
#include "OmUiToolPkg.h"



/// \brief Mod categories list
///
/// List of predefined Mod categories
///
static const wchar_t __categ_list[][16] = {
  L"Generic",
  L"Texture",
  L"Skin",
  L"Model",
  L"Level",
  L"Mission",
  L"UI",
  L"Audio",
  L"Feature",
  L"Plugin",
  L"Script",
  L"Patch",
};

static const size_t __categ_count = 12;

/// \brief Custom "Package Save Done" Message
///
/// Custom "Package Save Done" window message to notify the dialog that the
/// running thread finished his job.
///
#define UWM_PKGSAVE_DONE    (WM_APP+1)

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmUiToolPkg::OmUiToolPkg(HINSTANCE hins) : OmDialog(hins),
  _ModPack(new OmModPack(nullptr)),
  _has_unsaved(false),
  _method_cache(-1),
  _modpack_save_abort(0),
  _modpack_save_hth(nullptr),
  _modpack_save_hwo(nullptr),
  _modpack_save_hdp(nullptr)
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmUiToolPkg::~OmUiToolPkg()
{
  if(this->_ModPack)
    delete this->_ModPack;

  HBITMAP hBm = this->setStImage(IDC_SB_SNAP, nullptr);
  if(hBm && hBm != Om_getResImage(IDB_BLANK)) DeleteObject(hBm);

  HFONT hFt = reinterpret_cast<HFONT>(this->msgItem(IDC_EC_DESC, WM_GETFONT));
  DeleteObject(hFt);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
long OmUiToolPkg::id() const
{
  return IDD_TOOL_PKG;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_status_update_filename()
{
  OmWString file_path;

  // set definition file path to status bar
  if(!this->_ModPack->sourcePath().empty() && !this->_ModPack->sourceIsDir()) {
    file_path = this->_ModPack->sourcePath();
  } else {
    file_path = L"<unsaved package>";
  }

  this->setItemText(IDC_SC_FILE, file_path);

  OmWString caption = Om_getFilePart(file_path);
  caption += L" - Mod-Package editor";
  this->setCaption(caption);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_set_unsaved(bool enable)
{
  this->_has_unsaved = enable;

  bool file_exists = (!this->_ModPack->sourcePath().empty() && !this->_ModPack->sourceIsDir());

  // enable/disable and change tool bar 'save' button image
  TBBUTTONINFOA tbBi = {}; tbBi.cbSize = sizeof(TBBUTTONINFOA);
  tbBi.dwMask = TBIF_STATE;
  tbBi.fsState = (this->_has_unsaved && file_exists) ? TBSTATE_ENABLED : 0;
  this->msgItem(IDC_TB_TOOLS, TB_SETBUTTONINFO, IDC_BC_SAVE, reinterpret_cast<LPARAM>(&tbBi));

  // enable/disable and change menu 'save' item
  if(this->_has_unsaved && file_exists) {
    this->setPopupItem(MNU_ME_FILE, MNU_ME_FILE_SAVE, MF_ENABLED);
  } else {
    this->setPopupItem(MNU_ME_FILE, MNU_ME_FILE_SAVE, MF_GRAYED);
  }
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
int32_t OmUiToolPkg::_ask_unsaved()
{
  // Check and ask for unsaved changes
  if(this->_has_unsaved)
    return Om_dlgBox_ync(this->_hwnd, L"Mod-Package editor", IDI_QRY, L"Unsaved changes", L"Do you want to save changes before closing ?");

  return 0;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmUiToolPkg::_has_changes()
{
  // check for compression method difference
  if(this->_ModPack->hasSource()) {
    int32_t cb_sel = this->msgItem(IDC_CB_ZMD, CB_GETCURSEL);
    if(this->_method_cache != this->msgItem(IDC_CB_ZMD, CB_GETITEMDATA, cb_sel))
      return true;
  }

  // check for category difference
  if(this->_ModPack->category().empty()) {
    if(this->msgItem(IDC_CB_CAT, CB_GETCURSEL) != 0)
      return true;
  } else {
    if(!Om_namesMatches(this->_ModPack->category(), this->_categ_cache))
       return true;
  }

  // check for description difference
  if(this->_ModPack->description() != this->_desc_cache)
    return true;

  // check for dependencies differences
  if(this->_ModPack->dependCount() != this->_depend_cache.size()) {

    return true;

  } else {

    for(size_t i = 0; i < this->_ModPack->dependCount(); ++i)
      if(Om_arrayContain(this->_depend_cache, this->_ModPack->getDependIden(i)))
        return true;
  }

  // finally check for thumbnail difference (potentially the most costly)
  if(this->_ModPack->thumbnail() != this->_thumb_cache)
    return true;

  return false;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_reset_controls()
{

  // empty controls
  this->setItemText(IDC_EC_INP01, L"");
  this->enableItem(IDC_EC_INP01,  false);

  this->setItemText(IDC_EC_INP02, L"");
  this->enableItem(IDC_EC_INP02,  false);

  this->msgItem(IDC_CB_EXT, CB_SETCURSEL, 0);
  this->enableItem(IDC_CB_EXT,  false);

  this->setItemText(IDC_EC_RESUL, L"");
  this->enableItem(IDC_EC_RESUL,  false);

  //this->msgItem(IDC_CB_ZMD, CB_SETCURSEL, 4); //< FIXME : keep or reset ?
  //this->msgItem(IDC_CB_ZLV, CB_SETCURSEL, 2);
  this->enableItem(IDC_CB_ZMD,  false);
  this->enableItem(IDC_CB_ZLV,  false);

  this->enableItem(IDC_BC_BRW01,  false);
  this->enableItem(IDC_BC_BRW02,  false);
  this->enableItem(IDC_BC_DEL,    false);
  this->enableItem(IDC_LV_PAT,    false);
  this->_content_populate();

  //this->enableItem(IDC_EC_READ1,  false);
  //this->setItemText(IDC_EC_READ1, L"");

  //this->msgItem(IDC_CB_CAT, CB_SETCURSEL, 0);  //< FIXME : keep or reset ?
  //this->setItemText(IDC_EC_INP07, L"");  //< FIXME : keep or reset ?

  this->msgItem(IDC_BC_CKBX1, BM_SETCHECK, 0);
  this->enableItem(IDC_BC_CKBX1,  false);
  this->enableItem(IDC_BC_BRW03,  false);
  // set thumbnail placeholder
  HBITMAP hBm = this->setStImage(IDC_SB_SNAP, Om_getResImage(IDB_BLANK));
  if(hBm && hBm != Om_getResImage(IDB_BLANK)) DeleteObject(hBm);
  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_THMBSEL, MF_GRAYED);

  this->msgItem(IDC_BC_CKBX2, BM_SETCHECK, 0);
  this->enableItem(IDC_BC_CKBX2,  false);
  this->enableItem(IDC_BC_BRW04,  false);
  this->setItemText(IDC_EC_DESC, L"");
  this->enableItem(IDC_EC_DESC,  false);
  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_DESCSEL, MF_GRAYED);

  this->msgItem(IDC_BC_CKBX3, BM_SETCHECK, 0);
  this->enableItem(IDC_BC_CKBX3,  false);
  this->enableItem(IDC_BC_DPADD,  false);
  this->enableItem(IDC_BC_DPBRW,  false);
  this->enableItem(IDC_BC_DPDEL,  false);
  this->msgItem(IDC_LB_DPN, LB_RESETCONTENT); //< empty ListBox
  this->enableItem(IDC_LB_DPN, false);
  this->setItemText(IDC_EC_INP08, L"");
  this->enableItem(IDC_EC_INP08,  false);
  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_DEPIMP, MF_GRAYED);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_check_zip_method()
{
  int32_t cb_sel = this->msgItem(IDC_CB_ZMD, CB_GETCURSEL);
  int32_t method = this->msgItem(IDC_CB_ZMD, CB_GETITEMDATA, cb_sel);

  cb_sel = this->msgItem(IDC_CB_EXT, CB_GETCURSEL);
  OmWString cb_entry;
  this->getCbText(IDC_CB_EXT, cb_sel, cb_entry);

  if(Om_namesMatches(cb_entry, L".zip") && method != OM_METHOD_DEFLATE) {

    Om_dlgBox_ok(this->_hwnd, L"Mod-package editor", IDI_WRN, L"Non-standard Zip compression",
                 L"The selected compression method is not widely supported for Zip files, for "
                 "maximum compatibility prefer the \"Defalte\" method.");
  }

  // check for changes
  if(this->_ModPack->hasSource())
    this->_set_unsaved(this->_has_changes());
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_modpack_close()
{
   switch(this->_ask_unsaved()) {
    case  1: this->_modpack_save(); break; //< 'Yes'
    case -1: return;                       //< 'Cancel'
  }

  // new Mod-Package
  this->_ModPack->clearAll();

  // reset all cached data
  this->_method_cache = -1;
  this->_content_cache.clear();
  this->_categ_cache.clear();
  this->_thumb_cache.clear();
  this->_desc_cache.clear();
  this->_depend_cache.clear();

  // resets all controls to initial state
  this->_reset_controls();

}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_modpack_new()
{
   switch(this->_ask_unsaved()) {
    case  1: this->_modpack_save(); break; //< 'Yes'
    case -1: return;                       //< 'Cancel'
  }

  // new Mod-Package
  this->_ModPack->clearAll();

  // reset all cached data
  this->_method_cache = -1;
  this->_content_cache.clear();
  this->_categ_cache.clear();
  this->_thumb_cache.clear();
  this->_desc_cache.clear();
  this->_depend_cache.clear();

  // resets all controls to initial state
  this->_reset_controls();

  // set default file name and version
  this->enableItem(IDC_EC_INP01,  true);
  this->setItemText(IDC_EC_INP01, L"Untilted Mod");
  this->enableItem(IDC_EC_INP02,  true);
  this->setItemText(IDC_EC_INP02, L"1.0");
  this->enableItem(IDC_CB_EXT,    true);
  this->enableItem(IDC_EC_RESUL,  true);

  this->_name_compose();

  // Compression
  this->enableItem(IDC_CB_ZMD,    true);
  this->enableItem(IDC_CB_ZLV,    true);

  // enable Content actions and ListView
  this->enableItem(IDC_BC_BRW01,  true);
  this->enableItem(IDC_BC_BRW02,  true);
  this->enableItem(IDC_BC_DEL,    false);
  this->enableItem(IDC_LV_PAT,    true);

  this->_status_update_filename();

  // Category and CheckBoxes
  this->enableItem(IDC_CB_CAT,    true);
  this->enableItem(IDC_BC_CKBX1,  true);
  this->enableItem(IDC_BC_CKBX2,  true);
  this->enableItem(IDC_BC_CKBX3,  true);

  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_THMBSEL, MF_ENABLED);
  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_DESCSEL, MF_ENABLED);
  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_DEPIMP, MF_ENABLED);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_modpack_open()
{
   switch(this->_ask_unsaved()) {
    case  1: this->_modpack_save(); break; //< 'Yes'
    case -1: return;                       //< 'Cancel'
  }

  // if available, select current active channel library as start location
  OmWString start;
  OmModChan* ModChan = static_cast<OmModMan*>(this->_data)->activeChannel();
  if(ModChan) start = ModChan->libraryPath();

  // new dialog to open file (allow multiple selection)
  OmWString result;
  if(!Om_dlgOpenFile(result, this->_hwnd, L"Open Mod-Package(s)", OM_PKG_FILES_FILTER, start))
    return;

  // run add list thread
  if(!this->_modpack_parse(result)) {
    Om_dlgBox_okl(this->_hwnd, L"Mod-package editor", IDI_WRN, L"Mod-package parse error",
                 L"The following file parse failed, it is either corrupted or not a valid Mod-package",
                 result);
  }
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_modpack_build()
{
   switch(this->_ask_unsaved()) {
    case  1: this->_modpack_save(); break; //< 'Yes'
    case -1: return;                       //< 'Cancel'
  }

  // if available, select current active channel library as start location
  OmWString start;
  OmModChan* ModChan = static_cast<OmModMan*>(this->_data)->activeChannel();
  if(ModChan) start = ModChan->libraryPath();

  // new dialog to open file (allow multiple selection)
  OmWString result;
  if(!Om_dlgOpenDir(result, this->_hwnd, L"Open Mod directory", start))
    return;

  // run add list thread
  if(!this->_modpack_parse(result)) {
    Om_dlgBox_okl(this->_hwnd, L"Mod-package editor", IDI_WRN, L"Mod directory parse error",
                 L"The following directory parse failed, and this should never happen, so...",
                 result);
  }
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_modpack_save_as()
{
  if(this->_modpack_save_hth)
    return;

  OmWString dlg_start, dlg_result;

  // if available, select current active channel library as start location
  OmModChan* ModChan = static_cast<OmModMan*>(this->_data)->activeChannel();
  if(ModChan) dlg_start = ModChan->libraryPath();

  // get default filename
  OmWString filename;
  this->getItemText(IDC_EC_RESUL, filename);

  OmWString extension;
  this->getCbText(IDC_CB_EXT, this->msgItem(IDC_CB_EXT, CB_GETCURSEL), extension);
  // remove the leading dot
  extension.erase(0, 1);

  // send save dialog to user
  if(!Om_dlgSaveFile(dlg_result, this->_hwnd, L"Save Mod-Package", OM_PKG_FILES_FILTER, extension.c_str(), filename.c_str(), dlg_start))
    return;

/*
  standard save dialog (IFileSaveDialog) already ask for overwrite

  // ask user for overwirte
  if(!Om_dlgOverwriteFile(this->_hwnd, dlg_result))
    return;
*/

  // start the "save" thread
  this->_modpack_save_path = dlg_result;

  this->_modpack_save_hth = Om_createThread(OmUiToolPkg::_modpack_save_run_fn, this);
  this->_modpack_save_hwo = Om_waitForThread(this->_modpack_save_hth, OmUiToolPkg::_modpack_save_end_fn, this);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_modpack_save()
{
  if(this->_modpack_save_hth)
    return;

  if(this->_ModPack->sourcePath().empty() || this->_ModPack->sourceIsDir()) {
    this->_modpack_save_as();
    return;
  }

  // start the "save" thread
  this->_modpack_save_path = this->_ModPack->sourcePath();

  this->_modpack_save_hth = Om_createThread(OmUiToolPkg::_modpack_save_run_fn, this);
  this->_modpack_save_hwo = Om_waitForThread(this->_modpack_save_hth, OmUiToolPkg::_modpack_save_end_fn, this);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmUiToolPkg::_modpack_parse(const OmWString& path)
{
  // reset all cached data
  this->_method_cache = -1;
  this->_content_cache.clear();
  this->_categ_cache.clear();
  this->_thumb_cache.clear();
  this->_desc_cache.clear();
  this->_depend_cache.clear();

  // resets all controls to initial state
  this->_reset_controls();

  bool has_failed = false;

  if(!this->_ModPack->parseSource(path)) {
    this->_ModPack->clearAll();
    has_failed = true;
  }

  // update status
  this->_status_update_filename();

  if(has_failed)
    return false;

  // Filename / version / ext
  this->enableItem(IDC_EC_INP01,  true);
  this->enableItem(IDC_EC_INP02,  true);
  this->enableItem(IDC_CB_EXT,    true);
  this->enableItem(IDC_EC_RESUL,  true);

  // Compression
  this->enableItem(IDC_CB_ZMD,    true);
  this->enableItem(IDC_CB_ZLV,    true);

  // Mod content
  this->enableItem(IDC_BC_BRW01,  true);
  this->enableItem(IDC_BC_BRW02,  true);
  this->enableItem(IDC_LV_PAT,    true);
  //this->enableItem(IDC_EC_READ1,  true);

  // Category and CheckBoxes
  this->enableItem(IDC_CB_CAT,    true);
  this->enableItem(IDC_BC_CKBX1,  true);
  this->enableItem(IDC_BC_CKBX2,  true);
  this->enableItem(IDC_BC_CKBX3,  true);

  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_THMBSEL, MF_ENABLED);
  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_DESCSEL, MF_ENABLED);
  this->setPopupItem(MNU_ME_EDIT, MNU_ME_EDIT_DEPIMP, MF_ENABLED);

  // parse file name
  OmWString iden, core, vers, name;

  if(this->_ModPack->sourceIsDir()) {
    iden = Om_getFilePart(path);
  } else {
    iden = Om_getNamePart(path);
  }

  Om_parseModIdent(iden, &core, &vers, &name);

  this->setItemText(IDC_EC_INP01, name);
  this->setItemText(IDC_EC_INP02, vers);

  OmWString ext = Om_getFileExtPart(path);
  if(Om_namesMatches(ext, L"zip")) {
    this->msgItem(IDC_CB_EXT, CB_SETCURSEL, 1);
  } else {
    this->msgItem(IDC_CB_EXT, CB_SETCURSEL, 0);
  }

  // force filename preview to the current file
  this->setItemText(IDC_EC_RESUL, Om_getFilePart(path));

  // try to get a compression method
  this->_method_cache = this->_ModPack->getSourceCompMethod();

  if(this->_method_cache < 0) {
    this->msgItem(IDC_CB_ZMD, CB_SETCURSEL, 4);
  } else {
    uint32_t cb_count = this->msgItem(IDC_CB_ZMD, CB_GETCOUNT);
    for(uint32_t i = 0; i < cb_count; ++i) {
      if(this->msgItem(IDC_CB_ZMD, CB_GETITEMDATA, i) == this->_method_cache) {
        this->msgItem(IDC_CB_ZMD, CB_SETCURSEL, i);
      }
    }
  }

  // set pack content
  if(!this->_ModPack->sourceEntryCount()) {
    //this->setItemText(IDC_EC_READ1, L"<empty package>");
  } else {
    // copy to local cache
    for(size_t i = 0; i < this->_ModPack->sourceEntryCount(); ++i) {
      this->_content_cache.push_back(this->_ModPack->getSourceEntry(i));
    }
    // build-up ListView content
    this->_content_populate();

    /*
    OmWString ec_content;

    size_t n = this->_ModPack->sourceEntryCount();
    for(size_t i = 0; i < n; ++i) {
      ec_content += this->_ModPack->getSourceEntry(i).path;
      if(i < n - 1) ec_content += L"\r\n";
    }

    this->setItemText(IDC_EC_READ1, ec_content);
    */
  }

  // set category
  int32_t categ_id = -1;

  if(!this->_ModPack->category().empty()) {
    for(size_t i = 0; i < __categ_count; ++i) {
      if(Om_namesMatches(__categ_list[i], this->_ModPack->category())) {
        categ_id = i; break;
      }
    }
  } else {
    categ_id = 0; //< "Generic" is default category
  }

  if(categ_id >= 0) {
    this->enableItem(IDC_EC_INP07, false);
    this->msgItem(IDC_CB_CAT, CB_SETCURSEL, categ_id);
    this->getCbText(IDC_CB_CAT, categ_id, this->_categ_cache);
  } else {
    // copy to local cache
    this->_categ_cache = this->_ModPack->category();
    // set controls
    int32_t cb_last = this->msgItem(IDC_CB_CAT, CB_GETCOUNT) - 1;
    this->msgItem(IDC_CB_CAT, CB_SETCURSEL, cb_last);
    this->enableItem(IDC_EC_INP07, true);
    this->setItemText(IDC_EC_INP07, this->_ModPack->category());
  }

  // set thumbnail
  if(!this->_ModPack->thumbnail().valid()) {
    this->msgItem(IDC_BC_CKBX1, BM_SETCHECK, 0);
    //this->_thumb_toggle();
  } else {
    // copy to local cache
    this->_thumb_cache = this->_ModPack->thumbnail();
    // set controls
    this->msgItem(IDC_BC_CKBX1, BM_SETCHECK, 1);
    this->enableItem(IDC_BC_BRW03, true);
    HBITMAP hBm = this->setStImage(IDC_SB_SNAP, this->_thumb_cache.hbmp());
    if(hBm && hBm != Om_getResImage(IDB_BLANK)) DeleteObject(hBm);
  }

  // set description
  if(this->_ModPack->description().empty()) {
    this->msgItem(IDC_BC_CKBX2, BM_SETCHECK, 0);
    //this->_desc_toggle();
  } else {
    // copy to local cache
    this->_desc_cache = this->_ModPack->description();
    // set controls
    this->msgItem(IDC_BC_CKBX2, BM_SETCHECK, 1);
    this->enableItem(IDC_BC_BRW04, true);
    this->enableItem(IDC_EC_DESC, true);
    this->setItemText(IDC_EC_DESC, this->_desc_cache);
  }

  // set dependencies
  if(this->_ModPack->dependCount() == 0) {
    this->msgItem(IDC_BC_CKBX3, BM_SETCHECK, 0);
    //this->_depend_toggle();
  } else {
    // copy to local cache
    for(size_t i = 0; i < this->_ModPack->dependCount(); ++i)
      this->_depend_cache.push_back(this->_ModPack->getDependIden(i));
    // set controls
    this->msgItem(IDC_BC_CKBX3, BM_SETCHECK, 1);
    this->enableItem(IDC_LB_DPN, true);
    this->_depend_populate();
  }

  // nothing to save
  this->_set_unsaved(false);

  return true;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
DWORD WINAPI OmUiToolPkg::_modpack_save_run_fn(void* ptr)
{
  OmUiToolPkg* self = static_cast<OmUiToolPkg*>(ptr);

  // set cached data to local instance
  self->_ModPack->setCategory(self->_categ_cache);
  self->_ModPack->setThumbnail(self->_thumb_cache);
  self->_ModPack->setDescription(self->_desc_cache);
  self->_ModPack->clearDepend();
  for(size_t i = 0; i < self->_depend_cache.size(); ++i)
    self->_ModPack->addDependIden(self->_depend_cache[i]);

  //get compression method and level
  int32_t method = self->msgItem(IDC_CB_ZMD, CB_GETITEMDATA, self->msgItem(IDC_CB_ZMD, CB_GETCURSEL));
  int32_t level = self->msgItem(IDC_CB_ZLV, CB_GETITEMDATA, self->msgItem(IDC_CB_ZLV, CB_GETCURSEL));

  // Open progress dialog
  self->_modpack_save_abort = 0;
  self->_modpack_save_hdp = Om_dlgProgress(self->_hwnd, L"Save Mod-Package", IDI_PKG_ADD, L"Saving Mod-Package", &self->_modpack_save_abort, OM_DLGBOX_DUAL_BARS);

  // and here we go for saving
  OmResult result = self->_ModPack->saveAs(self->_modpack_save_path, method, level,
                                          OmUiToolPkg::_modpack_save_progress_fn,
                                          OmUiToolPkg::_modpack_save_compress_fn, self);

  // quit the progress dialog (dialogs must be opened and closed within the same thread)
  Om_dlgProgressClose(static_cast<HWND>(self->_modpack_save_hdp));
  self->_modpack_save_hdp = nullptr;

  return result;
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmUiToolPkg::_modpack_save_progress_fn(void* ptr, size_t tot, size_t cur, uint64_t param)
{
  OM_UNUSED(param);

  OmUiToolPkg* self = static_cast<OmUiToolPkg*>(ptr);

  // update progress bar
  Om_dlgProgressUpdate(static_cast<HWND>(self->_modpack_save_hdp), tot, cur, nullptr, 1);

  return (self->_modpack_save_abort != 1);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmUiToolPkg::_modpack_save_compress_fn(void* ptr, size_t tot, size_t cur, uint64_t param)
{
  OmUiToolPkg* self = static_cast<OmUiToolPkg*>(ptr);

  // update progress text
  OmWString progress_text = L"Compressing file: ";
  progress_text += Om_getFilePart(reinterpret_cast<wchar_t*>(param));
  Om_dlgProgressUpdate(static_cast<HWND>(self->_modpack_save_hdp), tot, cur, progress_text.c_str(), 0);

  return (self->_modpack_save_abort != 1);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
VOID WINAPI OmUiToolPkg::_modpack_save_end_fn(void* ptr, uint8_t fired)
{
  OM_UNUSED(fired);

  OmUiToolPkg* self = static_cast<OmUiToolPkg*>(ptr);

  OmResult result = static_cast<OmResult>(Om_threadExitCode(self->_modpack_save_hth));
  Om_clearThread(self->_modpack_save_hth, self->_modpack_save_hwo);

  self->_modpack_save_hth = nullptr;
  self->_modpack_save_hwo = nullptr;

  if(result == OM_RESULT_OK) {

    // parse the new created package
    self->_modpack_parse(self->_modpack_save_path);

  } else {

    if(result == OM_RESULT_ERROR) {
      Om_dlgBox_okl(self->_hwnd, L"Mod-package editor", IDI_WRN, L"Mod-Package save error",
                   L"Mod-Package save failed:", self->_ModPack->lastError());
    }

    // re-parse previous package
    OmWString previous_path = self->_ModPack->sourcePath();
    self->_modpack_parse(previous_path);
  }
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_name_compose()
{
  OmWString filename, name, vers, ext;

  this->getItemText(IDC_EC_INP01, name);

  if(name.empty())
    return;

  filename = Om_spacesToUnderscores(name);

  this->getItemText(IDC_EC_INP02, vers);

  OmVersion version(vers);
  if(!version.isNull()) {
    filename += L"_v";
    filename += version.asString();
  }

  int32_t cb_sel = this->msgItem(IDC_CB_EXT, CB_GETCURSEL);
  this->getCbText(IDC_CB_EXT, cb_sel, ext);

  filename += ext;

  this->setItemText(IDC_EC_RESUL, filename);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_content_resize()
{
  LONG size[4];

  // Resize the Mods ListView column
  GetClientRect(this->getItem(IDC_LV_PAT), reinterpret_cast<LPRECT>(&size));
  this->msgItem(IDC_LV_PAT, LVM_SETCOLUMNWIDTH, 0, size[2]-2);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_content_selchg()
{
  // get count of ListView selected item
  uint32_t lv_nsl = this->msgItem(IDC_LV_PAT, LVM_GETSELECTEDCOUNT);

  bool has_select = (lv_nsl > 0);

  this->enableItem(IDC_BC_DEL, has_select);

  /*
  // scan selection to check what can be done
  int32_t lv_sel = this->msgItem(IDC_LV_PAT, LVM_GETNEXTITEM, -1, LVNI_SELECTED);
  while(lv_sel != -1) {

    // next selected item
    lv_sel = this->msgItem(IDC_LV_PAT, LVM_GETNEXTITEM, lv_sel, LVNI_SELECTED);
  }
  */
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_content_populate()
{
  // Save list-view scroll position to lvRect
  RECT lvRec;
  this->msgItem(IDC_LV_PAT, LVM_GETVIEWRECT, 0, reinterpret_cast<LPARAM>(&lvRec));
  // empty list view
  this->msgItem(IDC_LV_PAT, LVM_DELETEALLITEMS);

  // add item to list view
  LVITEMW lvI = {};
  for(size_t i = 0; i < this->_content_cache.size(); ++i) {

    lvI.iItem = static_cast<int32_t>(i);

    // Single column, entry path, if it is a file
    lvI.iSubItem = 0; lvI.mask = LVIF_IMAGE|LVIF_TEXT|LVIF_PARAM;
    lvI.iImage = OM_HAS_BIT(this->_content_cache[i].attr, OM_MODENTRY_DIR) ? ICON_DIR : ICON_FIL;
    lvI.lParam = i;
    lvI.pszText = const_cast<LPWSTR>(this->_content_cache[i].path.c_str());
    this->msgItem(IDC_LV_PAT, LVM_INSERTITEMW, 0, reinterpret_cast<LPARAM>(&lvI));
  }

  // we enable the ListView
  this->enableItem(IDC_LV_PAT, true);
  // restore ListView scroll position from lvRec
  this->msgItem(IDC_LV_PAT, LVM_SCROLL, 0, -lvRec.top );

  // adapt ListView column size to client area
  this->_content_resize();

  // update Mods ListView selection
  this->_content_selchg();
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_categ_select()
{
  int32_t cb_sel = this->msgItem(IDC_CB_CAT, CB_GETCURSEL);

  // check whether user selected the last item (GENERIC)
  if(cb_sel == this->msgItem(IDC_CB_CAT, CB_GETCOUNT) - 1) {

    this->enableItem(IDC_EC_INP07, true);
    this->setItemText(IDC_EC_INP07, this->_categ_cache);

  } else {

    this->enableItem(IDC_EC_INP07, false);
    this->setItemText(IDC_EC_INP07, L"");

    // write changes to current Package
    this->getCbText(IDC_CB_CAT, cb_sel, this->_categ_cache);
  }

  // check for changes
  this->_set_unsaved(this->_has_changes());
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_categ_changed()
{
  // we keep a local copy of the last edited text to restore it
  if(IsWindowEnabled(this->getItem(IDC_EC_INP07))) {

    this->getItemText(IDC_EC_INP07, this->_categ_cache);

    // check for changes
    this->_set_unsaved(this->_has_changes());
  }
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_thumb_toggle()
{
  bool is_enabled = this->msgItem(IDC_BC_CKBX1, BM_GETCHECK);

  this->enableItem(IDC_BC_DPADD, is_enabled);
  this->enableItem(IDC_BC_BRW03, is_enabled);
  this->enableItem(IDC_SB_SNAP, is_enabled);

  HBITMAP hBm = nullptr;

  // clear local thumbnail
  this->_thumb_cache.clear();

  if(is_enabled) {

    // set thumbnail to current Mod Pack thumbnail
    if(this->_ModPack->thumbnail().valid()) {
      this->_thumb_cache = this->_ModPack->thumbnail();
      hBm = this->setStImage(IDC_SB_SNAP, this->_thumb_cache.hbmp());
    } else {
      hBm = this->setStImage(IDC_SB_SNAP, Om_getResImage(IDB_BLANK));
    }

  } else {

    // set thumbnail placeholder to static control
    hBm = this->setStImage(IDC_SB_SNAP, Om_getResImage(IDB_BLANK));

  }

  // check for changes
  this->_set_unsaved(this->_has_changes());

  if(hBm && hBm != Om_getResImage(IDB_BLANK)) DeleteObject(hBm);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_reference_thumb_load()
{
  OmWString open_start, open_result;

  // if available, select current active channel library as start location
  OmModChan* ModChan = static_cast<OmModMan*>(this->_data)->activeChannel();
  if(ModChan) open_start = ModChan->libraryPath();

  // new dialog to open file
  if(!Om_dlgOpenFile(open_result, this->_hwnd, L"Open image file", OM_IMG_FILES_FILTER, open_start))
    return;

  // try to load image file
  if(this->_thumb_cache.loadThumbnail(open_result, OM_MODPACK_THUMB_SIZE, OM_SIZE_FILL)) {

    // set image to static control
    HBITMAP hBm = this->setStImage(IDC_SB_SNAP, this->_thumb_cache.hbmp());
    if(hBm && hBm != Om_getResImage(IDB_BLANK)) DeleteObject(hBm);
  }

  // check for changes
  this->_set_unsaved(this->_has_changes());
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_desc_toggle()
{
  bool is_enabled = this->msgItem(IDC_BC_CKBX2, BM_GETCHECK);

  this->enableItem(IDC_BC_BRW04, is_enabled);
  this->enableItem(IDC_EC_DESC, is_enabled);

  // reset description
  this->_desc_cache.clear();

  if(is_enabled) {
    if(!this->_ModPack->description().empty())
      this->_desc_cache = this->_ModPack->description();
  }

  // check for changes
  this->_set_unsaved(this->_has_changes());

  this->setItemText(IDC_EC_DESC, this->_desc_cache);
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_reference_desc_load()
{
  OmWString open_start, open_result;

  // if available, select current active channel library as start location
  OmModChan* ModChan = static_cast<OmModMan*>(this->_data)->activeChannel();
  if(ModChan) open_start = ModChan->libraryPath();

  // new dialog to open file
  if(!Om_dlgOpenFile(open_result, this->_hwnd, L"Open text file", OM_TXT_FILES_FILTER, open_start))
    return;

  // we keep local copy of loaded text
  this->_desc_cache = Om_toCRLF(Om_toUTF16(Om_loadPlainText(open_result)));

  // assign new description
  this->setItemText(IDC_EC_DESC, this->_desc_cache);

  // check for changes
  this->_set_unsaved(this->_has_changes());
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_reference_desc_changed()
{
  // we keep a local copy of the last edited text to restore it
  if(IsWindowEnabled(this->getItem(IDC_EC_DESC))) {

    this->getItemText(IDC_EC_DESC, this->_desc_cache);

    // check for changes
    this->_set_unsaved(this->_has_changes());
  }
}
///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_depend_populate()
{
  this->msgItem(IDC_LB_DPN, LB_RESETCONTENT); //< empty ListBox

  for(size_t i = 0; i < this->_depend_cache.size(); ++i)
    this->msgItem(IDC_LB_DPN, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(this->_depend_cache[i].c_str()));
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_depend_toggle()
{
  bool is_enabled = this->msgItem(IDC_BC_CKBX3, BM_GETCHECK);

  this->enableItem(IDC_BC_DPADD, is_enabled);
  this->enableItem(IDC_BC_DPBRW, is_enabled);
  this->enableItem(IDC_LB_DPN, is_enabled);

  this->_depend_cache.clear();

  if(is_enabled) {
    if(this->_ModPack->dependCount()) {
      for(size_t i = 0; i < this->_ModPack->dependCount(); ++i)
        this->_depend_cache.push_back(this->_ModPack->getDependIden(i));
    }
  }

  // rebuild depend list
  this->_depend_populate();

  // check for changes
  this->_set_unsaved(this->_has_changes());
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_depend_sel_changed()
{
  // get ListBox current selection
  int32_t lb_sel = this->msgItem(IDC_LB_DPN, LB_GETCURSEL);

  this->enableItem(IDC_BC_DPDEL, (lb_sel >= 0));
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_depend_delete()
{
  // get ListBox current selection
  int32_t lb_sel = this->msgItem(IDC_LB_DPN, LB_GETCURSEL);
  if(lb_sel < 0) return;

  this->msgItem(IDC_LB_DPN, LB_DELETESTRING, lb_sel);
  this->_depend_cache.erase(this->_depend_cache.begin() + lb_sel);

  this->_depend_sel_changed();

  // check for changes
  this->_set_unsaved(this->_has_changes());
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_depend_browse()
{
  // if available, select current active channel library as start location
  OmWString start;
  OmModChan* ModChan = static_cast<OmModMan*>(this->_data)->activeChannel();
  if(ModChan) start = ModChan->libraryPath();

  // new dialog to open file (allow multiple selection)
  OmWStringArray result;
  if(!Om_dlgOpenFileMultiple(result, this->_hwnd, L"Open Mod-Package(s)", OM_PKG_FILES_FILTER, start))
    return;

  OmWString identity;

  for(size_t i = 0; i < result.size(); ++i) {

    identity = Om_getNamePart(result[i]);

    this->msgItem(IDC_LB_DPN, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(identity.c_str()));
    this->_depend_cache.push_back(identity);
  }

  this->_depend_sel_changed();

  // check for changes
  this->_set_unsaved(this->_has_changes());
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_depend_add_show(bool show)
{
  // Show/Hide the "Add Dependency" controls
  this->showItem(IDC_SC_LBL08, show);
  this->showItem(IDC_EC_INP08, show);
  this->showItem(IDC_BC_DPVAL, show);
  this->showItem(IDC_BC_DPABT, show);

  // Show/Hide regular Dependencies controls
  this->showItem(IDC_BC_DPADD, !show);
  this->showItem(IDC_BC_DPDEL, !show);
  this->showItem(IDC_BC_DPBRW, !show);
  this->showItem(IDC_LB_DPN,   !show);

  this->_depend_sel_changed();
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_depend_add_valid()
{
  this->enableItem(IDC_BC_DPVAL, false);

  OmWString ec_content;
  this->getItemText(IDC_EC_INP08, ec_content);
  this->setItemText(IDC_EC_INP08, L"");

  this->msgItem(IDC_LB_DPN, LB_ADDSTRING, 0, reinterpret_cast<LPARAM>(ec_content.c_str()));
  this->_depend_cache.push_back(ec_content);

  this->_depend_add_show(false);

  // check for changes
  this->_set_unsaved(this->_has_changes());
}

///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_onInit()
{
  // set dialog icon
  this->setIcon(Om_getResIcon(IDI_MOD_TOOL,2),Om_getResIcon(IDI_MOD_TOOL,1));

  // Set menu icons
  HMENU hMnuFile = this->getPopup(MNU_ME_FILE);
  //this->setPopupItemIcon(hMnuFile, MNU_ME_FILE_NEW, Om_getResIconPremult(IDI_BT_NEW));
  this->setPopupItemIcon(hMnuFile, MNU_ME_FILE_OPEN, Om_getResIconPremult(IDI_BT_OPN));
  this->setPopupItemIcon(hMnuFile, MNU_ME_FILE_BUIL, Om_getResIconPremult(IDI_PKG_BLD));
  this->setPopupItemIcon(hMnuFile, MNU_ME_FILE_SAVE, Om_getResIconPremult(IDI_BT_SAV));
  this->setPopupItemIcon(hMnuFile, MNU_ME_FILE_SAVAS, Om_getResIconPremult(IDI_BT_SVA));
  this->setPopupItemIcon(hMnuFile, MNU_ME_FILE_QUIT, Om_getResIconPremult(IDI_QUIT));

  HMENU hMnuEdit = this->getPopup(MNU_ME_EDIT);
  this->setPopupItemIcon(hMnuEdit, MNU_ME_EDIT_THMBSEL, Om_getResIconPremult(IDI_PIC_ADD));
  this->setPopupItemIcon(hMnuEdit, MNU_ME_EDIT_DESCSEL, Om_getResIconPremult(IDI_TXT_ADD));
  this->setPopupItemIcon(hMnuEdit, MNU_ME_EDIT_DEPIMP, Om_getResIconPremult(IDI_PKG_IMP));

  // dialog is modeless so we set dialog title with app name
  this->setCaption(L"Mod-Package editor");

  // Create the toolbar.
  CreateWindowExW(WS_EX_LEFT, TOOLBARCLASSNAMEW, nullptr, WS_CHILD|TBSTYLE_WRAPABLE|TBSTYLE_TOOLTIPS, 0, 0, 0, 0,
                  this->_hwnd, reinterpret_cast<HMENU>(IDC_TB_TOOLS), this->_hins, nullptr);

  HIMAGELIST himl = static_cast<OmUiMan*>(this->root())->toolBarsImgList();
  this->msgItem(IDC_TB_TOOLS, TB_SETIMAGELIST, 0, reinterpret_cast<LPARAM>(himl));

  // Initialize button info.
  TBBUTTON tbButtons[3] = {
    //{ICON_NEW, IDC_BC_NEW,  TBSTATE_ENABLED, 0/*BTNS_AUTOSIZE*/, {0}, 0, reinterpret_cast<INT_PTR>("New Package")},
    {ICON_OPN, IDC_BC_OPEN, TBSTATE_ENABLED, 0/*BTNS_AUTOSIZE*/, {0}, 0, reinterpret_cast<INT_PTR>("Open Package file")},
    {ICON_BLD, IDC_BC_OPEN2,TBSTATE_ENABLED, 0/*BTNS_AUTOSIZE*/, {0}, 0, reinterpret_cast<INT_PTR>("Build from directory")},
    {ICON_SAV, IDC_BC_SAVE, 0,               0/*BTNS_AUTOSIZE*/, {0}, 0, reinterpret_cast<INT_PTR>("Save Package file")}
  };

  // Add buttons
  this->msgItem(IDC_TB_TOOLS, TB_SETMAXTEXTROWS, 0); //< disable text under buttons
  this->msgItem(IDC_TB_TOOLS, TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON));
  this->msgItem(IDC_TB_TOOLS, TB_ADDBUTTONS, 3, reinterpret_cast<LPARAM>(&tbButtons));

  this->msgItem(IDC_TB_TOOLS, TB_SETBUTTONSIZE, 0, MAKELPARAM(26, 22));
  //this->msgItem(IDC_TB_TOOLS, TB_SETLISTGAP, 25);                       //< this does not work
  //this->msgItem(IDC_TB_TOOLS, TB_SETPADDING, 0, MAKELPARAM(25, 20));    //< this does not work

  // Resize and show the toolbar
  this->msgItem(IDC_TB_TOOLS, TB_AUTOSIZE);
  this->showItem(IDC_TB_TOOLS, true);

  // Shared Image list for ListView controls
  himl = static_cast<OmUiMan*>(this->root())->listViewImgList();

  // Initialize Mod content ListView control with explorer theme
  this->msgItem(IDC_LV_PAT, LVM_SETEXTENDEDLISTVIEWSTYLE, 0, LVS_EX_FULLROWSELECT|LVS_EX_SUBITEMIMAGES|LVS_EX_DOUBLEBUFFER);
  SetWindowTheme(this->getItem(IDC_LV_PAT),L"EXPLORER",nullptr);
  // add column into ListView
  LVCOLUMNW lvC = {}; lvC.mask = LVCF_WIDTH|LVCF_FMT;
  // Single column for paths
  lvC.fmt = LVCFMT_LEFT;
  lvC.iSubItem = 0; lvC.cx = 300;
  this->msgItem(IDC_LV_PAT, LVM_INSERTCOLUMNW, lvC.iSubItem, reinterpret_cast<LPARAM>(&lvC));
  // set shared ImageList
  this->msgItem(IDC_LV_PAT, LVM_SETIMAGELIST, LVSIL_SMALL, reinterpret_cast<LPARAM>(himl));
  this->msgItem(IDC_LV_PAT, LVM_SETIMAGELIST, LVSIL_NORMAL, reinterpret_cast<LPARAM>(himl));

  // add items to extension ComboBox
  this->msgItem(IDC_CB_EXT, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"." OM_PKG_FILE_EXT));
  this->msgItem(IDC_CB_EXT, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L".zip"));
  this->msgItem(IDC_CB_EXT, CB_SETCURSEL, 0, 0);

  // add items into Category ComboBox
  for(size_t i = 0; i < __categ_count; ++i) {
    this->msgItem(IDC_CB_CAT, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(__categ_list[i]));
  }
  this->msgItem(IDC_CB_CAT, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"- custom -"));
  this->msgItem(IDC_CB_CAT, CB_SETCURSEL, 0, 0);

  int32_t cb_id;

  // add items to Compression Method ComboBox
  cb_id = this->msgItem(IDC_CB_ZMD, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"None (Store only)"));
  this->msgItem(IDC_CB_ZMD, CB_SETITEMDATA, cb_id, OM_METHOD_STORE);
  cb_id = this->msgItem(IDC_CB_ZMD, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Deflate (Legacy Zip)"));
  this->msgItem(IDC_CB_ZMD, CB_SETITEMDATA, cb_id, OM_METHOD_DEFLATE);
  cb_id = this->msgItem(IDC_CB_ZMD, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"LZMA"));
  this->msgItem(IDC_CB_ZMD, CB_SETITEMDATA, cb_id, OM_METHOD_LZMA);
  cb_id = this->msgItem(IDC_CB_ZMD, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"LZMA2"));
  this->msgItem(IDC_CB_ZMD, CB_SETITEMDATA, cb_id, OM_METHOD_LZMA2);
  cb_id = this->msgItem(IDC_CB_ZMD, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Zstandard"));
  this->msgItem(IDC_CB_ZMD, CB_SETITEMDATA, cb_id, OM_METHOD_ZSTD);
  this->msgItem(IDC_CB_ZMD, CB_SETCURSEL, 4);

  // add items into Compression Level ComboBox
  cb_id = this->msgItem(IDC_CB_ZLV, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"None (Store only)"));
  this->msgItem(IDC_CB_ZLV, CB_SETITEMDATA, cb_id, OM_LEVEL_NONE);
  cb_id = this->msgItem(IDC_CB_ZLV, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Fast"));
  this->msgItem(IDC_CB_ZLV, CB_SETITEMDATA, cb_id, OM_LEVEL_FAST);
  cb_id = this->msgItem(IDC_CB_ZLV, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Normal"));
  this->msgItem(IDC_CB_ZLV, CB_SETITEMDATA, cb_id, OM_LEVEL_SLOW);
  cb_id = this->msgItem(IDC_CB_ZLV, CB_ADDSTRING, 0, reinterpret_cast<LPARAM>(L"Best"));
  this->msgItem(IDC_CB_ZLV, CB_SETITEMDATA, cb_id, OM_LEVEL_BEST);
  this->msgItem(IDC_CB_ZLV, CB_SETCURSEL, 2);

  // Set thumbnail placeholder image
  this->setStImage(IDC_SB_SNAP, Om_getResImage(IDB_BLANK));

  // Set buttons icons
  this->setBmIcon(IDC_BC_BRW01, Om_getResIcon(IDI_BT_FAD));   //< Add file
  this->setBmIcon(IDC_BC_BRW02, Om_getResIcon(IDI_BT_DAD));   //< Add directory
  this->setBmIcon(IDC_BC_DEL,   Om_getResIcon(IDI_BT_FRM));   //< Remove entry

  // Hide Content Actions buttons, feature is not implemented yet
  this->showItem(IDC_BC_BRW01,  false);
  this->showItem(IDC_BC_BRW02,  false);
  this->showItem(IDC_BC_DEL,    false);

  this->setBmIcon(IDC_BC_BRW03, Om_getResIcon(IDI_PIC_ADD));  //< Thumbnail Select
  this->setBmIcon(IDC_BC_BRW04, Om_getResIcon(IDI_TXT_ADD));  //< Description Load
  this->setBmIcon(IDC_BC_DPBRW, Om_getResIcon(IDI_PKG_IMP));  //< Dependencies Select
  this->setBmIcon(IDC_BC_DPADD, Om_getResIcon(IDI_BT_ADD));   //< Dependencies Add
  this->setBmIcon(IDC_BC_DPDEL, Om_getResIcon(IDI_BT_REM));   //< Dependencies Delete
  this->setBmIcon(IDC_BC_DPVAL, Om_getResIcon(IDI_BT_VAL));   //< Depend-Add Valid
  this->setBmIcon(IDC_BC_DPABT, Om_getResIcon(IDI_BT_ABT));   //< Depend-Add Abort

  // Hide the "Add Dependency" controls
  this->showItem(IDC_SC_LBL08, false);
  this->showItem(IDC_EC_INP08, false);
  this->showItem(IDC_BC_DPVAL, false);
  this->showItem(IDC_BC_DPABT, false);

  // set tool-tips
  this->_createTooltip(IDC_EC_INP01,  L"Display name");
  this->_createTooltip(IDC_EC_INP02,  L"Version string");
  this->_createTooltip(IDC_EC_RESUL,  L"Filename preview");

  this->_createTooltip(IDC_CB_ZMD,    L"Archive compression algorithm");
  this->_createTooltip(IDC_CB_ZLV,    L"Archive compression level");

  this->_createTooltip(IDC_BC_BRW01,  L"Add files");
  this->_createTooltip(IDC_BC_BRW02,  L"Add directory");
  this->_createTooltip(IDC_BC_DEL,    L"Delete entry");
  this->_createTooltip(IDC_EC_READ1,  L"List of Mod files");

  this->_createTooltip(IDC_CB_CAT,    L"Predefined categories");
  this->_createTooltip(IDC_EC_INP07,  L"Custom category");

  this->_createTooltip(IDC_BC_CKBX1,  L"Enable overview thumbnail");
  this->_createTooltip(IDC_BC_BRW03,  L"Load thumbnail image");

  this->_createTooltip(IDC_BC_CKBX2,  L"Enable overview description");
  this->_createTooltip(IDC_BC_BRW04,  L"Load description text");

  this->_createTooltip(IDC_BC_CKBX3,  L"Enable Mod dependencies");
  this->_createTooltip(IDC_BC_DPADD,  L"Add dependency Mod");
  this->_createTooltip(IDC_BC_DPBRW,  L"Select dependency Mod(s)");
  this->_createTooltip(IDC_BC_DPDEL,  L"Remove dependency");
  this->_createTooltip(IDC_EC_INP08,  L"Dependency Mod Identity string");
  this->_createTooltip(IDC_BC_DPVAL,  L"Valid");
  this->_createTooltip(IDC_BC_DPABT,  L"Abort");


  // update status
  this->_status_update_filename();

  // nothing to save
  this->_set_unsaved(false);

  // reset controls to initial states
  //this->_modpack_new();
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_onResize()
{
  int32_t half_w = this->cliWidth() * 0.5f;
  int32_t quar_w = this->cliWidth() * 0.25f;
  int32_t base_y = 34;
  int32_t foot_y = this->cliHeight() - (base_y+28);

  // resize the toolbar
  this->msgItem(IDC_TB_TOOLS, TB_AUTOSIZE);

  // toolbar separator
  this->_setItemPos(IDC_SC_SEPAR, -1, 28, this->cliWidth()+2, 1, true);

  // -- Left Frame --

  // Name & Version Labels
  this->_setItemPos(IDC_SC_LBL01, 10, base_y, 50, 16, true);
  this->_setItemPos(IDC_SC_LBL02, half_w-125, base_y, 60, 16, true);
  // Name & Version EditControl + Extension ComboBox
  this->_setItemPos(IDC_EC_INP01, 10, base_y+20, half_w-140, 21, true);
  this->_setItemPos(IDC_EC_INP02, half_w-125, base_y+20, 60, 21, true);
  this->_setItemPos(IDC_CB_EXT,   half_w-60, base_y+20, 50, 21, true);
  //filename preview Label & EditControl
  this->_setItemPos(IDC_EC_RESUL, 10, base_y+50, half_w-20, 21, true);

  // Compression Method / Level Labels
  this->_setItemPos(IDC_SC_LBL03, 10, base_y+90, 200, 21, true);
  // Compression Method /Level ComboBoxes
  this->_setItemPos(IDC_CB_ZMD, 10, base_y+110, quar_w-15, 21, true);
  this->_setItemPos(IDC_CB_ZLV, quar_w+5, base_y+110, quar_w-20, 21, true);

  // Content Label
  this->_setItemPos(IDC_SC_LBL06, 10, base_y+150, 200, 21, true);
  // Content Actions buttons
  this->_setItemPos(IDC_BC_BRW01, half_w-78, base_y+148, 22, 22, true);
  this->_setItemPos(IDC_BC_BRW02, half_w-55, base_y+148, 22, 22, true);
  this->_setItemPos(IDC_BC_DEL,   half_w-32, base_y+148, 22, 22, true);
  // Content Edit control
  //this->_setItemPos(IDC_EC_READ1, 10, base_y+170, half_w-20, foot_y-180, true);
  this->_setItemPos(IDC_LV_PAT, 10, base_y+172, half_w-20, foot_y-180, true);
  this->_content_resize();

  // -- Right Frame --

  // Category Label
  this->_setItemPos(IDC_SC_LBL07, half_w+10, base_y, 200, 16, true);
  // Category ComboBox
  this->_setItemPos(IDC_CB_CAT, half_w+10, base_y+20, half_w-20, 21, true);
  // Category EditControl
  this->_setItemPos(IDC_EC_INP07, half_w+10, base_y+50, half_w-20, 21, true);

  // Thumbnail CheckBox
  this->_setItemPos(IDC_BC_CKBX1, half_w+10, base_y+90, 100, 16, true);
  // Thumbnail notice text
  this->_setItemPos(IDC_SC_NOTES, half_w+120, base_y+91, 150, 16, true);
  // Thumbnail Action buttons
  this->_setItemPos(IDC_BC_BRW03, this->cliWidth()-32, base_y+87, 22, 22, true);
  // Thumbnail static bitmap
  this->_setItemPos(IDC_SB_SNAP, half_w+10, base_y+110, 128, 128, true);

  // Description CheckBox
  this->_setItemPos(IDC_BC_CKBX2, half_w+10, base_y+260, 100, 16, true);
  // Description Actions buttons
  this->_setItemPos(IDC_BC_BRW04, this->cliWidth()-32, base_y+257, 22, 22, true);
  // Description EditControl
  this->_setItemPos(IDC_EC_DESC, half_w+10, base_y+282, half_w-21, foot_y-(292+110), true);

  // Dependencies CheckBox
  this->_setItemPos(IDC_BC_CKBX3, half_w+10, foot_y-65, 90, 16, true);
  // Dependencies notice text
  this->_setItemPos(IDC_SC_HELP, half_w+120, foot_y-64, 140, 16, true);
  // Dependencies Actions buttons
  this->_setItemPos(IDC_BC_DPADD, this->cliWidth()-78, foot_y-68, 22, 22, true);
  this->_setItemPos(IDC_BC_DPBRW, this->cliWidth()-55, foot_y-68, 22, 22, true);
  this->_setItemPos(IDC_BC_DPDEL, this->cliWidth()-32, foot_y-68, 22, 22, true);
  // Dependencies ListBox
  this->_setItemPos(IDC_LB_DPN, half_w+10, foot_y-43, half_w-20, 68, true);

  // Dependencies Add Label
  this->_setItemPos(IDC_SC_LBL08, half_w+10, foot_y-35, 200, 16, true);
  // Dependencies Add Entry
  this->_setItemPos(IDC_EC_INP08, half_w+10, foot_y-15, half_w-20, 21, true);
  // Dependencies Add Buttons
  this->_setItemPos(IDC_BC_DPVAL, this->cliWidth()-55, foot_y-38, 22, 22, true);
  this->_setItemPos(IDC_BC_DPABT, this->cliWidth()-32, foot_y-38, 22, 22, true);

  // Foot status bar
  this->_setItemPos(IDC_SC_STATUS, 2, this->cliHeight()-24, this->cliWidth()-4, 22, true);
  this->_setItemPos(IDC_SC_FILE, 7, this->cliHeight()-20, this->cliWidth()-110, 16, true);
  this->_setItemPos(IDC_SC_INFO, this->cliWidth()-97, this->cliHeight()-20, 90, 16, true);

  // redraw the window
  RedrawWindow(this->_hwnd, nullptr, nullptr, RDW_INVALIDATE|RDW_UPDATENOW|RDW_ERASE);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_onRefresh()
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiToolPkg::_onClose()
{
   switch(this->_ask_unsaved()) {
    case  1: this->_modpack_save(); break; //< 'Yes'
    case -1: return;                    //< 'Cancel'
  }

  this->quit();
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
INT_PTR OmUiToolPkg::_onMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  OM_UNUSED(lParam);

  if(uMsg == WM_COMMAND) {
    switch(LOWORD(wParam))
    {
    case IDC_BC_NEW:
    case IDM_FILE_NEW:
      this->_modpack_new();
      break;

    case IDC_BC_OPEN:
    case IDM_FILE_OPEN:
      this->_modpack_open();
      break;

    case IDC_BC_OPEN2:
    case IDM_MOD_BUIL:
      this->_modpack_build();
      break;

    case IDC_BC_SAVE:
    case IDM_FILE_SAVE:
      this->_modpack_save();
      break;

    case IDM_FILE_SAVAS:
      this->_modpack_save_as();
      break;

    case IDM_QUIT:
      this->_onClose();
      break;

    case IDC_EC_INP01: //< Entry : Name
    case IDC_EC_INP02: //< Entry : Version
      if(HIWORD(wParam) == EN_CHANGE) {
        this->_name_compose();
      }
      break;

    case IDC_CB_EXT:  //< ComboBox: File extension
      if(HIWORD(wParam) == CBN_SELCHANGE) {
        this->_name_compose();
        this->_check_zip_method();
      }
      break;

    case IDC_CB_ZMD:  //< ComboBox: File extension
      if(HIWORD(wParam) == CBN_SELCHANGE)
        this->_check_zip_method();
      break;

    case IDC_CB_CAT:  //< ComboBox: Category
      if(HIWORD(wParam) == CBN_SELCHANGE)
        this->_categ_select();
      break;

    case IDC_EC_INP07: //< Entry : Custom Category
      //if(HIWORD(wParam) == EN_CHANGE)
      if(HIWORD(wParam) == EN_KILLFOCUS)
        this->_categ_changed();
      break;

    case IDC_BC_CKBX1: //< CheckBox: Description
      if(HIWORD(wParam) == BN_CLICKED)
        this->_thumb_toggle();
      break;

    case IDC_BC_BRW03: //< Button : Description: Load
      if(HIWORD(wParam) == BN_CLICKED)
        this->_reference_thumb_load();
      break;

    case IDM_THMB_SEL: //< Menu: Add thumbnail
      {
        this->msgItem(IDC_BC_CKBX1, BM_SETCHECK, 1);
        this->_thumb_toggle();
        this->_reference_thumb_load();
      }
      break;

    case IDC_BC_CKBX2: //< CheckBox: Description
      if(HIWORD(wParam) == BN_CLICKED)
        this->_desc_toggle();
      break;

    case IDC_BC_BRW04: //< Button : Description: Load
      if(HIWORD(wParam) == BN_CLICKED)
        this->_reference_desc_load();
      break;

    case IDM_DESC_SEL: //< Menu: Add thumbnail
      {
        this->msgItem(IDC_BC_CKBX2, BM_SETCHECK, 1);
        this->_desc_toggle();
        this->_reference_desc_load();
      }
      break;

    case IDC_EC_DESC: //< Entry : Description entry
      //if(HIWORD(wParam) == EN_CHANGE)
      if(HIWORD(wParam) == EN_KILLFOCUS)
        this->_reference_desc_changed();
      break;

    case IDC_BC_CKBX3: //< CheckBox: Dependencies
      if(HIWORD(wParam) == BN_CLICKED)
        this->_depend_toggle();
      break;

    case IDC_LB_DPN: //< Packages list ListBox
      // check for selection change
      if(HIWORD(wParam) == LBN_SELCHANGE) {
        this->_depend_sel_changed();
      }
      break;

    case IDC_BC_DPADD: //< Button : Dependencies: Add
      if(HIWORD(wParam) == BN_CLICKED) {
        this->_depend_add_show(true);
        SetFocus(this->getItem(IDC_EC_INP08));
      }
      break;

    case IDC_BC_DPBRW: //< Button : Dependencies: Browse
      if(HIWORD(wParam) == BN_CLICKED)
        this->_depend_browse();
      break;

    case IDM_DEP_ADD: //< Menu: Add thumbnail
      {
        this->msgItem(IDC_BC_CKBX3, BM_SETCHECK, 1);
        this->_depend_toggle();
        this->_depend_browse();
      }
      break;

    case IDC_BC_DPDEL: //< Button : Dependencies: Delete
      if(HIWORD(wParam) == BN_CLICKED)
        this->_depend_delete();
      break;

    case IDC_BC_DPABT: //< Button : Add-Depend Prompt: Abort
      if(HIWORD(wParam) == BN_CLICKED) {
        this->setItemText(IDC_EC_INP08, L"");
        this->_depend_add_show(false);
      }
      break;

    case IDC_EC_INP08: //< Entry : Add-Depend Prompt: entry
      if(HIWORD(wParam) == EN_CHANGE) {
        int32_t txt_len = GetWindowTextLengthW(this->getItem(IDC_EC_INP08));
        this->enableItem(IDC_BC_DPVAL, (txt_len > 1));
      }
      break;

    case IDC_BC_DPVAL: //< Button : Add-Depend Prompt: Abort
      if(HIWORD(wParam) == BN_CLICKED)
        this->_depend_add_valid();
      break;

    }
  }

  return false;
}
