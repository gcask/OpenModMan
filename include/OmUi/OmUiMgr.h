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
#ifndef OMUIMGR_H
#define OMUIMGR_H

#include "OmDialog.h"

/// \brief Base ID for Recent menu-items
///
/// Base resource ID for "Recent Files" menu-items dynamically manager
/// by the dialog.
///
#define IDM_FILE_RECENT_PATH      40990

/// \brief Custom "Mod Hub Changed" window Message
///
/// Custom window message to notify tab child dialogs that selected
/// Mod Hub in main window changed.
///
#define UWM_MAIN_CTX_CHANGED      (WM_APP+16)

/// \brief Custom "Request Abort" window Message
///
/// Custom window message to notify tab child dialogs that main dialog (user)
/// requested to quit, and must abort all running process
///
#define UWM_MAIN_ABORT_REQUEST    (WM_APP+17)

/// \brief Main menu intems indices
///
/// Indices for main menu items
///
#define MNU_FILE             static_cast<unsigned>(0)
    #define MNU_FILE_NEW         0
        #define MNU_FILE_NEW_CTX    0
    // ------------------------- 1
    #define MNU_FILE_OPEN        2
    #define MNU_FILE_RECENT      3
    // ------------------------- 4
    #define MNU_FILE_CLOSE       5
    // ------------------------- 6
    #define MNU_FILE_QUIT        7

#define MNU_EDIT             1
    #define MNU_EDIT_CTX         0
        #define MNU_EDIT_ADDLOC      0
        // ------------------------- 1
        #define MNU_EDIT_CTXPROP     2

    // ------------------------- 1
    #define MNU_EDIT_LOC         2
        #define MNU_EDIT_LOCPROP     0

    // ------------------------- 3
    #define MNU_EDIT_PKG         4
        #define MNU_EDIT_PKG_INST    0
        #define MNU_EDIT_PKG_UINS    1
        #define MNU_EDIT_PKG_CLNS    2
        // ------------------------- 3
        #define MNU_EDIT_PKG_DISC    4
        // ------------------------- 5
        #define MNU_EDIT_PKG_OPEN    6
        #define MNU_EDIT_PKG_TRSH    7
        // ------------------------- 8
        #define MNU_EDIT_PKG_EDIT    9
        // ------------------------- 10
        #define MNU_EDIT_PKG_INFO    11

    #define MNU_EDIT_RMT         5
        #define MNU_EDIT_RMT_DNLD    0
        #define MNU_EDIT_RMT_DNWS    1
        // ------------------------- 2
        #define MNU_EDIT_RMT_FIXD    3
        // ------------------------- 4
        #define MNU_EDIT_RMT_INFO    5
    // ------------------------- 9
    #define MNU_EDIT_PROPMGR     6

#define MNU_TOOL             2
    #define MNU_TOOL_EDITPKG     0
    #define MNU_TOOL_EDITREP     1

#define MNU_HELP   3
    #define MNU_HELP_DEBUG       0
    #define MNU_HELP_ABOUT       1

class OmUiMgrMain;
class OmUiMgrFoot;

/// \brief Application Main window
///
/// OmDialog class derived for the application Main dialog window
///
class OmUiMgr : public OmDialog
{
  public: ///          - - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    /// \brief Constructor.
    ///
    /// Default constructor.
    ///
    /// \param[in]  hins    : API Instance handle.
    ///
    OmUiMgr(HINSTANCE hins);

    /// \brief Destructor.
    ///
    /// Default destructor.
    ///
    ~OmUiMgr();

    /// \brief Get resource id.
    ///
    /// Returns dialog window resource id.
    ///
    /// \return dialog resource id.
    ///
    long id() const;

    /// \brief Set dialog install mode
    ///
    /// Enable or disable the dialog install mode.
    ///
    /// The install mode is a modal-kind emulation for threaded operations,
    /// it disables (almost) all controls of the dialog and its children
    /// to prevent user to interact with elements while a threaded process
    /// is running.
    ///
    /// \param[in]  enable  : Enable or disable freeze mode.
    ///
    void freeze(bool enable);

    /// \brief Set dialog safe mode
    ///
    /// Enables or disable the dialog safe mode.
    ///
    /// The safe mode is used to operate modifications on sensitive
    /// or monitored elements such as deleting or moving Mod Channel in
    /// order to prevent conflicts or crash during process.
    ///
    /// \param[in]  enable  : Enable or disable safe mode.
    ///
    void safemode(bool enable);

    /// \brief Open Mod Hub
    ///
    /// Try to load the specified Mod Hub file then refresh dialog.
    ///
    /// \param[in]  path  Mod Hub definition file path to load.
    ///
    void modHubLoad(const wstring& path);

    /// \brief Close current Mod Hub
    ///
    /// Close the current active Mod Hub file then refresh dialog.
    ///
    void modHubClose();

    /// \brief Select Mod Hub
    ///
    /// Select or unselect Mod Hub then refresh dialog.
    ///
    /// \param[in]  i  Index of Mod Hub to select or -1 to select none.
    ///
    void modHubSel(int i);

    /// \brief Get tab frame dialog
    ///
    /// Return pointer to main (top) frame dialog.
    ///
    /// \return Main (top) frame dialog.
    ///
    OmUiMgrMain* pUiMgrMain() const {
      return _pUiMgrMain;
    }

    /// \brief Get footer frame dialog
    ///
    /// Return pointer to secondary (bottom) frame dialog.
    ///
    /// \return Secondary (bottom) frame dialog.
    ///
    OmUiMgrFoot* pUiMgrFoot() const {
      return _pUiMgrFoot;
    }

    /// \brief Check whether in frame resize
    ///
    /// Checks whether dialog has captured the mouse to process frame
    /// resize by user.
    ///
    /// \return True if in frame resize process, false otherwise
    ///
    bool divIsCapt() const {
      return _divIsCapt;
    }

    /// \brief Check whether mouse hover frame split
    ///
    /// Checks whether the mouse cursor is currently hovering between
    /// frames.
    ///
    /// \return True mouse cursor hovering between frames, false otherwise
    ///
    bool divIsHovr() const {
      return _divIsHovr;
    }

  private: ///          - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    OmUiMgrMain*         _pUiMgrMain;

    OmUiMgrFoot*         _pUiMgrFoot;

    bool                _divIsHovr;

    bool                _divIsCapt;

    long                _divMove[3];

    bool                _freeze_mode;

    bool                _freeze_quit;

    void                _buildCaption();

    void                _buildSbCtx();

    void                _buildMnRct();

    void                _buildCbCtx();

    void                _onCbCtxSel();

    void                _onInit();

    void                _onShow();

    void                _onResize();

    void                _onRefresh();

    void                _onClose();

    void                _onQuit();

    INT_PTR             _onMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // OMUIMGR_H
