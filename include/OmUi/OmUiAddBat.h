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
#ifndef OMUIADDBAT_H
#define OMUIADDBAT_H

#include "OmDialog.h"

class OmModHub;

/// \brief Add Batch dialog
///
/// OmDialog class derived for Add Batch dialog window
///
class OmUiAddBat : public OmDialog
{
  public:

    /// \brief Constructor.
    ///
    /// Default constructor.
    ///
    /// \param[in]  hins    : API Instance handle.
    ///
    OmUiAddBat(HINSTANCE hins);

    /// \brief Destructor.
    ///
    /// Default destructor.
    ///
    ~OmUiAddBat();

    /// \brief Get resource id.
    ///
    /// Returns dialog window resource id.
    ///
    /// \return dialog resource id.
    ///
    long id() const;

    /// \brief Get associated Mod Hub.
    ///
    /// Returns associated Mod Hub object
    ///
    /// \return Associated Mod Hub or nullptr if none.
    ///
    OmModHub* modHubCur() const {
      return _modHub;
    }

    /// \brief Set associated Mod Hub.
    ///
    /// Define the associated Mod Hub, which the dialog is intended to
    /// work with and on. A valid Mod Hub must be set in order before
    /// opening the dialog.
    ///
    /// \param[in]  pModHub  : Mod Hub object to associate.
    ///
    void ctxSet(OmModHub* pModHub) {
      _modHub = pModHub;
    }

  protected:

  private: ///          - - - - - - - - - - - - - - - - - - - - - - - - - - - -

    OmModHub*           _modHub;

    vector<vector<int>> _excluded;

    vector<vector<int>> _included;

    void                _buildLbs();

    void                _autoInclude();

    void                _includePkg();

    void                _excludePkg();

    void                _onCkBoxAuto();

    void                _onLbExclsSel();

    void                _onLbInclsSel();

    void                _onBcUpPkg();

    void                _onBcDnPkg();

    bool                _onBcOk();

    void                _onInit();

    void                _onResize();

    INT_PTR             _onMsg(UINT uMsg, WPARAM wParam, LPARAM lParam);
};

#endif // OMUIADDBAT_H
