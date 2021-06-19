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
#include "OmManager.h"
#include "gui/OmUiAddRep.h"
#include "gui/OmUiPropLocNet.h"
#include "gui/OmUiPropLoc.h"
#include "OmSocket.h"



///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmUiPropLocNet::OmUiPropLocNet(HINSTANCE hins) : OmDialog(hins)
{
  // modified parameters flags
  for(unsigned i = 0; i < 8; ++i)
    this->_chParam[i] = false;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmUiPropLocNet::~OmUiPropLocNet()
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
long OmUiPropLocNet::id() const
{
  return IDD_PROP_LOC_NET;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiPropLocNet::setChParam(unsigned i, bool en)
{
  _chParam[i] = en;
  static_cast<OmDialogProp*>(this->_parent)->checkChanges();
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiPropLocNet::_onLbReplsSel()
{
  int lb_sel = this->msgItem(IDC_LB_REP, LB_GETCURSEL);
  if(lb_sel >= 0) {
    this->enableItem(IDC_BC_DEL, true);
    this->enableItem(IDC_BC_CHK, true);
    this->enableItem(IDC_SC_STATE, false);
    this->setItemText(IDC_SC_STATE, L"<no test launched>");
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiPropLocNet::_onBcAddRepo()
{
  OmLocation* pLoc = static_cast<OmUiPropLoc*>(this->_parent)->locCur();
  if(!pLoc) return;

  // Open new Repository dialog
  OmUiAddRep* pUiNewRep = static_cast<OmUiAddRep*>(this->siblingById(IDD_ADD_REP));
  pUiNewRep->locSet(pLoc);
  pUiNewRep->open(true);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiPropLocNet::_onBcDelRepo()
{
  OmLocation* pLoc = static_cast<OmUiPropLoc*>(this->_parent)->locCur();
  if(!pLoc) return;

  int lb_sel = this->msgItem(IDC_LB_REP, LB_GETCURSEL);
  if(lb_sel >= 0) {

    // warns the user before committing the irreparable
    wstring qry = L"Are your sure you want to delete the Repository \"";
    qry += pLoc->repGet(lb_sel)->base() + L" - " + pLoc->repGet(lb_sel)->name();
    qry += L"\" ?";

    if(Om_dialogBoxQuerryWarn(this->_hwnd, L"Delete Repository", qry)) {
      pLoc->repRem(lb_sel);
    }

    // refresh list and buttons
    this->_onRefresh();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiPropLocNet::_onBcChkRepo()
{
  OmLocation* pLoc = static_cast<OmUiPropLoc*>(this->_parent)->locCur();
  if(!pLoc) return;

  int lb_sel = this->msgItem(IDC_LB_REP, LB_GETCURSEL);

  if(lb_sel >= 0) {

    this->setItemText(IDC_SC_STATE, L"Pending...");

    OmRepository* pRep = pLoc->repGet(lb_sel);

    OmSocket sock;

    wstring msg;

    string data;

    if(sock.httpGet(pRep->url(), data)) {

      OmConfig config;

      if(config.parse(Om_fromUtf8(data.c_str()), OMM_CFG_SIGN_REP)) {
        int n = config.xml().child(L"packages").attrAsInt(L"count");
        msg = L"Available, providing " + std::to_wstring(n) + L" package(s)";
      } else {
        msg = L"Invalid XML definition";
      }
    } else {
      msg = L"HTTP request failed (" + sock.lastErrorStr() + L")";
    }

    // get local time
    int t_h, t_m, t_s;
    Om_getTime(&t_s, &t_m, &t_h);

    wchar_t hour[32];
    swprintf(hour, 32, L"[%02d:%02d:%02d] ", t_h, t_m, t_s);

    this->setItemText(IDC_SC_STATE, hour + msg);
    this->enableItem(IDC_SC_STATE, true);
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiPropLocNet::_onInit()
{
  // Set buttons inner icons
  this->setBmImage(IDC_BC_ADD, Om_getResImage(this->_hins, IDB_BTN_ADD));
  this->setBmImage(IDC_BC_DEL, Om_getResImage(this->_hins, IDB_BTN_REM));

  // define controls tool-tips
  this->_createTooltip(IDC_LB_LOC,  L"Network repositories");

  this->_createTooltip(IDC_BC_DEL,    L"Remove repository");
  this->_createTooltip(IDC_BC_ADD,    L"Add new repository");
  this->_createTooltip(IDC_BC_EDI,    L"Test repository availability");

  this->enableItem(IDC_SC_STATE, false);

  // Update values
  this->_onRefresh();
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiPropLocNet::_onResize()
{
  // Locations list Label & ListBox
  this->_setItemPos(IDC_SC_LBL01, 5, 20, 64, 9);
  this->_setItemPos(IDC_LB_REP, 70, 20, this->width()-107, 30);

  // Remove Button
  this->_setItemPos(IDC_BC_DEL, 70, 55, 50, 14);
  // Add button
  this->_setItemPos(IDC_BC_ADD, this->width()-87, 55, 50, 14);

  // Test label
  this->_setItemPos(IDC_SC_LBL02, 71, 80, 40, 9);
  // Test button & entry
  this->_setItemPos(IDC_BC_CHK, 70, 90, 50, 14);
  this->_setItemPos(IDC_SC_STATE, 124, 92, this->width()-137, 13);
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmUiPropLocNet::_onRefresh()
{
  OmLocation* pLoc = static_cast<OmUiPropLoc*>(this->_parent)->locCur();
  if(!pLoc) return;

  this->msgItem(IDC_LB_REP, LB_RESETCONTENT);
  if(pLoc) {

    wstring label;
    OmRepository* pRep;

    for(unsigned i = 0; i < pLoc->repCount(); ++i) {
      pRep = pLoc->repGet(i);
      label = pRep->base() + L" - " + pRep->name();
      this->msgItem(IDC_LB_REP, LB_ADDSTRING, i, reinterpret_cast<LPARAM>(label.c_str()));
    }
  }

  // Set controls default states and parameters
  this->enableItem(IDC_SC_STATE, false);
  this->setItemText(IDC_SC_STATE, L"<no test launched>");

  this->enableItem(IDC_BC_DEL,  false);
  this->enableItem(IDC_BC_CHK,  false);

  // reset modified parameters flags
  for(unsigned i = 0; i < 8; ++i) _chParam[i] = false;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmUiPropLocNet::_onMsg(UINT uMsg, WPARAM wParam, LPARAM lParam)
{
  if(uMsg == WM_COMMAND) {

    switch(LOWORD(wParam))
    {
    case IDC_LB_REP: //< Location(s) list List-Box
      if(HIWORD(wParam) == LBN_SELCHANGE)
        this->_onLbReplsSel();
      break;

    case IDC_BC_ADD: //< New button for Location(s) list
      this->_onBcAddRepo();
      break;

    case IDC_BC_DEL: //< Remove button for Location(s) list
      this->_onBcDelRepo();
      break;

    case IDC_BC_CHK:
      this->_onBcChkRepo();
      break;
    }
  }

  return false;
}
