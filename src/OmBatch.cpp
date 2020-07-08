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

#include "OmContext.h"
#include "OmBatch.h"


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmBatch::OmBatch() :
  _context(nullptr),
  _config(),
  _path(),
  _uuid(),
  _title(),
  _index(0),
  _locUuid(),
  _instHash(),
  _instIden(),
  _error()
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmBatch::OmBatch(OmContext* ctx) :
  _context(ctx),
  _config(),
  _path(),
  _uuid(),
  _title(),
  _index(0),
  _locUuid(),
  _instHash(),
  _instIden(),
  _error()
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
OmBatch::~OmBatch()
{

}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmBatch::parse(const wstring& path)
{
  // clear the instance
  this->close();

  // try to open and parse the XML file
  if(!this->_config.open(path, OMM_CFG_SIGN_BAT)) {
    this->_error = L"Error loading Batch definition: ";
    this->_error += this->_config.lastErrorStr();
    this->log(0, wstring(L"Batch(")+path+L") Parse", this->_error);
    return false;
  }

  // check for the presence of <uuid> entry
  if(!this->_config.xml().hasChild(L"uuid")) {
    this->_error = L"Parse error: <uuid> node missing, malformed Batch definition file.";
    log(0, L"Batch("+path+L")", this->_error);
    return false;
  }

  // check for the presence of <title> entry
  if(!this->_config.xml().hasChild(L"title")) {
    this->_error = L"Parse error: <title> node missing, malformed Batch definition file.";
    log(0, L"Batch("+path+L")", this->_error);
    return false;
  }

  this->_path = path;
  this->_uuid = this->_config.xml().child(L"uuid").content();
  this->_title = this->_config.xml().child(L"title").content();
  this->_index = this->_config.xml().child(L"title").attrAsInt(L"index");

  // get <location> entries
  vector<OmXmlNode> loc_list;
  this->_config.xml().children(loc_list, L"location");

  for(unsigned l = 0; l < loc_list.size(); ++l) {

    this->_locUuid.push_back(loc_list[l].attrAsString(L"uuid"));

    vector<uint64_t>  hash_list;
    vector<wstring>   iden_list;

    // retrieve all <install> entries in <location>
    vector<OmXmlNode> ins_list;
    loc_list[l].children(ins_list, L"install");

    for(unsigned i = 0; i < ins_list.size(); ++i) {
      hash_list.push_back(Om_toUint64(ins_list[i].attrAsString(L"hash")));
      iden_list.push_back(ins_list[i].attrAsString(L"ident"));
    }

    this->_instHash.push_back(hash_list);
    this->_instIden.push_back(iden_list);
  }

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmBatch::hasLocation(const wstring& uuid)
{
  for(unsigned i = 0; i < this->_locUuid.size(); ++i) {
    if(this->_locUuid[i] == uuid)
      return true;
  }

  return false;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmBatch::hasInstallHash(unsigned l, uint64_t hash)
{
  for(unsigned i = 0; i < this->_instHash[l].size(); ++i) {
    if(this->_instHash[l][i] == hash) return true;
  }
  return false;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmBatch::hasInstallIdent(unsigned l, const wstring& ident)
{
  for(unsigned i = 0; i < this->_instIden[l].size(); ++i) {
    if(this->_instIden[l][i] == ident) return true;
  }
  return false;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmBatch::setTitle(const wstring& title)
{
  if(this->_config.valid()) {

    this->_title = title;

    if(this->_config.xml().hasChild(L"title")) {
      this->_config.xml().child(L"title").setContent(title);
    } else {
      this->_config.xml().addChild(L"title").setContent(title);
    }

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmBatch::setIndex(unsigned index)
{
  if(this->_config.valid()) {

    this->_index = index;

    if(this->_config.xml().hasChild(L"title")) {
      this->_config.xml().child(L"title").setAttr(L"index", static_cast<int>(index));
    }

    this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmBatch::addLocation(const wstring& uuid)
{
  if(this->_config.valid()) {

    // Check whether <location> entry with uuid already exists
    vector<OmXmlNode> loc_list;
    this->_config.xml().children(loc_list, L"location");

    for(size_t i = 0; i < loc_list.size(); ++i) {
      if(uuid == loc_list[i].attrAsString(L"uuid")) {
        return;
      }
    }

    // create new <location> entry in definition
    OmXmlNode location = this->_config.xml().addChild(L"location");
    location.setAttr(L"uuid", uuid);

    // Write definition file
    if(this->_path.size())
      this->_config.save();
  }
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmBatch::remLocation(const wstring& uuid)
{
  if(this->_config.valid()) {

    vector<OmXmlNode> loc_list;
    this->_config.xml().children(loc_list, L"location");

    // search and remove <location> entry with specified uuid
    for(size_t i = 0; i < loc_list.size(); ++i) {
      if(uuid == loc_list[i].attrAsString(L"uuid")) {
        this->_config.xml().remChild(loc_list[i]); break;
      }
    }

    // Write definition file
    if(this->_path.size())
      this->_config.save();
  }
}
///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
bool OmBatch::rename(const wstring& name)
{
  wstring title = this->_title;
  wstring old_path = this->_path;

  // compose new Batch full path
  wstring new_path = old_path.substr(0, old_path.find_last_of(L"\\") + 1);
  new_path += name; new_path += L"."; new_path += OMM_BAT_FILE_EXT;

  // close to rename file safely
  this->close();

  // Rename Batch definition file
  int result = Om_fileMove(old_path, new_path);
  if(result != 0) {
    this->_error = L"Unable to rename Batch definition file \"";
    this->_error += old_path + L"\" : ";
    this->_error += Om_getErrorStr(result);
    this->log(0, L"Batch("+title+L")", L"Rename : " + this->_error);
    return false;
  }

  this->log(2, L"Batch("+title+L")", L"Renamed to \"" + new_path + L"\"");

  // Reload Batch
  this->parse(new_path);

  return true;
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmBatch::close()
{
  this->_config.close();
  this->_path.clear();
  this->_title.clear();
  this->_locUuid.clear();
  this->_instHash.clear();
  this->_instIden.clear();
}


///
///  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -  -
///
void OmBatch::log(unsigned level, const wstring& head, const wstring& detail)
{
  if(this->_context != nullptr) {

    wchar_t buff[128];
    swprintf(buff, L"Location(%ls):: %ls", this->_context->title().c_str(), head.c_str());

    this->_context->log(level, buff, detail);
  }
}

